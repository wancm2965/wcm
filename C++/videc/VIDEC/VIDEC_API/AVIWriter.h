// AVIWriter.h: interface for the AVIWriter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVIWRITER_H__
#define __AVIWRITER_H__


#include <string>
#include "VIDEC_AVIWriter.h"
#include "XThreadBase.h"
#include "XListPtr.h"
#include "XAutoLock.h"
#include "AVIFileWriter.h"
#include "MKVFileWriter.h"
#include "MP3FileWriter.h"
#include "MP4FileWriter.h"

#include "IXResample.h"

class AVIWriter  
	: public VIDEC_AVIWriter
	, public XThreadBase
{
public:
	AVIWriter(void);
	virtual ~AVIWriter(void);
public:
	/******************************************************************************
	* Open
	*��������¼���ļ�
	*���룺	cszFileName			-�ļ�����
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Open(const char*cszFileName,int nStandardFormat,int nMaxFileSize, int CacheTimeMS = 10000);

	/******************************************************************************
	* Close
	*�������ر�¼���ļ�
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void Close(void);

	/******************************************************************************
	* WriteVideo
	*������д��Ƶ��¼���ļ�
	*���룺	pPacketData			-��Ƶ������
			nPacketLen			-��Ƶ�����ݳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int WriteVideo(unsigned char*pPacketData,int nPacketLen);

	/******************************************************************************
	* WriteAudio
	*������д��Ƶ��¼���ļ�
	*���룺	pSampleData			-��������
			nSampleDataLen		-�������ݳ���
			nSampleRate			-������
			nSynTimestamp		-ͬ��ʱ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int WriteAudio(unsigned char*pSampleData,int nSampleDataLen,int nSampleRate,unsigned long nSynTimestamp);
protected:
	void ThreadProcMain(void);
	bool DoWriteVideo(void);
	bool DoWriteAudio(void);
	unsigned long GetAudioBufferedMS(void);
	unsigned long GetAudioFirstTimestamp(void);
	unsigned long GetVideoBufferedMS(void);
	unsigned long GetVideoFirstTimestamp(void);
	bool DoWrite(unsigned long ulMaxBufferedMS);
protected:
	bool			m_bRunning;
	int				m_nStandardFormat;

	IFileWriter*	m_pFileWriter;

	int				m_nWidth;
	int				m_nHeight;
	int				m_nSampleRate;
	bool			m_bOpenVideoStream;


	std::string		m_strPathName;
	unsigned long	m_nFileIndex;

	long			m_nFileSize;
	bool			m_bRenewFile;
	unsigned long	m_nRenewFileTimestamp;
	long			m_nMaxFileSize;
	unsigned long	m_nStartTimestamp;

	IXResample*		m_pIXResample;
	int				m_nSampleRateResample;
	unsigned char*	m_pResampleBuf;
	int				m_nResampleBufLen;

	struct XAudioBuffer
	{
		XAudioBuffer(void)
			: pBuffer(NULL)
			, nLen(0)
			, nSampleRate(0)
			, nSynTimestamp(0)
		{
		}
		virtual~XAudioBuffer(void)
		{
			if(pBuffer)
			{
				FREEMSG(pBuffer);
				pBuffer=NULL;
			}
		}

		void*			pBuffer;
		int				nLen;

		int				nSampleRate;
		unsigned long	nSynTimestamp;
	};

	XListPtr		m_ListXAudioBuffer;
	XCritSec		m_csListXAudioBuffer;

	struct XVideoBuffer
	{
		XVideoBuffer(void)
			: pBuffer(NULL)
			, nLen(0)
		{
		}
		virtual~XVideoBuffer(void)
		{
			if(pBuffer)
			{
				FREEMSG(pBuffer);
				pBuffer=NULL;
			}
		}

		void*			pBuffer;
		int				nLen;
	};

	XListPtr		m_ListXVideoBuffer;
	XCritSec		m_csListXVideoBuffer;

	int				m_CacheTimeMS;
};

#endif 