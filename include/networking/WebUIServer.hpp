
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2021-2023

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#pragma once

#include "ModuleBase.hpp"
#include "ModuleRegistry.hpp"
#include "WebSocketServer.h"
#include "Connection.hpp"

using namespace ci;


namespace act {
	namespace net {

		class WebUIServer : public ConnectionProvider, public UniqueIDBase
		{
		public:
			WebUIServer(MsgRecieverRef reciever);
			~WebUIServer();

			static std::shared_ptr<WebUIServer> create(MsgRecieverRef reciever) { return std::make_shared<WebUIServer>(reciever); }

			void				update();
			void				draw();


			ci::Json		toJson();
			void				fromJson(ci::Json json);

			void				sendMsg(ci::Json msg);

			bool				isConnected() { return m_isConnected; }

		private:
			MsgRecieverRef		m_reciever;

			WebSocketServer		m_server;
			unsigned int		m_port;
			bool				m_isConnected;

			std::string			m_text;
			
			void				recieveJson(ci::Json json);


		}; using WebUIServerRef = std::shared_ptr<WebUIServer>;

	}
}