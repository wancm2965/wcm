#pragma once

class INotifyMsg
{
public:

	//----------------------------------
	//函数功能：设置弹出消息路径
	//----------------------------------
	virtual	void	SetSkinFilePath(CString strSkinFilePath)=0;

	virtual	void	NotifyMsg(CString sMsg)=0;		//弹出相关的信息
	//----------------------------------
	//函数功能：销毁对应的对话框
	//----------------------------------
	virtual void Destory()=0;

};

//----------------------------------
//函数功能：创建弹出消息类
//参数说明：pParentWnd:父窗口指针 pNotify:回调接口指针
//返回值  ：控制云台接口指针
//----------------------------------

AFX_EXT_CLASS  INotifyMsg* CreateNotifyMsg(CWnd* pParentWnd);