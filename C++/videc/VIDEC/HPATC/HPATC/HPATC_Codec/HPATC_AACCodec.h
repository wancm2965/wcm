// HPATC_AACCodec.h: interface for the HPATC_AACCodec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HPATC_AACCODEC_H__
#define __HPATC_AACCODEC_H__

#include "../../include/HPATC_Codec.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

extern "C"
{
#include "../HPATC_FAAC_Encoder/faac.h"
//#include "../HPATC_AAC_Decoder/include/neaacdec.h"
}
// #ifdef __cplusplus
// }
// #endif


class HPATC_AACEncoder 
	: public HPATCEncoder 
{
public:
	HPATC_AACEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
	virtual ~HPATC_AACEncoder(void);
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

	faacEncHandle		m_hEncoder;
	unsigned char*		m_pEncodedBuf;

	short*				m_pLeftSamples;
	int					m_nLeftSamples;
};

/*
class HPATC_AACDecoder 
	: public HPATCDecoder 
{
public:
	HPATC_AACDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	virtual ~HPATC_AACDecoder(void);
public:
	int Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam=0);
	void ReleaseConnections(void);
	int InputStream(unsigned char*pEncoded,int nLen,bool bDiscontinious);
protected:
	HPATCDecoderNotify&	m_rHPATCDecoderNotify;
	int					m_nSampleRate;
	int					m_nSamplesPerFrame;
	int					m_nBitrate;
	int					m_nBytesPerFrame;

	faacDecHandle		m_hDecoder;
	faacDecFrameInfo	m_decFrameInfo;
	bool				m_bInited;

	//FILE*				m_pFile;
	short*				m_pLeftSamples;
	int					m_nLeftSamples;
};
*/

#endif

