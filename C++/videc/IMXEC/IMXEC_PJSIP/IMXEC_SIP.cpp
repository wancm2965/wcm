#include "stdafx.h"

#include "../DefineHeader/SIPDefineHeader.h"

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

#include <IMXEC/IMXEC_SIP.h>

#elif defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1)

#include <AVCONMP/IMXEC_SIP.h>

#endif

#include "CallMgr.h"

#ifdef HAS_SIP
#include "SIPCallMgr.h"
#endif

IMXEC_SIP::IMXEC_SIP(void)
{

}

IMXEC_SIP::~IMXEC_SIP(void)
{

}

/******************************************************************************
* AddVirtualLine
*描述：添加虚拟线路
*输入：	cszUserID		-用户帐号
		cszUserName		-用户名称
		cszPassword		-用户密码
		cszHost			-SIP代理服务器
*输出：无
*返回值：成功返回>=0[line id]，失败返回-1
*****************************************************************************/
int IMXEC_SIP::AddVirtualLine(const char*cszUserID,const char*cszUserName,const char*cszPassword,const char*cszSIPProxy,const char*cszAuthUserID,const char*cszDomain)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().AddVirtualLine(cszUserID,cszUserName,cszPassword,cszSIPProxy,cszAuthUserID,cszDomain);
#else
	return -1;
#endif
}

