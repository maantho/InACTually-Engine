
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

#pragma once

#include "RoomNodeBase.hpp"

using namespace ci;
using namespace ci::app;


namespace act {
	namespace room {

		class ComputerRoomNode : public RoomNodeBase
		{
		public:
			ComputerRoomNode(std::string name, ci::vec3 position, ci::vec3 rotation, act::UID replyUID = "");
			virtual ~ComputerRoomNode();

			static std::shared_ptr<ComputerRoomNode> create(std::string name, ci::vec3 position = ci::vec3(0.0f, 1.0f, 0.0f), ci::vec3 rotation = ci::vec3(0.0f,0.0f,0.0f), act::UID replyUID = "") { return std::make_shared<ComputerRoomNode>(name, position, rotation, replyUID); };


			virtual void setup()	override;
			virtual void update()	override;
			virtual void draw()		override;

			virtual void drawSpecificSettings() override;

			virtual ci::Json toParams() override;
			virtual void fromParams(ci::Json json) override;

		private:
			std::thread				m_thread;


		}; using ComputerRoomNodeRef = std::shared_ptr<ComputerRoomNode>;
		
	}
}