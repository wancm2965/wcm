#pragma once


// CWBTextAttributeDlg �Ի���

class CWBTextAttributeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBTextAttributeDlg)

public:
	CWBTextAttributeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBTextAttributeDlg();

// �Ի�������
	enum { IDD = IDD_WBTEXTATTRIBUTE };

	LOGFONT m_logFont; //����
	COLORREF m_clrFont; //������ɫ
	CString m_strText; //�ı�����

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTextfont();
	virtual BOOL OnInitDialog();

	CEdit m_editFont;
	//CFont m_Font;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
