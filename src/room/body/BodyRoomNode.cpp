
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
#include "body/BodyRoomNode.hpp"


act::room::BodyRoomNode::BodyRoomNode(std::string name, ci::vec3 position, ci::vec3 rotation, float radius, act::UID replyUID)
	: RoomNodeBase("body", position, rotation, radius, replyUID)
{
	m_body = Body::create();
}

act::room::BodyRoomNode::~BodyRoomNode()
{

}

void act::room::BodyRoomNode::setup()
{
}

void act::room::BodyRoomNode::update()
{
	
	
}

void act::room::BodyRoomNode::draw()
{
	//enableStatusColor(); 

	gl::pushMatrices();
	//gl::translate(m_position);
	//gl::rotate(m_rotation);
	gl::drawCube(ci::vec3(0.0f), ci::vec3(0.1f, 0.075f, 0.075f));
	auto color = ColorA(0.9f, 0.9f, 0.9f, 0.85f);
	float sphereRadius = 0.075f;

	for (auto&& joint : m_body->joints)
	{
		vec3 pos = joint->position;
		switch (joint->confidenceLevel) {
		case act::room::BJC_NONE:
			gl::color(ColorA(color.r, color.g, color.b, 0.4f));
			gl::lineWidth(2);
			break;
		case act::room::BJC_LOW:
			gl::color(ColorA(color.r, color.g, color.b, 0.6f));
			gl::lineWidth(4);
			break;
		case act::room::BJC_HIGH:
			gl::color(color);
			gl::lineWidth(4);
			break;
		default:
			break;
		}

		if (joint->type == act::room::BJT_HEAD)
			gl::drawSphere(pos, sphereRadius * 4.0f / 3.0f, 8);
		else
			gl::drawSphere(pos, sphereRadius, 8);

		vec3 parent = m_body->joints[act::room::bodyJointParentLookUp[joint->type]]->position;

		gl::drawLine(pos, parent);

		// drawVector only if debugging
		/*
		gl::pushMatrices();
		gl::translate(pos);
		gl::rotate(joint->orientation);
		gl::drawCoordinateFrame(0.2f);
		gl::popMatrices();
		*/
	}

	util::drawCoords();

	gl::popMatrices();
}

void act::room::BodyRoomNode::drawSpecificSettings()
{
	
}

ci::Json act::room::BodyRoomNode::toParams()
{
	ci::Json params = ci::Json::object();
	params["body"] = m_body->toJson();
	return params;
}

void act::room::BodyRoomNode::fromParams(ci::Json params)
{
	if (params.contains("body"))
		m_body->fromJson(params["body"]);
}

void act::room::BodyRoomNode::setBody(BodyRef body)
{
	m_body = body;

	publishParam("body", body->toJson());
}
