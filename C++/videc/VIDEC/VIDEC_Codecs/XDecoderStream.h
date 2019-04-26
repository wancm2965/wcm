//XDecoderStream.h
#ifndef __XDECODERSTREAM_H__
#define __XDECODERSTREAM_H__

#include "XDecoderStreamBase.h"


class XDecoderStream 
	: public XDecoderStreamBase
{
public:
	XDecoderStream(VIDEC_DecoderStreamCallback&rCallback);
	virtual ~XDecoderStream(void);
protected:
	virtual void OnDecoderStreamCallbackFrame(VIDEC_Image*pImage);
protected:
	VIDEC_DecoderStreamCallback&	m_rCallback;
};

#endif
