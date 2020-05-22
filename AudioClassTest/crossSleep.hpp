#pragma once
#ifdef __linux__
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

// Freezes Thread, but works for both platforms
/*
_WIN32 Defined as 1 when the compilation target is 32-bit ARM, 64-bit ARM, x86, or x64. Otherwise, undefined.
_WIN64 Defined as 1 when the compilation target is 64-bit ARM or x64. Otherwise, undefined.
*/

void crossSleep(int sleepMs);