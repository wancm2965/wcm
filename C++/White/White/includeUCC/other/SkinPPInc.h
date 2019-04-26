//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the Skin++ Library
// Copyright (C) 2002-2006 YongJinSoft Ltd.(http://www.uipower.com)
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************

#ifndef __SKINPPINC_H
#define __SKINPPINC_H

#if _MSC_VER >= 1000
#pragma once 
#endif // _MSC_VER >= 1000

#pragma pack(push, 8)

//功能	:	加载皮肤
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppLoadSkin(
	LPCTSTR strSkinFile,			//皮肤路径
	BOOL bWithoutHook = FALSE);		//该参数指定加载皮肤时是否开启Hook功能.

//功能	:	设置是否进行Hook
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetHookMessage(
	BOOL bHook,							//True为开启Hook功能,False为关闭Hook功能.
	DWORD dwHookStyle = 0x00000000L);	//HS_MENU开启菜单自动换肤功能,
										//HS_WINDOW开启窗口自动换肤功能,
										//HS_NONE关闭所有Hook,HS_ALL开启所有自动换肤功能.
										   
//功能	:	从资源加载皮肤
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppLoadSkinFromRes(
	HINSTANCE hInstance,			//资源实例句柄
	LPCTSTR szResourceName,			//资源名称
	LPCTSTR szResourceType,			//资源类型
	LPCTSTR szSkinName,				//皮肤名称
	BOOL bWithoutHook = FALSE);		//是否Hook
											
//功能	:	设置HookAPI的类型
//返回值:	返回当前API类型
DWORD __stdcall sppModifyHookAPIStyle(
	DWORD dwRemove,					//移除API类型
	DWORD dwAdd);					//添加API类型

//功能	:	获得当前API类型
//返回值:	返回当前API类型
DWORD __stdcall sppGetHookAPIStyle();

//功能	:	设置是否HookAPI
//返回值:	返回设置前的HookAPI状态
BOOL __stdcall sppSetHookAPI(
	BOOL bHookAPI);					//True为API Hook,False为去掉API Hook

//功能	:	获得HookAPI状态
//返回值:	True表示当前为HookAPI,False表示当前没有HookAPI
BOOL __stdcall sppGetHookAPI();

//功能	:	移除皮肤
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppRemoveSkin();

//功能	:	设置标题栏自定义按钮是否显示
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetCustSysBtnVisible(
	UINT uResID,					//按钮的ResourceID
	BOOL bVisible);					//是否可见

//功能	:	指定自定义系统按钮的Check状态
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetCustSysBtnStatus(
	UINT uResID,					//自定义系统按钮的资源ID数值
	BOOL bChecked = TRUE);			//是否按下状态

//功能	:	设置皮肤色调
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppColorize(
	COLORREF clr,					//皮肤色调 如果clr==-1,就恢复原来的效果
	double blend = 0.5,				//混合透明度 0-1 , 0为无效果,1为无透明
	BOOL bShowDialog = FALSE);		//是否显示颜色选择对话框

//功能	:	弹出文件选择框选择皮肤
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSelectSkin(); 

//功能	:	在给定的HDC上,画位图。
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppDrawSkinBitmap(
	HDC hdc,						//目标DC
	RECT rect,						//绘制区域
	HBITMAP hBitmap,				//要绘制的位图句柄
	BOOL bStretchAll,				//是否拉伸
	int nTopHeight = 0,				//不被拉伸的顶高
	int nBottomHeight = 0,			//不被拉伸的底高
	int nLeftWidth = 0,				//不被拉伸的左宽
	int nRightWidth = 0,			//不被拉伸的右宽
	COLORREF clrTrans = -1,			//透明色
	BOOL bTrans = FALSE				//是否透明
	);

//功能	:	在给定的HDC上,画位图
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppDrawSkinImage(
	HDC hdc,						//目标DC
	RECT rect,						//绘制区域
	LPCTSTR lpszObjName,			//要绘制皮肤图片名称
	BOOL bStretchAll,				//是否拉伸
	int nLeftWidth,					//不被拉伸的左宽
	int nRightWidth,				//不被拉伸的右宽
	int nTopHeight,					//不被拉伸的顶高
	int nBottomHeight,				//不被拉伸的底高
	COLORREF clrTrans = -1,			//透明色
	BOOL bTrans = FALSE				//是否透明
	);

