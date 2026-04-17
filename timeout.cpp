#include "timeout.h"
#include "resource.h"
#include "customDialog.h"

#include <windows.h>
#include <powrprof.h>

//display
static const GUID GUID_DISPLAY_SUBGROUP =
{ 0x7516b95f, 0xf776, 0x4464, 0x8c, 0x53, 0x06, 0x16, 0x7f, 0x40, 0xcc, 0x99 };
//display timeout
static const GUID GUID_DISPLAY_TIMEOUT =
{ 0x3c0bc021, 0xc8a8, 0x4e07, 0xa9, 0x73, 0x6b, 0x14, 0xcb, 0xcb, 0x2b, 0x7e };

/*
//sleep
static const GUID GUID_SLEEP_SUBGROUP =
{ 0x238C9FA8, 0x0AAD, 0x41ED, 0x83, 0x66, 0xE4, 0x2D, 0xA0, 0x19, 0xC2, 0x9D };
already defined in windows.h
*/

//sleep after
static const GUID GUID_SLEEP_TIMEOUT =
{ 0x29F6C1DB, 0x86DA, 0x48C5, 0x9F, 0xDB, 0xF2, 0xB6, 0x7B, 0x1F, 0x44, 0xDA };


bool SaveOriginalTimeout(GUID** ppActiveScheme, GUID GUID_SUBGROUP, GUID GUID_TIMEOUT, DWORD* originalTimeoutAC, DWORD* originalTimeoutDC, bool* pHasSavedTimeout)
{
    // active power plan
    if (PowerGetActiveScheme(NULL, ppActiveScheme) != ERROR_SUCCESS)
        return 0;  // Failed

    // read current display timeout value
    if ((PowerReadACValueIndex(NULL, *ppActiveScheme, &GUID_SUBGROUP, &GUID_TIMEOUT, originalTimeoutAC) != ERROR_SUCCESS) ||
        (PowerReadDCValueIndex(NULL, *ppActiveScheme, &GUID_SUBGROUP, &GUID_TIMEOUT, originalTimeoutDC) != ERROR_SUCCESS))
        return 0; // on error

    *pHasSavedTimeout = true;
    return 1;
}

bool SetNewTimeout(GUID* pActiveScheme, GUID GUID_SUBGROUP, GUID GUID_TIMEOUT, DWORD newTimeout)
{

    if ((PowerWriteACValueIndex(NULL, pActiveScheme, &GUID_SUBGROUP, &GUID_TIMEOUT, newTimeout) != ERROR_SUCCESS) ||
        (PowerWriteDCValueIndex(NULL, pActiveScheme, &GUID_SUBGROUP, &GUID_TIMEOUT, newTimeout) != ERROR_SUCCESS))
        return 0;

    PowerSetActiveScheme(NULL, pActiveScheme);


    return 1;
}

bool RestoreOriginalTimeout(GUID* pActiveScheme, GUID GUID_SUBGROUP, GUID GUID_TIMEOUT, DWORD originalTimeoutAC, DWORD originalTimeoutDC, bool hasSavedTimeout)
{
    if (!hasSavedTimeout) return 0;

    if (PowerWriteACValueIndex(NULL, pActiveScheme, &GUID_SUBGROUP, &GUID_TIMEOUT, originalTimeoutAC) != ERROR_SUCCESS) return 0;
    if (PowerWriteDCValueIndex(NULL, pActiveScheme, &GUID_SUBGROUP, &GUID_TIMEOUT, originalTimeoutDC) != ERROR_SUCCESS) return 0;

    PowerSetActiveScheme(NULL, pActiveScheme);

    return 1;
}

//helpers
//display
bool SaveOldDisplayTimeout(GUID*& pActiveScheme, DWORD& originalTimeoutAC, DWORD& originalTimeoutDC, bool& pHasSavedTimeout) {
    return SaveOriginalTimeout(&pActiveScheme, GUID_DISPLAY_SUBGROUP, GUID_DISPLAY_TIMEOUT, &originalTimeoutAC, &originalTimeoutDC, &pHasSavedTimeout);
}

bool SetNewDisplayTimeout(GUID* pActiveScheme, DWORD newTimeout) {
    return SetNewTimeout(pActiveScheme, GUID_DISPLAY_SUBGROUP, GUID_DISPLAY_TIMEOUT, newTimeout);
}

bool RestoreDisplayTimeout(GUID* pActiveScheme, DWORD originalTimeoutAC, DWORD originalTimeoutDC, bool& pHasSavedTimeout) {
    return RestoreOriginalTimeout(pActiveScheme, GUID_DISPLAY_SUBGROUP, GUID_DISPLAY_TIMEOUT, originalTimeoutAC, originalTimeoutDC, pHasSavedTimeout);
}
//sleep
bool SaveOldSleepTimeout(GUID*& pActiveScheme, DWORD& originalTimeoutAC, DWORD& originalTimeoutDC, bool& pHasSavedTimeout) {
    return SaveOriginalTimeout(&pActiveScheme, GUID_SLEEP_SUBGROUP, GUID_SLEEP_TIMEOUT, &originalTimeoutAC, &originalTimeoutDC, &pHasSavedTimeout);
}

bool SetNewSleepTimeout(GUID* pActiveScheme, DWORD newTimeout) {
    return SetNewTimeout(pActiveScheme, GUID_SLEEP_SUBGROUP, GUID_SLEEP_TIMEOUT, newTimeout);
}

bool RestoreSleepTimeout(GUID* pActiveScheme, DWORD originalTimeoutAC, DWORD originalTimeoutDC, bool& pHasSavedTimeout) {
    return RestoreOriginalTimeout(pActiveScheme, GUID_SLEEP_SUBGROUP, GUID_SLEEP_TIMEOUT, originalTimeoutAC, originalTimeoutDC, pHasSavedTimeout);
}

//custom dialog
int GetNewTimeout(HWND hwnd, DWORD newTimeout)
{
    INT_PTR result = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SET_TIMEOUT),
        hwnd, TimeDialogProc, (LPARAM)newTimeout);

    return (DWORD)result;
}