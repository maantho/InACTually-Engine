
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

#include "procpch.hpp"
#include "NetworkProcNode.hpp"

act::proc::NetworkProcNode::NetworkProcNode() : ProcNodeBase("Network") {

	auto image = createJsonInput("json", [&](ci::Json json) { this->onJson(json); });

	//m_imagePort = createImageOutput("pass-through image");

	m_port = 9002;
	
	m_text = "Initializing";

	m_server.connectCloseEventHandler([&]() {

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

		m_text = "Connection Interrupted";
		CI_LOG_I(m_text);
	});

	m_server.connectMessageEventHandler([&](std::string msg) {

		if (!msg.empty()) {

			try {
				this->recieveJson(ci::Json(msg));
				CI_LOG_I("JSON Message Received");
			}
			catch (const std::exception&)
			{
				m_text = msg + " (string)";
				CI_LOG_I("Could not read JSON Message");
			}
		}
	});

	m_server.connectOpenEventHandler([&]() {

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

act::proc::NetworkProcNode::~NetworkProcNode() {
}

void act::proc::NetworkProcNode::setup(act::room::RoomManagers roomMgrs) {
	
}

void act::proc::NetworkProcNode::sendJson(ci::Json json) {
	m_server.write(json.dump());
}

void act::proc::NetworkProcNode::recieveJson(ci::Json json) {

}

void act::proc::NetworkProcNode::update() {
	m_server.poll();
}

void act::proc::NetworkProcNode::draw() {
	beginNodeDraw();

	std::stringstream strstr;
	strstr << m_text << " on " << m_port;
	ImGui::TextUnformatted(strstr.str().c_str());
	
	endNodeDraw();
}

void act::proc::NetworkProcNode::onJson(ci::Json json) {
	sendJson(json);
}

ci::Json act::proc::NetworkProcNode::toParams() {
	return ci::Json::object();
}

void act::proc::NetworkProcNode::fromParams(ci::Json json) {
}
