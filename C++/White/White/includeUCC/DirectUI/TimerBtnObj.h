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
	//���ö�ʱ����
	virtual void SetTimerElapse(UINT uElapse){ m_uElapse = uElapse; }
	virtual void StartTimer();
	virtual void StopTimer();
	virtual void SetAutoStop(BOOL bAutoStop, UINT uElapse);

//��Ϣ
	virtual LRESULT OnTimer(UINT nIDEvent);

protected:
	//�����ڰ�ť�Լ�����ı�������ɫ�Ȳ�����
	UINT m_uElapse;
	UINT m_uAutoStopElapse; //�Զ�ֹͣ��ʱ����
	BOOL m_bAutoStop; //�Ƿ��Զ�ֹͣ
	int m_nIDTimer; //
	int m_nStopID; //�Զ�ֹͣ��ʱ��ID
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
	
	//��Ϣ
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

	//����
public:
	virtual void	SetPosition(const ObjPosition& ObjPos);
	
public:
	virtual void SetTimerElapse(UINT uElapse);
	virtual void StartTimer();
	virtual void StopTimer();
	
	//���ö�������
public:
	void SetAnimateObj(CAnimateObj* pAniObj);
	BOOL HasAnimateObj();
	CAnimateObj* GetAnimateObj();
	const CAnimateObj* GetAnimateObj() const;

	//��Ϣ
public:
	virtual LRESULT OnTimer( UINT uIDEvent );
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);

private:
	CAnimateObj* m_pAniObj;
};
*/