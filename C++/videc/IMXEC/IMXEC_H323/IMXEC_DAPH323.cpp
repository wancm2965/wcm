#include "stdafx.h"

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <AVCONMP/IMXEC_H323.h>
#else
#include <IMXEC/IMXEC_DAPH323.h>
#endif

#include "H323CallMgr.h"
#include "CallMgr.h"

IMXEC_H323::IMXEC_H323(void)
{

}

IMXEC_H323::~IMXEC_H323(void)
{

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
	return H323CallMgr::Instance().DAPStart();
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
	return H323CallMgr::Instance().DAPStop();
}

/******************************************************************************
* SetTypeRegisterToGK
*描述：设定注册到GK类型
*输入：nType	-注册类型 自动查找GK服务器设为0,注册到指定的GK服务器设为非0值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
//int IMXEC_H323::SetTypeRegisterToGK(int nType)
//{
//	return H323CallMgr::Instance().SetTypeDAPRegisterToGK(nType);
//}

/******************************************************************************
* SetTypeDAPAutoManualRegGK
*描述：设置自动注册还是手动注册 
*输入：nType	-注册类型 0自动注册,1手动注册
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
//int IMXEC_H323::SetTypeDAPAutoManualRegGK(int nType)
//{
//	return H323CallMgr::Instance().SetTypeDAPAutoManualRegGK(nType);
//}

/******************************************************************************
* RegisterToGK
*描述：注册到GK 注册帐号和电话号码至少设置一个 设定自动查找GK时cszHost可设为""或NULL
*输入：	cszH323ID			-注册帐号
cszTelNumber		-电话号码
cszHost				-注册的GK服务器地址 
cszRegPassword		-注册账号密码
*输出：无
*返回值：成功返回>=0[account id]，失败返回-1
*****************************************************************************/
int IMXEC_H323::RegisterToGK(const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword)
{
	return H323CallMgr::Instance().DAPRegisterToGK(cszH323ID,cszTelNumber,cszHost,cszRegPassword);
}

