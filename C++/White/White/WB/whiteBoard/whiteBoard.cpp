
// whiteBoard.cpp : ���� DLL �ĳ�ʼ�����̡�
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
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CwhiteBoardApp

BEGIN_MESSAGE_MAP(CwhiteBoardApp, CWinApp)
	
END_MESSAGE_MAP()


// CwhiteBoardApp ����

CwhiteBoardApp::CwhiteBoardApp()
{
	
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_hinstRE41 = LoadLibrary(_T("msftedit.dll"));
}

CwhiteBoardApp::~CwhiteBoardApp()  
{
	if(m_hinstRE41)
	{
		FreeLibrary(m_hinstRE41);
	}
}

// Ψһ��һ�� CwhiteBoardApp ����

CwhiteBoardApp theApp;
extern CWhiteBoardView *g_pWhiteBoardView = NULL; //��ǰ��ͼ����
extern CWhiteBoardView *g_pBackWhiteBoardView = NULL;
extern CDialog *g_whiteboardDlg = NULL;
extern CWBFile *g_pWBFile = NULL; //�װ��ļ�ָ��
extern CWBPage *g_pWBPage = NULL; //�װ�ҳָ��
extern BOOL g_bIsFullScreen = FALSE; //�Ƿ�ȫ��״̬
extern IHPNetNotify *g_pHPWBDataNotify = NULL; //����ͬ����Ϣָ��
extern IHPWBNotify *g_pConferenceRecordNotify = NULL; //¼����Ϣָ��
extern void *g_pParent = NULL; //������
extern CPoint g_ptComRotCenter = (0,0); //��ϵ���ת���ĵ�
extern CPoint g_ptDocRotCenter = (0,0); //�ĵ�����ת���ĵ�
extern double g_dbComRotAngle = 0; //��ϵ���ת�Ƕ�
extern double g_dbDocRotAngle = 0; //�ĵ�����ת�Ƕ�
extern CPoint g_ptComZoomCenter = (0,0); //��ϵ��������ĵ�
extern CPoint g_ptDocZoomCenter = (0,0); //�ĵ����������ĵ�
extern BOOL   g_bRunWater = FALSE;         //ˮ������Ч
extern BOOL   g_bProcessWater = TRUE;     //���ưװ���С��ʱ��ʾˮ����
extern int    g_nBrushSize = 10;          //���Ӵ�С
//��϶�������ű���
extern double g_dbXScale = 1;
extern double g_dbYScale = 1;
//�ĵ���������ű���
extern double g_dbDocXScale = 1;
extern double g_dbDocYScale = 1;
//��Сview
extern double g_reduce=0.00;
//�װ�view�ĳ��Ϳ�
extern int g_nWidth = WB_VIEWHSCROLL;
extern int g_nHeight = WB_VIEWVSCROLL;

extern DWORD g_dwPreZoomLayerIndex = 0; //ǰһ���������ŵĶ���ͼ���
extern BOOL g_bIsDrawScreen=FALSE;                 //��Ļ��ͼ�Ƿ�����־
extern unsigned long g_ulSessionID = 1; //�ͻ���ID
extern CString g_strAvconUserID = _T(""); //AVCON���ص�¼���û�ID����AVCONͨ���ӿں�������
extern BOOL g_bYunNanDeviceSupported = FALSE;//ר�������Ϲ���Ӳ����Ȩ
extern bool g_bAuthorizedByHandWare = false; //��Ӳ�������Ƿ��д���������Ȩ
extern unsigned long g_MediaNumber = 2101; //��Ƶ���ų�ʼֵ
extern bool g_bWBWndIsneedShowToolBtn = true; //���ݰװ��С���ж��Ƿ���ʾ��ť��Ĭ������ʾ��
extern CString g_strLocalUserName = _T("");
//��AVCON�ṩ��һЩ��ʱ·��
extern CString g_strAvconLanguageFile = _T("");
extern CString g_strAvconIniPath = _T(""); //�����ļ�Ŀ¼
/*extern CString g_strAvconTempPath = _T("d:\\WBTemp\\"); //��ʱĿ¼*/
extern CString g_strAvconTempPath = NULL; //��ʱĿ¼
extern bool g_bAvconLock = false; //AVCON�Ƿ������װ幦�ܣ�������ʱ���װ�򿪺�û�й�����
extern bool g_bIsConferenceRecord = FALSE; //��AVCON���ã��Ƿ�ʼ����¼��
extern HMODULE g_IsMedicalVersion = NULL;//�ж��Ƿ���ҽ�ư汾��
extern WB_TYPE g_WBType = WB_MULTIVER;//��ǰ�汾����
extern bool g_bIsSupportMTTouch = false;//�Ƿ�������Ȩʹ�ö�㴥����
extern BOOL g_bNoSupportTouchInWB = FALSE;//��������û����Ȩʱ����������Ĵ�������������
extern HMODULE g_IsMidiVersion = NULL; //midi�汾
//�¹��ܿ���
extern bool g_bShowWBNewFun = false;
extern bool g_bSupportI7 = false;//�Ƿ���I7ʹ��
extern CPoint g_preScrollPoint = (0,0);
extern bool g_bSimplifyWB = true;
extern bool g_bU7NewStyle = false;
#ifdef AVCON_U7_WHITEBOARD
  extern BOOL g_bSupportU7OpenFile = TRUE;// �ж��Ƿ�ΪU7�еĴ��ļ���ʽ
#else
  extern BOOL g_bSupportU7OpenFile = FALSE;// �ж��Ƿ�ΪU7�еĴ��ļ���ʽ
#endif


