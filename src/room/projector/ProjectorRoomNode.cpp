
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
	setIsCalibrating(false);

	updateCameraPersp();
	calculateViewProjectionMatrix();

	auto colorShader = ci::gl::getStockShader(ci::gl::ShaderDef().color());
	m_wirePlane = ci::gl::Batch::create(ci::geom::WirePlane().size(ci::vec2(20)).subdivisions(ci::ivec2(100)), colorShader);
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
		createWindowOnDisplay();
	}

	if (ImGui::InputInt("Display", &m_DisplayNumber)) {
		createWindowOnDisplay();
	}

	ImGui::Checkbox("Use cameraPersp", &m_useCameraPersp);


	if (ImGui::DragInt2("Resolution", &m_resolution, 1.0, 0.0, 10000, "%i"))
	{
		setResolution(m_resolution);
	}

	ImGui::InputInt("Total Points", &m_totalPoints);

	ImGui::InputInt("Total Rays", &m_totalCalibrationRays);


	if (ImGui::Checkbox("Calibrate", &m_isCalibrating))
	{
		setIsCalibrating(m_isCalibrating);
	}

	if (ImGui::Button("Reset Correspondences"))
	{
		resetCorrespondences();
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

	ImGui::LabelText("Mean Error", "%f", m_meanError);
	ImGui::LabelText("RMS Error", "%f", m_rmsError);
	ImGui::LabelText("GL Error", "%f", m_glMeanError);

	if (ImGui::Button("Calibrate with Test Pairs"))
	{
		calibrateDLT(true);
	}

	ImGui::Checkbox("Show Debug Grid", &m_showDebugGrid);

	ImGui::Checkbox("Show Window Borders", &m_showWindowBorders);

}

ci::Json act::room::ProjectorRoomNode::toParams()
{
	ci::Json json = ci::Json::object();

	json["resolution"] = util::valueToJson(getResolution());
	json["focalLengthPixel"] = util::valueToJson(getFocalLengthPixel());
	json["skew"] = getSkew();
	json["principalPoint"] = util::valueToJson(getPrincipalPoint());
	json["meanError"] = m_meanError;
	json["rmsError"] = m_rmsError;
	json["nextCorrespondence"] = m_nextCorrespondence;
	json["totalPoints"] = m_totalPoints;
	json["totalCalibrationRays"] = m_totalCalibrationRays;



	return json;
}

void act::room::ProjectorRoomNode::fromParams(ci::Json json)
{
	ci::ivec2 resolution = ci::ivec2(0);
	if (util::setValueFromJson(json, "resolution", resolution)) {
		setResolution(resolution, false);
	}

	ci::vec2 focalLengthPixel = ci::vec2(0.0f);
	if (util::setValueFromJson(json, "focalLengthPixel", focalLengthPixel)) {
		setFocalLengthPixel(focalLengthPixel, false);
	}

	float skew = 0;
	if (util::setValueFromJson(json, "skew", skew)) {
		setSkew(skew, false);
	}

	ci::vec2 principalPoint = ci::vec2(0.0f);
	if (util::setValueFromJson(json, "principalPoint", principalPoint)) {
		setPrincipalPoint(principalPoint, false);
	}

	bool isCalibrating;
	if (util::setValueFromJson(json, "isCalibrating", isCalibrating)) {
		setIsCalibrating(isCalibrating, false);
	}

	ci::vec3 objectPoint;
	if (util::setValueFromJson(json, "objectPoint", objectPoint)) {
		addCorrespondence(cv::Point3f(objectPoint.x, objectPoint.y, objectPoint.z));
	}

	float meanError = 0.0f;
	if (util::setValueFromJson(json, "meanError", meanError)) {
		m_meanError = meanError;
	}

	float rmsError = 0.0f;
	if (util::setValueFromJson(json, "rmsError", rmsError)) {
		m_rmsError = rmsError;
	}

	int nextCorrespondence = 0;
	if (util::setValueFromJson(json, "nextCorrespondence", nextCorrespondence)) {
		m_nextCorrespondence = nextCorrespondence;
	}

	int totalPoints = 0;
	if (util::setValueFromJson(json, "totalPoints", totalPoints)) {
		m_totalPoints = totalPoints;
	}

	int totalCalibrationRays = 0;
	if (util::setValueFromJson(json, "totalCalibrationRays", totalCalibrationRays)) {
		m_totalCalibrationRays = totalCalibrationRays;
	}
}

