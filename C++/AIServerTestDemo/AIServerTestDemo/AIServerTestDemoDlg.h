
// AIServerTestDemoDlg.h : ͷ�ļ�
//

#pragma once


#include "HttpAPIServer.h"
#include "afxwin.h"
#include "Resource.h"

class CHttpAPIServer;

// CAIServerTestDemoDlg �Ի���
class CAIServerTestDemoDlg : public CDialogEx
{
// ����
public:
	CAIServerTestDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AISERVERTESTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	void ShowAPIBCKMessage(const char* strBCKMsg);
	void SendCallBackMessage(const char* host, const char* message);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