extern bool g_bShowFullScreenBtn = true; //��ʾȫ����ť��
extern CString g_bU7FileTempDir = _T("");// ����U7���ļ�Ĭ��·��
extern BOOL g_MediaIntToSin = FALSE;//��Ƶ���絽����
extern int g_PageCount = 0;//ҳ�ţ���Ƶ���ļ�ʱ��
extern HWND g_hVedioHwnd = NULL;//��Ļ¼�ƴ���¼�ƾ��
extern BOOL g_bMedioScreenWndEnd = TRUE;//����¼�ƴ�������ر�
extern CString g_strMediaPhoto = _T(""); //��Ƶ˫�����մ��·����
const GUID CDECL _tlid = { 0x284145CA, 0x2220, 0x4A4A, { 0xAF, 0x2, 0x52, 0xF1, 0x3E, 0xF2, 0x76, 0xB9 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

typedef BOOL (*MTAuthorize)(void);
typedef BOOL (*SupportedDevice)();
void GetFilePath(CString &stringpath);
void SetConfigFilePath();
CString CurLoadServerTimeStr = _T("");//���ε�½��������ʱ��
CString PreOpenSoftTimeStr = _T("");//��һ��ʹ�������ʱ�䣬��Ҫ�ڵ�����ʹ��
bool g_isLoacalWB = false;
HMODULE g_hDESEncrypt = NULL;//��������ʱ��Ȩ����ؾ��
// CwhiteBoardApp ��ʼ��

BOOL CwhiteBoardApp::InitInstance()
{
	CWinApp::InitInstance();	
	AfxEnableControlContainer();

	/*	if (!AfxSocketInit())
	{
	AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	return FALSE;
	}*/

	// ������ OLE ������(����)ע��Ϊ���С��⽫ʹ
	//  OLE ����Դ�����Ӧ�ó��򴴽�����
	COleObjectFactory::RegisterAll();
	BOOL LanguageLocal =  ::SetThreadLocale(0x0804);

	g_ulSessionID = GenerateSSRC(); //�����ͻ�ID

	//�õ�ʣ��ռ������̷�����ʱĿ¼��ֵ
	GetFilePath(g_strAvconTempPath);
	g_strAvconTempPath += _T("\WBTemp\\");
	//���������ļ�·��
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
		FreeLibrary(g_IsMedicalVersion);//ж�ض�̬���ӿ⣻
		g_IsMedicalVersion = NULL;//���û�Ĭ�ϵ�ֵ���Ա�ˢ�£�
	}
	if (NULL != g_hDESEncrypt )
	{
		FreeLibrary(g_hDESEncrypt);//ж�ض�̬���ӿ⣻
		g_hDESEncrypt = NULL;//���û�Ĭ�ϵ�ֵ���Ա�ˢ�£�
	}
	if (g_IsMidiVersion != NULL)
	{
		BOOL bFree =::FreeLibrary(g_IsMidiVersion);
		g_IsMidiVersion = NULL;
	}
	//ɾ����ʱ�ļ��� wangxin 2010-12-28	
	DeleteDirectory(g_strAvconTempPath);
	return CWinApp::ExitInstance();

}

//WANGXIN 2010-12-28 ɾ��һ���ļ���
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
	//����ǰ�Ƴ���ʱ��д�������ļ��У������´���Ȩ��ʱ������ʱ����ж�
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

void SetConfigFilePath()//�ڰװ��ʼ��ʱ�����
{
	/*----------------���ݶ�Ȩʤ2011.11.22�ʼ�-------------------------------
	ͨ���ն����и�ExePath.ini �ļ���ʶ���ļ��ֶμ�Ŀ¼���£�
	C:\Program Files\avcon\ExePath.ini 

	[AVCON_SELECTED]
	#�Ƿ���XPE��0-PC,1-XPE
	AVCONXPE=0

	������նˣ�UCC�����ж�д�����ļ���ָ��D:\AVCON6_XPE ��Ŀ¼��
	----------------------------------------------------------------------*/		
	
	int nValue = 0;
	nValue = GetPrivateProfileInt(_T("AVCON_SELECTED"),
		_T("AVCONXPE"),
		0,
		_T("C:\\Program Files\\avcon\\ExePath.ini"));
	if (1 == nValue)//�ն˻�
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
	else if (0 == nValue)//pc��
	{
		g_strAvconIniPath = GetCurrentPath() + _T("\\config");
		g_strMediaPhoto = GetCurrentPath() + _T("\\user");
	}	
	//������Դ���е�iniĿ¼
	WBRESWND::SetAvconIniPath(g_strAvconIniPath.GetBuffer());
	g_strAvconIniPath.ReleaseBuffer();
}


// DllGetClassObject - �����๤��

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}


// DllCanUnloadNow - ���� COM ж�� DLL

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}


// DllRegisterServer - ������ӵ�ϵͳע���

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll())
		return SELFREG_E_CLASS;

	return S_OK;
}


