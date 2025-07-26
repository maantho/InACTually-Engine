
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
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

		class BodyToSoundProcNode : public ProcNodeBase
		{
		public:
			BodyToSoundProcNode();
			~BodyToSoundProcNode();

			PROCNODECREATE(BodyToSoundProcNode);

			void update()			override;
			void draw()				override;
			void setup(act::room::RoomManagers) override;

			void onBody(room::BodyRef event);

			ci::Json toParams() override;
			void fromParams(ci::Json json) override;

 
		private:
			OutputPortRef<float>	m_localMovementPort;
			OutputPortRef<float>	m_globalMovementPort;
			OutputPortRef<audio::NodeRef>	m_audioOutPort;
			
			float calcLocalMovement(room::BodyRef body);
			float calcGlobalMovement(room::BodyRef body);
			float calcHandDistance(room::BodyRef body);
			float calcVelocity(vec3 last, vec3 current);

			float m_localMovement;
			float m_globalMovement;
			float m_handDistance;
			float m_leftHandVelocity;
			float m_rightHandVelocity;
			float m_handDistanceY;
			float m_handDistanceXZ;
			float m_leftHandDistance;
			float m_leftHandDistanceY;
			float m_leftHandDistanceXZ;
			float m_rightHandDistance;
			float m_rightHandDistanceY;
			float m_rightHandDistanceXZ;
			float m_pelvisKneeDistanceY;

			int m_cutOff = 10000;
			int m_Q = 5;
			int m_freqAM = 80;
			int m_freqFM = 100;
			int m_modStrength = 500;
			float m_volume = 0.0f;

			room::BodyRef m_oldBody;
			float   m_scaleValue;

			audio::GenTriangleNodeRef m_osc;
			audio::GenSineNodeRef m_modFM;
			audio::GainNodeRef m_gainFM;
			audio::GenSineNodeRef m_modAM;
			audio::AddNodeRef m_add;
			audio::MultiplyNodeRef m_mul;
			audio::GainNodeRef m_gain;
			audio::FilterLowPassNodeRef m_lowP;


			static bool m_registered;
		}; using BodyToSoundProcNodeRef = std::shared_ptr<BodyToSoundProcNode>;

	}
}