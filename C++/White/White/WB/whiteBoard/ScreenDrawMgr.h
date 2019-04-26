#ifndef WHITEBOARD_SCREENDRAWMGR
#define WHITEBOARD_SCREENDRAWMGR

#include "ScreenDrawDlg.h"
#include "MouseDlg.h"
#include "AlphaDlg.h"

///单例屏幕绘制管理类
class CScreenDrawMgr : public CWBObject
{
public:
	friend class CScreenDrawDlg;
	friend class CMouseDlg;
	friend class CAlphaDlg;

	CScreenDrawMgr();
	~CScreenDrawMgr();

	///使用方法,在需要屏幕绘制时先调用CreateIns在调用Init。在需要关闭屏幕绘制时调用DestroyIns()即可，GetIns用来获得该类对象的指针

	static CScreenDrawMgr*	GetIns();											///获得实例
	static bool				CreateIns();										///创建实例
	static void				DestroyIns();										///销毁实例
	void					Init(CRect& rect);							///用来创建三层窗口

	///因为在CWBMTObjectManager管理触摸点的按下，更新，弹起时要用到CWBMTObject的子类指针，所以重写这三个函数

	virtual void			addMTCursor(MTCursor *tcur);						///用将触摸数据转给ScreenDrawDlg处理
	virtual void			removeMTCursor(MTCursor *tcur);						///用将触摸数据转给ScreenDrawDlg处理
	virtual void			updateMTCursor(MTCursor *tcur);						///用将触摸数据转给ScreenDrawDlg处理


	bool					PtInScreenDrawDlg(const POINT &pt)const;			///区域判断，被CWBMTObjectManager::BinRegin调用
	void					Invalidate();										///屏幕绘制对话框重绘,代替原来的g_pScreenDrawDlg->Invalidate();
	void					UpdateWindow();										///更新屏幕绘制对话框,代替原来的g_pScreenDrawDlg->UpdateWindow();
	bool					IsTransparent();									///是否透明,代替原来的g_pMouseDlg->m_isTransparentOrNot;
	bool					IsFullScreen();										///是否全屏,代替原来的g_pMouseDlg->m_isFullscreen;
	void					SetIsMinSize(BOOL bIsMinSize);						///最小化
	void					SetRichEditParent(CWnd **parent);					///设置文本框的父窗口
	void					SetMouseDlgWindowPos();								///将MouseDlg放到ScreenDrawDlg的下面
	void					ClearTouchDrawMap(DWORD dwLayerIndex);				///清除ScreenDrawDlg中的DrawMap,代替原来的g_pScreenDrawDlg->SendMessage(WM_SCREENCLEARMAP);
	void					GetMouseDlgScreenPosition(CPoint& pt);				///将在MouseDlg上的点客户坐标转换为屏幕坐标,相当于g_pMouseDlg->ClientToScreen(&pt);
	void					GetScreenDrawDlgClientPosition(CPoint& pt);			///将在ScreenDrawDlg上的点从屏幕坐标转换成客户坐标,相当于g_pScreenDrawDlg->ScreenToClient(&pt)

	///以下全为中转函数
	void					GuidesShift(BOOL isTransparent);					///效果切换
	void					DestroyScreenWindow();								///销毁窗口
	void					FullScreenShift(bool isFullScreen);					///全屏
	void					AlphaChange(const int alpha);						///改变透明度
	void					SetMoveIsNeedSendData( BOOL MoveIsNeedSendData );	///代替原来的g_pScreenDrawDlg->SetMoveIsNeedSendData(MoveIsNeedSendData);
	BOOL					GetMoveIsNeedSendData();							///代替原来的g_pScreenDrawDlg->GetMoveIsNeedSendData();
		
	CScreenDrawDlg* getScreenDrawDlg() {return m_pScreenDrawDlg;}
	void                    SetInvalidateway(bool buse){m_bChangeInvalidateway = buse;}
private:
	bool                   m_bChangeInvalidateway;                              ///设置刷新的不同方式
	static CScreenDrawMgr	*s_pScreenDrawMgrIns;								///单例指针
	CMouseDlg				*m_pMouseDlg;										///MouseDlg指针,用于放在ScreenDrawDlg下面来接收鼠标消息,因为窗口完全透明的部分无法接收鼠标消息
	CAlphaDlg				*m_pAlphaDlg;										///AlphaDlg指针,用于调节窗口透明度
	CScreenDrawDlg			*m_pScreenDrawDlg;									///ScreenDrawDlg指针,透明窗口，用来显示在窗口上绘制的图形
};

#endif