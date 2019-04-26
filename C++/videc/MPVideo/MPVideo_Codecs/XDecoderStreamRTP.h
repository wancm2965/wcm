//XDecoderStreamRTP.h
#ifndef __XDECODERSTREAMRTP_H__
#define __XDECODERSTREAMRTP_H__

#include <MPVideo/MPVideo_Codec.h>
#include <MPVideo/MPVideo_Decoder.h>
#include "XAutoLock.h"
#include "XListPtr.h"
#include "XThreadBase.h"
#include "XWorkThread.h"


class XDecoderStreamRTP 
	: public MPVideo_DecoderStream
	, public MPVideo_DecoderCallback
#ifdef __XWORKTHREAD_H__
	, public XWorkThread
#else
	, public XThreadBase
#endif
{
public:
	XDecoderStreamRTP(MPVideo_DecoderStreamCallback&rCallback);
	virtual ~XDecoderStreamRTP(void);
public:
	int Open(int nWidth,int nHeight,VIDEC_CODEC_TYPE CodecType);
	void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);
protected:
	int DoDecodePacket(void*pPacketData,int nPacketSize);
	void OnMPVideo_DecoderCallbackFrame(MPVideo_Decoder*pDecoderFrame,MPVideo_Image*pImage);
	void OnMPVideo_DecoderCallbackFramePacket(MPVideo_Decoder*pDecoderFrame,void*pPacketData,int nPacketLen,int nWidth,int nHeight);

#ifdef __XWORKTHREAD_H__
	virtual bool IsPending(void);
	virtual bool Running(void);
#else
	void ThreadProcMain(void);
#endif
protected:
	typedef struct PACKET
	{
		PACKET(void*pPacketData,int nPacketSize)
			:m_pPacketData(pPacketData)
			,m_nPacketSize(nPacketSize)
		{
		}
		void*	m_pPacketData;
		int		m_nPacketSize;
	}PACKET,*PPACKET;


	MPVideo_DecoderStreamCallback&	m_rCallback;
	VIDEC_CODEC_TYPE				m_codecType;

	bool							m_bDoubleField;
	unsigned short					m_nVirtualWidth;
	unsigned short					m_nVirtualHeight;

	XListPtr						m_ListPackets;
	XCritSec						m_csListPackets;

#ifndef __XWORKTHREAD_H__
	bool							m_bRunning;
#endif

	unsigned short					m_nPacketWidth;
	unsigned short					m_nPacketHeight;

	unsigned long					m_ulFrameTimestamp;

	bool							m_bGotKeyFrame;
	MPVideo_Decoder*					m_pDecoderFrame;

#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif
};

#endif
