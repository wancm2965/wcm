#pragma once


// CWBTextAttributeDlg 对话框

class CWBTextAttributeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBTextAttributeDlg)

public:
	CWBTextAttributeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBTextAttributeDlg();

// 对话框数据
	enum { IDD = IDD_WBTEXTATTRIBUTE };

	LOGFONT m_logFont; //字体
	COLORREF m_clrFont; //字体颜色
	CString m_strText; //文本内容

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTextfont();
	virtual BOOL OnInitDialog();

	CEdit m_editFont;
	//CFont m_Font;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
