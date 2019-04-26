//IntelVideoDecoder.h
#ifndef __INTEL_VIDEO_CODER_H__
#define __INTEL_VIDEO_CODER_H__

#include "HWVideoDecoder.h"

class IntelVideoDecoder
	: public HWVideoDecoder
{
public:
	IntelVideoDecoder(HWVideoDecoderCallback&rCallback);
	virtual~IntelVideoDecoder(void);
public:
	virtual int Open(int nWidth,int nHeight);
	virtual void Close(void);
	virtual int Decode(const unsigned char *buffer, int buflen);
	virtual int GetWidth(void);
	virtual int GetHeight(void);

	virtual void OnVideoDecoderCB(unsigned char*pData,int nLen,int nWidth,int nHeight,int nStride);
protected:
	HWVideoDecoderCallback&	m_rCallback;
	void*					m_pDecoder;
	int						m_nWidth;
	int						m_nHeight;
};


#endif
