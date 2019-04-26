//AVAudio.h
#ifndef __AVAUDIO_H__
#define __AVAUDIO_H__

#include <mmsystem.h>

typedef enum AUDIO_CODEC
{
	AVAUDIO_AVAC_G7231=2,
	AVAUDIO_AVAC_8K=25,
	AVAUDIO_AVAC_16K,
	AVAUDIO_AVAC_24K,
	AVAUDIO_AVAC_32K,
	AVAUDIO_G726H_24K,
	AVAUDIO_G722H_16K
}AUDIO_CODEC;


#define AVUM_AUDIO_CAPTURE_MAX_VOLUME	(WM_USER+310)
#define AVUM_AUDIO_PLAYER_MAX_VOLUME	(WM_USER+311)

/**************************************************************************/
/*音频采集并且压缩
/**************************************************************************/
class IAVAudioCapture : public AVSource
{
public:
	IAVAudioCapture(void){};
	virtual~IAVAudioCapture(void){};
public:
	/***********************************************************************/
	/*功能：关闭音频设备，释放音频采集所占有的所有资源
	/*参数：
	/*返回：
	/*备注：请在删除该对象前调用此函数，以确保释放所有资源
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;

	/***********************************************************************/
	/*功能：是否采集音频
	/*参数：bool bCapture[in,是否采集音频，如果bCapture=true采集，否则停止采集]
	/*返回：如果已经打开音频设备则返回true，否则返回false
	/*备注：
	/***********************************************************************/
	virtual bool Capture(bool bCapture)=0;
	
	/***********************************************************************/
	/*功能：判断是否在正在采集音频
	/*参数：
	/*返回：如果正在采集音频则返回true，否则返回false
	/*备注：如果没有打开音频设备的也返回false
	/***********************************************************************/
	virtual bool IsCapturing(void)=0;
};


/***********************************************************************/
/*功能：创建音频采集对象
/*参数：HWND hWnd[in,窗口句柄,在DirectSound中使用],int nDeviceID[in,音频设备的序号，从0开始],
/*		AUDIO_CODEC AudioCodec[in,音频编码器类型]
/*返回：如果打开音频设备成功并且音频编码器类型支持则返回音频采集对象，否则返回NULL
/*备注：
/***********************************************************************/
AVCONEC40_AFX_EXT_CLASS IAVAudioCapture*CreateAudioCapture(HWND hWnd,int nDeviceID,AUDIO_CODEC AudioCodec);

#define IAVAudioCaptureEx		IAVAudioCapture
#define CreateAudioCaptureEx	CreateAudioCapture


class IAVAudioPlayerCallback
{
public:
	IAVAudioPlayerCallback(void){};
	virtual~IAVAudioPlayerCallback(void){};
public:
	virtual void OnIAVAudioPlayerCallbackFrameData(char*pFrameData,int nLen){};
};

/***********************************************************************/
/*音频播放器，具有防抖动，解码和同步播放功能
/***********************************************************************/
class IAVAudioPlayer : public AVSink
{
public:
	IAVAudioPlayer(){};
	virtual~IAVAudioPlayer(){};
public:
	/***********************************************************************/
	/*功能：关闭音频设备，释放音频播放器所占有的所有资源
	/*参数：
	/*返回：
	/*备注：请在删除该对象前调用此函数，以确保释放所有资源
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;
	
	/***********************************************************************/
	/*功能：设置播放音量
	/*参数：int nVolume[in,播放音量0-100，100音量最大，0音量最小]
	/*返回：
	/*备注：
	/***********************************************************************/
	virtual void SetVolume(int nVolume)=0;

	/***********************************************************************/
	/*功能：获取播放音量
	/*参数：
	/*返回：播放音量0-100，100音量最大，0音量最小
	/*备注：
	/***********************************************************************/
	virtual int GetVolume(void)=0;

	/***********************************************************************/
	/*功能：是否播放静音
	/*参数：bool bMute[in,是否静音，如果bMute=true则静音，否则播放声音]
	/*返回：
	/*备注：
	/***********************************************************************/
	virtual void Mute(bool bMute)=0;
	
	/***********************************************************************/
	/*功能：判断是否播放静音
	/*参数：
	/*返回：如果静音则返回true，否则返回false
	/*备注：
	/***********************************************************************/
	virtual bool IsMute(void)=0;

	virtual void EnableJitterBuffer(bool bEnableJitterBuffer)=0;
	virtual bool IsEnableJitterBuffer(void)=0;

	virtual void StartIAVAudioPlayerCallback(IAVAudioPlayerCallback*pIAVAudioPlayerCallback)=0;
	virtual void StopIAVAudioPlayerCallback(void)=0;

	virtual void AutoDelay(bool bAutoDelay)=0;
};

/***********************************************************************/
/*功能：创建音频播放器对象
/*参数：WAVEFORMATEX*pwfx[in,音频采样]
/*返回：如果是不支持的音频采样则返回NULL，否则返回音频播放器对象
/*备注：
/***********************************************************************/
AVCONEC40_AFX_EXT_CLASS IAVAudioPlayer*CreateAudioPlayer(HWND hWnd,AUDIO_CODEC AudioCodec,AVSyn*pAVSyn);

AVCONEC40_AFX_EXT_CLASS int GetAudioCaptureDeviceCount(void);
AVCONEC40_AFX_EXT_CLASS bool GetAudioCaptureDeviceName(int nAudioCaptureDeviceIndex,TCHAR*szAudioCaptureDeviceName);

AVCONEC40_AFX_EXT_CLASS void EnableAEC(bool bEnableAEC);
AVCONEC40_AFX_EXT_CLASS bool IsEnableAEC(void);
AVCONEC40_AFX_EXT_CLASS void EnableANS(bool bEnableANS);
AVCONEC40_AFX_EXT_CLASS bool IsEnableANS(void);
AVCONEC40_AFX_EXT_CLASS void EnableCN(bool bEnableCN);
AVCONEC40_AFX_EXT_CLASS bool IsEnableCN(void);
AVCONEC40_AFX_EXT_CLASS void EnableVAD(bool bEnableVAD);
AVCONEC40_AFX_EXT_CLASS bool IsEnableVAD(void);
AVCONEC40_AFX_EXT_CLASS void EnableAGC(bool bEnableAGC);
AVCONEC40_AFX_EXT_CLASS bool IsEnableAGC(void);


AVCONEC40_AFX_EXT_CLASS bool XMCInitAudio(void);
AVCONEC40_AFX_EXT_CLASS void XMCUnitAudio(void);

class IXPCMFilePlayer
{
public:
	IXPCMFilePlayer(void){};
	virtual~IXPCMFilePlayer(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool IsDone(void)=0;
};

AVCONEC40_AFX_EXT_CLASS IXPCMFilePlayer*CreateIXPCMFilePlayer(const char*cszPCMFile,unsigned long ulSampleRate,bool bLoop=false);

AVCONEC40_AFX_EXT_CLASS int GetAECQuality(void);//0:好,1:较好,2:差,3:较差

#endif