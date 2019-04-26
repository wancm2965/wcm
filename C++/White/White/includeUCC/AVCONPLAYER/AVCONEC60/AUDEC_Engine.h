//AUDEC_Engine.h
#ifndef __AUDEC_ENGINE_H__
#define __AUDEC_ENGINE_H__

#include <stdio.h>

#include "AUDEC_Export.h"

typedef struct{
	int		id;
	int		pl_type;
	char	pl_name[32];
	int		pl_freq;
	int		framesize;
	int		bitrate;
	char	desc[128];//g711u 64kbps
}AUDEC_CodecInst;

class AUDEC_ENGINE_API AUDEC_SampleOut
{
public:
	AUDEC_SampleOut(void){};
	virtual~AUDEC_SampleOut(void){};
public:
	virtual int GetSampleRate(void)=0;
	// true for ok, false for fail
	virtual bool Write(const short*pSamples,int nSamples)=0;
};

class AUDEC_ENGINE_API AUDEC_StreamOut
{
public:
	AUDEC_StreamOut(void){};
	virtual~AUDEC_StreamOut(void){};
public:
	// true for ok, false for fail
	virtual bool Write(const void*data,int len)=0; 
};

class AUDEC_ENGINE_API AUDEC_PlayChan;
class AUDEC_ENGINE_API AUDEC_CapChan
{
public:
	AUDEC_CapChan(void){};
	virtual~AUDEC_CapChan(void){};
public:
	virtual int Connect(AUDEC_CodecInst*pCodecInst)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int SetCodec(AUDEC_CodecInst*pCodecInst)=0;
	virtual int GetCodec(AUDEC_CodecInst*pCodecInst)=0;

	virtual int StartCapture(void)=0;
	virtual int StopCapture(void)=0;
	//<0:error 0:false,else true
	virtual int GetCapturing(void)=0;

	//<0:error >=0:level[0-10]
	virtual int GetLevel(void)=0;

	//<0:error else bps
	virtual int GetBitrate(void)=0;

	//<0:error 0:ok nVAD[0:disable,1:enable]
	//virtual int SetVAD(int nVAD)=0;
	//<0:error 0:disable 1:enable
	//virtual int GetVAD(void)=0;

	virtual int AddStreamOut(AUDEC_StreamOut*pStreamOut)=0;
	virtual int RemoveStreamOut(AUDEC_StreamOut*pStreamOut)=0;

	//设置本通道的采集音量
	//volume取值范围0-20 0:最小 20:最大，放大两倍；10:不改变音量
	virtual int SetVolume(int volume)=0;//0-10-20
	virtual int GetVolume(void)=0;//0-10-20

	//和播放通道混音，发送给远端
	virtual int StartMixingWithPlayChan(AUDEC_PlayChan*pPlayChan)=0;
	virtual int StopMixingWidthPlayChan(AUDEC_PlayChan*pPlayChan)=0;
};

class AUDEC_ENGINE_API AUDEC_CapChanExternal
	: public AUDEC_CapChan
{
public:
	AUDEC_CapChanExternal(void){};
	virtual~AUDEC_CapChanExternal(void){};
public:
	virtual int GetBufferedTime(void)=0;//ms
	virtual int WriteSamples(short*pSamples,int nSamples,int nSampleRate,bool bDiscontinious)=0;
};

class AUDEC_ENGINE_API AUDEC_PlayChan
	: public AUDEC_StreamOut
{
public:
	AUDEC_PlayChan(void){};
	virtual~AUDEC_PlayChan(void){};
public:
	virtual int Connect(unsigned long*lpnVidFrameTimestamp=NULL,unsigned long*lpnAudFrameTimestamp=NULL)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int GetCodec(AUDEC_CodecInst*codec_inst)=0;

	virtual int SetVolume(int volume)=0;//0-10-20
	virtual int GetVolume(void)=0;//0-10-20

	//<0:error 0:ok mute[0:disable 1:enable]
	virtual int SetMute(int mute)=0;
	//<0:error else mute[0:disable 1:enable]
	virtual int GetMute(void)=0;

	//<0:error level[0-10]
	virtual int GetLevel(void)=0;//0-10

	//<0:error else bps
	virtual int GetBitrate(void)=0;

	//<0:error else 0:100
	virtual int GetLostPackets(void)=0;

	//<0:error else 0:100
	virtual int GetLostPacketsPercent(void)=0;

	//<0:error 0:ok status[0:disable 1:enable]
	virtual int SetJitterBuffer(int value)=0;
	//<0:error else status[0:disable 1:enable]
	virtual int GetJitterBuffer(void)=0;
	//min[60-3000]
	virtual int SetJitterMin(int min)=0;
	virtual int GetJitterMin(void)=0;
	//max[5000-10000]
	virtual int SetJitterMax(int max)=0;
	virtual int GetJitterMax(void)=0;

	virtual int AddStreamOut(AUDEC_StreamOut*pStreamOut)=0;
	virtual int RemoveStreamOut(AUDEC_StreamOut*pStreamOut)=0;

	virtual int AddSampleOut(AUDEC_SampleOut*pSampleOut)=0;
	virtual int RemoveSampleOut(AUDEC_SampleOut*pSampleOut)=0;

	virtual int GetBufferedTime(void)=0;

	//播放PCM的数据
	//pSamples,PCM的数据
	//nSamples,样本个数
	//nSampleRate,样本率,现在只支持8000HZ和16000HZ
	//成功返回0
	//失败返回-1
	virtual int PlaySamples(short*pSamples,int nSamples,int nSampleRate,unsigned short usSeqNo)=0;

	//设置同步标记,nFlag:0表示不需要同步,nFlag:1表示需要同步
	virtual int SetSynFlag(int nFlag)=0;
};

