// ScreenRXTestDlg.h : ͷ�ļ�
//

#pragma once
#include <IMXEC/IMXEC_Screen.h>
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Node.h>
#include "afxwin.h"


// CScreenRXTestDlg �Ի���
class CScreenRXTestDlg 
	: public CDialog
	, public IMXEC_ScreenRXCallback
	, public NETEC_Node
{
// ����
public:
	CScreenRXTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

	IMXEC_ScreenRX*	m_pScreenRX;
	CString				m_strPeerNodeID;
	unsigned long		m_nVNCServerID;

	virtual void OnIMXEC_ScreenRXCallbackConnected(void){};
	virtual void OnIMXEC_ScreenRXCallbackDisconnected(void){};
	virtual void OnIMXEC_ScreenRXCallbackFullScreenChanged(int nFullScreen){};

	
	/******************************************************************************
	* OnNETEC_NodeConnectStatusChanged
	*��������������״̬�Ļص�
	*���룺	cs					-��������״̬
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	/******************************************************************************
	* OnNETEC_NodeReceivedFromNode
	*�������յ�����ڵ����ݰ��Ļص�
	*���룺	cszNodeID				-����ڵ��ʶ
			pData					-����
			nLen					-���ݳ���
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_NodeReceivedFromNode(const char*cszNodeID,unsigned int nFromAgentID,unsigned int nToAgentID,const char* pData,int nLen);

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
	afx_msg void OnBnClickedButtonDisconnect();
};
