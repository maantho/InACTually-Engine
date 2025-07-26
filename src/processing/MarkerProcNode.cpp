
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
#include "MarkerProcNode.hpp"

act::proc::MarkerProcNode::MarkerProcNode() : ProcNodeBase("Marker") {
	m_selectedMarker= 0;
	m_drawSize = ci::ivec2(200, 100);

	m_markerPositionInPort = InputPort<vec3>::create(PT_VEC3, "markerPosIn", [&](ci::vec3 position) {
		m_markerPositionOutPort->send(position);
	});

	m_markerPositionOutPort = OutputPort<vec3>::create(PT_VEC3, "markerPosOut");
	m_outputPorts.push_back(m_markerPositionOutPort);

}

act::proc::MarkerProcNode::~MarkerProcNode() {
	auto&& port = m_markerMgr->getMarkerPort(m_selectedMarker);
	if (port)
		port->disconnect(m_markerPositionInPort);
}

void act::proc::MarkerProcNode::setup(act::room::RoomManagers roomMgrs) {
	m_markerMgr = roomMgrs.markerMgr;
	attachMarker();
}

void act::proc::MarkerProcNode::update() {

}

void act::proc::MarkerProcNode::draw() {
	beginNodeDraw();

	if (m_markerMgr->getMarkerIDs().empty())
		ImGui::Text("No Marker has been set up.");
	else {
		if (ImGui::Combo("Marker", &m_selectedMarker, m_markerMgr->getMarkerIDsAsStrings())) {
			attachMarker();
		}
	}

	endNodeDraw();
}



ci::Json act::proc::MarkerProcNode::toParams() {
	ci::Json json = ci::Json::object();
	json["selectedDevice"] = m_selectedMarker;
	return json;
}

void act::proc::MarkerProcNode::fromParams(ci::Json json) {
	if (util::setValueFromJson(json, "selectedDevice", m_selectedMarker)) {
		auto&& port = m_markerMgr->getMarkerPort(m_selectedMarker);
		if (port)
			port->connect(m_markerPositionInPort);

		m_selectedMarker = 0;
		for (auto&& id : m_markerMgr->getMarkerIDs()) {
			if (id == m_selectedMarker)
				break;
			m_selectedMarker++;
		}
	}
}

void act::proc::MarkerProcNode::attachMarker() {
	auto&& port = m_markerMgr->getMarkerPort(m_selectedMarker);
	if (port)
		port->disconnect(m_markerPositionInPort);

	if (m_selectedMarker >= 0 && m_selectedMarker < m_markerMgr->getMarkerIDs().size()) {
		port = m_markerMgr->getMarkerPort(m_selectedMarker);
		if (port)
			port->connect(m_markerPositionInPort);
	}
}
