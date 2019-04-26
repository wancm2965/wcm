#pragma once
#include "buttonobj.h"
#include "Objects.h"

class DIRECTUI_API CTimerBtnObj :
	public CButtonObj
{
public:
	CTimerBtnObj(HWND hwndOwner = NULL);
	~CTimerBtnObj(void);

public:
	//设置定时功能
	virtual void SetTimerElapse(UINT uElapse){ m_uElapse = uElapse; }
	virtual void StartTimer();
	virtual void StopTimer();
	virtual void SetAutoStop(BOOL bAutoStop, UINT uElapse);

//消息
	virtual LRESULT OnTimer(UINT nIDEvent);

protected:
	//（用于按钮自己间隔改变文字颜色等操作）
	UINT m_uElapse;
	UINT m_uAutoStopElapse; //自动停止的时间间隔
	BOOL m_bAutoStop; //是否自动停止
	int m_nIDTimer; //
	int m_nStopID; //自动停止的时间ID
	BOOL m_bTimerIsRuning;

}; 

//////////////////////////////////////////////////////////////////////////
//class CBtnFlashText
class DIRECTUI_API CBtnFlashText : public CTimerBtnObj
{
public:
	CBtnFlashText(HWND hwndOwner = NULL);
	~CBtnFlashText();

public:
	void AddTextFlashClr(COLORREF clrText);
	
	//消息
public:
	virtual LRESULT OnTimer(UINT nIDEvent);	
//	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	
private:
	std::deque<COLORREF> m_dequeTextClr;
};

/*
//////////////////////////////////////////////////////////////////////////
//class CBtnAniObj
class DIRECTUI_API CBtnAniObj : public CTimerBtnObj
{
public:
	CBtnAniObj(HWND hwndOwner = NULL);
	~CBtnAniObj();

	//属性
public:
	virtual void	SetPosition(const ObjPosition& ObjPos);
	
public:
	virtual void SetTimerElapse(UINT uElapse);
	virtual void StartTimer();
	virtual void StopTimer();
	
	//设置动画对象
public:
	void SetAnimateObj(CAnimateObj* pAniObj);
	BOOL HasAnimateObj();
	CAnimateObj* GetAnimateObj();
	const CAnimateObj* GetAnimateObj() const;

	//消息
public:
	virtual LRESULT OnTimer( UINT uIDEvent );
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);

private:
	CAnimateObj* m_pAniObj;
};
*/