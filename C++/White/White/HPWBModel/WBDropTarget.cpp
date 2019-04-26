// WBDropTarget.cpp : 实现文件
//

#include "stdafx.h"
#include "WBDropTarget.h"


// CWBDropTarget

IMPLEMENT_DYNAMIC(CWBDropTarget, COleDropTarget)
CWBDropTarget::CWBDropTarget()
	:m_hNotifyWnd(NULL)
{
	m_hFormatData = RegisterClipboardFormat(_T("WBBTN"));
}

CWBDropTarget::~CWBDropTarget()
{
	m_hNotifyWnd = NULL;
	Revoke();
}


BEGIN_MESSAGE_MAP(CWBDropTarget, COleDropTarget)
END_MESSAGE_MAP()

void CWBDropTarget::RegisterFormat(CString strFormat)
{
	m_hFormatData = RegisterClipboardFormat(strFormat);
}
DROPEFFECT CWBDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	if ( pDataObject && pDataObject->GetGlobalData(m_hFormatData))
	{
		dwEffect = DROPEFFECT_COPY;
	}
	return dwEffect;
}
void CWBDropTarget::OnDragLeave(CWnd* pWnd)
{
	COleDropTarget::OnDragLeave(pWnd);
}
DROPEFFECT CWBDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	if ( pDataObject && pDataObject->GetGlobalData (m_hFormatData))
	{
		dwEffect = DROPEFFECT_COPY;
	}
	return dwEffect;
}
BOOL CWBDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,	DROPEFFECT dropEffect, CPoint point)
{
	if (!pDataObject)
		return FALSE;

	UINT		format = m_hFormatData;
	HGLOBAL     hg = pDataObject->GetGlobalData(format);

	if (hg)
	{		
		char	*pData = (char*)GlobalLock(hg);
		int		nLen = (int)GlobalSize(hg);

		GlobalUnlock(hg);

		if (m_hNotifyWnd)
		{
			PostMessage(m_hNotifyWnd, WM_DROP_NOTIFY, (WPARAM)pData, nLen);
		}		
	}
	return TRUE;
}

// CWBDropTarget 消息处理程序