// DllUnregisterServer - �����ϵͳע������Ƴ�

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll(FALSE))
		return SELFREG_E_CLASS;

	return S_OK;
}
//�õ�ʣ��ռ������̷�
void GetFilePath(CString &stringpath)
{
	ULARGE_INTEGER   ulAvailSize;        //�����߿��õ��ֽ�����       
	ULARGE_INTEGER   ulTotalSize;        //�����ϵ����ֽ���
	ULARGE_INTEGER   ulAvailLableSize;   //���õ��ֽ���

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

//�Ƿ����ҽ�ư汾��dll
void SetMedicalVersion()
{	
	//ȷ����̬���صĿ�·����ȷ
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
//��������ѡ��
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

//����MIDI��
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
*                              ��ʾ���Ӱװ�ӿ�                          *
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
		GetLangPath();//��ȡ��Ŀ¼�µ�U7Config.iniȷ�����԰汾��Ĭ��Ϊcs���İ�
		CWhiteBoardDlg *m_whiteboard_dlg=new CWhiteBoardDlg;
		g_bU7NewStyle = true;
		//�õ�ϵͳ�汾��
		g_systemVersion = GetSystemVersion();

		//�Ƿ����ҽ�ư汾
		SetMedicalVersion();
		//����midi��
		SetMidiVerson();
		//_CrtDumpMemoryLeaks();
		//AFX_MANAGE_STATE(AfxGetStaticModuleState());
		HINSTANCE main_hInstance = AfxGetResourceHandle();    //��ȡ��������Դ���
		AfxSetResourceHandle(theApp.m_hInstance);             //��ȡdll������Դ���

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
				strPrompt = _T("��ʾ");
			}
			CString strCreateAppFail;
			strCreateAppFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CreateAppFail"));
			if (strCreateAppFail.IsEmpty())
			{
				strCreateAppFail = _T("����Ӧ�ó���ʧ�ܣ�");
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
		GetLangPath();//��ȡ��Ŀ¼�µ�U7Config.iniȷ�����԰汾��Ĭ��Ϊcs���İ�
		CWhiteBoardDlg *m_whiteboard_dlg=new CWhiteBoardDlg;

		//�õ�ϵͳ�汾��
		g_systemVersion = GetSystemVersion();

		//�Ƿ����ҽ�ư汾
		SetMedicalVersion();
		//����midi��
		SetMidiVerson();
		HINSTANCE main_hInstance = AfxGetResourceHandle();    //��ȡ��������Դ���
		AfxSetResourceHandle(theApp.m_hInstance);             //��ȡdll������Դ���

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
				strPrompt = _T("��ʾ");
			}
			CString strCreateAppFail;
			strCreateAppFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CreateAppFail"));
			if (strCreateAppFail.IsEmpty())
			{
				strCreateAppFail = _T("����Ӧ�ó���ʧ�ܣ�");
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


//�ı�װ崰��λ��
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
	//�װ�һ�򿪾ʹ�ӥ��

	if (g_bWBWndIsneedShowToolBtn)
	{
		WB_EagleEyeSetposAndShow(rect);
	}

	return;
}

//�ı�װ崰��λ��
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
	//�װ�һ�򿪾ʹ�ӥ��

	if (g_bWBWndIsneedShowToolBtn)
	{
		WB_EagleEyeSetposAndShow(rect);
	}

	return;
}

//�������ݷ��ʹ���ָ��
WB_API void WB_SetDataNotify(void *pDataNotify)
{
	g_pHPWBDataNotify = (IHPNetNotify *)pDataNotify;
}

/*************************************************************************
*                           void SetFullScreen( )                        *
*                              ���Ӱװ�ȫ���ӿ�                          *
*************************************************************************/
WB_API void WB_SetFullScreen(BOOL bFullScreen/* = TRUE*/)
	/*
	ȫ�����˳�ȫ��
	bFullScreenΪTRUE��ʾȫ����ΪFALSE��ʾ�˳�ȫ��
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
		g_pWhiteBoardView->SendLocalMessage(CMD_SCREEN,bFullScreen); //����ȫ��������Ϣ
		g_pWhiteBoardView->ChangeFullScreen(bFullScreen); //����ȫ��������Ϣ
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
*                              ���Ӱװ廹ԭ�ӿ�                          *
*************************************************************************/
WB_API void WB_SetNormalScreen( )
{
	//ȫ��ʱ���������������ݱ��浽�ļ����Ա㽫���ݴ��͵���ԭ���ڡ�
	//wbFullDlg.SendDataFullToFile(){}
	//g_pWhiteBoardView = NULL;
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//12-27�¼�
		CWhiteBoardDlg *m_whiteboard_dlg = (CWhiteBoardDlg*)g_whiteboardDlg;
		m_whiteboard_dlg->closeFullDlg();

		g_pWhiteBoardView = g_pBackWhiteBoardView;
		g_pWhiteBoardView->DeleteEdit();
		g_pObj = NULL;
	}

	//��ԭ�����ػ������ļ��е����ݶ��뵽��ԭ����
	//m_whiteboard_dlg->SendDataFileToNormal(){}
}
//��������״̬
WB_API void WB_SetDrawStatus(DRAW_STATE nDrawStatus)
{
	//�ж��Ƿ��ȼ��ض�̬���ӿ�
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

		//���ı��༭�����ɾ��
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
//�������ͣ�����ʵ�ߡ����ߡ��㻮�ߵ�PS_SOLID��PS_DOT;��ǰϵͳ��Ĭ������ΪPS_SOLIDʵ��
WB_API void WB_SetWBLineType(int nWBLineType)
{
	//�ж��Ƿ��ȼ��ض�̬���ӿ�
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_nWBLineType = nWBLineType;
	}
}

//�����ߺ�ͼ�ε��߿�Ĭ��ֵΪ4
WB_API void WB_SetWBCurObjWidth(int nWBCurObjWidth)
{
	//�ж��Ƿ��ȼ��ض�̬���ӿ�
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

//������ð
WB_API void WB_SetLineCap(int nStartLineCap,int nEndLineCap)
{
		//�ж��Ƿ��ȼ��ض�̬���ӿ�
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

//ˮ������Ч����
WB_API void WB_SetWaterStatus()
{
	g_bRunWater = !g_bRunWater;
}
WB_API void WB_SetCharfont()
{
	//�ж��Ƿ��ȼ��ض�̬���ӿ�
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
			if (CScreenDrawMgr::GetIns() != NULL) //�����Ļ��ͼ��
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

//������Ƥ���ߴ�
WB_API void WB_SetBrushSize(int nBrushSize)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_nBrushSize = nBrushSize;
	}	
}
//��ͼ���½�һҳ
WB_API void WB_AddNewPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}	
}

//��ͼ�в���һҳ
WB_API void WB_InsertNewPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->InsertPrePage(nPageIndex);
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}
}

