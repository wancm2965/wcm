//
// MP3FileWriter
//
#include "MP3FileWriter.h"
#include <MPVideo/MPVIDEO_Header.h>
#include <stdio.h>
#include <stdlib.h>



MP3FileWriter::MP3FileWriter(void) 
:m_pFile(NULL)
,m_bOpenAudioStream(false)

, m_nSampleNumber(0)
, m_nSampleRate(0)
, m_bGotFirstAudio(false)
, m_nLastAudioTimestamp(0)

, m_bGotFirstTimestamp(false)
, m_ulFirstTimestamp(0)

, m_pLameEncoder(NULL)
, m_nStreamSizeAudio(0)
, m_pInSamples(NULL)
, m_pStreamDataBuffer(NULL)

, m_strPathName("")
{
}

MP3FileWriter::~MP3FileWriter(void)
{
}

int MP3FileWriter::Open(const char* cszPathName)
{
	std::string strPathName=cszPathName;
	strPathName=strPathName.substr(0,strPathName.length()-3);
	strPathName+="mp3";

	::DeleteFile( strPathName.c_str() );

	m_strPathName=strPathName;

	m_pFile=fopen(strPathName.c_str(),"w+b");
	if (m_pFile==NULL)
	{
		-1;
	}

	return	0;
}

void	MP3FileWriter::Close()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile=NULL;
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
			if (nLen<=1*1024)
			{
				::DeleteFile( m_strPathName.c_str() );
			}
		}
	}
}

bool MP3FileWriter::WriteVideo( unsigned char* pVideoData, int nVideoLen )
{
	return false;
}



int MP3_ConvertBytesToInt(const unsigned char * inbuf,const unsigned int & inlen,int bytes_per_sample,int * intbuf)
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

bool MP3FileWriter::WriteAudio( unsigned char* pData, const int nLen ,unsigned long ulTimestamp,int&nBytes)
{
	if (m_pFile==NULL)
		return false;

	if( m_pInSamples==NULL || 
		m_pStreamDataBuffer==NULL)
	{
		return	false;
	}


	short*pSamples=(short*)pData;
	int nSamples=(nLen>>1);

	int nStreamDataBufLen=2*m_nSampleRate;

	int nSamplesRead = MP3_ConvertBytesToInt((unsigned char*)pSamples,nSamples<<1,2,m_pInSamples);


	int nStreamSize = lame_encode_buffer_int(m_pLameEncoder, m_pInSamples, m_pInSamples, nSamplesRead, m_pStreamDataBuffer, nStreamDataBufLen);

	nBytes=nStreamSize;

	if (nStreamSize>0)
	{
		fwrite(m_pStreamDataBuffer,1,nStreamSize,m_pFile);
	}

	return	true;
}


int MP3FileWriter::OpenAudioStream(int nSampleRate)
{
	if (m_pFile==NULL)
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

	m_bOpenAudioStream=true;

	return 0;
}

int MP3FileWriter::OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC)
{
	return -1;
}