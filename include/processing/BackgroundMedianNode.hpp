
/*
	InACTually
	2021

	participants:
	Lars Engeln - mail@lars-engeln.de

	interactive theater for actual acts
*/

#pragma once

#include "NodeBase.hpp"
#include "MatListener.hpp"

#include "CameraManager.hpp"

using namespace ci;
using namespace ci::app;

namespace act {
	namespace proc {

		class CameraNode : public NodeBase
		{
		public:
			CameraNode();
			~CameraNode();

			NODECREATE(CameraNode);

			void setup(act::mod::DeviceManagers deviceMgrs)			override;
			void update()			override;
			void draw()				override;

			ci::Json toJson() override;
			void fromJson(ci::Json json) override;

		private:
			act::scn::CameraManagerRef m_camMgr;
			int						m_selectedCamera = 0;
			std::string				m_selectedCameraName;

			ci::SurfaceRef			m_captureSurface;
			ci::gl::Texture2dRef	m_captureTexture;

			ci::ivec2				m_captureSize;
			ci::ivec2				m_displaySize;

			bool					m_show;

			cv::VideoWriter			writer;

			OutputPortRef<cv::UMat>	m_cameraImageOutPort;
			InputPortRef<cv::UMat>	m_cameraImageInPort;

			void attachCamera();
		};

		using CameraNodeRef = std::shared_ptr<CameraNode>;

	}
}