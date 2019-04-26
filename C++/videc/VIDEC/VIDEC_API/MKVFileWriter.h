//
// MKV File Writer
//
#ifndef	__MKVFILEWRITER__
#define	__MKVFILEWRITER__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <string>


#include "../libmp3lame/lame.h"
#include "IFileWriter.h"

//#define WRITE_RAW_DATA

class MKVFileWriter
	: public IFileWriter
{
public:
	MKVFileWriter(void);
	virtual	~MKVFileWriter(void);
public:
	virtual int	Open(const char* cszPathName);
	virtual void Close();

	virtual int OpenAudioStream(int nSampleRate);
	virtual bool WriteAudio( unsigned char* pAudioData, const int nAudioLen ,unsigned long ulTimestamp,int&nBytes);

	virtual int OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC);
	virtual bool WriteVideo( unsigned char* pVideoData, int nVideoLen );
	virtual bool GetOpenVideoStream(void){return m_bOpenVideoStream;};
	virtual bool GetOpenAudioStream(void){return m_bOpenAudioStream;};
private:
	bool			m_bOpenVideoStream;
	bool			m_bOpenAudioStream;
	void*			m_pMKVHandle;


	int				m_nVideoFrameRate;
	int				m_nWidth;
	int				m_nHeight;
	bool			m_bGotFirstVideo;

	LONG			m_nSampleNumber;
	int				m_nSampleRate;
	bool			m_bGotFirstAudio;
	unsigned long	m_nLastAudioTimestamp;

	bool			m_bGotFirstTimestamp;
	unsigned long	m_ulFirstTimestamp;

	lame_global_flags*	m_pLameEncoder;
	unsigned long		m_nStreamSizeAudio;
	int*				m_pInSamples;
	unsigned char*		m_pStreamDataBuffer;

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
