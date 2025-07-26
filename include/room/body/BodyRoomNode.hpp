
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

#pragma once

#include "RoomNodeBase.hpp"

#include "body/Body.hpp"


using namespace ci;
using namespace ci::app;


namespace act {
	namespace room {

		class BodyRoomNode : public RoomNodeBase, public std::enable_shared_from_this<BodyRoomNode>
		{
		public:
			BodyRoomNode(std::string name, ci::vec3 position, ci::vec3 rotation, float radius, act::UID replyUID);
			virtual ~BodyRoomNode();

			static std::shared_ptr<BodyRoomNode> create(std::string name, ci::vec3 position = ci::vec3(0.0f, 1.0f, 0.0f), ci::vec3 rotation = ci::vec3(0.0f,0.0f,0.0f), float radius = 0.5f, act::UID replyUID = "") { return std::make_shared<BodyRoomNode>(name, position, rotation, radius, replyUID); };
			

			virtual void setup()	override;
			virtual void update()	override;
			virtual void draw()		override;

			virtual void drawSpecificSettings() override;

			virtual ci::Json toParams() override;
			virtual void fromParams(ci::Json json) override;

			void setBody(BodyRef body);
			UID getBodyUID() {
				return m_body->getUID();
			}
			
		private:
			BodyRef m_body;

		}; using BodyRoomNodeRef = std::shared_ptr<BodyRoomNode>;
		
	}
}