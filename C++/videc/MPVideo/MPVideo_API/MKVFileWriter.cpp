//
// MKVFileWriter
//
#include "MKVFileWriter.h"
#include <MPVideo/MPVIDEO_Header.h>
#include <stdio.h>
#include <stdlib.h>



MKVFileWriter::MKVFileWriter(void) 
:m_pMKVHandle(NULL)
,m_bOpenVideoStream(false)
,m_bOpenAudioStream(false)

, m_nSampleNumber(0)
, m_nSampleRate(0)
, m_bGotFirstAudio(false)
, m_nLastAudioTimestamp(0)

, m_nVideoFrameRate(100)
, m_nWidth(0)
, m_nHeight(0)
, m_bGotFirstVideo(false)

, m_bGotFirstTimestamp(false)
, m_ulFirstTimestamp(0)

, m_pLameEncoder(NULL)
, m_nStreamSizeAudio(0)
, m_pInSamples(NULL)
, m_pStreamDataBuffer(NULL)

, m_strPathName("")
{
#ifdef WRITE_RAW_DATA
	m_pFileRawData=NULL;
#endif
}

MKVFileWriter::~MKVFileWriter(void)
{
}

int MKVFileWriter::Open(const char* cszPathName)
{
	if (m_pMKVHandle!=NULL)
	{
		return -1;
	}

	std::string strPathName=cszPathName;
	strPathName=strPathName.substr(0,strPathName.length()-3);
	strPathName+="mkv";

	::DeleteFile( strPathName.c_str() );

	m_pMKVHandle=VIDEC_MKV_Open(strPathName.c_str());
	if (m_pMKVHandle==NULL)
	{
		return -1;
	}

	m_strPathName=strPathName;

#ifdef WRITE_RAW_DATA
	m_pFileRawData=fopen("d:\\data.raw","w+b");
#endif

	return	0;
}

void	MKVFileWriter::Close()
{
	if (m_pMKVHandle!=NULL)
	{
		VIDEC_MKV_Close(m_pMKVHandle);
		m_pMKVHandle=NULL;
	}


	if (m_pLameEncoder!=NULL)
	{
		lame_close(m_pLameEncoder);
		m_pLameEncoder=NULL;
	}

	if (m_pInSamples!=NULL)
	{
		free(m_pInSamples);
		m_pInSamples=NULL;
	}

	if (m_pStreamDataBuffer!=NULL)
	{
		free(m_pStreamDataBuffer);
		m_pStreamDataBuffer=NULL;
	}

	if (m_strPathName.size()>0)
	{
		FILE*pFile=fopen(m_strPathName.c_str(),"rb");
		if (pFile!=NULL)
		{
			fseek(pFile,0L,SEEK_END);
			long nLen=ftell(pFile);
			fclose(pFile);
			pFile=NULL;
			if (nLen<=32*1024)
			{
				::DeleteFile( m_strPathName.c_str() );
			}
		}
	}


#ifdef WRITE_RAW_DATA
	if (m_pFileRawData)
	{
		fclose(m_pFileRawData);
		m_pFileRawData=NULL;
	}
#endif
}

bool MKVFileWriter::WriteVideo( unsigned char* pVideoData, int nVideoLen )
{
	unsigned short nKeyFrame=MPVIDEO_HEADER_EXT_GET_KEYFRAME(pVideoData);
	unsigned long ulTimestamp=MPVIDEO_HEADER_EXT_GET_TIMESTAMP(pVideoData);
	int nHeaderLen=MPVIDEO_HEADER_EXT_GET_LEN(pVideoData);

	if (m_pMKVHandle)
	{
		VIDEC_MKV_WriteVideo(m_pMKVHandle,pVideoData+nHeaderLen,nVideoLen-nHeaderLen,ulTimestamp,nKeyFrame);
	}

#ifdef WRITE_RAW_DATA
	if (m_pFileRawData)
	{
		RAW_DATA_HEADER tHeader;
		tHeader.nDataType=0;
		tHeader.nKeyFrame=(nKeyFrame?1:0);
		tHeader.nTimestamp=ulTimestamp;
		tHeader.nDataLen=nVideoLen-nHeaderLen;
		fwrite(&tHeader,1,sizeof(tHeader),m_pFileRawData);
		fwrite(pVideoData+nHeaderLen,1,nVideoLen-nHeaderLen,m_pFileRawData);
	}
#endif

	return 0;
}



