
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

#include "roompch.hpp"
#include "dmx/DMXRoomNodeBase.hpp"


act::room::DMXRoomNodeBase::DMXRoomNodeBase(DMXProRef dmxInterface, ci::Json description, int startAddress)
	: m_startAddress(startAddress)
{
	m_fixtureName		= description["name"];
	m_numberOfChannels	= description["channel"];

	auto mapping = description["mapping"];
	for (auto& [key, value] : mapping.items()) {
		m_channelMapping[key] = value;
		auto val = util::MinMaxValue<int>(0, 255);
		m_channelValues.insert(std::pair<std::string, util::MinMaxValue<int>>(key, util::MinMaxValue<int>(0, 255)));
	}

	setDMXInterface(dmxInterface);
}

act::room::DMXRoomNodeBase::~DMXRoomNodeBase()
{
	
}

void act::room::DMXRoomNodeBase::setDMXInterface(DMXProRef dmxInterface)
{
	m_dmxInterface = dmxInterface;
}

bool act::room::DMXRoomNodeBase::setValue(std::string channel, int value)
{
	if (!m_channelMapping.contains(channel))
		return false;
	try {
		
		int chn		= m_channelMapping.at(channel) - 1;
		auto val	= m_channelValues.at(channel);
		val.setValue(value);

		if(m_dmxInterface)
			m_dmxInterface->setValue(val.getValue(), chn + m_startAddress);

		return true;
	}
	catch (...) {
		return false;
	}
}
