#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "ResourceDef.h"
#include <GdiPlus.h>
#include "Resource.h"
using namespace Gdiplus;
// CDlgFuck 对话框
#define TIMER_ID_KEY 2001
#define WIDTH_TOOLBOX 405
#define HEIGHT_TOOLBOX 284
#define WIDTH_BTN 54
#define HEIGHT_BTN 54
#define WIDTH_SMALLBTN 32
#define HEIGHT_SMALLBTN 31
#define HEAD_TOOLBOX 40
#define ROW_NUM 3
#define COLUMN_NUM 5
#define BTN_NUM_PAGE 15
class CDlgFuck : public CDialog
{
	DECLARE_DYNAMIC(CDlgFuck)

public:
	CDlgFuck(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgFuck();

// 对话框数据
	enum { IDD = IDD_Dlg_ToolBox };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:	
	CPoint m_point;
	Bitmap* m_pBitmap;
	CWnd* m_pWndParent;
    CToolTipCtrl m_ToolTip;//工具提示控件
	CTransparentButtonEx *m_btnClose;
	CTransparentButtonEx *m_btnA;
	CTransparentButtonEx *m_btnB;
	CTransparentButtonEx *m_btnC;
	CTransparentButtonEx *m_btnD;
	CTransparentButtonEx *m_btnE;
	CTransparentButtonEx *m_btnF;
	CTransparentButtonEx *m_btnG;
	CTransparentButtonEx *m_btnH;
	CButton *m_btnI;
	CButton *m_btnJ;
	CButton *m_btnK;
	CButton *m_btnL;
	CButton *m_btnM;
	CButton *m_btnN;
	CButton *m_btnO;

	void Relayout(); //生成和布局按钮
	


public:
    static HWND g_toolBoxHWND;
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnA();
	afx_msg void OnBnClickedBtnB();
	afx_msg void OnBnClickedBtnC();
	afx_msg void OnBnClickedBtnD();
	afx_msg void OnBnClickedBtnE();
	afx_msg void OnBnClickedBtnF();
	afx_msg void OnBnClickedBtnG();
	afx_msg void OnBnClickedBtnH();
	afx_msg void OnBnClickedBtnI();
	afx_msg void OnBnClickedBtnJ();
	afx_msg void OnBnClickedBtnK();
	afx_msg void OnBnClickedBtnL();
	afx_msg void OnBnClickedBtnM();
	afx_msg void OnBnClickedBtnN();
	afx_msg void OnBnClickedBtnO();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);


	void SetParentWnd(CWnd* pWnd);
	CWnd* GetParentWnd();
   void ChangButton(CTransparentButtonEx *pDownBtn,int downX,int downY,CTransparentButtonEx * pUpBtn,int upX,int upY); //交换自身内部按钮的位置
   void DelAndNewBtn(CTransparentButtonEx *pBtn,int btnX,int btnY,eType btnEmumType,LPCTSTR lpNewCaption); //删除和生成按钮
   CString GetCaptureFromType(eType btnEmumType);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
