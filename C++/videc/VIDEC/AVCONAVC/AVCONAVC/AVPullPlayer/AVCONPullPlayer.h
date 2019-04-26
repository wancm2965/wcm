#include <AVCONAVC/avpullplayer.h>
#include <AVCONAVC/AVCONAVC_MediaFile.h>

#define	PLAY_STATE_STOP		0
#define	PLAY_STATE_PLAY		1
#define	PLAY_STATE_PAUSE	2 

class AVCONPullPlayer
	: public IAVPullPlayer
	, public AVCONAVC_MediaFileCallback
{
public:
	AVCONPullPlayer(void);
	virtual~AVCONPullPlayer(void);
public:
	bool	Connect(HWND hWnd,unsigned long ulUserMsg,const char*cszFileName);
	void	ReleaseConnections(void);
	bool	HasVideo(void);
	int		GetWidth(void);
	int		GetHeight(void);
	bool	SetWindowPos(int x,int y,int cx,int cy); 
	void	SetVideoQuality(int nVideoQuality);

	bool	HasAudio(void);
	void	SetVolume(int nVolume);
	int		GetVolume(void);
	void	SetMute(bool bMute=true);
	bool	GetMute(void);
	void	SetBalance(int nBalance);
	int		GetBalance(void);

	unsigned long GetDuration(void);
	unsigned long GetPosition(void);
	void	Seek(unsigned long ulSeconds);
	void	Seek(unsigned long ulTimeStamp,bool bToKeyFrame){};
	void	Pause(void);
	void	Play(void);
	bool	IsPlaying(void);
protected:
	virtual void OnAVCONAVC_MediaFileCallbackPCMData(AVCONAVC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp){};
	virtual void OnAVCONAVC_MediaFileCallbackYV12Buffer(unsigned char**ppBuffer,int nBufferLen){};
	virtual void OnAVCONAVC_MediaFileCallbackYV12Data(AVCONAVC_MediaFile*pMediaFile,unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp){};
protected:
	AVCONAVC_MediaFile*	m_pMediaFile;
	int					m_nVolume;
	bool				m_bMute;
	bool				m_bPlaying;
	HWND				m_hWndNotify;
	unsigned long		m_ulUserMsg;
};