//NETEC_DVC.h
#ifndef __NETEC_DVC_H__
#define __NETEC_DVC_H__

#include <NETEC/NETEC_Export.h>

class NETEC_DVCCallback
{
public:
	virtual void OnNETEC_DVCCallbackVideoFrameData(unsigned char*pFrameData,int32_t nFrameLen)=0;
	virtual void OnNETEC_DVCCallbackRcvdUserData(const char*pData,int32_t nLen){};
};

class NETEC_API NETEC_DVC
{
public:
	NETEC_DVC(void){};
	virtual~NETEC_DVC(void){};
public:
	virtual int32_t Open(const char*cszIP,unsigned short nPort,const char*cszPassword)=0;
	virtual void Close(void)=0;
	virtual int32_t GetConnected(void)=0;
	virtual int32_t GetErrorCode(void)=0;
	virtual void SetVideoParams(unsigned short usVideoCodec,unsigned short usBitrate,unsigned short usBitCtrlType,unsigned short usResolution,unsigned short usFps,unsigned short usVideoSource)=0;
	virtual void RequestKeyFrame(void)=0;
	virtual int32_t SendData(const char*pData,int32_t nLen)=0;

	static NETEC_DVC*Create(NETEC_DVCCallback&rCallback);
};

#endif
