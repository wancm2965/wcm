//AVVideo.h
#ifndef __AVVIDEO_H__
#define __AVVIDEO_H__

typedef enum VIDEO_SIZE
{
	VIDEO_SIZE_176X144=1,
	VIDEO_SIZE_240X176,
	VIDEO_SIZE_320X240,
	VIDEO_SIZE_352X288,
	VIDEO_SIZE_640X480,
	VIDEO_SIZE_704X576,
	VIDEO_SIZE_720X576,
	VIDEO_SIZE_480X352,
	VIDEO_SIZE_320X480
}VIDEO_SIZE;

typedef enum VIDEO_QUALITY
{	
	WORST=0,
	WORSE,
	BAD,
	GOOD,
	BETTER,
	BEST
}VIDEO_QUALITY;

typedef enum VIDEO_CODEC
{
	AVV1=0,		//divx502
	AVV2,		//divx505
	AVV3,		//xvid
	AVV4,		//H263
	AVV5,		//H263PLUS	
	AVV6,		//H264
	AVVIDEO_CODEC_H263PP,
	AVVIDEO_CODEC_WMV9,	
	AVVIDEO_CODEC_WMVA,
	AVVIDEO_CODEC_X264_VBR_HARDWARE,
	AVVIDEO_CODEC_X264_CBR_HARDWARE,
	AVVIDEO_CODEC_X264_VBR,
	AVVIDEO_CODEC_X264_CBR,
	AVVIDEO_CODEC_H263P_VBR,
	AVVIDEO_CODEC_H263P_CBR,
	AVVIDEO_CODEC_LSCC
}VIDEO_CODEC;

#define AVVIDEO_CODEC_X264_VBR_SOFTWARE AVVIDEO_CODEC_WMVA
#define AVVIDEO_CODEC_X264_CBR_SOFTWARE AVVIDEO_CODEC_WMV9

//{{xmc 20060630
//#ifndef HPMD_TYPE
//typedef enum
//{
//	HPMD_1X1=0,
//	HPMD_1X2,
//	HPMD_1P2,
//	HPMD_2X2,
//	HPMD_3X2,
//	HPMD_1P5,
//	HPMD_COUNT
//}HPMD_TYPE;
//#endif
//}}


#pragma pack(push,1)

/*
视频帧头
*/
typedef struct tagAVVIDEO_FRAME_HEADER
{
	unsigned char	ucVideoCodec;
	unsigned char	ucFrameRate;//PAL1-25 NTSC1-30
	unsigned short	usVideoWidth;
	unsigned short	usVideoHeight;
}AVVIDEO_FRAME_HEADER,*PAVVIDEO_FRAME_HEADER;

/*
视频压缩后包头，后面是包的数据
*/
typedef struct tagAVVIDEO_PACKET_HEADER
{
	bool 					bKeyFrame;	//是否是关键帧
	unsigned long			ulTimeStamp;//包产生的时间戳
	AVVIDEO_FRAME_HEADER	AVVideoFrameHeader;//视频帧头
}AVVIDEO_PACKET_HEADER,*PAVVIDEO_PACKET_HEADER;
#pragma pack(pop,1)
#pragma pack(push,4)


AFX_EXT_CLASS int GetVideoCaptureDeviceCount(void);
AFX_EXT_CLASS bool GetVideoCaptureDeviceName(int nVideoCaptureDeviceIndex,char*szVideoCaptureDeviceName);
AFX_EXT_CLASS bool IsVGADevice(int nDevID);

/***********************************************************************/
/*采集RGB24的视频，并压缩
/***********************************************************************/
class IAVVideoCaptureEx:public AVSource
{
public:
	IAVVideoCaptureEx(){};
	virtual~IAVVideoCaptureEx(){};
public:
	/***********************************************************************/
	/*功能：释放视频采集所占有的所有资源
	/*参数：
	/*返回：
	/*备注：请在删除该对象前调用此函数，以确保释放所有资源
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;

	/***********************************************************************/
	/*功能：是否采集视频
	/*参数：（bool bCapture）是否采集视频，如果bCapture=true采集，否则停止采集
	/*返回：如果已经打开视频设备则返回true，否则返回false
	/*备注：
	/***********************************************************************/
	virtual bool Capture(bool bCapture)=0;
	
