#pragma once
#include "common/WBData.h"
#include "afxwin.h"
#include "atltypes.h"
#include "WBPreviewDlg.h"


// CWBPreviewView 视图

class CWBPreviewView : public CView
{
	DECLARE_DYNCREATE(CWBPreviewView)

public:
	CWBPreviewView (HWND ParentHwnd = NULL,CWBPage *pPage = NULL );           // 动态创建所使用的受保护的构造函数
	virtual ~CWBPreviewView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	void	SetCurPage(CWBPage *pPage);
	void SetCurPageIndex(UINT PageIndex);
	UINT GetCurPageIndex();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
private:
	//COLORREF m_bgcolor;//背景色
	CWBPage *m_pPage;
	UINT m_CurPageIndex;

	CRect				m_RectInPreview;        //预览中的Rect的位置
	CRectTracker	m_RectTracker;
 	CPoint				m_ButtonDownPoint;    //鼠标点击的位置
 	CPoint				m_ButtonUpPoint;     //鼠标点击起来的位置
 	CPoint				m_ButtonGap;
	HWND				m_parentHwnd;
	BOOL				m_bCaptured;          //是否捕获鼠标消息
	bool					m_bAccordPreViewScroll;     //是否主动移动滚动条
	bool					m_bViewIsRedraw;				//view是否重画
	CRect				m_rectwbView; //白板的view的大小
	float					m_fRatioX;
	float					m_fRatioY;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void SetRectInPreview();
	void SetRatio();
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	CRect m_rect;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	
};