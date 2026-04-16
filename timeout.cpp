#include "timeout.h"
#include "resource.h"
#include "customDialog.h"

#include <windows.h>
#include <powrprof.h>


bool SaveOriginalTimeout(GUID** ppActiveScheme, DWORD* originalTimeoutAC, DWORD* originalTimeoutDC, bool* pHasSavedTimeout)
{
    // active power plan
    if (PowerGetActiveScheme(NULL, ppActiveScheme) != ERROR_SUCCESS)
        return 0;  // Failed

    // read current display timeout value
    if ((PowerReadACValueIndex(NULL, *ppActiveScheme, &GUID_DISPLAY_SUBGROUP, &GUID_DISPLAY_TIMEOUT, originalTimeoutAC) != ERROR_SUCCESS) ||
        (PowerReadDCValueIndex(NULL, *ppActiveScheme, &GUID_DISPLAY_SUBGROUP, &GUID_DISPLAY_TIMEOUT, originalTimeoutDC) != ERROR_SUCCESS))
        return 0;

    *pHasSavedTimeout = true;
    return 1;
}

bool SetNewTimeout(GUID* pActiveScheme, DWORD newTimeout)
{

    if ((PowerWriteACValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP, &GUID_DISPLAY_TIMEOUT, newTimeout) != ERROR_SUCCESS) ||
        (PowerWriteDCValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP, &GUID_DISPLAY_TIMEOUT, newTimeout) != ERROR_SUCCESS))
        return 0;

    PowerSetActiveScheme(NULL, pActiveScheme);


    return 1;
}

bool RestoreOriginalTimeout(GUID* pActiveScheme, DWORD originalTimeoutAC, DWORD originalTimeoutDC, bool hasSavedTimeout)
{
    if (!hasSavedTimeout) return 0;

    if ((PowerWriteACValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP, &GUID_DISPLAY_TIMEOUT, originalTimeoutAC) != ERROR_SUCCESS) ||
        (PowerWriteDCValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP, &GUID_DISPLAY_TIMEOUT, originalTimeoutDC) != ERROR_SUCCESS))
        return 0;

    PowerSetActiveScheme(NULL, pActiveScheme);

    return 1;
}

int GetNewTimeout(HWND hwnd, DWORD newTimeout)
{
    INT_PTR result = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SET_TIMEOUT),
        hwnd, TimeDialogProc, (LPARAM)newTimeout);

    return (DWORD)result;
}