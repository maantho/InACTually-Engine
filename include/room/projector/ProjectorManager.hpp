
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

#include "cinder/audio/audio.h" 
#include "RoomNodeManagerBase.hpp"

#include "projector/ProjectorRoomNode.hpp"
#include <marker/MarkerManager.hpp>




namespace act {
	namespace room {

		class ProjectorManager : public RoomNodeManagerBase {
		public:
			ProjectorManager();
			ProjectorManager(MarkerManagerRef markerMgr);
			~ProjectorManager();

			static	std::shared_ptr<ProjectorManager> create(MarkerManagerRef markerMgr) { return std::make_shared<ProjectorManager>(markerMgr); };

			void	setup() override;
			// void	update() override;
			// void	draw() override;

			act::room::RoomNodeBaseRef			drawMenu() override;

			virtual ci::Json					toJson();
			virtual void						fromJson(ci::Json json);

			act::room::ProjectorRoomNodeRef		getProjector(act::UID projectorUID);

			act::room::RoomNodeBaseRef addDevice(std::string name);

		private:
			void update();		

			MarkerManagerRef m_markerMgr;
		};
		using ProjectorManagerRef = std::shared_ptr<ProjectorManager>;
	}
}