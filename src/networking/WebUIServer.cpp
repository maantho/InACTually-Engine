
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

#include "WebUIServer.hpp"
#include "cinder/Log.h"

#include "Design.hpp"
#include "ModuleRegistry.hpp"
#include "Port.hpp"
#include "JsonMsgProcNode.hpp"
#include "ProcNodeBase.hpp"
#include <MatToBase64.hpp>
using namespace act::proc;
//#include "MatToBase64.hpp"


act::net::WebUIServer::WebUIServer(MsgRecieverRef reciever)
 : m_reciever(reciever)
{
	m_port = 9002;
	m_isConnected = false;
	m_text = "Initializing";

	m_server.connectCloseEventHandler([&]() {
		m_isConnected = false;
		m_reciever->onDisconnect(getUID());
		m_text = "Connection closed";
		CI_LOG_I(m_text);
	});


	m_server.connectFailEventHandler([&](std::string err) {

		m_text = "Error";
		if (!err.empty()) {
			m_text += ": " + err;
		}
		CI_LOG_I(m_text);
	});

	m_server.connectInterruptEventHandler([&]() {
		m_isConnected = false;
		m_reciever->onDisconnect(getUID());
		m_text = "Connection Interrupted";
		CI_LOG_I(m_text);
	});

	m_server.connectMessageEventHandler([&](std::string msg) {

		if (!msg.empty()) {

			try {
				this->recieveJson(ci::Json::parse(msg));
				CI_LOG_I("JSON Message Received");
			}
			catch (const std::exception& exc)
			{
				m_text = "[cannot interprete msg] " + msg + " (string) - " + exc.what();
				CI_LOG_I(m_text);
			}
		}
	});

	m_server.connectOpenEventHandler([&]() {
		m_isConnected = true;
		m_reciever->onConnect(getUID());
		m_text = "Connected";
		CI_LOG_I(m_text);
	});


	m_server.connectSocketInitEventHandler([&]() {

		// This routine reads the address of the incoming connection
		// and prints it to the log
		/*asio::ip::tcp::socket* socket = m_server.getSocket();
		if (socket != nullptr) {
			asio::ip::address address = socket->remote_endpoint().address();
			string host = "";
			if (address.is_v4()) {
				host += address.to_v4().to_string();
			}
			else if (address.is_v6()) {
				host += address.to_v6().to_string();
			}
			else {
				host += address.to_string();
			}
			host += ":" + toString(socket->remote_endpoint().port());
			CI_LOG_I(host);
		}*/
		});

	m_server.listen(m_port);
	m_text = "Listening";
}

act::net::WebUIServer::~WebUIServer() {
}

void act::net::WebUIServer::sendMsg(ci::Json msg) {
	if (!m_isConnected | !msg.is_null())
		return;

	auto str = msg.dump();
	m_server.write(str);
}

void act::net::WebUIServer::recieveJson(ci::Json json) {
	m_reciever->onMsg(json, getUID());
}


void act::net::WebUIServer::update() {
	m_server.poll();
}

void act::net::WebUIServer::draw() {

	std::stringstream strstr;
	strstr << "WebUI: " << m_text;
	ImGui::TextUnformatted(strstr.str().c_str());

}

ci::Json act::net::WebUIServer::toJson() {
	return ci::Json::object();
}

void act::net::WebUIServer::fromJson(ci::Json json) {
}
