//
// AVIFileWriter
//
#include "AVIFileWriter.h"
#include <VIDEC/VIDEC_Header.h>
#include <stdio.h>
#include <stdlib.h>
#include "../VIDEC_FRE/VIDECFREAPI.h"


#ifdef AVI_MP3
unsigned char MP3_HEADER[] = {0x01,0x00,0x02,0x00,0x00,0x00,0xDB,0x02,0x01,0x00,0x00,0x00};
#endif


AVIFileWriter::AVIFileWriter(void) 
#ifdef AVILIB_H
: m_pAVI(NULL)
#else
: m_pAVIFile(NULL)
, m_pAudioStream(NULL)
, m_pVideoStream(NULL)
#endif

, m_nSampleNumber(0)
, m_nSampleRate(0)
, m_bGotFirstAudio(false)
, m_nLastAudioTimestamp(0)

, m_lVideoFrameIndex(0)
, m_nVideoFrameRate(100)
, m_nVideoFrameInterval(10)
, m_nWidth(0)
, m_nHeight(0)
, m_bGotFirstVideo(false)

, m_bGotFirstTimestamp(false)
, m_ulFirstTimestamp(0)

#ifdef AVI_MP3
, m_pLameEncoder(NULL)
, m_nStreamSizeAudio(0)
, m_pInSamples(NULL)
, m_pStreamDataBuffer(NULL)
#endif

, m_strPathName("")
{
#ifdef WRITE_RAW_DATA
	m_pFileRawData=NULL;
#endif
}

AVIFileWriter::~AVIFileWriter(void)
{
}

int AVIFileWriter::Open(const char* cszPathName)
{
	::DeleteFile( cszPathName );

#ifdef AVILIB_H
	if (m_pAVI!=NULL)
	{
		return -1;
	}

	m_pAVI=AVI_open_output_file((char*)cszPathName);
	if (m_pAVI==NULL)
	{
		return -1;
	}
#else
	if (m_pAVIFile!=NULL)
	{
		return -1;
	}

	if( ::AVIFileOpen( &m_pAVIFile, cszPathName, OF_WRITE | OF_CREATE, NULL ) ) 
	{
		return	-1;
	}
#endif

	m_strPathName=cszPathName;

#ifdef WRITE_RAW_DATA
	m_pFileRawData=fopen("d:\\data.raw","w+b");
#endif

	return	0;
}

void	AVIFileWriter::Close()
{
#ifdef AVILIB_H
	if (m_pAVI!=NULL)
	{
		AVI_close(m_pAVI);
		m_pAVI=NULL;
	}
#else
	if( m_pAudioStream ) 
	{
		::AVIStreamClose( m_pAudioStream );
		m_pAudioStream = NULL;
	}

	if( m_pVideoStream ) 
	{
		::AVIStreamClose( m_pVideoStream );
		m_pVideoStream = NULL;
	}

	if( m_pAVIFile ) 
	{
		::AVIFileClose( m_pAVIFile );
		m_pAVIFile = NULL;
	}
#endif

#ifdef AVI_MP3
	if (m_pLameEncoder!=NULL)
	{
		lame_close(m_pLameEncoder);
		m_pLameEncoder=NULL;
	}

	if (m_pInSamples!=NULL)
	{
		FREEMSG(m_pInSamples);
		m_pInSamples=NULL;
	}

	if (m_pStreamDataBuffer!=NULL)
	{
		FREEMSG(m_pStreamDataBuffer);
		m_pStreamDataBuffer=NULL;
	}
#endif

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

bool AVIFileWriter::WriteVideo( unsigned char* pVideoData, int nVideoLen )
{
	bool bKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pVideoData)!=0;
	unsigned long ulTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pVideoData);
	if (!m_bGotFirstTimestamp)
	{
		if (ulTimestamp==0)
		{
			return true;
		}

		m_bGotFirstTimestamp=true;
		m_ulFirstTimestamp=ulTimestamp;
	}

	if (!m_bGotFirstVideo)
	{
		unsigned long ulOffset=ulTimestamp-m_ulFirstTimestamp;
		
		//char szDebug[1024];
		//sprintf(szDebug,"m_ulFirstTimestamp=%u ulTimestamp=%u ulOffset=%u\n",m_ulFirstTimestamp,ulTimestamp,ulOffset);
		//DebugOutput(szDebug);
		
		if (ulOffset>0x7FFFFFFF)
		{
			ulOffset=m_ulFirstTimestamp-ulTimestamp;
			if (ulOffset>4000)
			{
				return false;
			}

			//∏≤∏«“‘«∞µƒ“Ù∆µ
			m_bGotFirstAudio=false;
			m_ulFirstTimestamp=ulTimestamp;
		}

		m_bGotFirstVideo=true;
	}

	unsigned long ulDuration=ulTimestamp-m_ulFirstTimestamp;
	LONG lFrameIndex=(LONG)(ulDuration/m_nVideoFrameInterval);
	if (lFrameIndex<=m_lVideoFrameIndex)
		lFrameIndex=m_lVideoFrameIndex+1;
	m_lVideoFrameIndex=lFrameIndex;

