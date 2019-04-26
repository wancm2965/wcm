// MedioVedi0oScreenDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "MedioVideoScreenDlg.h"
#include "afxdialogex.h"
#include "WVedio.h"
#include <string>
#define  CHECKMEMORYSTATUSISFULL    19991111
#define  FULLSCREEN 1
#define  AREASCREEN 2
#define  WNDSCREEN  3

// CMedioVideoScreenDlg �Ի���

IMPLEMENT_DYNAMIC(CMedioVideoScreenDlg, CDialogEx)

CMedioVideoScreenDlg::CMedioVideoScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMedioVideoScreenDlg::IDD, pParent)
	, m_strFileName(_T(""))
	, m_strFilePath(_T(""))
	, m_nRate(5)
	, m_strPlace(_T(""))
{
	m_p1 = m_p2 = 0;
	m_IsStop = FALSE;
	m_IsPause = TRUE;
	m_nCount = 0;
	m_GetCurWndHwnd = NULL;
	m_nSize = 0;
	m_pBitmap = NULL;
	m_hWndVedio = NULL;
	g_hVedioHwnd = NULL;
	m_bPathisLoad = false;
	m_strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (m_strPrompt.IsEmpty())
	{
		m_strPrompt = _T("��ʾ");
	}
	if (g_bSupportU7OpenFile)
	{
		m_strFileName = _T("");
	}
	else
	{
		GetFileName(m_strFileName);
		GetFilePath(m_strFilePath);
	}
	m_bIsWndowClosing = FALSE;
	m_strCurrentFileName = _T("");
}

CRect CMedioVideoScreenDlg::m_rectLarge = CRect(0,0,0,0);

//�ļ������ܺ��С���������/������\���������ַ�
void CMedioVideoScreenDlg::GetFileName(CString &stringname)
{
	CTime time;
	CTime timeNow(time.GetCurrentTime());
	int year =   timeNow.GetYear();
	int mouth =  timeNow.GetMonth();
	int day   =  timeNow.GetDay();
	int hour  =  timeNow.GetHour();
	int minute = timeNow.GetMinute();
	int second = timeNow.GetSecond();
	CString strSeperator = _T("_");
	//CString string1 = _T("  ");
	//CString string2 = _T("��");
	stringname.Format(L"%d%s%d%s%d%s%d%s%d%s%d",year,strSeperator,mouth, strSeperator, day, strSeperator, hour, strSeperator, minute,strSeperator,second);
}

//��ȡ����ؼ����Ĵ�����
void CMedioVideoScreenDlg::GetFilePath(CString &stringpath)
{
	//���ȴ������ļ��л�ȡ·��
	/*CString configFilePath = GetCurrentPath() + _T("\\WBConfig.ini");
	CString strSection = _T("ArgumentSave");
	char buff[MAX_PATH];
	GetPrivateProfileString(strSection, _T("VedioScreenFilePath"), _T(""), buff, MAX_PATH, configFilePath);
	stringpath = buff;*/
	stringpath = ReadConfigText(_T("WhiteBoardControlText"),_T("VedioScreenFilePath"));
	if (!stringpath.IsEmpty())
	{
		return;
	}
	//��������ļ���û�м�¼·������ôѡ��ʣ��ռ������Ǹ�����·��
	ULARGE_INTEGER   ulTotalSize;       //�����ܿռ��С       
	ULARGE_INTEGER   ulFreeSize;        //����ʣ��ռ��С
	ULARGE_INTEGER   ulAvailLableSize;  //¼���ļ���Ҫ�Ŀռ��С

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
		CString str = ("%s", cLetter);
		CString str1 = str.Left(1);
		CString string2 = _T("\\");
		CString string1 = _T(":");
		csStr.Format(L"%s%s%s", str1, string1, string2);
		UINT driveType;

		//Get the drive type of drive I
		driveType = GetDriveType(csStr);
		switch(driveType)
		{
		case DRIVE_FIXED:
			::GetDiskFreeSpaceEx(csStr,&ulTotalSize,&ulFreeSize,&ulAvailLableSize);
			presize =  ulTotalSize.QuadPart/1024/1024;
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

CMedioVideoScreenDlg::~CMedioVideoScreenDlg()
{
	if (TRUE == m_IsStop)
	{
		StopThread();
	}
}

void CMedioVideoScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILE, m_FileName);
	//DDX_Text(pDX, IDC_EDIT_FILE, m_strFileName);
	DDX_Control(pDX, IDC_EDIT_PATH, m_FilePath);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strFilePath);
	DDX_Control(pDX, IDC_EDIT_RATE, m_Rate);
	//DDX_Text(pDX, IDC_EDIT_RATE, m_nRate);
	DDX_Text(pDX, IDC_STATICPLACE, m_strPlace);
	DDX_Control(pDX, IDC_STATICPLACE, m_csPlace);
	DDX_Control(pDX, IDC_BUT_PAUSE, m_bPauseScreen);
	DDX_Control(pDX, IDC_BUT_AREA, m_bAreaScreen);
	DDX_Control(pDX, IDC_BUT_PROPERTY, m_bPropertyScreen);
	DDX_Control(pDX, IDC_BUT_SET_PATH, m_bPathScreen);
	DDX_Control(pDX, IDC_BUT_STOP, m_bStopScreen);
	DDX_Control(pDX, IDC_BUT_WND_SCREEN, m_bWndScreen);
	DDX_Control(pDX, IDC_BUTT_FULL_SCREEN, m_bFullScreen);
	DDX_Control(pDX, IDC_FILENAME, m_screenFileName);
	DDX_Control(pDX, IDC_FRAME, m_screenFrame);
	DDX_Control(pDX, IDC_FRAMERATE, m_screenFrameRate);
	DDX_Control(pDX, IDC_PATHNAME, m_screenPathName);
	DDX_Control(pDX, IDC_STATICVEDIOSTOP, m_csVedioStop);
	//DDX_Control(pDX, IDC_STATIC_PATH, m_csSavePath);
	//DDV_MinMaxInt(pDX, m_nRate, 1, 10);

	
}

BEGIN_MESSAGE_MAP(CMedioVideoScreenDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTT_FULL_SCREEN, &CMedioVideoScreenDlg::OnBnClickedButtFullScreen)
	ON_BN_CLICKED(IDC_BUT_AREA, &CMedioVideoScreenDlg::OnBnClickedButArea)
	ON_BN_CLICKED(IDC_BUT_WND_SCREEN, &CMedioVideoScreenDlg::OnBnClickedButWndScreen)
	ON_BN_CLICKED(IDC_BUT_STOP, &CMedioVideoScreenDlg::OnBnClickedButStop)
	ON_BN_CLICKED(IDC_BUT_PAUSE, &CMedioVideoScreenDlg::OnBnClickedButPause)
	ON_BN_CLICKED(IDC_BUT_SET_PATH, &CMedioVideoScreenDlg::OnBnClickedButSetPath)
	ON_BN_CLICKED(IDC_BUT_PROPERTY, &CMedioVideoScreenDlg::OnBnClickedButProperty)
	ON_WM_ERASEBKGND()
//	ON_EN_KILLFOCUS(IDC_EDIT_FILE, &CMedioVideoScreenDlg::OnKillfocusEditFile)
//ON_WM_WINDOWPOSCHANGED()
//ON_WM_KILLFOCUS()
ON_EN_KILLFOCUS(IDC_EDIT_FILE, &CMedioVideoScreenDlg::OnKillfocusEditFile)
ON_EN_KILLFOCUS(IDC_EDIT_RATE, &CMedioVideoScreenDlg::OnKillfocusEditRate)
ON_WM_TIMER()
END_MESSAGE_MAP()


