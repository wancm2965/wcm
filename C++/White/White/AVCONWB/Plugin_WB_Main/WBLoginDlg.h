#pragma once
#include "IPlugin\IU7_IM\IPlugin_U7_IM_Login.h"
#include "WBBtn.h"
#include "WBMeetingListDlg.h"
#include "WBAttributeStatic.h"
#include "ErrorCode.h"
#include "windef.h"
#include "afxcmn.h"
#include "afxwin.h"
using namespace AVCONPlugin;
using namespace std;

#define  SAVEUSERINFOLONG   11
#define  SELECTIONNAME   _T("user")
#define  LASTSELECTIONNAME _T("lastUser")
#define  SERVER_KEYNAME   _T("ServerHost")
#define  LOGINID_KEYNAME _T("LoginID")
#define  PASSWORD_KEYNAME _T("Password")
#define  ISHIDELAND_KEYNAME  _T("IsHideLand")
typedef struct UserInfo
{
	CString serverHost;
	CString longinID;
	CString password;
	BOOL isAutoLand;
}LoginUserInfo , *pLoginUserInfo;


// CWBLoginDlg 对话框

	class CWBLoginDlg : public CDialog
		, public IPlugin_U7_IM_Login_Observer//登陆消息监视器类
	{
		DECLARE_DYNAMIC(CWBLoginDlg)

	public:
		CWBLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
		virtual ~CWBLoginDlg();

	// 对话框数据
		enum { IDD = IDD_WBLOGINDLG };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
		Bitmap * m_pBitmap;
		// 生成的消息映射函数
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		virtual void OnOK();
		virtual void OnCancel();
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		DECLARE_MESSAGE_MAP()
	protected:
		HICON               m_hIcon;
		CString			    m_strServer;
		CString			    m_strAccount;
		//CString			    m_strPassword;
		CString			    m_strLoginState;
	private:
		//控件
		//CWBBtn m_onlineBtn;           //联机登录按钮
		//CWBBtn m_singleLandBtn;       //单机启动按钮 
		//CWBBtn m_closeLoginDlgBtn;
		//CWBBtn m_cancelBtn;
		//CEdit m_serverHostEdit;//登陆地址文本框
		//CEdit m_passwordEdit;//登陆密码文本框
		//CComboBoxEx m_longinIDCombox;//登陆用户名组合框列表
		//CWBAttributeStatic m_showLandMessageSTATIC;
		//CString m_strServerHost;      //服务器地址文本框
		//CString m_strLoginID;         //登录ID文本框
		CRect m_rect;//登陆对话框显示区域

		LoginUserInfo m_userInfoArr[SAVEUSERINFOLONG];//存放用户信息，
		int m_indexUserInfoPos;//用来统计保存的节位置，是按照SAVEUSERINFOLONG-1个来循环保存
	public:
		//////////////////////////////////////////////////////////////////////////
		// 继承IPlugin_U7_IM_Login_Observer接口
		virtual void OnConnectStatus(unsigned long ulStatus, int nErrCode);
		virtual void OnServerListItem(const char* strMCUID,const char* strMCUType,
			unsigned long ulConnections,const char* strBandWidth,unsigned long ulActivity,
			const char* strIPAddr,unsigned short usIPPort,const char* strIPName){};
		virtual void OnServerListEnd(void) {};

		virtual void OnLogin(int nErrCode);
		virtual void OnLogout(int nErrCode);
		//////////////////////////////////////////////////////////////////////////

	public:
		void OnLogin(void);
		//void OnLoginOut();
		void SetSkinImage(CString strPath);//从文件导入图片并显示
		void ChangeLoginOrCancel(void);//显示连线按钮，隐藏取消按钮
		void InitLoginUserInfoStruct();//初始化用户信息结构体，下次填写的用户配置文件的位置
		//void SetShowLandMessage(unsigned long ulConnectStatus);//依据登录时反馈的信息，在STATIC上显示不同的文本
		void ShowLoginError(unsigned long ulErrorCode);//在m_showLandMessageSTATIC显示经过服务器判断的用户名和密码错误信息，但服务器地址是不能判断的

		BOOL CheckData(void);//在把数据传递到服务器之前对基本格式进行判断
		BOOL CheckServerIP(CString IPAddr);//在把数据传递到服务器之前对IP基本格式进行判断
		virtual BOOL ShowWindow(int nCmdShow);
		void SaveLoginInfo(void); //保存有效的
		CString *GetLoginID();
		afx_msg void OnBnClickedOnlineBtn();
		afx_msg void OnBnClickedSinglelandBtn();	
		afx_msg void OnBnClickedCloselogindlgBtn();	
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnCbnSelchangeLoginidCombox();
		void InitLoginIDCombox(CComboBox *comBoxData);
		afx_msg void OnBnClickedCancelBtn();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
		afx_msg LRESULT OnNcHitTest(CPoint point);
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnBnClickedCheckMTFunctionBtn();
	
	
	private:
		CWBBtn m_closeLoginDlgBtn; //关闭登陆框

		CComboBox m_longinIDCombox;
		//CString m_strLoginID;
		CEdit m_passwordEdit;
		CString m_strPassword;
		CWBBtn m_cancelBtn;
		CWBBtn m_singleLandBtn;
		CString m_strLoginID;
		CWBAttributeStatic m_showLandMessageSTATIC;
		CEdit m_serverHostEdit;
		CString m_strServerHost;
		CWBBtn m_onlineBtn;
		CWBBtn m_CheckMTFunctionBtn; //启动触摸功能复选框
		BOOL m_bNeedToCheckTouch; //选择了使用多点触摸，但还是要看服务器是否授权

		CWBMeetingListDlg * m_pMeetingListDlg;

	public:
		CWBMeetingListDlg* GetMeetingListDlg(void);
		void DeleteMeetingListDlg();
		afx_msg void OnDestroy();
		BOOL GetIsNeedToCheckTouch();
};

