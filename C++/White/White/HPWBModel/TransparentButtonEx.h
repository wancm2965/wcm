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
	static eType g_buttonType;  //��¼��ǰѡ��İ�ť���ͣ�eType��


    void SetParentWnd(CWnd* pWnd);
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    void SetButtonImage(CString filename);//���ð�ťͼƬ

    void SetType(eType enumType);//��ð�ťͼƬ·��
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


