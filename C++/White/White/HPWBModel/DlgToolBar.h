#pragma once
#include "TransparentButtonEx.h"
#include "afxwin.h"
#include <GdiPlus.h>
#include "DlgFuck.h"
#include "ResourceDef.h"
#include "Resource.h"
#include "DlgResource.h"
#include "DlgRubber.h"
#include "DlgGraphics.h"
#include "DlgFile.h"
#include "DlgColor.h"
#include "DlgPage.h"
#include "DLGDesktopDraw.h"
using namespace Gdiplus;
// CDlgToolBar 对话框
#define TIMER_ID_TOOL 2003
#define WIDTH_BTN 54
#define HEIGHT_BTN 54
#define WIDTH 70
#define HEIGHT 589
#define SPACE 2
#define WIDTH_HEAD 35
#define WM_FLASH WM_USER + 0x0001
class CDlgToolBar : public CDialog
{
	DECLARE_DYNAMIC(CDlgToolBar)

public:
	CDlgToolBar(CWnd* pParent = NULL);   //标准构造函数
	virtual ~CDlgToolBar();

// 对话框数据
	enum { IDD = IDD_DLG_TOOLBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    //DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    void FlashBackground();//加载工具条背景
    void ProcessCmd(eType cmdType,bool bFromBox = false);
	void SetWBMain(CWnd* pWnd);
    void OpenToolBox();
	void ReLayout();
	void ChangButton(CTransparentButtonEx *pDownBtn,int downY,CTransparentButtonEx * pUpBtn,int upY); //交换自身内部按钮的位置
    void DelAndNewBtn(CTransparentButtonEx *pBtn,int btnY,eType btnEmumType,LPCTSTR lpNewCaption); //删除和生成按钮
	CString GetCaptureFromType(eType btnEmumType);
	void CloseAllChildToolBox();
		//分类中的函数
    void OpenResource();//打开资源对话框
	void setResourceDlgNULL();// 设置资源对话框指针为空
	void OpenRubber();//打开擦子对话框
    void setRubberDlgNULL();// 设置擦子对话框指针为空
    void OpenGraphics();//打开图形对话框
    void setGraphicsDlgNULL();// 设置图形对话框指针为空
    void OpenFile();//打开文件对话框
    void setFileDlgNULL();// 设置文件对话框指针为空
	void OpenColor();
	void setColorDlgNULL();
    void OpenPage();
	void setPageDlgNULL();
    void OpenDesktopDraw();
	void setDesktopDrawDlgNULL();



	
private:
	int x; //按钮在工具条中X轴的位置

	CRect m_foreWindowRt; //记录工具条上一次的位置
	Bitmap* m_pBitmap; //工具条背景位图
	CDlgFuck* m_pToolBox; //设置对话框指针
	//分类中的变量
    CDlgResource *m_pResourceDlg;//资源对话框指针
    CDlgRubber * m_pRubber;//擦子对话框指针
    CDlgGraphics * m_pGraphics;//图形对话框指针
	CDlgFile * m_pFileDlg; //文件对话框指针
	CDlgColor *m_pColor;
    CDlgPage  *m_pPage;
	CDLGDesktopDraw *m_pDesktopDraw;
    
    CToolTipCtrl m_ToolTip;//工具提示控件
	CWnd* m_pWbWnd;
	bool m_bFullScreen;

	CTransparentButtonEx *m_btnOne;
	CTransparentButtonEx *m_btnTwo;
	CTransparentButtonEx *m_btnThree;
	CTransparentButtonEx *m_btnFour;
	CTransparentButtonEx *m_btnFive;
	CTransparentButtonEx *m_btnSix;
	CTransparentButtonEx *m_btnSeven;
	CTransparentButtonEx *m_btnEight;
	CTransparentButtonEx *m_btnNine;
	CTransparentButtonEx *m_btnTen;
public:
    static HWND g_toolBarHWND;
protected:
	virtual void OnOK();
	virtual void OnCancel();

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtn1();
	afx_msg void OnBnClickedBtn2();
	afx_msg void OnBnClickedBtn3();
	afx_msg void OnBnClickedBtn4();
	afx_msg void OnBnClickedBtn5();
	afx_msg void OnBnClickedBtn6();
	afx_msg void OnBnClickedBtn7();
	afx_msg void OnBnClickedBtn8();
	afx_msg void OnBnClickedBtn9();
	afx_msg void OnBnClickedBtn10();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnCloseToolbarMessage(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
