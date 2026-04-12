#include "timeout.h"
#include <windows.h>
#include <powrprof.h>


bool SaveOriginalTimeout(GUID** ppActiveScheme, DWORD* originalTimeout, bool* pHasSavedTimeout)
{
    // active power plan
    if (PowerGetActiveScheme(NULL, ppActiveScheme) != ERROR_SUCCESS)
        return 0;  // Failed

    // read current display timeout value
    if (PowerReadACValueIndex(NULL, *ppActiveScheme, &GUID_DISPLAY_SUBGROUP,
        &GUID_DISPLAY_TIMEOUT, originalTimeout) != ERROR_SUCCESS)
        return 0;

    *pHasSavedTimeout = true;
    return 1;
}

bool SetNewTimeout(GUID* pActiveScheme)
{
    DWORD newTimeout = 1; // in seconds

    if (PowerWriteACValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP,
        &GUID_DISPLAY_TIMEOUT, newTimeout) != ERROR_SUCCESS)
        return 0;

    PowerSetActiveScheme(NULL, pActiveScheme);


    return 1;
}

bool RestoreOriginalTimeout(GUID* pActiveScheme, DWORD originalTimeout, bool hasSavedTimeout)
{
    if (!hasSavedTimeout) return 0;

    if (PowerWriteACValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP,
        &GUID_DISPLAY_TIMEOUT, originalTimeout) != ERROR_SUCCESS)
        return 0;

    PowerSetActiveScheme(NULL, pActiveScheme);

    return 1;
}