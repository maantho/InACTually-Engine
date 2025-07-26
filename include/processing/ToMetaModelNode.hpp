
/*
	InACTually
	2022

	participants:
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

		class ToMetaModelNode : public NodeBase
		{
		public:
			ToMetaModelNode();
			~ToMetaModelNode();

			NODECREATE(ToMetaModelNode);

			void update()			override;
			void draw()				override;

			ci::JsonTree toJson() override;
			void fromJson(ci::JsonTree json) override;

		private:
			OutputPortRef<MetaModelRef>	m_metaModelPort;

			MetaModelRef m_currentMetaModel;
			bool m_hasChangedMetaModel = false;


			static bool m_registered;
		}; using ToMetaModelNodeRef = std::shared_ptr<ToMetaModelNode>;

	}
}