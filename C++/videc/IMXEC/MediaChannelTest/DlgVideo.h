#pragma once


// DlgVideo �Ի���

class DlgVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgVideo)

public:
	DlgVideo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgVideo();

// �Ի�������
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
