
#include "AudioPlayer.hpp"

#include <DynamicsNodes.hpp>

#include "cinder/CinderImGui.h"

AudioPlayer::AudioPlayer(lab::stage::StageRef stage)
{
	m_stage = stage;
	
	/*m_OSCSender->sendMsg(osc::Message("/clear/"));

	{
		osc::Message msg("/mixer/");
		msg.append("nearest");
		m_OSCSender->sendMsg(msg);
	}
	{
		osc::Message msg("/add/");
		msg.append("subwoofer");
		msg.append(10);		// id
		msg.append(3.0f);	// x
		msg.append(0.3f);	// y	
		msg.append(0.3f);	// z	
		msg.append(10);		// channel // 3
		// m_OSCSender->sendMsg(msg);
	}
	{
		osc::Message msg("/volume/");
		msg.append(10);		// id
		msg.append(85.0f);	// dB
		m_OSCSender->sendMsg(msg);
	}*/
}

AudioPlayer::~AudioPlayer()
{
	//if (m_playerGain)
	//	m_playerGain->disconnectAll();
}

void AudioPlayer::setupAudioNodes()
{
	auto ctx = audio::master();
	ctx->disable();

	if (m_playerGain)
		m_playerGain->disconnectAll();

	auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize(2048).windowSize(1024).channels(2);
	m_outputMonitorNode = ctx->makeNode(new audio::MonitorSpectralNode(monitorFormat));

	m_bufferPlayerNode = ctx->makeNode(new audio::BufferPlayerNode(audio::Node::Format().channels(2)));
	m_channelRouterNode = ctx->makeNode(new audio::ChannelRouterNode(audio::Node::Format().channels(ctx->getOutput()->getNumChannels())));
	auto leftChannelRouterNode = ctx->makeNode(new audio::ChannelRouterNode(audio::Node::Format().channels(2))); // just from stereo to 0
	auto rightChannelRouterNode = ctx->makeNode(new audio::ChannelRouterNode(audio::Node::Format().channels(2)));// just from stereo to 0
	// add a Gain to reduce the volume
	m_playerGain = ctx->makeNode(new audio::GainNode(m_playerGainValue));

	int headphoneChannel = m_headphones->getChannel();
	if (headphoneChannel >= ctx->getOutput()->getNumChannels()) {
		headphoneChannel = 0; // fallback
	}

	lab::stage::HeadphoneNodeRef headphoneNode = m_headphones->m_headphoneNode;
	//headphoneNode->setupAudioNodes();
	// connect and enable the Context

	auto stereoFmt	= audio::Node::Format().channels(2).autoEnable();
	auto limiter	= ctx->makeNode(new LimiterNode(stereoFmt));
	auto compressor = ctx->makeNode(new CompressorNode(stereoFmt));
	auto expander	= ctx->makeNode(new ExpanderNode(stereoFmt));


	auto leftOutChannelRouterNode = ctx->makeNode(new audio::ChannelRouterNode(audio::Node::Format().channels(ctx->getOutput()->getNumChannels()))); 
	auto rightOutChannelRouterNode = ctx->makeNode(new audio::ChannelRouterNode(audio::Node::Format().channels(ctx->getOutput()->getNumChannels())));
	m_bufferPlayerNode >> m_playerGain >> limiter; // >> compressor >> expander;
	
	limiter >> leftChannelRouterNode->route(0, 0, 1) >> headphoneNode->getL()->getIn();
	limiter >> rightChannelRouterNode->route(1, 0, 1) >> headphoneNode->getR()->getIn();
	
	headphoneNode->getL()->getOut() >> leftOutChannelRouterNode->route(0, headphoneNode->getL()->getChannel(), 1) >> m_outputMonitorNode;
	//headphoneNode->getR()->getOut() >> rightOutChannelRouterNode->route(headphoneNode->getR()->getChannel(), 1, 1) >> m_outputMonitorNode;

	int res = 2000;
	auto flt = cv::UMat(1, res, CV_32F, cv::Scalar(1.0f));
	int lowreject = 12;
	int highreject = 400;
	cv::multiply(flt(cv::Rect(0, 0, lowreject, 1)), 0.0f, flt(cv::Rect(0, 0, lowreject, 1)));
	cv::multiply(flt(cv::Rect(res-highreject, 0, highreject, 1)), 0.0f, flt(cv::Rect(res- highreject, 0, highreject, 1)));

	m_headphones->m_headphoneNode->setEQFilter(flt, flt);


	m_lvlNoiseGain = ci::audio::Context::master()->makeNode(new ci::audio::GainNode(ci::audio::decibelToLinear(m_lvlNoiseVolume)));

	m_lvlNoise = ci::audio::Context::master()->makeNode(new ci::audio::GenNoiseNode());
	m_lvlNoise >> m_lvlNoiseGain;
	m_lvlNoiseGain >> headphoneNode->getL()->getIn();
	m_lvlNoiseGain >> headphoneNode->getR()->getIn();

	m_lvlNoiseGain >> m_outputMonitorNode;
	m_lvlNoise->disable();


	ctx->enable();
}

ci::audio::BufferRef AudioPlayer::loadBuffer(std::string filePath)
{
	if (filePath.empty())
		return nullptr;

	ci::audio::SourceFileRef source;
	try {
		source = audio::load(ci::loadFile(filePath), audio::Context::master()->getSampleRate());
		return source->loadBuffer();
	}
	catch (...) {
		return nullptr;
	}
}