//功能	:	获得换肤后的系统颜色
//返回值:	成功返回当前皮肤的颜色,失败返回-1
COLORREF __stdcall sppGetSkinSysColor(
	int nColorIndex);				//要获取的颜色类型

//功能	:	获得Windows系统默认的颜色
//返回值:	成功返回Windows系统颜色,失败返回-1
COLORREF __stdcall sppGetDefaultSysColor(
	int nColorIndex);				//要获取的颜色类型

//功能	:	设置窗口的资源ID,该ID会在多语言版本中用到
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetWindowResID(
	HWND hWnd,						//对话框窗口的句柄
	int nResID);					//对话框资源ID

//功能	:	获得窗口的资源ID
//返回值:	成功返回资源ID,失败返回-1
int __stdcall sppGetWindowResID(
	HWND hWnd);						//对话框窗口的句柄

//功能	:	设置异型窗口的资源ID
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetFreeWindowID(
	HWND hWnd,						//对话框窗口的句柄
	int nResID);					//资源ID

//功能	:	获得异型窗口的资源ID
//返回值:	成功返回资源ID,失败返回-1
int __stdcall sppGetFreeWindowID(
	HWND hWnd);						//对话框窗口的句柄

//功能	:	设置窗口自画是否自己来处理,该方法用于自画部分需要自己处理的情况下
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetCustomDraw(
	HWND hWnd,						//自画窗口句柄
	BOOL bCustomDraw);				//TRUE为自己处理自画,FALSE为交给Skin++处理自画

//功能	:	设置窗口是否计算RGN区域
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetRgnEnable(
	HWND hWnd,						//窗口句柄
	BOOL bRgnEnable);				//True为计算区域,FALSE为不计算区域

//功能	:	设置ListHeader窗口的排序信息
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetListHeaderSortInfo(
	HWND hWnd,						//ListHeader的窗口句柄
	int nSortColumn,				//要对ListHeader排序的列的索引
	BOOL bSortAscending = TRUE);	//是否为升序

//功能	:	设置是否自己画对话框背景,该方法用在需要自己对背景进行处理的情况下.
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetDialogEraseBkgnd(
	HWND hWnd,						//对话框句柄
	BOOL bErase);					//TRUE 为自己画背景,FALSE 为Skin++画,
									//如果没有调用该方法,Skin++将画对话框背景.

//功能	:	设置对话框背景是否剪切子控件区域
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetDialogBkClipRgn(
	HWND hWnd,						//对话框句柄
	BOOL bClip,						//TRUE为不需要剪切,FALSE为需要剪切区域
	BOOL bAllChild = TRUE);			//TRUE为该窗体的所有子对话框都剪切.

//功能	:	设置按钮的提示
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetButtonTooltip(
	HWND hWnd,						//按钮句柄
	LPCTSTR szTooltip);				//按钮提示文本
	
//功能	:	对指定的窗口去掉皮肤,并且保证不会再被换肤,即使使用SetSkinHwnd也不会换肤.
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetNoSkinHwnd(
	HWND hWnd,						//窗口句柄
	BOOL bChildNoSkin = TRUE);		//是否对该窗口中的子窗口去掉皮肤

//功能	:	对指定的窗口进行换肤
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppSetSkinHwnd(
	HWND hWnd,						//窗口句柄
	LPCTSTR szClassName = NULL,		//要子类化的Skin类型 WC_DIALOGBOX/WC_CONTROLBAR等
	BOOL bChildSkin = TRUE);		//是否对该窗口中的子窗口加上皮肤

//功能	:	对指定的窗口临时去掉皮肤,可以通过SetSkinHwnd进行再次换肤
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppRemoveSkinHwnd(
	HWND hWnd);						//窗口句柄

//功能	:	获得Skin++的系统Metrics,作用同Windows API函数GetSystemMetrics,
//			取得Skin++实际的标题栏高度,菜单条高度等数值
//返回值:	成功返回Skin++的Metrics,失败返回-1
int __stdcall sppGetSystemMetrics(
	HWND hWnd,						//窗体句柄
	int nIndex);					//参数同GetSystemMetrics

//功能	:	多线程换肤用
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppInitializeSkin();

