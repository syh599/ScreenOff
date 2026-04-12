#pragma once
#include <windows.h>

static const GUID GUID_DISPLAY_SUBGROUP =
{ 0x7516b95f, 0xf776, 0x4464, 0x8c, 0x53, 0x06, 0x16, 0x7f, 0x40, 0xcc, 0x99 };

static const GUID GUID_DISPLAY_TIMEOUT =
{ 0x3c0bc021, 0xc8a8, 0x4e07, 0xa9, 0x73, 0x6b, 0x14, 0xcb, 0xcb, 0x2b, 0x7e };

bool SaveOriginalTimeout(GUID** pActiveScheme, DWORD* originalTimeout, bool* hasSavedTimeout);
bool SetNewTimeout(GUID* pActiveScheme);
bool RestoreOriginalTimeout(GUID* pActiveScheme, DWORD originalTimeout, bool hasSavedTimeout);