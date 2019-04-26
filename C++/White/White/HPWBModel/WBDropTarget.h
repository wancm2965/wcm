
#pragma once
#define WM_DROP_NOTIFY  (WM_USER + 5000)
// CWBDropTarget ÃüÁîÄ¿±ê

class CWBDropTarget : public COleDropTarget
{
	DECLARE_DYNAMIC(CWBDropTarget)

public:
	CWBDropTarget();
	virtual ~CWBDropTarget();

protected:
	DECLARE_MESSAGE_MAP()
public:
	HWND	m_hNotifyWnd;
	UINT	m_hFormatData;
public:
	void RegisterFormat(CString strFormat);
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,	DROPEFFECT dropEffect, CPoint point);

};


