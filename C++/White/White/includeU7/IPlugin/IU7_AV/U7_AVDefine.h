#pragma once
#include "AVCONSys/AUDEC/AUDEC_CodecID.h"
#include "AVCONSys/VIDEC/VIDEC_CodecDef.h"

//=======================================================================================
// 类型定义
//=======================================================================================
// 音频编码器类型定义
typedef enum {
	HPAV_AUDIO_CODEC_8		= X_AUDIO_CODEC_AMR_WB_9,		// 8kbps
	HPAV_AUDIO_CODEC_16		= X_AUDIO_CODEC_AMR_WB_16,		// 16kbps
	HPAV_AUDIO_CODEC_24		= X_AUDIO_CODEC_G7221_14_24,	// 24kbps
	HPAV_AUDIO_CODEC_32		= X_AUDIO_CODEC_G7221_14_32,	// 32kbps
	HPAV_AUDIO_CODEC_48		= X_AUDIO_CODEC_G7221_14_48,	// 48kbps
}THPAVAudioCodec;

// 视频编码器类型定义
typedef enum {
	HPAV_VIDEO_CODEC_H263P		= VIDEC_CODEC_H263P,		// H263+
	HPAV_VIDEO_CODEC_H264		= VIDEC_CODEC_H264,			// H264+
	HPAV_VIDEO_CODEC_H264_SVC	= VIDEC_CODEC_H264_SVC,		// H264 SVC
}THPAVVideoCodec;

// 网络类型定义
typedef enum {
	HPAV_VIDEO_NETWORK_INTERNET		= 0,	// 互联网
	HPAV_VIDEO_NETWORK_LAN,					// 局域网
	HPAV_VIDEO_NETWORK_PRIVATE,				// 专网
	HPAV_VIDEO_NETWORK_SATELLITE,			// 卫星网
	HPAV_VIDEO_NETWORK_CDMA,				// CDMA
}THPAVVideoNetwork;

//接口StartRecordToAVI(const char*cszPathName,int nStandardFormat=1)第二个参数
//nStandardFormat可以为以下宏定义值：
#define WRITER_STANDARD_FORMAT_NONE		0
#define WRITER_STANDARD_FORMAT_AVI		1
#define WRITER_STANDARD_FORMAT_MKV		2
#define WRITER_STANDARD_FORMAT_MP3		3
#define WRITER_STANDARD_FORMAT_MP4		4

//---------------------------------------------------------------------------------------
// 视频分辨率
typedef std::map<int,std::string> VIDEO_SIZE_MAP;
typedef VIDEO_SIZE_MAP::iterator VIDEO_SIZE_MAP_IT;

// 音频设备信息结构
typedef struct _AUDIO_DEV
{
	std::string	strDeviceName;		// 设备名称
	int			nDeviceNo;			// 设备编号
}AUDIO_DEV,*PAUDIO_DEV;

typedef std::map<int,AUDIO_DEV> AUDIO_DEV_MAP;
typedef AUDIO_DEV_MAP::iterator AUDIO_DEV_MAP_IT;

// 视频设备信息结构
typedef struct _VIDEO_DEV
{
	std::string	strDeviceName;		// 设备名称
	int			nDeviceNo;			// 设备编号
	bool		bHDDevice;			// 是否为高清卡
	bool		bHaveSoundCard;		// 是否有声卡
}VIDEO_DEV,*PVIDEO_DEV;

typedef std::map<int,VIDEO_DEV> VIDEO_DEV_MAP;
typedef VIDEO_DEV_MAP::iterator VIDEO_DEV_MAP_IT;

/************************************************************************/
/*音视频设置参数信息													*/
//视频输入设置参数信息
typedef struct _VIDEOSET_INFO
{
	int	nVideoDev;			// 视频设备
	int nVideoCodec;		// 视频编码器
	int	nVideoFrame;		// 图像帧率

	int	nBitrateCtrlType;	// 码流控制
	bool bVideoDenoise;		// 图像去噪
	int	nVideoSize;			// 分辨率
	int	nVideoStream;		// 视频码流

	int nVideoWidth;		// 分辨率宽度
	int nVideoHeigth;		// 分辨率高度
}VIDEOSET_INFO, *PVIDEOSET_INFO;

typedef std::map<int, VIDEOSET_INFO> VIDEOSET_INFO_MAP;
typedef VIDEOSET_INFO_MAP::iterator VIDEOSET_INFO_MAP_IT;

//音视频设置参数信息
typedef struct _AUDIOSET_INFO
{
	int	nSpeakerDev;		// 扬声器

	int nMicDev;			// 麦克风
	int	nAudioInType;		// 音频输入类型
	bool bAutoSetMicVol;	// 自动调整麦克风音量

	int nAudioCodec;		// 音频编码器
	int nFEC;				// 前向纠错
	bool bMicBoost;			// 麦克风增强
	bool bVAD;				// 静音检测
	bool bEchoCancel;		// 回音消除
	bool bLipSync;			// 唇音同步

	bool bSupportbPostAGC;	// 是否支持自动增益
	bool bEnablePostPos;	// 是否启用自动增益
	int nPostposition;		// 后置自动增益
	int nNoisSuppression;	// 噪音抑制

}AUDIOSET_INFO, *PAUDIOSET_INFO;

typedef std::map<int,AUDIOSET_INFO> AUDIOSET_INFO_MAP;
typedef AUDIOSET_INFO_MAP::iterator AUDIOSET_INFO_MAP_IT;
/************************************************************************/
