//VIDEC_AH400Player.h
#ifndef __VIDEC_AH400PLAYER_H__
#define __VIDEC_AH400PLAYER_H__

#ifdef VIDEC_AH400PLAYER_EXPORT
#define VIDEC_AH400PLAYER_API _declspec(dllexport)
#elif VIDEC_AH400PLAYER_DLL
#define VIDEC_AH400PLAYER_API _declspec(dllimport)
#else
#define VIDEC_AH400PLAYER_API
#endif

typedef enum {
	HIK_H=0,
	HIK_HC,
	HIK_CAMERA,
	HIK_IMKH,
	HIK_DS81,
	HIK_DS71,
	HIK_X62MF,
	HIK_MEGA,
	HIK_DS80HD,
}HIK_TYPE;

typedef void (*YUV_IMAGE_CALLBACK)(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp);

VIDEC_AH400PLAYER_API void* VIDEC_AH400PlayerOpen(HIK_TYPE nType,YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
VIDEC_AH400PLAYER_API void VIDEC_AH400PlayerInputData(void*pHandle,unsigned char*pData,int nLen);
VIDEC_AH400PLAYER_API void VIDEC_AH400PlayerClose(void*pHandle);

#endif