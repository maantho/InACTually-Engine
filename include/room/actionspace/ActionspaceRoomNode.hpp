
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

#pragma once
#include <memory>
#include <vector>

#include "RoomNodeBase.hpp"
#include "Bounding.hpp"

using namespace ci;
using namespace ci::app;

namespace act {
	namespace room {

		enum ASType {
			AST_UNKOWN = -1,
			AST_SPHERE = 0,
			AST_CYLINDER,
			AST_CUBOID,
			AST_CONE,
			AST_FREE
		};

		class ActionspaceRoomNode : public RoomNodeBase
		{
		public:
			ActionspaceRoomNode(std::string name, ci::vec3 position = ci::vec3(0.0f, 0.0f, 0.0f), ci::vec3 rotation = ci::vec3(0.0f, 0.0f, 0.0f), float radius = 0.0f, act::UID replyUID = "");
			virtual ~ActionspaceRoomNode();

			static std::shared_ptr<ActionspaceRoomNode> create(std::string name, ci::vec3 position = ci::vec3(0.0f, 1.0f, 0.0f), ci::vec3 rotation = ci::vec3(0.0f, 0.0f, 0.0f), float radius = 0.5f, act::UID replyUID = "") { return std::make_shared<ActionspaceRoomNode>(name, position, rotation, radius, replyUID); };

			virtual void setup()	override;
			virtual void update()	override;
			virtual void draw()		override;

			virtual void drawSpecificSettings() override;

			virtual ci::Json toParams() override;
			virtual void fromParams(ci::Json json) override;

		protected:
			ASType	m_type = AST_SPHERE;
			vec3	m_size = vec3(1.f, 1.f, 1.f);
			
			BoundingRef m_bounding;

		private:			
			ASType	fromTypeString(std::string typestring);

		}; using ActionspaceRoomNodeRef = std::shared_ptr<ActionspaceRoomNode>;

	}
}