// CMedioVideoScreenDlg ��Ϣ�������
BOOL CMedioVideoScreenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_csPlace.SetTextPosition(TRUE);
	m_csVedioStop.SetTextPosition(TRUE);
	CString strIsSelectRecFun;
	strIsSelectRecFun = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("SelectRecFun"));
	if (strIsSelectRecFun.IsEmpty())
	{
		strIsSelectRecFun = _T("��ѡ��������Ҫ��¼���ܣ�");
	}
	m_csVedioStop.SetCaption(strIsSelectRecFun);
	m_csVedioStop.SetWindowTextBool(false);

	CString strLanguageEn;
	int nLen = 0;//�����ַ������Ⱦ�����С
	strLanguageEn = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("Language"));
	if (strLanguageEn.IsEmpty())
	{
		strLanguageEn = _T("");
	}
	if (!strLanguageEn.IsEmpty())
	{
		nLen = 10;
	}
	CRect rectAttribute;
	m_csVedioStop.GetClientRect(&rectAttribute);
	m_csVedioStop.MoveWindow(rectAttribute.left,rectAttribute.top,rectAttribute.Width(),rectAttribute.Height() + nLen);
	//m_csSavePath.SetTextPosition(TRUE);
	//m_csSavePath.SetCaption(_T("��ѡ��������Ҫ��¼���ܣ�"));
	GetDlgItem(IDC_BUT_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_PAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTT_FULL_SCREEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT_WND_SCREEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT_AREA)->EnableWindow(TRUE);

	//��ʼ��֡�ʱ༭��
	m_Rate.SetWindowText(_T("5"));
	m_FileName.SetWindowText(m_strFileName);

	CRect fileNameRect;
	CRect patNamehRect;
	CRect frameRect;
	CRect frameRateRect;

	CString mediaFileName;
	mediaFileName = _T("");
	m_screenFileName.GetClientRect(&fileNameRect);
	m_screenFileName.ClientToScreen(&fileNameRect);
	ScreenToClient(&fileNameRect);
	mediaFileName = ReadConfigText(_T("WhiteBoardControlText"),_T("MediaFileName"));
	if (mediaFileName.IsEmpty())
	{
		mediaFileName = _T("�ļ���:");
	}
	m_screenFileName.SetCaption(mediaFileName);

	CString mediaFilePath;
	mediaFilePath = _T("");
	m_screenPathName.GetClientRect(&patNamehRect);
	m_screenPathName.ClientToScreen(&patNamehRect);
	ScreenToClient(&patNamehRect);
	mediaFilePath = ReadConfigText(_T("WhiteBoardControlText"),_T("MediaFilePath"));
	if (mediaFilePath.IsEmpty())
	{
		mediaFilePath = _T("·����:");
	}
	m_screenPathName.SetCaption(mediaFilePath);

	CString mediaFrame;
	mediaFrame = _T("");
	m_screenFrame.GetClientRect(&frameRect);
	m_screenFrame.ClientToScreen(&frameRect);
	ScreenToClient(&frameRect);
	mediaFrame = ReadConfigText(_T("WhiteBoardControlText"),_T("MediaFrame"));
	if (mediaFrame.IsEmpty())
	{
		mediaFrame = _T("֡��:");
	}
	m_screenFrame.SetCaption(mediaFrame);

	CString mediaFrameRate;
	mediaFrameRate = _T("");
	m_screenFrameRate.GetClientRect(&frameRateRect);
	m_screenFrameRate.ClientToScreen(&frameRateRect);
	ScreenToClient(&frameRateRect);
	mediaFrameRate = ReadConfigText(_T("WhiteBoardControlText"),_T("MEdiaFrameRate"));
	if (mediaFrameRate.IsEmpty())
	{
		mediaFrameRate = _T("֡��");
	}
	m_screenFrameRate.SetCaption(mediaFrameRate);
	CRect fullRect;
	CRect arerRect;
	CRect wndRect;
	CRect pauseRect;
	CRect stopRect;
	CRect propertyRect;
	CRect pathRect;

	m_bFullScreen.SetType(TYPE_FULLSCREENRECORD);
	m_bAreaScreen.SetType(TYPE_FULLSCREENRECORD);
	m_bWndScreen.SetType(TYPE_FULLSCREENRECORD);
	m_bPauseScreen.SetType(TYPE_FULLSCREENRECORD);
	m_bStopScreen.SetType(TYPE_FULLSCREENRECORD);
	m_bPropertyScreen.SetType(TYPE_FULLSCREENRECORD);
	m_bPathScreen.SetType(TYPE_FULLSCREENRECORD);

	m_bFullScreen.GetClientRect(&fullRect);
	m_bFullScreen.ClientToScreen(&fullRect);
	ScreenToClient(&fullRect);
	m_bFullScreen.SetWindowPos(&wndTop,fullRect.left,fullRect.top + nLen,fullRect.Width(),fullRect.Height(),SWP_SHOWWINDOW);

	m_bAreaScreen.GetClientRect(&arerRect);
	m_bAreaScreen.ClientToScreen(&arerRect);
	ScreenToClient(&arerRect);
	m_bAreaScreen.SetWindowPos(&wndTop,arerRect.left,arerRect.top + nLen,arerRect.Width(),arerRect.Height(),SWP_SHOWWINDOW);

	m_bWndScreen.GetClientRect(&wndRect);
	m_bWndScreen.ClientToScreen(&wndRect);
	ScreenToClient(&wndRect);
	m_bWndScreen.SetWindowPos(&wndTop, wndRect.left, wndRect.top + nLen, wndRect.Width(), wndRect.Height(), SWP_SHOWWINDOW);
	if(g_bSupportU7OpenFile)
	{
		CRect MVSDlgRc;
		GetClientRect(&MVSDlgRc);

		m_bPauseScreen.GetClientRect(&pauseRect);
		m_bPauseScreen.ClientToScreen(&pauseRect);
		ScreenToClient(&pauseRect);
		m_bPauseScreen.SetWindowPos(&wndTop, (MVSDlgRc.Width()/2 - pauseRect.Width())/2, pauseRect.top + nLen, pauseRect.Width(), pauseRect.Height(), SWP_SHOWWINDOW);

		m_bStopScreen.GetClientRect(&stopRect);
		m_bStopScreen.ClientToScreen(&stopRect);
		ScreenToClient(&stopRect);
		m_bStopScreen.SetWindowPos(&wndTop, MVSDlgRc.Width()/2 + (MVSDlgRc.Width()/2 - stopRect.Width())/2, stopRect.top + nLen, stopRect.Width(), stopRect.Height(), SWP_SHOWWINDOW);

		m_bPropertyScreen.ShowWindow(SW_HIDE);
		m_bPathScreen.ShowWindow(SW_HIDE);
	}
	else
	{
		m_bPauseScreen.GetClientRect(&pauseRect);
		m_bPauseScreen.ClientToScreen(&pauseRect);
		ScreenToClient(&pauseRect);
		m_bPauseScreen.SetWindowPos(&wndTop, pauseRect.left, pauseRect.top + nLen, pauseRect.Width(), pauseRect.Height(), SWP_SHOWWINDOW);

		m_bStopScreen.GetClientRect(&stopRect);
		m_bStopScreen.ClientToScreen(&stopRect);
		ScreenToClient(&stopRect);
		m_bStopScreen.SetWindowPos(&wndTop, stopRect.left, stopRect.top + nLen, stopRect.Width(), stopRect.Height(), SWP_SHOWWINDOW);

		m_bPropertyScreen.GetClientRect(&propertyRect);
		m_bPropertyScreen.ClientToScreen(&propertyRect);
		ScreenToClient(&propertyRect);
		m_bPropertyScreen.SetWindowPos(&wndTop, propertyRect.left, propertyRect.top + nLen, propertyRect.Width(), propertyRect.Height(), SWP_SHOWWINDOW);

		m_bPathScreen.GetClientRect(&pathRect);
		m_bPathScreen.ClientToScreen(&pathRect);
		ScreenToClient(&pathRect);
		m_bPathScreen.SetWindowPos(&wndTop, pathRect.left, pathRect.top + nLen, pathRect.Width(), pathRect.Height(), SWP_SHOWWINDOW);
	}
	//���ð�ť�ı�
	CString BtnText;
	BtnText = _T("");
	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("FullScleenRecordBtn"));
	if (BtnText.IsEmpty())
	{
		BtnText = _T("ȫ��");
	}
	m_bFullScreen.SetName(BtnText);
	m_bFullScreen.SetWindowText(BtnText);

	BtnText = _T("");
	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("AreaScleenRecordBtn"));
	if (BtnText.IsEmpty())
	{
		BtnText = _T("����");
	}
	m_bAreaScreen.SetName(BtnText);
	m_bAreaScreen.SetWindowText(BtnText);

	BtnText = _T("");
	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("WndScleenRecordBtn"));
	if(BtnText.IsEmpty())
	{
		BtnText = _T("����");
	}
	m_bWndScreen.SetName(BtnText);
	m_bWndScreen.SetWindowText(BtnText);

	BtnText = _T("");
	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("PropertyScleenRecordBtn"));
	if (BtnText.IsEmpty())
	{
		BtnText = _T("����");
	}
	m_bPropertyScreen.SetName(BtnText);
	m_bPropertyScreen.SetWindowText(BtnText);

	if (g_bSupportU7OpenFile)//U7����������
	{
		m_bPropertyScreen.ShowWindow(SW_HIDE);
	}

	BtnText = _T("");
	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("PauseScleenRecordBtn"));
	if (BtnText.IsEmpty())
	{
		BtnText = _T("��ͣ");
	}
	m_bPauseScreen.SetName(BtnText);
	m_bPauseScreen.SetWindowText(BtnText);

	BtnText = _T("");
	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("StopScleenRecordBtn"));
	if (BtnText.IsEmpty())
	{
		BtnText = _T("ֹͣ");
	}
	m_bStopScreen.SetName(BtnText);
	m_bStopScreen.SetWindowText(BtnText);

	BtnText = _T("");
	BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("PathScleenRecordBtn"));
	if (BtnText.IsEmpty())
	{
		BtnText = _T("���");
	}
	m_bPathScreen.SetName(BtnText);
	m_bPathScreen.SetWindowText(BtnText);

	//���ñ����ı�
	CString strScreenREC;
	strScreenREC = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("ScreenREC"));
	if (strScreenREC.IsEmpty())
	{
		strScreenREC = _T("��Ļ¼��");
	}
	SetWindowText(strScreenREC);

	CString strFunAddFail;
	strFunAddFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FunAddFail"));
	if (strFunAddFail.IsEmpty())
	{
		strFunAddFail = _T("����Ѱַʧ��");
	}
	if (g_bSupportU7OpenFile)
	{  
		m_bPathScreen.ShowWindow(SW_HIDE);
		m_FilePath.SetWindowText(g_bU7FileTempDir);
		m_nRate = 5;
		OnBnClickedButProperty();
	}
	else
	{
		m_strFilePath = g_strAvconTempPath;
		m_FilePath.SetWindowText(m_strFilePath);
		m_nRate = 5;
		OnBnClickedButProperty();
		m_FilePath.GetWindowText(m_strFilePath);
	}
	typedef  CString  (*lpGetDiskPlace)(CString strPath);
	//m_hDLL = LoadLibrary("Vedio.dll");
	//ȷ����̬���صĿ�·����ȷ
	HMODULE hModule = GetModuleHandle(TEXT("whiteboard.dll"));
	if (!hModule) return FALSE;

	TCHAR strName[MAX_PATH];
	GetModuleFileName(hModule, strName, MAX_PATH);
	CString strPath = strName;
	int nPos = strPath.ReverseFind('\\');
	if(-1 == nPos) nPos = strPath.ReverseFind('/');
	if(-1 == nPos) return FALSE;
	strPath = strPath.Left(nPos);

	CString tempPath = _T("");
	SetCurrentDirectory(strPath.GetBuffer());

	CString strResPath = _T("");

	strResPath.Format(TEXT("%s\\WBSCREC.dll"), strPath);

	m_hDLL = LoadLibrary(strResPath.GetBuffer());
	strPath.ReleaseBuffer();
	strResPath.ReleaseBuffer();
	
	if (NULL == m_hDLL)
	{
		CString strDllLoadFail;
		strDllLoadFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("DllLoadFail"));
		if (strDllLoadFail.IsEmpty())
		{
			strDllLoadFail = _T("��DLL�ļ�����ʧ��(lpGetDiskPlace)��");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strDllLoadFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strDllLoadFail);
		}
		else
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			MessageBox(_T("��(lpGetDiskPlace)��")+strDllLoadFail);
		}
		return FALSE;
	}
	
	if (g_bSupportU7OpenFile && g_pHPWBDataNotify)//֧��U7ʹ��MessageBox
	{
		m_strPlace = _T("");
		typedef void (* lpSetU7HPWBDataNotify)(IHPNetNotify* U7HPWBDataNotify);
		lpSetU7HPWBDataNotify SetU7HPWBDataNotify = (lpSetU7HPWBDataNotify)GetProcAddress(m_hDLL,"SetU7HPWBDataNotify");
		if (SetU7HPWBDataNotify == NULL)
		{		
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt,  _T("��SetU7HPWBDataNotify��")+strFunAddFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(_T("��SetU7HPWBDataNotify��")+strFunAddFail);
			FreeLibrary(m_hDLL); m_hDLL=NULL;
			return FALSE;
		}
		SetU7HPWBDataNotify(g_pHPWBDataNotify);
		
		if (g_bSupportU7OpenFile)
		{
			typedef void (* lpbSupportU7OpenFile)(BOOL bSupportU7OpenFile);
			lpbSupportU7OpenFile bSupportU7OpenFile = (lpbSupportU7OpenFile)GetProcAddress(m_hDLL,"SetSupportU7Open");
			if (bSupportU7OpenFile == NULL)
			{
				if(g_pHPWBDataNotify)
				{
					if (g_pWhiteBoardView)
					{
						g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
					}
					//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, _T("��SetSupportU7Open��")+strFunAddFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(_T("��SetSupportU7Open��")+strFunAddFail);
				}
				FreeLibrary(m_hDLL); m_hDLL=NULL;
				return FALSE;
			}
			bSupportU7OpenFile(g_bSupportU7OpenFile);
		}
	}
 	else
 	{
		lpGetDiskPlace pGetDiskPlace = (lpGetDiskPlace)GetProcAddress(m_hDLL,"GetResidualPlace");
		if (NULL == pGetDiskPlace)
		{
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}
				//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, _T("��GetResidualPlace��")+strFunAddFail, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(_T("��GetResidualPlace��")+strFunAddFail);
			}
			else
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}
				MessageBox(_T("��GetResidualPlace��")+strFunAddFail);
			}
			FreeLibrary(m_hDLL); m_hDLL=NULL;
			return FALSE;
		}	
 		m_strPlace = pGetDiskPlace(m_strFilePath);
 	}

	CString strDiskSpace;
	strDiskSpace = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("DiskSpace"));
	if (strDiskSpace.IsEmpty())
	{
		strDiskSpace = _T("����ʣ��ռ�");
	}
	m_csPlace.SetCaption(strDiskSpace+m_strPlace);
	CRect WndRect,ParentRect = (0,0,0,0);
	GetWindowRect(&WndRect);
	if (NULL != g_pWhiteBoardView)
	{
		g_pWhiteBoardView->GetWindowRect(&ParentRect);
	}
	
	//SetWindowPos(&wndTopMost, WndRect.left+100, WndRect.top+100, WndRect.Width(), WndRect.Height(), SWP_NOSIZE|SWP_NOMOVE| SWP_SHOWWINDOW);
	SetWindowPos(&wndTop, WndRect.left+100, WndRect.top+00, WndRect.Width(), WndRect.Height(), SWP_NOSIZE|SWP_NOMOVE| SWP_SHOWWINDOW);
	MoveWindow(ParentRect.left + 90,ParentRect.top + 70,WndRect.Width(), WndRect.Height() + nLen);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMedioVideoScreenDlg::OnBnClickedButtFullScreen()
{
	CString strDeleteFile = _T("");
	CString str = _T("");
	if (g_bSupportU7OpenFile)
	{
		//wchar_t szFilter[MAX_PATH];
		CString strFilter;
		strFilter = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("Filter"));
		if (strFilter.IsEmpty())
		{
			strFilter = _T("��Ƶ (*.avi)|*.avi||");
		}
		CString strfilterName;
		strfilterName = g_strAvconUserID;
		SYSTEMTIME sys; 
		GetLocalTime( &sys );
		CString strTime;
		strTime.Format(_T("%d%d%d%d%d"),sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute);
		strfilterName += strTime;
		//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);
		char* sFilePath = new char[MAX_PATH];
		memset(sFilePath,'\0',MAX_PATH);		
		bool bOpen = g_pHPWBDataNotify->OpenSaveFile(FALSE, sFilePath, NULL,strFilter, this->GetSafeHwnd(),strfilterName);

		
		str = A2U(sFilePath);
		
		//DDX_Control(pDX, IDC_EDIT_PATH, m_FilePath);
		//m_FilePath,SetWindowText(str);
		//str = sFilePath;
		if (str == _T(""))
		{
			delete[] sFilePath;
			return;
		}
		else
		{
			//��Ҫ��ʱ��OPenSave��������·�������룬���������ж�wangxiaobin
			BOOL bOK = IsFilePathOk(str);
			if (bOK == FALSE)
			{
				if (!m_bPathisLoad)
				{
					CString strEnterFileName = _T("");
					strEnterFileName = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SaveFilePathNull"));
					if (strEnterFileName.IsEmpty())
					{
						strEnterFileName = _T("¼���ļ������λ�ó�����ǰ·���޷������ļ�");
					}
					//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strEnterFileName, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strEnterFileName);
				}
				

				delete[] sFilePath;
				return;
			}
		}
		
		char FileName[MAX_PATH] = {0};
		char Name[MAX_PATH] = {0};
 		GetAbsolute_path(sFilePath,Name);
		GetCurFileName(Name,FileName);
		m_strFileName = A2U(FileName);
		strDeleteFile = str;

		if (g_pWhiteBoardView == NULL)
		{
			delete[] sFilePath;
			return;
		}
		if (bOpen)
		{
			char FileName[MAX_PATH] = {0};
			GetCurFileName(sFilePath,FileName,'\\');
			CString strchange ;
			strchange = A2U(FileName);
			m_FilePath.SetWindowText(strchange);
			delete[] sFilePath;
		}
		else
		{
			delete[] sFilePath;
			return;
		}
		if (!IsFileNameValidU7(m_strFileName))
		{
			CString strSetFileName;
			strSetFileName = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("SetFileName"));
			if (strSetFileName.IsEmpty())
			{
				strSetFileName = _T("�ļ������ԣ�¼����ֹ���ٴ������ļ���");
			}
			m_csVedioStop.SetCaption(strSetFileName);
			m_csVedioStop.SetWindowTextBool(false);
			Invalidate();
			return;
		}
	} 
	/*-----------------------------------------------------------------------*/
	if (!::IsWindow(m_csVedioStop.m_hWnd))
	{
		return;
	}
	CString strFullScreenRecording;
	strFullScreenRecording = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("FullScreenRecording"));
	if (strFullScreenRecording.IsEmpty())
	{
		strFullScreenRecording = _T("���ڽ���ȫ��¼����Ҫ��ֹ���뵥����ֹͣ����ť��");
	}
	m_csVedioStop.SetCaption(strFullScreenRecording);
	m_csVedioStop.SetWindowTextBool(false);

	UpdateData(TRUE);
	
	if (!g_bSupportU7OpenFile)
	{
		if (!IsFileNameValid())
		{
			SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
				SWP_NOMOVE | SWP_NOZORDER);
			m_nSize = 0;
			Invalidate();
			return;
		}

		if(!IsRateValid())
		{
			CString strFpsNeedNum;
			strFpsNeedNum = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("FpsNeedNum"));
			if (strFpsNeedNum.IsEmpty())
			{
				strFpsNeedNum = _T("֡�������������֣�1-10����");
			}
			MessageBox(strFpsNeedNum,m_strPrompt,MB_OK|MB_ICONASTERISK);
			SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
				SWP_NOMOVE | SWP_NOZORDER);
			m_nSize = 0;
			Invalidate();
			return;
		}
	}
	

	if (g_pWhiteBoardView->m_hWnd == NULL)
	{
		return;
	}

	if (m_strFilePath.IsEmpty())
	{
		CString strPathNotNull;
		strPathNotNull = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("PathNotNull"));
		if (strPathNotNull.IsEmpty())
		{
			strPathNotNull = _T("�ļ�·������Ϊ��");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strPathNotNull, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strPathNotNull);
		}
		else
		{
			MessageBox(strPathNotNull);
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
			SWP_NOMOVE | SWP_NOZORDER);
		m_nSize = 0;
		Invalidate();
		return;
	}

	if (IsFileExisted() && PathFileExists(strDeleteFile))
	{
		BOOL bflag = FALSE;
		bflag = ::DeleteFile(strDeleteFile);
	}
	
	/*-----------------------------------------------------------------------*/
	GetDlgItem(IDC_BUT_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT_PAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTT_FULL_SCREEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_WND_SCREEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_AREA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_SET_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RATE)->EnableWindow(FALSE);
	/*-----------------------------------------------------------------------*/

	typedef void (*lpStartVedio)(CString strFilePath,CString strFileName,int nRate,CPoint p1,CPoint p2,int nFlag,HWND GetCurHwnd,CPoint pt);
	lpStartVedio pStartVedio = (lpStartVedio)GetProcAddress(m_hDLL,"StartVedio");
	if (NULL == pStartVedio)
	{
		CString strFunAddError;
		strFunAddError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FunAddError"));
		if (strFunAddError.IsEmpty())
		{
			strFunAddError = _T("����Ѱַ����");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, _T("��pStartVedio��")+strFunAddError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(_T("��pStartVedio��")+strFunAddError);

		}
		else
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			MessageBox(_T("��pStartVedio��")+strFunAddError);
		}
		FreeLibrary(m_hDLL); m_hDLL=NULL;
		return ;
	}
	CRect rectTemp;
	::GetWindowRect(g_pWhiteBoardView->GetSafeHwnd(),&rectTemp);
	CPoint ptTemp;	
	ptTemp.x = rectTemp.left+rectTemp.Width()/2;
	ptTemp.y = rectTemp.top+ rectTemp.Height()/2;
	
	pStartVedio(m_strFilePath,m_strFileName,m_nRate,0,0,FULLSCREEN,NULL,ptTemp);
	m_IsStop = TRUE;
	m_strCurrentFileName = str;
	/*-----------------------------------------------------------------------*/
	KillTimer(CHECKMEMORYSTATUSISFULL);
	SetTimer(CHECKMEMORYSTATUSISFULL,180000,NULL);
}


