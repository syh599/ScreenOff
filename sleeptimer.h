#pragma once
#include <ctime>
#include <windows.h>

int GetCustomTime(HWND hwnd, time_t sleepAfterMinutes);
void StartSleepTimer(time_t sleepAfterMinutes, bool& sleepTimerActive, time_t& sleepTargetTime, HWND hwnd);