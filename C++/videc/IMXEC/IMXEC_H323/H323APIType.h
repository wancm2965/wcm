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
	VIDEC_CODEC_TYPE		VideoCodecType;			//��������
	int						nMaxBitrate;	

	//���²�������Ƶ������Ч(����������������)
	int				nQCIF;				//������QCIF������ֵ
	int				nCIF;				//������CIF������ֵ
	int				n4CIF;				//������4CIF������ֵ
	VIDEO_FORMAT_TYPE	VideoFormatType;
	int             nEntryNum;
	int             nFormatValue;
	int             nVideoFrameRate;
	int             nDirection;         //���䷽�� cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit

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
	int                 nDirection;         //���䷽�� cmCapDirection 1:cmCapReceive; 2:cmCapTransmit; 3:cmCapReceiveAndTransmit

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


//�ŵ���Ϣ
typedef struct tagCHANNEL_INFO{
	X_AUDIO_CODEC_ID	AudioCodecID;
	VIDEC_CODEC_TYPE	VideoCodecType;
	VIDEO_FORMAT_TYPE	VideoFormatType;

	int 			nDirection;			// Receive=1/Transmit=2/ReceiveAndTransmit=3
	int 			nPayloadType;		// G711A/G711U/G729/H261/H263/H224
	int 			nMaxBitrate;		// ����


	unsigned long	ulRTPLocalIP;		// �ŵ�����RTP��ַ
	unsigned short	nRTPLocalPort;		// �ŵ�����RTP�˿ں�
	unsigned long	ulRTPRemoteIP;		// �ŵ��Զ�RTP��ַ
	unsigned short	nRTPRemotePort;		// �ŵ��Զ�RTP�˿ں�

	unsigned long	ulRTCPLocalIP;		// �ŵ�����RTCP��ַ
	unsigned short	nRTCPLocalPort;		// �ŵ�����RTCP�˿ں�
	unsigned long	ulRTCPRemoteIP;		// �ŵ��Զ�RTCP��ַ
	unsigned short	nRTCPRemotePort;	// �ŵ��Զ�RTCP�˿ں�
	int				nH264Level;			// H264���level
	int				nWidth;				// �ֱ��ʿ�
	int				nHeight;			// �ֱ��ʸ�
	int             nFrameRate;         // ֡��

}CHANNEL_INFO;

#endif