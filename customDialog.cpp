#include "customDialog.h"
#include "resource.h"

#include <windows.h>
#include <cstdio>

INT_PTR CALLBACK TimeDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        int sleepAfterMinutes = (int)lParam;

        SetDlgItemInt(hDlg, IDC_CUSTOM_TIME, sleepAfterMinutes, FALSE);
        SetFocus(GetDlgItem(hDlg, IDC_CUSTOM_TIME));
        SendDlgItemMessage(hDlg, IDC_CUSTOM_TIME, EM_SETSEL, 0, -1);

        POINT pt;
        GetCursorPos(&pt);

        RECT rcDlg;
        GetWindowRect(hDlg, &rcDlg);
        int dlgWidth = rcDlg.right - rcDlg.left;
        int dlgHeight = rcDlg.bottom - rcDlg.top;

        int x = pt.x - dlgWidth / 2;
        int y = pt.y - dlgHeight - 5;

        /*
        //Debug
        char debug[256];
        sprintf_s(debug, "Cursor: %d, %d\nDialog size: %d x %d\nCalculated: %d, %d",
            pt.x, pt.y, dlgWidth, dlgHeight, x, y);
        MessageBoxA(NULL, debug, "Debug", MB_OK);
        */

        SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        return FALSE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            BOOL success;
            int minutes = GetDlgItemInt(hDlg, IDC_CUSTOM_TIME, &success, FALSE);
            if (success && minutes >= 1 && minutes <= 1440)
            {
                EndDialog(hDlg, minutes);
            }
            else
            {
                MessageBoxA(hDlg, "Please enter 1-1440", "Invalid", MB_OK);
            }
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, -1);
            return TRUE;
        }
        return TRUE;
    }
    return FALSE;
}