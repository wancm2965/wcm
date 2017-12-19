
// MFCtestDlg.h : 头文件
//

#pragma once
#include "NetStream.h"
#include "FFmpegrecord/Iffmpegfilerecord.h"
// CMFCtestDlg 对话框
class CMFCtestDlg : public CDialogEx
	,public IFFmpegfilerecordNotify
{
// 构造
public:
	CMFCtestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFCTEST_DIALOG };
	virtual  void OnCompoundResut(int nResult);
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
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();


private:
	CNetStream* m_pNetSream;
	IFFmpegrecord*m_precord;

	IFFmpegfilerecord*m_pfilerecord;
public:
	afx_msg void OnBnClickedOk();
};
