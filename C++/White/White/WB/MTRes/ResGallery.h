#pragma once

#include <list>
#include <map>
#include "MTFolder.h"
#include "Thumbnail.h"
#include "D2DDriver.h"
#include "ResData.h"
#include "ClickTarget.h"
#include "XAutoLock.h"
#include "ResManage.h"
#include "ShellDef.h"
#include "LanguageStr.h"
#define BOTTOM_PHOTOSTRIP_DIRECTORY     TEXT("D:\\WBTemp_\\")
#define RESGALLERY_CLASS_NAME TEXT("ThumbGalleryWindowClass")

#define INTERNAL_Y_MARGIN_DOWN_PERCENTAGE	(0.2f) //竖直方向的下边距
#define INTERNAL_Y_MARGIN_UP_PERCENTAGE		(0.2f) //竖直方向的上边距
#define INTERNAL_Y_MARGIN_RATIO (0.05f) // 
#define SIDE_MARGIN_PERCENTAGE (0.3f) //惯性操作时，平移到尽头的边界设定

#define WB_TRG_WINDOW_HEIGHT	(180)//触摸资源库的窗体高度(TRG==TOUCH_RESOURCE_GALLERY)
#define WB_TRG_THUMB_INTERNAL_MARGIN	(30)//触摸资源库内缩略图间的间隔（仅一排，及水平间隔）

#define WB_SINGLETIP_WIDTH 150
#define WB_SINGLETIP_HEIGHT 100
#define WB_CHANGE_LIST_WIDTH 150
#define WB_CHANGE_LIST_HEIGHT 85

#define  PATH_RESOURCE_EXCHANGE_PICTURE TEXT("Skin\\WhiteBoard\\Image\\ResourceMenu.png")//资源图片的相对路径

#define MOUSEEVENTF_FROMTOUCH 0xFF515700

#define	LEFT_SLIDE	    0x0001
#define UP_DRAG		    0x0002
#define RIGHT_SLIDE     0x0004
#define CHANGE_LIST     0x0008
#define UP_DEL			0x0010
#define ADD_TIP         0x0020

enum PANEL {
	RESSHOW = 0,//显示一个资源项内容
	RESLIST = 1,//以文件夹的形式显示资源列表
	RESLISTEDIT = 2,//编辑文件夹形式的资源列表
	RESCLASSLIST = 3//以资源类型显示资源列表
};

//触摸点击（弹出菜单）处的区域宽度大小【有效范围】
#define  CLICK_ZONE_WIDTH	(60)
#define  CLICK_ZONE_HEIGHT	(CLICK_ZONE_WIDTH)

typedef int (__cdecl *ResTypeCallbackFun)(const std::string &_strExt);
typedef std::list<CWBThumbnail*> LWBThumbnail, *PLWBThumbnail;
class CResChangeWnd;
class CWBResGallery : public AVCON_SDK_MTTouch::CMTObject
{
public:
	//创建窗口及关联类
	static HRESULT CreateThumbGallery(HINSTANCE hInstance, HWND hParent,
		int x, int y, int nWidth, int nHeight, CWBResGallery **ppStrip,
		ResTypeCallbackFun pResCallback, BOOL bAuthorize = FALSE);

	//获取窗口句柄
	const HWND GetHWND()const;
	HWND m_hWnd;
	//白板窗口句柄
	HWND m_hWBViewHwnd;
	//获取提示窗口句柄
	HWND GetHWndToolTip(){return m_hWndToolTip;};

	virtual void CleanUp(){delete this;};

	//获取当前资源目录
	LPCTSTR GetCurResDirectory();
	//获取当前资源条目索引
	unsigned int GetCurResIndex() const;
	//获得
	CD2DDriver *GetD2DDriver(){return m_d2dDriver;}
	//重置所有缩略图的水平偏移数据
	void ResetXPosition();
	//重置所有缩略图的竖直偏移数据
	void ResetYPosition();
	//更新资源窗体位置
	void UpdateResSize(unsigned int nNewWidth, unsigned int nNewHieght);
	//获取资源窗体位置
	void GetResSize(unsigned int &nNewWidth, unsigned int &nNewHieght);
	//有判断的强制更新（如：从资源库管理界面删除了单个资源，需要检测是否删除了当前触摸资源库中的当前资源，如是，则清空）
	void UpdateResource();
	//设置目录，供外界调用（初始化，切换等）
	BOOL LoadResFromDirectory(LPCTSTR lpszDirectory);
	//设置触摸授权
	BOOL SetMTAuthorize(BOOL);
	//配置选择文件插件信息
	void ConfigFileSelectPlug(MTFOLDERCONFIG &pconfig,wchar_t *,wchar_t *,wchar_t*);
	//初始化语言配置路径
	void InitLanPathNode(tstring);
	DWORD GetDrawTipFlag()const;
	void ConfigDrawTipFlag(DWORD nflag);
	void DarwTipBackground();
	
	PANEL AdjustState();

	void SetWBResHWnd(HWND hParent){m_hWBViewHwnd = hParent;};

