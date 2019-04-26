//XDecoderStreamBase.h
#ifndef __XDECODERSTREAMBASE_H__
#define __XDECODERSTREAMBASE_H__

#include <MPVideo/MPVideo_Codec.h>
#include <MPVideo/MPVideo_Decoder.h>
#include "XAutoLock.h"
#include "XListPtr.h"
#include "XWorkThread.h"


class XDecoderStreamBase 
	: public MPVideo_DecoderStream
	, public MPVideo_DecoderCallback
	, public XWorkThread
{
public:
	XDecoderStreamBase(void);
	virtual ~XDecoderStreamBase(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);
protected:
	virtual int DoDecodePacket(void*pPacketData,int nPacketSize);
	void OnMPVideo_DecoderCallbackFrame(MPVideo_Decoder*pDecoderFrame,MPVideo_Image*pImage);

	virtual bool IsPending(void);
	virtual bool Running(void);

	virtual void OnDecoderStreamCallbackFrame(MPVideo_Image*pImage)=0;

	virtual void OnPreDecodingFrame(unsigned long nFrameTimestamp,unsigned long nPaintTimestamp){};
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


	VIDEC_CODEC_TYPE				m_codecType;

	bool							m_bDoubleField;
	unsigned short					m_nVirtualWidth;
	unsigned short					m_nVirtualHeight;

	XListPtr						m_ListPackets;
	XCritSec						m_csListPackets;

	bool							m_bGotEvenKeyFrame;
	bool							m_bGotOddKeyFrame;
	unsigned short					m_usLastSeqNo;
	bool							m_bGotLastSeqNo;

	bool							m_bGotEvenKeyFrameDecode;
	bool							m_bGotOddKeyFrameDecode;
	unsigned short					m_usLastSeqNoDecode;
	bool							m_bGotLastSeqNoDecode;
	bool							m_bEvenFieldFirst;

	bool							m_bGotEvenKeyFrame0;
	bool							m_bGotEvenKeyFrame1;
	bool							m_bGotEvenKeyFrame2;
	bool							m_bGotEvenKeyFrame3;
	bool							m_bGotOddKeyFrame0;
	bool							m_bGotOddKeyFrame1;
	bool							m_bGotOddKeyFrame2;
	bool							m_bGotOddKeyFrame3;

	MPVideo_Decoder*				m_pDecoderFrameEven;
	MPVideo_Decoder*				m_pDecoderFrameOdd;

	unsigned short					m_nPacketWidth;
	unsigned short					m_nPacketHeight;

	unsigned long					m_ulFrameTimestamp;

	bool							m_bGotKeyFrame;
	MPVideo_Decoder*					m_pDecoderFrame;
	bool							m_bGotImage;

	bool							m_bOffsetInitialized;
	unsigned long					m_ulFirstTimestampOffset;
	unsigned long					m_ulFirstFrameTimestamp;
	unsigned long					m_ulFrameTimeDelta;

#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif
};

#endif
