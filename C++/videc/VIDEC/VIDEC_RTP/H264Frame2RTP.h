//H264Frame2RTP.h
#ifndef __H264FRAME2RTP_H__
#define __H264FRAME2RTP_H__

#include "RTPJitterBuffer.h"
#include "RTPBuffer.h"


class IH264Frame2RTPCallback
{
public:
	IH264Frame2RTPCallback(void){};
	virtual~IH264Frame2RTPCallback(void){};
public:
	virtual void OnIH264Frame2RTPCallbackRTPPacketData(RTPBuffer*pRTPBuffer)=0;
};

class IH264Frame2RTP
{
public:
	IH264Frame2RTP(void){};
	virtual~IH264Frame2RTP(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;
	virtual void SetRTPPayloadType(int nPayloadType)=0;
	virtual int GetRTPPayloadType(void)=0;
	virtual void SetMaxBitrate(int nMaxBitrate)=0;
	virtual void SetFrameRate(int nFrameRate)=0;
	virtual int SetFrameData(void*pData,int nLen,int nKeyFrame)=0;
	virtual int GetRTPBufferCount(void)=0;

	static IH264Frame2RTP*CreateSimple(IH264Frame2RTPCallback&rCallback);
	static IH264Frame2RTP*CreateJitter(IH264Frame2RTPCallback&rCallback);
};


class H264Frame2RTPSimple
	: public IH264Frame2RTP
{
public:
	H264Frame2RTPSimple(IH264Frame2RTPCallback&rCallback);
	virtual~H264Frame2RTPSimple(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	virtual void SetRTPPayloadType(int nRTPPayloadType);
	virtual int GetRTPPayloadType(void);
	virtual void SetMaxBitrate(int nMaxBitrate);
	virtual void SetFrameRate(int nFrameRate);
	virtual int SetFrameData(void*pData,int nLen,int nKeyFrame);
	virtual int GetRTPBufferCount(void){return 0;}
protected:
	virtual void nal_send(const unsigned char *buf, int size, int last);
	virtual void set_rtp_header(unsigned char*pHeader,bool bMarker);
protected:
	IH264Frame2RTPCallback&	m_rCallback;
	int						m_nRTPPayloadType;
	int						m_nMaxRTPPayloadLen;
	unsigned long			m_nRTPTimestamp;
	unsigned long			m_nSSRC;
	unsigned short			m_nSeqNo; 
	unsigned long			m_nRTPTimestampInc;
};

class H264Frame2RTPJitter
	: public IH264Frame2RTP
	, public RTPJitterBufferOut
{
public:
	H264Frame2RTPJitter(IH264Frame2RTPCallback&rCallback);
	virtual~H264Frame2RTPJitter(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	virtual void SetRTPPayloadType(int nRTPPayloadType);
	virtual int GetRTPPayloadType(void);
	virtual void SetMaxBitrate(int nMaxBitrate);
	virtual void SetFrameRate(int nFrameRate);
	virtual int SetFrameData(void*pData,int nLen,int nKeyFrame);
	virtual int GetRTPBufferCount(void);
protected:
	virtual void nal_send(const unsigned char *buf, int size, int last);
	virtual void set_rtp_header(unsigned char*pHeader,bool bMarker);
	virtual void OnRTPPacketOut(RTPBuffer*pRTPBuffer);
protected:
	IH264Frame2RTPCallback&	m_rCallback;

	int						m_nRTPPayloadType;
	int						m_nMaxRTPPayloadLen;
	unsigned long			m_nRTPTimestamp;
	unsigned long			m_nSSRC;
	unsigned short			m_nSeqNo; 
	unsigned long			m_nRTPTimestampInc;
};

#endif
