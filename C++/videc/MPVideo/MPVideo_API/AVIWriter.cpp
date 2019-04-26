// AVIWriter.cpp: implementation of the AVIWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "MPVIDEO_Header.h"
#include "AVIWriter.h"
#include "XUtil.h"
#include <VIDEC/VIDEC_CodecDef.h>

#define MAX_FILE_SIZE	671088640

MPVIDEO_AVIWriter*MPVIDEO_AVIWriter::Create(void)
{
	return new AVIWriter();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVIWriter::AVIWriter(void)
:m_pFileWriter(NULL)
,m_bRunning(false)
,m_nWidth(0)
,m_nHeight(0)
,m_nSampleRate(0)
,m_nStandardFormat(0)
,m_nFileIndex(0)
,m_nFileSize(0)
,m_bRenewFile(false)
,m_nRenewFileTimestamp(0)

,m_pIXResample(NULL)
,m_nSampleRateResample(0)
,m_pResampleBuf(NULL)
,m_nResampleBufLen(0)

,m_nMaxFileSize(MAX_FILE_SIZE)
,m_nStartTimestamp(0)

,m_bOpenVideoStream(false)
{
}
 
AVIWriter::~AVIWriter(void)
{

}

/******************************************************************************
* Open
*描述：打开录像文件
*输入：	cszFileName			-文件名称
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int AVIWriter::Open(const char*cszFileName,int nStandardFormat,int nMaxFileSize)
{
	m_nMaxFileSize=nMaxFileSize;
	if (m_nMaxFileSize<=0 || m_nMaxFileSize>MAX_FILE_SIZE)
	{
		m_nMaxFileSize=MAX_FILE_SIZE;
	}
	else if (m_nMaxFileSize<10000000)
	{
		m_nMaxFileSize=10000000;
	}

	if (nStandardFormat<WRITER_STANDARD_FORMAT_NONE ||
		nStandardFormat>WRITER_STANDARD_FORMAT_MKV)
	{
		if (nStandardFormat!=WRITER_STANDARD_FORMAT_MP3)
			nStandardFormat=WRITER_STANDARD_FORMAT_MKV;
	}

	m_nStandardFormat=nStandardFormat;
	if (cszFileName==NULL)
		return -1;
	m_strPathName=cszFileName;
	if (m_strPathName.size()==0)
	{
		return -1;
	}

	m_bRunning=true;
	if (!StartThread())
	{
		return -1;
	}

	return 0;
}

/******************************************************************************
* Close
*描述：关闭录像文件
*输入：	无
*输出： 无
*返回值：无
*****************************************************************************/
void AVIWriter::Close(void)
{
	m_bRunning=false;
	WaitForStop();

	while (DoWrite(1)){};

	if (m_pFileWriter!=NULL)
	{
		m_pFileWriter->Close();
		delete m_pFileWriter;
		m_pFileWriter=NULL;
	}

	{
		XAutoLock l(m_csListXAudioBuffer);
		while (m_ListXAudioBuffer.size()>0)
		{
			XAudioBuffer*pXAudioBuffer=(XAudioBuffer*)m_ListXAudioBuffer.front();
			m_ListXAudioBuffer.pop_front();
			delete pXAudioBuffer;
			pXAudioBuffer=NULL;
		}
	}

	{
		XAutoLock l(m_csListXVideoBuffer);
		while (m_ListXVideoBuffer.size()>0)
		{
			XVideoBuffer*pXVideoBuffer=(XVideoBuffer*)m_ListXVideoBuffer.front();
			m_ListXVideoBuffer.pop_front();
			delete pXVideoBuffer;
			pXVideoBuffer=NULL;
		}
	}

	if (m_pIXResample!=NULL)
	{
		m_pIXResample->ReleaseConnections();
		delete m_pIXResample;
		m_pIXResample=NULL;
	}

	if (m_pResampleBuf!=NULL)
	{
		free(m_pResampleBuf);
		m_pResampleBuf=NULL;
	}
}

/******************************************************************************
* WriteVideo
*描述：写视频到录像文件
*输入：	pPacketData			-视频包数据
		nPacketLen			-视频包数据长度
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int AVIWriter::WriteVideo(unsigned char*pPacketData,int nPacketLen)
{
	if (m_nStandardFormat==WRITER_STANDARD_FORMAT_MP3)
	{
		return 0;
	}

	if (pPacketData==NULL ||
		nPacketLen<=0)
	{
		return 0;
	}

	XAutoLock l(m_csListXVideoBuffer);
	XVideoBuffer*pXVideoBuffer=new XVideoBuffer;
	if (pXVideoBuffer)
	{
		pXVideoBuffer->pBuffer=malloc(nPacketLen);
		if (pXVideoBuffer->pBuffer)
		{
			memcpy(pXVideoBuffer->pBuffer,pPacketData,nPacketLen);
			pXVideoBuffer->nLen=nPacketLen;

			m_ListXVideoBuffer.push_back(pXVideoBuffer);
			return 0;
		}
		delete pXVideoBuffer;
		pXVideoBuffer=NULL;
	}

	return 0;
}

/******************************************************************************
* WriteAudio
*描述：写音频到录像文件
*输入：	pSampleData			-样本数据
		nSampleDataLen		-样本数据长度
		nSampleRate			-样本率
		nSynTimestamp		-同步时间戳
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int AVIWriter::WriteAudio(unsigned char*pSampleData,int nSampleDataLen,int nSampleRate,unsigned long nSynTimestamp)
{
	if (pSampleData==NULL ||
		nSampleDataLen<=0)
	{
		return 0;
	}

	if (nSampleRate<16000)
	{
		if (m_nSampleRateResample!=22050)
		{
			if (m_pIXResample!=NULL)
			{
				m_pIXResample->ReleaseConnections();
				delete m_pIXResample;
				m_pIXResample=NULL;
			}

			m_nSampleRateResample=22050;
		}	
	}
	else
	{
		if (m_nSampleRateResample!=22050)
		{
			if (m_pIXResample!=NULL)
			{
				m_pIXResample->ReleaseConnections();
				delete m_pIXResample;
				m_pIXResample=NULL;
			}

			m_nSampleRateResample=22050;
		}
	}

	if (m_pIXResample==NULL)
	{
		m_pIXResample=CreateIXResample();
		if (m_pIXResample!=NULL)
		{
			if (0!=m_pIXResample->Connect(nSampleRate,m_nSampleRateResample))
			{
				m_pIXResample->ReleaseConnections();
				delete m_pIXResample;
				m_pIXResample=NULL;
			}
		}
	}

	if (m_pIXResample!=NULL)
	{
		int nResampleBufLen=(nSampleDataLen*m_nSampleRateResample/nSampleRate)*2;
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
			if(0==m_pIXResample->Convert((short*)pSampleData,nSampleDataLen>>1,pOutSamples,nOutSamples))
			{
				pSampleData=(unsigned char*)pOutSamples;
				nSampleDataLen=(nOutSamples<<1);
				nSampleRate=m_nSampleRateResample;
			}
		}
	}

	XAutoLock l(m_csListXAudioBuffer);
	XAudioBuffer*pXAudioBuffer=new XAudioBuffer;
	if (pXAudioBuffer)
	{
		pXAudioBuffer->pBuffer=malloc(nSampleDataLen);
		if (pXAudioBuffer->pBuffer)
		{
			memcpy(pXAudioBuffer->pBuffer,pSampleData,nSampleDataLen);
			pXAudioBuffer->nLen=nSampleDataLen;
			pXAudioBuffer->nSampleRate=nSampleRate;
			pXAudioBuffer->nSynTimestamp=nSynTimestamp;

			m_ListXAudioBuffer.push_back(pXAudioBuffer);
			return 0;
		}
		delete pXAudioBuffer;
		pXAudioBuffer=NULL;
	}

	return 0;
}

void AVIWriter::ThreadProcMain(void)
{
	bool bRet=false;
	while (m_bRunning)
	{
		bRet=DoWrite(10000);

		if (!bRet)
		{
			XSleep(1);
		}
		else
		{
			if (m_bRenewFile==false)
			{
				unsigned long nStartTimestamp=m_nStartTimestamp;
				unsigned long nTimestamp=XGetTimestamp();
				if (m_nFileSize>=m_nMaxFileSize || nTimestamp-nStartTimestamp>=3600000)
				{
					m_nRenewFileTimestamp=XGetTimestamp();
					m_bRenewFile=true;
				}
			}
		}
	}
}

bool AVIWriter::DoWrite(unsigned long ulMaxBufferedMS)
{
	if (m_nStandardFormat==WRITER_STANDARD_FORMAT_MP3)
	{
		return DoWriteAudio();
	}

	bool bRet=false;
	unsigned long ulFirstAudioTimestamp=0;
	unsigned long ulFirstVideoTimestamp=0;
	unsigned long ulOffset1=0;
	unsigned long ulOffset2=0;
	unsigned long ulOffset=0;
	unsigned long ulAudioBufferedMS=0;
	unsigned long ulVideoBufferedMS=0;

	ulAudioBufferedMS=GetAudioBufferedMS();
	ulVideoBufferedMS=GetVideoBufferedMS();
	if (ulAudioBufferedMS && ulVideoBufferedMS)
	{
		ulFirstAudioTimestamp=GetAudioFirstTimestamp();
		ulFirstVideoTimestamp=GetVideoFirstTimestamp();
		ulOffset1=ulFirstAudioTimestamp-ulFirstVideoTimestamp;
		ulOffset2=ulFirstVideoTimestamp-ulFirstAudioTimestamp;
		ulOffset=(ulOffset1>ulOffset2?ulOffset2:ulOffset1);

		//char szDebug[1024];
		//sprintf(szDebug,"ulFirstAudioTimestamp=%u ulFirstVideoTimestamp=%u ulOffset=%u\n",ulFirstAudioTimestamp,ulFirstVideoTimestamp,ulOffset);
		//OutputDebugString(szDebug);

		if (ulOffset==ulOffset2) 
		{
			bRet|=DoWriteAudio();
		}
		else
		{
			bRet|=DoWriteVideo();
		}
	}
	else if (ulAudioBufferedMS>=ulMaxBufferedMS)
	{
		bRet|=DoWriteAudio();
	}
	else if (ulVideoBufferedMS>=ulMaxBufferedMS)
	{
		bRet|=DoWriteVideo();
	}

	return bRet;
}

bool AVIWriter::DoWriteVideo(void)
{
	bool bRet=false;
	XVideoBuffer*pXVideoBuffer=NULL;
	{
		XAutoLock l(m_csListXVideoBuffer);
		if (m_ListXVideoBuffer.size())
		{
			pXVideoBuffer=(XVideoBuffer*)m_ListXVideoBuffer.front();
			m_ListXVideoBuffer.pop_front();
			bRet=true;
		}
	}
	if (pXVideoBuffer!=NULL)
	{
		unsigned char*pData=(unsigned char*)pXVideoBuffer->pBuffer;
		int nLen=pXVideoBuffer->nLen;

		bool bKeyFrame=MPVIDEO_HEADER_EXT_GET_KEYFRAME(pData)!=0;
		if (bKeyFrame)
		{
			int nWidth=0;
			int nHeight=0;
			int nStandardFormat=m_nStandardFormat;
			if (MPVIDEO_HEADER_EXT_GET_DOUBLE_FIELD(pData))
			{
				nStandardFormat=WRITER_STANDARD_FORMAT_NONE;
			}
			if (nStandardFormat==WRITER_STANDARD_FORMAT_NONE)
			{
				nWidth=MPVIDEO_HEADER_EXT_GET_VIRTUAL_WIDTH(pData);
				nHeight=MPVIDEO_HEADER_EXT_GET_VIRTUAL_HEIGHT(pData);
			}
			else
			{
				nWidth=MPVIDEO_HEADER_EXT_GET_ACTUAL_WIDTH(pData);
				nHeight=MPVIDEO_HEADER_EXT_GET_ACTUAL_HEIGHT(pData);
			}

			if (m_nWidth!=nWidth ||
				m_nHeight!=nHeight ||
				m_bRenewFile ||
				m_pFileWriter==NULL ||
				m_bOpenVideoStream==false)
			{
				m_nFileSize=0;
				m_bRenewFile=false;
				m_nRenewFileTimestamp=0;

				VIDEC_CODEC_TYPE codecType=(VIDEC_CODEC_TYPE)MPVIDEO_HEADER_EXT_GET_CODEC_ID(pData);

				bool bNewFile=false;
				if (m_bOpenVideoStream && m_pFileWriter!=NULL)
				{
					bNewFile=true;
				}

				if (nStandardFormat==WRITER_STANDARD_FORMAT_MKV && m_pFileWriter!=NULL && (codecType==VIDEC_CODEC_H263 || codecType==VIDEC_CODEC_H263P || codecType==VIDEC_CODEC_H263PP))
				{
					bNewFile=true;
				}

				if (bNewFile)
				{
					m_nSampleRate=0;
					m_nWidth=0;
					m_nHeight=0;
					m_bOpenVideoStream=false;

					if (m_pFileWriter)
					{
						m_pFileWriter->Close();
						delete m_pFileWriter;
						m_pFileWriter=NULL;

						m_nFileIndex++;
					}
				}

				if (m_pFileWriter==NULL)
				{
					std::string strPathName=m_strPathName;
					if (m_nFileIndex>0)
					{
						char szFileIndex[128]="";
						sprintf(szFileIndex,"(%u)",m_nFileIndex);
						std::string strExt="";
						int nPos=strPathName.find_last_of(".");
						if (nPos>0)
						{
							strExt=strPathName.substr(nPos);
							strPathName=strPathName.substr(0,nPos);
							strPathName+=szFileIndex;
							strPathName+=strExt;
						}
					}

					if (nStandardFormat==WRITER_STANDARD_FORMAT_AVI)
					{
						m_pFileWriter=new AVIFileWriter();
					}
					else
					{
						m_pFileWriter=new MKVFileWriter();
					}

					if (m_pFileWriter->Open(strPathName.c_str())!=0)
					{
						m_pFileWriter->Close();
						delete m_pFileWriter;
						m_pFileWriter=NULL;
					}
					else
					{
						m_nStartTimestamp=XGetTimestamp();
					}
				}

				if (m_pFileWriter!=NULL)
				{
					m_nWidth=nWidth;
					m_nHeight=nHeight;
						
					FOURCC FourCC=0;
					if (nStandardFormat==WRITER_STANDARD_FORMAT_NONE)
					{
						FourCC=mmioFOURCC('V','D','E','C');
					}
					else
					{
						switch (codecType)
						{
						case VIDEC_CODEC_H264:
						case VIDEC_CODEC_H264_SVC:
							FourCC=mmioFOURCC('A','V','C','1');
							break;
						case VIDEC_CODEC_H263:
						case VIDEC_CODEC_H263P:
						case VIDEC_CODEC_H263PP:
							FourCC=mmioFOURCC('H','2','6','3');
							break;
						default:
							FourCC=mmioFOURCC('V','D','E','C');
							break;
						}
					}

					m_pFileWriter->OpenVideoStream(m_nWidth,m_nHeight,FourCC);
					m_bOpenVideoStream=true;
				}
			}
		}

		if (m_pFileWriter!=NULL)
		{
			m_nFileSize+=nLen;
			if (!m_pFileWriter->WriteVideo((unsigned char*)pData,nLen))
			{

			}
		}

		delete pXVideoBuffer;
		pXVideoBuffer=NULL;
	}

	return bRet;
}


bool AVIWriter::DoWriteAudio(void)
{
	bool bRet=false;
	XAudioBuffer*pXAudioBuffer=NULL;
	{
		XAutoLock l(m_csListXAudioBuffer);
		if (m_ListXAudioBuffer.size())
		{
			pXAudioBuffer=(XAudioBuffer*)m_ListXAudioBuffer.front();
			m_ListXAudioBuffer.pop_front();
			bRet=true;
		}
	}
	if (pXAudioBuffer!=NULL)
	{
		unsigned long nTimestamp=XGetTimestamp();
		if (m_nSampleRate!=pXAudioBuffer->nSampleRate || (m_bRenewFile && nTimestamp-m_nRenewFileTimestamp>=60000))
		{
			m_nFileSize=0;
			m_bRenewFile=false;
			m_nRenewFileTimestamp=0;

			if (m_nSampleRate!=0  && m_pFileWriter!=NULL)
			{
				m_nSampleRate=0;
				m_nWidth=0;
				m_nHeight=0;

				m_bOpenVideoStream=false;

				m_pFileWriter->Close();
				delete m_pFileWriter;
				m_pFileWriter=NULL;
				
				m_nFileIndex++;
			}

			if (m_pFileWriter==NULL)
			{
				std::string strPathName=m_strPathName;
				if (m_nFileIndex>0)
				{
					char szFileIndex[128]="";
					sprintf(szFileIndex,"(%u)",m_nFileIndex);
					std::string strExt="";
					int nPos=strPathName.find_last_of(".");
					if (nPos>0)
					{
						strExt=strPathName.substr(nPos);
						strPathName=strPathName.substr(0,nPos);
						strPathName+=szFileIndex;
						strPathName+=strExt;
					}
				}

				if (m_nStandardFormat==WRITER_STANDARD_FORMAT_AVI)
				{
					m_pFileWriter=new AVIFileWriter();
				}
				else if (m_nStandardFormat==WRITER_STANDARD_FORMAT_MP3)
				{
					m_pFileWriter=new MP3FileWriter();
				}
				else
				{
					m_pFileWriter=new MKVFileWriter();
				}

				if (m_pFileWriter->Open(strPathName.c_str())!=0)
				{
					m_pFileWriter->Close();
					delete m_pFileWriter;
					m_pFileWriter=NULL;
				}
				else
				{
					m_nStartTimestamp=XGetTimestamp();
				}
			}

			if (m_pFileWriter!=NULL)
			{
				m_nSampleRate=pXAudioBuffer->nSampleRate;
				m_pFileWriter->OpenAudioStream(m_nSampleRate);
			}
		}

		if (m_pFileWriter!=NULL)
		{
			int nBytes=0;
			m_pFileWriter->WriteAudio((unsigned char*)pXAudioBuffer->pBuffer,pXAudioBuffer->nLen,pXAudioBuffer->nSynTimestamp,nBytes);
			m_nFileSize+=nBytes;
		}

		delete pXAudioBuffer;
		pXAudioBuffer=NULL;
	}


	return bRet;
}

unsigned long AVIWriter::GetAudioBufferedMS(void)
{
	XAutoLock l(m_csListXAudioBuffer);
	if (m_ListXAudioBuffer.size())
	{
		XAudioBuffer*pXAudioBuffer=(XAudioBuffer*)m_ListXAudioBuffer.front();
		unsigned long ulFirstTimestamp=pXAudioBuffer->nSynTimestamp;
		pXAudioBuffer=(XAudioBuffer*)m_ListXAudioBuffer.back();
		unsigned long ulLastTimestamp=pXAudioBuffer->nSynTimestamp;
		unsigned long ulDuration=ulLastTimestamp-ulFirstTimestamp+20;
		return ulDuration;
	}
	else
	{
		return 0;
	}
}

unsigned long AVIWriter::GetAudioFirstTimestamp(void)
{
	XAutoLock l(m_csListXAudioBuffer);
	if (m_ListXAudioBuffer.size())
	{
		XAudioBuffer*pXAudioBuffer=(XAudioBuffer*)m_ListXAudioBuffer.front();
		unsigned long ulFirstTimestamp=pXAudioBuffer->nSynTimestamp;
		return ulFirstTimestamp;
	}

	return 0;
}

unsigned long AVIWriter::GetVideoBufferedMS(void)
{
	XAutoLock l(m_csListXVideoBuffer);
	if (m_ListXVideoBuffer.size())
	{
		XVideoBuffer*pXVideoBuffer=(XVideoBuffer*)m_ListXVideoBuffer.front();
		unsigned long ulFirstTimestamp=MPVIDEO_HEADER_EXT_GET_TIMESTAMP(pXVideoBuffer->pBuffer);
		pXVideoBuffer=(XVideoBuffer*)m_ListXVideoBuffer.back();
		unsigned long ulLastTimestamp=MPVIDEO_HEADER_EXT_GET_TIMESTAMP(pXVideoBuffer->pBuffer);
		unsigned long ulDuration=ulLastTimestamp-ulFirstTimestamp+20;
		return ulDuration;
	}
	else
	{
		return 0;
	}
}

unsigned long AVIWriter::GetVideoFirstTimestamp(void)
{
	XAutoLock l(m_csListXVideoBuffer);
	if (m_ListXVideoBuffer.size())
	{
		XVideoBuffer*pXVideoBuffer=(XVideoBuffer*)m_ListXVideoBuffer.front();
		unsigned long ulFirstTimestamp=MPVIDEO_HEADER_EXT_GET_TIMESTAMP(pXVideoBuffer->pBuffer);
		return ulFirstTimestamp;
	}
	else
	{
		return 0;
	}
}
