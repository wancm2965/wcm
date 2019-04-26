#pragma once
#include "AUDEC_CodecID.h"
#include "VIDEC_CodecDef.h"

//=======================================================================================
// ���Ͷ���
//=======================================================================================
// ��Ƶ���������Ͷ���
typedef enum {
	HPAV_AUDIO_CODEC_8		= X_AUDIO_CODEC_AMR_WB_9,		// 8kbps
	HPAV_AUDIO_CODEC_16		= X_AUDIO_CODEC_AMR_WB_16,		// 16kbps
	HPAV_AUDIO_CODEC_24		= X_AUDIO_CODEC_G7221_14_24,	// 24kbps
	HPAV_AUDIO_CODEC_32		= X_AUDIO_CODEC_G7221_14_32,	// 32kbps
	HPAV_AUDIO_CODEC_48		= X_AUDIO_CODEC_G7221_14_48,	// 48kbps
}THPAVAudioCodec;

// ��Ƶ���������Ͷ���
typedef enum {
	HPAV_VIDEO_CODEC_H263P		= VIDEC_CODEC_H263P,		// H263+
	HPAV_VIDEO_CODEC_H264		= VIDEC_CODEC_H264,			// H264+
	HPAV_VIDEO_CODEC_H264_SVC	= VIDEC_CODEC_H264_SVC,		// H264 SVC
}THPAVVideoCodec;

// �������Ͷ���
typedef enum {
	HPAV_VIDEO_NETWORK_INTERNET		= 0,	// ������
	HPAV_VIDEO_NETWORK_LAN,					// ������
	HPAV_VIDEO_NETWORK_PRIVATE,				// ר��
	HPAV_VIDEO_NETWORK_SATELLITE,			// ������
	HPAV_VIDEO_NETWORK_CDMA,				// CDMA
}THPAVVideoNetwork;

//---------------------------------------------------------------------------------------
// ��Ƶ�ֱ���
typedef std::map<int,std::string> VIDEO_SIZE_MAP;
typedef VIDEO_SIZE_MAP::iterator VIDEO_SIZE_MAP_IT;

// ��Ƶ�豸��Ϣ�ṹ
typedef struct _AUDIO_DEV
{
	std::string	strDeviceName;		// �豸����
	int			nDeviceNo;			// �豸���
}AUDIO_DEV,*PAUDIO_DEV;

typedef std::map<int,AUDIO_DEV> AUDIO_DEV_MAP;
typedef AUDIO_DEV_MAP::iterator AUDIO_DEV_MAP_IT;

// ��Ƶ�豸��Ϣ�ṹ
typedef struct _VIDEO_DEV
{
	std::string	strDeviceName;		// �豸����
	int			nDeviceNo;			// �豸���
	bool		bHDDevice;			// �Ƿ�Ϊ���忨
	bool		bHaveSoundCard;		// �Ƿ�������
}VIDEO_DEV,*PVIDEO_DEV;

typedef std::map<int,VIDEO_DEV> VIDEO_DEV_MAP;
typedef VIDEO_DEV_MAP::iterator VIDEO_DEV_MAP_IT;

/************************************************************************/
/*����Ƶ���ò�����Ϣ													*/
//��Ƶ�������ò�����Ϣ
typedef struct _VIDEOSET_INFO
{
	int	nVideoDev;			// ��Ƶ�豸
	int nVideoCodec;		// ��Ƶ������
	int	nVideoFrame;		// ͼ��֡��

	int	nBitrateCtrlType;	// ��������
	bool bVideoDenoise;		// ͼ��ȥ��
	int	nVideoSize;			// �ֱ���
	int	nVideoStream;		// ��Ƶ����


}VIDEOSET_INFO, *PVIDEOSET_INFO;

typedef std::map<int, VIDEOSET_INFO> VIDEOSET_INFO_MAP;
typedef VIDEOSET_INFO_MAP::iterator VIDEOSET_INFO_MAP_IT;

//����Ƶ���ò�����Ϣ
typedef struct _AUDIOSET_INFO
{
	int	nSpeakerDev;		// ������

	int nMicDev;			// ��˷�
	int	nAudioInType;		// ��Ƶ��������
	bool bAutoSetMicVol;	// �Զ�������˷�����

	int nAudioCodec;		// ��Ƶ������
	int nFEC;				// ǰ�����
	bool bMicBoost;			// ��˷���ǿ
	bool bVAD;				// �������
	bool bEchoCancel;		// ��������
	bool bLipSync;			// ����ͬ��

	int nPostposition;		// �����Զ�����
	int nNoisSuppression;	// ��������

}AUDIOSET_INFO, *PAUDIOSET_INFO;

typedef std::map<int,AUDIOSET_INFO> AUDIOSET_INFO_MAP;
typedef AUDIOSET_INFO_MAP::iterator AUDIOSET_INFO_MAP_IT;
/************************************************************************/
