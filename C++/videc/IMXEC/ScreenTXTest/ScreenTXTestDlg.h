// ScreenTXTestDlg.h : 头文件
//

#pragma once
#include <IMXEC/IMXEC_Screen.h>
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Node.h>
#include "afxwin.h"


// CScreenTXTestDlg 对话框
class CScreenTXTestDlg 
	: public CDialog
	, public IMXEC_ScreenTXCallback
	, public NETEC_Node
{
// 构造
public:
	CScreenTXTestDlg(CWnd* pParent = NULL);	// 标准构造函数

	IMXEC_ScreenTX*	m_pScreenTX;

	virtual void OnIMXEC_ScreenTXCallbackConnected(void){};
	virtual void OnIMXEC_ScreenTXCallbackDisconnected(void){};
	
	/******************************************************************************
	* OnNETEC_NodeConnectStatusChanged
	*描述：网络连接状态的回调
	*输入：	cs					-网络连接状态
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	// 对话框数据
	enum { IDD = IDD_VNCECTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