void act::room::ProjectorRoomNode::setResolution(ci::ivec2 resolution, bool publish)
{
	m_resolution = resolution;
	if (publish)
	{
		publishParam("resolution", util::valueToJson(m_resolution));
	}
}

void act::room::ProjectorRoomNode::setIsCalibrating(bool isCalibrating, bool publish)
{
	m_isCalibrating = isCalibrating;
	if (publish)
	{
		publishParam("isCalibrating", m_isCalibrating);
	}
}

void act::room::ProjectorRoomNode::setFocalLengthPixel(ci::vec2 focalLengthPixel, bool publish, bool updateCam)
{
	m_focalLenghtPixel = focalLengthPixel;
	if (publish)
	{
		publishParam("focalLengthPixel", util::valueToJson(m_focalLenghtPixel));
	}
	if (updateCam)
	{
		updateCameraPersp();
		calculateViewProjectionMatrix();
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
		calculateViewProjectionMatrix();
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
		calculateViewProjectionMatrix();
	}
}

void act::room::ProjectorRoomNode::createWindow()
{
	if (m_window) 
		m_window->close();
	
	m_window = WindowData::createWindow(getName(), ivec2(1920, 1080));
	CallbackDrawableRef drawable = CallbackDrawable::create();
	drawable->setDrawCallback(std::bind(&ProjectorRoomNode::drawProjection, this));
	drawable->setUpdateCallback(std::bind(&ProjectorRoomNode::updateProjection, this));
	m_window->getUserData<WindowData>()->setDrawable(drawable);
	m_window->getSignalClose().connect([this]() {
		m_window = nullptr; // reset the window pointer when closed
	});
	m_window->getSignalMouseUp().connect([this](ci::app::MouseEvent evt) {
		// onMouse()

		auto distance = ci::distance(vec2(m_mousePos), vec2(evt.getPos()));

		m_mousePos = evt.getPos();
			
	});
}

void act::room::ProjectorRoomNode::createWindowOnDisplay()
{
	createWindow(); // create new window otherwise context will be wrong
	FullScreenOptions fullScreenOptions;
	if (m_DisplayNumber >= 0 && m_DisplayNumber < Display::getDisplays().size()) {
		fullScreenOptions.display(Display::getDisplays()[m_DisplayNumber]);
		m_window->setFullScreen(true, fullScreenOptions);
	}
	else {
		m_window->setFullScreen(false);
	}
}

void act::room::ProjectorRoomNode::updateProjection()
{
}

void act::room::ProjectorRoomNode::drawProjection()
{	
	gl::clear(ci::Color::black());
	if (m_isCalibrating)
	{
		if (m_nextCorrespondence < m_totalPoints)
		{
			gl::ScopedMatrices();
			gl::setMatricesWindow(getWindowSize());

			int currentRay = getCurrentRay();
			gl::translate(m_calibrationRayCoords[currentRay].x * m_resolution.x, m_calibrationRayCoords[currentRay].y * m_resolution.y);
			drawCalibrationPoint();
		}
		else
		{
			setIsCalibrating(false);
		}
	}
	else if (m_showDebugGrid)
	{
		gl::ScopedMatrices();
		ci::gl::color(1.0f, 1, 1);

		if (m_useCameraPersp)
			gl::setMatrices(m_cameraPersp);
		else
			gl::setProjectionMatrix(m_glProjectionMatrix);

		gl::setViewMatrix(m_glViewMatrix);

		gl::ScopedLineWidth lineWidth(3.0f);
		m_wirePlane->draw();

		gl::ScopedLineWidth lineWidth2(5.0f);
		gl::color(ci::Color(1.0f, 0.5f, 0.5f)); // red for X axis
		gl::drawLine(ci::vec3(0.0f, 0.0f, 0.0f), ci::vec3(0.5f, 0.0f, 0.0f));
		gl::color(ci::Color(0.5f, 0.9f, 1.0f)); // Blue for Z axis
		gl::drawLine(ci::vec3(0.0f, 0.0f, 0.0f), ci::vec3(0.0f, 0.0f, 0.5f));

	}

	if (m_showWindowBorders)
	{
		gl::ScopedMatrices();
		gl::setMatricesWindow(getWindowSize());
		gl::color(ci::Color::white());

		gl::ScopedLineWidth lineWidth(5.0f);
		// Top edge
		gl::drawLine(ci::vec2(0, 0), ci::vec2(m_resolution.x, 0));
		// Right edge
		gl::drawLine(ci::vec2(m_resolution.x, 0), ci::vec2(m_resolution.x, m_resolution.y));
		// Bottom edge
		gl::drawLine(ci::vec2(m_resolution.x, m_resolution.y), ci::vec2(0, m_resolution.y));
		// Left edge
		gl::drawLine(ci::vec2(0, m_resolution.y), ci::vec2(0, 0));
	}

}