class AUDEC_ENGINE_API AUDEC_RTPCall
{
public:
	AUDEC_RTPCall(void){}
	virtual~AUDEC_RTPCall(void){};
public:
	virtual int Connect(const char*cszLocalIP,unsigned short usLocalPort)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int SetRemoteSDP(int nPayloadType,const char*cszPayloadName,const char*cszIP,unsigned short usPort)=0;

	virtual AUDEC_CapChan*GetCapChan(void)=0;
	virtual AUDEC_PlayChan*GetPlayChan(void)=0;

	virtual int SendDTMF(int nPayloadType,char dtmf)=0;
};


class AUDEC_ENGINE_API AUDEC_Engine
{
public:
	AUDEC_Engine(void){};
	virtual~AUDEC_Engine(void){};
public:
	//0:ok,<0:error,nAECType:0-4,0:最弱4:最强
	static int AUDEC_Init(int nAECType=4,int nUsedByRS=0);
	//0:ok,<0:error
	static int AUDEC_Terminate(void);

	//0:no dev,>0:dev count <0:error
	static int AUDEC_GetMicNumDevs(void);
	//0:ok,<0:error
	static int AUDEC_GetMicDevDesc(int dev,char desc[]);

	//0:no dev,>0:dev count <0:error
	static int AUDEC_GetSpeakerNumDevs(void);
	//0:ok,<0:error
	static int AUDEC_GetSpeakerDevDesc(int dev,char desc[]);

	//0:no codec,>0:codec count,<0:error
	static int	AUDEC_GetNumCodecs(void);
	//0:ok,<0:error
	static int	AUDEC_GetCodecByIndex(int index,AUDEC_CodecInst*pCodecInst);
	//0:ok,<0:error
	static int	AUDEC_GetCodecByID(int id,AUDEC_CodecInst*pCodecInst);


	virtual int AUDEC_SetAEC(int nAEC)=0;//0:diable aec,1:enable aec
	virtual int AUDEC_GetAEC(void)=0;
	virtual int AUDEC_GetAECQuality(void)=0;//0:very good,1:good,2:poor,3:very poor

	//设置缺省Mic的输入音量;nVolume[0-100]0:最小,1:最大
	virtual int AUDEC_SetDefaultMicVolume(int nVolume)=0;
	virtual int AUDEC_GetDefaultMicVolume(void)=0;

	//前置自动增益控制,可自动调节录音音量;nAGC取值范围(0-1),0:不启动,1:启动;缺省值为1
	virtual int AUDEC_SetPreAGC(int nAGC)=0;
	virtual int AUDEC_GetPreAGC(void)=0;

	//后置自动增益控制,可根据需要自动放大录音音量;nAGC取值范围(0-12),0:不启动,12:放大12倍;缺省值为4
	virtual int AUDEC_SetPostAGC(int nAGC)=0;
	virtual int AUDEC_GetPostAGC(void)=0;

	//前置噪音抑制;nNS取值范围(0-6),0:不启动,6:最强;缺省值为6
	virtual int AUDEC_SetPreNS(int nNS)=0;
	virtual int AUDEC_GetPreNS(void)=0;

	//静音检测;nVAD取值范围(0-1),0:不启动,1:启动
	virtual int AUDEC_SetVAD(int nVAD)=0;
	virtual int AUDEC_GetVAD(void)=0;

	//前向纠错;nFEC取值范围(0-1),0:不启动,1:启动
	virtual int AUDEC_SetFEC(int nFEC)=0;
	virtual int AUDEC_GetFEC(void)=0;

	virtual int AUDEC_SetSpeakerDev(int nDev,int nSteroPlay=0)=0;
	virtual int AUDEC_SetMicDev(int nDev,int nMicBoost=0)=0;

	virtual int AUDEC_CreateCapChan(AUDEC_CapChan**ppCapChan)=0;
	virtual int AUDEC_CreateCapChanExternal(AUDEC_CapChanExternal**ppCapChanExternal)=0;
	virtual int AUDEC_CreatePlayChan(AUDEC_PlayChan**ppPlayChan)=0;


	//dtmf['0'-'9','#','*']
	virtual int AUDEC_PlayDTMF(char dtmf)=0;

	virtual int AUDEC_CreateRTPCall(AUDEC_RTPCall**ppRTPCall)=0;

	//for debug only
	//record pcm raw 16khz mono
	virtual int AUDEC_StartRecordPreAEC(FILE*hFile)=0;
	virtual int AUDEC_StopRecordPreAEC(void)=0;
	virtual int AUDEC_StartRecordPostAEC(FILE*hFile)=0;
	virtual int AUDEC_StopRecordPostAEC(void)=0;
};

AUDEC_ENGINE_API AUDEC_Engine&AUDEC_GetEngine(void);

#endif