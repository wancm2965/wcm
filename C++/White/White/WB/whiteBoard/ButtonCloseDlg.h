#pragma once 


// CbuttonCloseDlg �Ի���
class CbuttonCloseDlg : public CDialogEx
{
	// ����
public:
	CbuttonCloseDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_DOCCONVERTTIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonClose();
	void setWBDoc(CWBDocument *pDoc);
private:
	//CButton m_btd;
	CWBDocument *pWBDoc;
};
