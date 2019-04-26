// HPATC_GSMCodec.h: interface for the HPATC_GSMCodec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HPATC_GSMCODEC_H__
#define __HPATC_GSMCODEC_H__

#include "../../include/HPATC_Codec.h"


#include "AUDEC_plc.h"

#ifdef __cplusplus
extern "C" {
#endif 
#include "../HPATC_GSM/include/gsm.h"
#ifdef __cplusplus
}
#endif 

class HPATC_GSMEncoder 
	: public HPATCEncoder 
{
public:
	HPATC_GSMEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
	virtual ~HPATC_GSMEncoder(void);
public:
	int Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam=0);
	void ReleaseConnections(void);
	int InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp);
protected:
	HPATCEncoderNotify&	m_rHPATCEncoderNotify;
	int			m_nSampleRate;
	int			m_nSamplesPerFrame;
	int			m_nBitrate;
	int			m_nBytesPerFrame;

	gsm					m_pst;

	unsigned char*		m_pEncoded;
	int					m_nEncodedLen;
};


class HPATC_GSMDecoder 
	: public HPATCDecoder 
{
public:
	HPATC_GSMDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	virtual ~HPATC_GSMDecoder(void);
public:
	int Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam=0);
	void ReleaseConnections(void);
	int InputStream(unsigned char*pEncoded,int nLen,bool bDiscontinious,unsigned long ulTimestamp);
protected:
	HPATCDecoderNotify&	m_rHPATCDecoderNotify;
	int			m_nSampleRate;
	int			m_nSamplesPerFrame;
	int			m_nBitrate;
	int			m_nBytesPerFrame;

	gsm					m_pst;

	short*				m_pSamples;
	int					m_nSamples;

	AUDEC_plc*			m_ptr_plc;
};

#endif 


