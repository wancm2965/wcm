//
// MP4 File Writer
//
#ifndef	__MP4FILEWRITER__
#define	__MP4FILEWRITER__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include "XAutoLock.h"
#include "../VIDECMP4/HPATC_Transcode.h"
#include "IFileWriter.h"

class MP4FileWriter
	: public IFileWriter
	, public HPATC_TranscodeCallback
{
public:
	MP4FileWriter(void);
	virtual	~MP4FileWriter(void);
public:
	virtual int	Open(const char* cszPathName);
	virtual void Close();

	virtual int OpenAudioStream(int nSampleRate);
	virtual bool WriteAudio( unsigned char* pAudioData, const int nAudioLen ,unsigned long ulTimestamp,int&nBytes);

	virtual int OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC);
	virtual bool WriteVideo( unsigned char* pVideoData, int nVideoLen );
	virtual bool GetOpenVideoStream(void){return m_bOpenVideoStream;};
	virtual bool GetOpenAudioStream(void){return m_bOpenAudioStream;};

	virtual void OnHPATC_TranscodeCallbackPacketData(HPATC_Transcode*pTranscode,void*pPacketData,int nPacketLen,unsigned long ulTimestamp/*‘≠“Ù∆µ±‡¬Î ±º‰*/);
private:
	bool			m_bOpenVideoStream;
	bool			m_bOpenAudioStream;
	void*			m_pMP4Handle;


	int				m_nVideoType;
	int				m_nWidth;
	int				m_nHeight;

	int				m_nSampleRate;

	XCritSec		m_csMP4Handle;
	HPATC_Transcode*m_pPCM2AAC;


	std::string			m_strPathName;
};

#endif	
