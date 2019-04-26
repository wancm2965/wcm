#ifndef _IAVFPLAYER_H
#define _IAVFPLAYER_H
class AVSingleStreamPlayer
{
public:
	virtual bool Connect(HWND hNotifyWnd, HWND hParentWnd)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int OpenAVFPlayFile(const char * cszFile, bool bAdd=false)=0;
	virtual void CloseAVFPlayFile()=0;	 

	virtual void SetTM(int nSize, DWORD dwType=0)=0;					//dwTM=-1时不改变当前模板，自动扩张模板

	virtual unsigned long GetDuration()=0;
	virtual unsigned long GetPos()=0;
	virtual void Seek(unsigned long ulSeekTime)=0;

	virtual void Pause(bool bPause)=0;
	virtual bool IsPaused(void)=0;
	virtual bool IsOpen()=0;
	virtual bool IsStop()=0;

	virtual unsigned short GetWidth(void)=0;
	virtual unsigned short GetHeight(void)=0;
	virtual void SetFullScreen(bool bFullScreen)=0;
	virtual bool IsFullScreen(void)=0;

	virtual void SetMute(bool bMute)=0;
	virtual bool IsMute(void)=0;
	virtual void SetVolume(int nAudioVolume)=0;
	virtual void SetPlayMode(int nMode)=0;
	virtual int GetPlayMode()=0;
	virtual int  Info(void *buffer, unsigned int dwFlag)=0;

	virtual bool SetStyle(unsigned __int64 dwStyle)=0;
	virtual unsigned __int64 GetStyle()=0;
	virtual HWND GetHwnd()=0;

	// 20071025
	virtual void InputPacket(unsigned char* data, int length)=0;
	virtual bool CaptureToFile(const char *fullpath)=0;

	// 20080110
	//virtual bool RegisterNotifyMessage(DWORD dwMsg);
	virtual void SetSkipVideo(unsigned long ulMS)=0;
	virtual unsigned long GetSkipVideo()=0;
	// + 20080626
	virtual void RegisterCallbackMsg(unsigned long uMsg)=0;
	virtual bool IsSupportMode(int nMode)=0;
	// + [9/10/2008] Jee	
	virtual void SetVideoParam(int nBrightness=128/*亮度值0-255*/, int nContrast=64/*对比度0-127*/, int nSaturation=64/*饱和度0-127*/, int nHue=0/*色调0-255*/,int nGamma=100/*伽马1-400*/,int nSharpen=0/*锐化0-127*/)=0;
	//
	virtual void SetVideoPos(int x, int y, int cx, int cy)=0;
	virtual unsigned long GetTotalFrames()=0;
	virtual unsigned long GetPlayedFrame()=0;
	virtual bool Fast()=0;
	virtual bool Slow()=0;
	virtual bool Play(HWND hWnd)=0;
	virtual bool Stop()=0;
	virtual bool RefreshPlay()=0;
	virtual unsigned long GetPlayedFrames()=0;
	virtual bool GetKeyFramePos(unsigned long value, int type, unsigned long &pos)=0;
	virtual bool GetNextKeyFramePos(unsigned long value, int type, unsigned long &pos)=0;
	virtual bool SetFileEndMsg(HWND hWnd, unsigned long ulMsg)=0;
	virtual int GetVolume()=0;
	virtual ~AVSingleStreamPlayer(){}
};
#if 1
// - 20081201 Jee
/*
	兼容熊接口IXMediaFileCaptureNotify、IXMediaFileCapture
*/
// 兼容老版本代码
class IXMediaFileCaptureNotify2
{
public:
	// from IHPAVCaptureNotify
	virtual void OnIHPAVCaptureNotifyVideoChanged(void)=0;
	virtual void OnIHPAVCaptureNotifyOutputData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual unsigned char OnIHPAVCaptureNotifyAudioCapture(void)=0;//return HPAV_ONAC
	virtual bool OnIHPAVCaptureNotifyVideoCapture(int nStreamType)=0;
	virtual void OnIHPAVCaptureNotifyMotionDetected(int*pResultList,int nResultCount)=0;//pResultList[i]==0,没有动，否则在动
	virtual void OnIHPAVCaptureNotifyMotionDetected(bool bDetected)=0;//bDetected==false,没有动，否则在动

	//
	virtual void OnIXMediaFileCaptureNotifyFilePositionChanged(unsigned long ulSeconds)=0;
	virtual void OnIXMediaFileCaptureNotifyFilePause(void)=0;
	virtual void OnIXMediaFileCaptureNotifyFilePlay(void)=0;

	// 20071113
	virtual void OnIXMediaFileCaptureNotifyFileEnd(void)=0;

	//+ 20080926 Jee
	virtual void OnIHPAVCaptureNotifyOutputAudioData(unsigned char *data, int len)=0;
	virtual void OnIHPAVCaptureNotifyOutputVideoData(unsigned char *data, int len)=0;
	virtual void OnIHPAVCaptureNotifyDurationChanged(unsigned long ms)=0;
};
class IXMediaFileCapture2
{
public:
	// from IHPVideoWindow
	virtual unsigned short GetWidth(void)=0;
	virtual unsigned short GetHeight(void)=0;
	virtual void ShowWindow(bool bShow)=0;
	virtual bool IsWindowVisible(void)=0;
	virtual void SetWindowPos(int x,int y,int cx,int cy)=0;
	virtual void RepaintVideo(void)=0;
	virtual void DisplayModeChanged(void)=0;

