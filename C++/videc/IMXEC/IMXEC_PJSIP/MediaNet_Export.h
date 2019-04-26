//MediaNet_Export.h
#ifndef __MediaNet_EXPORT_H__
#define __MediaNet_EXPORT_H__

#ifdef MediaNet_EXPORT
#define MediaNet_API _declspec(dllexport)
#elif MediaNet_DLL
#define MediaNet_API _declspec(dllimport)
#else
#define MediaNet_API
#endif

class MediaNet_RTPSessionCallback
{
public:
	virtual void OnMediaNet_RTPSessionCallbackRcvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)=0;
	//virtual void OnMediaNet_RTPSessionCallbackSendingPacket(unsigned char*pPacketData,int nPacketLen,const void*pRemoteAddr,int nRemoteAddrLen)=0;
};

#endif//__MediaNet_EXPORT_H__