#ifdef WRITE_RAW_DATA
	if (m_pFileRawData)
	{
		RAW_DATA_HEADER tHeader;
		tHeader.nDataType=0;
		tHeader.nKeyFrame=(bKeyFrame?1:0);
		tHeader.nTimestamp=ulTimestamp;
		tHeader.nDataLen=nVideoLen;
		fwrite(&tHeader,1,sizeof(tHeader),m_pFileRawData);
		fwrite(pVideoData,1,nVideoLen,m_pFileRawData);
	}
#endif
	//char szDebug[1024];
	//sprintf(szDebug,"WriteVideo m_lVideoFrameIndex=%d m_ulFirstTimestamp=%u ulTimestamp=%u ulDuration=%u\n",m_lVideoFrameIndex,m_ulFirstTimestamp,ulTimestamp,ulDuration);
	//DebugOutput(szDebug);

	return WriteVideo(pVideoData,nVideoLen,bKeyFrame,m_lVideoFrameIndex);
}

bool AVIFileWriter::WriteVideo( unsigned char* pVideoData, int nVideoLen, bool bKeyFrame, LONG lVideoFrameIndex )
{
#ifdef AVILIB_H
	if (m_pAVI==NULL)
	{
		return false;
	}

	AVI_write_frame(m_pAVI,(char*)pVideoData,nVideoLen,bKeyFrame?1:0,lVideoFrameIndex);
#else
	if( m_pVideoStream==NULL )
		return	false;

	LONG lSampWritten=0;  
	LONG lBytesWritten=0;  

	AVIStreamWrite(m_pVideoStream, 
				lVideoFrameIndex, 
				1, 
				pVideoData, 
				nVideoLen, 
				(bKeyFrame?AVIIF_KEYFRAME:0),
				&lSampWritten, 
				&lBytesWritten); 
#endif
	return true;
}

#ifdef AVI_MP3
int ConvertBytesToInt(const unsigned char * inbuf,const unsigned int & inlen,int bytes_per_sample,int * intbuf)
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
#endif

