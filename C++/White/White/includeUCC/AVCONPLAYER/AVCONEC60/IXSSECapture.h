#ifndef __IXSSECAPTURE_H__
#define __IXSSECAPTURE_H__

#include "AVCONECC.h"

class IXSSECaptureNotify
{
public:
	IXSSECaptureNotify(void){};
	virtual~IXSSECaptureNotify(void){};
public:
	virtual void OnIXSSECaptureNotifyOutputAudioData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void OnIXSSECaptureNotifyOutputVideoData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual bool OnIXSSECaptureNotifyVideoCapture(bool bSub)=0;//return bool,if true,encoder current frame,else skip it
};

class IXSSECapture
{
public:
	IXSSECapture(void){};
	virtual ~IXSSECapture(void){};
public:
	virtual void ReleaseConnections(void)=0;

	virtual void OpenAudioCapture(HP_AUDIO_CODEC_TYPE HPAudioCodec)=0;
	virtual bool HasAudioCapture(void)=0;
	virtual void ResetAudioParam(HP_AUDIO_CODEC_TYPE HPAudioCodec)=0;
	virtual void CloseAudioCapture(void)=0;
	virtual double GetAudioBitrate(void)=0;//建议每隔2000毫秒调用一次
	virtual void EnableVAD(bool bEnable)=0;
	virtual bool IsVADEnable(void)=0;
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次

	virtual void OpenVideoCapture(int x,int y,int cx,int cy,int nBitCount/*16,24,32*/,int nFrameRate,int nQuality,VIDEC_CODEC_TYPE codecType,bool bAutoQuality)=0;
	virtual bool HasVideoCapture(void)=0;
	virtual void ResetVideoCaptureParam(int x,int y,int cx,int cy,int nBitCount/*16,24,32*/,int nFrameRate,int nQuality,VIDEC_CODEC_TYPE codecType,bool bAutoQuality)=0;
	virtual void CloseVideoCapture(void)=0;
	virtual void RequestKeyFrame(void)=0;
	virtual double GetVideoBitrate(void)=0;//建议每隔2000毫秒调用一次

	virtual void GetScreenRect(int& x,int& y,int& cx,int& cy)=0;
};

AFX_EXT_CLASS IXSSECapture*CreateIXSSECapture(IXSSECaptureNotify&rIXSSECaptureNotify);

#endif