	/***********************************************************************/
	/*功能：判断是否在正在采集视频
	/*参数：
	/*返回：如果正在采集视频则返回true，否则返回false
	/*备注：如果没有打开视频设备的也返回false
	/***********************************************************************/
	virtual bool IsCapturing(void)=0;

	/***********************************************************************/
	/*功能：是否显示视频窗口
	/*参数：(bool bShow) 是否显示视频窗口，如果bShow=true显示，否则隐藏
	/*返回：如果没有打开视频设备则返回false，否则返回true
	/*备注：
	/***********************************************************************/
	virtual bool ShowVideoWindow(bool bShow)=0;
	
	/***********************************************************************/
	/*功能：判断视频窗口是否可见
	/*参数：
	/*返回：如果视频窗口可见则返回true，否则返回false
	/*备注：如果没有打开视频设备的也返回false
	/***********************************************************************/
	virtual bool IsVideoWindowVisible(void)=0;
	
	/***********************************************************************/
	/*功能：设置视频窗口的位置和大小
	/*参数：(int x, int y, int cx, int cy)x，y为左上角座标，cx为视频窗口宽度，
	/*		cy为视频窗口高度
	/*返回：如果没有打开视频设备则返回false，否则返回true
	/*备注：
	/***********************************************************************/
	virtual bool SetVideoWindowPosition(int x, int y, int cx, int cy)=0;
	
	/***********************************************************************/
	/*功能：捕获视频的一帧保存为指定的位图文件(RGB24)
	/*参数：(const char*cszBitmapFile)要保存的位图文件名
	/*返回：如果捕获并保存成功，则返回true，否则返回false
	/*备注：如果视频设备没有打开也返回false
	/***********************************************************************/
	virtual bool CaptureToBitmapFile(const char*cszBitmapFile)=0;

	/***********************************************************************/
	/*功能：获取视频原始宽度
	/*参数：
	/*返回：如果没有打开视频设备则返回0，否则返回视频原始宽度
	/*备注：
	/***********************************************************************/
	virtual int GetWidth(void)=0;

	/***********************************************************************/
	/*功能：获取视频原始高度
	/*参数：
	/*返回：如果没有打开视频设备则返回0，否则返回视频原始高度
	/*备注：
	/***********************************************************************/
	virtual int GetHeight(void)=0;

	/***********************************************************************/
	/*功能：显示视频调整窗口
	/*参数：
	/*返回：
	/*备注：如果没有打开视频设备，则不显示任何窗口
	/***********************************************************************/
	virtual void AdjustVideo(void)=0;
	
	/***********************************************************************/
	/*功能：显示视频输入窗口
	/*参数：
	/*返回：
	/*备注：如果没有打开视频设备或视频设备没有提供视频输入接口，则不显示任何窗口
	/***********************************************************************/
	virtual void Crossbar(void)=0;

	/***********************************************************************/
	/*功能：判断视频设备是否提供了视频输入接口
	/*参数：
	/*返回：如果视频设备提供了视频输入接口，则返回true，否则返回false
	/*备注：如果视频设备没有打开也返回false
	/***********************************************************************/
	virtual bool CanCrossBar(void)=0;

	/***********************************************************************/
	/*功能：颠倒图象
	/*参数：(bool bInverted)是否颠倒图象，如果bInverted=true则使图象颠倒，
	/*		否则正常显示图象
	/*返回：
	/*备注：如果使图象颠倒，则以后采集的图象就是倒过来的，
	/***********************************************************************/
	virtual void Invert(bool bInverted)=0;
	
