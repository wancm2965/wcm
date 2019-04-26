#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "WBAttributeStatic.h"

// CWBToolboxDlg 对话框

class CWBToolboxDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBToolboxDlg)

public:
	CWBToolboxDlg(CWnd* pParent = NULL , int nNewBtnNum = 0);   // 标准构造函数
	virtual ~CWBToolboxDlg();

// 对话框数据
	enum { IDD = IDD_WB_TOOLBOXDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
private:
	CWnd * m_pParentDlg;
	Bitmap* m_pBitmap;
	int m_nNewBtnnum ;
	CWBAttributeStatic m_toolboxStatic;
	CTransparentButtonEx *m_closeBtn;
	CTransparentButtonEx *m_btnA;
	CTransparentButtonEx *m_btnB;
	CTransparentButtonEx *m_btnC;
	CTransparentButtonEx *m_btnD;
	CTransparentButtonEx *m_btnE;
	CTransparentButtonEx *m_btnF;
	CTransparentButtonEx *m_btnG;
	CTransparentButtonEx *m_btnH;
	//CTransparentButtonEx *m_btnI;
	CTransparentButtonEx *m_btnJ;
	CTransparentButtonEx *m_btnK;
	//CTransparentButtonEx *m_btnK;
	//CTransparentButtonEx *m_btnL;
	//CTransparentButtonEx *m_btnM;
	//CTransparentButtonEx *m_btnN;
	//CTransparentButtonEx *m_btnO;
	//CTransparentButtonEx *m_btnP;
	//CTransparentButtonEx *m_btnQ;
	//CTransparentButtonEx *m_btnR;
	CTransparentButtonEx *m_NewbtnA;
	CTransparentButtonEx *m_NewbtnB;
	CTransparentButtonEx *m_NewbtnC;
	CTransparentButtonEx *m_NewbtnD;
	CTransparentButtonEx *m_NewbtnE;
	CTransparentButtonEx *m_NewbtnF;
	CTransparentButtonEx *m_NewbtnG;
	CButton m_setBtn;
private:
	int ReadCfgInt(CString strFile, CString strSection, CString strKey, int nDefValue = 0);//读取判断是否为终端机

public:
	static HWND g_toolBoxHWND;
	void Relayout();
	void SetSkinImage(CString strPath);
	void ChangButton(CTransparentButtonEx *pDownBtn,int downX,int downY,CTransparentButtonEx * pUpBtn,int upX,int upY); //交换自身内部按钮的位置	
	CString GetCaptureFromType(eType btnEmumType);


	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCloseBtn();
	afx_msg void OnBtnA();
	afx_msg void OnBtnB();
	afx_msg void OnBtnC();
	afx_msg void OnBtnD();
	afx_msg void OnBtnE();
	afx_msg void OnBtnF();
	afx_msg void OnBtnG();
	afx_msg void OnBtnH();
	//afx_msg void OnBtnI();
	afx_msg void OnBtnJ();
	afx_msg void OnBtnK();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNewBtnA();
	afx_msg void OnNewBtnB();
	afx_msg void OnNewBtnC();
	afx_msg void OnNewBtnD();
	afx_msg void OnNewBtnE();
	afx_msg void OnNewBtnF();
	afx_msg void OnNewBtnG();
};
