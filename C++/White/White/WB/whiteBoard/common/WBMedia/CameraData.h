#pragma once
#include <NETEC/NETEC_Setting.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <audec/AUDEC_CodecID.h>
#include <string>
#include "../WB/whiteBoard/tstring.h"
//���շ�����
struct SCameraReceiveData 
{
	unsigned long nAudioID;
	unsigned long nVideoID;
	std::string strNodeID;
	std::string strNATIP;
	unsigned short nNATPort;
	std::string strLocalIP;
	unsigned short nLocalPort;
	std::string strMCUID;
	std::string strMCUIP;
	unsigned short nServerPort;
	std::string m_strUCCUerID;
	SCameraReceiveData()
	{
		//Ĭ�Ϲ��캯��������һЩ��ʼֵ
		nVideoID = 0;
	};
	SCameraReceiveData(unsigned long _nAudioID, unsigned long _nVideoID, LPCTSTR _sNodeID,
		LPCTSTR _sNATIP, unsigned short _nNATPort, LPCTSTR _sLocalIP, unsigned short _nLocalPort,
		LPCTSTR _sMCUID, LPCTSTR _sMCUIP, unsigned short _nServerPort, LPCTSTR _sUCCUerID)
	{
		//�������캯��
		nAudioID = _nAudioID;
		nVideoID = _nVideoID;
		strNodeID = std::string(ToStr(_sNodeID));
		strLocalIP = std::string(ToStr(_sNATIP));
		nNATPort = _nNATPort;
		strLocalIP = std::string(ToStr(_sLocalIP));
		nLocalPort = _nLocalPort;
		strMCUID = std::string(ToStr(_sMCUID));
		strMCUIP = std::string(ToStr(_sMCUIP));
		nServerPort = _nServerPort;
		m_strUCCUerID = std::string(ToStr(_sUCCUerID));
	};
};

//����ͷ���ӵ�������Ϣ����
struct SCameraServerData
{
	//��������ַ�Ͷ˿�
	std::string sServerAddress;
	unsigned short nServerPort;
	//Ĭ��ֱ������
	//��
	//����Ƶ����Э������
	NETEC_Setting::PROTOCOL_TYPE nAudioType;
	NETEC_Setting::PROTOCOL_TYPE nVideoType;
	//Ĭ�Ϲ��캯��
	SCameraServerData()
	{
		sServerAddress = std::string("www.avcon.com.cn");
		nServerPort = 4222;
		nAudioType = NETEC_Setting::PT_UDP;
		nVideoType = NETEC_Setting::PT_RTP;
	};
	//�������캯��
	SCameraServerData(struct SCameraServerData &data):
	nServerPort(data.nServerPort), nVideoType(data.nVideoType), nAudioType(data.nAudioType)
	{
		sServerAddress = data.sServerAddress;
	};
};

//����ͷ�����ߵ���Ϣ����
struct SCameraLocalData
{
	//�豸������ƵID����
	unsigned long nLocalAudioID;
	unsigned long nLocalVideoID;
	//֡�ʣ� ���أ��������ͣ��߶ȣ������Ϣ
};

typedef struct Camera{
	unsigned long       m_nAudioID;
	unsigned long		m_nDevID;
	VIDEC_CODEC_TYPE	m_VideoCodecType;
	int					m_nFrameRate;
	int					m_nBitrate;
	int					m_nWidth;
	int					m_nHeight;
	X_AUDIO_CODEC_ID	m_AudioCodecID;
	//Ĭ�Ϲ��캯��
	Camera()
	{
		//����һЩĬ�ϳ�ʼֵ
	};
	//�������캯��
	Camera(struct Camera &data)
		:m_nAudioID(data.m_nAudioID),m_nDevID(data.m_nDevID), m_VideoCodecType(data.m_VideoCodecType)
		,m_nFrameRate(data.m_nFrameRate), m_nBitrate(data.m_nBitrate), m_nWidth(data.m_nWidth)
		,m_nHeight(data.m_nHeight), m_AudioCodecID(data.m_AudioCodecID){};

}CameraSet,*pCameraSet;