#include "sleeptimer.h"
#include "resource.h"
#include "customDialog.h"

#include <cstdio>
#include <windows.h>
#include <ctime>


int GetCustomTime(HWND hwnd, time_t sleepAfterMinutes)
{
    INT_PTR result = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SET_TIMER),
        hwnd, TimeDialogProc, (LPARAM)sleepAfterMinutes);

    // Result should be between 0-1440, safe to cast
    return (int)result;
}

void StartSleepTimer(time_t sleepAfterMinutes, bool& sleepTimerActive, time_t& sleepTargetTime, HWND hwnd)
{
    if (sleepAfterMinutes <= 0) return;

    sleepTargetTime = time(NULL) + (sleepAfterMinutes *60);
    sleepTimerActive = true;

    // set timer interval (ms)
    SetTimer(hwnd, 1, 10000, NULL);
}