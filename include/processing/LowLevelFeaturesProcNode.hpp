#pragma once
#include "ProcNodeBase.hpp"
#include <numeric>

using namespace ci;
using namespace ci::app;

namespace act {
	namespace proc {
		class LowLevelFeaturesProcNode : public ProcNodeBase {
		public:
			LowLevelFeaturesProcNode();
			~LowLevelFeaturesProcNode();

			PROCNODECREATE(LowLevelFeaturesProcNode);
			void update() override;
			void draw() override;

			ci::Json toParams() override;
			void fromParams(ci::Json json) override;

		private:
			InputPortRef<numberList>		m_spectrumIn;
			InputPortRef<number>			m_rmsIn;
			OutputPortRef<number>			m_centroidOut;
			OutputPortRef<number>			m_lowEnergyOut;

			numberList						m_rmsHistory;
			number							m_rmsAvg;
			unsigned int					m_lowEnergySamplesCount;
			number							m_lowEnergy;
			number							m_centroid;
			number							m_rms;

			void updateSpectrum(numberList spectrum);
			void updateLowEnergy(number rms);

		};
		using LowLevelFeaturesProcNodeRef = std::shared_ptr<LowLevelFeaturesProcNode>;
	}
}