#pragma once
#include <ctime>
#include <windows.h>

INT_PTR CALLBACK TimeDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam, time_t sleepAfterMinutes);
int GetCustomTime(HWND hwnd, time_t sleepAfterMinutes);
void StartSleepTimer(time_t sleepAfterMinutes, bool& sleepTimerActive, time_t& sleepTargetTime, HWND hwnd);