#pragma once


//namespace AVCONPlugin
//{
	class CWBBtn : public CBitmapButton
	{
		DECLARE_DYNAMIC(CWBBtn)

	public:
		CWBBtn();
		virtual ~CWBBtn();

	protected:
		DECLARE_MESSAGE_MAP()
	private:
		BOOL m_bMouseIn;
		BOOL m_isSelect;
		CString m_ImageFilePath; //当前按钮的文件路径
	public:

		CString GetCurrentPath();
		void  SetImageFilePath();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDS/*lpDrawItemStruct*/);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
 		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
 		afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
 		afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		void SetIsSelect(BOOL bIsSelect);
		BOOL GetIsSelect();
	};
//}


