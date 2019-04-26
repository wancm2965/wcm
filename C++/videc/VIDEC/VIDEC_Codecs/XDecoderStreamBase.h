//XDecoderStreamBase.h
#ifndef __XDECODERSTREAMBASE_H__
#define __XDECODERSTREAMBASE_H__

#include "VIDEC_Codec.h"
#include "VIDEC_Decoder.h"
#include "XAutoLock.h"
#include "XListPtr.h"
#include "XThreadBase.h"
#include "XWorkThread.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

class CRawFile;

class XDecoderStreamBase 
	: public VIDEC_DecoderStream
	, public VIDEC_DecoderCallback
	, public FrameRateEvent
#ifdef USE_XWORKTHREAD
	, public XWorkThread
#else
	, public XThreadBase
#endif
{
public:
	XDecoderStreamBase(void);
	virtual ~XDecoderStreamBase(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	int DecodeFramePacket(void*pPacketData,int nPacketSize);
	int DecodeRTPPacket(void*pPacketData,int nPacketSize);

	void SetVideoDelay(int nMilliSecond);
	void SetVideoSkipFrame(int nInterval,int nSkip);
	virtual	void ResetImage(VIDEC_Image*pImage);
protected:
	virtual int DoDecodePacket(void*pPacketData,int nPacketSize,int nFrameRate);
	virtual void ReDraw(void);
	void OnVIDEC_DecoderCallbackFrame(VIDEC_Decoder*pDecoderFrame,VIDEC_Image*pImage);

	virtual bool IsPending(void);
	virtual bool Running(void);

	virtual void OnDecoderStreamCallbackFrame(VIDEC_Image*pImage)=0;

	virtual void OnPreDecodingFrame(unsigned long nFrameTimestamp,unsigned long nPaintTimestamp){};

	void ReSetFrameRate(void);

	void PausePlay(bool bPause,bool bClear);

#ifdef _TIANZHU_CACHE_
	//天竺缓冲问题（bcz）
	void SetCacheFrameCount(int FrameCount);

#endif

#ifndef USE_XWORKTHREAD
	void ThreadProcMain(void);
protected:
	bool		m_bRunning;
	bool		m_bPause;
#endif

protected:
	typedef struct PACKET
	{
		PACKET(void*pPacketData,int nPacketSize,int nFrameRate)
			:m_pPacketData(pPacketData)
			,m_nPacketSize(nPacketSize)
			,m_nFrameRate(nFrameRate)
			,m_ulRecvTime(0)
		{
		}
		void*	m_pPacketData;
		int		m_nPacketSize;
		int		m_nFrameRate;
		unsigned long	m_ulRecvTime;
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

	VIDEC_Decoder*				m_pDecoderFrameEven;
	VIDEC_Decoder*				m_pDecoderFrameOdd;

	unsigned short					m_nPacketWidth;
	unsigned short					m_nPacketHeight;

	unsigned long					m_ulFrameTimestamp;

	bool							m_bGotKeyFrame;
	VIDEC_Decoder*					m_pDecoderFrame;
	bool							m_bGotImage;

	bool							m_bOffsetInitialized;
	unsigned long					m_ulFirstTimestampOffset;
	unsigned long					m_ulFirstFrameTimestamp;
	unsigned long					m_ulFrameTimeDelta;

	unsigned short					m_nFlagFrameFromHardware;
	int								m_nFrameRateWidthKeyFrame;

	XCritSec						m_csResetImage;
	VIDEC_Image*					m_pResetImage;

	int								m_nFrameCount;
	unsigned long					m_nLastTS;

	int								m_nMilliSecond;
	bool							m_bDelayFrame;
	int								m_nDelayFrameCount;

	int								m_nCountFrameIndex;
	unsigned long					m_ulLastCountFrameTimestamp;
	unsigned long					m_nFrameRateCur;
	unsigned long					m_nFrameRateCount;
	unsigned long					m_nFrameRateActual;
	bool							m_bVideoSkipFrame;

	//天竺缓冲问题（bcz）
	bool							m_bCacheFlag;			//标志量开关
	int								m_nCacheFrameCount;		//缓冲帧数

#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif

	CRawFile * m_pRawFile;

};

#endif
