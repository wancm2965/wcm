//VIDECICEAPI.h
#ifndef __VIDECICEAPI_H__
#define __VIDECICEAPI_H__

#ifdef VIDECICE_EXPORT
#define VIDECICE_API _declspec(dllexport)
#elif VIDECICE_DLL
#define VIDECICE_API _declspec(dllimport)
#else
#define VIDECICE_API
#endif

VIDECICE_API void*VIDECICE_Open(int nWidth,int nHeight,unsigned long nKey);
VIDECICE_API void VIDECICE_Close(void*pHandle);
VIDECICE_API int VIDECICE_Process(void*pHandle,unsigned char*pU,unsigned char*pV);

#endif