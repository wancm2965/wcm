#pragma once
#include "WhiteBoardView.h"
#include "atltypes.h"
#include "afxwin.h"
#include "MTGestureImport.h"
#include "WBMTDrawObjectTally.h"


// CScreenDrawDlg 对话框
class CScreenDrawDlg : public CDialog, public CWBObject
{
	DECLARE_DYNAMIC(CScreenDrawDlg)

public:
	CScreenDrawDlg(CWnd* pParent);   // 标准构造函数
	CScreenDrawDlg(CRect& rcScreenDlg);
	virtual ~CScreenDrawDlg();
	virtual void OnFinalRelease();
	// 对话框数据
	enum { IDD = IDD_SCREENDRAWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
public:
	void ScreenClearTouchMap(DWORD dwLayerIndex);			///清除m_mapMTDrawPar的数据
	void SetMoveIsNeedSendData(BOOL MoveIsNeedSendData);    ///设置在move的时候需要刷新
	BOOL GetMoveIsNeedSendData();                           ///获取在move的时候需要刷新的判断
	CRect& GetScreenDrawDlgRect(){return m_rect;}			///获取矩形
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	bool m_LButtonDown;           //判断鼠标左键是否按下按下为true，松开为false
	afx_msg void OnDestroy(); //取消全屏时，隐式调用的函数
	afx_msg LRESULT OnDestroyScreen(WPARAM wParam, LPARAM lParam); //关闭屏幕绘图时，显示调用的销毁窗口函数
public:
	///add by aolingwen 2012.11.23
	///继承自AVCON_SDK_MTTouch::CWBMTObject
	//触摸按下
	virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
	//触摸移动
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
	//触摸弹起
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	///end

	//用于触摸画图计数控制add by huanglongquan 2012年12月18日 
	CWBMTDrawObjectTally m_MTDrawObjectTally;
private:
	CPoint m_ptPointLineCurve;//保存鼠标按下时候直线q曲线的点坐标
	///窗口矩形
	CRect m_rect;
	///画图锁
	XCritSec m_csDraw;
	//触摸处理中
	DWORD m_dwTimeGetTime; //记录上一次鼠标移动时发送同步消息的时间
	BOOL  m_bMoveIsNeedSendData;//判断在MouseMove时是否需要发送同步消息
	
private:
	//覆盖系统的，不做任何处理
	afx_msg LRESULT OnGesture(WPARAM /*wParam*/, LPARAM lParam);
	afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);

	//仅用于wm_touch消息
	std::map<DWORD, MTCursor*>	m_mapTouchCursor;
	//touch
private:
	//触摸绘制参数结构
	typedef struct touchDrawingParameter
	{
	private:
		DWORD dwID;//此数据可以不用
		int nDrawType;
		CWBObject *pObject;
		DWORD dwTime;
		CPoint ptValid;
		CPoint pt;
		void *pData;
		
	public:
		touchDrawingParameter(DWORD _dwID, int _nDrawType, CWBObject*_pObject, CPoint _ptValid=CPoint(0,0), DWORD _dwTime=0, CPoint _pt=CPoint(0,0))
			: dwID(_dwID), pObject(_pObject), dwTime(_dwTime), ptValid(_ptValid), nDrawType(_nDrawType), pt(_pt){}
		void UpdateValidPt(CPoint _ptValid){ptValid=_ptValid;};
		void UpdatePt(CPoint _pt){pt=_pt;};
		void UpdateTime(DWORD _dwtime){dwTime=_dwtime;};
		void SetAppendData(void *_pData){pData=_pData;};
		int GetType(){return nDrawType;};
		CWBObject *GetWBObject(){return pObject;};
		DWORD GetTime(){return dwTime;};
		CPoint GetPt(){return pt;};
		CPoint GetValidPt(){return ptValid;};
		void *GetAppendData(){return pData;};
	}MTDrawPar, *PMTDrawPar;
	std::map<DWORD, MTDrawPar*> m_mapMTDrawPar;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};