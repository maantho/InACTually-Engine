#include "DMXDimmerDeviceNode.hpp"
#include "imnodes.h"

#include "Design.hpp"

bool act::proc::DMXDimmerDeviceNode::m_registered = act::proc::NodeRegistry::add("DMXDimmerDevice", act::proc::DMXDimmerDeviceNode::create);

act::proc::DMXDimmerDeviceNode::DMXDimmerDeviceNode() : NodeBase("DMXDimmerDevice") {


	auto image = InputPort<float>::create(PT_NUMBER, "value", [&](float value) { this->onValue(value); });
	m_inputPorts.push_back(image);
}

act::proc::DMXDimmerDeviceNode::~DMXDimmerDeviceNode() {
}

void act::proc::DMXDimmerDeviceNode::update() {
 }

void act::proc::DMXDimmerDeviceNode::setup(act::mod::DeviceManagers deviceMgrs) {
	m_dmxMgr = deviceMgrs.dmxMgr;
}

void act::proc::DMXDimmerDeviceNode::onValue(float value) {
 
	m_dimmer->setDimmer(value * 255);
}

void act::proc::DMXDimmerDeviceNode::draw() {

	ivec2 displaySize = ivec2(200, 200);

	ImNodes::BeginNode(m_id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(getName().c_str());
	ImNodes::EndNodeTitleBar();

 
	ImGui::SetNextItemWidth(200);

	if (m_dmxMgr->getFixtureNames().empty())
		ImGui::Text("No DMX devices have been set up.");
	else {
		ImGui::SetNextItemWidth(displaySize.x - ImGui::CalcTextSize("MovingHead").x);
		if (ImGui::Combo("Dimmer", &m_selectedDimmer, m_dmxMgr->getFixtureNames())) {
			m_dimmer = m_dmxMgr->getDimmerByIndex(m_selectedDimmer);
		}

		if (m_dimmer) {
			ImGui::PushItemWidth(300);
			ImGui::Text(m_dimmer->getName().c_str());
		 
 			ImGui::PopItemWidth();
		}
	}

	 
	drawPorts(displaySize.x);

	ImNodes::EndNode();
}

ci::JsonTree act::proc::DMXDimmerDeviceNode::toJson() {
	ci::JsonTree json = ci::JsonTree::makeObject("params");
 	return json;
}

void act::proc::DMXDimmerDeviceNode::fromJson(ci::JsonTree json) {
 }
