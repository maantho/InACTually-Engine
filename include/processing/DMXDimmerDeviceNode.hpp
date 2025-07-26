/*
	InACTually
	2022

	participants:
	Lars Engeln - mail@lars-engeln.de
	Christian Kinne - christian.kinne@tu-dresden.de

	interactive theater for actual acts
*/

#pragma once

#include "NodeBase.hpp"
#include "DimmerSceneNode.hpp"

using namespace ci;
using namespace ci::app;


namespace act {
	namespace proc {

		class DMXDimmerDeviceNode : public NodeBase
		{
		public:
			DMXDimmerDeviceNode();
			~DMXDimmerDeviceNode();

			NODECREATE(DMXDimmerDeviceNode);

			void update()			override;
			void draw()				override;
			void setup(act::scn::DeviceManagers deviceMgrs)			override;

			ci::Json toJson() override;
			void fromJson(ci::Json json) override;

		private:
 			float m_dim;
			scn::DMXManagerRef m_dmxMgr;
			scn::DimmerSceneNodeRef m_dimmer;
			int		m_selectedDimmer;

			void onValue(float value);

			static bool m_registered;
		}; using DMXDimmerDeviceNodeRef = std::shared_ptr<DMXDimmerDeviceNode>;

	}
}