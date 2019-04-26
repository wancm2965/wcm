//H323APIType.h
#ifndef __H323APITYPE_H__
#define __H323APITYPE_H__

//#ifdef HAS_H323

#include <AUDEC/AUDEC_CodecID.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <IMXEC/IMXEC_VideoSetting.h>
#include <IMXEC/IMXEC_AudioSetting.h>
//#endif

#ifdef HAS_RTPRTMP

#include <RTPRTMP/AUDEC_CodecID.h>
#include <RTPRTMP/VIDEC_CodecDef.h>
#include <RTPRTMP/VIDEO_FORMAT.h>
#include "../AudioVideoSetting.h"

#else

#include <AUDEC/AUDEC_CodecID.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <IMXEC/IMXEC_VideoSetting.h>
#include <IMXEC/IMXEC_AudioSetting.h>

#endif


#include <map>
#include <string>


#define	MAX_AUDIOCAP_NUM		32
#define	MAX_VIDEOCAP_NUM		56
#define	MAX_H239VIDEOCAP_NUM	20


typedef struct tagVIDEO_CAPABILITY_ITEM{
	VIDEC_CODEC_TYPE		VideoCodecType;			//能力类型
	int						nMaxBitrate;	

	//以下参数对视频能力有效(参数不够可以增加)
	int				nQCIF;				//能力中QCIF参数的值
	int				nCIF;				//能力中CIF参数的值
	int				n4CIF;				//能力中4CIF参数的值
	VIDEO_FORMAT_TYPE	VideoFormatType;
	int             nEntryNum;
	int             nFormatValue;
	int             nVideoFrameRate;
	int             nDirection;         //传输方向 cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit

	tagVIDEO_CAPABILITY_ITEM()
	{
		VideoCodecType = VIDEC_CODEC_H264;
		nQCIF = 0;
		nCIF = 0;
		n4CIF = 0;
		VideoFormatType =VIDEO_FORMAT_QCIF;
		nEntryNum =0;
		nVideoFrameRate =0;
		nDirection =0;
	}
}VIDEO_CAPABILITY_ITEM;

typedef struct tagAUDIO_CAPABILITY_ITEM
{
	X_AUDIO_CODEC_ID	AudioCodecID;
	int					nMaxBitrate;
	int                 nEntryNum;
	int                 nDirection;         //传输方向 cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit

	tagAUDIO_CAPABILITY_ITEM(void)
	{
		AudioCodecID=X_AUDIO_CODEC_G711U;
		nMaxBitrate=64;
		nEntryNum =0;
		nDirection =0;
	}
}AUDIO_CAPABILITY_ITEM;

//////////////////////////////////////////////////////////////////////////
struct AudioCap
{
	unsigned int				m_nCapNum;	//有效音频能力集个数
	AUDIO_CAPABILITY_ITEM		m_Caps[MAX_AUDIOCAP_NUM];

	AudioCap()
	{
		m_nCapNum = 0;
	}
};

struct VideoCap
{
	unsigned int				m_nCapNum;	//有效视频能力集个数
	VIDEO_CAPABILITY_ITEM		m_Caps[MAX_VIDEOCAP_NUM];

	VideoCap()
	{
		m_nCapNum = 0;
	}
};

struct H239VideoCap
{
	unsigned int				m_nCapNum;	//有效双流能力集个数
	VIDEO_CAPABILITY_ITEM		m_Caps[MAX_H239VIDEOCAP_NUM];

	H239VideoCap()
	{
		m_nCapNum = 0;
	}
};


//信道信息
typedef struct tagCHANNEL_INFO{
	X_AUDIO_CODEC_ID	AudioCodecID;
	VIDEC_CODEC_TYPE	VideoCodecType;
	VIDEO_FORMAT_TYPE	VideoFormatType;

	int 			nDirection;			// Receive=1/Transmit=2/ReceiveAndTransmit=3
	int 			nPayloadType;		// G711A/G711U/G729/H261/H263/H224
	int 			nMaxBitrate;		// 速率


	unsigned long	ulRTPLocalIP;		// 信道本端RTP地址
	unsigned short	nRTPLocalPort;		// 信道本端RTP端口号
	unsigned long	ulRTPRemoteIP;		// 信道对端RTP地址
	unsigned short	nRTPRemotePort;		// 信道对端RTP端口号

	unsigned long	ulRTCPLocalIP;		// 信道本端RTCP地址
	unsigned short	nRTCPLocalPort;		// 信道本端RTCP端口号
	unsigned long	ulRTCPRemoteIP;		// 信道对端RTCP地址
	unsigned short	nRTCPRemotePort;	// 信道对端RTCP端口号
	int				nH264Level;			// H264相关level
	int				nWidth;				// 分辨率宽
	int				nHeight;			// 分辨率高
	int             nFrameRate;         // 帧率

}CHANNEL_INFO;

#endif