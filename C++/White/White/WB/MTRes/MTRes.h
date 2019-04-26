// MTRes.cpp : 定义 DLL 应用程序的导出函数。
//
#include "ResGallery.h"

//设置资源拖拽的目标窗体
extern "C" LIB_AVCON_RESOURCE_DECL bool RegisterReceivedWnd(const HWND &hWnd);
extern "C" LIB_AVCON_RESOURCE_DECL bool UnRegisterReceivedWnd(const HWND &hWnd);

//创建触摸资源窗体
extern "C" LIB_AVCON_RESOURCE_DECL HRESULT CreateGalleryWnd(DWORD version, HINSTANCE hInstance, HWND hParent,
	int x, int y, int nWidth, int nHeight, ResTypeCallbackFun pResCallback = NULL, BOOL bAuthorize = FALSE);

//移动资源窗体
extern "C" LIB_AVCON_RESOURCE_DECL BOOL MoveGalleryWnd( HWND hWndInsertAfter,
	int x, int y, int nWidth, int nHeight, UINT uFlags);

//显示触摸资源窗口
extern "C" LIB_AVCON_RESOURCE_DECL int ShowGalleryWnd();
//隐藏触摸资源窗口
extern "C" LIB_AVCON_RESOURCE_DECL int HideGalleryWnd();

//打开资源窗口却换到特殊资源目录
extern "C" LIB_AVCON_RESOURCE_DECL void SwitchSpecialDirectory(LPCTSTR strPath);

//设置资源窗口句柄
extern "C" LIB_AVCON_RESOURCE_DECL void SetWBResHWnd(HWND hParent);

//触摸资源窗口是否已经创建
extern "C" LIB_AVCON_RESOURCE_DECL BOOL IsGalleryWndCreated();

//返回触摸资源窗口的显示状态（显示，不显示）
extern "C" LIB_AVCON_RESOURCE_DECL BOOL IsGalleryWndShow();

//更新资源窗口，WBResourceManager使用
extern "C" LIB_AVCON_RESOURCE_DECL void UpdateResGallery();

//关闭资源窗口
extern "C" LIB_AVCON_RESOURCE_DECL void CloseResGallery();

//获取当前触摸资源索引（用于保存配置文件）
extern "C" LIB_AVCON_RESOURCE_DECL unsigned int GetCurSelectResIndex();

//设置授权，得到授权系统
extern "C" LIB_AVCON_RESOURCE_DECL void SetMTAuthorizeRes(BOOL MT);

extern "C" LIB_AVCON_RESOURCE_DECL void SetAvconLock(BOOL bAvconLock);
extern "C" LIB_AVCON_RESOURCE_DECL void SetAvconIniPath(const LPCTSTR strAvconIniPath);
//extern "C" LIB_AVCON_RESOURCE_DECL void LoadResFromDirectoryWB(const LPCTSTR trAvconTempPath);

extern CWBResGallery *g_pResGallery;
extern BOOL g_bAvconLock;