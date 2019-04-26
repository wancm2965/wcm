//
// MP3 File Writer
//
#ifndef	__MP3FILEWRITER__
#define	__MP3FILEWRITER__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <string>


#include "../libmp3lame/lame.h"
#include "IFileWriter.h"


class MP3FileWriter
	: public IFileWriter
{
public:
	MP3FileWriter(void);
	virtual	~MP3FileWriter(void);
public:
	virtual int	Open(const char* cszPathName);
	virtual void Close();

	virtual int OpenAudioStream(int nSampleRate);
	virtual bool WriteAudio( unsigned char* pAudioData, const int nAudioLen ,unsigned long ulTimestamp,int&nBytes);

	virtual int OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC);
	virtual bool WriteVideo( unsigned char* pVideoData, int nVideoLen );
	virtual bool GetOpenVideoStream(void){return false;};
	virtual bool GetOpenAudioStream(void){return m_bOpenAudioStream;};
private:
	bool			m_bOpenAudioStream;
	FILE*			m_pFile;

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
};

#endif	
