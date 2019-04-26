#ifndef _WBLOGINDLG_H_
#define _WBLOGINDLG_H_

#include "stdafx.h"
#include "IPlugin/IU7_IM/U7_IMSDefine.h"
#include "win_impl_base.hpp"
#include "LoginDefault.h"

class CWBLoginDlg:
	public WindowImplBase
	,public IPlugin_U7_IM_Login_Observer //登录消息监视
{
public:
	CWBLoginDlg(void);
	~CWBLoginDlg(void);

public:
	//继承IPlugin_U7_IM_Login_Observer
	virtual void OnConnectStatus(unsigned long ulStatus, int nErrCode);
	virtual void OnServerListItem(const char* strMCUID,const char* strMCUType,
		unsigned long ulConnections,const char* strBandWidth,unsigned long ulActivity,
		const char* strIPAddr,unsigned short usIPPort,const char* strIPName){};
	virtual void OnServerListEnd(void){};
	// nErrCode:错误代码，为0则登录成功，不为0则登录失败，当为ERR_IMS_CLIENTAUTH(20013)时，表示
	// 域认证失败，此时hr有意义，可解析hr中的含义来显示错误信息，其他情况下hr均为S_OK，后续如对此
	// 逻辑有调整，必须在此处注明
	virtual void OnLogin(int nErrCode , HRESULT hr);
	virtual void OnLogout(int nErrCode);
	
	
	
	
public:
	//继承IPlugin_U7_IM_Room
	
	//继承WindowImplBase
public:

	virtual void Init();  //CDlgLogin类相关数据的初始化
	virtual LPCTSTR GetWindowClassName() const;	//得到窗口的类名
	virtual CString GetSkinFile();//得到登陆窗口的皮肤文件
protected:
	virtual void Notify(TNotifyUI& msg);//登陆窗口的上消息处理函数
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//登陆窗口的上响应按键的消息
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);	 //登陆窗口的上默认事件的消息处理
private:
	CString m_strServer;//用户IP
	CString m_strUserName;//用户名字
	CString m_strPassWord;//用户登录密码

	CRichEditUI* m_pEditServe;//IP编辑框指针
	CRichEditUI* m_pEditUserName;//用户名编辑框指针
	CRichEditUI* m_pEditPassWord;//密码编辑框指针

	CButtonUI*	m_pBtnNetStatus;//联机登录
	CLabelUI *  m_pLblTitle;//提示登录信息的lab

	bool m_bLogout;//是否退出登录了
	bool m_bIsCloseBtn;//关闭按钮
	bool m_bIsLoginCancle;//取消登录

	WBLoginInfo m_LoginInfoArr[SAVEUSERINFOLONG];//存放用户信息
	int m_indexUserInfoPos;//用来统计保存的节位置，是按照SAVEUSERINFOLONG-1个来循环保存
	CString m_strLanguageFile;//得到皮肤xml文件名

public:
	void OnLoginNet(void);//响应联机登录消息
	void OnHideDlg(bool bflag = true);//关闭按钮
	void OnCancleLogin(void);//取消登录

	void SaveLoginInfo();//保存登录信息

	void InitLoginInfo();//初始化登录信息
	CString GetSaveLoginPath();//路径

	BOOL CheckData(void);  //检查数据的输入格式是否正确
	BOOL CheckServerIP(CString IPAddr);//当输入服务器时，输入的是字母，则自动在后台把大写变为小写

	CButtonUI* GetButtonUI();
	CLabelUI *GetLabelUI() {return m_pLblTitle;}
	void SetLoginCancle(bool bflag) {m_bIsLoginCancle = bflag;}
	void DisConnect();
	void GetSkinPath();
	bool OnEditEvent( void* p );
	
};




#endif