void CMedioVideoScreenDlg::OnBnClickedButArea()
{
	
	/*-----------------------------------------------------------------------*/
	CWVedio Dlg;
	int ret = Dlg.DoModal();
	//�ϴδ�����
	g_hVedioHwnd = NULL;

	if (!::IsWindow(Dlg.m_hWnd) && g_bMedioScreenWndEnd == FALSE)
	{
		g_bMedioScreenWndEnd = TRUE;
		return;
	}
	if (ret == 100 )
	{
		Dlg.GetPoint(m_p1,m_p2);
	}
	else
	{
		m_p1 = CPoint(-1,-1);
		m_p2 = CPoint(-1,-1);
	}
	//����û��ѡ��������򣬶��ǵ���Ҽ��˳�ѡ����ôֱ�ӷ���
	if (m_p1 == CPoint(-1,-1) && m_p2 == CPoint(-1,-1))
	{
		CString strExitRecRegion;
		strExitRecRegion = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("ExitRecRegion"));
		if (strExitRecRegion.IsEmpty())
		{
			strExitRecRegion = _T("���Ѿ��˳�����¼�ƣ�");
		}
		m_csVedioStop.SetCaption(strExitRecRegion);
		m_csVedioStop.SetWindowTextBool(false);
		SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
		g_pWhiteBoardView->SetMediaVideoRButtonDown(true);
		return;
	}





	CString strDeleteFile = _T("");
	CString str = _T("");
	if (g_bSupportU7OpenFile)
	{
		CString strFilter;
		strFilter = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("Filter"));
		if (strFilter.IsEmpty())
		{
			strFilter = _T("��Ƶ (*.avi)|*.avi||");
		}
		CString strfilterName;
		strfilterName = g_strAvconUserID;
		SYSTEMTIME sys; 
		GetLocalTime( &sys );
		CString strTime;
		strTime.Format(_T("%d%d%d%d%d"),sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute);
		strfilterName += strTime;
		//wchar_t szFilter[MAX_PATH];
		//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);
		char* sFilePath = new char[MAX_PATH];
		memset(sFilePath,0,MAX_PATH);
		bool bOpen = g_pHPWBDataNotify->OpenSaveFile(FALSE, sFilePath, NULL,strFilter, this->GetSafeHwnd(),strfilterName);


		str = A2U(sFilePath);
		if (str == _T(""))
		{
			delete[] sFilePath;
			return;
		}
		else
		{
			//��Ҫ��ʱ��OPenSave��������·�������룬���������ж�wangxiaobin
			BOOL bOK = IsFilePathOk(str);
			if (bOK == FALSE)
			{
				if (!m_bPathisLoad)
				{
					CString strEnterFileName = _T("");
					strEnterFileName = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SaveFilePathNull"));
					if (strEnterFileName.IsEmpty())
					{
						strEnterFileName = _T("¼���ļ������λ�ó�����ǰ·���޷������ļ�");
					}
					//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strEnterFileName, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strEnterFileName);
				}

				delete[] sFilePath;
				return;
			}
		}

		char FileName[MAX_PATH] = {0};
		char Name[MAX_PATH] = {0};
		GetAbsolute_path(sFilePath,Name);
		GetCurFileName(Name,FileName);
		m_strFileName = A2U(FileName);
		strDeleteFile = str;

		if (g_pWhiteBoardView == NULL)
		{
			delete[] sFilePath;
			return;
		}
		if (bOpen)
		{
			char FileName[MAX_PATH] = {0};
			GetCurFileName(sFilePath,FileName,'\\');
			CString strchange ;
			strchange = A2U(FileName);
			m_FilePath.SetWindowText(strchange);
			delete[] sFilePath;
		}
		else
		{
			delete[] sFilePath;
			return;
		}

		if (!IsFileNameValidU7(m_strFileName))
		{
			CString strSetFileName;
			strSetFileName = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("SetFileName"));
			if (strSetFileName.IsEmpty())
			{
				strSetFileName = _T("�ļ������ԣ�¼����ֹ���ٴ������ļ���");
			}
			m_csVedioStop.SetCaption(strSetFileName);
			m_csVedioStop.SetWindowTextBool(false);
			Invalidate();
			return;
		}
	}

	/*-----------------------------------------------------------------------*/
	CString strSelectRecRegion;
	strSelectRecRegion = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("SelectRecRegion"));
	if (strSelectRecRegion.IsEmpty())
	{
		strSelectRecRegion = _T("�밴��������϶�����ѡ��¼�����򣬵����Ҽ��˳���");
	}
	m_csVedioStop.SetCaption(strSelectRecRegion);
	m_csVedioStop.SetWindowTextBool(false);
	//m_csSavePath.SetCaption(_T(""));
	UpdateData(TRUE);

	if (!g_bSupportU7OpenFile)
	{
		if (!IsFileNameValid())
		{
			SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
				SWP_NOMOVE | SWP_NOZORDER);
			m_nSize = 0;
			Invalidate();
			return;
		}

		if(!IsRateValid())
		{
			CString strFpsNeedNum;
			strFpsNeedNum = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("FpsNeedNum"));
			if (strFpsNeedNum.IsEmpty())
			{
				strFpsNeedNum = _T("֡�������������֣�1-10����");
			}
			MessageBox(strFpsNeedNum,m_strPrompt,MB_OK|MB_ICONASTERISK);
			SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
				SWP_NOMOVE | SWP_NOZORDER);
			m_nSize = 0;
			Invalidate();
			return;
		}
	}

	if (g_pWhiteBoardView->m_hWnd == NULL)
	{
		return;
	}
	if (m_strFilePath.IsEmpty())
	{
		CString strPathNotNull;
		strPathNotNull = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("PathNotNull"));
		if (strPathNotNull.IsEmpty())
		{
			strPathNotNull = _T("�ļ�·������Ϊ��");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strPathNotNull, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strPathNotNull);
		}
		else
		{
			MessageBox(strPathNotNull);
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
			SWP_NOMOVE | SWP_NOZORDER);
		m_nSize = 0;
		Invalidate();
		return;
	}

	CString strRegionRecordBegin;
	strRegionRecordBegin = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("RegionRecordBegin"));
	if (strRegionRecordBegin.IsEmpty())
	{
		strRegionRecordBegin = _T("��ʼ��������¼����Ҫ��ֹ���뵥����ֹͣ����ť��");
	}
	m_csVedioStop.SetCaption(strRegionRecordBegin);
	m_csVedioStop.SetWindowTextBool(false);


	if (IsFileExisted() && PathFileExists(strDeleteFile))
	{
		::DeleteFile(strDeleteFile);
	}

	/*-----------------------------------------------------------------------*/
	GetDlgItem(IDC_BUT_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT_PAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTT_FULL_SCREEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_WND_SCREEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_AREA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_SET_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RATE)->EnableWindow(FALSE);
	/*-----------------------------------------------------------------------*/
	typedef void (*lpStartVedio)(CString strFilePath,CString strFileName,int nRate,CPoint p1,CPoint p2,int nFlag,HWND GetCurHwnd);
	lpStartVedio pStartVedio = (lpStartVedio)GetProcAddress(m_hDLL,"StartVedio");
	if (NULL == pStartVedio)
	{
		CString strFunAddError;
		strFunAddError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FunAddError"));
		if (strFunAddError.IsEmpty())
		{
			strFunAddError = _T("����Ѱַ����");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}

			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, _T("��pStartVedio��")+strFunAddError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(_T("��pStartVedio��")+strFunAddError);
		}
		else
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			MessageBox(_T("��pStartVedio��")+strFunAddError);
		}
		FreeLibrary(m_hDLL); m_hDLL=NULL;
		return ;
	}
	pStartVedio(m_strFilePath,m_strFileName,m_nRate,m_p1,m_p2,AREASCREEN,NULL);
	m_IsStop = TRUE;
	m_strCurrentFileName = str;
	/*-----------------------------------------------------------------------*/
	KillTimer(CHECKMEMORYSTATUSISFULL);
	SetTimer(CHECKMEMORYSTATUSISFULL,180000,NULL);
}


