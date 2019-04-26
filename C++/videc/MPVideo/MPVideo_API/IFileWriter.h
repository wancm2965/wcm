//IFileWriter.h
#ifndef __IFILEWRITER_H__
#define __IFILEWRITER_H__

class IFileWriter
{
public:
	IFileWriter(void){};
	virtual	~IFileWriter(void){};
public:
	virtual int	Open(const char* cszPathName)=0;
	virtual void Close(void)=0;

	virtual int OpenAudioStream(int nSampleRate)=0;
	virtual bool WriteAudio( unsigned char* pAudioData, const int nAudioLen ,unsigned long ulTimestamp,int&nBytes)=0;

	virtual int OpenVideoStream(int nWidth,int nHeight,FOURCC FourCC)=0;
	virtual bool WriteVideo( unsigned char* pVideoData, int nVideoLen )=0;
};

#endif
