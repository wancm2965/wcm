#ifndef _IAVFPLAYER_H
#define _IAVFPLAYER_H

#define		PLAY_EVENT_MESSAGE			WM_USER+100	//WPARAM=PLAY_EVENT_XXXX, LPARAM=parameter
enum {
	PLAY_EVENT_TIMESTAMP=1,
	PLAY_EVENT_OVER,
	PLAY_EVENT_START_BUFFER,
	PLAY_EVENT_STOP_BUFFER,
	PLAY_EVENT_START_SEEK,
	PLAY_EVENT_STOP_SEEK,
	PLAY_EVENT_PAUSE,
	PLAY_EVENT_TIMEOSD,
	PLAY_EVENT_MUTE,
	PLAY_EVENT_VOLUME,
	PLAY_EVENT_MODECHANGED,
	PLAY_EVENT_SELECTED,
	PLAY_EVENT_MENU,		//�˵�
	PLAY_EVENT_FULLSCREEN, //TRUE ȫ��ģʽ�� FALSE �˳�ȫ��ģʽ //PLAY_EVENT_RESTORE,
	PLAY_EVENT_DOUBLESIZE,
	PLAY_EVENT_STREAM,
	PLAY_EVENT_VIDEO_CHANGED,
	PLAY_EVENT_NEWSTREAM
};

//����ģʽ
enum {
	PLAY_MODE_NORMAL=0,		//��������
	PLAY_MODE_FRAME,		//��֡��
	PLAY_MODE_X,			//����	��1/2X��
	PLAY_MODE_2X,			//���	��2X��
	PLAY_MODE_4X,			//���	��4X��
	PLAY_MODE_8X,			//���	��8X��
	PLAY_MODE_16X,			//���	��16X��
	PLAY_MODE_NULL=15,		//���˼��
	PLAY_MODE_FRAME_REVERSE,//��֡��
	PLAY_MODE_X_REVERSE,	//����	��1/2X��
	PLAY_MODE_2X_REVERSE,	//����	��2X��
	PLAY_MODE_4X_REVERSE,	//����	��4X��
	PLAY_MODE_8X_REVERSE,	//����	��8X��
	PLAY_MODE_16X_REVERSE	//����	��16X��
};
enum {
	PLAY_STYLE_FILE_INFO=1,
	PLAY_STYLE_FILE_LIST=2,
	PLAY_STYLE_OSD_TIME=4,
	PLAY_STYLE_OSD_CHANNEL=8,
	PLAY_STYLE_OSD_STATUS=0x0010,
	PLAY_STYLE_MENU=0x0020,//���������Ҽ��˵�,����WM_NOTIFY::NM_RCLICK֪ͨ
	PLAY_SYTLE_FIXEDRATIO=0x00C0,
	PLAY_SYTLE_FULLSCREEN=0x0100
};
enum {
	PLAY_ERR_SUCCESS=0//
};

enum 
{
	TM_1X1	= 0x0001,
	TM_2X2	= 0x0004,
	TM_3X3	= 0x0009,
	TM_4X4	= 0x0010,
	TM_5X5	= 0x0019,
	TM_6X6	= 0x0024,
	TM_7X7	= 0x0031,
	TM_8X8	= 0x0040,
	TM_9X9	= 0x0051,
	TM_RL5	= 0x0005,
	TM_RL6	= 0x0006,
	TM_RL8	= 0x0008,
	TM_RL10	= 0x000A,
	TM_RL12	= 12,
	TM_RL14 = 14,
	TM_RL18 = 18,
	TM_ALL  = 0xFFFF
};
class IDVRPlayer
{
public:
	virtual bool Connect(HWND hNotifyWnd, HWND hParentWnd)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int OpenAVFPlayFile(const char * cszFile, bool bAdd=false, int nCh=0)=0;
	virtual void CloseAVFPlayFile(int nCh=0)=0;	 

	virtual unsigned long GetDuration()=0;
	virtual unsigned long GetPos()=0;
	virtual void Seek(unsigned long ulSeekTime)=0;

	virtual void Pause(bool bPause)=0;
	virtual bool IsPaused(void)=0;
	virtual bool IsOpen()=0;
	virtual bool IsStop()=0;

	virtual unsigned short GetWidth(void)=0;
	virtual unsigned short GetHeight(void)=0;
	virtual void SetFullScreen(bool bFullScreen, int nCh=0)=0;
	virtual bool IsFullScreen(void)=0;

	virtual void SetMute(bool bMute)=0;
	virtual bool IsMute(void)=0;
	virtual void SetVolume(int nAudioVolume)=0;

	virtual void SetPlayMode(int nMode)=0;
	virtual int GetPlayMode()=0;
	virtual int  Info(void *buffer, unsigned int dwFlag, int nCh=0)=0;

	virtual bool SetStyle(unsigned __int64 dwStyle)=0;
	virtual unsigned __int64 GetStyle()=0;
	virtual HWND GetHwnd()=0;

	virtual void SetTM(int nSize, DWORD dwType=0)=0;					//nSize=-1ʱ���ı䵱ǰģ�壬�Զ�����ģ��
	//SetTmAllocator

	// 20071025
	virtual void InputPacket(unsigned char* data, int length, int nCh=0)=0;
	virtual bool CaptureToFile(const char *fullpath, int nCh=0)=0;

	// 20080110
	//virtual bool RegisterNotifyMessage(DWORD dwMsg);
	virtual void SetSkipVideo(unsigned long ulMS)=0;
	virtual unsigned long GetSkipVideo()=0;
};
class AVSingleStreamPlayer
{
public:
	virtual bool Connect(HWND hNotifyWnd, HWND hParentWnd)=0;
	virtual void ReleaseConnections(void)=0;