//��ͼ�е�ɾ��һҳ
WB_API void WB_DeletePage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{

		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->DeletePage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->DeletePrePage(nPageIndex);
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}

}
//����ҳ
WB_API void WB_InterchangePage(UINT nFormerPage, UINT nTargetPage)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->InterchangePage(nFormerPage, nTargetPage);
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}
}
//��ͼ�е���һҳ
WB_API void WB_PrePage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}

}
//��ͼ�е���һҳ
WB_API void WB_NextPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}

}
//��ͼ�еĵ�һҳ
WB_API void WB_FirstPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->FirstPage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}

}
//��ͼ�е����ҳ
WB_API void WB_LastPage()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->LastPage();
		//g_nDrawStatus = DRAW_NORMAL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}

}
// ͼ����һ��
WB_API void WB_SuperLayer()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//��ѡ���ܽ���ɾ��
		//g_pWBPage->ClearSelectFrameList();

		g_pWhiteBoardView->SuperLayer();

	}

}
// ͼ����һ��
WB_API void WB_UnderLayer()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//��ѡ���ܽ���ɾ��
		//g_pWBPage->ClearSelectFrameList();

		g_pWhiteBoardView->UnderLayer();
	}

}
// ͼ�㶥��
WB_API void WB_BottomLayer()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//��ѡ���ܽ���ɾ��
		//g_pWBPage->ClearSelectFrameList();
		//g_pWhiteBoardView->Invalidate();

		g_pWhiteBoardView->BottomLayer();
	}

}
// ͼ��ײ�
WB_API void WB_TopLayer()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//��ѡ���ܽ���ɾ��
		//g_pWBPage->ClearSelectFrameList();
		//g_pWhiteBoardView->Invalidate();

		g_pWhiteBoardView->TopLayer();
	}

}


// ���ͼƬ
WB_API void WB_AddPicture()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

//���ͼƬ
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

// �������Ƶ��jiabangping 20110804��
WB_API void WB_AddMedia()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

// �����ĵ�
WB_API void WB_AddDoc(CRect rect/* = CRect(0, 0, 0, 0)*/)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

// ɾ��
WB_API void WB_DeleteSelectedObjects()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_nDrawStatus = DRAW_NORMAL; //ѡ��״̬
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)//��ѡ�еĶ���ʱ��ִ��ɾ������
		{
			g_pWhiteBoardView->DeleteSelectedObjects();
			g_pWhiteBoardView->Invalidate(TRUE);
				
		}		
	}
}
// ������Դ��
WB_API void WB_OpenResource()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

//������Դ�����

//����flash
WB_API void WB_AddFlash()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

//����Dxf
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

