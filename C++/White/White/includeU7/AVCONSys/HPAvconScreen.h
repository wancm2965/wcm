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

class IMXEC_ScreenRXEx : public IMXEC_ScreenRX
{
public:
	virtual HWND StartEx(const char*cszPeerUserName,const char*cszPeerNodeID,unsigned long nScreenID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,const char*cszPeerMCUType)=0;
	virtual HWND StartEx(const char* Ip, int& error, HWND hwnd, RECT rect) = 0;
};

class AVCON_SCREEN_API AvconScreen 
{
public:
	static IMXEC_ScreenRXEx* CreateRX(IMXEC_ScreenRXCallback&rCallback);
	static IMXEC_ScreenTX* CreateTX(IMXEC_ScreenTXCallback&rCallback);
	static void SetRemoteViewOnly(int nViewOnly);
};

AVCON_SCREEN_API int TranslateScreenClientMsg(MSG *pMsg);

#endif