	/*************************************
	bAdd
	   false	�ر�
	   true		���ر�֮ǰ�򿪵��ļ�
    nPlayId		ʹ��ָ��������
		0		�Զ�Ѱ��
		>0		ָ��������,ֵΪ[1-nSize]
	����ֵ
		0		���ļ�ʧ��
		>0		��ǰ�ļ�ʹ�õĲ�����
	*************************************/
	virtual int OpenAVFPlayFile(const char * cszFile, bool bAdd=false)=0;

	/*************************************
	nPlayId
		0		�ر����в�����
		>0		�ر�ָ��������
	*************************************/
	virtual void CloseAVFPlayFile()=0;	 
	virtual void SetTM(int nSize, DWORD dwType=0)=0;					//dwTM=-1ʱ���ı䵱ǰģ�壬�Զ�����ģ��

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
	// Add 20080626
	virtual void RegisterCallbackMsg(unsigned long uMsg)=0;
	virtual bool IsSupportMode(int nMode)=0;
	// + [9/10/2008] Jee	
	virtual void SetVideoParam(int nBrightness=128/*����ֵ0-255*/, int nContrast=64/*�Աȶ�0-127*/, int nSaturation=64/*���Ͷ�0-127*/, int nHue=0/*ɫ��0-255*/,int nGamma=100/*٤��1-400*/,int nSharpen=0/*��0-127*/)=0;
	//
};

// �����ϰ汾����
class IXMediaFileCaptureNotify2
{
public:
	// from IHPAVCaptureNotify
	virtual void OnIHPAVCaptureNotifyVideoChanged(void)=0;
	virtual void OnIHPAVCaptureNotifyOutputData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual unsigned char OnIHPAVCaptureNotifyAudioCapture(void)=0;//return HPAV_ONAC
	virtual bool OnIHPAVCaptureNotifyVideoCapture(bool bSub)=0;//return bool,if true,encoder current frame,else skip it
	virtual void OnIHPAVCaptureNotifyMotionDetected(int*pResultList,int nResultCount)=0;//pResultList[i]==0,û�ж��������ڶ�
	virtual void OnIHPAVCaptureNotifyMotionDetected(bool bDetected)=0;//bDetected==false,û�ж��������ڶ�

	//
	virtual void OnIXMediaFileCaptureNotifyFilePositionChanged(unsigned long ulSeconds)=0;
	virtual void OnIXMediaFileCaptureNotifyFilePause(void)=0;
	virtual void OnIXMediaFileCaptureNotifyFilePlay(void)=0;

	// 20071113
	virtual void OnIXMediaFileCaptureNotifyFileEnd(void)=0;
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
	virtual void SetBalance(int nBalance)=0;//δʵ��
	virtual int GetBalance(void)=0;//δʵ��
	virtual int GetAudioLevel(void)=0;//����ÿ��200�������һ��,δʵ��

	virtual bool HasAudio(void)=0;
	virtual bool HasVideo(void)=0;
	virtual double GetAudioBitrate(void)=0;//δʵ��
	virtual void CaptureVideoKeyFrame(bool bSub)=0;//δʵ��
	virtual double GetVideoBitrate(void)=0;//δʵ��
	//
	virtual void SetPlayMode(int nMode)=0;
	virtual int GetPlayMode()=0;
	// 20071025
	virtual void InputPacket(unsigned char* data, int length)=0;
	// 20071030
	// ѭ������nTime=0��ѭ����<0����ѭ����>0ѭ��nTime��
	virtual void SetPlayLoop(int nTime)=0;
	virtual int GetPlayLoop()=0;

	// Add 20071130 Jee
	virtual int OpenAVFPlayFile(const char * cszFile, bool bAdd=false)=0;//bAdd=false�ر�/bAdd=true���ر�֮ǰ�򿪵��ļ�,֮ǰ�򿪵��ļ�
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
// �������ƻص��ӿ�
class IMixerVolumeCallback
{
public: // �ص�
	virtual HWND GetHwnd(void) = 0;
	// Volume�ı�ʱ֪ͨ
	virtual void OnMixerNotify_VolumeValueChanged(DWORD dwVal) = 0;
	// Mute�ı�ʱ֪ͨ
	virtual void OnMixerNotify_VolumeMuteChanged(BOOL bVal) = 0;
	// Mic select�ı�ʱ֪ͨ
	virtual void OnMixerNotify_MicValueChanged(DWORD bVal) = 0;
	// Mic Mute�ı�ʱ֪ͨ
	virtual void OnMixerNotify_MicMuteChanged(BOOL bVal) = 0;
};

// �����������ӿ�
class IMixerVolume
{
public:
	// ��ʼ��
	virtual BOOL InitConnection(void)=0;
	// �ͷ�
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
#ifdef __cplusplus
extern "C" 
{ 
#endif
	void AVFPlayerInit();
	IMixerVolume* CreateIMixerVolume(IMixerVolumeCallback* pDVDVolumeCallback);
	AVSingleStreamPlayer* CreateAVFPlayer(HWND hNotifyWnd, HWND hParentWnd);
	IDVRPlayer* CreateDVRPlayer(HWND hNotifyWnd, HWND hParentWnd);
	IXMediaFileCapture2* CreateMediaFileCapture2(IXMediaFileCaptureNotify2 *rNotify,HWND hWnd,LPCWSTR cszMediaFile=NULL,int HPFrameRate=0,int HPVideoQuality=0,int HPMaxBitrate=0,int HPVideoCodec=0);
	LPCTSTR GetSaveFileNameEx(LPTSTR pszFile, int nBufSize, LPCTSTR pszFilter, LPCTSTR pszDefaultExt, DWORD dwFlag=OFN_NOREADONLYRETURN | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT);	
	void AVFPlayerUninit();
#ifdef __cplusplus
}
#endif
#endif