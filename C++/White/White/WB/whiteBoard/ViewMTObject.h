#ifndef _AVCON_WHITEBOARD_VIEWMTOBJECT_H_
#define _AVCON_WHITEBOARD_VIEWMTOBJECT_H_
#include "WBMTObject.h"
#include <MTProtocolType.h>


/*------------------------------------------------------------------------
具体用途：专一处理带边框的触摸对象的动态平移操作
------------------------------------------------------------------------*/
typedef struct SObjectCursor
{
	CWBMTObject *pObject;
	AVCON_SDK_MTTouch::MTCursor *pCursor;
	struct SObjectCursor(CWBMTObject *_pObject, 
		AVCON_SDK_MTTouch::MTCursor *_pCursor)
		: pObject(_pObject), pCursor(_pCursor){}
	struct SObjectCursor(struct SObjectCursor &cpy){
		pObject = cpy.pObject; pCursor = cpy.pCursor;}
}SObjectCursor;


class CWBMTObjectFramePan : public CWBMTObject
{
public:
	CWBMTObjectFramePan(HWND hWnd);
	~CWBMTObjectFramePan();
public:
	void PreAddMTCursor(MTCursor *tcur, CWBMTObject *pObject);
	void removeMTCursor(MTCursor *tcur);
	void updateMTCursor(MTCursor *tcur);
	//
	void ClearAllMTDrawMap();
	bool ClearMTDrawMap(const DWORD &dwTouchID);
private:
	//触摸弹起
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	//触摸移动
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar);

private:
	//弹起子对象上的点，加入到整体对象处理中。
	void PopChildCursorAndPush2Frame();
private:
	///中转view中在move时的SendMoveSelectedObjOffset
	void SendMoveObjOffset(CPoint cpoffset,unsigned long ulEventId);
private:
	//相关联的窗口句柄
	HWND m_hViewWnd;
	//ID到触摸对象的的映射
	std::map<unsigned long, SObjectCursor> *m_pMapSelectObj;
	//是否集中处理
	bool m_bPanTogether;
	CPoint m_ptOffset;		//移动时记录偏移量
};
/*------------------------------------------------------------------------
具体用途：专一处理白板对象上图片,控制和播放器按钮的事件
------------------------------------------------------------------------*/
class CWBMTObjectButtonManager : public CWBMTObject
{
public:
	CWBMTObjectButtonManager(HWND hWnd);
	~CWBMTObjectButtonManager();

public:
	void SetAssociatedObject(CMTObject *pObject, const int &nButtonState);

private:
	//触摸按下
	virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
	//触摸移动
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
	//触摸弹起
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);

private:
	DWORD m_dwTimeGetTime; //记录上一次鼠标移动时发送同步消息的时间
	//相关联的触摸对象
	CMTObject * m_pAssociatedObject;
	//相关联的窗口句柄
	HWND m_hViewWnd;
	///所击中按钮类型(ImageButton, CtrlButton or IMediaCtrl),BUTTON_TYPE_IMAGEBUTTON为ImageButton,
	///BUTTON_TYPE_CTRLBUTTON为CtrlButton, BUTTON_TYPE_INMEDIACTRL为MediaCtrlButton,
	///BUTTON_TYPE_NONE为既不是ImageButton也不是CtrlButton
	int m_nButtonState;
};
/*------------------------------------------------------------------------
具体用途：所有白板的触摸数据的分发管理
------------------------------------------------------------------------*/
class CWBMTObjectManager : public CWBMTObject
{
protected:
	CWBMTObjectManager(HWND hViewWnd, CWBMTObject * pViewMTObject);
	~CWBMTObjectManager();
	//////////////////////////////////////////////////////////////////////////
	///单例模式
public:
	static CWBMTObjectManager *GetIns_();
	//初始化
	static HRESULT Init(HWND hViewWnd, CWBMTObject * pViewMTObject);
	//消亡
	void Destroy();

public:
	//保持同步[白板删除对象的同时，需要移除列表中的对象,如果存在的话]
	bool RemoveMTObject(const CMTObject *pObject);
	bool RemoveMTObject(const DWORD &dwObjectIndex);
	//同上，如页操作时：删除页，切换页等。
	void RemoveAllMTObject();

private:
	///唯一单例
	static CWBMTObjectManager *m_instance;
	//获取触摸数据的协议类型
	static EProtocolType  m_gType;
	//////////////////////////////////////////////////////////////////////////
	///继承与AVCON_SDK_MTTouch::CMTObject接口

public:
	//operation  for cursor updating message
	//触摸库中通过此函数来响应按下消息，从而调用CWhiteBoardView中的touchDownHandler函数
	virtual void addMTCursor(MTCursor *tcur);
	//触摸库中通过此函数来响应弹起消息，从而调用CWhiteBoardView中的touchUpHandler函数
	virtual void removeMTCursor(MTCursor *tcur);
	//触摸库中通过此函数来响应移动消息，从而调用CWhiteBoardView中的touchMoveHandler函数
	virtual void updateMTCursor(MTCursor *tcur);
	//设置触摸授权
	BOOL SetMTAuthorize(BOOL);
	BOOL GetMTAuthorizeState() {return m_bMTAuthorize;}//获取当前对象触摸授权状态
private:
	DWORD m_nFirstID;
	DWORD m_nEditTriangleTouchID;
	BOOL m_bMTUserCursor;	
	BOOL m_bIsTouchDustbin;
	//触摸授权状态
	BOOL m_bMTAuthorize;
	//触摸按下
	virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
	//触摸移动
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
	//触摸弹起
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);

	virtual bool  BInRegion(const POINT &pt)const;
	/*---------------------------------------------------------------------------------------------
	参数一：低字节表示手势的大类[ETouchGestureType]，高字节表示手势的小类[如动态手势中的]
	参数二: 低字节表示x的坐标，高字节表示y方向的坐标. 单位为像素；坐标为屏幕坐标
	----------------------------------------------------------------------------------------------*/
	virtual void OnCallbackGesture(const unsigned int &nMessage, 
		const unsigned int &nAppendMessage = 0);

	///内部操作
private:
	bool AddObjectMap(const unsigned long &dID, CWBMTObject *pObject);
	bool RemoveObjectMap(const unsigned long &dID);

private:
	//
	XCritSec m_csMTObjectOperation;
	//当前正在操作对象的列表
	std::map<unsigned long, CWBMTObject*> *m_pObjectMap;

	//真实触摸对象view的指针
	#ifdef SUPPORTMTGES
	CWBMTObject*m_pViewMTObject;
#else
	CMTObject * m_pViewMTObject;
#endif
	
	//真实触摸对象view的窗口句柄
	HWND m_hViewWnd;

	//两个虚拟的与窗口操作有关的触摸对象
	CWBMTObjectFramePan *m_pFrameMoveObject;
	CWBMTObjectButtonManager *m_pClickObject;

	///框选的矩形
	CRect m_rectSelect;
	///垃圾箱被移动前的坐标点
	CPoint m_ptPreMoveDustbin;
};

#endif
