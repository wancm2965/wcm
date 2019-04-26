#pragma once
#include "./deviceclient.h"
#include "../include/DeviceClient.h"

class CDeviceClientSession
	: public CDeviceClientNotify
	, public IDeviceClient
{
public:
	CDeviceClientSession(CDeviceClientSessionEvent& rNotify);
	~CDeviceClientSession(void);

private:
	virtual void OnLogin(void);
	virtual void OnLoginError(int nErrCode, CString strError);
	CDeviceClientSessionEvent& m_rNotify;
	CDeviceClient* m_pDeviceClient;

public:
	//创建、初始化
	virtual void CreateDeviceClientSession(void);
	virtual void DestroyDeviceClientSession(void);

	/* 登录
	strUserID		设备ID
	strPassword		设备密码
	strDevType		设备类型（HPD_DVR、HIK_DVR等）
	strClientType	客户端类型"SYS" "MON" "OCX" "MOB"
	strDevName		设备名称
	strSerialno		设备唯一标示（MAC地址等）
	usTermType		终端类型:0-软件终端 1-硬件终端 2-移动终端
	strTermName		终端名称:可以为空 */
	virtual void DeviceClientLogin(const std::string& strUserID, const std::string& strPassword, const std::string &strDevType, const std::string& strClientType,
		const std::string& strDevName, const std::string& strSerialno, unsigned short usTermType=0, const std::string& strTermName="");

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
	virtual void DeviceClientChannelOnline(int nChannelIndex, const std::string &strChannelName, const std::string &mcuid, const std::string &mcuaddr, unsigned short mcuport, unsigned long ulAudioChannel, 
		unsigned long ulVideoChannel, unsigned long ulVideoChannelSub=0, const std::string &strDomain="");
};


