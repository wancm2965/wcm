#pragma once


//========================================================================
//类名：网络连接属性窗口资源类
//功能：显示视频、子流、音频的连接方式、IP地址和通道ID
//作者：牛兴盛
//时间：2008-10-31
//说明：通过设置回调函数地址实现参数回调
//--------------------------------------------------------------------------

class AFX_EXT_CLASS HPDLG_ConnectPropertiesNotify
{
public:
	HPDLG_ConnectPropertiesNotify(){};
	~HPDLG_ConnectPropertiesNotify(){};
public:
	/*
	函数功能：窗口关闭回调出来
	参    数：无
	返 回 值：无
	*/
	virtual void IsShowVideoInfoNotify(bool bShowWindow)=0;
};

class AFX_EXT_CLASS HPDLG_ConnectProperties
	: public HPDLG_Base
{	
public:
	HPDLG_ConnectProperties(void);
	virtual~ HPDLG_ConnectProperties(void);

	bool Create(HPDLG_ConnectPropertiesNotify& rNotify, CWnd* pParent);
	void ShowWindow(int nCmdShow);
	void ShowWindow();

	void Destroy();/* 强制关闭对话框*/
public:
	/*HPDLG_SetType函数
	功能：显示视频：连接方式，IP地址，通道ID;
	参数：VideoType 视频连接方式；
	VideoIp IP地址；
	VideoId 通道ID 
	以下类同
	*/
	void SetConProperties(std::string& mcuid, std::string& mcuaddr, unsigned long VideoId, unsigned long AudioId, CString ChannelName, int videotype, int audiotype);

	//  [4/15/2009 licong]
	/*
	函数功能：修改父窗体
	参    数：pParent	父窗体指针
	返 回 值：无
	*/
	void ChangeParent(CWnd* pParent);
};