/******************************************************************************
* RemoveVirtualLine
*描述：删除虚拟线路
*输入：	nLineID			-线路标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_SIP::RemoveVirtualLine(int nLineID)
{
#ifdef HAS_SIP

	SIPCallMgr::Instance().HungupAll(nLineID);
	SIPCallMgr::Instance().RemoveVirtualLine(nLineID);

#else
	return ;
#endif
}

int IMXEC_SIP::SetCurrentVirtualLine(int nLineID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetCurrentVirtualLine(nLineID);
#else
	return -1;
#endif
}

int IMXEC_SIP::GetCurrentVirtualLine(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().GetCurrentVirtualLine();
#else
	return -1;
#endif
}

int IMXEC_SIP::IsVirtualLineRegistrationSuccess(int nLineID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().IsVirtualLineRegistrationSuccess(nLineID);
#else
	return -1;
#endif
}

int IMXEC_SIP::IsCurrentVirtualLineRegistrationSuccess(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().IsCurrentVirtualLineRegistrationSuccess();
#else
	return -1;
#endif
}

int IMXEC_SIP::IsVirtualLineRegistrationError(int nLineID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().IsVirtualLineRegistrationError(nLineID);
#else
	return -1;
#endif
}

int IMXEC_SIP::IsCurrentVirtualLineRegistrationError(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().IsCurrentVirtualLineRegistrationError();
#else
	return -1;
#endif
}

const char*IMXEC_SIP::GetVirtualLineRegistrationErrorCode(int nLineID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().GetVirtualLineRegistrationErrorCode(nLineID);
#else
	return "";
#endif
}

const char*IMXEC_SIP::GetCurrentVirtualLineRegistrationErrorCode(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().GetCurrentVirtualLineRegistrationErrorCode();
#else
	return "";
#endif
}

/******************************************************************************
* SetSIPListenPort
*描述：设置侦听端口
*输入：	nSIPListenPort			-侦听端口
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetSIPListenPort(unsigned short nSIPListenPort)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSIPListenPort(nSIPListenPort);
#else
	return -1;
#endif
}

/******************************************************************************
* SetLocalIP
*描述：设置注册帐号
*输入：	cszSIPID		-注册帐号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetLocalIP(const char*cszLocalIP)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetLocalIP(cszLocalIP);
#else
	return -1;
#endif
}

/******************************************************************************
* SetRTPBasePort
*描述：设置侦听端口
*输入：	nRTPBasePort			-RTP开始端口
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetRTPBasePort(unsigned short nRTPBasePort)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetRTPBasePort(nRTPBasePort);
#else
	return -1;
#endif
}


/******************************************************************************
* SetAudioDevParam
*描述：设置主流视频设备参数
*输入： nAudioCodecID		-音频编码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetAudioDevParam(nAudioCodecID);
#else
	return -1;
#endif
}

/******************************************************************************
* SetMainVideoDevParam
*描述：设置主流视频设备参数
*输入： nDevID				-设备标识
		nFrameRate			-帧率
		nBitrate			-码流
		nVideoCodecType		-视频编码
		nVideoFormatType	-视频格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
#else
	return -1;
#endif
}

/******************************************************************************
* SetSecondVideoDevParam
*描述：设置辅流视频设备参数
*输入： nDevID				-设备标识
		nFrameRate			-帧率
		nBitrate			-码流
		nVideoCodecType		-视频编码
		nVideoFormatType	-视频格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSecondVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
#else
	return -1;
#endif
}

/******************************************************************************
* SetSupportH239
*描述：设置是否支持H239
*输入：nSupportH239		-是否支持H239
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetSupportH239(int nSupportH239)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSupportH239(nSupportH239);
#else
	return -1;
#endif
}



/******************************************************************************
* Start
*描述：启动SIP服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::Start(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().Start();
#else
	return -1;
#endif
}

/******************************************************************************
* Stop
*描述：停止SIP服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::Stop(void)
{
#ifdef HAS_SIP
	SIPCallMgr::Instance().Stop();

	return 0;
#else
	return -1;
#endif
}

/******************************************************************************
* SetSIPTransportType
*描述：设置SIP信令传输类型
*输入：type			-类型
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetSIPTransportType(IMXEC_SIP::TRANS_TYPE type)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSIPTransportType(type);
#else
	return -1;
#endif
}


/******************************************************************************
* SetICETransportCount
*描述：设置创建的每种媒体流ICE通道数
*输入：	nCount		-每种媒体流ICE通道数(即如只有RTP则设为1，如还需要有RTCP则设为2)
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetICETransportCount(int nCount)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetICETransportCount(nCount);
#else
	return -1;
#endif
}
/******************************************************************************
* SetICEEnable
*描述：设置ICE应用状态
*输入：	bEnable		-ICE应用状态 true使用，false不使用，默认false
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetICEEnable(bool bEnable)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetICEEnable(bEnable);
#else
	return -1;
#endif
}
/******************************************************************************
* SetTURNLoginAuth
*描述：设置登录TURN服务器账户
*输入：	cszTurnUser		-TURN服务器账户名
cszTurnPWD		-TURN服务器账户密码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetTURNLoginAuth(const char*cszTurnUser,const char*cszTurnPWD)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetTURNLoginAuth(cszTurnUser,cszTurnPWD);
#else
	return -1;
#endif
}
/******************************************************************************
* SetTURNServerIPPort
*描述：设置TURN服务器地址,也可设为域名地址
*输入：	cszTurnIPPort		-TURN服务器地址 形如IP:Port
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetTURNServerIPPort(const char*cszTurnIPPort)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetTURNServerIPPort(cszTurnIPPort);
#else
	return -1;
#endif
}
/******************************************************************************
* SetSTUNServerIPPort
*描述：设置STUN服务器地址,也可设为域名地址
*输入：	cszStunIPPort		-STUN服务器地址 形如IP:Port
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetSTUNServerIPPort(const char*cszStunIPPort)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSTUNServerIPPort(cszStunIPPort);
#else
	return -1;
#endif
}
/******************************************************************************
* SetTurnTransportType
*描述：设置TURN传输通道类型
*输入：	nType		-TURN传输通道类型 17 udp, 6 tcp, 255 tls, 具体可参看pj_turn_tp_type
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetTurnTransportType(int nType)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetTurnTransportType(nType);
#else
	return -1;
#endif
}
/******************************************************************************
* SetDNSServerIP
*描述：设置DNS服务器地址
*输入：	cszDNSIP		-设置DNS服务器地址 形如IP
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetDNSServerIP(const char*cszDNSIP)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDNSServerIP(cszDNSIP);
#else
	return -1;
#endif
}
/******************************************************************************
* SetDNSServerIP2
*描述：设置DNS服务器地址2
*输入：	cszDNSIP2		-设置DNS服务器地址 形如IP
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetDNSServerIP2(const char*cszDNSIP2)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDNSServerIP2(cszDNSIP2);
#else
	return -1;
#endif
}
