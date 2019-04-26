#if !defined(UIFORM_H_INCLUDED_)
#define UIFORM_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Objects.h"

class CDirectUI;

class DIRECTUI_API CUIForm : public CObjectBase
{
	friend CObjectBase;
public:
	CUIForm(HWND hWndOwner = NULL);
	virtual ~CUIForm();
	
	CDirectUI*		m_pDirectUI;
	
	virtual void SetVisible(BOOL bVisible = TRUE,BOOL bRefresh = TRUE);
	void	ShowObjects(DEQUE_OBJECT& dequeObjects,BOOL bShow);
	BOOL	GetSavedVisible(){return m_bSaveVisible;}
	void	SetSavedVisible(BOOL bVisible){m_bSaveVisible = bVisible;}
	void	Attach(CDirectUI* pDirectUI);
	void	Detach();
	CObjectBase*	GetObjectBase(UINT uResID);
	const CObjectBase*	GetObjectBase(UINT uResID) const;
	BOOL	HasObjectBase(UINT uResID);
	void	DeleteObjectBase(UINT uResID);
	virtual void	ResizeAllObjects(CRect rect);
	void	DeleteAllObject();
	BOOL	RemoveObject(CObjectBase* pObjBase);
	BOOL	SetOwnerMsgRecvHwnd(HWND hWndOwner,HWND hWndMsgRecv);
	LRESULT OnDUICommand(WPARAM wParam,LPARAM lParam);
	CObjectBase*  GetObjectByID(int nID);
	CObjectBase*  GetObjectFromPoint(const CPoint& point);
	BOOL	 HasSubForm(int nID) const;
	CUIForm* GetSubForm(int nID);
	const CUIForm* GetSubForm(int nID) const;
	LRESULT OnTimer(UINT nIDEvent);
	
	void	EraseObject(CObjectBase* pObjBase);
	void	PushBackObject(CObjectBase* pObjBase);
	
	//�����Ӷ��󽹵�
	void SetNextObjFocus(BOOL bReverse = FALSE);
	CObjectBase* GetHoldFocusObj();
	CObjectBase* GetNextFocusObj(CObjectBase* pCurFocusObj, BOOL bReverse = FALSE);
	
	virtual void DrawFrame();

	//�õ��ȼ���Ӧ�Ķ���
	CObjectBase* GetHotKeyObj(BOOL bAtlKey, BOOL bShiftKey, BOOL bCtrlKey, UINT uHotKeyCode);
	
public:
	virtual void OnDirectUICommand(WPARAM wParam,LPARAM lParam){};
	virtual BOOL CreateFormObj(CDirectUI* pDirectUI);	//���ô˺���ǰ��һ������±����ȵ���Attach
	virtual void ShowSubForms(BOOL bShow){};
	virtual void OnOK();
	virtual void RecalcPosition();
	virtual BOOL DrawItem(HDC hDC,CPoint ptOffset = CPoint(0,0));
	virtual BOOL ShowRealWindow(BOOL bShow);
	//���������Ӷ���CObjectBase(�����)�Լ�����Ϣ
	virtual LRESULT OnObjOwnMsg(CObjectBase* pObj, UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bTurnDefProc){ bTurnDefProc = TRUE; return 0; }
	
	virtual LRESULT OnKeyDown(int nVirtKey,long lKeyData);
	virtual LRESULT OnMouseMove(UINT nHitTest, CPoint point);
	virtual LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnLButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnLButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnRButtonDown(UINT nHitTest, CPoint point);
	virtual LRESULT OnRButtonUp(UINT nHitTest, CPoint point);
	virtual LRESULT OnActivate(UINT nState, HWND hWndOther, BOOL bMinimized);
	virtual BOOL    OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	virtual LRESULT OnKillFocus(WPARAM wParam,LPARAM lParam);
	virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam){ return FALSE; }
	virtual BOOL	OnClose(){return FALSE;}
	virtual void	OnShowWindow(WPARAM wParam, LPARAM lParam){};
	//Tab����
private:
	void AddTabOrderObj(CObjectBase* pObjBase);
	void EraseTabOrderObj(CObjectBase* pObjBase);
	BOOL IsObjInTabOrderContainer(CObjectBase* pObjBase);

	//�˶�ĳ�������ǲ��Ƕ�Ӧ���ȼ�����
private:
	BOOL CheckHotKeyObj(CObjectBase* pObjBase, BOOL bAtlKey, BOOL bShiftKey, BOOL bCtrlKey, UINT uHotKeyCode);

private: 
	void KillOldFocus();

protected:
	BOOL			m_bSaveVisible;
private:
	DEQUE_OBJECT	m_dequeForm;
	DEQUE_OBJECT    m_dequeObjTabOrder;
	CObjectBase*	m_pOldFocusObj; //ǰ1���������
};

#endif //UIFORM_H_INCLUDED_
