//HPMP.h
#ifndef __HPMP_H__
#define __HPMP_H__


#include <HPMP/HPMP_Export.h>
#include <VIDEC/VIDEC_CodecDef.h>
//#include <windows.h>



HPMP_API int HPMP_Init(void);
//终止视频库，必须在使用完HPMP之后调用
HPMP_API void HPMP_Terminate(void);

#define HPMP_NETWORK_INTERNET			0			//互联网
#define HPMP_NETWORK_LAN				1			//局域网
#define HPMP_NETWORK_PRIVATE			2			//专网
#define HPMP_NETWORK_SATELLITE			3			//卫星网
#define HPMP_NETWORK_3G					4			//3G
#define HPMP_NETWORK_CDMA				HPMP_NETWORK_3G
#define HPMP_NETWORK_QUALITY			0x80000000	//是否质量优先


HPMP_API int GetBitrateControlTypeByNetworkType(int nNetworkType);

HPMP_API int GetCodecInfoWithBitrate(int& nBitrateControlType,int nBitrate,int&nQMin,int&nQMax,VIDEC_CODEC_TYPE&CodecType);


class HPMP_API HPMP_Image
{
public:
	HPMP_Image(void){};
	virtual~HPMP_Image(void){};
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

	virtual void SetAlignedFree(bool bAlignedFree)=0;

	static HPMP_Image*Create(unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate=30);
};

class HPMP_API HPMP_ImageSink;
class HPMP_API HPMP_ImageSource
{
public:
	HPMP_ImageSource(void){};
	virtual~HPMP_ImageSource(void){};
public:
	virtual int AddImageSink(HPMP_ImageSink*pImageSink)=0;
	virtual void RemoveImageSink(HPMP_ImageSink*pImageSink)=0;
};


class HPMP_API HPMP_ImageSink
{
public:
	HPMP_ImageSink(void){};
	virtual~HPMP_ImageSink(void){};
public:
	virtual void OnAttachedToImageSource(HPMP_ImageSource*pImageSource)=0;
	virtual void OnDetachedFromImageSource(HPMP_ImageSource*pImageSource)=0;
	//pData:IYUV格式的数据,Y+U+V
	virtual void OnReceivedImage(HPMP_Image*pImage)=0;
	//pResultList[i]==0,没有动，否则在动
	virtual void OnReceivedMotionDetect(int*pResultList,int nResultCount){};
	virtual void OnReceivedAudioSamples(short*pSamples,int nSamples,int nSampleRate){};
};


class HPMP_API HPMP_StreamSink;
class HPMP_API HPMP_StreamSource
{
public:
	HPMP_StreamSource(void){};
	virtual~HPMP_StreamSource(void){};
public:
	virtual int AddStreamSink(HPMP_StreamSink*pStreamSink)=0;
	virtual void RemoveStreamSink(HPMP_StreamSink*pStreamSink)=0;
};

class HPMP_API HPMP_StreamSink
{
public:
	HPMP_StreamSink(void){};
	virtual~HPMP_StreamSink(void){};
public:
	virtual void OnAttachedToStreamSource(HPMP_StreamSource*pStreamSource)=0;
	virtual void OnDetachedFromStreamSource(HPMP_StreamSource*pStreamSource)=0;
	virtual void OnReceivedStreamData(HPMP_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0)=0;
	virtual void OnReceivedSubStreamData(HPMP_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0){};
	virtual void OnReceivedAudioStreamData(HPMP_StreamSource*pStreamSource,unsigned char*pData,int nLen){};
};


class HPMP_API HPMP_CapChan;
class HPMP_CapChanCallback
{
public:
	HPMP_CapChanCallback(void){};
	virtual~HPMP_CapChanCallback(void){};
public:
	//nSkipFlag,1:跳过这帧不压缩，0:压缩当前帧
	virtual void OnHPMP_CapChanCallbackPreEncode(HPMP_CapChan*pCapChan,int&nSkipFlag)=0;
};

