
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

#include "OSCServer.hpp"
#include "cinder/Log.h"

using namespace asio;
using namespace asio::ip;

act::net::OSCServer::OSCServer(uint16_t localPort, asio::ip::address address)
	: m_socket(new udp::socket(ci::app::App::get()->io_context(), udp::endpoint(udp::v4(), localPort+1)))
	// The endpoint that we want to "send" to is the v4 broadcast address.
	, m_sender(m_socket, udp::endpoint(address, localPort)), m_isConnected(false)
{

	m_socket->set_option(asio::socket_base::broadcast(true));
	m_isConnected = true;
}


act::net::OSCServer::~OSCServer()
{
	m_sender.close();
	m_socket->close();
}

void act::net::OSCServer::sendMsg(ci::osc::Message msg)
{
	m_sender.send(msg, std::bind(&OSCServer::onSendError,
		this, std::placeholders::_1));
}

void act::net::OSCServer::onSendError(asio::error_code error)
{
	if (error) {
		CI_LOG_E("Error sending: " << error.message() << " val: " << error.value());
		m_isConnected = false;
		try {
			m_sender.close();
		}
		catch (const ci::osc::Exception &ex) {
			CI_LOG_EXCEPTION("Cleaning up socket: val -" << ex.value(), ex);
		}
	}
}