//功能	:	退出界面库,做清理工作。
//返回值:	成功返回True,失败返回False
BOOL __stdcall sppExitSkin();

//功能	:	设置窗口是否刷新
//返回值		: 成功返回True,失败返回False
//备注			: 在数据窗口添加数据时会导致刷新比较慢,
//这个时候可以用这个接口锁定刷新,数据插入完成后需重新开启刷新
BOOL __stdcall sppSetRedraw(
	HWND hWnd,						//窗口句柄
	BOOL bRedraw);					//True为刷新,False为锁定刷新

//功能		: 验证Skin++版本是否与当前程序的开发工具相匹配
//返回值	: 成功返回True,失败返回False
BOOL _stdcall sppValidateDevTools();

//[For PowerBuilder]
//功能			: 设置Tab控件的标签页的位置
//返回值		: 成功返回True,失败返回False
enum TABPOSITION
{
	TABPOS_UNKNOWN				=   0x00,
	TABPOS_TABSONTOP			=	0x01,
	TABPOS_TABSONTOPANDBOTTOM	=	0x02,
	TABPOS_TABSONLEFT			=	0x03,
	TABPOS_TABSONLEFTANDRIGHT	=	0x04,
	TABPOS_TABSONBOTTOM			=	0x05,
	TABPOS_TABSONBOTTOMANDTOP	=	0x06,
	TABPOS_TABSONRIGHT			=	0x07,
	TABPOS_TABSONRIGHTANDLEFT	=	0x08
};

BOOL __stdcall sppSetTabCtrlPosition(
	HWND	hWnd,						//Tab控件的窗口句柄
	TABPOSITION enumPosition,			//TABPOSITION的枚举值
	BOOL	bPictureOnRight = FALSE);	//图标是否显示在文本的右侧

//[For PowerBuilder]
//功能			: 设置Tab控件的标签页显示的索引值
//返回值		: 成功返回True,失败返回False
BOOL __stdcall sppSetTabCtrlItemsStatus(
	HWND    hWnd,						//Tab控件的窗口句柄
	int*	pIndexs,					//显示标签页的Index集合
	int		nCount						//显示标签页的总个数
	);

//功能	:	是对SetNoSkinHwnd的进一步处理,可以解决使用sppSetNoSkinHwnd引起的Debug版的断言错
#define SETNOSKINHWND(x) {\
    HWND w=(x).UnsubclassWindow();\
	sppSetNoSkinHwnd(w);\
	(x).SubclassWindow(w);\
}

#ifdef _SKINPP_DYNAMICCALLDLL