void act::room::ProjectorRoomNode::drawCalibrationPoint()
{
	gl::color(ci::Color::white());
	float radius = 2;
	gl::drawSolidCircle(vec2(0), radius);

	int numCircles = 25;

	gl::ScopedLineWidth lineWidth(2.0f);
	for (int i = 2; i <= numCircles; i++)
	{
		gl::color(ci::Color::gray(1.0f - float(i) / float(numCircles)));
		gl::drawStrokedCircle(vec2(), 10 * i * i /2);

	}


}

void act::room::ProjectorRoomNode::updateCameraPersp()
{
	float f = (m_focalLenghtPixel.x + m_focalLenghtPixel.y) / 2;
	float fov = 2 * atan(m_resolution.x / (2 * f)) * 180.0 / CV_PI;
	//float fovY = 2 * atan(m_resolution.y / (2 * m_focalLenghtPixel.y)) * 180.0 / CV_PI;
	m_cameraPersp = ci::CameraPersp(m_resolution.x, m_resolution.y, fov, 0.1f, 30.0f);

	//lens shift 1 -> shifte half the viewport size to the right
	float shiftX = -(m_principalPoint.x / m_resolution.x * 2.0f - 1.0f);
	float shiftY = (m_principalPoint.y / m_resolution.y * 2.0f - 1.0f); //needs to be negated (twice) since y up
	m_cameraPersp.setLensShift(shiftX, shiftY);

	m_cameraPersp.setEyePoint(vec3(0.0f));
	m_cameraPersp.lookAt(vec3(0.0f, 0.0f, -1.0f)); //along negative z (follows from calibration coordinate system convertion)
}

void act::room::ProjectorRoomNode::calculateViewProjectionMatrix()
{
	//view
	glm::mat4 rotationMatrix = glm::toMat4(m_orientation); // Convert quaternion to rotation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_position); // Translate to position

	glm::mat4 cameraMatrix = translationMatrix * rotationMatrix;

	m_glViewMatrix = glm::inverse(cameraMatrix);

	//projection
	float nearZ = 0.1f, farZ = 30.0f;

	float left = -(m_principalPoint.x) * nearZ / m_focalLenghtPixel.x;
	float right = (m_resolution.x - m_principalPoint.x) * nearZ / m_focalLenghtPixel.x;
	float bottom = -(m_resolution.y - m_principalPoint.y) * nearZ / m_focalLenghtPixel.y;
	float top = (m_principalPoint.y) * nearZ / m_focalLenghtPixel.y;

	m_glProjectionMatrix = glm::frustum(left, right, bottom, top, nearZ, farZ);
	m_glProjectionMatrix[1][0] = m_skew / m_focalLenghtPixel.x;
}

