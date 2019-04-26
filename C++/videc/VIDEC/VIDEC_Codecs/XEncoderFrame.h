//XEncoderFrame.h
#ifndef __XENCODERFRAME_H__
#define __XENCODERFRAME_H__

#include "VIDEC_Codec.h"
#include "VIDEC_Header.h"

#include "XAutoLock.h"
#include "XListPtr.h"
#include "XThreadBase.h"

struct XEncodDataBuffer
{
	XEncodDataBuffer(void)
		: pData(NULL)
		, nDatalen(0)
	{
	}
	virtual~XEncodDataBuffer(void)
	{
		if(pData)
		{
			FREEMSG(pData);
			pData=NULL;
		}
	}
	void*			pData;
	int				nDatalen;
};

class XEncoderFrame 
	: public VIDEC_Encoder
	, public XThreadBase
{
public:
	XEncoderFrame(VIDEC_EncoderCallback&rCallback,bool bRTPPacket);
	virtual ~XEncoderFrame(void);

	virtual void ThreadProcMain(void);
public:
	virtual int Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType);
	virtual void Close(void);
	virtual int EncodeFrame(void*pFrameData,int nFrameSize,int nVirtualWidth,int nVirtualHeight,bool bEventFieldFirst,bool bHardware,unsigned long nTimestamp);
	virtual void RequestKeyFrame(void);
protected:
	virtual int DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame)=0;
	virtual unsigned char GetCodecType(void)=0;
	virtual bool AsyncEncoding(void){return false;};
	virtual void OnPacketData(unsigned char*pPacketData,int nPacketLen,int nKeyFrame,int nBFrame,bool bHardware,unsigned long nTimestamp);
protected:
	VIDEC_EncoderCallback&		m_rCallback;
	bool						m_bRTPPacket;

	int							m_nWidth;
	int							m_nHeight;
	int							m_nFrameRate;
	
	unsigned char*				m_pPacketBuf;
	int							m_nFrameSize;
	int							m_nStride[3];
	int							m_nYImageSize;

	unsigned short				m_usSeqNo;

	int							m_nRequestKeyFrame;

	bool						m_bDoubleField;
	bool						m_bEventFirst;
	int							m_nVirtualWidth;
	int							m_nVirtualHeight;

	int							m_nBitrate;
	int							m_nMinQP;
	int							m_nMaxQP;
	int							m_nBitrateControlType;

	unsigned long				m_nLastKeyFrameTimestamp;

	bool						m_bHardware;
	unsigned long				m_nTimestamp;
	XListPtr					m_listPacketData;
	XCritSec					m_csListPacketData;
	bool							m_bWaitStop;

public:
	bool						m_bHardwareEncoding;
};

#endif