	virtual void EnableTitle(bool bEnable)=0;
	virtual bool IsTitleEnable(void)=0;
	virtual void SetTitleText(LPCWSTR cszText,int nAlign=0)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT

	virtual void ReleaseConnections(void)=0;
	virtual unsigned long GetDuration(void)=0;
	virtual unsigned long GetPosition(void)=0;
	virtual void Seek(unsigned long ulSeconds)=0;
	virtual void Pause(void)=0;
	virtual void Play(void)=0;
	virtual bool IsPlaying(void)=0;
	virtual void SetVolume(int nVolume)=0;
	virtual int GetVolume(void)=0;
	virtual void SetMute(bool bMute=true)=0;
	virtual bool GetMute(void)=0;
	virtual void SetBalance(int nBalance)=0;//未实现
	virtual int GetBalance(void)=0;//未实现
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次,未实现

	virtual bool HasAudio(void)=0;
	virtual bool HasVideo(void)=0;
	virtual int GetAudioBitrate(void)=0;//未实现
	virtual void CaptureVideoKeyFrame(bool bSub)=0;//未实现
	virtual int GetVideoBitrate(void)=0;//未实现
	//
	virtual void SetPlayMode(int nMode)=0;
	virtual int GetPlayMode()=0;
	// 20071025
	virtual void InputPacket(unsigned char* data, int length)=0;
	// 20071030
	// 循环播放nTime=0无循环，<0无限循环，>0循环nTime次
	virtual void SetPlayLoop(int nTime)=0;
	virtual int GetPlayLoop()=0;

	// Add 20071130 Jee
	virtual int OpenAVFPlayFile(const char * cszFile, bool bAdd=false)=0;//bAdd=false关闭/bAdd=true不关闭之前打开的文件,之前打开的文件
	virtual void CloseAVFPlayFile()=0;	 
	virtual bool IsOpen()=0;
	// Add 20071226 Jee
	virtual bool SetStyle(unsigned __int64 dwStyle)=0;
	virtual unsigned __int64 GetStyle()=0;
	virtual bool CaptureToFile(const char *fullpath)=0;
	// Add 20080611
	virtual HWND GetHwnd()=0;
	// Add 20080626
	virtual void RegisterCallbackMsg(unsigned long uMsg)=0;
};
#endif
// 音量控制回调接口
class IMixerVolumeCallback
{
public: // 回调
	virtual HWND GetHwnd(void) = 0;
	// Volume改变时通知
	virtual void OnMixerNotify_VolumeValueChanged(DWORD dwVal) = 0;
	// Mute改变时通知
	virtual void OnMixerNotify_VolumeMuteChanged(BOOL bVal) = 0;
	// Mic select改变时通知
	virtual void OnMixerNotify_MicValueChanged(DWORD bVal) = 0;
	// Mic Mute改变时通知
	virtual void OnMixerNotify_MicMuteChanged(BOOL bVal) = 0;
};

// 音量控制器接口
class IMixerVolume
{
public:
	// 初始化
	virtual BOOL InitConnection(void)=0;
	// 释放
	virtual void ReleaseConnections(void)=0;

	// Volume control
	virtual BOOL GetMute()=0;
	virtual void SetMute(BOOL bVal)=0;
	virtual DWORD GetVolume()=0;
	virtual void SetVolume(DWORD dwVal)=0;

	// Mic control
	virtual BOOL GetMicMute(void)=0;
	virtual void SetMicMute(BOOL bVal)=0;
	virtual DWORD GetMicValue()=0;
	virtual void SetMicValue(DWORD lVal)=0;
};
class IXMediaFileCaptureNotify;
class IXMediaFileCapture;
#ifdef __cplusplus
extern "C" 
{ 
#endif
	void AVFPlayerInit();
	void AVFPlayerUninit();

	AVSingleStreamPlayer* CreateAVFPlayer(HWND hNotifyWnd, HWND hParentWnd);
	IXMediaFileCapture2* CreateMediaFileCapture2(IXMediaFileCaptureNotify2 *rNotify,HWND hWnd,LPCWSTR cszMediaFile=NULL,int HPFrameRate=0,int HPVideoQuality=0,int HPMaxBitrate=0,int HPVideoCodec=0);
	IXMediaFileCapture* CreateXMediaFileCapture2(IXMediaFileCaptureNotify* rNotify,HWND hWnd,const TCHAR*cszMediaFile,int nFrameRate=15,int nQuality=0,int codecType=2,bool bLoop=false,int nBitrateControlType=0);

	IMixerVolume* CreateIMixerVolume(IMixerVolumeCallback* pDVDVolumeCallback);
	LPCTSTR GetSaveFileNameEx(LPTSTR pszFile, int nBufSize, LPCTSTR pszFilter, LPCTSTR pszDefaultExt, DWORD dwFlag=OFN_NOREADONLYRETURN | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT);	
#ifdef __cplusplus
}
#endif
#endif