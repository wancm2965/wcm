#pragma once
#include "stdafx.h"
#include "HPWBDataTransfer.h"


// CMainDlgWB 对话框

class CMainDlgWB : public CDialog
						,IHPWBModel
						,IHPWBDataNotify
{
	DECLARE_DYNAMIC(CMainDlgWB)

public:
	CMainDlgWB(CWnd* pParent = NULL);   // 标准构造函数
	CMainDlgWB(IHPNetNotify* pNotify,IHPWBNotify* pWBNotify,bool bNeedToCheckTouchScreen,const std::string& strLanguageFile,const std::string&  strIniPath,const std::string&  strTempPath,CWnd* pParent = NULL);
	virtual ~CMainDlgWB();

// 对话框数据
	enum { IDD = IDD_DLG_WB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	IHPNetNotify* 		m_pNetNotify;   // 同步消息指针
	IHPWBNotify*  		m_pWBNotify;
	
	CRect	      		m_oldRect;
	CWnd*         		m_pParentWnd;
	bool		  		m_bTouchScreen;//最终是否拥有授权
	bool		  		m_bNeedToCheckTouch;//avcon中选择了使用多点触摸，但还是要看服务器是否授权
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

	//白板所在屏为活动屏时可以接收广播
	 void ShowBroadCast(bool bShow);
	 //关闭工具栏子对话框窗口
	 void CloseAllChildBox();
	 //关闭屏幕绘图窗口
	 void DestroyWnd(bool bIsSend);
	 //白板是否最小化了，最小化则为TRUE，否则为FALSE
	 void SetIsMinSize(BOOL bIsMinSize);

	BOOL StartConferenceRecord();		// 开始会议录像
	void StopConferenceRecord();		// 停止会议录像	
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
