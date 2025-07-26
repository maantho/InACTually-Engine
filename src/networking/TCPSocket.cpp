
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

#include "TCPSocket.hpp"
#include "cinder/Log.h"

#include "Design.hpp"
#include "ModuleRegistry.hpp"
#include "Port.hpp"
#include "JsonMsgProcNode.hpp"
#include "ProcNodeBase.hpp"

#include <MatToBase64.hpp>
using namespace act::proc;
//#include "MatToBase64.hpp"

#include <future>
#include <thread>

act::net::TCPSocket::TCPSocket(MsgRecieverRef reciever)
 : m_reciever(reciever)
{
	m_port = 9000;
	m_isServer = true;
	m_text = "Initializing";

	m_ctx = std::make_shared<zmq::context_t>();
	
	if (m_isServer) {
		if (!establishServer()) {
			m_isServer = false;
			connectAsClient();
		}
	}
	else {
		connectAsClient();
	}

	m_recvThread = std::thread([&](std::shared_ptr<zmq::context_t> ctx) {
		while(1)
			listen();

	}, m_ctx);
	

	m_text = "Listening";
}

act::net::TCPSocket::~TCPSocket() {
	if(m_recvThread.joinable())
		m_recvThread.join();
}

void act::net::TCPSocket::sendMsg(ci::Json msg) {
	if (!msg.is_null())
		return;

	std::string str = msg.dump();
	zmq::message_t zmsg(str);
	//memcpy(zmsg.data(), str.c_str(), str.size());
	
	m_socket->send(zmsg, zmq::send_flags::dontwait);
	m_text = "sent msg";
}

std::string act::net::TCPSocket::getHostAddress()
{
	return "? :/";
}

std::string act::net::TCPSocket::getCurrentStatus()
{
	return m_text;
}

void act::net::TCPSocket::recieveJson(ci::Json json) {
	m_reciever->onMsg(json, getUID());
}


bool act::net::TCPSocket::establishServer()
{
	m_socket = std::make_shared<zmq::socket_t>(*m_ctx, ZMQ_DEALER);
	try {
		m_socket->bind("tcp://*:" + std::to_string(m_port));
		// m_socket->connect("tcp://127.0.0.1:" + std::to_string(m_port));
		return true;
	}
	catch (zmq::error_t err) {
		CI_LOG_E("TCPSocket binding failed: [" << err.num() << "] " << err.what());
		if (err.num() == 100) { // "Address in use"
			return false;
		}

		return false;
	}
	
}

void act::net::TCPSocket::connectAsClient()
{
	m_socket = std::make_shared<zmq::socket_t>(*m_ctx, ZMQ_DEALER);
	try {
		m_socket->connect("tcp://127.0.0.1:" + std::to_string(m_port));
	}
	catch (zmq::error_t err) {
		CI_LOG_E("TCPSocket connect failed: [" << err.num() << "] " << err.what());
	}

	zmq::message_t msg = zmq::message_t(std::string("here I am"));
	m_socket->send(msg, zmq::send_flags::dontwait);
}

void act::net::TCPSocket::listen() {
	//std::vector<zmq::message_t> recv_msgs;
	//zmq::recv_result_t result = zmq::recv_multipart(*m_socket, std::back_inserter(recv_msgs));
	zmq::message_t recv_msg;
	try {
		zmq::recv_result_t result = m_socket->recv(recv_msg);
		//assert(result && "recv failed");
		//assert(*result == 2);
	}
	catch (zmq::error_t err) {
		m_text = "recv error";
		return;
	}

	std::string msgStr = recv_msg.to_string();

	ci::Json json;

	bool isJson = true;
	try {
		json = ci::Json(msgStr);
	}
	catch (ci::Json::exception exc) {
		isJson = false;
	}

	if (isJson) {
		m_text = "got msg";
		recieveJson(json);
	}
	else {
		m_text = "got msg with invalid json";
		CI_LOG_W("TCPSocket msg is not a json : " << recv_msg.to_string());
	}

	std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(5));
}

void act::net::TCPSocket::draw() {

	std::stringstream strstr;
	strstr << "TCP: " << m_text;
	ImGui::TextUnformatted(strstr.str().c_str());

}

ci::Json act::net::TCPSocket::toJson() {
	return ci::Json::object();
}

void act::net::TCPSocket::fromJson(ci::Json json) {
}
