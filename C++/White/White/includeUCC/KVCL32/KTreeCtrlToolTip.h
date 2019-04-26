#pragma once

class AFX_EXT_CLASS KTreeCtrlToolTip : public CTreeCtrl
{
	DECLARE_DYNAMIC(KTreeCtrlToolTip)

public:
	KTreeCtrlToolTip();
	virtual ~KTreeCtrlToolTip();

protected:
	DECLARE_MESSAGE_MAP()
	BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	virtual void PreSubclassWindow();
public:
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual CString GetToolTipText(HTREEITEM hTreeItem){return _T("");};
};


