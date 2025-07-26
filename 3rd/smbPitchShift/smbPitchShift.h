#pragma once
void smb_Fft(float *fftBuffer, long fftFrameSize, long sign);
double smb_Atan2(double x, double y);
void smb_PitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);