//XStreamSource.h
#ifndef __XSTREAMSOURCE_H__
#define __XSTREAMSOURCE_H__

#include <map>

#include "VIDEC.h"
#include "XAutoLock.h"
#include "XImage.h"

typedef std::map<void*,VIDEC_StreamSink*>	MapStreamSink;

class XStreamSource
{
public:
	XStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual~XStreamSource(void);
public:
	virtual void Close(void);
	virtual int SendStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen);
	virtual int SendSubStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen);
	virtual int SendAudioStreamData(unsigned char*pData,int nLen);
	virtual int AddStreamSink(VIDEC_StreamSink*pStreamSink);
	virtual void RemoveStreamSink(VIDEC_StreamSink*pStreamSink);
	virtual int GetStreamSinkCount(void);
protected:
	VIDEC_StreamSource*	m_pStreamSource;
	MapStreamSink		m_MapStreamSink;
	XCritSec			m_csMapStreamSink;
};

#endif