class CSkinPPDynamicCallDll
{
public:
	typedef BOOL	(__stdcall *SPPLOADSKIN)(LPCTSTR strSkinFile,BOOL bWithoutHook);
	typedef BOOL	(__stdcall *SPPSETHOOKMESSAGE)(BOOL bHook,DWORD dwHookStyle);
	typedef BOOL	(__stdcall *SPPLOADSKINFROMRES)(HINSTANCE hInstance,LPCTSTR szResourceName,LPCTSTR szResourceType,LPCTSTR szSkinName,BOOL bWithoutHook);
	typedef DWORD	(__stdcall *SPPMODIFYHOOKAPISTYLE)(DWORD dwRemove,DWORD dwAdd);				
	typedef DWORD	(__stdcall *SPPGETHOOKAPISTYLE)();
	typedef BOOL	(__stdcall *SPPSETHOOKAPI)(BOOL bHookAPI);
	typedef BOOL	(__stdcall *SPPGETHOOKAPI)();
	typedef BOOL	(__stdcall *SPPREMOVESKIN)();
	typedef BOOL	(__stdcall *SPPSETCUSTSYSBTNVISIBLE)(UINT uResID,BOOL bVisible);
	typedef BOOL	(__stdcall *SPPSETCUSTSYSBTNSTATUS)(UINT uResID,BOOL bChecked);
	typedef BOOL	(__stdcall *SPPCOLORIZE)(COLORREF clr,double blend,BOOL bShowDialog);
	typedef BOOL	(__stdcall *SPPSELECTSKIN)(); 
	typedef BOOL	(__stdcall *SPPDRAWSKINBITMAP)(HDC hdc,RECT rect,HBITMAP hBitmap,BOOL bStretchAll,
		int nTopHeight,int nBottomHeight,int nLeftWidth,
		int nRightWidth,COLORREF clrTrans,BOOL bTrans);
	typedef BOOL	(__stdcall *SPPDRAWSKINIMAGE)(HDC hdc,RECT rect,LPCTSTR lpszObjName,BOOL bStretchAll,
		int nLeftWidth,int nRightWidth,int nTopHeight,
		int nBottomHeight,COLORREF clrTrans,BOOL bTrans);
	typedef COLORREF	(__stdcall *SPPGETSKINSYSCOLOR)(int nColorIndex);	
	typedef COLORREF	(__stdcall *SPPGETDEFAULTSYSCOLOR)();
	typedef BOOL		(__stdcall *SPPSETWINDOWRESID)(HWND hWnd,int nResID);	
	typedef int			(__stdcall *SPPGETWINDOWRESID)(HWND hWnd);	
	typedef BOOL		(__stdcall *SPPSETFREEWINDOWID)(HWND hWnd,int nResID);
	typedef int			(__stdcall *SPPGETFREEWINDOWID)(HWND hWnd);	
	typedef BOOL		(__stdcall *SPPSETCUSTOMDRAW)(HWND hWnd,BOOL bCustomDraw);
	typedef BOOL		(__stdcall *SPPSETRGNENABLE)(HWND hWnd,BOOL bRgnEnable);
	typedef BOOL		(__stdcall *SPPSETLISTHEADERSORTINFO)(HWND hWnd,int nSortColumn,BOOL bSortAscending);
	typedef BOOL		(__stdcall *SPPSETDIALOGERASEBKGND)(HWND hWnd,BOOL bErase);
	typedef BOOL		(__stdcall *SPPSETDIALOGBKCLIPRGN)(HWND hWnd,BOOL bClip,BOOL bAllChild);
	typedef BOOL		(__stdcall *SPPSETBUTTONTOOLTIP)(HWND hWnd,LPCTSTR szTooltip);	
	typedef BOOL		(__stdcall *SPPSETNOSKINHWND)(HWND hWnd,BOOL bChildNoSkin);
	typedef BOOL		(__stdcall *SPPSETSKINHWND)(HWND hWnd,LPCTSTR szClassName,BOOL bChildSkin);
	typedef BOOL		(__stdcall *SPPREMOVESKINHWND)(HWND hWnd);
	typedef int			(__stdcall *SPPGETSYSTEMMETRICS)(HWND hWnd,int nIndex);
	typedef BOOL		(__stdcall *SPPINITIALIZESKIN)();
	typedef BOOL		(__stdcall *SPPEXITSKIN)();
	typedef BOOL		(__stdcall *SPPSETREDRAW)(HWND hWnd,BOOL bRedraw);
	typedef BOOL		(__stdcall *SPPVALIDATEDEVTOOLS)();
		
	SPPLOADSKIN					LoadSkin;
	SPPSETHOOKMESSAGE			SetHookMessage;
	SPPLOADSKINFROMRES			LoadSkinFromRes;
	SPPMODIFYHOOKAPISTYLE		ModifyHookAPIStyle;
	SPPGETHOOKAPISTYLE			GetHookAPIStyle;
	SPPSETHOOKAPI				SetHookAPI;
	SPPGETHOOKAPI				GetHookAPI;
	SPPREMOVESKIN				RemoveSkin;
	SPPSETCUSTSYSBTNVISIBLE		SetCustSysBtnVisible;
	SPPSETCUSTSYSBTNSTATUS		SetCustSysBtnStatus;
	SPPCOLORIZE					Colorize;
	SPPSELECTSKIN				SelectSkin;
	SPPDRAWSKINBITMAP			DrawSkinBitmap;
	SPPDRAWSKINIMAGE			DrawSkinImage;
	SPPGETSKINSYSCOLOR			GetSkinSysColor;
	SPPGETDEFAULTSYSCOLOR		GetDefaultSysColor;
	SPPSETWINDOWRESID			SetWindowResID;
	SPPGETWINDOWRESID			GetWindowResID;
	SPPSETFREEWINDOWID			SetFreeWindowID;
	SPPGETFREEWINDOWID			GetFreeWindowID;
	SPPSETCUSTOMDRAW			SetCustomDraw;
	SPPSETRGNENABLE				SetRgnEnable;
	SPPSETLISTHEADERSORTINFO	SetListHeaderSortInfo;
	SPPSETDIALOGERASEBKGND		SetDialogEraseBkgnd;
	SPPSETDIALOGBKCLIPRGN		SetDialogBkClipRgn;
	SPPSETBUTTONTOOLTIP			SetButtonTooltip;
	SPPSETNOSKINHWND			SetNoSkinHwnd;
	SPPSETSKINHWND				SetSkinHwnd;
	SPPREMOVESKINHWND			RemoveSkinHwnd;
	SPPGETSYSTEMMETRICS			GetSystemMetrics;
	SPPINITIALIZESKIN			InitializeSkin;
	SPPEXITSKIN					ExitSkin;
	SPPSETREDRAW				SetRedraw;
	SPPVALIDATEDEVTOOLS			ValidateDevTools;

