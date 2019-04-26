//VIDECD3DAPI.h
#ifndef __VIDECD3DAPI_H__
#define __VIDECD3DAPI_H__

#ifdef VIDECD3D_EXPORT
#define VIDECD3D_API _declspec(dllexport)
#elif VIDECD3D_DLL
#define VIDECD3D_API _declspec(dllimport)
#else
#define VIDECD3D_API
#endif

VIDECD3D_API int VIDECD3D_Init(unsigned long nKey);

VIDECD3D_API void*VIDECD3D_PlayerOpen(HWND hWnd, int nWidth, int nHeight);
VIDECD3D_API void VIDECD3D_PlayerClose(void*hPlayer);

VIDECD3D_API int VIDECD3D_PlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int nWidth,int nHeight);
VIDECD3D_API int VIDECD3D_PlayerGetWidth(void*hPlayer);
VIDECD3D_API int VIDECD3D_PlayerGetHeight(void*hPlayer);
VIDECD3D_API int VIDECD3D_PlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay);


#endif