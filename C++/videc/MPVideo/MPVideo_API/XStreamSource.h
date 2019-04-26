//XStreamSource.h
#ifndef __XSTREAMSOURCE_H__
#define __XSTREAMSOURCE_H__

#include <map>

#include <MPVideo/MPVideo.h>
#include "XAutoLock.h"
#include "XImage.h"

typedef std::map<void*,MPVideo_StreamSink*>	MapStreamSink;

class XStreamSource
{
public:
	XStreamSource(MPVideo_StreamSource*pStreamSource);
	virtual~XStreamSource(void);
public:
	virtual void Close(void);
	virtual int SendStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen);
	virtual int SendSubStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen);
	virtual int SendAudioStreamData(unsigned char*pData,int nLen);
	virtual int AddStreamSink(MPVideo_StreamSink*pStreamSink);
	virtual void RemoveStreamSink(MPVideo_StreamSink*pStreamSink);
	virtual int GetStreamSinkCount(void);
protected:
	MPVideo_StreamSource*	m_pStreamSource;
	MapStreamSink		m_MapStreamSink;
	XCritSec			m_csMapStreamSink;
};

#endif
