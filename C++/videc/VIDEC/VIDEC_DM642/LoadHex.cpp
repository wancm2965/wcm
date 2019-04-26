//#include "stdafx.h"
#include <windows.h>
#include "LoadHex.h"



bool LoadHex(HMODULE hMod, WORD ResourceID,PBYTE*ppData,DWORD&dwLen)
{
	HRSRC hFound = FindResource(hMod, MAKEINTRESOURCE(ResourceID), "HEX");
	if (hFound!=NULL)
	{
		HGLOBAL hRes = LoadResource(hMod, hFound);
		if (hRes!=NULL)
		{
			PBYTE pbMemData = (BYTE *) LockResource(hRes);
			DWORD dwMemLength = SizeofResource(hMod, hFound);

			*ppData=pbMemData;
			dwLen=dwMemLength;

			FreeResource(hRes);

			return true;
		}
	}

	return false;
}

bool LoadResourceData(HMODULE hMod, WORD ResourceID,const char*cszType,PBYTE*ppData,DWORD&dwLen)
{
	HRSRC hFound = FindResource(hMod, MAKEINTRESOURCE(ResourceID), cszType);
	if (hFound!=NULL)
	{
		HGLOBAL hRes = LoadResource(hMod, hFound);
		if (hRes!=NULL)
		{
			PBYTE pbMemData = (BYTE *) LockResource(hRes);
			DWORD dwMemLength = SizeofResource(hMod, hFound);

			*ppData=pbMemData;
			dwLen=dwMemLength;

			FreeResource(hRes);

			return true;
		}
	}

	return false;
}