void act::room::ProjectorRoomNode::getTestPairs(std::vector<cv::Point3f>& objectPoints, std::vector<cv::Point2f>& imagePoints)
{
	//points from calibration
	///*
	objectPoints = {
	{-0.969012201,  0.104362249,  1.51150644},
	{-0.353259593,  0.495392919,  1.27609563},
	{ 0.859388232,  0.113188505,  0.946744561},
	{ 1.33750129,   0.713171959,  0.730579376},
	{-1.16010177,   0.161237001, -1.73243642},
	{-0.515584528,  0.551542282, -1.41896713},
	{ 0.896920264,  0.096350193, -0.897632599},
	{ 1.17355955,   0.457464814, -0.755033493}
	};

	imagePoints = {
	{192.000000, 108.000000},
	{192.000000, 108.000000},
	{192.000000, 972.000000},
	{192.000000, 972.000000},
	{1728.000000, 108.000000},
	{1728.000000, 108.000000},
	{1728.000000, 972.000000},
	{1728.000000, 972.000000}
	};
	//*/

	// synthetic test pairs
	/*
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
	double pitch = 0.0 * CV_PI / 180.0; // rotation around X
	double yaw = 0.0 * CV_PI / 180.0; // rotation around Y
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
	cv::Mat t = (cv::Mat_<double>(3, 1) << 0.0, 0.0, 5);
	t = -R * t; //to world to cam translation

	//calculate correspondences
	for (auto& p : objectPoints) {
		cv::Mat Pw = (cv::Mat_<double>(3, 1) << p.x, p.y, p.z);
		cv::Mat Pc = R * Pw + t;

		double Xc = Pc.at<double>(0);
		double Yc = Pc.at<double>(1);
		double Zc = Pc.at<double>(2);

		double u = fx * Xc / Zc + skew * Yc / Zc + cx;
		double v = fy * Yc / Zc + cy;
		v = m_resolution.y - v;

		imagePoints.push_back(cv::Point2f(u, v));
	}
	//*/
}

void act::room::ProjectorRoomNode::addCorrespondence(cv::Point3f objectPoint, bool calibrateIfPossible)
{
	if (!m_isCalibrating || m_nextCorrespondence >= m_totalPoints) {
		return;
	}

	int currentRay = getCurrentRay();

	cv::Point2f imagePoint(m_calibrationRayCoords[currentRay].x * m_resolution.x, m_calibrationRayCoords[currentRay].y * m_resolution.y);
	m_imagePoints.push_back(imagePoint);
	m_objectPoints.push_back(objectPoint);

	//update next image Point
	m_nextCorrespondence++;
	
	// issue calibration
	if (calibrateIfPossible && m_nextCorrespondence >= 6)
	{
		calibrateDLT();
	}
}

int act::room::ProjectorRoomNode::getCurrentRay()
{
	int pointsPerRay = m_totalPoints / m_totalCalibrationRays;
	int rest = m_totalPoints % m_totalCalibrationRays;

	std::vector<int> rayOrder;
	rayOrder.reserve(m_totalPoints);

	for (int ray = 0; ray < m_totalCalibrationRays; ++ray) {
		int count = pointsPerRay + (ray < rest ? 1 : 0); //ditribute rest
		for (int i = 0; i < count; ++i) {
			rayOrder.push_back(ray);
		}
	}

	return rayOrder[m_nextCorrespondence]; 
}

void act::room::ProjectorRoomNode::resetCorrespondences()
{
	m_imagePoints.clear();
	m_objectPoints.clear();
	m_nextCorrespondence = 0;
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
		assert(m_objectPoints.size() == m_imagePoints.size() && m_imagePoints.size() >= 6);
		objectPoints = m_objectPoints;
		imagePoints = m_imagePoints;
	}	

	cv::Mat P = dltSolveP(objectPoints, imagePoints);

	m_P = P;

	cv::Mat K, R, t;

	cv::decomposeProjectionMatrix(P, K, R, t);

	/*
	if (cv::determinant(R) < 0) {
		R = -R;
		t = -t;
	}
	*/

	//set intrinsics
	K /= K.at<double>(2, 2); //normalize
	setFocalLengthPixel(ci::vec2(K.at<double>(0, 0), K.at<double>(1, 1)), true, false);
	setSkew(K.at<double>(0, 1), true, false);
	setPrincipalPoint(ci::vec2(K.at<double>(0, 2), K.at<double>(1, 2)), true, false);

	updateCameraPersp();
	calculateViewProjectionMatrix();

	calculateErrors(m_P, objectPoints, imagePoints);

	//set extrinsics
	cv::Mat convertToGL = (cv::Mat_<double>(3, 3) <<
		1, 0, 0,
		0, -1, 0,
		0, 0, -1); // convert to openGL

	R = convertToGL * R; // rotation in World Coordinates optionally convertToCV with right side duplication

	ci::mat3 ciR(
		(float)R.at<double>(0, 0), (float)R.at<double>(0, 1), (float)R.at<double>(0, 2),
		(float)R.at<double>(1, 0), (float)R.at<double>(1, 1), (float)R.at<double>(1, 2),
		(float)R.at<double>(2, 0), (float)R.at<double>(2, 1), (float)R.at<double>(2, 2)
	);
	setOrientation(glm::toQuat(ciR));

	t = t.rowRange(0, 3) / t.at<double>(3); //unhomogenize and cut W
	//t = R * t; //transform to position in world coordinates... not necesarry is already in world coordinates
	setPosition(ci::vec3(t.at<double>(0), t.at<double>(1), t.at<double>(2)));
}

