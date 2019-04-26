// HPATC_G7221Codec.h: interface for the HPATC_G7221Codec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HPATC_G7221CODEC_H__
#define __HPATC_G7221CODEC_H__

#include "../../include/HPATC_Codec.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../HPATC_G7221/g722_1.h"

#ifdef __cplusplus
}
#endif

class HPATC_G7221Encoder 
	: public HPATCEncoder 
{
public:
	HPATC_G7221Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
	virtual ~HPATC_G7221Encoder(void);
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

	g722_1_encode_state_t*	m_pEncoder;
};


class HPATC_G7221Decoder 
	: public HPATCDecoder 
{
public:
	HPATC_G7221Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	virtual ~HPATC_G7221Decoder(void);
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

	g722_1_decode_state_t*	m_pDecoder;
};


#endif