	/***********************************************************************/
	/*功能：判断图象是否是颠倒的
	/*参数：
	/*返回：如果是颠倒的则返回true，否则返回false
	/*备注：
	/***********************************************************************/
	virtual bool IsInverted(void)=0;
	virtual bool IsDeinterlaceEnable(void)=0;
	virtual void EnableDeinterlace(bool bDeinterlace)=0;

	virtual bool SetH263PacketsSink(AVSink*pH263PacketsSink)=0;
	virtual void StartManualKeyFrame(void)=0;
	virtual void EnableSkipDisplay(bool bEnable)=0;

	virtual void EnableDenoise(bool bDenoise)=0;
	virtual bool IsDenoiseEnable(void)=0;

	//{{xmc add 20060630
	virtual bool EnableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex=0)=0;
	virtual bool DisableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex=0)=0;
	//}}

	//{{xmc add 20070505
	virtual void EnableTitle(bool bEnable)=0;
	virtual bool IsTitleEnable(void)=0;
	virtual void SetTitleText(const TCHAR*cszText,int nAlign=0)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT
	//}}
};

/***********************************************************************/
/*功能：创建视频采集压缩对象
/*参数：HWND hWnd[显示视频的窗口句柄],int nDevice[视频设备的序号，从0开始],
/*		unsigned short usFrameRate[视频帧率 PAL1-25,NTSC1-30],
/*		int nWidth[视频的宽度],int nHeight[视频的高度],
/*		VIDEO_QUALITY VideoQuality[视频质量],bool bDeinterlace[消除隔行扫描],
/*		VIDEO_CODEC VideoCodec[视频编码类型]
/*返回：如果创建成功则返回视频采集压缩对象，否则返回NULL
/*备注：如果无法打开视频设备或视频不支持的分辨率或不支持的视频编码都将返回NULL
/***********************************************************************/
AFX_EXT_CLASS IAVVideoCaptureEx*CreateVideoCaptureEx(HWND hWnd,int nDevice,unsigned short usFrameRate,int nWidth,int nHeight,VIDEO_CODEC VideoCodec,VIDEO_QUALITY VideoQuality,bool bDeinterlace,int nMaxBitrate,bool bAutoFrameRate);

/***********************************************************************/
/*视频编码器，压缩视频，只能压缩RGB24的位图
/***********************************************************************/
class IAVVideoEncoder
{
public:
	IAVVideoEncoder(){};
	virtual ~IAVVideoEncoder(void){};
public:
	/***********************************************************************/
	/*功能：释放编码器所占的资源
	/*参数：
	/*返回：
	/*备注：删除编码器对象前应先调用此方法，以确保释放所有资源
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;
	
	/***********************************************************************/
	/*功能：获取编码器类型
	/*参数：
	/*返回：编码器类型
	/*备注：
	/***********************************************************************/
	virtual VIDEO_CODEC GetCodec(void)=0;
	
	/***********************************************************************/
	/*功能：压缩一帧图象
	/*参数：void *input[in,未压缩的图象数据], int inputSizeBytes[in,未压缩的图象数据大小],
	/*		void *output[out,压缩后的图象数据], int *outputSizeBytes[out,压缩后的图象数据大小],
	/*		bool*pbIsKeyFrame[out,是否是关键帧]
	/*返回：如果成功，则返回压缩后的图象数据大小，否则返回0
	/*备注：只能压缩RGB24的位图
	/***********************************************************************/
	virtual int Encode(void *input, int inputSizeBytes, void *output, int *outputSizeBytes,bool*pbIsKeyFrame)=0;

	virtual bool SetH263PacketsSink(AVSink*pH263PacketsSink){return false;};

	virtual void StartManualKeyFrame(void){};

	virtual void GetExtraData(const void**ppExtraData,unsigned int*pnExtraDataLen){};
};

