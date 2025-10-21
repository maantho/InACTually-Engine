
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2025

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#include "procpch.hpp"
#include "ClockProcNode.hpp"
#include <ranges>

act::proc::ClockProcNode::ClockProcNode() : ProcNodeBase("Clock") {
	m_drawSize = ci::ivec2(200, 200);

	m_timeUntil = 10.f;
	m_value = 0.0f;

	m_startedAt = app::getElapsedSeconds();

	m_valuePort		 = createNumberOutput("out");
	m_bangPort		 = createBoolOutput("bang");
}

act::proc::ClockProcNode::~ClockProcNode() {
}

void act::proc::ClockProcNode::update() {
	if (m_hasStarted) {
		m_elapsed = (app::getElapsedSeconds() - m_startedAt);
		m_value = m_elapsed / m_timeUntil;
	}
	

	if(m_value <= 1.f) {
		m_valuePort->send(m_value);
	}
	else
	{
		if (!m_bang) {
			m_bang = true;
			m_bangPort->send(true);
		}
		m_valuePort->send(m_value);

	}
}

void act::proc::ClockProcNode::draw() {
	beginNodeDraw();

	if (!m_hasStarted) {
		if (ImGui::Button("start")) {
			m_hasStarted = true;
			m_value = 0.0f;
			m_startedAt = app::getElapsedSeconds();
		}
	}
	else {
		if (ImGui::Button("stop")) {
			m_hasStarted = false;
			m_bang = false;
		}
	}

	if (ImGui::SliderFloat("", &m_value, 0.0f, 1.0f)) {
	}

	if (ImGui::SliderFloat("seconds", &m_timeUntil, 0.0f, 3600.0f)) {
	}

	endNodeDraw();
}

ci::Json act::proc::ClockProcNode::toParams() {
	ci::Json json = ci::Json::object();
	json["value"] = m_value;
	json["timeUntil"] = m_timeUntil;
	return json;
}

void act::proc::ClockProcNode::fromParams(ci::Json json) {
	util::setValueFromJson(json, "value", m_value);
	util::setValueFromJson(json, "timeUntil", m_timeUntil);
}
