//WBResFunc.h(接口函数)
#ifndef _AVCON_WHITEBOARD_RESOURCE_WINDOWS_H_
#define _AVCON_WHITEBOARD_RESOURCE_WINDOWS_H_
#include "stdafx.h"

///必须和到处库的message ID值相同，否则接收不到消息
#define UWM_DRAGOUT_RESOUCE		(WM_USER + 3333) 
#define UWM_DELFILE_OPEN        (WM_USER + 5555)

namespace WBRESWND{
	/*******************************************************************
	函数名称：InitWBResModule
	函数功能：加载XML文件中保存的数据，初始化DLL资源句柄
	参数说明：无
	返回值  ：初始化成功，返回TRUE，否则返回FALSE
	备注    ：
	*******************************************************************/
	BOOL InitWBResModule();

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	void UnInitWBResModule();

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//创建触摸资源窗体
	HRESULT CreateGalleryWndEx(DWORD version, HINSTANCE hInstance, HWND hParent,
		int x, int y, int nWidth, int nHeight, BOOL bAuthorize);

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//移动资源窗体
	BOOL MoveGalleryWndEx( HWND hWndInsertAfter,
		int x, int y, int nWidth, int nHeight, UINT uFlags);

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//显示触摸资源窗口
	int ShowGalleryWndEx();
	//隐藏触摸资源窗口
	int HideGalleryWndEx();

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//打开资源窗口却换到特殊资源目录
	void SwitchSpecialDirectoryEx(LPCTSTR strPath);
	
	//设置资源窗口句柄
	void SetWBResHWndEx(HWND hParent);

	//更新资源窗口，WBResourceManager使用
	void UpdateResGalleryEx();

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//获得资源类型，WhiteBoardView使用
	int GetResTypeEx(CString url);

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//删除资源项,dll外部使用GetResWindow获得的指针直接加入会有错误，因为地址空间不一致， WBResourceManager使用
	void RemoveResPathEx( LPCTSTR strPath );

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//添加资源项，同上
	void AddResPathEx(LPCTSTR strPath);

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//触摸资源窗口是否已经创建
	BOOL IsGalleryWndCreated();

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//返回触摸资源窗口的显示状态（显示，不显示）
	BOOL IsGalleryWndShow();

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//关闭触摸资源窗体 
	void CloseResWnd();

	/*******************************************************************
	函数名称：
	函数功能：
	参数说明：
	返回值  ：
	备注    ：
	*******************************************************************/
	//获取当前触摸资源索引（用于保存配置文件）
	unsigned int GetCurSelectResIndex();
	void SetMTAuthorizeRes(bool bMTAuthorize);
    void SetAvconLock(BOOL bAvconLock);
    void SetAvconIniPath(LPCTSTR trAvconTempPath);
	bool RegisterReceivedWnd(const HWND &hWnd);
	bool UnRegisterReceivedWnd(const HWND &hWnd);
	//返回所接受的资源类型 20121121
	int GetResType(const std::string &_strExt);
	//void LoadResFromDirectoryWB(LPCTSTR DirectoryWB);
};
#endif