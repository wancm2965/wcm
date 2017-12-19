#pragma once
#include "FFmpegrecord\IFFmpegrecord.h"
#include "KBASE/AutoLock.h"
#include "KBASE/CritSec.h"

typedef struct _MP4_AVStream{
	char*			pAudioData;
	char*			pVideoData;
	int				nAudioBufLen;
	int				nVideoBufLen;
	int				nKeyFrame;
	int             nVideoWidth;
	int             nVideoHeight;
	unsigned long	ulAudioDataLen;
	unsigned long	ulVideoDataLen;
	unsigned long   ulAduioTimestamp;
	unsigned long   ulVideoTimestamp;
	unsigned long   ulAudioFilstTimestamp;
	unsigned long   ulVideoFilstTimestamp;
}MP4_AVStream;

class CAVFileRecord :public IFFmpegrecord
{
public:
	CAVFileRecord();
	~CAVFileRecord(void);
	virtual int Open(const char*cszFileName);
	virtual void Close(void);
	virtual int WriteHeader(int nWidth, int nHeight, int nFrameRate, int nSampleRate, int nChannels);
	virtual int WriteVideo(unsigned char*data, unsigned int len, unsigned long nTimestamp, bool bKeyFrame, unsigned int nWidth = 640, unsigned int nHeight = 480);
	virtual int WriteAudio(unsigned char*data, unsigned int len, unsigned long nTimestamp);
	unsigned long GetFileSize();
protected:
	int WriteAudioData(unsigned char*data, unsigned int len, uint32_t nTimestamp);
	int WriteVideoData(unsigned char*data, unsigned int len, unsigned long nTimestamp, int bKeyFrame);
	virtual AVStream* add_video_stream(AVFormatContext *oc, AVCodecID codec_id, int width, int height, int fps = 60);
	virtual AVStream* add_audio_stream(AVFormatContext *oc, AVCodecID codec_id, int samples, int channels = 2);
private:
	unsigned long m_uFileSize;
	KCritSec m_KCritSecAV;
	int64_t m_nVidoeTime;
	int64_t m_nAudioTime;
	bool      m_bStartStream;
	AVStream* m_pVideoStream;
	AVStream* m_pAudioStream;
	AVFormatContext* m_pFormatCtx;
	static bool m_bAVInit;
	std::string m_strFileName;
	int64_t m_ulVideoFrames;
	int64_t m_ulAudioFrames;
	MP4_AVStream* m_pMP4Stream;
	bool          m_bInitHeader;

};
