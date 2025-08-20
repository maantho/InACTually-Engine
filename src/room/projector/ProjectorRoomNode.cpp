
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

#include "roompch.hpp"
#include "projector/ProjectorRoomNode.hpp"
#include "WindowData.hpp"
#include "CallbackDrawable.hpp"


act::room::ProjectorRoomNode::ProjectorRoomNode(std::string name, ci::vec3 position, ci::vec3 rotation, float radius, act::UID replyUID)
	: RoomNodeBase("projector", position, rotation, radius, replyUID)
{	
	setResolution(ci::ivec2(1920, 1080));
	setFocalLengthPixel(ci::vec2(800, 800));
	setSkew(0);
	setPrincipalPoint(ci::vec2(0,0));

	updateCameraPersp();
}

act::room::ProjectorRoomNode::~ProjectorRoomNode()
{
	if (m_window) // if the window is still open, close it
		m_window->close();
}

void act::room::ProjectorRoomNode::setup()
{
}

void act::room::ProjectorRoomNode::update()
{
}

void act::room::ProjectorRoomNode::draw()
{
	gl::ScopedColor color;
	enableStatusColor(); 

	gl::pushMatrices();
	gl::translate(m_position);
	gl::rotate(m_rotation);
	gl::drawSphere(ci::Sphere(ci::vec3(0.0f), 0.05f));
	gl::drawCube(ci::vec3(0.1f, 0.0f, 0.15f), ci::vec3(0.4f, 0.15f, 0.3f));

	util::drawCoords();

	if (getIsUnfolded())
		gl::color(util::Design::highlightColor(0.85f));
	else
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 0.6f));
	gl::scale(vec3(0.1, 0.1, 0.1));
	gl::drawFrustum(m_cameraPersp);
	gl::popMatrices();
}

void act::room::ProjectorRoomNode::drawSpecificSettings()
{
	if(ImGui::Button("Open Projector Window"))
	{
		if (!m_window)
			createWindow();
		else
			m_window->show();
	}

	if (ImGui::DragInt2("Resolution", &m_resolution, 1.0, 0.0, 10000, "%i"))
	{
		setResolution(m_resolution);
	}

	if (ImGui::Button("Calibrate with DLT"))
	{
		calibrateDLT(true);
	}

	if (ImGui::DragFloat2("Focal Length", &m_focalLenghtPixel))
	{
		setFocalLengthPixel(m_focalLenghtPixel);
	}

	if (ImGui::DragFloat("Skew", &m_skew))
	{
		setSkew(m_skew);
	}

	if (ImGui::DragFloat2("Principle Point", &m_principalPoint))
	{
		setPrincipalPoint(m_principalPoint);
	}
}

ci::Json act::room::ProjectorRoomNode::toParams()
{
	ci::Json json = ci::Json::object();
	
	return json;
}

void act::room::ProjectorRoomNode::fromParams(ci::Json json)
{
}

void act::room::ProjectorRoomNode::setResolution(ci::ivec2 resolution, bool publish)
{
	m_resolution = resolution;
	if (publish)
	{
		publishParam("resolution", util::valueToJson(m_resolution));
	}
}

void act::room::ProjectorRoomNode::setFocalLengthPixel(ci::vec2 focalLengthPixel, bool publish, bool updateCam)
{
	m_focalLenghtPixel = focalLengthPixel;
	if (publish)
	{
		publishParam("focalLenghtPixel", util::valueToJson(m_focalLenghtPixel));
	}
	if (updateCam)
	{
		updateCameraPersp();
	}
}

void act::room::ProjectorRoomNode::setSkew(float skew, bool publish, bool updateCam)
{
	m_skew = skew;
	if (publish)
	{
		publishParam("skew", m_skew);
	}
	if (updateCam)
	{
		updateCameraPersp();
	}
}

void act::room::ProjectorRoomNode::setPrincipalPoint(ci::vec2 principalPoint, bool publish, bool updateCam)
{
	m_principalPoint = principalPoint;
	if (publish)
	{
		publishParam("principalPoint", util::valueToJson(m_principalPoint));
	}
	if (updateCam)
	{
		updateCameraPersp();
	}
}

