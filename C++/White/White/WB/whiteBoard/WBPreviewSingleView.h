#pragma once
#include "afxwin.h"
#include "common/WBData.h"
#include "WBFunctionBtn.h"
#define  BTN_EAGLEEYE_CLOSE  WM_USER +2


class CWBPreviewSingleView :
	public CView
{
public:
	CWBPreviewSingleView(HWND ParentHwnd = NULL,CWBPage*  pPage = NULL);
	virtual ~CWBPreviewSingleView(void);
private:
	/*DECLARE_DYNCREATE(CWBPreviewSingleView)*/
public:
	virtual void OnDraw(CDC* /*pDC*/);
	void SetCurPage(CWBPage *pPage);
	void SetCloseBtn();
	void SetBtnState(int BtnId,WB_MOUSE_STATE MouseState);	
	void SetIsRedrawView(bool bRedraw);
	bool bTransmitMessageToWBView();
// #ifdef _DEBUG
// 	virtual void AssertValid() const;
// #ifndef _WIN32_WCE
// 	virtual void Dump(CDumpContext& dc) const;
// #endif
// #endif

private:
	//COLORREF m_bgcolor;//背景色
	CWBPage*					m_pPage;
	CRect							m_rect;
	CRect							m_RectInPreview;					//预览中的Rect的位置
	CRectTracker				m_RectTracker;
	CPoint							m_ButtonDownPoint;			//鼠标点击的位置
	CPoint							m_ButtonUpPoint;				//鼠标点击起来的位置
	CPoint							m_ButtonGap;
	HWND							m_parentHwnd;
	BOOL								m_bCaptured;						//是否捕获鼠标消息
	bool								m_bViewIsRedraw;				//view是否重画
	bool								m_bAccordPreViewScroll;     //是否主动移动滚动条
	float								m_fRatioX;								//view与白板的比例
	float								m_fRatioY;
	CRect							m_rectwbView;						//白板的大小
	CWBFunctionBtn			m_CloseBtn;						//关闭按钮
	CPoint							m_offpoint;						//移动的偏移位置
public:
	void SetRectInPreview();
	void SetRatio();
	void ShowButtonPlace();
public:
// 	afx_msg void OnSize(UINT nType, int cx, int cy);
// 	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	afx_msg LRESULT Synchronizer(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnEagleEyeClose();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

