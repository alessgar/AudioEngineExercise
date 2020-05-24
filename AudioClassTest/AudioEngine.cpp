#include "AudioEngine.hpp"

OPI_AUDIO::OPI_AUDIO() {
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (!SUCCEEDED(hr)) {
		std::cout << "Failed to intialize COM!" << std::endl;
		exit(1);
	}

	FMODErrorCheck(FMOD::System_Create(&audioSystem));

	FMODErrorCheck(audioSystem->init(4093, FMOD_INIT_NORMAL, NULL));
}

void OPI_AUDIO::Destroy() {
	CoUninitialize();
}

void OPI_AUDIO::CreateSound(std::string SNDPath) {
	auto foundSounds = aSounds.find(SNDPath);
	if (foundSounds != aSounds.end()) {
		std::cout << "[" + SNDPath + "] is already loaded!" << std::endl;
		return;
	}

	FMOD::Sound* sound = NULL;
	FMODErrorCheck(audioSystem->createSound(SNDPath.c_str(), FMOD_DEFAULT, NULL, &sound));
	if (sound) {
		aSounds[SNDPath] = sound;
	}
}

void OPI_AUDIO::DestroySound(std::string SNDPath) {
	auto foundSounds = aSounds.find(SNDPath);
	if (foundSounds == aSounds.end()) {
		std::cout << "[" + SNDPath + "] does not exist/already destroyed!" << std::endl;
		return;
	}

	FMODErrorCheck(foundSounds->second->release());
	aSounds.erase(foundSounds);
}

int OPI_AUDIO::findFirstAvailableChannelID() {
	bool found = false;
	int foundID = 0;
	while (!found) {
		auto foundChannel = aChannels.find(foundID);
		if (foundChannel == aChannels.end()) {
			found = true;
		} else {
			foundID++;
		}
	}
	return foundID;
}

int OPI_AUDIO::findFirstAvailableDSPID() {
	bool found = false;
	int foundID = 0;
	while (!found) {
		auto foundDSP = aDSPs.find(foundID);
		if (foundDSP == aDSPs.end()) {
			found = true;
		}
		else {
			foundID++;
		}
	}
	return foundID;
}

int OPI_AUDIO::StartSound(std::string SNDPath) {
	int channelID = findFirstAvailableChannelID();

	auto foundSounds = aSounds.find(SNDPath);
	if (foundSounds == aSounds.end()) {
		std::cout << "[" + SNDPath + "] does not exist! We will attempt to load it now..." << std::endl;
		CreateSound(SNDPath);
		if (foundSounds == aSounds.end()) {
			std::cout << "Creation of sound [" + SNDPath + "] failed! As such, we must abort playing the audio." << std::endl;
			return -1;
		}
	}

	FMOD::Channel* channel = NULL;
	FMODErrorCheck(audioSystem->playSound(aSounds[SNDPath], NULL, true, &channel)); //possible popping noise if sound is modified while playing, resume it manually after modifying channel
	if (channel) { // won't be null if it works
		aChannels[channelID] = channel;
		FMODErrorCheck(channel->setPaused(false));
		return channelID;
	}

	return -1;
}

void OPI_AUDIO::DestroyChannel(int channelID) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	foundChannels->second->stop();
	aChannels.erase(channelID);
}

void OPI_AUDIO::WaitForChannel(int channelID) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	bool isPlaying = true;
	while (isPlaying) {
		isPlaying = getPlayingState(channelID);
		crossSleep(1);
	}
}

void OPI_AUDIO::PlayChannel(int channelID) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setPaused(false));
}

void OPI_AUDIO::PauseChannel(int channelID) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setPaused(true));
}

void OPI_AUDIO::MuteChannel(int channelID) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setMute(true));
}

void OPI_AUDIO::UnMuteChannel(int channelID) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setMute(true));
}

void OPI_AUDIO::SetChannelVolume(int channelID, float channelVolume) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setVolume(channelVolume));
}

void OPI_AUDIO::SetChannelPitch(int channelID, float channelPitch) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setPitch(channelPitch));
}

int OPI_AUDIO::CreatePresetDSP(FMOD_DSP_TYPE DSPType) {
	int DSPID = findFirstAvailableDSPID();

	FMOD::DSP* dsp = NULL;
	FMODErrorCheck(audioSystem->createDSPByType(DSPType, &dsp)); //possible popping noise if sound is modified while playing, resume it manually after modifying channel
	if (dsp) { // won't be null if it works
		aDSPs[DSPID] = dsp;
		return DSPID;
	}

	return -1;
}

void OPI_AUDIO::ApplyChannelDSP(int channelID, int DSPID) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	auto foundDSPs = aDSPs.find(DSPID);
	if (foundDSPs == aDSPs.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->addDSP(0, foundDSPs->second));
}

void OPI_AUDIO::DSPWetDryMix(int DSPID, float preWet, float postWet, float dry) {
	auto foundDSPs = aDSPs.find(DSPID);
	if (foundDSPs == aDSPs.end()) {
		return;
	}

	FMODErrorCheck(foundDSPs->second->setWetDryMix(preWet, postWet, dry));
}

void OPI_AUDIO::SetChannelFrequency(int channelID, float frequency) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setFrequency(frequency));
}

void OPI_AUDIO::SetChannelMode(int channelID, FMOD_MODE channelMode) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setMode(channelMode));
}

bool OPI_AUDIO::getPlayingState(int channelID) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return false;
	}

	bool isPlaying = false;
	FMODErrorCheck(foundChannels->second->isPlaying(&isPlaying));
	
	return isPlaying;
}

int OPI_AUDIO::FMODErrorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		std::cout << "FMod encountered an error: " + result << std::endl;
		return 0;
	}
	return 1;
}