void act::room::ProjectorRoomNode::createWindow()
{
	m_window = WindowData::createWindow(getName(), ivec2(1920, 1080));
	CallbackDrawableRef drawable = CallbackDrawable::create();
	drawable->setDrawCallback(std::bind(&ProjectorRoomNode::drawProjection, this));
	drawable->setUpdateCallback(std::bind(&ProjectorRoomNode::updateProjection, this));
	m_window->getUserData<WindowData>()->setDrawable(drawable);
	m_window->getSignalClose().connect([this]() {
		m_window = nullptr; // reset the window pointer when closed
	});
}

void act::room::ProjectorRoomNode::updateProjection()
{
}

void act::room::ProjectorRoomNode::drawProjection()
{		
	gl::clearColor(ci::Color::gray(0.5f));
	gl::color(ci::Color::white());

	float radius = 10;
	float padding = radius; // 0.1f;
	float padX = padding; // getWindowWidth()* padding;
	float padY = padding; // getWindowHeight()* padding;
	gl::drawSolidCircle(ivec2(padX, padY), radius); // TL
	gl::color(ci::Color::gray(0.5));
	gl::drawSolidCircle(getWindowCenter(), radius); // center
	gl::drawSolidCircle(ivec2(getWindowWidth() - padX, padY), radius); // TR
	gl::drawSolidCircle(ivec2(padX, getWindowHeight() - padY), radius); // BL
	gl::drawSolidCircle(getWindowSize() - ivec2(padX, padY), radius); // BR
}

void act::room::ProjectorRoomNode::updateCameraPersp()
{
	float fovX = 2 * atan(m_resolution.x / (2 * m_focalLenghtPixel.x)) * 180.0 / CV_PI;
	//float fovY = 2 * atan(m_resolution.y / (2 * m_focalLenghtPixel.y)) * 180.0 / CV_PI;
	m_cameraPersp = ci::CameraPersp(m_resolution.x, m_resolution.y, fovX, 0.1f, 30.0f);

	//lens shift 1 -> shifte half the viewport size to the right
	float shiftX = -(m_principalPoint.x / m_resolution.x * 2.0f - 1.0f);
	float shiftY = (m_principalPoint.y / m_resolution.y * 2.0f - 1.0f); //needs to be (double) negated since y up
	m_cameraPersp.setLensShift(shiftX, shiftY);

	m_cameraPersp.setEyePoint(vec3(0.0f));
	m_cameraPersp.lookAt(vec3(0.0f, 0.0f, -1.0f)); //along negative z (follows from calibration coordinate system convertion)
}