bool AudioPlayer::setBuffer(ci::audio::BufferRef buffer)
{
	if (!buffer || buffer->getNumFrames() == 0)
		return false;

	m_bufferPlot.load(buffer, Rectf(0, 0, app::getWindowWidth() * 0.7, app::getWindowHeight() * 0.2 * buffer->getNumChannels()));

	if (buffer->getNumChannels() == 1) {
		ci::audio::BufferRef monoBinaural = std::make_shared<ci::audio::Buffer>(buffer->getNumFrames(), 2);
		monoBinaural->copyChannel(0, buffer->getChannel(0));
		monoBinaural->copyChannel(1, buffer->getChannel(0));
		m_bufferPlayerNode->setBuffer(monoBinaural);
	}
	else {
		m_bufferPlayerNode->setBuffer(buffer);
	}

	return true;
}

void AudioPlayer::playBuffer(audio::BufferRef buffer)
{
	if(setBuffer(buffer));
		playBuffer();
}

void AudioPlayer::playBuffer() {
	m_bufferPlayerNode->start();
}

void AudioPlayer::playSound(std::string filePath)
{
	playBuffer(loadBuffer(filePath));
}

void AudioPlayer::playSound(std::string filePath, SpeakerRef speaker)
{
	if (filePath.empty())
		return;

	m_stage->removeSound(0);
	auto sound = m_stage->addSound(speaker->getPosition(), std::filesystem::path(filePath));
	sound->setID(0);
	sound->loop(false);
	sound->play();
}

void AudioPlayer::playSound(std::string filePath, int speakerID)
{
	SpeakerRef speaker;
	for (auto&& sp : m_speakers) {
		if (sp->getID() == speakerID)
			speaker = sp;
	}
	if(speaker)
		playSound(filePath, speaker);
}

void AudioPlayer::calibrateSpeaker(int speakerID, float duration, std::string method, float volume)
{
	m_stage->calibrate(speakerID, method, duration, volume);
}

float AudioPlayer::getPlayPosition()
{
	if (m_bufferPlayerNode->getNumFrames() == 0)
		return 0.0f;
	return m_bufferPlayerNode->getReadPosition() / m_bufferPlayerNode->getNumFrames();
}

float AudioPlayer::getCurrentVolume()
{
	return ci::audio::linearToDecibel(m_outputMonitorNode->getVolume());
}

const ci::audio::Buffer AudioPlayer::getCurrentBuffer() {
	return m_outputMonitorNode->getBuffer();
}

double AudioPlayer::getDuration()
{
	return m_bufferPlayerNode->getNumSeconds();
}

bool AudioPlayer::playNoise(float volume)
{
	if (!m_isAddingNoise) {
		m_lvlNoiseVolume = volume;
		m_lvlNoiseGain->setValue(audio::decibelToLinear(m_lvlNoiseVolume));
	}
	m_isAddingNoise = !m_isAddingNoise;
	m_lvlNoise->setEnabled(m_isAddingNoise);

	return m_isAddingNoise;
}

void AudioPlayer::setHeadsetVolume(float volume)
{
	m_playerGain->setValue(ci::audio::decibelToLinear(volume));
}

void AudioPlayer::addHeadsetVolume(float volume) {
	m_playerGain->setValue(m_playerGain->getValue() + ci::audio::decibelToLinear(volume));
}

void AudioPlayer::onOutputDeviceChange(ci::audio::OutputDeviceNodeRef out)
{
	m_outputDeviceNode = out;
	setupAudioNodes();
}

void AudioPlayer::draw()
{
	gl::pushMatrices();

	if (m_bufferPlayerNode && !m_bufferPlayerNode->isEof()) {
		gl::color(Color(1.0f, 0.0f, 0.1f));
	}
	else {
		gl::color(Color(1.0f, 1.0f, 0.0f));
	}

	m_bufferPlot.draw();
	gl::drawSolidRect(Rectf(vec2(m_bufferPlot.getBounds().getWidth() * getPlayPosition() + 2, 0), vec2(m_bufferPlot.getBounds().getWidth() * getPlayPosition(), m_bufferPlot.getBounds().getHeight())));
	
	gl::popMatrices();
}

void AudioPlayer::drawGUI()
{
	ImGui::Begin("Playback");
	if (ImGui::DragFloat("Headphone Volume", &m_playerVolume, 0.1f, 0.0f, 120.0f)) {
		m_playerGain->setValue(ci::audio::decibelToLinear(m_playerVolume));
	}
	float currentVolume = getCurrentVolume();
	ImGui::DragFloat("Current Volume", &currentVolume, 0.1f, 0.0f, 120.0f);
	
	//ImGui::Separator();
	//ImGui::DragFloat("Speaker Volume", &m_distributedVolume, 0.1f, 0.0f, 120.0f);

	
	ImGui::Separator();
	if (ImGui::Checkbox("add noise", &m_isAddingNoise)) {
		m_lvlNoise->setEnabled(m_isAddingNoise);
	}
	if (m_isAddingNoise) {
		if (ImGui::SliderFloat("noise volume", &m_lvlNoiseVolume, 0.0f, 100.0f)) {
			m_lvlNoiseGain->setValue(audio::decibelToLinear(m_lvlNoiseVolume));
		}
	}
	
	m_isSettingDistributedVolume = false;
	ImGui::End();
}
