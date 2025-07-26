#include "MarkerPositionDetection.h"
#include "imnodes.h"

#include "Design.hpp"

// register node
bool act::proc::MarkerPositionDetection::m_registered = act::proc::NodeRegistry::add("MarkerPositionDetection", act::proc::MarkerPositionDetection::create);

using act::proc::MarkerPosition;

std::weak_ptr<bool> markerPositionDetectionThreadsRunning; // used to stop threads

act::proc::MarkerPositionDetection::MarkerPositionDetection() : NodeBase("MarkerPositionDetection")
{
    /**
     * The output port for sending detected markers
     * @refitem MarkerPosition
     */
	m_markerPort = OutputPort<ci::JsonTree>::create(PT_JSON, "detected Markers");
	m_outputPorts.push_back(m_markerPort);

    // set origin marker id to some sane value. Else it's undefined
	m_originMarkerId = 5;
	sp = std::make_shared<bool>(true);
	markerPositionDetectionThreadsRunning = sp;

	//add origin to Ids & nodes
	MarkerPosition* originmarker = new MarkerPosition(m_originMarkerId, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0));
	originmarker->setIsFixed(true);
	m_markerIds.push_back(m_originMarkerId);
	m_nodes.push_back(originmarker);

    // thread for continuous marker detection.
    // calls findMarker every second
	m_continousThread = std::thread([&]() {
		while (true) {
			sleep(1000);
			
			std::shared_ptr<bool> shrd = markerPositionDetectionThreadsRunning.lock();
			if (shrd) {
				if (m_continous) {
					try {
						findMarkers();
					}
					catch (Exception e) {
						console() << e.what();
						continue;
					}
					catch (cv::Exception e) {
						console() << e.what();
						continue;
					}
				}
			}
			else break;
		}
	});
}

act::proc::MarkerPositionDetection::~MarkerPositionDetection()
{
	// reset the shared pointer, so no other strong refs exist. Next time, the thread wakes up the sp will be expired and the thread will become joinable.
	sp.reset();
	m_continousThread.join();
}

void act::proc::MarkerPositionDetection::setup(act::mod::DeviceManagers deviceMgrs)
{
	cameraMgr = deviceMgrs.cameraMgr;
	markerMgr = deviceMgrs.markerMgr;
	m_dmxMgr = deviceMgrs.dmxMgr;
}

void act::proc::MarkerPositionDetection::update()
{
}

