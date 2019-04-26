#ifndef __HPDVRDLL_H_
#define __HPDVRDLL_H_

class IHPDVRDLLNotify
{
public:
	IHPDVRDLLNotify(){}
	virtual ~IHPDVRDLLNotify(){}
public:
	virtual void OnIHPDVRDLLNotify_Exit(int nType=0) = 0;/*0退出，1重启，2进入会议*/
};

class IHPDVRDLL
{
public:
	IHPDVRDLL() {};
	virtual ~IHPDVRDLL() {};

public:
	virtual bool Connect(HWND hParent, int nType) = 0;
	virtual void Release(void) = 0;
	virtual HWND GetAppHwnd(void) = 0; 
	virtual HWND GetGuiHwnd(void) = 0; 
	virtual bool IsVisibled(void) = 0;
	virtual bool ShowWindow(bool bShow) = 0; 
	virtual void ShowConfig(int type) = 0;/*1、DVR用户管理 2、录像设置 3、录像回放 4、系统日志*/
	virtual HWND GetMsgHwnd(void) =0; //获取文字消息窗体句柄
	virtual HWND GetMsgComboxHwnd(void)=0; //获取文字消息窗体的Combox控件句柄

};

AFX_EXT_CLASS IHPDVRDLL* CreateDVRDevice(IHPDVRDLLNotify& notify);
AFX_EXT_CLASS void InitDVRDevice(unsigned long ulVIDECFlag=0);
AFX_EXT_CLASS void UnInitDVRDevice(void);

#endif