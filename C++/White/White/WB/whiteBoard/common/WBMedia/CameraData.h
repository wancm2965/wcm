#pragma once
#include <NETEC/NETEC_Setting.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <audec/AUDEC_CodecID.h>
#include <string>
#include "../WB/whiteBoard/tstring.h"
//接收方数据
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
		//默认构造函数，赋予一些初始值
		nVideoID = 0;
	};
	SCameraReceiveData(unsigned long _nAudioID, unsigned long _nVideoID, LPCTSTR _sNodeID,
		LPCTSTR _sNATIP, unsigned short _nNATPort, LPCTSTR _sLocalIP, unsigned short _nLocalPort,
		LPCTSTR _sMCUID, LPCTSTR _sMCUIP, unsigned short _nServerPort, LPCTSTR _sUCCUerID)
	{
		//拷贝构造函数
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

//摄像头连接的网络信息数据
struct SCameraServerData
{
	//服务器地址和端口
	std::string sServerAddress;
	unsigned short nServerPort;
	//默认直接连接
	//略
	//音视频传输协议类型
	NETEC_Setting::PROTOCOL_TYPE nAudioType;
	NETEC_Setting::PROTOCOL_TYPE nVideoType;
	//默认构造函数
	SCameraServerData()
	{
		sServerAddress = std::string("www.avcon.com.cn");
		nServerPort = 4222;
		nAudioType = NETEC_Setting::PT_UDP;
		nVideoType = NETEC_Setting::PT_RTP;
	};
	//拷贝构造函数
	SCameraServerData(struct SCameraServerData &data):
	nServerPort(data.nServerPort), nVideoType(data.nVideoType), nAudioType(data.nAudioType)
	{
		sServerAddress = data.sServerAddress;
	};
};

//摄像头创建者的信息数据
struct SCameraLocalData
{
	//设备的音视频ID索引
	unsigned long nLocalAudioID;
	unsigned long nLocalVideoID;
	//帧率， 比特，解码类型，高度，宽度信息
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
	//默认构造函数
	Camera()
	{
		//赋予一些默认初始值
	};
	//拷贝构造函数
	Camera(struct Camera &data)
		:m_nAudioID(data.m_nAudioID),m_nDevID(data.m_nDevID), m_VideoCodecType(data.m_VideoCodecType)
		,m_nFrameRate(data.m_nFrameRate), m_nBitrate(data.m_nBitrate), m_nWidth(data.m_nWidth)
		,m_nHeight(data.m_nHeight), m_AudioCodecID(data.m_AudioCodecID){};

}CameraSet,*pCameraSet;