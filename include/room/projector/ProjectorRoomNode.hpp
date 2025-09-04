
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021�2025 Lars Engeln, Fabian T�pfer
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
			int						m_DisplayNumber = 2;
			ci::CameraPersp			m_cameraPersp;
			ci::mat4				m_glProjectionMatrix;
			ci::mat4				m_glViewMatrix;

			bool					m_useCameraPersp = false;

			ci::ivec2				m_resolution;

			bool					m_isCalibrating;

			int						m_nextCorrespondence = 0;
			int						m_totalPoints = 6;
			int						m_totalCalibrationRays = 4;

			const std::vector<cv::Point2f> m_calibrationRayCoords = {
				// used for 4 points
				cv::Point2f(1.0f / 6.0f, 1.0f / 3.0f),
				cv::Point2f(1.0f / 6.0f, 2.0f / 3.0f),
				cv::Point2f(5.0f / 6.0f, 1.0f / 3.0f),
				cv::Point2f(5.0f / 6.0f, 2.0f / 3.0f),

				//additionally for  up to 10 points
				cv::Point2f(2.0f / 6.0f, 1.0f / 3.0f),
				cv::Point2f(3.0f / 6.0f, 1.0f / 3.0f),
				cv::Point2f(4.0f / 6.0f, 1.0f / 3.0f),
				cv::Point2f(2.0f / 6.0f, 2.0f / 3.0f),
				cv::Point2f(3.0f / 6.0f, 2.0f / 3.0f),
				cv::Point2f(4.0f / 6.0f, 2.0f / 3.0f),
			};

			cv::Mat					m_P;
			ci::vec2				m_focalLenghtPixel;
			float					m_skew;
			ci::vec2				m_principalPoint;

			ci::ivec2				m_mousePos = ivec2(0);

			ci::gl::BatchRef		m_wirePlane;
			bool					m_showDotPattern = false;
			bool					m_showDebugGrid = false;
			bool					m_showWindowBorders = true;

			float					m_totalError = 0.0f;
			float 					m_meanError = 0.0f;
			float                   m_minError = 0.0f;
			float                   m_maxError = 0.0f;
			float					m_totalSpuareError = 0.0f;
			float                   m_rmsError = 0.0f;



			float					m_glMeanError = 0.0f;

			void createWindow();
			void createWindowOnDisplay();

			void updateProjection();
			void drawProjection();
			void drawCalibrationPoint();
			void drawDotPattern();
			void drawDotGroundGrid();

			ci::vec2 getDotFromIndex(int i);

			void updateCameraPersp();
			void calculateViewProjectionMatrix();
			
			//calibration
			//correspondences
			void getTestPairs(std::vector<cv::Point3f>& objectPoints, std::vector<cv::Point2f>& imagePoints);
			std::vector<cv::Point3f> m_objectPoints;
			std::vector<cv::Point2f> m_imagePoints;

			void addCorrespondence(cv::Point3f objectPoint, bool calibrateIfPossible = true);
			int getCurrentRay();
			void resetCorrespondences();

			//using cv calibrate camera
			//void calibrateCV();

			//using dlt
			void calibrateDLT(const bool useTestPairs = false);
			cv::Mat dltSolveP(std::vector<cv::Point3f> objectPoints, std::vector<cv::Point2f> imagePoints); //DLT alg on vector of correspondences
			cv::Mat dltCreateMat(std::vector<cv::Point3f> objectPoints, std::vector<cv::Point2f> imagePoints);

			void calculateErrors(const cv::Mat& P, const std::vector<cv::Point3f>& objectPoints, const std::vector<cv::Point2f>& imagePoints);

		}; using ProjectorRoomNodeRef = std::shared_ptr<ProjectorRoomNode>;
		
	}
}