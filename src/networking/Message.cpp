
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

#include "Message.hpp"
#include "cinder/Log.h"

#include "Design.hpp"
#include "JsonMsgProcNode.hpp"

#include <MatToBase64.hpp>
#include <chrono>


act::net::Message::Message(act::UID uid, MsgType type, MsgMethod method)
	: m_type(type), m_method(method)
{
	if (!uid.empty())
		setUID(uid);
}

act::net::Message::Message(ci::Json json, ConnectionProviderRef sender)
	: m_sender(sender)
{
	fromJson(json);
}

bool act::net::Message::sendMissingError(std::string where, std::string missingItem)
{
	//m_text = missingItem + " missing in json";
	CI_LOG_W("Could not get " + missingItem + " from JSON Message.");

	return sendError(where, "Could not get " + missingItem + " from JSON Message.");
}

bool act::net::Message::sendError(std::string where, std::string what)
{
	return sendMsg(createErrorMsgJson(where, what));
}

ci::Json act::net::Message::createErrorMsgJson(std::string where, std::string what)
{
	ci::Json json = ci::Json::object();

	json["type"] = fromMsgType(getType());
	json["method"] = fromMsgMethod(MsgMethod::MM_ERROR);
	json["where"] = where;
	json["what"] = what;

	return json;
}

ci::Json act::net::Message::toJson()
{
	ci::Json json		= ci::Json::object();
	json["uid"]			= getUID();
	json["type"]		= fromMsgType(getType());
	json["method"]		= fromMsgMethod(getMethod());
	long long msec		= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	json["timestamp"]	= msec;
	json["data"]		= getData();
	return json;
}

void act::net::Message::fromJson(ci::Json json)
{
	if (!json.contains("type")) {
		sendMissingError("recieveJson", "type");
		return;
	}
	if (!json.contains("method")) {
		sendMissingError("recieveJson", "method");
		return;
	}
	if (!json.contains("timestamp")) {
		sendMissingError("recieveJson", "timestamp");
		return;
	}
	if (!json.contains("data")) {
		sendMissingError("recieveJson", "data");
		return;
	}

	setType(toMsgType(json["type"]));
	setMethod(toMsgMethod(json["method"]));
	m_timestamp = json["timestamp"];
	setData(json["data"]);

	if (m_method == MM_UNKNOWN) {
		sendError("recieveJson", "method is unknown");
		return;
	}
	if (m_method != MM_ERROR && m_type == MT_UNKNOWN) {
		sendError("recieveJson", "type is unknown");
		return;
	}
	

	if (m_method != MM_CONNECT
		&& m_method != MM_DISCONNECT
		&& m_method != MM_REQUEST) {

		if (!getData().contains("uid")) {
			sendMissingError("recieveJson", "data.uid");
			return;
		}
	}

	if (json.contains("uid"))
		setUID(json["uid"]);
}

act::net::MsgMethod act::net::Message::toMsgMethod(std::string method)
{
	if (method == "update")
		return MM_UPDATE;

	if (method == "rpc")
		return MM_REMOTEPROCEDURECALL;

	if (method == "create")
		return MM_CREATE;

	if (method == "delete")
		return MM_DELETE;

	if (method == "connect")
		return MM_CONNECT;

	if (method == "disconnect")
		return MM_DISCONNECT;

	if (method == "subscribe")
		return MM_SUBSCRIBE;

	if (method == "unsubscribe")
		return MM_UNSUBSCRIBE;

	if (method == "request")
		return MM_REQUEST;

	if (method == "upload")
		return MM_UPLOAD;

	if (method == "error")
		return MM_ERROR;

	if (method == "warning")
		return MM_WARNING;

	return MM_UNKNOWN;
}

std::string act::net::Message::fromMsgMethod(MsgMethod method)
{
	if (method == MM_UPDATE)
		return "update";

	if (method == MM_REMOTEPROCEDURECALL)
		return "rpc";

	if (method == MM_CREATE)
		return "create";

	if (method == MM_DELETE)
		return "delete";

	if (method == MM_CONNECT)
		return "connect";

	if (method == MM_DISCONNECT)
		return "disconnect";

	if (method == MM_SUBSCRIBE)
		return "subscribe";

	if (method == MM_UNSUBSCRIBE)
		return "unsubscribe";

	if (method == MM_REQUEST)
		return "request";

	if (method == MM_UPLOAD)
		return "upload";

	if (method == MM_ERROR)
		return "error";

	if (method == MM_WARNING)
		return "warning";

	return "unknown";
}

act::net::MsgType act::net::Message::toMsgType(std::string type)
{
	if (type == "procNode")
		return MT_PROCNODE;

	if (type == "roomNode")
		return MT_ROOMNODE;

	if (type == "app")
		return MT_APP;

	if (type == "interaction")
		return MT_INTERACTION;

	if (type == "asset")
		return MT_ASSET;

	if (type == "description")
		return MT_DESCRIPTION;

	return MT_UNKNOWN;
}

std::string act::net::Message::fromMsgType(MsgType type)
{
	if (type == MT_PROCNODE)
		return "procNode";

	if (type == MT_ROOMNODE)
		return "roomNode";

	if (type == MT_APP)
		return "app";

	if (type == MT_INTERACTION)
		return "interaction";

	if (type == MT_ASSET)
		return "asset";

	if (type == MT_DESCRIPTION)
		return "description";

	return "unknown";
}

bool act::net::Message::sendMsg(ci::Json json)
{
	if (!m_sender)
		return false;

	m_sender->sendMsg(json);
	return true;
}

bool act::net::Message::checkValidility()
{
	return false;
}