bool AVIFileWriter::WriteAudio( unsigned char* pData, const int nLen ,unsigned long ulTimestamp,int&nBytes)
{
#ifdef WRITE_RAW_DATA
	//if (m_pFileRawData)
	//{
	//	RAW_DATA_HEADER tHeader;
	//	tHeader.nDataType=1;
	//	tHeader.nKeyFrame=1;
	//	tHeader.nTimestamp=ulTimestamp;
	//	tHeader.nDataLen=nLen;
	//	fwrite(&tHeader,1,sizeof(tHeader),m_pFileRawData);
	//	fwrite(pData,1,nLen,m_pFileRawData);
	//}
	unsigned char*pMP3Data[2048];
	int nMP3DataLen=0;
#endif

	nBytes=nLen;

#ifdef AVILIB_H
	if (m_pAVI==NULL)
		return false;
#else
	if( m_pAudioStream==NULL )
		return	false;
#endif

#ifdef AVI_MP3
	if( m_pInSamples==NULL || 
		m_pStreamDataBuffer==NULL)
	{
		return	false;
	}
#endif

	if (!m_bGotFirstTimestamp)
	{
		if (ulTimestamp==0)
		{
			return true;
		}
		m_bGotFirstTimestamp=true;
		m_ulFirstTimestamp=ulTimestamp;
	}

	LONG lSampWritten=0;  
	LONG lBytesWritten=0;  
	int nSamples=nLen >> 1;
	LONG lFrameIndex=0;

	if (!m_bGotFirstAudio)
	{
		unsigned long ulDuration=ulTimestamp-m_ulFirstTimestamp;
		if (ulDuration>0x7FFFFFFF)
		{
			return false;
		}

		m_bGotFirstAudio=true;

		lFrameIndex=(LONG)(ulDuration*(m_nSampleRate/1000));
		if (lFrameIndex>nSamples)
		{
			lFrameIndex-=nSamples;
		}
		else
		{
			lFrameIndex=0;
		}

		LONG nDelta=lFrameIndex;
		short*pSilenceSamples=(short*)MALLOCMSG(m_nSampleRate*2);
		if (pSilenceSamples!=NULL)
		{
			memset(pSilenceSamples,0,m_nSampleRate*2);

			LONG nLeft=nDelta;
			LONG nTempSamples=0;
			while (nLeft>0)
			{
				nTempSamples=((nLeft>m_nSampleRate)?m_nSampleRate:nLeft);
				nLeft-=nTempSamples;

#ifdef AVI_MP3
				int nStreamDataBufLen=2*m_nSampleRate;

				int nSamplesRead = ConvertBytesToInt((unsigned char*)pSilenceSamples,nTempSamples<<1,2,m_pInSamples);

				int*p = m_pInSamples + nSamplesRead;

				int nStreamSize = lame_encode_buffer_int(m_pLameEncoder, m_pInSamples, m_pInSamples, nSamplesRead,
					m_pStreamDataBuffer, nStreamDataBufLen);

#ifdef AVILIB_H
				AVI_write_audio(m_pAVI,(char*)m_pStreamDataBuffer,nStreamSize);

#ifdef WRITE_RAW_DATA
				memcpy(pMP3Data+nMP3DataLen,m_pStreamDataBuffer,nStreamSize);
				nMP3DataLen+=nStreamSize;
#endif

#else
				AVIStreamWrite(m_pAudioStream,
							   m_nStreamSizeAudio ,
							   nStreamSize,
							   m_pStreamDataBuffer,
							   nStreamSize,
							   0,
							   &lSampWritten, 
							   &lBytesWritten); 
#endif

				m_nStreamSizeAudio+=nStreamSize;

				nBytes=nStreamSize;
#else
#ifdef AVILIB_H
				AVI_write_audio(m_pAVI,(char*)pSilenceSamples,nTempSamples<<1);
#else
				AVIStreamWrite(m_pAudioStream,
							   m_nSampleNumber ,
							   nTempSamples,
							   pSilenceSamples,
							   nTempSamples<<1,
							   0,
							   &lSampWritten, 
							   &lBytesWritten); 
#endif

#endif
				m_nSampleNumber+=nTempSamples;
			}

			FREEMSG(pSilenceSamples);
			pSilenceSamples=NULL;
		}
	}
	else
	{
		unsigned long ulAudioPacketOffset=ulTimestamp-m_nLastAudioTimestamp;
		unsigned long ulDuration=ulTimestamp-m_ulFirstTimestamp;
		lFrameIndex=(LONG)(ulDuration*(m_nSampleRate/1000));
		if (lFrameIndex>nSamples)
		{
			lFrameIndex-=nSamples;
		}
		else
		{
			lFrameIndex=0;
		}

		if (m_nSampleNumber<(lFrameIndex-(m_nSampleRate>>1)) || (m_nSampleNumber<lFrameIndex && ulAudioPacketOffset>=180))
		{
			//char szDebug[1024];
			//sprintf(szDebug,"11 m_nSampleNumber=%d lFrameIndex=%d m_lVideoFrameIndex=%d ulAudioPacketOffset=%u\n",m_nSampleNumber,lFrameIndex,m_lVideoFrameIndex,ulAudioPacketOffset);
			//DebugOutput(szDebug);

			LONG nDelta=lFrameIndex-m_nSampleNumber;
			short*pSilenceSamples=(short*)MALLOCMSG(m_nSampleRate*2);
			if (pSilenceSamples!=NULL)
			{
				memset(pSilenceSamples,0,m_nSampleRate*2);

				LONG nLeft=nDelta;
				LONG nTempSamples=0;
				while (nLeft>0)
				{
					nTempSamples=((nLeft>m_nSampleRate)?m_nSampleRate:nLeft);
					nLeft-=nTempSamples;

#ifdef AVI_MP3
					int nStreamDataBufLen=2*m_nSampleRate;

					int nSamplesRead = ConvertBytesToInt((unsigned char*)pSilenceSamples,nTempSamples<<1,2,m_pInSamples);

					int*p = m_pInSamples + nSamplesRead;

					int nStreamSize = lame_encode_buffer_int(m_pLameEncoder, m_pInSamples, m_pInSamples, nSamplesRead, m_pStreamDataBuffer, nStreamDataBufLen);

#ifdef AVILIB_H
					AVI_write_audio(m_pAVI,(char*)m_pStreamDataBuffer,nStreamSize);

#ifdef WRITE_RAW_DATA
					memcpy(pMP3Data+nMP3DataLen,m_pStreamDataBuffer,nStreamSize);
					nMP3DataLen+=nStreamSize;
#endif

#else
					AVIStreamWrite(m_pAudioStream,
								   m_nStreamSizeAudio ,
								   nStreamSize,
								   m_pStreamDataBuffer,
								   nStreamSize,
								   0,
								   &lSampWritten, 
								   &lBytesWritten); 
#endif

					m_nStreamSizeAudio+=nStreamSize;

					nBytes=nStreamSize;
#else
#ifdef AVILIB_H
					AVI_write_audio(m_pAVI,(char*)pSilenceSamples,nTempSamples<<1);
#else
					AVIStreamWrite(m_pAudioStream,
								   m_nSampleNumber ,
								   nTempSamples,
								   pSilenceSamples,
								   nTempSamples<<1,
								   0,
								   &lSampWritten, 
								   &lBytesWritten); 
#endif
#endif

					m_nSampleNumber+=nTempSamples;
				}

				FREEMSG(pSilenceSamples);
				pSilenceSamples=NULL;
			}
		}
	}

#ifdef AVI_MP3

	short*pSilenceSamples=(short*)pData;
	int nTempSamples=(nLen>>1);

	int nStreamDataBufLen=2*m_nSampleRate;

	int nSamplesRead = ConvertBytesToInt((unsigned char*)pSilenceSamples,nTempSamples<<1,2,m_pInSamples);

	int*p = m_pInSamples + nSamplesRead;

	int nStreamSize = lame_encode_buffer_int(m_pLameEncoder, m_pInSamples, m_pInSamples, nSamplesRead, m_pStreamDataBuffer, nStreamDataBufLen);

#ifdef AVILIB_H
	AVI_write_audio(m_pAVI,(char*)m_pStreamDataBuffer,nStreamSize);

#ifdef WRITE_RAW_DATA
	memcpy(pMP3Data+nMP3DataLen,m_pStreamDataBuffer,nStreamSize);
	nMP3DataLen+=nStreamSize;
#endif

#else
	AVIStreamWrite(m_pAudioStream,
				   m_nStreamSizeAudio ,
				   nStreamSize,
				   m_pStreamDataBuffer,
				   nStreamSize,
				   0,
				   &lSampWritten, 
				   &lBytesWritten); 
#endif

	m_nStreamSizeAudio+=nStreamSize;

	nBytes=nStreamSize;
#else

#ifdef AVILIB_H
	AVI_write_audio(m_pAVI,(char*)pData,nLen);
#else
	AVIStreamWrite(m_pAudioStream,
				   m_nSampleNumber ,
				   nSamples,
				   pData,
				   nLen,
				   0,
				   &lSampWritten, 
				   &lBytesWritten); 
#endif
#endif

#ifdef WRITE_RAW_DATA
	if (m_pFileRawData)
	{
		RAW_DATA_HEADER tHeader;
		tHeader.nDataType=1;
		tHeader.nKeyFrame=1;
		tHeader.nTimestamp=ulTimestamp;
		tHeader.nSamples=nSamples;
		tHeader.nDataLen=nMP3DataLen;
		fwrite(&tHeader,1,sizeof(tHeader),m_pFileRawData);
		fwrite(pMP3Data,1,nMP3DataLen,m_pFileRawData);
	}
#endif

	m_nSampleNumber+=nSamples;

	m_nLastAudioTimestamp=ulTimestamp;

	//char szDebug[1024];
	//sprintf(szDebug,"m_nSampleNumber=%d lFrameIndex=%d nSamples=%d m_lVideoFrameIndex=%d m_ulFirstTimestamp=%u\n",m_nSampleNumber,lFrameIndex,nSamples,m_lVideoFrameIndex,m_ulFirstTimestamp);
	//DebugOutput(szDebug);
	return	true;
}