/***********************************************************************/
/*功能：创建视频编码器
/*参数：VIDEO_CODEC VideoCodec[视频编码类型],int nWidth[视频的宽度],
/*		int nHeight[视频的高度],unsigned short usFrameRate[视频帧率 PAL1-25,NTSC1-30],
/*		VIDEO_QUALITY VideoQuality[视频质量],bool bDeinterlace[消除隔行扫描],
/*返回：如果是支持的编码器则返回视频编码器对象，否则返回NULL
/*备注：
/***********************************************************************/
AFX_EXT_CLASS IAVVideoEncoder*CreateVideoEncoder(VIDEO_CODEC VideoCodec,
												 int cx,int cy,
												 unsigned short usFrameRate,	
												 VIDEO_QUALITY VideoQuality,
												 bool bI420=false);

/***********************************************************************/
/*视频解码器，解压缩视频帧，只能解压缩成RGB24的位图
/***********************************************************************/
class IAVVideoDecoder
{
public:
	IAVVideoDecoder(void){};
	virtual~IAVVideoDecoder(void){};
public:
	/***********************************************************************/
	/*功能：释放解码器所占的资源
	/*参数：
	/*返回：
	/*备注：删除解码器对象前应先调用此方法，以确保释放所有资源
	/***********************************************************************/
	virtual void ReleaseConnections(void)=0;
	
	virtual void SetPostProcessQuality(int nQuality)=0;

	virtual bool Decompress(const unsigned char *pSrc,unsigned int nSrcLen,unsigned char* dst[],int dstStride[])=0;
};

/***********************************************************************/
/*功能：创建视频解码器
/*参数：VIDEO_CODEC VideoCodec[视频解码类型],int cx[视频的宽度],int cy[视频的高度]
/*返回：如果是支持的解码器则返回视频解码器对象，否则返回NULL
/*备注：
/***********************************************************************/
AFX_EXT_CLASS IAVVideoDecoder*CreateVideoDecoder(VIDEO_CODEC VideoCodec,int cx,int cy,unsigned short usFrameRate,const void *pExtraData,unsigned int nExtraDataLen);

/***********************************************************************/
/*功能：将VIDEO_SIZE转换成宽度和高度
/*参数：VIDEO_SIZE VideoSize[in,视频分辨率],int&nWidth[out,宽度],int&nHeight[out,高度]
/*返回：如果是支持的分辨率则返回true,并返回宽度和高度，否则返回false
/*备注：
/***********************************************************************/
AFX_EXT_CLASS bool VideoSizeConvert(VIDEO_SIZE VideoSize,int&nWidth,int&nHeight);

class XRGBCallback
{
public:
	XRGBCallback(void){};
	virtual~XRGBCallback(void){};
public:
	virtual void OnXRGBCallbackData(unsigned char*pRGBData,int nWidth,int nHeight)=0;
};


#define AV_VIDEO_PLAYER_RESIZE	(WM_USER+7086)
/***********************************************************************/
/*视频播放器，具有解码和同步播放功能
/***********************************************************************/
class IAVVideoPlayerEx : public AVSink
{
public:
	IAVVideoPlayerEx(){};
	virtual~IAVVideoPlayerEx(){};
public:
	/***********************************************************************/
	/*功能：释放播放器所占的资源
	/*参数：
	/*返回：
	/*备注：删除播放器对象前应先调用此方法，以确保释放所有资源
	/***********************************************************************/
	virtual void ReleaseConnections()=0;
	/***********************************************************************/
	/*功能：获取视频原始宽度
	/*参数：
	/*返回：如果没有打开视频设备则返回0，否则返回视频原始宽度
	/*备注：
	/***********************************************************************/
	virtual int GetWidth(void)=0;

	/***********************************************************************/
	/*功能：获取视频原始高度
	/*参数：
	/*返回：如果没有打开视频设备则返回0，否则返回视频原始高度
	/*备注：
	/***********************************************************************/
	virtual int GetHeight(void)=0;

