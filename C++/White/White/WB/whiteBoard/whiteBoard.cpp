
// whiteBoard.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "CatchScreenDlg.h"
#include "MTAuthorize.h"
#include "tlhelp32.h"
#include "ScreenDrawMgr.h"
#include "ViewMTObject.h"
#include "common/EncryptCommon.h"
#include "MyHook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CwhiteBoardApp

BEGIN_MESSAGE_MAP(CwhiteBoardApp, CWinApp)
	
END_MESSAGE_MAP()


// CwhiteBoardApp 构造

CwhiteBoardApp::CwhiteBoardApp()
{
	
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_hinstRE41 = LoadLibrary(_T("msftedit.dll"));
}

CwhiteBoardApp::~CwhiteBoardApp()  
{
	if(m_hinstRE41)
	{
		FreeLibrary(m_hinstRE41);
	}
}

// 唯一的一个 CwhiteBoardApp 对象

CwhiteBoardApp theApp;
extern CWhiteBoardView *g_pWhiteBoardView = NULL; //当前试图窗口
extern CWhiteBoardView *g_pBackWhiteBoardView = NULL;
extern CDialog *g_whiteboardDlg = NULL;
extern CWBFile *g_pWBFile = NULL; //白板文件指针
extern CWBPage *g_pWBPage = NULL; //白板页指针
extern BOOL g_bIsFullScreen = FALSE; //是否全屏状态
extern IHPNetNotify *g_pHPWBDataNotify = NULL; //发送同步消息指针
extern IHPWBNotify *g_pConferenceRecordNotify = NULL; //录像消息指针
extern void *g_pParent = NULL; //父窗口
extern CPoint g_ptComRotCenter = (0,0); //组合的旋转中心点
extern CPoint g_ptDocRotCenter = (0,0); //文档的旋转中心点
extern double g_dbComRotAngle = 0; //组合的旋转角度
extern double g_dbDocRotAngle = 0; //文档的旋转角度
extern CPoint g_ptComZoomCenter = (0,0); //组合的缩放中心点
extern CPoint g_ptDocZoomCenter = (0,0); //文档的缩放中心点
extern BOOL   g_bRunWater = FALSE;         //水波纹特效
extern BOOL   g_bProcessWater = TRUE;     //控制白板最小化时显示水波纹
extern int    g_nBrushSize = 10;          //擦子大小
//组合对象的缩放比例
extern double g_dbXScale = 1;
extern double g_dbYScale = 1;
//文档对象的缩放比例
extern double g_dbDocXScale = 1;
extern double g_dbDocYScale = 1;
//缩小view
extern double g_reduce=0.00;
//白板view的长和宽
extern int g_nWidth = WB_VIEWHSCROLL;
extern int g_nHeight = WB_VIEWVSCROLL;

extern DWORD g_dwPreZoomLayerIndex = 0; //前一个触摸缩放的对象图层号
extern BOOL g_bIsDrawScreen=FALSE;                 //屏幕绘图是否开启标志
extern unsigned long g_ulSessionID = 1; //客户端ID
extern CString g_strAvconUserID = _T(""); //AVCON本地登录的用户ID，由AVCON通过接口函数设置
extern BOOL g_bYunNanDeviceSupported = FALSE;//专用于云南公安硬件授权
extern bool g_bAuthorizedByHandWare = false; //从硬件层面是否有触摸触摸授权
extern unsigned long g_MediaNumber = 2101; //视频卡号初始值
extern bool g_bWBWndIsneedShowToolBtn = true; //根据白板大小来判断是否显示按钮（默认是显示）
extern CString g_strLocalUserName = _T("");
//有AVCON提供的一些临时路径
extern CString g_strAvconLanguageFile = _T("");
extern CString g_strAvconIniPath = _T(""); //配置文件目录
/*extern CString g_strAvconTempPath = _T("d:\\WBTemp\\"); //临时目录*/
extern CString g_strAvconTempPath = NULL; //临时目录
extern bool g_bAvconLock = false; //AVCON是否锁定白板功能，当锁定时，白板打开后没有工具栏
extern bool g_bIsConferenceRecord = FALSE; //由AVCON设置，是否开始会议录像
extern HMODULE g_IsMedicalVersion = NULL;//判断是否是医疗版本；
extern WB_TYPE g_WBType = WB_MULTIVER;//当前版本类型
extern bool g_bIsSupportMTTouch = false;//是否最终授权使用多点触摸屏
extern BOOL g_bNoSupportTouchInWB = FALSE;//当触摸屏没有授权时，包括单点的触摸操作都屏蔽
extern HMODULE g_IsMidiVersion = NULL; //midi版本
//新功能控制
extern bool g_bShowWBNewFun = false;
extern bool g_bSupportI7 = false;//是否是I7使用
extern CPoint g_preScrollPoint = (0,0);
extern bool g_bSimplifyWB = true;
extern bool g_bU7NewStyle = false;
#ifdef AVCON_U7_WHITEBOARD
  extern BOOL g_bSupportU7OpenFile = TRUE;// 判断是否为U7中的打开文件方式
#else
  extern BOOL g_bSupportU7OpenFile = FALSE;// 判断是否为U7中的打开文件方式
#endif


extern bool g_bShowFullScreenBtn = true; //显示全屏按钮；
extern CString g_bU7FileTempDir = _T("");// 设置U7的文件默认路径
extern BOOL g_MediaIntToSin = FALSE;//视频网络到单机
extern int g_PageCount = 0;//页号，视频打开文件时候
extern HWND g_hVedioHwnd = NULL;//屏幕录制窗口录制句柄
extern BOOL g_bMedioScreenWndEnd = TRUE;//区域录制窗口意外关闭
extern CString g_strMediaPhoto = _T(""); //视频双击拍照存放路径；
const GUID CDECL _tlid = { 0x284145CA, 0x2220, 0x4A4A, { 0xAF, 0x2, 0x52, 0xF1, 0x3E, 0xF2, 0x76, 0xB9 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

typedef BOOL (*MTAuthorize)(void);
typedef BOOL (*SupportedDevice)();
void GetFilePath(CString &stringpath);
void SetConfigFilePath();
CString CurLoadServerTimeStr = _T("");//本次登陆服务器的时间
CString PreOpenSoftTimeStr = _T("");//上一次使用软件的时间，主要在单机中使用
bool g_isLoacalWB = false;
HMODULE g_hDESEncrypt = NULL;//触摸屏临时授权库加载句柄
// CwhiteBoardApp 初始化

BOOL CwhiteBoardApp::InitInstance()
{
	CWinApp::InitInstance();	
	AfxEnableControlContainer();

	/*	if (!AfxSocketInit())
	{
	AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	return FALSE;
	}*/

	// 将所有 OLE 服务器(工厂)注册为运行。这将使
	//  OLE 库得以从其他应用程序创建对象。
	COleObjectFactory::RegisterAll();
	BOOL LanguageLocal =  ::SetThreadLocale(0x0804);

	g_ulSessionID = GenerateSSRC(); //产生客户ID

	//得到剩余空间最大的盘符给临时目录赋值
	GetFilePath(g_strAvconTempPath);
	g_strAvconTempPath += _T("\WBTemp\\");
	//设置配置文件路径
	SetConfigFilePath();
	return TRUE;
}

int CwhiteBoardApp::ExitInstance()
{
	/*if (g_pWBFile != NULL)
	{
	delete g_pWBFile;
	g_pWBFile = NULL;
	}*/
	EndHook();
	CMFCVisualManager * pVisualManager=CMFCVisualManager::GetInstance();
	if (pVisualManager!=NULL)
	{
		delete pVisualManager;
	}
	if (NULL != g_IsMedicalVersion )
	{
		FreeLibrary(g_IsMedicalVersion);//卸载动态链接库；
		g_IsMedicalVersion = NULL;//设置回默认的值，以便刷新；
	}
	if (NULL != g_hDESEncrypt )
	{
		FreeLibrary(g_hDESEncrypt);//卸载动态链接库；
		g_hDESEncrypt = NULL;//设置回默认的值，以便刷新；
	}
	if (g_IsMidiVersion != NULL)
	{
		BOOL bFree =::FreeLibrary(g_IsMidiVersion);
		g_IsMidiVersion = NULL;
	}
	//删除临时文件夹 wangxin 2010-12-28	
	DeleteDirectory(g_strAvconTempPath);
	return CWinApp::ExitInstance();

}

//WANGXIN 2010-12-28 删除一个文件夹
BOOL CwhiteBoardApp::DeleteDirectory(CString strDir)
{
	/*
	strDir += '\0';

	SHFILEOPSTRUCT FileOp ;
	FileOp.fFlags = FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = strDir;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE;

	BOOL nResult = SHFileOperation(&FileOp);*/

	if (strDir.GetLength() == 0)
	{
		return false;
	}
	CFileFind tempFind; 
	TCHAR tempFileFind[MAX_PATH]; 
	wsprintf(tempFileFind,_T("%s\\*.*"),strDir); 
	BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind); 
	while(IsFinded) 
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile(); 
		if(!tempFind.IsDots()) 
		{ 
			TCHAR foundFileName[MAX_PATH]; 
			wcscpy_s(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
			tempFind.GetFileName().ReleaseBuffer();
			if(tempFind.IsDirectory()) 
			{ 
				TCHAR tempDir[MAX_PATH]; 
				wsprintf(tempDir,_T("%s\\%s"),strDir,foundFileName); 
				DeleteDirectory(tempDir); 
			} 
			else 
			{ 
				TCHAR tempFileName[MAX_PATH]; 
				wsprintf(tempFileName,_T("%s\\%s"),strDir,foundFileName); 
				DeleteFile(tempFileName); 
			} 
		} 
	} 
	tempFind.Close(); 
	if(!RemoveDirectory(strDir)) 
	{ 	
		return FALSE; 
	} 

	return TRUE; 
}

void WriteEndTime()
{
	//将当前推出的时间写到配置文件中，以在下次授权的时候用于时间的判断
	SYSTEMTIME CurrentST = {0};
	CString CurLocalTimeStr = _T(""),TempStr;
	CString WriteStrPath = g_strAvconIniPath + _T("\\WBConfig.ini");
	CString StrSelection = _T("PreEndTime");
	CString StrName = _T("PreEndTimeName");
	GetLocalTime(&CurrentST);
	CurLocalTimeStr.Format(_T("%d"),CurrentST.wYear);

	if (CurrentST.wMonth < 10)
	{
		TempStr.Format(_T("0%d"),CurrentST.wMonth);
		CurLocalTimeStr += TempStr;
	}
	else
	{
		TempStr.Format(_T("%d"),CurrentST.wMonth);
		CurLocalTimeStr += TempStr;
	}
	if (CurrentST.wDay < 10)
	{
		TempStr.Format(_T("0%d"),CurrentST.wDay);
		CurLocalTimeStr += TempStr;
	}
	else
	{
		TempStr.Format(_T("%d"),CurrentST.wDay);
		CurLocalTimeStr += TempStr;
	}
	if (CurrentST.wHour < 10)
	{
		TempStr.Format(_T("0%d"),CurrentST.wHour);
		CurLocalTimeStr += TempStr;
	}
	else
	{
		TempStr.Format(_T("%d"),CurrentST.wHour);
		CurLocalTimeStr += TempStr;
	}
	if (CurrentST.wMinute < 10)
	{
		TempStr.Format(_T("0%d"),CurrentST.wMinute);
		CurLocalTimeStr += TempStr;
	}
	else
	{
		TempStr.Format(_T("%d"),CurrentST.wMinute);
		CurLocalTimeStr += TempStr;
	}
	for(int i = 0; i <CurLocalTimeStr.GetLength();++i )
	{
		CurLocalTimeStr.SetAt(i,CurLocalTimeStr.GetAt(i) - 50 -i);
	}

	WritePrivateProfileString(StrSelection,StrName,CurLocalTimeStr, WriteStrPath);
}

void SetConfigFilePath()//在白板初始的时候调用
{
	/*----------------根据杜权胜2011.11.22邮件-------------------------------
	通过终端中有个ExePath.ini 文件来识别。文件字段及目录如下：
	C:\Program Files\avcon\ExePath.ini 

	[AVCON_SELECTED]
	#是否是XPE，0-PC,1-XPE
	AVCONXPE=0

	如果是终端，UCC将所有读写配置文件都指向“D:\AVCON6_XPE ”目录。
	----------------------------------------------------------------------*/		
	
	int nValue = 0;
	nValue = GetPrivateProfileInt(_T("AVCON_SELECTED"),
		_T("AVCONXPE"),
		0,
		_T("C:\\Program Files\\avcon\\ExePath.ini"));
	if (1 == nValue)//终端机
	{	
		if (g_bSupportU7OpenFile)
		{
			g_strAvconIniPath = _T("D:\\AVCON7_XPE") ;	
			g_strMediaPhoto = _T("D:\\AVCON7_XPE");
		}
		else
		{
			g_strAvconIniPath = _T("D:\\AVCON6_XPE");	
			g_strMediaPhoto = _T("D:\\AVCON6_XPE");
		}					
	}
	else if (0 == nValue)//pc机
	{
		g_strAvconIniPath = GetCurrentPath() + _T("\\config");
		g_strMediaPhoto = GetCurrentPath() + _T("\\user");
	}	
	//设置资源库中的ini目录
	WBRESWND::SetAvconIniPath(g_strAvconIniPath.GetBuffer());
	g_strAvconIniPath.ReleaseBuffer();
}


// DllGetClassObject - 返回类工厂

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}


