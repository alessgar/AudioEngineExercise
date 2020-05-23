#pragma once
#include "fmod.hpp"
#include "windows.h"
#include <iostream>
#include <map>
#include <string>
#include "crossSleep.hpp"

typedef std::map<std::string, FMOD::Sound*> SoundMap;
typedef std::map<int, FMOD::Channel*> ChannelMap;
typedef std::map<int, FMOD::DSP*> DSPMap;

class OPI_AUDIO {
	private:
		FMOD::System* audioSystem = NULL;
		SoundMap aSounds;
		ChannelMap aChannels;
		DSPMap aDSPs;
		int findFirstAvailableChannelID();
		int findFirstAvailableDSPID();
		int FMODErrorCheck(FMOD_RESULT result);
	protected:
	public:
		OPI_AUDIO();
		void CreateSound(std::string SNDPath);
		void DestroySound(std::string SNDPath);
		int StartSound(std::string SNDPath);
		void DestroyChannel(int channelID);
		void WaitForChannel(int channelID);
		void PlayChannel(int channelID);
		void PauseChannel(int channelID);
		void SetChannelVolume(int channelID, float channelVolume);
		void SetChannelPitch(int channelID, float channelPitch);
		int CreatePresetDSP(FMOD_DSP_TYPE DSPType);
		void ApplyChannelDSP(int channelID, int DSPID);
		void DSPWetDryMix(int DSPID, float preWet, float postWet, float dry);
		void SetChannelFrequency(int ChannelID, float frequency);
		void Destroy();
};