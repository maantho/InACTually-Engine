
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
#include "projector/ProjectorManager.hpp"

#include <algorithm> 

act::room::ProjectorManager::ProjectorManager()
	: RoomNodeManagerBase("projectorManager")
{
}

act::room::ProjectorManager::~ProjectorManager()
{
}

void act::room::ProjectorManager::setup()
{

}

act::room::RoomNodeBaseRef act::room::ProjectorManager::drawMenu()
{
	
	if (ImGui::Button("add Device")) {
		
	}

	return nullptr;
}

void act::room::ProjectorManager::update()
{

}

ci::Json act::room::ProjectorManager::toJson()
{
	auto json = ci::Json::object();

	ci::Json nodes = ci::Json::array();
	for (auto&& node : m_nodes) {
		//auto device = std::dynamic_pointer_cast<act::room::CameraRoomNode>(node);
		auto nodeJson = node->toJson();

		nodes.push_back(nodeJson);
	}
	json["nodes"] = nodes;

	return json;	
}

void act::room::ProjectorManager::fromJson(ci::Json json)
{
	if (json.contains("nodes")) {
		auto nodesJson = json["nodes"];
		for (auto&& node : nodesJson) {

			std::string name = "";
			util::setValueFromJson(node, "name", name);
			auto projRoomNode = addDevice(name);

			projRoomNode->fromJson(node);

			refreshLists();
		}
	}
}

act::room::ProjectorRoomNodeRef act::room::ProjectorManager::getProjector(act::UID projectorUID) {
	auto nodeIter = std::find_if(m_nodes.begin(), m_nodes.end(),
		[projectorUID](RoomNodeBaseRef node) {
			return !node->getUID().compare(projectorUID);
		});
	if (nodeIter != std::end(m_nodes)) {
		return std::dynamic_pointer_cast<ProjectorRoomNode> (*nodeIter);
	}
	return nullptr;
}

act::room::RoomNodeBaseRef act::room::ProjectorManager::addDevice(std::string name)
{
	auto node = nullptr; //ProjectorRoomNode::create(window, name);
	//m_nodes.push_back(node);

	return node;
}