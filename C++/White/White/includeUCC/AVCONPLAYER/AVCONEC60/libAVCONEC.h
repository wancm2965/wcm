//libAVCONEC.h
#ifndef __LIBAVCONEC_H__
#define __LIBAVCONEC_H__

#include "AVCONECC.h"
#include "VIDEC.h"


AFX_EXT_CLASS bool IsAudioHardware(int nChannelIndex);
AFX_EXT_CLASS void SetAudioRecordDevice(unsigned int nDevice);
AFX_EXT_CLASS unsigned int GetAudioRecordDevice(void);
AFX_EXT_CLASS void SetAudioPlayDevice(unsigned int nDevice);
AFX_EXT_CLASS void EnableAudioAEC(bool bEnable);
AFX_EXT_CLASS bool IsEnableAudioAEC(void);
AFX_EXT_CLASS void EnableAudioAGC(bool bEnable);
AFX_EXT_CLASS bool IsEnableAudioAGC(void);

//判断是高清设备
AFX_EXT_CLASS bool IsHDDevice(int nDevID);


//{{xmc add 20070830
AFX_EXT_CLASS int SD_GetFormatCount(void);
AFX_EXT_CLASS bool SD_GetFormatName(int nIndex,char*szName);
AFX_EXT_CLASS bool SD_GetFormatSize(int nIndex,int&nWidth,int&nHeight);

AFX_EXT_CLASS int HD_GetFormatCount(void);
AFX_EXT_CLASS bool HD_GetFormatName(int nIndex,char*szName);
AFX_EXT_CLASS bool HD_GetFormatSize(int nIndex,int&nWidth,int&nHeight);
//}}


class IHPVideoWindow
{
public:
	IHPVideoWindow(void){};
	virtual~IHPVideoWindow(void){};
public:
	virtual unsigned short GetWidth(void)=0;
	virtual unsigned short GetHeight(void)=0;
	virtual void ShowWindow(bool bShow)=0;
	virtual bool IsWindowVisible(void)=0;
	virtual void SetWindowPos(int x,int y,int cx,int cy)=0;

	virtual void EnableTitle(bool bEnable)=0;
	virtual bool IsTitleEnable(void)=0;
	virtual void SetTitleText(const TCHAR*cszText,int nAlign=0)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT

	virtual HWND GetHWnd(void)=0;
};

class IHPAVCaptureNotify
{
public:
	IHPAVCaptureNotify(void){};
	virtual~IHPAVCaptureNotify(void){};
public:
	virtual void OnIHPAVCaptureNotifyOutputAudioData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void OnIHPAVCaptureNotifyOutputVideoData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void OnIHPAVCaptureNotifyOutputSubVideoData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual bool OnIHPAVCaptureNotifyVideoCapture(bool bSub)=0;//return bool,if true,encoder current frame,else skip it
	virtual void OnIHPAVCaptureNotifyMotionDetected(int*pResultList,int nResultCount)=0;//pResultList[i]==0,没有动，否则在动
};

