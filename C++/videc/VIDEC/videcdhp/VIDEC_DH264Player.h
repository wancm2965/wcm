//VIDEC_DH264Player.h
#ifndef __VIDEC_DH264PLAYER_H__
#define __VIDEC_DH264PLAYER_H__

#ifdef VIDEC_DH264PLAYER_EXPORT
#define VIDEC_DH264PLAYER_API _declspec(dllexport)
#elif VIDEC_DH264PLAYER_DLL
#define VIDEC_DH264PLAYER_API _declspec(dllimport)
#else
#define VIDEC_DH264PLAYER_API
#endif

typedef void (*YUV_IMAGE_CALLBACK)(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp);

VIDEC_DH264PLAYER_API void* VIDEC_DH264PlayerOpen(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
VIDEC_DH264PLAYER_API void VIDEC_DH264PlayerInputData(void*pHandle,unsigned char*pData,int nLen);
VIDEC_DH264PLAYER_API void VIDEC_DH264PlayerClose(void*pHandle);

#endif