// DllCanUnloadNow - 允许 COM 卸载 DLL

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}


// DllRegisterServer - 将项添加到系统注册表

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll())
		return SELFREG_E_CLASS;

	return S_OK;
}


// DllUnregisterServer - 将项从系统注册表中移除

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll(FALSE))
		return SELFREG_E_CLASS;

	return S_OK;
}
//得到剩余空间最大的盘符
void GetFilePath(CString &stringpath)
{
	ULARGE_INTEGER   ulAvailSize;        //调用者可用的字节数量       
	ULARGE_INTEGER   ulTotalSize;        //磁盘上的总字节数
	ULARGE_INTEGER   ulAvailLableSize;   //可用的字节数

	CString csStr;
	ULONGLONG size = 0;
	ULONGLONG presize = 0;
	TCHAR buf[100] = {NULL}; 
	DWORD len = GetLogicalDriveStrings(sizeof(buf)/sizeof(TCHAR),buf); 
	int i = 0;
	TCHAR cLetter[1]; 
	for( cLetter[0] = buf[i]; cLetter[0] != TCHAR(0); i = i + 4)
	{
		csStr.Empty();
		cLetter[0] = buf[i];
		if (cLetter[0] == _T('C'))
		{
			continue;
		}
		CString str = (_T("%s"), cLetter);
		CString str1 = str.Left(1);
		CString string2 = _T("\\");
		CString string1 = _T(":");
		csStr.Format(_T("%s%s%s"), str1, string1, string2);
		UINT driveType;

		//Get the drive type of drive I
		driveType = GetDriveType(csStr);
		switch(driveType)
		{
		case DRIVE_FIXED:
			::GetDiskFreeSpaceEx(csStr,&ulAvailSize,&ulTotalSize,&ulAvailLableSize);
			presize =  ulAvailSize.QuadPart/1024/1024;
			if (size < presize)
			{
				size = presize;
				stringpath = csStr;
			}
			break;
		default:
			break;
		}		
	}
}

//是否加载医疗版本的dll
void SetMedicalVersion()
{	
	//确保动态加载的库路径正确
	HMODULE hModule = GetModuleHandle(TEXT("whiteboard.dll"));
	if (!hModule) return;

	TCHAR strName[MAX_PATH];
	GetModuleFileName(hModule, strName, MAX_PATH);
	CString strPath = strName;
	int nPos = strPath.ReverseFind('\\');
	if(-1 == nPos) nPos = strPath.ReverseFind('/');
	if(-1 == nPos) return;
	strPath = strPath.Left(nPos);

	CString tempPath = _T("");
	SetCurrentDirectory(strPath.GetBuffer());

	CString strResPath = _T("");

	strResPath.Format(TEXT("%s\\WBDicomp.dll"), strPath);

	g_IsMedicalVersion = LoadLibrary(strResPath.GetBuffer());
	strPath.ReleaseBuffer();
	strResPath.ReleaseBuffer();
}
//进行语言选择
void GetLangPath()
{
	CString strCfgFilePath ;
	int nValue = 0;
	nValue = GetPrivateProfileInt(_T("AVCON_SELECTED"),
		_T("AVCONXPE"),
		0,
		_T("C:\\Program Files\\avcon\\ExePath.ini"));
	if (1 == nValue)
	{
		if (g_bSupportU7OpenFile)
		{
			strCfgFilePath = _T("D:\\AVCON7_XPE") ;
			strCfgFilePath += _T("\\U7Config.ini");
			
		}
	}
	else if (0 == nValue)
	{
		strCfgFilePath = GetCurrentPath() + _T("\\U7Config.ini");
	}
	
	TCHAR strValue[1024] = _T("");
	::GetPrivateProfileString(_T("LANGUAGE"), _T("Language"), _T("cs"), strValue, 1024, strCfgFilePath);
	g_strAvconLanguageFile = GetCurrentPath() +_T("\\config\\") + strValue + _T("_WBConfig.ini");

}

//加载MIDI库
void SetMidiVerson()
{
	
	HMODULE hModule = GetModuleHandle(TEXT("whiteboard.dll"));
	if (!hModule) return;

	TCHAR strName[MAX_PATH];
	GetModuleFileName(hModule, strName, MAX_PATH);
	CString strPath = strName;
	int nPos = strPath.ReverseFind('\\');
	if(-1 == nPos) nPos = strPath.ReverseFind('/');
	if(-1 == nPos) return;
	strPath = strPath.Left(nPos);

	CString tempPath = _T("");
	SetCurrentDirectory(strPath.GetBuffer());

	CString strResPath = _T("");

	strResPath.Format(TEXT("%s\\WBMIDI.dll"), strPath);
	g_IsMidiVersion = LoadLibrary(strResPath.GetBuffer());
	strPath.ReleaseBuffer();
	strResPath.ReleaseBuffer();

	//g_IsMidiVersion = LoadLibrary(_T("D:\\WhiteBoard\\TOUCH\\binU7\\Debug\\System\\wbmidi.dll"));

}

/*************************************************************************
*                           BOOL ShowWhiteBoardDlg()                     *
*                              显示电子白板接口                          *
*************************************************************************/

WB_API BOOL WB_ShowWhiteBoardDlg(void *pParent, IHPNetNotify* pDataNotify, const CWnd* pWndInsertAfter, CRect rect, UINT nFlags)
{
	if (NULL != g_whiteboardDlg)
	{
		return TRUE;
	}
	else
	{
		BOOL bFlag;
		GetLangPath();//读取根目录下的U7Config.ini确定语言版本，默认为cs中文版
		CWhiteBoardDlg *m_whiteboard_dlg=new CWhiteBoardDlg;
		g_bU7NewStyle = true;
		//得到系统版本号
		g_systemVersion = GetSystemVersion();

		//是否加载医疗版本
		SetMedicalVersion();
		//加载midi库
		SetMidiVerson();
		//_CrtDumpMemoryLeaks();
		//AFX_MANAGE_STATE(AfxGetStaticModuleState());
		HINSTANCE main_hInstance = AfxGetResourceHandle();    //获取主程序资源句柄
		AfxSetResourceHandle(theApp.m_hInstance);             //获取dll程序资源句柄

		if(m_whiteboard_dlg->Create(IDD_WHITEBOARD_DLG, (CWnd *)pParent))
		{

			m_whiteboard_dlg->ShowWindow(SW_SHOW);

			m_whiteboard_dlg->SetWindowPos(
				pWndInsertAfter,
				rect.left,
				rect.top,
				rect.right-rect.left,
				rect.bottom-rect.top,
				SWP_SHOWWINDOW); 
			m_whiteboard_dlg->m_winX = rect.right;
			m_whiteboard_dlg->m_winY = rect.bottom;
			bFlag=TRUE;
			g_pWhiteBoardView = m_whiteboard_dlg->m_pWhiteboard_view;
			g_pBackWhiteBoardView = g_pWhiteBoardView;
			g_whiteboardDlg = m_whiteboard_dlg;
			g_pHPWBDataNotify = (IHPNetNotify *)pDataNotify;
			g_pParent = pParent;
			
			//		WB_EagleEye();
		}
		else
		{
			CString strPrompt;
			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
			if (strPrompt.IsEmpty())
			{
				strPrompt = _T("提示");
			}
			CString strCreateAppFail;
			strCreateAppFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CreateAppFail"));
			if (strCreateAppFail.IsEmpty())
			{
				strCreateAppFail = _T("创建应用程序失败！");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strCreateAppFail, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strCreateAppFail);
			}
			else
			{
				AfxMessageBox(strCreateAppFail);
			}
			bFlag=FALSE;
		}
		return bFlag;
	}
} 

WB_API BOOL WB_ShowWhiteBoardDlgI7( HWND ParenthWnd, IHPNetNotify* pDataNotify, RECT whiteboardRect, UINT nFlags)
{
	if (NULL != g_whiteboardDlg)
	{
		return TRUE;
	}
	else
	{
		CRect rect(whiteboardRect); 
		BOOL bFlag;
		GetLangPath();//读取根目录下的U7Config.ini确定语言版本，默认为cs中文版
		CWhiteBoardDlg *m_whiteboard_dlg=new CWhiteBoardDlg;

		//得到系统版本号
		g_systemVersion = GetSystemVersion();

		//是否加载医疗版本
		SetMedicalVersion();
		//加载midi库
		SetMidiVerson();
		HINSTANCE main_hInstance = AfxGetResourceHandle();    //获取主程序资源句柄
		AfxSetResourceHandle(theApp.m_hInstance);             //获取dll程序资源句柄

		if(m_whiteboard_dlg->Create(IDD_WHITEBOARD_DLG, CWnd::FromHandle(ParenthWnd)))
		{
			::SetWindowPos(m_whiteboard_dlg->m_pWhiteboard_view->GetHWND(),
				HWND_TOP,
				rect.left,
				rect.top,
				rect.right-rect.left,
				rect.bottom-rect.top,
				SWP_SHOWWINDOW); 

			m_whiteboard_dlg->ShowWindow(SW_SHOW);
			::SetWindowPos(m_whiteboard_dlg->GetSafeHwnd(),
				HWND_TOP,
				rect.left,
				rect.top,
				rect.right-rect.left,
				rect.bottom-rect.top,
				SWP_SHOWWINDOW); 
			m_whiteboard_dlg->m_winX = rect.right;
			m_whiteboard_dlg->m_winY = rect.bottom;
			bFlag=TRUE;
			g_pWhiteBoardView = m_whiteboard_dlg->m_pWhiteboard_view;
			g_pBackWhiteBoardView = g_pWhiteBoardView;
			g_whiteboardDlg = m_whiteboard_dlg;
			g_pHPWBDataNotify = (IHPNetNotify *)pDataNotify;
			g_bSupportI7 = true;
			g_bSupportU7OpenFile = false;
			WB_SetTouchAuthorization(true);
			//		WB_EagleEye();
		}
		else
		{
			CString strPrompt;
			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
			if (strPrompt.IsEmpty())
			{
				strPrompt = _T("提示");
			}
			CString strCreateAppFail;
			strCreateAppFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CreateAppFail"));
			if (strCreateAppFail.IsEmpty())
			{
				strCreateAppFail = _T("创建应用程序失败！");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strCreateAppFail, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strCreateAppFail);
			}
			else
			{
				AfxMessageBox(strCreateAppFail);
			}
			bFlag=FALSE;
		}
		return bFlag;
	}
}


//改变白板窗口位置
WB_API void WB_MoveBorad( CRect rect, UINT nFlags)
{
	if (g_whiteboardDlg == NULL)
	{
		return;
	}
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->DeleteEdit();
	}
	
	::SetWindowPos(g_whiteboardDlg->GetSafeHwnd(),
		HWND_TOP,
		rect.left,
		rect.top,
		rect.right-rect.left,
		rect.bottom-rect.top,
		SWP_SHOWWINDOW); 
	//白板一打开就打开鹰眼

	if (g_bWBWndIsneedShowToolBtn)
	{
		WB_EagleEyeSetposAndShow(rect);
	}

	return;
}

//改变白板窗口位置
WB_API void WB_MoveBorad(const CWnd* pWndInsertAfter, CRect rect, UINT nFlags)
{
	if (g_whiteboardDlg == NULL)
	{
		return;
	}
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->DeleteEdit();
	}
	
	g_whiteboardDlg->SetWindowPos(
		pWndInsertAfter,
		rect.left,
		rect.top,
		rect.right-rect.left,
		rect.bottom-rect.top,
		SWP_SHOWWINDOW); 
	//白板一打开就打开鹰眼

	if (g_bWBWndIsneedShowToolBtn)
	{
		WB_EagleEyeSetposAndShow(rect);
	}

	return;
}

//设置数据发送窗口指针
WB_API void WB_SetDataNotify(void *pDataNotify)
{
	g_pHPWBDataNotify = (IHPNetNotify *)pDataNotify;
}

/*************************************************************************
*                           void SetFullScreen( )                        *
*                              电子白板全屏接口                          *
*************************************************************************/
WB_API void WB_SetFullScreen(BOOL bFullScreen/* = TRUE*/)
	/*
	全屏或退出全屏
	bFullScreen为TRUE表示全屏，为FALSE表示退出全屏
	*/
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->ProcessCloseColorDlg();
		g_pWhiteBoardView->SendLocalMessage(CMD_SCREEN,bFullScreen); //发送全屏本地消息
		g_pWhiteBoardView->ChangeFullScreen(bFullScreen); //发送全屏网络消息
		if (g_bSupportU7OpenFile == FALSE)
		{
			g_pWhiteBoardView->DeleteWaterCursorForAvcon6();
		}
		if (!bFullScreen)
		{
			g_pWhiteBoardView->HideUserCursor();
		}
	}
}

