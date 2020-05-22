#include "crossSleep.hpp"

void crossSleep(int sleepMs) {
	#ifdef __linux__
		usleep(sleepMs * 1000);   // usleep takes sleep time in us (1 millionth of a second)
	#endif
	#ifdef _WIN32
		Sleep(sleepMs);
	#endif
}