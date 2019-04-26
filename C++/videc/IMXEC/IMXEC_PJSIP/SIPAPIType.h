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
	VIDEC_CODEC_TYPE		eCapType;			//��������

	//���²�������Ƶ������Ч(����������������)
	int				nBitrate;			//����(��λ��Kbit/s)
	int				nQCIF;				//������QCIF������ֵ
	int				nCIF;				//������CIF������ֵ
	int				n4CIF;				//������4CIF������ֵ
	int             nLevel;             //����H264���� ����H26+��H263++�߷ֱ��ʿ��ֵ
	int             nProfile;           //����H264��� ����H26+��H263++�߷ֱ��ʸ߶�ֵ
	int             nPayLoad;           //payLoad type
	int             nVideoFormatType;         //�ֱ���
	int             nFormatValue;             //�ֱ��ʶ�Ӧʵ�ʴ�С
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
	unsigned int	m_nCapNum;	//��Ч��Ƶ����������
	SIPVIDEOCAPBILITY		m_Caps[MAX_VIDEOCAP_NUM];

	SIPVideoCap()
	{
		m_nCapNum = 0;
	}
};

struct SIPDualVideoCap
{
	unsigned int	m_nCapNum;	//��Ч��Ƶ����������
	SIPVIDEOCAPBILITY		m_Caps[MAX_VIDEOCAP_NUM];

	SIPDualVideoCap()
	{
		m_nCapNum = 0;
	}
};

typedef struct tagSIPAUDIOCapability{
	X_AUDIO_CODEC_ID	AudioCodecID;

	int				nMaxBitrate;			//����(��λ��Kbit/s)
    int             nClockRate;         //ʱ����
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
	unsigned int	m_nCapNum;	//��Ч��Ƶ����������
	SIPAUDIOCAPBILITY		m_Caps[MAX_AUDIOCAP_NUM];

	SIPAudioCap()
	{
		m_nCapNum = 0;
	}
};







/*
typedef struct tagVIDEO_CAPABILITY_ITEM{
	VIDEC_CODEC_TYPE		VideoCodecType;			//��������
	int						nMaxBitrate;	

	//���²�������Ƶ������Ч(����������������)
	int				nQCIF;				//������QCIF������ֵ
	int				nCIF;				//������CIF������ֵ
	int				n4CIF;				//������4CIF������ֵ

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
	unsigned int				m_nCapNum;	//��Ч��Ƶ����������
	AUDIO_CAPABILITY_ITEM		m_Caps[MAX_AUDIOCAP_NUM];

	AudioCap()
	{
		m_nCapNum = 0;
	}
};

struct VideoCap
{
	unsigned int				m_nCapNum;	//��Ч��Ƶ����������
	VIDEO_CAPABILITY_ITEM		m_Caps[MAX_VIDEOCAP_NUM];

	VideoCap()
	{
		m_nCapNum = 0;
	}
};

struct H239VideoCap
{
	unsigned int				m_nCapNum;	//��Ч˫������������
	VIDEO_CAPABILITY_ITEM		m_Caps[MAX_H239VIDEOCAP_NUM];

	H239VideoCap()
	{
		m_nCapNum = 0;
	}
};

*/

#endif
