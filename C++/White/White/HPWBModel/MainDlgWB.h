#pragma once
#include "stdafx.h"
#include "HPWBDataTransfer.h"


// CMainDlgWB �Ի���

class CMainDlgWB : public CDialog
						,IHPWBModel
						,IHPWBDataNotify
{
	DECLARE_DYNAMIC(CMainDlgWB)

public:
	CMainDlgWB(CWnd* pParent = NULL);   // ��׼���캯��
	CMainDlgWB(IHPNetNotify* pNotify,IHPWBNotify* pWBNotify,bool bNeedToCheckTouchScreen,const std::string& strLanguageFile,const std::string&  strIniPath,const std::string&  strTempPath,CWnd* pParent = NULL);
	virtual ~CMainDlgWB();

// �Ի�������
	enum { IDD = IDD_DLG_WB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
protected:
	IHPNetNotify* 		m_pNetNotify;   // ͬ����Ϣָ��
	IHPWBNotify*  		m_pWBNotify;
	
	CRect	      		m_oldRect;
	CWnd*         		m_pParentWnd;
	bool		  		m_bTouchScreen;//�����Ƿ�ӵ����Ȩ
	bool		  		m_bNeedToCheckTouch;//avcon��ѡ����ʹ�ö�㴥����������Ҫ���������Ƿ���Ȩ
	std::string		  	m_strLanguageFile;
	std::string			m_strIniPath;
	std::string			m_strTempPath;
public:
	void Init();
	void ShowMainWnd(bool bShow);
	void ReleaseModelResource();
	
	//IHPWBDataNotify
	void ProcessWBDataOut(const char* pData,int iLen);

	//IHPWBModel
	void Release(void);
	void SetParentWnd(CWnd* pWnd);
	void RegistNetNotify(IHPNetNotify* pNotify);
	void ProcessNetDataIn(const char* pData,int iLen);
	void SetWBWndPos(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag);
	void ShowWBWnd(bool bShow);
	void RequestAllObj();
	void ShowToolBar(bool bShow);
	
	void Start(bool bTouchScreen);
	void SetCheckTouchScreen(bool bCheck);
	BOOL OpenPic(CString strPath);
	void SetLocalUserID(CString strUserID);
	void MemberOffline(CString strUserID);

	//�װ�������Ϊ���ʱ���Խ��չ㲥
	 void ShowBroadCast(bool bShow);
	 //�رչ������ӶԻ��򴰿�
	 void CloseAllChildBox();
	 //�ر���Ļ��ͼ����
	 void DestroyWnd(bool bIsSend);
	 //�װ��Ƿ���С���ˣ���С����ΪTRUE������ΪFALSE
	 void SetIsMinSize(BOOL bIsMinSize);

	BOOL StartConferenceRecord();		// ��ʼ����¼��
	void StopConferenceRecord();		// ֹͣ����¼��	
	void Lock(bool bLock);
	//
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