//视频采集通道，支持压缩，以帧的方式输出
class HPMP_API HPMP_CapChan
	: public HPMP_ImageSink
	, public HPMP_StreamSource
{
public:
	HPMP_CapChan(void){};
	virtual~HPMP_CapChan(void){};
public:
	//打开采集通道压缩通道
	//nQuality,图象质量，0:32,1：64,2:128,3:256,4:384,5:512,6:768,7:1024,8:1536,9:2048,10:4096:11:6144
	//CodecType：视频编码
	//nScaleType:缩放类型，0:same,1:1/4,2:1/16
	//成功返回0，失败返回-1
	virtual int Open(int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType)=0;
	//关闭采集通道压缩通道
	virtual void Close(void)=0;
	//重设置压缩通道
	//nQuality,图象质量，0:32,1：64,2:128,3:256,4:384,5:512,6:768,7:1024,8:1536,9:2048,10:4096:11:6144
	//CodecType：视频编码
	//nScaleType:缩放类型，0:same,1:1/4,2:1/16
	//成功返回0，失败返回-1
	virtual int Reset(int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType)=0;
	//请求关键帧
	//成功返回0，失败返回-1
	virtual int RequestKeyFrame(void)=0;

	/*设置OSD信息
	 *nIndex：OSD的序号，可开启多个OSD，根据Index来区分
	 *hDC：HDC类型，把要显示的OSD画在此DC上
	 *nImageWidth,nImageHeight：源Image大小
	 *nOSDLeft,nOSDTop,nOSDWidth,nOSDHeight：OSD显示的位置及大小，需要与DC的大小相同，多个OSD的位置不能重叠
	 *bScaled：是否为缩放图像OSD，如果为ture，那么所有参数要根据缩放后的比例设置*/
	virtual void  SetOSD(int nIndex,void* hDC/*HDC*/,int  nImageWidth,int  nImageHeight,int nOSDLeft,int nOSDTop,int nOSDWidth,int nOSDHeight,bool bScaled=false)=0;

	/*是否启用OSD功能
	 *nIndex：OSD的序号，可开启多个OSD，根据Index来区分
	 *bEnable：true开启;false关闭*/
	virtual void  EnableOSD(int nIndex,bool bEnable)=0;

	static HPMP_CapChan*Create(HPMP_CapChanCallback&rCallback);

	virtual int SetSkipFrame(int nFlagSkipFrame)=0;//设置是否降低一半帧率
};

//视频播放通道，支持解压缩，以帧的方式输出入
class HPMP_API HPMP_PlayChan
	: public HPMP_ImageSource
	, public HPMP_StreamSink
{
public:
	HPMP_PlayChan(void){};
	virtual~HPMP_PlayChan(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;
	virtual int GetAverageKBPS(void)=0;

	static HPMP_PlayChan*Create(void);
};


HPMP_API int HPMP_SetScreenPos(int nScreenIndex,int x,int y,int cx,int cy);
HPMP_API int HPMP_SetScreenWndCount(int nScreenIndex,int nWndCount);
HPMP_API int HPMP_SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy);


class HPMP_API HPMP_MPScreen
	: public HPMP_ImageSource
{
public:
	HPMP_MPScreen(void){};
	virtual~HPMP_MPScreen(void){};
public:
	virtual int Open(int nWidth,int nHeight,int nFrameRate)=0;
	virtual void Close(void)=0;
	virtual int SetScreenIndex(int nScreenIndex)=0;

	static HPMP_MPScreen*Create(void);
};

HPMP_API void GetBitrateRange(int nWidth,int nHeight,int&nQualityMin,int&nQualityMax);

HPMP_API int HPMP_GetSupportHardwareDecoder(void);
HPMP_API int HPMP_GetSupportHardwareEncoder(void);
HPMP_API void HPMP_SetHardwareEncoderMaxCount(unsigned int nMaxCount);
HPMP_API void HPMP_SetHardwareDecoderMaxCount(unsigned int nMaxCount);


#endif