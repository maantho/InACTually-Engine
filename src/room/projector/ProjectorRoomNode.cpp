
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
	m_cameraPersp = ci::CameraPersp(1920, 1080, 70, 0.1f, 5.0f);
	m_cameraPersp.setEyePoint(vec3(0.0f));
	m_cameraPersp.lookAt(vec3(0.0f, 1.0f, 0.0f));
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
	gl::drawCube(ci::vec3(0.0f), ci::vec3(0.3f, 0.1f, 0.7f));

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

	if (ImGui::Button("Calibrate"))
	{
		calibrate();
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
	gl::drawSolidCircle(getWindowCenter(), radius); // center
	gl::drawSolidCircle(ivec2(padX, padY), radius); // TL
	gl::drawSolidCircle(ivec2(getWindowWidth() - padX, padY), radius); // TR
	gl::drawSolidCircle(ivec2(padX, getWindowHeight() - padY), radius); // BL
	gl::drawSolidCircle(getWindowSize() - ivec2(padX, padY), radius); // BR
}

void act::room::ProjectorRoomNode::calibrate()
{
	//Test values that should result in principle point (0,0) and focal length of 1000mm
	std::vector<cv::Point3f> objectPoints = { 
		cv::Point3f(0,0,0),
		cv::Point3f(100,0,0), 
		cv::Point3f(0,100,0), 
		cv::Point3f(100,100,0), 
		cv::Point3f(50,50,100), 
		cv::Point3f(150,50,100) 
	};
	std::vector<cv::Point2f> imagePoints = {
		cv::Point2f(320, 240),
		cv::Point2f(420, 240),
		cv::Point2f(320, 340),
		cv::Point2f(420, 340),
		cv::Point2f(370, 290),
		cv::Point2f(470, 290)
	};

	cv::Mat P = solveP(objectPoints, imagePoints);

	cv::Mat R, t;

	cv::decomposeProjectionMatrix(P, m_intrinsics, R, t);
	std::cout << m_intrinsics;
}

cv::Mat act::room::ProjectorRoomNode::solveP(std::vector<cv::Point3f> objectPoints, std::vector<cv::Point2f> imagePoints)
{
	//construc matrix of DLT equation
	cv::Mat A = createDLTMat(objectPoints, imagePoints);

	//compute SVD
	cv::Mat w, u, vt;
	cv::SVD::compute(A, w, u, vt);

	//use smalles singular value (last row of vt as collum)
	cv::Mat pVec = vt.row(vt.rows - 1).t();

	//normalize for stability
	cv::norm(pVec);

	//transform Vec to Mat (3 Rows)
	cv::Mat P = pVec.reshape(0, 3);
	
	return P;
}


//create matrix A of DLT equation
cv::Mat act::room::ProjectorRoomNode::createDLTMat(std::vector<cv::Point3f> objectPoints, std::vector<cv::Point2f> imagePoints)
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
