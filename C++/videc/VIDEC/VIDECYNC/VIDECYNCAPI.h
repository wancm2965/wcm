//VIDECYNCAPI.h
#ifndef __VIDECYNCAPI_H__
#define __VIDECYNCAPI_H__

#ifdef VIDECYNC_EXPORT
#define VIDECYNC_API _declspec(dllexport)
#elif VIDECYNC_DLL
#define VIDECYNC_API _declspec(dllimport)
#else
#define VIDECYNC_API
#endif

typedef void (*YNCD3D_CB_DC)(const void*pUserData,void* hDC/*HDC*/);

VIDECYNC_API int VIDECYNC_Init(unsigned long nKey);

VIDECYNC_API void VIDECYNC_PlayerSetMaxCount(unsigned int nMaxPlayer);

VIDECYNC_API int VIDECYNC_PlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight);

VIDECYNC_API void*VIDECYNC_PlayerOpen(YNCD3D_CB_DC cbdc,void*pUserData,void* hWnd/*HWND*/, int nWidth, int nHeight);
VIDECYNC_API void VIDECYNC_PlayerClose(void*hPlayer);

VIDECYNC_API int VIDECYNC_PlayerPlayFrame(void*hPlayer,const unsigned char*pFrameData, int nFrameLen,int nFrameRate,int&nFlagReset);
VIDECYNC_API int VIDECYNC_PlayerGetWidth(void*hPlayer);
VIDECYNC_API int VIDECYNC_PlayerGetHeight(void*hPlayer);
VIDECYNC_API int VIDECYNC_PlayerResize(void*hPlayer);
VIDECYNC_API int VIDECYNC_PlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay);
VIDECYNC_API int VIDECYNC_PlayerGetFrameRate(void*hPlayer);
VIDECYNC_API int VIDECYNC_PlayerGetShowType(void*hPlayer);
VIDECYNC_API bool VIDECYNC_PlayerCaptureToBMPFile(void*hPlayer,const char*cszFile);

VIDECYNC_API void*VIDECYNC_D3DPlayerOpen(YNCD3D_CB_DC cbdc,void*pUserData,void* hWnd/*HWND*/,int nWidth,int nHeight,bool bCreateWnd);
VIDECYNC_API void VIDECYNC_D3DPlayerClose(void*hPlayer);
VIDECYNC_API int  VIDECYNC_D3DPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int nWidth,int nHeight);
VIDECYNC_API void VIDECYNC_D3DPlayerRedraw(void);
VIDECYNC_API bool VIDECYNC_D3DPlayerSetWindowPos(void*hPlayer,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
VIDECYNC_API int  VIDECYNC_D3DPlayerGetWidth(void*hPlayer);
VIDECYNC_API int  VIDECYNC_D3DPlayerGetHeight(void*hPlayer);
VIDECYNC_API int  VIDECYNC_D3DPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay);
VIDECYNC_API int  VIDECYNC_D3DPlayerGetFrameRate(void*hPlayer);
VIDECYNC_API int  VIDECYNC_D3DPlayerGetShowType(void*hPlayer);

#endif