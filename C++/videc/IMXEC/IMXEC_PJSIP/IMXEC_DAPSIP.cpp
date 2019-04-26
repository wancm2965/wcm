#include "stdafx.h"

#include "../DefineHeader/SIPDefineHeader.h"

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1)

#include <AVCONMP/IMXEC_SIP.h>
#else
#include <IMXEC/IMXEC_DAPSIP.h>
#endif


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
* SetAgentName
*描述：设置应用实体名 userAgent
*输入：	cszAgent		-应用实体名
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetAgentName(const char*cszAgent)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetAgentName(cszAgent);
#else
	return -1;
#endif
}

/******************************************************************************
* SetCapPrintEnable
*描述：设置打印本端及远端能力集和通道能力集信令消息内容,默认不打印
*输入：	nPrint		-不打印设为0,打印设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetCapPrintEnable(int nPrint)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDAPCapPrintEnable(nPrint);
#else
	return -1;
#endif
}

int IMXEC_SIP::SetRegisterTimer(int regTimer,int regFailTimer)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetRegisterTimer(regTimer,regFailTimer);
#else
	return -1;
#endif
}

int IMXEC_SIP::SetSIPLogFile(int logLevel,const char*cszLogFileName)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSIPLogFile(logLevel,cszLogFileName);
#else
	return -1;
#endif
}



/******************************************************************************
* SetLocalMainVideoParam
*描述：设置主流视频帧率码率参数
*输入： cszCallID			-呼叫标识
nFrameRate			-最大帧率
nBitrate			-最大码率
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDAPLocalMainVideoParam(cszCallID,nFrameRate,nBitrate);
#else
	return -1;
#endif
}

/******************************************************************************
* SetLocalSecondVideoParam
*描述：设置辅流视频帧率码率参数
*输入： cszCallID			-呼叫标识
nFrameRate			-最大帧率
nBitrate			-最大码率
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDAPLocalSecondVideoParam(cszCallID,nFrameRate,nBitrate);
#else
	return -1;
#endif
}

/******************************************************************************
* SetSupportH239
*描述：设置是否支持H239
*输入：cszCallID -呼叫标识 nSupportH239		-是否支持H239
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetSupportH239(const char*cszCallID,int nSupportH239)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSIPCallSupportH239(cszCallID,nSupportH239);
#else
	return -1;
#endif
}

/******************************************************************************
* SetLocalMainVideoH264Level
*描述：设置本地主流视频H264能力使用的最大Level值 如有设置在呼叫加入能力集时会优先使用此Level值
*输入：	cszCallID		-呼叫标识
nLevel			-本地主视频Level值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetLocalMainVideoH264Level(const char*cszCallID,int nLevel)
{
	return SIPCallMgr::Instance().SetLocalMainVideoH264Level(cszCallID,nLevel);
}


/******************************************************************************
* SetLocalSecondVideoH264Level
*描述：设置本地辅流视频H264能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
*输入：	cszCallID		-呼叫标识
nLevel			-本地辅流视频Level值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetLocalSecondVideoH264Level(const char*cszCallID,int nLevel)
{
	return SIPCallMgr::Instance().SetLocalSecondVideoH264Level(cszCallID,nLevel);
}


/******************************************************************************
* SetLocalMainVideoH264HPLevel
*描述：设置本地主流视频H264HighProfile能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
*输入：	cszCallID		-呼叫标识
nLevel			-本地主视频Level值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetLocalMainVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	return SIPCallMgr::Instance().SetLocalMainVideoH264HPLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetLocalSecondVideoH264HPLevel
*描述：设置本地辅流视频H264HighProfile能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
*输入：	cszCallID		-呼叫标识
nLevel			-本地辅流视频Level值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetLocalSecondVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	return SIPCallMgr::Instance().SetLocalSecondVideoH264HPLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetCallRTPMediaMulticast 
*描述：设置RTP 媒体流组播 如有设置组播IP则会启用组播
*输入：	cszCallID		-呼叫标识
cszMulticastIP	-组播IP
ttl				-组播数据包的TTL(生存时间)
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_SIP::SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl)
{
	return SIPCallMgr::Instance().SetCallRTPMediaMulticast(cszCallID,cszMulticastIP,ttl);
}

/******************************************************************************
*描述：发送辅流控制令牌消息
*输入：	cszCallID		-呼叫标识
nType			-辅流控制令牌消息类型 1 请求 2 响应 3 指示 4 取消
transactionId	-事务ID  
userId			-用户ID  
floorId			-事务子类型ID 
status			-状态 如发送响应时 0表示拒绝 1表示允许
*****************************************************************************/
int IMXEC_SIP::SendCallDualVideoControl(const char*cszCallID,int nType,int transactionId,int userId,int floorId,int status)
{
	return SIPCallMgr::Instance().SendCallDualVideoControl(cszCallID,nType,transactionId,userId,floorId,status);
}

/******************************************************************************
*描述：向对端发送自定义非标消息
*输入：	cszCallID		-呼叫标识
		cszContent		-自定义非标准信息内容
		cszMimeType		-自定义非标信息内容类型 如application/xml
*****************************************************************************/

int IMXEC_SIP::SendCallCustomMsg(const char*cszCallID,const char * cszContent,const char * cszMimeType)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().DAPSendCallCustomMsg(cszCallID,cszContent,cszMimeType);
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