	CSkinPPDynamicCallDll() 
	{

	}

	CSkinPPDynamicCallDll(LPCTSTR strDllPathName)
	{
		Initialize(strDllPathName);
	}

	virtual ~CSkinPPDynamicCallDll()
	{
		Exit();
	}


	BOOL Initialize(LPCTSTR strDllPathName) 
	{
		if (m_hModuleDll)
			return FALSE;
		
		if (strDllPathName == NULL/* || _tcslen(strDllPathName) == 0*/)
		{
#ifdef _UNICODE
			m_hModuleDll = ::LoadLibraryW(L"SkinPlusPlusU.dll");
#else
			m_hModuleDll = ::LoadLibraryA("SkinPlusPlus.dll");
#endif
		}
		else
			m_hModuleDll = ::LoadLibrary(strDllPathName);
		
		if (!m_hModuleDll)
			return FALSE;
		
		LoadSkin = (SPPLOADSKIN)GetProcAddress(m_hModuleDll,"sppLoadSkin");
		
		SetHookMessage = (SPPSETHOOKMESSAGE)GetProcAddress(m_hModuleDll,"sppSetHookMessage");
		
		LoadSkinFromRes = (SPPLOADSKINFROMRES)GetProcAddress(m_hModuleDll,"sppLoadSkinFromRes");
		
		ModifyHookAPIStyle = (SPPMODIFYHOOKAPISTYLE)GetProcAddress(m_hModuleDll,"sppModifyHookAPIStyle");
		
		GetHookAPIStyle = (SPPGETHOOKAPISTYLE)GetProcAddress(m_hModuleDll,"sppGetHookAPIStyle");
		
		SetHookAPI = (SPPSETHOOKAPI)GetProcAddress(m_hModuleDll,"sppSetHookAPI");
		
		GetHookAPI = (SPPGETHOOKAPI)GetProcAddress(m_hModuleDll,"sppGetHookAPI");
		
		RemoveSkin = (SPPREMOVESKIN)GetProcAddress(m_hModuleDll,"sppRemoveSkin");
		
		SetCustSysBtnVisible = (SPPSETCUSTSYSBTNVISIBLE)GetProcAddress(m_hModuleDll,"sppSetCustSysBtnVisible");
		
		SetCustSysBtnStatus = (SPPSETCUSTSYSBTNSTATUS)GetProcAddress(m_hModuleDll,"sppSetCustSysBtnStatus");
				
		Colorize = (SPPCOLORIZE)GetProcAddress(m_hModuleDll,"sppColorize");
		
		SelectSkin = (SPPSELECTSKIN)GetProcAddress(m_hModuleDll,"sppSelectSkin");
		
		DrawSkinBitmap = (SPPDRAWSKINBITMAP)GetProcAddress(m_hModuleDll,"sppDrawSkinBitmap");
		
		DrawSkinImage = (SPPDRAWSKINIMAGE)GetProcAddress(m_hModuleDll,"sppDrawSkinImage");
		
		GetSkinSysColor = (SPPGETSKINSYSCOLOR)GetProcAddress(m_hModuleDll,"sppGetSkinSysColor");
		
		GetDefaultSysColor = (SPPGETDEFAULTSYSCOLOR)GetProcAddress(m_hModuleDll,"sppGetDefaultSysColor");
		
		SetWindowResID = (SPPSETWINDOWRESID)GetProcAddress(m_hModuleDll,"sppSetWindowResID");
		
		GetWindowResID = (SPPGETWINDOWRESID)GetProcAddress(m_hModuleDll,"sppGetWindowResID");
		
		SetFreeWindowID = (SPPSETFREEWINDOWID)GetProcAddress(m_hModuleDll,"sppSetFreeWindowID");
		
		GetFreeWindowID = (SPPGETFREEWINDOWID)GetProcAddress(m_hModuleDll,"sppGetFreeWindowID");
		
		SetCustomDraw = (SPPSETCUSTOMDRAW)GetProcAddress(m_hModuleDll,"sppSetCustomDraw");
		
		SetRgnEnable = (SPPSETRGNENABLE)GetProcAddress(m_hModuleDll,"sppSetRgnEnable");
		
		SetListHeaderSortInfo = (SPPSETLISTHEADERSORTINFO)GetProcAddress(m_hModuleDll,"sppSetListHeaderSortInfo");
		
		SetDialogEraseBkgnd = (SPPSETDIALOGERASEBKGND)GetProcAddress(m_hModuleDll,"sppSetDialogEraseBkgnd");
		
		SetDialogBkClipRgn = (SPPSETDIALOGBKCLIPRGN)GetProcAddress(m_hModuleDll,"sppSetDialogBkClipRgn");
		
		SetButtonTooltip = (SPPSETBUTTONTOOLTIP)GetProcAddress(m_hModuleDll,"sppSetButtonTooltip");
		
		SetNoSkinHwnd = (SPPSETNOSKINHWND)GetProcAddress(m_hModuleDll,"sppSetNoSkinHwnd");
		
		SetSkinHwnd = (SPPSETSKINHWND)GetProcAddress(m_hModuleDll,"sppSetSkinHwnd");
		
		RemoveSkinHwnd = (SPPREMOVESKINHWND)GetProcAddress(m_hModuleDll,"sppRemoveSkinHwnd");
		
		GetSystemMetrics = (SPPGETSYSTEMMETRICS)GetProcAddress(m_hModuleDll,"sppGetSystemMetrics");
		
		InitializeSkin = (SPPINITIALIZESKIN)GetProcAddress(m_hModuleDll,"sppInitializeSkin");
		
		ExitSkin = (SPPEXITSKIN)GetProcAddress(m_hModuleDll,"sppExitSkin");				
		
		SetRedraw = (SPPSETREDRAW)GetProcAddress(m_hModuleDll,"sppSetRedraw");

		ValidateDevTools = (SPPVALIDATEDEVTOOLS)GetProcAddress(m_hModuleDll,"sppValidateDevTools");				

		return TRUE;
	}

