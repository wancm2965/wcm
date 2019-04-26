//XOutputRTP.h
#ifndef __XOUTPUTRTP_H__
#define __XOUTPUTRTP_H__

#include <map>

#include "XAutoLock.h"
#include "VIDEC_OutputRTP.h"
#include "VIDEC_Codec.h"


class XOutputRTP
	: public VIDEC_EncoderRTPCallback
{
public:
	XOutputRTP(VIDEC_OutputRTP*pOutputRTP);
	virtual~XOutputRTP(void);
public:
	virtual int GetBitrate(void);
	virtual int GetMaxBitrate(void);
	virtual int GetFrameRate(void);
	virtual int GetQuant(void);
	virtual int GetQMin(void);
	virtual int GetQMax(void);
	virtual VIDEC_CODEC_TYPE GetCodecType(void);

	virtual int OutputImage(void*pData,int nLen,int nWidth,int nHeight);

	void OnVIDEC_EncoderRTPCallbackPacket(void*pPacketData,int nPacketSize);
protected:
	VIDEC_OutputRTP*	m_pOutputRTP;
	VIDEC_EncoderRTP*	m_pEncoderRTP;

	int					m_nWidth;
	int					m_nHeight;
};

typedef std::map<void*,XOutputRTP*>	MapXOutputRTP;

class XOutputRTPMgr
{
public:
	XOutputRTPMgr(void);
	virtual~XOutputRTPMgr(void);
public:
	virtual void Close(void);

	virtual int AddOutputRTP(VIDEC_OutputRTP*pOutputRTP);
	virtual void RemoveOutputRTP(VIDEC_OutputRTP*pOutputRTP);

	virtual int OutputImage(void*pData,int nLen,int nWidth,int nHeight);
	
	virtual int GetMaxFrameRate(void);
protected:
	MapXOutputRTP	m_MapXOutputRTP;
	XCritSec		m_csMapXOutputRTP;
};


#endif