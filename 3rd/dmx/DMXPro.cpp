/*
 *  (This version as been slightly modified by lars)
 * 
 *  DMXPro.cpp
 *
 *  Created by Andrea Cuius
 *  The MIT License (MIT)
 *  Copyright (c) 2014 Nocte Studio Ltd.
 *
 *  www.nocte.co.uk
 *
 */

#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"
#include <iostream>
#include "dmx/DMXPro.hpp"

using namespace ci;
using namespace app;
using namespace std;


DMXPro::DMXPro(const string& deviceName, DeviceMode mode) : mSerialDeviceName(deviceName)
{
	mDeviceMode = mode;
	mSerial = nullptr;
	mSenderThreadSleepFor = 1000 / DMXPRO_FRAME_RATE;

	init();

	setZeros();
}


DMXPro::~DMXPro()
{
	shutdown(true);
}


void DMXPro::shutdown(bool send_zeros)
{
	CI_LOG_I("DMXPro shutting down..");

	if (mSerial)
	{
		if (send_zeros)
			setZeros(); // send zeros to all channels

		sleep(mSenderThreadSleepFor * 2);

		mRunDataThread = false;

		if (mDataThread.joinable())
		{
			CI_LOG_I("thread is joinable");
			mDataThread.join();
		}
		else
			CI_LOG_W("cannot join thread!");

		mSerial->flush();
		mSerial = nullptr;
	}
	else
	{
		mDataThread.detach();
	}

	CI_LOG_I("DMXPro > shutdown!");
}


void DMXPro::init(bool initWithZeros)
{
	CI_LOG_I("DMXPro > Initializing device");

	initDMX();
	initSerial(initWithZeros);

	mDataThread = std::thread(&DMXPro::processDMXData, this);
}


void DMXPro::initSerial(bool initWithZeros)
{
	if (mSerial)
	{
		if (initWithZeros)
		{
			setZeros(); // send zeros to all channels
			CI_LOG_V("DMXPro > Init serial with zeros() before disconnect");
			sleep(100);
		}
		mSerial->flush();
		mSerial = nullptr;
		sleep(50);
	}

	try
	{
		Serial::Device dev = Serial::findDeviceByNameContains(mSerialDeviceName);

		if (dev.getName() == "")
		{
			CI_LOG_W("DMXPro > cannot open device, device not found! > " << mSerialDeviceName);
			return;
		}

		mSerial = Serial::create(dev, DMXPRO_BAUD_RATE);
		CI_LOG_I("DMXPro > Connected to usb DMX interface: " << dev.getName());
	}
	catch (...)
	{
		CI_LOG_E("DMXPro > There was an error initializing the usb DMX device");
		mSerial = nullptr;
	}
}


void DMXPro::initDMX()
{

	for (int i = 0; i < DMXPRO_PACKET_SIZE; i++) // initialize all channels with zeros, data starts from [5]
		mDMXPacketOut[i] = 0;

	mDMXPacketOut[0] = 0x7E; //DMXPRO_START_MSG;									// DMX start delimiter 0x7E
	mDMXPacketOut[1] = 0x06; // set message type
	mDMXPacketOut[2] = (int)DMXPRO_DATA_SIZE & 0xFF;						// Data Length LSB - 0x1
	mDMXPacketOut[3] = (static_cast<int>(DMXPRO_DATA_SIZE) >> 8) & 0xFF;	// Data Length MSBc - 0x2
	mDMXPacketOut[4] = 0x0; // DMX start code
	// init DMX data
	mDMXPacketOut[517] = 0xE7; // DMX start delimiter 0xE7
	
	// init incoming DMX data
	for (size_t k = 0; k < 512; k++)
		mDMXDataIn[k] = 0;
}


