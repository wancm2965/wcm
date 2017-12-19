#include "FFmpegrecord\IFFmpegrecord.h"
#include "KBASE\Thread.h"
#include "SourceData.h"

class CFFmpegrecord : public IFFmpegrecord
	,public KThread	
{

public:
	CFFmpegrecord();
	~CFFmpegrecord();

	virtual int Open(const char*cszFileName);
	virtual void Close(void);
	virtual int WriteHeader(int nWidth, int nHeight, int nFrameRate, int nSampleRate, int nChannels){ return 1; }
	
	virtual int WriteVideo(unsigned char*data, unsigned int len, uint32_t nTimestamp, bool bKeyFrame, unsigned int nWidth = 640, unsigned int nHeight = 480);

	virtual int WriteAudio(unsigned char*data, unsigned int len, uint32_t nTimestamp);

	int startRecord();
	void StopRecord();

	virtual void ThreadProcMain(void);
	SSDATA* ReadAudioBuffData();
	SSDATA* ReadVideoBuffData();
private:

	void startffmpeg();

	void  useffmpegstart();

#ifdef USE_OTHER_WAY

	int CreateMp4(AVFormatContext*&pOC,void*p,int len);
	int getVopType(const void*p,int len);
	int get_nal_type(void*p,int len);
	AVStream* add_stream(AVFormatContext*oc,AVCodec**codec,enum AVCodecID codec_id);
	void open_video(AVFormatContext*oc,AVCodec*codec,AVStream*st);
	void WriteVideo(AVFormatContext*&pOC,void*data,int nlen);
	void CloseMp4(AVFormatContext*&pOC);
#endif
private:
	std::string m_strFilename;
	CSourceData* m_pVideoDataBuffer;
	CSourceData* m_pAudioDataBuffer;	

	bool         m_bStartThread;
	bool         m_bWriteBuff;
	bool         m_bVideoAllow;
	bool         m_bAudioAllow;
	bool         m_bhasaudio;
#ifdef USE_OTHER_WAY
	int m_vi;
	int m_ptsInc;
	int m_waitkey;
	AVFormatContext* m_pOC;

#endif
};