void CMedioVideoScreenDlg::OnBnClickedButWndScreen()
{
	CString strDeleteFile = _T("");
	CString str = _T("");
	
	/*-----------------------------------------------------------------------*/
	CString strMoveDisc;
	strMoveDisc = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("MoveDisc"));
	if (strMoveDisc.IsEmpty())
	{
		strMoveDisc = _T("��ѡ���˴���¼�ƹ��ܣ����϶����������Բ������Ҫ¼�ƵĴ��ڡ�");
	}
	m_csVedioStop.SetCaption(strMoveDisc);
	m_csVedioStop.SetWindowTextBool(false);
	//m_csSavePath.SetCaption(_T(""));
	UpdateData(TRUE);
// 	if (!IsFileNameValid())
// 	{
// 		SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
// 			SWP_NOMOVE | SWP_NOZORDER);
// 		m_nSize = 0;
// 		m_csVedioStop.SetCaption(_T("�ļ������ԣ�¼����ֹ���ٴ������ļ���"));
// 		Invalidate();
// 		return;
// 	}
	if (!g_bSupportU7OpenFile)
	{
		if (!IsFileNameValid())
		{
			SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
				SWP_NOMOVE | SWP_NOZORDER);
			m_nSize = 0;
			Invalidate();
			return;
		}

		if(!IsRateValid())
		{
			CString strFpsNeedNum;
			strFpsNeedNum = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("FpsNeedNum"));
			if (strFpsNeedNum.IsEmpty())
			{
				strFpsNeedNum = _T("֡�������������֣�1-10����");
			}
			MessageBox(strFpsNeedNum,m_strPrompt,MB_OK|MB_ICONASTERISK);
			SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
				SWP_NOMOVE | SWP_NOZORDER);
			m_nSize = 0;
			Invalidate();
			return;
		}
	}
	if (g_pWhiteBoardView->m_hWnd == NULL)
	{
		return;
	}
	

// 	if (IsFileExisted())
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			m_csVedioStop.SetCaption(_T("���ļ��Ѿ����ڣ����ܸ��ǣ����޸��ļ���!"));
// 			return;
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("���ļ��Ѿ����ڣ����޸��ļ�����"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("���ļ��Ѿ����ڣ����޸��ļ�����"));
// 		}
// 		if (g_pWhiteBoardView == NULL)
// 		{
// 			return;
// 		}
// 		SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
// 			SWP_NOMOVE | SWP_NOZORDER);
// 		m_nSize = 0;
// 		Invalidate();
// 		return;
// 	}

	/*-----------------------------------------------------------------------*/

	CWndVedio WndDlg;
	int ret = WndDlg.DoModal();

	g_hVedioHwnd = NULL;
	if (!g_pWhiteBoardView)
	{
		return ;
	}
 	if (ret == 100)
 	{
 		m_GetCurWndHwnd = WndDlg.GetWndHwnd();
 		if ( NULL == m_GetCurWndHwnd)
 		{
 			CString strNotRecWin;
 			strNotRecWin = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("NotRecWin"));
 			if (strNotRecWin.IsEmpty())
 			{
 				strNotRecWin = _T("�벻Ҫ¼�Ƶ�ǰ���ڣ�");
 			}
 			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
 			{
 				if (::IsWindow(m_csVedioStop.m_hWnd))
 				{
 					m_csVedioStop.SetCaption(strNotRecWin);
 					m_csVedioStop.SetWindowTextBool(false);
 				}
 
 				//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNotRecWin, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strNotRecWin);
 			}
 			else
 			{
 				MessageBox(strNotRecWin);
 			}
 			return;
 		}
 	}
 	else if (ret == IDOK)
	{
		return;
	}
	else if (ret == IDCANCEL)
	{
		CString strSelectRecFun;
		strSelectRecFun = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("SelectRecFun"));
		if (strSelectRecFun.IsEmpty())
		{
			strSelectRecFun = _T("��ѡ��������Ҫ��¼���ܣ�");
		}
		m_csVedioStop.SetCaption(strSelectRecFun);
		m_csVedioStop.SetWindowTextBool(false);
		return;
	}
	if (!::IsWindow(m_csVedioStop.m_hWnd))
	{
		return;
	}

	if (g_bSupportU7OpenFile)
	{
		//wchar_t szFilter[MAX_PATH];
		CString strFilter;
		strFilter = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("Filter"));
		if (strFilter.IsEmpty())
		{
			strFilter = _T("��Ƶ (*.avi)|*.avi||");
		}
		CString strfilterName;
		strfilterName = g_strAvconUserID;
		SYSTEMTIME sys; 
		GetLocalTime( &sys );
		CString strTime;
		strTime.Format(_T("%d%d%d%d%d"),sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute);
		strfilterName += strTime;
		//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);
		char* sFilePath = new char[MAX_PATH];
		memset(sFilePath,0,MAX_PATH);
		bool bOpen = g_pHPWBDataNotify->OpenSaveFile(FALSE, sFilePath, NULL,strFilter, this->GetSafeHwnd(),strfilterName);


		str = A2U(sFilePath);
		//str = sFilePath;
		if (str == _T(""))
		{
			delete[] sFilePath;
			return;
		}
		else
		{
			//��Ҫ��ʱ��OPenSave��������·�������룬���������ж�wangxiaobin
			BOOL bOK = IsFilePathOk(str);
			if (bOK == FALSE)
			{
				if (!m_bPathisLoad)
				{
					CString strEnterFileName = _T("");
					strEnterFileName = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SaveFilePathNull"));
					if (strEnterFileName.IsEmpty())
					{
						strEnterFileName = _T("¼���ļ������λ�ó�����ǰ·���޷������ļ�");
					}
					//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strEnterFileName, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strEnterFileName);
				}

				delete[] sFilePath;
				return;
			}
		}

		char FileName[MAX_PATH] = {0};
		char Name[MAX_PATH] = {0};
		GetAbsolute_path(sFilePath,Name);
		GetCurFileName(Name,FileName);
		m_strFileName = A2U(FileName);
		strDeleteFile = str;

		if (g_pWhiteBoardView == NULL)
		{
			delete[] sFilePath;
			return;
		}
		if (bOpen)
		{			
			char FileName[MAX_PATH] = {0};
			GetCurFileName(sFilePath,FileName,'\\');
			CString strchange ;
			strchange = A2U(FileName);
			m_FilePath.SetWindowText(strchange);
			delete[] sFilePath;
		}
		else
		{
			delete[] sFilePath;
			return;
		}
		UpdateData(TRUE);
		if (!IsFileNameValidU7(m_strFileName))
		{
			CString strSetFileName;
			strSetFileName = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("SetFileName"));
			if (strSetFileName.IsEmpty())
			{
				strSetFileName = _T("�ļ������ԣ�¼����ֹ���ٴ������ļ���");
			}
			m_csVedioStop.SetCaption(strSetFileName);
			m_csVedioStop.SetWindowTextBool(false);
			Invalidate();
			return;
		}

	}
	if (m_strFilePath.IsEmpty())
	{
		CString strPathNotNull;
		strPathNotNull = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("PathNotNull"));
		if (strPathNotNull.IsEmpty())
		{
			strPathNotNull = _T("�ļ�·������Ϊ��");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strPathNotNull, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strPathNotNull);
		}
		else
		{
			MessageBox(strPathNotNull);
		}
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
			SWP_NOMOVE | SWP_NOZORDER);
		m_nSize = 0;
		Invalidate();
		return;
	}
	CString strWindRecroding;
	strWindRecroding = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("WindRecroding"));
	if (strWindRecroding.IsEmpty())
	{
		strWindRecroding = _T("���ڽ��д���¼��...��Ҫ��ֹ���뵥����ֹͣ����ť��");
	}
	m_csVedioStop.SetCaption(strWindRecroding);
	m_csVedioStop.SetWindowTextBool(false);

	if (IsFileExisted() && PathFileExists(strDeleteFile))
	{
		::DeleteFile(strDeleteFile);
	}
	/*-----------------------------------------------------------------------*/

	GetDlgItem(IDC_BUT_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT_PAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTT_FULL_SCREEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_WND_SCREEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_AREA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_SET_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RATE)->EnableWindow(FALSE);
	/*-----------------------------------------------------------------------*/

	typedef void (*lpStartVedio)(CString strFilePath,CString strFileName,int nRate,CPoint p1,CPoint p2,int nFlag,HWND GetCurHwnd);
	lpStartVedio pStartVedio = (lpStartVedio)GetProcAddress(m_hDLL,"StartVedio");
	if (NULL == pStartVedio)
	{
		CString strAddFail;
		strAddFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("AddFail"));
		if (strAddFail.IsEmpty())
		{
			strAddFail = _T("��pStopVedioѰַʧ�ܡ�");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}

			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strAddFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strAddFail);
		}
		else
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			MessageBox(strAddFail);
		}
		FreeLibrary(m_hDLL); m_hDLL=NULL;
		return ;
	}
	pStartVedio(m_strFilePath,m_strFileName,m_nRate,0,0,WNDSCREEN,m_GetCurWndHwnd);
	m_IsStop = TRUE;
	m_strCurrentFileName = str;
	/*-----------------------------------------------------------------------*/
	KillTimer(CHECKMEMORYSTATUSISFULL);
	SetTimer(CHECKMEMORYSTATUSISFULL,180000,NULL);
}


