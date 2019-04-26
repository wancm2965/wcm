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
	//COLORREF m_bgcolor;//����ɫ
	CWBPage*					m_pPage;
	CRect							m_rect;
	CRect							m_RectInPreview;					//Ԥ���е�Rect��λ��
	CRectTracker				m_RectTracker;
	CPoint							m_ButtonDownPoint;			//�������λ��
	CPoint							m_ButtonUpPoint;				//�����������λ��
	CPoint							m_ButtonGap;
	HWND							m_parentHwnd;
	BOOL								m_bCaptured;						//�Ƿ񲶻������Ϣ
	bool								m_bViewIsRedraw;				//view�Ƿ��ػ�
	bool								m_bAccordPreViewScroll;     //�Ƿ������ƶ�������
	float								m_fRatioX;								//view��װ�ı���
	float								m_fRatioY;
	CRect							m_rectwbView;						//�װ�Ĵ�С
	CWBFunctionBtn			m_CloseBtn;						//�رհ�ť
	CPoint							m_offpoint;						//�ƶ���ƫ��λ��
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

