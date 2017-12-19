// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "FFmpegrecord.h"
#include "AVFileRecord.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

IFFmpegrecord* IFFmpegrecord::Create()
{
	IFFmpegrecord* pFfmpeg = NULL;
#if USE_FFMPEGRECORD
	pFfmpeg = new CFFmpegrecord();
#endif
#if USE_AVFILERECORD
	pFfmpeg = new CAVFileRecord();
#endif
	
	return pFfmpeg;
}