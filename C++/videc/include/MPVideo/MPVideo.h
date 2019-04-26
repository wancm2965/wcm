//MPVideo.h
#ifndef __MPVIDEO_H__
#define __MPVIDEO_H__


#include <MPVideo/MPVideo_Export.h>
#include <VIDEC/VIDEC_CodecDef.h>




MPVIDEO_API int MPVideo_Init(void);
//终止视频库，必须在使用完MPVideo之后调用
MPVIDEO_API void MPVideo_Terminate(void);

#define MPVIDEO_NETWORK_INTERNET			0			//互联网
#define MPVIDEO_NETWORK_LAN				1			//局域网
#define MPVIDEO_NETWORK_PRIVATE			2			//专网
#define MPVIDEO_NETWORK_SATELLITE			3			//卫星网
#define MPVIDEO_NETWORK_3G				4			//3G
#define MPVIDEO_NETWORK_CDMA				MPVIDEO_NETWORK_3G
#define MPVIDEO_NETWORK_QUALITY			0x80000000	//是否质量优先


MPVIDEO_API int GetBitrateControlTypeByNetworkType(int nNetworkType);
//根据质量取名称
//nQuality,图象质量，0：64,1:128,2:256,3:384,4:512,5:768,6:1024,7:1536,8:2048,9:4096:10:6144
//szName,质量对应的名称
//nMaxCount,szName的大小
//失败返回-1，表示无效参数
//成功返回取到的名称的长度
MPVIDEO_API int GetQualityCount(void);
MPVIDEO_API int GetNameByQuality(int nQuality,char*szName,int nMaxCount);

MPVIDEO_API int GetCodecInfo(int&nQuality,int&nBitrate,int&nQMin,int&nQMax,VIDEC_CODEC_TYPE&CodecType,bool bRTP);
MPVIDEO_API int GetCodecInfo(int&nQuality,int&nBitrateControlType,int&nBitrate,int&nQMin,int&nQMax,VIDEC_CODEC_TYPE&CodecType,bool bRTP);
MPVIDEO_API int GetCodecInfoWithBitrate(int& nBitrateControlType,int nBitrate,int&nQMin,int&nQMax,VIDEC_CODEC_TYPE&CodecType,bool bRTP);

MPVIDEO_API int GetFrameRateCount(void);
MPVIDEO_API int GetFrameRateByIndex(int nIndex);
MPVIDEO_API int GetFrameRateDefaultIndex(void);

class MPVIDEO_API MPVideo_Image
{
public:
	MPVideo_Image(void){};
	virtual~MPVideo_Image(void){};
public:
	virtual void AddRef(void)=0;
	virtual void Release(void)=0;
	virtual void*GetData(void)=0;
	virtual int GetDataLen(void)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
	virtual int GetFrameRate(void)=0;

	virtual void SetStride(int nStride)=0;
	virtual int GetStride(void)=0;

	virtual void SetNV12(bool bNV12)=0;
	virtual bool GetNV12(void)=0;

	static MPVideo_Image*Create(unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate=30);
};

class MPVIDEO_API MPVideo_ImageSink;
class MPVIDEO_API MPVideo_ImageSource
{
public:
	MPVideo_ImageSource(void){};
	virtual~MPVideo_ImageSource(void){};
public:
	virtual int AddImageSink(MPVideo_ImageSink*pImageSink)=0;
	virtual void RemoveImageSink(MPVideo_ImageSink*pImageSink)=0;
};


class MPVIDEO_API MPVideo_ImageSink
{
public:
	MPVideo_ImageSink(void){};
	virtual~MPVideo_ImageSink(void){};
public:
	virtual void OnAttachedToImageSource(MPVideo_ImageSource*pImageSource)=0;
	virtual void OnDetachedFromImageSource(MPVideo_ImageSource*pImageSource)=0;
	//pData:IYUV格式的数据,Y+U+V
	virtual void OnReceivedImage(MPVideo_Image*pImage)=0;
	//pResultList[i]==0,没有动，否则在动
	virtual void OnReceivedMotionDetect(int*pResultList,int nResultCount){};
	virtual void OnReceivedAudioSamples(short*pSamples,int nSamples,int nSampleRate){};
};


class MPVIDEO_API MPVideo_StreamSink;
class MPVIDEO_API MPVideo_StreamSource
{
public:
	MPVideo_StreamSource(void){};
	virtual~MPVideo_StreamSource(void){};
public:
	virtual int AddStreamSink(MPVideo_StreamSink*pStreamSink)=0;
	virtual void RemoveStreamSink(MPVideo_StreamSink*pStreamSink)=0;
};

class MPVIDEO_API MPVideo_StreamSink
{
public:
	MPVideo_StreamSink(void){};
	virtual~MPVideo_StreamSink(void){};
public:
	virtual void OnAttachedToStreamSource(MPVideo_StreamSource*pStreamSource)=0;
	virtual void OnDetachedFromStreamSource(MPVideo_StreamSource*pStreamSource)=0;
	virtual void OnReceivedStreamData(MPVideo_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0)=0;
	virtual void OnReceivedSubStreamData(MPVideo_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0){};
	virtual void OnReceivedAudioStreamData(MPVideo_StreamSource*pStreamSource,unsigned char*pData,int nLen){};
};


class MPVIDEO_API MPVideo_CapChan;
class MPVideo_CapChanCallback
{
public:
	MPVideo_CapChanCallback(void){};
	virtual~MPVideo_CapChanCallback(void){};
public:
	//nSkipFlag,1:跳过这帧不压缩，0:压缩当前帧
	virtual void OnMPVideo_CapChanCallbackPreEncode(MPVideo_CapChan*pCapChan,int&nSkipFlag)=0;
};