void CMedioVideoScreenDlg::OnBnClickedButStop()
{
	CString strRecordEnd;
	strRecordEnd = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("RecordEnd"));
	if (strRecordEnd.IsEmpty())
	{
		strRecordEnd = _T("¼������ֹ��");
	}
	m_csVedioStop.SetCaption(strRecordEnd);
	m_csVedioStop.SetWindowTextBool(false);
	/*-----------------------------------------------------------------------*/
	GetDlgItem(IDC_BUT_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_PAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTT_FULL_SCREEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT_WND_SCREEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT_AREA)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT_SET_PATH)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_RATE)->EnableWindow(TRUE);
	/*-----------------------------------------------------------------------*/

	StopThread();
	
	m_FileName.GetWindowText(m_strFileName);
	if (g_bSupportU7OpenFile)
	{
		m_strFileName = _T("");
	}
	else
	{
		GetFileName(m_strFileName);
	}
	
	m_FileName.SetWindowText(m_strFileName);
	m_FilePath.GetWindowText(m_strFilePath);

	
	/*-----------------------------------------------------------------------*/
	if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
	{
		//m_strPlace = pGetDiskPlace(g_bU7FileTempDir);
		m_strPlace = _T("");
	}
	else
	{
		typedef CString(*lpGetDiskPlace)(CString strPath);
		lpGetDiskPlace pGetDiskPlace = (lpGetDiskPlace)GetProcAddress(m_hDLL,"GetResidualPlace");
		if (NULL == pGetDiskPlace)
		{
			CString strFunAddFail;
			strFunAddFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FunAddFail"));
			if (strFunAddFail.IsEmpty())
			{
				strFunAddFail = _T("����Ѱַʧ��");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}

				//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, _T("��pGetDiskPlace��")+strFunAddFail, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(_T("��pGetDiskPlace��")+strFunAddFail);
			}
			else
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}
				MessageBox(_T("��pGetDiskPlace��")+strFunAddFail);
			}
			FreeLibrary(m_hDLL); m_hDLL=NULL;
			return;
		}
		m_strPlace = pGetDiskPlace(m_strFilePath);


		CString strDiskSpace;
		strDiskSpace = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("DiskSpace"));
		if (strDiskSpace.IsEmpty())
		{
			strDiskSpace = _T("����ʣ��ռ�");
		}
		m_csPlace.SetCaption(strDiskSpace+m_strPlace);
	}
		
	CString strSuccess;
			
	if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
	{
		CString strCloseFileSuccess;
		strCloseFileSuccess = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("CloseFileSuccess"));
		if (strCloseFileSuccess.IsEmpty())
		{
			strCloseFileSuccess = _T("�ر��ļ��ɹ����ļ��ѱ��浽ѡ��·����");
		}
		strSuccess.Format(strCloseFileSuccess);
	}
	else
	{
		CString strSaveTo;
		strSaveTo = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("SaveTo"));
		if (strSaveTo.IsEmpty())
		{
			strSaveTo = _T("�ر��ļ��ɹ����ļ��ѱ��浽");
		}
		strSuccess.Format(strSaveTo+m_strFilePath);
	}
 	
 	

	m_csVedioStop.SetCaption(strSuccess);	
	m_csVedioStop.SetWindowTextBool(false);
	CFont font; 
	font.CreatePointFont(50,_T("Arial"));
	//m_csVedioStop.SetFont(&font);
	GetDlgItem(IDC_STATICVEDIOSTOP)->SetFont(&font);

	
	++m_nCount ;
	m_strCurrentFileName = _T("");
	font.DeleteObject();
}


void CMedioVideoScreenDlg::OnBnClickedButPause()
{
	UpdateData(TRUE);
	/*-----------------------------------------------------------------------*/

	if (TRUE == m_IsPause)
	{
		/*-----------------------------------------------------------------------*/

		GetDlgItem(IDC_BUT_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUT_PAUSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTT_FULL_SCREEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_WND_SCREEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_AREA)->EnableWindow(FALSE);
		CString strContinue;
		strContinue = ReadConfigText(_T("WhiteBoardControlText"), _T("ContinueScleenRecordBtn"));
		if (strContinue.IsEmpty())
		{
			strContinue = _T("����");
		}
		m_bPauseScreen.SetName(strContinue);
		m_bPauseScreen.SetWindowText(strContinue);
		m_IsPause = FALSE;

		/*-----------------------------------------------------------------------*/

		typedef void (*lpPauseVedio)();
		lpPauseVedio pPauseVedio = (lpPauseVedio)GetProcAddress(m_hDLL,"PauseVedio");
		if (NULL == pPauseVedio)
		{
			CString strAddPauseFail;
			strAddPauseFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("AddPauseFail"));
			if (strAddPauseFail.IsEmpty())
			{
				strAddPauseFail = _T("��pPauseVedioѰַʧ�ܡ�");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}

				//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt,strAddPauseFail, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strAddPauseFail);
			}
			else
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}
				MessageBox(strAddPauseFail);
			}
			FreeLibrary(m_hDLL); m_hDLL=NULL;
			return ;
		}

		/*-----------------------------------------------------------------------*/

		pPauseVedio();
		m_bPauseScreen.Invalidate();
	} 
	else
	{
		/*-----------------------------------------------------------------------*/

		GetDlgItem(IDC_BUT_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUT_PAUSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTT_FULL_SCREEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_WND_SCREEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_AREA)->EnableWindow(FALSE);
		m_IsPause = TRUE;
		CString BtnText;
		BtnText = ReadConfigText(_T("WhiteBoardControlText"),_T("PauseScleenRecordBtn"));
		if (BtnText.IsEmpty())
		{
			BtnText = _T("��ͣ");
		}
		m_bPauseScreen.SetName(BtnText);
		m_bPauseScreen.SetWindowText(BtnText);
		/*-----------------------------------------------------------------------*/

		typedef void (*lpUnPauseVedio)();
		lpUnPauseVedio pUnPauseVedio = (lpUnPauseVedio)GetProcAddress(m_hDLL,"UnPauseVedio");
		if (NULL == pUnPauseVedio)
		{
			CString strFunAddError;
			strFunAddError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FunAddError"));
			if (strFunAddError.IsEmpty())
			{
				strFunAddError = _T("����Ѱַ����");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{

				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}

				//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, _T("��pUnPauseVedio��")+strFunAddError, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(_T("��pUnPauseVedio��")+strFunAddError);
			}
			else
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}
				MessageBox(_T("��pUnPauseVedio��")+strFunAddError);
			}
			FreeLibrary(m_hDLL); m_hDLL=NULL;
			return ;
		}

		/*-----------------------------------------------------------------------*/

		pUnPauseVedio();
		m_bPauseScreen.Invalidate();
	}
}

