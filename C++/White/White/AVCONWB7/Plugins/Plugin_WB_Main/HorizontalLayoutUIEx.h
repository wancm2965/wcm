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
	// ��ǰ���еĶ�����������Ϣ�Ѿ�תΪ�϶�����Ӧ�ðѸö���״̬��ԭ
	CControlUI*		m_pCurClick;	
};
