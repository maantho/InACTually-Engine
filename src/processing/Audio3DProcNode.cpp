
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
#include "Audio3DProcNode.hpp"

#include "cinder/gl/Fbo.h"
#include "cinder/audio/Param.h"


act::proc::Audio3DProcNode::Audio3DProcNode() : ProcNodeBase("Audio3D", NT_OUTPUT) {

	m_3DPosition = vec3(0.0f, 1.0f, 0.0f);

	m_drawSize = ivec2(400, 150);
	
	auto in = InputPort<ci::audio::NodeRef>::create(PT_AUDIONODE, "audio in", [&](ci::audio::NodeRef node) {
		node >> m_soundRoomNode->getIn();
	});
	in->setConnectionCB([&]() {

	});
	in->setDisconnectionCB([&]() {

	});

	auto gain = InputPort<bool>::create(PT_NUMBER, "gain", [&](bool event) {
		m_volume = audio::linearToDecibel(event);
		m_soundRoomNode->setVolume(m_volume); 
	});
	auto position = InputPort<vec3>::create(PT_VEC3, "position", [&](vec3 event) { set3DPosition(event); });

	m_inputPorts.push_back(in);
	m_inputPorts.push_back(gain);
	m_inputPorts.push_back(position);

	m_outPort = OutputPort<ci::audio::NodeRef>::create(PT_AUDIONODE, "audio out");
	m_outPort->setConnectionCB([&]() {
		m_outPort->send(m_soundRoomNode->getOut());
	});
	m_outPort->setDisconnectionCB([&]() {

	});

	m_outputPorts.push_back(m_outPort);

	auto ctx = audio::Context::master();
	//ctx->disable();
}

act::proc::Audio3DProcNode::~Audio3DProcNode() {
	
}

void act::proc::Audio3DProcNode::setup(act::room::RoomManagers roomMgrs) {
	m_audioMgr = roomMgrs.audioMgr;
	m_soundRoomNode = m_audioMgr->createSound(m_3DPosition, 0.2f, "audio " + getUID());
}

void act::proc::Audio3DProcNode::init() {
	
}

void act::proc::Audio3DProcNode::update() {
	
}

void act::proc::Audio3DProcNode::draw() {
	beginNodeDraw();

	bool prvntDrag = false;

	ImGui::SetNextItemWidth(m_drawSize.x);
	if (ImGui::SliderFloat("volume", &m_toVolume, 0.0f, 120.f)) {
		//m_gain->setValue(audio::decibelToLinear(m_volume));
		m_soundRoomNode->setVolume(m_toVolume);
		prvntDrag = true;
	}

	preventDrag(prvntDrag);

	ImGui::SameLine();
	if (ImGui::Checkbox("show visual", &m_showVisual)) {
			
	}

	ImGui::BeginDisabled();
	ImGui::SliderFloat("", &m_currentLoudness, 0.0f, 1.0f);
	ImGui::EndDisabled();

	if (m_showVisual) {

	}

	endNodeDraw();
}

ci::Json act::proc::Audio3DProcNode::toParams() {
	ci::Json json = ci::Json::object();
	json["showVisual"]	= m_showVisual;
	json["tovolume"]		= m_toVolume;
	json["volume"]			= m_volume.value();

	vec3 pos;
	if (m_soundRoomNode) {
		pos = m_soundRoomNode->getPosition();
	}
	else {
		pos = vec3(0.0, 0.0, 0.0);
	}
	
	json["position"] = util::valueToJson(pos);
	return json;
}

void act::proc::Audio3DProcNode::fromParams(ci::Json json) {

	vec3 pos = m_3DPosition;
	if (util::setValueFromJson(json, "position", pos)) {
		set3DPosition(pos);
	};

	util::setValueFromJson(json, "volume", m_volume.value());
	if (util::setValueFromJson(json, "tovolume", m_toVolume)) {
		if(m_soundRoomNode)
			m_soundRoomNode->setVolume(m_toVolume);
	};
	util::setValueFromJson(json, "showVisual", m_showVisual);
}

void act::proc::Audio3DProcNode::set3DPosition(vec3 position)
{
	m_3DPosition = position;
	if(m_soundRoomNode)
		m_soundRoomNode->setPosition(position);
}
