//HPAvconScreen.h
#ifndef __HPAVCONSCREEN_H__
#define __HPAVCONSCREEN_H__

#include <windows.h>

#define AVCON_SCREEN_EXPORTS
#ifdef AVCON_SCREEN_EXPORTS
#define AVCON_SCREEN_API __declspec(dllexport)
#else
#define AVCON_SCREEN_API __declspec(dllimport)
#endif

class AVCON_SCREEN_API AvconScreen 
{
public:
	static IMXEC_ScreenRX* CreateRX(IMXEC_ScreenRXCallback&rCallback);
	static IMXEC_ScreenTX* CreateTX(IMXEC_ScreenTXCallback&rCallback);
	static void SetRemoteViewOnly(int nViewOnly);
};

AVCON_SCREEN_API int TranslateScreenClientMsg(MSG *pMsg);

#endif