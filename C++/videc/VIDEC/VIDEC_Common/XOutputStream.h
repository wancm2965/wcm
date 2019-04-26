//XOutputStream.h
#ifndef __XOUTPUTSTREAM_H__
#define __XOUTPUTSTREAM_H__

#include <map>

#include "XAutoLock.h"
#include "VIDEC_OutputStream.h"
#include "VIDEC_Encoder.h"


class XOutputStream
	: public VIDEC_EncoderStreamCallback
{
public:
	XOutputStream(VIDEC_OutputStream*pOutputStream);
	virtual~XOutputStream(void);
public:
	virtual int GetBitrate(void);
	virtual int GetMaxBitrate(void);
	virtual int GetFrameRate(void);
	virtual int GetQuant(void);
	virtual int GetQMin(void);
	virtual int GetQMax(void);
	virtual VIDEC_CODEC_TYPE GetCodecType(void);

	virtual int OutputImage(void*pData,int nLen,int nWidth,int nHeight);

	void OnVIDEC_EncoderStreamCallbackPacket(void*pPacketData,int nPacketSize);
protected:
	VIDEC_OutputStream*	m_pOutputStream;
	VIDEC_EncoderStream*m_pEncoderStream;
};

typedef std::map<void*,XOutputStream*>	MapXOutputStream;

class XOutputStreamMgr
{
public:
	XOutputStreamMgr(void);
	virtual~XOutputStreamMgr(void);
public:
	virtual void Close(void);

	virtual int AddOutputStream(VIDEC_OutputStream*pOutputStream);
	virtual void RemoveOutputStream(VIDEC_OutputStream*pOutputStream);

	virtual int OutputImage(void*pData,int nLen,int nWidth,int nHeight);
	
	virtual int GetMaxFrameRate(void);

	virtual int GetOutputStreamCount(void);
protected:
	MapXOutputStream	m_MapXOutputStream;
	XCritSec			m_csMapXOutputStream;
};


#endif