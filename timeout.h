#pragma once
#include <windows.h>

//bool SaveOriginalTimeout(GUID** pActiveScheme, GUID GUID_SUBGROUP, GUID GUID_TIMEOUT, DWORD* originalTimeoutAC, DWORD* originalTimeoutDC, bool* hasSavedTimeout);
//bool SetNewTimeout(GUID* pActiveScheme, GUID GUID_SUBGROUP, GUID GUID_TIMEOUT, DWORD newTimeout);
//bool RestoreOriginalTimeout(GUID* pActiveScheme, GUID GUID_SUBGROUP, GUID GUID_TIMEOUT, DWORD originalTimeoutAC, DWORD originalTimeoutDC, bool hasSavedTimeout);

int GetNewTimeout(HWND hwnd, DWORD newTimeout);

bool SaveOldDisplayTimeout(GUID*& pActiveScheme, DWORD& originalTimeoutAC, DWORD& originalTimeoutDC, bool& pHasSavedTimeout);
bool SetNewDisplayTimeout(GUID* pActiveScheme, DWORD newTimeout);
bool RestoreDisplayTimeout(GUID* pActiveScheme, DWORD originalTimeoutAC, DWORD originalTimeoutDC, bool& pHasSavedTimeout);

bool SaveOldSleepTimeout(GUID*& pActiveScheme, DWORD& originalTimeoutAC, DWORD& originalTimeoutDC, bool& pHasSavedTimeout);
bool SetNewSleepTimeout(GUID* pActiveScheme, DWORD newTimeout);
bool RestoreSleepTimeout(GUID * pActiveScheme, DWORD originalTimeoutAC, DWORD originalTimeoutDC, bool& pHasSavedTimeout);