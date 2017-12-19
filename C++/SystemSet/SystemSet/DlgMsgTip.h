#pragma once
#include <CoUI/Utils/WinImplBase.h>

enum MsgTipType{
	type_msgbox_showset = 0, //显示设置提示 (带有确定取消按钮)
	type_msgbox_restart,  //重启系统提示
	type_msgnormal,	 //普通提示 (只有提示信息)
};
#define  USERMSG_CLICK_CANCEL    (COUI_MSG_USER+1019)    //点击取消
#define  USETMSG_CLICK_OK		   (COUI_MSG_USER+1020)    //点击结束按钮
#define  USETMSG_TESTWNDSHOW			(COUI_MSG_USER+1022)   //检测系统Inter设置窗口是否显示
#define  USETMSG_TESTWNDCLOSE			(COUI_MSG_USER+1023)   //检测系统Inter设置窗口是否关闭
#define CONFIG_NAME  _T("Config.ini")    
#define MODULE_CONFIGRESTART  _T("Config_Restart")
class CDlgMsgTip :
	public WindowImplBaseBase
{
public:
	CDlgMsgTip();
	~CDlgMsgTip(void);
	//设置提示样式
	void SetMsgTipType(const MsgTipType& tipType);
	void SetMsgTipInfo(LPCTSTR strTipInfo, bool bTimer = false);
	void StartTimerTest(UINT uID, UINT uTime);
	void StopTimerTest(UINT uID);
	int GetPrivateProfile();
	void SetPrivateProfile(int nReStart);
protected:
	//继承自WindowImpBaseBase接口
	void Init();
	void Notify(TNotifyUI& msg);
	CStdString GetSkinFolder();
	CStdString GetSkinFile();
	LPCTSTR GetWindowClassName(void) const;
	EDialogResType GetResType() const;
	virtual unsigned int GetResourceID() const;
	HINSTANCE GetResouceInstance();
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

private:
	CLabelUI* m_plbTipInfo;
	MsgTipType m_uTipType;
	CString   m_pInfo;
	int		  m_nCount;
	//int		  m_nHotKeyId;
};
