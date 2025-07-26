
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2022-2023

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#include "roompch.hpp"
#include "display/DisplayManager.hpp"


act::room::DisplayManager::DisplayManager()
	: RoomNodeManagerBase("displayManager")
{

	refreshLists();
}

act::room::DisplayManager::~DisplayManager()
{
}

void act::room::DisplayManager::setup()
{
}

void act::room::DisplayManager::addSource(act::proc::OutputPortRef<ci::gl::Texture2dRef> outputPort)
{
	if (!m_ports.contains(outputPort->getUID())) {
		auto inputPort = proc::InputPort<ci::gl::Texture2dRef>::create(proc::PT_IMAGE, outputPort->getUID(), [&](ci::gl::Texture2dRef texture, act::UID uid) {
			if(!m_sources.contains(uid))
				m_sources.insert(std::pair<act::UID, ci::gl::Texture2dRef>(uid, texture));
			else
				m_sources.at(uid) = texture;
		});
		m_ports.insert(std::pair<act::UID, act::proc::InputPortRef<ci::gl::Texture2dRef>>(outputPort->getUID(), inputPort));
		outputPort->connect(inputPort);
	}
}

void act::room::DisplayManager::removeSource(act::proc::OutputPortRef<ci::gl::Texture2dRef> outputPort)
{
	if (m_ports.contains(outputPort->getUID())) {
		auto inputPort = m_ports.at(outputPort->getUID());
		outputPort->disconnect(inputPort);

		m_ports.erase(outputPort->getUID());

		if (m_sources.contains(inputPort->getUID())) {
			m_sources.erase(inputPort->getUID());
		}
	}
}

act::room::RoomNodeBaseRef act::room::DisplayManager::drawMenu()
{
	if (ImGui::Button("refresh Devicelist")) {
		refreshLists();
	}

	return nullptr;
}

ci::Json act::room::DisplayManager::toJson()
{
	auto json = ci::Json::object();

	ci::Json nodes = ci::Json::array();
	
	json["nodes"] = nodes;

	return json;
	
}

void act::room::DisplayManager::fromJson(ci::Json json)
{
	if (json.contains("nodes")) {
		auto nodesJson = json["nodes"];
		//for (auto&& node : nodesJson.getChildren()) {
		//}
	}
}

void act::room::DisplayManager::refreshLists()
{
}
