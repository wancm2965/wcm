// HPATC_G711ACodec.h: interface for the HPATC_G711ACodec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HPATC_G711ACODEC_H__
#define __HPATC_G711ACODEC_H__

#include "../../include/HPATC_Codec.h"


#include "AUDEC_plc.h"

class HPATC_G711AEncoder 
	: public HPATCEncoder 
{
public:
	HPATC_G711AEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
	virtual ~HPATC_G711AEncoder(void);
public:
	int Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam=0);
	void ReleaseConnections(void);
	int InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp);
protected:
	HPATCEncoderNotify&	m_rHPATCEncoderNotify;
	int					m_nSampleRate;
	int					m_nSamplesPerFrame;
	int					m_nBitrate;
	int					m_nBytesPerFrame;

	unsigned char*		m_pEncoded;
};


class HPATC_G711ADecoder 
	: public HPATCDecoder 
{
public:
	HPATC_G711ADecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	virtual ~HPATC_G711ADecoder(void);
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

	short*				m_pSamples;

	AUDEC_plc*			m_ptr_plc;
};

#endif 

