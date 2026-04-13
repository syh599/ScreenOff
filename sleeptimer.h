#pragma once
#include <ctime>
#include <windows.h>

int GetCustomTime();
void StartSleepTimer(int sleepAfterMinutes, bool& sleepTimerActive, time_t& sleepTargetTime, HWND hwnd);