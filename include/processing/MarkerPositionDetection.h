#pragma once
/*
InACTually
2023

Jonas Höpner
Antonia Gerdes
Katharina Volkenand
Leon Augustat
*/

#include "NodeBase.hpp"
#include "MatListener.hpp"
#include "CameraManager.hpp"
#include "CameraSceneNode.hpp"
#include "MarkerSceneNode.hpp"
#include "SceneNodeManagerBase.hpp"
#include "MarkerPosition.hpp"

#include <opencv2/aruco.hpp>

using namespace ci;
using namespace ci::app;
using namespace act::scn;

namespace act {
	namespace proc {

		class MarkerPositionDetection : public NodeBase {
		public:
			MarkerPositionDetection();
			~MarkerPositionDetection();

			NODECREATE(MarkerPositionDetection);

			void setup(act::scn::DeviceManagers deviceMgrs)			override;
			void update()			override;
			void draw()				override;

			void onMat(cv::UMat event);

			ci::Json toJson() override;
			void fromJson(ci::Json json) override;
			std::vector<int> getMarkerIds() { return m_markerIds; }
			std::vector<std::string> getMarkerIdsAsStrings();

		private:
			act::scn::CameraManagerRef cameraMgr;
			act::scn::MarkerManagerRef markerMgr;
			scn::DMXManagerRef m_dmxMgr;
			cv::Mat cameraMatrix, distCoeffs; // camera calibration

			std::thread m_continousThread; // threads for the checkboxes in Backend
			std::shared_ptr<bool> sp; // used to end threads

			int m_originMarkerId; // first marker to be detected as "fixed"
			std::vector<int> m_markerIds; // saves the detected marker ids
			std::vector<MarkerPosition*>	m_nodes; // saves the detected markers

			static bool m_registered;
			bool m_continous = false; // BE checkbox
			float markerLength = 0.15; // approximate size of the markers
			
			std::string status = "";
			int currentlyDetectedMarkers = 0;

			cv::Mat objPoints = cv::Mat(4, 1, CV_32FC3);
			cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
            // the type of marker to be detected
            // Future Work: Make this selectable in BE
			cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);

			std::shared_ptr<OutputPort<ci::Json>>	m_markerPort;

			cv::aruco::ArucoDetector detector = cv::aruco::ArucoDetector(dictionary, parameters);


			std::vector<cv::Vec3d> inversePerspective(cv::Vec3d rvec, cv::Vec3d tvec);

			std::vector<glm::vec3> calculateCameraOffsetForGivenMarker(MarkerPosition* marker);

			void findMarkers();
			MarkerPosition* getMarkerById(int id);
			virtual void refreshDeviceList() {};
			MarkerPosition* getNodeFromNodes(int id);
			void tryHighlightFoundDevice(int markerId);

			std::vector<double> eulerToRad(cv::Mat rotationMatrix);
			cv::Vec2i checkForLowerMarkerIndex(int markerID, int lowestIndex, int i);
			void updateDevicePositions();
		};

		using MarkerPositionDetectionRef = std::shared_ptr<MarkerPositionDetection>;
		using CameraSceneNodeRef = std::shared_ptr<CameraSceneNode>;

	}
}

