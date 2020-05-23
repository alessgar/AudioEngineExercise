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
		FMODErrorCheck(foundChannels->second->isPlaying(&isPlaying));
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

void OPI_AUDIO::SetChannelVolume(int channelID, float channelVolume) {
	auto foundChannels = aChannels.find(channelID);
	if (foundChannels == aChannels.end()) {
		return;
	}

	FMODErrorCheck(foundChannels->second->setVolume(channelVolume));
}

int OPI_AUDIO::FMODErrorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		std::cout << "FMod encountered an error: " + result << std::endl;
		return 0;
	}
	return 1;
}