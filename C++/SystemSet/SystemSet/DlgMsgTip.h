#pragma once
#include <CoUI/Utils/WinImplBase.h>

enum MsgTipType{
	type_msgbox_showset = 0, //��ʾ������ʾ (����ȷ��ȡ����ť)
	type_msgbox_restart,  //����ϵͳ��ʾ
	type_msgnormal,	 //��ͨ��ʾ (ֻ����ʾ��Ϣ)
};
#define  USERMSG_CLICK_CANCEL    (COUI_MSG_USER+1019)    //���ȡ��
#define  USETMSG_CLICK_OK		   (COUI_MSG_USER+1020)    //���������ť
#define  USETMSG_TESTWNDSHOW			(COUI_MSG_USER+1022)   //���ϵͳInter���ô����Ƿ���ʾ
#define  USETMSG_TESTWNDCLOSE			(COUI_MSG_USER+1023)   //���ϵͳInter���ô����Ƿ�ر�
#define CONFIG_NAME  _T("Config.ini")    
#define MODULE_CONFIGRESTART  _T("Config_Restart")
class CDlgMsgTip :
	public WindowImplBaseBase
{
public:
	CDlgMsgTip();
	~CDlgMsgTip(void);
	//������ʾ��ʽ
	void SetMsgTipType(const MsgTipType& tipType);
	void SetMsgTipInfo(LPCTSTR strTipInfo, bool bTimer = false);
	void StartTimerTest(UINT uID, UINT uTime);
	void StopTimerTest(UINT uID);
	int GetPrivateProfile();
	void SetPrivateProfile(int nReStart);
protected:
	//�̳���WindowImpBaseBase�ӿ�
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
