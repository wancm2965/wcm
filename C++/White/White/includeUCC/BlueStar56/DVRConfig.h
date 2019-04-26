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

//���Դ���ҳ
typedef enum
{
	BSRCONFIG_AUTO	= 0,	//�Զ�ģʽ,����ϵͳ�����Զ�ѡ��,������ϵͳ����ʾӢ��
	BSRCONFIG_CHINESE,		//ǿ������ģʽ
	BSRCONFIG_ENGLISH,		//ǿ��Ӣ��ģʽ
	BSRCONFIG_INVALID,		//��Чֵ
}BSRCONFIG_LANGUAGEID;

//����ģ��˿���Ϣ
typedef struct tagBSRCONFIG_PORTINFO
{
	WORD wPortScene;		//�ֳ�����˿�
	WORD wPortDown;			//Զ�����ض˿�
	WORD wPortVoice;		//�����Խ��˿�
	WORD wPortCommand;		//����ͨ�Ŷ˿�
}BSRCONFIG_PORTINFO, *PBSRCONFIG_PORTINFO;


////////////////////////////API//////////////////////////////////////////////
BSRCONFIG_API BOOL BSRCONFIG_STDCALL BSR_CONFIG_DoConfigByUserHandle(HANDLE hUserHandle, HWND hParentWnd, INT nActIdx, LONG nLangID, BOOL bTopMost, BSRCONFIG_PORTINFO *lpPortsInfo, LONG nRes = 0, unsigned __int64 right = -1);
BSRCONFIG_API BOOL BSRCONFIG_STDCALL BSR_CONFIG_DoConfigByRealHandle(HANDLE hRealHandle, HWND hParentWnd, INT nActIdx, LONG nLangID, BOOL bTopMost, BSRCONFIG_PORTINFO *lpPortsInfo, LONG nRes = 0, unsigned __int64 right = -1);
BSRCONFIG_API BOOL BSRCONFIG_STDCALL BSR_CONFIG_DoConfig(LPCSTR lpDVRIP, WORD wDVRPort, LPCSTR lpUserName, LPCSTR lpPasswd, INT nActIdx, LONG nLangID, BOOL bTopMost, BSRCONFIG_PORTINFO *lpPortsInfo, LONG nRes = 0, unsigned __int64 right = -1 );
////////////////////////////API//////////////////////////////////////////////

#endif	//__BLUESTAR_DVRCONFIG_INTERFACE_H__