	BOOL Exit()
	{
		if (!m_hModuleDll)
			return FALSE;
	
		::FreeLibrary(m_hModuleDll);
		
		m_hModuleDll				= NULL;
		
		LoadSkin					= NULL;
		SetHookMessage				= NULL;
		LoadSkinFromRes				= NULL;
		ModifyHookAPIStyle			= NULL;
		GetHookAPIStyle				= NULL;
		SetHookAPI					= NULL;
		GetHookAPI					= NULL;
		RemoveSkin					= NULL;
		SetCustSysBtnVisible		= NULL;
		SetCustSysBtnStatus			= NULL;
		Colorize					= NULL;
		SelectSkin					= NULL;
		DrawSkinBitmap				= NULL;
		DrawSkinImage				= NULL;
		GetSkinSysColor				= NULL;
		GetDefaultSysColor			= NULL;
		SetWindowResID				= NULL;
		GetWindowResID				= NULL;
		SetFreeWindowID				= NULL;
		GetFreeWindowID				= NULL;
		SetCustomDraw				= NULL;
		SetRgnEnable				= NULL;
		SetListHeaderSortInfo		= NULL;
		SetDialogEraseBkgnd			= NULL;
		SetDialogBkClipRgn			= NULL;
		SetButtonTooltip			= NULL;
		SetNoSkinHwnd				= NULL;
		SetSkinHwnd					= NULL;
		RemoveSkinHwnd				= NULL;
		GetSystemMetrics			= NULL;
		InitializeSkin				= NULL;
		ExitSkin					= NULL;
		SetRedraw					= NULL;
		ValidateDevTools			= NULL;
		
		return TRUE;
	}

private:
	HMODULE m_hModuleDll;

};
 
