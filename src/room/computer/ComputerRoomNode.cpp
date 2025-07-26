
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
#include "computer/ComputerRoomNode.hpp"

act::room::ComputerRoomNode::ComputerRoomNode(std::string name, ci::vec3 position, ci::vec3 rotation, act::UID replyUID)
	: RoomNodeBase("computer", position, rotation, 0.2f, replyUID)
{

	setPosition(vec3(0.0f, 0.0f, 0.0f));
	setRotation(vec3(0.0f, 0.0f, 0.0f));
}

act::room::ComputerRoomNode::~ComputerRoomNode()
{

}

void act::room::ComputerRoomNode::setup()
{

}

void act::room::ComputerRoomNode::update()
{

}

void act::room::ComputerRoomNode::draw()
{
	gl::ScopedColor color;
	enableStatusColor();

	gl::pushMatrices();
	gl::translate(m_position);
	gl::rotate(m_rotation);
	gl::drawCube(ci::vec3(0.0f), ci::vec3(0.2f, 0.1f, 0.2f));

	gl::popMatrices();
}

void act::room::ComputerRoomNode::drawSpecificSettings()
{
	
}

ci::Json act::room::ComputerRoomNode::toParams()
{
	ci::Json json = ci::Json::object();


	return json;
}

void act::room::ComputerRoomNode::fromParams(ci::Json json)
{

}

