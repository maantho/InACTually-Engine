
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2023

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#include "roompch.hpp"
#include "computer/ComputerManager.hpp"

#include "computer/ComputerRoomNode.hpp"


act::room::ComputerManager::ComputerManager()
	: RoomNodeManagerBase("computerManager")
{
	refreshLists();
}

act::room::ComputerManager::~ComputerManager()
{
}

void act::room::ComputerManager::setup()
{
}

act::room::RoomNodeBaseRef act::room::ComputerManager::drawMenu()
{
	act::room::RoomNodeBaseRef node;

	if (ImGui::Button("add Device")) {
		node = addDevice();
	}

	return node;
}

void act::room::ComputerManager::update()
{

	for (auto&& node : m_nodes) {
		node->update();
	}

}

ci::Json act::room::ComputerManager::toJson()
{
	auto json = ci::Json::object();

	ci::Json nodes = ci::Json::array();
	for (auto&& node : m_nodes) {
		auto nodeJson = node->toJson();

		nodes.push_back(nodeJson);
	}
	json["nodes"] = nodes;

	return json;
	
}

void act::room::ComputerManager::fromJson(ci::Json json)
{
	if (json.contains("nodes")) {
		auto nodesJson = json["nodes"];
		for (auto&& node : nodesJson) {

			//util::setValueFromJson(node, "name", name);
			auto computerNode = addDevice();
			computerNode->fromJson(node);
			refreshLists();
		}
	}
}


void act::room::ComputerManager::refreshLists()
{

}


act::room::RoomNodeBaseRef act::room::ComputerManager::addDevice()
{
	auto net = ComputerRoomNode::create("computer");
	m_nodes.push_back(net);
	refreshLists();
	return net;

	return nullptr;
}