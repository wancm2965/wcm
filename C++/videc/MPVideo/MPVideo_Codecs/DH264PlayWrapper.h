#ifndef __DH264PLAYWRAPPER_H__
#define __DH264PLAYWRAPPER_H__

#include "XDllLoadLibrary.h"

typedef void (*YUV_IMAGE_CALLBACK)(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp);

void DH264Play_init(void);
void DH264Play_terminate(void);

void*DH264PlayerOpen(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
void DH264PlayerInputData(void*pHandle,unsigned char*pData,int nLen);
void DH264PlayerClose(void*pHandle);

class DH264PlayWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	DH264PlayWrapper(HINSTANCE hInstance);
	~DH264PlayWrapper(void);
public:
	static DH264PlayWrapper&Instance(void);
	bool IsOK(void);

	void*(*pDH264PlayerOpen)(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
	void (*pDH264PlayerInputData)(void*pHandle,unsigned char*pData,int nLen);
	void (*pDH264PlayerClose)(void*pHandle);
};

#endif
