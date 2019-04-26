// HPATC_G729ACodec.h: interface for the HPATC_G729ACodec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HPATC_G729ACODEC_H__
#define __HPATC_G729ACODEC_H__

#include "../../include/HPATC_Codec.h"

extern "C"
{
#include "../HPATC_G729A/g729a.h"
}

class HPATC_G729AEncoder 
	: public HPATCEncoder 
{
public:
	HPATC_G729AEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
	virtual ~HPATC_G729AEncoder(void);
public:
	int Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam=0);
	void ReleaseConnections(void);
	int InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp);
protected:
	HPATCEncoderNotify&	m_rHPATCEncoderNotify;
	int					m_nSampleRate;
	unsigned long		m_nSamplesPerFrame;
	int					m_nBitrate;
	unsigned long		m_nBytesPerFrame;

	void*				m_pEncoder;

	short*				m_pLeftSamples;
	int					m_nLeftSamples;
};


class HPATC_G729ADecoder 
	: public HPATCDecoder 
{
public:
	HPATC_G729ADecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	virtual ~HPATC_G729ADecoder(void);
public:
	int Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam=0);
	void ReleaseConnections(void);
	int InputStream(unsigned char*pEncoded,int nLen,bool bDiscontinious,unsigned long ulTimestamp);
protected:
	HPATCDecoderNotify&	m_rHPATCDecoderNotify;
	int					m_nSampleRate;
	int					m_nSamplesPerFrame;
	int					m_nBitrate;
	int					m_nBytesPerFrame;

	bool				m_bInited;

	void*				m_pDecoder;

	short*				m_pLeftSamples;
	int					m_nLeftSamples;

};

#endif
