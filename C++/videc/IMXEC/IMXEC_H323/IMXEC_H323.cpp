#include "stdafx.h"
#include "../DefineHeader/H323DefineHeader.h"

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <AVCONMP/IMXEC_H323.h>
#else
#include <IMXEC/IMXEC_H323.h>
#endif

#include "H323CallMgr.h"
#ifdef HAS_SIP
#include "../IMXEC_PJSIP/SIPCallMgr.h"
#endif

IMXEC_H323::IMXEC_H323(void)
{

}

IMXEC_H323::~IMXEC_H323(void)
{

}

/******************************************************************************
* SetH323ID
*描述：设置注册帐号
*输入：	cszH323ID		-注册帐号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetH323ID(const char*cszH323ID)
{
	return H323CallMgr::Instance().SetH323ID(cszH323ID);
}

/******************************************************************************
* SetTelNumber
*描述：设置电话号码
*输入：	cszTelNumber		-电话号码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetTelNumber(const char*cszTelNumber)
{
	return H323CallMgr::Instance().SetTelNumber(cszTelNumber);
}

/******************************************************************************
* SetRegPassword
*描述：设置注册密码
*输入：	cszRegPassword	-注册帐号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetRegPassword(const char*cszRegPassword)
{
	return H323CallMgr::Instance().SetRegPassword(cszRegPassword);
}

/******************************************************************************
* SetUserName
*描述：设置用户名称
*输入：	cszUserName		-用户名称
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetUserName(const char*cszUserName)
{
	return H323CallMgr::Instance().SetUserName(cszUserName);
}

/******************************************************************************
* SetRegHost
*描述：设置用户名称
*输入：	cszHost			-注册服务器
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetRegHost(const char*cszHost)
{
	return H323CallMgr::Instance().SetRegHost(cszHost);
}

/******************************************************************************
* Register
*描述：注册
*输入：nType			-注册类型【0:自动查找GK服务器，非0:注册到指定的GK服务器】
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::Register(int nType)
{
	return H323CallMgr::Instance().Register(nType);
}

/******************************************************************************
* UnRegister
*描述：注销
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::UnRegister(void)
{
	return H323CallMgr::Instance().UnRegister();
}

/******************************************************************************
* GetRegState
*描述：获取注册状态
*输入：无
*输出：	nRegState			-注册状态
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::GetRegState(REG_STATE&nRegState)
{
	return H323CallMgr::Instance().GetRegState(nRegState);
}

/******************************************************************************
* SetH323ListenPort
*描述：设置侦听端口
*输入：	nH323ListenPort			-侦听端口
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetH323ListenPort(unsigned short nH323ListenPort)
{
	return H323CallMgr::Instance().SetH323ListenPort(nH323ListenPort);
}

/******************************************************************************
* SetLocalIP
*描述：设置注册帐号
*输入：	cszH323ID		-注册帐号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetLocalIP(const char*cszLocalIP)
{
	return H323CallMgr::Instance().SetLocalIP(cszLocalIP);
}

/******************************************************************************
* SetRTPBasePort
*描述：设置侦听端口
*输入：	nRTPBasePort			-RTP开始端口
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetRTPBasePort(unsigned short nRTPBasePort)
{
	//H323CallMgr::Instance().SetMediaPortArrayEmpty();
	//SIPCallMgr::Instance().SetMediaPortArrayEmpty();
	//CallMgr::Instance().SetRTPBasePort(nRTPBasePort); //注意调此需同时清端口回收池
	return H323CallMgr::Instance().SetRTPBasePort(nRTPBasePort);
}


/******************************************************************************
* SetAudioDevParam
*描述：设置主流视频设备参数
*输入： nAudioCodecID		-音频编码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
	return H323CallMgr::Instance().SetAudioDevParam(nAudioCodecID);
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
int IMXEC_H323::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
	return H323CallMgr::Instance().SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
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
int IMXEC_H323::SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
	return H323CallMgr::Instance().SetSecondVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
}


/******************************************************************************
* Start
*描述：启动H323服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::Start(void)
{
	return H323CallMgr::Instance().Start();
}

/******************************************************************************
* Stop
*描述：停止H323服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::Stop(void)
{
	H323CallMgr::Instance().Stop();

	return 0;
}

/******************************************************************************
* SetSupportH239
*描述：设置是否支持H239
*输入：nSupportH239		-是否支持H239
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetSupportH239(int nSupportH239)
{
	return H323CallMgr::Instance().SetSupportH239(nSupportH239);
}




//use H235 Media

/******************************************************************************
* H235SecuritySettingInitialize
*描述：	H235安全选项初始化
*输入：	
*****************************************************************************/
void IMXEC_H323::H235SecuritySettingInitialize()
{
	H323CallMgr::Instance().H235SecuritySettingInitialize();
}
/******************************************************************************
* SetH235SecurityAuthOption
*描述：	设置使用H235验证选项
*输入：	option 0为不使用，1为使用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAuthOption(int option)
{
	H323CallMgr::Instance().SetH235SecurityAuthOption(option);
}

/******************************************************************************
* SetH235MediaSecurityOption
*描述：	设置使用媒体流加密选项
*输入：	option 0为不使用，1为可选使用，2为强制使用H235媒体流加密
*****************************************************************************/
void IMXEC_H323::SetH235MediaSecurityOption(int option)
{
	H323CallMgr::Instance().SetH235MediaSecurityOption(option);
}


