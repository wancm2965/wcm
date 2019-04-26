/********************************************************************
*	Copyright (C) 2008 - BlueStar. All Rights Reserved
*
*	Date Created:2008-01-01
*	Filename	:DVRConfig.h
*	Author		:BlueStar luoyanhua
*	Description	:BlueStar DVRClient Full Config with GUI Interface
*********************************************************************/
#ifndef __BLUESTAR_DVRCONFIG_INTERFACE_H__
#define __BLUESTAR_DVRCONFIG_INTERFACE_H__

#include <windows.h>

#ifdef __cplusplus
#define BSRCONFIG_API extern "C"
#else
#define BSRCONFIG_API
#endif

#ifndef BSRCONFIG_STDCALL
#define BSRCONFIG_STDCALL __stdcall
#endif

//语言代码页
typedef enum
{
	BSRCONFIG_AUTO	= 0,	//自动模式,根据系统语言自动选择,非中文系统则显示英文
	BSRCONFIG_CHINESE,		//强制中文模式
	BSRCONFIG_ENGLISH,		//强制英文模式
	BSRCONFIG_INVALID,		//无效值
}BSRCONFIG_LANGUAGEID;

//配置模块端口信息
typedef struct tagBSRCONFIG_PORTINFO
{
	WORD wPortScene;		//现场传输端口
	WORD wPortDown;			//远程下载端口
	WORD wPortVoice;		//语音对讲端口
	WORD wPortCommand;		//命令通信端口
}BSRCONFIG_PORTINFO, *PBSRCONFIG_PORTINFO;


////////////////////////////API//////////////////////////////////////////////
BSRCONFIG_API BOOL BSRCONFIG_STDCALL BSR_CONFIG_DoConfigByUserHandle(HANDLE hUserHandle, HWND hParentWnd, INT nActIdx, LONG nLangID, BOOL bTopMost, BSRCONFIG_PORTINFO *lpPortsInfo, LONG nRes = 0, unsigned __int64 right = -1);
BSRCONFIG_API BOOL BSRCONFIG_STDCALL BSR_CONFIG_DoConfigByRealHandle(HANDLE hRealHandle, HWND hParentWnd, INT nActIdx, LONG nLangID, BOOL bTopMost, BSRCONFIG_PORTINFO *lpPortsInfo, LONG nRes = 0, unsigned __int64 right = -1);
BSRCONFIG_API BOOL BSRCONFIG_STDCALL BSR_CONFIG_DoConfig(LPCSTR lpDVRIP, WORD wDVRPort, LPCSTR lpUserName, LPCSTR lpPasswd, INT nActIdx, LONG nLangID, BOOL bTopMost, BSRCONFIG_PORTINFO *lpPortsInfo, LONG nRes = 0, unsigned __int64 right = -1 );
////////////////////////////API//////////////////////////////////////////////

#endif	//__BLUESTAR_DVRCONFIG_INTERFACE_H__