//VIDECUDAAPI.h
#ifndef __CUDAAPI_H__
#define __CUDAAPI_H__

#ifdef VIDECUDA_EXPORT
#define VIDECUDA_API _declspec(dllexport)
#elif VIDECUDA_DLL
#define VIDECUDA_API _declspec(dllimport)
#else
#define VIDECUDA_API
#endif

VIDECUDA_API int VIDECUDA_Init(unsigned long nKey);

VIDECUDA_API void VIDECUDA_PlayerSetMaxCount(unsigned int nMaxPlayer);

VIDECUDA_API int VIDECUDA_PlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight);

VIDECUDA_API void*VIDECUDA_PlayerOpen(HWND hWnd, int nWidth, int nHeight);
VIDECUDA_API void VIDECUDA_PlayerClose(void*hPlayer);

VIDECUDA_API int VIDECUDA_PlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate, int&nFlagReset);
VIDECUDA_API int VIDECUDA_PlayerGetWidth(void*hPlayer);
VIDECUDA_API int VIDECUDA_PlayerGetHeight(void*hPlayer);
VIDECUDA_API int VIDECUDA_PlayerResize(void*hPlayer);
VIDECUDA_API int VIDECUDA_PlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay);

#endif