//�����ť
void CMedioVideoScreenDlg::OnBnClickedButSetPath()
{
	/*��ѡ���ļ��жԻ���ѡ���ļ�����·��------------------------------------------*/
	//����1
	CString strFilePath;
	if (g_bSupportU7OpenFile)
	{
		char* sFilePath = new char[MAX_PATH];
		bool bOpen = g_pHPWBDataNotify->OpenSaveFile(TRUE, sFilePath, NULL, NULL, this->GetSafeHwnd());
		if (g_pWhiteBoardView == NULL)
		{
			return;
		}
		if (bOpen)
		{
			strFilePath = A2U(sFilePath);
			//strFilePath = sFilePath;
		}
		delete[] sFilePath;
	} 
	else
	{
		TCHAR  szPath[MAX_PATH];                                    //����·������
		BROWSEINFO bwInfo;                                          //����·�������ṹ����Ϣ
		//bwInfo.hwndOwner              = NULL;
		bwInfo.hwndOwner              = this->GetSafeHwnd();
		bwInfo.pidlRoot               = NULL;
		CString strFileSavePath;
		strFileSavePath = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("FileSavePath"));
		if (strFileSavePath.IsEmpty())
		{
			strFileSavePath = _T("�ļ�����·��");
		}
		bwInfo.lpszTitle              = strFileSavePath;         //�Ի������
		bwInfo.pszDisplayName         = szPath;                     //ѡ���ļ���·��
		bwInfo.ulFlags                = BIF_RETURNONLYFSDIRS;       //���
		bwInfo.lpfn                   = NULL;                       //Ĭ��ֵΪNULL
		bwInfo.lParam                 = NULL;                       //�ص���Ϣ      
		LPITEMIDLIST  lpItemIDList = SHBrowseForFolder(&bwInfo);    //��ʾ�ļ�����Ի���
		if (lpItemIDList)
		{
			if (SHGetPathFromIDList(lpItemIDList,szPath))        //�ж��Ƿ����ļ���·��
			{
				strFilePath = szPath;                   	    //����ļ���·��
			}
		}
		else
		{
			strFilePath = "";                                    //�ļ���·��Ϊ��
		}
		IMalloc* imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(lpItemIDList);
			imalloc->Release();
		}
	}
		
	/*
	//����2
	CString strFilePath;
	CString tmpFilePath(m_strFilePath);
	TCHAR  szPath[MAX_PATH];                                    //����·������
	BROWSEINFO bwInfo;                                          //����·�������ṹ����Ϣ
	//bwInfo.hwndOwner              = NULL;
	bwInfo.hwndOwner              = this->GetSafeHwnd();
	bwInfo.pidlRoot               = NULL;
	bwInfo.lpszTitle              = _T("��ѡ���ļ�����·��");         //�Ի������
	bwInfo.pszDisplayName         = szPath;                     //ѡ���ļ���·��
	bwInfo.ulFlags                = BIF_EDITBOX | BIF_NEWDIALOGSTYLE;       //���
	CFileFind finder;
	if (finder.FindFile(tmpFilePath) == FALSE)
	{
		bwInfo.lParam = 0;
		bwInfo.lpfn = NULL;
	}
	else
	{
		bwInfo.lParam = (long)(tmpFilePath.GetBuffer(tmpFilePath.GetLength()));
		bwInfo.lpfn = BrowseCallBackProc;
	}
	finder.Close();
	bwInfo.iImage = NULL;

	LPITEMIDLIST  lpItemIDList = SHBrowseForFolder(&bwInfo);    //��ʾ�ļ�����Ի���
	if (lpItemIDList)
	{
		if (SHGetPathFromIDList(lpItemIDList,szPath))        //�ж��Ƿ����ļ���·��
		{
			strFilePath = szPath;                   	    //����ļ���·��
		}
	}else
	{
		strFilePath = "";                                    //�ļ���·��Ϊ��
	}
	IMalloc* imalloc = 0;
	if (SUCCEEDED(SHGetMalloc(&imalloc)))
	{
		imalloc->Free(lpItemIDList);
		imalloc->Release();
	}*/
	/*
	//����3
	CString strFilePath;
	LPMALLOC g_pMalloc;
	if (::SHGetMalloc(&g_pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		LPSTR lpBuffer;
		LPITEMIDLIST pidlSpecialFolder;
		LPITEMIDLIST pidlBrowse;
		if ((lpBuffer = (LPSTR)g_pMalloc->Alloc(MAX_PATH)) != NULL)
		{
			if (SUCCEEDED(SHGetSpecialFolderLocation(this->GetSafeHwnd(),CSIDL_DRIVES, &pidlSpecialFolder)))
			{
				bi.hwndOwner = this->GetSafeHwnd();
				bi.pidlRoot = pidlSpecialFolder;
				bi.pszDisplayName = lpBuffer;
				bi.lpszTitle = _T("��ѡ�񱣴��ļ���");
				bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
				bi.lParam = 0;
				bi.lpfn = NULL;
				bi.iImage = NULL;

				pidlBrowse = SHBrowseForFolder(&bi);
				if (pidlBrowse != NULL)
				{
					if (SHGetPathFromIDList(pidlBrowse,lpBuffer))
					{
						strFilePath = lpBuffer;
					}
					g_pMalloc->Free(pidlBrowse);
				}
				g_pMalloc->Free(pidlSpecialFolder);
			}
			g_pMalloc->Free(lpBuffer);
		}
		g_pMalloc->Release();
	}
	*/

	if (!strFilePath.IsEmpty())
	{
		m_strFilePath = strFilePath;
		m_FilePath.SetWindowText(m_strFilePath);

		//��·�����浽�����ļ�
		//CString configFilePath = GetCurrentPath() + _T("\\WBConfig.ini");
		//CString strSection = _T("ArgumentSave");
		//WritePrivateProfileString(strSection, _T("VedioScreenFilePath"), strFilePath, configFilePath);
		//WriteControlText(_T("VedioScreenFilePath"), strFilePath);
		CString strSelection = _T("WhiteBoardControlText");
		CString strPosition = _T("VedioScreenFilePath");
		WriteWBConfigPathINI(strSelection,strPosition,strFilePath);
		//��ȡ�û�ѡ��·�����ڵĴ��̵�·��
		typedef CString (*lpGetDiskPlace)(CString strPath);
		lpGetDiskPlace pGetDiskPlace = (lpGetDiskPlace)GetProcAddress(m_hDLL,"GetResidualPlace");
		if (NULL == pGetDiskPlace)
		{
			CString strFunAddError;
			strFunAddError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FunAddError"));
			if (strFunAddError.IsEmpty())
			{
				strFunAddError = _T("����Ѱַ����");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}
				//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, _T("��pGetDiskPlace��")+strFunAddError, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(_T("��pGetDiskPlace��")+strFunAddError);
			}
			else
			{
				if (g_pWhiteBoardView)
				{
					g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
				}
				MessageBox(_T("��pGetDiskPlace��")+strFunAddError);
			}
			//MessageBox(_T("��pGetDiskPlace������Ѱַʧ��"));
			FreeLibrary(m_hDLL); m_hDLL=NULL;
			return;
		}
		m_strPlace = pGetDiskPlace(m_strFilePath);
		CString strDiskSpace;
		strDiskSpace = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("DiskSpace"));
		if (strDiskSpace.IsEmpty())
		{
			strDiskSpace = _T("����ʣ��ռ�");
		}
		m_csPlace.SetCaption(strDiskSpace+m_strPlace);
		m_csVedioStop.SetCaption(_T(""));
		m_csVedioStop.SetWindowTextBool(false);
		++m_nCount ;
	}
}

void CMedioVideoScreenDlg::OnFullCancel()
{
	if (TRUE == m_IsStop)
	{
		OnBnClickedButStop();

	}
	if (NULL != m_hDLL)
	{
		FreeLibrary(m_hDLL);
		m_hDLL = NULL;
	}
	m_bIsWndowClosing = TRUE;
	//CDialogEx::OnCancel();
	g_pWhiteBoardView->AddRECScreen();
}
void CMedioVideoScreenDlg::OnCancel()
{	
	if (TRUE == m_IsStop)
	{
		CString strScreenRecNotClose;
		strScreenRecNotClose = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScreenRecNotClose"));
		if (strScreenRecNotClose.IsEmpty())
		{
			strScreenRecNotClose = _T("��Ļ¼��û�йرգ�");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strScreenRecNotClose, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strScreenRecNotClose);
		}
		else
		{
			MessageBox(strScreenRecNotClose,m_strPrompt,MB_OKCANCEL|MB_ICONASTERISK);
		}
		return;
	}
	if (NULL != m_hDLL)
	{
		FreeLibrary(m_hDLL);
		m_hDLL = NULL;
	}
	m_bIsWndowClosing = TRUE;
	//CDialogEx::OnCancel();
	g_pWhiteBoardView->AddRECScreen();
}

void CMedioVideoScreenDlg::OnBnClickedButProperty()
{
	static CRect rectLarge;
	static CRect rectSmall;
	if(rectLarge.IsRectNull())
	{
		CRect rectSeparator;
		GetWindowRect(&rectLarge);
		GetDlgItem(IDC_STATIC_BOUNDARY)->GetWindowRect(&rectSeparator);

		rectSmall.left=rectLarge.left;
		rectSmall.top=rectLarge.top;
		rectSmall.right=rectLarge.right;
		rectSmall.bottom=rectSeparator.bottom;

		
		if (g_bSupportU7OpenFile)
		{
			m_rectLarge = rectSmall;
		}
		else
		{
			m_rectLarge = rectLarge;
		}
	}

	if(0 == m_nSize)
	{
		SetWindowPos(NULL,0,0,rectSmall.Width(),rectSmall.Height(),
			SWP_NOMOVE | SWP_NOZORDER);
		m_nSize = 1;
	}
	else
	{
		SetWindowPos(NULL,0,0,rectLarge.Width(),rectLarge.Height(),
			SWP_NOMOVE | SWP_NOZORDER);
		m_nSize = 0;
	}
	Invalidate();
}

void CMedioVideoScreenDlg::SetSkinImage(CString strPath)
{
	if (!m_pBitmap)
	{
		//wchar_t strFilename[MAX_PATH];
		CString picturePath = strPath;
		//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);//��CString����ת����WCHAR
		m_pBitmap = new Bitmap(picturePath);
	}
	else
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
		//wchar_t strFilename[MAX_PATH];
		CString picturePath = strPath;
		//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
		m_pBitmap = new Bitmap(picturePath);
	}

	if (m_pBitmap)
	{
		CRect rect;
		GetClientRect(&rect);
		CClientDC dc(this);
		Graphics graph(dc.GetSafeHdc());
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pBitmap, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);	
	}
}

BOOL CMedioVideoScreenDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString strPath = _T("");
	strPath =  GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\");
	SetSkinImage(strPath + _T("ScreenRecord.png"));
	return TRUE;
//	return CDialogEx::OnEraseBkgnd(pDC);
}

void CMedioVideoScreenDlg::StopThread()
{
	m_IsStop = FALSE;
	typedef void (*lpStopVedio)();
	lpStopVedio pStopVedio = (lpStopVedio)GetProcAddress(m_hDLL,"StopVedio");
	if (NULL == pStopVedio)
	{
		CString strAddFail;
		strAddFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("AddFail"));
		if (strAddFail.IsEmpty())
		{
			strAddFail = _T("��pStopVedioѰַʧ�ܡ�");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}

			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strAddFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strAddFail);

		}
		else
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			MessageBox(strAddFail);
		}
		FreeLibrary(m_hDLL); 
		m_hDLL=NULL;
		return ;
	}
	pStopVedio();
}

void CMedioVideoScreenDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialogEx::OnOK();
}

