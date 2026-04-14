#pragma once
#include <ctime>
#include <windows.h>

int GetCustomTime();
void StartSleepTimer(time_t sleepAfterMinutes, bool& sleepTimerActive, time_t& sleepTargetTime, HWND hwnd);