// ScreenTXTestDlg.h : ͷ�ļ�
//

#pragma once
#include <IMXEC/IMXEC_Screen.h>
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Node.h>
#include "afxwin.h"


// CScreenTXTestDlg �Ի���
class CScreenTXTestDlg 
	: public CDialog
	, public IMXEC_ScreenTXCallback
	, public NETEC_Node
{
// ����
public:
	CScreenTXTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

	IMXEC_ScreenTX*	m_pScreenTX;

	virtual void OnIMXEC_ScreenTXCallbackConnected(void){};
	virtual void OnIMXEC_ScreenTXCallbackDisconnected(void){};
	
	/******************************************************************************
	* OnNETEC_NodeConnectStatusChanged
	*��������������״̬�Ļص�
	*���룺	cs					-��������״̬
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	// �Ի�������
	enum { IDD = IDD_VNCECTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	CEdit m_editPeerNodeID;
	afx_msg void OnBnClickedButtonConnect();
	CButton m_btnViewOnly;
	afx_msg void OnBnClickedCheckViewOnly();
};