/******************************************************************************
* SetH235SecurityAuth
*描述：	设置H235验证信息 本端用senderId来检查对方发来的generalID 本端用generalID来检查对方发来的senderId
*输入：	senderId 发送标识即本端标识 
generalId接收标识即对端标识 
password 验证密码
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAuth(char * senderId,char * generalId,char * password)
{
	H323CallMgr::Instance().SetH235SecurityAuth(senderId,generalId,password);
}


/******************************************************************************
* SetH235SecurityAnnexIEnable
*描述：	设置安全验证模式可用状态，H235 AnnexD I规程消息完整性及密码认证
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAnnexIEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityAnnexIEnable(enable);
}

/******************************************************************************
* SetH235SecurityAnnexIAEnable
*描述：	设置安全验证模式可用状态，H235 AnnexD IA 规程仅密码认证
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAnnexIAEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityAnnexIAEnable(enable);
}

/******************************************************************************
* SetH235SecurityAnnexIIEnable
*描述：	设置安全验证模式可用状态，H235 AnnexE II 规程采用公钥/私钥对的数字签名
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAnnexIIEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityAnnexIIEnable(enable);
}

/******************************************************************************
* SetH235SecurityAnnexIIIEnable
*描述：	设置安全验证模式可用状态，H235 AnnexE III 规程端到端认证
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAnnexIIIEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityAnnexIIIEnable(enable);
}



/******************************************************************************
* SetH235SecurityAnnexIVEnable
*描述：	设置安全验证模式可用状态，H235 AnnexF IV规程逐段转接的安全性 结合了规程I和规程II
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/

//void IMXEC_H323::SetH235SecurityAnnexIVEnable(bool enable)
//{
//	H323CallMgr::Instance().SetH235SecurityAnnexIVEnable(enable);
//}


/******************************************************************************
* SetH235SecurityMediaDH512Enable
*描述：	设置安全验证模式可用状态，用于媒体流加密，使用512-bit Diffie-Hellman group生成7字节共享密钥
仅用于DES加密等使用7字节以内的密码加密的算法
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityMediaDH512Enable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityMediaDH512Enable(enable);
}

/******************************************************************************
* SetH235SecurityMediaDH1024Enable
*描述：	设置安全验证模式可用状态，用于媒体流加密，使用1024-bit Diffie-Hellman group生成21字节共享密钥
用于DES 3DES加密等
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityMediaDH1024Enable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityMediaDH1024Enable(enable);
}


/******************************************************************************
* SetH235SecurityMediaDH1536Enable
*描述：	设置安全验证模式可用状态，用于媒体流加密，使用1536-bit Diffie-Hellman group生成21字节共享密钥
用于DES 3DES加密等
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityMediaDH1536Enable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityMediaDH1536Enable(enable);
}


/******************************************************************************
* SetH235SecurityUserPrimeEnable
*描述：	设置可用状态，预定义素数是否用于媒体加密共享密钥协商
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityUserPrimeEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityUserPrimeEnable(enable);
}


/******************************************************************************
* SetH235SecurityCheckIncomingMsgEnable
*描述：	设置对接收呼叫消息进行验证检查状态
*输入：	enable 可用状态 true 设为可用 false不可用
*****************************************************************************/
void IMXEC_H323::SetH235SecurityCheckIncomingMsgEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityCheckIncomingMsgEnable(enable);
}


/******************************************************************************
* SetH235MediaEncryptionAlogrithmFirst
*描述：	设置媒体流优先加密算法
*输入：	alogrimId 算法ID,alogrimId 算法ID,des is set 2, aes-128 is set 7 ,aes-256 is set 9 ,3DES is set 5
*****************************************************************************/
void IMXEC_H323::SetH235MediaEncryptionAlogrithmFirst(int alogrimId)
{
	H323CallMgr::Instance().SetH235MediaEncryptionAlogrithmFirst(alogrimId);
}


/******************************************************************************
* SetH235MediaSecurityEncryptionPWD
*描述：	设置媒体流加密密码
*输入：	type 0 为不使用指定密码(自动协商) 1 为使用指定密码
		encrypPwd 使用指定密码时设置的加密密码
*****************************************************************************/
void IMXEC_H323::SetH235MediaSecurityEncryptionPWD(int type,char * encrypPwd)
{
	H323CallMgr::Instance().SetH235MediaSecurityEncryptionPWD(type,encrypPwd);
}

/******************************************************************************
* SendCallH245ControlMessage
*描述：发送H245控制消息 包含flowControlIndication H239令牌相关 会控消息
*输入：	cszCallID			-呼叫标识
nMsgType			-控制消息类型
cszPacket			-消息包
nPacketLen			-消息包长度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SendCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	return H323CallMgr::Instance().DAPSendCallH245ControlMessage(cszCallID,nMsgType, cszPacket,nPacketLen);
}

int IMXEC_H323::SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl)
{
	return H323CallMgr::Instance().SetCallRTPMediaMulticast(cszCallID,cszMulticastIP,ttl);
}

void IMXEC_H323::SetH460NATFwOption(int option)
{
	H323CallMgr::Instance().SetH460NATFwOption(option);
}

//void SetNETECSendEnable(const char*cszH323ID,int nOptions)

