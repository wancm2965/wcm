#ifndef __VIDECYNCWRAPPER_H__
#define __VIDECYNCWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

#include "../VIDECYNC/VIDECYNCAPI.h"

int SyncSupportPlayer(void);

int SyncInit(void);
void SyncTerminate(void);

void SyncPlayerSetMaxCount(unsigned int nMaxPlayer);
int SyncPlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight);

void*SyncPlayerOpen(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWnd, int nWidth, int nHeight);
void SyncPlayerClose(void*hPlayer);

int SyncPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int&nFlagReset);
int SyncPlayerGetWidth(void*hPlayer);
int SyncPlayerGetHeight(void*hPlayer);
int SyncPlayerResize(void*hPlayer);
int SyncPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay);
int SyncPlayerGetFrameRate(void*hPlayer);
int SyncPlayerGetShowType(void*hPlayer);
bool SyncPlayerCaptureToBMPFile(void*hPlayer,const char*cszFile);

class VIDECYNCWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	VIDECYNCWrapper(HINSTANCE hInstance);
	~VIDECYNCWrapper(void);
public:
	static VIDECYNCWrapper&Instance(void);
	bool IsOK(void);

	int (*pVIDECYNC_Init)(unsigned long nKey);

	void (*pVIDECYNC_PlayerSetMaxCount)(unsigned int nMaxPlayer);

	int (*pVIDECYNC_PlayerTry)(void*hPlayer,unsigned long nWidth,unsigned long nHeight);

	void*(*pVIDECYNC_PlayerOpen)(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWnd, int nWidth, int nHeight);
	void (*pVIDECYNC_PlayerClose)(void*hPlayer);

	int (*pVIDECYNC_PlayerPlayFrame)(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int&nFlagReset);
	int (*pVIDECYNC_PlayerGetWidth)(void*hPlayer);
	int (*pVIDECYNC_PlayerGetHeight)(void*hPlayer);
	int (*pVIDECYNC_PlayerResize)(void*hPlayer);
	int (*pVIDECYNC_PlayerSkipDisplay)(void*hPlayer,int nFlagSkipDisplay);
	int (*pVIDECYNC_PlayerGetFrameRate)(void*hPlayer);
	int (*pVIDECYNC_PlayerGetShowType)(void*hPlayer);
	bool (*pVIDECYNC_PlayerCaptureToBMPFile)(void*hPlayer,const char*cszFile);
};


int YNCD3DInit(void);
void YNCD3DTerminate(void);
void*YNCD3DPlayerOpen(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWnd,int nWidth,int nHeight);
void YNCD3DPlayerClose(void*hPlayer);
int  YNCD3DPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int nWidth,int nHeight);
void YNCD3DPlayerReDraw(void*hPlayer);
int  YNCD3DPlayerGetWidth(void*hPlayer);
int  YNCD3DPlayerGetHeight(void*hPlayer);
int  YNCD3DPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay);
int  YNCD3DPlayerGetFrameRate(void*hPlayer);
int  YNCD3DPlayerGetShowType(void*hPlayer);

class VIDECYNCD3DWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	VIDECYNCD3DWrapper(HINSTANCE hInstance);
	~VIDECYNCD3DWrapper(void);
public:
	static VIDECYNCD3DWrapper&Instance(void);
	bool IsOK(void);
	void*(*pVIDECYNC_D3DPlayerOpen)(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWnd,int nWidth,int nHeight,bool bCreateWnd);
	void (*pVIDECYNC_D3DPlayerClose)(void*hPlayer);
	int  (*pVIDECYNC_D3DPlayerPlayFrame)(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int nWidth,int nHeight);
	void*(*pVIDECYNC_D3DPlayerRedraw)(void*hPlayer);
	int  (*pVIDECYNC_D3DPlayerGetWidth)(void*hPlayer);
	int  (*pVIDECYNC_D3DPlayerGetHeight)(void*hPlayer);
	int  (*pVIDECYNC_D3DPlayerSkipDisplay)(void*hPlayer,int nFlagSkipDisplay);
	int  (*pVIDECYNC_D3DPlayerGetFrameRate)(void*hPlayer);
	int  (*pVIDECYNC_D3DPlayerGetShowType)(void*hPlayer);
};


#endif
