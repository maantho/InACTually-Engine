
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2022

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#include "roompch.hpp"
#include "kinect/KinectDummy.hpp"

using namespace cinder::app;
using namespace act;
using namespace room;

KinectDummy::KinectDummy(std::uint32_t deviceIndex, std::string name, std::string path, ci::Json bodyData)
{
	m_isDummy = true;

	m_name = name;
 
	m_playback = NULL;
	m_path = path;
	openDevice();
}


void KinectDummy::update()
{	
	if (m_state == KinectState::KS_CAPTURING)
	{
#ifdef WITHKINECT
		k4a::capture capture;
		if (m_device.get_capture(&capture, std::chrono::milliseconds(K4A_WAIT_INFINITE)))
		{
			k4a::image colorImage = capture.get_color_image();
			k4a::image depthImage = capture.get_depth_image();
			if (m_isCapturingImage)
				updateColor(colorImage);

			if (m_isCapturingDepth) {
				updateDepth(depthImage);
			}

			if (m_isProvidingPointCloud && depthImage != nullptr) {
				updatePointCloud(depthImage, colorImage);
			}


			if (m_isCapturingIR)
				updateIR(capture);


		}
#endif
	}
	 
}

KinectDummy::~KinectDummy()
{
}

void KinectDummy::openDevice()
{
	if (m_path == "")
		return;

	k4a_result_t result = k4a_playback_open(m_path.c_str(), &m_playback);

	if (result == K4A_RESULT_SUCCEEDED) {
		m_state = KinectState::KS_OPENED;
		setIsProvidingPointCloud(true);
	}
	else {
		CI_LOG_E("Failed to open playback");

	}

	k4a_playback_get_calibration(m_playback, &m_sensorCalibration);
	m_transformation = k4a_transformation_create(&m_sensorCalibration);


	k4a_playback_set_color_conversion(m_playback, K4A_IMAGE_FORMAT_COLOR_BGRA32);
}

void KinectDummy::startDevice(bool mode)
{
	m_state = KS_CAPTURING;
}

void KinectDummy::stopDevice()
{
	m_state = KS_OPENED;
}