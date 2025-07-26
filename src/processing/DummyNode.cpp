
#include "DummyNode.hpp"
#include "imnodes.h"

bool act::proc::DummyNode::m_registered = act::proc::NodeRegistry::add("Dummy", act::proc::DummyNode::create);

act::proc::DummyNode::DummyNode() : NodeBase("Dummy") {
 
}

act::proc::DummyNode::~DummyNode() {
	 
}

void act::proc::DummyNode::setup(act::mod::DeviceManagers deviceMgrs) {
	m_camMgr = deviceMgrs.cameraMgr;

 }

void act::proc::DummyNode::update() {

}

void act::proc::DummyNode::draw() {
	ivec2 displaySize = ivec2(100, 200);

	ImNodes::BeginNode(m_id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(getName().c_str());
	ImNodes::EndNodeTitleBar();
 
	drawPorts(displaySize.x);
	

	ImNodes::EndNode();
}



ci::JsonTree act::proc::DummyNode::toJson() {
	ci::JsonTree json = ci::JsonTree::makeObject("params");
 	return json;
}

void act::proc::DummyNode::fromJson(ci::JsonTree json) {
	 
}
 