
#pragma once

#include "NodeBase.hpp"
#include "MatListener.hpp"

#include "CameraManager.hpp"

using namespace ci;
using namespace ci::app;

namespace act {
	namespace proc {

		class AzureKinectPointCloudNode : public NodeBase
		{
		public:
			AzureKinectPointCloudNode();
			~AzureKinectPointCloudNode();

			NODECREATE(AzureKinectPointCloudNode);

			void setup(act::mod::DeviceManagers deviceMgrs)			override;
			void update()			override;
			void draw()				override;

			ci::JsonTree toJson() override;
			void fromJson(ci::JsonTree json) override;

		private:
			act::scn::CameraManagerRef m_camMgr;
 
			static bool m_registered;

 		};

		using DummyNodeRef = std::shared_ptr<AzureKinectPointCloudNode>;

	}
}