cv::Mat act::room::ProjectorRoomNode::dltSolveP(const std::vector<cv::Point3f> objectPoints, const std::vector<cv::Point2f> imagePoints)
{
	//construc matrix of DLT equation
	cv::Mat A = dltCreateMat(objectPoints, imagePoints);

	//compute SVD
	cv::Mat w, u, vt;
	cv::SVD::compute(A, w, u, vt);

	//use smalles singular value (last row of vt as collum)
	cv::Mat pVec = vt.row(vt.rows - 1);


	//transform Vec to Mat (3 Rows)
	cv::Mat P = pVec.reshape(0, 3);

	//normalize for stability
	if (P.at<double>(2, 3) != 0.0)
		P = P / P.at<double>(2, 3);
	else
		pVec /= cv::norm(pVec) * 12;

	
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
		A.at<double>(2 * i, 3) = 1.0;

		A.at<double>(2 * i, 8) = -u*X;
		A.at<double>(2 * i, 9) = -u*Y;
		A.at<double>(2 * i, 10) = -u*Z;
		A.at<double>(2 * i, 11) = -u;

		//set row 2i + 1 of matrix
		A.at<double>(2 * i + 1, 4) = X;
		A.at<double>(2 * i + 1, 5) = Y;
		A.at<double>(2 * i + 1, 6) = Z;
		A.at<double>(2 * i + 1, 7) = 1.0;

		A.at<double>(2 * i + 1, 8) = -v * X;
		A.at<double>(2 * i + 1, 9) = -v * Y;
		A.at<double>(2 * i + 1, 10) = -v * Z;
		A.at<double>(2 * i + 1, 11) = -v;
	}

	return A;
}

void act::room::ProjectorRoomNode::calculateErrors(const cv::Mat& P, const std::vector<cv::Point3f>& objectPoints, const std::vector<cv::Point2f>& imagePoints)
{
	int numPoints = objectPoints.size();
	std::vector<cv::Point2f> reprojectedPoints;

	double totalSquaredError = 0.0;
	double totalError = 0.0;
	double totalGLError = 0.0;

	for (int i = 0; i < numPoints; ++i)
	{
		const cv::Point3f& objPt = objectPoints[i];

		//Convert to homogeneous coordinates
		cv::Mat X = (cv::Mat_<double>(4, 1) << objPt.x, objPt.y, objPt.z, 1.0);

		//reproject 
		cv::Mat x = P * X;

		//Unhomogenize reprojectefc point
		double u = x.at<double>(0, 0) / x.at<double>(2, 0);
		double v = x.at<double>(1, 0) / x.at<double>(2, 0);

		//Compute error
		const cv::Point2f& imgPt = imagePoints[i];
		double dx = imgPt.x - u;
		double dy = imgPt.y - v;
		double error = dx * dx + dy * dy; //square distance

		totalSquaredError += error;
		totalError += std::sqrt(error);

		//gL projection
		glm::vec4 clip = m_glProjectionMatrix * m_glViewMatrix * glm::vec4(objPt.x, objPt.y, objPt.z, 1.0f);
		
		glm::vec3 ndc = glm::vec3(clip) / clip.w;
		float uGL = (ndc.x + 1.0f) * 0.5f * m_resolution.x;
		float vGL = (ndc.y + 1.0f) * 0.5f * m_resolution.y;

		double dxGL = uGL - u;
		double dyGL = vGL - v;
		double errorGL = dxGL * dxGL + dyGL * dyGL; //square distance

		totalGLError += std::sqrt(errorGL);
	}

	m_meanError = static_cast<float>(totalError / numPoints);
	m_rmsError = static_cast<float>(std::sqrt(totalSquaredError / numPoints));
	m_glMeanError = static_cast<float>(totalGLError / numPoints);

}
