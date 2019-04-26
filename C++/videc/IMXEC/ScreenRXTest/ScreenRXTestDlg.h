// ScreenRXTestDlg.h : 头文件
//

#pragma once
#include <IMXEC/IMXEC_Screen.h>
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Node.h>
#include "afxwin.h"


// CScreenRXTestDlg 对话框
class CScreenRXTestDlg 
	: public CDialog
	, public IMXEC_ScreenRXCallback
	, public NETEC_Node
{
// 构造
public:
	CScreenRXTestDlg(CWnd* pParent = NULL);	// 标准构造函数

	IMXEC_ScreenRX*	m_pScreenRX;
	CString				m_strPeerNodeID;
	unsigned long		m_nVNCServerID;

	virtual void OnIMXEC_ScreenRXCallbackConnected(void){};
	virtual void OnIMXEC_ScreenRXCallbackDisconnected(void){};
	virtual void OnIMXEC_ScreenRXCallbackFullScreenChanged(int nFullScreen){};

	
	/******************************************************************************
	* OnNETEC_NodeConnectStatusChanged
	*描述：网络连接状态的回调
	*输入：	cs					-网络连接状态
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	/******************************************************************************
	* OnNETEC_NodeReceivedFromNode
	*描述：收到网络节点数据包的回调
	*输入：	cszNodeID				-网络节点标识
			pData					-数据
			nLen					-数据长度
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_NodeReceivedFromNode(const char*cszNodeID,unsigned int nFromAgentID,unsigned int nToAgentID,const char* pData,int nLen);

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
	afx_msg void OnBnClickedButtonDisconnect();
};