// ����ļ����Ƿ�Ϸ�
BOOL CMedioVideoScreenDlg::IsFileNameValid(void)
{
	//UpdateData();
	CEdit *pEditFile = NULL;
	CString strFileName = _T("");
	m_FileName.GetWindowText(strFileName);
	m_strFileName = strFileName.Trim();
	pEditFile = ((CEdit*)GetDlgItem(IDC_EDIT_FILE));
	//ȥ��ͷβ�Ŀո�
	//m_strFileName = m_strFileName.Trim();
	CString strNameNot;
	CString strReEnter;
	strNameNot = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("NameNot"));
	if (strNameNot.IsEmpty())
	{
		strNameNot = _T("�ļ�������");
	}
	strReEnter = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ReEnter"));
	if (strReEnter.IsEmpty())
	{
		strReEnter = _T("���������������룡");
	}
	CString strNameExclude;
	strNameExclude = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("NameExclude"));
	if (strNameExclude.IsEmpty())
	{
		strNameExclude = _T("�ļ����в��ܰ���");
	}
	if (m_strFileName.IsEmpty())
	{
		CString strFileNameNotNull;
		strFileNameNotNull = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FileNameNotNull"));
		if (strFileNameNotNull.IsEmpty())
		{
			strFileNameNotNull = _T("�ļ�������Ϊ�գ�");
		}
		CString strEnterFileName;
		strEnterFileName = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("EnterFileName"));
		if (strEnterFileName.IsEmpty())
		{
			strEnterFileName = _T("�ļ���Ϊ�գ����ܼ��������������ļ���");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strEnterFileName, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strEnterFileName);
		}
		else
		{
			MessageBox(strFileNameNotNull);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView )
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (m_strFileName.GetLength() > 32)
	{
		CString strMax32;
		strMax32 = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Max32"));
		if (strMax32.IsEmpty())
		{
			strMax32 = _T("�ļ����ֻ������32λ��");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt,strMax32, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strMax32);
		}
		else
		{
			MessageBox(strMax32);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	//�������ַ�������Ч���ж�
	else if (_T(".") == m_strFileName || _T("..") == m_strFileName)
	{
		CString strReEnterName;
		strReEnterName = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ReEnterName"));
		if (strReEnterName.IsEmpty())
		{
			strReEnterName = _T("�ļ������ԡ�.����..�����������������룡");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strReEnterName, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strReEnterName);
		}
		else
		{
			MessageBox(strReEnterName);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (m_strFileName.ReverseFind(_T('.')) == (m_strFileName.GetLength()-1))
	{
		CString strLastName;
		strLastName = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("LastName"));
		if (strLastName.IsEmpty())
		{
			strLastName = _T("�ļ��������һ���ַ�����Ϊ��.����");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strLastName, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strLastName);
		}
		else
		{
			MessageBox(strLastName);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if ( _T("nul") == m_strFileName)
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameNot+_T("nul")+strReEnter, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameNot+_T("nul")+strReEnter);
		}
		else
		{
			MessageBox(strNameNot+_T("nul")+strReEnter);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if ( _T("aux") == m_strFileName)
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameNot+_T("aux")+strReEnter, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameNot+_T("aux")+strReEnter);
		}
		else
		{
			MessageBox(strNameNot+_T("aux")+strReEnter);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if ( _T("con") == m_strFileName)
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameNot+_T("con")+strReEnter, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameNot+_T("con")+strReEnter);
		}
		else
		{
			MessageBox(strNameNot+_T("con")+strReEnter);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if ( _T("com1") == m_strFileName)
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			g_pHPWBDataNotify->ShowTipWindow(strNameNot+_T("com1")+strReEnter);
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameNot+_T("com1")+strReEnter, MB_OK,true);
		}
		else
		{
			MessageBox(strNameNot+_T("com1")+strReEnter);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if ( _T("lpt1") == m_strFileName)
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt,strNameNot+_T("lpt1")+strReEnter, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameNot+_T("lpt1")+strReEnter);
		}
		else
		{
			MessageBox(strNameNot+_T("lpt1")+strReEnter);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T(".."),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameNot+_T("..")+strReEnter, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameNot+_T("..")+strReEnter);
		}
		else
		{
			MessageBox(strNameNot+_T("..")+strReEnter);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("#"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��#����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��#����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��#����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("%"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��%����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��%����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��%����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("&"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��&����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow( strNameExclude+_T("��&����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��&����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("*"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt,strNameExclude+_T("��*����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��*����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��*����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("|"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��|����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��|����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��|����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("\\"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��\\����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��\\����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��\\����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T(":"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��:����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��:����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��:����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("\""),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��\����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��\����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��\����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("<"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��<����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��<����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��<����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T(">"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��>����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��>����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��>����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("?"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��?����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��?����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��?����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	else if (-1 != m_strFileName.Find(_T("/"),0))
	{
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameExclude+_T("��/����"), MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNameExclude+_T("��/����"));
		}
		else
		{
			MessageBox(strNameExclude+_T("��/����"));
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (pEditFile != NULL && IsWindow(pEditFile->GetSafeHwnd()))
		{
			pEditFile->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		return FALSE;
	}
	return TRUE;
}
BOOL CMedioVideoScreenDlg::IsFileNameValidU7(CString strName)
{
 	CString strFileName;
 	strFileName = strName;
 	m_strFileName = strFileName.Trim();

	//ȥ��ͷβ�Ŀո�
	//m_strFileName = m_strFileName.Trim();
	if (m_strFileName.IsEmpty())
	{
		CString strEnterFileName;
		strEnterFileName = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("EnterFileName"));
		if (strEnterFileName.IsEmpty())
		{
			strEnterFileName = _T("�ļ���Ϊ�գ����ܼ��������������ļ���");
		}
		//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strEnterFileName, MB_OK,true);
		g_pHPWBDataNotify->ShowTipWindow(strEnterFileName);
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		return FALSE;
	}
	else if (m_strFileName.GetLength() > 32)
	{
		CString strMax32;
		strMax32 = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Max32"));
		if (strMax32.IsEmpty())
		{
			strMax32 = _T("�ļ����ֻ������32λ��");
		}
		//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strMax32, MB_OK,true);
		g_pHPWBDataNotify->ShowTipWindow(strMax32);
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
	
		return FALSE;
	}
	else if (_T(".") == m_strFileName || _T("..") == m_strFileName || m_strFileName.ReverseFind(_T('.')) == (m_strFileName.GetLength()-1) ||
		_T("nul") == m_strFileName || _T("aux") == m_strFileName || _T("con") == m_strFileName ||  _T("com1") == m_strFileName ||
		_T("lpt1") == m_strFileName || -1 != m_strFileName.Find(_T(".."),0) || -1 != m_strFileName.Find(_T("#"),0) ||
		-1 != m_strFileName.Find(_T("%"),0) || -1 != m_strFileName.Find(_T("&"),0) || -1 != m_strFileName.Find(_T("*"),0) ||
		-1 != m_strFileName.Find(_T("|"),0) || -1 != m_strFileName.Find(_T("\\"),0) || -1 != m_strFileName.Find(_T(":"),0) ||
		-1 != m_strFileName.Find(_T("\""),0) || -1 != m_strFileName.Find(_T("<"),0) || -1 != m_strFileName.Find(_T(">"),0) || 
		-1 != m_strFileName.Find(_T("?"),0) || -1 != m_strFileName.Find(_T("/"),0) )
	{
		CString strNameNot;
		CString strReEnter;
		strNameNot = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("NameNot"));
		if (strNameNot.IsEmpty())
		{
			strNameNot = _T("�ļ�������");
		}
		strReEnter = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ReEnter"));
		if (strReEnter.IsEmpty())
		{
			strReEnter = _T("���������������룡");
		}
		//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strNameNot+_T("��.����..����nul����aux����con����coml����lpt1����#����%����&����*����|����\\����:����\����<����>����?����/����")+strReEnter, MB_OK,true);
		g_pHPWBDataNotify->ShowTipWindow(strNameNot+_T("��.����..����nul����aux����con����coml����lpt1����#����%����&����*����|����\\����:����\����<����>����?����/����")+strReEnter);
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}

		return FALSE;

	}
	//�������ַ�������Ч���ж�
// 	else if (_T(".") == m_strFileName || _T("..") == m_strFileName)
// 	{
// 		g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ������ԡ�.����..������������������!"), MB_OK,true);
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		return FALSE;
// 	}
// 	else if (m_strFileName.ReverseFind(_T('.')) == (m_strFileName.GetLength()-1))
// 	{
// 	
// 		g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ��������һ���ַ�����Ϊ��.��!"), MB_OK,true);
// 
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 	
// 		return FALSE;
// 	}
// 	else if ( _T("nul") == m_strFileName)
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ������ԡ�nul������������������!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ������ԡ�nul������������������!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if ( _T("aux") == m_strFileName)
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ������ԡ�aux������������������!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ������ԡ�aux������������������!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if ( _T("con") == m_strFileName)
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ������ԡ�con������������������!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ������ԡ�con������������������!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if ( _T("com1") == m_strFileName)
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ������ԡ�com1������������������!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ������ԡ�com1������������������!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if ( _T("lpt1") == m_strFileName)
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ������ԡ�lpt1������������������!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ������ԡ�lpt1������������������!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T(".."),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ������ԡ�..������������������!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ������ԡ�..������������������!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("#"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����#��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����#��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("%"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����%��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����%��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("&"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����&��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����&��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("*"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����*��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����*��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("|"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����|��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����|��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("\\"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����\\��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����\\��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T(":"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����:��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����:��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("\""),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����\��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����\��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("<"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����<��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����<��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T(">"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����>��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����>��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("?"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����?��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����?��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
// 	else if (-1 != m_strFileName.Find(_T("/"),0))
// 	{
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("��ʾ"), _T("�ļ����в��ܰ�����/��!"), MB_OK,true);
// 		}
// 		else
// 		{
// 			MessageBox(_T("�ļ����в��ܰ�����/��!"));
// 		}
// 		//����װ��Ѿ��رգ���ôֱ�ӷ���
// 		if (NULL == g_pWhiteBoardView)
// 		{
// 			return TRUE;
// 		}
// 		((CEdit*)GetDlgItem(IDC_EDIT_FILE))->SetFocus();
// 		return FALSE;
// 	}
	return TRUE;
}

