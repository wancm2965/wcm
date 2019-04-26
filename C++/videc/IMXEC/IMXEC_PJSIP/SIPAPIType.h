//SIPAPIType.h

#ifndef __SIPCAPAPITYPE_H__
#define __SIPCAPAPITYPE_H__

#ifdef HAS_SIP

#include <AUDEC/AUDEC_CodecID.h>
#include <VIDEC/VIDEC_CodecDef.h>
#endif

#ifdef HAS_RTPRTMP

#include <RTPRTMP/AUDEC_CodecID.h>
#include <RTPRTMP/VIDEC_CodecDef.h>
#include <RTPRTMP/VIDEO_FORMAT.h>
#include "../AudioVideoSetting.h"

#endif


#include <map>
#include <string>


#define ENABLE_VIDEC_CODEC_H263		1
#define ENABLE_VIDEC_CODEC_H263P	1
#define ENABLE_VIDEC_CODEC_H264		1
#define ENALBE_VIDEC_CODEC_AH400	1
#define ENABLE_VIDEC_CODEC_DH264	1
#define ENABLE_VIDEC_CODEC_HIKH		1


#define	MAX_AUDIOCAP_NUM		32
#define	MAX_VIDEOCAP_NUM		56
#define	MAX_H239VIDEOCAP_NUM	20
#define	MAX_DATACAP_NUM		    20

typedef struct tagSIPVIDEOCapability{
	VIDEC_CODEC_TYPE		eCapType;			//能力类型

	//以下参数对视频能力有效(参数不够可以增加)
	int				nBitrate;			//速率(单位：Kbit/s)
	int				nQCIF;				//能力中QCIF参数的值
	int				nCIF;				//能力中CIF参数的值
	int				n4CIF;				//能力中4CIF参数的值
	int             nLevel;             //用于H264级别 用于H26+及H263++高分辨率宽度值
	int             nProfile;           //用于H264框架 用于H26+及H263++高分辨率高度值
	int             nPayLoad;           //payLoad type
	int             nVideoFormatType;         //分辨率
	int             nFormatValue;             //分辨率对应实际大小
	int             nFrame;
	int             nDirection;

	tagSIPVIDEOCapability()
	{
		eCapType = VIDEC_CODEC_H264;
		nBitrate = 0;
		nQCIF = 0;
		nCIF = 0;
		n4CIF = 0;
		nLevel=0;
		nProfile=0;
		nPayLoad=0;
		nFormatValue=0;
		nFrame=0;
		nDirection=0;
	}
}SIPVIDEOCAPBILITY;

struct SIPVideoCap
{
	unsigned int	m_nCapNum;	//有效视频能力集个数
	SIPVIDEOCAPBILITY		m_Caps[MAX_VIDEOCAP_NUM];

	SIPVideoCap()
	{
		m_nCapNum = 0;
	}
};

struct SIPDualVideoCap
{
	unsigned int	m_nCapNum;	//有效视频能力集个数
	SIPVIDEOCAPBILITY		m_Caps[MAX_VIDEOCAP_NUM];

	SIPDualVideoCap()
	{
		m_nCapNum = 0;
	}
};

typedef struct tagSIPAUDIOCapability{
	X_AUDIO_CODEC_ID	AudioCodecID;

	int				nMaxBitrate;			//速率(单位：Kbit/s)
    int             nClockRate;         //时钟率
	int             nPayLoad;           //payLoad type
	int             nDirection;
	tagSIPAUDIOCapability()
	{
		AudioCodecID=X_AUDIO_CODEC_G711U;
		nMaxBitrate=64;
		nPayLoad=0;
		nClockRate=8000;
		nDirection=0;
	}
}SIPAUDIOCAPBILITY;

struct SIPAudioCap
{
	unsigned int	m_nCapNum;	//有效视频能力集个数
	SIPAUDIOCAPBILITY		m_Caps[MAX_AUDIOCAP_NUM];

	SIPAudioCap()
	{
		m_nCapNum = 0;
	}
};







/*
typedef struct tagVIDEO_CAPABILITY_ITEM{
	VIDEC_CODEC_TYPE		VideoCodecType;			//能力类型
	int						nMaxBitrate;	

	//以下参数对视频能力有效(参数不够可以增加)
	int				nQCIF;				//能力中QCIF参数的值
	int				nCIF;				//能力中CIF参数的值
	int				n4CIF;				//能力中4CIF参数的值

	tagVIDEO_CAPABILITY_ITEM()
	{
		VideoCodecType = VIDEC_CODEC_H264;
		nQCIF = 0;
		nCIF = 0;
		n4CIF = 0;
	}
}VIDEO_CAPABILITY_ITEM;

typedef struct tagAUDIO_CAPABILITY_ITEM
{
	X_AUDIO_CODEC_ID	AudioCodecID;
	int					nMaxBitrate;	

	tagAUDIO_CAPABILITY_ITEM(void)
	{
		AudioCodecID=X_AUDIO_CODEC_G711U;
		nMaxBitrate=64;
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

*/

#endif
