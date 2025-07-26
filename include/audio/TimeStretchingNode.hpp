
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2023

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#pragma once

#include "cinder/Cinder.h"
#include "cinder/audio/Node.h"
#include "cinder/audio/dsp/Fft.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/audio/Source.h"
#include "smbPitchShift/smbPitchShift.h"
#include "cinder/Thread.h"


namespace act {
	namespace aio {

		typedef std::shared_ptr<class TimeStretchingNode>	TimeStrechingNodeRef;

		class TimeStretchingNode : public ci::audio::Node {
		public:

			TimeStretchingNode(ci::audio::SourceFileRef sf, double bpm, const Format &format = Format()) : Node(format) {
				m_sourceFile = sf;
				m_bpm = bpm;
				m_isIdle = false;
				m_isRequestingStop = false;
				m_isPaused = false;
				m_usePitchCorrection = false;

				m_minSpeed = 0.2f;
				m_maxSpeed = 4.0f;
				m_fftSize = 2048;
				m_fftBufferSize = 4096;
			}
			~TimeStretchingNode();

			void setPlaybackSpeed(float speed);
			double calcBPM();
			float getPlaybackSpeed();
			void play() { m_isPaused = false; };
			void pause() { m_isPaused = true; };
			bool isPaused() { return m_isPaused; };
			void togglePitchCorrection();
			bool getUsePitchCorrection();
			double setNewTrack(std::string assetName, double bpm);
			void emptyRingBuffer();
		protected:
	
			void initialize()						override;
			void process(ci::audio::Buffer *buffer)	override;
			void timestretching();
			void setNewSourceFile(ci::audio::SourceFileRef sf);
			void setNewSourceFileCashedBPM(ci::audio::SourceFileRef sf,double BPM);

		private:
			float m_minSpeed;
			float m_maxSpeed;
			ci::audio::SourceFileRef m_sourceFile;
			std::shared_ptr<std::thread>	m_thread;
			ci::audio::Buffer* m_fftBuffer;
			ci::audio::Buffer* m_speedModulationBuffer;
			std::atomic<float> m_speed;
			std::atomic<float> m_pitch;
			ci::audio::BufferRef m_buffer;
			std::shared_ptr<ci::audio::dsp::RingBuffer>	m_ringBuffer;	
			std::size_t m_fftSize;
			std::size_t m_fftBufferSize;
			std::atomic<int> m_currentPlayPosition;
			float m_sampleRateAdjustment;
			int m_fileSampleRate;
			bool m_isRequestingStop;
			bool m_isIdle;
			bool m_isPaused;
			bool m_usePitchCorrection;
			double m_bpm;
		};
	}
}
