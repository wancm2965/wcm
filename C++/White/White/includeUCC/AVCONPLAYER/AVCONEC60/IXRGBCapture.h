#ifndef __IXRGBCAPTURE_H__
#define __IXRGBCAPTURE_H__

#include "AVCONECC.h"
#include "libAVCONEC.h"

class IXRGBCaptureNotify
{
public:
	IXRGBCaptureNotify(void){};
	virtual~IXRGBCaptureNotify(void){};
public:
	virtual void OnIXRGBCaptureNotifyOutputData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void OnIXRGBCaptureNotifyAudioLevel(int nAudioLevel)=0;//nAudioLevel(0-10:0最小声音，10最大声音
	virtual unsigned char OnIXRGBCaptureNotifyAudioCapture(void)=0;//return HPAV_ONAC
	virtual bool OnIXRGBCaptureNotifyVideoCapture(bool bSub)=0;//return bool,if true,encoder current frame,else skip it
	virtual void OnIXRGBCaptureNotifyVideoChanged(void)=0;
};

class IXRGBCapture
	: public IHPVideoWindow
{
public:
	IXRGBCapture(void){};
	virtual ~IXRGBCapture(void){};
public:
	virtual void ReleaseConnections(void)=0;

	virtual void OpenAudioCapture(HP_AUDIO_CODEC_TYPE HPAudioCodec)=0;
	virtual bool HasAudioCapture(void)=0;
	virtual void ResetAudioParam(HP_AUDIO_CODEC_TYPE HPAudioCodec)=0;
	virtual void StartAudioCapture(void)=0;
	virtual bool IsAudioCaptureStarted(void)=0;
	virtual void StopAudioCapture(void)=0;
	virtual void CloseAudioCapture(void)=0;
	virtual double GetAudioBitrate(void)=0;//建议每隔2000毫秒调用一次
	virtual void EnableVAD(bool bEnable)=0;
	virtual bool IsVADEnable(void)=0;
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次

	virtual void OpenVideoCapture(int nBitCount/*16,32*/,HP_FRAME_RATE_TYPE HPFrameRate,HP_VIDEO_QUALITY_TYPE HPVideoQuality,HP_MAX_BITRATE_TYPE HPMaxBitrate,HP_VIDEO_CODEC_TYPE HPVideoCodec)=0;
	virtual bool HasVideoCapture(void)=0;
	virtual void ResetVideoCaptureParam(int nBitCount/*16,32*/,HP_FRAME_RATE_TYPE HPFrameRate,HP_VIDEO_QUALITY_TYPE HPVideoQuality,HP_MAX_BITRATE_TYPE HPMaxBitrate,HP_VIDEO_CODEC_TYPE HPVideoCodec)=0;
	virtual void StartVideoCapture(void)=0;
	virtual bool IsVideoCaptureStarted(void)=0;
	virtual void StopVideoCapture(void)=0;
	virtual void CloseVideoCapture(void)=0;
	virtual void RequestKeyFrame(void)=0;
	virtual double GetVideoBitrate(void)=0;//建议每隔2000毫秒调用一次
};

AFX_EXT_CLASS IXRGBCapture*CreateIXRGBCapture(IXRGBCaptureNotify&rIXRGBCaptureNotify,HWND hWndVideo);
AFX_EXT_CLASS bool HasXRGBCaptureDevice(void);

#endif
