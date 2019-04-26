//
// AVI File Writer
//
#ifndef	__AVIFILEWRITER__
#define	__AVIFILEWRITER__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <string>

#include "avilib.h"
#include "IFileWriter.h"

#ifndef AVILIB_H
#include <vfw.h>
#endif

#define AVI_MP3

#ifdef AVI_MP3
#include "../libmp3lame/lame.h"
#endif

//#define WRITE_RAW_DATA

class AVIFileWriter
	: public IFileWriter
{
public:
	AVIFileWriter(void);
	virtual	~AVIFileWriter(void);
public:
	virtual int	Open(const char* cszPathName);
	virtual void Close();

	virtual int OpenAudioStream(int nSampleRate);
	virtual bool WriteAudio( unsigned char* pAudioData, const int nAudioLen ,unsigned long ulTimestamp,int&nBytes);

	virtual int OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC);
	virtual bool WriteVideo( unsigned char* pVideoData, int nVideoLen );
protected:
	virtual bool WriteVideo( unsigned char* pVideoData, int nVideoLen, bool bKeyFrame, LONG lVideoFrameIndex );
private:
#ifdef AVILIB_H
	avi_t*			m_pAVI;
#else
	PAVIFILE		m_pAVIFile;
	PAVISTREAM		m_pVideoStream;
	PAVISTREAM		m_pAudioStream;
#endif


	LONG			m_lVideoFrameIndex;
	int				m_nVideoFrameRate;
	int				m_nVideoFrameInterval;
	int				m_nWidth;
	int				m_nHeight;
	bool			m_bGotFirstVideo;

	LONG			m_nSampleNumber;
	int				m_nSampleRate;
	bool			m_bGotFirstAudio;
	unsigned long	m_nLastAudioTimestamp;

	bool			m_bGotFirstTimestamp;
	unsigned long	m_ulFirstTimestamp;

#ifdef AVI_MP3
	lame_global_flags*	m_pLameEncoder;
	unsigned long		m_nStreamSizeAudio;
	int*				m_pInSamples;
	unsigned char*		m_pStreamDataBuffer;
#endif

	std::string			m_strPathName;

#ifdef WRITE_RAW_DATA
	typedef struct tagRAW_DATA_HEADER
	{
		unsigned char	nDataType;
		unsigned char	nKeyFrame;
		unsigned short	nDataLen;
		unsigned long	nTimestamp;
		unsigned long	nSamples;
	}RAW_DATA_HEADER,*PRAW_DATA_HEADER;
	FILE*				m_pFileRawData;
#endif
};

#endif	
