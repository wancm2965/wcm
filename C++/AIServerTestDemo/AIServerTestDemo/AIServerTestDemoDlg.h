
// AIServerTestDemoDlg.h : 头文件
//

#pragma once


#include "HttpAPIServer.h"
#include "afxwin.h"
#include "Resource.h"

class CHttpAPIServer;

// CAIServerTestDemoDlg 对话框
class CAIServerTestDemoDlg : public CDialogEx
{
// 构造
public:
	CAIServerTestDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AISERVERTESTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	void ShowAPIBCKMessage(const char* strBCKMsg);
	void SendCallBackMessage(const char* host, const char* message);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CHttpAPIServer*  m_pAPIServer;
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	CEdit m_pFileEdit;
	CStatic m_pShowMsg;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CEdit m_pCollectUsr;
	CEdit m_pVerNum;
	CEdit m_pVerthresold;
	CEdit m_pEditServerAPIPath;
};