#endif //_SKINPP_DYNAMICCALLDLL

#ifdef _SKINPP_STATIC //static 
	#ifdef _AFXDLL 
	//---------------------------------------------
	// MFC shared DLL, Skin++ static library
	//---------------------------------------------
		#ifdef _DEBUG
			#ifdef _UNICODE
				#pragma comment(lib, "SkinPPStaticDDU.lib")
				#pragma message("Automatically linking with SkinPPStaticDDU.lib")
			#else
				#pragma comment(lib, "SkinPPStaticDDA.lib")
				#pragma message("Automatically linking with SkinPPStaticDDA.lib")
			#endif //UNICODE
		#else //Release
			#ifdef _UNICODE
				#pragma comment(lib, "SkinPPStaticDRU.lib")
				#pragma message("Automatically linking with SkinPPStaticDRU.lib")
			#else
				#pragma comment(lib, "SkinPPStaticDRA.lib")
				#pragma message("Automatically linking with SkinPPStaticDRA.lib")
			#endif
		#endif //_DEBUG
	#else //MFC static library
	//--------------------------------------------------
	// MFC static library, Skin++ static library
	//--------------------------------------------------
		#ifdef _DEBUG
			#ifdef _UNICODE
				#pragma comment(lib, "SkinPPStaticSDU.lib")
				#pragma message("Automatically linking with SkinPPStaticSDU.lib")
			#else
				#pragma comment(lib, "SkinPPStaticSDA.lib")
				#pragma message("Automatically linking with SkinPPStaticSDA.lib")
			#endif
		#else
			#ifdef _UNICODE
				#pragma comment(lib, "SkinPPStaticSRU.lib")
				#pragma message("Automatically linking with SkinPPStaticSRU.lib")
			#else
				#pragma comment(lib, "SkinPPStaticSRA.lib")
				#pragma message("Automatically linking with SkinPPStaticSRA.lib")
			#endif
		#endif //_DEBUG
	#endif //_AFXDLL
#elif !defined _SKINPP_DYNAMICCALLDLL //DLL 
	//------------------------------------------
	// MFC shared DLL, Skin++ shared DLL
	//------------------------------------------
#if 0
	#ifdef _UNICODE
		#pragma comment(lib,"SkinPlusPlusU.lib")  
		#pragma message("Automatically linking with SkinPlusPlusU.dll")
	#else
		#pragma comment(lib,"SkinPlusPlus.lib")
		#pragma message("Automatically linking with SkinPlusPlus.dll") 
	#endif
#endif
#endif //_SKINPP_STATIC

#ifndef WM_TOOLBARPAINTPRE
	#define WM_TOOLBARPAINTPRE		WM_USER + 0x0322
#endif
#ifndef WM_TOOLBARPAINTEND
	#define WM_TOOLBARPAINTEND		WM_USER + 0x0323
#endif
#ifndef SM_LBUTTONUP
	#define SM_LBUTTONUP			WM_USER + 0x0324
#endif
#ifndef SM_TITLELBUTTONUP
	#define	SM_TITLELBUTTONUP		WM_USER + 0x0325
#endif
#ifndef SM_TITLELBUTTONDOWN
	#define	SM_TITLELBUTTONDOWN		WM_USER + 0x0326
#endif

//HookMsg
#define HS_MENU		0x80000000L
#define HS_WINDOW	0x40000000L
#define HS_NONE		0x00000000L
#define HS_ALL		HS_MENU | HS_WINDOW

//HookAPI
#define HOOKAPI_SCROLLBAR		0x80000000L
#define HOOKAPI_GETSYSCOLOR		0x40000000L
#define HOOKAPI_FILLRECT		0x20000000L
#define HOOKAPI_OTHER			0x10000000L
#define HOOKAPI_NONE			0x00000000L
#define HOOKAPI_ALL				HOOKAPI_SCROLLBAR | HOOKAPI_GETSYSCOLOR | HOOKAPI_FILLRECT | HOOKAPI_OTHER

#pragma pack(pop)

#endif // __SKINPPINC_H
