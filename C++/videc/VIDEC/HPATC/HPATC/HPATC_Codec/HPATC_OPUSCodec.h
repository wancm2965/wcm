// HPATC_OPUSCodec.h: interface for the HPATC_OPUSCodec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HPATC_OPUSCODEC_H__
#define __HPATC_OPUSCODEC_H__

#include "../../include/HPATC_Codec.h"


extern "C"
{
#include "../HPATC_opus/src/opus_api.h"	
}

class HPATC_OPUSEncoder 
	: public HPATCEncoder 
{
public:
	HPATC_OPUSEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
	virtual ~HPATC_OPUSEncoder(void);
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


class HPATC_OPUSDecoder 
	: public HPATCDecoder 
{
public:
	HPATC_OPUSDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	virtual ~HPATC_OPUSDecoder(void);
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