/*************************************************************************
*                           void SetNormalScreen( )                      *
*                              电子白板还原接口                          *
*************************************************************************/
WB_API void WB_SetNormalScreen( )
{
	//全屏时，将窗口所有数据保存到文件，以便将数据传送到还原窗口。
	//wbFullDlg.SendDataFullToFile(){}
	//g_pWhiteBoardView = NULL;
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		//12-27新加
		CWhiteBoardDlg *m_whiteboard_dlg = (CWhiteBoardDlg*)g_whiteboardDlg;
		m_whiteboard_dlg->closeFullDlg();

		g_pWhiteBoardView = g_pBackWhiteBoardView;
		g_pWhiteBoardView->DeleteEdit();
		g_pObj = NULL;
	}

	//还原窗口重画，将文件中的数据读入到还原窗口
	//m_whiteboard_dlg->SendDataFileToNormal(){}
}
//设置运行状态
WB_API void WB_SetDrawStatus(DRAW_STATE nDrawStatus)
{
	//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
				CScreenDrawMgr::GetIns()->UpdateWindow();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
				g_pWhiteBoardView->UpdateWindow();
			}
		}

		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		g_nDrawStatus = nDrawStatus;
		if (g_nDrawStatus != DRAW_ERASE && g_nDrawStatus != DRAW_RGNDELETE && g_nDrawStatus != DRAW_CLEANUP)
		{
			if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
			{			
				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);			
			}
		}
		
		
	}
}
//设置线型，比如实线、虚线、点划线等PS_SOLID、PS_DOT;当前系统中默认设置为PS_SOLID实线
WB_API void WB_SetWBLineType(int nWBLineType)
{
	//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_nWBLineType = nWBLineType;
	}
}

//设置线和图形的线宽，默认值为4
WB_API void WB_SetWBCurObjWidth(int nWBCurObjWidth)
{
	//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_nWBCurObjWidth = nWBCurObjWidth;
		g_pWhiteBoardView->UpdateObjLineWidth();

	}
}

//设置线冒
WB_API void WB_SetLineCap(int nStartLineCap,int nEndLineCap)
{
		//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_nStartLineCap = (LineCap)nStartLineCap;
		g_nEndLineCap = (LineCap)nEndLineCap;
		g_pWhiteBoardView->UpdateObjLineArrow();
	}
}

//水波纹特效处理
WB_API void WB_SetWaterStatus()
{
	g_bRunWater = !g_bRunWater;
}
WB_API void WB_SetCharfont()
{
	//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		CFontDialog dlg(&g_pWhiteBoardView->GetCurFont());		
		dlg.m_cf.rgbColors = g_pWhiteBoardView->GetFontColor();
		if(IDOK==dlg.DoModal())
		{
			g_pWhiteBoardView->SetTextFontColor(*(dlg.m_cf.lpLogFont), dlg.GetColor());
		}
	}
	g_pObj = NULL;
}

//设置橡皮擦尺寸
WB_API void WB_SetBrushSize(int nBrushSize)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		g_nBrushSize = nBrushSize;
	}	
}
//视图中新建一页
WB_API void WB_AddNewPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
				CScreenDrawMgr::GetIns()->UpdateWindow();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
				g_pWhiteBoardView->UpdateWindow();
			}
		}
		g_pWhiteBoardView->AddNewPage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}	
}

//视图中插入一页
WB_API void WB_InsertNewPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->InsertNewPage();
		//g_nDrawStatus = DRAW_NORMAL;
		g_pObj = NULL;
	}
}
WB_API void WB_InsertNewPage(UINT nPageIndex)
{	
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->InsertPrePage(nPageIndex);
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}
}

//视图中的删除一页
WB_API void WB_DeletePage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{

		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->DeletePage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}
}
WB_API void WB_DeletePage(UINT nPageIndex)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->DeletePrePage(nPageIndex);
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}

}
//互换页
WB_API void WB_InterchangePage(UINT nFormerPage, UINT nTargetPage)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->InterchangePage(nFormerPage, nTargetPage);
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}
}
//视图中的上一页
WB_API void WB_PrePage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
				CScreenDrawMgr::GetIns()->UpdateWindow();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
				g_pWhiteBoardView->UpdateWindow();
			}
		}
		g_pWhiteBoardView->PrePage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}

}
//视图中的下一页
WB_API void WB_NextPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
				CScreenDrawMgr::GetIns()->UpdateWindow();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
				g_pWhiteBoardView->UpdateWindow();
			}
		}
		g_pWhiteBoardView->NextPage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}

}
//视图中的第一页
WB_API void WB_FirstPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->FirstPage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}

}
//视图中的最后页
WB_API void WB_LastPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->LastPage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}

}
// 图层上一层
WB_API void WB_SuperLayer()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//对选择框架进行删除
		//g_pWBPage->ClearSelectFrameList();

		g_pWhiteBoardView->SuperLayer();

	}

}
// 图层下一层
WB_API void WB_UnderLayer()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//对选择框架进行删除
		//g_pWBPage->ClearSelectFrameList();

		g_pWhiteBoardView->UnderLayer();
	}

}
// 图层顶层
WB_API void WB_BottomLayer()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//对选择框架进行删除
		//g_pWBPage->ClearSelectFrameList();
		//g_pWhiteBoardView->Invalidate();

		g_pWhiteBoardView->BottomLayer();
	}

}
// 图层底层
WB_API void WB_TopLayer()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//对选择框架进行删除
		//g_pWBPage->ClearSelectFrameList();
		//g_pWhiteBoardView->Invalidate();

		g_pWhiteBoardView->TopLayer();
	}

}


// 添加图片
WB_API void WB_AddPicture()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDPICTURE;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);
			
		}
		g_pWhiteBoardView->AddPicture();
				
	}
}

//添加图片
WB_API BOOL WB_AddPicture(const char *pFilePathW,CRect rect /*= CRect(0,0,0,0)*/)
{
	if (NULL == g_pWhiteBoardView || pFilePathW == NULL)
	{
		return FALSE;
	}
	else
	{
		CString pFilePath = ToDRAWStr(pFilePathW).c_str();
		if (pFilePath.IsEmpty())
		{
			return FALSE;
		}
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDPICTURE;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		if (g_pWhiteBoardView->AddPicture(pFilePath,rect))
		{
			return TRUE;
		}		
	}

	return FALSE;
}

// 添加新视频（jiabangping 20110804）
WB_API void WB_AddMedia()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDNEWMOVIE;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		g_pWhiteBoardView->AddMedia();
				
	}
}

WB_API BOOL WB_AddMedia(const char *pFilePathW)
{
	if (NULL == g_pWhiteBoardView || pFilePathW == NULL)
	{
		return FALSE;
	}
	else
	{
		CString pFilePath = A2U(pFilePathW);
		if (pFilePath.IsEmpty())
		{
			return FALSE;
		}
		//CString pFilePath = CA2W(pFilePathW);//ToDRAWStr(pFilePathW).c_str();
		
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDNEWMOVIE;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		if (g_pWhiteBoardView->AddMedia(pFilePath))
		{
			return TRUE;
		}	
	}
	return FALSE;
}

WB_API void WB_AddCamera()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDCAMERA;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		g_pWhiteBoardView->AddCamera();	
	}
}

// 插入文档
WB_API void WB_AddDoc(CRect rect/* = CRect(0, 0, 0, 0)*/)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDDOC;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		g_pWhiteBoardView->AddDoc(rect);	
	}
}

WB_API BOOL WB_AddDoc(const char *pFilePathW,CRect rect /*= CRect(0, 0, 0, 0)*/)
{
	if (NULL == g_pWhiteBoardView || pFilePathW == NULL)
	{
		return FALSE;
	}
	else
	{
		CString pFilePath = A2U(pFilePathW);
		if (pFilePath.IsEmpty())
		{
			return FALSE;
		}
		//CString pFilePath = ToDRAWStr(pFilePathW).c_str();
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDDOC;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		if (g_pWhiteBoardView->AddDoc(pFilePath, rect))
		{
			return TRUE;
		}		
	}
	return FALSE;
}

// 删除
WB_API void WB_DeleteSelectedObjects()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_nDrawStatus = DRAW_NORMAL; //选择状态
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)//有选中的对象时才执行删除操作
		{
			g_pWhiteBoardView->DeleteSelectedObjects();
			g_pWhiteBoardView->Invalidate(TRUE);
				
		}		
	}
}
// 加载资源库
WB_API void WB_OpenResource()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}

		g_pWhiteBoardView->OpenResource();
	}

}

//加载资源库管理

//加载flash
WB_API void WB_AddFlash()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDFLASH;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		g_pWhiteBoardView->AddFlash();
	}
}

WB_API BOOL WB_AddFlash(const char *pFilePathW,CRect rect /*= CRect(0,0,0,0)*/)
{
	if (NULL == g_pWhiteBoardView || pFilePathW == NULL)
	{
		return FALSE;
	}
	else
	{
		CString pFilePath = A2U(pFilePathW);
		if (pFilePath.IsEmpty())
		{
			return FALSE;
		}
		//CString pFilePath = ToDRAWStr(pFilePathW).c_str();

		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDFLASH;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		if (g_pWhiteBoardView->AddFlash(pFilePath,rect))
		{
			return TRUE;
		}		
	}
	return FALSE;
}

//加载Dxf
WB_API BOOL WB_AddDxf(const char *pFilePathW, CRect rect /*= CRect(0,0,0,0)*/)
{
	if (NULL == g_pWhiteBoardView || pFilePathW == NULL)
	{
		return FALSE;
	}
	else
	{
		CString pFilePath = A2U(pFilePathW);
		if(pFilePath.IsEmpty())
		{
			return FALSE;
		}
		g_pObj = NULL;
		g_pWhiteBoardView->DeleteEdit();
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDDXF;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);
		}
		if (g_pWhiteBoardView->AddDxf(pFilePath,rect))
		{
			return TRUE;
		}		
	}
	return FALSE;
}

//保存
WB_API void WB_SaveWBFile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		if (!g_strCurFilePath.IsEmpty())
		{
			if (!g_pWhiteBoardView->SaveLines(g_strCurFilePath))
			{
				if (g_pWhiteBoardView == NULL)
				{
					return;
				}
				g_strCurFilePath.Empty();
			}
			return;
		}

		CString strFilePath;
		if (g_bSupportU7OpenFile)
		{
			//wchar_t szFilter[MAX_PATH];
			CString strFilter = _T("UCB Files (*.ucb)|*.ucb||");
			//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);
			CString strfilterName;
			strfilterName = g_strAvconUserID;
			SYSTEMTIME sys; 
			GetLocalTime( &sys );
			CString strTime;
			strTime.Format(_T("%d%d%d%d%d"),sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute);
			strfilterName += strTime;
			char* sFilePath = new char[MAX_PATH];
				memset(sFilePath,0,MAX_PATH);
			bool bOpen = g_pHPWBDataNotify->OpenSaveFile(FALSE,sFilePath, NULL, strFilter, g_pWhiteBoardView->GetSafeHwnd(),strfilterName);
			if (g_pWhiteBoardView == NULL)
			{
				return;
			}
			if (bOpen)
			{
				strFilePath = A2U(sFilePath);
				//strFilePath = ToDRAWStr(sFilePath).c_str();
			}
			delete[] sFilePath;

			if (strFilePath.IsEmpty())
			{
				return;
			}

			int nLen = strFilePath.GetLength();
			TCHAR* chFilePath = new TCHAR[nLen + 1];
			_tcscpy(chFilePath, strFilePath);
			PathRemoveFileSpec(chFilePath); //得到保存的路径名，不包括文件名
			if (!PathIsDirectory(chFilePath)) //判断路径是否是一个有效的目录
			{
				if(!CreateDirectory(chFilePath, NULL)) //根据路径创建文件夹
				{
					CString strPrompt;
					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
					if (strPrompt.IsEmpty())
					{
						strPrompt = _T("提示");
					}
					CString strFolderCreateFail;
					strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
					if (strFolderCreateFail.IsEmpty())
					{
						strFolderCreateFail = _T("文件夹创建失败！");
					}
					delete[] chFilePath;
					if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
					{
						//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
						g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
					}
					else
					{
						AfxMessageBox(strFolderCreateFail);
					}
					return;
				}
			}
			delete[] chFilePath;

			if (PathFileExists(strFilePath)) //判断文件是否存在
			{
				DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength())); //删除文件
				strFilePath.ReleaseBuffer();
			}
		} 
		else
		{
			CFileDialog dlgFile(FALSE, _T("ucb"), NULL, OFN_HIDEREADONLY , _T("UCB Files (*.ucb)|*.ucb||"), NULL, 0, FALSE);

			dlgFile.m_ofn.lpstrTitle = _T("保存为");

			if (dlgFile.DoModal() == IDOK)
			{
				if (!g_pWhiteBoardView)
				{
					return ;
				}
				strFilePath = dlgFile.GetPathName();
				if (PathFileExists(strFilePath))//如果文件存在就删除这个文件
				{
					DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength()));
					strFilePath.ReleaseBuffer();
				}
			}
			else
			{
				return;
			}
		}

		if (g_pWhiteBoardView->SaveLines(strFilePath))
		{
			g_strCurFilePath = strFilePath;
		}
	}
}

