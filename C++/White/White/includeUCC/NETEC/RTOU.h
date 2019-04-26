//RTOU.h
#ifndef __RTOU_H__
#define __RTOU_H__

#include <NETEC/NETEC_Export.h>


#define RTOU_MTU	1248

#ifdef _WIN32
#include <winsock2.h>
#ifndef __INLINE__
#define __INLINE__ _inline
#endif
#else
#ifndef SOCKET
#define SOCKET int
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
	virtual void OnRTOU_RTSPSessionCallbackSendingPacket(RTOU_RTSPSession*pRTOU_RTSPSession,unsigned char*pPacketData,int nPacketLen)=0;
	//�յ���ɵ�����֡
	virtual void OnRTOU_RTSPSessionCallbackRecvdFrame(RTOU_RTSPSession*pRTOU_RTSPSession,unsigned char*pFrameData,int nFrameLen)=0;
};

class NETEC_API RTOU_RTSPSession
{
public:
	RTOU_RTSPSession(void){};
	virtual~RTOU_RTSPSession(void){};
public:
	//��
	virtual int Open(void)=0;
	//�ر�
	virtual void Close(void)=0;

	virtual bool DoTick(void)=0;
	//�Ƿ����д
	virtual bool IsWritable(void)=0;
	//д����֡
	virtual int WriteFrame(unsigned char*pFrameData,int nFrameLen)=0;
	//�����յ������ݰ�
	virtual int ProcessRecvdPacket(unsigned char*pPacketData,int nPacketLen)=0;
	//���ô���
	virtual int SetMaxBandwidth(int nMaxBandwidth)=0;
	//����RTT
	virtual void UpdateRTT(unsigned long nRTT)=0;
	//���·��Ͷ�����
	virtual void UpdateTXPacketLossRate(int nTXPacketLossRate)=0;

	//����RTSPSessuion
	static RTOU_RTSPSession*Create(RTOU_RTSPSessionCallback&rCallback,int nMaxBandwidth,int nMTU=RTOU_MTU,int nHeaderMargin=0);
};


class RTOU_JitterBufferCallback
{
public:
	RTOU_JitterBufferCallback(void){};
	virtual~RTOU_JitterBufferCallback(void){};
public:
	virtual void OnRTOU_JitterBufferCallbackRecvdPacket(unsigned short usSeq,unsigned long ulRTPTimeMS,const unsigned char*pPacketData,int nPacketLen)=0;
};

class NETEC_API RTOU_JitterBuffer
{
public:
	RTOU_JitterBuffer(void){};
	virtual~RTOU_JitterBuffer(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;

	virtual int ProcessRecvdPacket(unsigned short usSeq,unsigned long ulRTPTimeMS,const unsigned char*pPacketData,int nPacketLen)=0;
	virtual bool DoTick(void)=0;

	virtual int GetTXPacketLossRate(void)=0;
	virtual unsigned long GetRXLostPacketCount(void)=0;

	//����RTPSessuion
	static RTOU_JitterBuffer*Create(RTOU_JitterBufferCallback&rCallback);
};


#endif