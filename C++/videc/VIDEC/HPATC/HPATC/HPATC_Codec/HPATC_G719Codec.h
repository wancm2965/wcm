// HPATC_G719Codec.h: interface for the HPATC_G719Codec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HPATC_G719CODEC_H__
#define __HPATC_G719CODEC_H__

#include "../../include/HPATC_Codec.h"


extern "C"
{
#include "../HPATC_G719/include/g719api.h"
}

class HPATC_G719Encoder 
	: public HPATCEncoder 
{
public:
	HPATC_G719Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
	virtual ~HPATC_G719Encoder(void);
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


class HPATC_G719Decoder 
	: public HPATCDecoder 
{
public:
	HPATC_G719Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	virtual ~HPATC_G719Decoder(void);
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
 