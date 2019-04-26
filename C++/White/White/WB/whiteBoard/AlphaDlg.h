#pragma once


// CAlphaDlg �Ի���

class CAlphaDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAlphaDlg)

public:
	CAlphaDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CAlphaDlg(const CRect* rect);
	virtual ~CAlphaDlg();

	// �Ի�������
	enum { IDD = IDD_ALPHACHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	const CRect* rects;
	int m_alpha;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL AlphaChange(const int alpha);
};
