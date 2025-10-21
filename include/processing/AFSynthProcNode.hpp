
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2025

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#pragma once

#include "ProcNodeBase.hpp"

using namespace ci;
using namespace ci::app;


namespace act {
	namespace proc {

		class AFSynthProcNode : public ProcNodeBase
		{
		public:
			AFSynthProcNode();
			~AFSynthProcNode();

			PROCNODECREATE(AFSynthProcNode);

			void update()			override;
			void draw()				override;
			void setup(act::room::RoomManagers) override;

			ci::Json toParams() override;
			void fromParams(ci::Json json) override;

 
		private:
			OutputPortRef<float>	m_volumePort;
			OutputPortRef<audio::NodeRef>	m_audioOutPort;

			void onFMValue(float value);
			void onAMValue(float value);
			void onModValue(float value);
		

			int m_cutOff = 10000;
			int m_Q = 5;
			int m_freqAM = 80;
			int m_freqFM = 100;
			int m_modStrength = 500;
			float m_volume = 0.0f;

			float   m_scaleValue;

			audio::GenTriangleNodeRef m_osc;
			audio::GenSineNodeRef m_modFM;
			audio::GainNodeRef m_gainFM;
			audio::GenSineNodeRef m_modAM;
			audio::AddNodeRef m_add;
			audio::MultiplyNodeRef m_mul;
			audio::GainNodeRef m_gain;
			audio::FilterLowPassNodeRef m_lowP;
			audio::MonitorNodeRef m_monitor;


			static bool m_registered;
		}; using AFSynthProcNodeRef = std::shared_ptr<AFSynthProcNode>;

	}
}