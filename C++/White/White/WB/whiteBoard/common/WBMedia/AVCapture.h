#pragma once
#include <imxec/IMXEC_MediaChannel.h>
//#include <kbase/CritSec.h>
#include <audec/AUDEC_CodecID.h>
#include <audec/AUDEC_Engine.h>
#include "AVDefine.h"
/*#include "../ClientSocket.h"*/

class CAVCapture 
{
public:
	CAVCapture(IMXEC_CapChanCallback& capChanHandle);
	~CAVCapture(void);
	
	int Create(HWND hwnd,int nOnlyPreview = 0);
	void Destroy(void);	

	int OpenVideo(PLBVideoSet videoSet);
	int OpenVideoDev(int nScreenIndex,int nScreenType,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub);
	//打开音频通道
	//nDevID:音频采集设备号，-2从采集卡中采集音频，>=-1从声卡中采集音频
	//CodecType:编码类型
	int OpenAudio(int nDevID, X_AUDIO_CODEC_ID CodecType);
	//关闭视频通道
	void CloseVideo(void);
	//关闭音频通道
	void CloseAudio(void);
	//关闭
	void Close(void);
	//暂停
	void Pause(void);
	void Play();
	void SetMute(bool bMute);
	

public:
	//继承自CAVLocalNotify
	virtual bool HasVideoAdjustWnd(void);
	virtual void ShowAdjustVideoWnd(void);
	virtual bool HasVideoInputWnd(void);
	virtual void ShowVideoInputWnd(void);
	virtual HWND GetVideoHWnd(void);

	virtual void SetInvert(int nInvert);
	virtual int  GetInvert(void);

	virtual bool HasVideoCapture(void);
	virtual void SetScreenWnd(int nScreenIndex, int nWndIndex, int nFullScreen);
	virtual int  GetVideoWidth(void);
	virtual int  GetVideoHeight(void);
	virtual int  SetForceEncodeAudio(int nForceEncodeAudio);
	virtual int  SetForceEncodeVideo(int nForceEncodeVideo);
	virtual bool SetVideoWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags);
	
	virtual int  SetVolume(int nVolume);
	virtual int  GetAudioLevel(void);
	virtual int  GetVideoPacketStatistics(unsigned long& ulTotalPackets, unsigned long& ulLostPackets, double& AvgPacketLossRate, double& CurPacketLossRate, int& nBitrate);
	virtual int  GetAudioPacketStatistics(unsigned long& ulTotalPackets, unsigned long& ulLostPackets, double& AvgPacketLossRate, double& CurPacketLossRate, int& nBitrate);

	virtual unsigned long GetAudioID(void);
	virtual unsigned long GetVideoID(void);

	//保持当前图象到BMP文件
	virtual bool CaptureToBMPFile(const char*cszFile);
	virtual int  SetVideoParam(int nBrightness=128, int nContrast=64, int nSaturation=64, int nHue=0, int nGamma=100, int nSharpen=0, int iRed=100, int iGreen=100, int iBlue=100);

	//图像去噪
	virtual void SetDenoise(int nDenoise);
	virtual	int  GetDenoise(void);

	virtual bool IsVideoDevOpen(void);
	virtual	bool IsAudioOpen(int nDevID);

	


	virtual bool StartRecordToAVI(const char*cszPathName,int nStandardFormat);
	virtual void StopRecordToAVI(void);
	virtual void RequestKeyFrame(void);

	virtual bool IsRecording() const;

	void Seek(unsigned long ulSeconds);

	virtual unsigned long GetDuration(void);
	virtual unsigned long GetPosition(void);

	IMXEC_CapChanDev*                GetCapChanDev();
protected:
	IMXEC_CapChanDev*	             m_pCapChanDev;      // 
	IMXEC_CapChanCallback&           m_capChanHandle;    //		
	//KCritSec			             m_csAVRecord;       //
	int							     m_nVideoWidth;
	int								 m_nVideoHeight;
	int								 m_nForceEncodeVideo;
	int							     m_nForceEncodeAudio;
	int					             m_nFrameRate;
	int					             m_nBitrate;
	int					             m_nBitrateControlType;
	VIDEC_CODEC_TYPE	             m_VideoCodecType;
	int					             m_nSub;
	int					             m_nQSub;
	bool							 m_bIsRecording;
	unsigned long                    nSeek;
private:
	//本通道所使用的设置信息
	LBVideoSet *m_pVideoSet;
};

class CCapChanPlayHandle : public IMXEC_CapChanCallback
{
public:
	CCapChanPlayHandle();
	~CCapChanPlayHandle();

	//音频数据回调函数
	virtual void OnIMXEC_CapChanCallbackAudioData(unsigned char*pData,int nLen,bool&bLoop);
	virtual void OnIMXEC_CapChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_CapChanCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame);
	virtual void OnIMXEC_CapChanCallbackMotionDetected(int*pResultList,int nResultCount){};//pResultList[i]==0,没有动，否则在动

	virtual void OnIMXEC_CapChanCallbackFilePositionChanged(unsigned long ulSeconds);
	virtual void OnIMXEC_CapChanCallbackFilePause(void);
	virtual void OnIMXEC_CapChanCallbackFilePlay(void);
	virtual void OnIMXEC_CapChanCallbackFileEnd(void);
	virtual void OnIMXEC_CapChanCallbackRecvdCtrlData(void*pData,int nLen);

	virtual void OnIMXEC_CapChanCallbackAudioIDChanged(unsigned long nAudioID);
	virtual void OnIMXEC_CapChanCallbackVideoDevOpen(void);

	virtual void OnIMXEC_CapChanCallbackVideoSignalChanged(void){};

	void  SetMedia(DWORD  pMedia) {m_dwMediaId = pMedia;}
private:
	DWORD m_dwMediaId;
	int m_nLen;
};



class CPlayChanEmptyHandle : public IMXEC_PlayChanCallback
{
public:
	CPlayChanEmptyHandle();
	~CPlayChanEmptyHandle(){};

	//音频数据回调函数
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop);
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode);

	virtual void OnIMXEC_PlayChanCallbackRemoteSnapJPG(const char*cszPathName){};

	void  SetMediaplay(DWORD  pMedia) {m_dwplayMediaId = pMedia;}
private:
	DWORD m_dwplayMediaId;
	int m_nLen;
};