void DMXPro::processDMXData()
{
	CI_LOG_V("DMXPro::processDMXData() start thread");

	mRunDataThread = true;

	if (mDeviceMode == SENDER)
	{
		while (mSerial && mRunDataThread)
		{
			std::unique_lock<std::mutex> dataLock(mDMXDataMutex); // get DMX packet UNIQUE lock
			mSerial->writeBytes(mDMXPacketOut, DMXPRO_PACKET_SIZE); // send data
			dataLock.unlock(); // unlock data
			std::this_thread::sleep_for(std::chrono::milliseconds(mSenderThreadSleepFor));
		}
	}


	else if (mDeviceMode == RECEIVER)
	{
		unsigned char value = '*'; // set to something different than packet label or start msg
		uint32_t packetDataSize;

		while (mSerial && mRunDataThread)
		{
			// wait for start message and label

			/*
			while ( value != DMXPRO_START_MSG && mSerial->getNumBytesAvailable() > 0 )
			{
				value = mSerial->readByte();
			}
			    
			if (  mSerial->getNumBytesAvailable() == 0 || mSerial->readByte() != DMXPRO_RECEIVE_PACKET_LABEL )
			{
			    value = '*';
			    continue;
			}
			*/

			while (mRunDataThread && value != DMXPRO_RECEIVE_PACKET_LABEL)
			{
				while (mRunDataThread && value != DMXPRO_START_MSG)
				{
					if (mSerial->getNumBytesAvailable() > 0)
						value = mSerial->readByte();
				}

				if (mSerial->getNumBytesAvailable() > 0)
					value = mSerial->readByte();
			}

			// read header
			if (mSerial->getNumBytesAvailable() < 2)
				continue;

			packetDataSize = mSerial->readByte(); // LSB
			packetDataSize += static_cast<uint32_t>(mSerial->readByte()) << 8; // MSB

			// Check Length is not greater than allowed
			if (packetDataSize <= 514) // dmx data + 2 start zeros
			{
				// Read the actual Response Data
				mSerial->readAvailableBytes(mDMXPacketIn, packetDataSize);

				// finally check the end code
				if (mSerial->getNumBytesAvailable() > 0 && mSerial->readByte() == DMXPRO_END_MSG)
				{
					// valid packet, parse DMX data
					// the first 2 bytes are 0(by specs there should only be 1 zero, not sure where the other one comes from!) 
					std::unique_lock<std::mutex> dataLock(mDMXDataMutex);
					for (size_t k = 2; k < packetDataSize; k++)
					{
						mDMXDataIn[k - 2] = mDMXPacketIn[k];
					}
					dataLock.unlock();
				}

				else // invalid packet, reset
					value = '*';
			}

			else // invalid packet, reset
				value = '*';

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	mRunDataThread = false;

	CI_LOG_V("DMXPro > sendDMXData() thread exited!");
}


void DMXPro::setValue(int value, int channel)
{
	if (channel <= 0 || channel > 512)
	{
		CI_LOG_W("DMXPro > invalid DMX channel: " << channel);
		return;
	}
	// DMX channels start form byte [5] and end at byte [DMXPRO_PACKET_SIZE-2], last byte is EOT(0xE7)        
	value = math<int>::clamp(value, 0, 255);
	std::unique_lock<std::mutex> dataLock(mDMXDataMutex); // get DMX packet UNIQUE lock
	mDMXPacketOut[4 + channel] = value; // update value
	dataLock.unlock(); // unlock mutex
}


size_t DMXPro::getValue(int channel)
{
	if (channel <= 0 || channel > 512)
	{
		CI_LOG_W("DMXPro > invalid DMX channel: " << channel);
		return 0;
	}

	size_t val;

	std::unique_lock<std::mutex> dataLock(mDMXDataMutex);
	val = mDMXDataIn[channel];
	dataLock.unlock();

	return val;
}


void DMXPro::setZeros()
{
	for (int i = 5; i < DMXPRO_PACKET_SIZE - 2; i++)
		// DMX channels start form byte [5] and end at byte [DMXPRO_PACKET_SIZE-2], last byte is EOT(0xE7)
		mDMXPacketOut[i] = 0;
}
