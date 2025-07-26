
#include "FromMetaModelNode.hpp"
#include "imnodes.h"

#include "Design.hpp"

bool act::proc::FromMetaModelNode::m_registered = act::proc::NodeRegistry::add("FromMetaModel", act::proc::FromMetaModelNode::create);

act::proc::FromMetaModelNode::FromMetaModelNode() : NodeBase("FromMetaModel") {

	m_currentMetaModel = MetaModel::create();

	auto metaModelPort = InputPort<MetaModelRef>::create(PT_METAMODEL_VMI5, "metaModel", [&](MetaModelRef metaModel) {
		sendValueIfUpdated(metaModel->arousal,		m_currentMetaModel->arousal,		m_arousalPort);
		sendValueIfUpdated(metaModel->speed,		m_currentMetaModel->speed,			m_speedPort);

		sendValueIfUpdated(metaModel->dynamic,		m_currentMetaModel->dynamic,		m_dynamicPort);
		sendValueIfUpdated(metaModel->linearity,	m_currentMetaModel->linearity,		m_linearityPort);
		sendValueIfUpdated(metaModel->periodicity,	m_currentMetaModel->periodicity,	m_periodicityPort);
		sendValueIfUpdated(metaModel->frequency,	m_currentMetaModel->frequency,		m_frequencyPort);
		
		sendValueIfUpdated(metaModel->structure,	m_currentMetaModel->structure,		m_structurePort);
		sendValueIfUpdated(metaModel->clarity,		m_currentMetaModel->clarity,		m_clarityPort);
		sendValueIfUpdated(metaModel->complexity,	m_currentMetaModel->complexity,		m_complexityPort);
		sendValueIfUpdated(metaModel->sharpness,	m_currentMetaModel->sharpness,		m_sharpnessPort);
		sendValueIfUpdated(metaModel->regularity,	m_currentMetaModel->regularity,		m_regularityPort);
		sendValueIfUpdated(metaModel->orientation,	m_currentMetaModel->orientation,	m_orientationPort);
		sendValueIfUpdated(metaModel->granularity,	m_currentMetaModel->granularity,	m_granularityPort);
		sendValueIfUpdated(metaModel->hierarchy,	m_currentMetaModel->hierarchy,		m_hierarchyPort);

		sendValueIfUpdated(metaModel->dominance,	m_currentMetaModel->dominance,		m_dominancePort);
		sendValueIfUpdated(metaModel->size,			m_currentMetaModel->size,			m_sizePort);
		sendValueIfUpdated(metaModel->intensity,	m_currentMetaModel->intensity,		m_intensityPort);
		sendValueIfUpdated(metaModel->temperature,	m_currentMetaModel->temperature,	m_temperaturePort);
		sendValueIfUpdated(metaModel->contrast,		m_currentMetaModel->contrast,		m_contrastPort);
		sendValueIfUpdated(metaModel->saturation,	m_currentMetaModel->saturation,		m_saturationPort);


		m_currentMetaModel->arousal		= metaModel->arousal;
		m_currentMetaModel->speed		= metaModel->speed;

		m_currentMetaModel->dynamic		= metaModel->dynamic;
		m_currentMetaModel->linearity	= metaModel->linearity;
		m_currentMetaModel->periodicity = metaModel->periodicity;
		m_currentMetaModel->frequency	= metaModel->frequency;

		m_currentMetaModel->structure	= metaModel->structure;
		m_currentMetaModel->clarity		= metaModel->clarity;
		m_currentMetaModel->complexity	= metaModel->complexity;
		m_currentMetaModel->sharpness	= metaModel->sharpness;
		m_currentMetaModel->regularity	= metaModel->regularity;
		m_currentMetaModel->orientation = metaModel->orientation;
		m_currentMetaModel->granularity = metaModel->granularity;
		m_currentMetaModel->hierarchy	= metaModel->hierarchy;

		m_currentMetaModel->dominance	= metaModel->dominance;
		m_currentMetaModel->size		= metaModel->size;
		m_currentMetaModel->intensity	= metaModel->intensity;
		m_currentMetaModel->temperature = metaModel->temperature;
		m_currentMetaModel->contrast	= metaModel->contrast;
		m_currentMetaModel->saturation	= metaModel->saturation;
	});
	m_inputPorts.push_back(metaModelPort);

	m_arousalPort		= createOutputPort("arousal");
	m_speedPort			= createOutputPort("speed");
	
	m_dynamicPort		= createOutputPort("dynamic");
	m_linearityPort		= createOutputPort("linearity");
	m_periodicityPort	= createOutputPort("periodicity");
	m_frequencyPort		= createOutputPort("frequency");
	
	m_structurePort		= createOutputPort("structure");
	m_clarityPort		= createOutputPort("clarity");
	m_complexityPort	= createOutputPort("complexity");
	m_sharpnessPort		= createOutputPort("sharpness");
	m_regularityPort	= createOutputPort("regularity");
	m_orientationPort	= createOutputPort("orientation");
	m_granularityPort	= createOutputPort("granularity");
	m_hierarchyPort		= createOutputPort("hierarchy");
 
	m_dominancePort		= createOutputPort("dominance");
	m_sizePort			= createOutputPort("size");
	m_intensityPort		= createOutputPort("intensity");
	m_temperaturePort	= createOutputPort("temperature");
	m_contrastPort		= createOutputPort("contrast");
	m_saturationPort	= createOutputPort("saturation");
}

act::proc::FromMetaModelNode::~FromMetaModelNode() {
}

void act::proc::FromMetaModelNode::update() {
}

void act::proc::FromMetaModelNode::draw() {

	ivec2 displaySize = ivec2(200, 200);
	
	ImNodes::BeginNode(m_id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(getName().c_str());
	ImNodes::EndNodeTitleBar();

	//ImGui::SetNextItemWidth(200);

	
	
	drawPorts(displaySize.x);

	ImNodes::EndNode();
}

ci::JsonTree act::proc::FromMetaModelNode::toJson() {
	ci::JsonTree json = ci::JsonTree::makeObject("params");
	return json;

	if (m_currentMetaModel)
		return m_currentMetaModel->toJson();
	else return json;
}

void act::proc::FromMetaModelNode::fromJson(ci::JsonTree json) {
	if (m_currentMetaModel)
		return m_currentMetaModel->fromJson(json);
}

act::proc::OutputPortRef<float> act::proc::FromMetaModelNode::createOutputPort(string featureName)
{
	auto port = OutputPort<float>::create(PT_NUMBER, featureName);
	m_outputPorts.push_back(port);
	return port;
}

void act::proc::FromMetaModelNode::sendValueIfUpdated(float newValue, float oldValue, OutputPortRef<float> port)
{
	if (newValue != oldValue)
		port->send(newValue);
}