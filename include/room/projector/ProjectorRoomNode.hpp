
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
#include <cinder/app/App.h>


using namespace ci;
using namespace ci::app;


namespace act {

	namespace room {

		class ProjectorRoomNode : public RoomNodeBase, public std::enable_shared_from_this<ProjectorRoomNode>
		{
		public:
			ProjectorRoomNode(std::string name, ci::vec3 position, ci::vec3 rotation, float radius, act::UID replyUID = "");

			virtual ~ProjectorRoomNode();

			static std::shared_ptr<ProjectorRoomNode> create(std::string name, ci::vec3 position = ci::vec3(0.0f, 1.0f, 0.0f), ci::vec3 rotation = ci::vec3(0.0f,0.0f,0.0f), float radius = 0.5f, act::UID replyUID = "") { return std::make_shared<ProjectorRoomNode>(name, position, rotation, radius, replyUID); };

			virtual void setup()	override;
			virtual void update()	override;
			virtual void draw()		override;

			virtual void drawSpecificSettings() override;

			virtual ci::Json toParams() override;
			virtual void fromParams(ci::Json json) override;
			
		private:
			ci::app::WindowRef		m_window;
			ci::CameraPersp			m_cameraPersp;

		}; using ProjectorRoomNodeRef = std::shared_ptr<ProjectorRoomNode>;
		
	}
}