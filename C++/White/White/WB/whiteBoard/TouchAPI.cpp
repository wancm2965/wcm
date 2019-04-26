#include <windows.h>
#include "TouchAPI.h"
#include <tpcshrd.h>
#include <tchar.h>

BOOL CEncapsulateTouchAPI::U_GetTouchInputInfo(const unsigned long &hTouchInput, 
	UINT cInputs, PTOUCHINPUT pInputs, int cbSize)
{
	static HMODULE hUserDll = ::LoadLibrary(TEXT("user32.dll"));
	//ASSERT(hUserDll != NULL);

	typedef	BOOL (__stdcall *PFNUNTOUCHWINDOW)(HTOUCHINPUT, UINT, PTOUCHINPUT, int);
	static PFNUNTOUCHWINDOW pfGetTouchInputInfo = (PFNUNTOUCHWINDOW)GetProcAddress(hUserDll, "GetTouchInputInfo");

	if (pfGetTouchInputInfo == NULL)
	{
		return FALSE;
	}

	return (*pfGetTouchInputInfo)((HTOUCHINPUT)hTouchInput, cInputs, pInputs, cbSize);
}

BOOL CEncapsulateTouchAPI::U_CloseTouchInputHandle(const unsigned long &hTouchInput)
{

	static HMODULE hUserDll = ::LoadLibrary(TEXT("user32.dll"));
	//ASSERT(hUserDll != NULL);

	typedef	BOOL (__stdcall *PFNUNTOUCHWINDOW)(HTOUCHINPUT);
	static PFNUNTOUCHWINDOW pfCloseTouchInputHandle = (PFNUNTOUCHWINDOW)GetProcAddress(hUserDll, "CloseTouchInputHandle");

	if (pfCloseTouchInputHandle == NULL)
	{
		return FALSE;
	}
	return (*pfCloseTouchInputHandle)((HTOUCHINPUT)hTouchInput);

}

BOOL CEncapsulateTouchAPI::U_RegisterTouchWindow(HWND hwnd, ULONG ulFlags)
{

	static HMODULE hUserDll = ::LoadLibrary(TEXT("user32.dll"));

	typedef	BOOL (__stdcall *PFNUNTOUCHWINDOW)(HWND, ULONG);
	static PFNUNTOUCHWINDOW pfRegister = (PFNUNTOUCHWINDOW)GetProcAddress(hUserDll, "RegisterTouchWindow");

	if (pfRegister == NULL)
	{
		return FALSE;
	}

	return (*pfRegister)(hwnd, ulFlags);

}

BOOL CEncapsulateTouchAPI::U_UnRegisterTouchWindow(HWND hWnd)
{
	static HMODULE hUserDll = ::LoadLibrary(TEXT("user32.dll"));

	typedef	BOOL (__stdcall *PFNUNTOUCHWINDOW)(HWND);
	static PFNUNTOUCHWINDOW pfUnregister = (PFNUNTOUCHWINDOW)GetProcAddress(hUserDll, "UnregisterTouchWindow");

	if (pfUnregister == NULL)
	{
		return FALSE;
	}

	return (*pfUnregister)(hWnd);
}

BOOL CEncapsulateTouchAPI::U_IsTouchWindow( HWND hWnd )
{
	static HMODULE hUserDll = ::LoadLibrary(TEXT("user32.dll"));
	//ASSERT(hUserDll != NULL);

	typedef	BOOL (__stdcall *PFNUNTOUCHWINDOW)(HWND, PULONG);
	static PFNUNTOUCHWINDOW pfUnregister = (PFNUNTOUCHWINDOW)GetProcAddress(hUserDll, "IsTouchWindow");

	if (pfUnregister == NULL)
	{
		return FALSE;
	}
	ULONG pulFlags;
	return (*pfUnregister)(hWnd, &pulFlags);
}


//copy from 标准
void CEncapsulateTouchAPI::DisableTheTabFeedback(HWND hWnd)
{
	static bool bTouchTested = false;
	static bool bTouchSupport = true;
	if (!bTouchTested)
	{
		int value = GetSystemMetrics(SM_DIGITIZER);
		BOOL bMTouchSupport = TRUE; //是否支持多点触摸功能

		if ( 0 == (value & (0x80 | 0x40)) )
		{
			bTouchSupport = FALSE;
		}
		bTouchTested = true;//test only once.
	}

	if (bTouchSupport)
	{
		DWORD_PTR dwHwndTabletProperty = 
			//TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
			TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
			TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
			TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)

		//如下这些API函数要求的系统版本低
		ATOM atom = GlobalAddAtom(MICROSOFT_TABLETPENSERVICE_PROPERTY);  
		SetProp(hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
		GlobalDeleteAtom(atom);
	}
}
