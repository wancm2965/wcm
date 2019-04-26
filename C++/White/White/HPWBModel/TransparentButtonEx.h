#pragma once
#include <GdiPlus.h>
using namespace Gdiplus;
#include <vector>



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

	MAP_PATH		 m_mapPath;
	Bitmap			 *m_pImg;
	CString			 m_strPath;
	eType            m_btnType;
	CWnd             *m_pParentWnd;
	CPoint           m_downPoint;
	CPoint           m_upPoint;
	UINT             m_btnID;
	CString			 m_strName;
	BOOL            m_bMouseIn;


	void Init();
public:
	static eType g_buttonType;  //记录当前选择的按钮类型（eType）


    void SetParentWnd(CWnd* pWnd);
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    void SetButtonImage(CString filename);//设置按钮图片

    void SetType(eType enumType);//获得按钮图片路径
	eType GetType();

	void SetName(CString strName);
	CString GetName();

	virtual void DrawItem(LPDRAWITEMSTRUCT);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT  OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnMouseHover(WPARAM wParam, LPARAM lParam);
};


