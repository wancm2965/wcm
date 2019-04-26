//LoadHex.h
#ifndef __LOADHEX_H__
#define __LOADHEX_H__

extern HINSTANCE g_hInstance;
bool LoadHex(HMODULE hMod, WORD ResourceID,PBYTE*ppData,DWORD&dwLen);
bool LoadResourceData(HMODULE hMod, WORD ResourceID,const char*cszType,PBYTE*ppData,DWORD&dwLen);


#endif