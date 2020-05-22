#include <iostream>
#include "AudioEngine.hpp"
#include "boost/filesystem.hpp"

/*
[NOTE FOR ME]

Things that are platform specific (would have to be rebuilt for other OSes):
Boost Filesystem Library (there are precompiled binaries for windows, but linux binaries must be generated manually)
FMod Core (as simple as downloading the other platform's installer from the API download page)
*/

int main()
{
	std::string binPath = boost::filesystem::initial_path().string();
	std::string basePath = binPath.substr(0,binPath.rfind("bin"));
	
	std::cout << "Creating and Initializing Audio Engine..." << std::endl;
	OPI_AUDIO* AudioEngine = new OPI_AUDIO();

	std::cout << "Loading Test Sound..." << std::endl;
	AudioEngine->CreateSound(basePath+"sounds/chungus2.mp3");

	std::cout << "Playing Test Sound..." << std::endl;
	int channel = AudioEngine->StartSound(basePath + "sounds/chungus2.mp3");

	std::cout << "Waiting Until Sound Completion..." << std::endl;
	AudioEngine->WaitForChannel(channel);

	std::cout << "Destroying Test Sound..." << std::endl;
	AudioEngine->DestroySound(basePath + "sounds/chungus2.mp3");

	std::cout << "Destroying Audio Engine..." << std::endl;
	AudioEngine->Destroy();

	std::cout << "Audio Test Complete!" << std::endl;
	return 0;
}