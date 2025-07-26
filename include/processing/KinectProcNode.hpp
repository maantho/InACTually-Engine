
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

#pragma once

#include "ProcNodeBase.hpp"
#include "MatListener.hpp"

#include "kinect/KinectManager.hpp"

#include "VideoRecorderProcNode.hpp"

using namespace ci;
using namespace ci::app;

namespace act {
	namespace proc {

		class KinectProcNode : public ProcNodeBase
		{
		public:
			KinectProcNode();
			~KinectProcNode();

			PROCNODECREATE(KinectProcNode);

			void setup(act::room::RoomManagers roomMgrs)			override;
			void update()			override;
			void draw()				override;

			ci::Json toParams() override;
			void fromParams(ci::Json json) override;

			bool isActive() { return m_active; };

		private:
			act::room::KinectManagerRef	m_kinectMgr;
			act::room::KinectDeviceRef			m_kinect;

			int				m_selectedKinect;
			std::string		m_selectedKinectName;

			ci::ivec2		m_captureSize;

			bool			m_active = false;

			bool			m_isShowingJoints = false;

			bool 			m_isDisconnected = false;

			ci::gl::Texture2dRef	m_captureTexture;

			InputPortRef<cv::UMat>  m_kinectImageInPort;
			InputPortRef<cv::UMat>  m_kinectDepthInPort;
			InputPortRef<cv::UMat>  m_kinectInfraRedInPort;
			InputPortRef<cv::UMat>  m_kinectBIMInPort;

			InputPortRef<room::BodyRefList> m_kinectBodiesInPort;

			OutputPortRef<cv::UMat> m_kinectImageOutPort;
			OutputPortRef<cv::UMat> m_kinectDepthRawOutPort;
			OutputPortRef<cv::UMat> m_kinectDepthOutPort;
			OutputPortRef<cv::UMat> m_kinectDepthFilledOutPort;
			OutputPortRef<cv::UMat> m_kinectDepthVisualizedOutPort;
			OutputPortRef<cv::UMat> m_kinectInfraRedOutPort;
			OutputPortRef<cv::UMat> m_kinectBIMOutPort;
			OutputPortRef<cv::UMat> m_kinectVisualizedBIMOutPort;
			OutputPortRef<ci::vec2> m_kinectFOVPort;

			OutputPortRef<room::BodyRefList> m_kinectBodiesOutPort;

			bool		m_isRecording = false;
			bool		m_isStartingRecording = false;
			bool		m_isPreparedForRecording = false;
			bool		m_isSaveDialog = false;
			std::string	m_path;
			void onRGBImage(cv::UMat image);
			void onDepthImage(cv::UMat image);
			void onIRImage(cv::UMat image);
			void onBodyIndexMap(cv::UMat image);
			void onBodies(room::BodyRefList& bodies);

			VideoRecorderProcNodeRef	m_rgbRec;
			VideoRecorderProcNodeRef	m_depthRec;
			VideoRecorderProcNodeRef	m_irRec;
			VideoRecorderProcNodeRef	m_bimRec;
			ci::Json				m_bodyRec;

			cv::UMat				m_lastRGBImage;
			cv::UMat				m_lastDepthImage;
			cv::UMat				m_lastIRImage;
			cv::UMat				m_lastBIM;
			ci::Json				m_lastBodies;

			void prepareRecording();
			void startRecording();
			void stopRecording();
			void recordCurrentFrames();

			void activate();

		}; using KinectProcNodeRef = std::shared_ptr<KinectProcNode>;
	}
}