void act::proc::MarkerPositionDetection::draw()
{
	ivec2 displaySize = ivec2(100, 200);

	ImNodes::BeginNode(m_id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(NodeBase::getName().c_str());
	ImNodes::EndNodeTitleBar();

	bool isSlider = false;
	ImGui::PushItemWidth(400);
	isSlider = ImGui::SliderFloat("markerLength", &markerLength, 0.f, 1.f);

	preventDrag(isSlider);
	ImGui::PopItemWidth();

	if (ImGui::InputInt("Origin Marker Id", &m_originMarkerId, 1, 1)) {
		m_originMarkerId = std::clamp(m_originMarkerId, 0, 4095);
		//change ID of origin marker in list
		m_markerIds[0] = m_originMarkerId;
		m_nodes[0]->setId(m_originMarkerId);
	}
	
	if (ImGui::Button("Find markers")) {
		findMarkers();
	}

	ImGui::Checkbox("Continous Finding", &m_continous);

	std::stringstream ss;
	ss << "Detected markers: " << m_markerIds.size() << " | in last image: " << currentlyDetectedMarkers;
	ImGui::Text(ss.str().c_str());
	
	ImGui::Text(status.c_str());
	
	
	drawPorts(displaySize.x);

	vec2 newPos = NodeBase::getPosition();
	if (newPos != NodeBase::m_position && m_initialized) {
		NodeBase::m_position = newPos;
	}
	setDrawingPosition(NodeBase::m_position);
	if (!m_initialized) {
		m_initialized = true;
	}

	ImNodes::EndNode();
}

ci::JsonTree act::proc::MarkerPositionDetection::toJson()
{
	ci::JsonTree json = ci::JsonTree::makeObject("params");
	json.addChild(ci::JsonTree("originMarkerId", m_originMarkerId));
	json.addChild(ci::JsonTree("continuousDetection", m_continous));
	json.addChild(ci::JsonTree("markerLength", markerLength));
	return json;
}

void act::proc::MarkerPositionDetection::fromJson(ci::JsonTree json)
{
	util::setValueFromJson(json, "originMarkerId", m_originMarkerId);
	//change ID of origin marker in list
	m_markerIds[0] = m_originMarkerId;
	m_nodes[0]->setId(m_originMarkerId);
	util::setValueFromJson(json, "continuousDetection", m_continous);
	util::setValueFromJson(json, "markerLength", markerLength);
}

std::vector<std::string> act::proc::MarkerPositionDetection::getMarkerIdsAsStrings()
{
		std::vector<std::string> markerStrings;
		for (auto markerId : m_markerIds) {
			markerStrings.push_back(std::to_string(markerId));
		}
		return markerStrings;
}


act::proc::MarkerPosition* act::proc::MarkerPositionDetection::getMarkerById(int id) {
	for (auto&& node : m_nodes) {
		MarkerPosition *marker = node;
		if (marker->getId() == id) {
			return marker;
		}
	}
	return nullptr;
}

// code from MarkerManager
// does some magic to make Worldcoordinates from cameracoordinates
std::vector<cv::Vec3d> act::proc::MarkerPositionDetection::inversePerspective(cv::Vec3d rvec, cv::Vec3d tvec) {
	cv::Mat rotMat;
	cv::Rodrigues(rvec, rotMat);
	cv::transpose(rotMat, rotMat);
	cv::Mat invTvec = rotMat * (-tvec);
	cv::Vec3d invTvec2 = cv::Vec3d(invTvec);
	cv::Vec3d invRvec;
	cv::Rodrigues(rotMat, invRvec);
	std::vector<cv::Vec3d> returnVals;
	returnVals.push_back(invTvec2);
	returnVals.push_back(invRvec);
	return returnVals;
}

MarkerPosition* act::proc::MarkerPositionDetection::getNodeFromNodes(int id) {
    std::vector<MarkerPosition*> found;
    // copies found nodes to found by evaluating the lambda.
    std::copy_if (m_nodes.begin(), m_nodes.end(), std::back_inserter(found), [id](MarkerPosition* it){ return it->getId() == id; });
    if (!found.empty())
        return found[0];
    else return nullptr;
}

/*
	input: cv::Mat rotation-matrix (3x3)
	output: vector<double> roll, pitch, yaw in reference to the origin (according to the rotation-matrix)
*/
std::vector<double> act::proc::MarkerPositionDetection::eulerToRad(cv::Mat rotationMatrix) {
		//reference: 
		// http://eecs.qmul.ac.uk/~gslabaugh/publications/euler.pdf

	//define variables
	double roll, pitch, yaw, cos_roll;
	double pi = 3.14159265359;

	//translate pseudocode from reference into real code
	if (rotationMatrix.at<double>(2, 0) != 1 || rotationMatrix.at<double>(2, 0) != -1) {
		roll = -1 * asin(rotationMatrix.at<double>(2, 0));
		cos_roll = cos(roll);
		
		pitch = atan2((rotationMatrix.at<double>(2, 1) / cos_roll), (rotationMatrix.at<double>(2, 2) / cos_roll));
		yaw = atan2((rotationMatrix.at<double>(1,0)/cos_roll),(rotationMatrix.at<double>(0,0) / cos_roll));
	}
	else {
		yaw = 0;
		if (rotationMatrix.at<double>(2, 0) == -1) {
			roll = pi / 2;
			pitch = atan2(rotationMatrix.at<double>(0, 1), rotationMatrix.at<double>(0, 2));
		}
		else {
			roll = -1*pi / 2;
			pitch = atan2(-1*rotationMatrix.at<double>(0, 1), -1*rotationMatrix.at<double>(0, 2));
		}
	}

	//create vector which is later returned
	std::vector<double> returnVector;
	//add calculated values to vector
	returnVector.push_back(roll);
	returnVector.push_back(pitch);
	returnVector.push_back(yaw);
	return returnVector;
}

cv::Vec2i act::proc::MarkerPositionDetection::checkForLowerMarkerIndex(int markerID, int lowestIndex, int i) {
	auto index = std::find(m_markerIds.begin(), m_markerIds.end(), markerID);
	cv::Vec2i vector;
	vector[1] = std::numeric_limits<int>::max();
	//if found
	if (index != m_markerIds.end()) {
		int newIndex = index - m_markerIds.begin();
		//console() << "newIndex: " << newIndex << std::endl;
		if (lowestIndex > newIndex) {
			//set new lowest marker index
			lowestIndex = newIndex;
			//console() << " i at new lowest: " << i << std::endl;
			vector[1] = i;
		}
	}
	
	vector[0] = lowestIndex;
	
	return vector;	
}
cv::Vec3d multiplyMatrixByVector(cv::Mat* matrix, cv::Vec3d* vector) {
	cv::Vec3d returnVector;

	cv::Vec3d currentRow;

	//iterate over rows
	for (int i = 0; i < 3; i++) {
		currentRow = matrix->row(i);
		returnVector[i] = vector->dot(currentRow);
	}

	return returnVector;
}
/*
Calculates the position of the camera based on the position & rotation of the current origin marker
*/
std::vector<glm::vec3> act::proc::MarkerPositionDetection::calculateCameraOffsetForGivenMarker(MarkerPosition* marker) {
	// reference:
	// https://www.euclideanspace.com/maths/geometry/affine/aroundPoint/index.htm
	glm::vec3 position = marker->getPosition();
	glm::vec3 rotation = marker->getRotation();

	cv::Vec3d cvRotationVector;
	cvRotationVector[0] = rotation.x;
	cvRotationVector[1] = rotation.y;
	cvRotationVector[2] = rotation.z;

	cv::Vec3d resultingTransform = cv::Vec3d().zeros();
	resultingTransform[0] = position[0];
	resultingTransform[1] = position[1];
	resultingTransform[2] = position[2];

	
	cv::Mat rotationMatrix;
	

	std::vector<cv::Vec3d> invVecs = inversePerspective(cvRotationVector, resultingTransform);
	resultingTransform = invVecs[0];
	cvRotationVector = invVecs[1];
	
	cv::Rodrigues(cvRotationVector, rotationMatrix);

	

	//rotationMatrix = rotationMatrix.t();

	// 3. translate to origin
	// 2. rotate around origin
	// 1. translate back

	//translate back
	

	// matrix * -position (step 1+2)
	cv::Vec3d negativePosition;
	negativePosition[0] = -position[0];
	negativePosition[1] = -position[1];
	negativePosition[2] = -position[2];

	cv::Vec3d productSecondMultiply = multiplyMatrixByVector(&rotationMatrix, &negativePosition);
	resultingTransform = resultingTransform * productSecondMultiply;

	//return
	std::vector<glm::vec3> retVector;
	
	console() << "calculated pos offset: x: " << resultingTransform[0] << " y: " << resultingTransform[2] << " z: " << resultingTransform[1] << std::endl;

	retVector.push_back(glm::vec3(resultingTransform[0], resultingTransform[1], resultingTransform[2]));
	retVector.push_back(rotation);

	return retVector;
}

/**
 * detects markers and saves them to m_markerIds and m_nodes.
 * Needs an origin marker in the image.
 *
 * Copied code from MarkerManager, but with MarkerPositions instead of MarkerSceneNodes
 */
void act::proc::MarkerPositionDetection::findMarkers() {

	//save markers that were processed by a camera to not do double work
	std::vector<int> processedMarkers;
	for (auto&& node : cameraMgr->getNodes()) {
		CameraSceneNodeRef cameraNode = std::dynamic_pointer_cast<CameraSceneNode> (node);
		act::scn::CameraRef camera = cameraNode->getCamera();
		cv::UMat image = camera->getCurrentImage();
		std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
		std::vector<int> markerIds;

		if(!image.empty())
			detector.detectMarkers(image, markerCorners, markerIds, rejectedCandidates); // TODO depr , camera->m_intrinsic, camera->m_distCoeffs);
		
		currentlyDetectedMarkers = markerIds.size();

		
		auto json = ci::JsonTree::makeObject("markerData");
		json.addChild(ci::JsonTree("markerCount", currentlyDetectedMarkers))
			.addChild(ci::JsonTree("someData", "42"));
			
		auto jsonArray = ci::JsonTree::makeArray("myNamedListe");
		for (int i = 0; i < 5; i++) {
			auto nodeJson = ci::JsonTree("{}");
			nodeJson.addChild(ci::JsonTree("index", i));
			nodeJson.addChild(ci::JsonTree("zweterParameter", i*42));
			jsonArray.addChild(nodeJson);
		}
		json.addChild(jsonArray);

		auto jsonOtherArray = ci::JsonTree::makeArray("myOtherListe");
		for (int i = 10; i < 15; i++) {
			jsonOtherArray.addChild(ci::JsonTree("", i));
		}

		json.addChild(jsonOtherArray);

		m_markerPort->send(json);
		bool originFound = false;
		int i = 0;
		int lowestIndex = std::numeric_limits<int>::max();
		int help_i = std::numeric_limits<int>::max();
		

		for (; i < markerIds.size(); i++) {
			auto maybeFixed = getNodeFromNodes(markerIds[i]);
			if (maybeFixed) console() << "found fixed marker " << maybeFixed->getId() << std::endl;
			
			if (markerIds[i] == m_originMarkerId || (maybeFixed && maybeFixed->isFixed())) {
				originFound = true;
				//find index of marker in detected list
				cv::Vec2i returnVals = checkForLowerMarkerIndex(markerIds[i], lowestIndex, i);
				lowestIndex = returnVals[0];
				//new lowest value
				if (returnVals[1] < std::numeric_limits<int>::max()) {
					help_i = returnVals[1];
					//console() << "help i: " << help_i << std::endl;
				}
				//unfix all fixed origins -> only if they exist yet
				if (maybeFixed) { maybeFixed->setIsFixed(false); }
				//break;
			}
		}
		
		//if origin id is found, set cameraposition
		if (originFound) {
			status = "Origin found!";
			console() << std::endl;


			i = help_i;
			//fix marker with lowest id 
			// 
				//get id of origin marker with lowest index
			if (m_markerIds.size() > 0) {
				//get origin marker & set fixed
				//console() << "is fix pre " << getNodeFromNodes(m_markerIds[lowestIndex])->isFixed() << std::endl;
				getNodeFromNodes(m_markerIds[lowestIndex])->setIsFixed(true);
				//console() << "is fix after " << getNodeFromNodes(m_markerIds[lowestIndex])->isFixed() << std::endl;
			}
			
			

			std::vector<cv::Vec3d> rvecs, tvecs;
			cv::aruco::estimatePoseSingleMarkers(markerCorners, markerLength, camera->m_intrinsic, camera->m_distCoeffs, rvecs, tvecs);

            std::vector<cv::Vec3d> invVecs = inversePerspective(rvecs[i], tvecs[i]);
			cv::Vec3d tvecCam = invVecs[0];
			cv::Vec3d rvecCam = invVecs[1];

			//create rotation matrix for further usage
			cv::Mat rotMat;
			cv::Rodrigues(rvecCam, rotMat);

			double roll = atan2(-rotMat.at<double>(2, 1), rotMat.at<double>(2, 2));
			double pitch = asin(rotMat.at<double>(2, 0));
			double yaw = atan2(-rotMat.at<double>(1, 0), rotMat.at<double>(0, 0));


		
			// "fixed" non-origin marker are not (0,0,0). So we need to add the already known marker position to the relative camera coordinates
			std::vector<glm::vec3> calculatedOffset = calculateCameraOffsetForGivenMarker(getNodeFromNodes(m_markerIds[lowestIndex]));
			glm::vec3 camPosOffsetFromMarker = calculatedOffset[0];
			glm::vec3 camRotOffsetFromMarker = calculatedOffset[1];
			//do some trigonometry to get the correct offset even when a marker is rotated

			console() << "calculated camPos: x: " << tvecCam[0] << " y: " << tvecCam[2] << " z: " << -tvecCam[1] << std::endl;

			cameraNode->setPosition(glm::vec3((tvecCam[0] + camPosOffsetFromMarker.x), tvecCam[2] + camPosOffsetFromMarker.y, -(tvecCam[1] + camPosOffsetFromMarker.z)));

			
			cameraNode->setRotation(glm::vec3(-(roll+abs(camRotOffsetFromMarker[0])), -(yaw + camRotOffsetFromMarker[1]), (pitch+camRotOffsetFromMarker[2])));

			cameraNode->setRotMat(rotMat);
			cv::aruco::estimatePoseSingleMarkers(markerCorners, markerLength, camera->m_intrinsic, camera->m_distCoeffs, rvecs, tvecs);

			// as origin was found, find other markers
			for (int j = 0; j < markerIds.size(); j++) {
				//if marker not in already processed markers:
				if (std::find(processedMarkers.begin(), processedMarkers.end(), markerIds[j]) == processedMarkers.end()) {
					bool newMarker = true;
					MarkerPosition* marker = nullptr;
					for (auto markerId : m_markerIds) {
						MarkerPosition* markerPosObject = getNodeFromNodes(markerId);
						if (markerPosObject->getId() == markerIds[j]) {
							marker = markerPosObject;
							newMarker = false;
							break;
						}
					}

					if (!marker) {
						// reject markers with id > 100 (during testing, CV would detect "ghost markers" with IDs > 100
						// these ghost markers resulted in wrong results at the end
						// For next KP: find better solution to this problem
						if (markerIds[j] > 100) {
							processedMarkers.push_back(markerIds[j]);
							continue;
						}

						marker = new MarkerPosition(markerIds[j]);
					}

					if (markerIds[j] == m_originMarkerId) {
						marker->setPosition(glm::vec3(0.0, 0.0, 0.0));
						marker->setRotation(glm::vec3(0.0, 0.0, 0.0));
					}
					else {
						//OpenCv uses right-handed coordinate system, cinder left-handed
						cv::Vec3d tvec = tvecs[j];

						cv::Vec3d rvec = rvecs[j];

						auto camRot = node->getRotation();
						cv::Vec3d camRotVec;
						cv::Rodrigues(rotMat, camRotVec);
						camRotVec = cv::Vec3d(camRotVec[0], camRotVec[1], camRotVec[2]);

						auto camPos = node->getPosition();
						cv::Vec3d camTVec = cv::Vec3d(camPos[0], camPos[2], camPos[1]);

						cv::Vec3d newTvec;
						cv::Vec3d newRvec;

						cv::composeRT(rvec, tvec, camRotVec, camTVec, newRvec, newTvec);

						cv::Mat newRotMat;
						cv::Rodrigues(newRvec, newRotMat);

						ci::vec3 newPosition = ci::vec3(newTvec[0], newTvec[2], 2*camTVec[1] - newTvec[1]);
                        //console() << "Extra marker: roll: "  << newTvec[0] << ", pitch: " << newTvec[2] << ", yaw: " << newTvec[1] << std::endl;
						marker->setPosition(newPosition);

						roll = -atan2(-newRotMat.at<double>(2, 1), newRotMat.at<double>(2, 2));
						pitch = -asin(newRotMat.at<double>(2, 0));
						yaw = -atan2(-newRotMat.at<double>(1, 0), newRotMat.at<double>(0, 0));
						marker->setRotation(glm::vec3(roll, yaw, pitch));

					}
					processedMarkers.push_back(markerIds[j]);
					if (newMarker) {
						m_markerIds.push_back(marker->getId());
						m_nodes.push_back(marker);

						//m_markerPort->send(currentlyDetectedMarkers);
					}

					//debug
					console() << "marker id: " << marker->getId() << std::endl;
					vec3 pos = marker->getPosition();
					console() << "pos x: " << pos[0] << " y: " << pos[1] << " z: " << pos[2] << std::endl;
					vec3 rot = marker->getRotation();
					double factor = 180 / 3.14159265359;
					console() << "rot x: " << rot[0]*factor << " y: " << rot[1]*factor << " z: " << rot[2]*factor << std::endl;
					console() << "is fixed: " << marker->isFixed() << std::endl<<std::endl;

					//fix all markers
					marker->setIsFixed(true);

					
					tryHighlightFoundDevice(markerIds[j]);
				}
			}
			//empty line to make console more readable
			console() << std::endl << std::endl;
		}
		else status = "Origin not found.";
	}
	//update device positions based on marker-ids
	updateDevicePositions();
}

void act::proc::MarkerPositionDetection::tryHighlightFoundDevice(int markerId) {
	// try getting a MovingHeadSceneNodeRef. If it fails, it is no moving head and thus cannot glow :>
	MovingHeadSceneNodeRef ref;
	// dynamic_pointer_cast to go down/across class hierarchy
	ref = dynamic_pointer_cast<MovingHeadSceneNode>(m_dmxMgr->getDeviceByMarkerID(markerId));
	if (!ref) {
		return;
	}
	std::thread([&]() {
		ref->highlight();
		sleep(750);
		ref->release();
	}).join();

}

void act::proc::MarkerPositionDetection::updateDevicePositions() {
	//iterate over all given markers
	for (auto marker : m_nodes) {
		//get device with given marker-ID
		act::scn::SceneNodeBaseRef sceneNode = m_dmxMgr->getDeviceByMarkerID(marker->getId());

		//check if device exists
		if (sceneNode == nullptr) {
			continue;
		}

		//set markers of devices as unfixed
		marker->setIsFixed(false);

		//update position & rotation
		vec3 position = marker->getPosition();
		vec3 rotation = marker->getRotation();
		sceneNode->setIsFixed(false); // if node is fixed, position and rotation cannot be set
		sceneNode->setPosition(position);
		sceneNode->setRotation(rotation);
	}

	//save devices to recentRoomSetup.json
	m_dmxMgr->saveDevicesToJson();
}