/******************************************************************************
* UnRegisterToGK
*描述：从GK上注销账号
*输入：	nAccountID			-注册帐号ID
*返回值：调用成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::UnRegisterToGK(int nAccountID)
{
	return H323CallMgr::Instance().UnDAPRegisterToGK(nAccountID);
}

/******************************************************************************
* SetUserName
*描述：设置用户显示名称
*输入：	cszUserName		-用户显示名称
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetUserName(const char*cszUserName)
{
	return H323CallMgr::Instance().SetUserName(cszUserName);
}

/******************************************************************************
* SetUserID
*描述：设置用户名
*输入：	cszUserID		-用户名
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetUserID(const char*cszUserID)
{
	return H323CallMgr::Instance().SetH323ID(cszUserID);
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
*描述：设置本机IP
*输入：	cszLocalIP		-本机IP
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetLocalIP(const char*cszLocalIP)
{
	return H323CallMgr::Instance().SetLocalIP(cszLocalIP);
}

/******************************************************************************
* SetExternalIP
*描述：设置本机所在的网关映射IP
*输入：	cszExternalIP		-本机网关映射IP
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetExternalIP(const char*cszExternalIP)
{
	//return H323CallMgr::Instance().SetExternalIP(cszExternalIP);
	return 0;
}

/******************************************************************************
* SetH245RangePort
*描述：设置H245连接端口范围
*输入：	nFromPort			-H245连接起始端口
nToPort				-H245连接最终端口
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetH245RangePort(unsigned short nFromPort,unsigned short nToPort)
{
	return H323CallMgr::Instance().SetDAPH245RangePort(nFromPort,nToPort);
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
	return H323CallMgr::Instance().SetRTPBasePort(nRTPBasePort);
}


/******************************************************************************
* SetProductID
*描述：设置产品ID
*输入：	cszProductID		-产品ID
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetProductID(const char*cszProductID)
{
	return H323CallMgr::Instance().SetDAPProductID(cszProductID);
}


/******************************************************************************
* SetProductVersionID
*描述：设置产品版本号
*输入：	cszVersionID		-版本号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetProductVersionID(const char*cszVersionID)
{
	return H323CallMgr::Instance().SetDAPProductVersionID(cszVersionID);
}


/******************************************************************************
* SetT35CountryCode
*描述：设置产品T35CountryCode 生产商所在区域代码
*输入：	nT35Code		-T35CountryCode
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetT35CountryCode(int nT35Code)
{
	return H323CallMgr::Instance().SetDAPT35CountryCode(nT35Code);
}


/******************************************************************************
* SetT35Extension
*描述：设置产品T35Extension 生产商所在区域代码扩展
*输入：	nT35Extension		-T35Extension
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetT35Extension(int nT35Extension)
{
	return H323CallMgr::Instance().SetDAPT35Extension(nT35Extension);
}

/******************************************************************************
* SetManufacturerCode
*描述：设置产品ManufacturerCode 生产商代码信息
*输入：	nManufacturerCode		-ManufacturerCode
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetManufacturerCode(int nManufacturerCode)
{
	return H323CallMgr::Instance().SetDAPManufacturerCode(nManufacturerCode);
}

/******************************************************************************
* SetH323EndPointType
*描述：设定终端类型值
*输入：	nType		-普通H323终端设为0,H323MCU设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetH323EndPointType(int nType)
{
	return H323CallMgr::Instance().SetDAPH323EndPointType(nType);
}

/******************************************************************************
* SetCapPrintEnable
*描述：设置打印本端及远端能力集和通道能力集信令消息内容,默认不打印
*输入：	nPrint		-不打印设为0,打印设置为1
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetCapPrintEnable(int nPrint)
{
	return H323CallMgr::Instance().SetDAPCapPrintEnable(nPrint);
}

/******************************************************************************
* SetMasterSlaveValue
*描述：设定本机的主从确定值 一般MCU设置为240或190 ,普通终端设置为50
*输入：	nValue		-本机的主从确定值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetMasterSlaveValue(int nValue)
{
	return H323CallMgr::Instance().SetDAPMasterSlaveValue(nValue);
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
int IMXEC_H323::SetLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
	return H323CallMgr::Instance().SetDAPLocalMainVideoParam(cszCallID,nFrameRate,nBitrate);
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
int IMXEC_H323::SetLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
	return H323CallMgr::Instance().SetDAPLocalSecondVideoParam(cszCallID,nFrameRate,nBitrate);
}

/******************************************************************************
* SetSupportH239
*描述：设置是否支持H239
*输入：cszCallID -呼叫标识 nSupportH239		-是否支持H239
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetSupportH239(const char*cszCallID,int nSupportH239)
{
	return H323CallMgr::Instance().SetH323CallSupportH239(cszCallID,nSupportH239);
}

/******************************************************************************
* SetLocalMainVideoH264Level
*描述：设置本地主流视频H264能力使用的最大Level值 如有设置在呼叫加入能力集时会优先使用此Level值
*输入：	cszCallID		-呼叫标识
nLevel			-本地主视频Level值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetLocalMainVideoH264Level(const char*cszCallID,int nLevel)
{
	return H323CallMgr::Instance().SetLocalMainVideoH264Level(cszCallID,nLevel);
}


/******************************************************************************
* SetLocalSecondVideoH264Level
*描述：设置本地辅流视频H264能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
*输入：	cszCallID		-呼叫标识
nLevel			-本地辅流视频Level值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetLocalSecondVideoH264Level(const char*cszCallID,int nLevel)
{
	return H323CallMgr::Instance().SetLocalSecondVideoH264Level(cszCallID,nLevel);
}


/******************************************************************************
* SetLocalMainVideoH264HPLevel
*描述：设置本地主流视频H264HighProfile能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
*输入：	cszCallID		-呼叫标识
nLevel			-本地主视频Level值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetLocalMainVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	return H323CallMgr::Instance().SetLocalMainVideoH264HPLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetLocalSecondVideoH264HPLevel
*描述：设置本地辅流视频H264HighProfile能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
*输入：	cszCallID		-呼叫标识
nLevel			-本地辅流视频Level值
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SetLocalSecondVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	return H323CallMgr::Instance().SetLocalSecondVideoH264HPLevel(cszCallID,nLevel);
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
int IMXEC_H323::SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl)
{
	return H323CallMgr::Instance().SetCallRTPMediaMulticast(cszCallID,cszMulticastIP,ttl);
}

/******************************************************************************
* SendCallNonstandardData
*描述：向对端发送非标消息
*输入：	cszCallID		-呼叫标识
cszPacket		-非标准信息负载
nPacketLen		-非标准信息长度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SendCallNonstandardData(const char*cszCallID,const char * cszPacket,int nPacketLen)
{
	return H323CallMgr::Instance().DAPSendCallH245NonstandardData(cszCallID,cszPacket);
}

/******************************************************************************
* OpenDualVideoOutChannel
*描述：打开辅流发送通道
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::OpenDualVideoOutChannel(const char*cszCallID)
{
	return H323CallMgr::Instance().OpenDualVideoOutChannel(cszCallID);
}


/******************************************************************************
* OpenFECCOutChannel
*描述：打开FECC控制发送通道
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
int IMXEC_H323::OpenFECCOutChannel(const char*cszCallID)
{
	return H323CallMgr::Instance().OpenFECCOutChannel(cszCallID);
}

/******************************************************************************
* CloseDualVideoOutChannel
*描述：关闭辅流发送通道
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::CloseDualVideoOutChannel(const char*cszCallID)
{
	return H323CallMgr::Instance().CloseDualVideoOutChannel(cszCallID);
}

/******************************************************************************
* CloseFECCOutChannel
*描述：关闭FECC控制发送通道
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::CloseFECCOutChannel(const char*cszCallID)
{
	return H323CallMgr::Instance().CloseFECCOutChannel(cszCallID);
}


/******************************************************************************
* AudioRecvChannelAnswer
*描述：接受对端打开的音频发送通道
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::AudioRecvChannelAnswer(const char*cszCallID)
{
	return H323CallMgr::Instance().AudioRecvChannelAnswer(cszCallID);
}


/******************************************************************************
* MainVideoRecvChannelAnswer
*描述：接受对端打开的主流视频发送通道
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::MainVideoRecvChannelAnswer(const char*cszCallID)
{
	return H323CallMgr::Instance().MainVideoRecvChannelAnswer(cszCallID);
}


/******************************************************************************
* DualVideoRecvChannelAnswer
*描述：接受对端打开的辅流视频发送通道
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::DualVideoRecvChannelAnswer(const char*cszCallID)
{
	return H323CallMgr::Instance().DualVideoRecvChannelAnswer(cszCallID);
}


/******************************************************************************
* FECCRecvChannelAnswer
*描述：接受对端打开的FECC控制发送通道
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::FECCRecvChannelAnswer(const char*cszCallID)
{
	return H323CallMgr::Instance().FECCRecvChannelAnswer(cszCallID);
}



/******************************************************************************
* AudioRecvChannelReject
*描述：拒绝接受对端打开的音频发送通道
*输入：	cszCallID			-呼叫标识
nReason				-拒绝原因
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::AudioRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	return H323CallMgr::Instance().AudioRecvChannelReject(cszCallID,nReason);
}

/******************************************************************************
* MainVideoRecvChannelReject
*描述：拒绝接受对端打开的主流视频发送通道
*输入：	cszCallID			-呼叫标识
nReason				-拒绝原因
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::MainVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	return H323CallMgr::Instance().MainVideoRecvChannelReject(cszCallID,nReason);
}


/******************************************************************************
* DualVideoRecvChannelReject
*描述：拒绝接受对端打开的辅流视频发送通道
*输入：	cszCallID			-呼叫标识
nReason				-拒绝原因
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::DualVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	return H323CallMgr::Instance().DualVideoRecvChannelReject(cszCallID,nReason);
}


/******************************************************************************
* FECCRecvChannelReject
*描述：拒绝接受对端打开的FECC控制发送通道
*输入：	cszCallID			-呼叫标识
nReason				-拒绝原因
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::FECCRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	return H323CallMgr::Instance().FECCRecvChannelReject(cszCallID,nReason);
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

/******************************************************************************
* SendRequestMainVideoKeyFrame
*描述：发送主流视频关键帧请求消息
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SendRequestMainVideoKeyFrame(const char*cszCallID)
{
	return H323CallMgr::Instance().SendRequestMainVideoKeyFrame(cszCallID);
}

/******************************************************************************
* SendRequestSecondVideoKeyFrame
*描述：发送辅流视频关键帧请求消息
*输入：	cszCallID			-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SendRequestSecondVideoKeyFrame(const char*cszCallID)
{
	return H323CallMgr::Instance().SendRequestSecondVideoKeyFrame(cszCallID);
}

/******************************************************************************
* SendCallChannelMessage
*描述：发送逻辑通道消息 包含FlowControlCommand videoFastUpdateGOB videoFastUpdateMB
*输入：	cszCallID			-呼叫标识
nMsgType			-控制消息类型
cszPacket			-消息包 含有媒体类型字段 收发方向字段
nPacketLen			-消息包长度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
//int IMXEC_H323::SendCallChannelMessage(const char*cszCallID,H245ChanMessageType nMsgType,const char * cszPacket,int nPacketLen)
//{
//	return H323CallMgr::Instance().SendCallChannelMessage(cszCallID,nMsgType, cszPacket,nPacketLen);
//}

/******************************************************************************
* SendRASMessage
*描述：发送RAS消息 nMsgType 重定义
*输入：	nMsgType			-消息类型
cszPacket			-消息包
nPacketLen			-消息包长度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_H323::SendRASMessage(H225RASMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	return H323CallMgr::Instance().SendRASMessage(nMsgType, cszPacket,nPacketLen);
}


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
*输入：	alogrimId 算法ID 2为DES CBC模式，5为3DES CBC模式
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