//����
WB_API void WB_SaveWBFile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
			PathRemoveFileSpec(chFilePath); //�õ������·�������������ļ���
			if (!PathIsDirectory(chFilePath)) //�ж�·���Ƿ���һ����Ч��Ŀ¼
			{
				if(!CreateDirectory(chFilePath, NULL)) //����·�������ļ���
				{
					CString strPrompt;
					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
					if (strPrompt.IsEmpty())
					{
						strPrompt = _T("��ʾ");
					}
					CString strFolderCreateFail;
					strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
					if (strFolderCreateFail.IsEmpty())
					{
						strFolderCreateFail = _T("�ļ��д���ʧ�ܣ�");
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

			if (PathFileExists(strFilePath)) //�ж��ļ��Ƿ����
			{
				DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength())); //ɾ���ļ�
				strFilePath.ReleaseBuffer();
			}
		} 
		else
		{
			CFileDialog dlgFile(FALSE, _T("ucb"), NULL, OFN_HIDEREADONLY , _T("UCB Files (*.ucb)|*.ucb||"), NULL, 0, FALSE);

			dlgFile.m_ofn.lpstrTitle = _T("����Ϊ");

			if (dlgFile.DoModal() == IDOK)
			{
				if (!g_pWhiteBoardView)
				{
					return ;
				}
				strFilePath = dlgFile.GetPathName();
				if (PathFileExists(strFilePath))//����ļ����ھ�ɾ������ļ�
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
		PathRemoveFileSpec(chFilePath); //�õ������·�������������ļ���
		if (!PathIsDirectory(chFilePath)) //�ж�·���Ƿ���һ����Ч��Ŀ¼
		{
			if(!CreateDirectory(chFilePath, NULL)) //����·�������ļ���
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("��ʾ");
				}
				CString strFolderCreateFail;
				strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
				if (strFolderCreateFail.IsEmpty())
				{
					strFolderCreateFail = _T("�ļ��д���ʧ�ܣ�");
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		
		if (PathFileExists(strFilePath))//����ļ����ھ�ɾ������ļ�
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

//�����ͼƬ
WB_API void WB_SaveWBFileToPicture()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	g_pObj = NULL;
	g_pWhiteBoardView->DeleteEdit();
	//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
	if (g_pWBPage->GetWBSelectFrameList().size() > 0)
	{
		//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

	//�ļ��Ի�����ʾ
	g_pWhiteBoardView->GetWBSavePictureDlg();
	
	return;
	//�����ļ��Ի����б���ΪͼƬ
	/*g_pWhiteBoardView->GetWBSavePictureDlg()->ShowWindow(SW_HIDE);*/

}
WB_API void WB_SaveWBFileToPicture(const char* pFilePath)
{
	;		
}
//���
WB_API void WB_Resavefile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
			PathRemoveFileSpec(chFilePath); //�õ������·�������������ļ���
			if (!PathIsDirectory(chFilePath)) //�ж�·���Ƿ���һ����Ч��Ŀ¼
			{
				if(!CreateDirectory(chFilePath, NULL)) //����·�������ļ���
				{
					CString strPrompt;
					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
					if (strPrompt.IsEmpty())
					{
						strPrompt = _T("��ʾ");
					}
					CString strFolderCreateFail;
					strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
					if (strFolderCreateFail.IsEmpty())
					{
						strFolderCreateFail = _T("�ļ��д���ʧ�ܣ�");
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

			if (PathFileExists(strFilePath)) //�ж��ļ��Ƿ����
			{
				DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength())); //ɾ���ļ�
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
				if (PathFileExists(strFilePath))//����ļ����ھ�ɾ������ļ�
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
		PathRemoveFileSpec(chFilePath); //�õ������·�������������ļ���
		if (!PathIsDirectory(chFilePath)) //�ж�·���Ƿ���һ����Ч��Ŀ¼
		{
			if(!CreateDirectory(chFilePath, NULL)) //����·�������ļ���
			{
				delete[] chFilePath;
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("��ʾ");
				}
				CString strFolderCreateFail;
				strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
				if (strFolderCreateFail.IsEmpty())
				{
					strFolderCreateFail = _T("�ļ��д���ʧ�ܣ�");
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

		if (PathFileExists(strFilePath))//����ļ����ھ�ɾ������ļ�
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

//��ӡ
WB_API void WB_Printfile()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

//���ļ�
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
			strPrompt = _T("��ʾ");
		}
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//�ж��ļ��Ƿ���Ҫ���� wangxin 2011-01-11
		if (!g_strCurFilePath.IsEmpty())
		{
			CString strIsCurFileSave;
 			strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
 			if (strIsCurFileSave.IsEmpty())
 			{
 				strIsCurFileSave = _T("�Ƿ���Ҫ����˴ε��ļ�?");
 			}

			//strIsCurFileSave = _T("�Ƿ���Ҫ����˴ε��ļ�");
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
				strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//ɾ����Ƶ����
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
					strFilePathError = _T("�ļ�·����������");
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
			//���͹رձ�ע��ͬ����Ϣ
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
					strFilePathError = _T("�ļ�·����������");
				}
				if (!PathFileExists(strFilePath))
				{
					AfxMessageBox(strFilePathError,MB_OK);
					return;
				}
				g_pWhiteBoardView->m_bLocalFileOperate = true;
				g_pWhiteBoardView->CloseLines();
				//���͹رձ�ע��ͬ����Ϣ
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
			//�ļ�����ҳ
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
			strPrompt = _T("��ʾ");
		}
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//�ж��ļ��Ƿ���Ҫ���� wangxin 2011-01-11

		if (!g_strCurFilePath.IsEmpty())
		{
			CString strIsCurFileSave;
 			strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
 			if (strIsCurFileSave.IsEmpty())
 			{
 				strIsCurFileSave = _T("�Ƿ���Ҫ����˴ε��ļ�?");
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
				strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//ɾ����Ƶ����
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
					strFilePathError = _T("�ļ�·����������");
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
			//���͹رձ�ע��ͬ����Ϣ
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
					strFilePathError = _T("�ļ�·����������");
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
			//�ļ�����ҳ
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
				strPrompt = _T("��ʾ");
			}
			CString strIsSave;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
			}
			CString strFilePathError;
			strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
			if (strFilePathError.IsEmpty())
			{
				strFilePathError = _T("�ļ�·����������");
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

		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//ɾ����Ƶ����
		g_pWhiteBoardView->RemoveAllMediaListDate();

		g_pWhiteBoardView->m_bLocalFileOperate = true;
 		g_pWhiteBoardView->CloseLines();
		//���͹رձ�ע��ͬ����Ϣ
		g_pWhiteBoardView->SendScreenDraw();
		bool bAvconlock = g_bAvconLock;
		WB_AvconLock(true);
		if (!g_bSupportI7)
			g_pWhiteBoardView->CreateLoadingPictureDlg();
		if (g_pWhiteBoardView->LoadLines(strFilePath))
		{
			g_strCurFilePath = strFilePath;
			g_nDrawStatus = DRAW_NORMAL;
			//�ļ�����ҳ
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

//�½��ļ�
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
			strPrompt = _T("��ʾ");
		}
		//�ж��ļ��Ƿ���Ҫ���� wangxin 2011-01-11
		if (!g_strCurFilePath.IsEmpty())
		{
			CString strIsCurFileSave;
			strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsCurFileSave"));
			if (strIsCurFileSave.IsEmpty())
			{
				strIsCurFileSave = _T("�Ƿ���Ҫ���˴ε��ļ����浽");
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
				strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//ɾ����Ƶ����
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		g_pWhiteBoardView->CloseLines();
		//���͹رձ�ע��ͬ����Ϣ
		g_pWhiteBoardView->SendScreenDraw();
		g_strCurFilePath.Empty();

		g_pObj = NULL;
		//�ر��ļ�Ԥ������
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
			strPrompt = _T("��ʾ");
		}
		if (bSaveFile)
		{
			//�ж��ļ��Ƿ���Ҫ���� wangxin 2011-01-11
			if (!g_strCurFilePath.IsEmpty())
			{
				CString strIsCurFileSave;
				strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsCurFileSave"));
				if (strIsCurFileSave.IsEmpty())
				{
					strIsCurFileSave = _T("�Ƿ���Ҫ���˴ε��ļ����浽");
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
					strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//ɾ����Ƶ����
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		g_pWhiteBoardView->CloseLines();
		//���͹رձ�ע��ͬ����Ϣ
		g_pWhiteBoardView->SendScreenDraw();
		g_strCurFilePath.Empty();

		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
		//�ر��ļ�Ԥ������
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
//�ر��ļ�
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
			strPrompt = _T("��ʾ");
		}
		//�ж��ļ��Ƿ���Ҫ���� wangxin 2011-01-11
		if (!g_strCurFilePath.IsEmpty())
		{
			int nChoose;
			CString strIsSave;
// 			strIsSave = _T("�Ƿ�رյ�ǰ�װ��ļ���");
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
				strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
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
// 			strIsSave = _T("�Ƿ�رյ�ǰ�װ��ļ���");
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
				strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
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
		//����׼��ҳ�Ÿ�Ϊ1
		g_pWhiteBoardView->SetNeedSetPageIndexForScreenDraw(true);
		WB_DestroyWnd();

		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

		//ɾ����Ƶ����
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		g_pWhiteBoardView->CloseLines();
		g_strCurFilePath.Empty();
		//����׼��ҳ��Ϊ��ȡ��ǰҳ��
		g_pWhiteBoardView->SetNeedSetPageIndexForScreenDraw(false);
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
		//�ر��ļ�Ԥ������
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
				strPrompt = _T("��ʾ");
			}
			//�ж��ļ��Ƿ���Ҫ���� wangxin 2011-01-11
			if (!g_strCurFilePath.IsEmpty())
			{
				CString strIsCurFileSave;
				strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsCurFileSave"));
				if (strIsCurFileSave.IsEmpty())
				{
					strIsCurFileSave = _T("�Ƿ���Ҫ���˴ε��ļ����浽");
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
					strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//ɾ����Ƶ����
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		g_pWhiteBoardView->CloseLines();
		//���͹رձ�ע��ͬ����Ϣ
		g_pWhiteBoardView->SendScreenDraw();
		g_strCurFilePath.Empty();

		g_pObj = NULL;
		//�ر��ļ�Ԥ������
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

//�˳�������װ�
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
			strPrompt = _T("��ʾ");
		}
		if (!g_strCurFilePath.IsEmpty())
		{
			CString strIsCurFileSave;
			strIsCurFileSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsCurFileSave"));
			if (strIsCurFileSave.IsEmpty())
			{
				strIsCurFileSave = _T("�Ƿ���Ҫ���˴ε��ļ����浽");
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
				strIsSave = _T("�Ƿ���Ҫ����˴ε��ļ���");
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//ɾ����Ƶ����
		g_pWhiteBoardView->RemoveAllMediaListDate();
		g_pWhiteBoardView->m_bLocalFileOperate = false;
		g_pWhiteBoardView->CloseLines();
		g_strCurFilePath.Empty();

		g_pObj = NULL;
		//�ر��ļ�Ԥ������
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
		//��ԭ�װ�����״̬
		g_pWhiteBoardView->ReductionPrimaryWB();
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("��ʾ");
		}
		if (!g_strCurFilePath.IsEmpty())
		{
			int nChoose;
			CString strIsSaveLocal;
			strIsSaveLocal = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSaveLocal"));
			if (strIsSaveLocal.IsEmpty())
			{
				strIsSaveLocal = _T("�װ���黥�������ӣ��Ƿ���Ҫ�������ļ����浽");
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
				strInteractionConnected = _T("�װ���黥�������ӣ��Ƿ���Ҫ���汾�ص��ļ���");
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

		//ɾ����Ƶ����
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

//����
WB_API void WB_Cutscreen()
{
	if (NULL == g_whiteboardDlg || NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}	
}

// �˳��װ�Ի���
WB_API void WB_Close()
{
	//��ʱ�����ñ���Ӳ����Ȩ���ã����Բ���Ҫ����WriteEndTime()����
	//WriteEndTime();
	ClipCursor(NULL);
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{	
		if (g_IsMidiVersion != NULL) //ж��MIDI��
		{
			typedef void (*lpCloseConvert)();
			lpCloseConvert CloseConvert = (lpCloseConvert)GetProcAddress(g_IsMidiVersion,"CloseConvert");
			if (CloseConvert != NULL)
			{
				CloseConvert();
			}		
		}
		
		//�ڵ�������֮ǰ�����һЩɨβ����
		g_pWhiteBoardView->ViewWindowFinalTreat();	
		if (NULL != g_whiteboardDlg)
		{
			delete g_whiteboardDlg;
			g_whiteboardDlg = NULL;
			g_pWhiteBoardView = NULL;
			g_nDrawStatus = DRAW_NORMAL;
			g_pConferenceRecordNotify = NULL;
			//g_IsSelectObj = FALSE;//�ͷ�ѡ��
			g_pObj = NULL;
			g_bRunWater = FALSE;
			g_reduce=0.00;//�˳�ʱ��0
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
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
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
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��

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

//����ͬ�����յ�����
WB_API void WB_ProcessDataIn(const char *pData, int nLen)
{
	if (g_pWhiteBoardView != NULL)
	{
		g_pWhiteBoardView->OnNETEC_NodeReceivedFromRouter(pData,nLen);
	}	
}

//�������ж���
WB_API void WB_ReqAllObj()
{
	if (g_pWhiteBoardView != NULL)
	{
		g_pWhiteBoardView->GetAllObj();
	}	
}
//�������ж�������
WB_API void WB_SendAllObj()
{
	if (g_pWhiteBoardView != NULL)
	{
		g_pWhiteBoardView->SendAllObj();
	}
}
//ɾ�����ж��󣬵��ļ������Ǳ��ز����ǣ��ͷ���ɾ�����ж����ͬ����Ϣ
WB_API void WB_CloseLine()
{
	if (g_pWhiteBoardView != NULL)
	{
		g_pWhiteBoardView->CloseLines();
	}
}
WB_API bool WB_BHardwareSupportMTTouch()
{

	//�ſ����Ӳ����Ȩ��Ϣ
	g_bAuthorizedByHandWare = true;
	return g_bAuthorizedByHandWare;
	/*-------------------------��ʼ��ⴥ������-----------------------------*/
	//���ȣ�ͨ��ֱ�Ӽ��ķ�ʽ��ȡ�Ƿ�֧�ִ������ܣ�
	int value = GetSystemMetrics(SM_DIGITIZER);
	if (value & (NID_READY | NID_INTEGRATED_TOUCH | NID_MULTI_INPUT | NID_INTEGRATED_PEN))
	{
		return true;
	}
	else
	{
		return false;
	}

	BOOL bMTouchSupport = TRUE; //�Ƿ�֧�ֶ�㴥������
	BOOL bDeviceSupported = MTAUTHORIZE::IsSupportedDevice();
	g_bAuthorizedByHandWare = bMTouchSupport && bDeviceSupported;

	if (!g_bAuthorizedByHandWare)//�������ǵ�����������Ȩ�ļ��ҺϷ��Ļ���������ʹ��
	{
		//�ӽ��ܿ��ȡ��Ȩʱ��
		CString ReadFilePath = GetCurrentPath() + _T("\\config\\AVCONWB.wbe");
		//char Resultc[64] = {0};
		int ResultLen = 0;
		SYSTEMTIME BeginTime = {0},EndTime = {0};
		CString BeginTimeStr = _T(""),EndTimeStr = _T(""),TempStr = _T("");
		if (!PathFileExists(ReadFilePath))//��Ȩ�ļ�������,ֱ�Ӳ���Ȩ
		{
			return false;
		}

		//�ӽ��ܿ��ȡ��Ȩʱ��,����Resultc�����У�
		if (!g_hDESEncrypt)
		{
			//ȷ����̬���صĿ�·����ȷ
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
			//wangxiaobin�Ҹĵ�20130819
			char * Resultc = new char[MAX_PATH];
			memset(Resultc,0,MAX_PATH);
			//GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,(LPCTSTR)g_hDESEncrypt,&g_hDESEncrypt); 
			typedef	void (__cdecl *UnCode)(CString FilePath,char* resultData,int &resultDataLenth);
			UnCode UnencryptCode = (UnCode)GetProcAddress(g_hDESEncrypt, "_UnencryptCode");
			if (NULL != UnencryptCode)
			{
				(*UnencryptCode)(ReadFilePath,Resultc,ResultLen);
				if (ResultLen < 16)//˵���û��Լ��Ķ�����Ȩ�ļ�
				{
					delete []Resultc;
					return false;
				}
				TempStr = ToDRAWStr(Resultc).c_str();
				BeginTimeStr = TempStr.Mid(0,8);
				EndTimeStr = TempStr.Mid(8,8);
				//ChangeDateToSystemtime(Resultc,BeginTime,EndTime,ResultLen);//���ַ���Ȩʱ��ת����SYSTEMTIMEʱ��
				//IsRightAuthorize(g_isLoacalWB,BeginTime,EndTime,CurLoadServerTime,PreOpenSoftTime);
				//��ȡ��һ��ʹ�ðװ�Ľ���ʱ��
				if (g_isLoacalWB)
				{
					TCHAR Text[256] = {0};
					ReadFilePath = GetCurrentPath() + _T("\\config\\WBConfig.ini");
					GetPrivateProfileString(_T("PreEndTime"), _T("PreEndTimeName"), _T(""),Text,256,ReadFilePath);
					PreOpenSoftTimeStr = Text;
					for(int i = 0; i <PreOpenSoftTimeStr.GetLength();++i )//д��ȥ��ʱ�������ֵ
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
	return g_bAuthorizedByHandWare;//����ʹ��
}

//�Ƿ���Ȩ��������,
WB_API bool WB_SetTouchAuthorization(bool bAuthorization)
{   //����bAuthorization���Ʒ�������Ȩ

	//bAuthorization = TRUE; //�Ȳ�����
	
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

		//�Դ�����Դ��Ĵ������ܽ�����Ȩ
		WBRESWND::SetMTAuthorizeRes(g_bIsSupportMTTouch);
	}
	return bAuthorization;
}

//���յ���Ȩ���
WB_API bool WB_GetTouchAuthorization()
{
	return g_bIsSupportMTTouch;
}

//��Ļ¼��
WB_API void WB_RECScreen(void)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}

		AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
		g_pWhiteBoardView->AddRECScreen();
	}
}

//����
WB_API void WB_Baffle(void)
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->addBaffle();
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;
	}
}
//ֱ��
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

//������
WB_API void WB_Protractor(void)
{
	if (g_pWhiteBoardView==NULL)
	{
		return;
	} 
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->AddProtractor();
		g_pObj = NULL;
	}
}

//���ǰ�
WB_API void WB_SetSquare(void)
{
	if (NULL == g_pWhiteBoardView) 
	{
		return ;
	}
	else
	{
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->AddSetSquare();
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
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
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
			g_pWhiteBoardView->setFrameListAndObjLayerIndex();
		}
		g_pWhiteBoardView->AddCompass();
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		g_pObj = NULL;

	}
}
//�������
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

//���ô����ʱ�ļ���Ŀ¼
WB_API void WB_SetWBTempDir(const char *pWBTempDir)
{
	g_strAvconTempPath = ToDRAWStr(pWBTempDir).c_str();
	if (!PathIsDirectory(g_strAvconTempPath))//wangxiaobin������ʱ�ļ�Ŀ¼
	{
		if(!CreateDirectory(g_strAvconTempPath, NULL))
		{
			return ;
		}
	}
}

//��ݼ�,MFC��DLL�д���Ҫ��ӦPreTranslateMessage����������Ϣ�����������̵�PreTranslateMessage������ʾ�ĵ���
WB_API BOOL  DllPreTranslateMessage(MSG *pMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.PreTranslateMessage(pMsg);
}

//�޸�Ĭ��������ɫ
WB_API void WB_SetDefaultLineColoe(int nRed, int nGreen, int nBlue)
{
		//�ж��Ƿ��ȼ��ض�̬���ӿ�
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

//�����Ƿ������ɫ
WB_API void WB_SetIsSolid(bool isSolid)
{
	//�ж��Ƿ��ȼ��ض�̬���ӿ�
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_bIsSolid = isSolid;
	}
}

//����ͼ�������ɫclrSolid
WB_API void WB_SetSolidColor(int nRed, int nGreen, int nBlue)
{
		//�ж��Ƿ��ȼ��ض�̬���ӿ�
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

//���ðװ屳����ɫg_clrWBPageColor
WB_API void WB_SetWBPageColor(int nRed, int nGreen, int nBlue)
{
		//�ж��Ƿ��ȼ��ض�̬���ӿ�
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

//������Ļ��ͼ
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
			//����ͬ����Ϣ
			g_pWhiteBoardView->SendScreenDraw();
		}
	}
}
//ȫ���л�
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
			//����ͬ����Ϣ
			//g_pWhiteBoardView->SendScreenDraw();
		}
	}
}
//����͸��Ч���л�
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
			//����ͬ����Ϣ
			g_pWhiteBoardView->SendScreenDraw();
		}
	}
}                                                                                  
//�ر���Ļ��ͼ����
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
				//����ͬ����Ϣ
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
		g_pWhiteBoardView->SetBroadcastUserCursor(bShow); //�װ಻�ڼ�����Ļ��false
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
//�ж��Ƿ�ȫ��Ļ
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
//�ж��Ƿ�͸��
WB_API bool WB_IsScreenDrawTrans()
{
	if(CScreenDrawMgr::GetIns() != NULL)
	{
		return CScreenDrawMgr::GetIns()->IsTransparent();
	}
	return false;
}
//�򿪿�����
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

//��㴰�ڴ���С���ָ�ʱ���Ƿ�Ҫ�򿪰װ��ע����
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

//�װ��Ƿ���С���ˣ���С����ΪTRUE������ΪFALSE
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

//�۹��
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
//ӥ��
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

//���ñ���AVCON�û�ID
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
//AVCON�û�����֪ͨ
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
		g_pWhiteBoardView->OnRejectBroadcastUserCursor(pUserID);//�رո��û��Ĺ��
		g_pWhiteBoardView->OnRejectBroadcasstUserWater(pUserID); //�ر�ˮ����
		g_pWhiteBoardView->ClearObjectOfOffLineUser(sUser);
	}
}

//����Ƕ���ת����ʱ��Ϊ����˳ʱ��Ϊ��
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

//����ˮƽ��ֱ��ת��ˮƽΪ0�ȣ���ֱΪ90��
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
//��¡
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
//���޿�¡
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

//����
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
//����
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
//ճ��
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
//ɾ��ȫ��
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
	const char *pTempPath) //��AVCON�ṩ����ʱ·��
{
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	if (pLanguageFile != NULL)
	{
		g_strAvconLanguageFile = ToDRAWStr(pLanguageFile).c_str();
		//Ucc�д����������Զ�ȡ�ļ�·���Ǵ��
		//g_strAvconLanguageFile = pLanguageFile;
	}

	if (pIniPath != NULL)
	{
		g_strAvconIniPath = ToDRAWStr(pIniPath).c_str();
		//������Դ���е�iniĿ¼
		WBRESWND::SetAvconIniPath(g_strAvconIniPath);
	}

	if (pTempPath != NULL)
	{
		g_strAvconTempPath = ToDRAWStr(pTempPath).c_str();
	}
}
//����DICOM�ӿ�
WB_API void WB_AddDICOM()
{
	if (NULL == g_pWhiteBoardView)
	{
		return;
	}
	else
	{
		g_pObj = NULL;
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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
//����DICOM�ӿ�,��·��
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
		//g_IsSelectObj = FALSE;//�ͷ�ѡ��
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		//�ж���ѡ��ʱ��ִ��ɾ��ѡ���ܲ���caoyuqin12-3-22	
		if (g_pWBPage->GetWBSelectFrameList().size() > 0)
		{
			//1-17��ѡ���ܽ���ɾ���͵�ǰѡ��ͼ�����0	
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

//��AVCON�����Ƿ������װ幦��
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
		//���ı��༭�����ɾ��
		g_pWhiteBoardView->DeleteEdit();
		WB_CloseAllChildBox();
		g_pWhiteBoardView->SendMessage(WM_CANCELMODE,0,0);
		g_pWhiteBoardView->ProcessCloseColorDlg();
		g_whiteboardDlg->SendMessage(WM_LBUTTONDOWN,0,0);
		g_whiteboardDlg->SendMessage(WM_LBUTTONUP,0,0);
 		if ((g_pWhiteBoardView->GetWBFunctionDlg() != NULL) &&(g_pWhiteBoardView->GetWBFunctionDlg()->IsWindowVisible()))
 		{
			//�װ屻�������ع������			
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

//��AVCON���ÿ�ʼ���������¼��
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
		//���͵�ǰҳ�����ж����¼�� ����m_bStartConferenceRecordΪtrue,��U7�в���Ҫ
		if (!g_bSupportU7OpenFile)
		{
			Sleep(5000);
		}
		
		SendMessage(g_pWhiteBoardView->m_hWnd,WM_SENDPAGEDATATO_CONFERENCE_RECORD,1,0);

	}
	else
	{
		//ÿһҳ����m_bStartConferenceRecord��Ϊfalse
		g_pWBFile->ReductionList();
	}
	return TRUE;
}

//���û���¼��ָ��
WB_API void WB_SetConferenceRecordNotify(IHPWBNotify *pConferenceRecordNotify)
{
	if (g_pWhiteBoardView == NULL)
	{
		return ;
	}
	g_pConferenceRecordNotify = (IHPWBNotify *)pConferenceRecordNotify;
}

//���ðװ�İ汾����
WB_API void WB_SetTypeOfWB( WB_TYPE typeOfWB )
{
	g_WBType = typeOfWB;
}
//�ر������ӹ�����
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
//����avconwb
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
//���ô����ʱ�ļ���Ŀ¼
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
			if (PageIndex <= nCount)//����ҳ��С�ڵ�����ҳ��
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
//�����ж�Ӳ����Ȩ֮ǰ����һ��
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

//�Ƿ���ʾȫ����ť 
WB_API void WB_ShowFullScreenBtn(bool show /*= true*/)
{	
	g_bShowFullScreenBtn = show;
}

WB_API void WB_ChangCourTranDlgPosition(int x,int y) //����U7����ק�װ崰�ں�ı����λ��
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
//��Ϲ��ܣ�����϶���֮ǰ�豣֤��������ѡ��״̬
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
//ȡ����Ϲ��ܣ���ȡ����϶���֮ǰ�豣֤��ȡ������϶�������ѡ��״̬
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

//��ȡ��ǰѡ�аװ��������
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

//��ȡ�װ��Ƿ��и��ƻ���ж���
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

//��ȡѡ�ж������Ƿ�����϶���
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

//��ȡ��ǰ���ͼ���
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
//��ȡ��ǰ��Сͼ���
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
//��ȡ��ǰ����ͼ���
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

//��ȡ�װ�Ŀ��
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

//��ȡ�װ�� �߶�
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
//�õ��������ڵ�ǰ��ͼ�е���ʼ��λ��
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
//���ù������ڵ�ǰ��ͼ�е���ʼ��λ��
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
		//����ˮƽ������ͬ����Ϣ
		g_pWhiteBoardView->SendScrollDraw();
	}
}
//�������������װ��й�����λ��
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
//�жϵ��������Ƿ�������ֻ��Ե���������Ч���������ʱ������
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