int MKV_ConvertBytesToInt(const unsigned char * inbuf,const unsigned int & inlen,int bytes_per_sample,int * intbuf)
{
	int i;
	memcpy(intbuf,inbuf,inlen);
	int *op;			/* output pointer */
	unsigned char *ip = (unsigned char *) intbuf; /* input pointer */
	const int b = sizeof(int) * 8;
	int nSamplesRead = inlen / bytes_per_sample;
	op = intbuf + nSamplesRead;

#define GA_URS_IFLOOP( ga_urs_bps ) \
	if( bytes_per_sample == ga_urs_bps ) \
	for( i = nSamplesRead * bytes_per_sample; (i -= bytes_per_sample) >=0;)

	GA_URS_IFLOOP( 1 )
		*--op = (ip[i] ^ 0x80)<<(b-8) | 0x7f<<(b-16);/* convert from unsigned*/
	GA_URS_IFLOOP( 2 )
		*--op = ip[i]<<(b-16) | ip[i+1]<<(b-8); 
	GA_URS_IFLOOP( 3 )
		*--op = ip[i]<<(b-24) | ip[i+1]<<(b-16) | ip[i+2]<<(b-8);
	GA_URS_IFLOOP( 4 )
		*--op = ip[i]<<(b-32) | ip[i+1]<<(b-24) | ip[i+2]<<(b-16) | ip[i+3] << (b-8);
#undef GA_URS_IFLOOP

	return nSamplesRead;
}

bool MKVFileWriter::WriteAudio( unsigned char* pData, const int nLen ,unsigned long ulTimestamp,int&nBytes)
{
	if (m_pMKVHandle==NULL)
		return false;

	if (!m_bOpenVideoStream)
	{
		return false;
	}

	if( m_pInSamples==NULL || 
		m_pStreamDataBuffer==NULL)
	{
		return	false;
	}


	short*pSamples=(short*)pData;
	int nSamples=(nLen>>1);

	int nStreamDataBufLen=2*m_nSampleRate;

	int nSamplesRead = MKV_ConvertBytesToInt((unsigned char*)pSamples,nSamples<<1,2,m_pInSamples);


	int nStreamSize = lame_encode_buffer_int(m_pLameEncoder, m_pInSamples, m_pInSamples, nSamplesRead, m_pStreamDataBuffer, nStreamDataBufLen);

	nBytes=nStreamSize;

	if (m_pMKVHandle && nStreamSize>0)
	{
		VIDEC_MKV_WriteAudio(m_pMKVHandle,m_pStreamDataBuffer,nStreamSize,ulTimestamp);
	}

#ifdef WRITE_RAW_DATA
	if (m_pFileRawData)
	{
		RAW_DATA_HEADER tHeader;
		tHeader.nDataType=1;
		tHeader.nKeyFrame=1;
		tHeader.nTimestamp=ulTimestamp;
		tHeader.nSamples=nSamples;
		tHeader.nDataLen=nStreamSize;
		fwrite(&tHeader,1,sizeof(tHeader),m_pFileRawData);
		fwrite(m_pStreamDataBuffer,1,nStreamSize,m_pFileRawData);
	}
#endif

	return	true;
}


int MKVFileWriter::OpenAudioStream(int nSampleRate)
{
	if (m_pMKVHandle==NULL)
	{
		return -1;
	}


	m_nSampleRate = nSampleRate;
	m_nSampleNumber = 0;

	if (m_pInSamples==NULL)
	{
		m_pInSamples=(int*)malloc(2*nSampleRate*sizeof(int));
		if (m_pInSamples==NULL)
		{
			return -1;
		}
	}

	if (m_pStreamDataBuffer==NULL)
	{
		m_pStreamDataBuffer=(unsigned char*)malloc(2*nSampleRate);
		if (m_pStreamDataBuffer==NULL)
		{
			return -1;
		}
	}

	m_nStreamSizeAudio = 0;
	int nBitrate=32;

	m_pLameEncoder=lame_init();

	lame_set_num_samples( m_pLameEncoder, 0xFFFFFFFF );
	if( -1 == lame_set_num_channels( m_pLameEncoder, 2 ) ) 
		return -1;
	lame_set_in_samplerate( m_pLameEncoder, nSampleRate );

	lame_set_out_samplerate(m_pLameEncoder, nSampleRate);
	lame_set_quality(m_pLameEncoder, 7);
    /* lame 3.91 doesn't work in mono */
    lame_set_mode(m_pLameEncoder, JOINT_STEREO);
    lame_set_brate(m_pLameEncoder, nBitrate);
	lame_set_bWriteVbrTag(m_pLameEncoder,0);


	if(lame_init_params(m_pLameEncoder) < 0)
		return -1;

	if (m_bOpenAudioStream==false)
	{
		VIDEC_MKV_SetAudio(m_pMKVHandle,32000,nSampleRate,2,16,AUDIO_MP3);
		m_bOpenAudioStream=true;
	}

	return 0;
}

int MKVFileWriter::OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC)
{
	if (m_pMKVHandle==NULL)
	{
		return -1;
	}

	VIDEO_TYPE nType=VIDEO_H264;
	if (FourCC==mmioFOURCC('H','2','6','4') || FourCC==mmioFOURCC('A','V','C','1'))
	{
		nType=VIDEO_H264;
	}
	else if (FourCC==mmioFOURCC('H','2','6','3'))
	{
		nType=VIDEO_H263;
	}

	if (m_bOpenAudioStream==false)
	{
		VIDEC_MKV_SetAudio(m_pMKVHandle,32000,22050,2,16,AUDIO_MP3);
		m_bOpenAudioStream=true;
	}


	VIDEC_MKV_SetVideo(m_pMKVHandle,60,nWidth,nHeight,nType);

	m_bOpenVideoStream=true;

	return 0;
}