#pragma once
#include "common/WBData.h"
#include "afxwin.h"
#include "atltypes.h"
#include "WBPreviewDlg.h"


// CWBPreviewView ��ͼ

class CWBPreviewView : public CView
{
	DECLARE_DYNCREATE(CWBPreviewView)

public:
	CWBPreviewView (HWND ParentHwnd = NULL,CWBPage *pPage = NULL );           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWBPreviewView();

public:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
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
	//COLORREF m_bgcolor;//����ɫ
	CWBPage *m_pPage;
	UINT m_CurPageIndex;

	CRect				m_RectInPreview;        //Ԥ���е�Rect��λ��
	CRectTracker	m_RectTracker;
 	CPoint				m_ButtonDownPoint;    //�������λ��
 	CPoint				m_ButtonUpPoint;     //�����������λ��
 	CPoint				m_ButtonGap;
	HWND				m_parentHwnd;
	BOOL				m_bCaptured;          //�Ƿ񲶻������Ϣ
	bool					m_bAccordPreViewScroll;     //�Ƿ������ƶ�������
	bool					m_bViewIsRedraw;				//view�Ƿ��ػ�
	CRect				m_rectwbView; //�װ��view�Ĵ�С
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