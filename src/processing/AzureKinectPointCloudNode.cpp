#include "AzureKinectPointCloudNode.hpp"
#include "imnodes.h"

#include <k4a/k4a.h>
#include <k4abt.h>

bool act::proc::AzureKinectPointCloudNode::m_registered = act::proc::NodeRegistry::add("AzureKinectPointCloud", act::proc::AzureKinectPointCloudNode::create);

act::proc::AzureKinectPointCloudNode::AzureKinectPointCloudNode() : NodeBase("AzureKinectPointCloud") {
    m_displaySize = ivec2(336, 189);

    m_depthImageInPort = InputPort<k4a_image_t>::create("Depth Image");
    m_inputPorts.push_back(m_depthImageInPort);

    m_pointCloudOutPort = OutputPort<k4a_image_t>::create("Point Cloud");
    m_outputPorts.push_back(m_pointCloudOutPort);
}

act::proc::AzureKinectPointCloudNode::~AzureKinectPointCloudNode() {
}

void act::proc::AzureKinectPointCloudNode::setup(act::mod::DeviceManagers deviceMgrs) {
    m_kinectMgr = deviceMgrs.kinectMgr;
}

void act::proc::AzureKinectPointCloudNode::update() {
    k4a_image_t depthImage = m_depthImageInPort->receive();

    if (depthImage != nullptr) {
        k4a_image_t pointCloudImage = nullptr;

        if (convertDepthImageToPointCloud(depthImage, pointCloudImage)) {
            m_pointCloudOutPort->send(pointCloudImage);
        }

        k4a_image_release(depthImage);
        k4a_image_release(pointCloudImage);
    }
}

void act::proc::AzureKinectPointCloudNode::draw() {
    ImNodes::BeginNode(m_id);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(getName().c_str());
    ImNodes::EndNodeTitleBar();

    drawPorts(m_displaySize.x);

    ImNodes::EndNode();
}

bool act::proc::AzureKinectPointCloudNode::convertDepthImageToPointCloud(k4a_image_t depthImage, k4a_image_t& pointCloudImage) {
    k4a_image_t transformedDepthImage = nullptr;
    k4a_image_t undistortedDepthImage = nullptr;
    k4a_image_t pointCloud = nullptr;

    k4a_calibration_t calibration;
    if (k4a_device_get_calibration(m_kinectMgr->getDevice(), K4A_DEPTH_MODE_NFOV_UNBINNED, K4A_COLOR_RESOLUTION_OFF, &calibration) != K4A_RESULT_SUCCEEDED) {
        return false;
    }

    if (k4a_image_create(K4A_IMAGE_FORMAT_DEPTH16, calibration.depth_camera_calibration.resolution_width,
        calibration.depth_camera_calibration.resolution_height, calibration.depth_camera_calibration.resolution_width * sizeof(uint16_t),
        &transformedDepthImage) != K4A_RESULT_SUCCEEDED) {
        return false;
    }

    if (k4a_image_create(K4A_IMAGE_FORMAT_DEPTH16, calibration.depth_camera_calibration.resolution_width,
        calibration.depth_camera_calibration.resolution_height, calibration.depth_camera_calibration.resolution_width * sizeof(uint16_t),
        &undistortedDepthImage) != K4A_RESULT_SUCCEEDED) {
        return false;
    }

    if (k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM, calibration.depth_camera_calibration.resolution_width,
        calibration.depth_camera_calibration.resolution_height, calibration.depth_camera_calibration.resolution_width * 3 * sizeof(int16_t),
        &pointCloud) != K4A_RESULT_SUCCEEDED) {
        return false;
    }

    if (k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM, calibration.depth_camera_calibration.resolution_width,
        calibration.depth_camera_calibration.resolution_height, calibration.depth_camera_calibration.resolution_width * 3 * sizeof(int16_t),
        &pointCloudImage) != K4A_RESULT_SUCCEEDED) {
        return false;
    }

    // Transform depth image
    k4a_transformation_t transformation = k4a_transformation_create(&calibration);
    k4a_transformation_depth_image_to_depth_image(transformation, depthImage, transformedDepthImage);
    k4a_transformation_depth_image_to_point_cloud(transformation, transformedDepthImage, K4A_CALIBRATION_TYPE_DEPTH, pointCloud);

    // Undistort depth image
    k4a_image_create(K4A_IMAGE_FORMAT_DEPTH16, calibration.depth_camera_calibration.resolution_width,
        calibration.depth_camera_calibration.resolution_height, calibration.depth_camera_calibration.resolution_width * sizeof(uint16_t),
        &undistortedDepthImage);
    k4a_image_t distortionTable = k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM, calibration.depth_camera_calibration.resolution_width,
        calibration.depth_camera_calibration.resolution_height, calibration.depth_camera_calibration.resolution_width * 2 * sizeof(int16_t), nullptr);

    k4a_undistort_depth_image(transformedDepthImage, calibration, undistortedDepthImage);
    k4a_image_release(transformedDepthImage);

    // Generate point cloud from undistorted depth image
    k4a_transformation_depth_image_to_point_cloud(transformation, undistortedDepthImage, K4A_CALIBRATION_TYPE_DEPTH, pointCloud);
    k4a_image_release(undistortedDepthImage);

    // Copy point cloud data to output image
    uint8_t* srcData = k4a_image_get_buffer(pointCloud);
    uint8_t* dstData = k4a_image_get_buffer(pointCloudImage);
    size_t dataSize = k4a_image_get_size(pointCloud);

    std::memcpy(dstData, srcData, dataSize);

    k4a_image_release(pointCloud);
    k4a_transformation_destroy(transformation);

    return true;
}
