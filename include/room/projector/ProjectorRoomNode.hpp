
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2025

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#pragma once

#include "RoomNodeBase.hpp"
#include <cinder/app/App.h>


using namespace ci;
using namespace ci::app;


namespace act {

	namespace room {

		class ProjectorRoomNode : public RoomNodeBase, public std::enable_shared_from_this<ProjectorRoomNode>
		{
		public:
			ProjectorRoomNode(std::string name, ci::vec3 position, ci::vec3 rotation, float radius, act::UID replyUID = "");

			virtual ~ProjectorRoomNode();

			static std::shared_ptr<ProjectorRoomNode> create(std::string name, ci::vec3 position = ci::vec3(0.0f, 1.0f, 0.0f), ci::vec3 rotation = ci::vec3(0.0f,0.0f,0.0f), float radius = 0.5f, act::UID replyUID = "") { return std::make_shared<ProjectorRoomNode>(name, position, rotation, radius, replyUID); };

			virtual void setup()	override;
			virtual void update()	override;
			virtual void draw()		override;

			virtual void drawSpecificSettings() override;

			virtual ci::Json toParams() override;
			virtual void fromParams(ci::Json json) override;

			void setResolution(ci::ivec2 resolution, bool publish = true);
			ci::ivec2 getResolution() { return m_resolution; };

			void setIsCalibrating(bool isCalibrating, bool publish = true);
			bool getIsCalibrating() { return m_isCalibrating; };

			void setFocalLengthPixel(ci::vec2 focalLengthPixel, bool publish = true, bool updateCam = true);
			ci::vec2 getFocalLengthPixel() { return m_focalLenghtPixel; };

			void setSkew(float skew, bool publish = true, bool updateCam = true);
			float getSkew() { return m_skew; }

			void setPrincipalPoint(ci::vec2 principalPoint, bool publish = true, bool updateCam = true);
			ci::vec2 getPrincipalPoint() { return m_principalPoint; };
			
		private:
			ci::app::WindowRef		m_window;
			ci::CameraPersp			m_cameraPersp;

			ci::ivec2				m_resolution;

			bool					m_isCalibrating;
			ci::vec2				m_focalLenghtPixel;
			float					m_skew;
			ci::vec2				m_principalPoint;


			void createWindow();
			void updateProjection();
			void drawProjection();
			void updateCameraPersp();
			
			//calibration
			//correspondences
			void getTestPairs(std::vector<cv::Point3f>& objectPoints, std::vector<cv::Point2f>& imagePoints);
			std::vector<cv::Point3f> m_objectPoints;
			std::vector<cv::Point2f> m_imagePoints;

			//using cv calibrate camera
			//void calibrateCV();

			//using dlt
			void calibrateDLT(const bool useTestPairs = false);
			cv::Mat dltSolveP(std::vector<cv::Point3f> objectPoints, std::vector<cv::Point2f> imagePoints); //DLT alg on vector of correspondences
			cv::Mat dltCreateMat(std::vector<cv::Point3f> objectPoints, std::vector<cv::Point2f> imagePoints);

			bool isRotationMatrix(const cv::Mat& R);
			ci::vec3 rotationMatrixToEulerAngles(const cv::Mat& R);

		}; using ProjectorRoomNodeRef = std::shared_ptr<ProjectorRoomNode>;
		
	}
}