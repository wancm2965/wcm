
// MFCtestDlg.h : ͷ�ļ�
//

#pragma once
#include "NetStream.h"
#include "FFmpegrecord/Iffmpegfilerecord.h"
// CMFCtestDlg �Ի���
class CMFCtestDlg : public CDialogEx
	,public IFFmpegfilerecordNotify
{
// ����
public:
	CMFCtestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCTEST_DIALOG };
	virtual  void OnCompoundResut(int nResult);
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
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();


private:
	CNetStream* m_pNetSream;
	IFFmpegrecord*m_precord;

	IFFmpegfilerecord*m_pfilerecord;
public:
	afx_msg void OnBnClickedOk();
};
