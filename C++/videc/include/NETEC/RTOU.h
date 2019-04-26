//RTOU.h
#ifndef __RTOU_H__
#define __RTOU_H__

#include <NETEC/NETEC_Export.h>
#include <NETEC/NETEC_Setting.h>


#define RTOU_MTU	(NETEC_Setting::GetMTU())

#ifdef _WIN32
#include <winsock2.h>
#ifndef __INLINE__
#define __INLINE__ _inline
#endif
#else
#ifndef SOCKET
#define SOCKET int32_t
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif
#ifndef __INLINE__
#define __INLINE__ inline
#endif
#endif


class NETEC_API RTOU_RTSPSession;
class RTOU_RTSPSessionCallback
{
public:
	RTOU_RTSPSessionCallback(void){};
	virtual~RTOU_RTSPSessionCallback(void){};
public:
	//�����ݰ�Ҫ����
	virtual void OnRTOU_RTSPSessionCallbackSendingPacket(RTOU_RTSPSession*pRTOU_RTSPSession,unsigned char*pPacketData,int32_t nPacketLen)=0;
	//�յ���ɵ�����֡
	virtual void OnRTOU_RTSPSessionCallbackRecvdFrame(RTOU_RTSPSession*pRTOU_RTSPSession,unsigned char*pFrameData,int32_t nFrameLen)=0;

	virtual void OnRTOU_RTSPSessionCallbackBitrateChanged(uint32_t nBitrate,uint32_t nFrameRate){};
};

class NETEC_API RTOU_RTSPSession
{
public:
	RTOU_RTSPSession(void){};
	virtual~RTOU_RTSPSession(void){};
public:
	//��
	virtual int32_t Open(bool bClient=false)=0;
	//�ر�
	virtual void Close(void)=0;

	virtual bool DoTick(void)=0;
	//�Ƿ����д
	virtual bool IsWritable(void)=0;
	//д����֡
	virtual int32_t WriteFrame(unsigned char*pFrameData,int32_t nFrameLen)=0;
	//�����յ������ݰ�
	virtual int32_t ProcessRecvdPacket(unsigned char*pPacketData,int32_t nPacketLen)=0;
	//���ô���
	virtual int32_t SetMaxBandwidth(int32_t nMaxBandwidth)=0;
	//����RTT
	virtual void UpdateRTT(uint32_t nRTT)=0;
	//���·��Ͷ�����
	virtual void UpdateTXPacketLossRate(int32_t nTXPacketLossRate)=0;

	//����RTSPSessuion
	static RTOU_RTSPSession*Create(RTOU_RTSPSessionCallback&rCallback,int32_t nMaxBandwidth,int32_t nMTU=RTOU_MTU,int32_t nHeaderMargin=0);
};


class RTOU_JitterBufferCallback
{
public:
	RTOU_JitterBufferCallback(void){};
	virtual~RTOU_JitterBufferCallback(void){};
public:
	virtual void OnRTOU_JitterBufferCallbackRecvdPacket(unsigned short usSeq,uint32_t ulRTPTimeMS,const unsigned char*pPacketData,int32_t nPacketLen)=0;
};

class NETEC_API RTOU_JitterBuffer
{
public:
	RTOU_JitterBuffer(void){};
	virtual~RTOU_JitterBuffer(void){};
public:
	virtual int32_t Open(void)=0;
	virtual void Close(void)=0;

	virtual int32_t ProcessRecvdPacket(unsigned short usSeq,uint32_t ulRTPTimeMS,const unsigned char*pPacketData,int32_t nPacketLen)=0;
	virtual bool DoTick(void)=0;

	virtual int32_t GetTXPacketLossRate(void)=0;
	virtual uint32_t GetRXLostPacketCount(void)=0;

	//����RTPSessuion
	static RTOU_JitterBuffer*Create(RTOU_JitterBufferCallback&rCallback);
};

typedef enum {
	ST_UDX=0,
	ST_VTE,
}SESSION_TYPE;
RTOU_RTSPSession* CreateUDXSession(RTOU_RTSPSessionCallback&callback,SESSION_TYPE nType=ST_UDX);


#endif