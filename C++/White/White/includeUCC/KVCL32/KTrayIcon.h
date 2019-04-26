#pragma once

class CTrayNotifyIcon;

//---------------------------------------------------------------------------------------
// 用于处理IE4任务栏通知消息
class AFX_EXT_CLASS CTrayRessurectionWnd : public CFrameWnd
{
public: 
	CTrayRessurectionWnd(CTrayNotifyIcon* pTrayIcon);

protected:
	CTrayNotifyIcon* m_pTrayIcon;
	afx_msg LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CTrayRessurectionWnd)
};

//---------------------------------------------------------------------------------------
// 用于实现动画托盘图标
class AFX_EXT_CLASS CTrayTimerWnd : public CFrameWnd
{
public: 
	CTrayTimerWnd(CTrayNotifyIcon* pTrayIcon, HICON* phIcons, int nNumIcons, DWORD dwDelay);
	~CTrayTimerWnd();

	void Play();
	void Stop();

	// 获得当前ICON
	HICON GetCurrentIcon() const { return m_phIcons[m_nCurrentIconIndex]; };

protected:
	CTrayNotifyIcon* m_pTrayIcon;
	HICON*           m_phIcons;
	int              m_nNumIcons;
	DWORD            m_dwDelay;
	UINT             m_nTimerID;
	int              m_nCurrentIconIndex;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CTrayTimerWnd)
};

//---------------------------------------------------------------------------------------
// CTrayNotifyIcon 类定义
class AFX_EXT_CLASS CTrayNotifyIcon : public CObject
{
public:
	CTrayNotifyIcon();
	~CTrayNotifyIcon();

	BOOL Create(CWnd* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON hIcon, UINT nNotifyMessage);                  
	BOOL Create(CWnd* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON* phIcons, int nNumIcons, DWORD dwDelay, UINT nNotifyMessage);

	BOOL    SetTooltipText(LPCTSTR pszTooltipText);
	BOOL    SetTooltipText(UINT nID);
	CString GetTooltipText() const;

	BOOL  SetIcon(HICON hIcon);
	BOOL  SetIcon(LPCTSTR lpIconName);
	BOOL  SetIcon(UINT nIDResource);
	BOOL  SetIcon(HICON* phIcons, int nNumIcons, DWORD dwDelay);
	BOOL  SetStandardIcon(LPCTSTR lpIconName);
	BOOL  SetStandardIcon(UINT nIDResource);
	HICON GetIcon() const;
	HICON GetIconStock() const {return m_hIcon;}
	BOOL  UsingAnimatedIcon() const { return m_bAnimated; };

	BOOL  SetNotificationWnd(CWnd* pNotifyWnd);
	CWnd* GetNotificationWnd() const;

	void  HideIcon();
	void  ShowIcon();
	void  RemoveIcon();
	void  MoveToExtremeRight();

	virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

	BOOL IsShowing() const { return !IsHidden(); };
	BOOL IsHidden() const { return m_bHidden; };

	void Play();
	void Stop();

	BOOL ShowBalloonTip(LPCTSTR szMsg, LPCTSTR szTitle,UINT uTimeout, DWORD dwInfoFlags=1);

protected:
	BOOL CreateTimerWindow(HICON* phIcons, int nNumIcons, DWORD dwDelay);
	void DestroyTimerWindow();
	BOOL CreateRessurectionWindow();
	void DestroyResurrectionWindow();

	NOTIFYICONDATA        m_NotifyIconData;
	BOOL                  m_bCreated;
	BOOL                  m_bHidden;
	CWnd*                 m_pNotificationWnd;
	CTrayTimerWnd*        m_pTimerWnd;
	CTrayRessurectionWnd* m_pResurrectionWnd;
	BOOL                  m_bAnimated;

	HICON				  m_hIcon;

	DECLARE_DYNAMIC(CTrayNotifyIcon)

	friend class CTrayTimerWnd;
};