WB_API void WB_SaveWBFile( const char* pFilePathW )
{
	if (pFilePathW == NULL || g_pWhiteBoardView == NULL)
	{
		return;
	}
	else
	{
		CString strFilePath = ToDRAWStr(pFilePathW).c_str();
		if (strFilePath.IsEmpty())
		{
			return;
		}

		int nLen = strFilePath.GetLength();
		TCHAR* chFilePath = new TCHAR[nLen + 1];
		_tcscpy(chFilePath, strFilePath);
		PathRemoveFileSpec(chFilePath); //得到保存的路径名，不包括文件名
		if (!PathIsDirectory(chFilePath)) //判断路径是否是一个有效的目录
		{
			if(!CreateDirectory(chFilePath, NULL)) //根据路径创建文件夹
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("提示");
				}
				CString strFolderCreateFail;
				strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
				if (strFolderCreateFail.IsEmpty())
				{
					strFolderCreateFail = _T("文件夹创建失败！");
				}
				delete[] chFilePath;
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
				}
				else
				{
					AfxMessageBox(strFolderCreateFail);
				}
				return;
			}
		}
		delete[] chFilePath;

		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		
		if (PathFileExists(strFilePath))//如果文件存在就删除这个文件
		{
			DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength()));
			strFilePath.ReleaseBuffer();
		}
		if (g_pWhiteBoardView->SaveLines(strFilePath))
		{
			g_strCurFilePath = strFilePath;
		}
	}	
}

//保存成图片
WB_API void WB_SaveWBFileToPicture()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	g_pObj = NULL;
	g_pWhiteBoardView->DeleteEdit();
	//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
	if (g_pWBPage->GetWBSelectFrameList().size() > 0)
	{
		//1-17对选择框架进行删除和当前选择图层号置0	
		g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			g_pWhiteBoardView->Invalidate();
		}
	}

	//文件对话框显示
	g_pWhiteBoardView->GetWBSavePictureDlg();
	
	return;
	//隐藏文件对话框中保存为图片
	/*g_pWhiteBoardView->GetWBSavePictureDlg()->ShowWindow(SW_HIDE);*/

}
WB_API void WB_SaveWBFileToPicture(const char* pFilePath)
{
	;		
}
//另存
WB_API void WB_Resavefile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}

		CString strFilePath;
		if (g_bSupportU7OpenFile)
		{
			//wchar_t szFilter[MAX_PATH];
			CString strFilter = _T("UCB Files (*.ucb)|*.ucb||");
			//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);
			CString strfilterName;
			strfilterName = g_strAvconUserID;
			SYSTEMTIME sys; 
			GetLocalTime( &sys );
			CString strTime;
			strTime.Format(_T("%d%d%d%d%d"),sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute);
			strfilterName += strTime;
			char* sFilePath = new char[MAX_PATH];
			bool bOpen = g_pHPWBDataNotify->OpenSaveFile(FALSE,sFilePath, NULL, (LPCTSTR)strFilter, g_pWhiteBoardView->GetSafeHwnd(),strfilterName);
			if (g_pWhiteBoardView == NULL)
			{
				return;
			}
			if (bOpen)
			{
				strFilePath = A2U(sFilePath);
				//strFilePath = ToDRAWStr(sFilePath).c_str();
			}
			delete[] sFilePath;

			if (strFilePath.IsEmpty())
			{
				return;
			}

			int nLen = strFilePath.GetLength();
			TCHAR* chFilePath = new TCHAR[nLen + 1];
			_tcscpy(chFilePath, strFilePath);
			PathRemoveFileSpec(chFilePath); //得到保存的路径名，不包括文件名
			if (!PathIsDirectory(chFilePath)) //判断路径是否是一个有效的目录
			{
				if(!CreateDirectory(chFilePath, NULL)) //根据路径创建文件夹
				{
					CString strPrompt;
					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
					if (strPrompt.IsEmpty())
					{
						strPrompt = _T("提示");
					}
					CString strFolderCreateFail;
					strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
					if (strFolderCreateFail.IsEmpty())
					{
						strFolderCreateFail = _T("文件夹创建失败！");
					}
					delete[] chFilePath;
					if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
					{
						//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
						g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
					}
					else
					{
						AfxMessageBox(strFolderCreateFail);
					}
					return;
				}
			}
			delete[] chFilePath;

			if (PathFileExists(strFilePath)) //判断文件是否存在
			{
				DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength())); //删除文件
				strFilePath.ReleaseBuffer();
			}
		} 
		else
		{
			CFileDialog dlgFile(FALSE, _T("ucb"), NULL, OFN_HIDEREADONLY /*| OFN_OVERWRITEPROMPT*/, _T("UCB Files (*.ucb)|*.ucb||"), NULL, 0, FALSE);

			if (dlgFile.DoModal() == IDOK)
			{
				if (!g_pWhiteBoardView)
				{
					return;
				}
				strFilePath = dlgFile.GetPathName();
				if (PathFileExists(strFilePath))//如果文件存在就删除这个文件
				{
					DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength()));
					strFilePath.ReleaseBuffer();
				}
			}
			else
			{
				return;
			}
		}

		if (g_pWhiteBoardView->SaveLines(strFilePath))
		{
			g_strCurFilePath = strFilePath;
		}
	}
}

WB_API void WB_Resavefile( const char* pFilePathW )
{
	if (NULL == g_pWhiteBoardView || pFilePathW == NULL)
	{
		return;
	}
	else
	{
		CString strFilePath = ToDRAWStr(pFilePathW).c_str();
		if (strFilePath.IsEmpty())
		{
			return;
		}

		int nLen = strFilePath.GetLength();
		TCHAR* chFilePath = new TCHAR[nLen + 1];
		_tcscpy(chFilePath, strFilePath);
		PathRemoveFileSpec(chFilePath); //得到保存的路径名，不包括文件名
		if (!PathIsDirectory(chFilePath)) //判断路径是否是一个有效的目录
		{
			if(!CreateDirectory(chFilePath, NULL)) //根据路径创建文件夹
			{
				delete[] chFilePath;
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("提示");
				}
				CString strFolderCreateFail;
				strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
				if (strFolderCreateFail.IsEmpty())
				{
					strFolderCreateFail = _T("文件夹创建失败！");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
				}
				else
				{
					AfxMessageBox(strFolderCreateFail);
				}
				return;
			}
		}
		delete[] chFilePath;

		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}

		if (PathFileExists(strFilePath))//如果文件存在就删除这个文件
		{
			DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength()));
			strFilePath.ReleaseBuffer();
		}
		if (g_pWhiteBoardView->SaveLines(strFilePath))
		{
			g_strCurFilePath = strFilePath;
		}
	}
}

//打印
WB_API void WB_Printfile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_pWhiteBoardView->PrintPage();
		//CDC *pDC =g_whiteboardDlg->GetDC();
		//UINT FromPage,ToPage;
		//CWBPage * pWBPrintPage;
		//int ScreenX = pDC->GetDeviceCaps(LOGPIXELSX);
		//int ScreenY = pDC->GetDeviceCaps(LOGPIXELSY);
		//CPrintDialog m_PrintDlg(FALSE,PD_ALLPAGES| PD_USEDEVMODECOPIES  ,NULL);
		//FromPage = m_PrintDlg.m_pd.nMinPage = m_PrintDlg.m_pd.nFromPage = 1;
		//ToPage = m_PrintDlg.m_pd.nMaxPage = m_PrintDlg.m_pd.nToPage = g_pWBFile->GetPageCount();	
		///*LPDEVMODE dv = new DEVMODE;
		////dv = m_PrintDlg.GetDevMode();
		////m_PrintDlg.m_pd.hDevMode
		//dv->dmOrientation = DMORIENT_LANDSCAPE;
		//m_PrintDlg.m_pd.hDevMode = dv;*/
		////m_PrintDlg.ResetDC(dv);

		//if(m_PrintDlg.DoModal() == IDOK)
		//{
		//	CDC pdc;
		//	pdc.Attach(m_PrintDlg.GetPrinterDC());
		//	if(m_PrintDlg.m_pd.nFromPage>m_PrintDlg.m_pd.nMinPage)
		//		FromPage = m_PrintDlg.m_pd.nFromPage;
		//	if(m_PrintDlg.m_pd.nToPage < m_PrintDlg.m_pd.nMaxPage)
		//		ToPage =  m_PrintDlg.m_pd.nToPage;
		//	for(UINT PageContr = FromPage;PageContr <= ToPage; ++PageContr)
		//	{
		//		pWBPrintPage =g_pWBFile->GetPage(PageContr);

		//		g_pWhiteBoardView->PrintPage(&pdc,TRUE,ScreenX,ScreenY,pWBPrintPage);  
		//	}
		//}
		//else
		//{
		//	return;
		//}
		//ReleaseDC(g_pWhiteBoardView->m_hWnd, pDC->m_hDC);
	}

}

//打开文件
WB_API void WB_Openfile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		//g_IsSelectObj = FALSE;//释放选择
		//判断文件是否需要保存 wangxin 2011-01-11
		if (!g_strCurFilePath.IsEmpty())
		{
			CString strIsCurFileSave;
 			strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
 			if (strIsCurFileSave.IsEmpty())
 			{
 				strIsCurFileSave = _T("是否需要保存此次的文件?");
 			}

			//strIsCurFileSave = _T("是否需要保存此次的文件");
			int nChoose;
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsCurFileSave, MB_YESNO,false);
				/*nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsCurFileSave+g_strCurFilePath+_T("?"), MB_YESNOCANCEL,false);*/
			}
			else
			{
				/*nChoose = AfxMessageBox(strIsCurFileSave+g_strCurFilePath+_T("?"), MB_YESNOCANCEL);*/
				nChoose = AfxMessageBox(strIsCurFileSave+_T("?"), MB_YESNO);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{	
			}
			else
			{
				return;
			}
		}
		else
		{
			int nChoose = 0;

			CString strIsSave;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsSave, MB_YESNO,false);
			}
			else
			{
				nChoose = AfxMessageBox(strIsSave,MB_YESNO);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{
			}
			else
			{
				return;
			}
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		//删除视频链表
		g_pWhiteBoardView->RemoveAllMediaListDate();

		CString strFilePath;
		if (g_bSupportU7OpenFile)
		{
			//wchar_t szFilter[MAX_PATH];
			CString strFilter = _T("UCB Files (*.ucb)|*.ucb||");
			//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);

			char* sFilePath = new char[MAX_PATH];
			bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE,sFilePath, NULL, strFilter, g_pWhiteBoardView->GetSafeHwnd());
			if (g_pWhiteBoardView == NULL)
			{
				return;
			}
			if (bOpen)
			{
				strFilePath = A2U(sFilePath);
				//strFilePath = ToDRAWStr(sFilePath).c_str();
			}
			delete[] sFilePath;

			if (strFilePath.IsEmpty())
			{
				return;
			}
			if (!PathFileExists(strFilePath))
			{
				CString strFilePathError;
				strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
				if (strFilePathError.IsEmpty())
				{
					strFilePathError = _T("文件路径输入有误！");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strFilePathError, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
				}
				else
				{
					AfxMessageBox(strFilePathError,MB_OK);
				}
				return;
			}
			g_pWhiteBoardView->m_bLocalFileOperate = true;
			g_pWhiteBoardView->CloseLines();
			//发送关闭标注的同步消息
			g_pWhiteBoardView->SendScreenDraw();
		} 
		else
		{
			CFileDialog dlgFile(TRUE, _T("ucb"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("UCB Files (*.ucb)|*.ucb||"), NULL, 0, FALSE);

			if (dlgFile.DoModal() == IDOK)
			{
				if (!g_pWhiteBoardView)
				{
					return ;
				}
				strFilePath = dlgFile.GetPathName();
				CString strFilePathError;
				strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
				if (strFilePathError.IsEmpty())
				{
					strFilePathError = _T("文件路径输入有误！");
				}
				if (!PathFileExists(strFilePath))
				{
					AfxMessageBox(strFilePathError,MB_OK);
					return;
				}
				g_pWhiteBoardView->m_bLocalFileOperate = true;
				g_pWhiteBoardView->CloseLines();
				//发送关闭标注的同步消息
				g_pWhiteBoardView->SendScreenDraw();
			}
			else
			{
				return;
			}
		}
		bool bAvconlock = g_bAvconLock;
		WB_AvconLock(true);
		g_pWhiteBoardView->CreateLoadingPictureDlg();
		if (g_pWhiteBoardView->LoadLines(strFilePath))
		{
			g_strCurFilePath = strFilePath;
			g_nDrawStatus = DRAW_NORMAL;
			//文件打开新页
			if (g_pWhiteBoardView->IsOpenPreviewPanel())
			{
				g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();		
			}
			else
			{
				g_pWhiteBoardView->m_bLocalFileOperate = false;
			}
		}
		g_pWhiteBoardView->DestoryLoadingPictureDlg();
		WB_AvconLock(bAvconlock);
	}
}