	//画视频照相图片删除时在面板显示垃圾箱图标
	void DrawRubbishIcon();
	void SetIsDrawRubbishIcon(BOOL bdraw){m_bisDrawRubbishIcon=bdraw;};
	BOOL GetIsDrawRubbishIcon(){return m_bisDrawRubbishIcon;};
	void SetResChange(BOOL bchange){m_bResChange = bchange;};
	void SwitchVideoPictureDirectory(LPCTSTR strPath);   //切换视频拍照路径
	void InitSpecialResPath();    //初始化特殊资源路径
private:
//	LRESULT OnTouch(WPARAM wParam, LPARAM lParam);
	//仅用于wm_touch消息
	std::map<DWORD, MTCursor*>	m_mapTouchCursor;
	//提示信息
	LanguageStr m_lStr;
	tstring m_lanPath;
	void RenderTip();
	HINSTANCE m_ResHinstance;
	//触摸授权状态
	BOOL m_bMTAuthorize;
	//第一次创建时初始化资源数据
	void InitialResData();

	//获取对象链表指针
	LWBThumbnail *GetLstAddress(){return m_lThumbnail;}

	//创建触摸点击对象
	void CreateSpecialClickObject();
	//创建并显示Tip信息
	TOOLINFO m_ToolTip;
	HWND m_hWndToolTip;
	void CreateTipWnd(HWND hWnd,TOOLINFO toolTip);
	

private:
	//operation  for cursor updating message
	virtual void addMTCursor(MTCursor *tcur);
	virtual void removeMTCursor(MTCursor *tcur);
	virtual void updateMTCursor(MTCursor *tcur);

	///内部操作
private:
	bool AddCurorMap(const unsigned long &dID, CMTObject *pObject);
	bool RemoveCurorMap(const unsigned long &dID);

	XCritSec m_csMapChildObject;
	XCritSec m_csListChildThumbnail;
	//当前正在操作对象的列表
	std::map<unsigned long, CMTObject*> *m_pMapChildObject;

private:
	virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar);
	virtual bool BInRegion(const POINT &pt)const;

private:
	//绘制
	void Render(BOOL bRenderBackgroundOnly = FALSE);

private:
	//不和其它的窗口并行处理，则暂没有用
    //BOOL InRegion(const float &x, const float &y)const;
	//处理加载缩略图并转化为D2D图像的线程函数
	static UINT CWBResGallery::AfxThreadProc( LPVOID pParam );

private:
	//私有的构造和析构函数
    CWBResGallery(HWND hWnd, int nWidth, int nHeight, int nXPos, 
		int nYPos, ResTypeCallbackFun pResCallback);
    ~CWBResGallery();
	//清除
    void FreePhotos();
	//主要是一些com对象的初始化
    HRESULT Initialize();
	//读取图片
	HRESULT LoadPhoto(LPCTSTR const uri);
    
	//切换选择标签
	void ChangeResTab(unsigned nResItemIndex);
 
	//标准的窗口消息处理函数
    static LRESULT CALLBACK S_WndProc(HWND, UINT, WPARAM, LPARAM);

	ResTypeCallbackFun BSupportedResType;

	HDEVNOTIFY m_hDevNotify;//驱动器监视注册句柄

private:
	//宽度，高度，水平和竖直方向的偏移量
    int m_nWidth, m_nHeight, m_nXPos, m_nYPos;
    float m_fpXOffset;
	CD2DDriver *m_d2dDriver;
	//TouchDispatch *m_pDispatch;
    std::list<CWBThumbnail*> *m_lThumbnail;
	//资源模式与目录模式
	PANEL m_panel;
	BOOL m_bResChange;

	//点击弹出菜单的触摸对象
	CWBTouchClick *m_mtSpecialClick;
	//当前资源的路径
	tstring m_strCurDir;
	//资源数据（指向外面的资源管理对象）
	PResMapArray m_pLstResData;
	//当前显示的资源序列号（0标识为空，1表示第一个, 2, ...）
	int m_nCurResIndex;
	//当前资源名
	tstring m_strCurName;

	//触摸点，鼠标down是否按下
	BOOL m_fIsMouseDown;

	//切换资源的bitmap
	ID2D1Bitmap *m_pResChangeBitmap;
	Bitmap* m_pResChangeGDIBmp;

	HRESULT TranslateXP(int xOffset = 0 , int yOffset = 0);

	static int GetResType(const std::string &_strExt);

	unsigned int m_nDrawTipFalg;     //画提示信息的状态
	unsigned int m_nDrawIntermediateFalg;     //画提示信息的中间变量状态

	BOOL m_bisDrawRubbishIcon;   //是否需要画删除垃圾图标
	tstring m_strVideoPicturePath;   //特殊资源路径
	BOOL m_bisSpecialResource;   //是否是特殊资源
	float m_fResTotalLen;        //目录资源的总长度;

///拖拽接收窗体
public:
	bool RegisterReceivedWnd(const HWND &hWnd);
	bool UnRegisterReceivedWnd(const HWND &hWnd);
	bool BInReceiveDragWndList(const HWND &hWnd);
	HMODULE m_hFolder;
///提示对话框
public:
	void SetThumbnail(CWBThumbnail *thumbnail){m_pThumbnail = thumbnail;};
	HMODULE m_hDialog;
	CWBThumbnail *m_pThumbnail;
	BOOL m_bDrapFlag;    //拖拽的状态标志
private:
	std::list<HWND> m_lstReceiveDragWnd;
};

void DisableTheTabFeedback(HWND hWnd);
