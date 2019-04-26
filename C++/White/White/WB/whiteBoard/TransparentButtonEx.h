#pragma once
#include <GdiPlus.h>
#include "common/ResourceDef.h"
using namespace Gdiplus;
#include <vector>
#include "WBToolTip.h"
#include "WBAboutDlg.h"


// CTransparentButtonEx

class CTransparentButtonEx : public CButton
{
	DECLARE_DYNAMIC(CTransparentButtonEx)

public:
	CTransparentButtonEx();
	virtual ~CTransparentButtonEx();

protected:
	DECLARE_MESSAGE_MAP()
protected:

	
	//m_pImg按钮的图片，因为是透明图所以m_pImgBG是额外添加了背景图
	Bitmap			 *m_pImg,*m_pImgBG;
	//按钮图片得文件路径
	CString			 m_strPath;
	//本按钮的类型
	eType            m_btnType;
	//按钮所在的父窗口指针
	CWnd             *m_pParentWnd;
	//按钮按下的鼠标点坐标
	CPoint           m_downPoint;
	//按钮弹起的鼠标点坐标
	CPoint           m_upPoint;
	//按钮的ID号
	UINT             m_btnID;
	//按钮的标题文本
	CString			 m_strName;
	//
	BOOL            m_bMouseIn;
	DWORD            m_beginTime,m_endTime;//长按的开始结束时间
	int				 m_iTimerID; //判断时候开启定时器
    XCritSec			m_ButtonUpWriteLock;	//按钮弹起时写按钮位置信息上锁
	BOOL          m_bBTNStatus;
	
public:
	static MAP_PATH		 m_mapPath;
	static eType g_GraphicsbuttonType;  //记录图形当前选择的按钮类型（eType）
	static eType g_LineTypebuttonType;  //记录线型当前选择的按钮类型（eType）
	static eType g_RubberbuttonType;  //记录擦子当前选择的按钮类型（eType）

	static void Init();
    void SetParentWnd(CWnd* pWnd);
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    void SetButtonImage(CString filename);//设置按钮图片

    void SetType(eType enumType,bool isboxbtn = false);//获得按钮图片路径
	eType GetType();

	void SetName(CString strName);
	CString GetName();
	void SetButtonStatus(BOOL bStatus, eType ntype = TYPE_NONE);
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT  OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnMouseHover(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTimer(UINT nIDEvent);


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};


