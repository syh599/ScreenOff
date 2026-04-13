#include "sleeptimer.h"

#include <cstdio>
#include <windows.h>
#include <ctime>

int GetCustomTime()
{
    // Allocate a console
    AllocConsole();

    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONIN$", "r", stdin);

    int minutes = 0;
    int result;

    do {
        printf("Enter minutes until sleep (1-1440): ");
        result = scanf_s("%d", &minutes);

        // Clear input buffer
        while (getchar() != '\n');

        if (result != 1 || minutes <= 0 || minutes > 1440)
        {
            printf("Invalid input! Please enter an integer in [1, 1440].\n\n");
            minutes = 0;
        }
        else break;

    } while (true);

    // Clean up
    fclose(stdout);
    fclose(stdin);
    FreeConsole();

    return minutes;
}

void StartSleepTimer(int sleepAfterMinutes, bool& sleepTimerActive, time_t& sleepTargetTime, HWND hwnd)
{
    if (sleepAfterMinutes <= 0) return;

    sleepTargetTime = time(NULL) + (sleepAfterMinutes *60);
    sleepTimerActive = true;

    // set timer interval (ms)
    SetTimer(hwnd, 1, 10000, NULL);
}