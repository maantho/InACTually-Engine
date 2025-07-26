/*
	InACTually
	2021

	participants:
	Lars Engeln - mail@lars-engeln.de

	interactive theater for actual acts
*/

#pragma once

#include "cinder/audio/audio.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"

#include "OSCServer.hpp"
#include "AudioDrawUtils.h"
#include "Headphone.hpp"
#include "Speaker.hpp"

#include "Stage.hpp"

using namespace ci;

class AudioPlayer
{
public:
	AudioPlayer(lab::stage::StageRef stage);
	~AudioPlayer();

	static std::shared_ptr<AudioPlayer> create(lab::stage::StageRef stage) { return std::make_shared<AudioPlayer>(stage); }

	void draw();
	void drawGUI();
	void onOutputDeviceChange(ci::audio::OutputDeviceNodeRef out);

	ci::audio::OutputDeviceNodeRef getOutputDevice() { return m_outputDeviceNode; }

	void setSpeaker(std::vector<SpeakerRef> speaker) { m_speakers = speaker; }
	std::vector<SpeakerRef> getSpeaker() { return m_speakers; };
	void setHeadphones(HeadphoneRef headphones) { m_headphones = headphones; }
	HeadphoneRef getHeadphones() { return m_headphones; }

	void setupAudioNodes();

	ci::audio::BufferRef loadBuffer(std::string filePath);
	bool setBuffer(ci::audio::BufferRef buffer);
	void playBuffer(audio::BufferRef buffer);
	void playBuffer();
	
	void playSound(std::string filePath);
	void playSound(std::string filePath, SpeakerRef speaker);
	void playSound(std::string filePath, int speakerID);

	void calibrateSpeaker(int speakerID, float duration, std::string method = "sine", float volume = 95.0f);

	float getPlayPosition();
	float getCurrentVolume();
	const ci::audio::Buffer getCurrentBuffer();
	double getDuration();

	bool playNoise(float volume = 95.f);
	void setHeadsetVolume(float volume);
	void addHeadsetVolume(float volume);

	ci::vec2 getSize() { return m_bufferPlot.getBounds().getSize(); };

private:
	audio::OutputDeviceNodeRef m_outputDeviceNode;
	audio::MonitorSpectralNodeRef m_outputMonitorNode;

	lab::stage::StageRef m_stage;

	std::vector<SpeakerRef> m_speakers;
	HeadphoneRef m_headphones;

	audio::GainNodeRef m_playerGain;
	float m_playerGainValue = 0.99f;
	float m_playerVolume = 100.0f;
	float m_distributedVolume = 95.0f;
	audio::BufferPlayerNodeRef m_bufferPlayerNode;
	audio::ChannelRouterNodeRef m_channelRouterNode;
	WaveformPlot m_bufferPlot;

	bool m_isSettingDistributedVolume = false;
	bool m_distributedVolumeNeedsUpdate = false;

	bool m_isAddingNoise = false;
	float m_lvlNoiseVolume = 40;
	ci::audio::GainNodeRef m_lvlNoiseGain;
	ci::audio::GenNoiseNodeRef m_lvlNoise;

}; using AudioPlayerRef = std::shared_ptr<AudioPlayer>;

