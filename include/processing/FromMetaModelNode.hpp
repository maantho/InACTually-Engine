
/*
	InACTually
	2022

	contributor:
	Lars Engeln - mail@lars-engeln.de

	interactive theater for actual acts
*/

#pragma once

#include "NodeBase.hpp"
#include "MetaModel.hpp"

using namespace ci;
using namespace ci::app;


namespace act {
	namespace proc {

		class FromMetaModelNode : public NodeBase
		{
		public:
			FromMetaModelNode();
			~FromMetaModelNode();

			NODECREATE(FromMetaModelNode);

			void update()			override;
			void draw()				override;

			ci::JsonTree toJson() override;
			void fromJson(ci::JsonTree json) override;

		private:
			OutputPortRef<float>	m_arousalPort;
			OutputPortRef<float>	m_speedPort;
					  		 
			OutputPortRef<float>	m_dynamicPort;
			OutputPortRef<float>	m_linearityPort;
			OutputPortRef<float>	m_periodicityPort;
			OutputPortRef<float>	m_frequencyPort;
					  		 
			OutputPortRef<float>	m_structurePort;
			OutputPortRef<float>	m_clarityPort;
			OutputPortRef<float>	m_complexityPort;
			OutputPortRef<float>	m_sharpnessPort;
			OutputPortRef<float>	m_regularityPort;
			OutputPortRef<float>	m_orientationPort;
			OutputPortRef<float>	m_granularityPort;
			OutputPortRef<float>	m_hierarchyPort;
					  		 
			OutputPortRef<float>	m_dominancePort;
			OutputPortRef<float>	m_sizePort;
			OutputPortRef<float>	m_intensityPort;
			OutputPortRef<float>	m_temperaturePort;
			OutputPortRef<float>	m_contrastPort;
			OutputPortRef<float>	m_saturationPort;

			MetaModelRef m_currentMetaModel;

			OutputPortRef<float> createOutputPort(string featureName);
			void sendValueIfUpdated(float newValue, float oldValue, OutputPortRef<float> port);

			static bool m_registered;
		}; using FromMetaModelNodeRef = std::shared_ptr<FromMetaModelNode>;

	}
}