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


// CWBLoginDlg �Ի���

	class CWBLoginDlg : public CDialog
		, public IPlugin_U7_IM_Login_Observer//��½��Ϣ��������
	{
		DECLARE_DYNAMIC(CWBLoginDlg)

	public:
		CWBLoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
		virtual ~CWBLoginDlg();

	// �Ի�������
		enum { IDD = IDD_WBLOGINDLG };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
		Bitmap * m_pBitmap;
		// ���ɵ���Ϣӳ�亯��
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
		//�ؼ�
		//CWBBtn m_onlineBtn;           //������¼��ť
		//CWBBtn m_singleLandBtn;       //����������ť 
		//CWBBtn m_closeLoginDlgBtn;
		//CWBBtn m_cancelBtn;
		//CEdit m_serverHostEdit;//��½��ַ�ı���
		//CEdit m_passwordEdit;//��½�����ı���
		//CComboBoxEx m_longinIDCombox;//��½�û�����Ͽ��б�
		//CWBAttributeStatic m_showLandMessageSTATIC;
		//CString m_strServerHost;      //��������ַ�ı���
		//CString m_strLoginID;         //��¼ID�ı���
		CRect m_rect;//��½�Ի�����ʾ����

		LoginUserInfo m_userInfoArr[SAVEUSERINFOLONG];//����û���Ϣ��
		int m_indexUserInfoPos;//����ͳ�Ʊ���Ľ�λ�ã��ǰ���SAVEUSERINFOLONG-1����ѭ������
	public:
		//////////////////////////////////////////////////////////////////////////
		// �̳�IPlugin_U7_IM_Login_Observer�ӿ�
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
		void SetSkinImage(CString strPath);//���ļ�����ͼƬ����ʾ
		void ChangeLoginOrCancel(void);//��ʾ���߰�ť������ȡ����ť
		void InitLoginUserInfoStruct();//��ʼ���û���Ϣ�ṹ�壬�´���д���û������ļ���λ��
		//void SetShowLandMessage(unsigned long ulConnectStatus);//���ݵ�¼ʱ��������Ϣ����STATIC����ʾ��ͬ���ı�
		void ShowLoginError(unsigned long ulErrorCode);//��m_showLandMessageSTATIC��ʾ�����������жϵ��û��������������Ϣ������������ַ�ǲ����жϵ�

		BOOL CheckData(void);//�ڰ����ݴ��ݵ�������֮ǰ�Ի�����ʽ�����ж�
		BOOL CheckServerIP(CString IPAddr);//�ڰ����ݴ��ݵ�������֮ǰ��IP������ʽ�����ж�
		virtual BOOL ShowWindow(int nCmdShow);
		void SaveLoginInfo(void); //������Ч��
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
		CWBBtn m_closeLoginDlgBtn; //�رյ�½��

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
		CWBBtn m_CheckMTFunctionBtn; //�����������ܸ�ѡ��
		BOOL m_bNeedToCheckTouch; //ѡ����ʹ�ö�㴥����������Ҫ���������Ƿ���Ȩ

		CWBMeetingListDlg * m_pMeetingListDlg;

	public:
		CWBMeetingListDlg* GetMeetingListDlg(void);
		void DeleteMeetingListDlg();
		afx_msg void OnDestroy();
		BOOL GetIsNeedToCheckTouch();
};

