//IMXEC_MediaChannel.h
#ifndef __IMXEC_MEDIACHANNEL_H__
#define __IMXEC_MEDIACHANNEL_H__

#include <AVCONMP/IMXEC_Export.h>
#include <AVCONMP/IMXEC_Call.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <NETEC/NETEC.h>

AVCONMP_API int AVCONMP_Init(int nScreenCount=4/*合成屏个数*/);

AVCONMP_API void AVCONMP_Terminate(void);

class AVCONMP_CapChanCallback
{
public:
	AVCONMP_CapChanCallback(void){};
	virtual~AVCONMP_CapChanCallback(void){};
public:
	//音频数据回调函数
	virtual void OnAVCONMP_CapChanCallbackAudioData(unsigned char*pData,int nLen,bool&bLoop){};
	//视频数据回调函数nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnAVCONMP_CapChanCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame){};
	virtual void OnAVCONMP_CapChanCallbackAudioIDChanged(unsigned long nAudioID){};
};

class AVCONMP_API AVCONMP_CapChanDev
{
public:
	AVCONMP_CapChanDev(void){};
	virtual~AVCONMP_CapChanDev(void){};
public:
	virtual int Open(int nScreenIndex)=0;
	virtual void Close(void)=0;
	virtual int GetScreenIndex(void)=0;
	virtual int SetScreenPos(int x,int y,int cx,int cy)=0;
	virtual int SetScreenWndCount(int nWndCount)=0;
	virtual int SetScreenWndPos(int nWndIndex,int x,int y,int cx,int cy)=0;
	virtual int OpenVideoDev(int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub)=0;
	virtual int OpenAudio(X_AUDIO_CODEC_ID CodecType)=0;	
	virtual unsigned long GetAudioID(void)=0;
	virtual unsigned long GetVideoID(void)=0;
	
	virtual int SetForceEncodeVideo(int nForceEncodeVideo)=0;
	virtual int GetForceEncodeVideo(void) = 0;
	virtual void RequestKeyFrame(void)=0;
	
	/*设置中码流与子码流编码的分辨率与码流大小，起作用的前提是在调用OpenVideoDev接口之前设置，并且OpenVideoDev中的int nSub与int nQSub 必须为1，如果不调用此接口中码流与子码流按照主码流的1/4与1/9的比例缩小*/
	virtual int SetOtherCodec(int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate)=0;

	static AVCONMP_CapChanDev*Create(AVCONMP_CapChanCallback&rCallback);
};


class AVCONMP_PlayChanCallback
{
public:
	AVCONMP_PlayChanCallback(void){};
	virtual~AVCONMP_PlayChanCallback(void){};
public:
	//音频数据回调函数
	virtual void OnAVCONMP_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop)=0;
	virtual void OnAVCONMP_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)=0;
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnAVCONMP_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode)=0;

	//无法解码的回调，nHardwareError不等于0时，表示解码超出范围，nHardwareError等于0时，表示恢复正常；
	//此函数为线程中回调出来的函数，操作窗口时请转为Windows消息
	virtual void OnAVCONMP_PlayChanCallbackHardwareError(int nHardwareError){};
};

class AVCONMP_API AVCONMP_PlayChan
{
public:
	AVCONMP_PlayChan(void){};
	virtual~AVCONMP_PlayChan(void){};
public:
	virtual int Open(unsigned long nAudChanID,unsigned long nVidChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU=0)=0;

	virtual int Open(void)=0;

	virtual void Close(void)=0;

	//取视频宽度
	virtual int GetVideoWidth(void)=0;
	//取视频高度
	virtual int GetVideoHeight(void)=0;

	/******************************************************************************
	* SetScreenWnd
	*描述：设置所在屏幕窗口位置
	*输入：	nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

	virtual int SetVolume(int nVolume)=0;
	virtual int GetLevel(int&nLevel)=0;

	virtual int StartAudio(void)=0;
	virtual int EnableAudio(int nEnable/*0:不接收，1:接收*/)=0;
	virtual int SetAudioID(unsigned long nAudioID)=0;
	virtual int StopAudio(void)=0;
	virtual int StartVideo(void)=0;
	virtual int EnableVideo(int nEnable/*0:不接收，1:接收*/)=0;
	virtual int StopVideo(void)=0;
	virtual int SetAudioCast(bool bAudioCast)=0;
	virtual int SetVideoCast(bool bVideoCast)=0;

	/******************************************************************************
	* RequestKeyFrame
	*描述：请求关键帧
	*输入：	无
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int RequestKeyFrame(void)=0;

	/******************************************************************************
	* SetFrameRateControlMode
	*描述：设置帧率控制模式
	*输入：	nMode			-帧率控制模式
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE nMode)=0;

	/******************************************************************************
	* SetFrameTypeControlMode
	*描述：设置帧类型控制模式
	*输入：	nMode			-帧类型控制模式
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE nMode)=0;

	/******************************************************************************
	* GetAudioPacketStatistics
	*描述：获取音频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

	/******************************************************************************
	* GetVideoPacketStatistics
	*描述：获取视频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

	/******************************************************************************
	* PlayAudioData
	*描述：播放音频数据
	*输入：	pData			-数据指针
			nLen			-数据长度
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int PlayAudioData(const void*pData,int nLen)=0;

	/******************************************************************************
	* PlayVideoData
	*描述：播放视频数据
	*输入：	pData			-数据指针
			nLen			-数据长度
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int PlayVideoData(const void*pData,int nLen)=0;

	/******************************************************************************
	* SetSynFlag
	*描述：设置视频音频同步标记
	*输入：	nSynFlag		-视频音频同步标记【0：视频音频不同步，非0：视频音频同步】
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSynFlag(int nSynFlag)=0;



	static AVCONMP_PlayChan*Create(AVCONMP_PlayChanCallback&rCallback);

	static unsigned long GetTotalBytes(void);
	static int GetBitrate(void);
	static void ClearTotalBytes(void);
	static void SetForcibleReceiving(bool bForcible);
	static bool GetForcibleReceiving(void);

	virtual int StartMixingAudio(X_AUDIO_CODEC_ID nCodecID)=0;
	virtual void StopMixingAudio(void)=0;
	virtual int GetMixingAudioID(unsigned long&nAudioID)=0;
};


#endif