int AVIFileWriter::OpenAudioStream(int nSampleRate)
{
#ifdef AVILIB_H
	if (m_pAVI==NULL)
	{
		return -1;
	}
#else
	if (m_pAVIFile==NULL)
	{
		return -1;
	}

	if (m_pAudioStream!=NULL)
	{
		return -1;
	}
#endif

	m_nSampleRate = nSampleRate;
	m_nSampleNumber = 0;

#ifdef AVI_MP3
	if (m_pInSamples==NULL)
	{
		m_pInSamples=(int*)MALLOCMSG(2*nSampleRate*sizeof(int));
		if (m_pInSamples==NULL)
		{
			return -1;
		}
	}

	if (m_pStreamDataBuffer==NULL)
	{
		m_pStreamDataBuffer=(unsigned char*)MALLOCMSG(2*nSampleRate);
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

#ifdef AVILIB_H
	AVI_set_audio(m_pAVI,2,nSampleRate,0,0x55,nBitrate);
#else
	PWAVEFORMATEX pWaveFormat=NULL;

	pWaveFormat=(PWAVEFORMATEX) MALLOCMSG(sizeof(WAVEFORMATEX)+12);
    pWaveFormat->wFormatTag      = 0x55;
	pWaveFormat->nChannels       = 2;
    pWaveFormat->nSamplesPerSec  = nSampleRate;
	pWaveFormat->nAvgBytesPerSec = nBitrate/8*1000;
	pWaveFormat->nBlockAlign     = 1;
	pWaveFormat->wBitsPerSample  = 0;
	pWaveFormat->cbSize          = 12;
	memcpy(((char *)pWaveFormat)+sizeof(WAVEFORMATEX),MP3_HEADER,12);

	AVISTREAMINFO astrinfo;
    memset(&astrinfo, 0, sizeof(AVISTREAMINFO));
    astrinfo.dwSuggestedBufferSize = 0;
    astrinfo.fccType               = streamtypeAUDIO;
    astrinfo.fccHandler            = 0;
    astrinfo.dwFlags               = 0;
    astrinfo.dwScale               = pWaveFormat->nBlockAlign;
    astrinfo.dwRate                = pWaveFormat->nAvgBytesPerSec;
    astrinfo.dwStart               = 0;
    astrinfo.dwLength              = -1;
    astrinfo.dwQuality             = -1;
    astrinfo.dwInitialFrames       = 0;
    astrinfo.dwSampleSize          = pWaveFormat->nBlockAlign;
    astrinfo.wLanguage             = 0;

	// Audio Stream
	if( ::AVIFileCreateStream( m_pAVIFile, &m_pAudioStream, &astrinfo ) ) 
	{
		return	-1;
	}

	if( ::AVIStreamSetFormat( m_pAudioStream, 0, pWaveFormat, sizeof(WAVEFORMATEX)+pWaveFormat->cbSize ) ) 
	{
		return	-1;
	}
#endif

#else

#ifdef AVILIB_H
	AVI_set_audio(m_pAVI,1,nSampleRate,16,WAVE_FORMAT_PCM,0);
#else
	WAVEFORMATEX wfx;
	wfx.wFormatTag      = WAVE_FORMAT_PCM;
	wfx.nSamplesPerSec  = nSampleRate;
	wfx.wBitsPerSample  = 16;
	wfx.nChannels       = 1;
	wfx.nBlockAlign     = wfx.nChannels * wfx.wBitsPerSample / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize          = 0;

	AVISTREAMINFO AudioStreamInfo;
	memset( &AudioStreamInfo, 0, sizeof(AudioStreamInfo) );
	AudioStreamInfo.fccType         = streamtypeAUDIO;
	AudioStreamInfo.fccHandler      = wfx.wFormatTag;
	AudioStreamInfo.dwScale         = wfx.nBlockAlign;
	AudioStreamInfo.dwInitialFrames = 1;
	AudioStreamInfo.dwRate          = wfx.nAvgBytesPerSec;
	AudioStreamInfo.dwQuality       = (DWORD)-1;
	AudioStreamInfo.dwSampleSize    = wfx.nBlockAlign;

	// Audio Stream
	if( ::AVIFileCreateStream( m_pAVIFile, &m_pAudioStream, &AudioStreamInfo ) ) 
	{
		return	-1;
	}

	if( ::AVIStreamSetFormat( m_pAudioStream, 0, &wfx, sizeof(WAVEFORMATEX) ) ) 
	{
		return	-1;
	}
#endif

#endif


	return 0;
}

int AVIFileWriter::OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC)
{
#ifdef AVILIB_H
	if (m_pAVI==NULL)
	{
		return -1;
	}

	char szFourCC[5]="";
	memcpy(szFourCC,&FourCC,4);
	szFourCC[4]='\0';

	if (FourCC==mmioFOURCC('H','2','6','4'))
	{
		strcpy(szFourCC,"H264");
	}
	else if (FourCC==mmioFOURCC('H','2','6','3'))
	{
		strcpy(szFourCC,"H263");
	}

	AVI_set_video(m_pAVI,nWidth,nHeight,100.00,szFourCC);
#else
	if (m_pAVIFile==NULL)
	{
		return -1;
	}

	if (m_pVideoStream!=NULL)
	{
		return -1;
	}

	if (nWidth<=0 || nHeight<=0)
	{
		return -1;
	}

	AVISTREAMINFO VideoStreamInfo;
	memset( &VideoStreamInfo, 0, sizeof(VideoStreamInfo) );
	VideoStreamInfo.fccType = streamtypeVIDEO;
	VideoStreamInfo.fccHandler = FourCC;
	VideoStreamInfo.dwFlags = 0;
	VideoStreamInfo.dwCaps = 0;
	VideoStreamInfo.wPriority = 0;
	VideoStreamInfo.wLanguage = 0;

	VideoStreamInfo.dwScale = 1; 
	VideoStreamInfo.dwRate = m_nVideoFrameRate;

	VideoStreamInfo.dwStart = 0;
	VideoStreamInfo.dwLength = 0;
	VideoStreamInfo.dwInitialFrames = 0;
	VideoStreamInfo.dwSuggestedBufferSize = 0;
	VideoStreamInfo.dwQuality = (DWORD)-1;
	VideoStreamInfo.dwSampleSize = 0;
	VideoStreamInfo.rcFrame.left = 0;
	VideoStreamInfo.rcFrame.top = 0;
	VideoStreamInfo.rcFrame.right = 0;
	VideoStreamInfo.rcFrame.bottom = 0;
	VideoStreamInfo.dwEditCount = 0;
	VideoStreamInfo.dwFormatChangeCount = 0;
	VideoStreamInfo.szName[0] = 0;


	AVIFileCreateStream(m_pAVIFile, &m_pVideoStream, &VideoStreamInfo);
	if (m_pVideoStream==NULL)
		return false;

	BITMAPINFOHEADER bmiHeader;
	memset(&bmiHeader,0,sizeof(bmiHeader));

	bmiHeader.biSize        = sizeof( BITMAPINFOHEADER);
	bmiHeader.biWidth		= nWidth;
	bmiHeader.biHeight		= nHeight;
	bmiHeader.biPlanes      = 1;
	bmiHeader.biBitCount    = 12;
	bmiHeader.biCompression = FourCC;
	bmiHeader.biSizeImage	= nWidth*nHeight*3/2;

	LPBITMAPINFOHEADER pFormat = (LPBITMAPINFOHEADER)&bmiHeader;
	LONG lFormatLen = sizeof(bmiHeader);

	AVIStreamSetFormat(m_pVideoStream, 0, pFormat, lFormatLen);

#endif

	return 0;
}