class IHPAVCapture
	: public IHPVideoWindow
{
public:
	IHPAVCapture(void){};
	virtual~IHPAVCapture(void){};
public:
	virtual void ReleaseConnections(void)=0;

	virtual void OpenAudioCapture(HP_AUDIO_CODEC_TYPE HPAudioCodec,bool bUseMic=false)=0;
	virtual bool HasAudioCapture(void)=0;
	virtual void ResetAudioParam(HP_AUDIO_CODEC_TYPE HPAudioCodec,bool bUseMic=false)=0;
	virtual void CloseAudioCapture(void)=0;
	virtual double GetAudioBitrate(void)=0;//建议每隔2000毫秒调用一次
	virtual void PlayDTMF(int nEventNumber)=0;
	virtual void EnableVAD(bool bEnable)=0;
	virtual bool IsVADEnable(void)=0;
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次

	/*
	bEnableSub:是否启动子流,缺省为false
	nSubScaleType:子流图像缩小比率，0:不缩小,1:缩小1/4,2:缩小1/8,缺省为1
	nSubQuality:主流图像质量,定义同主流图像质量定义，缺省为0:64kbps
	*/
	virtual void OpenVideoCapture(int nVideoCaptureDevice,int nWidth,int nHeight,int nFrameRate,int nQuality,VIDEC_CODEC_TYPE codecType,bool bAutoQuality,bool bEnableSub=false,int nSubScaleType=1,int nSubQuality=0)=0;
	virtual bool HasVideoCapture(void)=0;
	virtual void ResetVideoCaptureParam(int nVideoCaptureDevice,int nWidth,int nHeight,int nFrameRate,int nQuality,VIDEC_CODEC_TYPE codecType,bool bAutoQuality,bool bEnableSub=false,int nSubScaleType=1,int nSubQuality=0)=0;
	virtual void CloseVideoCapture(void)=0;
	virtual void CaptureVideoKeyFrame(bool bSub)=0;
	virtual void Denoise(bool bDenoise)=0;
	virtual bool IsDenoised(void)=0;
	virtual double GetVideoBitrate(void)=0;//建议每隔2000毫秒调用一次
	virtual double GetSubVideoBitrate(void)=0;//建议每隔2000毫秒调用一次
	virtual bool HasVideoInputWindow(void)=0;
	virtual void ShowVideoInputWindow(void)=0;
	virtual bool HasVideoAdjustWindow(void)=0;
	virtual void ShowVideoAdjustWindow(void)=0;
	virtual void InvertVideo(bool bInvert)=0;
	virtual bool IsVideoInverted(void)=0;
	virtual bool CaptureToJPGFile(const char*cszJPGFile)=0;

	virtual bool GetMotionImageSize(int&nWidth,int&nHeight)=0;
	virtual bool StartMotionDetect(void)=0;
	virtual void StopMotionDetect(void)=0;
	virtual bool SetupMotionDetect(	int iGrade/*灵敏度等级0-6*/,
									int iFastMotionDetectFps/*高速运动检测的帧间隔 0-12 缺省2 0不作检测*/,
									int iSlowMotionDetectFps/*低速运动检测的帧间隔 13-30 缺省18 0不作检测*/,
									RECT *RectList, int iRectCount,bool bSimple=false)=0;

	virtual bool TVTunerPutChannel(long lChannel/*1-n*/)=0;

	//设置遮掩并启动遮掩
	virtual bool SetupMask(RECT *RectList, int iRectCount)=0;
	//停止遮掩
	virtual bool StopMask(void)=0;

	virtual bool CanSetVideoParam(void)=0;
	virtual void SetVideoParam(int nBrightness=128/*亮度值0-255*/, int nContrast=64/*对比度0-127*/, int nSaturation=64/*饱和度0-127*/, int nHue=0/*色调0-255*/)=0;
};


AFX_EXT_CLASS IHPAVCapture*CreateIHPAVCapture(IHPAVCaptureNotify&rIHPAVCaptureNotify
#ifdef _WIN32
											  ,HWND hWndVideo
#endif
											  );


class IHPAVPlayer
	: public IHPVideoWindow
{
public:
	IHPAVPlayer(void){};
	virtual~IHPAVPlayer(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual void InputAudioPacket(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void InputVideoPacket(unsigned char*pPacketData,int nPacketLen)=0;

	virtual bool HasAudio(void)=0;
	virtual void Mute(bool bMute)=0;
	virtual bool IsMute(void)=0;
	virtual void SetVolume(int nNewVolume)=0;
	virtual int GetVolume(void)=0;
	virtual unsigned long GetAudioLostCount(void)=0;
	virtual double GetAudioBitrate(void)=0;
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次

	virtual bool HasVideo(void)=0;
	virtual unsigned long GetVideoLostCount(void)=0;
	virtual double GetVideoBitrate(void)=0;

	virtual bool CaptureToJPGFile(const char*cszJPGFile)=0;

	virtual void EnableSyn(bool bEnableSyn)=0;
};

AFX_EXT_CLASS IHPAVPlayer*CreateIHPAVPlayer(HWND hWndVideo);

#endif