WB_API void WB_Combinefile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		//g_IsSelectObj = FALSE;//释放选择
		//判断文件是否需要保存 wangxin 2011-01-11

		if (!g_strCurFilePath.IsEmpty())
		{
			CString strIsCurFileSave;
 			strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
 			if (strIsCurFileSave.IsEmpty())
 			{
 				strIsCurFileSave = _T("是否需要保存此次的文件?");
 			}

			
			int nChoose;
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				/*nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsCurFileSave+g_strCurFilePath+_T("?"), MB_YESNOCANCEL,false);*/
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsCurFileSave, MB_YESNO,false);
			}
			else
			{
				nChoose = AfxMessageBox(strIsCurFileSave+_T("?"), MB_YESNO);
				/*nChoose = AfxMessageBox(strIsCurFileSave+g_strCurFilePath+_T("?"), MB_YESNOCANCEL);*/
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{	
			}
			else
			{
				return;
			}
		}
		else
		{
			int nChoose = 0;
			CString strIsSave;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsSave, MB_YESNO,false);
			}
			else
			{
				nChoose = AfxMessageBox(strIsSave,MB_YESNO);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{
			}
			else
			{
				return;
			}
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		//删除视频链表
		//g_pWhiteBoardView->RemoveAllMediaListDate();

		CString strFilePath = _T("");
		if (g_bSupportU7OpenFile)
		{
			//wchar_t szFilter[MAX_PATH];
			CString strFilter = _T("UCB Files (*.ucb)|*.ucb||");
			//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);

			char* sFilePath = new char[MAX_PATH];
			memset(sFilePath,0,MAX_PATH);
			bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE, sFilePath, NULL, strFilter, g_pWhiteBoardView->GetSafeHwnd());
			if (g_pWhiteBoardView == NULL)
			{
				return;
			}
			if (bOpen)
			{
				if (sFilePath != NULL)
				{
					strFilePath = A2U(sFilePath);
					//strFilePath = ToDRAWStr(sFilePath).c_str();
				}
			}
			delete[] sFilePath;

			if (strFilePath.IsEmpty())
			{
				return;
			}
			if (!PathFileExists(strFilePath))
			{
				CString strFilePathError;
				strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
				if (strFilePathError.IsEmpty())
				{
					strFilePathError = _T("文件路径输入有误！");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strFilePathError, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
				}
				else
				{
					AfxMessageBox(strFilePathError,MB_OK);
				}
				return;
			}
			g_pWhiteBoardView->m_bLocalFileOperate = true;
		//	g_pWhiteBoardView->CloseLines();
			//发送关闭标注的同步消息
		//	g_pWhiteBoardView->SendScreenDraw();
		} 
		else
		{
			CFileDialog dlgFile(TRUE, _T("ucb"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("UCB Files (*.ucb)|*.ucb||"), NULL, 0, FALSE);

			if (dlgFile.DoModal() == IDOK)
			{
				if (!g_pWhiteBoardView)
				{
					return ;
				}
				strFilePath = dlgFile.GetPathName();
				CString strFilePathError;
				strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
				if (strFilePathError.IsEmpty())
				{
					strFilePathError = _T("文件路径输入有误！");
				}
				if (!PathFileExists(strFilePath))
				{
					AfxMessageBox(strFilePathError,MB_OK);
					return;
				}
				g_pWhiteBoardView->m_bLocalFileOperate = true;
			}
			else
			{
				return;
			}
		}

		
		bool bAvconlock = g_bAvconLock;
		WB_AvconLock(true);
		g_pWhiteBoardView->CreateLoadingPictureDlg();
		if (g_pWhiteBoardView->LoadLines(strFilePath, true))
		{
			g_strCurFilePath = strFilePath;
			g_nDrawStatus = DRAW_NORMAL;
			//文件打开新页
			if (g_pWhiteBoardView->IsOpenPreviewPanel())
			{
				g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();			
			}
			else
			{
				g_pWhiteBoardView->m_bLocalFileOperate = false;
			}
		}
		g_pWhiteBoardView->DestoryLoadingPictureDlg();
		WB_AvconLock(bAvconlock);
	}
}

WB_API void WB_Openfile( const char* pFilePathW )
{
	if (NULL == g_pWhiteBoardView || NULL== pFilePathW)
	{
		return;
	}
	else
	{
		CString strFilePath = ToDRAWStr(pFilePathW).c_str();
		if (strFilePath.IsEmpty())
		{
			return;
		}
		if (!PathFileExists(strFilePath))
		{
			CString strPrompt;
			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
			if (strPrompt.IsEmpty())
			{
				strPrompt = _T("提示");
			}
			CString strIsSave;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			CString strFilePathError;
			strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
			if (strFilePathError.IsEmpty())
			{
				strFilePathError = _T("文件路径输入有误！");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsSave, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strIsSave);
			}
			else
			{
				AfxMessageBox(strFilePathError,MB_OK);
			}
			return;
		}

		g_pObj = NULL;

		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		//删除视频链表
		g_pWhiteBoardView->RemoveAllMediaListDate();

		g_pWhiteBoardView->m_bLocalFileOperate = true;
 		g_pWhiteBoardView->CloseLines();
		//发送关闭标注的同步消息
		g_pWhiteBoardView->SendScreenDraw();
		bool bAvconlock = g_bAvconLock;
		WB_AvconLock(true);
		if (!g_bSupportI7)
			g_pWhiteBoardView->CreateLoadingPictureDlg();
		if (g_pWhiteBoardView->LoadLines(strFilePath))
		{
			g_strCurFilePath = strFilePath;
			g_nDrawStatus = DRAW_NORMAL;
			//文件打开新页
			if (g_pWhiteBoardView->IsOpenPreviewPanel())
			{
				g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();			
			}
			else
			{
				g_pWhiteBoardView->m_bLocalFileOperate = false;
			}
		}
		g_pWhiteBoardView->ClearUndo();
		g_pWhiteBoardView->DestoryLoadingPictureDlg();
		if (g_bSupportI7)
		{
			WB_SetWBPageColor(238, 238, 238);
		}
		WB_AvconLock(bAvconlock);
	}
}

//新建文件
WB_API void WB_Newfile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		//判断文件是否需要保存 wangxin 2011-01-11
		if (!g_strCurFilePath.IsEmpty())
		{
			CString strIsCurFileSave;
			strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsCurFileSave"));
			if (strIsCurFileSave.IsEmpty())
			{
				strIsCurFileSave = _T("是否需要将此次的文件保存到");
			}
			int nChoose;
			CString strSavePath;
			strSavePath = strIsCurFileSave+g_strCurFilePath;
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strSavePath +_T("?"), MB_YESNOCANCEL,false);
			}
			else
			{
				nChoose = AfxMessageBox(strSavePath+_T("?"),MB_YESNOCANCEL);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{					
			}
			else
			{
				return;
			}
		}
		else
		{
			int nChoose;
			CString strIsSave;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNOCANCEL,false);
			}
			else
			{
				nChoose = AfxMessageBox(strIsSave,MB_YESNOCANCEL);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{	
			}
			else
			{
				return;
			}
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		//删除视频链表
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		g_pWhiteBoardView->CloseLines();
		//发送关闭标注的同步消息
		g_pWhiteBoardView->SendScreenDraw();
		g_strCurFilePath.Empty();

		g_pObj = NULL;
		//关闭文件预览操作
		/*		g_pWhiteBoardView->m_IsClosefile = TRUE; */
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		if (g_pWhiteBoardView->IsOpenPreviewPanel())
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();
		}
		else
		{
			g_pWhiteBoardView->m_bLocalFileOperate = false;
		}
	}  
	return;
}

WB_API void WB_Newfile(bool bSaveFile)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		if (bSaveFile)
		{
			//判断文件是否需要保存 wangxin 2011-01-11
			if (!g_strCurFilePath.IsEmpty())
			{
				CString strIsCurFileSave;
				strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsCurFileSave"));
				if (strIsCurFileSave.IsEmpty())
				{
					strIsCurFileSave = _T("是否需要将此次的文件保存到");
				}
				int nChoose;
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsCurFileSave+g_strCurFilePath+_T("?"), MB_YESNOCANCEL,false);
				}
				else
				{
					nChoose = AfxMessageBox(strIsCurFileSave+g_strCurFilePath+_T("?"),MB_YESNOCANCEL);
				}
				if (nChoose == IDYES)
				{
					WB_SaveWBFile();
				}
				else if (nChoose == IDNO)
				{	
				}
				else
				{
					return;
				}
			}
			else
			{
				int nChoose;
				CString strIsSave;
				strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
				if (strIsSave.IsEmpty())
				{
					strIsSave = _T("是否需要保存此次的文件？");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNOCANCEL,false);
				}
				else
				{
					nChoose = AfxMessageBox(strIsSave,MB_YESNOCANCEL);
				}
				if (nChoose == IDYES)
				{
					WB_SaveWBFile();
				}
				else if (nChoose == IDNO)
				{
				}
				else
				{
					return;
				}
			}
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		//删除视频链表
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		g_pWhiteBoardView->CloseLines();
		//发送关闭标注的同步消息
		g_pWhiteBoardView->SendScreenDraw();
		g_strCurFilePath.Empty();

		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
		//关闭文件预览操作
		/*		g_pWhiteBoardView->m_IsClosefile = TRUE; */
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		if (g_pWhiteBoardView->IsOpenPreviewPanel())
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();
		}
		else
		{
			g_pWhiteBoardView->m_bLocalFileOperate = false;
		}
	}  
	return;
}
//关闭文件
WB_API void WB_Closefile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		//判断文件是否需要保存 wangxin 2011-01-11
		if (!g_strCurFilePath.IsEmpty())
		{
			int nChoose;
			CString strIsSave;
// 			strIsSave = _T("是否关闭当前白板文件？");
// 			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 			{
// 				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNO,true);
// 			}
// 			else
// 			{
// 				nChoose = AfxMessageBox(strIsSave,MB_YESNO);
// 			}
// 			if (nChoose == IDNO)
// 			{
// 				return;
// 			}
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNO,true);
			}
			else
			{
				nChoose = AfxMessageBox(strIsSave,MB_YESNO);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{
			}
			else
			{
				return;
			}
		}
		else
		{
			CString strIsSave;
			int nChoose;
// 			strIsSave = _T("是否关闭当前白板文件？");
// 			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 			{
// 				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNO,true);
// 			}
// 			else
// 			{
// 				nChoose = AfxMessageBox(strIsSave,MB_YESNO);
// 			}
// 			if (nChoose == IDNO)
// 			{
// 				return;
// 			}
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}			
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNO,true);
			}
			else
			{
				nChoose = AfxMessageBox(strIsSave,MB_YESNO);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{
			}
			else
			{
				return;
			}
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		//将标准的页号改为1
		g_pWhiteBoardView->SetNeedSetPageIndexForScreenDraw(true);
		WB_DestroyWnd();

		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}

		//删除视频链表
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		g_pWhiteBoardView->CloseLines();
		g_strCurFilePath.Empty();
		//将标准的页号为获取当前页号
		g_pWhiteBoardView->SetNeedSetPageIndexForScreenDraw(false);
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
		//关闭文件预览操作
		/*		g_pWhiteBoardView->m_IsClosefile = TRUE; */
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		if (g_pWhiteBoardView->IsOpenPreviewPanel())
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();
		}
		else
		{
			g_pWhiteBoardView->m_bLocalFileOperate = false;
		} 
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
	}
	return;
}

WB_API void WB_Closefile(bool bSaveFlie)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		if (bSaveFlie)
		{
			CString strPrompt;
			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
			if (strPrompt.IsEmpty())
			{
				strPrompt = _T("提示");
			}
			//判断文件是否需要保存 wangxin 2011-01-11
			if (!g_strCurFilePath.IsEmpty())
			{
				CString strIsCurFileSave;
				strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsCurFileSave"));
				if (strIsCurFileSave.IsEmpty())
				{
					strIsCurFileSave = _T("是否需要将此次的文件保存到");
				}
				int nChoose;
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt,strIsCurFileSave+g_strCurFilePath+_T("?"), MB_YESNOCANCEL,false);
				}
				else
				{
					nChoose = AfxMessageBox(strIsCurFileSave+g_strCurFilePath+_T("?"),MB_YESNOCANCEL);
				}
				if (nChoose == IDYES)
				{
					WB_SaveWBFile();
				}
				else if (nChoose == IDNO)
				{	
				}
				else
				{
					return;
				}
			}
			else
			{
				int nChoose;
				CString strIsSave;
				strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
				if (strIsSave.IsEmpty())
				{
					strIsSave = _T("是否需要保存此次的文件？");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNOCANCEL,false);
				}
				else
				{
					nChoose = AfxMessageBox(strIsSave,MB_YESNOCANCEL);
				}
				
				if (nChoose == IDYES)
				{
					WB_SaveWBFile();
				}
				else if (nChoose == IDNO)
				{	
				}
				else
				{
					return;
				}
			}
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		//删除视频链表
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		g_pWhiteBoardView->CloseLines();
		//发送关闭标注的同步消息
		g_pWhiteBoardView->SendScreenDraw();
		g_strCurFilePath.Empty();

		g_pObj = NULL;
		//关闭文件预览操作
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		if (g_pWhiteBoardView->IsOpenPreviewPanel())
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();
		}
		else
		{
			g_pWhiteBoardView->m_bLocalFileOperate = false;
		}
	}  
	return;
}

