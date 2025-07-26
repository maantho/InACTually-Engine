#include "procpch.hpp"
#include "LowLevelFeaturesProcNode.hpp"

act::proc::LowLevelFeaturesProcNode::LowLevelFeaturesProcNode() : ProcNodeBase("LowLevelFeatures") {

	m_drawSize = ivec2(200, 200);

	m_spectrumIn = InputPort<numberList>::create(PT_NUMBERLIST, "spectrumIn", [&](numberList spectrum) {this->updateSpectrum(spectrum); });
	m_rmsIn = InputPort<number>::create(PT_NUMBER, "rmsIn", [&](number rms) {this->updateLowEnergy(rms); });
	m_centroidOut = OutputPort<number>::create(PT_NUMBER, "centroidOut");
	m_lowEnergyOut = OutputPort<number>::create(PT_NUMBER, "lowEnergyOut");

	m_outputPorts.push_back(m_lowEnergyOut);
	m_outputPorts.push_back(m_centroidOut);
	m_inputPorts.push_back(m_spectrumIn);
	m_inputPorts.push_back(m_rmsIn);

	m_centroid = 0;
	m_lowEnergy = 0;
	m_lowEnergySamplesCount = 0;
	m_rmsAvg = 0;
}

act::proc::LowLevelFeaturesProcNode::~LowLevelFeaturesProcNode() {}

void act::proc::LowLevelFeaturesProcNode::update() {
	m_centroidOut->send(m_centroid);
	m_lowEnergyOut->send(m_lowEnergy);
}

void act::proc::LowLevelFeaturesProcNode::draw() {
	beginNodeDraw();

	ImGui::SetNextItemWidth(m_drawSize.x);
	ImGui::SliderFloat("centroid", &m_centroid, 0.0f, 1.0f);

	ImGui::SetNextItemWidth(m_drawSize.x);
	ImGui::SliderFloat("lowEnergy", &m_lowEnergy, 0.0f, 1.0f);

	endNodeDraw();
}

void act::proc::LowLevelFeaturesProcNode::updateSpectrum(numberList spectrum) {
	
	m_centroid = audio::dsp::spectralCentroid(spectrum.data(), spectrum.size(), audio::master()->getSampleRate()) / 22000;
}

void act::proc::LowLevelFeaturesProcNode::updateLowEnergy(number rms) {
	if (rms > 0.001f && m_centroid > 0.001f) {
		m_rmsHistory.push_back(rms);
		m_rmsAvg = reduce(m_rmsHistory.begin(), m_rmsHistory.end(), 0.0) / m_rmsHistory.size();
		if (rms < m_rmsAvg) { m_lowEnergySamplesCount++; }
		m_lowEnergy = (number)m_lowEnergySamplesCount / m_rmsHistory.size();
	}
	else {

		m_centroid = 0;
		m_lowEnergy = 0;
		m_lowEnergySamplesCount = 0;
		m_rmsAvg = 0;
		m_rmsHistory.clear();
	}
}

ci::Json act::proc::LowLevelFeaturesProcNode::toParams() {
	ci::Json json = ci::Json::object();
	return json;
}

void act::proc::LowLevelFeaturesProcNode::fromParams(ci::Json json) {
}



