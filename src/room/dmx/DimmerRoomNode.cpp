
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2021

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#include "roompch.hpp"
#include "dmx/DimmerRoomNode.hpp"

#include "RGBAWHelper.h"

act::room::DimmerRoomNode::DimmerRoomNode(DMXProRef dmxInterface, ci::Json description, std::string name, int startAddress, ci::vec3 position, ci::vec3 rotation, float radius, act::UID replyUID)
	: DMXRoomNodeBase(dmxInterface, description, startAddress), RoomNodeBase("dimmer", position, rotation, radius, replyUID)
{
	 
	setPosition(vec3(1.0f, 1.0f, 0.0f));
	setRotation(vec3(0.0f, 0.0f, 0.0f));
	lookAt(vec3(5.0f, 2.0f, 5.0f));
}

act::room::DimmerRoomNode::~DimmerRoomNode()
{
	
}

void act::room::DimmerRoomNode::setup()
{
	
}

void act::room::DimmerRoomNode::update()
{
	
}

void act::room::DimmerRoomNode::draw()
{
	gl::ScopedColor color;
	
	gl::pushMatrices();
		gl::translate(m_position);

		gl::color(Color::white());

		 

		gl::color(Color(1.0f, 1.0f, 0.0f));

		//vec3 v = m_cameraPersp.getViewDirection();
		//gl::drawVector(ci::vec3(0.0f), normalize(vec3(v.x, 0.0f, v.z)));
		//gl::drawVector(ci::vec3(0.0f), normalize(vec3(0.0f, v.y, v.z)));

		enableStatusColor();

		gl::drawCube(ci::vec3(0.0f,0.0f,0.0f), ci::vec3(1.0f, 1.0f, 1.0f));
		 
	gl::popMatrices();

	if (m_isLookingAt) {
		glEnable(GL_BLEND); 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//TODO: change to project color
		
		gl::color(util::Design::primaryColor());
		gl::drawSphere(m_lookAt, 0.25f);
	}
	 
}

void act::room::DimmerRoomNode::cleanUp()
{
}

void act::room::DimmerRoomNode::drawSpecificSettings()
{
  

}

ci::Json act::room::DimmerRoomNode::toParams()
{
	ci::Json json = ci::Json::object();
 
	return json;
}

void act::room::DimmerRoomNode::fromParams(ci::Json json)
{
	m_isFixed = true;
 
}
 
void act::room::DimmerRoomNode::setDimmer(float dim)
{
	m_dimmer.setValue(dim);
	setValue("dimmer1", m_dimmer.getValue());
}
 
