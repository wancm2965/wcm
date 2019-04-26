// HPATC_GSMCodec.cpp: implementation of the HPATC_GSMCodec class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "HPATC_GSMCodec.h"

HPATCDecoder*HPATCDecoder::CreateGSMDecoder(HPATCDecoderNotify&rHPATCDecoderNotify)
{
	return new HPATC_GSMDecoder(rHPATCDecoderNotify);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HPATC_GSMEncoder::HPATC_GSMEncoder(HPATCEncoderNotify&rHPATCEncoderNotify)
:m_rHPATCEncoderNotify(rHPATCEncoderNotify)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)

,m_pst(NULL)

,m_pEncoded(NULL)
,m_nEncodedLen(0)
{

}

HPATC_GSMEncoder::~HPATC_GSMEncoder(void)
{

}

int HPATC_GSMEncoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if (m_nBitrate!=13200 ||
		m_nSampleRate!=8000 ||
		m_nSamplesPerFrame%160)
	{
		return -1;
	}

	m_pst=gsm_create();
	if (m_pst==NULL)
	{
		return -1;
	}

	return 0;
}

void HPATC_GSMEncoder::ReleaseConnections(void)
{
	if (m_pst!=NULL)
	{
		gsm_destroy(m_pst);
		m_pst=NULL;
	}

	if (m_pEncoded!=NULL)
	{
		free(m_pEncoded);
		m_pEncoded=NULL;
	}
}

int HPATC_GSMEncoder::InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp)
{
	if (m_pst!=NULL && pSamples!=NULL && nSamples>0 && nSamples%160==0)
	{
		int nFrameCount=nSamples/160;
		int nEncodedLen=nFrameCount*33;

		if (m_pEncoded==NULL ||
			m_nEncodedLen<nEncodedLen)
		{
			if (m_pEncoded!=NULL)
			{
				free(m_pEncoded);
				m_pEncoded=NULL;
			}

			m_nEncodedLen=nEncodedLen;
			m_pEncoded=(unsigned char*)malloc(m_nEncodedLen);
		}

		unsigned char*pEncoded=m_pEncoded;
		if (pEncoded!=NULL)
		{
			unsigned char*pTempEncoded=pEncoded;
			short*pTempSamples=pSamples;
			for (int i=0;i<nFrameCount;i++)
			{
				gsm_encode(m_pst, pTempSamples, pTempEncoded);
				pTempEncoded+=33;
				pTempSamples+=160;
			}

			m_rHPATCEncoderNotify.OnHPATCEncoderNotifyOutput(pEncoded, nEncodedLen,ulTimestamp);
			return 0;
		}
	}

	return -1;
}

HPATC_GSMDecoder::HPATC_GSMDecoder(HPATCDecoderNotify&rHPATCDecoderNotify)
:m_rHPATCDecoderNotify(rHPATCDecoderNotify)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)

,m_pst(NULL)

,m_pSamples(NULL)
,m_nSamples(0)
,m_ptr_plc(NULL)
{

}

HPATC_GSMDecoder::~HPATC_GSMDecoder(void)
{

}

int HPATC_GSMDecoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if (m_nSamplesPerFrame%160)
	{
		return -1;
	}

	m_pst=gsm_create();
	if (m_pst==NULL)
	{
		return -1;
	}

	m_ptr_plc=new AUDEC_plc();
	if (0!=m_ptr_plc->Open(m_nSampleRate,m_nSamplesPerFrame))
	{
		m_ptr_plc->Close();
		delete m_ptr_plc;
		m_ptr_plc=NULL;
	}

	return 0;
}

void HPATC_GSMDecoder::ReleaseConnections(void)
{
	if (m_pst!=NULL)
	{
		gsm_destroy(m_pst);
		m_pst=NULL;
	}

	if (m_pSamples!=NULL)
	{
		free(m_pSamples);
		m_pSamples=NULL;
	}
	if (m_ptr_plc!=NULL)
	{
		m_ptr_plc->Close();
		delete m_ptr_plc;
		m_ptr_plc=NULL;
	}
}

int HPATC_GSMDecoder::InputStream(unsigned char*pEncoded,int nLen,bool bDiscontinious,unsigned long ulTimestamp)
{
	if (pEncoded!=NULL && m_pst!=NULL && nLen>0 && nLen%33==0)
	{
		int nFrameCount=nLen/33;
		int nSamples=nFrameCount*160;
		if (m_pSamples==NULL ||
			m_nSamples<nSamples)
		{
			if (m_pSamples!=NULL)
			{
				free(m_pSamples);
				m_pSamples=NULL;
			}

			m_nSamples=nSamples;
			m_pSamples=(short*)malloc(m_nSamples<<1);
		}

		short*pSamples=m_pSamples;
		if (pSamples!=NULL)
		{
			if (bDiscontinious)
			{
				bDiscontinious=false;
				if (m_ptr_plc!=NULL)
				{
					printf("AUDEC: HPATC_GSMDecoder::InputStream : m_ptr_plc->Recover!\n");
					m_ptr_plc->Recover(m_pSamples);
					m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(m_pSamples,nSamples,m_nSampleRate,bDiscontinious,ulTimestamp-10);
				}
			}

			unsigned char*pTempEncoded=pEncoded;
			short*pTempSamples=pSamples;
			for (int i=0;i<nFrameCount;i++)
			{
				gsm_decode(m_pst, pTempEncoded, pTempSamples);
				pTempEncoded+=33;
				pTempSamples+=160;
			}

			if (m_ptr_plc!=NULL)
			{
				m_ptr_plc->Save(pSamples);
			}

			m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pSamples,nSamples,m_nSampleRate,bDiscontinious,ulTimestamp);
		}
		return 0;
	}

	return -1;
}


HPATCEncoder*CreateHPATC_GSMEncoder(HPATCEncoderNotify&rHPATCEncoderNotify)
{
	return new HPATC_GSMEncoder(rHPATCEncoderNotify);
}


