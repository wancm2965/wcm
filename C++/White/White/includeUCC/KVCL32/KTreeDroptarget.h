#pragma once

class AFX_EXT_CLASS KTreeDropTarget :public COleDropTarget	
{
public:
	KTreeDropTarget();
	virtual ~KTreeDropTarget();

	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave( CWnd* pWnd );
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

public:
	bool		m_bBeginDrop;
	bool		m_bOnDrop;
	CTreeCtrl	*m_pTreeCtrl;
};
