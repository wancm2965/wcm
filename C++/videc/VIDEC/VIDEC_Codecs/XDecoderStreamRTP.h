//XDecoderStreamRTP.h
#ifndef __XDECODERSTREAMRTP_H__
#define __XDECODERSTREAMRTP_H__

#include "VIDEC_Codec.h"
#include "VIDEC_Decoder.h"
#include "XAutoLock.h"
#include "XListPtr.h"
#include "XThreadBase.h"
#include "XWorkThread.h"


class XDecoderStreamRTP 
	: public VIDEC_DecoderStream
	, public VIDEC_DecoderCallback
#ifdef USE_XWORKTHREAD
	, public XWorkThread
#else
	, public XThreadBase
#endif
{
public:
	XDecoderStreamRTP(VIDEC_DecoderStreamCallback&rCallback);
	virtual ~XDecoderStreamRTP(void);
public:
	int Open(int nWidth,int nHeight,VIDEC_CODEC_TYPE CodecType);
	void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);
protected:
	int DoDecodePacket(void*pPacketData,int nPacketSize);
	void OnVIDEC_DecoderCallbackFrame(VIDEC_Decoder*pDecoderFrame,VIDEC_Image*pImage);
	void OnVIDEC_DecoderCallbackFramePacket(VIDEC_Decoder*pDecoderFrame,void*pPacketData,int nPacketLen,int nWidth,int nHeight);

#ifdef USE_XWORKTHREAD
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


	VIDEC_DecoderStreamCallback&	m_rCallback;
	VIDEC_CODEC_TYPE				m_codecType;

	bool							m_bDoubleField;
	unsigned short					m_nVirtualWidth;
	unsigned short					m_nVirtualHeight;

	XListPtr						m_ListPackets;
	XCritSec						m_csListPackets;

#ifndef USE_XWORKTHREAD
	bool							m_bRunning;
#endif

	unsigned short					m_nPacketWidth;
	unsigned short					m_nPacketHeight;

	unsigned long					m_ulFrameTimestamp;

	bool							m_bGotKeyFrame;
	VIDEC_Decoder*					m_pDecoderFrame;

#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif
};

#endif
