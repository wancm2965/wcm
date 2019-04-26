// AudioTranscode.cpp: implementation of the AudioTranscode class.
//
//////////////////////////////////////////////////////////////////////
#include "AudioTranscode.h"


void* HPATC_Open(void*pUserData,HPATC_CALLBACK_AAC_DATA cb_aac_data)
{
	AudioTranscode* pATC = new AudioTranscode(pUserData,cb_aac_data);
	if (pATC)
	{
		pATC->Open(44100,1024);
	}
	return pATC;
}

void  HPATC_Close(void*pHandle)
{
	AudioTranscode* pATC = (AudioTranscode *)pHandle;  
	if (pATC)
	{
		pATC->Close();
		delete pATC;
		pATC = NULL;
	}
}

int   HPATC_TranscodePCMData(void*pHandle,short*pSamples,int nSamples,int nSampleRate,unsigned long ulTimestamp)
{
	AudioTranscode* pATC = (AudioTranscode *)pHandle;  
	if (pATC)
	{
		return pATC->TranscodePCMData(pSamples, nSamples, nSampleRate, ulTimestamp);
	}
	return -1;
}

int   HPATC_TranscodePacketData(void*pHandle,void*pPacketData,int nPacketLen)
{
	AudioTranscode* pATC = (AudioTranscode *)pHandle;  
	if (pATC)
	{
		return pATC->TranscodePacketData(pPacketData,nPacketLen);
	}
	return -1;
}


