
#include "ToMetaModelNode.hpp"
#include "imnodes.h"

#include "Design.hpp"


#define METAMODAL_CRETATEINPUTPORT(feature, featureName, additionalFunc)	m_inputPorts.push_back(InputPort<float>::create(PT_NUMBER, featureName, [&](float value) { \
																				m_currentMetaModel->feature = value; \
																				additionalFunc(); \
																				m_hasChangedMetaModel = true; \
																			}));

bool act::proc::ToMetaModelNode::m_registered = act::proc::NodeRegistry::add("ToMetaModel", act::proc::ToMetaModelNode::create);

act::proc::ToMetaModelNode::ToMetaModelNode() : NodeBase("ToMetaModel") {

	m_currentMetaModel = MetaModel::create();

	m_metaModelPort = OutputPort<MetaModelRef>::create(PT_METAMODEL_VMI5, "metaModel");
	m_outputPorts.push_back(m_metaModelPort);

	METAMODAL_CRETATEINPUTPORT(arousal,		"arousal",		[]() {});
	METAMODAL_CRETATEINPUTPORT(speed,		"speed",		[&]() { m_currentMetaModel->calcArousal(); });

	METAMODAL_CRETATEINPUTPORT(dynamic,		"dynamic",		[&]() { m_currentMetaModel->calcArousal(); });
	METAMODAL_CRETATEINPUTPORT(linearity,	"linearity",	[&]() { m_currentMetaModel->calcDynamic(); });
	METAMODAL_CRETATEINPUTPORT(periodicity, "periodicity",	[&]() { m_currentMetaModel->calcDynamic(); });
	METAMODAL_CRETATEINPUTPORT(frequency,	"frequency",	[&]() { m_currentMetaModel->calcDynamic(); });
															
	METAMODAL_CRETATEINPUTPORT(structure,	"structure",	[&]() { m_currentMetaModel->calcArousal(); });
	METAMODAL_CRETATEINPUTPORT(clarity,		"clarity",		[&]() { m_currentMetaModel->calcStructure(); });
	METAMODAL_CRETATEINPUTPORT(complexity,	"complexity",	[&]() { m_currentMetaModel->calcStructure(); });
	METAMODAL_CRETATEINPUTPORT(sharpness,	"sharpness",	[&]() { m_currentMetaModel->calcStructure(); });
	METAMODAL_CRETATEINPUTPORT(regularity,	"regularity",	[&]() { m_currentMetaModel->calcStructure(); });
	METAMODAL_CRETATEINPUTPORT(orientation, "orientation",	[&]() { m_currentMetaModel->calcStructure(); });
	METAMODAL_CRETATEINPUTPORT(granularity, "granularity",	[&]() { m_currentMetaModel->calcStructure(); });
	METAMODAL_CRETATEINPUTPORT(hierarchy,	"hierarchy",	[&]() { m_currentMetaModel->calcStructure(); });
															
	METAMODAL_CRETATEINPUTPORT(dominance,	"dominance",	[&]() { m_currentMetaModel->calcArousal(); });
	METAMODAL_CRETATEINPUTPORT(size,		"size",			[&]() { m_currentMetaModel->calcDominance(); });
	METAMODAL_CRETATEINPUTPORT(intensity,	"intensity",	[&]() { m_currentMetaModel->calcDominance(); });
	METAMODAL_CRETATEINPUTPORT(temperature, "temperature",	[&]() { m_currentMetaModel->calcDominance(); });
	METAMODAL_CRETATEINPUTPORT(contrast,	"contrast",		[&]() { m_currentMetaModel->calcDominance(); });
	METAMODAL_CRETATEINPUTPORT(saturation,	"saturation",	[&]() { m_currentMetaModel->calcDominance(); });

}

act::proc::ToMetaModelNode::~ToMetaModelNode() {
}

void act::proc::ToMetaModelNode::update() {
	if (m_hasChangedMetaModel) {
		m_metaModelPort->send(m_currentMetaModel);
		m_hasChangedMetaModel = false;
	}
}

void act::proc::ToMetaModelNode::draw() {

	ivec2 displaySize = ivec2(200, 200);
	
	ImNodes::BeginNode(m_id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(getName().c_str());
	ImNodes::EndNodeTitleBar();

	//ImGui::SetNextItemWidth(200);

	
	drawPorts(displaySize.x);

	ImNodes::EndNode();
}

ci::JsonTree act::proc::ToMetaModelNode::toJson() {
	return m_currentMetaModel->toJson();
}

void act::proc::ToMetaModelNode::fromJson(ci::JsonTree json) {
}
