//XDecoderStream.h
#ifndef __XDECODERSTREAM_H__
#define __XDECODERSTREAM_H__

#include "XDecoderStreamBase.h"


class XDecoderStream 
	: public XDecoderStreamBase
{
public:
	XDecoderStream(MPVideo_DecoderStreamCallback&rCallback);
	virtual ~XDecoderStream(void);
protected:
	virtual void OnDecoderStreamCallbackFrame(MPVideo_Image*pImage);
protected:
	MPVideo_DecoderStreamCallback&	m_rCallback;
};

#endif
