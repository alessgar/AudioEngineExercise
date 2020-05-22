#pragma once
#include "fmod.hpp"
#include "windows.h"
#include <iostream>
#include <map>
#include <string>
#include "crossSleep.hpp"

typedef std::map<std::string, FMOD::Sound*> SoundMap;
typedef std::map<int, FMOD::Channel*> ChannelMap;

class OPI_AUDIO {
	private:
		FMOD::System* audioSystem = NULL;
		SoundMap aSounds;
		ChannelMap aChannels;
		int findFirstAvailableChannelID();
		int FMODErrorCheck(FMOD_RESULT result);
	protected:
	public:
		OPI_AUDIO();
		void CreateSound(std::string SNDPath);
		void DestroySound(std::string SNDPath);
		int StartSound(std::string SNDPath);
		void DestroyChannel(int channelID);
		void WaitForChannel(int channelID);
		void Destroy();
};