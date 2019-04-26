#pragma once


// CWBAttributeStatic
//namespace AVCONPlugin
//{
	class CWBAttributeStatic : public CStatic
	{
		DECLARE_DYNAMIC(CWBAttributeStatic)

	public:
		CWBAttributeStatic();
		virtual ~CWBAttributeStatic();

	protected:
		DECLARE_MESSAGE_MAP()
	private:
		CString m_strCaption;
		BOOL    m_bText;
	public:
		void SetCaption( CString strCaption );
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnPaint();
		void SetTextPosition(BOOL bText);
	};
//}


