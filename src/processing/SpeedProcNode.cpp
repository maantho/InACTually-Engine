
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2024

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#include "procpch.hpp"
#include "SpeedProcNode.hpp"

#include <numeric>


act::proc::SpeedProcNode::SpeedProcNode() : ProcNodeBase("Speed") {
	m_drawSize = ivec2(300, 300);

	auto positionInput = InputPort<vec3>::create(PT_VEC3, "position", [&](vec3 pos) { this->onPosition(pos); });
	m_inputPorts.push_back(positionInput);
	
	m_speedPort = OutputPort<float>::create(PT_NUMBER, "speed");
	m_outputPorts.push_back(m_speedPort);
	

}

act::proc::SpeedProcNode::~SpeedProcNode() {
}

void act::proc::SpeedProcNode::update() {
}

void act::proc::SpeedProcNode::draw() {
	beginNodeDraw();
 
	bool prvntDrag = false;
	if (ImGui::SliderFloat("factor", &m_factor, 0.01f, 10.0f)) {
		prvntDrag = true;
	}

	preventDrag(prvntDrag);
	
	endNodeDraw();
}

ci::Json act::proc::SpeedProcNode::toParams() {
	ci::Json json = ci::Json::object();
	 
	return json;
}

void act::proc::SpeedProcNode::fromParams(ci::Json json) {
}

void act::proc::SpeedProcNode::onPosition(ci::vec3 position) {

	float distance = ci::distance(position, m_lastPosition);
	m_lastPosition = position;

	float speed = distance * m_factor;

	m_speedPort->send(speed);
}