//退出单机版白板
WB_API BOOL WB_SingleExit()
{
	if (NULL == g_pWhiteBoardView)
	{
		return FALSE;
	}
	else
	{
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		if (!g_strCurFilePath.IsEmpty())
		{
			CString strIsCurFileSave;
			strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsCurFileSave"));
			if (strIsCurFileSave.IsEmpty())
			{
				strIsCurFileSave = _T("是否需要将此次的文件保存到");
			}
			int nChoose;
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsCurFileSave+g_strCurFilePath+_T("?"), MB_YESNOCANCEL,false);
			}
			else
			{
				nChoose = AfxMessageBox(strIsCurFileSave+g_strCurFilePath+_T("?"),MB_YESNOCANCEL);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			int nChoose;
			CString strIsSave;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNOCANCEL,false);
			}
			else
			{
				nChoose = AfxMessageBox(strIsSave,MB_YESNOCANCEL);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{
			}
			else
			{
				return FALSE;
			}
		}
		if (g_pWhiteBoardView == NULL)
		{
			return FALSE;
		}
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		//删除视频链表
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = false;
		g_pWhiteBoardView->CloseLines();
		g_strCurFilePath.Empty();

		g_pObj = NULL;
		//关闭文件预览操作
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		if (g_pWhiteBoardView->IsOpenPreviewPanel())
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();
		}
		else
		{
			g_pWhiteBoardView->m_bLocalFileOperate = false;
		}
	}
	return TRUE;
}

WB_API BOOL WB_SingleCloseFile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return FALSE;
	}
	else
	{
		//还原白板的最初状态
		g_pWhiteBoardView->ReductionPrimaryWB();
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("提示");
		}
		if (!g_strCurFilePath.IsEmpty())
		{
			int nChoose;
			CString strIsSaveLocal;
			strIsSaveLocal = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSaveLocal"));
			if (strIsSaveLocal.IsEmpty())
			{
				strIsSaveLocal = _T("白板会议互动已连接，是否需要将本地文件保存到");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSaveLocal+g_strCurFilePath+_T("?"), MB_YESNO,false);
			}
			else
			{
				nChoose = AfxMessageBox( strIsSaveLocal+g_strCurFilePath+_T("?"),MB_YESNO);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			int nChoose;
			CString strInteractionConnected;
			strInteractionConnected = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InteractionConnected"));
			if (strInteractionConnected.IsEmpty())
			{
				strInteractionConnected = _T("白板会议互动已连接，是否需要保存本地的文件？");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strInteractionConnected, MB_YESNO,false);
			}
			else
			{
				nChoose = AfxMessageBox(strInteractionConnected,MB_YESNO);
			}
			if (nChoose == IDYES)
			{
				WB_SaveWBFile();
			}
			else if (nChoose == IDNO)
			{
			}
			else
			{
				return FALSE;
			}
		}
		if (g_pWhiteBoardView == NULL)
		{
			return FALSE;
		}
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}

		//删除视频链表
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = false;
		g_pWhiteBoardView->CloseLines();
		g_strCurFilePath.Empty();

		g_pObj = NULL;
	}
	return TRUE;
}

WB_API BOOL WB_CloseFuntion()
{
	if (g_pWhiteBoardView == NULL)
	{
		return FALSE;
	}
	else
	{
		g_pWhiteBoardView->SendMessage(WM_CLOSEWBFUNCTIONDLG,0,0);
	}
	return TRUE;
}

//截屏
WB_API void WB_Cutscreen()
{
	if (NULL == g_whiteboardDlg || NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		CCatchScreenDlg dlg;

		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		int nResponse = dlg.DoModal();
		g_hVedioHwnd = NULL;
		if (!g_pWhiteBoardView)
		{
			return ;
		}
		if(nResponse == IDOK)
		{
			g_pWhiteBoardView->ScreenShoot(dlg.m_filePath);
		}	
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}	
}

// 退出白板对话框
WB_API void WB_Close()
{
	//暂时不适用本地硬件授权设置，所以不需要调用WriteEndTime()函数
	//WriteEndTime();
	ClipCursor(NULL);
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{	
		if (g_IsMidiVersion != NULL) //卸载MIDI库
		{
			typedef void (*lpCloseConvert)();
			lpCloseConvert CloseConvert = (lpCloseConvert)GetProcAddress(g_IsMidiVersion,"CloseConvert");
			if (CloseConvert != NULL)
			{
				CloseConvert();
			}		
		}
		
		//在调用析构之前处理的一些扫尾工作
		g_pWhiteBoardView->ViewWindowFinalTreat();	
		if (NULL != g_whiteboardDlg)
		{
			delete g_whiteboardDlg;
			g_whiteboardDlg = NULL;
			g_pWhiteBoardView = NULL;
			g_nDrawStatus = DRAW_NORMAL;
			g_pConferenceRecordNotify = NULL;
			//g_IsSelectObj = FALSE;//释放选择
			g_pObj = NULL;
			g_bRunWater = FALSE;
			g_reduce=0.00;//退出时置0
			g_strCurFilePath.Empty();
		}		 
	}
}	

// UNDO
WB_API void WB_Undo(int nStep = 1)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}		

		/*if (g_pWBFile != NULL)
		{
		if (g_pWBFile->Undo(nStep))
		{
		g_pWBPage = g_pWBFile->GetCurPage();
		//g_pWhiteBoardView->Invalidate();
		}			
		}*/

		g_pWhiteBoardView->Undo();
		g_pWhiteBoardView->Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}
}

// REDO
WB_API void WB_Redo()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}


		/*if (g_pWBFile != NULL)
		{
		g_pWBFile->Redo();
		g_pWBPage = g_pWBFile->GetCurPage();
		g_pWhiteBoardView->Invalidate();
		}*/

		g_pWhiteBoardView->Redo();
		g_pWhiteBoardView->Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		//g_IsSelectObj = FALSE;//释放选择

		if (g_pHPWBDataNotify && g_bSupportI7)
		{
			MSG msg;
			memset(&msg, 0, sizeof(MSG));
			msg.hwnd = NULL;
			msg.lParam = (LPARAM)0;
			msg.message = 1;
			msg.wParam = (WPARAM)g_pWBPage->GetWBSelectFrameList().size();
			g_pHPWBDataNotify->SetSelectObjStatue(msg);
		}

		g_pObj = NULL;
	}
}

//处理同步接收的数据
WB_API void WB_ProcessDataIn(const char *pData, int nLen)
{
	if (g_pWhiteBoardView != NULL)
	{
		g_pWhiteBoardView->OnNETEC_NodeReceivedFromRouter(pData,nLen);
	}	
}

//请求所有对象
WB_API void WB_ReqAllObj()
{
	if (g_pWhiteBoardView != NULL)
	{
		g_pWhiteBoardView->GetAllObj();
	}	
}
//发送所有对象属性
WB_API void WB_SendAllObj()
{
	if (g_pWhiteBoardView != NULL)
	{
		g_pWhiteBoardView->SendAllObj();
	}
}
//删除所有对象，当文件操作是本地操作是，就发送删除所有对象的同步消息
WB_API void WB_CloseLine()
{
	if (g_pWhiteBoardView != NULL)
	{
		g_pWhiteBoardView->CloseLines();
	}
}
WB_API bool WB_BHardwareSupportMTTouch()
{

	//放开检测硬件授权信息
	g_bAuthorizedByHandWare = true;
	return g_bAuthorizedByHandWare;
	/*-------------------------开始检测触摸功能-----------------------------*/
	//首先，通过直接检测的方式获取是否支持触摸功能；
	int value = GetSystemMetrics(SM_DIGITIZER);
	if (value & (NID_READY | NID_INTEGRATED_TOUCH | NID_MULTI_INPUT | NID_INTEGRATED_PEN))
	{
		return true;
	}
	else
	{
		return false;
	}

	BOOL bMTouchSupport = TRUE; //是否支持多点触摸功能
	BOOL bDeviceSupported = MTAUTHORIZE::IsSupportedDevice();
	g_bAuthorizedByHandWare = bMTouchSupport && bDeviceSupported;

	if (!g_bAuthorizedByHandWare)//不是我们的屏但是有授权文件且合法的话就允许他使用
	{
		//从解密库获取授权时间
		CString ReadFilePath = GetCurrentPath() + _T("\\config\\AVCONWB.wbe");
		//char Resultc[64] = {0};
		int ResultLen = 0;
		SYSTEMTIME BeginTime = {0},EndTime = {0};
		CString BeginTimeStr = _T(""),EndTimeStr = _T(""),TempStr = _T("");
		if (!PathFileExists(ReadFilePath))//授权文件不存在,直接不授权
		{
			return false;
		}

		//从解密库获取授权时间,放在Resultc数组中，
		if (!g_hDESEncrypt)
		{
			//确保动态加载的库路径正确
			HMODULE hModule = GetModuleHandle(TEXT("whiteboard.dll"));
			if (!hModule) return false;

			TCHAR strName[MAX_PATH];
			GetModuleFileName(hModule, strName, MAX_PATH);
			CString strPath = strName;
			int nPos = strPath.ReverseFind('\\');
			if(-1 == nPos) nPos = strPath.ReverseFind('/');
			if(-1 == nPos) return false;
			strPath = strPath.Left(nPos);

			CString tempPath = _T("");
			SetCurrentDirectory(strPath.GetBuffer());

			CString strResPath = _T("");

			strResPath.Format(TEXT("%s\\DESEncrypt.dll"), strPath);

			g_hDESEncrypt = LoadLibrary(strResPath.GetBuffer());
			strPath.ReleaseBuffer();
			strResPath.ReleaseBuffer();
			
		}
		
		if (NULL != g_hDESEncrypt)
		{
			//wangxiaobin我改的20130819
			char * Resultc = new char[MAX_PATH];
			memset(Resultc,0,MAX_PATH);
			//GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,(LPCTSTR)g_hDESEncrypt,&g_hDESEncrypt); 
			typedef	void (__cdecl *UnCode)(CString FilePath,char* resultData,int &resultDataLenth);
			UnCode UnencryptCode = (UnCode)GetProcAddress(g_hDESEncrypt, "_UnencryptCode");
			if (NULL != UnencryptCode)
			{
				(*UnencryptCode)(ReadFilePath,Resultc,ResultLen);
				if (ResultLen < 16)//说明用户自己改动了授权文件
				{
					delete []Resultc;
					return false;
				}
				TempStr = ToDRAWStr(Resultc).c_str();
				BeginTimeStr = TempStr.Mid(0,8);
				EndTimeStr = TempStr.Mid(8,8);
				//ChangeDateToSystemtime(Resultc,BeginTime,EndTime,ResultLen);//将字符授权时间转换成SYSTEMTIME时间
				//IsRightAuthorize(g_isLoacalWB,BeginTime,EndTime,CurLoadServerTime,PreOpenSoftTime);
				//读取上一次使用白板的结束时间
				if (g_isLoacalWB)
				{
					TCHAR Text[256] = {0};
					ReadFilePath = GetCurrentPath() + _T("\\config\\WBConfig.ini");
					GetPrivateProfileString(_T("PreEndTime"), _T("PreEndTimeName"), _T(""),Text,256,ReadFilePath);
					PreOpenSoftTimeStr = Text;
					for(int i = 0; i <PreOpenSoftTimeStr.GetLength();++i )//写进去的时候加了数值
					{
						PreOpenSoftTimeStr.SetAt(i,PreOpenSoftTimeStr.GetAt(i) - 50 -i);
					}
				}	
				g_bAuthorizedByHandWare = IsRightAuthorize(g_isLoacalWB,BeginTimeStr,EndTimeStr,CurLoadServerTimeStr,PreOpenSoftTimeStr);
			}
			if (Resultc)
			{
				delete []Resultc;
				Resultc = NULL;
			}
			
		}
		else
		{
			OutputDebugString(L"NULL == g_hDESEncrypt\n");
		}
	}
	return g_bAuthorizedByHandWare;//正规使用
}

