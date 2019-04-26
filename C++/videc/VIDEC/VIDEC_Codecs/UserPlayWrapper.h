#ifndef __USERPLAYWRAPPER_H__
#define __USERPLAYWRAPPER_H__

#include "XDllLoadLibrary.h"

typedef void (*YUV_IMAGE_CALLBACK)(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp);

void UserPlay_init(void);
void UserPlay_terminate(void);

void*UserPlayerOpen(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
void UserPlayerInputData(void*pHandle,unsigned char*pData,int nLen);
void UserPlayerClose(void*pHandle);

class UserPlayWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	UserPlayWrapper(HINSTANCE hInstance);
	~UserPlayWrapper(void);
public:
	static UserPlayWrapper&Instance(void);
	bool IsOK(void);

	void (*pUserPlayerInit)(void);
	void (*pUserPlayerTerminate)(void);
	void*(*pUserPlayerOpen)(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
	void (*pUserPlayerInputData)(void*pHandle,unsigned char*pData,int nLen);
	void (*pUserPlayerClose)(void*pHandle);
};

#endif