//视频采集通道，支持压缩，以帧的方式输出
class MPVIDEO_API MPVideo_CapChan
	: public MPVideo_ImageSink
	, public MPVideo_StreamSource
{
public:
	MPVideo_CapChan(void){};
	virtual~MPVideo_CapChan(void){};
public:
	//打开采集通道压缩通道
	//nQuality,图象质量，0:32,1：64,2:128,3:256,4:384,5:512,6:768,7:1024,8:1536,9:2048,10:4096:11:6144
	//CodecType：视频编码
	//nScaleType:缩放类型，0:same,1:1/4,2:1/16
	//成功返回0，失败返回-1
	virtual int Open(int nQuality,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nSubWidth,int nSubHeight)=0;
	//关闭采集通道压缩通道
	virtual void Close(void)=0;
	//重设置压缩通道
	//nQuality,图象质量，0:32,1：64,2:128,3:256,4:384,5:512,6:768,7:1024,8:1536,9:2048,10:4096:11:6144
	//CodecType：视频编码
	//nScaleType:缩放类型，0:same,1:1/4,2:1/16
	//成功返回0，失败返回-1
	virtual int Reset(int nQuality,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nSubWidth,int nSubHeight)=0;
	//请求关键帧
	//成功返回0，失败返回-1
	virtual int RequestKeyFrame(void)=0;

	static MPVideo_CapChan*Create(MPVideo_CapChanCallback&rCallback);

	virtual int SetSkipFrame(int nFlagSkipFrame)=0;//设置是否降低一半帧率
};

//视频播放通道，支持解压缩，以帧的方式输出入
class MPVIDEO_API MPVideo_PlayChan
	: public MPVideo_ImageSource
	, public MPVideo_StreamSink
{
public:
	MPVideo_PlayChan(void){};
	virtual~MPVideo_PlayChan(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;
	virtual int GetAverageKBPS(void)=0;

	static MPVideo_PlayChan*Create(void);
};


//视频播放通道RTP，支持解压缩
class MPVIDEO_API MPVideo_PlayChanRTP
	: public MPVideo_ImageSource
	, public MPVideo_StreamSource
{
public:
	MPVideo_PlayChanRTP(void){};
	virtual~MPVideo_PlayChanRTP(void){};
public:
	//成功返回0，失败返回-1
	virtual int Open(int nPayloadType,VIDEC_CODEC_TYPE CodecType)=0;
	virtual void Close(void)=0;
	virtual void OnRecvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen)=0;

	static MPVideo_PlayChanRTP*Create(void);
};


class MPVIDEO_API MPVideo_CapChanRTP;
class MPVideo_CapChanRTPCallback
{
public:
	MPVideo_CapChanRTPCallback(void){}
	virtual~MPVideo_CapChanRTPCallback(void){};
public:
	virtual void OnMPVideo_CapChanRTPCallbackPacketData(MPVideo_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)=0;
};

//视频采集通道RTP，支持压缩
class MPVIDEO_API MPVideo_CapChanRTP
	: public MPVideo_ImageSink
	, public MPVideo_StreamSource
{
public:
	MPVideo_CapChanRTP(void){};
	virtual~MPVideo_CapChanRTP(void){};
public:
	//打开采集通道压缩通道
	//nPayloadType:RTP的负荷类型
	//nWidth,图象宽度
	//nHeight,图象高度
	//nFrameRate,帧率，1-30
	//nBitrate,视频码流，32,64,128,256,384,512,768,1024,1536,2048,4096:6144
	//CodecType：视频编码
	//成功返回0，失败返回-1
	virtual int Open(int nPayloadType,int nWidth,int nHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE CodecType)=0;
	//关闭采集通道压缩通道
	virtual void Close(void)=0;
	//请求关键帧
	//成功返回0，失败返回-1
	virtual int RequestKeyFrame(void)=0;

	static MPVideo_CapChanRTP*Create(MPVideo_CapChanRTPCallback&rCallback);
};


MPVIDEO_API int MPVideo_SetScreenPos(int nScreenIndex,int x,int y,int cx,int cy);
MPVIDEO_API int MPVideo_SetScreenWndCount(int nScreenIndex,int nWndCount);
MPVIDEO_API int MPVideo_SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy);


class MPVIDEO_API MPVideo_MPScreen
	: public MPVideo_ImageSource
{
public:
	MPVideo_MPScreen(void){};
	virtual~MPVideo_MPScreen(void){};
public:
	virtual int Open(int nWidth,int nHeight,int nFrameRate)=0;
	virtual void Close(void)=0;
	virtual int SetScreenIndex(int nScreenIndex)=0;

	static MPVideo_MPScreen*Create(void);
};

MPVIDEO_API void GetBitrateRange(int nWidth,int nHeight,int&nQualityMin,int&nQualityMax);

MPVIDEO_API int GetSupportDXVA(void);

MPVIDEO_API int GetHarewareDecoder(void);

MPVIDEO_API int MPVIDEO_GetSupportHardwareEncoder(void);
MPVIDEO_API void MPVideo_SetHardwareEncoderMaxCount(unsigned int nMaxCount);

MPVIDEO_API int  MPVideo_GetSupportHardwareDecoder(void);
MPVIDEO_API void MPVideo_SetHardwareDecoderMaxCount(unsigned int nMaxCount);


#endif