//是否授权触摸功能,
WB_API bool WB_SetTouchAuthorization(bool bAuthorization)
{   //其中bAuthorization控制服务器授权

	//bAuthorization = TRUE; //先测试用
	
	if ( bAuthorization )
	{
		g_bIsSupportMTTouch = true;	

	}
	else
	{
		g_bIsSupportMTTouch = false;		
	}
	if (g_whiteboardDlg != NULL && ::IsWindow(g_whiteboardDlg->m_hWnd))
	{
		((CWhiteBoardDlg *)g_whiteboardDlg)->ShowIsTouchStatic();
		CWBMTObjectManager::GetIns_()->SetMTAuthorize(bAuthorization);

		//对触摸资源库的触摸功能进行授权
		WBRESWND::SetMTAuthorizeRes(g_bIsSupportMTTouch);
	}
	return bAuthorization;
}

//最终的授权情况
WB_API bool WB_GetTouchAuthorization()
{
	return g_bIsSupportMTTouch;
}

//屏幕录制
WB_API void WB_RECScreen(void)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}

		AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
		g_pWhiteBoardView->AddRECScreen();
	}
}

//挡板
WB_API void WB_Baffle(void)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->addBaffle();
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;
	}
}
//直尺
WB_API void WB_Ruler(void)
{
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	else
	{
		g_nDrawStatus = ADDRULER;
		g_pWhiteBoardView->AddRuler();
	}
}

//量角器
WB_API void WB_Protractor(void)
{
	if (g_pWhiteBoardView==NULL)
	{
		return;
	} 
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->AddProtractor();
		g_pObj = NULL;
	}
}

//三角板
WB_API void WB_SetSquare(void)
{
	if (NULL == g_pWhiteBoardView) 
	{
		return ;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->AddSetSquare();
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;

	}
}

WB_API void WB_Compass(void)
{
	if (NULL == g_pWhiteBoardView) 
	{
		return ;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->AddCompass();
		//g_IsSelectObj = FALSE;//释放选择
		g_pObj = NULL;

	}
}
//功能面板
WB_API void WB_FunctionPanel()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->addFunctionPanel();
	}
}

//设置存放临时文件的目录
WB_API void WB_SetWBTempDir(const char *pWBTempDir)
{
	g_strAvconTempPath = ToDRAWStr(pWBTempDir).c_str();
	if (!PathIsDirectory(g_strAvconTempPath))//wangxiaobin创建临时文件目录
	{
		if(!CreateDirectory(g_strAvconTempPath, NULL))
		{
			return ;
		}
	}
}

//快捷键,MFC的DLL中窗口要响应PreTranslateMessage（）函数消息，则在主工程的PreTranslateMessage（）显示的调用
WB_API BOOL  DllPreTranslateMessage(MSG *pMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.PreTranslateMessage(pMsg);
}

//修改默认线条颜色
WB_API void WB_SetDefaultLineColoe(int nRed, int nGreen, int nBlue)
{
		//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_clrWBCurObjColor = RGB(nRed, nGreen, nBlue);
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetToolbarDlg()->ReflashBtn(TYPE_DLG_COLORPALETTE);
	}

}

//设置是否填充颜色
WB_API void WB_SetIsSolid(bool isSolid)
{
	//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_bIsSolid = isSolid;
	}
}

//设置图形填充颜色clrSolid
WB_API void WB_SetSolidColor(int nRed, int nGreen, int nBlue)
{
		//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_clrSolid = RGB(nRed, nGreen, nBlue);
		g_pWhiteBoardView->UpdateObjSolidColor();


	}
}

//设置白板背景颜色g_clrWBPageColor
WB_API void WB_SetWBPageColor(int nRed, int nGreen, int nBlue)
{
		//判断是否先加载动态链接库
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//g_clrWBPageColor = RGB(nRed, nGreen, nBlue);
		if (g_pWBPage)
		{
			g_pWBPage->SetBKGColor(RGB(nRed, nGreen, nBlue));
		}
		g_pWhiteBoardView->Invalidate();
		g_pWhiteBoardView->UpdateWindow();
	}
}

//调用屏幕绘图
WB_API void WB_DrawScreen(const CRect* rect)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		if (g_pWhiteBoardView->DrawOnScreen(rect))
		{
			//发送同步信息
			g_pWhiteBoardView->SendScreenDraw();
		}
	}
}
//全屏切换
WB_API void WB_FullScreenShift(bool isFullScreen)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		if (g_pWhiteBoardView->FullScreenShift(isFullScreen))
		{
			//发送同步信息
			//g_pWhiteBoardView->SendScreenDraw();
		}
	}
}
//玻璃透明效果切换
WB_API void WB_GuiseShift(bool isTransparent)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		if (g_pWhiteBoardView->ShiftGuise(isTransparent))
		{
			//发送同步信息
			g_pWhiteBoardView->SendScreenDraw();
		}
	}
}                                                                                  
//关闭屏幕绘图窗口
WB_API void WB_DestroyWnd(bool bIsSend)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//g_pWhiteBoardView->SetBroadcastUserCursor(bIsSend);
		if (bIsSend)
		{
			if (g_pWhiteBoardView->DestroyScreenDraw())
			{
				//发送同步信息
				g_pWhiteBoardView->SendScreenDraw();
			}
		}
	}
}

WB_API void WB_ShowBroadCast(bool bShow)
{
	ClipCursor(NULL);
	
	//g_nDrawStatus = DRAW_NORMAL;
	if (NULL ==g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->SendMessage(WM_CANCELMODE, 0, 0);
		g_pWhiteBoardView->ProcessCloseColorDlg();
		g_pWhiteBoardView->SetBroadcastUserCursor(bShow); //白班不在激活屏幕传false
		if (CWBToolTip::GetIns())
		{
			CWBToolTip::GetIns()->Restore();
		}

	}
}
WB_API void WB_IsWBFullScreen( bool isFullScreen )
{
	if (g_whiteboardDlg == NULL)
	{
		return;		
	}
	else
	{		
		if (!isFullScreen)
		{
			g_pWhiteBoardView->HideUserCursor();
		}
		((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg()->SetWBFullScreen(isFullScreen);
	}

}
//判断是否全屏幕
WB_API bool WB_IsFullScreen()
{
	if(CScreenDrawMgr::GetIns() != NULL)
	{
		return CScreenDrawMgr::GetIns()->IsFullScreen();
	}
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->SendMessage(WM_CANCELMODE, 0, 0);
		g_pWhiteBoardView->ProcessCloseColorDlg();
		
	}
	return false;
}
//判断是否透明
WB_API bool WB_IsScreenDrawTrans()
{
	if(CScreenDrawMgr::GetIns() != NULL)
	{
		return CScreenDrawMgr::GetIns()->IsTransparent();
	}
	return false;
}
//打开控制条
WB_API void WB_OnCtrlDlg()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->OpenCtrlDlg();
	}
}

//外层窗口从最小化恢复时，是否要打开白板标注窗口
WB_API void WB_OpenSCDraw()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->OpenSCDrawReStore();
	}
}

//白板是否最小化了，最小化则为TRUE，否则为FALSE
WB_API void WB_SetIsMinSize(BOOL bIsMinSize)
{
	g_bProcessWater = !bIsMinSize;
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->SetIsMinSize(bIsMinSize);
		g_pWhiteBoardView->SetBroadcastUserCursor(bIsMinSize);
	}
}

//聚光灯
WB_API void WB_NewSpotlight()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->AddSpotlight();
	}

}
//鹰眼
WB_API void WB_EagleEye()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->EagleEyeFunction();
	}
}
WB_API void WB_EagleEyeSetposAndShow(CRect rect)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->EagleEyeSetposAndShow(rect);
	}
}
WB_API void WB_Maqnifier()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//  		g_pWhiteBoardView->DeleteEdit();
		//  		g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		g_pWhiteBoardView->AddMagnifier();
	}
}

//设置本地AVCON用户ID
WB_API void WB_SetLocalUserID(const char *pUserID)
{
	g_strAvconUserID = ToDRAWStr(pUserID).c_str();
#ifdef HUBEI_MEDICAL_SDK
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->GetUserColor();
	}
#endif
}

WB_API void WB_SetLocalUserName(const char* str)
{
	g_strLocalUserName = ToDRAWStr(str).c_str();
}
//AVCON用户下线通知
WB_API void WB_MemberOffline(const char *pUserID)
{
	CString sUser = _T("");
	if (pUserID == NULL)
	{
		return;
	}
	else
	{
		sUser = ToDRAWStr(pUserID).c_str();
	}
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->OnRejectBroadcastUserCursor(pUserID);//关闭该用户的光标
		g_pWhiteBoardView->OnRejectBroadcasstUserWater(pUserID); //关闭水波纹
		g_pWhiteBoardView->ClearObjectOfOffLineUser(sUser);
	}
}

//对象角度旋转，逆时针为正，顺时针为负
WB_API void WB_RotateByAngle(int angle)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->RotateByAngle(angle);
	}
}

//对象水平垂直旋转，水平为0度，垂直为90度
WB_API void WB_RotateByHV(int HVAngle)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->RotateByHV(HVAngle);
	}
}
//克隆
WB_API void WB_CloneObject()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->CloneObject();
	}
}
//无限克隆
WB_API void WB_InfiniteCloneObj()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->InfiniteCloneObj();
	}
}

//剪切
 WB_API void WB_CutObject()
 {
 	if (NULL == g_pWhiteBoardView)
 	{
 		return;
 	}
 	else
 	{
 		g_pWhiteBoardView->CutObject();
 	}
 }
//复制
 WB_API void WB_CopyObject()
 {
 	if (NULL == g_pWhiteBoardView)
 	{
 		return;
 	}
 	else
 	{
 		g_pWhiteBoardView->CopyObject();
 	}
 }
//粘贴
 WB_API void WB_PasteObject()
 {
 	if (NULL == g_pWhiteBoardView)
 	{
 		return;
 	}
 	else
 	{
 		if (!g_pWhiteBoardView->GetIsCopy() && !g_pWhiteBoardView->GetIsCut())
 		{
 			return;
 		}
 		g_pWhiteBoardView->PasteObject();
 	}
 }
//删除全部
WB_API void WB_DeleteAll()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->DeleteAll();
	}
}
//
WB_API void WB_Dustbin()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->AddDustbin();
	}
}

WB_API void WB_SetFilePathsByavcon(
	const char *pLanguageFile,
	const char *pIniPath,
	const char *pTempPath) //由AVCON提供的临时路径
{
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	if (pLanguageFile != NULL)
	{
		g_strAvconLanguageFile = ToDRAWStr(pLanguageFile).c_str();
		//Ucc中传进来的语言读取文件路径是错的
		//g_strAvconLanguageFile = pLanguageFile;
	}

	if (pIniPath != NULL)
	{
		g_strAvconIniPath = ToDRAWStr(pIniPath).c_str();
		//设置资源库中的ini目录
		WBRESWND::SetAvconIniPath(g_strAvconIniPath);
	}

	if (pTempPath != NULL)
	{
		g_strAvconTempPath = ToDRAWStr(pTempPath).c_str();
	}
}
//调用DICOM接口
WB_API void WB_AddDICOM()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDPICTURE;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		g_pWhiteBoardView->AddDicom();	
	}	
}
//调用DICOM接口,带路径
WB_API BOOL WB_AddDICOM(const char *pFilePathW,CRect rect/* = CRect(0,0,0,0)*/)
{
	if (NULL == g_pWhiteBoardView || pFilePathW == NULL)
	{
		return FALSE;
	}
	else
	{
		//CString pFilePath = ToDRAWStr(pFilePathW).c_str();
		CString pFilePath = A2U(pFilePathW);
		if (pFilePath.IsEmpty())
		{
			return FALSE;
		}
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//释放选择
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		//有对象被选中时才执行删除选择框架操作caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17对选择框架进行删除和当前选择图层号置0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				g_pWhiteBoardView->Invalidate();
			}
		}
		g_nDrawStatus = ADDPICTURE;
		if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && ::IsWindow(g_pWhiteBoardView->GetWBFunctionDlg()->GetSafeHwnd()))
		{
			::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBPEN,0,0);

		}
		if (g_pWhiteBoardView->AddDicom(pFilePath,rect))
		{
			return TRUE;
		}	
	}
	return FALSE;
}

