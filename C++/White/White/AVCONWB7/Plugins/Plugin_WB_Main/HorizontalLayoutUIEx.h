#pragma once

class COptionUIEx : public COptionUI
{
public:
	COptionUIEx(void);

public:
	LPVOID GetInterface(LPCTSTR pstrName);
	void DoEvent(TEventUI& event);
	bool Activate();
};

class CHorizontalLayoutUIEx
	: public CHorizontalLayoutUI
{
public:
	CHorizontalLayoutUIEx(void);

public:
	void DoEvent(TEventUI& event);
	void SetPos(RECT rc);
	void DoPaint(HDC hDC, const RECT& rcPaint);
	CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);

private:
	// 当前击中的对象，如果鼠标消息已经转为拖动，则应该把该对象状态还原
	CControlUI*		m_pCurClick;	
};
