#ifndef __AH400PLAYWRAPPER_H__
#define __AH400PLAYWRAPPER_H__

#include "XDllLoadLibrary.h"

typedef void (*YUV_IMAGE_CALLBACK)(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp);

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

void AH400Play_init(void);
void AH400Play_terminate(void);

void*AH400PlayerOpen(HIK_TYPE nType,YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
void AH400PlayerInputData(void*pHandle,unsigned char*pData,int nLen);
void AH400PlayerClose(void*pHandle);

class AH400PlayWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	AH400PlayWrapper(HINSTANCE hInstance);
	~AH400PlayWrapper(void);
public:
	static AH400PlayWrapper&Instance(void);
	bool IsOK(void);

	void*(*pAH400PlayerOpen)(HIK_TYPE nType,YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
	void (*pAH400PlayerInputData)(void*pHandle,unsigned char*pData,int nLen);
	void (*pAH400PlayerClose)(void*pHandle);
};

#endif