//由AVCON设置是否锁定白板功能
WB_API void WB_AvconLock(bool bLock)
{
	if (g_pWhiteBoardView == NULL)
	{
		return ;
	}
	if (bLock)
	{
		g_pWhiteBoardView->CloseBroadcastUserCursor();
		if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg())
		{
			((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg()->OnBtnone();
		}
		

	}
	g_bAvconLock = bLock;
	if (g_pWhiteBoardView != NULL)
	{
		//对文本编辑框进行删除
		g_pWhiteBoardView->DeleteEdit();
		WB_CloseAllChildBox();
		g_pWhiteBoardView->SendMessage(WM_CANCELMODE,0,0);
		g_pWhiteBoardView->ProcessCloseColorDlg();
		g_whiteboardDlg->SendMessage(WM_LBUTTONDOWN,0,0);
		g_whiteboardDlg->SendMessage(WM_LBUTTONUP,0,0);
 		if ((g_pWhiteBoardView->GetWBFunctionDlg() != NULL) &&(g_pWhiteBoardView->GetWBFunctionDlg()->IsWindowVisible()))
 		{
			//白板被锁后隐藏功能面板			
			g_pWhiteBoardView->GetWBFunctionDlg()->ShowWindow(SW_HIDE);

			//-----------------------------//
			if ((g_pWhiteBoardView->GetWBFunctionDlg())->GetPreviewDLG())
			{
				(g_pWhiteBoardView->GetWBFunctionDlg())->GetPreviewDLG()->SendMessage(WM_CANCELMODE,0,0);
			}
			
			if ((g_pWhiteBoardView->GetWBFunctionDlg())->GetAttributeDlg())
			{
				(g_pWhiteBoardView->GetWBFunctionDlg())->GetAttributeDlg()->SetAllComboxExStatue();
			}
			
 			//(g_pWhiteBoardView->GetWBFunctionDlg())->SendMessage(WM_CANCELMODE,0,0);
  			//((CWhiteBoardDlg*)g_whiteboardDlg)->MoveWBFunctionDlg(FALSE);
  			//((CWhiteBoardDlg*)g_whiteboardDlg)->MoveOptionDlg(FALSE);
 		}
	}
	if (g_pWBPage->GetWBSelectFrameList().size() > 0)
	{
		g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			g_pWhiteBoardView->Invalidate();
		}
	}

	WBRESWND::SetAvconLock(bLock);
	if (CWBToolTip::GetIns())
	{
		CWBToolTip::GetIns()->Restore();
	}
	//WBRESWND::SetMTAuthorizeRes(!bLock);
	//((CWhiteBoardDlg *)g_whiteboardDlg)->ShowAllToolbars(!bLock);
}

//由AVCON设置开始或结束会议录像
WB_API BOOL WB_StartConferenceRecord(bool bStart = true)
{
	if (g_pWhiteBoardView == NULL)
	{
		return FALSE;
	}
	if (g_pConferenceRecordNotify == NULL)
		return FALSE;

	g_bIsConferenceRecord = bStart;
	if (bStart)
	{
		//发送当前页的所有对象给录像 并设m_bStartConferenceRecord为true,在U7中不需要
		if (!g_bSupportU7OpenFile)
		{
			Sleep(5000);
		}
		
		SendMessage(g_pWhiteBoardView->m_hWnd,WM_SENDPAGEDATATO_CONFERENCE_RECORD,1,0);

	}
	else
	{
		//每一页都把m_bStartConferenceRecord设为false
		g_pWBFile->ReductionList();
	}
	return TRUE;
}

//设置会议录像指针
WB_API void WB_SetConferenceRecordNotify(IHPWBNotify *pConferenceRecordNotify)
{
	if (g_pWhiteBoardView == NULL)
	{
		return ;
	}
	g_pConferenceRecordNotify = (IHPWBNotify *)pConferenceRecordNotify;
}

//设置白板的版本类型
WB_API void WB_SetTypeOfWB( WB_TYPE typeOfWB )
{
	g_WBType = typeOfWB;
}
//关闭所有子工具栏
WB_API void WB_CloseAllChildBox()
{
	if ((CWhiteBoardDlg*)g_whiteboardDlg == NULL)
	{
		return;
	}
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	if(((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg())
	{
		((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg()->CloseAllChildBox();
	}
	g_pWhiteBoardView->HideResBar();
	WB_DestroyWnd();
}
//关于avconwb
WB_API void WB_AboutAvconWB()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->OpenAboutDlg();
	}
}

WB_API void WB_MediaSingelToNet()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	g_pWhiteBoardView->DeleteIntnetMediaToSin();
	//g_pWhiteBoardView->RemoveAllMediaListDate();

}
WB_API void WB_SetMediaSinToIntnet( BOOL bMedia )
{
	g_MediaIntToSin = bMedia;
}
//设置存放临时文件的目录
WB_API void WB_SetU7FileDir(const char* lpszU7FileDir)
{
	g_bU7FileTempDir = ToDRAWStr(lpszU7FileDir).c_str();
}
WB_API BOOL WB_SaveWBPageToBitmap(UINT PageIndex,HBITMAP hBitmap)
{
	if (g_pWhiteBoardView->m_hWnd && g_pWhiteBoardView)
	{
		if (g_pWBFile != NULL)
		{
			CWBPage *pPage = NULL;
			UINT nCount = g_pWBFile->GetPageCount();
			if (PageIndex <= nCount)//输入页号小于等于总页数
			{
				pPage = g_pWBFile->GetPage(PageIndex);

				if (pPage != NULL)
				{
					if (hBitmap != NULL)
					{
						DeleteObject(hBitmap);
					}
					hBitmap = pPage->GetCurPageIndexBitmap();
					return TRUE;
				}
			}	
		}
	}
	return FALSE;
}
WB_API UINT WB_GetCurPageCount()
{
	if (g_pWhiteBoardView->m_hWnd && g_pWhiteBoardView)
	{
		if (g_pWBFile != NULL)
		{
			return g_pWBFile->GetPageCount();

		}
	}
	return 0;
}

WB_API void WB_IsSuccessLoad(bool bLoadSuccess)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	((CWhiteBoardDlg *)g_whiteboardDlg)->GetToolbarDlg()->SetNotifyIsSuccessLoad(bLoadSuccess);

	if (!bLoadSuccess)
	{
		g_pWhiteBoardView->ExitRoomDeleteStreamObj();
	}
	if (g_WBType == WB_SINGLERVER)
	{
		CWBFunctionDlg* pfundlg = ((CWhiteBoardDlg *)g_whiteboardDlg)->GetFuntDlg();
		if (pfundlg != NULL)
		{
			CWBTeachToolDlg *pteachdlg = pfundlg->GetTeachtoolDlg();
			if (pteachdlg != NULL)
			{
				pteachdlg->ShowFiveBtnVisable(bLoadSuccess);
			}
		}
	}
}
//请在判断硬件授权之前调用一次
WB_API void WB_SetIsLoacalWhiteboard(bool isLoacalWhiteboard)
{
	g_isLoacalWB = isLoacalWhiteboard;
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	g_pWhiteBoardView->SetIsLocalWhiteboard(isLoacalWhiteboard);
}
WB_API void WB_SetServerTime(const char *pLoadServerTime)
{
	CurLoadServerTimeStr = ToDRAWStr(pLoadServerTime).c_str();
//	ChangeServerLoadStrToSystemTime(LoadServerTime , CurLoadServerTime);
}

WB_API void WB_SetDropNotifyMediaCamara(MediaCamaraU7Data *pMediaCamaraU7Data)
{
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->OpenCamaraFromU7(pMediaCamaraU7Data);
	}
}

//是否显示全屏按钮 
WB_API void WB_ShowFullScreenBtn(bool show /*= true*/)
{	
	g_bShowFullScreenBtn = show;
}

WB_API void WB_ChangCourTranDlgPosition(int x,int y) //处理U7中拖拽白板窗口后改变光标的位置
{
	if (g_pWhiteBoardView)
	{
		CPoint pt(x,y);
		g_pWhiteBoardView->ChangeUserCurorTranDlgPosition(pt);
	}
}

WB_API void WB_HideFuncionBoard()
{
	if (g_pWhiteBoardView)
	{
		CWBFunctionDlg* fundlg = g_pWhiteBoardView->GetWBFunctionDlg();
		if(fundlg&& fundlg->GetSafeHwnd()&&fundlg->IsWindowVisible())
		{
			fundlg->ShowWindow(SW_HIDE);
		}
	}
}
//组合功能，在组合对象之前需保证对象是在选择状态
WB_API void WB_SetCombineObj()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		WPARAM wParam = ID_OBMENU_COMBINE;
		g_pWhiteBoardView->OnCommand(wParam,0);
	}
}
//取消组合功能，在取消组合对象之前需保证被取消的组合对象是在选择状态
WB_API void WB_SetUnCombineObj()
{
	if ((NULL == g_pWhiteBoardView) && (NULL == g_pWBPage))
	{
		return;
	}
	else
	{  
		WPARAM wParam = ID_OBMENU_UNCOMBINE;
		DWORD tempObjLayerIndex = g_pWhiteBoardView->GetdwMenuLayerIndex();
		g_pWhiteBoardView->SetdwMenuLayerIndex(	g_pWBPage->GetCurEditingObjLayerIndex());
		g_pWhiteBoardView->OnCommand(wParam,0);
		g_pWhiteBoardView->SetdwMenuLayerIndex(	tempObjLayerIndex);
	}
}

//获取当前选中白板对象数量
WB_API int WB_GetSelectWhiteBoardObjectCount()
{
	if(NULL == g_pWhiteBoardView)
	{
		return 0;
	}
	else
	{
		if(g_pWBPage != NULL)
		{			
			return g_pWBPage->GetWBSelectFrameList().size();
		}
		return 0;
		
	}

}

//获取白板是否有复制或剪切对象
WB_API bool WB_GetIsCutOrCopyObject()
{
	if(NULL == g_pWhiteBoardView)
	{
		return 0;
	}
	else
	{
		return g_pWhiteBoardView->GetObjCopyListCount();
	}
}

//获取选中对象中是否有组合对象
WB_API bool WB_GetIsCombinObject()
{
	if(NULL == g_pWhiteBoardView)
	{
		return false;
	}
	else
	{
		if(g_pWBPage != NULL)
		{			
			return g_pWBPage->IsCombinInSelectObj();
		}
		return false;
	}
}

//获取当前最大图层号
WB_API int WB_GetPageMaxLayer()
{
	if(NULL == g_pWhiteBoardView)
	{
		return 0;
	}
	else
	{
		if(g_pWBPage != NULL)
		{			
			return g_pWBPage->GetCurrentPageMaxLayer();
		}	
		return 0;
	}
	
}
//获取当前最小图层号
WB_API int WB_GetPageMinLayer()
{
	if(NULL == g_pWhiteBoardView)
	{
		return 0;
	}
	else
	{
		if(g_pWBPage != NULL)
		{			
			return g_pWBPage->GetCurrentPageMinLayer();
		}
		return 0;
	}
}
//获取当前对象图层号
WB_API int WB_GetPageObjCurrentLayer()
{
	if(NULL == g_pWhiteBoardView)
	{
		return 0;
	}
	else
	{
		if(g_pWBPage != NULL)
		{			
			return g_pWBPage->GetSelectOneObjectLayer();
		}
		return 0;
	}
}

//获取白板的宽度
WB_API int WB_GetWhiteboardWidth()
{
	
	if(NULL == g_pWhiteBoardView)
	{
		return 0;
	}
	else
	{
		return g_nWidth;
	}
}

//获取白板的 高度
WB_API int WB_GetWhiteboardHeight()
{
	
	if(NULL == g_pWhiteBoardView)
	{
		return 0;
	}
	else
	{
		return g_nHeight;
	}
}
//得到滚动条在当前视图中的起始点位置
WB_API POINT  WB_GetScrollPosition()
{
	if(NULL == g_pWhiteBoardView)
	{
		return (POINT)(CPoint(0, 0));
	}
	else
	{
		return (POINT)(g_pWhiteBoardView->GetScrollPosition());
	}
}
//设置滚动条在当前视图中的起始点位置
WB_API void WB_SetScrollPosition(POINT scrollPoint)
{
	if(NULL == g_pWhiteBoardView || NULL == g_pWBPage)
	{
		return;
	}
	else
	{
		POINT ptscroll = g_pWBPage->GetPageViewScroll();
		if ((ptscroll.x == scrollPoint.x) && (ptscroll.y == scrollPoint.y))
		{
			return;
		}
		g_pWBPage->SetPageViewScroll(scrollPoint);
		g_pWhiteBoardView->setCurrentViewScroll(scrollPoint);	
		//发送水平滚动条同步消息
		g_pWhiteBoardView->SendScrollDraw();
	}
}
//设置网络其他白板中滚动条位置
WB_API void WB_SendScrollPosToNet()
{
	if((NULL == g_pWhiteBoardView) && (WB_SINGLERVER == g_WBType))
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->SendScrollDraw();
		
	}
}
//判断单个对象是否锁定，只针对单个对象有效，多个对象时不处理
WB_API bool WB_GetSingleObjIsLock()
{
	if((NULL == g_pWhiteBoardView))
	{
		return 0;
	}
	else
	{
		return g_pWhiteBoardView->SingleObjIsLock();
		
	}
}

WB_API int WB_GetObjCount()
{
	if((NULL == g_pWhiteBoardView))
	{
		return 0;
	}
	else
	{
		return g_pWhiteBoardView->GetWBObjCount();
	}
}

WB_API void WB_Refresh()
{
	if((NULL == g_pWhiteBoardView))
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->RefreshWB();
	}
}

WB_API void WB_PostMessageToWhiteBoard(UINT msg, WPARAM wparam, LPARAM lparam)
{
	if((NULL == g_pWhiteBoardView))
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->PostMessageToWhiteBoard(msg, wparam, lparam);
	}
}