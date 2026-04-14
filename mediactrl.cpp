#include "mediactrl.h" //唱跳rap篮球！

#include <windows.h>

void PressMediaPlayPause() {
    // Send to Windows System Media Transport
    HWND hwnd = FindWindow(L"Windows.UI.Core.CoreWindow", NULL);
    if (!hwnd) {
        // Fallback to Shell
        hwnd = FindWindow(L"Progman", NULL);
    }

    // These are the actual AVRCP commands
    // Play/Pause
    if (hwnd) {
        SendMessage(hwnd, WM_APPCOMMAND, 0,
            MAKELPARAM(0, APPCOMMAND_MEDIA_PLAY_PAUSE));
    }
}