HPATC_Transcode*HPATC_Transcode::Create(HPATC_TranscodeCallback*pCallback)
{
	return new AudioTranscode(pCallback);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AudioTranscode::AudioTranscode(HPATC_TranscodeCallback*pCallback)
:m_pCallback(pCallback)
,m_cb_aac_data(NULL)
,m_pUserData(NULL)
//,m_bWaitForStop(false)
,m_nSequence(0)
//,m_nTimeStamp(0)
,m_pHPATCEncoder(NULL)
,m_pHPATCDecoder(NULL)
,m_pHPATCResample(NULL)
,m_pResampleBuf(NULL)
,m_nResampleBufLen(0)
,m_nDecodeCodecID(X_AUDIO_CODEC_COUNT)
,m_nDecodeSampleRate(0)
,m_nEncodeSampleRate(44100)
{

}

AudioTranscode::AudioTranscode(void*pUserData,HPATC_CALLBACK_AAC_DATA cb_aac_data)
:m_pCallback(NULL)
,m_cb_aac_data(cb_aac_data)
,m_pUserData(pUserData)
//,m_bWaitForStop(false)
,m_nSequence(0)
//,m_nTimeStamp(0)
,m_pHPATCEncoder(NULL)
,m_pHPATCDecoder(NULL)
,m_pHPATCResample(NULL)
,m_pResampleBuf(NULL)
,m_nResampleBufLen(0)
,m_nDecodeCodecID(X_AUDIO_CODEC_COUNT)
,m_nDecodeSampleRate(0)
,m_nEncodeSampleRate(44100)
{

}

AudioTranscode::~AudioTranscode(void)
{

}

int AudioTranscode::Open(int nSampleRate,int nSamplesPerFrame)
{
	//m_bWaitForStop = false;
	//StartThread();

	if (m_pHPATCEncoder == NULL)
	{
		m_nEncodeSampleRate = nSampleRate;
		m_pHPATCEncoder = HPATCEncoder::CreateAACEncoder(*this);
		m_pHPATCEncoder->Connect(m_nEncodeSampleRate,nSamplesPerFrame,0);
	}

	return 0;
}

void AudioTranscode::Close(void)
{
	//m_bWaitForStop = true;
	//WaitForStop();

	if (m_pHPATCDecoder)
	{
		m_pHPATCDecoder->ReleaseConnections();
		delete m_pHPATCDecoder;
		m_pHPATCDecoder = NULL;
	}
	if (m_pHPATCEncoder)
	{
		m_pHPATCEncoder->ReleaseConnections();
		delete m_pHPATCEncoder;
		m_pHPATCEncoder = NULL;
	}
	if (m_pHPATCResample)
	{
		m_pHPATCResample->ReleaseConnections();
		delete m_pHPATCResample;
		m_pHPATCResample = NULL;
	}
	if (m_pResampleBuf!=NULL)
	{
		free(m_pResampleBuf);
		m_pResampleBuf=NULL;
	}

	//{
	//	XAutoLock lock(m_csMapAudioPacket);
	//	MapAudioPacket::iterator it = m_mapAudioPacket.begin();
	//	while(it != m_mapAudioPacket.end())
	//	{
	//		XDataBuffer*pXDataBuffer = it->second;
	//		if (pXDataBuffer)
	//		{
	//			delete pXDataBuffer;
	//			pXDataBuffer=NULL;
	//		}
	//		it++;
	//	}
	//	m_mapAudioPacket.empty();
	//}
}

int AudioTranscode::TranscodePacketData(void*pPacketData,int nPacketLen)
{
	unsigned short nSequence=AUDEC_HEADER_GET_SEQUENCE(pPacketData);
	unsigned short nDelta = nSequence - m_nSequence;
	bool bDiscontinious=(nDelta>1);
	m_nSequence = nSequence;
	return DoTranscodePacketData((char *)pPacketData,nPacketLen,bDiscontinious);
	//{
	//	XAutoLock lock(m_csMapAudioPacket);
	//	if (m_mapAudioPacket.find(nSequence) == m_mapAudioPacket.end())
	//	{
	//		XDataBuffer*pXDataBuffer=new XDataBuffer();
	//		pXDataBuffer->StoreData((char*)pPacketData,nPacketLen);
	//		m_mapAudioPacket[nSequence] = pXDataBuffer;
	//	}
	//}
	return -1;
}

//void AudioTranscode::ThreadProcMain(void)
//{
//	while(!m_bWaitForStop)
//	{
//		{
//			XAutoLock lock(m_csMapAudioPacket);
//			MapAudioPacket::iterator it = m_mapAudioPacket.begin();
//			if(it != m_mapAudioPacket.end())
//			{
//				XDataBuffer*pXDataBuffer = it->second;
//				if (pXDataBuffer)
//				{
//					bool bDiscontinious=false;
//					bool bDecode = false;
//					unsigned short nSequence=AUDEC_HEADER_GET_SEQUENCE(pXDataBuffer->GetData());
//					if (nSequence <= m_nSequence) //来了一个比较小的包，直接丢弃
//					{
//						delete pXDataBuffer;
//						pXDataBuffer=NULL;
//						m_mapAudioPacket.erase(it);
//						continue;
//					}
//					unsigned short nSequenceTemp = nSequence - m_nSequence;
//					if (( nSequenceTemp == 1) || m_nSequence == 0 ) //连续的包，直接送解码
//					{
//						bDecode = true;
//						bDiscontinious = false;
//					}
//					else 
//					{
//						unsigned int  nTimeStamp = XGetTimestamp();
//						if (m_nTimeStamp == 0)
//						{
//							m_nTimeStamp = nTimeStamp;
//						}
//						else if (nTimeStamp - m_nTimeStamp > 1000) //不连续的包，如果时间超过1秒钟，送解码
//						{
//							bDecode = true;
//							bDiscontinious = true;
//						}
//					}
//
//					if (bDecode)
//					{
//						m_nSequence = nSequence;
//						m_nTimeStamp= 0;
//						//解码
//						DoTranscodePacketData(pXDataBuffer->GetData(),pXDataBuffer->GetLen(),bDiscontinious);
//						delete pXDataBuffer;
//						pXDataBuffer=NULL;
//						m_mapAudioPacket.erase(it);
//					}
//				}
//			}
//		}
//		XSleep(1);
//	}
//}

int AudioTranscode::DoTranscodePacketData(char*pPacketData,int nPacketLen,bool bDiscontinious)
{
	X_AUDIO_CODEC_ID nAudioCodecID=(X_AUDIO_CODEC_ID)AUDEC_HEADER_GET_CODEC_ID(pPacketData);

	if (m_pHPATCDecoder && m_nDecodeCodecID != nAudioCodecID)
	{
		m_pHPATCDecoder->ReleaseConnections();
		delete m_pHPATCDecoder;
		m_pHPATCDecoder = NULL;
	}
	m_nDecodeCodecID = nAudioCodecID;

	if (m_pHPATCDecoder == NULL)
	{
		switch (nAudioCodecID)
		{
		case X_AUDIO_CODEC_SIREN_G7221_7_24:
		case X_AUDIO_CODEC_G7221_7_24://16khz
		case X_AUDIO_CODEC_SIREN_G7221_7_32:
		case X_AUDIO_CODEC_G7221_7_32://16khz
		case X_AUDIO_CODEC_SIREN_G7221_14_24:
		case X_AUDIO_CODEC_G7221_14_24://32khz
		case X_AUDIO_CODEC_AUDEC_24:
		case X_AUDIO_CODEC_SIREN_G7221_14_32:
		case X_AUDIO_CODEC_G7221_14_32:
		case X_AUDIO_CODEC_SIREN_G7221_14_48:
		case X_AUDIO_CODEC_G7221_14_48:
			{
				m_pHPATCDecoder = HPATCDecoder::CreateG7221Decoder(*this);	
			}
			break;
		case X_AUDIO_CODEC_G719_32:
		case X_AUDIO_CODEC_G719_48:
		case X_AUDIO_CODEC_G719_64:
			{
				m_pHPATCDecoder= HPATCDecoder::CreateG719Decoder(*this);
			}
			break;
		case X_AUDIO_CODEC_OPUS_FWB_24:
		case X_AUDIO_CODEC_OPUS_FWB_48:
			{
				m_pHPATCDecoder= HPATCDecoder::CreateOPUSDecoder(*this);
			}
			break;
		case X_AUDIO_CODEC_G729:
			{
				m_pHPATCDecoder= HPATCDecoder::CreateG729Decoder(*this);
			}
			break;	
		case X_AUDIO_CODEC_GSM:
			{
				m_pHPATCDecoder= HPATCDecoder::CreateGSMDecoder(*this);
			}
			break;	
		case X_AUDIO_CODEC_G711U:
			{
			m_pHPATCDecoder= HPATCDecoder::CreateG711UDecoder(*this);
			}
			break;	
		case X_AUDIO_CODEC_G711A:
			{
				m_pHPATCDecoder= HPATCDecoder::CreateG711ADecoder(*this);
			}
			break;	
		}

		if (m_pHPATCDecoder)
		{
			HPATC_CodecInst codecInst;
			HPATCGetCodecInstByID(m_nDecodeCodecID,&codecInst);
			printf("HPATC::CreateDecoder CodecID = %d CodecInst = %s\n",nAudioCodecID,codecInst.desc);
			m_pHPATCDecoder->Connect(codecInst.pl_freq,codecInst.framesize,codecInst.bitrate);
		}
		else
		{
			printf("HPATC::CreateDecoder Invalid CodecID = %d\n",nAudioCodecID);
		}
	}

	if (m_pHPATCDecoder)
	{
		unsigned int nTimeStamp = AUDEC_HEADER_GET_TIMESTAMP((void *)pPacketData);
		int nHeaderSize=AUDEC_HEADER_GET_LEN(pPacketData);
		unsigned char*pEncoded=(unsigned char*)pPacketData+nHeaderSize;
		int nEncodedLen=nPacketLen-nHeaderSize;

		// {{ liurui : fixed Polycom m100 bug
		if(( X_AUDIO_CODEC_G719_48 == nAudioCodecID ) && (nEncodedLen - 120) == 2 )
			pEncoded = pEncoded + 2;
		else if(( X_AUDIO_CODEC_G719_64 == nAudioCodecID ) && (nEncodedLen - 160) == 2 )
			pEncoded = pEncoded + 2;
		else if(( X_AUDIO_CODEC_G719_32 == nAudioCodecID ) && (nEncodedLen - 80) == 2 )
			pEncoded = pEncoded + 2;
		// }} liurui
		m_pHPATCDecoder->InputStream(pEncoded,nEncodedLen,bDiscontinious,nTimeStamp);
		
		return 0;
	}
	return -1;
}

int AudioTranscode::TranscodePCMData(short*pSamples,int nSamples,int nSampleRate,unsigned long ulTimestamp)
{
	OnHPATCDecoderNotifyOutput(pSamples, nSamples, nSampleRate,false,ulTimestamp);
	return -1;
}

void AudioTranscode::OnHPATCDecoderNotifyOutput(short*pSamples,int nSamples,int nSampleRate,bool bDiscontinious,unsigned long ulTimestamp)
{
	if (m_nDecodeSampleRate != nSampleRate)
	{
		m_nDecodeSampleRate = nSampleRate;
		if (m_pHPATCResample)
		{
			m_pHPATCResample->ReleaseConnections();
			delete m_pHPATCResample;
			m_pHPATCResample = NULL;
		}
	}

	if (m_pHPATCResample == NULL && nSampleRate != m_nEncodeSampleRate)
	{
		m_pHPATCResample = HPATCResample::Create();
		if (m_pHPATCResample!=NULL)
		{
			if (0!=m_pHPATCResample->Connect(nSampleRate,m_nEncodeSampleRate))
			{
				m_pHPATCResample->ReleaseConnections();
				delete m_pHPATCResample;
				m_pHPATCResample=NULL;
			}
		}
	}

	short*pOutSamples;
	int  nOutSamples;

	if (m_pHPATCResample)
	{
		int nResampleBufLen=((nSamples<<1)*m_nEncodeSampleRate/nSampleRate)*2;
		if (nResampleBufLen>m_nResampleBufLen || m_pResampleBuf==NULL)
		{
			m_nResampleBufLen=nResampleBufLen;
			if (m_pResampleBuf!=NULL)
			{
				free(m_pResampleBuf);
				m_pResampleBuf=NULL;
			}
			m_pResampleBuf=(unsigned char*)malloc(m_nResampleBufLen);
		}

		if (m_pResampleBuf!=NULL)
		{
			short*pOutSamples=(short*)m_pResampleBuf;
			int nOutSamples=0;
			if(0==m_pHPATCResample->Convert((short*)pSamples,nSamples,pOutSamples,nOutSamples))
			{
				pSamples=pOutSamples;
				nSamples=nOutSamples;
			}
		}
	}

	if (m_pHPATCEncoder)
	{
		m_pHPATCEncoder->InputSamples(pSamples,nSamples,ulTimestamp);
	}
}

void AudioTranscode::OnHPATCEncoderNotifyOutput(unsigned char*pEncoded,int nLen,unsigned long ulTimestamp)
{
	if (m_pCallback)
	{
		m_pCallback->OnHPATC_TranscodeCallbackPacketData(this,pEncoded, nLen,ulTimestamp);
	}

	if (m_cb_aac_data)
	{
		m_cb_aac_data(m_pUserData,this,pEncoded, nLen,ulTimestamp);
	}
}