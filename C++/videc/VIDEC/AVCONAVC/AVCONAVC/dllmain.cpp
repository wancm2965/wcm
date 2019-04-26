// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <malloc.h>
#include <crtdbg.h>


extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);


//extern "C" int pthread_win32_process_attach_np(void);
//extern "C" int pthread_win32_process_detach_np(void);
//extern "C" int pthread_win32_thread_attach_np(void);
//extern "C" int pthread_win32_thread_detach_np(void);

HMODULE g_hAVCONAVCInstance;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hAVCONAVCInstance = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//pthread_win32_process_attach_np();
		break;
	case DLL_THREAD_ATTACH:
		//pthread_win32_thread_attach_np();
		//_CrtSetBreakAlloc(492);
		break;
	case DLL_THREAD_DETACH:
		//pthread_win32_thread_detach_np();
	case DLL_PROCESS_DETACH:
		//pthread_win32_process_detach_np();
		break;
	}
	return DllEntryPoint(reinterpret_cast<HINSTANCE>(hModule), ul_reason_for_call, lpReserved);
}

