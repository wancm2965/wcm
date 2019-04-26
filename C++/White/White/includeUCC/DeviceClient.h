#pragma once

class IDeviceClientNotify
{
public:
	IDeviceClientNotify(void){};
	~IDeviceClientNotify(void){};

	/*
	登录失败回调
	nErrCode:错误代码
	strError:错误内容
	*/
	virtual void OnDeviceClientLogin(int nErrCode, CString strError)=0;

	/*
	设备上线成功回调
	在此进行通道发布
	allow_count:允许发布的通道个数
	*/
	virtual void OnDeviceOnline(int allow_count)=0;

	/*
	设备下线通知
	*/
	virtual void OnDeviceOffline(void)=0;

	/*
	通道上线成功回调
	*/
	virtual void OnChannelOnline(void)=0;

	/*
	通道下线通知
	*/
	virtual void OnChannelOffline(void)=0;

	virtual void OnUDPRcvAlert(const UDP_RCV_ALART_INFO& info)=0;
	virtual void OnUDPRcvGHJInfo(const UDP_RCV_GHJ_INFO& info)=0;
	virtual void OnP2P(const std::string& strUserID,KCmdUserDataPacket& packet)=0;
	virtual void OnYTControl(KCmdPacket& rCmdPacket)=0;
};

class AFX_EXT_CLASS IDeviceClient
{
public:
	/* 登录
	strUserID		设备ID
	strPassword		设备密码
	strClientType	客户端类型"SYS" "MON" "OCX" "MOB"
	strDevName		设备名称
	strSerialno		设备唯一标示（MAC地址等）
	usTermType		终端类型:0-软件终端 1-硬件终端 2-移动终端
	strTermName		终端名称:可以为空 */
	virtual void DeviceClientLogin(const std::string& strUserID, const std::string& strPassword, MON_DEVTYPE eDevType, const std::string& strDevName, 
		const std::string& strSerialno, unsigned short usTermType=0, const std::string& strTermName="")=0;

	/*登出*/
	virtual void DeviceClientLogout(void)=0;

	/* 发送设备上线信息
	strDevType		设备类型（HPD_DVR、HIK_DVR等）
	strDevID		设备ID
	strPwd			密码
	strName			设备名称
	strMask			唯一标示（传空为本机MAC地址）
	strVersion		设备版本信息（无特殊情况可传空，内部处理）
	格式：<version><system>系统版本</system><software>软件版本号<software></version>
	strDomain		设备域（实现方式修改，暂时可以为空）
	*/
	virtual void DeviceOnline(MON_DEVTYPE eDevType, const std::string &strDevID, const std::string &strPwd,const std::string &strName,  const std::string &strMask="", 
		const std::string &strVersion="", const std::string &strDomain="")=0;

	/* 发送设备下线信息
	strDevID		设备ID
	strDomain		设备域（实现方式修改，暂时可以为空）
	*/
	virtual void DeviceOffline(const std::string &strDevID, const std::string &strDomain="")=0;

	/* 发送通道上线信息
	nChannelIndex		通道号
	strChannelName		通道名称
	mcuid				MCU-ID
	mcuaddr				MCU地址
	mcuport				MCU端口
	ulAudioChannel		音频号
	ulVideoChannel		视频号
	ulVideoChannelSub	视频子流号（可以为空）
	strDomain			设备域（实现方式修改，暂时可以为空）
	*/
	virtual void ChannelOnline(int nChannelIndex, const std::string &strChannelName, const std::string &mcuid, const std::string &mcuaddr, unsigned short mcuport, unsigned long ulAudioChannel, 
		unsigned long ulVideoChannel, unsigned long ulVideoChannelSub=0, const std::string &strDomain="", const std::string &strDevID="",const std::string& strDevName="")=0;

	/* 发送DVR通道注销请求
	strDevID		设备ID
	nCHNum=-1		所有通道注销
	strDomain		设备域（实现方式修改，暂时可以为空）
	*/
	virtual void ChannelOffline(const std::string &strDevID,unsigned short nCHNum,const std::string &strDomain="")=0;

	/* 修改设备名称
	strDevID		设备ID
	strName			设备名称
	strDomain		设备域（实现方式修改，暂时可以为空）
	*/
	virtual void UpdateDeviceName(const std::string &strDevID,const std::string &strName, const std::string &strDomain="")=0;
};

