#pragma once
#include "WBPreviewDlg.h"
#include "WBBackgroundDlg.h"
#include "WBFunctionAttributeDlg.h"
#include "afxwin.h"
#include "WBFunctionBtn.h"
#include "WBTeachToolDlg.h"
//#include "../WB/whiteBoard/common/WBMedia/WBGlobalMediaCtrl.h"
#include "WBFunctionResDlg.h"

#define ID_TIMER_HIDE           WM_USER + 4000
#define ID_TIMER_SHOW           WM_USER + 4001	
#define ID_TIMER_HIDETIME       WM_USER + 4003
#define ID_TIEMR_STARTGIF       WM_USER + 4004
#define ID_TIEMR_MAGICPEN       WM_USER + 4005
#define ID_TIEMR_REFRESH        WM_USER + 4006
 
// CWBFunctionDlg 对话框

class CWBFunctionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBFunctionDlg)

public:
	CWBFunctionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBFunctionDlg();

	// 对话框数据
	enum { IDD = IDD_SINWB_FUNCTIONDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CWBPreviewDlg     *m_wbPreviewDlg;
	CWBFunctionAttributeDlg    *m_wbAttributeDlg;  //属性对话框
	CWBFunctionResDlg    *m_wbResDlg;  //资源对话框
	CWBBackgroundDlg   *m_wbBackgroundDlg; //背景对话框
	CWBTeachToolDlg   *m_wbTeachToolDlg;  //工具对话框
	CRect   m_rect;    //功能面板客户区矩形
	CWnd* m_pParent;
	CRect   m_PreviewRect; //预览对话框客服区矩形
	//CWBFunctionBtn m_cHideFuncdlgButton;//隐藏或显示控制面板按钮
	//CWBAttributeStatic m_cFuncdlgHideStatic;
	//CWBFunctionBtn m_btnCloseFuncPad;
	CWBFunctionBtn m_cCloseButton;
	CWBFunctionBtn m_cPreviewButton;
	CWBFunctionBtn m_cAttributeButton;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg LRESULT OnWM_SELECTWBOBJ(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_SELECTWBPEN(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_NOSELECTWBOBJ(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_OPENARRIBUTEDLG(WPARAM wParam, LPARAM lParam);
	afx_msg	LRESULT OnWM_PALETTECOLOR(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_OPENPREVIEWDLG(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_OPENRESDLG(WPARAM wParam, LPARAM lParam);
	afx_msg	LRESULT OnWM_OPENBACKGROUNDDLG(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_OPENTEACHTOOLDLG(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_SELECTWBERASER(WPARAM wParam, LPARAM lParam);
	//afx_msg void OnLButtonDown( UINT nFlags, CPoint point);
	afx_msg void OnBnClickedClosewbfunctiondlg();
	afx_msg void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	CWBFunctionAttributeDlg *GetAttributeDlg();
	CWBPreviewDlg* GetPreviewDLG();
	CWBFunctionResDlg* GetResDlg(){return m_wbResDlg;}
	CWBBackgroundDlg *GetBackGroundDlg();
	CWBTeachToolDlg *GetTeachtoolDlg();
	void SetBtnState(int BtnId,WB_MOUSE_STATE MouseState);
	virtual void PostNcDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void ShowOrHideFunctionDlg(bool bIsShow); 
	
private:
	DWORD m_dLeaveTime;     //离开功能面板系统运行时间
	DWORD m_dLeaveTimeDiff;//时间差
	bool  m_bIsClickSolidBtn;//标注填充按钮禁止点击
	CRect m_rectPreviewMenu; //大概预览菜单的矩形区域
	bool  m_bIshidewindow;   //判断功能面板是否隐藏（用于解决跟U7-T换模板和屏幕的SHOW跟HIDE）


public:
	bool GetClickSolidBtn() {return m_bIsClickSolidBtn;}
	void SetClickSolidBtn(bool bl) {m_bIsClickSolidBtn = bl;}
	void DiffTimeHideFunctionDlg();
	void SetMouseLeaveTime(DWORD dw);
	void SetPreviewMenuRect(CRect rect);
	CRect m_rectShowOrHideFunction; //显示或隐藏功能面板的矩形
	int  m_nShowOrHideCount;    //调用隐藏或Show的次数，以便移动功能面板
	bool GetWindowIsHide(){return m_bIshidewindow;} //获取功能面板是否隐藏
	void SetWindowIsHide(bool ishide){m_bIshidewindow = ishide;} //设置功能面板是否隐藏
};