// ��������֡���Ƿ�Ϸ�
BOOL CMedioVideoScreenDlg::IsRateValid(void)
{
	CString strRate;
	CEdit *prate = NULL;
	prate = (CEdit*)GetDlgItem(IDC_EDIT_RATE);
	m_Rate.GetWindowText(strRate);
	strRate = strRate.Trim();
	int i = strRate.GetLength();
	CString strFpsNeedNum;
	strFpsNeedNum = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FpsNeedNum"));
	if (strFpsNeedNum.IsEmpty())
	{
		strFpsNeedNum = _T("֡�������������֣�1-10����");
	}
	if (0 == i)
	{		
		CString strFpsNotNull;
		strFpsNotNull = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FpsNotNull"));
		if (strFpsNotNull.IsEmpty())
		{
			strFpsNotNull = _T("֡�ʲ���Ϊ�գ�");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strFpsNotNull, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFpsNotNull);
		}
		else
		{
			MessageBox(strFpsNotNull);
		}
		//����װ��Ѿ��رգ���ôֱ�ӷ���
		if (NULL == g_pWhiteBoardView)
		{
			return TRUE;
		}
		if (prate != NULL && IsWindow(prate->GetSafeHwnd()))
		{
			prate->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_RATE))->SetFocus();
		return FALSE;
	}
	for(int j=0; j<i; ++j)
	{
		TCHAR ch = strRate.GetAt(j);
		if (ch < '0' || ch >'9')
		{
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strFpsNeedNum, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strFpsNeedNum);
			}
			else
			{
				MessageBox(strFpsNeedNum);
			}
			if (NULL == g_pWhiteBoardView)
			{
				return TRUE;
			}
			
			if (prate != NULL && IsWindow(prate->GetSafeHwnd()))
			{
				prate->SetFocus();
			}
			//((CEdit*)GetDlgItem(IDC_EDIT_RATE))->SetFocus();
			return FALSE;
		}
	}
	try
	{
		int nTmp = atoi((char*)strRate.GetBuffer(strRate.GetLength()));
		if (nTmp<=0 || nTmp>10)
		{
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strFpsNeedNum, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strFpsNeedNum);
			}
			else
			{
				MessageBox(strFpsNeedNum);
			}
			if (NULL == g_pWhiteBoardView)
			{
				return TRUE;
			}
			if (prate != NULL && IsWindow(prate->GetSafeHwnd()))
			{
				prate->SetFocus();
			}
			//((CEdit*)GetDlgItem(IDC_EDIT_RATE))->SetFocus();
			return FALSE;
		}
		m_nRate = nTmp;
		strRate.ReleaseBuffer();
	}
	catch (CException* e)
	{
		OutputDebugString(_T("����ת������"));
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strFpsNeedNum, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFpsNeedNum);
		}
		else
		{
			MessageBox(strFpsNeedNum);
		}
		if (prate != NULL && IsWindow(prate->GetSafeHwnd()))
		{
			prate->SetFocus();
		}
		//((CEdit*)GetDlgItem(IDC_EDIT_RATE))->SetFocus();
		return FALSE;
	}
	return TRUE;
}

BOOL CMedioVideoScreenDlg::IsFileExisted(void)
{
	CString filePath;
	if (m_strFilePath.GetAt(m_strFilePath.GetLength()-1) == '\\')
	{
		filePath = m_strFilePath + m_strFileName + _T(".avi");
	}
	else
	{
		filePath = m_strFilePath + _T("\\") + m_strFileName + _T(".avi");
	}
	CFileFind fileFind;
	BOOL bFileExisted = fileFind.FindFile(filePath);
	if (bFileExisted)
	{
		return TRUE;
	}
	return FALSE;
}

void CMedioVideoScreenDlg::OnKillfocusEditFile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�жϴ����Ƿ��йر�

	if (TRUE == m_bIsWndowClosing || !IsWindow(m_FileName.GetSafeHwnd()))
	{
		return;
	}

// 	if (!IsFileNameValid())
// 	{
// 		SetWindowPos(NULL,0,0,m_rectLarge.Width(),m_rectLarge.Height(),
// 			SWP_NOMOVE | SWP_NOZORDER);
// 		m_nSize = 0;
// 		Invalidate();
// 		return;
// 	}
	if (!g_bSupportU7OpenFile)
	{
		if (!IsFileNameValid())
		{
			if (!IsWindow(m_FileName.GetSafeHwnd()))
			{
				return;
			}
			CString str = _T("");
			GetFileName(str);
			m_FileName.SetWindowText(str);
			m_FileName.UpdateData(FALSE);			

		}
		CString strFileName = _T("");
		m_FileName.GetWindowText(strFileName);
		m_strFileName = strFileName;	
	}
		
}


void CMedioVideoScreenDlg::OnKillfocusEditRate()
{
	if (TRUE == m_bIsWndowClosing || !IsWindow(m_Rate.GetSafeHwnd()))
	{
		return;
	}
    BOOL bRateValid = IsRateValid();

	//����װ��Ѿ�-�ر�,��ôֱ�ӷ���
	if (NULL == g_pWhiteBoardView || !IsWindow(m_Rate.GetSafeHwnd()))
	{
		return ;
	}
	if (!bRateValid)
	{
		char pTenm[10] ={0};
		sprintf_s(pTenm,"%d",5);
		m_Rate.SetWindowText(ToDRAWStr(pTenm).c_str());

		m_Rate.UpdateData(FALSE);
		
		
	}
	//��ȡ֡��
 	CString strRate;
 	m_Rate.GetWindowText(strRate);
 	strRate = strRate.Trim();
 	m_nRate =  atoi((char*)strRate.GetBuffer(strRate.GetLength()));
	strRate.ReleaseBuffer();
}

void CMedioVideoScreenDlg::GetAbsolute_path( const char* pPath,char* pAbsolute_path )
{
	if (pPath == NULL)
	{
		return;
	}
	while(*(pPath++));                     //�Ƶ�ԭʼ�ַ���β��
	while(*(--pPath)!='\\'&& (*pPath) != ':');               //��ԭʼ�ַ���β��ǰ�������һ����б�ܴ�
	if ((*pPath) == ':')
	{
		*pAbsolute_path = '\0';
		return;
	}
	while(*(pAbsolute_path++)=*(++pPath));       //�����һ����б�ܴ���ԭʼ�ַ��������θ�ֵ������ļ�����
}
void CMedioVideoScreenDlg::GetCurFileName(char* filename,char* pAbsolute_Name,char ch/* '.'*/)
{
	if (filename == NULL)
	{
		return;
	}
	while(*(pAbsolute_Name++) = *(filename ++));
	while(*(--pAbsolute_Name) != ch);
	*(pAbsolute_Name) = '\0';
}

BOOL CMedioVideoScreenDlg::GetAreaVideoScreenHwnd()
{
	typedef void(*lpGetAreaVideoScreenHwnd)(HWND& hwnd);
	lpGetAreaVideoScreenHwnd pGetAreaVideoScreenHwnd = (lpGetAreaVideoScreenHwnd)GetProcAddress(m_hDLL,"GetAreaVideoScreenHwnd");
	if (pGetAreaVideoScreenHwnd == NULL)
	{
		CString strFunAddError;
		strFunAddError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FunAddError"));
		if (strFunAddError.IsEmpty())
		{
			strFunAddError = _T("����Ѱַ����");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, _T("��(GetAreaVideoScreenHwnd)��")+strFunAddError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(_T("��(GetAreaVideoScreenHwnd)��")+strFunAddError);
		}
		else
		{
			if (g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetRECScreenIsShow(FALSE);
			}
			MessageBox(_T("��(GetAreaVideoScreenHwnd)��")+strFunAddError);
		}
		FreeLibrary(m_hDLL); m_hDLL=NULL;
		return FALSE;
	}
	pGetAreaVideoScreenHwnd(g_hVedioHwnd);
	if (g_hVedioHwnd)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CMedioVideoScreenDlg::IsFilePathOk( CString str )
{
	if (str.IsEmpty())
	{
		return FALSE;
	}
	int nflag = 0;
	nflag = str.Find(':');
	if (nflag == -1)
	{
		nflag = 0;
		return FALSE;
	}
	nflag = str.Find('\\');
	if (nflag == -1)
	{
		return FALSE;
	}
	PAVIFILE m_pFile = NULL;
	HRESULT m_hr = NULL;
	m_hr = AVIFileOpen(&m_pFile,str,OF_WRITE|OF_CREATE|OF_SHARE_DENY_READ|OF_READ,NULL);
	if (0 != m_hr)
	{
		CString msg;
		CString strUnKnowError;
		strUnKnowError = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("UnKnowError"));
		if (strUnKnowError.IsEmpty())
		{
			strUnKnowError = _T("δ֪����");
		}
		switch (m_hr)
		{
		case AVIERR_BADFORMAT:
			msg = _T("AVIERR_BADFORMAT--The file couldn't be read, indicating a corrupt file or an unrecognized format");
			break;
		case AVIERR_MEMORY:
			msg = _T("AVIERR_MEMORY--The file could not be opened because of insufficient memory.");
			break;
		case AVIERR_FILEREAD:
			msg = _T("AVIERR_FILEREAD--A disk error occurred while reading the file.");
			break;
		case AVIERR_FILEOPEN:
			msg = _T("AVIERR_FILEOPEN--A disk error occurred while opening the file.");
			break;
		case REGDB_E_CLASSNOTREG:
			msg = _T("REGDB_E_CLASSNOTREG--According to the registry, the type of file specified in AVIFileOpen does not have a handler to process it. ");
			break;
		default:
			msg = _T("δ֪����");
			break;
		}
		CString errorMsg;
		CString strAVIFail;
		strAVIFail = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("AVIFail"));
		if (strAVIFail.IsEmpty())
		{
			strAVIFail = _T("����AVI�ļ�ʧ��! ���ļ��ѱ�ռ�ã�");
		}
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("��ʾ");
		}
		errorMsg = strAVIFail /*+ msg*/;

		if (g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, errorMsg, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(errorMsg);
		}
		else
		{
			::MessageBox(NULL, errorMsg, strPrompt, MB_OK);
		}
		if (NULL != m_pFile)
		{
			AVIFileRelease(m_pFile);
			AVIFileExit();
		}
		m_bPathisLoad = true;
		str += '\0';
		SHFILEOPSTRUCT FileOp ;
		FileOp.fFlags = FOF_NOCONFIRMATION;
		FileOp.hNameMappings = NULL;
		FileOp.hwnd = NULL;
		FileOp.lpszProgressTitle = NULL;
		FileOp.pFrom = str;
		FileOp.pTo = NULL;
		FileOp.wFunc = FO_DELETE;
		BOOL nResult = SHFileOperation(&FileOp);
		return FALSE;
	}
	if (NULL != m_pFile)
	{
		AVIFileRelease(m_pFile);
		AVIFileExit();
	}
	m_bPathisLoad = false;
	str += '\0';
	SHFILEOPSTRUCT FileOp ;
	FileOp.fFlags = FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = str;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE;
	BOOL nResult = SHFileOperation(&FileOp);
	return TRUE;
}



void CMedioVideoScreenDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == CHECKMEMORYSTATUSISFULL)
	{
		bool bFlag = true;
		ULARGE_INTEGER   ulTotalSize;       //�����ܿռ��С       
		ULARGE_INTEGER   ulFreeSize;        //����ʣ��ռ��С
		ULARGE_INTEGER   ulAvailLableSize;  //¼���ļ���Ҫ�Ŀռ��С
		if (m_strFilePath.IsEmpty())
		{
			CDialogEx::OnTimer(nIDEvent);
			return;
		}
		CString csStr;
		csStr = m_strFilePath.Left(3);
		::GetDiskFreeSpaceEx(csStr,&ulTotalSize,&ulFreeSize,&ulAvailLableSize);

		if (ulTotalSize.QuadPart / 1024 /1024 < 512)
		{
			bFlag = false;
		} 
		if (!bFlag)
		{
			OnBnClickedButStop();
			KillTimer(CHECKMEMORYSTATUSISFULL);
		}

	}
	CDialogEx::OnTimer(nIDEvent);
}