void act::room::ProjectorRoomNode::getTestPairs(std::vector<cv::Point3f>& objectPoints, std::vector<cv::Point2f>& imagePoints)
{

	objectPoints = {
		{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
		{-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}
	};

	double fx = 800.0;   
	double fy = 800.0;
	double cx = 1920.0 / 2.0;   
	double cy = 240.0;
	double skew = 0;  

	//camera rotation
	double pitch = 15.0 * CV_PI / 180.0; // rotation around X
	double yaw = 45.0 * CV_PI / 180.0; // rotation around Y
	double roll = 0.0 * CV_PI / 180.0; // rotation around Z


	cv::Mat Rx = (cv::Mat_<double>(3, 3) <<
		1, 0, 0,
		0, cos(pitch), -sin(pitch),
		0, sin(pitch), cos(pitch));

	cv::Mat Ry = (cv::Mat_<double>(3, 3) <<
		cos(yaw), 0, sin(yaw),
		0, 1, 0,
		-sin(yaw), 0, cos(yaw));

	cv::Mat Rz = (cv::Mat_<double>(3, 3) <<
		cos(roll), -sin(roll), 0,
		sin(roll), cos(roll), 0,
		0, 0, 1);

	//combine in order: yaw pitch roll 
	cv::Mat R = Ry * Rx * Rz;
	R = R.t(); //to inverse world to cam rotation
	
	//camera center
	cv::Mat t = (cv::Mat_<double>(3, 1) << -5.0, 2.0, -5.0);
	t = -R * t; //to world to cam translation

	//calculate correspondences
	for (auto& p : objectPoints) {
		cv::Mat Pw = (cv::Mat_<double>(3, 1) << p.x, p.y, p.z);
		cv::Mat Pc = R * Pw + t;

		double Xc = Pc.at<double>(0);
		double Yc = -Pc.at<double>(1);
		double Zc = Pc.at<double>(2);

		double u = fx * Xc / Zc + skew * Yc / Zc + cx;
		double v = fy * Yc / Zc + cy;

		imagePoints.push_back(cv::Point2f(u, v));
	}
}

/* //does not work
void act::room::ProjectorRoomNode::calibrateCV()
{
	std::vector<cv::Point3f> objectPoints;
	std::vector<cv::Point2f> imagePoints;

	getTestPairs(objectPoints, imagePoints);

	std::vector<std::vector<cv::Point3f>> wrappedObjectPoints = { objectPoints };
	std::vector<std::vector<cv::Point2f>> wrappedImagePoints = { imagePoints };

	cv::Mat K, distCoeffs;
	std::vector<cv::Mat> rvecs, tvecs;

	CV_Assert(!wrappedImagePoints.empty());
	CV_Assert(!wrappedObjectPoints.empty());
	CV_Assert(wrappedImagePoints.size() == wrappedObjectPoints.size());
	CV_Assert(wrappedImagePoints.size() > 0);
	CV_Assert(wrappedImagePoints[0].size() == wrappedObjectPoints[0].size());
	CV_Assert(wrappedImagePoints[0].size() >= 6);
	CV_Assert(m_resolution.x > 6 && m_resolution.y > 6);

	cv::calibrateCamera(wrappedObjectPoints, wrappedImagePoints, cv::Size(1920, 1080), K, distCoeffs, rvecs, tvecs);

	//set intrinsics
	
} */

void act::room::ProjectorRoomNode::calibrateDLT(const bool useTestPairs)
{
	std::vector<cv::Point3f> objectPoints;
	std::vector<cv::Point2f> imagePoints;
	
	if (useTestPairs)
	{
		getTestPairs(objectPoints, imagePoints);
	}
	else
	{
		assert(m_objectPoints.size() == m_imagePoints.size() >= 6);
		objectPoints = m_objectPoints;
		imagePoints = m_imagePoints;
	}	

	cv::Mat P = dltSolveP(objectPoints, imagePoints);

	cv::Mat K, R, t;

	cv::decomposeProjectionMatrix(P, K, R, t);

	//set intrinsics
	K /= K.at<double>(2, 2); //normalize
	setFocalLengthPixel(ci::vec2(K.at<double>(0, 0), K.at<double>(1, 1)), true, false);
	setSkew(K.at<double>(0, 1), true, false);
	setPrincipalPoint(ci::vec2(K.at<double>(0, 2), K.at<double>(1, 2)), true, false);
	updateCameraPersp();

	//set extrinsics
	//cv::Mat convertToCV = (cv::Mat_<double>(3, 3) <<
	//	1, 0, 0,
	//	0, -1, 0,
	//	0, 0, -1); //optinally convert rotation to CV convention (y down, z foreward) or define cameraPersp in -z direction with y up
	t = t.rowRange(0, 3) / t.at<double>(3); //unhomogenize and cut W
	R = R.t(); // rotation in World Coordinates optionally convertToCV with right side duplication

	//tVec = R.t() * tVec; //transform to position in world coordinates... not necesarry is already in world coordinates
	setPosition(ci::vec3(t.at<double>(0), t.at<double>(1), t.at<double>(2)));

	setRotation(rotationMatrixToEulerAngles(R));
}

cv::Mat act::room::ProjectorRoomNode::dltSolveP(const std::vector<cv::Point3f> objectPoints, const std::vector<cv::Point2f> imagePoints)
{
	//construc matrix of DLT equation
	cv::Mat A = dltCreateMat(objectPoints, imagePoints);

	//compute SVD
	cv::Mat w, u, vt;
	cv::SVD::compute(A, w, u, vt);

	//use smalles singular value (last row of vt as collum)
	cv::Mat pVec = vt.row(vt.rows - 1).t();

	//normalize for stability
	pVec /= cv::norm(pVec) * 12;

	//transform Vec to Mat (3 Rows)
	cv::Mat P = pVec.reshape(0, 3);
	
	return P;
}


//create matrix A of DLT equation
cv::Mat act::room::ProjectorRoomNode::dltCreateMat(const std::vector<cv::Point3f> objectPoints, const std::vector<cv::Point2f> imagePoints)
{	
	//check if object = image points !!!
	
	//create Matrix
	int	nPairs = (int)objectPoints.size();
	cv::Mat A(2 * nPairs, 12, CV_64F, cv::Scalar(0));

	//asign corespondences according to sheme
	for (int i = 0; i < nPairs; i++)
	{
		//current opject pos
		double X = objectPoints[i].x;
		double Y = objectPoints[i].y;
		double Z = objectPoints[i].z;

		//current imagePos
		double u = imagePoints[i].x;
		double v = imagePoints[i].y;

		//set row 2i of matrix
		A.at<double>(2 * i, 0) = X;
		A.at<double>(2 * i, 1) = Y;
		A.at<double>(2 * i, 2) = Z;
		A.at<double>(2 * i, 3) = 1;

		A.at<double>(2 * i, 8) = -u*X;
		A.at<double>(2 * i, 9) = -u*Y;
		A.at<double>(2 * i, 10) = -u*Z;
		A.at<double>(2 * i, 11) = -u;

		//set row 2i + 1 of matrix
		A.at<double>(2 * i + 1, 4) = X;
		A.at<double>(2 * i + 1, 5) = Y;
		A.at<double>(2 * i + 1, 6) = Z;
		A.at<double>(2 * i + 1, 7) = 1;

		A.at<double>(2 * i + 1, 8) = -v * X;
		A.at<double>(2 * i + 1, 9) = -v * Y;
		A.at<double>(2 * i + 1, 10) = -v * Z;
		A.at<double>(2 * i + 1, 11) = -v;
	}

	return A;
}

bool act::room::ProjectorRoomNode::isRotationMatrix(const cv::Mat& R)
{
	//https://learnopencv.com/rotation-matrix-to-euler-angles/

	cv::Mat Rt;
	transpose(R, Rt);
	cv::Mat shouldBeIdentity = Rt * R;
	cv::Mat I = cv::Mat::eye(3, 3, shouldBeIdentity.type());

	return  norm(I, shouldBeIdentity) < 1e-6;

}

ci::vec3 act::room::ProjectorRoomNode::rotationMatrixToEulerAngles(const cv::Mat& R)
{
	//https://learnopencv.com/rotation-matrix-to-euler-angles/

	assert(isRotationMatrix(R));

	float sy = sqrt(R.at<double>(0, 0) * R.at<double>(0, 0) + R.at<double>(1, 0) * R.at<double>(1, 0));

	bool singular = sy < 1e-6; // If

	float x, y, z;
	if (!singular)
	{
		x = atan2(R.at<double>(2, 1), R.at<double>(2, 2));
		y = atan2(-R.at<double>(2, 0), sy);
		z = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
	}
	else
	{
		x = atan2(-R.at<double>(1, 2), R.at<double>(1, 1));
		y = atan2(-R.at<double>(2, 0), sy);
		z = 0;
	}

	//normalize to positive between 0 and 2pi
	x = fmod(fmod(x, CV_PI * 2) + CV_PI * 2, CV_PI * 2);
	y = fmod(fmod(y, CV_PI * 2) + CV_PI * 2, CV_PI * 2);
	z = fmod(fmod(z, CV_PI * 2) + CV_PI * 2, CV_PI * 2);

	return ci::vec3(x, y, z);

}