	/***********************************************************************/
	/*功能：是否显示视频窗口
	/*参数：(bool bShow) 是否显示视频窗口，如果bShow=true显示，否则隐藏
	/*返回：如果没有打开视频设备则返回false，否则返回true
	/*备注：
	/***********************************************************************/
	virtual bool ShowVideoWindow(bool bShow)=0;
	
	/***********************************************************************/
	/*功能：判断视频窗口是否可见
	/*参数：
	/*返回：如果视频窗口可见则返回true，否则返回false
	/*备注：如果没有打开视频设备的也返回false
	/***********************************************************************/
	virtual bool IsVideoWindowVisible(void)=0;
	
	/***********************************************************************/
	/*功能：设置视频窗口的位置和大小
	/*参数：(int x, int y, int cx, int cy)x，y为左上角座标，cx为视频窗口宽度，cy为视
	/*		频窗口高度
	/*返回：如果没有打开视频设备则返回false，否则返回true
	/*备注：
	/***********************************************************************/
	virtual bool SetVideoWindowPosition(int x, int y, int cx, int cy)=0;

	virtual void SetVideoQuality(int nVideoQuality)=0;
	virtual int GetVideoQuality(void)=0;

	virtual bool HasVideo(void)=0;
	virtual bool CaptureToBitmapFile(const char*cszBitmapFile)=0;
	virtual void EnableSkipDisplay(bool bEnable)=0;

	virtual void EnableVideoJitterBuffer(bool bEnable)=0;
	virtual bool IsVideoJitterBufferEnable(void)=0;

	//{{xmc add 20060629
	virtual bool UseHardwareDecoder(bool bUse)=0;
	virtual bool IsUsingHardwareDecoder(void)=0;
	virtual bool EnableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex=0)=0;
	virtual bool DisableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex=0)=0;
	//}}

	//{{xmc add 20070505
	virtual void EnableTitle(bool bEnable)=0;
	virtual bool IsTitleEnable(void)=0;
	virtual void SetTitleText(const TCHAR*cszText,int nAlign=0)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT
	//}}

	virtual void SetXRGBCallback(XRGBCallback*pXRGBCallback)=0;
};

/***********************************************************************/
/*功能：创建视频同步播放对象
/*参数：HWND hWnd[in,视频显示句柄],int cx[in,视频宽度],int cy[in,视频高度],
/*		unsigned short usFrameRate[in,视频播放帧率],
/*		VIDEO_CODEC VideoCodec[in,视频解码器类型],AVSyn*pAVSyn[in,同步器]
/*返回：如果创建成功则返回视频同步播放对象，否则返回NULL
/*备注：如果视频解码器类型不支持，就返回NULL
/***********************************************************************/
AFX_EXT_CLASS IAVVideoPlayerEx*CreateVideoPlayerEx(HWND hWnd,int cx,int cy,unsigned short usFrameRate,VIDEO_CODEC VideoCodec,AVSyn*pAVSyn);

AFX_EXT_CLASS bool IsVideoCaptureOpen(void);

//{{xmc add 20060629
AFX_EXT_CLASS long GetHiKHWDisplayChannelCount(void);
AFX_EXT_CLASS long GetHiKHWDecoderChannelCount(void);
AFX_EXT_CLASS bool SetHiKHWDisplayChannelTM(unsigned long ulDisplayChannelIndex,HPMD_TYPE HPType);
AFX_EXT_CLASS bool GetHiKHWDisplayChannelTM(unsigned long ulDisplayChannelIndex,HPMD_TYPE&HPType);
AFX_EXT_CLASS unsigned long GetHiKHWDisplayChannelRectCount(unsigned long ulDisplayChannelIndex);
//}}

//{{xmc add 20060814
AFX_EXT_CLASS void LoadHiKHWCapture(void);
AFX_EXT_CLASS void UnLoadHiKHWCapture(void);
AFX_EXT_CLASS void LoadHiKHWDisplay(void);
AFX_EXT_CLASS void UnLoadHiKHWDisplay(void);
//}}


#endif

