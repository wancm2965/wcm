// WhiteBoardView.cpp : 实现文件
//
/*
* 版权所有(C) 2010，华平信息技术股份有限公司
* 
* 文件名称：WhiteBoardView.cpp
* 文件标识：
* 摘    要：多点互动电子白板主对话框视图区的定义文件
* 
* 当前版本：
* 作    者：DengHong
* 完成日期：2010年11月6日
*/
#include "stdafx.h"
#include "WhiteBoard.h"
#include "WhiteBoardView.h"
#include "WBTextAttributeDlg.h"
//#include "Shlwapi.h"
#include <gdiplus.h>
#include <string>
#include "shai/AVDataPacket.h"
#include "DrawCtl.h"
#include "WhiteBoardDlg.h"
#include "WBToolboxDlg.h"
#include "common/WBMedia/WBGlobalMediaCtrl.h"
#include "WBImageSliderControl.h"
#include "WBResFunc.h"
#include <direct.h>
#include <MMSystem.h>
#include "ViewMTObject.h"//触摸数据分发管理
#include "ScreenDrawMgr.h"
#include "TouchAPI.h"
#include "ResData.h"
#include "MyHook.h"
#define  ROLLCALLSENDTIMER  1989101
using namespace std;
#define  CHECKMEMORYSTATUS    19920828
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///命中事件信号
HANDLE g_eventPrintMutex = NULL;
HANDLE g_eventPrintingBegin = INVALID_HANDLE_VALUE;
HANDLE g_eventPrintingEnd = INVALID_HANDLE_VALUE;
#define CONFIG_FILEPATH_OF_TOOLBAR	_T("\\WBConfig.ini")


#define  WATERDLG_WIDTH 60  //水波纹对话框宽
#define  WATERDLG_HEIGHT 60 //水波纹对话框高
#define  COMBINE_DELETE  10086//删除组合
#define  COMCURVE_DELETE 10087//删除融合到文档中的曲线
//码流设置保存wangxiaobin
int ReadMediaBitrate()//从WBConfig.ini中读取按钮的类型
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	CString strSelection = _T("Bitrate");
	LPCTSTR Position = _T("Position");
	return GetPrivateProfileInt(strSelection, Position, 200/*默认值*/, strPath);
}
void WriteMediaBitrate(int iValue)
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	CString strValue = _T("");
	strValue.Format(_T("%d"),iValue);
	CString strSelection = _T("Bitrate");
	LPCTSTR Position = _T("Position");
	WritePrivateProfileString(strSelection,Position,strValue, strPath);

}
//码流控制保存wangxiaobin
int ReadMediaControlBitrate()//从WBConfig.ini中读取按钮的类型
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	CString strSelection = _T("ControlBitrate");
	LPCTSTR Position = _T("Position");
	return GetPrivateProfileInt(strSelection, Position, 200/*默认值*/, strPath);
}
void WriteMediaControlBitrate(int iValue)
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	CString strValue = _T("");
	strValue.Format(_T("%d"),iValue);
	CString strSelection = _T("ControlBitrate");
	LPCTSTR Position = _T("Position");
	WritePrivateProfileString(strSelection,Position,strValue, strPath);
}

int ReadBroadCastStatus()
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	CString strSelection = _T("BroadCast");
	LPCTSTR Position = _T("Status");
	return GetPrivateProfileInt(strSelection, Position, 200/*默认值*/, strPath);
}

void WriteBroadCastStatus(int nStatus)
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	CString strValue = _T("");
	strValue.Format(_T("%d"),nStatus);
	CString strSelection = _T("BroadCast");
	LPCTSTR Position = _T("Status");
	WritePrivateProfileString(strSelection,Position,strValue, strPath);
}
////
static DWORD WINAPI PrintEventListener( LPVOID obj )
{
	////保证同一时间只能运行惟一一个当前线程
	if(WAIT_OBJECT_0 != WaitForSingleObject(g_eventPrintMutex, 1))
		return 0;

	//开始禁用白板
	if(g_pWhiteBoardView->GetParent() != NULL)  
	{

		::EnableWindow(g_pWhiteBoardView->GetParent()->GetSafeHwnd(), FALSE);
	}
	if (WAIT_OBJECT_0 == WaitForSingleObject(g_eventPrintingBegin, 5000))
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(g_eventPrintingEnd, INFINITE))
		{
			//
		}
		else
		{
			//OutputDebugString(TEXT("打印应用程序在预估时间内完成打印功能"));
		}
	}
	else
	{
		//OutputDebugString(TEXT("打印应用程序在预定时间内还没有启动"));
	}
	///重置信号
	ResetEvent(g_eventPrintingBegin);
	ResetEvent(g_eventPrintingEnd);
	ReleaseMutex(g_eventPrintMutex);

	//开启解禁白板
	if(g_pWhiteBoardView->GetParent() != NULL)   
	{

		::EnableWindow(g_pWhiteBoardView->GetParent()->GetSafeHwnd(), TRUE);
	}

	return 0;
}
using namespace Gdiplus;
// CWhiteBoardView

IMPLEMENT_DYNCREATE(CWhiteBoardView, CScrollView)
	int CWhiteBoardView::m_nViewCount = 0;



CWhiteBoardView::CWhiteBoardView()
	:  m_dwCurSelectObjLayerIndex(0)
	, m_pRgnDeleteWBCurve(NULL)
	, m_XnPos(0)
	, m_YnPos(0)
	, m_scrollX(WB_VIEWHSCROLL)
	, m_scrollY(WB_VIEWVSCROLL)
	, m_RectWbSize(NULL)
	, m_fWbRatioX(0.0f)
	,m_dReduceRate(0.00)//缩放
	, m_fWbRatioY(0.0f)
	, m_bIsCopy(FALSE)
	, m_bIsCut(FALSE)
	, m_bLocalFileOperate(false)
	, m_IsGetAllObj(FALSE)
	, m_IsInsertPageOver(FALSE)
	, m_hTOOLBARWnd(NULL)
	, m_pWBFunctionDlg(NULL)
	, m_pWBOptionDlg(NULL)
	, m_WBDataModel()
	, m_WBSpotlight(NULL)
	, m_pEagleEyeDlg(NULL)
	, m_bIsCloseScreenDlg(FALSE)
	, m_bIsTranspartentOrNot(TRUE)
	, m_dwCurAnnotationsLayerIndex(0)
	, m_dwCurTrapeziumLayerIndex(0)
	, m_pMediaCtrl(NULL)
	, m_ScreenDrawCtrl(NULL)
	, m_bSDIsTransparent(TRUE)
	, m_nSDAlpha(0)
	, m_bIsAnnotations(FALSE)
	, m_bIsEditing(FALSE)
	, m_pEditObj(NULL)
	, m_pUniqueWBCamera(NULL)
	, m_ulRecOldEventID(0)
	, m_bRefurbishPreview(TRUE)
	, m_nComCount(0)
	, m_pComObj(NULL)
	, m_dwComLayerIndex(0)
	, m_pPage(NULL)
	, m_pageIsNeedDraw(TRUE)
	, m_viewIsNeedDraw(TRUE)
	, m_nPrePage(0)
	, m_bIsMinSize(FALSE)
	, m_bIsOpenSCDraw(FALSE)
	, m_bIsSDTransparent(TRUE)
	, m_bIsFlashCurPage(FALSE)
	, m_bIsCloneMedia(FALSE)
	, m_bIsTouch(FALSE)
	, m_pDlgPrintLoadingMsg(NULL)
	, m_bIsDocConvert(FALSE)
	, m_bIsMedialubo(FALSE)
	, m_RECScreenIsShow(TRUE)
	, m_dwTimeGetTime(0)
	,m_dwCheckItem(0)
	, m_bMoveIsNeedSendData(FALSE)
	, m_EraseCurveLine(FALSE)
	, m_pFullScreen(NULL)
	, m_pDocTip(NULL)
	, m_ulMoveCounts(0)
	, m_nMoveObj_Realtime_Synchronizatio_Counts(0)
	, m_bBroadcastUserCursor(false)
	, m_EraseLineMoveMsg(TRUE)
	, m_bEraseLineUp(FALSE)
	, m_nCameraObjId(0)
	, m_isLocalWhiteboard(false)
	,m_bIsColorPaletteDlg(false)
	,m_bIsneedjionBroadcast(TRUE)
	, m_WBRuler(NULL)
	, m_ulTouchEventId(0)
	,m_bIsTouchUp(false)
	,m_bCamaraMediaClk(false)
	,m_bCamaraMedia(false)
	,m_pUserNameID(NULL)
	,m_pCamaraObj(NULL)
	,m_pEstimate(false)
	,m_WBProtractor(NULL)
	,m_WBAngle(NULL)
	,m_bIsRButtonDown(false)
	,m_ntouchID(0)
	,m_bIsCameraUNDO(false)
	,m_bIsconfernce(false)
	,m_bIsCombin(false)
	,m_pWBSavePictureDlg(NULL)
	,m_ptIsKeyDel(FALSE)
	,m_ptIsTouchCom(FALSE)
	,m_ptIsRationButton(FALSE)
	,m_nDocCurPageIndex(0)
	,m_ulIcoCurEvent(GenerateSSRC())
	,m_dwIcoCurId(0)
	,m_pLoadWBFile(NULL)
	,m_nCompassDrawShape(0)
	,m_bCompassOptMenu(TRUE)
	,m_pResourcePic(NULL)
	,m_bSelectStatueChange(1)
{
	//工具条的相关变量初始化
	m_sLastDragFileURL = _T("");

	//放大镜
	m_pWBRectMaqn  = NULL;
	//垃圾箱
	m_pWBDustbin = NULL;
	m_IsMoveDustbin = FALSE;
	m_PreMousePos = MOUSE_OUTSIDE;// add by xiewenxiu 2011
	m_MedioVideoScreenDlg = NULL;
	m_isFullScreen = FALSE;
	m_nTimerID=0;
	m_nTimerLimitObjPro = 0;
	m_XOffSetPicture = 0;
	m_YOffSetPicture = 0;
	m_MediaID = 0;
	m_MediaIDIsActive = 0;
	m_waterdlg = NULL;
	m_wbrcprodlg = NULL;
	m_nRollCallTimer = 40;
	m_wbrcshowdlg = NULL;
	m_EraseRectDowm = false;
	m_ptMovePoint = CPoint(0,0);
	m_lmovecounts = 0;
	m_ptPointLineCurve = CPoint(0,0);
	m_RichEditRect = NULL;
	m_ptDownPartRefresh = CPoint(0, 0);
	m_ptMovePartRefresh = CPoint(0, 0);
	m_rectRefresh = CRect(0, 0, 0, 0);
	m_PreRectRefresh = CRect(0, 0, 0, 0);
	m_bflag = false;
	//	InitCreateThread();
	//
	m_strUserCurCameraName = "";
	m_strUserID = "";
	m_channelID = 0;
	m_DropChanelID = 0;
	m_ReceiveWaterUserID = "";
	m_ReceiveWaterPoint = CPoint(0,0);	
	m_pTransparent_dlg = NULL;
	m_pUserNameID = new std::map<std::string ,DWORD>;
	m_pResourcePic = new std::map<CString ,VECTORSPECPIC>;
	m_dwCurOperationObjId = -1;// -1 表示没有操作对象 0 表示正在操作多个选择框 
	//增加GDI+支持
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	m_nLDBKPoint = CPoint(0,0);
	m_bIsdrop = FALSE;
	m_strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	m_nSendRollCallBroadCastID = 0;
	if (m_strPrompt.IsEmpty())
	{
		m_strPrompt = _T("提示");
	}
// 	int nStatus = ReadBroadCastStatus();
// 	if (nStatus == 50)
// 	{
// 		m_bBroadcastUserCursor = true;
// 	}
// 	else
// 	{
// 		m_bBroadcastUserCursor = false;
// 	}
	m_RcDrawText = CRect(0,0,0,0);
	m_bUseToControlDraw = true;
	m_bLimitCursorArea = false;
	m_nTriangleType = 0;
	//m_ptstore = NULL;
	m_bStartRecognition = false;
	m_bRecogniMouseDown = false;
	m_bRecogniTouchDown = false;
	m_pGeoRecognizer = NULL;
	m_bNeedSetPageIndexForScreenDraw = false;
	m_pWBPicture = NULL;
	m_bIsDBClick = false;
	m_dDBClickobjlayer = 0;
	m_strResourceUpdataPath = _T("");
	m_strDBClickSavePath = _T("");
	//控制U7下人员列表摄像头的是否支持拖拽到白板操作
  if (g_WBType == WB_MULTIVER)//网络版
	{
		m_strLocalNotDropCameraToWB = _T("");
		CString strDropCamera;
		strDropCamera = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("DropCarema"));
		if (strDropCamera.IsEmpty())
		{
			strDropCamera = _T("0");
		}
		if (strDropCamera == _T("0"))
		{
			m_bIsAllowDropCamera = FALSE;
		}
		else if (strDropCamera == _T("1"))
		{
			m_bIsAllowDropCamera =  TRUE;
			//本地会议调用
			if (g_isLoacalWB)
			{
				m_strLocalNotDropCameraToWB = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("DropNotFromU7Carema"));
				if (m_strLocalNotDropCameraToWB.IsEmpty())
				{
					m_strLocalNotDropCameraToWB = _T("本地会议不支持拖拽摄像头到白板!");
				}
			}
		}
	}
	else
	{
		m_strLocalNotDropCameraToWB = _T("");
		m_bIsAllowDropCamera = FALSE;
	}
  if(g_bSupportI7)//I7设置背景色
  {
	  g_clrWBPageColor = RGB(238, 238, 238);
  }
}

CWhiteBoardView::~CWhiteBoardView()
{		
	if(m_pFullScreen)
	{
		delete m_pFullScreen;
		m_pFullScreen = NULL;
	}
	if(m_pDocTip)
	{
		delete m_pDocTip;
		m_pDocTip = NULL;
	}
	m_pCaremaPicture.clear();
	m_RecShowpoint.clear();
	m_pDropPicObj.clear();
	for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
	{
		if (m_pObjCopyList[i] != NULL)
		{
			delete m_pObjCopyList[i];
			m_pObjCopyList[i] = NULL;
		}
	}
	m_pObjCopyList.clear();
	if (m_pGeoRecognizer)
	{
		delete m_pGeoRecognizer;
		m_pGeoRecognizer = NULL;
	}
	if (m_pWBSavePictureDlg)
	{
		delete m_pWBSavePictureDlg;
		m_pWBSavePictureDlg = NULL;
	}
	if (m_pUserNameID != NULL)
	{
		m_pUserNameID->clear();
		delete m_pUserNameID;
		m_pUserNameID = NULL;
	}
	if (m_pCamaraObj)
	{
		m_pCamaraObj = NULL;
	}
	m_nViewCount --;
	if (m_pRgnDeleteWBCurve != NULL)
	{
		delete m_pRgnDeleteWBCurve;
	}

	if (m_nViewCount <= 0 && g_pWBFile != NULL)
	{
		delete g_pWBFile;
		g_pWBFile = NULL;
	}
	if (m_pLoadWBFile != NULL)
	{
		if (m_pLoadWBFile->GetWBPageList() != NULL)
		{
			m_pLoadWBFile->GetWBPageList()->RemoveAll();
			delete m_pLoadWBFile->GetWBPageList();
			m_pLoadWBFile->WBPageListSetNull();
		}
		if (m_pLoadWBFile->GetFileHeadItem() != NULL)
		{
			delete m_pLoadWBFile->GetFileHeadItem();
			m_pLoadWBFile->PWBFileHeadItemSetNull();
		}
		delete m_pLoadWBFile;
		m_pLoadWBFile = NULL;
	}
	if (m_WBRuler != NULL)
	{
		delete m_WBRuler;
		m_WBRuler = NULL;
	}
	if (m_WBProtractor != NULL)
	{
		delete m_WBProtractor;
		m_WBProtractor = NULL;
	}
	if (m_WBAngle !=NULL)
	{
		delete m_WBAngle;
		m_WBAngle = NULL;
	}
	for (int i = 0; i < (int)m_vecLineLayerIndexBeforeErase.size(); ++i )
	{
		if (m_vecLineLayerIndexBeforeErase[i] != NULL)
		{
			delete m_vecLineLayerIndexBeforeErase[i];
		}
	}
	if (m_wbrcprodlg)
	{
		m_wbrcprodlg->DestroyWindow();
		delete m_wbrcprodlg;
		m_wbrcprodlg = NULL;
	}
	if (m_wbrcshowdlg)
	{
		delete m_wbrcshowdlg;
		m_wbrcshowdlg = NULL;
	}
	m_vecLineLayerIndexBeforeErase.clear();
	//m_ZDeleteMediaIDList.clear();
	m_RollCallNameList.clear();
	//m_ZDeleteCameraIDList.clear();
	//
	
	if (m_pWBPicture)
	{
		delete m_pWBPicture;
		m_pWBPicture = NULL;
	}
	if (m_pResourcePic)
	{
		m_pResourcePic->clear();
		delete m_pResourcePic;
		m_pResourcePic = NULL;
	}
	if (NULL != m_pWBRectMaqn)
	{
		delete m_pWBRectMaqn;
		m_pWBRectMaqn = NULL;
	}

	if (NULL != m_pWBDustbin)
	{
		delete m_pWBDustbin;
		m_pWBDustbin = NULL;
	}
	if (NULL != m_WBSpotlight)
	{
		delete m_WBSpotlight;
		m_WBSpotlight = NULL;
	}
	if (NULL != m_ScreenDrawCtrl)
	{
		delete m_ScreenDrawCtrl;
		m_ScreenDrawCtrl = NULL;
	}
	if (NULL != m_MedioVideoScreenDlg)
	{
		delete m_MedioVideoScreenDlg;
		m_MedioVideoScreenDlg = NULL;
	}
	m_mapMTDrawPar.clear();
	if (m_pMediaCtrl != NULL)
	{
		delete m_pMediaCtrl;
		m_pMediaCtrl = NULL;
	}
	if (m_pEagleEyeDlg != NULL)
	{
		m_pEagleEyeDlg = NULL;
	}
	if (m_MediaList.size() != 0)
	{
		m_MediaList.clear();
	}
	m_MediaStringPath.clear();

	if(NULL != g_eventPrintMutex)
	{
		ReleaseMutex(g_eventPrintMutex);
		::CloseHandle(g_eventPrintMutex);
		g_eventPrintMutex = NULL;
	}
	if(INVALID_HANDLE_VALUE != g_eventPrintingBegin)
	{
		ReleaseMutex(g_eventPrintingBegin);
		::CloseHandle(g_eventPrintingBegin);
		g_eventPrintingBegin = INVALID_HANDLE_VALUE;
	}
	if(INVALID_HANDLE_VALUE != g_eventPrintingEnd)
	{
		ReleaseMutex(g_eventPrintingEnd);
		::CloseHandle(g_eventPrintingEnd);
		g_eventPrintingEnd = INVALID_HANDLE_VALUE;
	}
	//释放GDI+
	GdiplusShutdown(m_gdiplusToken);

	if(m_pRecvFileWBIcoCurveMap.size() != 0)
	{
		m_pRecvFileWBIcoCurveMap.clear();
	}

	/*if(m_pSendFileWBIcoCurveMap.size() != 0)
	{
		m_pSendFileWBIcoCurveMap.clear();
	}*/
}

BEGIN_MESSAGE_MAP(CWhiteBoardView, CScrollView)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_MESSAGE(WM_GESTURE, OnGesture)
	ON_MESSAGE(WM_TOUCH, OnTouch)
	ON_MESSAGE(UWM_TRACKPOPUPMENU, OnTrackPopupMenu)
	//ON_MESSAGE(UWM_RESGALLERY_BOUNDARY, OnReceiveDragResouce)
	ON_MESSAGE(UWM_DRAGOUT_RESOUCE, OnReceiveTouchResouce)
	ON_MESSAGE(UWM_DELFILE_OPEN,ProcessResourceMessageBox)
	ON_MESSAGE(WB_DELETEOBJ, OnWbDeleteobj)
	ON_MESSAGE(WB_MAQNOBJ, OnWbMaqnobj)
	ON_MESSAGE(WB_SPOTLIGHTOBJ, OnWbSpotlightobj)
	ON_MESSAGE(UWM_CREATE_RICHEDIT, OnWbCreateRichEdit)
	ON_MESSAGE(UWM_EDIT_RICHEDIT, OnWbEditRichText)
	ON_MESSAGE(UWM_DESTROY_RICHEDIT, OnWbDestroyRichText)
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WB_PROCESSOBJ, &CWhiteBoardView::OnWbProcessobj)
	ON_MESSAGE(WB_ADDFLASH, &CWhiteBoardView::OnProcessAddFlash)
	ON_MESSAGE(WM_ADDMEDIA, &CWhiteBoardView::OnProcessAddMedia)
	ON_MESSAGE(WM_ADDCAMERA, &CWhiteBoardView::OnProcessAddCamera)
	ON_MESSAGE(WM_WBCAMERA_NET_CONNECT_SYNCHRONOUS, &CWhiteBoardView::OnSendCameraConnectSynchronous)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_MESSAGE(WB_SCREENDRAW, &CWhiteBoardView::OnProcessScreenDraw)
	ON_MESSAGE(WM_PROCESS_FILE_OPERATE, &CWhiteBoardView::OnProcessFileOperate)
	//	ON_MESSAGE(WM_DEL_ALL_ATTRIBUTE, &CWhiteBoardView::OnWbDelAllAttribute)
	ON_MESSAGE(WM_NOTICE_PRAVIEW_REFRESH, &CWhiteBoardView::OnNoticePraviewRefresh)
	ON_MESSAGE(WM_PRAVIEW_REFRESH, &CWhiteBoardView::OnPraviewRefresh)
	ON_MESSAGE(WM_DELALLOBJECT, &CWhiteBoardView::OnDelallobject)
	ON_WM_SETCURSOR()
	//ON_MESSAGE(WM_CLOSEWBFUNCTIONDLG, &CWhiteBoardView::OnClosewbfunctiondlg)
	ON_COMMAND_RANGE(ID__20,ID__320,&CWhiteBoardView::OnReduce)
	ON_MESSAGE(WM_DEL_PAGE_ALL_OBJ, &CWhiteBoardView::OnWbDelPageAllObj)
	ON_MESSAGE(WM_CREATE_PRINT_LOADINGMSG, &CWhiteBoardView::OnCreatePrintLoadingMsg)
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_SENDPAGEDATATO_CONFERENCE_RECORD, &CWhiteBoardView::OnSendPageDatatoConferenceRecord)
	ON_MESSAGE(WM_CREATE_WATER, &CWhiteBoardView::OnCreateWaterDlg)
	ON_MESSAGE(WM_MTSENDCURSOR, &CWhiteBoardView::ProcessMTUserCursor)
	ON_MESSAGE(WM_PROCESS_TRANDLG, &CWhiteBoardView::OnProcess_CurorTranDlg)
	ON_MESSAGE(WM_PROCESS_WATERDLG, &CWhiteBoardView::OnProcess_WaterDlg)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PROCESSBROADCASTOBJ, &CWhiteBoardView::OnProcessBroadcastObj)
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_POPMENU, &CWhiteBoardView::OnPopmenu)
	ON_MESSAGE(WM_RectMagn_CLOSE, &CWhiteBoardView::OnProcessRectMagnClose)
	ON_WM_MBUTTONDOWN()
	ON_MESSAGE(WM_ON_DROP_NOTIFY, OnDropNotify)	

	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CWhiteBoardView 绘图

HWND CWhiteBoardView::GetHWND()
{
	return m_hWnd;
}


void CWhiteBoardView::OnDraw(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	//CDC dcMem;//以下是输出位图的标准操作
	CBitmap *pOldBitmap = NULL;
	if (NULL != m_dcMem.m_hDC)
	{
		m_dcMem.DeleteDC();
	}
	if (m_bmpBuf.m_hObject != NULL)//wangxiaobin 
	{
		m_bmpBuf.DeleteObject();
	}
	m_dcMem.CreateCompatibleDC(pDC);

	BITMAPINFOHEADER bmih;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = m_scrollX;
	bmih.biHeight = m_scrollY;
	bmih.biPlanes = 1;
	bmih.biCompression = BI_RGB;
	bmih.biBitCount = 24;

	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader = bmih;

	void* p;
	HBITMAP hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), &bmi, DIB_RGB_COLORS, &p, NULL, 0);

	m_bmpBuf.Attach(hBitmap);
	if (m_bmpBuf.m_hObject == NULL)
	{
		return;
	}
	pOldBitmap = m_dcMem.SelectObject(&m_bmpBuf);
	if (::DeleteObject(hBitmap))
	{
		hBitmap = NULL;
	}

	BITMAP bmpinfo;
	m_bmpBuf.GetBitmap(&bmpinfo);
	m_dcMem.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));

	if(g_reduce>0.00000000000000000)
	{
		ChangeViewCoordinate(pDC);
	}
	else
	{
		OnPrepareDC(&m_dcMem);
	}
	

	//   局部刷新  创建剪切区域
	if (m_bflag)
	{
		CRect mrect=((CPaintDC*)pDC)->m_ps.rcPaint; //获得无效客户区域
		CRgn rgn;
 		rgn.CreateRectRgnIndirect(&mrect);
		m_dcMem.SelectClipRgn(&rgn, RGN_AND);
		//m_bflag = false;
	}
	
	if (g_pWBPage && m_pageIsNeedDraw)//如果全局页指针不为空和页内容有改变
	{
		g_pWBPage->Draw(&m_dcMem);
	}

	::BitBlt(m_dcMem.m_hDC,0, 0, g_nWidth, g_nHeight, g_pWBPage->GetMemHDC(), 0, 0, SRCCOPY);	

	switch(g_nDrawStatus)
	{
	case DRAW_SELECTINT:
		{
			//m_pageIsNeedDraw = TRUE;    //放开开关	
			CPen selectPen(PS_DASHDOT, 1, RGB(0, 0, 0));
			CPen *pselectOldPen = m_dcMem.SelectObject(&selectPen);
			m_dcMem.SelectStockObject(NULL_BRUSH);
			CRect tempRect = m_rectSelect;
			tempRect.NormalizeRect();
			m_dcMem.Rectangle(&tempRect);
			m_dcMem.SelectObject(pselectOldPen);
			::DeleteObject(selectPen);
		}
		break;
	case DRAWING_TEXT:
		{
			m_pageIsNeedDraw = TRUE;    //放开开关	
			m_dcMem.SelectStockObject(NULL_BRUSH);
			m_dcMem.Rectangle(&m_rectText);
		}
		break;
	case DRAW_ERASING://画橡皮擦
	case DRAW_CLEANUPING:
		{
			m_pageIsNeedDraw = FALSE;    //关闭开关
			CPen brushPen(PS_SOLID, 2, RGB(0, 0, 0));
			CPen *pBrushOldPen = m_dcMem.SelectObject(&brushPen);
			m_dcMem.Rectangle(&m_rectBrush);
			m_dcMem.SelectObject(pBrushOldPen);
			::DeleteObject(brushPen);
		}
		break;
	case DRAWED_TEXT:
		{
			
			CRect rc;		
			if (m_bUseToControlDraw)
			{
				rc = GetRichEditRect();
				rc.left -= 1;
				rc.top -= 1;
				rc.bottom += 1;
				rc.right += 1;
				if (g_reduce>0.00)
				{
					rc.left = rc.left/g_reduce;
					rc.top = rc.top/g_reduce;
					rc.bottom = rc.bottom/g_reduce;
					rc.right = rc.right/g_reduce;
				}
				rc.left = rc.left + g_pWhiteBoardView->getXnPos();
				rc.top = rc.top + g_pWhiteBoardView->getYnPos();
				rc.right = rc.right + g_pWhiteBoardView->getXnPos();
				rc.bottom = rc.bottom + g_pWhiteBoardView->getYnPos();				
				m_RcDrawText = rc;
				m_bUseToControlDraw = false;
			}
			m_dcMem.SelectStockObject(NULL_BRUSH);
			m_dcMem.Rectangle(&m_RcDrawText);
		}
		break;
	default:
		{
			m_pageIsNeedDraw = TRUE;    //放开开关	
		} 

	}
	 
	if (m_bStartRecognition)
	{
		if (m_RecShowpoint.size()>2)
		{
			CPen selectPen(PS_SOLID, 1, RGB(0, 0, 0));
			CPen *pselectOldPen = m_dcMem.SelectObject(&selectPen);
			m_dcMem.SelectStockObject(NULL_BRUSH);
			for (int i = 0;i<m_RecShowpoint.size()-1;++i)
			{
				
				CPoint pt = m_RecShowpoint[i];
				CPoint pt1 = m_RecShowpoint[i+1];
				m_dcMem.MoveTo(pt);
				m_dcMem.LineTo(pt1);
			}
			m_dcMem.SelectObject(pselectOldPen);
			::DeleteObject(selectPen);
		}
	}
	
	if (m_pWBPicture != NULL)
	{
		m_pWBPicture->Draw(&m_dcMem);
	}
	//放大镜
	if(m_pWBRectMaqn != NULL)
	{
		m_pWBRectMaqn->DrawMagnifier(&m_dcMem);
	}
	//垃圾箱
	if (m_pWBDustbin != NULL )
	{ 
		if (m_IsMoveDustbin)
		{
			CPoint point =  m_DustbinPoint - m_ptPreDustbinPoint;
			m_ptPreDustbinPoint = m_DustbinPoint;			
			m_pWBDustbin->MoveRect(point);
			m_pWBDustbin->MoveOffset(point);
		}		
		m_pWBDustbin->Draw(&m_dcMem);
		
	}
	if(!g_bSupportI7)
	{
		//显示页
		CString str;
		str.Format(_T("%d/%d"), g_pWBFile->GetCurPageIndex(), g_pWBFile->GetPageCount());
		m_dcMem.TextOut(10, 10, str);
	}
	
	::PostMessage(CWBPageDlg::g_WBPageDlgHWND,WM_WB_SHOWPAGENUMBER,0,0);

	::BitBlt(pDC->m_hDC,0,0,bmpinfo.bmWidth,bmpinfo.bmHeight,m_dcMem.m_hDC,0,0,SRCCOPY);// 改成播放图片点
	//::TransparentBlt(pDC->m_hDC,0,0,rect.Width(),rect.Height(),dcMem,0,0,rect.Width(),rect.Height(),RGB(255, 255, 255));

	m_dcMem.SelectObject(pOldBitmap);	

	if(IsOpenPreviewPanel())
	{
		if (g_pWhiteBoardView->GetWBFunctionDlg()&& NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG())
		{
			::SendMessage(g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->m_hWnd,WM_WB_FUNCTIONVIEWSYN,0,0);
		}
	}
	if (m_pEagleEyeDlg != NULL)
	{
		if (m_pEagleEyeDlg->GetPreviewSingleView() != NULL)
		{
			::PostMessage(m_pEagleEyeDlg->GetPreviewSingleView()->m_hWnd,WM_EAGLEEYE_SYNCHRONIZER,0,0);
		}
	}
	m_bmpBuf.DeleteObject();
	if (m_dcMem.m_hDC && m_dcMem.GetSafeHdc())//有可能为空就不需要释放
	{
		DeleteDC(m_dcMem.GetSafeHdc()); 
	}
}


// CWhiteBoardView 诊断

#ifdef _DEBUG
void CWhiteBoardView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CWhiteBoardView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWhiteBoardView 消息处理程序

/*************************************************************************
*                        CWhiteBoardView::OnCreate()                     *
*                注册应用程序客户端视图窗口来接收触控消息                *
*************************************************************************/
int CWhiteBoardView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	//初始化触摸
	CWBMTObjectManager::Init( GetSafeHwnd(), this);

	if (g_pWBFile == NULL)
	{
		g_pWBFile = new CWBFile();
		g_pWBFile->AddPage(this);

		g_pWBPage = g_pWBFile->GetCurPage();
	}
	//加载弹出菜单中的图片
	LoadPopupMenuBitmap();
	//设置响应网络数据缓冲池定时器
	m_nTimerID=::SetTimer(NULL,0,100,(TIMERPROC)&OnCallbackData);
	StartProReliefTimer();

	//SetTimer(CHECKMEMORYSTATUS,180000,NULL);
	///创建信号对象
	g_eventPrintMutex = ::CreateMutex(NULL, FALSE, TEXT("print_order_97B18EEF-BA36-4BAC-BF51-6F6C5A6D243E"));
	g_eventPrintingBegin = ::CreateEvent(NULL, FALSE, FALSE, TEXT("print_begin_97B18EEF-BA36-4BAC-BF51-6F6C5A6D243E"));
	ResetEvent(g_eventPrintingBegin);
	g_eventPrintingEnd = ::CreateEvent(NULL, FALSE, FALSE, TEXT("print_end_97B18EEF-BA36-4BAC-BF51-6F6C5A6D243E"));
	ResetEvent(g_eventPrintingEnd);
	m_myDropTarget.DragDropRegister(m_hWnd);
	return 0;
}

unsigned int PtLength(POINT ptStart, POINT ptEnd)
{
	unsigned int dx = ptEnd.x - ptStart.x;
	unsigned int dy = ptEnd.y - ptStart.y;
	return (unsigned int)sqrtl(dx*dx+dy*dy);
}

bool CWhiteBoardView::BInRegion(const POINT &pt)const
{
	if (::WindowFromPoint(pt) == m_hWnd)
	{
		return true;
	}
	return false;
}

void CWhiteBoardView::touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te)
{
	XAutoLock lock(m_csDraw);
	////转换成客户区坐标
	CPoint pt;
	pt.x = (LONG)te.xpos;
	pt.y = (LONG)te.ypos;

	ScreenToClient(&pt);
	CClientDC dc(this);
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&pt);//将视图坐标转换为文档作标

	DWORD dwTouchID = te.id;
	m_dwTimeGetTime = ::timeGetTime();
	//处理放大镜
	if(MouseDownMagn(pt))
	{
		return ;
	}
	if (m_bStartRecognition)
	{
		if (!path2D.empty())
		{
			path2D.clear();
		}
		if (!m_RecShowpoint.empty())
		{
			m_RecShowpoint.clear();
		}
		PointRetain point(pt.x,pt.y);
		path2D.push_back(point);
		m_bRecogniTouchDown = true;
		m_RecShowpoint.push_back(pt);
		Invalidate();
		return;
	}
	//发送关闭子工具条的消息
	::PostMessage(g_pWhiteBoardView->m_hTOOLBARWnd,WM_CLOSE_TOOBAR_MES,0,0);
	if ((m_pWBFunctionDlg != NULL)&&(m_pWBFunctionDlg->GetAttributeDlg() != NULL))
	{
		::PostMessage(m_pWBFunctionDlg->GetAttributeDlg()->m_hWnd,WM_CLOSE_TOOBAR_MES,0,0);
	}
	//移动垃圾箱  
	if (NULL != m_pWBDustbin)
	{
		int MousePos = m_pWBDustbin->PointIn(pt);
		m_PreMousePos = MousePos;//add by xiewenxiu 20111208
		if (MOUSE_OUTSIDE != MousePos)
		{
			m_DustbinPoint =m_ptPreDustbinPoint = pt;
			m_IsMoveDustbin = TRUE;
			m_nDrawStatus = g_nDrawStatus;
			g_nDrawStatus = MOVE_DUSTBIN;
		}

	}

	m_MTDrawObjectTally.touchDownHandler(te.id); //huanglongquan add
	switch (g_nDrawStatus)
	{
	case DRAW_LINE:   //TouchDown直线
		{
			CWBLine *pLine = new CWBLine(pt, pt);
			pLine->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pLine);
			m_ptPointLineCurve = pt;//wangxiabin保存按下点坐标
			//m_lstDrawingObject.push_back(pLine);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pLine, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case MIDI_DRAG:
		{
			int ntemp = g_pWBPage->PointInSelectFrame(pt);
			if (ntemp == WB_PS_SELECTFRAME)
			{
				DWORD dcurseobj = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject* pobj = g_pWBPage->FindByLayerIndex(dcurseobj);
				if (pobj == NULL)
				{
					break;
				}
				if (pobj->GetType() == WB_OBJ_MIDI)
				{
					((CWBMidi*)pobj)->DownChangePosition(pt);
					PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pobj, pt, te.time);
					m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
				}
			}
		}
		break;
	case DRAW_ROUND:		
		break;
	case DRAW_POLYLINE: //TouchDown曲线
		{
			CWBCurve *pCurve = new CWBCurve();
			if (pCurve == NULL)
			{
				break;
			}

			pCurve->SetResume_broken_taransfer(true);
			pCurve->SetAlpha((int)g_nTranspar);
			pCurve->SetLeftTopRightBottomPt(pt, pt);
			pCurve->AddPoint(pt);
			m_ptPointLineCurve = pt;
			m_topLeft = pt;		
			g_pWBPage->AddWBObject(pCurve);
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pCurve, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAW_POLYICON:
		{
			CWBIcoCurve *pIcoCure = new CWBIcoCurve();
			if (pIcoCure != NULL)
			{
				pIcoCure->SetResume_broken_taransfer(true);
				pIcoCure->SetAlpha((int)g_nTranspar);
				pIcoCure->AddPoint(pt);
				pIcoCure->SetBeginPoint(pt);
				pIcoCure->SetLeftTopRightBottomPt(pt,pt);
				pIcoCure->SetIsLoadPicFromFile(g_bIsWBLoadIcoFromFile);
				g_pWBPage->AddWBObject(pIcoCure);
				PMTDrawPar drawPar = new MTDrawPar(dwTouchID,g_nDrawStatus,pIcoCure,pt,te.time);
				m_mapMTDrawPar.insert(std::pair<DWORD,MTDrawPar*>(dwTouchID,drawPar));
			}
		}
		break;
	case DRAW_MAGICLINE:
		{
			CWBMagicPen *pMagicPen = new CWBMagicPen();
			if (pMagicPen != NULL)
			{
				pMagicPen->SetResume_broken_taransfer(true);
				pMagicPen->SetAlpha((int)g_nTranspar);
				pMagicPen->SetLeftTopRightBottomPt(pt,pt);
				pMagicPen->AddPoint(pt);
				g_pWBPage->AddWBObject(pMagicPen);
				PMTDrawPar drawPar = new MTDrawPar(dwTouchID,g_nDrawStatus,pMagicPen,pt,te.time);
				m_mapMTDrawPar.insert(std::pair<DWORD,MTDrawPar*>(dwTouchID,drawPar));
			}
		}
		break;
	case DRAW_RECT: //TouchDown矩形
		{
			CWBRectangle *pRect = new CWBRectangle(pt, pt);
			pRect->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pRect);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pRect, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAW_PALLELOGAM:
		{
			CWBPallelogam *pPallelogam = new CWBPallelogam(pt,pt);
			pPallelogam->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pPallelogam);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pPallelogam, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAWANYTRIANGLE:
		{		
			CWBArbitraryTriangle *pTriangle = new CWBArbitraryTriangle(pt,CPoint(pt.x+30,pt.y+100),CPoint(pt.x+100,pt.y+100));
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTriangle);
			pTriangle->SetAlpha((int)g_nTranspar);
			g_nDrawStatus = DRAWINGANYTRIANGLE;
			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pTriangle, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAW_TRIANGLE:	//TouchDown画三角形
		{
			CWBTriangle *pTriangle = new CWBTriangle(pt, pt,m_nTriangleType);
			pTriangle->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pTriangle);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pTriangle, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;

	case DRAW_ELLIPSE:  //TouchDown椭圆
		{
			CWBEllipse *pEllipse = new CWBEllipse(pt, pt);
			pEllipse->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pEllipse);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pEllipse, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;
	case DRAWED_TEXT:
		{
			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				//SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
				//PostMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}

			PostMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			g_nDrawStatus = DRAWEUP_TEXT;
		}
		break;
	case DRAW_TEXT:    //TouchDown文本
		{
			m_bIsAnnotations = FALSE;
			m_rectText.left = pt.x;
			m_rectText.top = pt.y;
			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			CWBRichEditMgr::GetInst_()->SetBeginPoint(pt.x, pt.y, te.id);

			SetPageIsNeedDraw(FALSE);
			g_nDrawStatus = DRAWING_TEXT;
		}
		break;

	case DRAW_POLYGON:	//TouchDown画多边形的第一个点
		{
			CWBPolygon *pPolygonTouch = new CWBPolygon();
			pPolygonTouch->SetAlpha((int)g_nTranspar);
			pPolygonTouch->AddPoint(pt);//添加起始点
			pPolygonTouch->AddPoint(pt);//添加第二点（以便于touchmove时动态改变第二个点时看到改变的状态）
			g_pWBPage->AddWBObject(pPolygonTouch);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pPolygonTouch);
			drawPar->UpdatePt(pt);
			drawPar->UpdateValidPt(pt);
			drawPar->UpdateTime(te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));
		}
		break;

	case DRAW_RHOMBUS:	//TouchDown菱形
		{
			CWBRhombus *pRhombus = new CWBRhombus(pt, pt);
			pRhombus->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pRhombus);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pRhombus, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_PENTAGON:	//TouchDown五边形
		{
			CWBPentagon *pPentagon = new CWBPentagon(pt, pt);
			pPentagon->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pPentagon);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pPentagon, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_HEXAGON:	//TouchDown六边形
		{
			CWBHexagon *pHexagon = new CWBHexagon(pt, pt);
			pHexagon->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pHexagon);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pHexagon, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_FIVEPOINTSTART:
		{
			CWBFivepointstart *pFivepointstart = new CWBFivepointstart(pt, pt);
			pFivepointstart->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pFivepointstart);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pFivepointstart, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_TRAPEZIUM:
		{
			CWBTrapezium *pTrapezium = new CWBTrapezium(pt, pt);
			pTrapezium->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pTrapezium);

			PMTDrawPar drawPar = new MTDrawPar(dwTouchID, g_nDrawStatus, pTrapezium, pt, te.time);
			m_mapMTDrawPar.insert(std::pair<DWORD, MTDrawPar *>(dwTouchID, drawPar));	
		}
		break;
	case DRAW_ERASE: //TouchDown擦除
		{
			m_dwCurBrushTouchID = te.id;
			g_nDrawStatus = DRAW_ERASING;

			g_pWBPage->GetAllObjLayerIndex(m_vecLineLayerIndexBeforeErase, WB_OBJ_LINE); //保存擦除之前所有直线对象的图层号
		}
		break;
	case DRAW_RGNDELETING:
		break;
	case DRAW_RGNDELETE: //TouchDown区域删除
		{
			std::map<int, CWBCurve*>::iterator iter = m_pRgnDeleteWBCurveMap.find(te.id);
			if (iter == m_pRgnDeleteWBCurveMap.end())
			{
				PWBCurveItem pWBCurveItem = new WBCurveItem;
				pWBCurveItem->SetWidth(2);
				pWBCurveItem->SetColor(RGB(0, 255, 0));
				CWBCurve* pWBCurve = new CWBCurve(pWBCurveItem);
				pWBCurve->AddPoint(pt);
				m_pRgnDeleteWBCurveMap.insert(std::pair<int, CWBCurve*>(te.id,pWBCurve));
			}
			g_nDrawStatus = DRAW_RGNDELETING;
		}		
		break;
	case DRAW_CLEANUP: //TouchDown清除
		m_dwCurBrushTouchID = te.id;
		g_nDrawStatus = DRAW_CLEANUPING;
		break;
	case ADD_ANNOTATIONS:	//添加标注
		{
			m_bIsAnnotations = TRUE;
			m_rectText.left = pt.x;
			m_rectText.top = pt.y;

			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			CWBRichEditMgr::GetInst_()->SetBeginPoint(pt.x, pt.y, te.id);

			SetPageIsNeedDraw(FALSE);
			g_nDrawStatus = DRAWING_TEXT;
		}
		break;
	default:
		break;
	}
}

void CWhiteBoardView::touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te)
{
	XAutoLock lock(m_csDraw);
	////转换成客户区坐标
	CPoint pt;
	pt.x = (LONG)te.xpos;
	pt.y = (LONG)te.ypos;

	ScreenToClient(&pt);
	CClientDC dc(this);
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&pt);//将视图坐标转换为文档作标
	DWORD dwTime_difference = ::timeGetTime();
	if ( dwTime_difference - m_dwTimeGetTime >= MOVEISNEEDSENDDATA_TIME)
	{
		//需要发送同步消息
		m_dwTimeGetTime = dwTime_difference;
		m_bMoveIsNeedSendData = TRUE;
	}

	//   
	if (NULL != m_pWBDustbin)
	{
		if (m_IsMoveDustbin == TRUE)
		{
			m_DustbinPoint = pt;
			Invalidate();
		}
	}


	DWORD dwTouchID = te.id;

	//然后检验判断是否是画图操作(这里不再依赖于全局状态g_nDrawStatus)
	std::map<DWORD, MTDrawPar*>::iterator itDrawPar = m_mapMTDrawPar.find(dwTouchID);
	unsigned int nNum = m_mapMTDrawPar.size();
	if ( itDrawPar != m_mapMTDrawPar.end() )
	{
		m_MTDrawObjectTally.touchMoveHandler(te.id); //huanglongquan add
		MTDrawPar* pDrawPar = itDrawPar->second;
		int nDrawType = pDrawPar->GetType();
		switch(nDrawType)
		{
		case MIDI_DRAG:
			{
				CWBMidi *pmidi = static_cast<CWBMidi *>(pDrawPar->GetWBObject());
				if (pmidi)
				{
					if (pmidi->GetObjSelectFrame()->PointIn(pt))
					{
						SetCursor(LoadCursor(NULL,IDC_HAND));
						pmidi->ChangeShowPosition(pt);
						SetPageIsNeedDraw(TRUE);
						Invalidate();
						UpdateWindow();
						SetPageIsNeedDraw(FALSE);


						XMemIni xmlobj;
						pmidi->SetXmlValue(xmlobj);
						xmlobj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlobj);
					}
				}
			}
			break;
		case DRAW_LINE:
			{
				CWBLine *pLine = static_cast<CWBLine *>(pDrawPar->GetWBObject());
				if(pLine)
				{
					pLine->Move(pt, FALSE);
					pLine->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pLine->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case DRAW_POLYLINE:
			{
				CWBCurve *pCurve = static_cast<CWBCurve *>(pDrawPar->GetWBObject());
				if(pCurve)
				{
					UINT nPtNum = pCurve->AddPoint(pt);
					HDC hDC = ::GetDC(GetSafeHwnd());
					CPoint ptViewOffset = GetScrollPosition();
					pCurve->DrawLast(hDC, ptViewOffset.x, ptViewOffset.y,nPtNum);
					::ReleaseDC(GetSafeHwnd(),hDC);

					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						pCurve->SetResume_broken_taransfer(true);
						XMemIni xmlObj;
						pCurve->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}

					// 					if ( pCurve->IsSendToNetOfDrawingByTouch())
					// 					{
					// 						//发送曲线同步消息
					// 						pCurve->CreateSelecrtFrame();
					// 						pCurve->SetIsDraw();
					// 						pCurve->SetResume_broken_taransfer(true);
					// 						XMemIni xmlObj;
					// 						xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
					// 						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					// 						unsigned long ulEventId = pCurve->GetTouchMoveEventId();
					// 						if (ulEventId == 0)
					// 						{
					// 							ulEventId = m_dwMoveMessageEventID;
					// 						}
					// 						xmlObj.Set(KEY_EVENT_ID, ulEventId);
					// 						pCurve->SetXmlValue(xmlObj);
					// 						Broadcast_Data(xmlObj);
					// 					}
					// 					pCurve->IncrementMoveCounts();
				}
			}
			break;
		case DRAW_POLYICON:
			{
				CWBIcoCurve *pIcoCure = static_cast<CWBIcoCurve*>(pDrawPar->GetWBObject());
				if (pIcoCure != NULL)
				{
					pIcoCure->SetAlpha((int)g_nTranspar);
					CDC *pDC = GetDC();
					UINT x = g_pWhiteBoardView->getXnPos();
					UINT y = g_pWhiteBoardView->getYnPos();
					pIcoCure->DrawIcoTrack(pDC,pt,x,y);
					ReleaseDC(pDC);
					if (m_bMoveIsNeedSendData)
					{
						pIcoCure->SetResume_broken_taransfer(true);
						XMemIni xmlObj;
						xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
						pIcoCure->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						if (g_bIsWBLoadIcoFromFile)
						{
							xmlObj.Set(KEY_FILE_ID,m_dwIcoCurId);
							
						}
						Broadcast_Data(xmlObj);
						m_bMoveIsNeedSendData = FALSE;
					}
				}
			}
			break;
		case DRAW_MAGICLINE:
			{
				CWBMagicPen *pMagicPen = static_cast<CWBMagicPen*>(pDrawPar->GetWBObject());
				if (pMagicPen)
				{
					UINT nPtNum  = pMagicPen->AddPoint(pt);
					HDC hDC = ::GetDC(GetSafeHwnd());
					CPoint ptViewOffset = GetScrollPosition();
					pMagicPen->DrawLast(hDC,ptViewOffset.x,ptViewOffset.y,nPtNum);
					::ReleaseDC(GetSafeHwnd(),hDC);
					KillTimer(ID_TIEMR_MAGICPEN);

					if(g_pHPWBDataNotify && m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						pMagicPen->SetResume_broken_taransfer(true);
						XMemIni xmlObj;
						xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
						pMagicPen->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);

					}


				}
			}
			break;
		case DRAW_RECT:
			{
				CWBRectangle *pRect = static_cast<CWBRectangle *>(pDrawPar->GetWBObject());
				if(pRect)
				{
					pRect->Move(pt);
					pRect->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pRect->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case DRAW_PALLELOGAM:
			{
				CWBPallelogam *pPallelogam = static_cast<CWBPallelogam*>(pDrawPar->GetWBObject());
				if (pPallelogam)
				{
					pPallelogam->Move(pt);
					pPallelogam->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify && m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						XMemIni xmlObj;
						pPallelogam->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case EDITTRIANGLE:
			{
				break;;
				CWBArbitraryTriangle *pTriangle = static_cast<CWBArbitraryTriangle *>(pDrawPar->GetWBObject());
				pTriangle->EditTrianglePoint(pt);
				XMemIni xmlObj;					
				pTriangle->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
				Broadcast_Data(xmlObj);
				Invalidate();
			}
			break;
		case DRAWINGANYTRIANGLE:
			{
				CWBArbitraryTriangle *pTriangle = static_cast<CWBArbitraryTriangle *>(pDrawPar->GetWBObject());
				if (pTriangle != NULL)
				{
					pTriangle->Move(pt);
					pTriangle->SetAlpha((int)g_nTranspar);
					if ( m_bMoveIsNeedSendData)
					{

						XMemIni xmlObj;					
						pTriangle->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);

						m_bMoveIsNeedSendData = FALSE;
					}
					Invalidate();
				}
			}
			break;
		case DRAW_TRIANGLE: //三角形
			{
				CWBTriangle *pTriangle = static_cast<CWBTriangle *>(pDrawPar->GetWBObject());
				if(pTriangle)
				{
					pTriangle->Move(pt);
					pTriangle->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pTriangle->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case  DRAW_RHOMBUS:
			{
				CWBRhombus *pRhombus = static_cast<CWBRhombus *>(pDrawPar->GetWBObject());
				if(pRhombus)
				{
					pRhombus->Move(pt);
					pRhombus->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pRhombus->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case  DRAW_PENTAGON:
			{
				CWBPentagon *pPentagon = static_cast<CWBPentagon *>(pDrawPar->GetWBObject());
				if(pPentagon)
				{
					pPentagon->Move(pt);
					pPentagon->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pPentagon->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case  DRAW_HEXAGON:
			{
				CWBHexagon *pHexagon = static_cast<CWBHexagon *>(pDrawPar->GetWBObject());
				if(pHexagon)
				{
					pHexagon->Move(pt);
					pHexagon->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pHexagon->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case  DRAW_FIVEPOINTSTART:
			{
				CWBFivepointstart *pFivepointstart = static_cast<CWBFivepointstart *>(pDrawPar->GetWBObject());
				if(pFivepointstart)
				{
					pFivepointstart->Move(pt);
					pFivepointstart->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pFivepointstart->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case  DRAW_TRAPEZIUM:
			{
				CWBTrapezium *pTrapezium = static_cast<CWBTrapezium *>(pDrawPar->GetWBObject());
				if(pTrapezium)
				{
					pTrapezium->Move(pt);
					pTrapezium->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pTrapezium->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case DRAW_ELLIPSE:
			{
				CWBEllipse *pEllipse = static_cast<CWBEllipse *>(pDrawPar->GetWBObject());
				if(pEllipse)
				{
					pEllipse->Move(pt);
					pEllipse->SetAlpha((int)g_nTranspar);
					if (g_pHPWBDataNotify &&  m_MTDrawObjectTally.NeedTouchSynchronize(te.id))
					{
						//发送同步消息
						XMemIni xmlObj;
						pEllipse->SetXmlValue(xmlObj);
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
					}
					Invalidate();
				}
			}
			break;
		case DRAW_POLYGON: //暂时屏蔽 多边形
			{
				CWBPolygon *pPolygon = static_cast<CWBPolygon *>(pDrawPar->GetWBObject());
				if(pPolygon)
				{
					CPoint ptPrev = pDrawPar->GetPt();
					pDrawPar->UpdatePt(pt);
					CPoint ptPrevValid = pDrawPar->GetValidPt();

					DWORD dwTimePrev = pDrawPar->GetTime();
					DWORD dwTime = te.time;
					DWORD diffTime = labs(dwTime - dwTimePrev);
					pDrawPar->UpdateTime(dwTime);
					if (PtLength(pt, ptPrevValid) > 10 && PtLength(pt,ptPrev) < 2 && diffTime>50)
					{
						pPolygon->AddPoint(pt);
						//pPolygon->DeletePoint();
						pDrawPar->UpdateValidPt(pt);
						Invalidate();
					}
					else
					{
						pPolygon->Move(pt);
						pPolygon->SetAlpha((int)g_nTranspar);
						Invalidate();
					}

				}
				else
				{
					ASSERT(0);
				}
			}
			break;
		}
	}

	//处理放大镜
	if(MouseMoveMagn(pt))
	{
		return ;
	}
	if (m_bStartRecognition &&m_bRecogniTouchDown)
	{

		PointRetain point(pt.x,pt.y);
		path2D.push_back(point);
		m_RecShowpoint.push_back(pt);
		Invalidate();
		return;


	}

	switch (g_nDrawStatus)
	{
	case DRAW_RECTMAQN:
		{
			if(MouseMoveMagn(pt))
			{
				return ;
			}
		}
		break;
	case MOVING_LINE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				g_nDrawStatus = DRAW_NORMAL;
				return ;
			}
			if (pObj->IsClone())
			{
				if (!pObj->IsLock() && !pObj->IsLimit())
				{
					g_pWBPage->ClearSelectFrameList();
					g_pWBPage->AddSelectFrame(pObj->GetLayerIndex());
					CloneObject();
					pObj->SetClone(FALSE);
				}
			}
			else
			{
				CPoint ptOffset;
				ptOffset = pt - m_ptSelect;
				g_pWBPage->MoveSelectedObj(ptOffset);

				if (m_ulMoveCounts %10 == 0)
				{
					CPoint ptOffsetSend = pt - m_ptPrePoint;
					m_ptPrePoint = pt;
					SendMoveSelectedObjOffset(ptOffsetSend,GenerateSSRC());
				}

				m_ulMoveCounts ++;
				//g_pWBPage->MoveSelectedObj(m_ptSelect, point);
				m_ptSelect = pt;
			}
			if ( m_bMoveIsNeedSendData)
			{
				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
				Broadcast_Data(xmlObj);
			}

			Invalidate();
			UpdateWindow();
		}
		break;	
	case DRAWING_TEXT:		//画编辑框
		{
			SetPageIsNeedDraw(FALSE);
			m_rectText.right = pt.x;
			m_rectText.bottom = pt.y;
			Invalidate();
		}
		break;
	case DRAW_ROUND:
		break;
	case DRAW_ERASING: //TOUCHMOVE正在擦除
		{
			if (te.id != m_dwCurBrushTouchID)
			{
				break;
			}
			//CRect rectErase(CPoint(point.x-10, point.y-10), CPoint(point.x+10, point.y+10));
			m_rectBrush = CRect(CPoint(pt.x-g_nBrushSize, pt.y-g_nBrushSize), 
				CPoint(pt.x+g_nBrushSize, pt.y+g_nBrushSize));
			g_pWBPage->EraseObjects(m_rectBrush);
			Invalidate();

			int left,right,top,botton;
			left = pt.x-g_nBrushSize;
			right = pt.x+g_nBrushSize;
			top = pt.y-g_nBrushSize;
			botton = pt.y+g_nBrushSize;

			XMemIni xmlobj;
			xmlobj.Set(KEY_BROADCAST_CMD,_E_BROADE_ERASE_LINE);
			xmlobj.Set(KEY_ERASE_RECT_LEFT,left);
			xmlobj.Set(KEY_ERASE_RECT_RIGHT,right);
			xmlobj.Set(KEY_ERASE_RECT_TOP,top);
			xmlobj.Set(KEY_ERASE_RECT_BOTTON,botton);
			xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
			Broadcast_Data(xmlobj);
		}
		break;
	case DRAW_CLEANUPING: //TOUCHMOVE清除
		{
			if (te.id != m_dwCurBrushTouchID)
			{
				break;
			}
			m_rectBrush = CRect(CPoint(pt.x-40, pt.y-40), CPoint(pt.x+40, pt.y+40));
			DWORD dwDelCount;
			dwDelCount = g_pWBPage->DeleteWBObjects(m_rectBrush);
			if (dwDelCount > 0)
			{
				SetPageIsNeedDraw(TRUE);
			}
			DeleteObjMsg();
			Invalidate();
		}
		break;
	case DRAW_RGNDELETING: //TOUCHMOVE区域删除
		{
			std::map<int, CWBCurve*>::iterator iter = m_pRgnDeleteWBCurveMap.find(te.id);
			if (iter != m_pRgnDeleteWBCurveMap.end())
			{
				CWBCurve* pWBCurve = iter->second;
				UINT nPtNum = pWBCurve->AddPoint(pt);//wangxiaobin

				HDC hDC = ::GetDC(GetSafeHwnd());
				CPoint ptViewOffset = GetScrollPosition();
				pWBCurve->DrawLast(hDC, ptViewOffset.x, ptViewOffset.y, nPtNum);
				::ReleaseDC(GetSafeHwnd(),hDC);	
			}
		}
		break;

	case ADDDICOM:
		{
			int Dicom_Return = 0;

			//清除所有创建的选择框
			g_pWBPage->ClearSelectFrameList();

			//DICOM中的初始化
			CString strFilePath;
			strFilePath = g_pWhiteBoardView->GetResFilePath();
			char BMPFileName[512] = "";

			if (!PathFileExists(g_pWhiteBoardView->GetResFilePath()))
			{
				g_nDrawStatus = DRAW_NORMAL;
				break;
			}

			pDicomToBmpDll GetDicom;
			GetDicom = (pDicomToBmpDll)GetProcAddress(g_IsMedicalVersion,"DicomToBmpDll");
			if (GetDicom == NULL)
			{
				return;
			}

			if (GetDicom)
				Dicom_Return = GetDicom(strFilePath,g_strAvconTempPath);

			if(Dicom_Return == DICOM_TOBMP_SUCCEES)
			{
				//把转换后的文件名传给strfilepath，以便CWBPicture调用
				GetBmpPath GetDicomBmpPath;
				GetDicomBmpPath = (GetBmpPath)GetProcAddress(g_IsMedicalVersion,"GetDicomBmpPath");
				if (GetDicomBmpPath == NULL)
				{
					return;
				}
				if (GetDicomBmpPath)
				{
					if ( !GetDicomBmpPath(BMPFileName,512))
					{
						break;;
					}
					strFilePath = BMPFileName;
				}		
			}

			//创建一个新对象
			CWBPicture *pWBPicture = new CWBPicture(strFilePath);			
			pWBPicture->SetAlpha((int)g_nTranspar);
			//控制图片的最大不能超出view的大小4-21金鹏
			CRect viewRect;
			GetClientRect(&viewRect);
			pWBPicture->SetPictureBigerSzie(viewRect);
			g_pWBPage->AddWBObject(pWBPicture);
			CPoint ptCenter,ptOffset;
			//获得对象中心点
			pWBPicture->GetCenterPoint(ptCenter);
			m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
			}
			ptOffset = pt- ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
			m_ptSelect = pt;
			Invalidate();			
			//发送同步消息
			XMemIni xmlObj;
			pWBPicture->SetXmlValue(xmlObj);
			AddObj(xmlObj);
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;    
	case CHANGE_ANNOTATIONS:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurAnnotationsLayerIndex);
			if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档
			{
				m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject *pObject = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
				if (pObject != NULL)
				{
					if (pObject->IsLock() || pObject->IsLimit())
					{
						break;
					}
					if (pObject->GetType() == WB_OBJ_DOCUMENT)
					{
						pObj = ((CWBDocument *)pObject)->GetAnn();
					}
					if (pObject->GetType() == WB_OBJ_PICTURE)
					{
						pObj = ((CWBPicture *)pObject)->GetAnn();
					}
					if (pObject->GetType() == WB_OBJ_MIDI)
					{
						pObj = ((CWBMidi*)pObject)->GetAnn();
					}
				}
			}
			if (pObj != NULL)
			{
				CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;

				if (!pAnnotations->IsLock())
				{
					BOOL bOut = FALSE;
					m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObject = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
					CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
					if (pObject->GetType() == WB_OBJ_DOCUMENT)
					{
						pObj = ((CWBDocument *)pObject)->GetAnn();
						//if (((CWBDocument *)pObject)->ObjectIsComponent(pObj))
						if (((CWBDocument *)pObject)->PtIsComponent(pt))
						{
							bOut =TRUE;
						}
					}
					else if (pObject->GetType() == WB_OBJ_PICTURE)
					{
						pObj = ((CWBPicture *)pObject)->GetAnn();
						if (((CWBPicture *)pObject)->PtIsComponent(pt))
						{
							bOut =TRUE;
						}
					}
					else if (pObject->GetType() == WB_OBJ_MIDI)
					{
						pObj = ((CWBMidi*)pObject)->GetAnn();
						if (((CWBMidi*)pObject)->PtIsComponent(pt))
						{
							int nheight = 0;
							nheight = ((CWBMidi*)pObject)->GetCopyView();
							pAnnotations->SetIsChange(TRUE);

							pAnnotations->SetChangePoint(CPoint(pt.x,pt.y+nheight));
							((CWBMidi*)pObject)->m_bSetZoomMidi = true;
							((CWBMidi*)pObject)->m_bshowCompont = true;
							((CWBMidi*)pObject)->SetBneedDraw(FALSE);
						}
					}
					else
					{
						bOut =TRUE;
					}
					if (bOut)
					{
						/*CPoint pt = CPoint(10,10);
						point -= pt;*/
						pAnnotations->SetIsChange(TRUE);
						pAnnotations->SetChangePoint(pt);
					} 

					if ( m_bMoveIsNeedSendData)
					{
						//发送同步消息
						XMemIni xmlObj;
						CWBObject *pDoc = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
						if(pDoc != NULL)
						{
						    pDoc->SetXmlValue(xmlObj);
						}				
						xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
						Broadcast_Data(xmlObj);
						/*
						if (pAnnotations->GetbSendAddCmd())
						{
						AddObj(xmlObj);
						pAnnotations->SetbSendAddCmd(false);
						}
						else
						{
						ChangeObj(xmlObj);
						}
						*/
						m_bMoveIsNeedSendData = FALSE;
					}
					Invalidate();
				}	
			}

		}
		break;
		//case DRAWED_TEXT:
		//	SetCursor(LoadCursor(NULL, IDC_ARROW));
		//	break;
	default:
		break;
	}
}

void CWhiteBoardView::touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te)
{
	XAutoLock lock(m_csDraw);
	////转换成客户区坐标
	CPoint pt;
	pt.x = (LONG)te.xpos;
	pt.y = (LONG)te.ypos;

	ScreenToClient(&pt);
	CClientDC dc(this);
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&pt);//将视图坐标转换为文档作标

	DWORD dwTouchID = te.id;
	
	//然后检验判断是否是画图操作(这里不再依赖于全局状态g_nDrawStatus)
	std::map<DWORD, MTDrawPar*>::iterator itDrawPar = m_mapMTDrawPar.find(dwTouchID);
	if ( itDrawPar != m_mapMTDrawPar.end() )
	{
		m_MTDrawObjectTally.touchUpHandler(te.id);	//huanglongquan add

		MTDrawPar* pDrawPar = itDrawPar->second;
		int nDrawType = pDrawPar->GetType(); 
		switch(nDrawType)
		{
		case MIDI_DRAG:
			{
				CWBMidi *pwbmidi = static_cast<CWBMidi *>(pDrawPar->GetWBObject());
				if (pwbmidi != NULL)
				{
					if (pwbmidi->GetObjSelectFrame()->PointIn(pt))
					{
						SetCursor(LoadCursor(NULL,IDC_ARROW));
						pwbmidi->ChangeShowPosition(pt);					
					}

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					UpdateWindow();
					SetPageIsNeedDraw(FALSE);
					XMemIni xmlObj;
					pwbmidi->SetXmlValue(xmlObj);
					ChangeObj(xmlObj);
				}
				g_nDrawStatus = DRAW_NORMAL;
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_LINE:
			{
				CWBLine *pLine = static_cast<CWBLine *>(pDrawPar->GetWBObject());
				if(pLine)
				{
					if (CalDistancePointToPoint(pt,m_ptPointLineCurve) <5.0)
					{
						pt.x += 5;
					}
					pLine->Move(pt, FALSE);

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pLine->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_POLYLINE:
			{
				CWBCurve *pCurve = static_cast<CWBCurve *>(pDrawPar->GetWBObject());
				if(pCurve)
				{
					if (CalDistancePointToPoint(pt,m_ptPointLineCurve) <5.0)
					{
						pt.x += 5;
					}
					pCurve->SetAlpha((int)g_nTranspar);
					pCurve->AddPoint(pt);
					pCurve->SetIsDraw();	
					pCurve->SetTouchID(0); 
					pCurve->CreateSelecrtFrame();
					pCurve->SetResume_broken_taransfer(false); //必须放在发送同步消息之前，不然undo的时候会出错

					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pCurve->SetXmlValue(xmlObj);
					AddObj(xmlObj);
					
					//判断是否添加到文档里
					if (!AddComponentToObj(pCurve, m_MTDrawObjectTally.GenerateEvent()))
					{
						//当所有正在绘制曲线的手指都弹起时，执行刷新，同步操作
						if( 1 == GetMapSizeOfSpecialDrawTypeObj(nDrawType) )
						{
							SetPageIsNeedDraw(TRUE);
							Invalidate();
						}			
					}
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_POLYICON:
			{
				CWBIcoCurve *pIcoCure = static_cast<CWBIcoCurve*>(pDrawPar->GetWBObject());
				if (pIcoCure != NULL)
				{
					pIcoCure->SetAlpha((int)g_nTranspar);
					pIcoCure->AdjustObject();
					pIcoCure->CreateSelecrtFrame();
					pIcoCure->SetIsRefush(TRUE);
					SetPageIsNeedDraw(TRUE);
					Invalidate();
					pIcoCure->SetResume_broken_taransfer(true);
					XMemIni xmlObj;
					xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
					if(g_bIsWBLoadIcoFromFile)
					{
						xmlObj.Set(KEY_EVENT_ID,m_ulIcoCurEvent);
						xmlObj.Set(KEY_FILE_ID,m_dwIcoCurId);
					}
					pIcoCure->SetXmlValue(xmlObj);
					AddObj(xmlObj);

				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_MAGICLINE:
			{
				CWBMagicPen* pMagicPen = static_cast<CWBMagicPen*>(pDrawPar->GetWBObject());
				if (pMagicPen != NULL)
				{
					if(CalDistancePointToPoint(pt,m_ptPointLineCurve)<5)
					{
						pt.x += 5;

					}
					pMagicPen->SetAlpha((int)g_nTranspar);
					pMagicPen->AddPoint(pt);
					pMagicPen->AdjustObject();
					pMagicPen->SetIsDraw();
					pMagicPen->CreateSelecrtFrame();
					pMagicPen->SetResume_broken_taransfer(true);
					SetPageIsNeedDraw(TRUE);
					Invalidate();
					UpdateWindow();
					
					XMemIni xmlObj;
					xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_NONE);
					pMagicPen->SetXmlValue(xmlObj);
					AddObj(xmlObj);
					g_pWBPage->DelWBObj(pMagicPen->GetLayerIndex());
					(g_pWBPage->m_pMagicList).push_back(pMagicPen);
					SetTimer(ID_TIEMR_MAGICPEN,1000,NULL);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case EDITTRIANGLE:
			{
				break;
				CWBArbitraryTriangle *pTriangle = static_cast<CWBArbitraryTriangle *>(pDrawPar->GetWBObject());
				if (pTriangle != NULL)
				{
					pTriangle->EditTrianglePoint(pt,true);	
					pTriangle->AdjustObject();
					pTriangle->SetIsEdit(false);
					SetPageIsNeedDraw(TRUE);
					Invalidate();

					//发送同步消息
					XMemIni xmlObj;
					pTriangle->SetXmlValue(xmlObj);
					ChangeObj(xmlObj);
				}                    
				g_nDrawStatus = DRAW_NORMAL;
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAWINGANYTRIANGLE:
			{
				CWBArbitraryTriangle *pTriangle = static_cast<CWBArbitraryTriangle *>(pDrawPar->GetWBObject());
				if (pTriangle != NULL)
				{
					pTriangle->Move(pt);	
					if (pTriangle->CanbeTriangle())
					{
						pTriangle->CreateSelecrtFrame();
					}
					else
					{
						pTriangle->Move(pt,CPoint(pt.x +30,pt.y+100),CPoint(pt.x+100,pt.y+100));
						pTriangle->CreateSelecrtFrame();
					}
					pTriangle->AdjustObject();

					SetPageIsNeedDraw(TRUE);
					Invalidate();

					//发送同步消息
					XMemIni xmlObj;
					pTriangle->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}                               
				g_nDrawStatus = DRAWANYTRIANGLE;
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case  DRAW_TRIANGLE:
			{
				CWBTriangle *pTriangle = static_cast<CWBTriangle *>(pDrawPar->GetWBObject());
				if(pTriangle)
				{
					pTriangle->Move(pt);
					pTriangle->AdjustObject();

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pTriangle->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_RECT:
			{
				CWBRectangle *pRect = static_cast<CWBRectangle *>(pDrawPar->GetWBObject());
				if(pRect)
				{
					pRect->Move(pt);
					pRect->AdjustObject();
					pRect->SetTouchID(0);

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pRect->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_PALLELOGAM:
			{
				CWBPallelogam *pPallelogam = static_cast<CWBPallelogam*>(pDrawPar->GetWBObject());
				if (pPallelogam)
				{
					pPallelogam->Move(pt);
					pPallelogam->AdjustObject();
					pPallelogam->SetTouchID(0);
					SetPageIsNeedDraw(TRUE);
					Invalidate();

					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pPallelogam->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_ELLIPSE:
			{
				CWBEllipse *pEllipse = static_cast<CWBEllipse *>(pDrawPar->GetWBObject());
				if(pEllipse)
				{
					pEllipse->Move(pt);	
					pEllipse->AdjustObject();
					pEllipse->SetTouchID(0);

					SetPageIsNeedDraw(TRUE);
					Invalidate();

					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pEllipse->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_RHOMBUS:
			{
				CWBRhombus *pRhombus = static_cast<CWBRhombus *>(pDrawPar->GetWBObject());
				if(pRhombus)
				{
					pRhombus->Move(pt);
					pRhombus->AdjustObject();
					pRhombus->SetTouchID(0);

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pRhombus->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_PENTAGON:
			{
				CWBPentagon *pPentagon = static_cast<CWBPentagon *>(pDrawPar->GetWBObject());
				if(pPentagon)
				{
					pPentagon->Move(pt);
					pPentagon->AdjustObject();
					pPentagon->SetTouchID(0);

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pPentagon->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_HEXAGON:
			{
				CWBHexagon *pHexagon = static_cast<CWBHexagon *>(pDrawPar->GetWBObject());
				if(pHexagon)
				{
					pHexagon->Move(pt);
					pHexagon->AdjustObject();
					pHexagon->SetTouchID(0);

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pHexagon->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_FIVEPOINTSTART:
			{
				CWBFivepointstart *pFivepointstart = static_cast<CWBFivepointstart *>(pDrawPar->GetWBObject());
				if(pFivepointstart)
				{
					pFivepointstart->Move(pt);
					pFivepointstart->AdjustObject();
					pFivepointstart->SetTouchID(0);

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pFivepointstart->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_TRAPEZIUM:
			{
				CWBTrapezium *pTrapezium = static_cast<CWBTrapezium *>(pDrawPar->GetWBObject());
				if(pTrapezium)
				{
					pTrapezium->Move(pt);
					pTrapezium->AdjustObject();
					pTrapezium->SetTouchID(0);

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pTrapezium->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		case DRAW_POLYGON:
			{
				CWBPolygon *pPolygon = static_cast<CWBPolygon *>(pDrawPar->GetWBObject());
				if(pPolygon)
				{
					//如果点在起始点附近，直接close，否则，添加一个点，然后close
					pPolygon->SetClosed();

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					//发送同步消息
					XMemIni xmlObj;
					xmlObj.Set(KEY_EVENT_ID,m_MTDrawObjectTally.GenerateEvent());
					pPolygon->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
				delete pDrawPar;
				m_mapMTDrawPar.erase(itDrawPar);
			}
			break;
		}

	}
	//处理放大镜
	if(MouseUpMagn(pt))
	{
		return ;
	}
	if (m_bStartRecognition)
	{
		m_bRecogniTouchDown = false;
		if (m_pGeoRecognizer == NULL)
		{
			m_pGeoRecognizer = new GeometricRecognizer;
			m_pGeoRecognizer->loadTemplates();
		}
		if (path2D.size() <10)
		{
			path2D.clear();
			m_RecShowpoint.clear();
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
			return;
		}

		RecognitionResult recsult  = m_pGeoRecognizer->recognize(path2D);
		string name;
		name = recsult.name;
		path2D.clear();
		PointRectangle box = m_pGeoRecognizer->getboundingbox();
		double x;
		double y;
		double width;
		double height;
		x = box.RectangleX;
		y = box.RectangleY;
		width = box.RectangleWidth;
		height = box.RectangleHeight;

		if (name == "Triangle")
		{
			CPoint pt = m_RecShowpoint.front();									
			CPoint pt1(x,y+height);						
			CPoint pt2(x+width,y+height);
			for (int i =0;i<m_RecShowpoint.size();++i)
			{
				CPoint temp = m_RecShowpoint[i];
				if (temp.x == pt2.x)
				{
					pt2.y =temp.y;
				}
			}
			if (pt.x > pt1.x)
			{
				CPoint pttemp;
				pttemp = pt;
				pt= pt1;
				pt1 =pttemp;
			}
			CWBArbitraryTriangle *pTriangle = new CWBArbitraryTriangle(pt,pt1,pt2);
			pTriangle->SetAlpha((int)g_nTranspar);
			pTriangle->CreateSelecrtFrame();
			g_pWBPage->AddWBObject(pTriangle);
			m_RecShowpoint.clear();
			
		}
		if (name == "Circle")
		{
			m_RecShowpoint.clear();
			CWBEllipse *pEllipse = new CWBEllipse(CPoint(x,y),CPoint(x+width,y+height));
			pEllipse->SetAlpha((int)g_nTranspar);
			pEllipse->CreateSelecrtFrame();
			g_pWBPage->AddWBObject(pEllipse);
			

		}
		if (name == "Rectangle")
		{
			m_RecShowpoint.clear();
			CWBRectangle *pRectangle = new CWBRectangle(CPoint(x,y),CPoint(x+width,y+height));
			pRectangle->SetAlpha((int)g_nTranspar);
			pRectangle->CreateSelecrtFrame();
			g_pWBPage->AddWBObject(pRectangle);
			
		}					
		if (name == "Star")
		{
			m_RecShowpoint.clear();
			CWBFivepointstart *pFiveStart = new CWBFivepointstart(CPoint(x,y),CPoint(x+width,y+height));
			pFiveStart->SetAlpha((int)g_nTranspar);
			pFiveStart->CreateSelecrtFrame();
			g_pWBPage->AddWBObject(pFiveStart);
			
		}	

		m_RecShowpoint.clear();
		SetPageIsNeedDraw(TRUE);
		Invalidate();
		UpdateWindow();
		SetPageIsNeedDraw(FALSE);
		return;
	}
	switch (g_nDrawStatus)
	{
	case DRAW_RECTMAQN:
		{
			if(MouseUpMagn(pt))
			{
				return ;
			}
		}
		break;
	case MOVING_LINE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj != NULL)
			{
				pObj->SetClone(FALSE);
			}

			//denghong1227移动直线偏移量
			CPoint moveOffset = pt - m_moveFirstPoint;

			g_nDrawStatus = DRAW_NORMAL;

			AddComListToObj();

			MoveMsg(moveOffset);

			g_nDrawStatus = DRAW_NORMAL;

			m_ulMoveCounts = 0;

			///画对象被垃圾桶删除时的动画
			//CWBDustbin *pWBDustbin = g_pWhiteBoardView->GetDustbin();
			CPoint point((int)te.xpos, (int)te.ypos);
			::ScreenToClient(m_hWnd, &point);
			CClientDC dc(this);
			OnPrepareDC(&dc);//进行坐标原点的匹配
			dc.DPtoLP(&point);//将视图坐标转换为文档作标
			if (NULL != m_pWBDustbin) //假如创建了垃圾箱
			{
				int mousepos = m_pWBDustbin->PointIn(point);
				if (MOUSE_OUTSIDE != mousepos)
				{
					CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
					CWBObject *pObj = NULL;
					int iSelectObjCounts = pSelectObjLst->GetSize();
					POSITION pos;

					CPoint ptDustbin;
					CRect rect;
					CPoint *pptCenter = new CPoint[iSelectObjCounts];
					CPoint *pptOffset = new CPoint[iSelectObjCounts];
					m_pWBDustbin->GetRect(rect);
					m_pWBDustbin->GetCenterPoint(ptDustbin);
					ptDustbin.y = rect.top;
					for (int i = 0; i < iSelectObjCounts; ++i)
					{
						pos = pSelectObjLst->FindIndex(i);
						pObj = pSelectObjLst->GetAt(pos);
						if (pObj == NULL)
						{
							continue;
						}
						pObj->GetCenterPoint(pptCenter[i]);
						pptOffset[i] = ptDustbin - pptCenter[i];
					}
					if (!(iSelectObjCounts == 1 && pObj->GetType() == WB_OBJ_COMBINE))
					{
						int alpha = 255/2;
						int index = 0;
						int stepNumber = 10;
						int initialAlpha = alpha;
						double offsetDescRate[10] = {-0.2, -0.2, -0.2, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, -1.1};
						double zoomRate[10]       = {1.1 , 1.1 , 1.1 , 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 1.0 };
						while (index < stepNumber)
						{
							for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
							{
								pos = pSelectObjLst->FindIndex(i);
								if (pos == NULL)
								{
									continue;
								}
								pObj = pSelectObjLst->GetAt(pos);
								if (pObj == NULL)
								{
									continue;
								}
								if (pObj->IsLock() || pObj->IsLimit())
								{
									continue;
								}
								else
								{
									if (pObj->GetType() == WB_OBJ_ICOCURVE)
									{
										((CWBIcoCurve*)pObj)->SetIsRefush(TRUE);
									}
									if (pObj->GetType() == WB_OBJ_CURVE)
									{
										((CWBCurve*)pObj)->SetIsDraw();
									}
									if (pObj->GetType() == WB_OBJ_PICTURE)
									{
										((CWBPicture*)pObj)->SetIsPath();
									}

									if (pObj->GetType() != WB_OBJ_COMBINE)
									{
										if (pObj->GetType() == WB_OBJ_DOCUMENT)
										{
											CWBObject *pDocCom = ((CWBDocument *)pObj)->GetSelectComObj();
											if (pDocCom != NULL)
											{
												pDocCom->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
												CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
												pDocCom->MoveOffset(ptOnce);
												if ((int)offsetDescRate[index] > 0)
												{
													pDocCom->RotateByCenter(60);
												}
												pDocCom->SetAlpha(alpha);
												if (pDocCom->GetType() == WB_OBJ_CURVE)
												{
													((CWBCurve*)pDocCom)->SetIsDraw();
												}
											}
											else
											{
												if ((int)offsetDescRate[index] > 0)
												{
													((CWBDocument *)pObj)->GetSrcAnnPt();

													//当对象缩放且旋转后，因坐标系变化原因会产生偏移，在这里纠正偏移
													if (pObj->GetIsZoom() && pObj->GetRotationAngle() != 0)
													{
														CPoint pt;
														CWBSelectRectangle *pWBSelectRectangle = pObj->GetObjSelectFrame();
														CPoint ptRotationCenter;
														pObj->GetCenterPoint(ptRotationCenter);
														pWBSelectRectangle->GetMapCenter(pt);
														pt -= ptRotationCenter;
														//使对象中心点偏移                                  
														ptRotationCenter += pt;
														pObj->RotateByPoint(ptRotationCenter,0);
														//移动对象
														pObj->MoveOffset(pt);

														pObj->SetIsZoom(FALSE);
													}

													((CWBDocument *)pObj)->GetSrcCenterPt();
													pObj->GetCenterPoint(g_ptDocRotCenter);
													pObj->RotateByCenter(60);
												}
												pObj->SetAlpha(alpha);
												pObj->SetSelectFrameShow(FALSE);
											}
										} 
										else if (pObj->GetType() == WB_OBJ_PICTURE && ((CWBPicture *)pObj)->GetIsPrintScreen())
										{
											CWBObject *pPicCom = ((CWBPicture *)pObj)->GetSelectComObj();
											if (pPicCom != NULL)
											{
												pPicCom->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
												CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
												pPicCom->MoveOffset(ptOnce);
												if ((int)offsetDescRate[index] > 0)
												{
													pPicCom->RotateByCenter(60);
												}
												pPicCom->SetAlpha(alpha);
												if (pPicCom->GetType() == WB_OBJ_CURVE)
												{
													((CWBCurve*)pPicCom)->SetIsDraw();
												}
											}
											else
											{
												if ((int)offsetDescRate[index] > 0)
												{
													((CWBPicture *)pObj)->GetSrcAnnPt();

													//当对象缩放且旋转后，因坐标系变化原因会产生偏移，在这里纠正偏移
													if (pObj->GetIsZoom() && pObj->GetRotationAngle() != 0)
													{
														CPoint pt;
														CWBSelectRectangle *pWBSelectRectangle = pObj->GetObjSelectFrame();
														CPoint ptRotationCenter;
														pObj->GetCenterPoint(ptRotationCenter);
														pWBSelectRectangle->GetMapCenter(pt);
														pt -= ptRotationCenter;
														//使对象中心点偏移                                  
														ptRotationCenter += pt;
														pObj->RotateByPoint(ptRotationCenter,0);
														//移动对象
														pObj->MoveOffset(pt);

														pObj->SetIsZoom(FALSE);
													}

													((CWBPicture *)pObj)->GetSrcCenterPt();
													pObj->GetCenterPoint(g_ptDocRotCenter);
													pObj->RotateByCenter(60);
												}
												pObj->SetAlpha(alpha);
												pObj->SetSelectFrameShow(FALSE);
											}
										}
										else
										{
											pObj->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
											CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
											pObj->MoveOffset(ptOnce);
											if (offsetDescRate[index] > 0)
											{
												pObj->RotateByCenter(60);
											}
											pObj->SetAlpha(alpha);
											pObj->SetSelectFrameShow(FALSE);
										}
									}
								}
							}
							if (stepNumber > 1)
							{
								if (index != stepNumber - 1)
								{
									alpha -= (initialAlpha / 2) / (stepNumber - 1);
								}
							}
							index = index + 1;
							Invalidate();
							UpdateWindow();
							Sleep(3);
						}
					}

					delete [] pptCenter;
					delete [] pptOffset;
				}
			}
		}
		break;
	case DRAW_NORMAL:
		break;
	case DRAW_ROUND:
		break;
	case DRAWEUP_TEXT:
		{
			Invalidate();
			g_nDrawStatus = DRAW_NORMAL;	
		}
		break;
	case DRAWING_TEXT:
		{
			if (!CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				unsigned long nTouchID = te.id;
				PostMessage( UWM_CREATE_RICHEDIT, (WPARAM)(nTouchID), (LPARAM)(MAKELONG(pt.x, pt.y)) );

				SetPageIsNeedDraw(TRUE);

				Invalidate();
			}
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			g_nDrawStatus = DRAWED_TEXT;
		}
		break;
	case DRAW_ERASING: //擦除
		{
			if (te.id != m_dwCurBrushTouchID)
			{
				break;
			}
			SetPageIsNeedDraw(TRUE);
			g_nDrawStatus = DRAW_ERASE;
			Invalidate();

			//denghong1227
			//发送同步信息

			//设置发送事件ID
			m_ulSendEventID = GenerateSSRC();

			XMemIni xmlobj;
			xmlobj.Set(KEY_BROADCAST_CMD,_E_BROAD_ERASE_UP);
			xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
			Broadcast_Data(xmlobj);

			EraseLineMsg(); //擦除直线协同操作

			for (int i = 0; i < (int)m_vecLineLayerIndexBeforeErase.size(); ++i )
			{
				if (m_vecLineLayerIndexBeforeErase[i] != NULL)
				{
					delete m_vecLineLayerIndexBeforeErase[i];
				}
			}
			m_vecLineLayerIndexBeforeErase.clear();

			EraseCurveMsg();//擦除曲线协同操作
			EraseDocMsg(); //擦除文档中的曲线同步

			//在同步时让接收方及时刷新，额外发送一个消息

		}
		break;
	case DRAW_CLEANUPING:
		{
// 			if (te.id != m_dwCurBrushTouchID)
// 			{
// 				break;
// 			}
// 
// 			//发送同步信息
// 			DeleteObjMsg();
// 			SetPageIsNeedDraw(TRUE);
// 			Invalidate();
			g_nDrawStatus = DRAW_CLEANUP;				
		}			
		break;
	case DRAW_RGNDELETING: //TOUCHUP区域删除
		{
			std::map<int, CWBCurve*>::iterator iter = m_pRgnDeleteWBCurveMap.find(te.id);
			if (iter != m_pRgnDeleteWBCurveMap.end())
			{
				CWBCurve* pWBCurve = iter->second;

				pWBCurve->AddPoint(pt);

				CRgn *pRgn = new CRgn();
				if (pWBCurve->CreateRgn(pRgn))
				{
					g_pWBPage->DeleteWBObjects(pRgn);
				}
				delete pRgn;

				delete pWBCurve;
				pWBCurve = NULL;
				m_pRgnDeleteWBCurveMap.erase(iter);

				//发送同步信息
				DeleteObjMsg();
				EraseDocMsg(); //擦除文档中的曲线同步
				Invalidate();				
			}

			if (m_pRgnDeleteWBCurveMap.size() == 0)
			{
				g_nDrawStatus = DRAW_RGNDELETE;
			}
		}
		break;
	case CHANGE_ANNOTATIONS:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurAnnotationsLayerIndex);
			if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档
			{
				m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject *pDoc = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
				if (pDoc != NULL)
				{
					if (pDoc->GetType() == WB_OBJ_DOCUMENT)
					{
						pObj = pDoc;
					}
				}
			}
			if (pObj != NULL)
			{
				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
				Invalidate();
			}
			g_nDrawStatus = DRAW_NORMAL;

		}
		break;
	case  MOVE_DUSTBIN:	//移动垃圾箱结束  	
		{
			if (NULL != m_pWBDustbin)
			{
				ReleaseCapture();//释放当前文档对象对鼠标的捕获，SetCapture()函数获取对鼠标的捕获
				m_IsMoveDustbin = FALSE;
				g_nDrawStatus = m_nDrawStatus;
				Invalidate();
				break;
			}
		}	
		break;   
	default:
		break;
	}

	return ;
}

/*************************************************************************
*                        CWhiteBoardView::OnPaint()                      *
*                           绘制所有已完成的笔画                         *
*************************************************************************/
void CWhiteBoardView::OnPaint()
{
	if (m_bLimitCursorArea)
	{
		CRect WBDlgRect;
		::GetWindowRect(this->m_hWnd,&WBDlgRect);
		int x,y;
		x = ::GetSystemMetrics(SM_CXVSCROLL);
		y = ::GetSystemMetrics(SM_CYHSCROLL);
		WBDlgRect.left = WBDlgRect.left + 2;
		WBDlgRect.top = WBDlgRect.top + 2;
		WBDlgRect.right = WBDlgRect.right - x - 2;
		WBDlgRect.bottom = WBDlgRect.bottom - y - 2;
		CPoint ptcursor;
		GetCursorPos(&ptcursor);
		if (g_bSupportI7)
		{
			WBDlgRect.right -= 5;
			WBDlgRect.bottom -= 7;
		}
		if (WBDlgRect.PtInRect(ptcursor))
		{
			ClipCursor(&WBDlgRect);	
		}
		else
		{
			ClipCursor(NULL);
			m_bLimitCursorArea = false;
			if (NULL != m_pWBDustbin && m_IsMoveDustbin == TRUE)
			{
				m_IsMoveDustbin = FALSE;
				g_nDrawStatus = DRAW_NORMAL;
			}
		}
	}
	//XAutoLock lock(m_csOnWbDeleteObjLock);
	if (!m_viewIsNeedDraw)//如果view不需要刷新则直接返回
	{
		m_viewIsNeedDraw = TRUE;
		return;
	}	
	
	CPaintDC dc(this); // device context for painting
	OnPrepareDC(&dc);
	OnDraw(&dc);

}

BOOL CWhiteBoardView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

/*************************************************************************
*                    CWhiteBoardView::OnMouseDown()                      *
*                 按下鼠标事件，根据m_draw的状态进行操作                 *
*************************************************************************/
void CWhiteBoardView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bYunNanDeviceSupported && !g_bAuthorizedByHandWare)//若是云南公安的版本没有授权直接返回
		return;
	if (g_bAvconLock) 
		return;
	//发送关闭子工具条的消息,需要放在桌面绘图返回之前
	::PostMessage(m_hTOOLBARWnd,WM_CLOSE_TOOBAR_MES,0,0);
	if ((m_pWBFunctionDlg != NULL)&&(m_pWBFunctionDlg->GetAttributeDlg() != NULL))
	{
		//if (m_pWBFunctionDlg->GetAttributeDlg()->GetIsEditintText())
		//{
		//	m_pWBFunctionDlg->GetAttributeDlg()->OnEnKillfocusAttributeEdbox();
		//}
		::PostMessage(m_pWBFunctionDlg->GetAttributeDlg()->m_hWnd,WM_CLOSE_TOOBAR_MES,0,0);
	}
	if(g_bIsDrawScreen) //桌面绘图打开时屏蔽白板鼠标事件
		return;

	SetPrePage(0); //翻页后，如果不是Move或者UP状态，就把m_nPrePage的值设为0
	if (m_waterdlg) 
	{
		m_waterdlg->ShowWindow(SW_HIDE);
	}

	if (g_systemVersion >= 6 && CScreenDrawMgr::GetIns() != NULL)
	{
		//将mousedlg放在screendrawdlg的下面
		CScreenDrawMgr::GetIns()->SetMouseDlgWindowPos();
	}

	if(((CWhiteBoardDlg *)g_whiteboardDlg)->GetToolbarDlg())
	{
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetToolbarDlg()->CloseAllChildBox();
	}
	
	BOOL bIgnoreTouchOrMouse = FALSE;
	m_dwTimeGetTime = ::timeGetTime();

	if (nFlags & MK_WB_ULBUTTON)
	{
		bIgnoreTouchOrMouse = TRUE; //继续往下执行，绕开当前消息是触摸屏消息的判断
	}
	else if((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
	{
		return;
	}

	CClientDC dc(this);
	if(g_reduce>0.00000000000000000)
	{

		ChangeViewCoordinate(&dc);

	}
	else
	{
	   OnPrepareDC(&dc);
	   //dc.DPtoLP(&point);
	}
	dc.DPtoLP(&point);//将视图坐标转换为文档作标

	m_ptDownPartRefresh = point; 
	m_ptMovePartRefresh = point;
	m_PreRectRefresh = CRect(0, 0, 0, 0);
	m_rectRefresh   = CRect(0, 0, 0, 0);
	//处理放大镜
	if(MouseDownMagn(point))
	{
		return;
	}
	if (m_bStartRecognition)
	{		
		PointRetain pt(point.x,point.y);
		path2D.push_back(pt);
		m_bRecogniMouseDown = true;
		m_RecShowpoint.push_back(point);
		Invalidate();
		return;		
		
	}
	if (NULL != m_pWBDustbin) //假如创建了垃圾箱
	{
		int mousepos = m_pWBDustbin->PointIn(point);
		m_PreMousePos = mousepos;//add by xiewenxiu 20111201
		if (MOUSE_OUTSIDE != mousepos)
		{
			m_DustbinPoint = m_ptPreDustbinPoint = point;
			m_IsMoveDustbin = TRUE;
			m_nDrawStatus = g_nDrawStatus;
			g_nDrawStatus = MOVE_DUSTBIN;
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 如果正在画线或擦除，添加鼠标钩子，监测鼠标左键单击事件
	// 修复移出view区域释放鼠标后收不到UP消息等相关bug fix by hxl 
	if( (DRAW_LINE       <= g_nDrawStatus && g_nDrawStatus <=DRAWING_POLYLINE) || 
		(DRAW_RECT       <= g_nDrawStatus && g_nDrawStatus <=DRAW_RGNDELETING) ||
		(DRAWANYTRIANGLE <= g_nDrawStatus && g_nDrawStatus <=MIDI_DRAGING)     ||
		 DRAW_NORMAL     == g_nDrawStatus )
	{
			InstallMouseHook(m_hWnd);
	}
	//////////////////////////////////////////////////////////////////////////
	switch(g_nDrawStatus)
	{	
	case MIDI_DRAG:
		{
			CWBMidi* pObj = (CWBMidi*)g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj->GetType() == WB_OBJ_MIDI )
			{

				((CWBMidi*)pObj)->DownChangePosition(point);
				g_nDrawStatus = MIDI_DRAGING;
			}
			else
			{
				g_nDrawStatus = MOVING_LINE;
			}
		}
		break;
	case DRAWANYTRIANGLE:
		{		
			CWBArbitraryTriangle *pTriangle = new CWBArbitraryTriangle(point,CPoint(point.x+30,point.y+100),CPoint(point.x+100,point.y+100));
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTriangle);
			pTriangle->SetAlpha((int)g_nTranspar);
			g_nDrawStatus = DRAWINGANYTRIANGLE;
 		}
		break;
	case DRAW_LINE:	
		{
			CWBLine *pLine = new CWBLine(point, point);
			m_ptPointLineCurve = point;
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pLine);
			pLine->SetAlpha((int)g_nTranspar);
			g_nDrawStatus = DRAWING_LINE;
		}
		break;
	case  TODUSTBIN_OVER:
		break;
	case DXF_PREDRAG: //add by jiangchao 2013.09.31
	case DRAW_NORMAL:
		{
			//判断点是否在某个选择框或按钮内
			int nTemp;
			bool bflagdown = false;
			nTemp = g_pWBPage->PointInSelectFrame(point);

			switch(nTemp)
			{
			case WB_PS_TRAPEZIUMPOINT://选中梯形的点
				{
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurTrapeziumLayerIndex);
					if (pObj != NULL)
					{
						if (pObj->IsLock() || pObj->IsLimit())
						{
							bflagdown = true;
							return;
						}
						g_nDrawStatus = MOVE_TRAPEZIUMPOINT;
					}
					bflagdown = true;
				}
				break;
			case WB_PS_TRIAMGLEPOINT:
				{
					
					m_dwCurObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					g_nDrawStatus = EDITTRIANGLE;
					return;
				}
				break;
			case WB_PS_NONE:
				{
 					if (g_pHPWBDataNotify && g_bSupportI7)
 					{
 						MSG msg;
 						memset(&msg,0,sizeof(MSG));
 						msg.hwnd = NULL;
 						msg.message = 0;
 						msg.wParam = (WPARAM)0;
 						msg.lParam = (LPARAM)0;
 						m_bSelectStatueChange = 0;
 						g_pHPWBDataNotify->SetSelectObjStatue(msg);
 					}					
				}
				break;
			case WB_PS_OBJ:
				{
					m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
					if (pObj == NULL)
					{
// 						if (g_pHPWBDataNotify && m_bSelectStatueChange == 1 && g_bSupportI7)
// 						{
// 							MSG msg;
// 							memset(&msg,0,sizeof(MSG));
// 							msg.hwnd = NULL;
// 							msg.message = 0;
// 							msg.wParam = (WPARAM)0;
// 							msg.lParam = (LPARAM)0;
// 							m_bSelectStatueChange = 0;
// 							g_pHPWBDataNotify->SetSelectObjStatue(msg);
// 						}	
						g_nDrawStatus = DRAW_NORMAL;
						return;
					}
					else
					{
							
					}
					if (pObj->GetType() == WB_OBJ_PROTRACTOR)
					{
						if (g_pWBPage->GetWBSelectFrameList().size() > 0)
						{
							//1-17对选择框架进行删除和当前选择图层号置0	
							g_pWhiteBoardView->setFrameListAndObjLayerIndex();
						}
						g_pWBPage->AddSelectFrame(m_dwCurSelectObjLayerIndex);
						((CWBProtractor*)pObj)->MouseDown(point);

						g_nDrawStatus = ENTER_PROTRACTOR;						
						return;
					}		
					if (pObj->GetType() == WB_OBJ_SETSQUARE)
					{
						if (g_pWBPage->GetWBSelectFrameList().size() > 0)   //可以解决 因选中了其他对象 再选三角板 其他对象的虚线框不删除问题
						{
							//1-17对选择框架进行删除和当前选择图层号置0	
							//g_pWhiteBoardView->setFrameListAndObjLayerIndex();
							g_pWBPage->DeleteListForSetquare();
							Invalidate();

						}
						((CWBSetSquare*)pObj)->MouseDown(point);
						g_pWBPage->AddSelectFrame(m_dwCurSelectObjLayerIndex);
						//pObj->SetAlpha(127);
						g_nDrawStatus = ENTER_SETSQUARE;
						return;
					}
					if (pObj->GetType() == WB_OBJ_COMPASS)
					{
						if (g_pWBPage->GetWBSelectFrameList().size() > 0)
						{
							//1-17对选择框架进行删除和当前选择图层号置0	
							g_pWhiteBoardView->setFrameListAndObjLayerIndex();
						}
						((CWBCompass*)pObj)->MouseDown(point);
						g_pWBPage->AddSelectFrame(m_dwCurSelectObjLayerIndex);
						g_nDrawStatus = ENTER_COMPASS;

						return;
					}
					//bflagdown = true;
				}
				break;
			case WB_PS_CTRLBUTTON://如果点在某个控制按钮内
				{
					m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
					if (pObj == NULL)
					{
						g_nDrawStatus = DRAW_NORMAL;
						return;
					}
					if (pObj->GetType() == WB_OBJ_COMBINE)
					{
						CWBObject *pCtrlObj = ((CWBCombine *)pObj)->SetControlObj(point);
						if (pCtrlObj == NULL)
						{
							g_nDrawStatus = DRAW_NORMAL;
							return;
						}
						pObj = pCtrlObj;
					}
					(pObj->GetControlButton())->ButtonDown(point);
					g_nDrawStatus = ENTER_CONTROL;
					Invalidate(FALSE);
					bflagdown = true;
				}
				break;
			case WB_PS_SELECTBUTTON://如果点在某个选择框按钮内
				{
					m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
					if (pObj == NULL)
					{
						g_nDrawStatus = DRAW_NORMAL;
						return;
					}
					(pObj->GetImageButton())->ButtonDown(point);
					g_nDrawStatus = ENTER_BUTTON;
					m_ptPaste = point;
					bflagdown = true;
				}
				break;
			case WB_PS_SELECTFRAME://如果点在某个选择框内
				{
					m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
					if (pObj == NULL)
					{
						g_nDrawStatus = DRAW_NORMAL;
						return;
					}

					if (pObj->GetType() == WB_OBJ_PROTRACTOR)
					{
						if (g_pWBPage->GetWBSelectFrameList().size() > 0)
						{
							//1-17对选择框架进行删除和当前选择图层号置0	
							g_pWhiteBoardView->setFrameListAndObjLayerIndex();
						}
						g_pWBPage->AddSelectFrame(m_dwCurSelectObjLayerIndex);
						((CWBProtractor*)pObj)->MouseDown(point);

						g_nDrawStatus = ENTER_PROTRACTOR;
						//m_nProtractorDownpt = point;

						if (NULL != m_pWBDustbin)
						{
							CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
							int iSelectObjCounts = pSelectObjLst->GetSize();
							POSITION pos;
							//CWBObject *pObj = NULL;
							for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
							{
								pos = pSelectObjLst->FindIndex(i);

								pObj = pSelectObjLst->GetAt(pos);
								if (pObj == NULL)
								{
									continue;
								}
								if (pObj->IsLock() || pObj->IsLimit())
								{
									continue;
								}
								else
								{
									//pObj->m_preAlpha = pObj->GetAlpha();
									pObj->SetPreAlpha(pObj->GetAlpha());
								}
							}
						}
						return;
					}
					if (pObj->GetType() == WB_OBJ_SETSQUARE)
					{
						if (g_pWBPage->GetWBSelectFrameList().size() > 0)   //可以解决 因选中了其他对象 再选三角板 其他对象的虚线框不删除问题
						{
							//1-17对选择框架进行删除和当前选择图层号置0	
							g_pWBPage->DeleteListForSetquare();
						}
						((CWBSetSquare*)pObj)->MouseDown(point);
						g_pWBPage->AddSelectFrame(m_dwCurSelectObjLayerIndex);
						//pObj->SetAlpha(127);
						g_nDrawStatus = ENTER_SETSQUARE;

						return;
					}
					if (pObj->GetType() == WB_OBJ_COMPASS)
					{
						if (g_pWBPage->GetWBSelectFrameList().size() > 0)   //可以解决 因选中了其他对象 再选圆规 其他对象的虚线框不删除问题
						{
							//1-17对选择框架进行删除和当前选择图层号置0	
							g_pWBPage->DeleteListForSetquare();
						}
						((CWBCompass*)pObj)->MouseDown(point);
						g_pWBPage->AddSelectFrame(m_dwCurSelectObjLayerIndex);
						//pObj->SetAlpha(127);
						g_nDrawStatus = ENTER_COMPASS;

						return;
					}
					if (pObj->GetType() == WB_OBJ_RULER)
					{					
						if (((CWBRuler*)pObj)->RulerPointIn(point))
						{
							g_nDrawStatus = MOVEING_RULERBTN;
							return;
						}
					}
					if (pObj->IsInfiniteClone() && !pObj->IsLock() && !pObj->IsLimit())
					{
						pObj->SetClone(TRUE);
					}

					m_ptSelect = point;
					m_ptPrePoint = point;

					//denghong设置移动偏移基点
					m_moveFirstPoint = point;

					m_ulMoveCounts = 0;

					//g_nDrawStatus = MOVING_LINE;
					if (pObj->GetType() == WB_OBJ_DXF && g_nDrawStatus == DXF_PREDRAG) //jiangchao 2013.09.02
					{
						((CWBDxf*)pObj)->SetOrgPoint(point);
						g_nDrawStatus = DXF_DRAG;
					} 
					else
					{
						g_nDrawStatus = MOVING_LINE;
					}

					
					if (NULL != m_pWBDustbin)
					{
						CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
						int iSelectObjCounts = pSelectObjLst->GetSize();
						POSITION pos;
						//CWBObject *pObj = NULL;
						for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
						{
							pos = pSelectObjLst->FindIndex(i);

							pObj = pSelectObjLst->GetAt(pos);
							if (pObj == NULL)
							{
								continue;
							}
							if (pObj->IsLock() || pObj->IsLimit())
							{
								continue;
							}
							else
							{
								//pObj->m_preAlpha = pObj->GetAlpha();
								pObj->SetPreAlpha(pObj->GetAlpha());
							}
						}
					}
					bflagdown = true;
				}
				break;
			case WB_PS_SELECTPOINT://如果点在标注尖角的点的范围内
				{
					CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurAnnotationsLayerIndex);
					if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档
					{
						m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
						CWBObject *pObject = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
						if (pObject != NULL)
						{
							if (pObject->IsLock() || pObject->IsLimit())
							{
								bflagdown = true;
							}
							if (pObject->GetType() == WB_OBJ_DOCUMENT)
							{
								pObj = ((CWBDocument *)pObject)->GetAnn();
							}
							if (pObject->GetType() == WB_OBJ_PICTURE)
							{
								pObj = ((CWBPicture *)pObject)->GetAnn();
							}
							if (pObject->GetType() == WB_OBJ_MIDI)
							{
								pObj = ((CWBMidi*)pObject)->GetAnn();
							}
						}
					}
					if (pObj != NULL)
					{
						if (pObj->IsLock() || pObj->IsLimit())
						{
							bflagdown = true;
						}
						g_nDrawStatus = CHANGE_ANNOTATIONS;
					}
					bflagdown = true;
				}
				break;
			case WB_PS_INMEDIACTRL:
				{
					g_pWhiteBoardView->m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
					g_nDrawStatus = MEDIA_CTRLING;
					Invalidate();
					bflagdown = true;
				}
				break;
			default:
				break;
			}
			if (bflagdown)
			{
				break;
			}
			if ((nFlags & MK_CONTROL) != MK_CONTROL)
			{
				BOOL bFlag = FALSE;
				if ((int)((g_pWBPage->GetWBSelectFrameList()).size()) > 0)
				{
					bFlag = TRUE;
				}
				g_pWBPage->ClearSelectFrameList();
				if (bFlag)
				{
					SetPageIsNeedDraw(TRUE);
					Invalidate();
					UpdateWindow();
					SetPageIsNeedDraw(FALSE);
				}
			}
			m_rectSelect = CRect(point, point);
			if(g_nDrawStatus == DXF_PREDRAG)
			{
				SetCursor(LoadCursor(NULL,IDC_ARROW));
			}
			g_nDrawStatus = DRAW_SELECTINT;

			if (g_pHPWBDataNotify && m_bSelectStatueChange == 1 && g_bSupportI7)
			{
				MSG msg;
				memset(&msg,0,sizeof(MSG));
				msg.hwnd = NULL;
				msg.message = 0;
				msg.wParam = (WPARAM)0;
				msg.lParam = (LPARAM)0;
				m_bSelectStatueChange = 0;
				g_pHPWBDataNotify->SetSelectObjStatue(msg);
			}
		}	
		break;
	case DRAW_ROUND:		
		break;
	case DRAW_POLYLINE: //画任意线
		{
			CWBCurve *pCurve = new CWBCurve();
			if (pCurve == NULL)
			{
				break;
			}
			pCurve->SetResume_broken_taransfer(true);

			pCurve->SetAlpha((int)g_nTranspar);
			pCurve->SetLeftTopRightBottomPt(point, point);
			pCurve->AddPoint(point);
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pCurve);
			m_ptPointLineCurve = point;
			g_nDrawStatus = DRAWING_POLYLINE;
			Invalidate();
		}
		break;
		
	// add by jiangchao 2013.05.10
	case DRAW_POLYICON: //画任意图标
		{
			CWBIcoCurve * pIcoCurve = new CWBIcoCurve();
			if(pIcoCurve == NULL)
			{
				break;
			}
			pIcoCurve->SetResume_broken_taransfer(true);
			pIcoCurve->SetAlpha((int)g_nTranspar);
			pIcoCurve->AddPoint(point);
			pIcoCurve->SetBeginPoint(point);
			pIcoCurve->SetLeftTopRightBottomPt(point, point);

			pIcoCurve->SetIsLoadPicFromFile(g_bIsWBLoadIcoFromFile); //设置是否从文件加载

			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pIcoCurve);
			g_nDrawStatus = DRAWING_POLYICON;
			Invalidate();
		}
		break;
	case DRAW_MAGICLINE: //画魔术线
		{
			CWBMagicPen *pMagicPen = new CWBMagicPen();
			if (pMagicPen == NULL)
			{
				break;
			}
			pMagicPen->SetResume_broken_taransfer(true);
			pMagicPen->SetAlpha((int)g_nTranspar);
			pMagicPen->SetLeftTopRightBottomPt(point, point);
			pMagicPen->AddPoint(point);
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pMagicPen);
			m_ptPointLineCurve = point;
			g_nDrawStatus = DRAWING_MAGICLINE;
			Invalidate();
		}
		break;
	case DRAW_RECT:
		{
			CWBRectangle *pRect = new CWBRectangle(point, point);
			//pRect->SetFillColor(RGB(0, 123, 34));
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pRect);
			pRect->SetAlpha((int)g_nTranspar);
			g_nDrawStatus = DRAWING_RECT;
		}
		break;
	case DRAWED_TEXT:
		{
			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				
			}
			PostMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			g_nDrawStatus = DRAWEUP_TEXT;
		}
		break;
	case DRAW_TEXT:
		{
			m_bIsAnnotations = FALSE;
			m_rectText.left = point.x;
			m_rectText.top = point.y;
			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			CWBRichEditMgr::GetInst_()->SetBeginPoint(point.x, point.y);
			SetPageIsNeedDraw(FALSE);
			g_nDrawStatus = DRAWING_TEXT;
		}
		break;
	case DRAW_ELLIPSE:
		{	
			m_topLeft = point;
			CWBEllipse *pEllipse = new CWBEllipse(m_topLeft, point);
			pEllipse->SetAlpha((int)g_nTranspar);
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pEllipse);			
			g_nDrawStatus = DRAWING_ELLIPSE;
		}
		break;
	case DRAW_TRIANGLE:	//画三角形
		{
			CWBTriangle *pTriangle = new CWBTriangle(point, point,m_nTriangleType);
			//			pTriangle->SetFillColor(RGB(0, 123, 34));
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTriangle);
			pTriangle->SetAlpha((int)g_nTranspar);
			g_nDrawStatus = DRAWING_TRIANGLE;
		}
		break;
	case DRAW_POLYGON:	//画多边形
		{
			m_topLeft = point;
			CWBPolygon *pPolygon = new CWBPolygon();
			pPolygon->SetAlpha((int)g_nTranspar);
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPolygon);
			pPolygon->AddPoint(point);
		}
		break;
	case DRAW_RHOMBUS:	//菱形
		{
			CWBRhombus *pRhombus = new CWBRhombus(point, point);
			//			pRhombus->SetFillColor(RGB(0, 123, 34));
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pRhombus);
			pRhombus->SetAlpha((int)g_nTranspar);
			g_nDrawStatus = DRAWING_RHOMBUS;

		}
		break;
	case DRAW_PALLELOGAM: //画平行四边形
		{
			CWBPallelogam *pPallelogam = new CWBPallelogam(point,point);
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPallelogam);
			pPallelogam->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_PALLELOGAM;
		}
		break;
	case DRAW_PENTAGON: //画五边形
		{
			CWBPentagon *pPentagon = new CWBPentagon(point,point);
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pPentagon);
			pPentagon->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_PENTAGON;
		}
		break;
	case DRAW_HEXAGON: //画六边形
		{
			CWBHexagon *pHexagon = new CWBHexagon(point,point);
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pHexagon);
			pHexagon->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_HEXAGON;
		}
		break;
	case DRAW_FIVEPOINTSTART: //画五角星
		{
			CWBFivepointstart *pFivepointstart = new CWBFivepointstart(point,point);
			if (pFivepointstart == NULL)
			{
				break;
			}
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pFivepointstart);
			pFivepointstart->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_FIVEPOINTSTART;
		}
		break;
	case DRAW_TRAPEZIUM: //画梯形
		{
			CWBTrapezium *pTrapezium = new CWBTrapezium(point,point);
			if (pTrapezium == NULL)
			{
				break;
			}
			m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pTrapezium);
			pTrapezium->SetAlpha((int)g_nTranspar);
			g_nDrawStatus =DRAWING_TRAPEZIUM;
		}
		break;
	case DRAW_ERASE: //擦除
		g_nDrawStatus = DRAW_ERASING;
		m_ptErasePre = point;

		m_EraseRectDowm = true;
		g_pWBPage->GetAllObjLayerIndex(m_vecLineLayerIndexBeforeErase, WB_OBJ_LINE); 
		break;
	case DRAW_RGNDELETE: //区域删除
		{
			if (m_pRgnDeleteWBCurve != NULL)
			{
				delete m_pRgnDeleteWBCurve;
				m_pRgnDeleteWBCurve = NULL;
			}
			PWBCurveItem pWBCurveItem = new WBCurveItem;
			//pWBCurveItem->nWidth = 1;
			//pWBCurveItem->clrLine = RGB(0, 255, 0);
			pWBCurveItem->SetWidth(2);
			pWBCurveItem->SetColor(RGB(0, 255, 0));
			m_pRgnDeleteWBCurve = new CWBCurve(pWBCurveItem);
			m_pRgnDeleteWBCurve->AddPoint(point);

			delete pWBCurveItem;
			g_nDrawStatus = DRAW_RGNDELETING;
		}		
		break;
	case DRAW_CLEANUP: //清除
		g_nDrawStatus = DRAW_CLEANUPING;
		break;
	case ADD_ANNOTATIONS:	//添加标注
		{
			//g_pWhiteBoardView->SetIsKillFocus(FALSE);
			m_bIsAnnotations = TRUE;
			m_rectText.left = point.x;
			m_rectText.top = point.y;

			if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
			}
			CWBRichEditMgr::GetInst_()->SetBeginPoint(point.x, point.y);

			SetPageIsNeedDraw(FALSE);
			g_nDrawStatus = DRAWING_TEXT;
		}
		break;
	case DXF_DRAG:
		break;	
	default:
		break;
	}
	m_dwCurOperationObjId = m_dwCurObjLayerIndex;  //得到正在操作对象的id

	SCROLLBARINFO sbInfo; 
	ZeroMemory((void*)&sbInfo, sizeof(SCROLLBARINFO)); 
	sbInfo.cbSize = sizeof(SCROLLBARINFO); 
	BOOL bRet2 = GetScrollBarInfo(OBJID_HSCROLL, &sbInfo);
	DWORD dRet = sbInfo.rgstate[0];
	if (g_bSupportI7 && (dRet & STATE_SYSTEM_INVISIBLE))
	{
		POINT ptCur ={0};
		GetCursorPos(&ptCur);
		RECT rcWnd = {0};
		MONITORINFO oMonitor = {0}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromRect(&rcWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		RECT rcWork = oMonitor.rcWork;
		rcWork.right -= 14;
		rcWork.bottom -= 86;

		if (PtInRect(&rcWork, ptCur))
		{
			ClipCursor(&rcWork);
		}
	}

	if (bIgnoreTouchOrMouse)
	{
		nFlags &=  ~MK_WB_ULBUTTON;
	}
	CScrollView::OnLButtonDown(nFlags, point);

}

/*************************************************************************
*                    CWhiteBoardView::OnMouseUp()							*
*               释放鼠标事件，根据m_draw的状态进行操作                   *
*************************************************************************/
void CWhiteBoardView::OnLButtonUp(UINT nFlags, CPoint point)
{
	EndHook();
	ClipCursor(NULL);
	RefreshWB();
	m_bLimitCursorArea = false;
	if(m_WBSpotlight != NULL)
	{
		SendSpotlight();
	}
	m_rectRefresh = CRect(0, 0, 0, 0);
	m_PreRectRefresh = CRect(0, 0, 0, 0);
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	if (g_bYunNanDeviceSupported && !g_bAuthorizedByHandWare)//若是云南公安的版本没有授权直接返回
	{
		return;
	}
	if (g_systemVersion >= 6 && CScreenDrawMgr::GetIns() != NULL)
	{
		///将mousedlg放在screendrawdlg的下面
		CScreenDrawMgr::GetIns()->SetMouseDlgWindowPos();
		return;
	}

	BOOL bIgnoreTouchOrMouse = FALSE;
	if (nFlags & MK_WB_ULBUTTON)
	{
		bIgnoreTouchOrMouse = TRUE; //继续往下执行，绕开当前消息是触摸屏消息的判断
	}
	else if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH )
	{
		return ;
	}

	CClientDC dc(this);
	if(g_reduce>0.00000000000000000)
	{
		ChangeViewCoordinate(&dc);
	}
	else
	{
		OnPrepareDC(&dc);//进行坐标原点的匹配
	}
	dc.DPtoLP(&point);//将视图坐标转换为文档作标
	if (m_bIsDBClick)
	{
		m_bIsDBClick = false;
		CWBObject* pObj = NULL;
		pObj = g_pWBPage->FindByLayerIndex(m_dDBClickobjlayer);
		if (pObj == NULL)
		{
			return;
		}
// 		int nIndex;
// 		POSITION pos;
// 		CWBObject *pSleObj = NULL;
// 		CObList *pPageList = g_pWBPage->GetWBList();
// 		bool bfind = false;
// 		if (pPageList != NULL)
// 		{
// 			for (nIndex =0;nIndex<pPageList->GetCount();++nIndex)
// 			{
// 				pos = pPageList->FindIndex(nIndex);
// 				if (pos == NULL)
// 				{
// 					continue;
// 				}
// 				pSleObj = (CWBObject *)pPageList->GetAt(pos);
// 				if (pSleObj == NULL)
// 				{
// 					continue;
// 				}
// 				if (pSleObj->IsBkg())
// 				{
// 					continue;
// 				}
// 				if (pSleObj->GetType() == WB_OBJ_DOCUMENT)
// 				{
// 					if (((CWBDocument *)pSleObj)->GetIsConvertTip())
// 					{
// 						continue;
// 					}
// 				}
// 				if (pSleObj->GetLayerIndex() == pObj->GetLayerIndex())
// 				{
// 					g_pWBPage->DeleteSelectFrame(pObj->GetLayerIndex());
// 					pPageList->RemoveAt(pos);							
// 					CWBMTObjectManager::GetIns_()->RemoveMTObject(pObj);
// 					bfind = true;
// 					Invalidate();
// 					UpdateWindow();
// 				}
// 			}
// 
// 		}
// 		if (bfind)
		{
			CWBPicture* pWBpic = NULL;
			if (pObj->GetType() == WB_OBJ_PICTURE)
			{
				pWBpic = new CWBPicture(m_strDBClickSavePath,((CWBPicture*)pObj)->m_pItem->rect);
				if (pWBpic == NULL)
				{
					delete pWBpic;
					pWBpic = NULL;
				}
				pWBpic->CopyAttribute(pObj);
				SetWBPictureEffect((CWBPicture *)pWBpic);
				CRect rcResGaller;
				((CWhiteBoardDlg*)g_whiteboardDlg)->GetResGalleryRect(rcResGaller);
				CPoint ptScren(rcResGaller.right,rcResGaller.top);
				CRect rcWBdlg;
				g_whiteboardDlg->GetWindowRect(&rcWBdlg);
				bool bgoright = true;
				int x = ((CWBPicture *)pWBpic)->m_pItem->rect.right;
				int y =rcWBdlg.right - rcWBdlg.Width()/3;
				x = x - ((CWBPicture *)pWBpic)->m_pItem->rect.Width()/2;
				if (x > y)
				{
					bgoright = false;
				}
				ProcessMediaPhotoEffect(ptScren,bgoright);
				if (m_strResourceUpdataPath.IsEmpty())
				{
					CString strFilename;
					CString strReadName;
					strReadName = ReadConfigText(_T("WhiteBoardMenuListText"),_T("MediaPhoto"));
					if (strReadName.IsEmpty())
					{
						strReadName = _T("视频拍照");
					}
					strFilename = g_strMediaPhoto + _T("\\") + strReadName;
					m_strResourceUpdataPath = strFilename;
				}
				WBRESWND::SwitchSpecialDirectoryEx(m_strResourceUpdataPath);
			}
			
		}
	}
	//更新
	if(MouseUpMagn(point))
	{
		return;
	}
	if (m_bStartRecognition)
	{
		m_bRecogniMouseDown = false;
		if (m_pGeoRecognizer == NULL)
		{
			m_pGeoRecognizer = new GeometricRecognizer;
			m_pGeoRecognizer->loadTemplates();
		}
		if (path2D.size() <10)
		{
			path2D.clear();
			m_RecShowpoint.clear();
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
			return;
		}
		
		RecognitionResult recsult  = m_pGeoRecognizer->recognize(path2D);
		string name;
		name = recsult.name;
		path2D.clear();
		PointRectangle box = m_pGeoRecognizer->getboundingbox();
		double x;
		double y;
		double width;
		double height;
		x = box.RectangleX;
		y = box.RectangleY;
		width = box.RectangleWidth;
		height = box.RectangleHeight;

		if (name == "Triangle")
		{
			CPoint pt = m_RecShowpoint.front();
								
			CPoint pt1(x,y+height);						
			CPoint pt2(x+width,y+height);
			for (int i =0;i<m_RecShowpoint.size();++i)
			{
				CPoint temp = m_RecShowpoint[i];
				if (temp.x == pt2.x)
				{
					pt2.y =temp.y;
				}
			}
			if (pt.x > pt1.x)
			{
				CPoint pttemp;
				pttemp = pt;
				pt= pt1;
				pt1 =pttemp;
			}
			CWBArbitraryTriangle *pTriangle = new CWBArbitraryTriangle(pt,pt1,pt2);
			pTriangle->SetAlpha((int)g_nTranspar);
			pTriangle->CreateSelecrtFrame();
			g_pWBPage->AddWBObject(pTriangle);
			m_RecShowpoint.clear();	
			
		}
		if (name == "Circle")
		{
			m_RecShowpoint.clear();
			CWBEllipse *pEllipse = new CWBEllipse(CPoint(x,y),CPoint(x+width,y+height));
			pEllipse->SetAlpha((int)g_nTranspar);
			pEllipse->CreateSelecrtFrame();
			g_pWBPage->AddWBObject(pEllipse);
			

		}
		if (name == "Rectangle")
		{
			m_RecShowpoint.clear();
			CWBRectangle *pRectangle = new CWBRectangle(CPoint(x,y),CPoint(x+width,y+height));
			pRectangle->SetAlpha((int)g_nTranspar);
			pRectangle->CreateSelecrtFrame();
			g_pWBPage->AddWBObject(pRectangle);
			
		}					
		if (name == "Star")
		{
			m_RecShowpoint.clear();
			CWBFivepointstart *pFiveStart = new CWBFivepointstart(CPoint(x,y),CPoint(x+width,y+height));
			pFiveStart->SetAlpha((int)g_nTranspar);
			pFiveStart->CreateSelecrtFrame();
			g_pWBPage->AddWBObject(pFiveStart);
			
		}	
		
		m_RecShowpoint.clear();
		SetPageIsNeedDraw(TRUE);
		Invalidate();
		UpdateWindow();
		SetPageIsNeedDraw(FALSE);
		return;
	}
	switch(g_nDrawStatus)
	{
	case MIDI_DRAGING:
		{
			CWBMidi *pwbmidi = (CWBMidi *)g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex,WB_OBJ_MIDI);
			if (pwbmidi != NULL)
			{
				if (pwbmidi->GetObjSelectFrame()->PointIn(point))
				{
					SetCursor(LoadCursor(NULL,IDC_ARROW));
					pwbmidi->ChangeShowPosition(point);					
				}

				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				SetPageIsNeedDraw(FALSE);
				XMemIni xmlObj;
				pwbmidi->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
			}
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case EDITTRIANGLE:
		{
			CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_ARBITRARYTRIANGLE);
			if (pTriangle != NULL)
			{
				pTriangle->EditTrianglePoint(point,true);	
				pTriangle->AdjustObject();
				//pTriangle->SetIsEdit(false);
				SetPageIsNeedDraw(TRUE);
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pTriangle->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
			}                    
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case DRAWINGANYTRIANGLE:
		{
			CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_ARBITRARYTRIANGLE);
			if (pTriangle != NULL)
			{
				pTriangle->Move(point);	
				if (pTriangle->CanbeTriangle())
				{
					pTriangle->CreateSelecrtFrame();
				}
				else
				{
					pTriangle->Move(point,CPoint(point.x +30,point.y+100),CPoint(point.x+100,point.y+100));
					pTriangle->CreateSelecrtFrame();
				}
				pTriangle->AdjustObject();

				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				//发送同步消息
				XMemIni xmlObj;
				pTriangle->SetXmlValue(xmlObj);
				AddObj(xmlObj);
				
			}                               
			g_nDrawStatus = DRAWANYTRIANGLE;	
		}
		break;
	case ENTER_PROTRACTOR:
		{   
			m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
			CWBObject *pobj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);

			if (pobj != NULL)
			{
				bool bInBtn = ((CWBProtractor*)pobj)->GetInButton();

				((CWBProtractor*)pobj)->MouseUp(point);
				g_nDrawStatus = DRAW_NORMAL;
				if (bInBtn)
				{
					return ;
				}
				bool bSendMoveMsg = false;
				CPoint moveOffset;
				{
					moveOffset = point - m_moveFirstPoint;
					if ((nFlags & MK_CONTROL) == MK_CONTROL && moveOffset.x == 0 && moveOffset.y == 0)
					{
						g_pWBPage->DeleteSelectFrame(m_dwCurSelectObjLayerIndex);
						g_nDrawStatus = DRAW_NORMAL;
						Invalidate();
						UpdateWindow();
						break;
					}
					else if (moveOffset.x == 0 && moveOffset.y == 0)
					{
						SetPageIsNeedDraw(FALSE);
					}
					bSendMoveMsg = true;
					g_nDrawStatus = DRAW_NORMAL;
				}

				Invalidate(FALSE);
			
				if (NULL != m_pWBDustbin) //假如创建了垃圾箱
				{

					int mousepos = m_pWBDustbin->PointIn(point);
					if (MOUSE_OUTSIDE != mousepos)
					{
						CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
						CWBObject *pObj = NULL;
						int iSelectObjCounts = pSelectObjLst->GetSize();
						POSITION pos;

						CPoint ptDustbin;
						CRect rect;
						CPoint *pptCenter = new CPoint[iSelectObjCounts];
						CPoint *pptOffset = new CPoint[iSelectObjCounts];
						m_pWBDustbin->GetRect(rect);
						m_pWBDustbin->GetCenterPoint(ptDustbin);
						ptDustbin.y = rect.top;
						for (int i = 0; i < iSelectObjCounts; ++i)
						{
							pos = pSelectObjLst->FindIndex(i);
							pObj = pSelectObjLst->GetAt(pos);
							if (pObj == NULL)
							{
								continue;
							}
							pObj->GetCenterPoint(pptCenter[i]);
							pptOffset[i] = ptDustbin - pptCenter[i];
						}
						if (!(iSelectObjCounts == 1 && pObj->GetType() == WB_OBJ_COMBINE))
						{
							int alpha = 255/2;
							int index = 0;
							int stepNumber = 10;
							int initialAlpha = alpha;
							double offsetDescRate[10] = {-0.2, -0.2, -0.2, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, -1.1};
							double zoomRate[10]       = {1.1 , 1.1 , 1.1 , 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 1.0 };
							while (index < stepNumber)
							{
								for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
								{
									pos = pSelectObjLst->FindIndex(i);
									pObj = pSelectObjLst->GetAt(pos);
									if (pObj == NULL)
									{
										continue;
									}
									if (pObj->IsLock() || pObj->IsLimit())
									{
										continue;
									}
									else
									{
										if (pObj->GetType() == WB_OBJ_CURVE)
										{
											((CWBCurve*)pObj)->SetIsDraw();
										}
										if (pObj->GetType() == WB_OBJ_PICTURE)
										{
											((CWBPicture*)pObj)->SetIsPath();
										}

										if (pObj->GetType() != WB_OBJ_COMBINE)
										{
											if (pObj->GetType() == WB_OBJ_DOCUMENT)
											{
												CWBObject *pDocCom = ((CWBDocument *)pObj)->GetSelectComObj();
												if (pDocCom != NULL)
												{
													pDocCom->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
													CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
													pDocCom->MoveOffset(ptOnce);
													if ((int)offsetDescRate[index] > 0)
													{
														pDocCom->RotateByCenter(60);
													}
													pDocCom->SetAlpha(alpha);
													if (pDocCom->GetType() == WB_OBJ_CURVE)
													{
														((CWBCurve*)pDocCom)->SetIsDraw();
													}
												}
												else
												{
													if ((int)offsetDescRate[index] > 0)
													{
														((CWBDocument *)pObj)->GetSrcAnnPt();

														//当对象缩放且旋转后，因坐标系变化原因会产生偏移，在这里纠正偏移
														if (pObj->GetIsZoom() && pObj->GetRotationAngle() != 0)
														{
															CPoint pt;
															CWBSelectRectangle *pWBSelectRectangle = pObj->GetObjSelectFrame();
															CPoint ptRotationCenter;
															pObj->GetCenterPoint(ptRotationCenter);
															pWBSelectRectangle->GetMapCenter(pt);
															pt -= ptRotationCenter;
															//使对象中心点偏移                                  
															ptRotationCenter += pt;
															pObj->RotateByPoint(ptRotationCenter,0);
															//移动对象
															pObj->MoveOffset(pt);

															pObj->SetIsZoom(FALSE);
														}

														((CWBDocument *)pObj)->GetSrcCenterPt();
														pObj->GetCenterPoint(g_ptDocRotCenter);
														pObj->RotateByCenter(60);
													}
													pObj->SetAlpha(alpha);
												}
											} 
											else
											{
												pObj->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
												CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
												pObj->MoveOffset(ptOnce);
												if ((int)offsetDescRate[index] > 0)
												{
													pObj->RotateByCenter(60);
												}
												pObj->SetAlpha(alpha);
											}
										}
									}
								}
								if (stepNumber > 1)
								{
									if (index != stepNumber - 1)
									{
										alpha -= (initialAlpha / 2) / (stepNumber - 1);
									}
								}
								index = index + 1;
								Invalidate();
								UpdateWindow();
								Sleep(3);
							}
						}
						bSendMoveMsg = false;
						delete [] pptCenter;
						delete [] pptOffset;
						DeleteSelectedObjects();
						g_nDrawStatus = DRAW_NORMAL;
						Invalidate();
						UpdateWindow();
					}
				}
				if (true == bSendMoveMsg)
				{

					AddComListToObj();
					if (point != m_moveFirstPoint)
					{
						MoveMsg(moveOffset,GenerateSSRC());
					}	
				}
				Invalidate();
				UpdateWindow();
			}
		}
		break;
	case ENTER_SETSQUARE:
		{
			m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
			CWBObject *pobj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pobj !=NULL)
			{
				((CWBSetSquare*)pobj)->MouseUp(point);
				g_nDrawStatus = DRAW_NORMAL;
				//pobj->SetAlpha(255);
				Invalidate();
				UpdateWindow();
			}
			BOOL bInBtn;
			bInBtn= ((CWBSetSquare *)pobj)->GetIsInBotton();
			if (TRUE == bInBtn)
			{
				((CWBSetSquare *)pobj)->SetIsInBotton();
				return;
			}
			bool bSendMoveMsg = false;
			CPoint moveOffset;
			moveOffset = point - m_moveFirstPoint;
			//if ((nFlags & MK_CONTROL) == MK_CONTROL && moveOffset.x == 0 && moveOffset.y == 0)
			// 			{
			// 				g_pWBPage->DeleteSelectFrame(m_dwCurSelectObjLayerIndex);
			// 				g_nDrawStatus = DRAW_NORMAL;
			// 				Invalidate();
			// 				break;
			// 			}
			// 			else if (moveOffset.x == 0 && moveOffset.y == 0)
			// 			{
			// 				SetPageIsNeedDraw(FALSE);
			// 			}
			// 	
			// 			bSendMoveMsg = true;
			// 			g_nDrawStatus = DRAW_NORMAL;
			// 
			// 			Invalidate(FALSE);

			if (NULL != m_pWBDustbin) //假如创建了垃圾箱
			{
				int mousepos = m_pWBDustbin->PointIn(point);
				if (MOUSE_OUTSIDE != mousepos)         
				{//如果在垃圾箱区域内
					CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
					CWBObject *pObj = NULL;
					int iSelectObjCounts = pSelectObjLst->GetSize();
					POSITION pos;

					CPoint ptDustbin;
					CRect rect;
					CPoint *pptCenter = new CPoint[iSelectObjCounts];
					CPoint *pptOffset = new CPoint[iSelectObjCounts];
					m_pWBDustbin->GetRect(rect);
					m_pWBDustbin->GetCenterPoint(ptDustbin);
					ptDustbin.y = rect.top;
					for (int i = 0; i < iSelectObjCounts; ++i)
					{
						pos = pSelectObjLst->FindIndex(i);
						pObj = pSelectObjLst->GetAt(pos);
						if (pObj == NULL)
						{
							continue;
						}
						pObj->GetCenterPoint(pptCenter[i]);
						pptOffset[i] = ptDustbin - pptCenter[i];
					}

					if (!(iSelectObjCounts == 1 && pObj->GetType() == WB_OBJ_COMBINE))
					{
						int alpha = 255/2;
						int index = 0;
						int stepNumber = 10;
						int initialAlpha = alpha;
						double offsetDescRate[10] = {-0.2, -0.2, -0.2, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, -1.1};
						double zoomRate[10]       = {1.1 , 1.1 , 1.1 , 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 1.0 };

						while (index < stepNumber)
						{
							for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
							{
								pos = pSelectObjLst->FindIndex(i);
								pObj = pSelectObjLst->GetAt(pos);
								if (pObj == NULL)
								{
									continue;
								}
								if (pObj->IsLock() || pObj->IsLimit())
								{
									continue;
								}
								else
								{

									if (pObj->GetType() != WB_OBJ_COMBINE)   
									{

										pObj->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
										CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
										pObj->MoveOffset(ptOnce);
										if ((int)offsetDescRate[index] > 0)
										{
											pObj->RotateByCenter(60);
										}
										pObj->SetAlpha(alpha);
										pObj->SetSelectFrameShow(FALSE);

									}
								}
							}
							if (stepNumber > 1)
							{
								if (index != stepNumber - 1)
								{
									alpha -= (initialAlpha / 2) / (stepNumber - 1);
								}
							}
							index = index + 1;
							Invalidate();
							UpdateWindow();
							Sleep(3);
						}
					}
					bSendMoveMsg = false;

					delete [] pptCenter;
					delete [] pptOffset;
					DeleteSelectedObjects();
					g_nDrawStatus = DRAW_NORMAL;
					Invalidate();
					UpdateWindow();
				}
			}
			if (true == bSendMoveMsg)
			{
				AddComListToObj();
				if (point != m_moveFirstPoint)
				{

					MoveMsg(moveOffset,GenerateSSRC());
				}	
			}	
		}
		break;
	case ENTER_COMPASS:
		{
			m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
			CWBObject *pobj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pobj !=NULL)
			{
				((CWBCompass*)pobj)->MouseUp(point);
				g_nDrawStatus = DRAW_NORMAL;
				//pobj->SetAlpha(255);
				Invalidate();
				UpdateWindow();
			}
			BOOL bInBtn = FALSE;
			//bInBtn= ((CWBCompass *)pobj)->GetIsInBotton();
			if (TRUE == bInBtn)
			{
				//((CWBSetSquare *)pobj)->SetIsInBotton();
				return;
			}
			bool bSendMoveMsg = false;
			CPoint moveOffset;
			moveOffset = point - m_moveFirstPoint;
			//if ((nFlags & MK_CONTROL) == MK_CONTROL && moveOffset.x == 0 && moveOffset.y == 0)
			// 			{
			// 				g_pWBPage->DeleteSelectFrame(m_dwCurSelectObjLayerIndex);
			// 				g_nDrawStatus = DRAW_NORMAL;
			// 				Invalidate();
			// 				break;
			// 			}
			// 			else if (moveOffset.x == 0 && moveOffset.y == 0)
			// 			{
			// 				SetPageIsNeedDraw(FALSE);
			// 			}
			// 	
			// 			bSendMoveMsg = true;
			// 			g_nDrawStatus = DRAW_NORMAL;
			// 
			// 			Invalidate(FALSE);

			if (NULL != m_pWBDustbin) //假如创建了垃圾箱
			{
				int mousepos = m_pWBDustbin->PointIn(point);
				if (MOUSE_OUTSIDE != mousepos)         
				{//如果在垃圾箱区域内
					CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
					CWBObject *pObj = NULL;
					int iSelectObjCounts = pSelectObjLst->GetSize();
					POSITION pos;

					CPoint ptDustbin;
					CRect rect;
					CPoint *pptCenter = new CPoint[iSelectObjCounts];
					CPoint *pptOffset = new CPoint[iSelectObjCounts];
					m_pWBDustbin->GetRect(rect);
					m_pWBDustbin->GetCenterPoint(ptDustbin);
					ptDustbin.y = rect.top;
					for (int i = 0; i < iSelectObjCounts; ++i)
					{
						pos = pSelectObjLst->FindIndex(i);
						pObj = pSelectObjLst->GetAt(pos);
						if (pObj == NULL)
						{
							continue;
						}
						pObj->GetCenterPoint(pptCenter[i]);
						pptOffset[i] = ptDustbin - pptCenter[i];
					}

					if (!(iSelectObjCounts == 1 && pObj->GetType() == WB_OBJ_COMBINE))
					{
						int alpha = 255/2;
						int index = 0;
						int stepNumber = 10;
						int initialAlpha = alpha;
						double offsetDescRate[10] = {-0.2, -0.2, -0.2, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, -1.1};
						double zoomRate[10]       = {1.1 , 1.1 , 1.1 , 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 1.0 };

						while (index < stepNumber)
						{
							for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
							{
								pos = pSelectObjLst->FindIndex(i);
								pObj = pSelectObjLst->GetAt(pos);
								if (pObj == NULL)
								{
									continue;
								}
								if (pObj->IsLock() || pObj->IsLimit())
								{
									continue;
								}
								else
								{

									if (pObj->GetType() != WB_OBJ_COMBINE)   
									{

										pObj->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
										CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
										pObj->MoveOffset(ptOnce);
										if ((int)offsetDescRate[index] > 0)
										{
											pObj->RotateByCenter(60);
										}
										pObj->SetAlpha(alpha);
										pObj->SetSelectFrameShow(FALSE);

									}
								}
							}
							if (stepNumber > 1)
							{
								if (index != stepNumber - 1)
								{
									alpha -= (initialAlpha / 2) / (stepNumber - 1);
								}
							}
							index = index + 1;
							Invalidate();
							UpdateWindow();
							Sleep(3);
						}
					}
					bSendMoveMsg = false;

					delete [] pptCenter;
					delete [] pptOffset;
					DeleteSelectedObjects();
					g_nDrawStatus = DRAW_NORMAL;
					Invalidate();
					UpdateWindow();
				}
			}
			if (true == bSendMoveMsg)
			{
				AddComListToObj();
				if (point != m_moveFirstPoint)
				{

					MoveMsg(moveOffset,GenerateSSRC());
				}	
			}	
		}
		break;
	case MOVEING_RULERBTN:
		{
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case DRAW_SELECTINT:    //框选和点选
		{
			if ((g_pWBPage->m_pMagicList).empty())
			{
				SetPageIsNeedDraw(FALSE);
			}
			//调整画出的选择框矩形
			int nTemp;
			if (m_rectSelect.left > m_rectSelect.right)
			{
				nTemp = m_rectSelect.left;
				m_rectSelect.left = m_rectSelect.right;
				m_rectSelect.right = nTemp;
			}
			if (m_rectSelect.bottom < m_rectSelect.top)
			{
				nTemp = m_rectSelect.top;
				m_rectSelect.top = m_rectSelect.bottom;
				m_rectSelect.bottom = nTemp;
			}
			if ((m_rectSelect.right - m_rectSelect.left) < 10 
				|| (m_rectSelect.bottom - m_rectSelect.top) < 10) //点选
			{
				m_dwCurSelectObjLayerIndex = g_pWBPage->GetSelectObject(point);	
				g_pWBPage->AddSelectFrame(m_dwCurSelectObjLayerIndex);

				g_nDrawStatus = DRAW_NORMAL;
				if (m_dwCurSelectObjLayerIndex > 0 )
				{
					SetPageIsNeedDraw(TRUE);
				}
				if (g_pHPWBDataNotify && m_bSelectStatueChange == 0 && g_bSupportI7)
				{
					int nSelectCpunt = 0;
					if(g_pWBPage != NULL)
					{			
						nSelectCpunt = g_pWBPage->GetWBSelectFrameList().size();
					}
					if (nSelectCpunt > 0)
					{
						MSG msg;
						memset(&msg, 0, sizeof(MSG));
						msg.hwnd = NULL;
						CObList * ptemplist = g_pWBPage->GetSelectObjLst();
						POSITION pos;
						for (int i = 0; i < nSelectCpunt; ++i)//遍历选中对象列表
						{
							pos = ptemplist->FindIndex(i);
							if (pos == NULL)
								continue;
							CWBObject* pObject = (CWBObject*)ptemplist->GetAt(pos);
							if (pObject->IsLock() || pObject->IsLimit())
							{
								msg.lParam = (LPARAM)1;
							}
							else
								msg.lParam = (LPARAM)0;
						}
						msg.message = 1;
						msg.wParam = (WPARAM)nSelectCpunt;
						m_bSelectStatueChange = 1;
						g_pHPWBDataNotify->SetSelectObjStatue(msg);
					}					
				}
				Invalidate();
				UpdateWindow();
			}
			else //框选
			{
				if (
					m_rectSelect.left >= g_pWhiteBoardView->m_rect.left
					|| m_rectSelect.bottom >= g_pWhiteBoardView->m_rect.bottom
					)
				{
					if (g_pWBPage->AddSelectFrames(m_rectSelect) > 0)
					{
						SetPageIsNeedDraw(TRUE);
					}
					std::vector<DWORD> m_vecSelectFrameList = g_pWBPage->GetWBSelectFrameList();
					if (g_bSupportI7 && m_vecSelectFrameList.size() == 1)
					{
						m_dwCurSelectObjLayerIndex = m_vecSelectFrameList[0];
					}
					g_nDrawStatus = DRAW_NORMAL;
					SetPageIsNeedDraw(TRUE);//加定时器开启的GIF后，图形类对象因框选设置为False而无法重绘
					if (g_pHPWBDataNotify && m_bSelectStatueChange == 0 && g_bSupportI7)
					{
						int nSelectCpunt = 0;
						if(g_pWBPage != NULL)
						{			
							nSelectCpunt = g_pWBPage->GetWBSelectFrameList().size();
						}
						if (nSelectCpunt > 0)
						{
							MSG msg;
							memset(&msg, 0, sizeof(MSG));
							msg.hwnd = NULL;
							CObList * ptemplist = g_pWBPage->GetSelectObjLst();
							POSITION pos;
							for (int i = 0; i < nSelectCpunt; ++i)//遍历选中对象列表
							{
								pos = ptemplist->FindIndex(i);
								if (pos == NULL)
									continue;
								CWBObject* pObject = (CWBObject*)ptemplist->GetAt(pos);
								if (pObject->IsLock() || pObject->IsLimit())
								{
									msg.lParam = (LPARAM)1;
								}
								else
									msg.lParam = (LPARAM)0;
							}
							msg.message = 1;
							msg.wParam = (WPARAM)nSelectCpunt;
							m_bSelectStatueChange = 1;
							g_pHPWBDataNotify->SetSelectObjStatue(msg);
						}						
					}
					Invalidate();
					UpdateWindow();
				}				
			}
		}
		break;
	case DRAWING_LINE:
		{	
			m_ulMoveCounts = 0;
			CWBLine *pLine = (CWBLine *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_LINE);
			if (pLine != NULL)
			{
				//wangxiaobin由于点击一下点太小了，看不到，所以扩大点击只有一点时候点的大小
				if (CalDistancePointToPoint(point,m_ptPointLineCurve) <5.0)
				{
					point.x += g_nWBCurObjWidth;
				}
				pLine->Move(point, FALSE);	

				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				//发送同步消息
				XMemIni xmlObj;
				pLine->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_LINE;
		}			
		break;

	case MOVING_LINE:
		{
			//denghong 移动直线偏移量
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				g_nDrawStatus = DRAW_NORMAL;
				return;
			}
			pObj->SetClone(FALSE);
			bool bSendMoveMsg = false;
			CPoint moveOffset;
			//			if(!AddComponentToDocument(pObj))
			{
				moveOffset = point - m_moveFirstPoint;
				if ((nFlags & MK_CONTROL) == MK_CONTROL && moveOffset.x == 0 && moveOffset.y == 0)
				{
					g_pWBPage->DeleteSelectFrame(m_dwCurSelectObjLayerIndex);
					g_nDrawStatus = DRAW_NORMAL;
					Invalidate();
					UpdateWindow();
					break;
				}
				else if (moveOffset.x == 0 && moveOffset.y == 0)
				{
					if (moveOffset.x == 0 && moveOffset.y == 0 && pObj->GetType() == WB_OBJ_MEDIA)
					{
						SetPageIsNeedDraw(TRUE);
					}
					else
					{
						SetPageIsNeedDraw(FALSE);
					}
					
				}
				/*MoveMsg(moveOffset);*/
				bSendMoveMsg = true;
				g_nDrawStatus = DRAW_NORMAL;
			}

			Invalidate(FALSE);

			if (NULL != m_pWBDustbin) //假如创建了垃圾箱
			{
				int mousepos = m_pWBDustbin->PointIn(point);
				if (MOUSE_OUTSIDE != mousepos)
				{
					CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
					CWBObject *pObj = NULL;
					int iSelectObjCounts = pSelectObjLst->GetSize();
					POSITION pos;

					CPoint ptDustbin;
					CRect rect;
					CPoint *pptCenter = new CPoint[iSelectObjCounts];
					CPoint *pptOffset = new CPoint[iSelectObjCounts];
					m_pWBDustbin->GetRect(rect);
					m_pWBDustbin->GetCenterPoint(ptDustbin);
					ptDustbin.y = rect.top;
					for (int i = 0; i < iSelectObjCounts; ++i)
					{
						pos = pSelectObjLst->FindIndex(i);
						pObj = pSelectObjLst->GetAt(pos);
						if (pObj == NULL)
						{
							continue;
						}
						pObj->GetCenterPoint(pptCenter[i]);
						pptOffset[i] = ptDustbin - pptCenter[i];
					}
					if (!(iSelectObjCounts == 1 && pObj->GetType() == WB_OBJ_COMBINE))
					{
						int alpha = 255/2;
						int index = 0;
						int stepNumber = 10;
						int initialAlpha = alpha;
						double offsetDescRate[10] = {-0.2, -0.2, -0.2, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, -1.1};
						double zoomRate[10]       = {1.1 , 1.1 , 1.1 , 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 1.0 };
						while (index < stepNumber)
						{
							for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
							{
								pos = pSelectObjLst->FindIndex(i);
								if (pos == NULL)
								{
									continue;
								}
								pObj = pSelectObjLst->GetAt(pos);
								if (pObj == NULL)
								{
									continue;
								}
								if (pObj->IsLock() || pObj->IsLimit())
								{
									continue;
								}
								else
								{
									if (pObj->GetType() == WB_OBJ_ICOCURVE)
									{
										((CWBIcoCurve*)pObj)->SetIsRefush(TRUE);
									}
									if (pObj->GetType() == WB_OBJ_CURVE)
									{
										((CWBCurve*)pObj)->SetIsDraw();
									}
									if (pObj->GetType() == WB_OBJ_PICTURE)
									{
										((CWBPicture*)pObj)->SetIsPath();
									}

									if (pObj->GetType() != WB_OBJ_COMBINE)
									{
										if (pObj->GetType() == WB_OBJ_DOCUMENT)
										{
											CWBObject *pDocCom = ((CWBDocument *)pObj)->GetSelectComObj();
											if (pDocCom != NULL)
											{
												pDocCom->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
												CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
												pDocCom->MoveOffset(ptOnce);
												if ((int)offsetDescRate[index] > 0)
												{
													pDocCom->RotateByCenter(60);
												}
												pDocCom->SetAlpha(alpha);
												if (pDocCom->GetType() == WB_OBJ_CURVE)
												{
													((CWBCurve*)pDocCom)->SetIsDraw();
												}
											}
											else
											{
												if ((int)offsetDescRate[index] > 0)
												{
													((CWBDocument *)pObj)->GetSrcAnnPt();

													//当对象缩放且旋转后，因坐标系变化原因会产生偏移，在这里纠正偏移
													if (pObj->GetIsZoom() && pObj->GetRotationAngle() != 0)
													{
														CPoint pt;
														CWBSelectRectangle *pWBSelectRectangle = pObj->GetObjSelectFrame();
														CPoint ptRotationCenter;
														pObj->GetCenterPoint(ptRotationCenter);
														pWBSelectRectangle->GetMapCenter(pt);
														pt -= ptRotationCenter;
														//使对象中心点偏移                                  
														ptRotationCenter += pt;
														pObj->RotateByPoint(ptRotationCenter,0);
														//移动对象
														pObj->MoveOffset(pt);

														pObj->SetIsZoom(FALSE);
													}

													((CWBDocument *)pObj)->GetSrcCenterPt();
													pObj->GetCenterPoint(g_ptDocRotCenter);
													pObj->RotateByCenter(60);
												}
												pObj->SetAlpha(alpha);
												pObj->SetSelectFrameShow(FALSE);
											}
										} 
										else if (pObj->GetType() == WB_OBJ_PICTURE && ((CWBPicture *)pObj)->GetIsPrintScreen())
										{
											CWBObject *pPicCom = ((CWBPicture *)pObj)->GetSelectComObj();
											if (pPicCom != NULL)
											{
												pPicCom->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
												CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
												pPicCom->MoveOffset(ptOnce);
												if ((int)offsetDescRate[index] > 0)
												{
													pPicCom->RotateByCenter(60);
												}
												pPicCom->SetAlpha(alpha);
												if (pPicCom->GetType() == WB_OBJ_CURVE)
												{
													((CWBCurve*)pPicCom)->SetIsDraw();
												}
											}
											else
											{
												if ((int)offsetDescRate[index] > 0)
												{
													((CWBPicture *)pObj)->GetSrcAnnPt();

													//当对象缩放且旋转后，因坐标系变化原因会产生偏移，在这里纠正偏移
													if (pObj->GetIsZoom() && pObj->GetRotationAngle() != 0)
													{
														CPoint pt;
														CWBSelectRectangle *pWBSelectRectangle = pObj->GetObjSelectFrame();
														CPoint ptRotationCenter;
														pObj->GetCenterPoint(ptRotationCenter);
														pWBSelectRectangle->GetMapCenter(pt);
														pt -= ptRotationCenter;
														//使对象中心点偏移                                  
														ptRotationCenter += pt;
														pObj->RotateByPoint(ptRotationCenter,0);
														//移动对象
														pObj->MoveOffset(pt);

														pObj->SetIsZoom(FALSE);
													}

													((CWBPicture *)pObj)->GetSrcCenterPt();
													pObj->GetCenterPoint(g_ptDocRotCenter);
													pObj->RotateByCenter(60);
												}
												pObj->SetAlpha(alpha);
												pObj->SetSelectFrameShow(FALSE);
											}
										}
										else
										{
											pObj->Zoom(pptCenter[i],zoomRate[index],zoomRate[index]);
											CPoint ptOnce(pptOffset[i].x * (int)offsetDescRate[index] , pptOffset[i].y * (int)offsetDescRate[index]);
											pObj->MoveOffset(ptOnce);
											if (offsetDescRate[index] > 0)
											{
												pObj->RotateByCenter(60);
											}
											pObj->SetAlpha(alpha);
											pObj->SetSelectFrameShow(FALSE);
										}
									}
								}
							}
							if (stepNumber > 1)
							{
								if (index != stepNumber - 1)
								{
									alpha -= (initialAlpha / 2) / (stepNumber - 1);
								}
							}
							index = index + 1;
							Invalidate();
							UpdateWindow();
							Sleep(3);
						}
					}
					bSendMoveMsg = false;

					delete [] pptCenter;
					delete [] pptOffset;
					DeleteSelectedObjects();
					//g_nDrawStatus = TODUSTBIN_OVER;
					g_nDrawStatus = DRAW_NORMAL;
					Invalidate();
					UpdateWindow();
				}
				// add by xiewenxiu 20111130 end
			}
			if (true == bSendMoveMsg)
			{
				AddComListToObj();
				if (point != m_moveFirstPoint)
				{
					//MoveMsg(moveOffset);
					MoveMsg(moveOffset,GenerateSSRC());
				}	
			}
		}			
		break;

	case DRAW_NORMAL:
		{
		}
		break;
	case DRAW_RECTMAQN://处理放大镜
		{
			if(MouseUpMagn(point))
			{
				return;
			}
		}
		break;
	case  MOVE_DUSTBIN:
		{
			if (NULL != m_pWBDustbin)
			{
				ReleaseCapture();//释放当前文档对象对鼠标的捕获，SetCapture()函数获取对鼠标的捕获				
				m_IsMoveDustbin = FALSE;
				g_nDrawStatus = m_nDrawStatus;
				Invalidate();
				UpdateWindow();
				break;
			}
		}
		break;
	case DRAW_ROUND:
		//m_draw = DRAW_NORMAL;
		break;
	case DRAWING_POLYLINE://正在画任意线(已经画完)
		{
			CWBCurve *pCurve = (CWBCurve *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_CURVE);
			if (pCurve != NULL)
			{
				//wangxiaobin由于点击一下点太小了，看不到，所以扩大点击只有一点时候点的大小
				if (CalDistancePointToPoint(point,m_ptPointLineCurve) <5.0)
				{
					point.x += g_nWBCurObjWidth;
				}
				pCurve->SetAlpha((int)g_nTranspar);
				pCurve->AddPoint(point);					
				pCurve->AdjustObject();
				pCurve->SetIsDraw();//判断是否重画wangxiaobin
				pCurve->CreateSelecrtFrame();

				pCurve->SetResume_broken_taransfer(false);

				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();

				//发送同步消息
				XMemIni xmlObj;
				xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_NONE);
				pCurve->SetXmlValue(xmlObj);
				AddObj(xmlObj);

				//判断是否添加到文档里
				AddComponentToObj(pCurve, GenerateSSRC());
			}
			SetTimer(ID_TIEMR_STARTGIF,10,NULL);
			g_nDrawStatus = DRAW_POLYLINE; 
		}
		break;
	 
	//add by jiangchao 2013.4.5
	case DRAWING_POLYICON: //正在画任意按钮(已经画完)
		{
			CWBIcoCurve * pIcoCurve = (CWBIcoCurve*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_ICOCURVE);
			if(pIcoCurve != NULL)
			{
				pIcoCurve->SetAlpha(g_nTranspar);
				pIcoCurve->AdjustObject();
				pIcoCurve->CreateSelecrtFrame();
				pIcoCurve->SetIsRefush(TRUE);
				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				//发送同步消息
				pIcoCurve->SetResume_broken_taransfer(false);
				XMemIni xmlObj;
				xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_NONE);

				if(g_bIsWBLoadIcoFromFile) //从文件加载
				{
					xmlObj.Set(KEY_EVENT_ID,m_ulIcoCurEvent);
					xmlObj.Set(KEY_FILE_ID, m_dwIcoCurId);
				}

				pIcoCurve->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_POLYICON;
		}
		break;
	case DRAWING_MAGICLINE://正在画魔术线(已经画完)
		{
			CWBMagicPen *pMagicPen = (CWBMagicPen *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_MAGICPEN);
			if (pMagicPen != NULL)
			{
				//wangxiaobin由于点击一下点太小了，看不到，所以扩大点击只有一点时候点的大小
				if (CalDistancePointToPoint(point,m_ptPointLineCurve) <5.0)
				{
					point.x += g_nWBCurObjWidth;
				}
				pMagicPen->SetAlpha((int)g_nTranspar);
				pMagicPen->AddPoint(point);					
				pMagicPen->AdjustObject();
				pMagicPen->SetIsDraw();//判断是否重画wangxiaobin
				pMagicPen->CreateSelecrtFrame();

				pMagicPen->SetResume_broken_taransfer(false);

				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
					
				//发送同步消息
				XMemIni xmlObj;
				xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_NONE);
				pMagicPen->SetXmlValue(xmlObj);
				AddObj(xmlObj);

				g_pWBPage->DelWBObj(m_dwCurObjLayerIndex);
				(g_pWBPage->m_pMagicList).push_back(pMagicPen);
			}
			SetTimer(ID_TIEMR_MAGICPEN, 1000, NULL);
			g_nDrawStatus = DRAW_MAGICLINE;
		}
		break;
	case DRAWING_RECT:
		{
			CWBRectangle *pRect = (CWBRectangle *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_RECT);
			if (pRect != NULL)
			{
				pRect->Move(point);
				pRect->AdjustObject();

				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				//发送同步消息
				XMemIni xmlObj;
				pRect->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}                               
		}

		g_nDrawStatus = DRAW_RECT;
		break;
	case DRAWING_ELLIPSE:
		{
			CWBEllipse *pEllipse = (CWBEllipse *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_ELLIPSE);
			if (pEllipse != NULL)
			{
				pEllipse->Move(point);	
				pEllipse->AdjustObject();

				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				//发送同步消息
				XMemIni xmlObj;
				pEllipse->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}				
			g_nDrawStatus = DRAW_ELLIPSE;
		}
		break;
	case DRAWEUP_TEXT:
		{
			Invalidate();
			/*UpdateWindow();*/
			g_nDrawStatus = DRAW_NORMAL;	
		}
		break;
	case DRAWING_TEXT:
		{
			if (!CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				PostMessage(UWM_CREATE_RICHEDIT, (WPARAM)(MOUSE_ID), (LPARAM)(MAKELONG(point.x, point.y)));
			}
			SetPageIsNeedDraw(TRUE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			g_nDrawStatus = DRAWED_TEXT;
			m_bUseToControlDraw = true;
			Invalidate();
			/*UpdateWindow();*/
		}
		break;
	case DRAWING_TRIANGLE:		//三角形
		{
			CWBTriangle *pTriangle = (CWBTriangle *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_TRIANGLE);
			if (pTriangle != NULL)
			{
				pTriangle->Move(point);	
				pTriangle->AdjustObject();

				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				//发送同步消息
				XMemIni xmlObj;
				pTriangle->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}                               
			g_nDrawStatus = DRAW_TRIANGLE;		
		}
		break;		
	case DRAW_POLYGON:		//多边形
		{		
			CWBPolygon *pPolygon = (CWBPolygon *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_POLYGON);
			if (pPolygon != NULL)
			{
				pPolygon->AddPoint(point);	
				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
			} 
			g_nDrawStatus = DRAWING_POLYGON;				
		}							
		break;	
	case DRAWING_POLYGON:		
		{
			CWBPolygon *pPolygon = (CWBPolygon *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_POLYGON);
			if(point.x < m_topLeft.x+10 && point.x>m_topLeft.x-10 &&point.y < m_topLeft.y+10 && point.y>m_topLeft.y-10)
			{	
				g_nDrawStatus = DRAW_POLYGON;
				pPolygon->DeletePoint();
				pPolygon->SetClosed(TRUE);
				//pPolygon->SetFillColor(RGB(0, 123, 34));
				Invalidate();	
				UpdateWindow();
				//发送同步消息
				XMemIni xmlObj;
				pPolygon->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			else
			{					
				if (pPolygon != NULL)
				{
					pPolygon->AddPoint(point);	

					SetPageIsNeedDraw(TRUE);
					Invalidate();
					UpdateWindow();
				}     
			}
		}
		break;
	case DRAWING_RHOMBUS:
		{
			CWBRhombus *pRhombus = (CWBRhombus *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_RHOMBUS);
			if (pRhombus != NULL)
			{
				pRhombus->Move(point);
				pRhombus->AdjustObject();
				Invalidate();
				UpdateWindow();
				//发送同步消息
				XMemIni xmlObj;
				pRhombus->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
		}
		g_nDrawStatus = DRAW_RHOMBUS;
		break;
	case DRAWING_PALLELOGAM: //画平行四边形
		{
			CWBPallelogam *pPallelogam = (CWBPallelogam*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_PALLELOGAM);
			if (pPallelogam != NULL)
			{
				pPallelogam->Move(point);
				pPallelogam->AdjustObject();
				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				XMemIni xmlObj;
				pPallelogam->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_PALLELOGAM;
		}
		break;
	case DRAWING_PENTAGON: //画五边形
		{
			CWBPentagon *pPentagon = (CWBPentagon*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_PENTAGON);
			if (pPentagon != NULL)
			{
				pPentagon->Move(point);
				pPentagon->AdjustObject();
				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				XMemIni xmlObj;
				pPentagon->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_PENTAGON;
		}
		break;
	case DRAWING_HEXAGON: //画六边形
		{
			CWBHexagon *pHexagon = (CWBHexagon*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_HEXAGON);
			if (pHexagon != NULL)
			{
				pHexagon->Move(point);
				pHexagon->AdjustObject();
				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				XMemIni xmlObj;
				pHexagon->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_HEXAGON;
		}
		break;
	case DRAWING_FIVEPOINTSTART: //画五角星
		{
			CWBFivepointstart *pFivepointstart = (CWBFivepointstart*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_FIVEPOINTSTART);
			if (pFivepointstart != NULL)
			{
				pFivepointstart->Move(point);
				pFivepointstart->AdjustObject();
				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				XMemIni xmlObj;
				pFivepointstart->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_FIVEPOINTSTART;
		}
		break;
	case DRAWING_TRAPEZIUM: //画梯形
		{
			CWBTrapezium *pTrapezium = (CWBTrapezium*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_TRAPEZIUM);
			if (pTrapezium != NULL)
			{
				pTrapezium->Move(point);
				pTrapezium->AdjustObject();
				SetPageIsNeedDraw(TRUE);
				Invalidate();
				UpdateWindow();
				XMemIni xmlObj;
				pTrapezium->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			g_nDrawStatus = DRAW_TRAPEZIUM;
		}
		break;
	case DRAW_ERASING: //擦除
		{				
			XMemIni xmlobj;
			xmlobj.Set(KEY_BROADCAST_CMD,_E_BROAD_ERASE_UP);
			xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
			Broadcast_Data(xmlobj);

			//ÉèÖÃ·¢ËÍÊÂ¼þID
			m_ulSendEventID = GenerateSSRC();

			EraseLineMsg(); //擦除直线协同操作
			for (int i = 0; i < (int)m_vecLineLayerIndexBeforeErase.size(); ++i )
			{
				if (m_vecLineLayerIndexBeforeErase[i] != NULL)
				{
					delete m_vecLineLayerIndexBeforeErase[i];
				}
			}
			m_vecLineLayerIndexBeforeErase.clear();

			EraseCurveMsg();//擦除曲线协同操作
			EraseDocMsg(); //擦除文档中的曲线同步
			SetPageIsNeedDraw(TRUE);

			g_nDrawStatus = DRAW_ERASE;
			Invalidate();
			UpdateWindow();
		}			
		break;
	case DRAW_CLEANUPING: //正在清除
		{

			//发送同步信息
			//DeleteObjMsg();
			//SetPageIsNeedDraw(TRUE);
			g_nDrawStatus = DRAW_CLEANUP;
			/*Invalidate();	*/			
		}			
		break;
	case DRAW_RGNDELETING: //区域删除
		{
			if (m_pRgnDeleteWBCurve != NULL)
			{
				m_pRgnDeleteWBCurve->AddPoint(point);

				CRgn *pRgn = new CRgn();
				if (m_pRgnDeleteWBCurve->CreateRgn(pRgn))
				{
					g_pWBPage->DeleteWBObjects(pRgn);
				}
				delete pRgn;

				delete m_pRgnDeleteWBCurve;
				m_pRgnDeleteWBCurve = NULL;

				//发送同步消息
				DeleteObjMsg();
				EraseDocMsg(); //擦除文档中的曲线同步
				Invalidate();	
				UpdateWindow();

			}
			SetTimer(ID_TIEMR_STARTGIF,10,NULL);
			g_nDrawStatus = DRAW_RGNDELETE;
		}
		break;
	case ENTER_BUTTON://图片按钮
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				CWBPage *pPage = g_pWBFile->GetPage(m_nPrePage);
				if (pPage == NULL)
				{
					return;
				}
				pObj = pPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			}
			if (pObj != NULL)
			{
				m_bIsTouch = FALSE;
				if (pObj->GetImageButton())
				{
					(pObj->GetImageButton())->ButtonUp(point);
				}
				else
				{
					OutputDebugString(L"pObj->GetImageButton() is Null \n");
				}

			}
			g_nDrawStatus = DRAW_NORMAL;
			if (g_pWhiteBoardView == NULL)
			{
				return;
			}
			Invalidate();
			UpdateWindow();
		}
		break;
	case ENTER_CONTROL://控制按钮
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				CWBPage *pPage = g_pWBFile->GetPage(m_nPrePage);
				if (pPage == NULL)
				{
					return;
				}
				pObj = pPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			}
			if (pObj != NULL)
			{
				CWBObject *pComObj = pObj;
				if (pObj->GetType() == WB_OBJ_COMBINE)
				{
					CWBObject *pCtrlObj = ((CWBCombine *)pObj)->GetControlObj();
					pObj = pCtrlObj;
				}
				(pObj->GetControlButton())->ButtonUp(point);

				//如果是组合里的对象就发送组合的同步消息
				if (pComObj->GetType() == WB_OBJ_COMBINE)
				{
					XMemIni xmlObj;
					pComObj->SetXmlValue(xmlObj);
					ChangeObj(xmlObj);
				}
			}

			//g_nDrawStatus = DRAW_NORMAL;
			if (DXF_PREDRAG == g_nDrawStatus) //add by jiangchao 2013.08.23
			{
				SetCursor(LoadCursor(NULL,IDC_HAND));
			}
			else if (g_nDrawStatus == MIDI_DRAG)
			{

			}
			else
			{
				g_nDrawStatus = DRAW_NORMAL;
			}
			if (g_pWhiteBoardView == NULL)
			{
				return;
			}
			Invalidate();
			UpdateWindow();
		}
		break;
	case CHANGE_ANNOTATIONS:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurAnnotationsLayerIndex);
			if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档
			{
				m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject *pDoc = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
				if (pDoc != NULL)
				{
					if (pDoc->GetType() == WB_OBJ_DOCUMENT)
					{
						pObj = pDoc;
					}
					if (pDoc->GetType() == WB_OBJ_MIDI)
					{
						pObj = pDoc;
					}
				}
			}
			if (pObj != NULL)
			{

				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
				Invalidate();
				UpdateWindow();
			}
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case MEDIA_CTRLING:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				g_nDrawStatus = DRAW_NORMAL;
				return;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				((CWBMedia*)pObj)->GetMediaCtrl()->CtrlUp(point);
			}
			if (NULL == g_pWhiteBoardView)
			{
				return ;
			}
			g_nDrawStatus = DRAW_NORMAL;
			Invalidate();
			UpdateWindow();
		}
		break;
	case MOVE_TRAPEZIUMPOINT:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurTrapeziumLayerIndex);
			if (pObj != NULL)
			{
				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
				Invalidate();
				UpdateWindow();
			}
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case DXF_DRAG:
		{
			CWBObject* pObj=g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if(pObj != NULL)
			{
				if (WB_OBJ_DXF == pObj->GetType())
				{
					XMemIni xmlObj;
					pObj->SetXmlValue(xmlObj);
					ChangeObj(xmlObj);
				}
			}
			g_nDrawStatus=DXF_PREDRAG;
		}
		break;
	case DXF_PREDRAG:
		break;
	default:
		break;;
	}
	if (bIgnoreTouchOrMouse)
	{
		nFlags &=  ~MK_WB_ULBUTTON;
	}
	if (g_nMethodOfResToView == DRAG_AND_DROP)
	{
		//SetCurrentCursorPos();
		AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
	}

	if (g_bRunWater == TRUE)
	{
		CPoint ptSend = point;
		if (g_reduce>0.00)
		{
			point.x = point.x*g_reduce;
			point.y = point.y*g_reduce;
		}
		int nIndex;
		POSITION pos;
		CWBObject *pWBObj;
		CRect viewRectInScreen;
		CObList *pObjList = NULL;
		GetWindowRect(&viewRectInScreen);
		pObjList = g_pWBPage->GetWBObjList();
		if (pObjList == NULL)
		{
			return;
		}
		int nCount =pObjList->GetCount();
		for (nIndex = 0; nIndex < nCount; ++nIndex )
		{
			pos = pObjList->FindIndex(nIndex);
			if (pos == NULL)
			{
				continue;
			}
			pWBObj = (CWBObject *)pObjList->GetAt(pos);
			if (pWBObj == NULL)
			{
				continue;
			}
			if (pWBObj->GetType() ==WB_OBJ_MEDIA)
			{
				CRect rc = ((CWBMedia*)pWBObj)->GetMediaRect();						
				rc.left = rc.left - WATERDLG_HEIGHT/2 + getXnPos() + viewRectInScreen.left;
				rc.top = rc.top - WATERDLG_HEIGHT/2 + getYnPos() + viewRectInScreen.top;
				rc.right = rc.right + WATERDLG_HEIGHT/2 + getXnPos() + viewRectInScreen.left;
				rc.bottom = rc.bottom + WATERDLG_HEIGHT/2 + 95 + getYnPos() + viewRectInScreen.top;
				if (rc.PtInRect(point))
				{
					return;
				}
			}
			else if (pWBObj->GetType() == WB_OBJ_FLASH)
			{
				CRect rc = ((CWBFlash*)pWBObj)->GetCurRect();
				rc.left = rc.left - WATERDLG_HEIGHT/2;
				rc.top = rc.top - WATERDLG_HEIGHT/2;
				rc.right = rc.right + WATERDLG_HEIGHT/2;
				rc.bottom = rc.bottom + WATERDLG_HEIGHT/2;
				if (rc.PtInRect(point))
				{
					return;
				}
			}
			else if (pWBObj->GetType() == WB_OBJ_CAMERA)
			{
				CRect rc = ((CWBCamera*)pWBObj)->GetCaremaRect();
				rc.left = rc.left - WATERDLG_HEIGHT/2 + getXnPos();
				rc.top = rc.top - WATERDLG_HEIGHT/2 + getYnPos();
				rc.right = rc.right + WATERDLG_HEIGHT/2 + getXnPos();
				rc.bottom = rc.bottom + WATERDLG_HEIGHT/2 + 95 + getYnPos() ;
				if (rc.PtInRect(point))
				{
					return;
				}
			}

		}

		CPoint waterpt;
		waterpt.x = point.x - g_pWhiteBoardView->getXnPos();
		waterpt.y = point.y - g_pWhiteBoardView->getYnPos();
		
		waterpt.x += viewRectInScreen.left; 
		waterpt.y += viewRectInScreen.top;
		HDC       hScrDC, hMemDC;       
		HBITMAP  hBitmap,	hOldBitmap;    
		int       nX, nY;      
		int       nWidth, nHeight;
		nHeight = WATERDLG_HEIGHT;
		nWidth = WATERDLG_WIDTH;
		CBitmap *Tempbitmap;  		
		hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);  
		hMemDC = CreateCompatibleDC(hScrDC);
		// 获得选定区域坐标
		nX = waterpt.x - WATERDLG_WIDTH/2;
		nY = waterpt.y - WATERDLG_HEIGHT/2;
		// 创建一个与屏幕设备描述表兼容的位图
		hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
		// 把新位图选到内存设备描述表中
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		BitBlt(hMemDC, 0, 0, nWidth, nHeight,hScrDC, nX, nY, SRCCOPY);
		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hScrDC);
		DeleteDC(hMemDC);
		Tempbitmap = CBitmap::FromHandle(hBitmap);
		if (Tempbitmap == NULL)
		{
			return;
		}
		
		if (m_waterdlg == NULL)
		{			
			m_waterdlg = new CWaterDlg(Tempbitmap,waterpt);		
			m_waterdlg->Create(IDD_WATER_DIALOG,g_pWhiteBoardView);	
			
		}	
		else
		{
			m_waterdlg->SetBackgroundBitmap(Tempbitmap,waterpt);			
		}
		BroadcastUserWater(ptSend,ToStr(g_strAvconUserID.GetBuffer(g_strAvconUserID.GetLength())));
		g_strAvconUserID.ReleaseBuffer();
		DeleteObject(hBitmap);
	}
	m_dwCurOperationObjId = -1;
	CScrollView::OnLButtonUp(nFlags, point);
}
/*************************************************************************
*                    CWhiteBoardView::OnMouseMove()                      *
*               鼠标移动事件，根据m_draw的状态进行操作                   *
*************************************************************************/
void CWhiteBoardView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(g_bIsDrawScreen) //桌面绘图打开时屏蔽白板鼠标事件
	{
		return;
	}
	CRect WBDlgRect;
	::GetWindowRect(this->m_hWnd,&WBDlgRect);
	int x,y;
	x = ::GetSystemMetrics(SM_CXVSCROLL);
	y = ::GetSystemMetrics(SM_CYHSCROLL);
	WBDlgRect.left = WBDlgRect.left + 2;
	WBDlgRect.top = WBDlgRect.top + 2;
	WBDlgRect.right = WBDlgRect.right - x - 2;
	WBDlgRect.bottom = WBDlgRect.bottom - y - 2;
	if (g_nDrawStatus ==DRAWING_LINE||g_nDrawStatus == DRAWING_ELLIPSE||g_nDrawStatus ==DRAWING_PALLELOGAM||g_nDrawStatus ==DRAW_RECTMAQN||g_nDrawStatus ==DRAWING_POLYLINE
		||g_nDrawStatus ==DRAWING_RECT||g_nDrawStatus ==DRAWING_TRIANGLE||g_nDrawStatus ==DRAWING_POLYGON||g_nDrawStatus ==DRAWING_RHOMBUS||g_nDrawStatus ==DRAWING_PENTAGON||g_nDrawStatus ==DRAWING_HEXAGON||g_nDrawStatus ==DRAWING_FIVEPOINTSTART||g_nDrawStatus ==DRAWING_TRAPEZIUM
		||g_nDrawStatus == ENTER_BUTTON||g_nDrawStatus == ENTER_CONTROL||g_nDrawStatus ==DRAWING_TEXT||g_nDrawStatus == DRAW_SELECTINT
		||g_nDrawStatus ==DRAW_ERASING||g_nDrawStatus ==DRAW_CLEANUPING||g_nDrawStatus == DRAW_RGNDELETING||g_nDrawStatus ==CHANGE_ANNOTATIONS
		||g_nDrawStatus == DRAWINGANYTRIANGLE||g_nDrawStatus ==EDITTRIANGLE)
	{
		SCROLLBARINFO sbInfo; 
		ZeroMemory((void*)&sbInfo, sizeof(SCROLLBARINFO)); 
		sbInfo.cbSize = sizeof(SCROLLBARINFO); 
		BOOL bRet2 = GetScrollBarInfo(OBJID_HSCROLL, &sbInfo);
		DWORD dRet = sbInfo.rgstate[0];
		if (!(dRet & STATE_SYSTEM_INVISIBLE))	//有滚动条
		{
			CPoint ptcursor;
			GetCursorPos(&ptcursor);
			if (g_bSupportI7)
			{
				WBDlgRect.right -= 5;
				WBDlgRect.bottom -= 7;
			}
			if (WBDlgRect.PtInRect(ptcursor))
			{
				ClipCursor(&WBDlgRect);	
				m_bLimitCursorArea = true;
			}
			else
			{
				ClipCursor(NULL);
				m_bLimitCursorArea = false;
			}
		}
	}
// 	else
// 	{
// 		ClipCursor(NULL);
// 	}

	//解决：选中白板上的对象，例如曲线，改变其线宽前鼠标移出功能面板，
	//发现线宽输入框输入一个数字后，光标光标就隐藏了。（透明度输入框也是如此）
	SetCursor (LoadCursor (NULL, IDC_ARROW));
	ShowCursor(TRUE);

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock && g_nDrawStatus != DRAW_NORMAL) //如果会议中白板功能被锁定
	{
		g_bAvconLock = FALSE;
		SendMessage(WM_LBUTTONUP, NULL, MAKELPARAM(point.x, point.y));
		g_bAvconLock = TRUE;
		g_nDrawStatus = DRAW_NORMAL;
		
		return;
	}
	

	DWORD dwDistance =  (DWORD)CalDistancePointToPoint(m_ptMovePoint,point);
	if (dwDistance < 2)
	{
		return;
	}
	m_ptMovePoint = point;


	if (g_systemVersion >= 6 && CScreenDrawMgr::GetIns() != NULL)
	{
		///将mousedlg放在screendrawdlg的下面
		CScreenDrawMgr::GetIns()->SetMouseDlgWindowPos();
	}

	BOOL bIgnoreTouchOrMouse = FALSE;
	if (nFlags & MK_WB_ULBUTTON)
	{
		bIgnoreTouchOrMouse = TRUE; //继续往下执行，绕开当前消息是触摸屏消息的判断
	}
	else if((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
	{
		return;
	}

	CClientDC dc(this);
	if(g_reduce>0.00000000000000000)
	{
		ChangeViewCoordinate(&dc);
	}
	else
	{
		OnPrepareDC(&dc);//进行坐标原点的匹配
	}

	dc.DPtoLP(&point);//将视图坐标转换为文档作标	
	if (m_bBroadcastUserCursor&&!g_pWhiteBoardView->GetIsLocalWhiteboard())
	{			
		
		if (!g_strLocalUserName.IsEmpty())
		{
			if (m_pTransparent_dlg == NULL)
			{
				
				std::string strUserid = ToStr(g_strLocalUserName.GetBuffer(g_strLocalUserName.GetLength()));
				char*   pszAvconUserID = new char[strUserid.length() +1]();
				strcpy(pszAvconUserID,strUserid.c_str());
				m_pTransparent_dlg = new CTransparent_Dlg(this,false);
				m_pTransparent_dlg->SetStrname(pszAvconUserID);
				m_pTransparent_dlg->Create(CTransparent_Dlg::IDD,this);

				if (pszAvconUserID)
				{
					delete []pszAvconUserID;
					pszAvconUserID = NULL;
				}
				g_strLocalUserName.ReleaseBuffer();
			}		
			else
			{
				if ((m_pTransparent_dlg->GetUsrName()) == "")
				{
					delete m_pTransparent_dlg;
					m_pTransparent_dlg = NULL;					
				}

			}
			CPoint ptSend = point;			
			CPoint pt = point;
			if (g_reduce>0.00)
			{
				pt.x = pt.x*g_reduce;
				pt.y = pt.y*g_reduce;
			}
			pt.x = pt.x - g_pWhiteBoardView->getXnPos();
			pt.y = pt.y - g_pWhiteBoardView->getYnPos();			
			CRect rectImg;
			m_pTransparent_dlg->GetClientRect(&rectImg);
			POINT _ptBR = pt;
			POINT _ptpoint = pt;
			_ptBR.y -= 28;
			_ptpoint.x += rectImg.Width() ;
			_ptpoint.y += 40;
			rectImg.SetRect(_ptBR,_ptpoint);
			ClientToScreen(&rectImg);
			m_pTransparent_dlg->MoveWindow(&rectImg);
			m_pTransparent_dlg->ShowWindow(SW_SHOW);

			if (m_lmovecounts%10 == 0)
			{
				BroadcastUserCursor(ptSend,ToStr(g_strLocalUserName.GetBuffer(g_strLocalUserName.GetLength())));
				g_strLocalUserName.ReleaseBuffer();
			}
			++m_lmovecounts ;
			if (m_lmovecounts > 10000)
			{
				m_lmovecounts = 0;
			}
		}

	}
	DWORD dwTime_difference = ::timeGetTime();
	if ( dwTime_difference - m_dwTimeGetTime >= MOVEISNEEDSENDDATA_TIME)
	{
		//需要发送同步消息
		m_dwTimeGetTime = dwTime_difference;
		m_bMoveIsNeedSendData = TRUE;
	}

	//处理放大镜
	if(MouseMoveMagn(point))
	{
		return;
	}
	if (m_bStartRecognition &&m_bRecogniMouseDown)
	{
		
		PointRetain pt(point.x,point.y);
		path2D.push_back(pt);
		m_RecShowpoint.push_back(point);
		Invalidate();
		return;
			
		
	}
	if (NULL != m_pWBDustbin && m_IsMoveDustbin == TRUE)
	{
		m_DustbinPoint = point;
		CPoint ptcursor;
		GetCursorPos(&ptcursor);
		if (WBDlgRect.PtInRect(ptcursor))
		{
			ClipCursor(&WBDlgRect);	
			m_bLimitCursorArea = true;
		}
		else
		{
			ClipCursor(NULL);
			m_bLimitCursorArea = false;
		}
		/*ClipCursor(&WBDlgRect);	*/
		Invalidate();
	}

	switch(g_nDrawStatus)
	{
	case MIDI_DRAGING:
		{
			CWBMidi *pwbmidi = (CWBMidi *)g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex,WB_OBJ_MIDI);
			if (pwbmidi != NULL)
			{
				if (pwbmidi->GetObjSelectFrame()->PointIn(point))
				{
					SetCursor(LoadCursor(NULL,IDC_HAND));
					pwbmidi->ChangeShowPosition(point);
					SetPageIsNeedDraw(TRUE);
					Invalidate();
					UpdateWindow();
					SetPageIsNeedDraw(FALSE);


					XMemIni xmlobj;
					pwbmidi->SetXmlValue(xmlobj);
					xmlobj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlobj);
				}
			}
		}
		break;
	case EDITTRIANGLE:
		{

			CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_ARBITRARYTRIANGLE);
			if (pTriangle == NULL)
			{
				return;
			}
			pTriangle->EditTrianglePoint(point);
			XMemIni xmlObj;					
			pTriangle->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
			Broadcast_Data(xmlObj);
			Invalidate();
		}
		break;
	case DRAWINGANYTRIANGLE:
		{
			CWBArbitraryTriangle *pTriangle = (CWBArbitraryTriangle *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_ARBITRARYTRIANGLE);
			if (pTriangle != NULL)
			{
				pTriangle->Move(point);
				pTriangle->SetAlpha((int)g_nTranspar);
// 				if ( m_bMoveIsNeedSendData)
// 				{
					
					XMemIni xmlObj;					
					pTriangle->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);

					//m_bMoveIsNeedSendData = FALSE;
				//}
				Invalidate();
			}
		}
		break;
	case ENTER_PROTRACTOR:
		{
			//CWBProtractor *pWBProtractor = (CWBProtractor*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_PROTRACTOR);
			//g_nDrawStatus = DRAW_NORMAL;
			m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj != NULL)
			{
				((CWBProtractor*)pObj)->MouseMove(point);
				Invalidate();
			}
			bool bInBtn = ((CWBProtractor*)pObj)->GetInButton();
			if (bInBtn)
			{
				return;
			}
			if (pObj)			
			{
				CPoint ptOffset;
				ptOffset = point - m_ptSelect;
				m_dwCurOperationObjId = 0;   //表示操作多个选择框
				if (m_ulMoveCounts %10 == 0)
				{
					CPoint ptOffsetSend = point - m_ptPrePoint;
					m_ptPrePoint = point;
					SendMoveSelectedObjOffset(ptOffsetSend,GenerateSSRC());
				}
				++m_ulMoveCounts ;

				m_ptSelect = point;
				MoveObjToDustbin(point);
			}
		}
		break;
	case ENTER_SETSQUARE:
		{
			m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj != NULL)
			{
				((CWBSetSquare *)pObj)->MouseMove(point);
				Invalidate();
			}
			BOOL bInBtn;
			bInBtn= ((CWBSetSquare *)pObj)->GetIsInBotton();
			if (TRUE == bInBtn)
			{
				return;
			}		
			if (pObj)
			{
				CPoint ptOffset;
				ptOffset = point - m_ptSelect;  
				m_ptSelect = point;
				MoveObjToDustbin(point);
			}
		} 
		break;
	case ENTER_COMPASS:
		{
			m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj != NULL)
			{
				((CWBCompass *)pObj)->MouseMove(point);
				Invalidate();
			}
			BOOL bInBtn = FALSE;
			//bInBtn= ((CWBCompass *)pObj)->GetIsInBotton();
			if (TRUE == bInBtn)
			{
				return;
			}		
			if (pObj)
			{
				CPoint ptOffset;
				ptOffset = point - m_ptSelect;  
				m_ptSelect = point;
				MoveObjToDustbin(point);
			}
		} 
		break;
	case DRAW_SELECTINT: //矩形选择
		{
			//SetPageIsNeedDraw(TRUE);
			m_rectSelect.right = point.x;
			m_rectSelect.bottom = point.y;
			Invalidate();
		}
		break;
	case MOVEING_RULERBTN:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj && (pObj->GetType() == WB_OBJ_RULER))
			{
				((CWBRuler*)pObj)->MoveRulerBtn(point);
				Invalidate();
			}

		}
		break;
	case DRAWING_LINE:
		{
			CWBLine *pLine = (CWBLine *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_LINE);
			if (pLine != NULL)
			{
				pLine->Move(point, FALSE);
				pLine->SetAlpha((int)g_nTranspar);

				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
					pLine->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					
					m_bMoveIsNeedSendData = FALSE;
				}

				int nXScrollPosOffset = getXnPos();
				int nYScrollPosOffset = getYnPos();
				CPoint ptDown = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//DOWN 下的点
				CPoint ptDown2 = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);
				CPoint ptPreMove = m_ptMovePartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//上次移动点
				CPoint ptCurMove = point - CPoint(nXScrollPosOffset, nYScrollPosOffset); //当前的点
				//处理各个方向移动的的的校正
				ChangeLeftTop_RightBottomPoint(ptDown, ptPreMove);
				ChangeLeftTop_RightBottomPoint(ptDown2, ptCurMove);
				//第一、二个矩形  加上线宽
				CRect rcPreRect = CRect(ptDown.x - pLine->m_pItem->nWidth, ptDown.y - pLine->m_pItem->nWidth,
					ptPreMove.x + pLine->m_pItem->nWidth , ptPreMove.y + pLine->m_pItem->nWidth);
				CRect rcCurRect =CRect(ptDown2.x - pLine->m_pItem->nWidth, ptDown2.y - pLine->m_pItem->nWidth,
					ptCurMove.x + pLine->m_pItem->nWidth , ptCurMove.y + pLine->m_pItem->nWidth);
				m_rectRefresh.UnionRect(&rcPreRect, &rcCurRect);
				m_bflag = true;
				InvalidateRect(m_rectRefresh); //局部刷新
				//Invalidate();
			}
		}		
		break;
	case MOVING_LINE:
		{			
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj && (pObj->GetType() == WB_OBJ_MEDIA))
			{
				ClipCursor(NULL);
			}
			else
			{
				CPoint ptcursor;
				GetCursorPos(&ptcursor);
				if (WBDlgRect.PtInRect(ptcursor))
				{
					ClipCursor(&WBDlgRect);	
					m_bLimitCursorArea = true;
				}
				else
				{
					ClipCursor(NULL);
					m_bLimitCursorArea = false;
				}
			}
			if (pObj)
			{
				if (pObj->IsClone())
				{
					if (!pObj->IsLock() && !pObj->IsLimit())
					{
						g_pWBPage->ClearSelectFrameList();
						g_pWBPage->AddSelectFrame(pObj->GetLayerIndex());
						CloneObject();
						pObj->SetClone(FALSE);
					}
				}
				else
				{
					/************************************************************************/
					/*       阳兴平添加2013-07-05                                                               */
					if (pObj->IsLock())
					return;
					/************************************************************************/


					CPoint ptOffset;
					ptOffset = point - m_ptSelect;

					g_pWBPage->MoveSelectedObj(ptOffset);
					m_dwCurOperationObjId = 0;   //表示操作多个选择框
					if (m_ulMoveCounts %10 == 0)
					{
						CPoint ptOffsetSend = point - m_ptPrePoint;
						m_ptPrePoint = point;
						SendMoveSelectedObjOffset(ptOffsetSend,GenerateSSRC());
					}
					++m_ulMoveCounts ;

					m_ptSelect = point;

					if (NULL != m_pWBDustbin) //假如创建了垃圾箱
					{
						int podde = m_pWBDustbin->PointIn(point);						
						CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
						int iSelectObjCounts = pSelectObjLst->GetSize();
						POSITION pos;

						//add by xiewenxiu 20111201 begin
						if(podde != m_PreMousePos)//假如鼠标移入或者移出垃圾桶
						{
							
							CPoint upDustbinOffset(0, -5);
							m_pWBDustbin->MoveOffset(upDustbinOffset);
							m_pWBDustbin->MoveRect(upDustbinOffset);
							Sleep(3);
							Invalidate();
							UpdateWindow();
							CPoint downDustbinOffset(0, 5);
							m_pWBDustbin->MoveOffset(downDustbinOffset);
							m_pWBDustbin->MoveRect(downDustbinOffset);			

							if (MOUSE_OUTSIDE != podde)
							{
								int alpha = 255/2;
								for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
								{
									pos = pSelectObjLst->FindIndex(i);

									pObj = pSelectObjLst->GetAt(pos);
									//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
									if (pObj->IsLock() || 	pObj->IsLimit()) //huanglongquan add
									{
										continue;
									}
									else
									{
										if (pObj->GetType() == WB_OBJ_ICOCURVE)
										{
											((CWBIcoCurve*)pObj)->SetIsRefush(TRUE);
										}
										if (pObj->GetType() == WB_OBJ_CURVE)
										{
											((CWBCurve*)pObj)->SetIsDraw();
										}

										if (pObj->GetType() == WB_OBJ_PICTURE)
										{
											((CWBPicture *)pObj)->SetIsPath();
										}

										if(pObj->GetType() == WB_OBJ_DXF) //判断是否重画DXF
										{
											((CWBDxf *)pObj)->SetIsNeedReDraw(); 
										}

										if (pObj->GetType() != WB_OBJ_COMBINE)
										{
											if (pObj->GetType() == WB_OBJ_DOCUMENT)
											{
												CWBObject *pDocCom = ((CWBDocument *)pObj)->GetSelectComObj();
												if (pDocCom != NULL)
												{
													pDocCom->SetAlpha(alpha);
													if (pDocCom->GetType() == WB_OBJ_CURVE)
													{
														((CWBCurve*)pDocCom)->SetIsDraw();
													}
												} 
												else
												{
													pObj->SetAlpha(alpha);
												}
											} 
											else
											{
												pObj->SetAlpha(alpha);
											}
										}
									}
								}
							}
							else
							{

								for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
								{
									pos = pSelectObjLst->FindIndex(i);

									pObj = pSelectObjLst->GetAt(pos);
									//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
									if (pObj->IsLock() || 	pObj->IsLimit()) //huanglongquan add
									{
										continue;
									}
									else
									{
										if (pObj->GetType() == WB_OBJ_CURVE)
										{
											((CWBCurve*)pObj)->SetIsDraw();
										}

										if (pObj->GetType() == WB_OBJ_PICTURE)
										{
											((CWBPicture *)pObj)->SetIsPath();
										}

										if (pObj->GetType() == WB_OBJ_DXF) //判断是否重画DXF
										{
											((CWBDxf *)pObj)->SetIsNeedReDraw(); 
										}

										if (pObj->GetType() != WB_OBJ_COMBINE)
										{
											if (pObj->GetType() == WB_OBJ_DOCUMENT)
											{
												CWBObject *pDocCom = ((CWBDocument *)pObj)->GetSelectComObj();
												if (pDocCom != NULL)
												{
													pDocCom->SetAlpha(pDocCom->GetPreAlpha());
													if (pDocCom->GetType() == WB_OBJ_CURVE)
													{
														((CWBCurve*)pDocCom)->SetIsDraw();
													}
												} 
												else
												{
													pObj->SetAlpha(pObj->GetPreAlpha());
												}
											} 
											else
											{
												pObj->SetAlpha(pObj->GetPreAlpha());
											}
										}
									}
								}
							}
						
						}
						m_PreMousePos = podde;// add by xiewenxiu 20111201
					}
				}
				//局部刷新
				//if (pObj->GetType() == WB_OBJ_ANNOTATIONS)
				//{
				//	Invalidate();
				//	UpdateWindow();
				//}
				//else
				//{
				//	int nXScrollPosOffset = getXnPos();
				//	int nYScrollPosOffset = getYnPos();
				//	int nHeight, nWidth;
				//	CPoint ptRectRefreshCenter;
				//	int nDiagonal;  //对角线

				//	nWidth  = m_rectRefresh.Width();
				//	nHeight = m_rectRefresh.Height();
				//	ptRectRefreshCenter = CPoint(m_rectRefresh.TopLeft() + CPoint(nWidth/2, nHeight/2))
				//		- CPoint(nXScrollPosOffset, nYScrollPosOffset);


				//	nDiagonal =(int) sqrt(double(nHeight * nHeight + nWidth * nWidth)) + 1;
				//	//计算对象旋转后的矩形范围
				//	CRect TempSelectRect = CRect(CPoint(ptRectRefreshCenter - CPoint(nDiagonal/2, nDiagonal/2))
				//		,CPoint(ptRectRefreshCenter + CPoint(nDiagonal/2, nDiagonal/2)));

				//	CRect TempUnionRect = (0, 0, 0, 0);
				//	TempUnionRect.UnionRect(&m_PreRectRefresh,&TempSelectRect);
				//	TempUnionRect.InflateRect(30, 30); //按钮的宽度

				//	m_bflag = true;
				//	InvalidateRect(TempUnionRect);
				//	UpdateWindow();
				//	m_PreRectRefresh = TempSelectRect;
				//}
				Invalidate();
				UpdateWindow();
			}
		}
		break;
	case DRAW_ROUND:
		break;
	case DRAW_RECTMAQN://处理放大镜
		{
			if(MouseMoveMagn(point))
			{
				return;
			}
		}
		break;
	case DRAWING_POLYLINE:
		{
			CWBCurve *pCurve = (CWBCurve *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_CURVE);
			if (pCurve != NULL)
			{
				UINT nPtNum = pCurve->AddPoint(point);
				pCurve->SetAlpha((int)g_nTranspar);
				HDC hDC = ::GetDC(GetSafeHwnd());
				CPoint ptViewOffset = GetScrollPosition();
				pCurve->DrawLast(hDC, ptViewOffset.x, ptViewOffset.y, nPtNum);
				::ReleaseDC(GetSafeHwnd(),hDC);
				KillTimer(ID_TIEMR_STARTGIF);

				if (m_bMoveIsNeedSendData)
				{
					//发送曲线同步消息
					/*	pCurve->CreateSelecrtFrame();*/  //暂时没有发现这创建选择框的用处  huanglongquan 屏蔽
					//	pCurve->SetIsDraw();
					pCurve->SetResume_broken_taransfer(true);
					XMemIni xmlObj;
					xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
					pCurve->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}
		}
		break;

    //add by jiangchao 2013.4.3
	case DRAWING_POLYICON:
		{
			CWBIcoCurve * pIcoCurve = (CWBIcoCurve *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_ICOCURVE);
			if(pIcoCurve != NULL)
			{		
				pIcoCurve->SetAlpha(g_nTranspar);
				CDC * pDC = GetDC();
				UINT x = g_pWhiteBoardView->getXnPos();
				UINT y = g_pWhiteBoardView->getYnPos();
				pIcoCurve->DrawIcoTrack(pDC, point, x, y);
				ReleaseDC(pDC);

				if (m_bMoveIsNeedSendData)
				{
					//发送曲线同步消息
					pIcoCurve->SetResume_broken_taransfer(true);

					XMemIni xmlObj;
					xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
					pIcoCurve->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);

					if(g_bIsWBLoadIcoFromFile)
					{
						xmlObj.Set(KEY_FILE_ID, m_dwIcoCurId);
					}
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}
		}
		break;
	case DRAWING_MAGICLINE:
		{
			CWBMagicPen *pMagicPen = (CWBMagicPen *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_MAGICPEN);
			if (pMagicPen != NULL)
			{
				UINT nPtNum = pMagicPen->AddPoint(point);
				pMagicPen->SetAlpha((int)g_nTranspar);
				HDC hDC = ::GetDC(GetSafeHwnd());
				CPoint ptViewOffset = GetScrollPosition();
				pMagicPen->DrawLast(hDC, ptViewOffset.x, ptViewOffset.y, nPtNum);
				::ReleaseDC(GetSafeHwnd(),hDC);
				KillTimer(ID_TIEMR_MAGICPEN);

				if (m_bMoveIsNeedSendData)
				{
					pMagicPen->SetResume_broken_taransfer(true);
					XMemIni xmlObj;
					xmlObj.Set(KEY_ADDENDUM_CURVE,_E_DRAWING_ADDENDUMCURVE);
					pMagicPen->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
			}
		}
		break;
	case DRAWING_RECT:
		{
			CWBRectangle *pRect = (CWBRectangle *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_RECT);
			if (pRect != NULL)
			{
				pRect->Move(point);
				pRect->SetAlpha((int)g_nTranspar);
				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
					//pRect->AdjustObject();		//add  by yang
					pRect->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				//区域刷新
				int nXScrollPosOffset = getXnPos();
				int nYScrollPosOffset = getYnPos();
				CPoint ptDown = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//DOWN 下的点
				//CPoint ptDown2 = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);
				CPoint ptPreMove = m_ptMovePartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//上次移动点
				CPoint ptCurMove = point - CPoint(nXScrollPosOffset, nYScrollPosOffset); //当前的点
				//处理各个方向移动的的的校正
				ChangeLeftTop_RightBottomPoint(ptDown, ptPreMove);
				ChangeLeftTop_RightBottomPoint(ptDown, ptCurMove);
				//第一、二个矩形  加上线宽
				CRect rcPreRect = CRect(ptDown.x - pRect->m_pItem->nWidth, ptDown.y - pRect->m_pItem->nWidth,
					ptPreMove.x + pRect->m_pItem->nWidth , ptPreMove.y + pRect->m_pItem->nWidth);
				CRect rcCurRect =CRect(ptDown.x - pRect->m_pItem->nWidth, ptDown.y - pRect->m_pItem->nWidth,
					ptCurMove.x + pRect->m_pItem->nWidth , ptCurMove.y + pRect->m_pItem->nWidth);
				m_rectRefresh.UnionRect(&rcPreRect, &rcCurRect);
// 					DWORD DrawRect = ::GetTickCount();
// 					TRACE("DrawRect = %u\n",DrawRect);
				m_bflag = true;
				InvalidateRect(m_rectRefresh); //局部刷新
				UpdateWindow();
				//Invalidate();
			}
		}

		break;
	case DRAWING_ELLIPSE:
		{
			CWBEllipse *pEllipse = (CWBEllipse *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_ELLIPSE);
			if (m_topLeft != point && pEllipse != NULL)
			{
				pEllipse->Move(point);
				pEllipse->SetAlpha((int)g_nTranspar);
				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
				//	pEllipse->AdjustObject();		//add by yang
					pEllipse->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				//区域刷新
				int nXScrollPosOffset = getXnPos();
				int nYScrollPosOffset = getYnPos();
				CPoint ptDown = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//DOWN 下的点
				CPoint ptDown2 = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);
				CPoint ptPreMove = m_ptMovePartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//上次移动点
				CPoint ptCurMove = point - CPoint(nXScrollPosOffset, nYScrollPosOffset); //当前的点
				//处理各个方向移动的的的校正
				ChangeLeftTop_RightBottomPoint(ptDown, ptPreMove);
				ChangeLeftTop_RightBottomPoint(ptDown2, ptCurMove);
				//第一、二个矩形  加上线宽
				CRect rcPreRect = CRect(ptDown.x - pEllipse->m_pItem->nWidth, ptDown.y - pEllipse->m_pItem->nWidth,
					ptPreMove.x + pEllipse->m_pItem->nWidth , ptPreMove.y + pEllipse->m_pItem->nWidth);
				CRect rcCurRect =CRect(ptDown2.x - pEllipse->m_pItem->nWidth, ptDown2.y - pEllipse->m_pItem->nWidth,
					ptCurMove.x + pEllipse->m_pItem->nWidth , ptCurMove.y + pEllipse->m_pItem->nWidth);
				m_rectRefresh.UnionRect(&rcPreRect, &rcCurRect);
				m_bflag  = true;
				InvalidateRect( m_rectRefresh); //局部刷新
				//Invalidate();
			}				 
		}
		break;
	case DRAWING_TRIANGLE:	
		{
			CWBTriangle *pTriangle = (CWBTriangle *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_TRIANGLE);
			if (pTriangle != NULL)
			{
				pTriangle->Move(point);
				pTriangle->SetAlpha((int)g_nTranspar);
				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
					//pTriangle->AdjustObject();		//add by yang
					pTriangle->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
			
					m_bMoveIsNeedSendData = FALSE;
				}
				//局部刷新
				int nXScrollPosOffset = getXnPos();
				int nYScrollPosOffset = getYnPos();
				CPoint ptDown = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//DOWN 下的点
				CPoint ptDown2 = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);
				CPoint ptPreMove = m_ptMovePartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//上次移动点
				CPoint ptCurMove = point - CPoint(nXScrollPosOffset, nYScrollPosOffset); //当前的点
				//处理各个方向移动的的的校正
				ChangeLeftTop_RightBottomPoint(ptDown, ptPreMove);
				ChangeLeftTop_RightBottomPoint(ptDown2, ptCurMove);
				//第一、二个矩形  加上线宽
				CRect rcPreRect = CRect(ptDown.x - pTriangle->m_pItem->nWidth, ptDown.y - pTriangle->m_pItem->nWidth,
					ptPreMove.x + pTriangle->m_pItem->nWidth , ptPreMove.y + pTriangle->m_pItem->nWidth);
				CRect rcCurRect =CRect(ptDown2.x - pTriangle->m_pItem->nWidth, ptDown2.y - pTriangle->m_pItem->nWidth,
					ptCurMove.x + pTriangle->m_pItem->nWidth , ptCurMove.y + pTriangle->m_pItem->nWidth);
				m_rectRefresh.UnionRect(&rcPreRect, &rcCurRect);
				m_rectRefresh.InflateRect(20,20);
				m_bflag = true;
				InvalidateRect(m_rectRefresh); //局部刷新
				//Invalidate();
			}                               
		}			
		break;
	case DRAWING_POLYGON:		//多边形
		{
			CWBPolygon *pPolygon = (CWBPolygon *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_POLYGON);
			if (pPolygon != NULL)
			{
				pPolygon->Move(point);
				pPolygon->SetAlpha((int)g_nTranspar);
				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
				//	pPolygon->AdjustObject();	//add by yang
					pPolygon->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				Invalidate();
			}                               
		}
		break;
	case DRAWING_RHOMBUS:
		{
			CWBRhombus *pRhombus = (CWBRhombus *)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex, WB_OBJ_RHOMBUS);
			if (pRhombus != NULL)
			{
				pRhombus->Move(point);
				pRhombus->SetAlpha((int)g_nTranspar);
				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
				//	pRhombus->AdjustObject();		//add by yang
					pRhombus->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);

					m_bMoveIsNeedSendData = FALSE;
				}
				//局部刷新
				int nXScrollPosOffset = getXnPos();
				int nYScrollPosOffset = getYnPos();
				CPoint ptDown = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//DOWN 下的点
				CPoint ptDown2 = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);
				CPoint ptPreMove = m_ptMovePartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//上次移动点
				CPoint ptCurMove = point - CPoint(nXScrollPosOffset, nYScrollPosOffset); //当前的点
				//处理各个方向移动的的的校正
				ChangeLeftTop_RightBottomPoint(ptDown, ptPreMove);
				ChangeLeftTop_RightBottomPoint(ptDown2, ptCurMove);
				//第一、二个矩形  加上线宽
				CRect rcPreRect = CRect(ptDown.x - pRhombus->m_pItem->nWidth, ptDown.y - pRhombus->m_pItem->nWidth,
					ptPreMove.x + pRhombus->m_pItem->nWidth , ptPreMove.y + pRhombus->m_pItem->nWidth);
				CRect rcCurRect =CRect(ptDown2.x - pRhombus->m_pItem->nWidth, ptDown2.y - pRhombus->m_pItem->nWidth,
					ptCurMove.x + pRhombus->m_pItem->nWidth , ptCurMove.y + pRhombus->m_pItem->nWidth);
				m_rectRefresh.UnionRect(&rcPreRect, &rcCurRect);
				m_rectRefresh.InflateRect(20,20);
				m_bflag = true;
				InvalidateRect( m_rectRefresh); //局部刷新
				//Invalidate();
			}
		}
		break;
	case DRAWING_PALLELOGAM://画平行四边形
		{
			CWBPallelogam *pPallelogam = (CWBPallelogam*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_PALLELOGAM);
			if (pPallelogam != NULL)
			{
				pPallelogam->Move(point);
				pPallelogam->SetAlpha((int)g_nTranspar);
				if (m_bMoveIsNeedSendData)
				{
					XMemIni xmlObj;
					pPallelogam->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				Invalidate();
			}
		}
		break;
	case DRAWING_PENTAGON: //画五边形
		{
			CWBPentagon *pPentagon = (CWBPentagon*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_PENTAGON);
			if (pPentagon != NULL)
			{
				pPentagon->Move(point);
				pPentagon->SetAlpha((int)g_nTranspar);
				if (m_bMoveIsNeedSendData)
				{
					XMemIni xmlObj;
					pPentagon->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				Invalidate();
			}
		}
		break;
	case DRAWING_HEXAGON: //画六边形
		{
			CWBHexagon *pHexagon = (CWBHexagon*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_HEXAGON);
			if (pHexagon != NULL)
			{
				pHexagon->Move(point);
				pHexagon->SetAlpha((int)g_nTranspar);
				if (m_bMoveIsNeedSendData)
				{
					XMemIni xmlObj;
					pHexagon->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				Invalidate();
			}
		}
		break;
	case DRAWING_FIVEPOINTSTART: //画五角星
		{
			CWBFivepointstart *pFivepointstart = (CWBFivepointstart*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_FIVEPOINTSTART);
			if (pFivepointstart != NULL)
			{
				pFivepointstart->Move(point);
				pFivepointstart->SetAlpha((int)g_nTranspar);
				if (m_bMoveIsNeedSendData)
				{
					XMemIni xmlObj;
					pFivepointstart->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				Invalidate();
			}
		}
		break;
	case DRAWING_TRAPEZIUM: //画梯形
		{
			CWBTrapezium *pTrapezium = (CWBTrapezium*)g_pWBPage->FindByLayerIndex(m_dwCurObjLayerIndex,WB_OBJ_TRAPEZIUM);
			if (pTrapezium != NULL)
			{
				pTrapezium->Move(point);
				pTrapezium->SetAlpha((int)g_nTranspar);
				if (m_bMoveIsNeedSendData)
				{
					XMemIni xmlObj;
					pTrapezium->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}
				Invalidate();
			}
		}
		break;
	case DRAWING_TEXT:		//画编辑框
		{
			SetPageIsNeedDraw(FALSE);
			m_rectText.right = point.x;
			m_rectText.bottom = point.y;

			//局部刷新
			int nXScrollPosOffset = getXnPos();
			int nYScrollPosOffset = getYnPos();
			CPoint ptDown = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//DOWN 下的点
			CPoint ptDown2 = m_ptDownPartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);
			CPoint ptPreMove = m_ptMovePartRefresh - CPoint(nXScrollPosOffset, nYScrollPosOffset);//上次移动点
			CPoint ptCurMove = point - CPoint(nXScrollPosOffset, nYScrollPosOffset); //当前的点
			//处理各个方向移动的的的校正
			ChangeLeftTop_RightBottomPoint(ptDown, ptPreMove);
			ChangeLeftTop_RightBottomPoint(ptDown2, ptCurMove);
			//第一、二个矩形  加上线宽
			CRect rcPreRect = CRect(ptDown.x , ptDown.y,
				ptPreMove.x  , ptPreMove.y);
			CRect rcCurRect =CRect(ptDown2.x , ptDown2.y ,
				ptCurMove.x , ptCurMove.y);
			m_rectRefresh.UnionRect(&rcPreRect, &rcCurRect);
			m_bflag = true;
			InvalidateRect(m_rectRefresh); //局部刷新
			//Invalidate();
		}
		break;
	case DRAW_ERASING: //正在擦除
		{
			//CRect rectErase(CPoint(point.x-10, point.y-10), CPoint(point.x+10, point.y+10));
			/*	if (CalDistancePointToPoint(m_ptErasePre, point) < g_nBrushSize)//如果移动距离小于擦子半径，则返回
			{
			break;
			}*/

			m_rectBrush = CRect(CPoint(point.x-g_nBrushSize, point.y-g_nBrushSize), 
				CPoint(point.x+g_nBrushSize, point.y+g_nBrushSize));
			g_pWBPage->EraseObjects(m_rectBrush);

			Invalidate();

			int left,right,top,botton;
			left = point.x-g_nBrushSize;
			right = point.x+g_nBrushSize;
			top = point.y-g_nBrushSize;
			botton = point.y+g_nBrushSize;

			XMemIni xmlobj;
			xmlobj.Set(KEY_BROADCAST_CMD,_E_BROADE_ERASE_LINE);
			xmlobj.Set(KEY_ERASE_RECT_LEFT,left);
			xmlobj.Set(KEY_ERASE_RECT_RIGHT,right);
			xmlobj.Set(KEY_ERASE_RECT_TOP,top);
			xmlobj.Set(KEY_ERASE_RECT_BOTTON,botton);
			xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
			Broadcast_Data(xmlobj);

		}
		break;
	case DRAW_CLEANUPING: //正在清除
		{
			m_rectBrush = CRect(CPoint(point.x-40, point.y-40), CPoint(point.x+40, point.y+40));
			DWORD dwDelCount;
			dwDelCount = g_pWBPage->DeleteWBObjects(m_rectBrush);
			if (dwDelCount > 0)
			{
				SetPageIsNeedDraw(TRUE);
			}
			DeleteObjMsg();
			Invalidate();

			int left,right,top,botton;
			left = point.x-g_nBrushSize;
			right = point.x+g_nBrushSize;
			top = point.y-g_nBrushSize;
			botton = point.y+g_nBrushSize;

			XMemIni xmlobj;
			xmlobj.Set(KEY_BROADCAST_CMD,_E_BROADE_ERASE_LINE);
			xmlobj.Set(KEY_ERASE_RECT_LEFT,left);
			xmlobj.Set(KEY_ERASE_RECT_RIGHT,right);
			xmlobj.Set(KEY_ERASE_RECT_TOP,top);
			xmlobj.Set(KEY_ERASE_RECT_BOTTON,botton);
			xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
			Broadcast_Data(xmlobj);
		}
		break;
	case DRAW_RGNDELETING: //区域删除
		{
			if (m_pRgnDeleteWBCurve != NULL)
			{
				//wangxiaobin
				UINT nPtNum = m_pRgnDeleteWBCurve->AddPoint(point);
				KillTimer(ID_TIEMR_STARTGIF);
				HDC hDC = ::GetDC(GetSafeHwnd());
				CPoint ptViewOffset = GetScrollPosition();
				m_pRgnDeleteWBCurve->DrawLast(hDC, ptViewOffset.x, ptViewOffset.y, nPtNum);
				::ReleaseDC(GetSafeHwnd(),hDC);	

				//Invalidate();
			}
		}
		break;
	case ADDPICTURE:		//从资源库拖拽出图片
		{			
			if (!PathFileExists(m_sLastDragFileURL/*m_pResource->m_strFileName*/))
			{
				break;
			}
			//清除所有创建的选择框
			g_pWBPage->ClearSelectFrameList();

			//创建一个新对象
			CWBPicture *pWBPicture = new CWBPicture(m_sLastDragFileURL/*m_pResource->m_strFileName*/);			
			pWBPicture->SetAlpha((int)g_nTranspar);
			//控制图片的最大不能超出view的大小4-21金鹏
			CRect viewRect;
			GetClientRect(&viewRect);
			pWBPicture->SetPictureBigerSzie(viewRect);
			g_pWBPage->AddWBObject(pWBPicture);
			CPoint ptCenter,ptOffset;
			//获得对象中心点
			pWBPicture->GetCenterPoint(ptCenter);
			m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
			}
			//g_pObj = pWBPicture;
			ptOffset = point - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
			m_ptSelect = point;
			Invalidate();			


			//发送同步消息
			XMemIni xmlObj;
			pWBPicture->SetXmlValue(xmlObj);
			AddObj(xmlObj);
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;

	case ADDDICOM:
		{
			/*CString strFilePath;*/
			int Dicom_Return = 0 ;

			//清除所有创建的选择框
			g_pWBPage->ClearSelectFrameList();

			//DICOM中的初始化
			CString strFilePath = _T("");
			strFilePath = g_pWhiteBoardView->GetResFilePath();
			char BMPFileName[512] = "";

			if (!PathFileExists(g_pWhiteBoardView->GetResFilePath()))
			{
				g_nDrawStatus = DRAW_NORMAL;
				break;
			}

			pDicomToBmpDll GetDicom;
			GetDicom = (pDicomToBmpDll)GetProcAddress(g_IsMedicalVersion,"DicomToBmpDll");

			if (GetDicom)
				Dicom_Return = GetDicom(strFilePath,g_strAvconTempPath);

			if(Dicom_Return == DICOM_TOBMP_SUCCEES)
			{
				//把转换后的文件名传给strfilepath，以便CWBPicture调用
				GetBmpPath GetDicomBmpPath;
				GetDicomBmpPath = (GetBmpPath)GetProcAddress(g_IsMedicalVersion,"GetDicomBmpPath");

				if (GetDicomBmpPath)
				{
					if ( !GetDicomBmpPath(BMPFileName,512))
					{
						break;;
					}

					strFilePath = BMPFileName;
				}		
			}

			//创建一个新对象
			CWBPicture *pWBPicture = new CWBPicture(strFilePath);			
			pWBPicture->SetAlpha((int)g_nTranspar);
			//控制图片的最大不能超出view的大小4-21金鹏
			CRect viewRect;
			GetClientRect(&viewRect);
			pWBPicture->SetPictureBigerSzie(viewRect);
			g_pWBPage->AddWBObject(pWBPicture);
			CPoint ptCenter,ptOffset;
			//获得对象中心点
			pWBPicture->GetCenterPoint(ptCenter);
			m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
			}
			//g_pObj = pWBPicture;
			ptOffset = point - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
			m_ptSelect = point;
			Invalidate();			
			//发送同步消息
			XMemIni xmlObj;
			pWBPicture->SetXmlValue(xmlObj);
			AddObj(xmlObj);
			g_nDrawStatus = DRAW_NORMAL;
		}
		break;    
	case ADDNEWMOVIE:		//从资源库拖拽出视频
		{	
			//			if (m_pResource == NULL)
			// 			{
			// 				g_nDrawStatus = DRAW_NORMAL;
			// 				break;
			// 			}
			if (!PathFileExists(m_sLastDragFileURL/*m_pResource->m_strFileName*/))
			{
				break;
			}
			//只能主动添加一个视频
			int nPage = g_pWBFile->GetPageCount();
			if (nPage >= 1)
			{
				for (int i =1 ; i <= nPage; ++i)
				{
					CWBPage *pWBPage = g_pWBFile->GetPage(i); 
					CObList* nMediaCameraList = pWBPage->GetMediaCameraList();
					if (nMediaCameraList->GetSize() > 0)
					{
						int nIndex;
						CWBObject * pObj = NULL;
						POSITION pos = NULL;
						for (nIndex = 0; nIndex < nMediaCameraList->GetSize(); ++nIndex)
						{
							pos = nMediaCameraList->FindIndex(nIndex);
							if (pos == NULL)
							{
								continue;
							}
							pObj = (CWBObject *)nMediaCameraList->GetAt(pos);

							if (pObj->GetType() == WB_OBJ_MEDIA &&((CWBMedia*)pObj)->IsActiveCreated())
							{
								CString strSupportOneMedia;
								strSupportOneMedia = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SupportOneMedia"));
								if (strSupportOneMedia.IsEmpty())
								{
									strSupportOneMedia = _T("目前支持主动添加一个视频");
								}
								if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
								{
									//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strSupportOneMedia, MB_OK,true);
									g_pHPWBDataNotify->ShowTipWindow(strSupportOneMedia);
								}
								else
								{
									AfxMessageBox(strSupportOneMedia,MB_OK);
								}
								g_nDrawStatus = DRAW_NORMAL;
								return;
							}
							else
							{
								continue;
							}
						}
					}
				}
			}
			//清除所有创建的选择框
			g_pWBPage->ClearSelectFrameList();
			//创建一个新对象
			CRect WhiteBoardClientRt;
			((CWhiteBoardDlg*)GetParent())->GetWindowRect(&WhiteBoardClientRt);

			CRect rct = CRect( 74, 43, 580, 450 );
			rct.MoveToXY(point.x - rct.Width()/2 - getXnPos(), point.y - rct.Height()/2 - getYnPos());

			CWBMedia *pWBMedia = new CWBMedia(m_sLastDragFileURL, rct);

			g_pWBPage->AddWBObject(pWBMedia);


			//创建选择框，并移动对象到当前位置		
			if (g_pWBPage->AddSelectFrame(pWBMedia->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBMedia->GetLayerIndex();
			}

			Invalidate();

			XMemIni xmlObj;
			xmlObj.Set(KEY_MEDIA_ADDSESSION,g_ulSessionID);
			pWBMedia->SetXmlValue(xmlObj);
			AddObj(xmlObj);

			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case ADDFLASH:		//从资源库拖拽出Flash
		{
			if (!PathFileExists(m_sLastDragFileURL/*m_pResource->m_strFileName*/))
			{
				break;
			}
			//清除所有创建的选择框
			g_pWBPage->ClearSelectFrameList();
			//创建一个新对象
			CWBFlash *pWBFlash = new CWBFlash(m_sLastDragFileURL/*m_pResource->m_strFileName*/);			
			pWBFlash->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pWBFlash);
			pWBFlash->init();
			//获得对象中心点
			CPoint ptCenter,ptOffset;
			pWBFlash->GetCenterPoint(ptCenter);		
			m_moveFirstPoint = ptCenter;//将移动起始点设为中心点

			//创建选择框，并移动对象到当前位置		
			if (g_pWBPage->AddSelectFrame(pWBFlash->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBFlash->GetLayerIndex();
			}
			//g_pObj = pWBFlash;
			ptOffset = point - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
			m_ptSelect = point;
			Invalidate();

			//发送同步消息
			XMemIni xmlObj;
			pWBFlash->SetXmlValue(xmlObj);
			AddObj(xmlObj);

			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case ADDDOC:		//从资源库拖拽出文档
		{
			if (DocConvertTip()) //文档还在转换过程中
			{
				break;
			}
			

			if (!PathFileExists(m_sLastDragFileURL/*m_pResource->m_strFileName*/))
			{
				break;
			}
			//清除所有创建的选择框
			g_pWBPage->ClearSelectFrameList();
			//创建一个新对象
			CWBDocument *pWBDocument = new CWBDocument(m_sLastDragFileURL/*m_pResource->m_strFileName*/);
			if (pWBDocument == NULL)
			{
				break;
			}
			if (g_bSupportU7OpenFile)
			{
				CreateFullScreen(pWBDocument);
			}
			pWBDocument->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pWBDocument);

			int nPageIndex;
			nPageIndex = (int)g_pWBFile->GetCurPageIndex();
			pWBDocument->SetPageIndex(nPageIndex);

			//获得对象中心点
			CPoint ptCenter,ptOffset;
			pWBDocument->GetCenterPoint(ptCenter);		
			m_moveFirstPoint = ptCenter;//将移动起始点设为中心点

			//创建选择框，并移动对象到当前位置		
			if (g_pWBPage->AddSelectFrame(pWBDocument->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBDocument->GetLayerIndex();
			}
			//g_pObj = pWBDocument;
			ptOffset = point - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
			m_ptSelect = point;
			Invalidate();

			//发送同步消息
			XMemIni xmlObj;
			pWBDocument->SetXmlValue(xmlObj);
			AddObj(xmlObj);

			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case ADDDXF:  //从外部拖拽出DXF
		{
			if (!PathFileExists(m_sLastDragFileURL/*m_pResource->m_strFileName*/))
			{
				break;
			}

			//清除所有创建的选择框
			g_pWBPage->ClearSelectFrameList();

			//创建一个新对象
			CWBDxf *pWBDxf = new CWBDxf(m_sLastDragFileURL);
			if (pWBDxf == NULL)
			{
				break;
			}
			pWBDxf->SetAlpha((int)g_nTranspar);
			g_pWBPage->AddWBObject(pWBDxf);
			pWBDxf->SetIsNeedReDraw(); //判断是否重画DXF

			//获得对象中心点
			CPoint ptCenter,ptOffset;
			pWBDxf->GetCenterPoint(ptCenter);		
			m_moveFirstPoint = ptCenter;//将移动起始点设为中心点

			//创建选择框，并移动对象到当前位置		
			if (g_pWBPage->AddSelectFrame(pWBDxf->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBDxf->GetLayerIndex();
			}

			ptOffset = point - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
			m_ptSelect = point;
			Invalidate();

			//发送同步消息
			XMemIni xmlObj;
			pWBDxf->SetXmlValue(xmlObj);
			AddObj(xmlObj);

			g_nDrawStatus = DRAW_NORMAL;
		}
		break;
	case ENTER_BUTTON:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				return;
			}
			if (pObj->GetImageButton())
			{
				//OutputDebugString("pObj->GetImageButton() is have \n");
				(pObj->GetImageButton())->ButtonMove(point);
			}
			else
			{
				OutputDebugString(L"pObj->GetImageButton() is Null \n");
			}
			
			//局部刷新
			if (pObj->GetType() == WB_OBJ_ANNOTATIONS)
			{
				Invalidate();
			}
			else
			{
				int nXScrollPosOffset = getXnPos();
				int nYScrollPosOffset = getYnPos();

				int nHeight, nWidth;
				CPoint ptRectSelCenter; 
				int nDiagonal;  //旋转需要的圆形区域对角线
				int nSquareDiagonalHalf;//对象旋转需要的正方形区域的对角线的
				CPoint ptCenter;
				nWidth  = pObj->m_pRectSelect->Width();
				nHeight = pObj->m_pRectSelect->Height();
				pObj->m_pRectSelect->GetCenterPoint(ptRectSelCenter);

				pObj->ReserveObjPoint(ptRectSelCenter,ptCenter);//获得对象旋转后拉伸对象偏移了的中心点
				ptCenter = ptCenter - CPoint(nXScrollPosOffset, nYScrollPosOffset);//添加滚动条偏移量

				nDiagonal =nHeight * nHeight + nWidth * nWidth;
				nSquareDiagonalHalf = (int) sqrt((double)nDiagonal) + 1;    //获得对角线

				//计算对象旋转后的矩形范围
				CRect RatoteSelectRect = CRect(CPoint(ptCenter - CPoint(nSquareDiagonalHalf/2, nSquareDiagonalHalf/2))
					,CPoint(ptCenter + CPoint(nSquareDiagonalHalf/2, nSquareDiagonalHalf/2)));
				CRect TempUnionRect = CRect(0, 0, 0, 0);
				TempUnionRect.UnionRect(&RatoteSelectRect, &m_PreRectRefresh);
				TempUnionRect.InflateRect(30, 30); //按钮的宽度
				m_bflag = true;
				InvalidateRect(TempUnionRect);
				UpdateWindow();
				m_PreRectRefresh = RatoteSelectRect;
			}

		}
		break;
	case ENTER_CONTROL:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj == NULL)
			{
				return;
			}
			if (pObj->GetType() == WB_OBJ_COMBINE)
			{
				CWBObject *pCtrlObj = ((CWBCombine *)pObj)->GetControlObj();
				pObj = pCtrlObj;
			}
			(pObj->GetControlButton())->ButtonMove(point);
		}
		break;
	case CHANGE_ANNOTATIONS:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurAnnotationsLayerIndex);
			if (pObj == NULL) //如果pObj为NULL，则备注可能属于某一文档
			{
				m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject *pObject = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
				if (pObject != NULL)
				{
					//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
					if (pObject->IsLock() || 	pObject->IsLimit()) //huanglongquan add
					{
						break;
					}
					if (pObject->GetType() == WB_OBJ_DOCUMENT)
					{
						pObj = ((CWBDocument *)pObject)->GetAnn();
					}
					if (pObject->GetType() == WB_OBJ_PICTURE)
					{
						pObj = ((CWBPicture *)pObject)->GetAnn();
					}
					if (pObject->GetType() == WB_OBJ_MIDI)
					{
						pObj = ((CWBMidi*)pObject)->GetAnn();
					}
				}
			}
			if (pObj != NULL)
			{   BOOL bOut = FALSE;
				m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
				CWBObject *pObject = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
				CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
				if (pObject->GetType() == WB_OBJ_DOCUMENT)
				{
					pObj = ((CWBDocument *)pObject)->GetAnn();
					//if (((CWBDocument *)pObject)->ObjectIsComponent(pObj))
					if (((CWBDocument *)pObject)->PtIsComponent(point))
					{
						pAnnotations->SetIsChange(TRUE);
						pAnnotations->SetChangePoint(point);
					}
				}
				else if (pObject->GetType() == WB_OBJ_PICTURE)
				{
					pObj = ((CWBPicture *)pObject)->GetAnn();
					if (((CWBPicture *)pObject)->PtIsComponent(point))
					{
						pAnnotations->SetIsChange(TRUE);
						pAnnotations->SetChangePoint(point);
					}
				}
				else if (pObject->GetType() == WB_OBJ_MIDI)
				{
					pObj = ((CWBMidi*)pObject)->GetAnn();
					if (((CWBMidi*)pObject)->PtIsComponent(point))
					{
						int nheight = 0;
						nheight = ((CWBMidi*)pObject)->GetCopyView();
						pAnnotations->SetIsChange(TRUE);
						
						pAnnotations->SetChangePoint(CPoint(point.x,point.y+nheight));
						((CWBMidi*)pObject)->m_bSetZoomMidi = true;
						((CWBMidi*)pObject)->m_bshowCompont = true;
						((CWBMidi*)pObject)->SetBneedDraw(FALSE);
					}
				}
				else
				{
					pAnnotations->SetIsChange(TRUE);
					pAnnotations->SetChangePoint(point);
				}
				
				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
					pObject->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}

				Invalidate();
			}
		}
		break;
	case DRAWED_TEXT:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	case MOVE_TRAPEZIUMPOINT:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurTrapeziumLayerIndex);
			if (pObj != NULL)
			{
				if (pObj->GetType() == WB_OBJ_TRAPEZIUM)
				{
					((CWBTrapezium *)pObj)->MovePoint(point);
				}

				if ( m_bMoveIsNeedSendData)
				{
					//发送同步消息
					XMemIni xmlObj;
					pObj->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJDRAW_MOVE);
					Broadcast_Data(xmlObj);
					m_bMoveIsNeedSendData = FALSE;
				}

				Invalidate();
			}
		}
		break;
	case DXF_PREDRAG: //add by jiangchao 2013.08.23
		{
			CWBObject* pObj=g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if(pObj != NULL)
			{
				if (WB_OBJ_DXF==pObj->GetType()&&pObj->GetObjSelectFrame()->PointIn(point))
				{
					SetCursor(LoadCursor(NULL,IDC_HAND));
				}
				else
				{
					SetCursor(LoadCursor(NULL,IDC_ARROW));
				}
			}
		}
		break;
	case DXF_DRAG:
		{
			CWBObject* pObj=g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if(pObj != NULL)
			{
				if (WB_OBJ_DXF==pObj->GetType()&&pObj->GetObjSelectFrame()->PointIn(point))
				{
					SetCursor(LoadCursor(NULL,IDC_HAND));
					((CWBDxf*)pObj)->DxfDrag(point);
					((CWBDxf *)pObj)->SetIsNeedReDraw();
					Invalidate();
				}
				else
				{
					SetCursor(LoadCursor(NULL,IDC_ARROW));
					XMemIni xmlObj;
					pObj->SetXmlValue(xmlObj);
					ChangeObj(xmlObj);
					g_nDrawStatus=DXF_PREDRAG;
				}
			}
		}
		break;
	default:
		break;
	}

	m_ptMovePartRefresh = point;

	if (bIgnoreTouchOrMouse)
	{
		nFlags &=  ~MK_WB_ULBUTTON;
	}

	CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);

	if (pObj && (pObj->GetType() == WB_OBJ_FLASH))
	{
		((CWBFlash*)pObj)->GetControlButton()->FlashTip(point);
	}
	if (pObj && (pObj->GetType() == WB_OBJ_MEDIA))
	{
		((CWBMedia*)pObj)->SetMediaCtrlTip(point);
	}
	if (pObj && (pObj->GetType() == WB_OBJ_DOCUMENT))
	{
		((CWBDocument*)pObj)->GetControlButton()->DocTip(point);
	}
	if (pObj && (pObj->GetType() == WB_OBJ_MIDI))
	{
		((CWBMidi*)pObj)->GetControlButton()->MidiTip(point);
	}
	CScrollView::OnMouseMove(nFlags, point);
}


void CWhiteBoardView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	GetClientRect(&m_RectWbSize);

	//
	if (NULL != m_pWBFunctionDlg && m_pWBFunctionDlg->GetPreviewDLG())
	{
		 m_pWBFunctionDlg->GetPreviewDLG()->SetCourrtPrvRatio();
		
	} 
	
	if (m_pEagleEyeDlg &&(nType == SIZE_RESTORED))
	{
		CRect rect;
		m_pEagleEyeDlg->GetClientRect(&rect);
		m_pEagleEyeDlg->ClientToScreen(&rect);

		CRect wb_rect;
		this->GetClientRect(&wb_rect);
		this->ClientToScreen(&wb_rect);
		wb_rect.NormalizeRect();

		rect.NormalizeRect();
		CRect UnRect;
		UnRect.UnionRect(wb_rect,rect);
		if (UnRect != wb_rect)
		{
			CRect EageleRect;
			m_pEagleEyeDlg->GetClientRect(&EageleRect);
			int nEagelEyeX = wb_rect.right - EageleRect.Width()-20;
			int nEagelEyeY = wb_rect.bottom - EageleRect.Height()-20;
			CWBPreviewSingleView* ptempview = m_pEagleEyeDlg->GetPreviewSingleView();
			ptempview->SetRatio();
			ptempview->SetRectInPreview();
			ptempview = NULL;
			/*	CMultiWBFunctionDlg *pDlgFunction =  g_pWhiteBoardView->GetMulWBFunctionDlg();*/
			::SetWindowPos(m_pEagleEyeDlg->GetSafeHwnd(), HWND_TOP, nEagelEyeX, nEagelEyeY, 0, 0, /*SWP_NOMOVE|*/SWP_NOSIZE /*| SWP_HIDEWINDOW*/);
		}
	}
	UINT nFlags = this->IsWindowVisible() ? SW_SHOW : SW_HIDE ;
	for (IT_MapCursorDlgPointer it = m_MapCursorDlgPointer.begin();it != m_MapCursorDlgPointer.end();++it)
	{
		CTransparent_Dlg* pDlg =  (CTransparent_Dlg*)it->second;
		if(pDlg == NULL)
			continue;
		pDlg->ShowWindow(nFlags);
	}

	float fWbWidth = (float)m_RectWbSize.Width();
	float fWbHeight = (float)m_RectWbSize.Height();
	m_fWbRatioX = (float)g_nWidth;
	m_fWbRatioY = (float)g_nHeight;
	m_fWbRatioX = m_fWbRatioX/fWbWidth;
	m_fWbRatioY = m_fWbRatioY/fWbHeight;

	CRect clientRC;
	((CWhiteBoardDlg*)GetParent())->GetClientRect(&clientRC);
	ClientToScreen(&clientRC);

	clientRC.left = clientRC.left + WB_CTRLBTNDLG_WIDTH;
	clientRC.top = clientRC.top + WB_TOOLBOXBTN_SPEC_HEIGHT + 10;
	clientRC.right = clientRC.right   -20;
	clientRC.bottom = clientRC.bottom - 20;

  	if (NULL != m_pWBDustbin&&m_pWBFunctionDlg != NULL)
  	{
// 		bool bshow = m_pWBFunctionDlg->IsWindowVisible();
//   		if (bshow)
//   		{
// 			m_pWBDustbin->MoveOffset(CPoint(-244,0));
//   		}
// 		else
// 		{
// 			m_pWBDustbin->MoveOffset(CPoint(244,0));
// 		}
  	}
	//add by zhuyuyuan begin
	//聚光灯
	if (NULL != m_WBSpotlight)
	{
		try
		{
			//当屏幕大小调整时，调整聚光灯大小
			CRect viewRect;
			GetClientRect(&viewRect);
			//ClientToScreen(&viewRect);
			if(g_reduce>0.00)
			{
				/*double nViewWidth=(double)WB_VIEWHSCROLL*g_reduce;
				double nViewHight=(double)WB_VIEWVSCROLL*g_reduce;

				CRect rc;
				g_whiteboardDlg->GetClientRect(&rc);
				double nWindowWidth;
				double nWindowHight=rc.bottom;*/
				viewRect.left=viewRect.left-110;
				viewRect.top=viewRect.top-100;
				viewRect.bottom=viewRect.bottom-100;
				viewRect.right=viewRect.right-110;//缩放时补上偏差

				
				
			}
			m_WBSpotlight->AdjustSize(viewRect,false);
			//CloseSpotlight();
		}
		catch (CException* e)
		{
			/*CString errorMsg;
			errorMsg.Format(_T("错误位于 CWhiteBoardView::OnSize::聚光灯, 描述：%s"), errorMsg);
			OutputDebugString(errorMsg);*/
		}
	}

	//屏幕录制
	if (NULL != m_MedioVideoScreenDlg)
	{
		try
		{
			if ( m_isFullScreen&&!g_bSupportU7OpenFile)
			{
				//确保窗口从全屏创建后，恢复到小窗口时，屏幕录制窗口不消失。但是会导致窗口变成白板内窗口，不能拖出白板之外
				//因为全屏后白板的父窗口和拥有者是AVCON6中的全屏窗口，而回来后变成模板中的窗口，如果不改变父窗口就会被销毁

				if (m_MedioVideoScreenDlg)//wangxiaobin
				{
					m_MedioVideoScreenDlg->OnFullCancel();
					m_MedioVideoScreenDlg = NULL;
				}
				// m_MedioVideoScreenDlg->SetParent(this);
				AddRECScreen();
			}

			::SetWindowPos(m_MedioVideoScreenDlg->GetSafeHwnd(), HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); 

		}
 		catch (CException* e)
 		{
// 			CString errorMsg;
// 			errorMsg.Format(_T("错误位于 CWhiteBoardView::OnSize::屏幕录制, 描述：%s"), errorMsg);
// 			OutputDebugString(errorMsg);
 		}
	}
	// add by zhuyuyuan end
}

BOOL CWhiteBoardView::SaveLines(CString strFilePath)
{
	if (!g_pWBFile->Save(strFilePath))
	{
		CString strFileSaveError;
		CString strError;
		strError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Error"));
		if (strError.IsEmpty())
		{
			strError = _T("错误");
		}
		strFileSaveError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FileSaveError"));
		if (strFileSaveError.IsEmpty())
		{
			strFileSaveError = _T("文件保存错误！");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify != NULL)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strError, strFileSaveError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFileSaveError);

		}
		else
		{
			MessageBox(strFileSaveError, strError, MB_OK);
		}

		return FALSE;
	}
	return TRUE;
}

void CWhiteBoardView::BroadcastUserWater( CPoint pt,string str_username )
{
	if (!g_pHPWBDataNotify || str_username == "")
	{
		return;
	}
	XMemIni xmlObj;
	xmlObj.Set(KEY_FILE_SENDER,g_ulSessionID);
	xmlObj.Set(KEY_BROADCAST_CMD,_E_WATER_SPECIAL_EFFICIENCY);
	xmlObj.Set(KEY_BROADCASTUSERWATER,_E_SHOWWATER);
	//xmlObj.Set(KEY_WATER_EVENT,1); //second值为1的时候表示：产生水波纹
	std::string str_temp = str_username;
	xmlObj.Set(KEY_WATER_USER, str_temp);
	char pData[256] = {0};
	sprintf_s(pData,"%04d%04d ",pt.x,pt.y);
	str_temp = pData;
	xmlObj.Set(KEY_WATER_POINT,str_temp);
	string strValue ="";
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_BROADCAST_DATA;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}
		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
}
BOOL CWhiteBoardView::LoadLines(CString strFilePath, bool IsCombin)
{
	/*CWBFile **/m_pLoadWBFile = new CWBFile();

	g_PageCount = 0;

	if (NULL == m_pLoadWBFile)
	{
		return FALSE;
	}
	m_bIsCombin = IsCombin;
	//CDataBridge::GetInstance()->GenerateTempInstance();
	if (!m_pLoadWBFile->Load(strFilePath))
	{
		DestoryLoadingPictureDlg();
		WB_AvconLock(false);
		CString strError;
		strError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Error"));
		if (strError.IsEmpty())
		{
			strError = _T("错误");
		}
		CString strFlieLoadError;
		strFlieLoadError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FileLoadError"));
		if (strFlieLoadError.IsEmpty())
		{
			strFlieLoadError = _T("文件读取错误！");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify != NULL)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strError, strFlieLoadError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFlieLoadError);
		}
		else if (!g_bSupportI7)
		{
			MessageBox(strFlieLoadError, strError, MB_OK);
		}	

		m_pLoadWBFile->ClearPageList();
		delete m_pLoadWBFile;
		m_pLoadWBFile = NULL;
		//CDataBridge::GetInstance()->RestoreInstance();
		return FALSE;
	}
	else
	{
		if (!IsCombin) //打开文档
		{
			g_pWBFile->ClearPageList();
			delete g_pWBFile;
			g_pWBFile = m_pLoadWBFile;
			g_pWBPage = NULL;
			m_pLoadWBFile = NULL;
			g_pWhiteBoardView->setCurrentViewScroll(CPoint(0, 0));
			
		}
		else	//合并文档
		{
			CombineFile(g_pWBFile, m_pLoadWBFile);
			//pLoadWBFile = NULL;
			g_pWBPage = NULL;
		}
		g_pWhiteBoardView->FirstPage();
		//g_pWhiteBoardView->setCurrentViewScroll(CPoint(0,0));
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		XMemIni xmlbegin;
		xmlbegin.Set(KEY_BROADCAST_CMD,_E_PROCESS_FILE);
		xmlbegin.Set(KEY_OBJ_LIMIT,0);
		xmlbegin.Set(KEY_FILEPROC_MARCH,KEY_BEGIN_EVENT);
		Broadcast_Data(xmlbegin);
		SendAllObj(); //向服务器发送所有对象
		XMemIni xmlendobj;
		xmlendobj.Set(KEY_BROADCAST_CMD,_E_PROCESS_FILE);
		xmlendobj.Set(KEY_OBJ_LIMIT,0);
		xmlendobj.Set(KEY_FILEPROC_MARCH,KEY_END_EVENT);
		Broadcast_Data(xmlendobj);

		//		CDataBridge::GetInstance()->ExchangeInstance();
	}

	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}

	return TRUE;
}

void CWhiteBoardView::CloseLines()
{
	//	ReleaseWBLine(); 
	//向服务器发送删除所有对象
	// 	if (m_ulRecOldEventID != KEY_EVENT_OPENFILE)
	//	if(!(g_pWhiteBoardView->m_IsOpenFile))
	if(g_pWhiteBoardView->m_bLocalFileOperate)   //如果是本地操作的文件，就发送删除所有对象消息
	{
		//		RemoveAllObj();
		SendMessage(WM_DELALLOBJECT);
	}
	//还原白板打开的开始状态
	BOOL bIsResCreted = WBRESWND::IsGalleryWndCreated();
	if(bIsResCreted)
	{
		WBRESWND::HideGalleryWndEx();
	}

	if (m_MedioVideoScreenDlg)
	{
		delete m_MedioVideoScreenDlg;
		m_MedioVideoScreenDlg = NULL;
	}
	DestroyScreenDraw();
	//SendMessage(WM_CLOSEWBFUNCTIONDLG,0,0);
	if (NULL != m_WBSpotlight)//聚光灯
	{
		delete m_WBSpotlight;
		m_WBSpotlight = NULL;
	}
	if (m_pWBRectMaqn) //放大镜
	{
		delete m_pWBRectMaqn;
		m_pWBRectMaqn = NULL;
	}
	if (m_pWBFunctionDlg)
	{
		m_pWBFunctionDlg->ShowWindow(SW_HIDE);
		if (m_pWBFunctionDlg->GetPreviewDLG())
		{
			m_pWBFunctionDlg->GetPreviewDLG()->ClearAllView();
		}
	}
	g_pWBFile->ClearPageList();

	//CDataBridge::GetInstance()->NewFile();
	/*	g_pWBFile->AddPage(this);*/
	
	if (m_cFile.m_hFile != INVALID_HANDLE_VALUE)
	{
		m_cFile.Close();
	}

	CWBPage* pPage = g_pWBFile->AddPage(this);
	if(IsOpenPreviewPanel())
	{
		CDC *ViewDC = NULL;
		ViewDC =GetDC();
		if (NULL == ViewDC || NULL == ViewDC->m_hDC)
		{
			return ;
		}
		pPage->Draw(ViewDC);
		ReleaseDC(ViewDC);
		//CWBFunctionDlg* ptemfundlg = g_pWhiteBoardView->GetWBFunctionDlg();
		CWBPreviewDlg * pTemp = m_pWBFunctionDlg->GetPreviewDLG();
		if (m_pWBFunctionDlg != NULL && pTemp != NULL)
		{
			::SendMessage(pTemp->m_hWnd,WM_WB_FUNCTION_ADDPREVIEW,(WPARAM)pPage,0);
		}
		//ptemfundlg = NULL;
		pTemp = NULL;
	}
	g_pWBPage = g_pWBFile->GetCurPage();

	//wangxiaobin暂时注释，推出打开文件删除HPDTS会影响文件目录丢失，文件无法保存到临时目录
	//RemoveDir(ToStr(g_strAvconTempPath.GetBuffer(g_strAvconTempPath.GetLength())).c_str());
	Invalidate();
}

void CWhiteBoardView::MovePage(UINT nSrcPageIndex, UINT nDesPageIndex) //移动页
{
	g_pWBFile->MovePage(nSrcPageIndex, nDesPageIndex);
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(FALSE);
	g_pWBPage->StartOrPauseMovie();
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(TRUE);
	g_pWBPage->StartOrPauseMovie();
	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
}
void CWhiteBoardView::AddNewPage()
{
	XAutoLock lock(m_csDraw);
	CWBPage* pPage = g_pWBFile->AddPage(this);
	SetFlashCurPage(FALSE);
	g_pWBPage->StartOrPauseMovie();
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(TRUE);
	g_pWBPage->StartOrPauseMovie();
	//让新建的页滚动条在开始位置
	setCurrentViewScroll(CPoint(0,0));
	m_pageIsNeedDraw = TRUE;
	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}

	if(IsOpenPreviewPanel())
	{
		CWBPreviewDlg* ptemview = m_pWBFunctionDlg->GetPreviewDLG();
		if (m_pWBFunctionDlg && NULL != ptemview)
		{
			::SendMessage(ptemview->m_hWnd,WM_WB_FUNCTION_ADDPREVIEW,(WPARAM)pPage,0);	
		}
		ptemview = NULL;
	}
	//设置页按钮的显示状态
	((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
	//翻页同步
	SendChangePage();
	//新建页时把如果上一页有恢复默认背景按钮，把它隐藏
	if (GetWBFunctionDlg() && GetWBFunctionDlg()->IsWindowVisible() 
		&& GetWBFunctionDlg()->GetAttributeDlg()&& GetWBFunctionDlg()->GetAttributeDlg()->IsWindowVisible())
	{
		GetWBFunctionDlg()->GetAttributeDlg()->m_DefaultBackGround.ShowWindow(SW_HIDE);
	}
	CWBMTObjectManager::GetIns_()->RemoveAllMTObject();

}

void CWhiteBoardView::PrePage()
{
	XAutoLock lock(m_csDraw);
	if (g_pWBFile->PrePage())
	{
		SetFlashCurPage(FALSE);
		g_pWBPage->StartOrPauseMovie();
		g_pWBPage = g_pWBFile->GetCurPage();
		SetFlashCurPage(TRUE);
		g_pWBPage->StartOrPauseMovie();
		setCurrentViewScroll(g_pWBPage->GetPageViewScroll());
		
		Invalidate();
		
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		//设置页按钮的显示状态
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
		//翻转页时把如果上一页有恢复默认背景按钮，把它隐藏
		if (GetWBFunctionDlg() && GetWBFunctionDlg()->IsWindowVisible() 
			&& GetWBFunctionDlg()->GetAttributeDlg()&& GetWBFunctionDlg()->GetAttributeDlg()->IsWindowVisible())
		{
			if (g_pWBPage->GetIsExistBackGround())
			{
				GetWBFunctionDlg()->GetAttributeDlg()->m_DefaultBackGround.ShowWindow(SW_SHOW);
			}
			else
			{
				GetWBFunctionDlg()->GetAttributeDlg()->m_DefaultBackGround.ShowWindow(SW_HIDE);
			}
		}
		//翻页同步
		SendScrollDraw();
		CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
	}
}

void CWhiteBoardView::InsertNewPage()
{
	XAutoLock lock(m_csDraw);
	UINT nCurPage = g_pWBFile->GetCurPageIndex();
	g_pWBFile->InsertPage(this);
	SetFlashCurPage(FALSE);
	g_pWBPage->StartOrPauseMovie();
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(TRUE);
	g_pWBPage->StartOrPauseMovie();
	if (NULL != m_pWBFunctionDlg)
	{
		if (m_pWBFunctionDlg->GetPreviewDLG())
		{
			m_pWBFunctionDlg->GetPreviewDLG()->RefurbishPreview();
		}
		
	}
	//设置页按钮的显示状态
	((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
	//发送同步消息
	XMemIni xmlpage;
	int nForm = g_pWBFile->GetCurPageIndex()-1;
	xmlpage.Set(KEY_OBJ_PAGE,nForm);
	SendInsertPage(xmlpage);
	CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}

}
void CWhiteBoardView::InsertPrePage(UINT nPageIndex)
{
	XAutoLock lock(m_csDraw);
	CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
	g_pWBFile->InsertPage(nPageIndex, this);
	SetFlashCurPage(FALSE);
	g_pWBPage->StartOrPauseMovie();
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(TRUE);
	if (NULL != m_pWBFunctionDlg)
	{
		if (m_pWBFunctionDlg->GetPreviewDLG())
		{
			m_pWBFunctionDlg->GetPreviewDLG()->RefurbishPreview();
		}
		
	}
	g_pWBPage->StartOrPauseMovie();
	//设置页按钮的显示状态
	((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
	//发送同步消息
	XMemIni xmlpage;
	int nForm = nPageIndex;
	xmlpage.Set(KEY_OBJ_PAGE,nForm);
	SendInsertPage(xmlpage);
	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	// 
}
void CWhiteBoardView::DeletePage()
{
	XAutoLock lock(m_csDraw);
	//触摸删除正在操作的对象
	CWBMTObjectManager::GetIns_()->RemoveAllMTObject();

	unsigned long  nDelePage = g_pWBFile->GetCurPageIndex();
	g_pWBFile->DeletePage(this);
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(FALSE);
	g_pWBPage->StartOrPauseMovie();
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(TRUE);
	g_pWBPage->StartOrPauseMovie();
	if (NULL != m_pWBFunctionDlg)
	{
		if (m_pWBFunctionDlg->GetPreviewDLG())
		{
			m_pWBFunctionDlg->GetPreviewDLG()->RefurbishPreview();
		}
		
	}
	XMemIni xmlpage;
	xmlpage.Set(KEY_OBJ_PAGE,nDelePage);
	SendDeletePage(xmlpage);
	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	//设置页按钮的显示状态
	((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
}

void CWhiteBoardView::DeletePrePage(UINT nPageIndex)
{
	XAutoLock lock(m_csDraw);
	//触摸删除正在操作的对象
	CWBMTObjectManager::GetIns_()->RemoveAllMTObject();

	g_pWBFile->DeletePage(nPageIndex, this);
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(FALSE);
	g_pWBPage->StartOrPauseMovie();
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(TRUE);
	g_pWBPage->StartOrPauseMovie();
	if (NULL != m_pWBFunctionDlg)
	{
		if (m_pWBFunctionDlg->GetPreviewDLG())
		{
			m_pWBFunctionDlg->GetPreviewDLG()->RefurbishPreview();
		}
		
	}
	XMemIni xmlpage;
	int nDelePage = nPageIndex;
	xmlpage.Set(KEY_OBJ_PAGE,nDelePage);
	SendDeletePage(xmlpage);
	//设置页按钮的显示状态
	((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
	ASSERT(!m_mapMTDrawPar.size());
	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
}
void CWhiteBoardView::InterchangePage(UINT nFormerPage, UINT nTargetPage)
{
	XAutoLock lock(m_csDraw);
	CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
	g_pWBFile->InterchangePage(nFormerPage, nTargetPage);
	int nForm = nFormerPage;
	int nTo = nTargetPage;
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(FALSE);
	g_pWBPage->StartOrPauseMovie();
	g_pWBPage = g_pWBFile->GetCurPage();
	SetFlashCurPage(TRUE);
	g_pWBPage->StartOrPauseMovie();
	//发送同步消息
	XMemIni xmlpage;
	xmlpage.Set(KEY_OBJ_PAGE,nForm);
	xmlpage.Set(KEY_PAGEINDEX,nTo);
	SendExchangePage(xmlpage);
	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	//设置页按钮的显示状态
	((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
}

void CWhiteBoardView::NextPage()
{
	XAutoLock lock(m_csDraw);
	if (g_pWBFile->NextPage())
	{
		SetFlashCurPage(FALSE);
		g_pWBPage->StartOrPauseMovie();
		g_pWBPage = g_pWBFile->GetCurPage();
		SetFlashCurPage(TRUE);
		g_pWBPage->StartOrPauseMovie();
		setCurrentViewScroll(g_pWBPage->GetPageViewScroll());
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		//设置页按钮的显示状态
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
		//翻转页时把如果上一页有恢复默认背景按钮，把它隐藏
		if (GetWBFunctionDlg() && GetWBFunctionDlg()->IsWindowVisible() 
			&& GetWBFunctionDlg()->GetAttributeDlg()&& GetWBFunctionDlg()->GetAttributeDlg()->IsWindowVisible())
		{
			if (g_pWBPage->GetIsExistBackGround())
			{
				GetWBFunctionDlg()->GetAttributeDlg()->m_DefaultBackGround.ShowWindow(SW_SHOW);
			}
			else
			{
				GetWBFunctionDlg()->GetAttributeDlg()->m_DefaultBackGround.ShowWindow(SW_HIDE);
			}
		}
		CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		//翻页同步
		SendScrollDraw();

	}
}

void CWhiteBoardView::FirstPage()
{
	XAutoLock lock(m_csDraw);
	if (g_pWBFile->FirstPage())
	{
		if (g_pWBPage != NULL)//防止打开文件时错误操作
		{
			SetFlashCurPage(FALSE);
			g_pWBPage->StartOrPauseMovie();
		}

		g_pWBPage = g_pWBFile->GetCurPage();
		SetFlashCurPage(TRUE);
		g_pWBPage->StartOrPauseMovie();
		setCurrentViewScroll(g_pWBPage->GetPageViewScroll());
		CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		//翻页同步
		SendScrollDraw();
	}
	//设置页按钮的显示状态
	if (((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg())
	{
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
	}
	
	if (g_pWBPage == NULL)
	{
		g_pWBPage = g_pWBFile->GetCurPage();
	}
}

void CWhiteBoardView::LastPage()
{
	XAutoLock lock(m_csDraw);
	if (g_pWBFile->LastPage())
	{
		SetFlashCurPage(FALSE);
		g_pWBPage->StartOrPauseMovie();
		g_pWBPage = g_pWBFile->GetCurPage();
		SetFlashCurPage(TRUE);
		g_pWBPage->StartOrPauseMovie();
		setCurrentViewScroll(g_pWBPage->GetPageViewScroll());
		CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		//设置页按钮的显示状态
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
		//翻页同步
		SendScrollDraw();
	}
}


//4-1新添加为了功能面板中预览和白板view同步
void CWhiteBoardView::GotoPage(UINT  nPage)
{
	XAutoLock lock(m_csDraw);
	if (g_pWBFile->GotoPage(nPage))
	{
		SetFlashCurPage(FALSE);
		g_pWBPage->StartOrPauseMovie();
		g_pWBPage = g_pWBFile->GetCurPage();
		SetFlashCurPage(TRUE);
		g_pWBPage->StartOrPauseMovie();
		setCurrentViewScroll(g_pWBPage->GetPageViewScroll());
		CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		//设置页按钮的显示状态
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
		//翻页同步
		//		SendChangePage();
		SendScrollDraw();
	}
}

void CWhiteBoardView::UnderLayer()
{
	if (g_pWBPage->SetObjUnderLayer(m_dwCurSelectObjLayerIndex))
	{
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			Invalidate();
		}
	}
}

void CWhiteBoardView::SuperLayer()
{

	if (g_pWBPage->SetObjSuperLayer(m_dwCurSelectObjLayerIndex))
	{
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			Invalidate();
		}
	}
}

void CWhiteBoardView::TopLayer()
{
	if (g_pWBPage->SetObjTopLayer(m_dwCurSelectObjLayerIndex))
	{
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			Invalidate();
		}
	}
}

void CWhiteBoardView::BottomLayer()
{
	if (g_pWBPage->SetObjBottomLayer(m_dwCurSelectObjLayerIndex))
	{
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			Invalidate();
		}
	}
}

void CWhiteBoardView::SetTextFontColor(const LOGFONT& lfFont, COLORREF clrText)
{
	CFont font;
	if(font.m_hObject)
		font.DeleteObject();
	font.CreateFont(lfFont.lfHeight
		, lfFont.lfWidth, 0,0
		, lfFont.lfWeight
		, lfFont.lfItalic
		, lfFont.lfUnderline
		, lfFont.lfStrikeOut, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,lfFont.lfFaceName);
	CWBRichEdit* ptemEdit = CWBRichEditMgr::GetInst_()->GetRichEdit();
	if (ptemEdit != NULL)
	{
		ptemEdit->SetCurFont(lfFont);
		ptemEdit->SetFontColor(clrText);
	}
	ptemEdit = NULL;
	font.DeleteObject();
}

BOOL CWhiteBoardView::Copy()
{
	int nIndex;
	POSITION pos;
	CWBObject *pWBObj = NULL;
	CWBObject *pLayerObj = NULL;// 记录根据图层号找到的对象地址
	CWBObject *pObj; //存放复制后的对象的指针
	CObList *pObjList = NULL;
	g_pWBPage = g_pWBFile->GetCurPage();

	if (0 == (g_pWBPage->GetWBSelectFrameList()).size())
	{
		return FALSE;
	}

	//清除列表中上一次记录的对象指针
	for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
	{
		if (m_pObjCopyList[i] != NULL)
		{
			delete m_pObjCopyList[i];
			m_pObjCopyList[i] = NULL;
		}
	}
	m_pObjCopyList.clear();


	//向复制的对象列表中添加项
	if (CScreenDrawMgr::GetIns() != NULL)
	{
		CObList *ptemList = g_pWBPage->GetWBLabelObjList();
		for (nIndex = 0; nIndex < ptemList->GetCount(); ++nIndex )
		{
			pos = ptemList->FindIndex(nIndex);
			if (pos == NULL)
			{
				continue;
			}
			pWBObj = (CWBObject *)ptemList->GetAt(pos);
			if (pWBObj == NULL)
			{
				continue;
			}
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pWBObj->IsLock() || pWBObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			if (pWBObj->IsBkg())
			{
				continue;
			}
			if (pWBObj->GetType() == WB_OBJ_CAMERA
				|| pWBObj->GetType() == WB_OBJ_BAFFLE)
			{
				continue;
			}

			for(int i = 0; i < (int)(g_pWBPage->GetWBSelectFrameList()).size(); ++i)
			{
				if (pWBObj->GetLayerIndex() == (g_pWBPage->GetWBSelectFrameList())[i])
				{
					pLayerObj = g_pWBPage->FindByLayerIndex((g_pWBPage->GetWBSelectFrameList())[i]);

					if (NULL == pLayerObj)
					{
						for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
						{
							if (m_pObjCopyList[i] != NULL)
							{
								delete m_pObjCopyList[i];
								m_pObjCopyList[i] = NULL;
							}
						}
						m_pObjCopyList.clear();
						return FALSE;
					}
					// 					if (pLayerObj->IsLock())
					// 					{
					// 						continue;
					// 					}
					//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
					if (pLayerObj->IsLock() || pLayerObj->IsLimit()) //huanglongquan add
					{
						continue;
					}
					if (pLayerObj->GetType() == WB_OBJ_CAMERA)
					{
						continue;
					}
					if (pLayerObj->IsBkg())
					{
						continue;
					}
					if (pLayerObj->GetType() == WB_OBJ_PICTURE && bIsInCamInfoMap(pLayerObj))
					{
						continue;
					}
					switch (pLayerObj->GetType())
					{
					case WB_OBJ_LINE:
						pObj = new CWBLine(*(CWBLine *)pLayerObj);
						break;
					case WB_OBJ_CURVE:
						{
							pObj = new CWBCurve(*(CWBCurve *)pLayerObj);
							CPoint pt;
							((CWBCurve *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ICOCURVE:
						{
							pObj = new CWBIcoCurve(*(CWBIcoCurve *)pLayerObj);
							CPoint pt;
							((CWBIcoCurve *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_POLYGON:
						{
							pObj = new CWBPolygon(*(CWBPolygon *)pLayerObj);
							CPoint pt;
							((CWBPolygon *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_RECT:
						{
							pObj = new CWBRectangle(*(CWBRectangle *)pLayerObj);
							CPoint pt;
							((CWBRectangle *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_PALLELOGAM: //平行四边形
						{
							pObj = new CWBPallelogam(*(CWBPallelogam*)pLayerObj);
							CPoint pt;
							((CWBPallelogam *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_PENTAGON:
						{
							pObj = new CWBPentagon(*(CWBPentagon *)pLayerObj);
							CPoint pt;
							((CWBPentagon *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_HEXAGON:
						{
							pObj = new CWBHexagon(*(CWBHexagon *)pLayerObj);
							CPoint pt;
							((CWBHexagon *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_FIVEPOINTSTART:
						{
							pObj = new CWBFivepointstart(*(CWBFivepointstart *)pLayerObj);
							CPoint pt;
							((CWBFivepointstart *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_TRAPEZIUM:
						{
							pObj = new CWBTrapezium(*(CWBTrapezium *)pLayerObj);
							CPoint pt;
							((CWBTrapezium *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_PICTURE:
						{
							pObj = new CWBPicture(*(CWBPicture *)pLayerObj);
							CPoint pt;
							((CWBPicture *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
							((CWBPicture*)pObj)->SetIsPrintScreen(((CWBPicture *)pLayerObj)->GetIsPrintScreen());
						}
						break;
					case WB_OBJ_MEDIA:
						{
							if (((CWBMedia*)pLayerObj)->IsActiveCreated())
							{
								pObj = new CWBMedia(*(CWBMedia *)pLayerObj);
							}
							else
							{
								continue;
								/*pObj = new CWBMedia(*(CWBMedia *)pLayerObj, FALSE);*/
							}
						}
						break;
					case WB_OBJ_FLASH:
						{
							pObj = new CWBFlash(*(CWBFlash *)pLayerObj);
							CPoint pt;
							((CWBFlash *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_DOCUMENT:
						{
							pObj = new CWBDocument(*(CWBDocument *)pLayerObj);
							CPoint pt;
							((CWBDocument *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_TEXT:
						{
							pObj = new CWBText(*(CWBText *)pLayerObj);
							CPoint pt;
							((CWBText *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ELLIPSE:
						{
							pObj = new CWBEllipse(*(CWBEllipse *)pLayerObj);
							CPoint pt;
							((CWBEllipse *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_TRIANGLE:
						{
							pObj = new CWBTriangle(*(CWBTriangle *)pLayerObj);
							CPoint pt;
							((CWBTriangle *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ARBITRARYTRIANGLE:
						{
							pObj = new CWBArbitraryTriangle(*(CWBArbitraryTriangle *)pLayerObj);
							CPoint pt;
							((CWBArbitraryTriangle *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_RHOMBUS://菱形
						{
							pObj = new CWBRhombus(*(CWBRhombus *)pLayerObj);
							CPoint pt;
							((CWBRhombus *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_COMBINE:
						{
							pObj = new CWBCombine(*(CWBCombine *)pLayerObj);
							CPoint pt;
							((CWBCombine *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ANNOTATIONS:
						pObj = new CWBAnnotations(*(CWBAnnotations *)pLayerObj);
						break;
					case WB_OBJ_DXF:
						{
							pObj = new CWBDxf(*(CWBDxf *)pLayerObj);
							CPoint pt;
							((CWBDxf *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ARC:
						{
							pObj = new CWBArc(*(CWBArc *)pLayerObj);
							CPoint pt;
							((CWBArc *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_SECTOR:
						{
							pObj = new CWBSector(*(CWBSector *)pLayerObj);
							CPoint pt;
							((CWBSector *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;	
					case WB_OBJ_MIDI:
						{
							pObj = new CWBMidi(*(CWBMidi*)pLayerObj);
							CPoint pt;
							((CWBMidi*)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
							((CWBMidi*)pObj)->SetBneedDraw(FALSE);
							((CWBMidi*)pObj)->SetBSetZoomM(true);
						}
						break;
					default:
						for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
						{
							if (m_pObjCopyList[i] != NULL)
							{
								delete m_pObjCopyList[i];
								m_pObjCopyList[i] = NULL;
							}
						}
						m_pObjCopyList.clear();
						return FALSE;
					}

					if (NULL == pObj)
					{
						for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
						{
							if (m_pObjCopyList[i] != NULL)
							{
								delete m_pObjCopyList[i];
								m_pObjCopyList[i] = NULL;
							}
						}
						m_pObjCopyList.clear();
						return FALSE;
					}

					if (pObj->GetType() == WB_OBJ_CURVE)
					{
						CPoint ptLeftTop, ptRightBottom;
						ptLeftTop = ((CWBCurve*)pLayerObj)->GetLeftTop();
						ptRightBottom = ((CWBCurve*)pLayerObj)->GetRightBottom();
						CPoint pt;
						((CWBCurve*)pLayerObj)->GetDrawRotationCenter(pt);
						((CWBCurve*)pObj)->SetDrawRotationCenter(pt);
						((CWBCurve*)pObj)->SetLeftTopRightBottomPt(ptLeftTop, ptRightBottom);
					}

					if (pObj->GetType() == WB_OBJ_ICOCURVE)
					{
						CPoint ptLeftTop, ptRightBottom;
						ptLeftTop = ((CWBIcoCurve*)pLayerObj)->GetLeftTop();
						ptRightBottom = ((CWBIcoCurve*)pLayerObj)->GetRightBottom();
						((CWBIcoCurve*)pObj)->SetLeftTopRightBottomPt(ptLeftTop, ptRightBottom);
					}

					if (pObj->GetType() != WB_OBJ_COMBINE && pObj->GetType() != WB_OBJ_ANNOTATIONS)
					{
						pObj->CreateSelecrtFrame();
					}

					pObj->CopyAttribute(pLayerObj);
					m_pObjCopyList.push_back(pObj);
					break;
				}
			}
		}
	} 
	else
	{
		pObjList = g_pWBPage->GetWBObjList();
		if (pObjList == NULL)
		{
			return FALSE;
		}
		for (nIndex = 0; nIndex < pObjList->GetCount(); ++nIndex )
		{
			pos = pObjList->FindIndex(nIndex);
			if (pos == NULL)
			{
				continue;
			}
			pWBObj = (CWBObject *)pObjList->GetAt(pos);
			if (pWBObj == NULL)
			{
				continue;
			}
			// 			if (pWBObj->IsLock())
			// 			{
			// 				continue;
			// 			}
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pWBObj->IsLock() || pWBObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			if (pWBObj->IsBkg())
			{
				continue;
			}
			if (bIsInCamInfoMap(pWBObj))
			{
				continue;
			}
			if (pWBObj->GetType() == WB_OBJ_CAMERA
				|| pWBObj->GetType() == WB_OBJ_BAFFLE
				|| pWBObj->GetType() == WB_OBJ_PROTRACTOR
				|| pWBObj->GetType() == WB_OBJ_COMPASS)
			{
				continue;
			}

			for(int i = 0; i < (int)(g_pWBPage->GetWBSelectFrameList()).size(); ++i)
			{
				if (pWBObj->GetLayerIndex() == (g_pWBPage->GetWBSelectFrameList())[i])
				{
					pLayerObj = g_pWBPage->FindByLayerIndex((g_pWBPage->GetWBSelectFrameList())[i]);

					if (NULL == pLayerObj)
					{
						for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
						{
							if (m_pObjCopyList[i] != NULL)
							{
								delete m_pObjCopyList[i];
								m_pObjCopyList[i] = NULL;
							}
						}
						m_pObjCopyList.clear();
						return FALSE;
					}
					// 					if (pLayerObj->IsLock())
					// 					{
					// 						continue;
					// 					}
					//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
					if (pLayerObj->IsLock() || pLayerObj->IsLimit()) //huanglongquan add
					{
						continue;
					}
					if (pLayerObj->GetType() == WB_OBJ_CAMERA)
					{
						continue;
					}
					if (pLayerObj->IsBkg())
					{
						continue;
					}

					switch (pLayerObj->GetType())
					{
					case WB_OBJ_LINE:
						pObj = new CWBLine(*(CWBLine *)pLayerObj);
						break;
					case WB_OBJ_CURVE:
						{
							pObj = new CWBCurve(*(CWBCurve *)pLayerObj);
							CPoint pt;
							((CWBCurve *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ICOCURVE:
						{
							pObj = new CWBIcoCurve(*(CWBIcoCurve *)pLayerObj);
							CPoint pt;
							((CWBIcoCurve *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_POLYGON:
						{
							pObj = new CWBPolygon(*(CWBPolygon *)pLayerObj);
							CPoint pt;
							((CWBPolygon *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						
						break;
					case WB_OBJ_RECT:
						{
							pObj = new CWBRectangle(*(CWBRectangle *)pLayerObj);
							CPoint pt;
							((CWBRectangle *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_PALLELOGAM: //平行四边形
						{
							pObj = new CWBPallelogam(*(CWBPallelogam*)pLayerObj);
							CPoint pt;
							((CWBPallelogam *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_PENTAGON:
						{
							pObj = new CWBPentagon(*(CWBPentagon *)pLayerObj);
							CPoint pt;
							((CWBPentagon *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_HEXAGON:
						{
							pObj = new CWBHexagon(*(CWBHexagon *)pLayerObj);
							CPoint pt;
							((CWBHexagon *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_FIVEPOINTSTART:
						{
							pObj = new CWBFivepointstart(*(CWBFivepointstart *)pLayerObj);
							CPoint pt;
							((CWBFivepointstart *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_TRAPEZIUM:
						{
							pObj = new CWBTrapezium(*(CWBTrapezium *)pLayerObj);
							CPoint pt;
							((CWBTrapezium *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_PICTURE:
						{
							pObj = new CWBPicture(*(CWBPicture *)pLayerObj);
							CPoint pt;
							((CWBPicture *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
							((CWBPicture*)pObj)->SetIsPrintScreen(((CWBPicture *)pLayerObj)->GetIsPrintScreen());
						}
						break;
					case WB_OBJ_MEDIA:
						{
							if (((CWBMedia*)pLayerObj)->IsActiveCreated())
							{
								pObj = new CWBMedia(*(CWBMedia *)pLayerObj);
							}
							else
							{
								continue;
								/*pObj = new CWBMedia(*(CWBMedia *)pLayerObj, FALSE);*/
							}
						}
						break;
					case WB_OBJ_FLASH:
						{
							pObj = new CWBFlash(*(CWBFlash *)pLayerObj);
							CPoint pt;
							((CWBFlash *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_DOCUMENT:
						{
							pObj = new CWBDocument(*(CWBDocument *)pLayerObj);
							CPoint pt;
							((CWBDocument *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_TEXT:
						{
							pObj = new CWBText(*(CWBText *)pLayerObj);
							CPoint pt;
							((CWBText *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ELLIPSE:
						{
							pObj = new CWBEllipse(*(CWBEllipse *)pLayerObj);
							CPoint pt;
							((CWBEllipse *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_TRIANGLE:
						{
							pObj = new CWBTriangle(*(CWBTriangle *)pLayerObj);
							CPoint pt;
							((CWBTriangle *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ARBITRARYTRIANGLE:
						{
							pObj = new CWBArbitraryTriangle(*(CWBArbitraryTriangle*)pLayerObj);
							CPoint pt;
							((CWBArbitraryTriangle *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_RHOMBUS://菱形
						{
							pObj = new CWBRhombus(*(CWBRhombus *)pLayerObj);
							CPoint pt;
							((CWBRhombus *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_COMBINE:
						{
							pObj = new CWBCombine(*(CWBCombine *)pLayerObj);
							CPoint pt;
							((CWBCombine *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ANNOTATIONS:
						pObj = new CWBAnnotations(*(CWBAnnotations *)pLayerObj);
						break;
					case WB_OBJ_DXF:
						{
							pObj = new CWBDxf(*(CWBDxf *)pLayerObj);
							CPoint pt;
							((CWBDxf *)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
						}
						break;
					case WB_OBJ_ARC:
						pObj = new CWBArc(*(CWBArc *)pLayerObj);
						break;
					case WB_OBJ_SECTOR:
						pObj = new CWBSector(*(CWBSector *)pLayerObj);
						break;
					case WB_OBJ_MIDI:
						{
							pObj = new CWBMidi(*(CWBMidi*)pLayerObj);
							CPoint pt;
							((CWBMidi*)pLayerObj)->GetDrawRotationCenter(pt);
							pObj->SetDrawRotationCenter(pt);
							((CWBMidi*)pObj)->SetBneedDraw(FALSE);
							((CWBMidi*)pObj)->SetBSetZoomM(true);
						}
						break;
					default:
						for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
						{
							if (m_pObjCopyList[i] != NULL)
							{
								delete m_pObjCopyList[i];
								m_pObjCopyList[i] = NULL;
							}
						}
						m_pObjCopyList.clear();
						return FALSE;
					}

					if (NULL == pObj)
					{
						for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
						{
							if (m_pObjCopyList[i] != NULL)
							{
								delete m_pObjCopyList[i];
								m_pObjCopyList[i] = NULL;
							}
						}
						m_pObjCopyList.clear();
						return FALSE;
					}

					if (pObj->GetType() == WB_OBJ_CURVE)
					{
						CPoint ptLeftTop, ptRightBottom;
						ptLeftTop = ((CWBCurve*)pLayerObj)->GetLeftTop();
						ptRightBottom = ((CWBCurve*)pLayerObj)->GetRightBottom();
						CPoint pt;
						((CWBCurve*)pLayerObj)->GetDrawRotationCenter(pt);
						((CWBCurve*)pObj)->SetDrawRotationCenter(pt);
						((CWBCurve*)pObj)->SetLeftTopRightBottomPt(ptLeftTop, ptRightBottom);
					}

					if (pObj->GetType() == WB_OBJ_ICOCURVE)
					{
						CPoint ptLeftTop, ptRightBottom;
						ptLeftTop = ((CWBIcoCurve*)pLayerObj)->GetLeftTop();
						ptRightBottom = ((CWBIcoCurve*)pLayerObj)->GetRightBottom();
						((CWBIcoCurve*)pObj)->SetLeftTopRightBottomPt(ptLeftTop, ptRightBottom);
					}

					if (pObj->GetType() != WB_OBJ_COMBINE && pObj->GetType() != WB_OBJ_ANNOTATIONS)
					{
						pObj->CreateSelecrtFrame();
					}

					pObj->CopyAttribute(pLayerObj);
					m_pObjCopyList.push_back(pObj);
					break;
				}
			}
		}
	}

	return TRUE;
}

void CWhiteBoardView::CopyObject()
{
	m_bIsCut = FALSE;

	if (!Copy())
	{
		m_bIsCopy = FALSE;
		return;
	}

	m_bIsCopy = TRUE;

	return;
}

void CWhiteBoardView::CutObject()
{
	m_bIsCopy = FALSE;

	if (!Copy())
	{
		m_bIsCut = FALSE;
		return;
	}

	DWORD dwLayerIndex;
	m_ulSendEventID = GenerateSSRC();

	for (int i = 0; i < (int)(g_pWBPage->GetWBSelectFrameList()).size(); ++i)
	{
		dwLayerIndex = (g_pWBPage->GetWBSelectFrameList())[i];
		CWBObject *pObj = g_pWBPage->FindByLayerIndex(dwLayerIndex);
		if (pObj != NULL)
		{
			if (pObj->GetType() == WB_OBJ_COMBINE)
			{
				((CWBCombine *)pObj)->DelComMebMsg();
				g_pWBPage->DeleteSelectFrame(dwLayerIndex);
			}
			if (pObj->GetType() == WB_OBJ_CAMERA)
			{
				continue;
			}
			if (pObj->IsBkg())
			{
				continue;
			}
			if (pObj->GetType() == WB_OBJ_PICTURE && bIsInCamInfoMap(pObj))
			{
				continue;
			}
		}
		if (!g_pWBPage->DeleteWBObject(dwLayerIndex))
		{
			m_bIsCut = FALSE;
			return;
		}
		i--;

		//发送同步消息
		//RemoveObj(dwLayerIndex);
		RemoveDelCombineObj(dwLayerIndex);
	}

	m_bIsCut = TRUE;

	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		Invalidate();
	}
	if (g_pHPWBDataNotify &&  g_bSupportI7)//剪切对象后，发送取消选择的消息
	{
		MSG msg;
		memset(&msg,0,sizeof(MSG));
		msg.hwnd = NULL;
		msg.message = 0;
		msg.wParam = (WPARAM)0;
		msg.lParam = (LPARAM)0;
		m_bSelectStatueChange = 0;
		g_pHPWBDataNotify->SetSelectObjStatue(msg);
	}
	return;
}

BOOL CWhiteBoardView::PasteObject()
{
	CWBObject *pObj; //存放复制后的对象的指针
	unsigned long ulEventID = GenerateSSRC();

	g_pWBPage = g_pWBFile->GetCurPage();
	g_pWBPage->ClearSelectFrameList();

	for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
	{
		if (m_pObjCopyList[i] == NULL)
		{
			continue;
		}
		switch (m_pObjCopyList[i]->GetType())
		{
		case WB_OBJ_LINE:
			pObj = new CWBLine(*(CWBLine *)m_pObjCopyList[i]);
			break;
		case WB_OBJ_CURVE:
			{
				pObj = new CWBCurve(*(CWBCurve *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBCurve *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_ICOCURVE:
			{
				pObj = new CWBIcoCurve(*(CWBIcoCurve *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBPolygon *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_POLYGON:
			{
				pObj = new CWBPolygon(*(CWBPolygon *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBPolygon *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_RECT:
			{
				pObj = new CWBRectangle(*(CWBRectangle *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBRectangle *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_PALLELOGAM: //平行四边形
			{
				pObj = new CWBPallelogam(*(CWBPallelogam*)m_pObjCopyList[i]);
				CPoint pt;
				((CWBPallelogam *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_PENTAGON:
			{
				pObj = new CWBPentagon(*(CWBPentagon*)m_pObjCopyList[i]);
				CPoint pt;
				((CWBPentagon *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_HEXAGON:
			{
				pObj = new CWBHexagon(*(CWBHexagon*)m_pObjCopyList[i]);
				CPoint pt;
				((CWBHexagon *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_FIVEPOINTSTART:
			{
				pObj = new CWBFivepointstart(*(CWBFivepointstart *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBFivepointstart *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_TRAPEZIUM:
			{
				pObj = new CWBTrapezium(*(CWBTrapezium *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBTrapezium *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_PICTURE:
			{
				pObj = new CWBPicture(*(CWBPicture *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBPicture *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
				((CWBPicture*)pObj)->SetIsPrintScreen(((CWBPicture*)m_pObjCopyList[i])->GetIsPrintScreen());
			}
			break;
		case WB_OBJ_MEDIA:
			{
				if (((CWBMedia *)m_pObjCopyList[i])->IsActiveCreated())
				{
					pObj = new CWBMedia(*(CWBMedia *)m_pObjCopyList[i]);
				} 
				else
				{
					continue;
					/*pObj = new CWBMedia(*(CWBMedia *)m_pObjCopyList[i],FALSE);*/
				}
			}
			break;
		case WB_OBJ_FLASH:
			{
				pObj = new CWBFlash(*(CWBFlash *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBFlash *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_DOCUMENT:
			{
				pObj = new CWBDocument(*(CWBDocument *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBDocument *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			if (pObj != NULL)
			{
				int nPageIndex;
				nPageIndex = (int)g_pWBFile->GetCurPageIndex();
				((CWBDocument *)pObj)->SetPageIndex(nPageIndex);
			}
			break;
		case WB_OBJ_TEXT:
			{
				pObj = new CWBText(*(CWBText *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBText *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_ELLIPSE:
			{
				pObj = new CWBEllipse(*(CWBEllipse *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBEllipse *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_TRIANGLE:
			{
				pObj = new CWBTriangle(*(CWBTriangle *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBTriangle *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_ARBITRARYTRIANGLE:
			{
				pObj = new CWBArbitraryTriangle(*(CWBArbitraryTriangle*)m_pObjCopyList[i]);
				CPoint pt;
				((CWBArbitraryTriangle *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_RHOMBUS://菱形
			{
				pObj = new CWBRhombus(*(CWBRhombus *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBRhombus *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_COMBINE:
			{
				pObj = new CWBCombine(*(CWBCombine *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBCombine *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_ANNOTATIONS:
			pObj = new CWBAnnotations(*(CWBAnnotations *)m_pObjCopyList[i]);
			break;
		case WB_OBJ_DXF:
			{
				pObj = new CWBDxf(*(CWBDxf *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBDxf *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
			}
			break;
		case WB_OBJ_ARC:
			pObj = new CWBArc(*(CWBArc *)m_pObjCopyList[i]);
			break;
		case WB_OBJ_SECTOR:
			pObj = new CWBSector(*(CWBSector *)m_pObjCopyList[i]);
			break;
		case WB_OBJ_MIDI:
			{
				pObj = new CWBMidi(*(CWBMidi *)m_pObjCopyList[i]);
				CPoint pt;
				((CWBMidi *)m_pObjCopyList[i])->GetDrawRotationCenter(pt);
				pObj->SetDrawRotationCenter(pt);
				((CWBMidi*)pObj)->SetBneedDraw(FALSE);
				((CWBMidi*)pObj)->SetBSetZoomM(true);
				((CWBMidi*)pObj)->SetBShowCompont(true);
				
			}
			break;
		default:
			return FALSE;
		}

		if (NULL == pObj)
		{
			return FALSE;
		}
		WB_OBJ_TYPE ntemType = (WB_OBJ_TYPE)pObj->GetType();
		if (WB_OBJ_MEDIA == ntemType)
		{
			//如果该多媒体对象的控制对话框是指定为要显示的话，让控制对话框显示
			if ( ((CWBMedia *)m_pObjCopyList[i])->IsActiveCreated())
			{
				if (((CWBMedia *)pObj)->GetFilePath().GetLength() != 0)
				{
					CString strMediaPath = ((CWBMedia *)pObj)->GetFilePath();
					//((CWBMedia *)pObj)->OpenMedia(ToStr(strMediaPath.GetBuffer(strMediaPath.GetLength())).c_str());
					((CWBMedia *)pObj)->OpenMedia(U2A(strMediaPath).c_str());
					
				}
			} 
			else
			{
				((CWBMedia *)pObj)->OpenMedia(NULL, FALSE);
			}

		}

		if (WB_OBJ_FLASH == ntemType)
		{
			((CWBFlash *)pObj)->init();
		}

		if (WB_OBJ_COMBINE == ntemType)
		{
			((CWBCombine *)pObj)->InitFlash();
		}

		pObj->CopyAttribute(m_pObjCopyList[i]);

		if (ntemType == WB_OBJ_CURVE)
		{
			CPoint ptLeftTop, ptRightBottom;
			ptLeftTop = ((CWBCurve*)m_pObjCopyList[i])->GetLeftTop();
			ptRightBottom = ((CWBCurve*)m_pObjCopyList[i])->GetRightBottom();
			((CWBCurve*)pObj)->SetLeftTopRightBottomPt(ptLeftTop, ptRightBottom);
		}
        if (ntemType == WB_OBJ_ICOCURVE)
		{
			CPoint ptLeftTop, ptRightBottom;
			ptLeftTop = ((CWBIcoCurve*)m_pObjCopyList[i])->GetLeftTop();
			ptRightBottom = ((CWBIcoCurve*)m_pObjCopyList[i])->GetRightBottom();
			((CWBIcoCurve*)pObj)->SetLeftTopRightBottomPt(ptLeftTop, ptRightBottom);
		}
		if (ntemType != WB_OBJ_COMBINE && ntemType != WB_OBJ_ANNOTATIONS)
		{
			pObj->CreateSelecrtFrame();
		}
		g_pWBPage->AddWBObject(pObj, pObj->GetLayerType());

		//////////////////////////////////////////////////////
		//录像wangxiaobin2013/8/2
		DWORD dwlayer = pObj->GetLayerIndex();
		if (ntemType == WB_OBJ_CAMERA && pObj)
		{
			CWBCamera *pCamera = (CWBCamera*)pObj;
			if (pCamera && pCamera->GetCameraDlg()
				&& pCamera->GetCameraDlg()->GetCapChanPlayHandle_Empty())
			{
				pCamera->GetCameraDlg()->GetCapChanPlayHandle_Empty()->SetCaremaID(dwlayer);
			}
		}

		if (ntemType == WB_OBJ_MEDIA && pObj)
		{
			CWBMedia *pMeida = (CWBMedia*)pObj;
			if (pMeida && pMeida->GetMediaWindow()
				&& pMeida->GetMediaWindow()->GetCapChanPlayHandleMedia())
			{
				pMeida->GetMediaWindow()->GetCapChanPlayHandleMedia()->SetMedia(dwlayer);
			}
		}
		//////////////////////////////////////////////////////
		if(!(CWBImageButton*)pObj->GetImageButton())
		{
			pObj->InitImageButton();
			pObj->SetSelectFrameShow(FALSE);			   
		}
		((CWBImageButton*)pObj->GetImageButton())->ChangeRationCenterOperator();
		
		g_pWBPage->AddSelectFrame(pObj->GetLayerIndex());

		CPoint point(20,20);
		CRect rect;

		if (m_bIsCopy || m_bIsCut) //如果是复制或粘贴
		{
			CPoint ptOffset = m_ptPaste - LeftTop();

			if (ntemType == WB_OBJ_MEDIA)
			{
				//滚动条偏移量
				ptOffset.x -= g_pWhiteBoardView->getXnPos();
				ptOffset.y -= g_pWhiteBoardView->getYnPos();
			}
			//CPoint newRationpt;
			//((CWBObject*)m_pObjCopyList[i])->GetRotationCenter(newRationpt);
			//((CWBImageButton*)pObj->GetImageButton())->SetObjRationCenterpt(newRationpt);
			//使对象中心点偏移
			//CPoint ptRotationCenter;
			//pObj->GetRotationCenter(ptRotationCenter);
			//ptRotationCenter += ptOffset;
			CPoint Drawpt = CPoint(0,0);
			pObj->GetDrawRotationCenter(Drawpt);
			Drawpt += ptOffset;
			pObj->SetDrawRotationCenter(Drawpt);
			pObj->SetRotationCenter(Drawpt);
			pObj->RotateByPoint(Drawpt,0);
			//((CWBImageButton*)pObj->GetImageButton())->SetObjRationCenterpt(ptRotationCenter);
			
			if (ntemType == WB_OBJ_CURVE || ntemType == WB_OBJ_POLYGON || ntemType == WB_OBJ_ICOCURVE)
			{
				CPoint ptTemp;
				pObj->GetZoomCenter(ptTemp);
				ptTemp += ptOffset;
				pObj->SetZoomCenter(ptTemp);
			}
			pObj->MoveOffset(ptOffset);
			
			if (ntemType == WB_OBJ_MEDIA)
			{
				rect = ((CWBMedia *)pObj)->GetMediaRect();
				rect.left = rect.left + ptOffset.x;
				rect.right = rect.right + ptOffset.x;
				rect.top = rect.top + ptOffset.y;
				rect.bottom = rect.bottom + ptOffset.y;
			}
		}
		else
		{
			if (!pObj->IsInfiniteClone()) //如果是克隆
			{
				//使对象中心点偏移
				CPoint ptRotationCenter;
				pObj->GetRotationCenter(ptRotationCenter);
				ptRotationCenter += point;
				CPoint Drawpt = CPoint(0,0);
				pObj->GetDrawRotationCenter(Drawpt);
				Drawpt += point;
				pObj->SetDrawRotationCenter(Drawpt);
				pObj->SetRotationCenter(Drawpt);
				pObj->RotateByPoint(Drawpt,0);

				if (ntemType == WB_OBJ_CURVE || ntemType == WB_OBJ_POLYGON)
				{
					CPoint ptTemp;
					pObj->GetZoomCenter(ptTemp);
					ptTemp += point;
					pObj->SetZoomCenter(ptTemp);
				}
				pObj->MoveOffset(point);

				if (ntemType == WB_OBJ_MEDIA)
				{
					rect = ((CWBMedia *)pObj)->GetMediaRect();
					rect.left = rect.left + point.x;
					rect.right = rect.right + point.x;
					rect.top = rect.top + point.y;
					rect.bottom = rect.bottom + point.y;
				}
				//g_pWBPage->MoveSelectedObj(point);
			}
		}

		pObj->SetInfiniteClone(FALSE);
		g_pObj = pObj;

		if (ntemType == WB_OBJ_CURVE)//曲线重画问题wangxiaobin
		{
			((CWBCurve*)pObj)->SetIsDraw();
		}
		if (ntemType == WB_OBJ_ICOCURVE)
		{
			((CWBIcoCurve*)pObj)->SetIsRefush(TRUE);
		}
		//发送同步消息
		XMemIni xmlObj;

		if (ntemType == WB_OBJ_COMBINE)
		{
			((CWBCombine *)pObj)->SetComXmlValue(ulEventID);
		}

		if (ntemType == WB_OBJ_MEDIA)
		{
			SetCloneMedia(TRUE);
		}
		if (ntemType == WB_OBJ_CAMERA)
		{
           xmlObj.Set(KEY_CAREMA_ADDSESSION,g_ulSessionID);
		}
		pObj->SetXmlValue(xmlObj);
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
		
		if (pObj->GetType() == WB_OBJ_MEDIA)
		{
			xmlObj.Set(KEY_MEDIA_ADDSESSION,g_ulSessionID);
			xmlObj.Set(KEY_OBJ_RECT_LEFT, rect.left);
			xmlObj.Set(KEY_OBJ_RECT_TOP, rect.top);
			xmlObj.Set(KEY_OBJ_RECT_RIGHT, rect.right);
			xmlObj.Set(KEY_OBJ_RECT_BOTTOM, rect.bottom);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
		}
		AddObj(xmlObj);
	}
	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		Invalidate();
	}
	if (pObj && g_bSupportI7)
	{
		CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
		int iSelectObjCounts = pSelectObjLst->GetSize();
		if (iSelectObjCounts == 0)
			return TRUE;
		CObList * ptemplist = g_pWBPage->GetSelectObjLst();
		POSITION pos;
		for (int i = 0; i < ptemplist->GetSize(); ++i)//遍历选中对象列表
		{
			pos = ptemplist->FindIndex(i);
			if (pos == NULL)
				continue;
			CWBObject* pObject = (CWBObject*)ptemplist->GetAt(pos);
			if (NULL != pObject)
			{
				pObject->SetSelectFrameShow(FALSE);
			}
		}
	}

	return TRUE;
}

BOOL CWhiteBoardView::CloneObject()
{
	CopyObject();

	if (!m_bIsCopy)
	{
		return FALSE;
	}

	m_bIsCopy = FALSE;

	PasteObject();

	return TRUE;
}

void CWhiteBoardView::InfiniteCloneObj()
{
	m_dwCurSelectObjLayerIndex = g_pWBPage->GetCurEditingObjLayerIndex();
	CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
	if (NULL == pObj)
	{
		return;
	}
	if (pObj->GetType() == WB_OBJ_CAMERA)
	{
		return;
	}

	pObj->SetInfiniteClone(!pObj->IsInfiniteClone());

	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		Invalidate();
	}

	//发送同步消息
	XMemIni xmlObj;
	pObj->SetXmlValue(xmlObj);
	if (pObj->GetType() == WB_OBJ_COMBINE)
	{
		ChangeObj(xmlObj,true);
	}
	else
	{
		ChangeObj(xmlObj);
	}

}

CPoint CWhiteBoardView::LeftTop()
{
	std::vector<CPoint> point; //记录 复制或剪切的对象指针列表 每个对象最左最上的点
	CPoint lefttop = (0,0);

	for(int i = 0; i < (int)m_pObjCopyList.size(); ++i)
	{
		if (m_pObjCopyList[i] == NULL)
		{
			continue;
		}
		CPoint pt;

		if (WB_OBJ_LINE == m_pObjCopyList[i]->GetType())
		{
			WBLineItem ObjectItem;
			((CWBLine *)m_pObjCopyList[i])->GetItem(&ObjectItem); 

			pt.x = (ObjectItem.ptBegin.x < ObjectItem.ptEnd.x ? ObjectItem.ptBegin.x : ObjectItem.ptEnd.x);
			pt.y = (ObjectItem.ptBegin.y < ObjectItem.ptEnd.y ? ObjectItem.ptBegin.y : ObjectItem.ptEnd.y);
		}

		else
		{
			if (m_pObjCopyList[i]->GetObjSelectFrame() == NULL)
			{
				m_pObjCopyList[i]->CreateSelecrtFrame();
			}
			CPoint ptLeftTop = (m_pObjCopyList[i]->GetObjSelectFrame())->GetLeftTop();

			pt.x = ptLeftTop.x;
			pt.y = ptLeftTop.y;
		}

		CPoint ptReserve, ptRotationCenter;

		//根据对象的旋转角度，换算鼠标击中的点
		m_pObjCopyList[i]->GetRotationCenter(ptRotationCenter);
		PointRotationPoint(ptRotationCenter, m_pObjCopyList[i]->GetRotationAngle(), pt, ptReserve);

		point.push_back(ptReserve);
	}

	if ((int)point.size() <= 0)
	{
		return lefttop;
	}
	lefttop.x = point[0].x;
	lefttop.y = point[0].y;
	for(int i = 1; i < (int)point.size(); ++i)
	{
		if ( point[i].x < lefttop.x)
		{
			lefttop.x = point[i].x;
		}
		if ( point[i].y < lefttop.y)
		{
			lefttop.y = point[i].y;
		}
	}
	point.clear();

	return lefttop;
}

BOOL CWhiteBoardView::GetIsCopy()
{
	return m_bIsCopy;
}

BOOL CWhiteBoardView::GetIsCut()
{
	return m_bIsCut;
}

void CWhiteBoardView::SetCurAnnotationsLayerIndex(DWORD dwLayerIndex)
{
	m_dwCurAnnotationsLayerIndex = dwLayerIndex;
}

void CWhiteBoardView::SetCurTrapeziumLayerIndex(DWORD dwLayerIndex)
{
	m_dwCurTrapeziumLayerIndex = dwLayerIndex;
}

LOGFONT CWhiteBoardView::GetCurFont()
{
	if (CWBRichEditMgr::GetInst_()->GetRichEdit() != NULL)
	{
		return (CWBRichEditMgr::GetInst_()->GetRichEdit())->GetCurFont();
	}

	LOGFONT fontlog;
	memset(&fontlog, 0, sizeof(LOGFONT));
	fontlog.lfHeight = 20;
	fontlog.lfCharSet = 134;
	_tcscpy(fontlog.lfFaceName, L"宋体");
	return fontlog;
}

COLORREF CWhiteBoardView::GetFontColor()
{
	if (CWBRichEditMgr::GetInst_()->GetRichEdit() != NULL)
	{
		return (CWBRichEditMgr::GetInst_()->GetRichEdit())->GetFontColor();
	} 
	return 0;
}

LRESULT CWhiteBoardView::OnWbCreateRichEdit( WPARAM wParam, LPARAM lParam )
{
	CWnd *pParent = NULL;
	if(CScreenDrawMgr::GetIns() != NULL) //如果是在桌面绘图，则将文本框加载CMouseDlg上，否则加在CWhiteBoardView上
	{
		///设置文本框的父窗口,如果是win7 vista以上则是mousedlg,否则为screendrawdlg
		CScreenDrawMgr::GetIns()->SetRichEditParent(&pParent);
	}
	else
	{
		pParent = this;
	}
	const unsigned long nTouchID = (const unsigned long)(wParam);
	CWBRichEditMgr::GetInst_()->SetEndPoint(LOWORD(lParam), HIWORD(lParam), nTouchID);
	CWBRichEditMgr::GetInst_()->ShowWBRichEdit(nTouchID, g_bIsVertical, pParent, true);
	return S_OK;
}


LRESULT CWhiteBoardView::OnWbDestroyRichText( WPARAM wParam, LPARAM lParam )
{
	CWBRichEditMgr::GetInst_()->DestroyRichWindow();
	return S_FALSE;
}


LRESULT CWhiteBoardView::OnWbEditRichText( WPARAM wParam, LPARAM lParam )
{
	CWnd *pParent = NULL;
	if(NULL != CScreenDrawMgr::GetIns()) //如果是在桌面绘图，则将文本框加载CMouseDlg上，否则加在CWhiteBoardView上
	{
		CScreenDrawMgr::GetIns()->SetRichEditParent(&pParent);
	}
	else
	{
		pParent = this;
	}

	CWBText *pText = NULL;
	CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
	if (pObj->GetType() == WB_OBJ_ANNOTATIONS)
	{
		pText = ((CWBAnnotations*)pObj)->GetText();
	} 
	else
	{
		pText = (CWBText*)pObj;
	}
	PWBTextItem pTextItem = new WBTextItem;
	pText->GetItem(pTextItem);

	//当前字体的信息
	g_clrWBCurObjColor = pTextItem->clrText;
	g_nWBCurObjFontFaceName = pTextItem->fontItem.lfFaceName;
	g_nWBCurObjFontSize = pTextItem->fontItem.lfHeight;
	if (pTextItem->fontItem.lfWeight == FW_BOLD)
	{
		g_isBold = TRUE;					
	}
	if (pTextItem->fontItem.lfWeight == FW_NORMAL)
	{
		g_isBold = FALSE;					
	}
	g_isItalic = pTextItem->fontItem.lfItalic;
	g_isUnderline = pTextItem->fontItem.lfUnderline;
	CWBRichEditMgr* ptemMGR = CWBRichEditMgr::GetInst_();
	//ptemMGR->SetBeginPoint(LOWORD(wParam), HIWORD(wParam), MOUSE_ID);
	if (pTextItem->rectText.left<100)
	{
		pTextItem->rectText.right = pTextItem->rectText.right - pTextItem->rectText.left +100;
		pTextItem->rectText.left =100;
	}
	if (pTextItem->rectText.right>2400)
	{
		pTextItem->rectText.left = pTextItem->rectText.left - pTextItem->rectText.right + 2400; 
		pTextItem->rectText.right = 2400;
	}
	ptemMGR->SetBeginPoint(pTextItem->rectText.left,pTextItem->rectText.top,MOUSE_ID);
	//ptemMGR->SetEndPoint(LOWORD(lParam), HIWORD(lParam), MOUSE_ID);
	ptemMGR->SetEndPoint(pTextItem->rectText.right,pTextItem->rectText.bottom,MOUSE_ID);
	ptemMGR->ShowWBRichEdit(MOUSE_ID, g_bIsVertical, pParent, true);
	ptemMGR->SetRichEditText(pTextItem->strText);
	ptemMGR = NULL;
	pText->SetIsVertical(g_bIsVertical);
	delete pTextItem;
	CWBRichEdit* ptemRichEdit = GetCurEdit();
	if (NULL != ptemRichEdit)
	{
		int iFontWeight;
		if (g_isBold)
		{
			iFontWeight = FW_BOLD;
		}
		else
		{
			iFontWeight = FW_NORMAL;
		}
		CFont font;
		if (font.GetSafeHandle())
		{
			font.Detach();
		}
		font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
		ptemRichEdit->SetFont(&font);

		SetEditCharFormat(ptemRichEdit);
		SetCurTextFont();
		font.DeleteObject();
	}
	ptemRichEdit->SetFocus();
	(ptemRichEdit->GetRichEditCtrl()).SetSel(-1, -1);//光标到末尾

	ptemRichEdit = NULL;
	return S_OK;
}

HBRUSH CWhiteBoardView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CScrollView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if(pWnd->GetDlgCtrlID()==1 && nCtlColor == CTLCOLOR_EDIT)
	{
		COLORREF clrCurTextColor = RGB(0,0,0);
		CWBRichEdit* ptempRich = CWBRichEditMgr::GetInst_()->GetRichEdit();
		if (ptempRich != NULL)
		{
			clrCurTextColor = ptempRich->GetFontColor();
			pDC->SetTextColor(clrCurTextColor);
		}
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
///打印的时候调用的在设备上画页
HBITMAP CWhiteBoardView:: GetSrcBit(HDC hDC,DWORD BitWidth, DWORD BitHeight)//lvjie
{     
	// HDC hDC=::GetDC(NULL);
	HDC hBufDC;
	HBITMAP hBitmap, hBitTemp;
	//创建设备上下文(HDC)
	hBufDC = CreateCompatibleDC(hDC);
	//创建HBITMAP
	hBitmap = CreateCompatibleBitmap(hDC, BitWidth, BitHeight);
	hBitTemp = (HBITMAP) SelectObject(hBufDC, hBitmap);
	//得到位图缓冲区
	StretchBlt(hBufDC, 0, 0, BitWidth, BitHeight,hDC, 0, 0, BitWidth, BitHeight, SRCCOPY);
	//得到最终的位图信息
	hBitmap = (HBITMAP) SelectObject(hBufDC, hBitTemp);
	//释放内存
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);
	::ReleaseDC(NULL,hDC); 
	return hBitmap;
}

HBITMAP CWhiteBoardView::GetPictureBit(HDC hDC, DWORD BitTop, DWORD BitBottom, DWORD BitLeft, DWORD BitRight)
{
	HDC hBufDC;
	HBITMAP hBitmap, hBitTemp;
	DWORD BitWidth, BitHeight;
	BitWidth = BitRight - BitLeft;
	BitHeight = BitBottom - BitTop;

	//创建设备上下文(HDC)
	hBufDC = CreateCompatibleDC(hDC);
	//创建HBITMAP
	hBitmap = CreateCompatibleBitmap(hDC, BitWidth, BitHeight);
	hBitTemp = (HBITMAP) SelectObject(hBufDC, hBitmap);
	//得到位图缓冲区
	StretchBlt(hBufDC, 0, 0, BitWidth, BitHeight, hDC, BitLeft, BitTop, BitWidth, BitHeight, SRCCOPY);
	//得到最终的位图信息
	hBitmap = (HBITMAP) SelectObject(hBufDC, hBitTemp);
	//释放内存
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);
	::ReleaseDC(NULL,hDC); 
	return hBitmap;
}

//lvjie
int CWhiteBoardView::SaveBitmapToFile(HBITMAP hBitmap, CString lpFileName) //hBitmap 为刚才的屏幕位图句柄
{ //lpFileName 为位图文件名
	HDC hDC; 
	//设备描述表
	int iBits; 
	//当前显示分辨率下每个像素所占字节数
	WORD wBitCount; 
	//位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ， 位图文件大小 ， 写入文件字节数
	DWORD dwPaletteSize=0,dwBmBitsSize,dwDIBSize, dwWritten;
	BITMAP Bitmap; 
	//位图属性结构
	BITMAPFILEHEADER bmfHdr; 
	//位图文件头结构
	BITMAPINFOHEADER bi; 
	//位图信息头结构 
	LPBITMAPINFOHEADER lpbi; 
	//指向位图信息头结构
	HANDLE fh, hDib, hPal;
	HPALETTE hOldPal=NULL;
	//定义文件，分配内存句柄，调色板句柄
	//计算位图文件每个像素所占字节数
	hDC = CreateDC(L"DISPLAY",NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * 
		GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 32;
	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);
	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;
	//为位图内容分配内存
	/*xxxxxxxx计算位图大小分解一下(解释一下上面的语句)xxxxxxxxxxxxxxxxxxxx 
	//每个扫描行所占的字节数应该为4的整数倍，具体算法为:
	int biWidth = (Bitmap.bmWidth*wBitCount) / 32;
	if((Bitmap.bmWidth*wBitCount) % 32)
	biWidth++; //不是整数倍的加1
	biWidth *= 4;//到这里，计算得到的为每个扫描行的字节数。
	dwBmBitsSize = biWidth * Bitmap.bmHeight;//得到大小
	xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/

	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	if (!hDib)
	{
		CString strError;
		strError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Error"));
		if (strError.IsEmpty())
		{
			strError = _T("错误");
		}
		CString strCannotPrint;
		strCannotPrint = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CannotPrint"));
		if (strCannotPrint.IsEmpty())
		{
			strCannotPrint = _T("还在写数据，不能打印！");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strError, strCannotPrint, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strCannotPrint);
		}
		else
		{
			MessageBox(strCannotPrint, strError, MB_OK);
		}		
		return 0;
	}
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板 
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值
	GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize, (BITMAPINFO *)lpbi,DIB_RGB_COLORS);
	//恢复调色板 
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	//创建位图文件 
	fh=CreateFile(lpFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh==INVALID_HANDLE_VALUE)
		return FALSE;
	// 设置位图文件头
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize; 
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	// 写入位图文件头
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容
	WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize , &dwWritten, NULL); 
	//清除 
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

//打印的时候调用的在设备上画页
BOOL CWhiteBoardView:: DeleteDirectory(CString DirName)  //lvjie
{//DirName += '\0';

	SHFILEOPSTRUCT FileOp ;
	FileOp.fFlags = FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = DirName;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE;

	BOOL nResult = SHFileOperation(&FileOp);

	return TRUE;

}
//lvjie
void CWhiteBoardView::PrintPage(/*CDC *pDC,BOOL m_IsPrint,int m_ScreenX,int m_ScreenY,CWBPage * pWBPrintPage*/)
{   
	CWBPage *pWBPrintPage;
	UINT PageContr;
	TCHAR szPath[MAX_PATH];

	GetModuleFileName(NULL, szPath, MAX_PATH); 

	CString CurrentstrPath = szPath;
	int nPos; 
	nPos = CurrentstrPath.ReverseFind('\\'); 
	CurrentstrPath = CurrentstrPath.Left(nPos); 
	CString pathBmp;
	// AfxMessageBox(strPath);
	// int s=strlen(strPath);
	//int s2=s-12;
	//path0= strPath.Mid( 0,s2);
	pathBmp= CurrentstrPath + L"\\HPDTS\\PrintPicture";
	for(PageContr = 1;PageContr <= g_pWBFile->GetPageCount();++PageContr)
	{
		pWBPrintPage = g_pWBFile->GetPage(PageContr);
		HDC hDC=pWBPrintPage->GetMemHDC();

		// CRect rect;
		// GetClientRect(&rect);	
		if(!PathIsDirectory(pathBmp))	 
		{	
			if(!CreateDirectory(pathBmp, NULL))
			{
				//AfxMessageBox("截图文件夹创建失败");
			}		
		}	
		CString pathBmp1;
		pathBmp1.Format(_T("\\%d.bmp"), PageContr);//="D://a//"+time.Format("%Y%m%d %H-%M-%S")+".bmp";
		HBITMAP hBitmap1;

		hBitmap1=GetSrcBit(hDC,2400, 1600);
		if (!SaveBitmapToFile( hBitmap1, (pathBmp+pathBmp1)))
		{
			return;
		}
		//存放路径的变量                       //获取程序的当前目录
	}

	//int s4=strlen(strPath);
	//int s5=s4-12;
	//strPath= strPath.Mid( 0,s5);
	CString strPrintPath;
	// CString path9;
	// path9.Format(_T("C:\\a"));
	PulseEvent(g_eventPrintMutex);
	strPrintPath=CurrentstrPath + L"\\WBPrint.exe";
	//ShellExecute(NULL,"open","_T(strPath2)",NULL,NULL,SW_SHOWNORMAL);
	// AfxMessageBox(strPath2);
	WinExec(ToStr(strPrintPath.GetBuffer(strPrintPath.GetLength())).c_str(),SW_SHOW);
	strPrintPath.ReleaseBuffer();
	///创建一个打印事件监听线程
	DWORD threadId;
	HANDLE hTempHandle = CreateThread(0, 0, PrintEventListener, this, 0, &threadId);
	CloseHandle(hTempHandle);

	
}

BOOL CWhiteBoardView::AddPicture()
{
	//记录当前路径
	wchar_t szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	BOOL bFlag = FALSE;

	TCHAR lpszFilter[] = _T("JPEG (*.jpg;*.jpeg;*.jpe;*.jfif)|*.jpg;*.jpeg;*.jpe;*.jfif")
		_T("|GIF (*.gif)|*.gif|位图文件(*.bmp;*.dib)|*.bmp;*.dib;")
		_T("|TIFF (*.tif;*.tiff)|*.tif;*.tiff |PNG (*.png)|*.png")
		_T("|ICO (*.ico)|*.ico")
		_T("|所有图片文件|*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.bmp;*.dib;*.tif;*.tiff;*.ico;*.png||");
	//_T("|所有文件|*.*||");

	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,lpszFilter,NULL,0,FALSE);

	fileDlg.m_ofn.nFilterIndex = 7;	//设置默认类型为所有图片文件

	if(fileDlg.DoModal() == IDOK)
	{
		if (!g_pWhiteBoardView)
		{
			return FALSE;
		}
		CString strFilePath = fileDlg.GetPathName();
		//CString fileName = fileDlg.GetFileTitle();
		//strFilePath = _T("d:\\11.jpg");
		m_bIsdrop = FALSE;
		CWBPicture *pWBPicture = new CWBPicture(strFilePath);
		if (!PathFileExists(strFilePath))//wangxiaobin 
		{
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
				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strIsSave, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strIsSave);
			}
			else
			{
				AfxMessageBox(strFilePathError,MB_OK);
			}
			return FALSE;
		}
		//g_pObj = pWBPicture;
		//控制图片的最大不能超出view的大小4-21金鹏
		CRect viewRect;
		GetClientRect(&viewRect);
		pWBPicture->SetPictureBigerSzie(viewRect);
		g_pWBPage->AddWBObject(pWBPicture, WB_LAYER_NORMAL);



		pWBPicture->SetIsPath();//图片是否重先加载wangxiaobin

		Invalidate();

		//发送同步消息
		XMemIni xmlObj;
		pWBPicture->SetXmlValue(xmlObj);
		AddObj(xmlObj);

		bFlag = TRUE;
	}
	else
	{
		bFlag = FALSE;
	}	
	g_nDrawStatus = DRAW_NORMAL;
	SetCurrentDirectory(szCurPath);
	return bFlag;
}

BOOL CWhiteBoardView::AddPicture(CString strFilePath,CRect rect /*= CRect(0,0,0,0)*/)
{
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
			//g_pHPWBDataNotify->MessageBox(NULL, m_strPrompt, strFilePathError, MB_OK, true);
			g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
		}
		else
		{
			AfxMessageBox(strFilePathError,MB_OK);
		}
		return FALSE;
	}
	g_pWBPage->ClearSelectFrameList();
	m_bIsdrop = FALSE;
	//创建一个新对象
	CWBPicture *pWBPicture = new CWBPicture(strFilePath,rect);		
	pWBPicture->SetAlpha((int)g_nTranspar);
	//控制图片的最大不能超出view的大小
	CRect viewRect,viewRectInScreen;
	GetClientRect(&viewRect);
	GetWindowRect(&viewRectInScreen);//得到白板在屏幕上的位置
	//if (!(rect.left || rect.bottom || rect.right || rect.top)) 
	if(rect.Width() > viewRect.Width() ||rect.Height() > viewRect.Height())
	{
		pWBPicture->SetPictureBigerSzie(viewRect);
	}	
	g_pWBPage->AddWBObject(pWBPicture, WB_LAYER_NORMAL);


	CPoint point,ptCenter;
	GetCursorPos(&point);//鼠标在屏幕中的坐标
	point.x -= viewRectInScreen.left;//鼠标在屏幕中的位置转化为在白板中的坐标
	point.y -= viewRectInScreen.top;//鼠标在屏幕中的位置转化为在白板中的坐标
	CClientDC dc(this);
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&point);//将视图坐标转换为文档作标
	//获得对象中心点
	pWBPicture->GetCenterPoint(ptCenter);
	m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
	//创建选择框，并移动对象到当前位置
	if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
	{
		m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
	}
	//ptOffset = point - ptCenter;
	CPoint ptOffset(m_XOffSetPicture,m_YOffSetPicture);
	CRect pictureRect;
	pictureRect = pWBPicture->GetPictureRect();
	CPoint Rightbotton;
	Rightbotton.x = m_XOffSetPicture + pictureRect.right;
	Rightbotton.y = m_YOffSetPicture + pictureRect.bottom;
	if (Rightbotton.x <= 2400 && Rightbotton.y <= 1600)
	{
		g_pWBPage->MoveSelectedObj(ptOffset);
	}

	m_ptSelect = point;
	m_XOffSetPicture += 20;
	m_YOffSetPicture += 20;



	pWBPicture->SetIsPath();//图片是否重先加载wangxiaobin
	Invalidate();
	g_nDrawStatus = DRAW_NORMAL;

	//发送同步消息
	XMemIni xmlObj;
	pWBPicture->SetXmlValue(xmlObj);
	AddObj(xmlObj);

	return TRUE;
}

BOOL CWhiteBoardView::AddDicom()
{
	int Dicom_Return = 0;     //转换文件后返回的值；
	CString strFilePath ;  //选择的文件路径

	//记录当前的路径
	wchar_t szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	BOOL bFlag = FALSE;

	TCHAR lpszFilter[] = _T("dicom文件|*.DCM|All Files|*.*||");

	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,lpszFilter,NULL,0,FALSE);

	if(fileDlg.DoModal() == IDOK)
	{
		if (!g_pWhiteBoardView)
		{
			return FALSE;
		}
		strFilePath = fileDlg.GetPathName();

		if (g_IsMedicalVersion != NULL)
		{
			pDicomToBmpDll GetDicom;
			GetDicom = (pDicomToBmpDll)GetProcAddress(g_IsMedicalVersion,"DicomToBmpDll");

			if (GetDicom)
				Dicom_Return = GetDicom(strFilePath,g_strAvconTempPath);
		}

		char BMPFileName[512] = "";
		CString strFileConvertFail;
		strFileConvertFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FileConvertFail"));
		if (strFileConvertFail.IsEmpty())
		{
			strFileConvertFail= _T("文件转换失败！");
		}
		CString strNotOpenDicom;
		strNotOpenDicom = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("NotOpenDicom"));
		if (strNotOpenDicom.IsEmpty())
		{
			strNotOpenDicom = _T("打开的不是dicom文件！");
		}
		CString strDiskNotEnough;
		strDiskNotEnough = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("DiskNotEnough"));
		if (strDiskNotEnough.IsEmpty())
		{
			strDiskNotEnough = _T("保存文件时剩余磁盘空间不足！");
		}
		switch(Dicom_Return)
		{
		case DICOM_TOBMP_SUCCEES: 
			//把转换后的文件名传给strfilepath，以便CWBPicture调用

			if (g_IsMedicalVersion!= NULL)
			{
				GetBmpPath GetDicomBmpPath;
				GetDicomBmpPath = (GetBmpPath)GetProcAddress(g_IsMedicalVersion,"GetDicomBmpPath");

				if (GetDicomBmpPath)
				{
					if ( !GetDicomBmpPath(BMPFileName,512))
					{
						return FALSE;
					}

					strFilePath = BMPFileName;
					if (!PathFileExists(strFilePath))
					{
						if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
						{
							//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,L"DICOM"+strFileConvertFail, MB_OK,true);
							g_pHPWBDataNotify->ShowTipWindow(L"DICOM"+strFileConvertFail);
						}
						else
						{
							AfxMessageBox(L"DICOM"+strFileConvertFail,MB_OK);
						}
						return FALSE;
					}
				}

			}
			break;
		case DICOM_ERROR_OPENFAILED:
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strNotOpenDicom, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strNotOpenDicom);
			}
			else
			{
				AfxMessageBox(strNotOpenDicom,MB_OK);
			}
			return FALSE;
			break;
		case DICOM_FILEWRITEFAIL:
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strFileConvertFail, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strFileConvertFail);
			}
			else
			{
				AfxMessageBox(strFileConvertFail,MB_OK);
			}
			return FALSE;
			break;
		case FREEDISK_NOENOUGH:
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strDiskNotEnough, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strDiskNotEnough);
			}
			else
			{
				AfxMessageBox(strDiskNotEnough,MB_OK);
			}
			return FALSE;
			break;
		case NO_DICOM_FIEL:
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strNotOpenDicom, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strNotOpenDicom);
			}
			else
			{
				AfxMessageBox(strNotOpenDicom,MB_OK);
			}
			return FALSE;
			break;
		}

		CWBPicture *pWBPicture = new CWBPicture(strFilePath);

		//g_pObj = pWBPicture;
		//控制图片的最大不能超出view的大小4-21金鹏
		CRect viewRect;
		GetClientRect(&viewRect);
		pWBPicture->SetPictureBigerSzie(viewRect);
		g_pWBPage->AddWBObject(pWBPicture);


		pWBPicture->SetIsPath();//图片是否重先加载wangxiaobin
		
		Invalidate();

		//发送同步消息
		XMemIni xmlObj;
		pWBPicture->SetXmlValue(xmlObj);
		AddObj(xmlObj);

		bFlag = TRUE;
		//return TRUE;
	}
	else
	{
		bFlag = FALSE;
		//return FALSE;
	}	
	g_nDrawStatus = DRAW_NORMAL;
	SetCurrentDirectory(szCurPath);
	return bFlag;
}

BOOL CWhiteBoardView::AddDicom(CString strFilePath,CRect rect /*= CRect(0,0,0,0)*/)
{
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
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strFilePathError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
		}
		else
		{
			AfxMessageBox(strFilePathError,MB_OK);
		}
		return FALSE;
	}

	int Dicom_Return = 0;     //转换文件后返回的值；

	if (g_IsMedicalVersion != NULL)
	{
		pDicomToBmpDll GetDicom;
		GetDicom = (pDicomToBmpDll)GetProcAddress(g_IsMedicalVersion,"DicomToBmpDll");

		if (GetDicom)
			Dicom_Return = GetDicom(strFilePath,g_strAvconTempPath);
	}

	char BMPFileName[512] = "";
	CString strFileConvertFail;
	strFileConvertFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FileConvertFail"));
	if (strFileConvertFail.IsEmpty())
	{
		strFileConvertFail= _T("文件转换失败！");
	}
	CString strNotOpenDicom;
	strNotOpenDicom = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("NotOpenDicom"));
	if (strNotOpenDicom.IsEmpty())
	{
		strNotOpenDicom = _T("打开的不是dicom文件！");
	}
	CString strDiskNotEnough;
	strDiskNotEnough = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("DiskNotEnough"));
	if (strDiskNotEnough.IsEmpty())
	{
		strDiskNotEnough = _T("保存文件时剩余磁盘空间不足！");
	}
	CString strFileOpenFail;
	strFileOpenFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FileOpenFail"));
	if (strFileOpenFail.IsEmpty())
	{
		strFileOpenFail= _T("文件打开失败！");
	}
	switch(Dicom_Return)
	{
	case DICOM_TOBMP_SUCCEES: 
		//把转换后的文件名传给strfilepath，以便CWBPicture调用
		if (g_IsMedicalVersion!= NULL)
		{
			GetBmpPath GetDicomBmpPath;
			GetDicomBmpPath = (GetBmpPath)GetProcAddress(g_IsMedicalVersion,"GetDicomBmpPath");

			if (GetDicomBmpPath)
			{
				if ( !GetDicomBmpPath(BMPFileName,512))
				{
					return FALSE;
				}
				strFilePath = BMPFileName;
				if (!PathFileExists(strFilePath))
				{
					if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
					{
						//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,L"DICOM"+strFileConvertFail, MB_OK,true);
						g_pHPWBDataNotify->ShowTipWindow(L"DICOM"+strFileConvertFail);
					}
					else
					{
						AfxMessageBox(L"DICOM"+strFileConvertFail,MB_OK);
					}
					return FALSE;
				}
			}
		}
		break;
	case DICOM_ERROR_OPENFAILED:
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strFileOpenFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFileOpenFail);
		}
		else
		{
			AfxMessageBox(strFileOpenFail,MB_OK);
		}
		return FALSE;
		break;
	case DICOM_FILEWRITEFAIL:
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strFileConvertFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFileConvertFail);
		}
		else
		{
			AfxMessageBox(strFileConvertFail,MB_OK);
		}
		return FALSE;
		break;
	case FREEDISK_NOENOUGH:
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strDiskNotEnough, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strDiskNotEnough);
		}
		else
		{
			AfxMessageBox(strDiskNotEnough,MB_OK);
		}
		return FALSE;
		break;
	case NO_DICOM_FIEL:
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strNotOpenDicom, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNotOpenDicom);
		}
		else
		{
			AfxMessageBox(strNotOpenDicom,MB_OK);
		}
		return FALSE;
		break;
	}

	CWBPicture *pWBPicture = new CWBPicture(strFilePath,rect);

	//g_pObj = pWBPicture;
	//控制图片的最大不能超出view的大小4-21金鹏
	CRect viewRect;
	GetClientRect(&viewRect);
	if (!(rect.left || rect.bottom || rect.right || rect.top)) 
	{
		pWBPicture->SetPictureBigerSzie(viewRect);
	}
	g_pWBPage->AddWBObject(pWBPicture);

	pWBPicture->SetIsPath();//图片是否重先加载wangxiaobin
	/*CWBDustbin *pWBDustbin = new CWBDustbin();
	g_pWBPage->AddWBObject(pWBDustbin);*/

	Invalidate();

	//发送同步消息
	XMemIni xmlObj;
	pWBPicture->SetXmlValue(xmlObj);
	AddObj(xmlObj);

	g_nDrawStatus = DRAW_NORMAL;
	return TRUE;
}

void CWhiteBoardView::AddDustbin()
{
	if (NULL == m_pWBDustbin)
	{
		m_pWBDustbin = new CWBDustbin();
		CPoint ptOffset;
		CPoint ptCenter;
		CRect clientRC;

		GetClientRect(&clientRC);

		m_pWBDustbin->GetCenterPoint(ptCenter);
		ptCenter.x = ptCenter.x * 2;

		CPoint ptClient(clientRC.right,clientRC.bottom/2);
		ptClient.x += getXnPos();
		ptClient.y += getYnPos();
		ptOffset = ptClient - ptCenter;
		m_pWBDustbin->MoveOffset(ptOffset);
		Invalidate();
	}
	else
	{
		delete m_pWBDustbin;
		m_pWBDustbin = NULL;
		Invalidate();
	}

}

BOOL CWhiteBoardView::AddDoc(CRect rect)
{
	if (DocConvertTip()) //文档还在转换过程中
	{
		return FALSE;
	}

	//记录当前路径
	wchar_t szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	BOOL bFlag = FALSE;
	CString strFilePath = _T("");
	CString	Filter = _T("Print Files(*.pdf;*.doc;*.xls;*.ppt;*.docx;*.xlsx;*.pptx)|*.pdf;*.doc;*.xls;*.ppt;*.docx;*.xlsx;*.pptx||");

	CFileDialog dlgFile(TRUE, NULL, NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT /*| OFN_FILEMUSTEXIST*/, Filter, NULL, 0, FALSE);
	if(dlgFile.DoModal() == IDOK)
	{
		if (!g_pWhiteBoardView)
		{
			return FALSE;
		}
		strFilePath = dlgFile.GetPathName(); 

		if (!PathFileExists(strFilePath))//wangxiaobin 
		{
			CString strIsSave;
			CString strFilePathError;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
			if (strFilePathError.IsEmpty())
			{
				strFilePathError = _T("文件路径输入有误！");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strIsSave, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strIsSave);
			}
			else
			{
				AfxMessageBox(strFilePathError,MB_OK);
			}
			return FALSE;
		}
		g_pWBPage->ClearSelectFrameList();
		SetCurrentDirectory(szCurPath);
		CWBDocument *pWBDocument = new CWBDocument(strFilePath);
		if (pWBDocument == NULL)
		{
			return FALSE;
		}
		pWBDocument->SetRect(rect);

		g_pWBPage->AddWBObject(pWBDocument, WB_LAYER_NORMAL);

		//创建选择框，并移动对象到当前位置		
		if (g_pWBPage->AddSelectFrame(pWBDocument->GetLayerIndex()))
		{
			m_dwCurSelectObjLayerIndex = pWBDocument->GetLayerIndex();
		}
		Invalidate();

		int nPageIndex;
		nPageIndex = (int)g_pWBFile->GetCurPageIndex();
		pWBDocument->SetPageIndex(nPageIndex);

		//发送同步消息
		XMemIni xmlObj;
		pWBDocument->SetXmlValue(xmlObj);
		AddObj(xmlObj);

		bFlag = TRUE;
		//return TRUE;
	}
	else
	{
		bFlag = FALSE;
	}
	g_nDrawStatus = DRAW_NORMAL;
	SetCurrentDirectory(szCurPath);
	return bFlag;
}

BOOL CWhiteBoardView::AddDoc(CString strFilePath, CRect rect)
{
	if (DocConvertTip()) //文档还在转换过程中
	{
		return FALSE;
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
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strFilePathError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
		}
		else
		{
			AfxMessageBox(strFilePathError,MB_OK);
		}
		return FALSE;
	}

	g_pWBPage->ClearSelectFrameList();
	CWBDocument *pWBDocument = new CWBDocument(strFilePath,rect);
	if (pWBDocument == NULL)
	{
		return FALSE;
	}
	//pWBDocument->SetRect(rect);
	g_pWhiteBoardView->CreateFullScreen(pWBDocument);
	g_pWBPage->AddWBObject(pWBDocument, WB_LAYER_NORMAL);

	//创建选择框，并移动对象到当前位置		
	if (g_pWBPage->AddSelectFrame(pWBDocument->GetLayerIndex()))
	{
		m_dwCurSelectObjLayerIndex = pWBDocument->GetLayerIndex();
	}
	Invalidate();

	int nPageIndex;
	nPageIndex = (int)g_pWBFile->GetCurPageIndex();
	pWBDocument->SetPageIndex(nPageIndex);

	//发送同步消息
	XMemIni xmlObj;
	pWBDocument->SetXmlValue(xmlObj);
	AddObj(xmlObj);

	g_nDrawStatus = DRAW_NORMAL;
	return TRUE;
}

DWORD CWhiteBoardView::DeleteSelectedObjects()
{
	if (g_pWBPage == NULL)
	{
		return 0;
	}
	if (0 != m_ulTouchEventId && m_bIsTouchUp)
	{
		m_ulSendEventID = m_ulTouchEventId;
		m_bIsTouchUp = false;
	}
	else
	{
		m_ulSendEventID = GenerateSSRC();
	}
	DWORD** delObjArray = new DWORD*[2];
	DWORD dwDelCount = g_pWBPage->DeleteSelectedWBObjects(delObjArray);

	if (dwDelCount > 0)
	{
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}

		//同步删除
		for (int nIndex = 0; nIndex < (int)dwDelCount; ++nIndex)
		{
		//	RemoveObj(delObjArray[0][nIndex]);
			RemoveDelCombineObj(delObjArray[0][nIndex]);
		}
		UINT j = g_pWBPage->GetSelectObjNumber();
		POSITION pos;
		if (j == 1)//删除所选列表只剩一个对象时,此即为全局对象
		{
			for (int k = 0;k < j;++k )//遍历选中对象列表
			{
				pos = g_pWBPage->GetSelectObjLst()->FindIndex(k);
				if (pos == NULL)
				{
					continue;
				}
				g_pObj = (CWBObject *)g_pWBPage->GetSelectObjLst()->GetAt(pos);
			}
		}
		if (NULL != g_pWhiteBoardView->m_pWBFunctionDlg && ::IsWindow(g_pWhiteBoardView->m_pWBFunctionDlg->GetSafeHwnd()))//控制属性面板显示相应的属性
		{

			if (j == 0)//选中对象列表中无对象时
			{
				if (!g_bAttriDlginitialize)
				{
					::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_NOSELECTWBOBJ,0,0);
				}					
			}
			//else if (j == 1)//选中对象列表中只有一个对象时
			//{
			//	::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,0,j);
			//}
			//else //选中对象列表中存在多个对象时
			//{
			//	if (g_bAttriDlginitialize)
			//	{
			//		;
			//	} 
			//	else
			//	{
			//		::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_NOSELECTWBOBJ,0,0);//属性面板上的控件初始化
			//	}				
			//	for (int k = 0;k < j;++k )//遍历选中对象列表
			//	{
			//		pos = g_pWBPage->GetSelectObjLst()->FindIndex(j);
			//		if (pos == NULL)
			//		{
			//			continue;
			//		}
			//		pObj = (CWBObject *)g_pWBPage->GetSelectObjLst()->GetAt(pos);
			//		if (pObj == NULL)
			//		{
			//			continue;
			//		}
			//		int ntype = pObj->GetType();
			//		switch(ntype)
			//		{
			//		case WB_OBJ_FLASH:									
			//		case WB_OBJ_COMBINE:					
			//		case WB_OBJ_DOCUMENT:					
			//		case WB_OBJ_PICTURE:
			//			{
			//				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_PICTURE,j);//隐藏线宽,线形,箭头
			//			}
			//			break;
			//		case WB_OBJ_ANNOTATIONS:
			//			{
			//				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_ANNOTATIONS,j);//隐藏线宽,线形,箭头
			//			}
			//			break;
			//		case WB_OBJ_TEXT:
			//			{
			//				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_TEXT,j);//隐藏线宽,线形,箭头
			//			}
			//			break;
			//		case WB_OBJ_RHOMBUS:					
			//		case WB_OBJ_TRIANGLE:	
			//		case WB_OBJ_PALLELOGAM: //平行四边形
			//		case WB_OBJ_ARBITRARYTRIANGLE:
			//		case WB_OBJ_ELLIPSE:					
			//		case WB_OBJ_RECT:
			//		case WB_OBJ_PENTAGON:
			//		case WB_OBJ_HEXAGON:
			//		case WB_OBJ_FIVEPOINTSTART:
			//		case WB_OBJ_TRAPEZIUM:
			//		case WB_OBJ_POLYGON:
			//			{
			//				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_POLYGON,j);//隐藏线形,箭头,字体相关属性
			//			}
			//			break;
			//		case WB_OBJ_CURVE:					
			//		case WB_OBJ_LINE:
			//			{
			//				::PostMessage(g_pWhiteBoardView->GetWBFunctionDlg()->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_LINE,j);//隐藏线形,箭头,字体相关属性
			//			}
			//			break;
			//		default:
			//			break;
			//		}					

			//	}
			//}					

		}
	}
	//在同步时让接收方及时刷新，额外发送一个消息
	// 	if (g_pScreenDlg != NULL) //如果屏幕绘图打开
	// 	{
	// 		g_pScreenDlg->Invalidate();
	// 	}
	// 	else
	// 	{
	// 		Invalidate();
	// 	}
	delete []delObjArray[0];
	delete []delObjArray[1];
	delete []delObjArray;
	if (g_pHPWBDataNotify &&  g_bSupportI7)//I7删除对象后，发送取消选择的消息
	{
		MSG msg;
		memset(&msg,0,sizeof(MSG));
		msg.hwnd = NULL;
		msg.message = 0;
		msg.wParam = (WPARAM)0;
		msg.lParam = (LPARAM)0;
		m_bSelectStatueChange = 0;
		g_pHPWBDataNotify->SetSelectObjStatue(msg);
	}
	return dwDelCount;
}

//创建对象菜单
void CWhiteBoardView::CreateObjMenu(DWORD dwLayerIndex)
{
	//多选的时候通过BOOL类型变量去掉一些不能共有的菜单
	int nSelectedObj = g_pWBPage->GetSelectObjLst()->GetCount();
	CWBObject * pSelObj = NULL;
	POSITION pos = NULL;
	BOOL bMedia = FALSE;
	BOOL bAnno    = FALSE;
	//BOOL bLine   =FALSE;
	if (nSelectedObj > 1)
	{
		for (int i = 0; i < nSelectedObj; ++i)
		{
			pos = g_pWBPage->GetSelectObjLst()->FindIndex(i);
			if (pos == NULL)
			{
				continue;
			}
			pSelObj = (CWBObject *)g_pWBPage->GetSelectObjLst()->GetAt(pos);
			if (pSelObj == NULL)
			{
				continue;
			}
			if (pSelObj->GetType() == WB_OBJ_MEDIA || pSelObj->GetType() == WB_OBJ_CAMERA)
			{
				bMedia = TRUE;
			}
			//else if (pSelObj->GetType() == WB_OBJ_FLASH)
			//{
			//	bFlash = TRUE;
			//}
			//else if (pSelObj->GetType() == WB_OBJ_COMBINE || pSelObj->GetType() == WB_OBJ_DOCUMENT || pSelObj->GetType() == WB_OBJ_LINE
			//	|| pSelObj->GetType() == WB_OBJ_CURVE || pSelObj->GetType() == WB_OBJ_RECT || pSelObj->GetType() == WB_OBJ_TRIANGLE 
			//	|| pSelObj->GetType() == WB_OBJ_POLYGON || pSelObj->GetType() == WB_OBJ_HEXAGON || pSelObj->GetType() == WB_OBJ_ELLIPSE
			//	|| pSelObj->GetType() == WB_OBJ_RHOMBUS || pSelObj->GetType() == WB_OBJ_ARBITRARYTRIANGLE || pSelObj->GetType() == WB_OBJ_PALLELOGAM
			//	|| pSelObj->GetType() == WB_OBJ_PENTAGON || pSelObj->GetType() == WB_OBJ_FIVEPOINTSTART || pSelObj->GetType() == WB_OBJ_TRAPEZIUM
			//	|| pSelObj->GetType() == WB_OBJ_TEXT || pSelObj->GetType() == WB_OBJ_PICTURE)
			//{
			//	bLine = TRUE;
			//}
			else if (pSelObj->GetType() == WB_OBJ_PICTURE && (bIsInCamInfoMap(pSelObj)||((CWBPicture*)pSelObj)->GetIsPrintScreen()))
			{
				--nSelectedObj;
			}
			else if (pSelObj->GetType() == WB_OBJ_ANNOTATIONS)
			{
				bAnno = TRUE;
			}
			else
			{
				continue;
			}
		}
	}

	CWBObject *pObj = g_pWBPage->FindByLayerIndex(dwLayerIndex);
	if (pObj == NULL)
	{
		return;
	}
	WB_OBJ_TYPE nTemp = (WB_OBJ_TYPE)pObj->GetType();
	CMenu PopupMenu, RotationPopupMenu, MoviePopupMenu, DocPopupMenu, DocPrintMenu,ProtractorMenu;
	CMenu BitratePopupMenu,BitrateControlPopupMenu;
	CMenu UnitPopupMenu; //单位弹出菜单
	CMenu DxfPopupMenu; //DXF操作弹出菜单
	CMenu CompassPopupMenu; //圆规私有弹出菜单
	if (!UnitPopupMenu.CreatePopupMenu()||!PopupMenu.CreatePopupMenu() || !RotationPopupMenu.CreatePopupMenu()
		|| !MoviePopupMenu.CreatePopupMenu() || !DocPopupMenu.CreatePopupMenu()|| !DocPrintMenu.CreatePopupMenu()
		|| !BitratePopupMenu.CreatePopupMenu()|| !BitrateControlPopupMenu.CreatePopupMenu()|| !ProtractorMenu.CreatePopupMenu()
		|| !DxfPopupMenu.CreatePopupMenu() || !CompassPopupMenu.CreatePopupMenu())
	{
		return;
	}
	m_dwMenuLayerIndex = dwLayerIndex;
	CBitmap movieCircleBmp;    //循环播放
	//wangxiaobin主要说性能下降，多次调用
	CObList *pObjList = NULL;
	CWBObject *pbkObject = NULL;
	pbkObject = g_pWBPage->GetBkgObj();
	pObjList = g_pWBPage->GetWBObjList();
	if (pObjList == NULL)
	{
		return;
	}

	if (pObj->IsInfiniteClone())
	{
		CString strInfiniteClone;
		strInfiniteClone = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_infiniteClone"));
		if (strInfiniteClone.IsEmpty())
		{
			strInfiniteClone = _T("无限克隆");
		}
		PopupMenu.AppendMenu(MF_CHECKED|MF_STRING, ID_OBMENU_INFINITECLONE, strInfiniteClone);
	} 
	else if (pObj->IsLock())
	{
		if (nTemp == WB_OBJ_LINE)
		{
			CString strUnlock;
			strUnlock = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_unlock"));
			if (strUnlock.IsEmpty())
			{
				strUnlock = _T("解锁");
			}
			PopupMenu.AppendMenu(MF_CHECKED|MF_STRING, ID_OBMENU_LOCK, strUnlock);
		}
	}
	else
	{
		CString strPhotograph;
		strPhotograph = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_photograph"));
		if (strPhotograph.IsEmpty())
		{
			strPhotograph = _T("拍照");
		}
		if (nTemp == WB_OBJ_CAMERA)
		{
			CPoint point;
			GetCursorPos(&point);
			if (!g_bSupportU7OpenFile)
			{
				dynamic_cast<CWBCamera*>(pObj)->GetCameraDlg()->menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, dynamic_cast<CWBCamera*>(pObj)->GetCameraDlg());
			}
			
			return;
		}
		
		if(nTemp == WB_OBJ_FLASH && nSelectedObj == 1)     //    FLASH对象
		{
			CString strPlay;
			strPlay = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_play"));
			if (strPlay.IsEmpty())
			{
				strPlay = _T("播放");
			}
			CString strStop;
			strStop = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_Stop"));
			if (strStop.IsEmpty())
			{
				strStop = _T("停止");
			}
			CString strPause;
			strPause = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_pause"));
			if (strPause.IsEmpty())
			{
				strPause = _T("暂停");
			}
			CString strLoop;
			
			if (((CWBFlash *)pObj)->m_bIsRunning)
			{
				if (((CWBFlash *)pObj)->m_bIsPause)
				{
					MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_PALY, strPlay);
					MoviePopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_MOVIE_PAUSE, strPause);
				} 
				else
				{
					MoviePopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_MOVIE_PALY, strPlay);
					MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_PAUSE, strPause);
				}			
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_STOP,strStop);
			} 
			else
			{
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_PALY, strPlay);
				MoviePopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_MOVIE_PAUSE, strPause);
				MoviePopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_MOVIE_STOP, strStop);
			}


			if (((CWBFlash *)pObj)->GetCircle())
			{
				strLoop = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_loop"));
				if (strLoop.IsEmpty())
				{
					strLoop = _T("循环播放");
				}
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_CIRCLE, strLoop);
				movieCircleBmp.LoadBitmap(IDB_OBMENU_MOVIE_CIRCLE); //视频循环播放
				TranslateBitmapSizeToFitXP(&movieCircleBmp);
			} 
			else
			{
				strLoop = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_singlePlay"));
				if (strLoop.IsEmpty())
				{
					strLoop = _T("单放");
				}
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_CIRCLE, strLoop);
				movieCircleBmp.LoadBitmap(IDB_OBMENU_MOVIE_SINGLE); //视频循环播放
				TranslateBitmapSizeToFitXP(&movieCircleBmp);
			}

			PopupMenu.AppendMenu(MF_POPUP, (UINT)MoviePopupMenu.m_hMenu,strPlay);
			PopupMenu.AppendMenu(MF_SEPARATOR);
			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_CATCHFRAME, strPhotograph);
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}
		if (nTemp == WB_OBJ_MEDIA && nSelectedObj ==1)
		{
			CString strPlay;
			strPlay = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_play"));
			if (strPlay.IsEmpty())
			{
				strPlay = _T("播放");
			}
			CString strStop;
			strStop = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_Stop"));
			if (strStop.IsEmpty())
			{
				strStop = _T("停止");
			}
			CString strPause;
			strPause = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_pause"));
			if (strPause.IsEmpty())
			{
				strPause = _T("暂停");
			}
			CString strLoop;
			strLoop = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_loop"));
			if (strLoop.IsEmpty())
			{
				strLoop = _T("循环播放");
			}
			//播放前
			if (((CWBMedia*)pObj)->GetMediaPlayState() == Media_playing_before )
			{
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_PALY, strPlay);
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_PAUSE, strPause);
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_STOP, strStop);
			}
			//播放
			else   if (((CWBMedia*)pObj)->GetMediaPlayState() == Media_playing)
			{
				MoviePopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_MOVIE_PALY, strPlay);
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_PAUSE, strPause);
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_STOP, strStop);
			}
			//停止
			else	if (((CWBMedia*)pObj)->GetMediaPlayState() == Media_stop)
			{
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_PALY, strPlay);
				MoviePopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_MOVIE_PAUSE, strPause);
				MoviePopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_MOVIE_STOP, strStop);
			}
			//暂停
			else	if (((CWBMedia*)pObj)->GetMediaPlayState() == Media_pausing)
			{
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_PALY, strPlay);
				MoviePopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_MOVIE_PAUSE, strPause);
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_STOP, strStop);
			}
			//循环
			BOOL bLoop = ((CWBMedia*)pObj)->GetMediaLoopState();
			if (bLoop == FALSE)
			{
				strLoop = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_loop"));
				if (strLoop.IsEmpty())
				{
					strLoop = _T("循环播放");
				}
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_CIRCLE, strLoop);
				movieCircleBmp.LoadBitmap(IDB_OBMENU_MOVIE_CIRCLE); //视频循环播放
				TranslateBitmapSizeToFitXP(&movieCircleBmp);
				//MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_CIRCLE, strLoop);
			}
			else if (bLoop == TRUE)
			{
				strLoop = ReadConfigText(_T("WhiteBoardMenuListText"), _T("movie_singlePlay"));
				if (strLoop.IsEmpty())
				{
					strLoop = _T("单放");
				}
				MoviePopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MOVIE_CIRCLE, strLoop);
				movieCircleBmp.LoadBitmap(IDB_OBMENU_MOVIE_SINGLE); //视频循环播放
				TranslateBitmapSizeToFitXP(&movieCircleBmp);
				//MoviePopupMenu.AppendMenu(MF_CHECKED|MF_STRING, ID_OBMENU_MOVIE_CIRCLE, strLoop);
			}
			PopupMenu.AppendMenu(MF_POPUP, (UINT)MoviePopupMenu.m_hMenu, strPlay);

			if (((CWBMedia*)pObj)->IsActiveCreated())
			{
				//码流wangxiaobin
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_ONE,_T("32kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_TWO,_T("64kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_THREE,_T("128kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_FOUR,_T("256kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_FIVE,_T("384kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING|MF_CHECKED,ID_OBMENU_MEDIA_BIRTRATE_SIX,_T("512kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_SEV,_T("768kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_EIG,_T("1000kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_NINE,_T("1500kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_TEN,_T("2000kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_ELE,_T("3000kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_TVE,_T("4000kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_THI,_T("5000kbps"));
				BitratePopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_FOTH,_T("6000kbps"));
				CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
				//int bitrate = GetBirtrateCur();
				int bitrate;										
				bitrate = ReadMediaBitrate();
				if (bitrate == 200)
				{
					bitrate = 5;
				}

				CString strVideoStream;
				strVideoStream = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_videoStream"));
				if (strVideoStream.IsEmpty())
				{
					strVideoStream = _T("视频码流");
				}
				CString strStreamControl;
				strStreamControl = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_transmissionNetwork"));
				if (strStreamControl.IsEmpty())
				{
					strStreamControl = _T("传输网络");
				}
				CString strNetwork;
				strNetwork = ReadConfigText(_T("WhiteBoardMenuListText"), _T("bitrate_network"));
				if (strNetwork.IsEmpty())
				{
					strNetwork = _T("互联网");
				}
				CString strLAN;
				strLAN = ReadConfigText(_T("WhiteBoardMenuListText"), _T("bitrate_LAN"));
				if (strLAN.IsEmpty())
				{
					strLAN = _T("局域网");
				}
				CString strPrivateNetwork;
				strPrivateNetwork = ReadConfigText(_T("WhiteBoardMenuListText"), _T("bitrate_privateNetwork"));
				if (strPrivateNetwork.IsEmpty())
				{
					strPrivateNetwork = _T("专网");
				}
				CString strSatelliteNetwork;
				strSatelliteNetwork = ReadConfigText(_T("WhiteBoardMenuListText"), _T("bitrate_satelliteNetwork"));
				if (strSatelliteNetwork.IsEmpty())
				{
					strSatelliteNetwork = _T("卫星网");
				}
				BOOL bMenu = BitratePopupMenu.CheckMenuRadioItem(0,13,bitrate,MF_CHECKED|MF_BYPOSITION);
				PopupMenu.AppendMenu(MF_POPUP,(UINT)BitratePopupMenu.m_hMenu,strVideoStream);

				//码流控制wangxiaobin
				BitrateControlPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_COL_ONE,strNetwork);
				BitrateControlPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_COL_TWO,strLAN);
				BitrateControlPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_COL_THE,strPrivateNetwork);
				BitrateControlPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_COL_FOR,strSatelliteNetwork);
				BitrateControlPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_MEDIA_BIRTRATE_COL_FIV,_T("3G"));

				//int ControlBitrate = GetControlBirtrateCur();
				int ControlBitrate;
				ControlBitrate = ReadMediaControlBitrate();
				if (ControlBitrate == 200)
				{
					ControlBitrate = 0;
				}
				BOOL bControlMenu = BitrateControlPopupMenu.CheckMenuRadioItem(0,4,ControlBitrate,MF_CHECKED|MF_BYPOSITION);

				PopupMenu.AppendMenu(MF_POPUP,(UINT)BitrateControlPopupMenu.m_hMenu,strStreamControl);
			}


			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MEDIA_CATCHFRAME, strPhotograph);
			EVoiceState voiceState = ((CWBMedia*)pObj)->GetVoiceState();
			CString strMute;
			strMute = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_mute"));
			if (strMute.IsEmpty())
			{
				strMute = _T("静音");
			}
			if (voiceState == VOICECLOSE )
			{
				PopupMenu.AppendMenu(MF_STRING|MF_CHECKED, ID_OBMENU_MEDIA_SETMUTE,strMute);
			}
			else
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MEDIA_SETMUTE, strMute);
			}
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}
		if (nTemp == WB_OBJ_DOCUMENT && nSelectedObj ==1) //文档
		{
			CWBDocument *pDoc = (CWBDocument *)pObj;

			CString strPageUp;
			strPageUp = ReadConfigText(_T("WhiteBoardMenuListText"), _T("doc_pageUp"));
			if (strPageUp.IsEmpty())
			{
				strPageUp = _T("上一页");
			}
			CString strNextPage;
			strNextPage = ReadConfigText(_T("WhiteBoardMenuListText"), _T("doc_nextPage"));
			if (strNextPage.IsEmpty())
			{
				strNextPage = _T("下一页");
			}
			CString strFirstPage;
			strFirstPage = ReadConfigText(_T("WhiteBoardMenuListText"), _T("doc_firstPage"));
			if (strFirstPage.IsEmpty())
			{
				strFirstPage = _T("首页");
			}
			CString strEndPage;
			strEndPage = ReadConfigText(_T("WhiteBoardMenuListText"), _T("doc_endPage"));
			if (strEndPage.IsEmpty())
			{
				strEndPage = _T("末页");
			}
			CString strTurnPage;
			strTurnPage = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_turnPage"));
			if (strTurnPage.IsEmpty())
			{
				strTurnPage = _T("文档翻页");
			}

			if (pDoc->GetCurPageIndex() <= 1) //如果当前页是第一页
			{
				DocPopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_DOC_PREPAGE, strPageUp);
				DocPopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_DOC_FIRSTPAGE, strFirstPage);
			}
			else
			{
				DocPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DOC_PREPAGE, strPageUp);
				DocPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DOC_FIRSTPAGE, strFirstPage);
			}

			if (pDoc->GetCurPageIndex() >= pDoc->GetPageCount()) //如果当前页是最后一页
			{
				DocPopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_DOC_NEXTPAGE, strNextPage);		
				DocPopupMenu.AppendMenu(MF_GRAYED|MF_STRING, ID_OBMENU_DOC_LASTPAGE, strEndPage);
			}
			else
			{
				DocPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DOC_NEXTPAGE, strNextPage);		
				DocPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DOC_LASTPAGE, strEndPage);
			}		

			PopupMenu.AppendMenu(MF_POPUP, (UINT)DocPopupMenu.m_hMenu, strTurnPage);
			PopupMenu.AppendMenu(MF_SEPARATOR);

			if (!g_bSupportU7OpenFile) //在U7中不显示打印按钮
			{
				CString strThisPage;
				strThisPage = ReadConfigText(_T("WhiteBoardMenuListText"), _T("docprint_thisPage"));
				if (strThisPage.IsEmpty())
				{
					strThisPage = _T("当前页");
				}
				CString strAllPage;
				strAllPage = ReadConfigText(_T("WhiteBoardMenuListText"), _T("docprint_allPage"));
				if (strAllPage.IsEmpty())
				{
					strAllPage = _T("所有页");
				}
				CString strDocPrint;
				strDocPrint = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_docPrint"));
				if (strDocPrint.IsEmpty())
				{
					strDocPrint = _T("文档打印");
				}
				DocPrintMenu.AppendMenu(MF_STRING, ID_OBMENU_DOC_PRINTCUR, strThisPage);
				DocPrintMenu.AppendMenu(MF_STRING, ID_OBMENU_DOC_PRINTALL, strAllPage);
				PopupMenu.AppendMenu(MF_POPUP, (UINT)DocPrintMenu.m_hMenu, strDocPrint);
				PopupMenu.AppendMenu(MF_SEPARATOR);
			}
		}
		if (nTemp == WB_OBJ_DXF && nSelectedObj ==1)
		{
			CString strDrag;
			strDrag = ReadConfigText(_T("WhiteBoardMenuListText"), _T("dxf_drag"));
			if (strDrag.IsEmpty())
			{
				strDrag = _T("拖拽");
			}
			CString strZoomIn;
			strZoomIn = ReadConfigText(_T("WhiteBoardMenuListText"), _T("dxf_zoomin"));
			if (strZoomIn.IsEmpty())
			{
				strZoomIn = _T("放大");
			}
			CString strZoomOut;
			strZoomOut = ReadConfigText(_T("WhiteBoardMenuListText"), _T("dxf_zoomout"));
			if (strZoomOut.IsEmpty())
			{
				strZoomOut = _T("缩小");
			}
			CString strOperate;
			strOperate = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_dxfoperate"));
			if (strOperate.IsEmpty())
			{
				strOperate = _T("Dxf操作");
			}
		
			DxfPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DXF_DRAG, strDrag);
			DxfPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DXF_ZOOMIN, strZoomIn);
			DxfPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DXF_ZOOMOUT, strZoomOut);	
			PopupMenu.AppendMenu(MF_POPUP, (UINT)DxfPopupMenu.m_hMenu,strOperate);
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}
		if (nTemp != WB_OBJ_SETSQUARE && nTemp!= WB_OBJ_PROTRACTOR && nTemp != WB_OBJ_COMPASS )//三角板、圆规、量角器不添加全选
		{
			if (g_bIsDrawScreen) //如果屏幕绘图打开
			{
				CString strSelectAll;
				strSelectAll = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_selectAll"));
				if (strSelectAll.IsEmpty())
				{
					strSelectAll = _T("全选");
				}
				if ((g_pWBPage->GetWBLabelObjList())->GetCount())
				{
					PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_SELECTALL, strSelectAll);
				} 
				else
				{
					PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_SELECTALL,strSelectAll);
				}
			} 
			else
			{
				CString strSelectAll;
				strSelectAll = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_selectAll"));
				if (strSelectAll.IsEmpty())
				{
					strSelectAll = _T("全选");
				}
				if ((pObjList->GetCount() <= 0) || (pObjList->GetCount() == 1 && pbkObject != NULL))
				{
					PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_SELECTALL, strSelectAll);
				} 
				else
				{
					PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_SELECTALL, strSelectAll);
				}
			}

		}
		CString strUnselect;
		strUnselect = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_unselect"));
		if (strUnselect.IsEmpty())
		{
			strUnselect = _T("取消选择");
		}

		PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_UNSELECT, strUnselect);
		PopupMenu.AppendMenu(MF_SEPARATOR);

		if (pObj->GetType()==WB_OBJ_PROTRACTOR)
		{
			CString str = ReadConfigText(_T("WhiteBoardMenuListText"),(L"Production Scale")); 
			if (str.IsEmpty())
			{
				str = _T("生成刻度");
			}
			PopupMenu.AppendMenu(MF_STRING,ID_OBMENU_PRODUCTIONSCALE,str);
		}
		if (nTemp != WB_OBJ_PROTRACTOR && nTemp != WB_OBJ_SETSQUARE && nTemp != WB_OBJ_COMPASS)//直线添加锁定菜单
		{
			CString strLock;
			strLock = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_lock"));
			if (strLock.IsEmpty())
			{
				strLock = _T("锁定");
			}
			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_LOCK, strLock);

			CString strUnLock;
			strUnLock = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_allunlock"));
			if (strUnLock.IsEmpty())
			{
				strUnLock = _T("解除全部锁定");
			}
			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_ALLUNLOCK, strUnLock);

			PopupMenu.AppendMenu(MF_SEPARATOR);
		}			
		if (nTemp == WB_OBJ_RULER)
		{
			CString sckd  = ReadConfigText(_T("WhiteBoardMenuListText"),(L"Production Scale")); 
			if (sckd.IsEmpty())
			{
				sckd = _T("生成刻度");
			}
			PopupMenu.AppendMenu(MF_STRING,ID_OBMENU_SCKD,sckd);
		}
		if ((nTemp != WB_OBJ_LINE&& nTemp != WB_OBJ_RULER &&nTemp != WB_OBJ_MIDI&& nTemp != WB_OBJ_SETSQUARE &&nTemp !=WB_OBJ_PROTRACTOR && nTemp != WB_OBJ_COMPASS)
			&& nSelectedObj ==1)//三角板不添加等比缩放
		{
			CString strEqualScaleZoom;
			strEqualScaleZoom = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_equalScaleZoom"));
			if (strEqualScaleZoom.IsEmpty())
			{
				strEqualScaleZoom = _T("等比例缩放");
			}							
			if (pObj->IsEqualScaleZoom())
			{
				if (pObj->GetType() == WB_OBJ_COMBINE || pObj->GetType() == WB_OBJ_DOCUMENT||pObj->GetType() == WB_OBJ_ANGLE)
				{
					PopupMenu.AppendMenu(MF_CHECKED | MF_STRING | MF_DISABLED, ID_OBMENU_EQUALSCALEZOOM, strEqualScaleZoom);
				}
				else
				{
					PopupMenu.AppendMenu(MF_CHECKED | MF_STRING, ID_OBMENU_EQUALSCALEZOOM, strEqualScaleZoom);
				}
			}
			else
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_EQUALSCALEZOOM, strEqualScaleZoom);
			}
			
		}

		if (!g_bIsDrawScreen)
		{
			if (pbkObject != NULL)
			{
				CString strDelBackground;
				strDelBackground = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_delBackground"));
				if (strDelBackground.IsEmpty())
				{
					strDelBackground = _T("删除当前背景图");
				}
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DELETEBACKGROUND, strDelBackground);
			}
			if (((pObj->GetType() == WB_OBJ_PICTURE && !((CWBPicture *)pObj)->GetImage()->IsAnimatedGIF()) && !((CWBPicture *)pObj)->GetIsPrintScreen()|| pObj->GetType() == WB_OBJ_MOVIE)
				&& nSelectedObj ==1) //如果对象是图片或视频、只选中单个对象
			{
				CString strAddBackground;
				strAddBackground = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_addBackground"));
				if (strAddBackground.IsEmpty())
				{
					strAddBackground = _T("设为背景图");
				}
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_SETASBACKGROUND, strAddBackground);
			}
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if ((nTemp == WB_OBJ_TEXT || nTemp == WB_OBJ_ANNOTATIONS||nTemp == WB_OBJ_ARBITRARYTRIANGLE)
			&& nSelectedObj ==1)//如果对象是文本或者标注、任意三角形 单个选中对象
		{
			CString strEdit;
			strEdit = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_edit"));
			if (strEdit.IsEmpty())
			{
				strEdit = _T("编辑");
			}
			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_EDIT, strEdit);
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}


		if ((nTemp != WB_OBJ_MEDIA) /*&& (!((CWBMedia*)pObj)->IsActiveCreated())*/ && (nTemp!=WB_OBJ_PROTRACTOR)
			&& (nTemp!=WB_OBJ_ANGLE) && (nTemp != WB_OBJ_SETSQUARE) && (!bMedia) && (nTemp != WB_OBJ_COMPASS))//去掉克隆复制粘贴剪切
		{
			CString strCopy;
			strCopy = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_copy"));
			if (strCopy.IsEmpty())
			{
				strCopy = _T("复制");
			}
			CString strCut;
			strCut = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_cut"));
			if (strCut.IsEmpty())
			{
				strCut = _T("剪切");
			}
			CString strPaste;
			strPaste = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_paste"));
			if (strPaste.IsEmpty())
			{
				strPaste = _T("粘贴");
			}
			CString strClone;
			strClone = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_clone"));
			if (strClone.IsEmpty())
			{
				strClone = _T("克隆");
			}
			if (pObj->IsLock())
			{
				PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_COPY, strCopy);
				PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_CUT, strCut);
			} 
			else
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_COPY, strCopy);
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_CUT, strCut);
			}

			if (m_bIsCopy || m_bIsCut)
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_PASTE, strPaste);
			}
			else
			{
				PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_PASTE, strPaste);
			}
			PopupMenu.AppendMenu(MF_SEPARATOR);

			if (pObj->IsLock())
			{
				PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_CLONE, strClone);
			} 
			else
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_CLONE, strClone);
			}
		}


		if (nTemp != WB_OBJ_MEDIA && nTemp != WB_OBJ_CAMERA&&nTemp!=WB_OBJ_PROTRACTOR
			&&nTemp!=WB_OBJ_ANGLE && nTemp !=WB_OBJ_SETSQUARE && nTemp != WB_OBJ_COMPASS && !bMedia && nSelectedObj ==1) //去掉无限克隆
		{
			CString strInfiniteClone;
			strInfiniteClone = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_infiniteClone"));
			if (strInfiniteClone.IsEmpty())
			{
				strInfiniteClone = _T("无限克隆");
			}
			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_INFINITECLONE, strInfiniteClone);
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		CString strDelete;
		strDelete = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_delete"));
		if (strDelete.IsEmpty())
		{
			strDelete = _T("删除");
		}
		CString strDeleteAll;
		strDeleteAll = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_deleteAll"));
		if (strDeleteAll.IsEmpty())
		{
			strDeleteAll = _T("删除全部");
		}
		PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DELETE, strDelete);

		if (g_bIsDrawScreen)
		{
			if ((g_pWBPage->GetWBLabelObjList())->GetCount())
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DELALL, strDeleteAll);
			} 
			else
			{
				PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_DELALL, strDeleteAll);
			}
		} 
		else
		{
			if ((pObjList->GetCount() <= 0) || (pObjList->GetCount() == 1 && pbkObject))
			{
				PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_DELALL, strDeleteAll);
			} 
			else
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DELALL, strDeleteAll);
			}
		}
		PopupMenu.AppendMenu(MF_SEPARATOR);				
		if (!g_bIsDrawScreen && nTemp != WB_OBJ_MEDIA && nTemp != WB_OBJ_CAMERA && nTemp != WB_OBJ_SETSQUARE
			&& nTemp != WB_OBJ_COMPASS &&nTemp!=WB_OBJ_PROTRACTOR && !bMedia && (!(nTemp == WB_OBJ_PICTURE && ((CWBPicture*)pObj)->GetIsPrintScreen())))//去组合
		{
			CString strCombine;
			strCombine = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_combine"));
			if (strCombine.IsEmpty())
			{
				strCombine = _T("组合");
			}
			CString strUncombine;
			strUncombine = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_uncombine"));
			if (strUncombine.IsEmpty())
			{
				strUncombine = _T("取消组合");
			}
			if (nSelectedObj - g_pWBPage->GetSelectLockCount() > 1)
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_COMBINE, strCombine);
			}
			else
			{
				PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_COMBINE, strCombine);
			}
			if (WB_OBJ_COMBINE == nTemp)
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_UNCOMBINE, strUncombine);
			} 
			else
			{
				PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_UNCOMBINE, strUncombine);
			}
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}
		//如果对象不是新视频和摄像头则加上下列菜单项
		int nMaxLayId = 0;
		int nMinLayId = 0;
		int nObjLayId = 0;
		if (nTemp != WB_OBJ_MEDIA && nTemp != WB_OBJ_CAMERA)
		{
			nMaxLayId = g_pWBPage->GetCurrentPageMaxLayer();
			nMinLayId = g_pWBPage->GetCurrentPageMinLayer();
			nObjLayId = pObj->GetPageLayerIndex();
			CString strTopLayer;
			strTopLayer = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_topLayer"));
			if (strTopLayer.IsEmpty())
			{
				strTopLayer = _T("最顶层");
			}
			CString strUpLayer;
			strUpLayer = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_upLayer"));
			if (strUpLayer.IsEmpty())
			{
				strUpLayer = _T("上一层");
			}
			CString strUnderLayer;
			strUnderLayer = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_underLayer"));
			if (strUnderLayer.IsEmpty())
			{
				strUnderLayer = _T("下一层");
			}
			CString strBottomLayer;
			strBottomLayer = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_bottomLayer"));
			if (strBottomLayer.IsEmpty())
			{
				strBottomLayer = _T("最底层");
			}
			if (nSelectedObj == 1)
			{
				if (nObjLayId >= nMaxLayId)
				{
					PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_TOPLAYER, strTopLayer);
					PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_UPLAYER, strUpLayer);
				}
				else
				{
					PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_TOPLAYER, strTopLayer);
					PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_UPLAYER, strUpLayer);
				}
				if (nMinLayId >= nObjLayId)
				{
					PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_UNDERLAYER, strUnderLayer);
					PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_BOTTPMLAYER, strBottomLayer);
				}
				else
				{
					PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_UNDERLAYER, strUnderLayer);
					PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_BOTTPMLAYER, strBottomLayer);
				}
				PopupMenu.AppendMenu(MF_SEPARATOR);	
			}
		}
		if (nTemp != WB_OBJ_LINE
			&& nTemp != WB_OBJ_MEDIA
			&& nTemp != WB_OBJ_CAMERA
			&& nTemp != WB_OBJ_ANNOTATIONS
			&& nTemp != WB_OBJ_PROTRACTOR
			&& nTemp != WB_OBJ_SETSQUARE
			&& nTemp != WB_OBJ_COMPASS
			&& nTemp != WB_OBJ_MIDI
			&& !bMedia
			&& !bAnno) 
		{
			CString strDisplayCenter;
			strDisplayCenter = ReadConfigText(_T("WhiteBoardMenuListText"), _T("display_center"));
			if (strDisplayCenter.IsEmpty())
			{
				strDisplayCenter = _T("显示中心点");
			}

			if (pObj->IsRorationCenterZoom())
			{
				PopupMenu.AppendMenu(MF_CHECKED | MF_STRING, ID_OBMENU_RORATIONCENTERZOOM, strDisplayCenter);
			}
			else
			{
				PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_RORATIONCENTERZOOM,strDisplayCenter);
			}
			PopupMenu.AppendMenu(MF_SEPARATOR);	
		}
		//对象不是直线，也不是新视频
		if (nTemp != WB_OBJ_LINE
			&& nTemp != WB_OBJ_MEDIA
			&& nTemp != WB_OBJ_CAMERA
			&& nTemp != WB_OBJ_SETSQUARE
			&& nTemp != WB_OBJ_PROTRACTOR
			&& nTemp != WB_OBJ_COMPASS
			&& nTemp != WB_OBJ_MIDI
			&& !bMedia && !bAnno) 
		{
			if (nTemp != WB_OBJ_ANNOTATIONS)
			{
				CString strDeasil45;
				strDeasil45 = ReadConfigText(_T("WhiteBoardMenuListText"), _T("rotation_deasil45"));
				if (strDeasil45.IsEmpty())
				{
					strDeasil45 = _T("顺时针旋转45度");
				}
				CString strDeasil90;
				strDeasil90 = ReadConfigText(_T("WhiteBoardMenuListText"), _T("rotation_deasil90"));
				if (strDeasil90.IsEmpty())
				{
					strDeasil90 = _T("顺时针旋转90度");
				}
				CString strWidderShins45;
				strWidderShins45 = ReadConfigText(_T("WhiteBoardMenuListText"), _T("rotation_widderShins45"));
				if (strWidderShins45.IsEmpty())
				{
					strWidderShins45 = _T("逆时针旋转45度");
				}
				CString strWidderShins90;
				strWidderShins90 = ReadConfigText(_T("WhiteBoardMenuListText"), _T("rotation_widderShins90"));
				if (strWidderShins90.IsEmpty())
				{
					strWidderShins90 = _T("逆时针旋转90度");
				}
				CString strRotation180;
				strRotation180 = ReadConfigText(_T("WhiteBoardMenuListText"), _T("rotation_rotation180"));
				if (strRotation180.IsEmpty())
				{
					strRotation180 = _T("旋转180度");
				}
				CString strVertical;
				strVertical = ReadConfigText(_T("WhiteBoardMenuListText"), _T("rotation_vertical"));
				if (strVertical.IsEmpty())
				{
					strVertical = _T("垂直放置");
				}
				CString strHirizontal;
				strHirizontal = ReadConfigText(_T("WhiteBoardMenuListText"), _T("rotation_hirizontal"));
				if (strHirizontal.IsEmpty())
				{
					strHirizontal = _T("水平放置");
				}
				CString strPreciseRotation;
				strPreciseRotation = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_preciseRotation"));
				if (strPreciseRotation.IsEmpty())
				{
					strPreciseRotation = _T("精确旋转");
				}
				RotationPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DEASIL45, strDeasil45);
				RotationPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DEASIL90, strDeasil90);
				RotationPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_WIDDERSHINS45, strWidderShins45);
				RotationPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_WIDDERSHINS90, strWidderShins90);
				RotationPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_ROTATION180, strRotation180);
				RotationPopupMenu.AppendMenu(MF_SEPARATOR);
				if (pObj->GetRotationAngle() == 0)
				{
					RotationPopupMenu.AppendMenu(MF_STRING, ID_ROTATION_VERTICAL, strVertical);
				}
				else if (pObj->GetRotationAngle() == 90)
				{
					RotationPopupMenu.AppendMenu(MF_STRING, ID_ROTATION_HIRIZONTAL, strHirizontal);
				}
				else
				{
					RotationPopupMenu.AppendMenu(MF_STRING, ID_ROTATION_HIRIZONTAL, strHirizontal);
					RotationPopupMenu.AppendMenu(MF_STRING, ID_ROTATION_VERTICAL, strVertical);
				}
				PopupMenu.AppendMenu(MF_POPUP, (UINT)RotationPopupMenu.m_hMenu, strPreciseRotation);
				PopupMenu.AppendMenu(MF_SEPARATOR);
			}
		}

		if (nTemp == WB_OBJ_PROTRACTOR)
		{
			CString str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("UnitConversion"));
			if (str.IsEmpty())
			{
				str = _T("单位转换");
			}
			PopupMenu.AppendMenu(MF_POPUP, (UINT)ProtractorMenu.m_hMenu, str);
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("Angle"));
			if (str.IsEmpty())
			{
				str = _T("角度");
			}
			ProtractorMenu.AppendMenu(MF_STRING, ID_PROTRACTOR_ANGLE, str);
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("Radian"));
			if (str.IsEmpty())
			{
				str = _T("弧度");
			}
			ProtractorMenu.AppendMenu(MF_STRING, ID_PROTRACTOR_RADIAN,str );
			
		}

		if (nTemp == WB_OBJ_SETSQUARE)     //添加三角板独有的菜单
		{
			CString str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("Unit"));
			if (str.IsEmpty())
			{
				str = _T("单位");
			}
			PopupMenu.AppendMenu(MF_POPUP,(UINT)UnitPopupMenu.m_hMenu,str);
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("Pixel"));
			if (str.IsEmpty())
			{
				str = _T("像素");
			}
			UnitPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_SETSQUARE_PIXEL,str);	
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("Centimeter"));
			if (str.IsEmpty())
			{
				str = _T("厘米");
			}
			UnitPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_SETSQUARE_CENTIMETRE,str);
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("Inch"));
			if (str.IsEmpty())
			{
				str = _T("英寸");
			}
			//UnitPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_SETSQUARE_MILLIMETRE,_T("毫米"));
			UnitPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_SETSQUARE_INCHES,str);
			PopupMenu.AppendMenu(MF_SEPARATOR);
		}

		if (nTemp == WB_OBJ_COMPASS)   //添加圆规的独有菜单
		{
			CString str = ReadConfigText(_T("WhiteBoardMenuListText"), _T("DrawShapes"));//绘制图形
			if (str.IsEmpty())
			{
				str = _T("绘制图形");
			}
			PopupMenu.AppendMenu(MF_POPUP,(UINT)CompassPopupMenu.m_hMenu, str);
			CString strArc = ReadConfigText(_T("WhiteBoardMenuListText"), _T("Arc"));   //圆弧
			if (strArc.IsEmpty())
			{
				strArc = _T("圆弧");
			}
			
			CString strSector = ReadConfigText( _T("WhiteBoardMenuListText"), _T("Sector")); //扇形
			if (strSector.IsEmpty())
			{
				strSector = _T("扇形");
			}
			if (GetCompassOptMenu())
			{
				CompassPopupMenu.AppendMenu(MF_CHECKED | MF_STRING,ID_OBMENU_COMPASS_ARC,strArc);
				CompassPopupMenu.AppendMenu(MF_STRING, ID_OBMENU_COMPASS_SECTOR,strSector); 
			}
			else
			{
				CompassPopupMenu.AppendMenu(MF_STRING,ID_OBMENU_COMPASS_ARC,strArc);
				CompassPopupMenu.AppendMenu(MF_CHECKED | MF_STRING, ID_OBMENU_COMPASS_SECTOR,strSector); 
			}
			
		}
		if (nTemp != WB_OBJ_MOVIE
			&&nTemp != WB_OBJ_MEDIA
			&& nTemp != WB_OBJ_CAMERA
			&& nTemp !=WB_OBJ_PROTRACTOR
			&& nTemp !=WB_OBJ_ANGLE	
			&& nTemp != WB_OBJ_SETSQUARE
			&& nTemp != WB_OBJ_COMPASS
			&& g_bWBWndIsneedShowToolBtn
			&& !bMedia)
		{
			CString strAttribute;
			strAttribute = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_attribute"));
			if (strAttribute.IsEmpty())
			{
				strAttribute = _T("属性");
			}
			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_ATTRIBUTE, strAttribute);
		}	
		//菜单中加载图片
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_INFINITECLONE, MF_BYCOMMAND, &m_InfiniteCloneBmp,NULL);//无限克隆		
		//PopupMenu.SetMenuItemBitmaps(ID_OBMENU_LOCK, MF_BYCOMMAND,&m_LockBmp ,&m_UnLockBmp);
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_LOCK, MF_BYCOMMAND,&m_LockBmp ,NULL);
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_ALLUNLOCK, MF_BYCOMMAND,&m_UnLockBmp ,NULL);
		//视频
		MoviePopupMenu.SetMenuItemBitmaps(ID_OBMENU_MOVIE_PALY, MF_BYCOMMAND, &m_MoviePlayBmp,NULL); // 播放
		MoviePopupMenu.SetMenuItemBitmaps(ID_OBMENU_MOVIE_PAUSE, MF_BYCOMMAND, &m_MoviePauseBmp,NULL); // 暂停
		MoviePopupMenu.SetMenuItemBitmaps(ID_OBMENU_MOVIE_STOP, MF_BYCOMMAND, &m_MovieStopBmp,NULL); // 停止		
		MoviePopupMenu.SetMenuItemBitmaps(ID_OBMENU_MOVIE_CIRCLE, MF_BYCOMMAND, &movieCircleBmp,NULL); // 循环播放
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_MEDIA_CATCHFRAME, MF_BYCOMMAND, &m_MediaCatchFrameBmp,NULL);//截图
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_MEDIA_SETMUTE, MF_BYCOMMAND, &m_MediaSetmuteBmp,&m_MediaNoSetmuteBmp);  //静音

		//文档
		DocPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DOC_PREPAGE, MF_BYCOMMAND, &m_DocPrePageBmp,NULL); // 上一页
		DocPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DOC_FIRSTPAGE, MF_BYCOMMAND, &m_DocFirstPageBmp,NULL); // 首页
		DocPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DOC_NEXTPAGE, MF_BYCOMMAND, &m_DocNextPageBmp,NULL); // 下一页
		DocPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DOC_LASTPAGE, MF_BYCOMMAND, &m_DocLastPageBmp,NULL); // 末页

		//DXF
		DxfPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DXF_DRAG, MF_BYCOMMAND, &m_DxfDragBmp,NULL);//拖拽
		DxfPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DXF_ZOOMIN, MF_BYCOMMAND, &m_DxfZoomInBmp,NULL);//放大
		DxfPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DXF_ZOOMOUT, MF_BYCOMMAND, &m_DxfZoomOutBmp,NULL);//缩小

		DocPrintMenu.SetMenuItemBitmaps(ID_OBMENU_DOC_PRINTCUR, MF_BYCOMMAND, &m_DocPrintCurBmp,NULL);//打印当前页
		DocPrintMenu.SetMenuItemBitmaps(ID_OBMENU_DOC_PRINTALL, MF_BYCOMMAND, &m_DocPrintAllBmp,NULL);//打印所有页

		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_SELECTALL, MF_BYCOMMAND, &m_SelectAllBmp,NULL);  //全选		
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_UNSELECT, MF_BYCOMMAND, &m_UnselectBmp,NULL); // 取消选择
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_EQUALSCALEZOOM, MF_BYCOMMAND, &m_EqualScaleZoomBmp,&m_CheckedBmp); // 等比缩放
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_SCKD, MF_BYCOMMAND,&m_SckdBmp,NULL); //生产刻度
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_DELETEBACKGROUND, MF_BYCOMMAND, &m_DeleteBackgroundBmp,NULL); // 删除背景图片
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_SETASBACKGROUND, MF_BYCOMMAND, &m_SetAsBackgroundBmp,NULL); // 设为背景

		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_EDIT, MF_BYCOMMAND, &m_EditBmp,NULL);//编辑
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_CUT, MF_BYCOMMAND, &m_CutBmp,NULL);//剪切
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_COPY, MF_BYCOMMAND, &m_CopyBmp,NULL);  //复制	
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_DELETE, MF_BYCOMMAND, &m_DeleteBmp,NULL); //删除
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_DELALL, MF_BYCOMMAND, &m_DeleteAllBmp,NULL); //删除全部
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_PASTE, MF_BYCOMMAND, &m_PasteBmp,NULL); // 粘贴
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_CLONE, MF_BYCOMMAND, &m_CloneBmp,NULL); // 克隆
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_COMBINE, MF_BYCOMMAND, &m_CombineBmp,NULL); // 组合
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_UNCOMBINE, MF_BYCOMMAND, &m_UnCombineBmp,NULL);  //取消组合	
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_TOPLAYER, MF_BYCOMMAND, &m_TopLayerBmp,NULL); // 顶层
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_UPLAYER, MF_BYCOMMAND, &m_PreLayerBmp,NULL); // 上一层
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_UNDERLAYER, MF_BYCOMMAND, &m_NextLayerBmp,NULL); // 下一层
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_BOTTPMLAYER, MF_BYCOMMAND, &m_BottomLayerBmp,NULL); // 底层
		PopupMenu.SetMenuItemBitmaps(ID_OBMENU_ATTRIBUTE, MF_BYCOMMAND, &m_AttributeBmp,NULL); // 属性

		RotationPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DEASIL45, MF_BYCOMMAND, &m_Deasil45Bmp,NULL); // 顺时针45度
		RotationPopupMenu.SetMenuItemBitmaps(ID_OBMENU_DEASIL90, MF_BYCOMMAND, &m_Deasil90Bmp,NULL);  //顺时针90度
		RotationPopupMenu.SetMenuItemBitmaps(ID_OBMENU_WIDDERSHINS45, MF_BYCOMMAND, &m_Widdershins45Bmp,NULL); // 逆时针45度
		RotationPopupMenu.SetMenuItemBitmaps(ID_OBMENU_WIDDERSHINS90, MF_BYCOMMAND, &m_Widdershins90Bmp,NULL); // 逆时针90度
		RotationPopupMenu.SetMenuItemBitmaps(ID_OBMENU_ROTATION180, MF_BYCOMMAND, &m_Rotation180Bmp,NULL); //旋转180度
		RotationPopupMenu.SetMenuItemBitmaps(ID_ROTATION_HIRIZONTAL, MF_BYCOMMAND, &m_HirizontalBmp,NULL); //水平放置
		RotationPopupMenu.SetMenuItemBitmaps(ID_ROTATION_VERTICAL, MF_BYCOMMAND, &m_VerticalBmp,NULL); //垂直放置
	}

	CPoint point;
	CWBImageButton* ptemimgbtn = pObj->GetImageButton();
	if (ptemimgbtn == NULL)
	{
		pObj->InitImageButton();
	}
	point = (ptemimgbtn)->GetMenuPt();
	if (g_reduce>0.00)
	{
		point.x = point.x*g_reduce;
		point.y = point.y*g_reduce;
	}
	ptemimgbtn = NULL;
	CRect viewRectInScreen;	
	if (CScreenDrawMgr::GetIns() != NULL)
	{
		::GetWindowRect(CScreenDrawMgr::GetIns()->getScreenDrawDlg()->GetSafeHwnd(),&viewRectInScreen);
		point.x += viewRectInScreen.left; 
		point.y += viewRectInScreen.top;
	}
	else
	{
		point.x = point.x - g_pWhiteBoardView->getXnPos();
		point.y = point.y - g_pWhiteBoardView->getYnPos();
		GetWindowRect(&viewRectInScreen);
		point.x += viewRectInScreen.left; 
		point.y += viewRectInScreen.top;
	}

	if (nTemp == WB_OBJ_SETSQUARE||nTemp == WB_OBJ_PROTRACTOR || nTemp == WB_OBJ_COMPASS)
	{
		GetCursorPos(&point);  //使三角板菜单在弹出鼠标点上，不使用这个函数 菜单会位于左上角弹出
	}
	
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);  //菜单弹出的位置
	movieCircleBmp.DeleteObject();
	PopupMenu.DestroyMenu();
}

BOOL CWhiteBoardView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nItemID = LOWORD(wParam);

	switch (nItemID)
	{
	case ID_OBMENU_SELECTALL: //全选
		if (g_pWBPage->AddAllSelectFrames() > 0)
		{
			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				Invalidate();
			}
			UpdateWindow();
		}
		break;
	case ID_OBMENU_SCKD:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
			if (pObj->GetType() == WB_OBJ_RULER)
			{
				CPoint pt =((CWBRuler*)pObj)->GetRulerLT();
				int n = ((CWBRuler*)pObj)->GetNum();
				if (n == 0)
				{
					break;
				}
				else
				{
					CreateAnnRule(pt,n);
				}

			}

		}
		break;
	case ID_OBMENU_UNSELECT: //取消选择
		if (g_pWBPage->DeleteSelectFrame(m_dwMenuLayerIndex))
		{
			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				Invalidate();
			}
			UpdateWindow();
		}
		break;
	case ID_OBMENU_DELETE: //删除
		{
			//denghong
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (NULL == pObj)
			{
				break;
			}
			// 			if (pObj->IsLock()) //如果对象被锁定
			// 			{
			// 				break;
			// 			}
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				break;
			}
			int nType = pObj->GetType();

			m_ulSendEventID = GenerateSSRC();

			if (nType == WB_OBJ_COMBINE)
			{
				((CWBCombine *)pObj)->DelComMebMsg();
				//删除当前对象在选择框列表中的记录
				g_pWBPage->DeleteSelectFrame(m_dwMenuLayerIndex);
			}
			if (g_pWBPage->DeleteWBObject(m_dwMenuLayerIndex))
			{

				int j = g_pWBPage->GetSelectObjNumber();
				POSITION pos;
				if (j == 1)//删除所选列表只剩一个对象时,此即为全局对象
				{
					for (int k = 0;k < j;++k )//遍历选中对象列表
					{
						pos = g_pWBPage->GetSelectObjLst()->FindIndex(k);
						if (pos == NULL)
						{
							continue;
						}
						g_pObj = (CWBObject *)g_pWBPage->GetSelectObjLst()->GetAt(pos);
					}
				}
				if (NULL != g_pWhiteBoardView->m_pWBFunctionDlg && ::IsWindow(g_pWhiteBoardView->m_pWBFunctionDlg->GetSafeHwnd()))//控制属性面板显示相应的属性
				{						
					if (j == 0)//选中对象列表中无对象时
					{
						if (!g_bAttriDlginitialize)
						{
							::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_NOSELECTWBOBJ,0,0);
						}					
					}
					//else if (j == 1)//选中对象列表中只有一个对象时
					//{
					//	::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,0,j);
					//}
					//else//选中对象列表中存在多个对象时
					//{			
					//	for (int k = 0;k < j;++k )//遍历选中对象列表
					//	{
					//		CObList *ptempList = g_pWBPage->GetSelectObjLst();
					//		pos = ptempList->FindIndex(k);
					//		if (pos == NULL)
					//		{
					//			continue;
					//		}
					//		pObj = (CWBObject *)ptempList->GetAt(pos);
					//		int ntype = pObj->GetType();
					//		switch(ntype)
					//		{
					//		case WB_OBJ_FLASH:									
					//		case WB_OBJ_COMBINE:					
					//		case WB_OBJ_DOCUMENT:					
					//		case WB_OBJ_PICTURE:
					//			{
					//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_PICTURE,j);//隐藏线宽,线形,箭头
					//			}
					//			break;
					//		case WB_OBJ_ANNOTATIONS:
					//			{
					//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_ANNOTATIONS,j);//隐藏线宽,线形,箭头
					//			}
					//			break;
					//		case WB_OBJ_TEXT:
					//			{
					//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_TEXT,j);//隐藏线宽,线形,箭头
					//			}
					//			break;
					//		case WB_OBJ_RHOMBUS:					
					//		case WB_OBJ_TRIANGLE:
					//		case WB_OBJ_PALLELOGAM: //平行四边形
					//		case WB_OBJ_ELLIPSE:				
					//		case WB_OBJ_ARBITRARYTRIANGLE:
					//		case WB_OBJ_RECT:
					//		case WB_OBJ_PENTAGON:
					//		case WB_OBJ_HEXAGON:
					//		case WB_OBJ_FIVEPOINTSTART:
					//		case WB_OBJ_TRAPEZIUM:
					//		case WB_OBJ_POLYGON:
					//			{
					//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_POLYGON,j);//隐藏线形,箭头,字体相关属性
					//			}
					//			break;
					//		case WB_OBJ_CURVE:					
					//		case WB_OBJ_LINE:
					//			{
					//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_LINE,j);//隐藏线形,箭头,字体相关属性
					//			}
					//			break;
					//		default:
					//			break;
					//		}	
					//	}
					//}					
				}
				g_pWBPage->SelectMoreObjAttribute();//同步属性面板共同属性
				//发送同步消息
				//RemoveObj(m_dwMenuLayerIndex);
				RemoveDelCombineObj(m_dwMenuLayerIndex);
				if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
				{
					CScreenDrawMgr::GetIns()->Invalidate();
				}
				else
				{
					g_nDrawStatus = DRAW_NORMAL;
					Invalidate();
				}

				UpdateWindow();
				g_nDrawStatus = DRAW_NORMAL; //选择状态  解决三角板 删除一个其他对象移不动问题
			}
		}
		break;
	case ID_OBMENU_DELALL: //删除全部
		{
			DeleteAll();
			
		}
		break;
	case ID_OBMENU_UPLAYER: //上一层
		if (g_pWBPage->SetObjSuperLayer(m_dwMenuLayerIndex))
		{
			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				Invalidate();
			}
		}
		break;
	case ID_OBMENU_UNDERLAYER: //下一层
		if (g_pWBPage->SetObjUnderLayer(m_dwMenuLayerIndex))
		{
			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				Invalidate();
			}
		}

		break;
	case ID_OBMENU_TOPLAYER: //最顶层
		if (g_pWBPage->SetObjTopLayer(m_dwMenuLayerIndex))
		{
			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				Invalidate();
			}	
		}
		break;
	case ID_OBMENU_BOTTPMLAYER: //最底层
		if (g_pWBPage->SetObjBottomLayer(m_dwMenuLayerIndex))
		{
			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				Invalidate();
			}	
		}
		break;
	case ID_OBMENU_RORATIONCENTERZOOM://任意中心点旋转
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			pObj->SetRorationCenterZoom(!pObj->IsRorationCenterZoom());

			////发送同步消息
			//XMemIni xmlObj;
			//pObj->SetXmlValue(xmlObj);
			//ChangeObj(xmlObj);
			//((CWBImageButton*)pObj->GetImageButton())->ChangeRationCenterOperator();
			Invalidate();
			break;
		}
	case ID_OBMENU_DEASIL45: //顺时针旋转45度
		{
			RotateByAngle(-45);
		}
		break;
	case ID_OBMENU_DEASIL90: //顺时针旋转90度
		{
			RotateByAngle(-90);
		}
		break;
	case ID_OBMENU_WIDDERSHINS45: //逆时针旋转45度
		{
			RotateByAngle(45);
		}
		break;
	case ID_OBMENU_WIDDERSHINS90: //逆时针旋转90度
		{
			RotateByAngle(90);
		}
		break;
	case ID_OBMENU_ROTATION180: //旋转180度
		{
			RotateByAngle(180);
		}
		break;
	case ID_ROTATION_HIRIZONTAL: //水平放置
		{
			RotateByHV(0);
		}
		break;
	case ID_ROTATION_VERTICAL: //垂直放置
		{
			RotateByHV(90);
		}
		break;
	case ID_PROTRACTOR_ANGLE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			pObj->SetUnit(UNIT_ANGLE);
		}
		break;
	case ID_PROTRACTOR_RADIAN:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			pObj->SetUnit(UNIT_RADIAN);
		}
		break;
	case ID_OBMENU_SETSQUARE_CENTIMETRE://转换厘米
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			pObj->SetUnit(UNIT_CM);
			//pObj->g_iSetSquareUnit = UNIT_CM;
		}
		break;
	case ID_OBMENU_SETSQUARE_INCHES:   //英寸
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			pObj->SetUnit(UNIT_INCH);
		}
		break;
		//case ID_OBMENU_SETSQUARE_MILLIMETRE: //毫米
		//	{
		//		g_iSetSquareUnit = UNIT_MM;
		//	}
		//	break;
	case  ID_OBMENU_SETSQUARE_PIXEL:    //像素
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			pObj->SetUnit(UNIT_PIEXL);
		}
		break;
	case ID_OBMENU_ATTRIBUTE: //属性
		{
			OpenAttributeDlg();
		}
		break;
	case ID_OBMENU_SETASBACKGROUND: //设置背景
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj)
			{
				g_pWBPage->SetIsExistBackGround(TRUE);
				//取消已有的背景对象
				CWBObject *pBkgObj = g_pWBPage->GetBkgObj();
				if (pBkgObj)
				{
					pBkgObj->SetBkg(FALSE);
					//发送同步信息
					XMemIni xmlObj;
					pBkgObj->SetXmlValue(xmlObj);
					ChangeObj(xmlObj);
				}

				XMemIni xmlObj;

				//根据图片大小改变白板尺寸
				if (pObj->GetType() == WB_OBJ_PICTURE)
				{
					int nWidth, nHeight;
					nWidth = ((CWBPicture *)pObj)->GetWidth();
					nHeight = ((CWBPicture *)pObj)->GetHeight();
					if (!(nWidth < 2400 && nHeight < 1600))
					{
						if (nWidth > 37000)
						{
							nHeight = 37000 * nHeight / nWidth;
							nWidth = 37000;
						}
						if (nHeight > 37000)
						{
							nWidth = 37000 * nWidth / nHeight;
							nHeight = 37000;
						}

						g_nWidth = nWidth;
						g_nHeight = nHeight;
						setViewScroll();
						m_pEagleEyeDlg->GetPreviewSingleView()->SetRatio();
						m_pEagleEyeDlg->GetPreviewSingleView()->SetRectInPreview();

						xmlObj.Set(KEY_WHITEBOARD_WIDTH, g_nWidth);
						xmlObj.Set(KEY_WHITEBOARD_HEIGHT, g_nHeight);
					}
				}

				//设置背景
				pObj->SetBkg(TRUE);
				g_pWBPage->DeleteSelectFrame(m_dwMenuLayerIndex);
				Invalidate();

				//发送同步信息
				pObj->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
			}
		}
		break;
	case ID_OBMENU_DELETEBACKGROUND://删除当前背景图
		{
			//取消已有的背景对象
			CWBObject *pBkgObj = g_pWBPage->GetBkgObj();
			if (pBkgObj)
			{
				//取消背景时，将白板大小设成默认值大小，
				/*g_nWidth = WB_VIEWHSCROLL;
				g_nHeight = WB_VIEWVSCROLL;
				setViewScroll();
				m_pEagleEyeDlg->GetPreviewSingleView()->SetRatio();
				m_pEagleEyeDlg->GetPreviewSingleView()->SetRectInPreview();*/
				g_pWBPage->SetIsExistBackGround(FALSE);
				pBkgObj->SetBkg(FALSE);
				Invalidate();

				//发送同步信息
				XMemIni xmlObj;
				pBkgObj->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
			}
		}
		break;
	case ID_OBMENU_MOVIE_PALY: //视频播放
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			WB_OBJ_TYPE ntemptype = (WB_OBJ_TYPE)pObj->GetType();
			if ( ntemptype== WB_OBJ_FLASH) //如果对象是flash
			{
				((CWBFlash *)pObj)->Start();
			}
			if (ntemptype == WB_OBJ_MEDIA)
			{
				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					((CWBMedia*)pObj)->MediaPlay();
				}
				else
				{
					((CWBMedia*)pObj)->SetMediaPlayState(Media_playing);
				}
			}
			Invalidate();
			//发送同步信息
			XMemIni xmlObj;			
			pObj->SetXmlValue(xmlObj);
			if (ntemptype == WB_OBJ_MEDIA || ntemptype == WB_OBJ_FLASH)
			{
				xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_START);
			}
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case ID_OBMENU_MOVIE_STOP: //视频停止
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			WB_OBJ_TYPE ntemptype = (WB_OBJ_TYPE)pObj->GetType();
			if (ntemptype == WB_OBJ_FLASH) //如果对象是flash
			{
				((CWBFlash *)pObj)->Stop();
			}
			if (ntemptype == WB_OBJ_MEDIA)
			{
				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					((CWBMedia*)pObj)->MediaStop();
				}
				else
				{
					((CWBMedia*)pObj)->SetMediaPlayState(Media_stop);
				}
			}
			Invalidate();
			//发送同步信息
			XMemIni xmlObj;			
			pObj->SetXmlValue(xmlObj);
			if (ntemptype == WB_OBJ_MEDIA || ntemptype == WB_OBJ_FLASH)
			{
				xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_STOP);
			}
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case ID_OBMENU_MOVIE_PAUSE: //视频暂停
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			WB_OBJ_TYPE ntemptype = (WB_OBJ_TYPE)pObj->GetType();
			if (ntemptype == WB_OBJ_FLASH) //如果对象是flash
			{
				((CWBFlash *)pObj)->Pause();
			}
			if (ntemptype == WB_OBJ_MEDIA)
			{
				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					((CWBMedia*)pObj)->MediaPause();
				}
				else
				{
					((CWBMedia*)pObj)->SetMediaPlayState(Media_pausing);
				}
			}
			Invalidate();
			//发送同步信息
			XMemIni xmlObj;			
			pObj->SetXmlValue(xmlObj);
			if (ntemptype == WB_OBJ_MEDIA || ntemptype == WB_OBJ_FLASH)
			{
				xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_PAUSE);
			}
			g_pWhiteBoardView->ChangeObj(xmlObj);

		}
		break;
	case ID_OBMENU_MOVIE_CIRCLE: //视频循环播放
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			WB_OBJ_TYPE ntemptype = (WB_OBJ_TYPE)pObj->GetType();
			if (ntemptype == WB_OBJ_FLASH) //如果对象是flash
			{
				BOOL isLoop = ((CWBFlash*)pObj)->GetCircle();
				if (isLoop)
				{
					((CWBFlash *)pObj)->SetSinglePlay();
				}
				else
				{
					((CWBFlash *)pObj)->SetCircle();
				}
			}			
			if (ntemptype == WB_OBJ_MEDIA)
			{
				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					((CWBMedia*)pObj)->MediaLoop();
				}
				else
				{
					BOOL isLoop = ((CWBMedia*)pObj)->GetMediaLoopState();
					((CWBMedia*)pObj)->SetMediaLoopState(!isLoop);
				}
			}
			Invalidate();
			//发送同步信息
			XMemIni xmlObj;			
			pObj->SetXmlValue(xmlObj);
			if (ntemptype == WB_OBJ_MOVIE)
			{
				xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_LOOP);
			}
			if (ntemptype == WB_OBJ_FLASH)
			{
				BOOL isLoop = ((CWBFlash*)pObj)->GetCircle();
				if (isLoop)
				{
					xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_LOOP);
				}
				else
				{
					xmlObj.Set(KEY_MEDIA_OPERATION, WB_MOVIE_SINGLEPLAY);
				}
			}
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case ID_OBMENU_MEDIA_CATCHFRAME:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				if (((CWBMedia*)pObj)->CaptureToBMPFile())
				{
					break;
				}
				return FALSE;
			}
		}
	case ID_OBMENU_MEDIA_SETMUTE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				/*
				*得到当前的是否静音的状态，并让视频在静音和非静音中切换
				*如果是本地创建的，则直接调用静音方法,然后发送同步
				*如果是被动创建的则先设置状态，然后发送同步
				*其它相似
				*/
				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					EVoiceState voiceState = ((CWBMedia*)pObj)->GetVoiceState();
					if (voiceState == VOICEOPEN)
					{
						((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
					} 
					else
					{
						((CWBMedia*)pObj)->MediaSetMute(VOICEOPEN);
					}
				}
				else
				{
					EVoiceState voiceState = ((CWBMedia*)pObj)->GetVoiceState();
					if (voiceState == VOICEOPEN)
					{
						((CWBMedia*)pObj)->SetVoiceState(VOICECLOSE);
					} 
					else
					{
						((CWBMedia*)pObj)->SetVoiceState(VOICEOPEN);
					}
				}
				Invalidate();
			}
			//发送同步信息
			XMemIni xmlObj;			
			pObj->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
		break;
	case ID_OBMENU_DOC_PREPAGE: //文档上一页
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() != WB_OBJ_DOCUMENT)
			{
				break;
			}

			if (((CWBDocument *)pObj)->PrePage())
			{
				Invalidate();

				//文档翻页同步
				XMemIni xmlObj;
				((CWBDocument *)pObj)->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
			}			
		}
		break;
	case ID_OBMENU_DOC_NEXTPAGE: //文档下一页
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() != WB_OBJ_DOCUMENT)
			{
				break;
			}

			if (((CWBDocument *)pObj)->NextPage())
			{
				Invalidate();

				//文档翻页同步
				XMemIni xmlObj;
				((CWBDocument *)pObj)->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
			}
		}
		break;
	case ID_OBMENU_DOC_FIRSTPAGE: //文档第一页
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() != WB_OBJ_DOCUMENT)
			{
				break;
			}

			if (((CWBDocument *)pObj)->FirstPage())
			{
				Invalidate();

				//文档翻页同步
				XMemIni xmlObj;
				((CWBDocument *)pObj)->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
			}
		}
		break;
	case ID_OBMENU_DOC_LASTPAGE: //文档末页
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() != WB_OBJ_DOCUMENT)
			{
				break;
			}

			if (((CWBDocument *)pObj)->LastPage())
			{
				Invalidate();

				//文档翻页同步
				XMemIni xmlObj;
				((CWBDocument *)pObj)->SetXmlValue(xmlObj);
				ChangeObj(xmlObj);
			}

		}
		break;
	case ID_OBMENU_LOCK: //锁定/解锁
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			pObj->Lock(!pObj->IsLock());
			if (CScreenDrawMgr::GetIns() != NULL)
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			} 
			else
			{
				Invalidate();
			}

			POSITION pos;
			CObList *ptemplist = g_pWBPage->GetSelectObjLst();
			int i = ptemplist->GetSize();
			if (i == 1)//选中一个对象时
			{
				for (int j = 0;j < i;++j )//遍历选中对象列表
				{
					pos = ptemplist->FindIndex(j);
					if (pos == NULL)
					{
						continue;
					}
					g_pObj = (CWBObject *)ptemplist->GetAt(pos);
				}
				if (NULL != g_pWhiteBoardView->m_pWBFunctionDlg && ::IsWindow(g_pWhiteBoardView->m_pWBFunctionDlg->GetSafeHwnd()))
				{					
					::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,0,i);					
				}
			}
			//wangxin 12-27
			//LockObjectMsg(pObj->GetLayerIndex());

			//发送同步消息
			XMemIni xmlObj;
			pObj->SetXmlValue(xmlObj);
			ChangeObj(xmlObj);
		}
		break;
	case ID_OBMENU_ALLUNLOCK:
		{
			AllUnLock();
		}
		break;
	case ID_OBMENU_COPY:
		{
			CopyObject();
		}
		break;
	case ID_OBMENU_CUT:
		{
			CutObject();
		}
		break;
	case ID_OBMENU_PASTE:
		{
			PasteObject();
		}
		break;
	case ID_OBMENU_CLONE:
		{
			CloneObject();
		}
		break;
	case ID_OBMENU_INFINITECLONE: //无限克隆
		{
			InfiniteCloneObj();
		}
		break;
	case ID_OBMENU_EQUALSCALEZOOM: //设置等比例缩放
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			pObj->SetEqualScaleZoom(!pObj->IsEqualScaleZoom());

			//发送同步消息
			XMemIni xmlObj;
			pObj->SetXmlValue(xmlObj);
			ChangeObj(xmlObj);
		}
		break;
	case ID_OBMENU_PRODUCTIONSCALE:  //生成刻度
		{
			AddAngle();
		}
		break;
	case ID_OBMENU_COMPASS_SECTOR:  //扇形
		{
			SetCompassDrawShape(1);
			SetCompassOptMenu(FALSE);
		}
		break;
	case ID_OBMENU_COMPASS_ARC:
		{
			SetCompassDrawShape(0);
			SetCompassOptMenu(TRUE);
		}
		break;
	case ID_OBMENU_COMBINE: //组合
		{
			CWBCombine *pObj = new CWBCombine();
			pObj->AddCombineLayer(); //添加组合对象的图层
			if (!pObj->AddCombineObj())
			{
				delete pObj;
				break;
			}
			g_pWBPage->ClearSelectFrameList();
			pObj->CreateSelecrtFrame();
			pObj->SetEqualScaleZoom(TRUE);
			pObj->SetSelectFrameShow(TRUE);
			g_pWBPage->AddWBObject(pObj, WB_LAYER_NORMAL);


			pObj->InitImageButton();
			g_pWBPage->AddSelectFrame(pObj->GetLayerIndex());
			//g_pObj = pObj;
			unsigned long ulEventID = GenerateSSRC();
			pObj->ChangeObjItem(ulEventID, pObj->GetLayerIndex(), TRUE);
			//因I7中组合后，需要设置当前的对象为正在编辑的对象
			if(g_bSupportI7)
			{
				g_pWBPage->SetCurEditingObjLayerIndex(pObj->GetLayerIndex());
			}

			//发送同步消息
			XMemIni xmlObj;
			xmlObj.Set(KEY_EVENT_ID, ulEventID);
			pObj->SetXmlValue(xmlObj);
			AddObj(xmlObj);
			((CWBCombine *)pObj)->SetComLayerNum();

			m_dwCurSelectObjLayerIndex = pObj->GetLayerIndex();
			if (g_pHPWBDataNotify && g_bSupportI7)
			{
				MSG msg;
				memset(&msg, 0, sizeof(MSG));
				msg.hwnd = NULL;
				msg.lParam = (LPARAM)0;
				msg.message = 1;
				msg.wParam = (WPARAM)1;
				g_pHPWBDataNotify->SetSelectObjStatue(msg);
			}
			Invalidate();
		}
		break;
	case ID_OBMENU_UNCOMBINE: //取消组合
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->IsLock() ||pObj->IsLimit())
			{
				break;
			}
			g_pWBPage->ClearSelectFrameList();
			m_ulSendEventID = GenerateSSRC();
			((CWBCombine *)pObj)->ChangeObjItem(m_ulSendEventID, 0, FALSE);
			((CWBCombine *)pObj)->CancelCombineObj(m_dwMenuLayerIndex,g_ulSessionID);
			g_pWBPage->DeleteWBObject(m_dwMenuLayerIndex);
			//发送同步消息
			RemoveObj(m_dwMenuLayerIndex);
			Invalidate();
		}
	case ID_OBMENU_EDIT://编辑文本
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->IsLock() || pObj->IsLimit())
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_ARBITRARYTRIANGLE)
			{
				
				pObj->SetSelectFrameShow(FALSE);
				((CWBArbitraryTriangle*)pObj)->SetIsEdit(true);
				g_nDrawStatus = DRAW_NORMAL;
				if (CScreenDrawMgr::GetIns() != NULL)
				{
					CScreenDrawMgr::GetIns()->Invalidate();
				}
				else
				{
					Invalidate();
				}
				break;
			}
			m_bIsEditing = TRUE;
			m_pEditObj = pObj;
			if (pObj->GetType() == WB_OBJ_ANNOTATIONS)
			{
				m_bIsAnnotations = TRUE;
			}
			else if (pObj->GetType() == WB_OBJ_TEXT)
			{
				m_bIsAnnotations = FALSE;
			}
			g_pWBPage->DeleteSelectFrame(m_dwMenuLayerIndex);
			setFrameListAndObjLayerIndex();

			CRect rect = GetEditTextRect();
			if (!CWBRichEditMgr::GetInst_()->IsRichEditShow())
			{
				SendMessage( UWM_EDIT_RICHEDIT, (WPARAM)(MAKELONG(rect.left, rect.top)), (LPARAM)(MAKELONG(rect.right, rect.bottom)) );
				SetPageIsNeedDraw(TRUE);
				Invalidate();
			}

			g_pWBPage->DelWBObj(m_dwMenuLayerIndex);
			//RemoveObj(m_dwMenuLayerIndex);

			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			else
			{
				Invalidate();
			}

			if (m_bIsAnnotations)
			{
				g_nDrawStatus = ADD_ANNOTATIONS;
			} 
			else
			{
				g_nDrawStatus = DRAW_TEXT;
			}
			g_nDrawStatus = DRAWED_TEXT;

			DRAW_STATE nDrawStatus = g_nDrawStatus;
			g_nDrawStatus = DRAW_TEXT;
			if (NULL != m_pWBFunctionDlg && ::IsWindow(m_pWBFunctionDlg->GetSafeHwnd()))
			{
				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBPEN,0,0);

			}
			g_nDrawStatus = nDrawStatus;
			if (CScreenDrawMgr::GetIns())
			{
				m_bUseToControlDraw = false;
			}
			else
			{
				m_bUseToControlDraw = true;
			}
			
		}
		break;
	case ID_OBMENU_ANNOTATIONS: //添加标注
		{
			WB_SetDrawStatus(ADD_ANNOTATIONS);
			
		}
		break;
	case ID_OBMENU_WATER:
		{
			WB_SetWaterStatus();
			if (!g_bRunWater)
			{
				RejectBroadcastUserWater(ToStr(g_strAvconUserID.GetBuffer(g_strAvconUserID.GetLength())));
				g_strAvconUserID.ReleaseBuffer();
			}
		}
		break;
	case ID_START_RECOGNITION:
		{
			m_bStartRecognition = !m_bStartRecognition;

		}
		break;
	case ID_OBMENU_BROADCAST_CURSOR: 
		{
			m_bBroadcastUserCursor = !m_bBroadcastUserCursor;
			if (m_bBroadcastUserCursor&&!g_pWhiteBoardView->GetIsLocalWhiteboard())
			{
				POINT CurPoint;
				GetCursorPos(&CurPoint);
				CPoint pt = CurPoint;
				
				std::string strUserid = ToStr(g_strLocalUserName.GetBuffer(g_strLocalUserName.GetLength()));
				g_strLocalUserName.ReleaseBuffer();
				char*   pszAvconUserID = new char[strUserid.length() +1]();
				strcpy(pszAvconUserID,strUserid.c_str());	
				if (m_pTransparent_dlg == NULL)
				{
					m_pTransparent_dlg = new CTransparent_Dlg(this,false);
					m_pTransparent_dlg->SetStrname(pszAvconUserID);
					m_pTransparent_dlg->Create(CTransparent_Dlg::IDD,this);
				}
				CRect rectImg;
				m_pTransparent_dlg->GetClientRect(&rectImg);
				POINT _ptBR = CurPoint;
				POINT _ptpoint = CurPoint;
				_ptBR.y -= 30;
				_ptpoint.x += rectImg.Width() ;
				_ptpoint.y += 40;
				rectImg.SetRect(_ptBR,_ptpoint);
				ClientToScreen(&rectImg);

				m_pTransparent_dlg->MoveWindow(&rectImg);
				m_pTransparent_dlg->ShowWindow(SW_SHOW);
				BroadcastUserCursor(pt,pszAvconUserID);
				delete []pszAvconUserID;
				pszAvconUserID = NULL;
				//WriteBroadCastStatus(50);
			}
			else
			{
				if (m_pTransparent_dlg != NULL)
				{
					delete m_pTransparent_dlg;
					m_pTransparent_dlg = NULL;
				}
				RejectBroadcastUserCursor(ToStr(g_strLocalUserName.GetBuffer(g_strLocalUserName.GetLength())));
				g_strLocalUserName.ReleaseBuffer();
				//WriteBroadCastStatus(10);
			}
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_ONE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();
					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 32)
					{
						//SetBirtrateCur(0);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(32);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(0);

					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);
			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);

		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_TWO:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 64)
					{
						//SetBirtrateCur(1);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(64);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(1);

					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);
			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_THREE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 128)
					{
						//SetBirtrateCur(2);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(128);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(2);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);
			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_FOUR:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 256)
					{
						//SetBirtrateCur(3);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(256);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(3);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_FIVE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 384)
					{
						//SetBirtrateCur(4);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(384);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(4);
					}
				}
			}
			//发送同步消息 
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_SIX:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					//if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 512)
					//{
					//SetBirtrateCur(5);
					int volume = ((CWBMedia*)pObj)->GetVedioVolume();
					((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(512);
					((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
					((CWBMedia*)pObj)->SetVedioVolume(volume);
					if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
					{
						((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
					}
					WriteMediaBitrate(5);
					//}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_SEV:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 768)
					{
						//SetBirtrateCur(6);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(768);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(6);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_EIG:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 1000)
					{
						//SetBirtrateCur(7);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(1000);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(7);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_NINE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 1500)
					{
						//SetBirtrateCur(8);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(1500);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(8);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_TEN:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 2000)
					{
						//SetBirtrateCur(9);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(2000);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(9);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_ELE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 3000)
					{
						//SetBirtrateCur(10);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(3000);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(10);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_TVE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 4000)
					{
						//SetBirtrateCur(11);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(4000);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(11);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_THI:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 5000)
					{
						//SetBirtrateCur(12);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(5000);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaBitrate(12);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_FOTH:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != 6000)
					{
						//SetBirtrateCur(13);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						((CWBMedia*)pObj)->GetMediaWindow()->SetBirtrate(6000);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						
						WriteMediaBitrate(13);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_COL_ONE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();

					//if (((CWBMedia*)pObj)->GetMediaWindow()->GetBirtrate() != GetControlBirtrateCur())
					//{
					//SetControlBirtrateCur(0);
					int volume = ((CWBMedia*)pObj)->GetVedioVolume();
					((CWBMedia*)pObj)->GetMediaWindow()->SetControlBirtrate(0);
					((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
					((CWBMedia*)pObj)->SetVedioVolume(volume);
					if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
					{
						((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
					}
					WriteMediaControlBitrate(0);
					//}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_COL_TWO:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();
					CWBMediaWindow *ptempWindow = ((CWBMedia*)pObj)->GetMediaWindow();
					if (ptempWindow->GetControlBirtrate() != 1)
					{
						//SetControlBirtrateCur(1);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						ptempWindow->SetControlBirtrate(1);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaControlBitrate(1);
						ptempWindow = NULL;
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_COL_THE:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();
					CWBMediaWindow *ptempWindow = ((CWBMedia*)pObj)->GetMediaWindow();
					if (ptempWindow->GetControlBirtrate() != 2)
					{
						//SetControlBirtrateCur(2);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						ptempWindow->SetControlBirtrate(2);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaControlBitrate(2);
						ptempWindow = NULL;
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_COL_FOR:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();
					CWBMediaWindow *ptempWindow = ((CWBMedia*)pObj)->GetMediaWindow();
					if (ptempWindow->GetControlBirtrate() != 3)
					{
						//SetControlBirtrateCur(3);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						ptempWindow->SetControlBirtrate(3);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaControlBitrate(3);
						ptempWindow = NULL;
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MEDIA_BIRTRATE_COL_FIV:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{

				if (((CWBMedia*)pObj)->IsActiveCreated())
				{
					unsigned long bitrate = ((CWBMedia*)pObj)->GetVedioPosition();
					CWBMediaWindow *ptempWindow = ((CWBMedia*)pObj)->GetMediaWindow();
					if (ptempWindow->GetControlBirtrate() != 4)
					{
						//SetControlBirtrateCur(4);
						int volume = ((CWBMedia*)pObj)->GetVedioVolume();
						ptempWindow->SetControlBirtrate(4);
						((CWBMedia*)pObj)->SeekVedioPosition(bitrate);
						((CWBMedia*)pObj)->SetVedioVolume(volume);
						if (((CWBMedia*)pObj)->GetVoiceState() == VOICECLOSE)
						{
							((CWBMedia*)pObj)->MediaSetMute(VOICECLOSE);
						}
						WriteMediaControlBitrate(4);
					}
				}
			}
			//发送同步信息
			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			((CWBMedia*)pObj)->SetMediaBitrate(TRUE);
			if (pObj->GetType() == WB_OBJ_MEDIA)
			{
				xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);

			}
			((CWBMedia*)pObj)->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_MEDIA_PLAY,Media_playing);
			g_pWhiteBoardView->ChangeObj(xmlObj);
			((CWBMedia*)pObj)->SetMediaBitrate(FALSE);
		}
		break;
	case ID_OBMENU_MOVIE_CATCHFRAME:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_FLASH)
			{
				CString strFileName;
				strFileName = ((CWBFlash *)pObj)->CatchFlash();
				CWBPicture *pPictrue = new CWBPicture(strFileName);
				g_pWBPage->AddWBObject(pPictrue);
				CPoint FlashPoint(72, 44);
				pPictrue->MoveOffset(FlashPoint);

				//发送同步消息
				XMemIni xmlObj;
				int nPageIndex = GetPrePage();
				if (nPageIndex != 0)
				{
					xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
					SetPrePage(0);
				}
				pPictrue->SetXmlValue(xmlObj);
				g_pWhiteBoardView->Invalidate(TRUE);
				AddObj(xmlObj);
			}
		}
		break;
	case ID_OBMENU_DOC_PRINTCUR:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_DOCUMENT)
			{
				((CWBDocument *)pObj)->PrintCurPage();
			}
		}
		break;
	case ID_OBMENU_DOC_PRINTALL:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_DOCUMENT)
			{
				int nPageCount = ((CWBDocument *)pObj)->GetPageCount();
				g_pWhiteBoardView->PrintBmp(nPageCount);
			}

		}
		break;
	case ID_OBMENU_DXF_DRAG:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_DXF)
			{
				g_nDrawStatus=DXF_PREDRAG;
			}
		}
		break;
	case ID_OBMENU_DXF_ZOOMIN:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_DXF)
			{
				((CWBDxf*)pObj)->ZoomIn(1.2);
				((CWBDxf *)pObj)->SetIsNeedReDraw(); 
				Invalidate();

				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
		}
		break;
	case ID_OBMENU_DXF_ZOOMOUT:
		{
			CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
			if (pObj == NULL)
			{
				break;
			}
			if (pObj->GetType() == WB_OBJ_DXF)
			{
				((CWBDxf*)pObj)->ZoomOut(1.2);
				((CWBDxf *)pObj)->SetIsNeedReDraw(); 
				Invalidate();

				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				g_pWhiteBoardView->ChangeObj(xmlObj);
			}
		}
		break;
// 	case ID_OBMENU_MAGICPEN:
// 		{
// 			WB_SetDrawStatus(DRAW_MAGICLINE);
// 			if (g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
// 			{
// 				g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
// 			}
// 		}
// 		break;
	default:
		return CScrollView::OnCommand(wParam, lParam);
	}

	return TRUE;
}

//更新对象透明度 caoyuqin2011-6-23
void CWhiteBoardView::UpdateObjAlpha()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	}
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID		
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			if (pObj == NULL)
			{
				continue;
			}

			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();

				switch (nObjType)
				{
				case WB_OBJ_LINE: //直线
					{
						CWBLine *pLine = (CWBLine *)pObj;
						pLine->SetAlpha((int)g_nTranspar);

						pLine->SetXmlValue(xmlObj);						
					}
					break;
				case WB_OBJ_CURVE: //曲线
					{
						CWBCurve *pCurve = (CWBCurve *)pObj;
						pCurve->SetAlpha((int)g_nTranspar);
						pCurve->SetIsDraw();//重画曲线wangxiaobin

						pCurve->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_ICOCURVE: //图标曲线
					{
						CWBIcoCurve *pIcoCurve = (CWBIcoCurve *)pObj;
						pIcoCurve->SetAlpha((int)g_nTranspar);
						pIcoCurve->SetIsRefush(TRUE);
						pIcoCurve->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_POLYGON: //多边形
				case WB_OBJ_RECT:	//矩形
				case WB_OBJ_PALLELOGAM: //平行四边形
				case WB_OBJ_TRIANGLE: //三角形
				case WB_OBJ_ARBITRARYTRIANGLE://任意三角形
				case WB_OBJ_PENTAGON://五边形
				case WB_OBJ_HEXAGON://六边形
				case WB_OBJ_FIVEPOINTSTART:
				case WB_OBJ_TRAPEZIUM:
				case WB_OBJ_RHOMBUS: //菱形
					{
						CWBPolygon *pPolygon = (CWBPolygon *)pObj;
						pPolygon->SetAlpha((int)g_nTranspar);
						pPolygon->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_ELLIPSE: //椭圆
					{
						CWBEllipse *pEllipse = (CWBEllipse *)pObj;
						pEllipse->SetAlpha((int)g_nTranspar);

						pEllipse->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_PICTURE: //图片
					{
						CWBPicture *pPicture = (CWBPicture *)pObj;
						pPicture->SetAlpha((int)g_nTranspar);

						pPicture->SetIsPath();//图片加载
						pPicture->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_FLASH: //flash
					{
						CWBFlash *pFlash = (CWBFlash *)pObj;
						pFlash->SetAlpha((int)g_nTranspar);
						pFlash->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_DOCUMENT: //文档
					{
						CWBDocument *pDocument = (CWBDocument *)pObj;
						pDocument->SetAlpha((int)g_nTranspar);
						pDocument->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_TEXT: //文字
					{
						CWBText *pText = (CWBText *)pObj;
						pText->SetAlpha((int)g_nTranspar);

						pText->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_COMBINE: //组合
					{
						CWBCombine *pCombine = (CWBCombine *)pObj;
						pCombine->SetAlpha((int)g_nTranspar);
						pCombine->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						pAnnotations->SetAlpha((int)g_nTranspar);
						pAnnotations->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_DXF: //DXF
					{
						CWBDxf *pDxf = (CWBDxf *)pObj;
						pDxf->SetAlpha((int)g_nTranspar);
						pDxf->SetIsNeedReDraw();
						pDxf->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_ARC://圆弧
					{
						CWBArc *pArc = (CWBArc *)pObj;
						pArc->SetAlpha((int)g_nTranspar);
						pArc->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_SECTOR://扇形
					{
						CWBSector *pSector = (CWBSector *)pObj;
						pSector->SetAlpha((int)g_nTranspar);
						pSector->SetXmlValue(xmlObj);
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->SetInvalidateway(true);
			CScreenDrawMgr::GetIns()->Invalidate();
			CScreenDrawMgr::GetIns()->SetInvalidateway(false);
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjColor()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case WB_OBJ_LINE: //直线
					{
						PWBLineItem pLineItem = new WBLineItem;
						CWBLine *pLine = (CWBLine *)pObj;
						pLine->GetItem(pLineItem); 
						pLineItem->clrLine = g_clrWBCurObjColor;
						pLine->SetItem(pLineItem);

						pLine->SetXmlValue(xmlObj);

						delete pLineItem;
					}
					break;
				case WB_OBJ_CURVE: //曲线
					{
						CWBCurve *pCurve = (CWBCurve *)pObj;
						PWBCurveItem pCurveItem = pCurve->GetItem();
						pCurveItem->SetColor(g_clrWBCurObjColor);
						pCurve->SetItem(pCurveItem);
						pCurve->SetIsDraw();//重画曲线wangxiaobin
						pCurve->SetXmlValue(xmlObj);

						//CWBCurve::ClearCurveItem(pCurveItem);//by jiabangping 2011.7.17下面的语句已经完成了此功能
						delete pCurveItem;
					}
					break;
				case WB_OBJ_POLYGON: //多边形
				case WB_OBJ_RECT:	//矩形
				case WB_OBJ_PALLELOGAM: //平行四边形
				case WB_OBJ_TRIANGLE: //三角形
				case WB_OBJ_ARBITRARYTRIANGLE: //任意三角形
				case WB_OBJ_PENTAGON://五边形
				case WB_OBJ_HEXAGON://六边形
				case WB_OBJ_FIVEPOINTSTART:
				case WB_OBJ_TRAPEZIUM:
				case WB_OBJ_RHOMBUS: //菱形
					{
						PWBPolygonItem pPolygonItem = new WBPolygonItem;
						CWBPolygon *pPolygon = (CWBPolygon *)pObj;
						pPolygon->GetItem(pPolygonItem);
						pPolygonItem->clrLine = g_clrWBCurObjColor;
						pPolygon->SetItem(pPolygonItem);
						pPolygon->CreateSelecrtFrame();

						pPolygon->SetXmlValue(xmlObj);
						CWBPolygon::ClearItem(pPolygonItem);
						delete pPolygonItem;
					}
					break;
				case WB_OBJ_ELLIPSE: //椭圆
					{
						PWBEllipseItem pEllipseItem = new WBEllipseItem;
						CWBEllipse *pEllipse = (CWBEllipse *)pObj;
						pEllipse->GetItem(pEllipseItem); 			
						pEllipseItem->clrLine = g_clrWBCurObjColor;
						pEllipse->SetItem(pEllipseItem);
						pEllipse->CreateSelecrtFrame();
						pEllipse->SetXmlValue(xmlObj);
						delete pEllipseItem;
					}
					break;
				case  WB_OBJ_TEXT://文字
					{
						PWBTextItem pTextItem = new WBTextItem;
						CWBText *pText = (CWBText *)pObj;
						pText->GetItem(pTextItem);
						pTextItem->clrText = g_clrWBCurObjColor;
						pText->SetItem(pTextItem);
						pText->SetXmlValue(xmlObj);
						delete pTextItem;
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						CWBPolygon *pPolygon = pAnnotations->GetPolygon();
						PWBPolygonItem pPolygonItem = new WBPolygonItem;
						pPolygon->GetItem(pPolygonItem);
						pPolygonItem->clrLine = g_clrWBCurObjColor;
						pPolygon->SetItem(pPolygonItem);
						CWBPolygon::ClearItem(pPolygonItem);
						delete pPolygonItem;

						CWBText *pText = pAnnotations->GetText();
						PWBTextItem pTextItem = new WBTextItem;
						pText->GetItem(pTextItem);
						pTextItem->clrText = g_clrWBCurObjColor;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						delete pTextItem;

						pAnnotations->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_ARC://圆弧
					{
						PWBArcItem pArcItem = new WBArcItem;
						CWBArc *pArc = (CWBArc *)pObj;
						pArc->GetItem(pArcItem); 
						pArcItem->clrLine = g_clrWBCurObjColor;
						pArc->SetItem(pArcItem);
						pArc->SetXmlValue(xmlObj);

						delete pArcItem;
					}
					break;
				case WB_OBJ_SECTOR: //扇形
					{
						PWBSectorItem pSectorItem = new WBSectorItem;
						CWBSector *pSector = (CWBSector *)pObj;
						pSector->GetItem(pSectorItem); 			
						pSectorItem->clrLine = g_clrWBCurObjColor;
						pSector->SetItem(pSectorItem);
						pSector->CreateSelecrtFrame();
						pSector->SetXmlValue(xmlObj);
						delete pSectorItem;
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		//if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		//{
		//	g_pWhiteBoardView->SetPageIsNeedDraw(TRUE);
		//	CScreenDrawMgr::GetIns()->Invalidate();
		//}
		//else
		//{
		//	Invalidate();
		//}
	}
}

void CWhiteBoardView::UpdateObjLineWidth()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case WB_OBJ_LINE: //直线
					{
						PWBLineItem pLineItem = new WBLineItem;
						CWBLine *pLine = (CWBLine *)pObj;
						pLine->GetItem(pLineItem); 
						pLineItem->nWidth = g_nWBCurObjWidth;
						pLine->SetItem(pLineItem);

						pLine->SetXmlValue(xmlObj);
						delete pLineItem;
					}
					break;
				case WB_OBJ_CURVE: //曲线
					{
						CWBCurve *pCurve = (CWBCurve *)pObj;
						PWBCurveItem pCurveItem = pCurve->GetItem();
						pCurveItem->SetWidth(g_nWBCurObjWidth);
						pCurve->SetItem(pCurveItem);
						pCurve->SetIsDraw();//重画曲线wangxiaobin
						pCurve->SetXmlValue(xmlObj);
						//CWBCurve::ClearCurveItem(pCurveItem);//by jiabangping 2011.7.17下面的语句已经完成了此功能
						delete pCurveItem;
					}
					break;
				case WB_OBJ_ICOCURVE: //曲线图标 add by jiangchao 2013.5.14
					{
						CWBIcoCurve * pIcoCurve = (CWBIcoCurve *)pObj;
						PWBIcoCurveItem pIcoCurveItem = pIcoCurve->GetItem();
						pIcoCurveItem->SetScaleWidth(g_nWBCurObjWidth);
						pIcoCurve->SetItem(pIcoCurveItem);
						pIcoCurve->SetIcoImage();
						pIcoCurve->SetIsRefush(TRUE);
						pIcoCurve->SetXmlValue(xmlObj);
						delete pIcoCurveItem;
					}
					break;
				case WB_OBJ_POLYGON: //多边形
				case WB_OBJ_RECT:	//矩形
				case WB_OBJ_PALLELOGAM: //平行四边形
				case WB_OBJ_TRIANGLE: //三角形
				case WB_OBJ_ARBITRARYTRIANGLE: //任意三角形
				case WB_OBJ_PENTAGON://五边形
				case WB_OBJ_HEXAGON://六边形
				case WB_OBJ_FIVEPOINTSTART:
				case WB_OBJ_TRAPEZIUM:
				case WB_OBJ_RHOMBUS: //菱形
					{
						PWBPolygonItem pPolygonItem = new WBPolygonItem;
						CWBPolygon *pPolygon = (CWBPolygon *)pObj;
						pPolygon->GetItem(pPolygonItem);
						pPolygonItem->nWidth = g_nWBCurObjWidth;
						pPolygon->SetItem(pPolygonItem);
						pPolygon->CreateSelecrtFrame();

						pPolygon->SetXmlValue(xmlObj);
						CWBPolygon::ClearItem(pPolygonItem);
						delete pPolygonItem;
					}
					break;
				case WB_OBJ_ELLIPSE: //椭圆
					{
						PWBEllipseItem pEllipseItem = new WBEllipseItem;
						CWBEllipse *pEllipse = (CWBEllipse *)pObj;
						pEllipse->GetItem(pEllipseItem); 			
						pEllipseItem->nWidth = g_nWBCurObjWidth;
						pEllipse->SetItem(pEllipseItem);
						pEllipse->CreateSelecrtFrame();
						pEllipse->SetXmlValue(xmlObj);
						delete pEllipseItem;
					}
					break;
				case WB_OBJ_ARC://圆弧
					{
						PWBArcItem pArcItem = new WBArcItem;
						CWBArc *pArc = (CWBArc *)pObj;
						pArc->GetItem(pArcItem); 
						pArcItem->nWidth = g_nWBCurObjWidth;
						pArc->SetItem(pArcItem);
						pArc->CreateSelecrtFrame();
						pArc->SetXmlValue(xmlObj);

						delete pArcItem;
					}
					break;
				case WB_OBJ_SECTOR: //扇形
					{
						PWBSectorItem pSectorItem = new WBSectorItem;
						CWBSector *pSector = (CWBSector *)pObj;
						pSector->GetItem(pSectorItem); 			
						pSectorItem->nWidth = g_nWBCurObjWidth;
						pSector->SetItem(pSectorItem);
						pSector->CreateSelecrtFrame();
						pSector->SetXmlValue(xmlObj);
						delete pSectorItem;
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->SetInvalidateway(true);
			CScreenDrawMgr::GetIns()->Invalidate();
			CScreenDrawMgr::GetIns()->SetInvalidateway(false);
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjIcoStyle()//jiangchao 2013.4.23
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;j ++)//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit())
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case WB_OBJ_ICOCURVE: //图标曲线
					{
						CWBIcoCurve * pIcoCurve = (CWBIcoCurve *)pObj;
						PWBIcoCurveItem pIcoCurveItem = pIcoCurve->GetItem();
						pIcoCurveItem->SetIcoFilePath(g_nWBCurObjIcoFilePath);
						pIcoCurve->SetItem(pIcoCurveItem);
						pIcoCurve->SetIcoImage();
						pIcoCurve->SetIsLoadPicFromFile(g_bIsWBLoadIcoFromFile);
						pIcoCurve->SetXmlValue(xmlObj);
						delete pIcoCurveItem;
					}
					break;
				default:
					break;
				}

				if(g_bIsWBLoadIcoFromFile)
				{
					xmlObj.Set(KEY_FILE_ID, m_dwIcoCurId);
					xmlObj.Set(KEY_EVENT_ID, m_ulIcoCurEvent);
				}
				else
				{
					xmlObj.Set(KEY_EVENT_ID, ulEventID);
				}
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjLineType()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case WB_OBJ_LINE: //直线
					{
						PWBLineItem pLineItem = new WBLineItem;
						CWBLine *pLine = (CWBLine *)pObj;
						pLine->GetItem(pLineItem); 
						pLineItem->nLineType = g_nWBLineType;
						pLine->SetItem(pLineItem);
						pLine->SetXmlValue(xmlObj);
						delete pLineItem;
					}
					break;
				case WB_OBJ_CURVE: //曲线
					{
						CWBCurve *pCurve = (CWBCurve *)pObj;
						PWBCurveItem pCurveItem = pCurve->GetItem();
						pCurveItem->SetType(g_nWBLineType);
						pCurve->SetItem(pCurveItem);
						pCurve->SetIsDraw();//重画曲线wangxiaobin
						pCurve->SetXmlValue(xmlObj);
						//CWBCurve::ClearCurveItem(pCurveItem);//by jiabangping 2011.7.17下面的语句已经完成了此功能
						delete pCurveItem;
					}
					break;
				case WB_OBJ_ARC: //直线
					{
						PWBArcItem pArcItem = new WBArcItem;
						CWBArc *pArc = (CWBArc *)pObj;
						pArc->GetItem(pArcItem); 
						pArcItem->nLineType = g_nWBLineType;
						pArc->SetItem(pArcItem);
						pArc->SetXmlValue(xmlObj);
						delete pArcItem;
					}
					break;

				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjSolidCheck()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			} 
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case WB_OBJ_POLYGON: //多边形
				case WB_OBJ_RECT:	//矩形
				case WB_OBJ_PALLELOGAM: //平行四边形
				case WB_OBJ_TRIANGLE: //三角形
				case  WB_OBJ_ARBITRARYTRIANGLE: //任意三角形
				case WB_OBJ_PENTAGON://五边形
				case WB_OBJ_HEXAGON://六边形
				case WB_OBJ_FIVEPOINTSTART:
				case WB_OBJ_TRAPEZIUM:
				case WB_OBJ_RHOMBUS: //菱形
					{
						PWBPolygonItem pPolygonItem = new WBPolygonItem;
						CWBPolygon *pPolygon = (CWBPolygon *)pObj;
						pPolygon->GetItem(pPolygonItem);
						if (g_bIsSolid)
						{
							if (pPolygon->IsSolid())
							{
								CWBPolygon::ClearItem(pPolygonItem);
								delete pPolygonItem;
								continue;
							} 
							else
							{
								pPolygonItem->bIsFillSolid = g_bIsSolid;
								pPolygonItem->clrFillSolid = g_clrSolid;
							}
						} 
						else
						{
							pPolygonItem->bIsFillSolid = g_bIsSolid;
							pPolygonItem->clrFillSolid = g_clrSolid;
						}
						pPolygon->SetItem(pPolygonItem);
						pPolygon->CreateSelecrtFrame();
						pPolygon->SetXmlValue(xmlObj);



						CWBPolygon::ClearItem(pPolygonItem);
						delete pPolygonItem;
					}
					break;
				case WB_OBJ_ELLIPSE: //椭圆
					{
						PWBEllipseItem pEllipseItem = new WBEllipseItem;
						CWBEllipse *pEllipse = (CWBEllipse *)pObj;
						pEllipse->GetItem(pEllipseItem); 			
						if (g_bIsSolid)
						{
							if (pEllipse->IsSolid())
							{
								delete pEllipseItem;
								continue;
							} 
							else
							{
								pEllipseItem->bIsFillSolid = g_bIsSolid;
								pEllipseItem->clrFillSolid = g_clrSolid;
							}
						} 
						else
						{
							pEllipseItem->bIsFillSolid = g_bIsSolid;
							pEllipseItem->clrFillSolid = g_clrSolid;
						}


						pEllipse->SetItem(pEllipseItem);
						pEllipse->CreateSelecrtFrame();
						pEllipse->SetXmlValue(xmlObj);
						delete pEllipseItem;
					}
					break;
				case WB_OBJ_SECTOR: //扇形
					{
						PWBSectorItem pSectorItem = new WBSectorItem;
						CWBSector *pSector = (CWBSector *)pObj;
						pSector->GetItem(pSectorItem); 			
						if (g_bIsSolid)
						{
							if (pSector->IsSolid())
							{
								delete pSectorItem;
								continue;
							} 
							else
							{
								pSectorItem->bIsFillSolid = g_bIsSolid;
								pSectorItem->clrFillSolid = g_clrSolid;
							}
						} 
						else
						{
							pSectorItem->bIsFillSolid = g_bIsSolid;
							pSectorItem->clrFillSolid = g_clrSolid;
						}


						pSector->SetItem(pSectorItem);
						pSector->CreateSelecrtFrame();
						pSector->SetXmlValue(xmlObj);
						delete pSectorItem;
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
			
		}
	}
}

void CWhiteBoardView::UpdateObjSolidColor()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			} 
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case WB_OBJ_POLYGON: //多边形
				case WB_OBJ_RECT:	//矩形
				case WB_OBJ_PALLELOGAM: //平行四边形
				case WB_OBJ_TRIANGLE: //三角形
				case WB_OBJ_ARBITRARYTRIANGLE: //任意三角形
				case WB_OBJ_PENTAGON://五边形
				case WB_OBJ_HEXAGON://六边形
				case WB_OBJ_FIVEPOINTSTART:
				case WB_OBJ_TRAPEZIUM:
				case WB_OBJ_RHOMBUS: //菱形
					{
						PWBPolygonItem pPolygonItem = new WBPolygonItem;
						CWBPolygon *pPolygon = (CWBPolygon *)pObj;
						pPolygon->GetItem(pPolygonItem);
						pPolygonItem->clrFillSolid = g_clrSolid;
						pPolygon->SetItem(pPolygonItem);
						pPolygon->CreateSelecrtFrame();
						pPolygon->SetFillColor(g_clrSolid);

						pPolygon->SetXmlValue(xmlObj);
						CWBPolygon::ClearItem(pPolygonItem);
						delete pPolygonItem;
					}
					break;
				case WB_OBJ_ELLIPSE: //椭圆
					{
						PWBEllipseItem pEllipseItem = new WBEllipseItem;
						CWBEllipse *pEllipse = (CWBEllipse *)pObj;
						pEllipse->GetItem(pEllipseItem); 			
						pEllipseItem->clrFillSolid =  g_clrSolid; 
						pEllipse->SetItem(pEllipseItem);
						pEllipse->CreateSelecrtFrame();
						pEllipse->SetFillColor(g_clrSolid);
						pEllipse->SetXmlValue(xmlObj);
						delete pEllipseItem;
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						BOOL bFlag = FALSE;

						if (NULL != m_pWBFunctionDlg)
						{
							if (NULL != m_pWBFunctionDlg->GetAttributeDlg())
							{
								bFlag = TRUE;
							}
						}
						else if (NULL != m_pWBFunctionDlg)
						{
							if (NULL != m_pWBFunctionDlg->GetAttributeDlg())
							{
								bFlag = TRUE;
							}
						}

						if (bFlag)
						{
							CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;

							CWBPolygon *pPolygon = pAnnotations->GetPolygon();
							PWBPolygonItem pPolygonItem = new WBPolygonItem;
							pPolygon->GetItem(pPolygonItem);
							pPolygonItem->clrFillSolid = g_clrSolid;
							pPolygon->SetItem(pPolygonItem);
							CWBPolygon::ClearItem(pPolygonItem);
							delete pPolygonItem;

							pAnnotations->SetXmlValue(xmlObj);
						}
					}
					break;
				case WB_OBJ_SECTOR: //扇形
					{
						PWBSectorItem pSectorItem = new WBSectorItem;
						CWBSector *pSector = (CWBSector *)pObj;
						pSector->GetItem(pSectorItem); 			
						pSectorItem->clrFillSolid =  g_clrSolid; 
						pSector->SetItem(pSectorItem);
						pSector->CreateSelecrtFrame();
						pSector->SetXmlValue(xmlObj);
						delete pSectorItem;
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjLineArrow()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case WB_OBJ_LINE: //直线
					{
						PWBLineItem pLineItem = new WBLineItem;
						CWBLine *pLine = (CWBLine *)pObj;
						pLine->GetItem(pLineItem); 
						pLineItem->nStartLineCap = g_nStartLineCap;
						pLineItem->nEndLineCap = g_nEndLineCap;
						pLine->SetItem(pLineItem);

						pLine->SetXmlValue(xmlObj);
						delete pLineItem;
					}
					break;
				case WB_OBJ_CURVE: //曲线
					{
						CWBCurve *pCurve = (CWBCurve *)pObj;
						PWBCurveItem pCurveItem = pCurve->GetItem();
						pCurveItem->SetStartLineCap(g_nStartLineCap);
						pCurveItem->SetEndLineCap(g_nEndLineCap);
						pCurve->SetItem(pCurveItem);
						pCurve->SetIsDraw();//重画曲线wangxiaobin
						pCurve->SetXmlValue(xmlObj);
						//CWBCurve::ClearCurveItem(pCurveItem);//by jiabangping 2011.7.17下面的语句已经完成了此功能
						delete pCurveItem;
					}
					break;
				case WB_OBJ_ARC: //圆弧
					{
						PWBArcItem pArcItem = new WBArcItem;
						CWBArc *pArc = (CWBArc *)pObj;
						pArc->GetItem(pArcItem); 
						pArcItem->nStartLineCap = g_nStartLineCap;
						pArcItem->nEndLineCap = g_nEndLineCap;
						pArc->SetItem(pArcItem);
						pArc->SetXmlValue(xmlObj);
						delete pArcItem;
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjFontFace()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case  WB_OBJ_TEXT://文字
					{
						PWBTextItem pTextItem = new WBTextItem;
						CWBText *pText = (CWBText *)pObj;
						pText->GetItem(pTextItem);
						_tcscpy_s(pTextItem->fontItem.lfFaceName, g_nWBCurObjFontFaceName);
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						pText->SetXmlValue(xmlObj);
						delete pTextItem;
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						CWBText *pText = pAnnotations->GetText();
						PWBTextItem pTextItem = new WBTextItem;
						pText->GetItem(pTextItem);
						_tcscpy_s(pTextItem->fontItem.lfFaceName, g_nWBCurObjFontFaceName);
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						delete pTextItem;
						pAnnotations->SetXmlValue(xmlObj);
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjFontSize()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case  WB_OBJ_TEXT://文字
					{
						PWBTextItem pTextItem = new WBTextItem;
						CWBText *pText = (CWBText *)pObj;
						pText->GetItem(pTextItem);
						pTextItem->fontItem.lfHeight = g_nWBCurObjFontSize;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						pText->SetXmlValue(xmlObj);
						delete pTextItem;
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						CWBText *pText = pAnnotations->GetText();
						PWBTextItem pTextItem = new WBTextItem;
						pText->GetItem(pTextItem);
						pTextItem->fontItem.lfHeight = g_nWBCurObjFontSize;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						delete pTextItem;
						pAnnotations->SetXmlValue(xmlObj);
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->SetInvalidateway(true);
			CScreenDrawMgr::GetIns()->Invalidate();
			CScreenDrawMgr::GetIns()->SetInvalidateway(false);
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjFontBold()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			} 
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case  WB_OBJ_TEXT://文字
					{
						PWBTextItem pTextItem = new WBTextItem;
						CWBText *pText = (CWBText *)pObj;
						pText->GetItem(pTextItem);
						if (g_isBold == TRUE)
						{
							pTextItem->fontItem.lfWeight = FW_BOLD;
						} 
						else
						{
							pTextItem->fontItem.lfWeight = FW_NORMAL;
						}
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						pText->SetXmlValue(xmlObj);
						delete pTextItem;
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						CWBText *pText = pAnnotations->GetText();
						PWBTextItem pTextItem = new WBTextItem;
						pText->GetItem(pTextItem);
						if (g_isBold == TRUE)
						{
							pTextItem->fontItem.lfWeight = FW_BOLD;
						} 
						else
						{
							pTextItem->fontItem.lfWeight = FW_NORMAL;
						}
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						delete pTextItem;
						pAnnotations->SetXmlValue(xmlObj);
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjFontItalic()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case  WB_OBJ_TEXT://文字
					{
						PWBTextItem pTextItem = new WBTextItem;
						CWBText *pText = (CWBText *)pObj;
						pText->GetItem(pTextItem);
						pTextItem->fontItem.lfItalic = g_isItalic;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						pText->SetXmlValue(xmlObj);
						delete pTextItem;
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						CWBText *pText = pAnnotations->GetText();
						PWBTextItem pTextItem = new WBTextItem;
						pText->GetItem(pTextItem);
						pTextItem->fontItem.lfItalic = g_isItalic;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						delete pTextItem;
						pAnnotations->SetXmlValue(xmlObj);
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjUnderline()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case  WB_OBJ_TEXT://文字
					{
						PWBTextItem pTextItem = new WBTextItem;
						CWBText *pText = (CWBText *)pObj;
						pText->GetItem(pTextItem);
						pTextItem->fontItem.lfUnderline = g_isUnderline;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						pText->SetXmlValue(xmlObj);
						delete pTextItem;
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						CWBText *pText = pAnnotations->GetText();
						PWBTextItem pTextItem = new WBTextItem;
						pText->GetItem(pTextItem);
						pTextItem->fontItem.lfUnderline = g_isUnderline;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						delete pTextItem;
						pAnnotations->SetXmlValue(xmlObj);
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}

void CWhiteBoardView::UpdateObjVertical()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj == NULL || pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case  WB_OBJ_TEXT://文字
					{
						CWBText *pText = (CWBText *)pObj;
						pText->SetIsVertical(g_bIsVertical);
						pText->SetCalRectText();
						pText->SetXmlValue(xmlObj);
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						CWBText *pText = pAnnotations->GetText();
						pText->SetIsVertical(g_bIsVertical);
						pText->SetCalRectText();
						pAnnotations->SetXmlValue(xmlObj);
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
	}
}


void CWhiteBoardView::UpdateObjTextStr()//caoyuqin 2011.8.18
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj == NULL || pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			else
			{
				if (g_cTextstr.IsEmpty() && g_pWhiteBoardView->GetIsKeyDel() == TRUE && (pObj->GetType() == WB_OBJ_ANNOTATIONS || pObj->GetType() == WB_OBJ_TEXT))
				{
					
					//g_pWhiteBoardView->SetIsKillFocus(FALSE);
					int nType = pObj->GetType();

					if (g_pWBPage->DeleteWBObject(g_pWBPage->GetCurEditingObjLayerIndex()))
					{

						CWBObject *pObj;
						int j = g_pWBPage->GetSelectObjNumber();
						POSITION pos;
						if (j == 1)//删除所选列表只剩一个对象时,此即为全局对象
						{
							for (int k = 0;k < j;++k )//遍历选中对象列表
							{
								pos = g_pWBPage->GetSelectObjLst()->FindIndex(k);
								if (pos == NULL)
								{
									continue;
								}
								g_pObj = (CWBObject *)g_pWBPage->GetSelectObjLst()->GetAt(pos);
							}
						}
						if (NULL != g_pWhiteBoardView->m_pWBFunctionDlg && ::IsWindow(g_pWhiteBoardView->m_pWBFunctionDlg->GetSafeHwnd()))//控制属性面板显示相应的属性
						{						
							if (j == 0)//选中对象列表中无对象时
							{
								if (!g_bAttriDlginitialize)
								{
									::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_NOSELECTWBOBJ,0,0);
								}					
							}
							else if (j == 1)//选中对象列表中只有一个对象时
							{
								::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,0,j);
							}
							else//选中对象列表中存在多个对象时
							{			
								for (int k = 0;k < j;++k )//遍历选中对象列表
								{
									CObList *ptempList = g_pWBPage->GetSelectObjLst();
									pos = ptempList->FindIndex(k);
									if (pos == NULL)
									{
										continue;
									}
									pObj = (CWBObject *)ptempList->GetAt(pos);
									int ntype = pObj->GetType();
									switch(ntype)
									{
									case WB_OBJ_ANNOTATIONS:
										{
											::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_ANNOTATIONS,j);//隐藏线宽,线形,箭头
										}
										break;
									case WB_OBJ_TEXT:
										{
											::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_TEXT,j);//隐藏线宽,线形,箭头
										}
										break;
									default:
										break;
									}	
								}
							}					
						}

						//发送同步消息
						RemoveObj(g_pWBPage->GetCurEditingObjLayerIndex());
						SetPageIsNeedDraw(TRUE);
						if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
						{
							CScreenDrawMgr::GetIns()->SetInvalidateway(true);
							CScreenDrawMgr::GetIns()->Invalidate();
							CScreenDrawMgr::GetIns()->SetInvalidateway(false);
						}
						else
						{
							g_nDrawStatus = DRAW_NORMAL;
							
							Invalidate();
						}
						UpdateWindow();
						g_nDrawStatus = DRAW_NORMAL; //选择状态  解决三角板 删除一个其他对象移不动问题
						SetPageIsNeedDraw(FALSE);
					}

					return;
				}
				if (g_cTextstr.IsEmpty()&&pObj->GetType() == WB_OBJ_ANNOTATIONS)
				{
					return;
				}
				//得到对象类型
				int nObjType = pObj->GetType();
				switch (nObjType)
				{
				case  WB_OBJ_TEXT://文字
					{
						PWBTextItem pTextItem = new WBTextItem;
						CWBText *pText = (CWBText *)pObj;
						pText->GetItem(pTextItem);
						pTextItem->strText = g_cTextstr;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();

						pText->SetXmlValue(xmlObj);
						delete pTextItem;
					}
					break;
				case WB_OBJ_ANNOTATIONS: //标注
					{
						CWBAnnotations *pAnnotations = (CWBAnnotations *)pObj;
						CWBText *pText = pAnnotations->GetText();
						PWBTextItem pTextItem = new WBTextItem;
						pText->GetItem(pTextItem);
						pTextItem->strText = g_cTextstr;
						pText->SetItem(pTextItem);
						pText->SetCalRectText();
						pAnnotations->SetXmlValue(xmlObj);
						delete pTextItem;
					}
					break;
				default:
					break;
				}
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}			
		}
		SetPageIsNeedDraw(TRUE);
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->SetInvalidateway(true);
			CScreenDrawMgr::GetIns()->Invalidate();
			CScreenDrawMgr::GetIns()->SetInvalidateway(false);
		}
		else
		{
			
			Invalidate();
			UpdateWindow();
			
		}
		SetPageIsNeedDraw(FALSE);
	}
}

void CWhiteBoardView::UpdateMTOperate()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	} 
	else
	{
		XMemIni xmlObj;
		unsigned long ulEventID = GenerateSSRC(); //事件ID	
		POSITION pos;
		CWBObject *pObj = NULL;
		for (int j = 0;j < iSelectObjCounts;++j )//遍历选中对象列表
		{
			pos = pSelectObjLst->FindIndex(j);
			if (pos == NULL)
			{
				continue;
			}
			pObj = pSelectObjLst->GetAt(pos);
			//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
			if (pObj == NULL || pObj->IsLock() || pObj->IsLimit()) //huanglongquan add
			{
				continue;
			}
			if (pObj->GetType() == WB_OBJ_DOCUMENT)
			{
				CWBDocument *pDoc = (CWBDocument *)pObj;
				pDoc->SetIsMTRotate(g_bIsMTRotate);
				pDoc->SetIsMTZoom(!g_bIsMTRotate);
				pDoc->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}
			if (pObj->GetType() == WB_OBJ_PICTURE)
			{
				CWBPicture *pPic = (CWBPicture *)pObj;
				pPic->SetIsMTRotate(g_bIsMTRotate);
				pPic->SetIsMTZoom(!g_bIsMTRotate);
				pPic->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}
		}
	}
}

void CWhiteBoardView::OpenAttributeDlg()//开启属性面板-曹裕钦11.4.27
{	
	if ((NULL == m_pWBFunctionDlg)||(!m_pWBFunctionDlg->IsWindowVisible()) || (m_pWBFunctionDlg->GetWindowIsHide()))
	{
		//g_pWhiteBoardView->addFunctionPanel();
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetFuntDlg();
		if (NULL ==m_pWBFunctionDlg)
		{
			return;
		}
		//解决：因点击对象弹出菜单中的属性菜单时，功能面板隐藏，而面板的上的显示的窗口也被功能面板一起隐藏，
		//窗口本身并没有调用函数隐藏自己。当下次功能面板show其他属性面板的时候会跟着show出来，
		//但是show其他属性函数中的隐藏上次显示窗口的的函数会不起作用。这样两个窗口就会重叠，而且当前
		//要显示的窗口时不起作用的。
		if (m_pWBFunctionDlg->GetPreviewDLG())
		{
			m_pWBFunctionDlg->GetPreviewDLG()->ShowWindow(SW_HIDE);
		}	
		 if (m_pWBFunctionDlg->GetBackGroundDlg())
		{
			m_pWBFunctionDlg->GetBackGroundDlg()->ShowWindow(SW_HIDE);
		}
		 if (m_pWBFunctionDlg->GetTeachtoolDlg())
		{
			m_pWBFunctionDlg->GetTeachtoolDlg()->ShowWindow(SW_HIDE);
		}
		 if (m_pWBFunctionDlg->GetResDlg())
		{
			m_pWBFunctionDlg->GetResDlg()->ShowWindow(SW_HIDE);
		}
		 if(((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg())
		 {
			 ((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->SetTimer(ID_TIMER_SHOW,10,NULL);
		 }
		
		if (::IsWindow(m_pWBFunctionDlg->GetSafeHwnd()))
		{	
			//::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_OPENARRIBUTEDLG,0,0);
			if (((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg())
			{
				((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->OnOptionBtnClicked(IDC_WB_ATTRIBUTEBTN);
			}
			
			//获取选中对象列表
			CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();
			if (NULL == pSelectObjLst)
			{
				return;
			}
			int selectObjCounts = pSelectObjLst->GetCount();
			//选中一个对象时:开启面板,并发送消息
			if (selectObjCounts == 1)
			{
				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,0,selectObjCounts);	
			}
			//选中多个对象时:开启面板,并发送消息	
			if (selectObjCounts > 1)
			{
				g_pWBPage->SelectMoreObjAttribute();
			}
		}			
	}
	//CWBFunctionAttributeDlg *ptempfunAttdlg = m_pWBFunctionDlg->GetAttributeDlg();
//	if ((NULL == ptempfunAttdlg)||(!ptempfunAttdlg->IsWindowVisible()))
//	{
		//((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->OnOptionBtnClicked(IDC_WB_ATTRIBUTEBTN);
		//::SendMessage(m_pWBFunctionDlg->m_hWnd,WM_OPENARRIBUTEDLG,0,0);
	//}
	//((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->SetTimer(ID_TIMER_SHOW,10,NULL);

}

//添加新视频（jiabangping 20110804）
BOOL CWhiteBoardView::AddMedia()	
{
	//弹出文件选择对话框（仅显示视频文件）
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
	wchar_t szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	BOOL bFlag = FALSE;

	//TCHAR lpszFilter[] = _T("视频 (*.avi;*.wmv;*.mpeg;*.mpg;*.asf;*.mov;*.dat;*.rmvb;*.vob;*.mp4;*.f4v;*.mp3;*.wm;*.wma;*.rm;*.mts;*.mkv)|*.avi;*.wmv;*.mpeg;*.mpg;*.asf;*.mov;*.dat;*.rmvb;*.vob;*.mp4;*.f4v;*.mp3;*.wm;*.wma;*.rm;*.mts;*.mkv||");
	TCHAR lpszFilter[] = _T("视频 (*.avi;*.wmv;*.mpeg;*.mpg)|*.avi;*.wmv;*.mpeg;*.mpg||");

	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,lpszFilter,NULL,0,FALSE);

	if(fileDlg.DoModal() == IDOK)
	{	
		if (!g_pWhiteBoardView)
		{
			return FALSE;
		}
		CString strFilePath = fileDlg.GetPathName();
		if (m_MedioVideoScreenDlg)
		{
			CString str = m_MedioVideoScreenDlg->GetCurrentFilePath();
			if (str.Compare(strFilePath) == 0)
			{
				CString strIsRecord;
				strIsRecord = ReadConfigText(_T("WhiteBoardMessageBoxText"),_T("IsRecord"));
				if (strIsRecord.IsEmpty())
				{
					strIsRecord = _T("该文件正在录制，不能打开！");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strIsRecord, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strIsRecord);
				}
				else
				{
					AfxMessageBox(strIsRecord,MB_OK);
				}
				return FALSE;
			}
		}
		if (!PathFileExists(strFilePath))//wangxiaobin 
		{
			CString strIsSave;
			CString strFilePathError;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
			if (strFilePathError.IsEmpty())
			{
				strFilePathError = _T("文件路径输入有误！");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strIsSave, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strIsSave);
			}
			else
			{
				AfxMessageBox(strFilePathError,MB_OK);
			}
			return FALSE;
		}
		XAutoLock lock(m_csAddObj);
		CWBMedia *pWBMedia = new CWBMedia(strFilePath);
		if (pWBMedia == NULL)
		{
			return FALSE;
		}
		g_pWBPage->AddWBObject(pWBMedia, WB_LAYER_NORMAL);


		//录像
		///////////////////////////////////////////////////////////////
		DWORD layerIndex = pWBMedia->GetLayerIndex();
		if (pWBMedia && pWBMedia->GetMediaWindow()
			&& pWBMedia->GetMediaWindow()->GetCapChanPlayHandleMedia())
		{
			pWBMedia->GetMediaWindow()->GetCapChanPlayHandleMedia()->SetMedia(layerIndex);
		}
		/////////////////////////////////////////////////////////////////
		
		m_MediaIDIsActive = layerIndex;
		/* m_MediaStringPath.clear();*/
		m_MediaStringPath.insert(pair<DWORD,CString>(layerIndex,strFilePath));

		Invalidate();//没有偶尔出现控制条不出现
		//发送同步消息
		XMemIni xmlObj;
		xmlObj.Set(KEY_MEDIA_ADDSESSION,g_ulSessionID);
		pWBMedia->SetXmlValue(xmlObj);
		AddObj(xmlObj);
		pWBMedia->SetMediaBeCreated(TRUE);
		bFlag = TRUE;
	}
	else
	{
		bFlag = FALSE;
		//return FALSE;
	}	
	g_nDrawStatus = DRAW_NORMAL;
	SetCurrentDirectory(szCurPath);
	return bFlag;
}

BOOL CWhiteBoardView::AddMedia(CString strFilePath)
{
	if (!PathFileExists(strFilePath))//wangxiaobin 
	{
		CString strFilePathError;
		strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
		if (strFilePathError.IsEmpty())
		{
			strFilePathError = _T("文件路径输入有误！");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strFilePathError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
		}
		else
		{
			AfxMessageBox(strFilePathError,MB_OK);
		}
		return FALSE;
	}
	if (m_MedioVideoScreenDlg)
	{
		CString str = m_MedioVideoScreenDlg->GetCurrentFilePath();
		if (str.Compare(strFilePath) ==0)
		{
			CString strIsRecord;
			strIsRecord = ReadConfigText(_T("WhiteBoardMessageBoxText"),_T("IsRecord"));
			if (strIsRecord.IsEmpty())
			{
				strIsRecord = _T("该文件正在录制，不能打开！");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{

				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strIsRecord, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strIsRecord);
			}
			return FALSE;
		}
	}
	//只能主动添加一个视频
	int nPage = g_pWBFile->GetPageCount();
	if (nPage >= 1)
	{
		for (int i =1 ; i <= nPage; ++i)
		{
			CWBPage *pWBPage = g_pWBFile->GetPage(i); 
			CObList* nMediaCameraList = pWBPage->GetMediaCameraList();
			if (nMediaCameraList->GetSize() > 0)
			{
				int nIndex;
				CWBObject * pObj = NULL;
				POSITION pos = NULL;
				for (nIndex = 0; nIndex < nMediaCameraList->GetSize(); ++nIndex)
				{
					pos = nMediaCameraList->FindIndex(nIndex);
					if (pos == NULL)
					{
						continue;
					}
					pObj = (CWBObject *)nMediaCameraList->GetAt(pos);

					if (pObj->GetType() == WB_OBJ_MEDIA &&((CWBMedia*)pObj)->IsActiveCreated())
					{
						CString strSupportOneMedia;
						strSupportOneMedia = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SupportOneMedia"));
						if (strSupportOneMedia.IsEmpty())
						{
							strSupportOneMedia = _T("目前支持主动添加一个视频");
						}
						if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
						{
							//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strSupportOneMedia, MB_OK,true);
							g_pHPWBDataNotify->ShowTipWindow(strSupportOneMedia);
						}
						else
						{
							AfxMessageBox(strSupportOneMedia,MB_OK);
						}
						g_nDrawStatus = DRAW_NORMAL;
						return FALSE;
					}
					else
					{
						continue;
					}
				}
			}
		}
	}

	XAutoLock lock(m_csAddObj);
	CWBMedia *pWBMedia = new CWBMedia(strFilePath);
	if (pWBMedia == NULL)
	{
		return FALSE;
	}
	g_pWBPage->AddWBObject(pWBMedia, WB_LAYER_NORMAL);


	//录像
	///////////////////////////////////////////////////////////////
	DWORD layerIndex = pWBMedia->GetLayerIndex();
	if (pWBMedia && pWBMedia->GetMediaWindow()
		&& pWBMedia->GetMediaWindow()->GetCapChanPlayHandleMedia())
	{
		pWBMedia->GetMediaWindow()->GetCapChanPlayHandleMedia()->SetMedia(layerIndex);
	}
	/////////////////////////////////////////////////////////////////
	
	m_MediaIDIsActive = layerIndex;

	//m_MediaStringPath.clear();
	m_MediaStringPath.insert(pair<DWORD,CString>(layerIndex,strFilePath));

	Invalidate();//没有偶尔出现控制条不出现
	//发送同步消息
	XMemIni xmlObj;
	xmlObj.Set(KEY_MEDIA_ADDSESSION,g_ulSessionID);
	pWBMedia->SetXmlValue(xmlObj);
	AddObj(xmlObj);

	pWBMedia->SetMediaBeCreated(TRUE);
	g_nDrawStatus = DRAW_NORMAL;

	return TRUE;
}

BOOL CWhiteBoardView::AddCamera()
{
	if (!m_pUniqueWBCamera)
	{
		CWBCamera *pWBCamera = new CWBCamera();
		m_nCameraObjId = g_pWBPage->AddWBObject(pWBCamera);
		DWORD layerIndex = pWBCamera->GetLayerIndex();
		if (pWBCamera && pWBCamera->GetCameraDlg()
			&& pWBCamera->GetCameraDlg()->GetCapChanPlayHandle_Empty())
		{
			pWBCamera->GetCameraDlg()->GetCapChanPlayHandle_Empty()->SetCaremaID(layerIndex);
		}
		//m_MediaIDIsActive = layerIndex;
		XMemIni xmlObj;
		xmlObj.Set(KEY_CAREMA_ADDSESSION,g_ulSessionID);
		pWBCamera->SetOwner(g_ulSessionID);
		pWBCamera->SetXmlValue(xmlObj);
		AddObj(xmlObj);
		//
		m_pUniqueWBCamera = pWBCamera;
	}
	else
	{
		CString strSupportOneCamera;
		strSupportOneCamera = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SupportOneCamera"));
		if (strSupportOneCamera.IsEmpty())
		{
			strSupportOneCamera = _T("目前支持主动添加一个摄像头");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strSupportOneCamera, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strSupportOneCamera);
		}
		else
		{
			AfxMessageBox(strSupportOneCamera,MB_OK);
		}
	}

	g_nDrawStatus = DRAW_NORMAL;
	return TRUE;
}
BOOL CWhiteBoardView::AddFlash()	
{
	//记录当前路径
	wchar_t szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	BOOL bFlag = FALSE;

	CLSID clsid;
	if ( !SUCCEEDED( CLSIDFromProgID( L"ShockwaveFlash.ShockwaveFlash", &clsid ) ))
	{
		CString strNotFlashPlug;
		strNotFlashPlug = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("sNotFlashPlug"));
		if (strNotFlashPlug.IsEmpty())
		{
			strNotFlashPlug = _T("未安装FLASH播放插件，无法正常打开，请安装插件");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strNotFlashPlug, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNotFlashPlug);
		}
		else
		{
			AfxMessageBox(strNotFlashPlug,MB_OK);
		}
		return FALSE;
	}

	TCHAR lpszFilter[] = _T("视频 (*.swf)|*.swf||");

	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,lpszFilter,NULL,0,FALSE);

	if(fileDlg.DoModal() == IDOK)
	{
		if (!g_pWhiteBoardView)
		{
			return FALSE;
		}
		CString strFilePath = fileDlg.GetPathName();

		if (!PathFileExists(strFilePath))//wangxiaobin 
		{
			CString strIsSave;
			CString strFilePathError;
			strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
			if (strIsSave.IsEmpty())
			{
				strIsSave = _T("是否需要保存此次的文件？");
			}
			strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
			if (strFilePathError.IsEmpty())
			{
				strFilePathError = _T("文件路径输入有误！");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strIsSave, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strIsSave);
			}
			else
			{
				AfxMessageBox(strFilePathError,MB_OK);
			}
			return FALSE;
		}

		CWBFlash *pWBFlash = new CWBFlash(strFilePath);

		g_pWBPage->AddWBObject(pWBFlash, WB_LAYER_NORMAL);


		pWBFlash->init();

		Invalidate();

		//发送同步消息
		XMemIni xmlObj;
		pWBFlash->SetXmlValue(xmlObj);
		AddObj(xmlObj);

		bFlag = TRUE;
		//return TRUE;
	}
	else
	{
		bFlag = FALSE;
	}
	g_nDrawStatus = DRAW_NORMAL;
	SetCurrentDirectory(szCurPath);
	return bFlag;
}

BOOL CWhiteBoardView::AddFlash(CString strFilePath,CRect rect /*= CRect(0,0,0,0)*/)
{
	CLSID clsid;
	if ( !SUCCEEDED( CLSIDFromProgID( L"ShockwaveFlash.ShockwaveFlash", &clsid ) ))
	{
		CString strNotFlashPlug;
		strNotFlashPlug = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("sNotFlashPlug"));
		if (strNotFlashPlug.IsEmpty())
		{
			strNotFlashPlug = _T("未安装FLASH播放插件，无法正常打开，请安装插件");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strNotFlashPlug, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strNotFlashPlug);
		}
		else
		{
			AfxMessageBox(strNotFlashPlug,MB_OK);
		}
		return FALSE;
	}

	if (!PathFileExists(strFilePath))//wangxiaobin 
	{
		CString strFilePathError;
		strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
		if (strFilePathError.IsEmpty())
		{
			strFilePathError = _T("文件路径输入有误！");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strFilePathError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
		}
		else
		{
			AfxMessageBox(strFilePathError,MB_OK);
		}
		return FALSE;
	}

	CWBFlash *pWBFlash = new CWBFlash(strFilePath,rect);

	g_pWBPage->AddWBObject(pWBFlash, WB_LAYER_NORMAL);


	pWBFlash->init();

	Invalidate();

	//发送同步消息
	XMemIni xmlObj;
	pWBFlash->SetXmlValue(xmlObj);
	AddObj(xmlObj);

	g_nDrawStatus = DRAW_NORMAL;
	return TRUE;
}

BOOL CWhiteBoardView::AddDxf(CString strFilePath,CRect rect /*= CRect(0,0,0,0)*/) //add by jiangchao 2013.08.22
{
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
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strFilePathError, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFilePathError);
		}
		else
		{
			AfxMessageBox(strFilePathError,MB_OK);
		}
		return FALSE;
	}

	CWBDxf *pWBDxf = new CWBDxf(strFilePath);
	if(pWBDxf == NULL)
	{
		return FALSE;
	}
	pWBDxf->SetAlpha((int)g_nTranspar); //设置透明度
	g_pWBPage->AddWBObject(pWBDxf, WB_LAYER_NORMAL);
	
	pWBDxf->SetIsNeedReDraw(); //判断是否重画DXF

	Invalidate();

	XMemIni xmlObj;
	pWBDxf->SetXmlValue(xmlObj);
	AddObj(xmlObj);

	g_nDrawStatus = DRAW_NORMAL;
	return TRUE;
}

BOOL CWhiteBoardView::ScreenShoot(CString strFilePath)
{

	if (!PathFileExists(strFilePath))
	{
		// 		CString strIsSave;
		// 		strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSave"));
		// 		if (strIsSave.IsEmpty())
		// 		{
		// 			strIsSave = _T("是否需要保存此次的文件");
		// 		}
		CString strFilePathError;
		strFilePathError = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FilePathError"));
		if (strFilePathError.IsEmpty())
		{
			strFilePathError = _T("文件路径输入有误");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strIsSave, MB_OK,true);
		}
		else
		{
			AfxMessageBox(strFilePathError,MB_OK);
		}
		return FALSE;
	}
	g_pWBPage->ClearSelectFrameList();

	//创建一个新对象
	CWBPicture *pWBPicture = new CWBPicture(strFilePath/*m_pResource->m_strFileName*/);			
	pWBPicture->SetAlpha((int)g_nTranspar);
	//控制图片的最大不能超出view的大小
	CRect viewRect,viewRectInScreen;
	GetClientRect(&viewRect);
	GetWindowRect(&viewRectInScreen);//得到白板在屏幕上的位置
	//pWBPicture->SetPictureBigerSzie(viewRect);
	g_pWBPage->AddWBObject(pWBPicture);
	CPoint point,ptCenter;
	GetCursorPos(&point);//鼠标在屏幕中的坐标
	point.x -= viewRectInScreen.left;//鼠标在屏幕中的位置转化为在白板中的坐标
	point.y -= viewRectInScreen.top;//鼠标在屏幕中的位置转化为在白板中的坐标
	CClientDC dc(this);
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&point);//将视图坐标转换为文档作标
	//获得对象中心点
	pWBPicture->GetCenterPoint(ptCenter);
	CPoint pt(viewRect.Width()/2+getXnPos(),viewRect.Height()/2+getYnPos() -20);
	pWBPicture->MoveOffset(pt - ptCenter);
	m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
	//创建选择框，并移动对象到当前位置
	if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
	{
		m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
	}

	Invalidate();
	g_nDrawStatus = DRAW_NORMAL;

	//发送同步消息
	XMemIni xmlObj;
	pWBPicture->SetXmlValue(xmlObj);
	AddObj(xmlObj);

	return TRUE;
}

//打开资源库
BOOL CWhiteBoardView::OpenResource()
{		
	//判断资源库对话框是否存在
	BOOL bIsResCreted = WBRESWND::IsGalleryWndCreated();
	if(!bIsResCreted)
	{
		::SendMessage(g_whiteboardDlg->GetSafeHwnd(), UWM_CREATE_RESOURCE_DLG, 0, 0);
	}
	else
	{
		WBRESWND::ShowGalleryWndEx();
	}
	return TRUE;
	////
	//if(m_pResource == NULL)
	//{
	//	char strKeyData[MAX_PATH];
	//	CString strResourceDir;

	//	strResourceDir = GetCurrentPath();	//获取当前程序执行的目录	
	//	strResourceDir	+= "\\资源库";	

	//	::GetPrivateProfileString("资源库", "资源库", "", strKeyData, 256, ".\\WBResConfig.ini");

	//	//判断键值是否存在，如果找不到键值，则重新填写
	//	if((CString)strKeyData == "")
	//	{
	//		//	AfxMessageBox("资源库配置文件丢失，无法读取用户自定义的目录！");

	//		CString strConfigIni = ".\\WBResConfig.ini";				

	//		//填写基本的资源库文件夹名
	//		WritePrivateProfileString("资源库", "资源库", "*" + strResourceDir, strConfigIni);
	//		WritePrivateProfileString("资源库", "截图", "*" + g_strTempPath + "截图", strConfigIni);			
	//	}

	//	//判断资源库文件夹是否存在，没有则创建
	//	if(!PathIsDirectory(strResourceDir))	
	//	{		
	//		if(!CreateDirectory(strResourceDir, NULL))
	//		{
	//			AfxMessageBox("资源库文件夹创建失败");
	//		}		
	//	}


	//	//增加GDI+支持
	//	//GdiplusStartupInput gdiplusStartupInput;
	//	//GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	//	//新建资源库对话框
	//	m_pResource = new CWBResourceDlg();
	//	if (!m_pResource->Create(IDD_WBRESOURCEDLG, this))
	//	{
	//		return FALSE;
	//	}	

	//	//将资源库对话框置底居中
	//	CRect rcWnd, rcDlg;
	//	GetClientRect(&rcWnd);
	//	m_pResource->GetClientRect(&rcDlg);
	//	rcDlg.left = (rcWnd.right - rcDlg.right) / 2;
	//	rcDlg.top = (rcWnd.bottom - rcDlg.bottom);	
	//	rcDlg.bottom = rcWnd.bottom;
	//	rcDlg.right += rcDlg.left;
	//	/*	rcDlg.left = (rcWnd.right - rcDlg.right) / 2;
	//	rcDlg.top = 0;
	//	rcDlg.right += rcDlg.left;*/
	//	ClientToScreen(&rcDlg);
	//	m_pResource->MoveWindow(rcDlg);
	//	m_pResource->ShowWindow(SW_SHOW);
	//}	
	//else
	//{
	//	if (!m_pResource->IsWindowVisible())
	//	{
	//		m_pResource->ShowWindow(SW_SHOW);
	//	}
	//	else
	//	{
	//		if (!m_pResource->DestroyWindow())
	//		{
	//			return FALSE;
	//		}		
	//		delete m_pResource;
	//		m_pResource = NULL;
	//	}
	//}
	//return TRUE;
}

void CWhiteBoardView::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case ID_TIEMR_REFRESH:
		{
			Invalidate();
			UpdateWindow();
			KillTimer(ID_TIEMR_REFRESH);
		}
	case ID_TIEMR_STARTGIF://gif
		{
			if (g_pWBPage->m_lOnTimer >= 360000000)
			{
				g_pWBPage->m_lOnTimer = 0;
			}
			g_pWBPage->m_lOnTimer += 10; 
			CWBObject *pObj;
			CObList   *pList;
			POSITION  pos;

			pList = g_pWBPage->GetWBObjList();
			for (int i = 0; i < pList->GetSize(); i++)
			{
				pos = pList->FindIndex(i);
				if (pos == NULL)
				{
					continue;
				}
				pObj = (CWBObject *)pList->GetAt(pos);

				if (pObj == NULL)
				{
					continue;
				}
				if ((pObj->GetType() == WB_OBJ_PICTURE)
					&& (((CWBPicture*)pObj)->GetImage())
					&& (((CWBPicture*)pObj)->GetImage()->IsAnimatedGIF()))
				{
					if (((CWBPicture*)pObj)->GetImage()->lPause == 0)
					{
						(((CWBPicture*)pObj)->GetImage())->DrawFrameGIF();
					}
					else
					{
						if (g_pWBPage->m_lOnTimer % (((CWBPicture*)pObj)->GetImage()->lPause) == 0)
						{
							(((CWBPicture*)pObj)->GetImage())->DrawFrameGIF();
						}
					}
				}	
			}
			//刷新组合中的GIF图片
			std::vector<CWBObject *>::iterator it = g_pWBPage->m_pVecCombineGif.begin();
			for (;it != g_pWBPage->m_pVecCombineGif.end();++it)
			{
				pObj = (*it);
				if (pObj == NULL)
				{
					continue;
				}
				if ((pObj->GetType() == WB_OBJ_PICTURE)
					&& (((CWBPicture*)pObj)->GetImage())
					&& (((CWBPicture*)pObj)->GetImage()->IsAnimatedGIF()))
				{
					if (((CWBPicture*)pObj)->GetImage()->lPause == 0)
					{
						(((CWBPicture*)pObj)->GetImage())->DrawFrameGIF();
					}
					else
					{
						if (g_pWBPage->m_lOnTimer % (((CWBPicture*)pObj)->GetImage()->lPause) == 0)
						{
							(((CWBPicture*)pObj)->GetImage())->DrawFrameGIF();
						}
					}
				}	
			}
		}
		break;
	case ID_TIMER_FLASH:
		Invalidate();
		break;
	case ROLLCALLSENDTIMER:
		{
			m_nRollCallTimer--;
			if (m_nRollCallTimer == 0)
			{
				KillTimer(ROLLCALLSENDTIMER);

				if (!m_wbrcshowdlg)
				{

					CRect viewRectInScreen;
					GetWindowRect(&viewRectInScreen);
					m_wbrcshowdlg = new CWBRollCallShowDlg(viewRectInScreen.left,viewRectInScreen.top);
					m_wbrcshowdlg->Create(CWBRollCallShowDlg::IDD,g_pWhiteBoardView);
				}
				else
				{
					CRect viewRectInScreen;
					GetWindowRect(&viewRectInScreen);
					m_wbrcshowdlg->ShowagainDlg(viewRectInScreen.left,viewRectInScreen.top);
				}
			}
		}
		break;
	case ID_TIEMR_MAGICPEN:
		{
			g_pWBPage->ChangeMagicAlpha();
		}
		break;
	default:
		break;
	}
	if (CHECKMEMORYSTATUS == nIDEvent)
	{
		MEMORYSTATUS MemStat;
		MemStat.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&MemStat);
		int  ulUsges = MemStat.dwMemoryLoad;
		if (ulUsges > 85)
		{
			if (g_bSupportU7OpenFile)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,_T("提示"),_T("内存不足！！！"), MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(_T("内存不足！！！"));
			}
			else
			{
				AfxMessageBox(_T("内存不足！！！"),MB_OK);
			}
		}
	}
	CScrollView::OnTimer(nIDEvent);
}

//denghong
BOOL CWhiteBoardView::EraseLineMsg()
{
	int i;
	DWORD dwLayerIndex;
	CWBObject *pObj;
	XMemIni xmlObj;

	//删除擦除之前的直线对象
	vector<DWORD*>::iterator iter;
	for( iter = m_vecLineLayerIndexBeforeErase.begin(); iter != m_vecLineLayerIndexBeforeErase.end(); ++iter )
	{
		if( *iter == NULL ) continue;
		dwLayerIndex = **iter; 
		pObj = g_pWBPage->FindByLayerIndex(dwLayerIndex);
		//如果对象还存在，则不删除对象
		if (pObj != NULL) 
			continue;
		//发送同步消息，删除对象
		RemoveObj(dwLayerIndex);
	}

	int nIndex;
	POSITION pos;
	CWBObject *pObject = NULL;
	CObList *pWBList = NULL;
	pWBList = g_pWBPage->GetWBObjList();
	if (pWBList == NULL)
	{
		return FALSE;
	}
	if (!g_bIsDrawScreen) //如果屏幕绘图没有打开
	{
		//删除组合里的直线对象
		for (nIndex = 0; nIndex < pWBList->GetCount(); ++nIndex )
		{
			pos = pWBList->FindIndex(nIndex);
			if (pos == NULL)
			{
				continue;
			}
			pObject = (CWBObject *)pWBList->GetAt(pos);

			if (pObject == NULL)
			{
				continue;;
			}

			if (pObject->GetType() == WB_OBJ_COMBINE)
			{
				((CWBCombine *)pObject)->DelComLineMsg();
			}
		}
	}

	//得到当前的所有直线对象图层号
	std::vector<DWORD *> vecLine;
	g_pWBPage->GetAllObjLayerIndex(vecLine, WB_OBJ_LINE);

	for (i = 0; i < (int)vecLine.size(); ++i )
	{
		if (vecLine[i] == NULL)
		{
			continue;
		}
		dwLayerIndex = *(vecLine[i]);
		pObj = g_pWBPage->FindByLayerIndex(dwLayerIndex);

		if (pObj == NULL)
		{
			continue;
		}

		pObj->SetXmlValue(xmlObj);
		xmlObj.Set(KEY_EVENT_ID, m_ulSendEventID);
		AddObj(xmlObj);

		delete vecLine[i];
	}
	vecLine.clear();

	if (!g_bIsDrawScreen) //如果屏幕绘图没有打开
	{
		//添加组合里的直线对象
		for (nIndex = 0; nIndex < pWBList->GetCount(); ++nIndex )
		{
			pos = pWBList->FindIndex(nIndex);
			if (pos == NULL)
			{
				continue;
			}
			pObject = (CWBObject *)pWBList->GetAt(pos);

			if (pObject == NULL)
			{
				continue;
			}

			if (pObject->GetType() == WB_OBJ_COMBINE)
			{
				((CWBCombine *)pObject)->AddComLineMsg(m_ulSendEventID);

				//发送同步消息，改变组合中的对象图层列表
				XMemIni xmlComObj;
				xmlComObj.Set(KEY_EVENT_ID, m_ulSendEventID);
				pObject->SetXmlValue(xmlComObj);
				ChangeObj(xmlComObj);
			}
		}
	}

	return TRUE;
}


BOOL CWhiteBoardView::EraseCurveMsg()
{
	int nIndex;
	DWORD dwLen = 0;
	POSITION pos;
	CWBObject *pWBObj;
	CObList *ptemlist = g_pWBPage->GetEraseCurveList();
	if (NULL == ptemlist)
	{
		return FALSE;
	}
	int nCount = ptemlist->GetCount();
	if ( 0 == nCount)
	{
		return FALSE;
	}
	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{
		pos = ptemlist->FindIndex(nIndex);
		if ( pos == NULL ) continue;
		pWBObj = (CWBObject *)ptemlist->GetAt(pos);
		if (pWBObj == NULL) continue;

		if (WB_OBJ_CURVE == pWBObj->GetType())
		{
			CWBCurve *pCurve =(CWBCurve *)pWBObj;
			pCurve->SetIsDraw();//判断是否重画wangxiaobin
			XMemIni xmlObj;
			pCurve->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_EVENT_ID, m_ulSendEventID);
			ChangeObj(xmlObj);
		}

		//add by jiangchao 2013.5.15
		if(WB_OBJ_ICOCURVE == pWBObj->GetType())
		{
			CWBIcoCurve * pIcoCurve = (CWBIcoCurve *)pWBObj;
			pIcoCurve->SetIsRefush(TRUE);
			XMemIni xmlObj;
			pIcoCurve->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_EVENT_ID, m_ulSendEventID);
			ChangeObj(xmlObj);
		}
	}
	g_pWBPage->DeleteEraseCurveList();
	return TRUE;
}

BOOL CWhiteBoardView::EraseDocMsg()
{
	int nIndex;
	DWORD dwLen = 0;
	POSITION pos;
	CWBObject *pWBObj;
	CObList *ptemList = g_pWBPage->GetEraseDocList();
	if (NULL == ptemList)
	{
		return FALSE;
	}
	int nCount = ptemList->GetCount();
	if ( 0 == nCount)
	{
		return FALSE;
	}
	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{
		pos = ptemList->FindIndex(nIndex);
		if (pos == NULL)
		{
			continue;
		}
		pWBObj = (CWBObject *)ptemList->GetAt(pos);
		if (pWBObj == NULL)
		{
			continue;
		}

		XMemIni xmlObj;
		pWBObj->SetXmlValue(xmlObj);
		xmlObj.Set(KEY_EVENT_ID, m_ulSendEventID);
		ChangeObj(xmlObj);
	}
	g_pWBPage->DeleteEraseDocList();
	return TRUE;
}


BOOL CWhiteBoardView::DeleteObjMsg()
{
	POSITION pos;
	CWBObject *pWBObj;
	CObList *pDelList = g_pWBPage->GetWBDeleteObjList();
	if (NULL ==pDelList)
	{
		return FALSE;
	}

	//设置发送事件ID
	m_ulSendEventID = GenerateSSRC();

	int nCount = pDelList->GetCount();
	for (int nIndex = 0; nIndex < nCount; ++nIndex )
	{
		pos = pDelList->FindIndex(nIndex);
		if (pos == NULL)
		{
			continue;
		}
		pWBObj = (CWBObject *)pDelList->GetAt(pos);
		if (pWBObj == NULL)
		{
			continue;
		}
		//注释原因：修改BUG#27729，删除组合对象不将组合列表中的对象发送
		//if (pWBObj->GetType() == WB_OBJ_COMBINE)
		//{
		//	((CWBCombine *)pWBObj)->DelComMebMsg();
		//}
		//触摸删除正在操作的对象
		CWBMTObjectManager::GetIns_()->RemoveMTObject(pWBObj->GetLayerIndex());
		//发送同步消息
		//RemoveObj(pWBObj->GetLayerIndex());
		RemoveDelCombineObj(pWBObj->GetLayerIndex());
	}

	//在同步时让接收方及时刷新，额外发送一个消息
	if (nCount > 1)
	{
		SendChangePage();
	}

	g_pWBPage->DeleteWBDeleteObjList();
	return TRUE;
}

BOOL CWhiteBoardView::MoveMsg(CPoint moveOffset)
{
	int nIndex;
	DWORD dwLen = 0;
	CWBObject *pObj = NULL;
	std::vector<DWORD> vecSelectFrameList;

	vecSelectFrameList = g_pWBPage->GetWBSelectFrameList();
	int nCount = vecSelectFrameList.size();

	unsigned long ulEventID = GenerateSSRC(); //事件ID

	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{

		//查找选择的对象
		pObj = g_pWBPage->FindByLayerIndex(vecSelectFrameList[nIndex]);
		if (pObj == NULL)
		{
			continue;
		}

		XMemIni xmlObj;
		pObj->SetXmlValue(xmlObj);
		xmlObj.Set(KEY_EVENT_OBJNUMBER, nCount - nIndex);
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
		xmlObj.Set(KEY_BROADCAST_CMD,CMD_TOUCH_MOVE_OBJ);
		//ChangeObj(xmlObj);
		Broadcast_Data(xmlObj);
	}
	return TRUE;
}

BOOL CWhiteBoardView::MoveMsg( CPoint moveOffset,unsigned long ulEventID )
{
	int nIndex;
	DWORD dwLen = 0;
	CWBObject *pObj = NULL;
	std::vector<DWORD> vecSelectFrameList;

	vecSelectFrameList = g_pWBPage->GetWBSelectFrameList();
	int nCount = vecSelectFrameList.size();

	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{
		pObj = g_pWBPage->FindByLayerIndex(vecSelectFrameList[nIndex]);
		if (pObj == NULL)
		{
			continue;
		}
		XMemIni xmlObj;
		pObj->SetXmlValue(xmlObj);
		xmlObj.Set(KEY_EVENT_OBJNUMBER, nCount - nIndex);
		xmlObj.Set(KEY_MOVE_OFFSET,"");
		xmlObj.Set(KEY_EVENT_ID, ulEventID);

		int nType = 0;
		xmlObj.Get(KEY_OBJ_TYPE, nType);
		WB_OBJ_TYPE objType  = (WB_OBJ_TYPE)nType;
		if (objType == WB_OBJ_COMBINE)
		{
			ChangeObj(xmlObj,true);
		}
		else
		{
			ChangeObj(xmlObj);
		}
		
	}
	return TRUE;
}

BOOL CWhiteBoardView::MoveMsg()
{
	int nIndex;
	DWORD dwLen = 0;
	CWBObject *pObj = NULL;
	std::vector<DWORD> vecSelectFrameList;

	vecSelectFrameList = g_pWBPage->GetWBSelectFrameList();
	int nCount = vecSelectFrameList.size();

	unsigned long ulEventID = GenerateSSRC(); //事件ID

	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{
		//查找选择的对象
		pObj = g_pWBPage->FindByLayerIndex(vecSelectFrameList[nIndex]);
		if (pObj == NULL)
		{
			continue;
		}
		XMemIni xmlObj;
		pObj->SetXmlValue(xmlObj);
		xmlObj.Set(KEY_EVENT_OBJNUMBER, nCount - nIndex);
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
		ChangeObj(xmlObj);
	}
	return TRUE;
}

LRESULT CWhiteBoardView::OnTrackPopupMenu(WPARAM wParam, LPARAM lParam)
{
	CPoint pt(LOWORD(lParam), HIWORD(lParam));
	CWBObject *pObj = NULL;
	CMTObject *pMTObject = (CMTObject*)(wParam);
	if (pMTObject)
	{
		pObj = dynamic_cast<CWBObject*>(pMTObject);
	}
	if (pObj != NULL)//先不用关心是否不存在（如网络同步）
	{
		CWBImageButton *ptemImgBtn = pObj->GetImageButton();
		if (ptemImgBtn)
		{
			ptemImgBtn->ButtonUp(pt);
		}
		else
		{
			OutputDebugString(L"pObj->GetImageButton() is Null \n");
		}

	}
	g_nDrawStatus = DRAW_NORMAL;

	if (CScreenDrawMgr::GetIns())
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		g_pWhiteBoardView->Invalidate();
	}

	return 0;
}

void CWhiteBoardView::RecieveMsg(char* tempBuff, int nRead)
{

}

void CWhiteBoardView::DeleteEdit()
{
	if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
	{
		SendMessage(UWM_DESTROY_RICHEDIT, 0, 0);
	}
}

void CWhiteBoardView::DestroyRichWindow()
{   
	//当进行了文本编辑但没有写，判断m_pEdit是否为空，不为空则删除
	CWBRichEdit* ptemEdit = CWBRichEditMgr::GetInst_()->GetRichEdit();
	if (NULL != ptemEdit)
	{
		m_pageIsNeedDraw = TRUE;
		m_RcDrawText = CRect(0,0,0,0);
		CString str;
		ptemEdit->GetWindowText(str);
		LOGFONT lfCurFont = ptemEdit->GetCurFont();
		COLORREF clrCurTextColor = ptemEdit->GetFontColor();

		if (m_bIsEditing)
		{
			if (str.GetLength() > 0) //如果文本中有字符
			{
				PWBTextItem pTextItem = new WBTextItem;
				if (m_bIsAnnotations)
				{
					CWBAnnotations *pAnnotations = (CWBAnnotations *)m_pEditObj;
					m_pEditObj = NULL;
					CWBText *pText = pAnnotations->GetText();
					pText->GetItem(pTextItem);
					pTextItem->strText = str;

					if (NULL != m_pWBFunctionDlg)
					{
						pAnnotations->SetAlpha((int)g_nTranspar);
						_tcscpy_s(pTextItem->fontItem.lfFaceName, g_nWBCurObjFontFaceName);
						pTextItem->fontItem.lfHeight = g_nWBCurObjFontSize;
						pTextItem->fontItem.lfItalic = g_isItalic;
						pTextItem->fontItem.lfUnderline = g_isUnderline;						
						if (g_isBold == TRUE)
						{
							pTextItem->fontItem.lfWeight = FW_BOLD;
						} 
						else
						{
							pTextItem->fontItem.lfWeight = FW_NORMAL;
						}
						pText->SetIsVertical(g_bIsVertical);
					}
					
					//pTextItem->fontItem = lfCurFont;
					pTextItem->clrText = g_clrWBCurObjColor;
					pText->SetItem(pTextItem);
					pText->SetCalRectText();

					DWORD dwLayerIndex = pAnnotations->GetLayerIndex();
					m_dwCurObjLayerIndex = dwLayerIndex;
					if (g_bIsDrawScreen)
					{
						g_pWBPage->AddWBObject(pAnnotations, WB_LAYER_LABEL);
					} 
					else
					{
						g_pWBPage->AddWBObject(pAnnotations);
					}
					pAnnotations->SetLayerIndex(dwLayerIndex);

					//发送同步消息
					XMemIni xmlObj;
					pAnnotations->SetXmlValue(xmlObj);
					ChangeObj(xmlObj);
				} 
				else
				{
					CWBText *pText = (CWBText *)m_pEditObj;
					m_pEditObj = NULL;
					pText->GetItem(pTextItem);
					pTextItem->strText = str;

					if (NULL != m_pWBFunctionDlg)
					{
						pText->SetAlpha((int)g_nTranspar);
						_tcscpy_s(pTextItem->fontItem.lfFaceName, g_nWBCurObjFontFaceName);
						pTextItem->fontItem.lfHeight = g_nWBCurObjFontSize;
						pTextItem->fontItem.lfItalic = g_isItalic;
						pTextItem->fontItem.lfUnderline = g_isUnderline;
						pText->SetIsVertical(g_bIsVertical);
						if (g_isBold == TRUE)
						{
							pTextItem->fontItem.lfWeight = FW_BOLD;
						} 
						else
						{
							pTextItem->fontItem.lfWeight = FW_NORMAL;
						}
					}
					
					//pTextItem->fontItem = lfCurFont;
					pTextItem->clrText = g_clrWBCurObjColor;
					pText->SetItem(pTextItem);
					pText->SetCalRectText();

					DWORD dwLayerIndex = pText->GetLayerIndex();
					m_dwCurObjLayerIndex = dwLayerIndex;
					if (g_bIsDrawScreen)
					{
						g_pWBPage->AddWBObject(pText, WB_LAYER_LABEL);
					} 
					else
					{
						g_pWBPage->AddWBObject(pText);
					}
					pText->SetLayerIndex(dwLayerIndex);

					//发送同步消息
					XMemIni xmlObj;
					pText->SetXmlValue(xmlObj);
					ChangeObj(xmlObj);
				}
				delete pTextItem;
			}
			else
			{
				DWORD dwLayerIndex = m_pEditObj->GetLayerIndex();
				delete m_pEditObj;
				m_pEditObj = NULL;

				//发送同步消息						
				RemoveObj(dwLayerIndex);
			}

			m_bIsEditing = FALSE;
		} 
		else
		{
			if (str.GetLength() > 0) //如果文本中有字符
			{
				_tcscpy_s(lfCurFont.lfFaceName, g_nWBCurObjFontFaceName);
				lfCurFont.lfHeight = g_nWBCurObjFontSize;
				lfCurFont.lfItalic = g_isItalic;
				lfCurFont.lfUnderline = g_isUnderline;
				if (m_bIsAnnotations)
				{
					
					CWBAnnotations *pAnnotations = new CWBAnnotations(m_topLeft,lfCurFont,clrCurTextColor,str);
					CWBText *pText = pAnnotations->GetText();
					pText->SetIsVertical(g_bIsVertical);
					pAnnotations->SetAlpha((int)g_nTranspar);
					if (g_bIsDrawScreen)
					{
						m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pAnnotations, WB_LAYER_LABEL);
					} 
					else
					{
						m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pAnnotations);
					}
					Invalidate();
					UpdateWindow();

					unsigned long ulEventID = GenerateSSRC(); //事件ID
					if (!AddComponentToObj(pAnnotations, ulEventID))
					{
						//添加同步信息
						XMemIni xmlObj;
						pAnnotations->SetXmlValue(xmlObj);
						AddObj(xmlObj);
					}
				} 
				else
				{   
					CWBText *pText = new CWBText(m_topLeft,lfCurFont,clrCurTextColor,str);
					pText->SetIsVertical(g_bIsVertical);
					pText->SetAlpha((int)g_nTranspar);
					if (g_bIsDrawScreen)
					{
						m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pText, WB_LAYER_LABEL);
					} 
					else
					{
						m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pText);
					}

					//添加同步信息
					XMemIni xmlObj;
					pText->SetXmlValue(xmlObj);
					AddObj(xmlObj);
				}
			}
		}

		if (g_pWhiteBoardView->m_hWnd == NULL)
		{
			return;
		}

		//wangxiaobin屏蔽的。吧刷新放到UP中。这里刷新有问题
		if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
		{
			CScreenDrawMgr::GetIns()->Invalidate();
			CScreenDrawMgr::GetIns()->UpdateWindow();
		}
		else
		{
			Invalidate();
			UpdateWindow();
		}
	}
	
	if (NULL != m_pWBFunctionDlg && NULL != m_pWBFunctionDlg->GetAttributeDlg())
	{
		CWBFunctionAttributeDlg* ptemfunattdlg = m_pWBFunctionDlg->GetAttributeDlg();
		if (_tcscmp(ptemfunattdlg->m_strFontSize, L"") == 0)
		{
			ptemfunattdlg->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
			ptemfunattdlg->UpdateData(FALSE);
		}
		ptemfunattdlg = NULL;
	}
	ptemEdit = NULL;
}


//快捷键12-25添加jin 
//快捷键
BOOL CWhiteBoardView::PreTranslateMessage(MSG* pMsg)
{
	if(g_bSupportI7)
	{
		return TRUE;
	}
	//删除键
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
	{
		DeleteSelectedObjects();
		return TRUE;	
	}
	//页中的首页
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_END)
	{

		g_pWBPage->ClearSelectFrameList();
		DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		if (IsWindow(m_hWnd))
		{
			Invalidate();
		}
		LastPage();
		return TRUE;	
	}
	//页中的末页
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_HOME)
	{

		g_pWBPage->ClearSelectFrameList();
		DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		if (IsWindow(m_hWnd))
		{
			Invalidate();
		}
		FirstPage();
		return TRUE;	
	}
	//页中的上一页
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_PRIOR)
	{

		g_pWBPage->ClearSelectFrameList();
		DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		if (IsWindow(m_hWnd))
		{
			Invalidate();
		}
		PrePage();
		return TRUE;	
	}
	//页中的下一页
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_NEXT)
	{

		g_pWBPage->ClearSelectFrameList();
		DeleteEdit();
		g_nDrawStatus = DRAW_NORMAL;
		if (IsWindow(m_hWnd))
		{
			Invalidate();
		}
		NextPage();
		return TRUE;	
	}
	//UNDO
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 'Z' && IsCTRLPressed())
	{
		Undo();
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		return TRUE;	
	}
	//REDO
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 'Y' && IsCTRLPressed())
	{
		Redo();
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		return TRUE;	
	}
	//建新文件
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 'N' && IsCTRLPressed())
	{
		WB_Newfile();
		return TRUE;
	}
	//打开文件
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 'O' && IsCTRLPressed())
	{
		WB_Openfile();
		return TRUE;
	}
	//保存文件
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 'S' && IsCTRLPressed())
	{
		WB_SaveWBFile();
		return TRUE;
	}
	//另存文件
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 'A' && IsCTRLPressed())
	{
		WB_Resavefile();
		return TRUE;
	}
	//打印文件
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 'P' && IsCTRLPressed())
	{
		DeleteEdit();
		g_pWBPage->ClearSelectFrameList();
		g_pWhiteBoardView->PrintPage();
		return TRUE;	
	}
	//关闭文件
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 'W' && IsCTRLPressed())
	{
		WB_Closefile();
		return TRUE;
	}
	//移动<-

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_LEFT) 

	{		
		g_pWBPage->MoveSelectedObj(CPoint(-1, 0));
		//MoveMsg(m_moveFirstPoint);
		Invalidate();
		return TRUE;	
	}
	//移动->
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RIGHT)
	{
		g_pWBPage->MoveSelectedObj(CPoint(1, 0));
		MoveMsg(m_moveFirstPoint);
		Invalidate();
		return TRUE;	
	}
	//移动上箭头
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP)
	{
		g_pWBPage->MoveSelectedObj(CPoint(0, -1));
		MoveMsg(m_moveFirstPoint);
		Invalidate();
		return TRUE;	
	}
	//移动下箭头
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN)
	{
		g_pWBPage->MoveSelectedObj(CPoint(0, 1));
		MoveMsg(m_moveFirstPoint);
		Invalidate();
		return TRUE;	
	}
	//屏蔽键盘Esc消息（有编辑框的时候）
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
	{
		return TRUE;
	}
	return CScrollView::PreTranslateMessage(pMsg);
}

void CWhiteBoardView::SendPacketInfoOnlyToNet( char *pData, int nLen, bool bToNet /*= ture*/ )//发送广播
{
	if (pData == NULL || nLen <= 0)
	{
		return;
	}
	try
	{
		if (g_pHPWBDataNotify != NULL  )
		{
			((IHPNetNotify *)g_pHPWBDataNotify)->SendWbData(pData, nLen, bToNet);
		}
	}
	catch (...)
	{
		TRACE(TEXT("catch void HPDTS::processDTSData \n"));
	}
	return;
}

void CWhiteBoardView::SendPacketData(char *pData, int nLen, bool bToNet /* = ture */)//发送保存服务器
{
	if (pData == NULL || nLen <= 0)
	{
		return;
	}
	try
	{
		if (g_pHPWBDataNotify != NULL && !g_isLoacalWB) //modify by yang
		{

			((IHPNetNotify *)g_pHPWBDataNotify)->SendWbData(pData, nLen, bToNet);

			if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord ) 
			{
				SendLocalDataToConferenceRecord(pData + 2*sizeof(int),nLen - 2*sizeof(int),true);
			}
		}
		else//单机版没有连接网络是用来实现undo redo
		{
			UINT usCmd;
			AVDataInPacket in((char*)pData,nLen);
			in >> usCmd;

			switch(usCmd)
			{
			case WB_NEW_COMMAND_DATA:
				{
					processWBDTSData(in);
					break;
				}

			default:
				TRACE("WhiteBoardView::processSendPacketData unknown command.usCmd = %d\n",usCmd);
			}
		}

	}
	catch (...)
	{
		TRACE(TEXT("catch void HPDTS::processDTSData \n"));
	}
	return;
}
void CWhiteBoardView::MediaConfVideoID(const char *pData,int nLen)
{
	CWBObject *pObj = NULL;
	POSITION pos;
	UINT npage = g_pWBFile->GetCurPageIndex();
	CWBPage *pPage = NULL;

	DWORD layerID = 0;
	memcpy((char*)&layerID,pData,sizeof(DWORD));
	char *pBuffer = (char*)pData + sizeof(DWORD);
	//int nMedialenth = nLen - sizeof(DWORD);
	pPage = g_pWBFile->GetPage(npage);
	if (pPage == NULL)
	{
		return;
	}
	CObList *MediaCameraList = pPage->GetMediaCameraList();
	if (MediaCameraList == NULL)
	{
		return;
	}
	for(int index = 0;index <MediaCameraList->GetCount();++index )
	{
		pos = MediaCameraList->FindIndex(index);
		if (pos == NULL)
		{
			continue;
		}
		pObj = (CWBObject *)MediaCameraList->GetAt(pos);
		if (pObj == NULL)
		{
			continue;
		}
		if (pObj->GetType() == WB_OBJ_MEDIA)
		{
			//((CWBMedia*)pObj)->GetMediaWindow()->PlayVideoData(pData,nLen);
			if (layerID == ((CWBMedia*)pObj)->GetLayerIndex())
			{
				((CWBMedia*)pObj)->GetMediaWindow()->PlayVideoData(pBuffer,nLen);
			}

		}
		if (pObj->GetType() == WB_OBJ_CAMERA)
		{
			if (layerID == ((CWBCamera*)pObj)->GetLayerIndex())
			{
				((CWBCamera*)pObj)->GetCameraDlg()->PlayVideoDataCarema(pBuffer,nLen);
			}

		}
	}
}
void CWhiteBoardView::MediaAudioID(const char *pData,int nLen)
{
	CWBObject *pObj = NULL;
	POSITION pos;
	UINT npage = g_pWBFile->GetCurPageIndex();
	CWBPage *pPage = NULL;
	DWORD layerID = 0;
	memcpy((char*)&layerID,pData,sizeof(DWORD));
	char *pBuffer = (char*)pData + sizeof(DWORD);
	int nMedialenth = nLen - sizeof(DWORD);
	pPage = g_pWBFile->GetPage(npage);
	if (pPage == NULL)
	{
		return;
	}
	CObList *MediaCameraList = pPage->GetMediaCameraList();
	if (MediaCameraList == NULL)
	{
		return;
	}
	for(int index = 0;index <MediaCameraList->GetCount();++index )
	{
		pos = MediaCameraList->FindIndex(index);
		if (pos == NULL)
		{
			continue;
		}
		pObj = (CWBObject *)MediaCameraList->GetAt(pos);
		if (pObj == NULL)
		{
			continue;
		}
		if (pObj->GetType() == WB_OBJ_MEDIA)
		{
			if (layerID == ((CWBMedia*)pObj)->GetLayerIndex())
			{
				((CWBMedia*)pObj)->GetMediaWindow()->PlayAudioData(pBuffer,nMedialenth);
			}

		}
		if (pObj->GetType() == WB_OBJ_CAMERA)
		{
			if (layerID == ((CWBCamera*)pObj)->GetLayerIndex())
			{
				((CWBCamera*)pObj)->GetCameraDlg()->PlayAudioDataCarema(pBuffer,nLen);
			}

		}
	}
}
void CWhiteBoardView::ProcessPacketData(const char *pData, int nLen) 
{
	if (pData == NULL || nLen < sizeof(int))
	{
		return;
	}
	XAutoLock lock(m_csOnWbDeleteObjLock);
	char* pXmlData = new char[nLen];
	if (pXmlData == NULL)
		return;
	memcpy(pXmlData,pData,nLen);
	int iCmdType = *((int*)pXmlData);
	char* pBuff = (char*)pXmlData + sizeof(int);
	switch(iCmdType)
	{
	case CMD_ADD_ATTRIBUTE: //添加对象
		{
			std::string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);

			//接收曲线图标从文件加载时发送的文件 与对象无关 发送到本地缓存文件夹目录 add by jiangchao 2013.5.28 
			//int mark = 0;
			//xmlPacket.Get(KEY_SENDFILEMARK, mark);
			//if (1 == mark)
			//{
			//	std::string strPath = "";
			//	DWORD dwObjId = 0;
			//	xmlPacket.Get(KEY_FILE_PATH, strPath);
			//	xmlPacket.Get(KEY_OBJ_ID, dwObjId);
			//	m_pRecvFileWBIcoCurveMap.insert(std::pair<DWORD, std::string>(dwObjId, strPath));
			//	break;
			//}

			// 			int nCmd = -1;
			// 			if (xmlPacket.Get(KEY_DRAG_CAMERA_TO_WB,nCmd) && nCmd == CMD_CAMARA_DROP_OBJ)
			// 			{
			// 				m_ulUserCompure = g_ulSessionID;
			// 			}
			ProcessObj(xmlPacket, iCmdType);
			//开启录播功能
			if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord)
			{
				string str_path = "";
				if (xmlPacket.Get(KEY_FILE_PATH,str_path) && str_path != "")
				{
					SendFileToConferenceRecord(str_path);					
				}
				SendDataToConferenceRecord(xmlPacket,iCmdType,true);
			}
			SetPageIsNeedDraw(TRUE);
			Invalidate();
			UpdateWindow();
			SetPageIsNeedDraw(FALSE);
		}
		break;
	case CMD_DEL_ATTRIBUTE: //删除对象
		//ProcessRemoveObj(xmlPacket);
		{
			int CMD = 0;
			unsigned int nLayerIndex = 0;
			unsigned long ulEventID = 0;
			unsigned long ulSendID = 0;
			memcpy((char *)&nLayerIndex, pBuff, sizeof(int));
			memcpy((char *)&ulEventID, pBuff + sizeof(int), sizeof(long));
			memcpy((char*)&CMD,pBuff +sizeof(int) + sizeof(long) +sizeof(long),sizeof(int));
			memcpy((char*)&ulSendID,pBuff+sizeof(int) + sizeof(long),sizeof(long));
			
			if (CWBToolTip::GetIns() != NULL)
			{
				CWBToolTip::GetIns()->Restore();
			}
			if (CMD == CMD_MEDIA_USER_LINEOUT)//wangxiaobin
			{
				break;
			}
			if (CMD == COMBINE_DELETE)
			{
				g_pWBPage->m_bIsDelCombine = TRUE;
			}
			if(CMD == COMCURVE_DELETE)
			{
				g_pWBPage->m_bIsDelComCurve = TRUE;
			}
			CString cstrLayerIndex, cstrSenderId,cstrLayer_Sender;
			cstrLayerIndex.Format(_T("%u"),nLayerIndex);
			cstrSenderId.Format(_T("%u"),ulSendID);
			cstrLayer_Sender.Format(_T("%s;%s"),cstrLayerIndex,cstrSenderId);

			SendMessage(WB_DELETEOBJ,(WPARAM)(LPCTSTR)cstrLayer_Sender, (LPARAM)ulEventID);
			//WBDeleteSingleObject(nLayerIndex,ulEventID);	
		}
		break;	
	case CMD_CHANGE_ATTRIBUTE: //修改对象
		{
			if (m_pEditObj != NULL)
			{
				return;
			}
			std::string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			ProcessObj(xmlPacket, iCmdType);
			if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord)
			{
				SendDataToConferenceRecord(xmlPacket,iCmdType,true);
			}
			//			PostMessage(WB_PROCESSOBJ, (WPARAM)pData, (LPARAM)iCmdType);
		}
		break;
	case CMD_CHANGE_LAYER: //改变图层
		{
			std::string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			ProcessObjLayer(xmlPacket);
		}		
		break;
	case CMD_DEL_ALL_ATTRIBUTE:  //删除所有属性
		{
			//已经在OnBroadcast_Data中处理了
			//	PostMessage(WM_DEL_ALL_ATTRIBUTE,0, 0);
			unsigned long ulSessionID = 0;
			memcpy((char *)&ulSessionID, pBuff, sizeof(unsigned long));
			if (ulSessionID != g_ulSessionID)
			{
				//删除正在操作的触摸对象
				CWBMTObjectManager::GetIns_()->RemoveAllMTObject();

				SendMessage(WM_CLOSEWBFUNCTIONDLG,0,0);
				//::SendMessage(m_pWBFunctionDlg->GetSafeHwnd(),IDC_SINWB_CLOSEFUNCPAD,0,0);
				if (NULL != m_pWBFunctionDlg)
				{
					m_pWBFunctionDlg->OnBnClickedClosewbfunctiondlg();
				}
				SendMessage(WM_PROCESS_FILE_OPERATE);
				//还原白板打开的开始状态
				BOOL bIsResCreted = WBRESWND::IsGalleryWndCreated();
				if(bIsResCreted)
				{
					WBRESWND::HideGalleryWndEx();
				}
				if (m_MedioVideoScreenDlg)
				{
					delete m_MedioVideoScreenDlg;
					m_MedioVideoScreenDlg = NULL;
				}
				DestroyScreenDraw();
				if (NULL != m_WBSpotlight)
				{
					delete m_WBSpotlight;
					m_WBSpotlight =NULL;
				}
				if (NULL != m_pWBRectMaqn)
				{
					delete m_pWBRectMaqn;
					m_pWBRectMaqn = NULL;
				}
			}	
			((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
		}
		break;
	case CMD_INSERT_PAGE:
		{
			std::string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			ProcessInsertPage(xmlPacket,iCmdType);

			//删除正在操作的触摸对象
			CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		}
		break;
	case CMD_DEL_PAGE:
		{
			std::string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			ProcessDeletePage(xmlPacket,iCmdType);
			//删除正在操作的触摸对象
			CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		}
		break;
	case CMD_CHANGE_PAGE:
		{
			std::string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			ProcessExchangPage(xmlPacket,iCmdType);
			//删除正在操作的触摸对象
			CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		}
		break;
	case CMD_MEDIA_OPERATION:	//对媒体进行操作
		{

			int medialen = nLen - sizeof(int);//视频会议录制图像录制
			MediaConfVideoID(pBuff,medialen);
		}
		break;
	case CMD_BROADCAST_DATA:
		{
			std::string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			OnBroadcast_Data(xmlPacket);
		}
		break;
	case CMD_DEL_PAGE_ALL_ATTRIBUTE:
		{
			int nPageid = 0;
			unsigned long ulEventID = 0;
			memcpy((char *)&nPageid, pBuff, sizeof(int));
			memcpy((char *)&ulEventID, pBuff + sizeof(int), sizeof(long));
			SendMessage(WM_DEL_PAGE_ALL_OBJ,(WPARAM)nPageid, (LPARAM)ulEventID);
			//删除正在操作的触摸对象
			CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		}
		break;
	case CMD_MEDIA_AUDIO_ID:
		{
			int medialen = nLen - sizeof(int);//视频会议录制音频录制
			MediaAudioID(pBuff,medialen);
		}
		break;
#ifdef HUBEI_MEDICAL_SDK
	case CMD_RELATIVE_USERCOLOR:
		{
			int iCmdType = *((int*)pXmlData);
			char* pBuff = (char*)pXmlData + sizeof(int);
			unsigned int nRed = *((unsigned int*)pBuff);
			pBuff += sizeof(unsigned int);
			unsigned int nGreen = *((unsigned int*)pBuff);
			pBuff += sizeof(unsigned int);
			unsigned int nBlue = *((unsigned int*)pBuff);
			pBuff += sizeof(unsigned int);
			CString str_name = ToDRAWStr(pBuff).c_str();
			if (str_name == g_strAvconUserID)
			{
				WB_SetDefaultLineColoe(nRed,nGreen,nBlue);
			}
		}
		break;
#endif

	default:
		break;
	}	
	if (pXmlData)
	{
		delete[] pXmlData;
		pXmlData = NULL;
	}
}

void CWhiteBoardView::ProcessRemoveObj(DWORD dwLayerIndex, unsigned long ulEventID)
{
	//如果删除的是聚光灯对象
	if (m_WBSpotlight != NULL)
	{
		if (m_WBSpotlight->GetLayerIndex() == dwLayerIndex)
		{
			CloseSpotlight();
			return;
		}
	}

	CWBPage *pObjPage = NULL;

	for (int i = 1; i <= (int)g_pWBFile->GetPageCount(); ++i )
	{
		pObjPage = g_pWBFile->GetPage(i);
		if (pObjPage == NULL)
			continue;

		CWBObject *pObj = pObjPage->FindByLayerIndex(dwLayerIndex);
		if (pObj != NULL)
		{
			if (pObj->GetType() == WB_OBJ_COMBINE)
			{
				//删除当前对象在选择框列表中的记录
				pObjPage->DeleteSelectFrame(dwLayerIndex);
			}
		}
		if (pObjPage->DeleteWBObject(dwLayerIndex))
		{
			if (i == g_pWBFile->GetCurPageIndex())
			{
				if (m_ulRecEventID != ulEventID)
				{
					if (IsWindow(m_hWnd))
					{
						if (CScreenDrawMgr::GetIns() != NULL)
							CScreenDrawMgr::GetIns()->Invalidate();
						else
							Invalidate();
					}
					//Invalidate();
					m_ulRecEventID = ulEventID;
				}
			}
			break;
		}
	}
	return;
}

void CWhiteBoardView::ProcessObj(XMemIni xmlObj, int nCmdType /* = CMD_ADD_ATTRIBUTE */)
{
	DWORD dwObjLayerIndex = 0; 
	int nType = 0;
	int nPageIndex = 0;
	unsigned long ulEventID = 0; //接收事件ID
	unsigned long ulSender = 0;

	if (g_pWhiteBoardView->m_hWnd == NULL)
		return;
	xmlObj.Get(KEY_OBJ_TYPE, nType);
	xmlObj.Get(KEY_FILE_SENDER, ulSender);

	if (!xmlObj.Get(KEY_WHITEBOARD_WIDTH, g_nWidth))
	{
		g_nWidth = WB_VIEWHSCROLL;
	}
	if (!xmlObj.Get(KEY_WHITEBOARD_HEIGHT, g_nHeight))
	{
		g_nHeight = WB_VIEWVSCROLL;
	}

	//取对象图层号、类型、页码
	if (!xmlObj.Get(KEY_OBJ_ID, dwObjLayerIndex)
		|| !xmlObj.Get(KEY_OBJ_PAGE, nPageIndex)
		|| !xmlObj.Get(KEY_EVENT_ID, ulEventID)
		)
	{
		return;
	}	
	//如果总页数小于对象页码，则添加新页
	UINT nCurPage = g_pWBFile->GetCurPageIndex();

	while (nPageIndex > (int)g_pWBFile->GetPageCount())
	{
		if (g_pWhiteBoardView->m_hWnd == NULL)
		{
			return;
		}
		m_pageIsNeedDraw = TRUE;	
		CWBPage* pPage = g_pWBFile->AddPage(this);
		g_pWhiteBoardView->SendMessage(WM_CANCELMODE, 0, 0);
		//删除正在操作的触摸对象
		CWBMTObjectManager::GetIns_()->RemoveAllMTObject();
		if (CWBToolTip::GetIns())
		{
			CWBToolTip::GetIns()->Restore();
		}
		if(IsOpenPreviewPanel())
		{
			CDC *ViewDC = NULL;
			ViewDC =GetDC();
			if (NULL == ViewDC || NULL == ViewDC->m_hDC)
			{
				return ;
			}
			pPage->Draw(ViewDC);
			ReleaseDC(ViewDC);
			if (m_pWBFunctionDlg && NULL != m_pWBFunctionDlg->GetPreviewDLG())
			{
				::SendMessage(m_pWBFunctionDlg->GetPreviewDLG()->m_hWnd,WM_WB_FUNCTION_ADDPREVIEW,(WPARAM)pPage,0);
			}
		}
		g_pWBPage->StartOrPauseMovie(ulSender);
		SetFlashCurPage(FALSE);
		//让新建的页滚动条在开始位置
		setCurrentViewScroll(CPoint(0,0));
	}

	if (nPageIndex!=nCurPage)
	{
		ProcessChangePage(nPageIndex);//主要处理视频flash
		g_pWBFile->GotoPage(nPageIndex);
		g_pWBPage = g_pWBFile->GetPage(nPageIndex);

	}

	CWBPage *pObjPage = g_pWBFile->GetPage(nPageIndex);

	//如果对象ID号为0
	if (dwObjLayerIndex == 0)
	{
		ProcessAttiObj(xmlObj);
		m_nComCount = 0;
		//设置页按钮的显示状态
		if (nCurPage != nPageIndex)
		{
			((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
		}
		ShowOrHideMediaCarema(nPageIndex);
		return;
	}
	//设置页按钮的显示状态
	if (nCurPage != nPageIndex)
	{
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetPageDlg()->SetPreAndNextBtnStatic();
	}
	WB_OBJ_TYPE objType = (WB_OBJ_TYPE)nType;

	CWBObject *pObj = pObjPage->FindByLayerIndex(dwObjLayerIndex, objType);

	if (pObj == NULL) //如果没在对象列表里找到，就到组合列表里找
	{
		pObj = pObjPage->FindComObjByLayerIndex(dwObjLayerIndex);
		if (pObj != NULL)
		{
			m_pComObj = pObj;
			m_dwComLayerIndex = dwObjLayerIndex;
		}
	}
	ShowOrHideMediaCarema(nPageIndex);
	if (pObjPage->GetvecLineObjID().size() > 0)//为擦除直线服务
	{
		pObjPage->DeleteLine();
	}

	if (g_pWhiteBoardView->m_hWnd == NULL)
		return;
	int bConferenceRecord = 0;
	if(xmlObj.Get(KEY_CONFERENCE_RECORD,bConferenceRecord) && bConferenceRecord == 1)
	{
		string str_path = "";
		if (xmlObj.Get(KEY_FILE_PATH,str_path) && str_path != "")//设置录播下新的临时路径  用于查找文件路径的
		{
			int npos = str_path.find_last_of("\\") +1;
			string str_name = str_path.substr(npos);
			str_path = ToStr(g_strAvconTempPath.GetBuffer(g_strAvconTempPath.GetLength()));
			str_path += "\\";
			str_path += str_name;
			xmlObj.Set(KEY_FILE_PATH,str_path);	
			g_strAvconTempPath.ReleaseBuffer();
		}
	}

	if (pObj == NULL/*(pObj == NULL && CMD_ADD_ATTRIBUTE == nCmdType) 
		|| (pObj == NULL && (objType == WB_OBJ_MAQN || objType == WB_OBJ_SPOTLIGHT))
		|| (pObj == NULL && m_bIsconfernce)*/)//如果没有找到对象，则添加新对象
	{
		m_bIsconfernce = false;
		XAutoLock l(m_csOnWbChangeObjLock);
		switch (objType)
		{
		case WB_OBJ_NORMAL:
			{
			}
			break;
		case WB_OBJ_LINE: //直线
			{
				CWBLine *pLine = new CWBLine();
				if (!pLine->GetXmlValue(xmlObj))
				{
					delete pLine;
					break;
				}
				if (pLine->IsCombineMember())
				{
					DWORD dwLayerIndex = 0;
					dwLayerIndex = pLine->GetComLayerIndex();
					if (!(pObjPage->AddComObject(dwLayerIndex, pLine)))
					{
						pObjPage->AddWBObject(pLine, pLine->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pLine, pLine->GetLayerType());
				}
				pLine->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_CURVE: //曲线
			{
				CWBCurve *pCurve = new CWBCurve();
				pCurve->SetIsDraw();
				if (!pCurve->GetXmlValue(xmlObj))
				{
					delete pCurve;
					break;
				}

				if (pCurve->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pCurve))
					{
						pObjPage->AddWBObject(pCurve, pCurve->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pCurve, pCurve->GetLayerType());
				}

				pCurve->SetLayerIndex(dwObjLayerIndex);
				pCurve->SetIsDraw();
				
			}
			break;
		case WB_OBJ_ICOCURVE: //图标曲线
			{
				CWBIcoCurve * pIcoCurve = new CWBIcoCurve();
				pIcoCurve->SetIsRefush(TRUE);
				if (!pIcoCurve->GetXmlValue(xmlObj))
				{
					delete pIcoCurve;
					break;
				}

				if (pIcoCurve->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pIcoCurve))
					{
						pObjPage->AddWBObject(pIcoCurve, pIcoCurve->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pIcoCurve, pIcoCurve->GetLayerType());
				}

				pIcoCurve->SetLayerIndex(dwObjLayerIndex);
				pIcoCurve->SetIsRefush(TRUE);
				//pIcoCurve->SetIsNeedSetImage(TRUE);
				pIcoCurve->SetIcoImage(); //设置Image对象
			}
			break;
		case WB_OBJ_MAGICPEN: //魔术线
			{
				CWBMagicPen *pMagicPen = new CWBMagicPen();
				if (!pMagicPen->GetXmlValue(xmlObj))
				{
					delete pMagicPen;
					break;
				}
				pMagicPen->SetLayerIndex(dwObjLayerIndex);
				pMagicPen->SetIsDraw();
				pMagicPen->CreateSelecrtFrame();
				(g_pWBPage->m_pMagicList).push_back(pMagicPen);
				SetTimer(ID_TIEMR_MAGICPEN, 1000, NULL);
			}
			break;
		case WB_OBJ_POLYGON: //多边形
			{
				CWBPolygon *pPolygon = new CWBPolygon();
				if (!pPolygon->GetXmlValue(xmlObj))
				{
					delete pPolygon;
					break;
				}
				if (pPolygon->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pPolygon))
					{
						pObjPage->AddWBObject(pPolygon, pPolygon->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pPolygon, pPolygon->GetLayerType());
				}
				pPolygon->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_RECT:	//矩形
			{
				CWBRectangle *pRect = new CWBRectangle();
				if (!pRect->GetXmlValue(xmlObj))
				{
					delete pRect;
					break;
				}

				if (pRect->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pRect))
					{
						pObjPage->AddWBObject(pRect, pRect->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pRect, pRect->GetLayerType());
				}

				pRect->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_PALLELOGAM: //平行四边形
			{
				CWBPallelogam *pPallelogam = new CWBPallelogam();
				if (!pPallelogam->GetXmlValue(xmlObj))
				{
					delete pPallelogam;
					break;
				}
				if (pPallelogam->IsCombineMember())
				{
					if (!pObjPage->AddObjIntoCom(pPallelogam))
					{
					}
				}
				pObjPage->AddWBObject(pPallelogam,pPallelogam->GetLayerType());

				pPallelogam->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_ARBITRARYTRIANGLE:
			{
				CWBArbitraryTriangle *pArbTriangle = new CWBArbitraryTriangle();
				if (!pArbTriangle->GetXmlValue(xmlObj))
				{
					delete pArbTriangle;
					pArbTriangle = NULL;
				}
				if (pArbTriangle->IsCombineMember())
				{
					if (!pObjPage->AddObjIntoCom(pArbTriangle))
					{
						
					}
				}
				pObjPage->AddWBObject(pArbTriangle,pArbTriangle->GetLayerType());
				pArbTriangle->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_TRIANGLE: //三角形
			{
				CWBTriangle *pTriangle = new CWBTriangle();
				if (!pTriangle->GetXmlValue(xmlObj))
				{
					delete pTriangle;
					break;
				}
				if (pTriangle->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pTriangle))
					{
						pObjPage->AddWBObject(pTriangle, pTriangle->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pTriangle, pTriangle->GetLayerType());
				}

				pTriangle->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_RHOMBUS: //菱形		
			{
				CWBRhombus *pRhombus = new CWBRhombus();
				if (!pRhombus->GetXmlValue(xmlObj))
				{
					delete pRhombus;
					break;
				}
				if (pRhombus->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pRhombus))
					{
						pObjPage->AddWBObject(pRhombus, pRhombus->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pRhombus, pRhombus->GetLayerType());
				}

				pRhombus->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_PENTAGON: //五边形		
			{
				CWBPentagon *pPentagon = new CWBPentagon();
				if (!pPentagon->GetXmlValue(xmlObj))
				{
					delete pPentagon;
					break;
				}
				if (pPentagon->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pPentagon))
					{
						pObjPage->AddWBObject(pPentagon, pPentagon->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pPentagon, pPentagon->GetLayerType());
				}

				pPentagon->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_HEXAGON: //六边形		
			{
				CWBHexagon *pHexagon = new CWBHexagon();
				if (!pHexagon->GetXmlValue(xmlObj))
				{
					delete pHexagon;
					break;
				}
				if (pHexagon->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pHexagon))
					{
						pObjPage->AddWBObject(pHexagon, pHexagon->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pHexagon, pHexagon->GetLayerType());
				}

				pHexagon->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_FIVEPOINTSTART: //五角星	
			{
				CWBFivepointstart *pFivepointstart = new CWBFivepointstart();
				if (!pFivepointstart->GetXmlValue(xmlObj))
				{
					delete pFivepointstart;
					break;
				}
				if (pFivepointstart->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pFivepointstart))
					{
						pObjPage->AddWBObject(pFivepointstart, pFivepointstart->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pFivepointstart, pFivepointstart->GetLayerType());
				}

				pFivepointstart->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_TRAPEZIUM: //梯形		
			{
				CWBTrapezium *pTrapezium = new CWBTrapezium();
				if (!pTrapezium->GetXmlValue(xmlObj))
				{
					delete pTrapezium;
					break;
				}
				if (pTrapezium->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pTrapezium))
					{
						pObjPage->AddWBObject(pTrapezium, pTrapezium->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pTrapezium, pTrapezium->GetLayerType());
				}

				pTrapezium->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_ELLIPSE: //圆形
			{
				CWBEllipse *pEllipse = new CWBEllipse();
				if (!pEllipse->GetXmlValue(xmlObj))
				{
					delete pEllipse;
					break;
				}
				if (pEllipse->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pEllipse))
					{
						pObjPage->AddWBObject(pEllipse, pEllipse->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pEllipse, pEllipse->GetLayerType());
				}

				pEllipse->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_SECTOR:
			{
				CWBSector *pSector = new CWBSector();
				if (!pSector->GetXmlValue(xmlObj))
				{
					delete pSector;
					break;
				}
				if (pSector->IsCombineMember())//如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pSector))
					{
						pObjPage->AddWBObject(pSector, pSector->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pSector, pSector->GetLayerType());
				}
				pSector->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_ARC: //直线
			{
				CWBArc *pArc = new CWBArc();
				if (!pArc->GetXmlValue(xmlObj))
				{
					delete pArc;
					break;
				}
				if (pArc->IsCombineMember())
				{
				    if (!pObjPage->AddObjIntoCom(pArc))
					{
						pObjPage->AddWBObject(pArc, pArc->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pArc, pArc->GetLayerType());
				}
				pArc->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_TEXT: //文本
			{
				CWBText *pText = new CWBText();
				if (!pText->GetXmlValue(xmlObj))
				{
					delete pText;
					break;
				}
				if (pText->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pObjPage->AddObjIntoCom(pText))
					{
						pObjPage->AddWBObject(pText, pText->GetLayerType());
					}
				}
				else
				{
					pObjPage->AddWBObject(pText, pText->GetLayerType());
				}

				pText->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_PICTURE: //图片
			{
				bool bflag = true;
				int nData = 0; //判断是否是自己的
				if (xmlObj.Get(KEY_PICTURE_SENDSTING,nData)&& nData == 100)
				{
					if (g_WBType == WB_SINGLERVER)
					{
						break;
					}
					string str = "";
					if (xmlObj.Get(KEY_PICTURE_STRING,str)&& str != "")
					{
						std::string strName = "";
						if (xmlObj.Get(KEY_CAMERA_ID_OF_UCC_LOGIN_NAME,strName)&& strName != "")
						{
							m_pUserNameID->insert(pair<std::string,DWORD>(strName,dwObjLayerIndex));
						}
						unsigned long ulChannelID = 0;
						xmlObj.Get(KEY_PICTURE_CHANGNEL_ID,ulChannelID);
						//CString str1 = ToDRAWStr(str).c_str();
						CString str1 = A2U(str);
						bflag = ProcessGetCamInfo(dwObjLayerIndex ,str1,strName,ulChannelID);
					}
				}
				if (!bflag)
				{
					break;
				}
				CWBPicture *pPicture = new CWBPicture();
				if (!pPicture->GetXmlValue(xmlObj))
				{
					delete pPicture;
					break;
				}

				if (!pObjPage->AddObjIntoCom(pPicture))
				{
					pObjPage->AddWBObject(pPicture, pPicture->GetLayerType());
					if (pPicture->IsBkg())
					{
						int nWidth, nHeight;
						nWidth = ((CWBPicture *)pPicture)->GetWidth();
						nHeight = ((CWBPicture *)pPicture)->GetHeight();
						if (!(nWidth < 2400 && nHeight < 1600))
						{
							if (nWidth > 37000)
							{
								nHeight = 37000 * nHeight / nWidth;
								nWidth = 37000;
							}
							if (nHeight > 37000)
							{
								nWidth = 37000 * nWidth / nHeight;
								nHeight = 37000;
							}
							g_nWidth = nWidth;
							g_nHeight = nHeight;
							setViewScroll();
							CWBPreviewSingleView* ptempsingleview = m_pEagleEyeDlg->GetPreviewSingleView();
							ptempsingleview->SetRatio();
							ptempsingleview->SetRectInPreview();
							ptempsingleview = NULL;
						}
					}

				}

				pPicture->SetLayerIndex(dwObjLayerIndex);
				pPicture->SetIsPath();
			}
			break;
		case WB_OBJ_DXF: //Dxf
			{
				CWBDxf *pDxf = new CWBDxf();
				if(!pDxf->GetXmlValue(xmlObj))
				{
					delete pDxf;
					pDxf = NULL;
				}
				if (!pObjPage->AddObjIntoCom(pDxf))
				{
					pObjPage->AddWBObject(pDxf, pDxf->GetLayerType());
				}
				pDxf->SetLayerIndex(dwObjLayerIndex);
				pDxf->SetIsNeedReDraw();
			}
			break;
		case WB_OBJ_MEDIA:
			{
				XMemIni *pMediaXmlobj = new XMemIni();
				*pMediaXmlobj = xmlObj;
				SendMessage(WM_ADDMEDIA,(WPARAM)pMediaXmlobj,NULL);//发送消息，通知更新					
				
			}
			break;
		case WB_OBJ_CAMERA://摄像头
			{
				XMemIni *pCameraXmlobj = new XMemIni();
				*pCameraXmlobj = xmlObj;
				SendMessage(WM_ADDCAMERA,(WPARAM)pCameraXmlobj,NULL);//发送消息，通知更新
			}
			break;
		case WB_OBJ_DOCUMENT: //文档
			{
				CWBDocument *pDocument = new CWBDocument();
				if (pDocument == NULL)
				{
					break;
				}
				pDocument->SetPageIndex(nPageIndex);
				if (!pDocument->GetXmlValue(xmlObj))
				{
					delete pDocument;
					break;
				}
				if (!pObjPage->AddObjIntoCom(pDocument))
				{
					pObjPage->AddWBObject(pDocument, pDocument->GetLayerType());
				}
				pDocument->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_FLASH: //FLASH
			{
				XMemIni* pFlashXMemini =  new XMemIni();
				*pFlashXMemini = xmlObj;
				SendMessage(WB_ADDFLASH,(WPARAM)pFlashXMemini,NULL);//发送消息，通知更新
			}
			break;
		case WB_OBJ_COMBINE: //组合对象
			{
				CWBCombine *pCombine = new CWBCombine();
				if (pCombine == NULL)
				{
					break;
				}
				m_nComCount = 0;
				if (!pCombine->GetComLayerXml(xmlObj))
				{
					delete pCombine;
					break;
				}
				int nAlpha = 0;
				xmlObj.Get(KEY_OBJ_ALPHA, nAlpha);
				pCombine->SetAlpha(nAlpha);
				if (!pCombine->AddCombineObj())//撤销后重做，找不到组合的对象，返回FALSE
				{
					if (!pCombine->GetComXml(xmlObj))
					{
						delete pCombine;
						break;
					}
					g_pWBPage->ClearSelectFrameList();
					pCombine->SetCombineMember(FALSE);
					pCombine->SetSelectFrameShow(FALSE);
					pCombine->SetEqualScaleZoom(TRUE);
					if (!pObjPage->AddObjIntoCom(pCombine))
					{
						pObjPage->AddWBObject(pCombine, pCombine->GetLayerType());
					}
					pCombine->InitImageButton();
					pCombine->SetLayerIndex(dwObjLayerIndex);
					pCombine->SetComLayerNum();
					break;
				}
				if (!pCombine->GetXmlValue(xmlObj))
				{
					delete pCombine;
					break;
				}
				g_pWBPage->ClearSelectFrameList();
				pCombine->SetCombineMember(FALSE);
				pCombine->SetSelectFrameShow(FALSE);
				pCombine->SetEqualScaleZoom(TRUE);
				if (!pObjPage->AddObjIntoCom(pCombine))
				{
					pObjPage->AddWBObject(pCombine, pCombine->GetLayerType());
				}
				pCombine->InitImageButton();
				pCombine->SetLayerIndex(dwObjLayerIndex);
				pCombine->SetComLayerNum();
			}
			break;

		case WB_OBJ_BAFFLE: //挡板
			{
				CWBBafflePlate *pBafflePlate = new CWBBafflePlate();
				if (!pBafflePlate->GetXmlValue(xmlObj))
				{
					delete pBafflePlate;
					break;
				}
				pObjPage->AddWBObject(pBafflePlate, pBafflePlate->GetLayerType()); //添加对象
				pBafflePlate->SetLayerIndex(dwObjLayerIndex);
			}
			break;

		case WB_OBJ_MAQN://放大镜
			{
				//此处不能屏蔽自身消息，因为关闭本会议窗口后，恢复时要处理自己发送的消息
				XMemIni* pXMemini =  new XMemIni();
				*pXMemini = xmlObj;
				SendMessage(WB_MAQNOBJ,(WPARAM)pXMemini,NULL);//发送消息，通知更新
			}
			break;
		case WB_OBJ_SPOTLIGHT://聚光灯
			{
				XMemIni* pXMemini =  new XMemIni();
				*pXMemini = xmlObj;
				SendMessage(WB_SPOTLIGHTOBJ,(WPARAM)pXMemini,NULL);
			}
			break;
		case WB_OBJ_ANNOTATIONS: //标注
			{
				CWBAnnotations *pAnnotations = new CWBAnnotations();
				if (!pAnnotations->GetXmlValue(xmlObj))
				{
					delete pAnnotations;
					break;
				}
				pObjPage->AddWBObject(pAnnotations, pAnnotations->GetLayerType());
				pAnnotations->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_MIDI:
			{
				CWBMidi* pWBmidi = new CWBMidi();
				if (nCmdType == CMD_ADD_ATTRIBUTE)
				{
					pWBmidi->Setbneedinit(true);
					pWBmidi->SetBneedDraw(FALSE);
					pWBmidi->SetBSetZoomM(true);
					pWBmidi->SetBShowCompont(true);
				}
				if (!pWBmidi->GetXmlValue(xmlObj))
				{
					delete pWBmidi;
					pWBmidi = NULL;
					break;
				}
				if (!pObjPage->AddObjIntoCom(pWBmidi))
				{
					pObjPage->AddWBObject(pWBmidi,pWBmidi->GetLayerType());
				}

				pWBmidi->SetLayerIndex(dwObjLayerIndex);

			}
			break;
		default:
			break;
		}
	}
	else//如果对象已经存在，则修改对象
	{
		//这里无条件的打开其限制 add by huanglongquan 2012年12月14日
		//	pObj->Limit(false);
		RemoveLimitObjId((void*)pObj);

		if (nCmdType == CMD_ADD_ATTRIBUTE)
		{
			if (pObj->GetType() == WB_OBJ_PICTURE ||pObj->GetType() == WB_OBJ_FLASH ||pObj->GetType() == WB_OBJ_DOCUMENT 
			|| pObj->GetType() == WB_OBJ_CURVE || pObj->GetType() == WB_OBJ_ICOCURVE /*add by jiangchao 2013.5.14*/) //广播实时同步的是曲线不刷新
			{

			}
			else
			{
				XMemIni* pChangeXMemini =  new XMemIni();
				*pChangeXMemini = xmlObj;
				SendMessage(WB_PROCESSOBJ,(WPARAM)pChangeXMemini,NULL);//发送消息，通知更新
				return;
			}
		}
		//服务器返回的消息如果是本地发送的，则直接返回。
		if(ulSender == g_ulSessionID)
			 return;
		
		XMemIni* pChangeXMemini =  new XMemIni();
		*pChangeXMemini = xmlObj;
		SendMessage(WB_PROCESSOBJ,(WPARAM)pChangeXMemini,NULL);//发送消息，通知更新
	}

	int nFlag;
	if (!xmlObj.Get(KEY_EVENT_OBJNUMBER, nFlag))
	{
		nFlag = 1;
	}

}


void CWhiteBoardView::ProcessBroadcastAttiObj( XMemIni xmlObj )
{

	m_xmlBroadCastAttiObj = xmlObj;
	SendMessage(WM_PROCESSBROADCASTOBJ);
}

afx_msg LRESULT CWhiteBoardView::OnProcessBroadcastObj(WPARAM wParam, LPARAM lParam)
{
	XMemIni xmlObj;
	xmlObj = m_xmlBroadCastAttiObj;
	m_xmlBroadCastAttiObj.DeleteAllKey();
	DWORD dwObjLayerIndex = 0; 
	int nType = 0;
	int nPageIndex = 0;
	unsigned long ulEventID = 0; //接收事件ID
	if (g_pWhiteBoardView->m_hWnd == NULL) return 0;
	xmlObj.Get(KEY_OBJ_TYPE, nType);
	//取对象图层号、类型、页码
	if (!xmlObj.Get(KEY_OBJ_ID, dwObjLayerIndex)
		|| !xmlObj.Get(KEY_OBJ_PAGE, nPageIndex)
		|| !xmlObj.Get(KEY_EVENT_ID, ulEventID) )
		return 0;

	if (nPageIndex > (int)g_pWBFile->GetPageCount()) return 0;

	CWBPage *pPage = g_pWBFile->GetPage(nPageIndex);

	WB_OBJ_TYPE objType = (WB_OBJ_TYPE)nType;

	BOOL bFlag = FALSE; //标记是不是组合里的对象

	XAutoLock l(m_csOnWbChangeObjLock);
	CWBObject *pObj = pPage->FindByLayerIndex(dwObjLayerIndex, objType);

	if (pObj == NULL)//如果没有找到对象，则添加新对象
	{
		XAutoLock l(m_csOnWbChangeObjLock);
		switch (objType)
		{
		case WB_OBJ_NORMAL:
			{
			}
			break;
		case WB_OBJ_LINE: //直线
			{
				CWBLine *pLine = new CWBLine();
				if (!pLine->GetXmlValue(xmlObj))
				{
					delete pLine;
					break;
				}
				if (pLine->IsCombineMember())
				{
					DWORD dwLayerIndex = 0;
					dwLayerIndex = pLine->GetComLayerIndex();
					if (!(pPage->AddComObject(dwLayerIndex, pLine)))
					{
						pPage->AddWBObject(pLine, pLine->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pLine, pLine->GetLayerType());
				}
				pLine->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_CURVE: //曲线
			{
				CWBCurve *pCurve = new CWBCurve();
				if (pCurve == NULL)
				{
					break;
				}
				if (!pCurve->GetXmlValue(xmlObj))
				{
					delete pCurve;
					break;
				}

				if (pCurve->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pCurve))
					{
						pPage->AddWBObject(pCurve, pCurve->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pCurve, pCurve->GetLayerType());
				}

				pCurve->SetLayerIndex(dwObjLayerIndex);
				pCurve->SetIsDraw();
			}
			break;
		case WB_OBJ_ICOCURVE: //图标曲线
			{
				CWBIcoCurve *pIcoCurve = new CWBIcoCurve();
				if (pIcoCurve == NULL)
				{
					break;
				}
				if (!pIcoCurve->GetXmlValue(xmlObj))
				{
					delete pIcoCurve;
					break;
				}

				if (pIcoCurve->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pIcoCurve))
					{
						pPage->AddWBObject(pIcoCurve, pIcoCurve->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pIcoCurve, pIcoCurve->GetLayerType());
				}
				pIcoCurve->SetIsRefush(TRUE);
				//pIcoCurve->SetIsNeedSetImage(TRUE);
				pIcoCurve->SetIcoImage();
				pIcoCurve->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_POLYGON: //多边形
			{
				CWBPolygon *pPolygon = new CWBPolygon();
				if (!pPolygon->GetXmlValue(xmlObj))
				{
					delete pPolygon;
					break;
				}
				if (pPolygon->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pPolygon))
					{
						pPage->AddWBObject(pPolygon, pPolygon->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pPolygon, pPolygon->GetLayerType());
				}

				pPolygon->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_RECT:	//矩形
			{
				CWBRectangle *pRect = new CWBRectangle();
				if (!pRect->GetXmlValue(xmlObj))
				{
					delete pRect;
					break;
				}

				if (pRect->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pRect))
					{
						pPage->AddWBObject(pRect, pRect->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pRect, pRect->GetLayerType());
				}

				pRect->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_PALLELOGAM: //平行四边形
			{
				CWBPallelogam *pPallelogam = new CWBPallelogam();
				if (!pPallelogam->GetXmlValue(xmlObj))
				{
					delete pPallelogam;
					break;
				}
				if (pPallelogam->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pPallelogam))
					{
						pPage->AddWBObject(pPallelogam, pPallelogam->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pPallelogam, pPallelogam->GetLayerType());
				}

				pPallelogam->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_ARBITRARYTRIANGLE:
			{
				CWBArbitraryTriangle *pTriangle = new CWBArbitraryTriangle();
				if (!pTriangle->GetXmlValue(xmlObj))
				{
					delete pTriangle;
					break;
				}
				if (pTriangle->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pTriangle))
					{
						pPage->AddWBObject(pTriangle, pTriangle->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pTriangle, pTriangle->GetLayerType());
				}

				pTriangle->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_TRIANGLE: //三角形
			{
				CWBTriangle *pTriangle = new CWBTriangle();
				if (!pTriangle->GetXmlValue(xmlObj))
				{
					delete pTriangle;
					break;
				}
				if (pTriangle->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pTriangle))
					{
						pPage->AddWBObject(pTriangle, pTriangle->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pTriangle, pTriangle->GetLayerType());
				}

				pTriangle->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_RHOMBUS: //菱形		
			{
				CWBRhombus *pRhombus = new CWBRhombus();
				if (!pRhombus->GetXmlValue(xmlObj))
				{
					delete pRhombus;
					break;
				}
				if (pRhombus->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pRhombus))
					{
						pPage->AddWBObject(pRhombus, pRhombus->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pRhombus, pRhombus->GetLayerType());
				}

				pRhombus->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_PENTAGON: //五边形		
			{
				CWBPentagon *pPentagon = new CWBPentagon();
				if (!pPentagon->GetXmlValue(xmlObj))
				{
					delete pPentagon;
					break;
				}
				if (pPentagon->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pPentagon))
					{
						pPage->AddWBObject(pPentagon, pPentagon->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pPentagon, pPentagon->GetLayerType());
				}

				pPentagon->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_HEXAGON: //六边形		
			{
				CWBHexagon *pHexagon = new CWBHexagon();
				if (!pHexagon->GetXmlValue(xmlObj))
				{
					delete pHexagon;
					break;
				}
				if (pHexagon->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pHexagon))
					{
						pPage->AddWBObject(pHexagon, pHexagon->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pHexagon, pHexagon->GetLayerType());
				}

				pHexagon->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_FIVEPOINTSTART: //五角星	
			{
				CWBFivepointstart *pFivepointstart = new CWBFivepointstart();
				if (!pFivepointstart->GetXmlValue(xmlObj))
				{
					delete pFivepointstart;
					break;
				}
				if (pFivepointstart->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pFivepointstart))
					{
						pPage->AddWBObject(pFivepointstart, pFivepointstart->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pFivepointstart, pFivepointstart->GetLayerType());
				}

				pFivepointstart->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_TRAPEZIUM: //梯形		
			{
				CWBTrapezium *pTrapezium = new CWBTrapezium();
				if (!pTrapezium->GetXmlValue(xmlObj))
				{
					delete pTrapezium;
					break;
				}
				if (pTrapezium->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pTrapezium))
					{
						pPage->AddWBObject(pTrapezium, pTrapezium->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pTrapezium, pTrapezium->GetLayerType());
				}

				pTrapezium->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_ELLIPSE: //圆形
			{
				CWBEllipse *pEllipse = new CWBEllipse();
				if (!pEllipse->GetXmlValue(xmlObj))
				{
					delete pEllipse;
					break;
				}
				if (pEllipse->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pEllipse))
					{
						pPage->AddWBObject(pEllipse, pEllipse->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pEllipse, pEllipse->GetLayerType());
				}

				pEllipse->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_SECTOR:
			{
				CWBSector *pSector = new CWBSector();
				if (!pSector->GetXmlValue(xmlObj))
				{
					delete pSector;
					break;
				}
				if (pSector->IsCombineMember())//如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pSector))
					{
						pPage->AddWBObject(pSector, pSector->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pSector, pSector->GetLayerType());
				}
				pSector->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_ARC: //直线
			{
				CWBArc *pArc = new CWBArc();
				if (!pArc->GetXmlValue(xmlObj))
				{
					delete pArc;
					break;
				}
				if (pArc->IsCombineMember())
				{
					if (!pPage->AddObjIntoCom(pArc))
					{
						pPage->AddWBObject(pArc, pArc->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pArc, pArc->GetLayerType());
				}
				pArc->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_TEXT: //文本
			{
				CWBText *pText = new CWBText();
				if (!pText->GetXmlValue(xmlObj))
				{
					delete pText;
					break;
				}
				if (pText->IsCombineMember()) //如果是组合成员就加入组合
				{
					if (!pPage->AddObjIntoCom(pText))
					{
						pPage->AddWBObject(pText, pText->GetLayerType());
					}
				}
				else
				{
					pPage->AddWBObject(pText, pText->GetLayerType());
				}

				pText->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_PICTURE: //图片
			{
				CWBPicture *pPicture = new CWBPicture();
				if (!pPicture->GetXmlValue(xmlObj))
				{
					delete pPicture;
					break;
				}
				if (!pPage->AddObjIntoCom(pPicture))
				{
					pPage->AddWBObject(pPicture, pPicture->GetLayerType());
				}

				pPicture->SetLayerIndex(dwObjLayerIndex);

				pPicture->SetIsPath();
			}
			break;
		case WB_OBJ_DXF: //Dxf add by jiangchao 2013.08.27
			{
				CWBDxf *pDxf = new CWBDxf();
				if (!pDxf->GetXmlValue(xmlObj))
				{
					delete pDxf;
					break;
				}
				if (!pPage->AddObjIntoCom(pDxf))
				{
					pPage->AddWBObject(pDxf, pDxf->GetLayerType());
				}
				pDxf->SetLayerIndex(dwObjLayerIndex);
				pDxf->SetIsNeedReDraw();
			}
			break;
		case WB_OBJ_MIDI:
			{
				CWBMidi* pwbmidi = new CWBMidi();
				if (!pwbmidi->GetXmlValue(xmlObj))
				{
					delete pwbmidi;
					pwbmidi = NULL;
					break;
				}
				if (!pPage->AddObjIntoCom(pwbmidi))
				{
					pPage->AddWBObject(pwbmidi,pwbmidi->GetLayerType());
				}
				pwbmidi->SetLayerIndex(dwObjLayerIndex);

			}
			break;
		case WB_OBJ_DOCUMENT: //文档
			{
				CWBDocument *pDocument = new CWBDocument();
				if (pDocument == NULL)
				{
					break;
				}
				pDocument->SetPageIndex(nPageIndex);
				if (!pDocument->GetXmlValue(xmlObj))
				{
					delete pDocument;
					break;
				}
				if (!pPage->AddObjIntoCom(pDocument))
				{
					pPage->AddWBObject(pDocument, pDocument->GetLayerType());
				}
				pDocument->SetLayerIndex(dwObjLayerIndex);
			}
			break;
		case WB_OBJ_FLASH: //FLASH
			{
				XMemIni* pFlashXMemini =  new XMemIni();
				*pFlashXMemini = xmlObj;
				SendMessage(WB_ADDFLASH,(WPARAM)pFlashXMemini,NULL);//发送消息，通知更新
			}
			break;
		case WB_OBJ_COMBINE: //组合对象
			{
				CWBCombine *pCombine = new CWBCombine();
				if (pCombine == NULL)
				{
					break;
				}
				m_nComCount = 0;
				if (!pCombine->GetComLayerXml(xmlObj))
				{
					delete pCombine;
					break;
				}
				int nAlpha = 0;
				xmlObj.Get(KEY_OBJ_ALPHA, nAlpha);
				pCombine->SetAlpha(nAlpha);
				if (!pCombine->AddCombineObj())//撤销后重做，找不到组合的对象，返回FALSE
				{
					if (!pCombine->GetComXml(xmlObj))
					{
						delete pCombine;
						break;
					}
					g_pWBPage->ClearSelectFrameList();
					pCombine->SetCombineMember(FALSE);
					pCombine->SetSelectFrameShow(FALSE);
					pCombine->SetEqualScaleZoom(TRUE);
					if (!pPage->AddObjIntoCom(pCombine))
					{
						pPage->AddWBObject(pCombine, pCombine->GetLayerType());
					}
					pCombine->InitImageButton();
					pCombine->SetLayerIndex(dwObjLayerIndex);
					pCombine->SetComLayerNum();
					break;
				}
				if (!pCombine->GetXmlValue(xmlObj))
				{
					delete pCombine;
					break;
				}
				g_pWBPage->ClearSelectFrameList();
				pCombine->SetCombineMember(FALSE);
				pCombine->SetSelectFrameShow(FALSE);
				pCombine->SetEqualScaleZoom(TRUE);
				if (!pPage->AddObjIntoCom(pCombine))
				{
					pPage->AddWBObject(pCombine, pCombine->GetLayerType());
				}
				pCombine->InitImageButton();
				pCombine->SetLayerIndex(dwObjLayerIndex);
				pCombine->SetComLayerNum();
			}
			break;

		case WB_OBJ_BAFFLE: //挡板
			{

				CWBBafflePlate *pBafflePlate = new CWBBafflePlate();
				if (!pBafflePlate->GetXmlValue(xmlObj))
				{
					delete pBafflePlate;
					break;
				}
				pPage->AddWBObject(pBafflePlate, pBafflePlate->GetLayerType()); //添加对象
				pBafflePlate->SetLayerIndex(dwObjLayerIndex);
			}
			break;

		case WB_OBJ_MAQN://放大镜
			{
				//此处不能屏蔽自身消息，因为关闭本会议窗口后，恢复时要处理自己发送的消息
				XMemIni* pXMemini =  new XMemIni();
				*pXMemini = xmlObj;
				SendMessage(WB_MAQNOBJ,(WPARAM)pXMemini,NULL);//发送消息，通知更新
			}
			break;
		case WB_OBJ_ANNOTATIONS: //标注
			{
				CWBAnnotations *pAnnotations = new CWBAnnotations();
				if (!pAnnotations->GetXmlValue(xmlObj))
				{
					delete pAnnotations;
					break;
				}
				pPage->AddWBObject(pAnnotations, pAnnotations->GetLayerType());
				pAnnotations->SetLayerIndex(dwObjLayerIndex);
			}
			break;

		default:
			break;
		}
		return 0;
	}

	xmlObj.DeleteKey(KEY_FILE_PATH);


	//  只用对象存在的时候 ,让广播数据处理完了之后限制其操作，在接受到chang消息或add消息的时候才打开 huanglongquan add by 2012年12月14日
	//pObj->Limit(false); //先打开限制，处理完数据之后又把它限制
	RemoveLimitObjId((void*)pObj);

	pObj->GetXmlValue(xmlObj);
	if (pObj->GetType() == WB_OBJ_ICOCURVE)
	{
		((CWBIcoCurve*)pObj)->SetIsRefush(TRUE);
	}
	if (pObj->GetType() == WB_OBJ_CURVE)
	{
		((CWBCurve*)pObj)->SetIsDraw();
	}
	if (pObj->GetType() == WB_OBJ_PICTURE)
	{
		((CWBPicture*)pObj)->SetIsPath();
	}
	if(pObj->GetType() == WB_OBJ_DXF) //判断是否重画DXF
	{
		((CWBDxf *)pObj)->SetIsNeedReDraw(); 
	}

	if (pObj->GetType() == WB_OBJ_COMBINE)
	{
		((CWBCombine *)pObj)->GetComLayerXml(xmlObj);
		if (m_bEraseLineUp)
		{
			((CWBCombine *)pObj)->DeleteCurCombineObjectList();
			m_bEraseLineUp = FALSE;
		}

		int nCount = ((CWBCombine *)pObj)->GetComCount();
		m_nComCount -= nCount;

		CPoint ptRotationCenter;
		pObj->GetRotationCenter(ptRotationCenter);
		pObj->SetRectSelectRotationCenter(ptRotationCenter);
		pObj->SetRectSelectAngle(pObj->GetRotationAngle());
	}

	// 	//限制其操作
	int nbObjLimit = -1;
	xmlObj.Get(KEY_OBJ_LIMIT,nbObjLimit);
	bool bLimit =( nbObjLimit== 1 ? true : false );	
	//	pObj->Limit(bLimit);
	if(bLimit)		
		AddLimitObjId((void*)pObj);
	//end

	if (CScreenDrawMgr::GetIns() != NULL)
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		SetPageIsNeedDraw(TRUE);
		Invalidate();
		UpdateWindow();
		SetPageIsNeedDraw(FALSE);
	}
	return 0;
}


void CWhiteBoardView::ProcessObjLayer(XMemIni xmlObj)
{
	DWORD dwObjLayerIndex = 0; 
	int nPageIndex = 0;
	int nLayerType; //修改图层类型
	unsigned long ulSessionID; //发送者
	int nLayerNum = 0; //图层

	//取对象图层号、页码
	if (!xmlObj.Get(KEY_OBJ_ID, dwObjLayerIndex)
		|| !xmlObj.Get(KEY_OBJ_PAGE, nPageIndex)
		|| !xmlObj.Get(KEY_LAYER_CHANGETYPE, nLayerType)
		|| !xmlObj.Get(KEY_FILE_SENDER, ulSessionID)
		|| !xmlObj.Get(KEY_OBJ_LAYER_NUM,nLayerNum))
	{
		return;
	}
	//如果是自己发送的，则返回
	if (ulSessionID == g_ulSessionID)
		return;
	//如果页码不对
	if (nPageIndex <= 0 || nPageIndex > (int)g_pWBFile->GetPageCount())
	{
		return;
	}

	CWBPage *pObjPage = g_pWBFile->GetPage(nPageIndex);

	switch (nLayerType)
	{
	case WB_LAYER_SUPER: //上一层
		if (!pObjPage->SetObjSuperLayer(dwObjLayerIndex))
			return;
		break;
	case WB_LAYER_UNDER: //下一层
		if (!pObjPage->SetObjUnderLayer(dwObjLayerIndex))
			return;
		break;
	case  WB_LAYER_TOP: //置顶
		if (!pObjPage->SetObjTopLayer(dwObjLayerIndex))
			return;
		break;
	case WB_LAYER_BOTTOM: //置底
		if (!pObjPage->SetObjBottomLayer(dwObjLayerIndex))
			return;
		break;
	default:
		return;
	}
	//如果当前页与修改页相同
	if (nPageIndex == g_pWBFile->GetCurPageIndex())
	{
		if (g_pWhiteBoardView->m_hWnd == NULL)
			return;
		if (CScreenDrawMgr::GetIns() != NULL)
			CScreenDrawMgr::GetIns()->Invalidate();
		else
			Invalidate();
	}
}

void CWhiteBoardView::GetAllObj()
{
	//关闭文件 先清空白板内容 然后再请求 （在单机版时要用到）
	//CloseLines();  单机版已经处理了 黄龙泉修改 在u_7中也处理了

	AVDataOutPacket outPacket;
	int nLen = sizeof(int);
	m_IsGetAllObj = true;
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		int iCmd = CMD_REQUEST_ALL_OBJ_ATTRIBUTE;
		memset(pData,0,nLen);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
	}

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete pData;
		pData = NULL;
	}
}

void CWhiteBoardView::SendAllObj()
{
	int nPageIndex = 1;
	XMemIni xmlObj;
	CWBPage *pWBPage = NULL;
	CWBObject *pWBObj = NULL;
	std::vector<DWORD *> vecPageOBjLayerIndex; //页所有对象的图层号
	unsigned long ulEventID = GenerateSSRC(); //事件ID

	m_bFirstLoginConference = TRUE; //add by jiangchao 2013.06.08

	BOOL bIsDrawScreen = g_bIsDrawScreen; //记录屏幕绘图窗口打开与否
	UINT nPageCount = g_pWBFile->GetPageCount();
	for( ; nPageIndex <= nPageCount; ++nPageIndex  )
	{
		pWBPage = g_pWBFile->GetPage(nPageIndex);
		if (pWBPage == NULL)
			continue;
		//发送标注图层对象
		g_bIsDrawScreen = TRUE;

		//得到页所有对象的图层号
		pWBPage->GetAllObjLayerIndex(vecPageOBjLayerIndex);
		if (vecPageOBjLayerIndex.empty())
		{
			CWBObject* pObj = new CWBObject();
			pObj->SetType(WB_OBJ_NORMAL);
			pObj->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_EVENT_ID, ulEventID);
			xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
			AddObj(xmlObj);
			delete pObj;
		} 
		else
		{
			for (int i = 0; i < (int)vecPageOBjLayerIndex.size();++ i )
			{
				if (vecPageOBjLayerIndex[i] == NULL)
					continue;
				pWBObj = pWBPage->FindByLayerIndex(*(vecPageOBjLayerIndex[i]));
				if (pWBObj == NULL)
					continue;
				if (pWBObj->GetType() == WB_OBJ_MEDIA)
				{
					xmlObj.Set(KEY_MEDIA_ADDSESSION,g_ulSessionID);
				}
				if (pWBObj->GetType() == WB_OBJ_CAMERA)
				{
					xmlObj.Set(KEY_CAREMA_ADDSESSION,g_ulSessionID);
				}
				pWBObj->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				AddObj(xmlObj);
				delete vecPageOBjLayerIndex[i];
			}//for
		}
		vecPageOBjLayerIndex.clear();

		//发送白板对象
		g_bIsDrawScreen = FALSE;

		//得到页所有对象的图层号
		pWBPage->GetAllObjLayerIndex(vecPageOBjLayerIndex);
		if (vecPageOBjLayerIndex.empty())
		{
			CWBObject* pObj = new CWBObject();
			pObj->SetType(WB_OBJ_NORMAL);
			pObj->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_EVENT_ID, ulEventID);
			xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
			AddObj(xmlObj);
			delete pObj;
		} 
		else
		{
			for (int i = 0; i < (int)vecPageOBjLayerIndex.size(); ++i )
			{
				if (vecPageOBjLayerIndex[i] == NULL)
				{
					continue;
				}
				pWBObj = pWBPage->FindByLayerIndex(*(vecPageOBjLayerIndex[i]));
				if (pWBObj == NULL)
					continue;
				if (pWBObj->GetType() == WB_OBJ_COMBINE)
				{
					((CWBCombine *)pWBObj)->AddComMebMsg(ulEventID, nPageIndex);
				}
				BOOL bBirate = TRUE;
				if (pWBObj->GetType() == WB_OBJ_MEDIA)
				{
					((CWBMedia*)pWBObj)->SetMediaBitrate(TRUE);
					xmlObj.Set(KEY_MEDIA_ADDSESSION,g_ulSessionID);
					xmlObj.Set(KEY_MEDIA_BITRATE_BOOL,bBirate);
					((CWBMedia*)pWBObj)->SetMediaPlayState(Media_playing);
				}
				if (pWBObj->GetType() == WB_OBJ_CAMERA)
				{
					xmlObj.Set(KEY_CAREMA_ADDSESSION,g_ulSessionID);
				}

				pWBObj->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				xmlObj.Set(KEY_OBJ_PAGE, nPageIndex);
				AddObj(xmlObj);		

				if (pWBObj->GetType() == WB_OBJ_MEDIA)
				{
					((CWBMedia*)pWBObj)->SetMediaBitrate(FALSE);
					((CWBMedia*)pWBObj)->SetMediaBeCreated(TRUE);
				}
				delete vecPageOBjLayerIndex[i];
			}//for
		}
		vecPageOBjLayerIndex.clear();
	}//for

	//如果后面有空白页的话就发送最后面的页号
	XMemIni xmlPageObj;
	xmlPageObj.Set(KEY_EVENT_ID, 0);
	xmlPageObj.Set(KEY_OBJ_ID,0);
	int nPage = g_pWBFile->GetPageCount();
	xmlPageObj.Set(KEY_OBJ_PAGE,nPage);
	ChangeObj(xmlPageObj);

	SendScrollDraw();
	g_bIsDrawScreen = bIsDrawScreen;
	//发送同步信息
	g_pWhiteBoardView->SendScreenDraw();

	m_bFirstLoginConference = FALSE; //add by jiangchao 2013.06.08
}

void CWhiteBoardView::RemoveAllObj()
{
	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + sizeof(unsigned long);
	m_IsGetAllObj = true;
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		int iCmd = CMD_DEL_ALL_ATTRIBUTE;
		memset(pData,0,nLen);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		memcpy(pData + sizeof(int),(char*)&g_ulSessionID,sizeof(unsigned long));
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
	}

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete pData;
		pData = NULL;
	}

	if(!m_IsGetAllObj) //如果不是在请求所有对象时，就发送额外的消息，供对方客服端更新用
	{
		m_IsGetAllObj = FALSE;
	}
}

void CWhiteBoardView::RemovePageAllObj(int nDelPageId)
{
	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + sizeof(int) + sizeof(unsigned long);
	unsigned long ulEventID = GenerateSSRC(); //事件ID
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];

	int iCmd = CMD_DEL_PAGE_ALL_ATTRIBUTE;
	memset(pData,0,nLen);
	memcpy(pData,(char*)&iCmd,sizeof(int));
	memcpy(pData + sizeof(int),(char*)&nDelPageId,sizeof(int));
	//memcpy(pData + sizeof(int) + sizeof(int),(char*)&ulEventID,sizeof(unsigned long));
	memcpy(pData + sizeof(int) + sizeof(int),(char*)&m_ulSendEventID,sizeof(unsigned long));
	pData[nLen] = '\0';
	outPacket.WriteData((unsigned char*)pData,nLen+1);
	

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}

void CWhiteBoardView::Broadcast_Data( XMemIni xmlObj )
{
	if (g_pHPWBDataNotify == NULL)  return;
	xmlObj.DeleteKey(KEY_FILE_TYPE);	
	int nBroadCastCmd = _E_ROADCASTCMD_NONE;
	if (!xmlObj.Get(KEY_BROADCAST_CMD,nBroadCastCmd))
	{
		xmlObj.Set(KEY_BROADCAST_CMD,nBroadCastCmd);
	}
	unsigned long ulSend = 0;
	if (!xmlObj.Get(KEY_FILE_SENDER,ulSend))
	{
		xmlObj.Set(KEY_FILE_SENDER,g_ulSessionID);
	}
	int nPageIndex =  0;
	if(!xmlObj.Get(KEY_OBJ_PAGE,nPageIndex))
	{
		nPageIndex = (int)g_pWBFile->GetCurPageIndex();
		xmlObj.Set(KEY_OBJ_PAGE, nPageIndex); //填写页码
	}
	unsigned long ulEventId = 0;
	if (!xmlObj.Get(KEY_EVENT_ID, ulEventId))
	{
		xmlObj.Set(KEY_EVENT_ID,GenerateSSRC());
	}
	//广播操作默认的限制互动方操作该对象
	int nbObjLimit =  -1;
	if(!xmlObj.Get(KEY_OBJ_LIMIT,nbObjLimit) && nbObjLimit == -1)
	{
		nbObjLimit = 1;
		xmlObj.Set(KEY_OBJ_LIMIT,nbObjLimit);
	}

	std::string strValue = "";
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		
		int iCmd = CMD_BROADCAST_DATA;
		memset(pData,0,nLen);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
		
		SendPacketInfoOnlyToNet((char*)outPacket.GetData(), outPacket.GetLength());
		if (pData)
		{
			delete []pData;
			pData = NULL;
		}
	}
}

void CWhiteBoardView::AddObj(XMemIni xmlObj)
{
	if (g_pHPWBDataNotify == NULL) return ;

	std::string strValue = "";
	std::string strPath = "";
	if (!xmlObj.Get(KEY_FILE_PATH, strPath) || strPath == "")
	{
		xmlObj.Set(KEY_FILE_SIZE,0);
		xmlObj.Set(KEY_FILE_POS,0);
		xmlObj.Set(KEY_FILE_TYPE,"");
		xmlObj.Set(KEY_FILE_PATH,"");
	}
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	unsigned long ulSender = 0;
	if (!xmlObj.Get(KEY_MEDIA_ADDSESSION,ulSender))
	{
		xmlObj.Set(KEY_MEDIA_ADDSESSION, g_ulSessionID);
	}
	if (!xmlObj.Get(KEY_CAREMA_ADDSESSION,ulSender))
	{
		xmlObj.Set(KEY_CAREMA_ADDSESSION, g_ulSessionID);
	}

	//填写事件ID
	unsigned long ulEventID;
	if (!xmlObj.Get(KEY_EVENT_ID, ulEventID))
	{
		ulEventID = GenerateSSRC();
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
	}

	int nPageIndex;
	if (!xmlObj.Get(KEY_OBJ_PAGE, nPageIndex)) //如果没有填写页码，则填写当前页的页码
	{
		nPageIndex = (int)g_pWBFile->GetCurPageIndex();
		xmlObj.Set(KEY_OBJ_PAGE, nPageIndex); //填写页码
	}	
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		
		int iCmd = CMD_ADD_ATTRIBUTE;
		memset(pData,0,nLen);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
		

		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

		if (pData)
		{
			delete []pData;
			pData = NULL;
		}
	}
}

void CWhiteBoardView::RemoveObj(DWORD dwObjLayerIndex)
{
	//这里暂时不删，以后可能会用这种方式发送删除对象
	/*	XMemIni xmlObj;
	std::string strValue = "";

	xmlObj.Set(KEY_OBJ_ID, dwObjLayerIndex);
	xmlObj.Set(KEY_OBJ_TYPE, 0);
	xmlObj.Set(KEY_FILE_SIZE,0);
	xmlObj.Set(KEY_FILE_POS,0);
	xmlObj.Set(KEY_FILE_TYPE,"");
	xmlObj.Set(KEY_FILE_PATH,"");
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);

	int nPageIndex = (int)g_pWBFile->GetCurPageIndex();
	xmlObj.Set(KEY_OBJ_PAGE, nPageIndex); //填写页码
	xmlObj.GetString(strValue);

	if (strValue != "")
	{
	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + strValue.size();
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
	int iCmd = CMD_DEL_ATTRIBUTE;
	memset(pData,0,nLen);
	memcpy(pData,(char*)&iCmd,sizeof(int));
	memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
	pData[nLen] = '\0';
	outPacket.WriteData((unsigned char*)pData,nLen+1);
	}

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	//测试
	//ProcessPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
	delete pData;
	pData = NULL;
	}
	}*/

	unsigned int nLayerIndex = (unsigned int)dwObjLayerIndex;

	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + sizeof(int) + sizeof(long) + sizeof(long) +sizeof(int);
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	
	int iCmd = CMD_DEL_ATTRIBUTE;
	int iCD = 8;//wangxiaobin
	memset(pData,0,nLen);
	memcpy(pData,(char*)&iCmd,sizeof(int));
	//memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
	memcpy(pData + sizeof(int), (char *)&nLayerIndex, sizeof(int)); //对象ID
	memcpy(pData + sizeof(int) + sizeof(int), (char *)&m_ulSendEventID, sizeof(long)); //事件ID
	memcpy(pData + sizeof(int) + sizeof(int) + sizeof(long),(char*)&g_ulSessionID, sizeof(long));
	memcpy(pData + sizeof(int) + sizeof(int) + sizeof(long) +sizeof(long),(char*)&iCD, sizeof(int));
	pData[nLen] = '\0';
	outPacket.WriteData((unsigned char*)pData,nLen+1);
	

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}
void CWhiteBoardView::RemoveDelCombineObj( DWORD dwObjLayerIndex )
{
	unsigned int nLayerIndex = (unsigned int)dwObjLayerIndex;

	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + sizeof(int) + sizeof(long) + sizeof(long) +sizeof(int);
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];

	int iCmd = CMD_DEL_ATTRIBUTE;
	int iCD = COMBINE_DELETE;//wangxiaobin
	memset(pData,0,nLen);
	memcpy(pData,(char*)&iCmd,sizeof(int));
	//memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
	memcpy(pData + sizeof(int), (char *)&nLayerIndex, sizeof(int)); //对象ID
	memcpy(pData + sizeof(int) + sizeof(int), (char *)&m_ulSendEventID, sizeof(long)); //事件ID
	memcpy(pData + sizeof(int) + sizeof(int) + sizeof(long),(char*)&g_ulSessionID, sizeof(long));
	memcpy(pData + sizeof(int) + sizeof(int) + sizeof(long) +sizeof(long),(char*)&iCD, sizeof(int));
	pData[nLen] = '\0';
	outPacket.WriteData((unsigned char*)pData,nLen+1);


	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}
void CWhiteBoardView::RemoveMediaObj(DWORD dwObjLayerIndex)
{
	unsigned int nLayerIndex = (unsigned int)dwObjLayerIndex;

	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + sizeof(int) + sizeof(long) + sizeof(long) +sizeof(int);
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		int iCmd = CMD_DEL_ATTRIBUTE;
		int iCD = CMD_MEDIA_USER_LINEOUT;
		memset(pData,0,nLen);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		memcpy(pData + sizeof(int), (char *)&nLayerIndex, sizeof(int)); //对象ID
		memcpy(pData + sizeof(int) + sizeof(int), (char *)&m_ulSendEventID, sizeof(long)); //事件ID
		memcpy(pData + sizeof(int) + sizeof(int) + sizeof(long),(char*)&g_ulSessionID, sizeof(long));
		memcpy(pData + sizeof(int) + sizeof(int) + sizeof(long) +sizeof(long),(char*)&iCD, sizeof(int));
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
	}

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}


void CWhiteBoardView::RemoveDelComCurve( DWORD dwObjLayerIndex )
{
	unsigned int nLayerIndex = (unsigned int)dwObjLayerIndex;

	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + sizeof(int) + sizeof(long) + sizeof(long) +sizeof(int);
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		int iCmd = CMD_DEL_ATTRIBUTE;
		int iCD = COMCURVE_DELETE;
		memset(pData,0,nLen);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		memcpy(pData + sizeof(int), (char *)&nLayerIndex, sizeof(int)); //对象ID
		memcpy(pData + sizeof(int) + sizeof(int), (char *)&m_ulSendEventID, sizeof(long)); //事件ID
		memcpy(pData + sizeof(int) + sizeof(int) + sizeof(long),(char*)&g_ulSessionID, sizeof(long));
		memcpy(pData + sizeof(int) + sizeof(int) + sizeof(long) +sizeof(long),(char*)&iCD, sizeof(int));
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
	}

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}


void CWhiteBoardView::ChangeObj(XMemIni xmlObj, bool bchange /*= false*/ )
{

	if (g_pHPWBDataNotify == NULL) return ;

	std::string strValue = "";
	xmlObj.DeleteKey(KEY_FILE_SIZE);
	xmlObj.DeleteKey(KEY_FILE_POS);
	xmlObj.DeleteKey(KEY_FILE_TYPE);	
	// 	std::string strPoints = "";
	// 	if (!xmlObj.Get(KEY_MOVE_OFFSET,strPoints))
	// 	{
	// 		xmlObj.Set(KEY_MOVE_OFFSET,"");
	// 	}
	unsigned long ulEventID;
	if (!xmlObj.Get(KEY_EVENT_ID, ulEventID))
	{
		ulEventID = GenerateSSRC();
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
	}
	int nPageIndex =  0;
	if(!xmlObj.Get(KEY_OBJ_PAGE,nPageIndex))
	{
		nPageIndex = (int)g_pWBFile->GetCurPageIndex();
		xmlObj.Set(KEY_OBJ_PAGE, nPageIndex); //填写页码
	}

	unsigned long ulSenderID = 0;
	if (!xmlObj.Get(KEY_FILE_SENDER,ulSenderID))
	{
		ulSenderID = g_ulSessionID;
		xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	}
	if (ulSenderID == g_ulSessionID)
	{
		xmlObj.Set(KEY_OWEN_SEND_OBJ, CMD_OWEN_SEND_OBJ);
	}

	int nWidth, nHeight;
	if (!xmlObj.Get(KEY_WHITEBOARD_WIDTH, nWidth))
	{
		xmlObj.Set(KEY_WHITEBOARD_WIDTH, g_nWidth);
	}
	if (!xmlObj.Get(KEY_WHITEBOARD_HEIGHT, nHeight))
	{
		xmlObj.Set(KEY_WHITEBOARD_HEIGHT, g_nHeight);
	}

	//如果是组合，同步过程不刷新，组合对象发送前刷新，都发送完毕之后再刷新
	int nType = 0;
	xmlObj.Get(KEY_OBJ_TYPE, nType);

	WB_OBJ_TYPE objType  = (WB_OBJ_TYPE)nType;
	if (objType == WB_OBJ_COMBINE)
	{
		if (bchange)
		{
			m_ulComEventID = ulEventID;
		}
		m_nComPageIndex = nPageIndex;
		m_ulComSenderID = ulSenderID;
		//xmlObj.Set(KEY_EVENT_OBJNUMBER, 2);
		xmlObj.Set(KEY_EVENT_ID,m_ulComEventID);
		xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	}
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_CHANGE_ATTRIBUTE;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}

		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

		if (pData)
		{
			delete []pData;
			pData = NULL;
		}
	}

	DWORD dwObjLayerIndex = 0;
	xmlObj.Get(KEY_OBJ_ID, dwObjLayerIndex);
	if (objType == WB_OBJ_COMBINE)
	{
		CWBObject *pObj = (g_pWBFile->GetPage(nPageIndex))->FindByLayerIndex(dwObjLayerIndex, objType);
		if (pObj != NULL)
		{
			ChangeComObj(pObj);
		}
	}
}

void CWhiteBoardView::ChangeComObj(CWBObject *pObj)
{
	if (g_pHPWBDataNotify == NULL)  return;
	XMemIni xmlComObj;
	for(int i = 0; i < (int)((((CWBCombine *)pObj)->GetCombineList()).size()); ++i)
	{
		//填写事件ID
		xmlComObj.Set(KEY_EVENT_ID, m_ulComEventID);
		xmlComObj.Set(KEY_OBJ_PAGE, m_nComPageIndex);
		xmlComObj.Set(KEY_FILE_SENDER, m_ulComSenderID);

		(((CWBCombine *)pObj)->GetCombineList())[i]->SetXmlValue(xmlComObj);
		xmlComObj.Set(KEY_EVENT_OBJNUMBER, 2);

		std::string strValue = "";
		xmlComObj.GetString(strValue);

		if (strValue != "")
		{
			AVDataOutPacket outPacket;
			int nLen = sizeof(int) + strValue.size();
			outPacket<<(UINT)WB_NEW_COMMAND_DATA;
			outPacket<<(unsigned int)nLen;
			char* pData = new char[nLen+1];
			if (pData)
			{
				int iCmd = CMD_CHANGE_ATTRIBUTE;
				memset(pData,0,nLen);
				memcpy(pData,(char*)&iCmd,sizeof(int));
				memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
				pData[nLen] = '\0';
				outPacket.WriteData((unsigned char*)pData,nLen+1);
			}

			SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

			if (pData)
			{
				delete []pData;
				pData = NULL;
			}
		}

		int nType = 0;
		DWORD dwObjLayerIndex = 0;
		xmlComObj.Get(KEY_OBJ_TYPE, nType);
		xmlComObj.Get(KEY_OBJ_ID, dwObjLayerIndex);

		WB_OBJ_TYPE objType = (WB_OBJ_TYPE)nType;
		if (objType == WB_OBJ_COMBINE)
		{
			CWBObject *pComObj = (g_pWBFile->GetCurPage())->FindComObjByLayerIndex(dwObjLayerIndex);
			if (pComObj != NULL)
			{
				ChangeComObj(pComObj);
			}
		}
	}
}

void CWhiteBoardView::Undo()
{
	AVDataOutPacket outPacket;
	int nLen = sizeof(int);
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		int iCmd = CMD_UNDO_STATUS;
		memset(pData,0,nLen+1);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen);
	}
	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}

void CWhiteBoardView::Redo()
{
	AVDataOutPacket outPacket;
	int nLen = sizeof(int);
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		int iCmd = CMD_REDO_STATUS;
		memset(pData,0,nLen+1);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen);
	}
	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}

void CWhiteBoardView::SendLocalMessage(eCommend_Type eComType,BOOL bFullScreen /* = TRUE */)
{
	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + sizeof(BOOL);
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		memset(pData,0,nLen+1);
		memcpy(pData,(char*)&eComType,sizeof(int));
		memcpy(pData+sizeof(int), (char *)&bFullScreen, sizeof(BOOL));
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen);
	}
	SendPacketInfoOnlyToNet((char*)outPacket.GetData(), outPacket.GetLength(), false);
	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}
void CWhiteBoardView::SendDropNotifyU7Message(HPMC_DROP *pBuffer,bool bflag /*= true*/)
{
	AVDataOutPacket outPacket;
	int		Type;	 		
	HWND    hWnd;    		
	int		AreaID;	 		
	unsigned long	ChannelID;	
	std::string strpath = "";
	Type = pBuffer->Type;
	hWnd = pBuffer->hWnd;
	AreaID = pBuffer->AreaID;
	strpath = pBuffer->MemberID;
	ChannelID = pBuffer->ChannelID;
	int iCmd = CMD_DROP_NOTIFY;

	int xxlen = strpath.length();

	int nLen = sizeof(int) + sizeof(int) + sizeof(unsigned long) + sizeof(int) + sizeof(unsigned long) + xxlen;
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		memset(pData,0,nLen+1);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		memcpy(pData+sizeof(int), (char *)&Type, sizeof(int));
		memcpy(pData+sizeof(int) +sizeof(int), (char *)&hWnd, sizeof(unsigned long));
		memcpy(pData+sizeof(int) +sizeof(int) + sizeof(unsigned long), (char *)&AreaID, sizeof(int));
		memcpy(pData+sizeof(int) +sizeof(int) + sizeof(unsigned long) +sizeof(int), (char *)&ChannelID, sizeof(unsigned long));
		memcpy(pData+sizeof(int) +sizeof(int) + sizeof(unsigned long) +sizeof(int) +sizeof(unsigned long),strpath.c_str(), xxlen);

		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen);
	}
	SendPacketInfoOnlyToNet((char*)outPacket.GetData(), outPacket.GetLength(), bflag);
	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}
void CWhiteBoardView::SendDropNotify( HPMC_DROP *pBuffer )
{
	HPMC_DROP *pData= pBuffer;
	if (pData == NULL)
	{
		return;
	}
	int		Type;	 		
	HWND    hWnd;    		
	int		AreaID;	 		
	unsigned long	ChannelID;	
	CString StrPathTemp1 = _T(""),StrPathTemp2 = _T("");
	Type = pData->Type;
	hWnd = pData->hWnd;
	AreaID = pData->AreaID;
	StrPathTemp1 = pData->MemberID;
	ChannelID = pData->ChannelID;
	int iCmd = CMD_DROP_NOTIFY;

	StrPathTemp2.Format(L"%d|%d|%d|%d|%s|%d",iCmd,Type,hWnd,AreaID,StrPathTemp1,ChannelID);
	int nlen = StrPathTemp2.GetLength();
	char *szTemp = new char[nlen +1];
	memset( szTemp, 0, nlen +1);
	strcpy( szTemp, ToStr(StrPathTemp2.GetBuffer(nlen)).c_str());
	StrPathTemp2.ReleaseBuffer();

	if (szTemp == NULL || nlen <= 0)
	{
		return;
	}
	try
	{
		if (g_pHPWBDataNotify != NULL  )
		{
			((IHPNetNotify *)g_pHPWBDataNotify)->SendWbData(szTemp, nlen, false);
		}
	}
	catch (...)
	{
		TRACE(TEXT("catch void HPDTS::processDTSData \n"));
	}
	return;

}

void CWhiteBoardView::ChangeFullScreen(BOOL bFullScreen /* = TRUE */)
{
	XMemIni xmlObj;
	xmlObj.Set(KEY_OBJ_ID, 0);
	xmlObj.Set(KEY_OBJ_TYPE, 0);
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	xmlObj.Set(KEY_FULLSCREEN, bFullScreen);
	ChangeObj(xmlObj);
}

void CWhiteBoardView::SendChangePage()
{
	XMemIni xmlObj;
	xmlObj.Set(KEY_OBJ_ID, 0);
	xmlObj.Set(KEY_OBJ_TYPE, 0);
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	xmlObj.Set(KEY_PAGEINDEX, (int)(g_pWBFile->GetCurPageIndex()));
	ChangeObj(xmlObj);
}
void CWhiteBoardView::SendScrollDraw()
{
	XMemIni xmlObj;
	xmlObj.Set(KEY_OBJ_ID, 0);
	xmlObj.Set(KEY_OBJ_TYPE, 0);
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	float fScrollScaleX = (float)(g_pWBPage->GetPageViewScroll().x);
	float fScrollScaleY = (float)(g_pWBPage->GetPageViewScroll().y);
	fScrollScaleX = fScrollScaleX/m_fWbRatioX;
	fScrollScaleY = fScrollScaleY/m_fWbRatioY;
	xmlObj.Set(KEY_PAGEINDEX, (int)(g_pWBFile->GetCurPageIndex()));
	xmlObj.Set(KEY_PAGE_SCROLL_XY, g_pWBPage->GetPageViewScroll().x);
	xmlObj.Set(KEY_PAGE_SCROLL_YX, g_pWBPage->GetPageViewScroll().y);
	xmlObj.Set(KEY_PAGE_SCROLL_X, fScrollScaleX);
	xmlObj.Set(KEY_PAGE_SCROLL_Y, fScrollScaleY);
	xmlObj.Set(KEY_MOVE_EAGEL_EYE,_E_MOVE_EAGEL_EYE);//鹰眼撤销操作命令
	ChangeObj(xmlObj);


	if (m_WBSpotlight && m_WBSpotlight->GetSpotlightDlg()
		&& m_WBSpotlight->GetSpotlightDlg()->IsWindowVisible())
	{
		XMemIni xmlobj;
		m_WBSpotlight->SetXmlValue(xmlobj);
		ChangeObj(xmlobj);
	}
	//这里是换页的时候把当前页的所有对象发送到录播里面，是为了中途打开录播的时候才把当前页的内容发送出去。
	if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord)
	{
		SendMessage(WM_SENDPAGEDATATO_CONFERENCE_RECORD,0,0);
	}
}
void CWhiteBoardView::SendDeletePage(XMemIni xmlObj)
{
	xmlObj.Set(KEY_OBJ_ID, 0);
	xmlObj.Set(KEY_OBJ_TYPE, 0);
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	std::string strValue = "";
	xmlObj.DeleteKey(KEY_FILE_SIZE);
	xmlObj.DeleteKey(KEY_FILE_POS);
	xmlObj.DeleteKey(KEY_FILE_TYPE);	
	xmlObj.DeleteKey(KEY_FILE_PATH);
	unsigned long ulEventID;
	if (!xmlObj.Get(KEY_EVENT_ID, ulEventID))
	{
		ulEventID = GenerateSSRC();
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
	}	
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_DEL_PAGE;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}

		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

		//测试

		if (pData)
		{
			delete []pData;
			pData = NULL;
		}
	}
}

void CWhiteBoardView::SendInsertPage(XMemIni xmlpage)
{
	xmlpage.Set(KEY_OBJ_ID, 0);
	xmlpage.Set(KEY_FILE_SENDER, g_ulSessionID);
	xmlpage.Set(KEY_PAGEINDEX, (int)(g_pWBFile->GetCurPageIndex()));
	std::string strValue = "";
	xmlpage.DeleteKey(KEY_FILE_SIZE);
	xmlpage.DeleteKey(KEY_FILE_POS);
	xmlpage.DeleteKey(KEY_FILE_TYPE);	
	xmlpage.DeleteKey(KEY_FILE_PATH);
	unsigned long ulEventID;
	if (!xmlpage.Get(KEY_EVENT_ID, ulEventID))
	{
		ulEventID = GenerateSSRC();
		xmlpage.Set(KEY_EVENT_ID, ulEventID);
	}	
	xmlpage.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_INSERT_PAGE;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}

		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());
		if (pData)
		{
			delete[] pData;
			pData = NULL;
		}
	} 
}

void CWhiteBoardView::SendExchangePage(XMemIni xmlobj)
{
	XMemIni xmlpage = xmlobj;
	xmlpage.Set(KEY_OBJ_ID, 0);
	xmlpage.Set(KEY_OBJ_TYPE, 0);
	xmlpage.Set(KEY_FILE_SENDER, g_ulSessionID);
	xmlpage.Set(KEY_PAGEINDEX, (int)(g_pWBFile->GetCurPageIndex()));
	// 	xmlpage.Set(KEY_PAGE_SCROLL_X, g_pWBPage->GetPageViewScroll().x);
	// 	xmlpage.Set(KEY_PAGE_SCROLL_Y, g_pWBPage->GetPageViewScroll().y);
	float fScrollScaleX = (float)(g_pWBPage->GetPageViewScroll().x);
	float fScrollScaleY = (float)(g_pWBPage->GetPageViewScroll().y);
	fScrollScaleX = fScrollScaleX/m_fWbRatioX;
	fScrollScaleY = fScrollScaleY/m_fWbRatioY;
	xmlpage.Set(KEY_PAGE_SCROLL_X, fScrollScaleX);
	xmlpage.Set(KEY_PAGE_SCROLL_Y, fScrollScaleY);
	std::string strValue = "";
	xmlpage.DeleteKey(KEY_FILE_SIZE);
	xmlpage.DeleteKey(KEY_FILE_POS);
	xmlpage.DeleteKey(KEY_FILE_TYPE);	
	xmlpage.DeleteKey(KEY_FILE_PATH);
	unsigned long ulEventID;
	if (!xmlpage.Get(KEY_EVENT_ID, ulEventID))
	{
		ulEventID = GenerateSSRC();
		xmlpage.Set(KEY_EVENT_ID, ulEventID);
	}	
	xmlpage.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_CHANGE_PAGE;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}

		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

		//测试

		//ProcessPacketData((char*)outPacket.GetData(), outPacket.GetLength());

		if (pData)
		{
			delete[] pData;
			pData = NULL;
		}
	} 
}
void CWhiteBoardView::ProcessAttiObj(XMemIni xmlObj)
{
	int nFullScreen;
	int nPageIndex;

	BOOL bIsDrawScreen;

	if (xmlObj.Get(KEY_FULLSCREEN, nFullScreen)) //改变全屏方式
	{
		{
			BOOL bFullScreen = nFullScreen ? TRUE : FALSE;
			((CWhiteBoardDlg *)g_whiteboardDlg)->GetToolbarDlg()->SetIsFullScreen(bFullScreen);
			if ((CScreenDrawMgr::GetIns()))//全屏状态发生变化时且打开了桌面绘图,则关闭桌面绘图，在CWhiteBoardDlg::OnSize中打开
			{
				((CWhiteBoardDlg *)g_whiteboardDlg)->SetIsOpenScreenDraw(true);
				WB_DestroyWnd();				
			}
			g_pWhiteBoardView->SendMessage(WM_CANCELMODE, 0, 0);
			ProcessCloseColorDlg();
			if (CWBToolTip::GetIns())
			{
				CWBToolTip::GetIns()->Restore();
			}
			
			if (g_bSupportU7OpenFile == FALSE)
			{//在U7中被动接收方全屏，是通过U7进行，而在A6中被动接收方全屏还是需要白板传给A6来完成。
				SendLocalMessage(CMD_SCREEN,bFullScreen);
				DeleteWaterCursorForAvcon6();
			}
		}
		((CWhiteBoardDlg *)g_whiteboardDlg)->GetToolbarDlg()->CloseAllChildBox();
	}

	if (xmlObj.Get(KEY_PAGEINDEX, nPageIndex)) //翻页
	{
		if (nPageIndex <= 0)
		{
			return;
		}
		g_pWhiteBoardView->SendMessage(WM_CANCELMODE, 0, 0);
		ProcessChangePage(nPageIndex);	
		//删除正在操作的触摸对象
		CWBMTObjectManager::GetIns_()->RemoveAllMTObject();

		int nScrollX = 0, nScrollY = 0;
		float fnScrollRatioX = 0.0,fScrollYRatioY = 0.0;
		float fnScrX,fScroY;
		if (xmlObj.Get(KEY_PAGE_SCROLL_X, fnScrollRatioX) //页滚动同步
			&& xmlObj.Get(KEY_PAGE_SCROLL_Y, fScrollYRatioY)
			&& xmlObj.Get(KEY_PAGE_SCROLL_XY,fnScrX)
			&& xmlObj.Get(KEY_PAGE_SCROLL_YX,fScroY))
		{
			nScrollX = (int)(fnScrollRatioX * m_fWbRatioX);
			nScrollY = (int)(fScrollYRatioY * m_fWbRatioY);
			unsigned long ulSend = 0;
			if (!((xmlObj.Get(KEY_FILE_SENDER,ulSend))
				&&g_ulSessionID == ulSend))
			{
				CPoint pt;
				pt.x = fnScrX - nScrollX;
				pt.y = fScroY - nScrollY;
				g_ptAddPoint = pt;
			}
			g_pWBPage->SetPageViewScroll(CPoint(nScrollX, nScrollY));
			setCurrentViewScroll(CPoint(nScrollX, nScrollY));	
			
		}
		m_viewIsNeedDraw = TRUE; //被动移动页面时，页面有残影需要将view的刷新打开

		if (g_pWhiteBoardView->m_hWnd == NULL)
			return;
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL)
			CScreenDrawMgr::GetIns()->Invalidate();
	}

	if (xmlObj.Get(KEY_SCREENDRAW, bIsDrawScreen))
	{
		XMemIni *pDrawScreenXmlobj = new XMemIni();
		*pDrawScreenXmlobj = xmlObj;
		SendMessage(WB_SCREENDRAW,(WPARAM)pDrawScreenXmlobj,NULL);//发送消息，通知更新
		m_viewIsNeedDraw = TRUE; //被动移动页面时，页面有残影需要将view的刷新打开

		if (g_pWhiteBoardView->m_hWnd == NULL)
			return;
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL)
			CScreenDrawMgr::GetIns()->Invalidate();
	}
	m_viewIsNeedDraw = FALSE;//被动移动页面时，页面有残影需要将view的刷新打开，出去的时候设为false
	return;
}



void CWhiteBoardView::ChangeObjLayer(XMemIni xmlObj)
{
	std::string strValue = "";
	std::string strPath = "";
	if (!xmlObj.Get(KEY_FILE_PATH, strPath) || strPath == "")
	{
		xmlObj.Set(KEY_FILE_SIZE,0);
		xmlObj.Set(KEY_FILE_POS,0);
		xmlObj.Set(KEY_FILE_TYPE,"");
		xmlObj.Set(KEY_FILE_PATH,"");
	}
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);

	//填写事件ID
	unsigned long ulEventID;
	if (!xmlObj.Get(KEY_EVENT_ID, ulEventID))
	{
		ulEventID = GenerateSSRC();
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
	}

	int nPageIndex = (int)g_pWBFile->GetCurPageIndex();
	xmlObj.Set(KEY_OBJ_PAGE, nPageIndex); //填写页码
	xmlObj.GetString(strValue);
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA; 
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_CHANGE_ATTRIBUTE; //待定			
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}

		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

		//测试
		//ProcessPacketData((char*)outPacket.GetData(), outPacket.GetLength());

		if (pData)
		{
			delete [] pData;
			pData = NULL;
		}
	}
}
//1-16jin添加
void CWhiteBoardView::setFrameListAndObjLayerIndex()
{
	SetPageIsNeedDraw(TRUE);
	g_pWBPage->ClearSelectFrameList();
	m_dwCurSelectObjLayerIndex = 0;
}
// 01-19 wangxin 处理手势修改的对象
// void CWhiteBoardView::ProcessGesture(CUIntArray &aLayerBox)
// {
// 	for (int i = 0; i < aLayerBox.GetSize(); ++i)
// 	{
// 		if (aLayerBox[i] != 0)
// 		{
// 			g_pWBPage->AddSelectFrame((DWORD)aLayerBox.GetAt(i));
// 			CWBObject *pObj = g_pWBPage->FindByLayerIndex((DWORD)aLayerBox.GetAt(i));
// 			if (pObj != NULL)
// 			{
// 				XMemIni xmlObj;
// 				pObj->SetXmlValue(xmlObj);
// 				ChangeObj(xmlObj);
// 			}			
// 		}
// 	}
// }

void CWhiteBoardView::ProcessChangePage(int nPageIndex)
{
	//如果总页数小于对象页码，则添加新页
	UINT nCurPage = g_pWBFile->GetCurPageIndex();
	if (nPageIndex == nCurPage)
	{
		return;
	}
	m_nPrePage = nCurPage;

	if (nPageIndex != g_pWBFile->GetCurPageIndex())
	{	
		if (g_pWBFile->GotoPage(nPageIndex))
		{
			if (CWBToolTip::GetIns())
			{
				CWBToolTip::GetIns()->Restore();
			}
			m_pPage = g_pWBPage;
			SetFlashCurPage(FALSE);
			g_pWBPage->StartOrPauseMovie(nCurPage); 
			g_pWBPage = g_pWBFile->GetCurPage(); 
			SetFlashCurPage(TRUE);
			g_pWBPage->StartOrPauseMovie(nPageIndex); 
			POINT point = g_pWBPage->GetPageViewScroll();
			setCurrentViewScroll(point);
			//Invalidate();
		}
	}
}
void CWhiteBoardView::ProcessDeletePage( XMemIni xmlObj, int nCmdType /*= CMD_DEL_PAGE*/ )
{
	unsigned long ulsender = 0;
	if (xmlObj.Get(KEY_FILE_SENDER,ulsender) && ulsender ==g_ulSessionID)
	{
		return;
	}
	DWORD dwObjLayerIndex = 0; 
	int nPageIndex = 0;
	unsigned long ulOldPage = 0;
	/*	xmlObj.Get(KEY_PAGEINDEX,nPageIndex);*/
	xmlObj.Get(KEY_OBJ_PAGE,ulOldPage);
	// 	if (nPageIndex <= 0 || ulOldPage <= 0)
	// 	{
	// 		return;
	// 	}
	if(ulOldPage <=0)
	{
		return;
	}
	if (ulOldPage <=g_pWBFile->GetPageCount())
	{
		DeletePrePage(ulOldPage);
	}
	if (nPageIndex != g_pWBFile->GetCurPageIndex())
	{	
		if (g_pWBFile->GotoPage(nPageIndex))
		{
			SetFlashCurPage(FALSE);
			g_pWBPage->StartOrPauseMovie();
			g_pWBPage = g_pWBFile->GetCurPage();
			SetFlashCurPage(TRUE);
			g_pWBPage->StartOrPauseMovie();
		}
	}

}

void CWhiteBoardView::ProcessInsertPage( XMemIni xmlObj, int nCmdType /*= CMD_INSERT_PAGE*/ )
{
	unsigned long ulsender = 0;
	if (xmlObj.Get(KEY_FILE_SENDER,ulsender) && ulsender ==g_ulSessionID)
	{
		return;
	}
	DWORD dwObjLayerIndex = 0; 
	int nPageIndex = 0;
	unsigned long ulEventID; //接收事件ID
	unsigned long ulOldPage = 0;
	xmlObj.Get(KEY_OBJ_PAGE,ulOldPage);
	xmlObj.Get(KEY_PAGEINDEX,nPageIndex);
	// 	if (nPageIndex <= 0 || ulOldPage <= 0)
	// 	{
	// 		return;
	// 	}
	if (nPageIndex <=g_pWBFile->GetPageCount())
	{
		InsertPrePage(ulOldPage);
	}
	if (nPageIndex != g_pWBFile->GetCurPageIndex())
	{	
		if (g_pWBFile->GotoPage(nPageIndex))
		{
			SetFlashCurPage(FALSE);
			g_pWBPage->StartOrPauseMovie();
			g_pWBPage = g_pWBFile->GetCurPage();
			SetFlashCurPage(TRUE);
			g_pWBPage->StartOrPauseMovie();
		}
	}
	if (xmlObj.Get(KEY_EVENT_ID,ulEventID)&&(ulEventID == CMD_DEL_PAGE))//插入页完成，是为了undo redo用的 只有插入页完成才接受undo redo过来的对象
	{
		m_IsInsertPageOver = true;
	}
}

void CWhiteBoardView::ProcessExchangPage( XMemIni xmlObj, int nCmdType /*= CMD_CHANGE_PAGE*/ )
{
	unsigned long ulsender = 0;
	if (xmlObj.Get(KEY_FILE_SENDER,ulsender) && ulsender ==g_ulSessionID)
	{
		return;
	}
	DWORD dwObjLayerIndex = 0; 
	int nPageIndex = 0;
	unsigned long ulOldPage = 0;
	xmlObj.Get(KEY_OBJ_PAGE,ulOldPage);
	xmlObj.Get(KEY_PAGEINDEX,nPageIndex);
	if (nPageIndex <= 0 || ulOldPage <= 0)
	{
		return;
	}
	if (nPageIndex <=g_pWBFile->GetPageCount())
	{
		InterchangePage(ulOldPage,nPageIndex);
	}
	if (nPageIndex != g_pWBFile->GetCurPageIndex())
	{	
		if (g_pWBFile->GotoPage(nPageIndex))
		{
			SetFlashCurPage(FALSE);
			g_pWBPage->StartOrPauseMovie();
			g_pWBPage = g_pWBFile->GetCurPage();
			SetFlashCurPage(TRUE);
			g_pWBPage->StartOrPauseMovie();
		}
	}
}


void CWhiteBoardView::setViewScroll()
{
	CSize sizeTotal; 
	if(g_reduce>0.00000000000000000)
	{

		sizeTotal.cx = m_scrollX*g_reduce;  
		sizeTotal.cy = m_scrollY*g_reduce;

		if(!g_bSupportI7)
		{
			sizeTotal.cx = 0;
			sizeTotal.cy = 0;
		}
		SetScrollSizes(MM_TEXT, sizeTotal);

		m_XnPos = GetScrollPosition().x;
		m_YnPos = GetScrollPosition().y;
	}
	else
	{
		sizeTotal.cx = m_scrollX;  
		sizeTotal.cy = m_scrollY;

// 		if(!g_bSupportI7&&!g_bU7NewStyle)
// 		{
// 			sizeTotal.cx = 0;
// 			sizeTotal.cy = 0;
// 		}
		SetScrollSizes(MM_TEXT, sizeTotal);

		m_XnPos = GetScrollPosition().x;
		m_YnPos = GetScrollPosition().y;
	}
}

UINT CWhiteBoardView::getXnPos()
{
	m_XnPos = GetScrollPosition().x;	
	return m_XnPos;
}

UINT CWhiteBoardView::getYnPos()
{
	m_YnPos = GetScrollPosition().y;
	return m_YnPos;
}

void CWhiteBoardView::setCurrentViewScroll(POINT pt)
{
	ScrollToPosition(pt);
	if(g_pHPWBDataNotify &&g_bSupportI7)//I7中滚动条特殊处理
	{				
		MSG msg;
		memset(&msg,0,sizeof(MSG));
		msg.hwnd = NULL;
		msg.message = 2;
		msg.wParam = (WPARAM)(pt.x);
		msg.lParam = (LPARAM)(pt.y);
		g_pHPWBDataNotify->SetSelectObjStatue(msg);				
	}
}

CRect CWhiteBoardView::getWBViewRECT()
{
	GetClientRect(&m_clientRect);
	return m_clientRect;
}


BOOL CWhiteBoardView::DocConvertTip()
{
	if (m_bIsDocConvert) //文档还在转换过程中
	{
		CRect clientRC;
		((CWhiteBoardDlg*)GetParent())->GetClientRect(&clientRC);
		ClientToScreen(&clientRC);
		CDialogEx* pDialog = new CbuttonCloseDlg();
		pDialog->Create(IDD_DOCCONVERTTIP, this);
		pDialog->MoveWindow((clientRC.CenterPoint()).x - 120, (clientRC.CenterPoint()).y - 50, 257, 105, TRUE);
		pDialog->SetBackgroundImage(IDB_DOCCONVERTTIP);
		pDialog->ShowWindow(SW_SHOW);

		Sleep(3000);

		delete pDialog;
		pDialog = NULL;

		g_nDrawStatus = DRAW_NORMAL;
		return TRUE;
	}
	return FALSE;
}

void CWhiteBoardView::CreateFullScreen(CWBDocument *pWBDocument)
{
	m_pEstimate = false;
	if (m_pFullScreen == NULL || m_pFullScreen->m_hWnd == NULL)
	{
		m_pFullScreen = new CWBFullScreen;
		m_pFullScreen->Create(IDD_FULLSCREEN);		
	}
	
	m_pFullScreen->ShowWindow(SW_HIDE);

	
	
	//获取屏幕分辩率
	POINT pt;
	::GetCursorPos(&pt);
	MONITORINFO oMonitor = {}; 
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CRect clientRC(oMonitor.rcWork.left, oMonitor.rcWork.top, oMonitor.rcWork.right, oMonitor.rcWork.bottom);

	if (m_pDocTip == NULL || m_pDocTip->m_hWnd == NULL)
	{
		m_pDocTip = new CbuttonCloseDlg;
		m_pDocTip->Create(IDD_DOCCONVERTTIP, m_pFullScreen);
		if (g_bSupportU7OpenFile)
		{
			m_pDocTip->SetBackgroundImage(IDB_DOCTIP_EN);
		} 
		else
		{
			m_pDocTip->SetBackgroundImage(IDB_DOCTIP);
		}
	}
	
	m_pDocTip->MoveWindow((clientRC.CenterPoint()).x - 120, (clientRC.CenterPoint()).y - 50, 257, 105, TRUE);
	m_pDocTip->setWBDoc(pWBDocument);	
	m_pDocTip->ShowWindow(SW_HIDE);


}

void CWhiteBoardView::SetFullScreenPos()
{
	if (m_pEstimate == true)//当点击终止文件转换按钮的时候，不能让此函数继续运行，否则会中断
	{                       //m_pEstimate就是判断是否点击了终止文件转换按钮
		return;
	}
	if (m_pFullScreen != NULL && m_pFullScreen->m_hWnd != NULL)
	{
		m_pFullScreen->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	}
	if (m_pDocTip != NULL && m_pDocTip->m_hWnd != NULL)
	{
		m_pDocTip->ShowWindow(SW_SHOW);
	}
}

void CWhiteBoardView::HideFullScreen()
{
	if (m_pEstimate == true)//当点击终止文件转换按钮的时候，不能让此函数继续运行，否则会中断
	{                       //m_pEstimate就是判断是否点击了终止文件转换按钮
		return;
	}
	if (m_pFullScreen != NULL &&m_pFullScreen->m_hWnd != NULL)
	{
		m_pFullScreen->ShowWindow(SW_HIDE);
	}
	if (m_pDocTip != NULL && m_pDocTip->m_hWnd != NULL)
	{
		m_pDocTip->ShowWindow(SW_HIDE);
	}
}

void CWhiteBoardView::DestroyFullScreen()
{
	HideFullScreen();
	m_pEstimate = true;
	
	/*if (m_pFullScreen != NULL)
	{
		
		delete m_pFullScreen;
		m_pFullScreen = NULL;
	}
	if (m_pDocTip != NULL)
	{
		delete m_pDocTip;
		m_pDocTip = NULL;
	}*/
	Invalidate();
}

BOOL CWhiteBoardView::addBaffle()
{
	GetClientRect(&m_rect);
	CPoint ptLeftTop,ptRightBottom;
	ptLeftTop.x = m_rect.left + getXnPos();
	ptLeftTop.y = m_rect.top + getYnPos();
	ptRightBottom.x = m_rect.right + getXnPos();
	ptRightBottom.y = m_rect.bottom + getYnPos();
	CWBBafflePlate *pBafflePlate = new CWBBafflePlate(ptLeftTop,ptRightBottom,RGB(192,200,209));
	g_pWBPage->AddWBObject(pBafflePlate); //添加对象
	if (g_pWhiteBoardView->m_hWnd == NULL)
		return FALSE;
	Invalidate();
	//发送同步消息
	XMemIni xmlObj;
	pBafflePlate->SetXmlValue(xmlObj);
	AddObj(xmlObj);

	return TRUE;	
}

afx_msg LRESULT CWhiteBoardView::OnWbDeleteobj(WPARAM wParam, LPARAM lParam)
{
	CWBPage *pObjPage = NULL;
	CString csLayer_Sender((LPCTSTR)wParam);
	int nNumber = csLayer_Sender.Find(';');
	CString csLayerIndex = csLayer_Sender.Left(nNumber);
	CString csSendeId = csLayer_Sender.Right(csLayer_Sender.GetLength()-nNumber-1);

	DWORD dwLayerIndex = _tcstoul((LPCTSTR)csLayerIndex,NULL,0);
	DWORD dwSendeId = _tcstoul((LPCTSTR)csSendeId,NULL,0);
	unsigned long ulEventID = (unsigned long)lParam;
	
	//m_hEvent = CreateEvent(NULL, TRUE, TRUE, _T("CREATE EVENT FOR SYNCHONIZE"));
	//WaitForSingleObject(m_hEvent, INFINITE);
	//ResetEvent(m_hEvent);
	//删除放大镜
	if (NULL != m_pWBRectMaqn)
	{
		if (dwLayerIndex == m_pWBRectMaqn->GetLayerIndex())
		{
			delete m_pWBRectMaqn;
			m_pWBRectMaqn = NULL;

			if (IsWindow(m_hWnd))
			{
				Invalidate(TRUE);
			}
			m_ulRecEventID = ulEventID;
			return 1;
		}
	}
	//删除聚光灯
	if (NULL != m_WBSpotlight)
	{
		if (dwLayerIndex == m_WBSpotlight->GetLayerIndex())
		{
			delete m_WBSpotlight;
			m_WBSpotlight = NULL;
			if (IsWindow(m_hWnd))
			{
				Invalidate(TRUE);
			}
			/*Invalidate(TRUE);*/
			m_ulRecEventID = ulEventID;
			return 1;
		}
	}

	XAutoLock lock(m_csOnWbDeleteObjLock);
	SendMessage(WM_CANCELMODE,0,0);
	for (int i = 1; i <= (int)g_pWBFile->GetPageCount(); ++i )
	{
		pObjPage = g_pWBFile->GetPage(i);
		if (pObjPage == NULL)
		{
			continue;
		}

		CWBObject *pObj = pObjPage->FindByLayerIndex(dwLayerIndex);
		if (pObj == NULL) //如果没在对象列表里找到，就到组合列表里找
		{
			pObj = pObjPage->FindComObjByLayerIndex(dwLayerIndex);
		}
		if (m_pEditObj != NULL)
		{
			XMemIni xmlObj;
			m_pEditObj->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_EVENT_ID, ulEventID);
			AddObj(xmlObj);
			g_pWBPage->m_bIsDelCombine = FALSE;
			continue;
		}
		if (pObj == NULL)
		{
			continue;
		}
		if (!g_pWBPage->m_bIsDelComCurve)
		{
			if (g_pWhiteBoardView->AllowDeleteObj(pObj->GetLayerIndex(),ulEventID))
			{
				WB_OBJ_TYPE objType = (WB_OBJ_TYPE) pObj->GetType();
				if (WB_OBJ_MEDIA != objType
					&& WB_OBJ_CAMERA != objType
					&& WB_OBJ_PICTURE != objType
					&& WB_OBJ_DOCUMENT != objType
					&& WB_OBJ_FLASH != objType
					&& !bIsInCamInfoMap(pObj))
				{
					if(WB_OBJ_COMBINE == objType)
					{
						//将组合列表中的对象发送过去
						((CWBCombine *)pObj)->SetComXmlValue(ulEventID);
					}
					XMemIni xmlObj;
					pObj->SetXmlValue(xmlObj);
					xmlObj.Set(KEY_EVENT_ID, ulEventID);
					AddObj(xmlObj);
					g_pWBPage->m_bIsDelCombine = FALSE;
					continue;
				}
			}
		}
		if (!g_pWBPage->m_bIsDelCombine)
		{
			if (pObj->GetType() == WB_OBJ_COMBINE)
			{
				//删除当前对象在选择框列表中的记录
				pObjPage->DeleteSelectFrame(dwLayerIndex);
				((CWBCombine *)pObj)->CancelCombineObj(dwLayerIndex,dwSendeId);
				m_nComCount = 0;
			}
		}
		g_pWBPage->m_bIsDelCombine = FALSE;
		if (pObj->GetType() == WB_OBJ_MEDIA)
		{
			if (((CWBMedia*)pObj)->IsActiveCreated())
			{
				if (g_WBType == WB_SINGLERVER && g_MediaIntToSin)
				{
					g_MediaIntToSin = FALSE;
					return 0;
				}
			}
		}
		if (i != g_pWBFile->GetCurPageIndex())
		{
			g_pWhiteBoardView->GotoPage(i);
		}

		if (pObj->GetType() == WB_OBJ_CURVE)
		{
			#ifndef SUPPORTMT
			CWBMTObjectManager::GetIns_()->RemoveMTObject(pObj);
			#endif
		}

		if (g_bIsDrawScreen && CScreenDrawMgr::GetIns() != NULL ) //
		{
			///清除screendrawdlg中的DrawMap
			CScreenDrawMgr::GetIns()->ClearTouchDrawMap(dwLayerIndex);
		}
		//
		if (pObjPage->DeleteWBObject(dwLayerIndex))
		{
			//wangxiaobin注释，这个刷新在撤销删除时候有问题。刷新放到oncallbackdata
			// 			if (i == g_pWBFile->GetCurPageIndex())
			// 			{
			// 				if (IsWindow(m_hWnd) && m_bChangeObjectList)
			// 				{
			// 					if (CScreenDrawMgr::GetIns() != NULL)
			// 						CScreenDrawMgr::GetIns()->Invalidate();
			// 					else
			//  						Invalidate();
			// 					m_bChangeObjectList = false;
			// 				}
			// 			}
			break;
		}
	}
	
	return 0;
}

//处理消息，响应服务器端的更新，该消息在未创建放大镜对象时发送。
afx_msg LRESULT CWhiteBoardView::OnWbMaqnobj(WPARAM wParam, LPARAM lParam)
{
	XMemIni* pXMemini = (XMemIni*)wParam;	
	//DWORD dwObjLayerIndex = 0;
	BOOL reValue = TRUE;

	if (NULL == m_pWBRectMaqn)
	{
		m_pWBRectMaqn = new CWBMagnifier();	
		ASSERT(m_pWBRectMaqn);
	} 

	if (!m_pWBRectMaqn->GetXmlValue(*pXMemini))
	{
		reValue = FALSE;
	}
	unsigned long ulobjID;
	if (pXMemini->Get(KEY_OBJ_ID,ulobjID))
	{
		m_pWBRectMaqn->SetLayerIndex(ulobjID);
	}
	else
	{
		reValue = FALSE;
	}
	delete pXMemini;
	if (IsWindow(m_hWnd))
	{
		Invalidate();
	}
	//Invalidate();
	return reValue;	
}

//处理消息，响应服务器端的更新
afx_msg LRESULT CWhiteBoardView::OnWbSpotlightobj(WPARAM wParam, LPARAM lParam)
{
	XMemIni* pXMemini = (XMemIni*)wParam;	
	BOOL reValue = TRUE;
	unsigned long ulSender = 0;
	//创建聚光灯
	if (NULL == m_WBSpotlight)
	{
		//获取中心园的坐标值
		unsigned long left,top, right,bottom;
		if (!pXMemini->Get(KEY_SPOTLIGHT_ROUND_LEFT,left)
			|| !pXMemini->Get(KEY_SPOTLIGHT_ROUND_TOP,top)
			|| !pXMemini->Get(KEY_SPOTLIGHT_ROUND_RIGHT,right)
			|| !pXMemini->Get(KEY_SPOTLIGHT_ROUND_BOTTOM,bottom)
			)
		{
			reValue = FALSE;
		}

		if (!CreateSpotlight(CRect(left,top,right,bottom)))
		{
			delete pXMemini;
			OutputDebugString(_T("Create spot light failed!"));
			return FALSE;
		}
	}
	//if (((*pXMemini).Get(KEY_FILE_SENDER, ulSender))
	//	/*&& (g_ulSessionID == ulSender)*/)
	//{
	//	return 0;
	//}
	
	//读取数据值
	if (!m_WBSpotlight->GetXmlValue(*pXMemini))
	{
		reValue = FALSE;
	}
	//重设ID
	unsigned long ulobjID;
	if ( pXMemini->Get(KEY_OBJ_ID,ulobjID))
	{
		m_WBSpotlight->SetLayerIndex(ulobjID);
	}
	else
	{
		reValue = FALSE;
	}

	delete pXMemini;
	if (IsWindow(m_hWnd))
	{
		Invalidate();
	}
	//Invalidate();
	return reValue;		

	/*if (TRUE == m_bSpotlightState)
	{
	if (NULL == m_WBSpotlight)
	{
	AddSpotlight();
	m_WBSpotlight->ProcessSpotlight(m_rectSpotlight);
	} 
	else
	{
	m_WBSpotlight->ProcessSpotlight(m_rectSpotlight);
	}
	} 
	else if (FALSE == m_bSpotlightState)
	{
	CloseSpotlight();
	}*/
	//return 0;
}

void CWhiteBoardView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(g_bSupportI7)
		return;
	m_pageIsNeedDraw = TRUE;    //放开开关

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	if (m_bIsRButtonDown)//屏幕录制区域录制右键弹起
	{
		m_bIsRButtonDown = false;
		return;
	}
	if (g_bYunNanDeviceSupported && !g_bAuthorizedByHandWare)//若是云南公安的版本没有授权直接返回
	{
		return;
	} 

	if(g_bIsDrawScreen) //桌面绘图打开时屏蔽白板鼠标事件
	{
		return;
	}

	if(DXF_DRAG==g_nDrawStatus||DXF_PREDRAG==g_nDrawStatus) //add by jiangchao 2013.08.23
	{
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		g_nDrawStatus=DRAW_NORMAL;
		return;
	}

	if (DRAW_NORMAL != g_nDrawStatus)
	{
		return;
	}

	CMenu PopupMenu;

	if(!PopupMenu.CreatePopupMenu())
	{
		return;
	}

	CWBObject *pWBObjectbk = NULL;
	CObList * pWBObjectList = NULL;
	pWBObjectbk = g_pWBPage->GetBkgObj();
	pWBObjectList = g_pWBPage->GetWBObjList();
	
	if (m_bIsCopy || m_bIsCut)
	{
		CString strPaste;
		strPaste = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_paste"));
		if (strPaste.IsEmpty())
		{
			strPaste = _T("粘贴");
		}
		PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_PASTE,strPaste);		
	}
	else
	{
		CString strPaste;
		strPaste = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_paste"));
		if (strPaste.IsEmpty())
		{
			strPaste = _T("粘贴");
		}
		PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_PASTE, strPaste);
	}

	if (pWBObjectbk != NULL)
	{
		CString strDelBackground;
		strDelBackground = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_delBackground"));
		if (strDelBackground.IsEmpty())
		{
			strDelBackground = _T("删除当前背景图");
		}
		PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DELETEBACKGROUND, strDelBackground);
	}

	CString strSelectAll;
	strSelectAll = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_selectAll"));
	if (strSelectAll.IsEmpty())
	{
		strSelectAll = _T("全选");
	}
	CString strDeleteAll;
	strDeleteAll = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_deleteAll"));
	if (strDeleteAll.IsEmpty())
	{
		strDeleteAll = _T("删除全部");
	}
	if (pWBObjectList == NULL)
	{
		return;
	}
	if ((pWBObjectList->GetCount() <= 0) || (pWBObjectList->GetCount() == 1 && pWBObjectbk))
	{
		PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_SELECTALL, strSelectAll);
		PopupMenu.AppendMenu(MF_STRING | MF_DISABLED, ID_OBMENU_DELALL, strDeleteAll);
	} 
	else
	{
		PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_SELECTALL, strSelectAll);
		PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_DELALL, strDeleteAll);
	}
	CString strUnLock;
	strUnLock = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_allunlock"));
	if (strUnLock.IsEmpty())
	{
		strUnLock = _T("解除全部锁定");
	}
	PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_ALLUNLOCK, strUnLock);
	CString strAddNotation;
	strAddNotation = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_addNotation"));
	if (strAddNotation.IsEmpty())
	{
		strAddNotation = _T("添加备注");
	}
	CString strWater;
	strWater = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_water"));
	if (strWater.IsEmpty())
	{
		strWater = _T("水波纹");
	}
	if (!g_bU7NewStyle)
	{
		PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_ANNOTATIONS, strAddNotation);
		PopupMenu.AppendMenu(MF_STRING,ID_OBMENU_WATER,strWater);
	}
	
	if (!g_pWhiteBoardView->GetIsLocalWhiteboard() && !g_bU7NewStyle)
	{
		if ( !m_bBroadcastUserCursor )//未广播用户光标
		{
			CString strBroadcast;
			strBroadcast = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_broadcast"));
			if (strBroadcast.IsEmpty())
			{
				strBroadcast = _T("广播用户光标");
			}
			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_BROADCAST_CURSOR, strBroadcast);
		}
		else
		{
			CString strUnbroadcast;
			strUnbroadcast = ReadConfigText(_T("WhiteBoardMenuListText"), _T("popup_unbroadcast"));
			if (strUnbroadcast.IsEmpty())
			{
				strUnbroadcast = _T("取消广播用户光标");
			}
			PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_BROADCAST_CURSOR, strUnbroadcast);
		}
	}	
	CString startRecog;	
	if (!m_bStartRecognition)
	{
		startRecog = ReadConfigText(_T("WhiteBoardMenuListText"),_T("startRecog"));
		if (startRecog.IsEmpty())
		{
			startRecog = _T("开启识别");
		}
	}
	else
	{
		startRecog = ReadConfigText(_T("WhiteBoardMenuListText"),_T("endRecog"));
		if (startRecog.IsEmpty())
		{
			startRecog = _T("取消识别");
		}
		
	}
	
	
	CMenu menu,*pSubMenu;
	menu.LoadMenu(IDR_ZOOMOUTMENU);
	pSubMenu=menu.GetSubMenu(0);
	pSubMenu->CheckMenuRadioItem(ID__20,ID__320,m_dwCheckItem,MF_BYCOMMAND);
	CString strZoomMenu;
	strZoomMenu = ReadConfigText(_T("WhiteBoardMenuListText"),_T("zoomMenu"));
	if (strZoomMenu.IsEmpty())
	{
		strZoomMenu = _T("缩放比例");
	}

// 	CString strMagicPen;
// 	strMagicPen = ReadConfigText(_T("WhiteBoardMenuListText"),_T("magicpen"));
// 	if (strMagicPen.IsEmpty())
// 	{
// 		strMagicPen = _T("魔术笔");
// 	}
	
	if (g_bShowWBNewFun)
	{
		//PopupMenu.AppendMenu(MF_STRING, ID_OBMENU_MAGICPEN, strMagicPen);
		PopupMenu.AppendMenu(MF_STRING,ID_START_RECOGNITION,startRecog);
		//PopupMenu.AppendMenu(MF_STRING|MF_POPUP,(UINT_PTR)pSubMenu->m_hMenu,strZoomMenu );
	}



	//添加位图
	CBitmap PasteBmp;
	CBitmap DeleteBackgroundBmp;
	CBitmap SelectAllBmp;    //全选
	CBitmap DeleteAllBmp;
	CBitmap AnnotationBmp;    
	CBitmap WaterBmp;        
	CBitmap BroadcastCursorBmp;
	CBitmap ZoomOutBmp;//缩放
	CBitmap AllUnLockBmp;
	PasteBmp.LoadBitmap(IDB_OBMENU_PASTE);
	TranslateBitmapSizeToFitXP(&PasteBmp);
	DeleteBackgroundBmp.LoadBitmap(IDB_OBMENU_DELETEBACKGROUND);
	TranslateBitmapSizeToFitXP(&DeleteBackgroundBmp);
	SelectAllBmp.LoadBitmap(IDB_OBMENU_SELECTALL);
	TranslateBitmapSizeToFitXP(&SelectAllBmp);
	DeleteAllBmp.LoadBitmap(IDB_OBMENU_DELALL);
	TranslateBitmapSizeToFitXP(&DeleteAllBmp);
	AnnotationBmp.LoadBitmap(IDB_OBMENU_ANNOTATION);
	TranslateBitmapSizeToFitXP(&AnnotationBmp);
	AllUnLockBmp.LoadBitmap(IDB_OBMENU_UNLOCK);
	TranslateBitmapSizeToFitXP(&AllUnLockBmp);
	if (g_bRunWater)
	{
		WaterBmp.LoadBitmap(IDB_OBMENU_WATERT);
	}
	else
	{
		WaterBmp.LoadBitmap(IDB_OBMENU_WATERF);
	}
	TranslateBitmapSizeToFitXP(&WaterBmp);

	if (m_bBroadcastUserCursor) //已经广播了用户光标了
	{
		BroadcastCursorBmp.LoadBitmap(IDB_OBMENU_RM_BROADCASTCURSOR);
	}
	else
	{
		BroadcastCursorBmp.LoadBitmap(IDB_OBMENU_BROADCASTUSERCURSOR);
	}
    ZoomOutBmp.LoadBitmap(IDB_OBMENU_EQUALSCALEZOOM);//缩放
	TranslateBitmapSizeToFitXP(&ZoomOutBmp);
	TranslateBitmapSizeToFitXP(&BroadcastCursorBmp);

	PopupMenu.SetMenuItemBitmaps(ID_OBMENU_PASTE, MF_BYCOMMAND, &PasteBmp,NULL);
	PopupMenu.SetMenuItemBitmaps(ID_OBMENU_DELETEBACKGROUND, MF_BYCOMMAND, &DeleteBackgroundBmp,NULL);
	PopupMenu.SetMenuItemBitmaps(ID_OBMENU_SELECTALL, MF_BYCOMMAND, &SelectAllBmp,NULL);
	PopupMenu.SetMenuItemBitmaps(ID_OBMENU_DELALL, MF_BYCOMMAND, &DeleteAllBmp,NULL);
	PopupMenu.SetMenuItemBitmaps(ID_OBMENU_ANNOTATIONS, MF_BYCOMMAND, &AnnotationBmp,NULL);
	PopupMenu.SetMenuItemBitmaps(ID_OBMENU_WATER,MF_BYCOMMAND,&WaterBmp,NULL);
	PopupMenu.SetMenuItemBitmaps(ID_OBMENU_BROADCAST_CURSOR, MF_BYCOMMAND, &BroadcastCursorBmp,NULL);
    PopupMenu.SetMenuItemBitmaps((UINT_PTR)pSubMenu->m_hMenu, MF_BYCOMMAND, &ZoomOutBmp,NULL);//缩放
	PopupMenu.SetMenuItemBitmaps(ID_OBMENU_ALLUNLOCK, MF_BYCOMMAND, &AllUnLockBmp, NULL);
	CPoint pt;
	GetCursorPos(&pt);
	BOOL b = PopupMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
	PopupMenu.DestroyMenu();
	//释放位图资源
	PasteBmp.DeleteObject();
	DeleteBackgroundBmp.DeleteObject();
	SelectAllBmp.DeleteObject();
	DeleteAllBmp.DeleteObject();
	AnnotationBmp.DeleteObject();
	WaterBmp.DeleteObject();
	ZoomOutBmp.DeleteObject();
	BroadcastCursorBmp.DeleteObject();
	if (!g_pWhiteBoardView)//12-2-1jinpeng 如果掉线或者对方宣布所有会议结束就直接返回
	{
		return;
	}

	m_ptPaste = point;

	//滚动条偏移量
	UINT XnPos = g_pWhiteBoardView->getXnPos();
	UINT YnPos = g_pWhiteBoardView->getYnPos();
	m_ptPaste.x += XnPos;
	m_ptPaste.y += YnPos;

	CScrollView::OnRButtonUp(nFlags, point);
}

BOOL CWhiteBoardView::addFunctionPanel()
{
	//功能面板
	if (!m_bRefurbishPreview)
	{
		return FALSE;
	}
	if (NULL != m_pWBFunctionDlg)
	{
		if (!m_pWBFunctionDlg->IsWindowVisible())
		{
			((CWhiteBoardDlg*)GetParent())->MoveWBFunctionDlg(TRUE);
			((CWhiteBoardDlg*)GetParent())->MoveOptionDlg(TRUE);

		} 
		else
		{	

			if (g_pWhiteBoardView->GetWBFunctionDlg() && g_pWhiteBoardView->GetWBFunctionDlg()->IsWindowVisible())
			{
				//g_pWhiteBoardView->GetWBFunctionDlg()->OnBnClickedClosewbfunctiondlg();
			}
			//if (m_pWBFunctionDlg->GetAttributeDlg() != NULL && m_pWBFunctionDlg->GetAttributeDlg()->IsWindowVisible())
			//{
			//	m_pWBFunctionDlg->GetAttributeDlg()->CloseColorPaletteDlg();
			//}
			//((CWhiteBoardDlg*)GetParent())->MoveWBFunctionDlg(FALSE);
			//((CWhiteBoardDlg*)GetParent())->MoveOptionDlg(FALSE);

		}

	} 
	else
	{
		m_pWBFunctionDlg = ((CWhiteBoardDlg*)GetParent())->GetWBFunctionDlg();	
		m_pWBOptionDlg = ((CWhiteBoardDlg*)GetParent())->CreateOptionDlg();		
		((CWhiteBoardDlg*)GetParent())->MoveWBFunctionDlg(TRUE);
		((CWhiteBoardDlg*)GetParent())->MoveOptionDlg(TRUE);
	}
	
	return TRUE;
}

void  CWhiteBoardView::SetToolbarHWND(HWND hTOOLBARWnd)
{
	m_hTOOLBARWnd = hTOOLBARWnd;
}

void CWhiteBoardView::ProcessMediaOperation( XMemIni xmlObj )
{
	DWORD dwObjLayerIndex = 0; 
	int nPageIndex = 0;	
	int nMediaOperation; //媒体操作类型

	//取对象图层号、页码
	if (!xmlObj.Get(KEY_OBJ_ID, dwObjLayerIndex)
		|| !xmlObj.Get(KEY_OBJ_PAGE, nPageIndex)
		|| !xmlObj.Get(KEY_MEDIA_OPERATION, nMediaOperation))
	{
		return;
	}

	//如果页码不对
	if (nPageIndex <= 0 || nPageIndex > (int)g_pWBFile->GetPageCount())
	{
		return;
	}

	CWBPage *pObjPage = g_pWBFile->GetPage(nPageIndex);
	CWBObject *pObj = pObjPage->FindByLayerIndex(dwObjLayerIndex);
	if (pObj == NULL)
	{
		return;
	}
	WB_OBJ_TYPE ntemptype = (WB_OBJ_TYPE)pObj->GetType();
	switch (nMediaOperation)
	{
	case WB_MOVIE_START: //视频、FLASH开始播放或暂停
		{
			if (ntemptype == WB_OBJ_FLASH)
			{
				((CWBFlash *)pObj)->Start();
			}
		}
		break;

	case WB_MOVIE_STOP: //视频、FLASH停止
		{
			if (ntemptype == WB_OBJ_FLASH)
			{
				((CWBFlash *)pObj)->Stop();
			}
		}
		break;
	case WB_MOVIE_LOOP: //视频、FLASH循环播放
		{
			if (ntemptype == WB_OBJ_FLASH)
			{
				((CWBFlash *)pObj)->SetCircle();
			}
		}
		break;
	case WB_MOVIE_SYNCHRONISATION:
		{
			unsigned long ulCurrentTime = 0;
			if (xmlObj.Get(KEY_MOVIE_CURRENTTIME, ulCurrentTime))
			{
				/*((CWBMovie *)pObj)->Seek(ulCurrentTime);*/
			}							
		}
		break;
	case WB_DOCUMENT_PREPAGE: //文档上一页
		{
			if (!((CWBDocument *)pObj)->PrePage())
			{
				return;
			}
		}
		break;
	case WB_DOCUMENT_NEXTPAGE: //文档下一页
		{
			if (!((CWBDocument *)pObj)->NextPage())
			{
				return;
			}
		}		
		break;
	case WB_DOCUMENT_FIRSTPAGE: //文档首页
		{
			if (!((CWBDocument *)pObj)->FirstPage())
			{
				return;
			}
		}		
		break;
	case WB_DOCUMENT_LASTPAGE: //文档尾页
		{
			if (!((CWBDocument *)pObj)->LastPage())
			{
				return;
			}
		}		
		break;
	default:
		return;
	}

	//如果当前页与修改页相同
	if (nPageIndex == g_pWBFile->GetCurPageIndex())
	{
		if (IsWindow(m_hWnd))
		{
			Invalidate();
		}
		//Invalidate();
	}
}

void CWhiteBoardView::MediaOperation( XMemIni xmlObj )
{
	std::string strValue = "";

	xmlObj.Set(KEY_FILE_SIZE,0);
	xmlObj.Set(KEY_FILE_POS,0);
	xmlObj.Set(KEY_FILE_TYPE,"");
	xmlObj.Set(KEY_FILE_PATH,"");

	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);

	//填写事件ID
	unsigned long ulEventID;
	if (!xmlObj.Get(KEY_EVENT_ID, ulEventID))
	{
		ulEventID = GenerateSSRC();
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
	}

	int nPageIndex = (int)g_pWBFile->GetCurPageIndex();
	xmlObj.Set(KEY_OBJ_PAGE, nPageIndex); //填写页码
	xmlObj.GetString(strValue);

	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA; 
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_MEDIA_OPERATION; //待定			
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}

		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

		if (pData)
		{
			delete [] pData;
			pData = NULL;
		}
	}
}

void CWhiteBoardView::SendScreenDraw()
{
	XMemIni xmlObj;
	xmlObj.Set(KEY_OBJ_ID, 0);
	xmlObj.Set(KEY_OBJ_TYPE, 0);
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	xmlObj.Set(KEY_SCREENDRAW, g_bIsDrawScreen);
	if (CScreenDrawMgr::GetIns() != NULL)
	{
		xmlObj.Set(KEY_SCREENDRAW_GUISE, m_bSDIsTransparent);
	}
	else
	{
		xmlObj.Set(KEY_SCREENDRAW_GUISE, false);
	}
	//	xmlObj.Set(KEY_SCREENDRAW_FULLSCREEN, m_bSDIsFullScreen);
	xmlObj.Set(KEY_SCREENDRAW_ALPHA, m_nSDAlpha);	
	if (m_bNeedSetPageIndexForScreenDraw)
	{
		xmlObj.Set(KEY_OBJ_PAGE,1);
	}
	ChangeObj(xmlObj);
}

BOOL CWhiteBoardView::SaveSCDraw(char *pBuf, DWORD& dwLen)
{
	if (pBuf == NULL)
	{
		return FALSE;
	}

	DWORD dwLength = sizeof(g_bIsDrawScreen) + sizeof(m_bSDIsTransparent);

	if (dwLen < dwLength)
	{
		return FALSE;
	}

	int nIndex = 0;
	if (CScreenDrawMgr::GetIns() != NULL)
	{
		m_bSDIsTransparent = CScreenDrawMgr::GetIns()->IsTransparent();
	}

	memcpy(pBuf+nIndex, (char *)&g_bIsDrawScreen, sizeof(g_bIsDrawScreen));
	nIndex += sizeof(g_bIsDrawScreen);

	memcpy(pBuf+nIndex, (char *)&m_bSDIsTransparent, sizeof(m_bSDIsTransparent));
	nIndex += sizeof(m_bSDIsTransparent); 

	dwLen = dwLength;
	return TRUE;
}

BOOL CWhiteBoardView::LoadSCDraw(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength)
{
	if (pFile == NULL)
	{
		return FALSE;
	}
	BOOL bSDIsTransparent;

	//计算长度
	LONGLONG lLength = sizeof(g_bIsDrawScreen) + sizeof(bSDIsTransparent);

	if ((lFileLength - lOff) < (ULONGLONG)lLength)
	{
		return FALSE;
	}

	pFile->Seek(lOff, CFile::begin);
	pFile->Read((char *)&g_bIsDrawScreen, sizeof(g_bIsDrawScreen));
	lOff += sizeof(g_bIsDrawScreen);

	pFile->Seek(lOff, CFile::begin);
	pFile->Read((char *)&bSDIsTransparent, sizeof(bSDIsTransparent));
	lOff += sizeof(bSDIsTransparent);

	if (g_bIsDrawScreen)
	{
		DrawOnScreen();
		ShiftGuise(bSDIsTransparent);
		//发送同步信息
		SendScreenDraw();
	}
	else
	{
		WB_DestroyWnd();
	}

	return TRUE;
}

void CWhiteBoardView::OpenSCDrawReStore()
{
	if (m_bIsOpenSCDraw)
	{
		m_bIsOpenSCDraw = FALSE;
		DrawOnScreen();
		ShiftGuise(m_bIsSDTransparent);
	}
}

void CWhiteBoardView::SetIsMinSize(BOOL bIsMinSize)
{
	m_bIsMinSize = bIsMinSize;
	if (CScreenDrawMgr::GetIns())
	{
		///将屏幕绘制的三层窗口最小化
		CScreenDrawMgr::GetIns()->SetIsMinSize(m_bIsMinSize);
	}
}

afx_msg LRESULT CWhiteBoardView::OnWbProcessobj(WPARAM wParam, LPARAM lParam)
{

	XMemIni* pXMemini = (XMemIni*)wParam;	
	DWORD dwObjLayerIndex = 0; 
	int nType = 0;
	int nPageIndex = 0;
	unsigned long ulEventID; //接收事件ID
	unsigned long ulSender = 0;
	int nLayerNum = 0;

	if (!(*pXMemini).Get(KEY_OBJ_ID, dwObjLayerIndex)
		|| !(*pXMemini).Get(KEY_OBJ_TYPE, nType)
		|| !(*pXMemini).Get(KEY_OBJ_PAGE, nPageIndex)
		|| !(*pXMemini).Get(KEY_EVENT_ID, ulEventID)
		|| !(*pXMemini).Get(KEY_FILE_SENDER, ulSender)
		|| !(*pXMemini).Get(KEY_OBJ_LAYER_NUM,nLayerNum))
	{
		delete pXMemini;
		return 0;
	}	
	CWBPage *pObjPage = g_pWBFile->GetPage(nPageIndex);
	if (pObjPage == NULL)
	{
		delete pXMemini;
		return 0;
	}

	WB_OBJ_TYPE objType = (WB_OBJ_TYPE)nType;

	BOOL bFlag = FALSE; //标记是不是组合里的对象

	XAutoLock l(m_csOnWbChangeObjLock);
	CWBObject *pObj = pObjPage->FindByLayerIndex(dwObjLayerIndex, objType);
	if (pObj == NULL) //如果没在对象列表里找到，就到组合列表里找
	{
		if (m_pComObj != NULL && m_dwComLayerIndex == dwObjLayerIndex)
		{
			pObj = m_pComObj;
		}
		else
		{
			pObj = pObjPage->FindComObjByLayerIndex(dwObjLayerIndex);
		}
		m_pComObj = NULL;
		m_dwComLayerIndex = 0;

		bFlag = TRUE;
	}
	if (pObj == NULL)
	{
		delete pXMemini;
		return 0;
	}

	if (nLayerNum != pObj->GetPageLayerIndex())
	{
		//图层操作
		g_pWBPage->ChangeOBJLayer(dwObjLayerIndex,nLayerNum);
	}
	BOOL bIsBackGround = FALSE;
	if (pObj->IsBkg())
	{
		 bIsBackGround = TRUE;
	}
	
	pObj->GetXmlValue(*pXMemini);
	//wang暂时这样处理
	///////////////////////////////////////////////////////////////////
	if (pObj->GetType() == WB_OBJ_MEDIA)
	{
		if (((CWBMedia*)pObj)->GetMediaPlayState() == Media_pausing)
		{
			((CWBMedia*)pObj)->MediaPlay();
			Sleep(1);
			((CWBMedia*)pObj)->MediaPause();
		}
		if (((CWBMedia*)pObj)->GetMediaPlayState() == Media_stop)
		{
			((CWBMedia*)pObj)->MediaPlay();
			Sleep(1);
			((CWBMedia*)pObj)->MediaStop();
		}

		if (((CWBMedia*)pObj)->IsActiveCreated())
		{
			EVoiceState voiceState = ((CWBMedia*)pObj)->GetVoiceState();
			int voice = ((CWBMedia*)pObj)->GetVedioVolume();
			if (voiceState == VOICEOPEN)
			{
				((CWBMedia*)pObj)->SetVedioVolume(voice);
			} 
		}
	}
	//////////////////////////////////////////////////////////////////
	if (pObj->GetType() == WB_OBJ_PICTURE)
	{
		if (pObj->IsBkg())
		{
			//撤销、重做后判断是否要隐藏背景按钮
			if (g_pWhiteBoardView->GetWBFunctionDlg() 
				&& g_pWhiteBoardView->GetWBFunctionDlg()->IsWindowVisible()
				&& g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()
				&& g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->IsWindowVisible())
			{
				g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_DefaultBackGround.ShowWindow(SW_SHOW);
				g_pWBPage->SetIsExistBackGround(TRUE);
			}

			int nWidth, nHeight;
			nWidth = ((CWBPicture *)pObj)->GetWidth();
			nHeight = ((CWBPicture *)pObj)->GetHeight();
			if (!(nWidth < 2400 && nHeight < 1600))
			{
				if (nWidth > 37000)
				{
					nHeight = 37000 * nHeight / nWidth;
					nWidth = 37000;
				}
				if (nHeight > 37000)
				{
					nWidth = 37000 * nWidth / nHeight;
					nHeight = 37000;
				}

				g_nWidth = nWidth;
				g_nHeight = nHeight;
				setViewScroll();
				m_pEagleEyeDlg->GetPreviewSingleView()->SetRatio();
				m_pEagleEyeDlg->GetPreviewSingleView()->SetRectInPreview();

			}
		}
		else
		{
			//撤销、重新后判断是否要隐藏背景按钮
			if (bIsBackGround
				&& g_pWhiteBoardView->GetWBFunctionDlg() 
				&& g_pWhiteBoardView->GetWBFunctionDlg()->IsWindowVisible()
				&& g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()
				&& g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->IsWindowVisible())
			{
				g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_DefaultBackGround.ShowWindow(SW_HIDE);
				g_pWBPage->SetIsExistBackGround(FALSE);
			}
		}
	}

	if (pObj->IsLock())
	{
		SendMessage(WM_CANCELMODE, NULL, NULL);
	}

	if (bFlag)
	{
		++m_nComCount;
	}
	if (pObj->GetType() == WB_OBJ_COMBINE)
	{
		((CWBCombine *)pObj)->GetComLayerXml(*pXMemini);
		if (m_bEraseLineUp)
		{
			//注释原因:修改BUG28455
			/*((CWBCombine *)pObj)->DeleteCurCombineObjectList();*/
			m_bEraseLineUp = FALSE;
		}

		int nCount = ((CWBCombine *)pObj)->GetComCount();
		m_nComCount -= nCount;

		CPoint ptRotationCenter;
		pObj->GetRotationCenter(ptRotationCenter);
		pObj->SetRectSelectRotationCenter(ptRotationCenter);
		pObj->SetRectSelectAngle(pObj->GetRotationAngle());
	}

	if (pObj->GetType() == WB_OBJ_CURVE)
	{
		((CWBCurve *)pObj)->SetIsDraw();
	}	
	if(pObj->GetType() == WB_OBJ_ICOCURVE)/*add by jiangchao 2013.5.14*/
	{
		CWBIcoCurve* pIcoCurve = (CWBIcoCurve *)pObj;
		//pIcoCurve->SetIsNeedSetImage(TRUE); //先这样做,以后改.
		((CWBIcoCurve*)pObj)->SetIsRefush(TRUE);
		pIcoCurve->SetIcoImage();
	}

	if (pObj->GetType() == WB_OBJ_PICTURE)
	{
		((CWBPicture*)pObj)->SetIsPath();
	}
	
	if(pObj->GetType() == WB_OBJ_DXF) //判断是否重画DXF
	{
		((CWBDxf *)pObj)->SetIsNeedReDraw(); 
	}

	SetPageIsNeedDraw(TRUE);
	Invalidate();
	UpdateWindow();
	SetPageIsNeedDraw(FALSE);
	delete pXMemini;
	return 0;
}

afx_msg LRESULT CWhiteBoardView::OnProcessAddMedia(WPARAM wParam, LPARAM lParam)
{
	XMemIni* pXMemini = (XMemIni*)wParam;	
	DWORD dwObjLayerIndex = 0; 
	int nType = 0;
	int nPageIndex = 0;
	unsigned long ulEventID; //接收事件ID
	unsigned long ulSender = 0;
	string strFilePath = "";
	//取对象图层号、类型、页码
	if (!(*pXMemini).Get(KEY_OBJ_ID, dwObjLayerIndex)
		|| !(*pXMemini).Get(KEY_OBJ_TYPE, nType)
		|| !(*pXMemini).Get(KEY_OBJ_PAGE, nPageIndex)
		|| !(*pXMemini).Get(KEY_EVENT_ID, ulEventID)
		|| !(*pXMemini).Get(KEY_MEDIA_ADDSESSION, ulSender)
		|| !(*pXMemini).Get(KEY_MEDIA_FILE_PATH, strFilePath))
	{
		delete pXMemini;
		return 0;
	}
	CWBPage *pObjPage = g_pWBFile->GetPage(nPageIndex);
	int CurPageIndex = 0;
	CurPageIndex = g_pWBFile->GetCurPageIndex();

	if (nType == WB_OBJ_MEDIA && pObjPage)
	{
		CString strFile = ToDRAWStr(strFilePath).c_str();
		CRect rct = CRect( 74, 43, 580, 450 );

		bool bflagMedia = false;
		std::map<DWORD,CRect>:: iterator it;
		if (pObjPage && pObjPage->GetMediaRectMap())
		{
			it = pObjPage->GetMediaRectMap()->find(dwObjLayerIndex);
			if (pObjPage->GetMediaRectMap()->end() != it)
			{
				rct = it->second;
				bflagMedia = true;
			}
		}

		int nTop, nLeft, nRight, nBottom;
		if ((*pXMemini).Get(KEY_OBJ_RECT_TOP, nTop)
			&& (*pXMemini).Get(KEY_OBJ_RECT_LEFT, nLeft)
			&& (*pXMemini).Get(KEY_OBJ_RECT_RIGHT, nRight)
			&& (*pXMemini).Get(KEY_OBJ_RECT_BOTTOM, nBottom))
		{
			//两者有些重复
			if (bflagMedia)
			{
				pObjPage->GetMediaRectMap()->erase(it);
				bflagMedia = false;
			}
			else
			{
				rct = CRect(nLeft, nTop, nRight, nBottom);
			}
		}
		else
		{
			delete pXMemini;
			return 0;
		}

		CWBMedia *pMeida = NULL;

		if (g_ulSessionID == ulSender )
		{
			std::map<DWORD,CString>:: iterator iter;
			if(strFile == _T(""))
			{
				for(iter = m_MediaStringPath.begin();iter != m_MediaStringPath.end();++iter )
				{
					if (iter->first == dwObjLayerIndex)
					{
						strFile = iter->second;
					}
				}
			}

			pMeida = new CWBMedia(strFile, rct);

			if (CurPageIndex != nPageIndex)
			{
				pMeida->HideAndPauseMedia();
			}

			pObjPage->AddWBObject(pMeida, pMeida->GetLayerType());			
			pMeida->SetLayerIndex(dwObjLayerIndex);	
			pMeida->SetOwner(ulSender);
			//录像
			///////////////////////////////////////////////////////////////
			DWORD layerIndex = pMeida->GetLayerIndex();
			if (pMeida && pMeida->GetMediaWindow()
				&& pMeida->GetMediaWindow()->GetCapChanPlayHandleMedia())
			{
				pMeida->GetMediaWindow()->GetCapChanPlayHandleMedia()->SetMedia(layerIndex);
			}
			/////////////////////////////////////////////////////////////////

			XMemIni xmlObj;	
			BOOL bitratr = TRUE;
			pMeida->SetMediaBitrate(TRUE);
			xmlObj.Set(KEY_MEDIA_BITRATE_BOOL, bitratr);
			xmlObj.Set(KEY_EVENT_ID,ulEventID);
			xmlObj.Set(KEY_OBJ_PAGE,nPageIndex);
			xmlObj.Set(KEY_MEDIA_ADDSESSION,ulSender);
			pMeida->SetMediaPlayState(Media_playing);
			pMeida->SetXmlValue(xmlObj);
			pMeida->SetMediaBeCreated(TRUE);
			ChangeObj(xmlObj);
			//xmlObj.Set(KEY_BROADCAST_CMD,_E_BROAD_UNDO_REDO_MEDIA);
			//Broadcast_Data(xmlObj);


			pMeida->SetMediaBitrate(FALSE); 

			delete pXMemini;
			return 0;
		}
		else
		{
			pMeida = new CWBMedia(_T(""), rct, FALSE);
			if (pMeida ==NULL)
			{
				delete pXMemini;
				return 0;
			}
			pMeida->SetSaveGetMediaName(strFile);
		}
		if (!pMeida->GetXmlValue(*pXMemini))
		{
			delete pMeida;

			delete pXMemini;
			return 0;
		}
		pObjPage->AddWBObject(pMeida, pMeida->GetLayerType());			
		pMeida->SetLayerIndex(dwObjLayerIndex);
		m_MediaID = dwObjLayerIndex;
		pMeida->SetOwner(ulSender);

		//录像
		///////////////////////////////////////////////////////////////
		DWORD layerIndex = pMeida->GetLayerIndex();
		if (pMeida && pMeida->GetMediaWindow()
			&& pMeida->GetMediaWindow()->GetPlayChanEmptyHandleMedia())
		{
			pMeida->GetMediaWindow()->GetPlayChanEmptyHandleMedia()->SetMediaplay(layerIndex);
		}
		/////////////////////////////////////////////////////////////////
	}

	delete pXMemini;
	return 0;
}


afx_msg LRESULT CWhiteBoardView::OnProcessAddCamera(WPARAM wParam, LPARAM lParam)
{
	DWORD dwObjLayerIndex = 0; 
	int nType = 0;
	int nPageIndex = 0;
	unsigned long ulEventID; //接收事件ID
	unsigned long ulSender = 0;

	//	int nLayerChangeType;
	XMemIni* pXMemini = (XMemIni*)wParam;
	if (pXMemini == NULL)
	{
		return 0;
	}
	//取对象图层号、类型、页码
	if (!(*pXMemini).Get(KEY_OBJ_ID, dwObjLayerIndex)
		|| !(*pXMemini).Get(KEY_OBJ_TYPE, nType)
		|| !(*pXMemini).Get(KEY_OBJ_PAGE, nPageIndex)
		|| !(*pXMemini).Get(KEY_EVENT_ID, ulEventID)
		|| !(*pXMemini).Get(KEY_CAREMA_ADDSESSION, ulSender))
	{
		delete pXMemini;
		return 0;
	}		

	bool bCarema = false;
	std::map<DWORD,CRect>::iterator itor;
	CWBPage *pObjPage = g_pWBFile->GetPage(nPageIndex);
	int nCmd = -1;
	CRect rct = CRect(74, 43, 474, 343);//要改
	CWBCamera *pCamera = NULL;
	if (nType == WB_OBJ_CAMERA)
	{

		if (pObjPage && pObjPage->GetCaremaRectMap())
		{
			itor = pObjPage->GetCaremaRectMap()->find(dwObjLayerIndex);
			if (pObjPage->GetCaremaRectMap()->end() != itor)
			{
				rct = itor->second;
				bCarema = true;
 			}
 		}
 		
 		int nTop, nLeft, nRight, nBottom;
 		if ((*pXMemini).Get(KEY_OBJ_RECT_TOP, nTop)
 			&& (*pXMemini).Get(KEY_OBJ_RECT_LEFT, nLeft)
 			&& (*pXMemini).Get(KEY_OBJ_RECT_RIGHT, nRight)
 			&& (*pXMemini).Get(KEY_OBJ_RECT_BOTTOM, nBottom))
 		{
 			//两者有些重复
 			if (bCarema)
 			{
 				pObjPage->GetCaremaRectMap()->erase(itor);
 				bCarema = false;
 			}
 			else
 			{
 				rct = CRect(nLeft, nTop, nRight, nBottom);
 			}
 		}
 		else
 		{
 			delete pXMemini;
 			return 0;
 		}
		//视频拖拽的撤销
		if ((*pXMemini).Get(KEY_DRAG_CAMERA_TO_WB, nCmd) && nCmd == CMD_CAMARA_DROP_OBJ)
		{
			int a = 10;
		}
		else
		{
			if (g_ulSessionID == ulSender)
			{
				//RemoveObj(dwObjLayerIndex);
				pCamera = new CWBCamera(TRUE, rct);
				if (m_pCamaraObj == NULL)
				{
					//m_pCamaraObj = pCamera;
				}
				//g_pWBPage->AddWBObject(pCamera);
				pObjPage->AddWBObject(pCamera, pCamera->GetLayerType());			
				pCamera->SetLayerIndex(dwObjLayerIndex);
				m_pUniqueWBCamera = pCamera;
				pCamera->SetOwner(ulSender);
				BOOL bUNDOREDO = TRUE;
			//录像
				///////////////////////////////////////////////////////////////
				DWORD layerIndex = pCamera->GetLayerIndex();
				if (pCamera && pCamera->GetCameraDlg()
					&& pCamera->GetCameraDlg()->GetCapChanPlayHandle_Empty())
				{
					pCamera->GetCameraDlg()->GetCapChanPlayHandle_Empty()->SetCaremaID(layerIndex);
				}
				/////////////////////////////////////////////////////////////////	
				//发送同步消息
				XMemIni xmlObj;
				pCamera->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_EVENT_ID,ulEventID);
				xmlObj.Set(KEY_CAREMA_ADDSESSION,ulSender);
				xmlObj.Set(KEY_CAMERA_UNDO_REDO,bUNDOREDO);
				//xmlObj.Set(KEY_BROADCAST_CMD,_E_BROAD_UNDO_REDO_CAMERA);
				//Broadcast_Data(xmlObj);
				ChangeObj(xmlObj);
				//AddObj(xmlObj);
				delete pXMemini;
				return 0;
			}
		}

		pCamera = new CWBCamera(FALSE, rct);//被动创建者
		if (m_pCamaraObj == NULL)
		{
			//m_pCamaraObj = pCamera;
		}
		if (!pCamera->GetXmlValue(*pXMemini))
		{
			delete pXMemini;
			delete pCamera;
			return 0;
		}
		else
		{
			pObjPage->AddWBObject(pCamera, pCamera->GetLayerType());			
			pCamera->SetLayerIndex(dwObjLayerIndex);
			
			int nDrapObj = 0;
			pXMemini->Get(KEY_DRAG_CAMERA_TO_WB,nDrapObj);
			if(nDrapObj ==CMD_CAMARA_DROP_OBJ)
			{
				m_pCamaraObj = pCamera;
			}
			pCamera->SetOwner(ulSender);
			//录像
			///////////////////////////////////////////////////////////////
			DWORD layerIndex = pCamera->GetLayerIndex();
			if (pCamera && pCamera->GetCameraDlg()
				&& pCamera->GetCameraDlg()->GetPlayChanCallback_Empty())
			{
				pCamera->GetCameraDlg()->GetPlayChanCallback_Empty()->SetCaremaPlay(layerIndex);
			}
			/////////////////////////////////////////////////////////////////
		}	
	}
	delete pXMemini;
	return 0;
}
//发送连接同步
LRESULT CWhiteBoardView::OnSendCameraConnectSynchronous(WPARAM wPar, LPARAM lPar)
{
	CWBCamera *pCamera = (CWBCamera *)wPar;
	if (pCamera)
	{
		//
		XMemIni xmlObj;
		pCamera->SetSynchronousLable(Connect_Synchronous);
		pCamera->SetXmlValue(xmlObj);
		ChangeObj(xmlObj);
	}
	return S_OK;
}

afx_msg LRESULT CWhiteBoardView::OnProcessAddFlash(WPARAM wParam, LPARAM lParam)
{
	DWORD dwObjLayerIndex = 0; 
	int nType = 0;
	int nPageIndex = 0;
	unsigned long ulEventID; //接收事件ID
	unsigned long ulSender = 0;
	string strFilePath;
	//	int nLayerChangeType;
	XMemIni* pXMemIni = (XMemIni*)wParam;
	//取对象图层号、类型、页码
	if (!(*pXMemIni).Get(KEY_OBJ_ID, dwObjLayerIndex)
		|| !(*pXMemIni).Get(KEY_OBJ_TYPE, nType)
		|| !(*pXMemIni).Get(KEY_OBJ_PAGE, nPageIndex)
		|| !(*pXMemIni).Get(KEY_EVENT_ID, ulEventID)
		|| !(*pXMemIni).Get(KEY_FILE_SENDER, ulSender)
		// 		|| !m_XMemIni.Get(KEY_FILE_PATH, strFilePath)
		)
	{
		return 0;
	}		

	CWBPage *pObjPage = g_pWBFile->GetPage(nPageIndex);

	if (nType == WB_OBJ_FLASH)
	{
		//CString strFile = strFilePath.c_str();
		CWBFlash *pFlash = new CWBFlash();

		if (!pFlash->GetXmlValue(*pXMemIni))
		{
			delete pFlash;
		}
		else
		{
			if (!pObjPage->AddObjIntoCom(pFlash))
			{
				pObjPage->AddWBObject(pFlash);	
			}
			//pFlash->init();
			pFlash->SetLayerIndex(dwObjLayerIndex);

			pFlash->SetOwner(ulSender);
		}		
	}

	delete pXMemIni;
	return 0;
}

void CWhiteBoardView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	if (SB_ENDSCROLL == nSBCode)
	{
		int x = (int)getXnPos();
		int y = (int)(g_pWBPage->GetPageViewScroll().y);
		g_pWBPage->SetPageViewScroll(CPoint(x , y));
		//发送水平滚动条同步消息
		SendScrollDraw();
	}
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CWhiteBoardView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	if (SB_ENDSCROLL == nSBCode)
	{
		int x = (int)(g_pWBPage->GetPageViewScroll().x);
		int y = (int)getYnPos();
		g_pWBPage->SetPageViewScroll(CPoint(x,y));

		//发送垂直滚动条同步消息
		SendScrollDraw();

	}
	
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

CWBEagelEye *CWhiteBoardView::GetWBEagleEyedlg()
{
	return m_pEagleEyeDlg;
}
BOOL CWhiteBoardView::IsOpenPreviewPanel()
{
	if (NULL != m_pWBFunctionDlg)
	{
		if (m_pWBFunctionDlg->GetPreviewDLG()/*->IsWindowVisible()*/)
		{
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CWhiteBoardView::IsOpenFunctionDlg()
{
	if (NULL != m_pWBFunctionDlg)
	{
		if (m_pWBFunctionDlg->IsWindowVisible())
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CWhiteBoardView::DrawOnScreen(const CRect *rect /* =  NULL */)
{
	if (NULL != m_WBSpotlight)
	{
		CString strCloseSpotlight;
		strCloseSpotlight = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CloseSpotlight"));
		if (strCloseSpotlight.IsEmpty())
		{
			strCloseSpotlight = _T("请关闭聚光灯！");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(this->m_hWnd,m_strPrompt, strCloseSpotlight, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strCloseSpotlight);
		}
		else
		{
			MessageBox(strCloseSpotlight, m_strPrompt, MB_OK);
		}	
		return FALSE;
	}

	CRect clientRC;
	((CWhiteBoardDlg*)GetParent())->GetClientRect(&clientRC);
	ClientToScreen(&clientRC);

	if (clientRC.left < -100 && clientRC.right < -100 && clientRC.top < -100 && clientRC.bottom < -100)
	{
		m_bIsOpenSCDraw = TRUE;
		return FALSE;
	}

	clientRC.left = clientRC.left + WB_CTRLBTNDLG_WIDTH;
	clientRC.right = clientRC.right   -20;
	clientRC.top = clientRC.top + WB_TOOLBOXBTN_SPEC_HEIGHT + 10;
	clientRC.bottom = clientRC.bottom - 20;
	rect = & clientRC;

	if(CScreenDrawMgr::GetIns()==NULL)
	{   
		//对文本编辑框进行删除
		DeleteEdit();
		m_bIsCopy = FALSE;
		m_bIsCut = FALSE;
		g_pWBPage->ClearSelectFrameList();

		CScreenDrawMgr::CreateIns();			///创建屏幕绘制管理类实例
		CScreenDrawMgr::GetIns()->Init(clientRC);	///创建屏幕绘制的三层窗口
		m_bSDIsTransparent = CScreenDrawMgr::GetIns()->IsTransparent();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CWhiteBoardView::FullScreenShift(bool isFullScreen)
{
	if(CScreenDrawMgr::GetIns() != NULL)
	{
		CScreenDrawMgr::GetIns()->FullScreenShift(isFullScreen);
		//		m_bSDIsFullScreen = isFullScreen;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CWhiteBoardView::DestroyScreenDraw(void)
{
	if(CScreenDrawMgr::GetIns() != NULL)
	{
		//对文本编辑框进行删除
		DeleteEdit();
		m_bIsCopy = FALSE;
		m_bIsCut = FALSE;
		g_pWBPage->ClearSelectFrameList();
		CScreenDrawMgr::DestroyIns();			///销毁屏幕绘制管理类实例
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CWhiteBoardView::ShiftGuise(BOOL isTransparent)
{
	if(CScreenDrawMgr::GetIns() != NULL)
	{
		CScreenDrawMgr::GetIns()->GuidesShift(isTransparent);
		m_bSDIsTransparent = isTransparent;
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}
void CWhiteBoardView::OpenCtrlDlg()
{
	m_ScreenDrawCtrl = new CDrawCtl(NULL);
	m_ScreenDrawCtrl->Create(IDD_DRAWCTRL,NULL);
	m_ScreenDrawCtrl->ShowWindow(SW_SHOW);
}

void CWhiteBoardView::SetSDAlpha(int nAlpha)
{
	m_nSDAlpha = nAlpha;
}

//创建放大器
void CWhiteBoardView::AddMagnifier()
{
	if (NULL == m_pWBRectMaqn)
	{
		//获取当前视图左上角坐标
		int nXPos = getXnPos();
		int nYPos = getYnPos();
		//放大框的大小
		CRect RectMaqnBtn(0, 0, 300, 300);
		//移动到当前视图相对位置（130,130）,避免左侧工具栏挡住放大镜
		RectMaqnBtn.MoveToXY(nXPos+130,nYPos+130);
		m_pWBRectMaqn = new CWBMagnifier(RectMaqnBtn);

		//设置对象的ID
		DWORD dwObjID = (DWORD)GenerateSSRC();
		m_pWBRectMaqn->SetLayerIndex(dwObjID);

		//SendMagn();
		XMemIni xmlObj;
		m_pWBRectMaqn->SetXmlValue(xmlObj);
		AddObj(xmlObj);
		Invalidate(TRUE);
	}
	else
	{
		CRect magnRect;
		m_pWBRectMaqn->GetRect(magnRect);
		magnRect.left -= 5;
		magnRect.right += 5;
		magnRect.top -= 5;
		magnRect.bottom += 80;
		CloseRectMagn();
		SetPageIsNeedDraw(TRUE);
		//InvalidateRect(magnRect);
		Invalidate();
		SetPageIsNeedDraw(FALSE);
	}
}

//发送放大镜同步
void CWhiteBoardView::SendMagn() 
{
	if (NULL != m_pWBRectMaqn)
	{
		XMemIni xmlObj;
		m_pWBRectMaqn->SetXmlValue(xmlObj);
		//
		ChangeObj(xmlObj);
	}
}

//关闭放大镜
void CWhiteBoardView::CloseRectMagn()
{
	if (m_pWBRectMaqn != NULL)
	{
		//设置发送事件ID
		m_ulSendEventID = GenerateSSRC();
		RemoveObj(m_pWBRectMaqn->GetLayerIndex());

		delete m_pWBRectMaqn;
		m_pWBRectMaqn = NULL;

		//g_nDrawStatus = DRAW_NORMAL;
		//m_nMaqnCount = 0;
	}	
}

//执行左键按下时放大镜的相关处理，返回TRUE,则说明当前鼠标操作的是放大镜，
//调用函数直接返回；为FALSE，调用函数继续下面的操作
BOOL CWhiteBoardView::MouseDownMagn(const CPoint& point)
{
	if (NULL != m_pWBRectMaqn)//假如创建了放大器
	{
		BOOL bState = m_pWBRectMaqn->MouseDown(point);
		if (MAQN_NEED_REFRESH == bState)
		{
			Invalidate(TRUE);
			return TRUE;
		}
		else if (TRUE == bState)
		{
			return TRUE;
		}

	}
	return FALSE;
}
//执行左键移动时放大镜的相关处理
BOOL CWhiteBoardView::MouseMoveMagn(const CPoint& point)
{
	if (NULL !=m_pWBRectMaqn)
	{
		BOOL bState = m_pWBRectMaqn->MouseMove(point);
		if (MAQN_NEED_REFRESH == bState)
		{
			CRect WBDlgRect;
			::GetWindowRect(this->m_hWnd,&WBDlgRect);
			int x,y;
			x = ::GetSystemMetrics(SM_CXVSCROLL);
			y = ::GetSystemMetrics(SM_CYHSCROLL);
			WBDlgRect.left = WBDlgRect.left + 2;
			WBDlgRect.top = WBDlgRect.top + 2;
			WBDlgRect.right = WBDlgRect.right - x - 2;
			WBDlgRect.bottom = WBDlgRect.bottom - y - 2;
			CPoint ptcursor;
			GetCursorPos(&ptcursor);
			if (WBDlgRect.PtInRect(ptcursor))
			{
				ClipCursor(&WBDlgRect);	
				m_bLimitCursorArea = true;
			}
			else
			{
				ClipCursor(NULL);
				m_bLimitCursorArea = false;
			}
			//SendMagn();
			Invalidate(TRUE);
			return TRUE;
		}
		else if (TRUE == bState)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//执行左键弹起时放大镜的相关处理
BOOL CWhiteBoardView::MouseUpMagn(const CPoint& point)
{
	if (NULL != m_pWBRectMaqn)
	{
		BOOL bState = m_pWBRectMaqn->MouseUp(point);
		if(MAQN_NEED_REFRESH == bState)
		{
			SendMagn();
			Invalidate(TRUE);
			return TRUE;
		}
		else if (TRUE == bState)
		{	SendMagn();
		return TRUE;
		}
	}
	return FALSE;
}

void CWhiteBoardView::AddRECScreen()
{
	if (NULL == m_MedioVideoScreenDlg)
	{
		//判断是在avcon下创建，还是全屏创建的
		POINT pt;
		::GetCursorPos(&pt);
		MONITORINFO oMonitor = {}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CRect rcWork = oMonitor.rcWork;
		int xScreen = rcWork.right;
		int yScreen = rcWork.bottom;
		CRect wbRect;
		this->GetWindowRect(&wbRect);
		if (wbRect.Width() == xScreen || wbRect.Height() == yScreen)
		{
			m_isFullScreen = TRUE;
		}
		else
		{
			m_isFullScreen = FALSE;
		}
		////
		m_MedioVideoScreenDlg = new CMedioVideoScreenDlg;//无父窗口
		m_MedioVideoScreenDlg->Create(IDD_MEDIOVEDIOSCREEN_DIALOG,this);
		//	CRect rect;
		//	m_MedioVideoScreenDlg->GetWindowRect(&rect);
		//	m_MedioVideoScreenDlg->MoveWindow(300,300,rect.Width(),rect.Height());
		if (m_RECScreenIsShow )
		{
			m_MedioVideoScreenDlg->ShowWindow(SW_SHOW); 
		}
		else
		{	
			m_RECScreenIsShow = TRUE;
			m_MedioVideoScreenDlg->ShowWindow(SW_HIDE); 
		}

	}
	else
	{
		delete m_MedioVideoScreenDlg;
		m_MedioVideoScreenDlg = NULL;
	}
	return;
}


void CWhiteBoardView::AddSpotlight()//聚光灯
{
	if (NULL == m_WBSpotlight)
	{
		CRect slientrect;
		GetClientRect(&slientrect);
		//GetClientRect(&slientrect);
		((CWhiteBoardDlg*)g_whiteboardDlg)->GetClientRect(&slientrect);
		CRect screenRect;
		
		if(g_reduce>0.00)
		{
			
			screenRect.top = g_reduce*slientrect.Height()/4;
			screenRect.bottom = g_reduce*slientrect.Height()*2/3;
			screenRect.left = g_reduce*slientrect.Width()/3;
			screenRect.right = g_reduce*slientrect.Width()*2/3;


			screenRect.top=screenRect.top+100;
			screenRect.left=screenRect.left+100;
			screenRect.right=screenRect.right+100;
			screenRect.bottom=screenRect.bottom+100;




		}
		else
		{
			screenRect.top = slientrect.Height()/4;
			screenRect.bottom = slientrect.Height()*2/3;
			screenRect.left = slientrect.Width()/3;
			screenRect.right = slientrect.Width()*2/3;
		}
		if (!CreateSpotlight(screenRect))
		{
			OutputDebugString(_T("Create spot light failed!"));
			return;
		}

		//假如标注窗口打开，那么先关闭标注窗口
		if (g_bIsDrawScreen)
		{
			m_bIsTranspartentOrNot = WB_IsScreenDrawTrans();
		}
		if (DestroyScreenDraw())
		{
			m_bIsCloseScreenDlg = TRUE;
			//发送同步信息
			SendScreenDraw();
		}

		CRect viewRect;
		GetClientRect(&viewRect);
		if(g_reduce>0.00)
		{
			double nViewWidth=(double)WB_VIEWHSCROLL*g_reduce;
			double nViewHight=(double)WB_VIEWVSCROLL*g_reduce;

			CRect rc;
			g_whiteboardDlg->GetClientRect(&rc);
			double nWindowWidth;
			double nWindowHight=rc.bottom;
			//此处判断功能面板是否打开
			if(((CWhiteBoardDlg*)g_whiteboardDlg)->GetSinFuncShow())
			{
				nWindowWidth=rc.right-(WB_FUNCTION_DLG_WIDTH + WB_OPTIONDLG_WIDTH);

			}
			else
			{
				if(((CWhiteBoardDlg*)g_whiteboardDlg)->GetSinOptShow())
				{
					nWindowWidth=rc.right- WB_OPTIONDLG_WIDTH;


				}
				else
				{
					nWindowWidth=rc.right;

				}
			}
			if((nViewWidth<nWindowWidth)&&(nViewHight<nWindowHight))
			{
			    viewRect.top=(nWindowHight-nViewHight)/2;//先缩放后打开聚光灯，进行位置的调整
				viewRect.left=(nWindowWidth-nViewWidth)/2-10;
				viewRect.right=viewRect.left+nViewWidth;
				viewRect.bottom=viewRect.top+nViewHight-10;

			}
		}
		
		m_WBSpotlight->AdjustSize(viewRect,false);
		//设置对象的ID
		DWORD dwObjID = (DWORD)GenerateSSRC();
		m_WBSpotlight->SetLayerIndex(dwObjID);
		XMemIni xmlObj;
		m_WBSpotlight->SetXmlValue(xmlObj);
		AddObj(xmlObj);

	}
	else
	{
		CloseSpotlight();
	}
}
BOOL CWhiteBoardView::CreateSpotlight(CRect screenRect /*= NULL*/)
{
	m_WBSpotlight = new CWBSpotlight(screenRect);
	ASSERT(m_WBSpotlight);
	CRect slientrect;
	GetClientRect(&slientrect);
	HWND hwnd = ((CWhiteBoardDlg*)g_whiteboardDlg)->m_hWnd;
	if (!m_WBSpotlight->CreateSpotlight(slientrect, hwnd))
	{
		delete m_WBSpotlight;
		m_WBSpotlight = NULL;
		return FALSE;
	}

	return TRUE;
}

void CWhiteBoardView::CloseSpotlight()
{
	if (NULL != m_WBSpotlight)
	{
		//设置事件ID
		m_ulSendEventID = GenerateSSRC();
		//发送删除消息
		RemoveObj(m_WBSpotlight->GetLayerIndex());
		delete m_WBSpotlight;
		m_WBSpotlight = NULL;

		if ( m_bIsCloseScreenDlg)
		{
			m_bIsCloseScreenDlg = FALSE;
			if (DrawOnScreen())
			{
				ShiftGuise(m_bIsTranspartentOrNot);
				//发送同步信息
				SendScreenDraw();
			}
		}
	}
}

void CWhiteBoardView::SendSpotlight()
{
	XMemIni xmlObj;
	xmlObj.Set(KEY_OBJ_ID, 0);
	xmlObj.Set(KEY_OBJ_TYPE, 0);
	xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
	float fScrollScaleX = (float)(g_pWBPage->GetPageViewScroll().x);
	float fScrollScaleY = (float)(g_pWBPage->GetPageViewScroll().y);
	fScrollScaleX = fScrollScaleX/m_fWbRatioX;
	fScrollScaleY = fScrollScaleY/m_fWbRatioY;
	xmlObj.Set(KEY_PAGEINDEX, (int)(g_pWBFile->GetCurPageIndex()));
	xmlObj.Set(KEY_PAGE_SCROLL_XY, g_pWBPage->GetPageViewScroll().x);
	xmlObj.Set(KEY_PAGE_SCROLL_YX, g_pWBPage->GetPageViewScroll().y);
	xmlObj.Set(KEY_PAGE_SCROLL_X, fScrollScaleX);
	xmlObj.Set(KEY_PAGE_SCROLL_Y, fScrollScaleY);
	xmlObj.Set(KEY_MOVE_EAGEL_EYE,_E_MOVE_EAGEL_EYE);//鹰眼撤销操作命令
	xmlObj.Set(KEY_BROADCAST_CMD,_E_BROAD_UNDO_REDO_SPOLITE);//鹰眼撤销操作命令
	Broadcast_Data(xmlObj);
	//ChangeObj(xmlObj);

	XMemIni xmlobj;
	m_WBSpotlight->SetXmlValue(xmlobj);
	ChangeObj(xmlobj);
}
void CWhiteBoardView::EagleEyeFunction()
{
	if (NULL ==m_pEagleEyeDlg)
	{
		if (!CreateEagleEye())
		{
			OutputDebugString(_T("Create Eagle Eye failed!"));
			return;
		}
		//假如标注窗口打开，那么先关闭标注窗口
		if (DestroyScreenDraw())
		{
			m_bIsCloseScreenDlg = TRUE;
			//发送同步信息
			SendScreenDraw();
		}
	}
	else
	{
		int nSW_Type = m_pEagleEyeDlg->IsWindowVisible() ? SW_HIDE : SW_SHOW;
		m_pEagleEyeDlg->ShowWindow(nSW_Type);
	}
}
void CWhiteBoardView::EagleEyeSetposAndShow( CRect rect )
{
	((CWhiteBoardDlg*)GetParent())->SetAndShowEagleEye(rect);
	m_pEagleEyeDlg = ((CWhiteBoardDlg*)GetParent())->GetWBEagleEyeDlg();
}
BOOL CWhiteBoardView::CreateEagleEye()
{
	CRect rect;
	GetClientRect(&rect);
	BOOL bFalse = ((CWhiteBoardDlg*)GetParent())->SetAndShowEagleEye(rect);
	m_pEagleEyeDlg = ((CWhiteBoardDlg*)GetParent())->GetWBEagleEyeDlg();
	return bFalse;
}

BOOL CWhiteBoardView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return TRUE;
}

CWBRichEdit* CWhiteBoardView::GetCurEdit()
{
	if (NULL != CWBRichEditMgr::GetInst_()->GetRichEdit())
	{
		return CWBRichEditMgr::GetInst_()->GetRichEdit();
	} 
	else
	{
		return NULL;
	}
}

CWBPromptFontSetDlg *CWhiteBoardView::GetPromptFontSetDlg()
{
	if (NULL != CWBRichEditMgr::GetInst_()->GetRichEditPrompt())
	{
		return CWBRichEditMgr::GetInst_()->GetRichEditPrompt();
	} 
	else
	{
		return NULL;
	}
}

void CWhiteBoardView::SetCurEdit(CWBRichEdit *pEdit)
{
	if (pEdit != NULL)
	{
		CWBRichEditMgr::GetInst_()->SetRichEdit(pEdit);
	}
}

void CWhiteBoardView::SetEditCharFormat(CWBRichEdit *pEdit)
{
	if (NULL != pEdit)
	{
		//字体样式luowei
		CHARFORMAT cf;
		ZeroMemory(&cf, sizeof(CHARFORMAT));
		(pEdit->GetRichEditCtrl()).GetSelectionCharFormat(cf);
		cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
		cf.dwEffects = 0;
		if (g_isBold)
		{
			cf.dwEffects += CFE_BOLD;
		}
		if (g_isItalic)
		{
			cf.dwEffects += CFE_ITALIC;
		}
		if (g_isUnderline)
		{
			cf.dwEffects += CFE_UNDERLINE;
		}
		cf.crTextColor = g_clrWBCurObjColor;
		long lLenght = pEdit->GetTextLength();
		long nStartChar, nEndChar;
		(pEdit->GetRichEditCtrl()).GetSel(nStartChar, nEndChar);
		(pEdit->GetRichEditCtrl()).SetSel(0, lLenght);//
		(pEdit->GetRichEditCtrl()).SetSelectionCharFormat(cf);
		(pEdit->GetRichEditCtrl()).SetSel(nEndChar, nEndChar);
		(pEdit->GetRichEditCtrl()).SetSel(-1, -1);
	}
}

CRect CWhiteBoardView::GetEditRect()
{
	return m_rectText;
}

void CWhiteBoardView::SetEditRect(CRect rectText)
{
	m_rectText = rectText;
}

void CWhiteBoardView::SetEditTopLeft(CPoint ptTopLeft)
{
	m_topLeft = ptTopLeft;
}

BOOL CWhiteBoardView::GetIsEditing()
{
	return m_bIsEditing;
}

CWBObject *CWhiteBoardView::GetEditObj()
{
	return m_pEditObj;
}

void CWhiteBoardView::SetCurTextFont()
{
	if (CWBRichEditMgr::GetInst_()->GetRichEdit() != NULL)
	{
		(CWBRichEditMgr::GetInst_()->GetRichEdit())->SetCurTextFont();
	}
}

void CWhiteBoardView::SetCurTextClr()
{
	if (CWBRichEditMgr::GetInst_()->GetRichEdit() != NULL)
	{
		//(CWBRichEditMgr::GetInst_()->GetRichEdit())->SetFontColor(g_clrWBCurObjColor);
	}
}

void CWhiteBoardView::AllUnLock()
{
	POSITION pos;
	CWBObject *pObj = NULL;
	CObList *pObjList, *pLabelObjList;
	unsigned long ulEventID = GenerateSSRC();

	if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
	{
		pLabelObjList = g_pWBPage->GetWBLabelObjList();
		if (pLabelObjList == NULL)
		{
			return;
		}
		for (int nIndex = 0; nIndex < pLabelObjList->GetCount(); ++nIndex )
		{
			pos = pLabelObjList->FindIndex(nIndex);
			if (pos == NULL)
			{
				continue;
			}
			pObj = (CWBObject *)pLabelObjList->GetAt(pos);
			if (pObj == NULL)
			{
				continue;
			}
			if (pObj->IsLock())
			{
				pObj->Lock(FALSE);

				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}
		}
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	else
	{
		pObjList = g_pWBPage->GetWBObjList();
		if (pObjList == NULL)
		{
			return;
		}
		for (int nIndex = 0; nIndex < pObjList->GetCount(); ++nIndex )
		{
			pos = pObjList->FindIndex(nIndex);
			if (pos == NULL)
			{
				continue;
			}
			pObj = (CWBObject *)pObjList->GetAt(pos);
			if (pObj == NULL)
			{
				continue;
			}
			if (pObj->IsLock())
			{
				pObj->Lock(FALSE);

				//发送同步消息
				XMemIni xmlObj;
				pObj->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_EVENT_ID, ulEventID);
				ChangeObj(xmlObj);
			}
		}
		Invalidate();
	}
}

CRect CWhiteBoardView::GetEditTextRect()
{
	CWBText *pText = NULL;
	CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
	if (pObj->GetType() == WB_OBJ_ANNOTATIONS)
	{
		pText = ((CWBAnnotations*)pObj)->GetText();
	} 
	else
	{
		pText = (CWBText*)pObj;
	}
	PWBTextItem pTextItem = new WBTextItem;
	pText->GetItem(pTextItem);
	m_rectText = pTextItem->rectText;//得到文本对象的矩形框

	m_topLeft = m_rectText.TopLeft();

	//编辑框最小 100 x 100
	int nWidth = m_rectText.right - m_rectText.left;
	nWidth = nWidth > 100 ? nWidth : 100;
	int nHeight = m_rectText.bottom - m_rectText.top;
	nHeight = nHeight > 100 ? nHeight : 100;

	if (pText->GetIsVertical())
	{
		int nTemp;
		nTemp = m_rectText.right;
		m_rectText.right = m_rectText.left;
		m_rectText.left = nTemp;
		m_rectText.left = (long)(m_rectText.right - 2 * nWidth);
		m_rectText.bottom = (long)(m_rectText.top + 1.2 * nHeight);
	} 
	else
	{
		m_rectText.right = (long)(m_rectText.left + 1.2 * nWidth);
		m_rectText.bottom = m_rectText.top + 2 * nHeight;
	}
	g_bIsVertical = pText->GetIsVertical();
	delete pTextItem;
	return m_rectText;
}

/*************************************************************************
*                    CWhiteBoardView::OnGesture							 *
*                         屏蔽系统手势处理								*
*************************************************************************/
LRESULT CWhiteBoardView::OnGesture( WPARAM /*wParam*/, LPARAM lParam )
{
	return S_OK;
}


LRESULT CWhiteBoardView::OnTouch( WPARAM wParam, LPARAM lParam )
{
#ifndef SUPPORTMT

	if (g_bAvconLock) 
	{
		if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(lParam))
		{
			OutputDebugString(_T("warning, the touch input handle was not closed successfully!"));
		}
		return S_OK;
	}

	int iNumContacts = LOWORD(wParam);
	HTOUCHINPUT hInput = (HTOUCHINPUT)lParam;

#ifdef _DEBUG
	PTOUCHINPUT pInputs = new TOUCHINPUT[iNumContacts];
#else
	PTOUCHINPUT pInputs = new (std::nothrow) TOUCHINPUT[iNumContacts];
#endif


	if(pInputs != NULL)
	{
		//获取每个触摸点信息，然后分发到各个处理容器中。
		//if(GetTouchInputInfo(hInput, iNumContacts, pInputs, sizeof(TOUCHINPUT)))
		if( CEncapsulateTouchAPI::U_GetTouchInputInfo(lParam, iNumContacts, pInputs, sizeof(TOUCHINPUT)) )
		{
			for(int i = 0; i < iNumContacts; ++i)
			{
				//////转换成客户区坐标
				POINT ptInputs;

				ptInputs.x = TOUCH_COORD_TO_PIXEL(pInputs[i].x);
				ptInputs.y = TOUCH_COORD_TO_PIXEL(pInputs[i].y);

				//ClientToScreen(&ptInputs);
				// 				CClientDC dc(this);
				//				OnPrepareDC(&dc);//进行坐标原点的匹配
				// 				dc.DPtoLP(&ptInputs);//将视图坐标转换为文档作标
				pInputs[i].x = ptInputs.x;
				pInputs[i].y = ptInputs.y;

				if ((pInputs[i].dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)// 触摸按下事件
				{
					if (!g_bAvconLock) //如果会议中白板功能没有被锁定
					{
						std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
						if (m_mapTouchCursor.end() != it)
						{
							delete it->second;
							m_mapTouchCursor.erase(it);
						}
						static long session_id = 1;
						MTCursor *pCursor = new MTCursor(MTTime::getSessionTime(),++session_id, pInputs[i].dwID, (float)pInputs[i].x, (float)pInputs[i].y);
						m_mapTouchCursor.insert(std::pair<DWORD, MTCursor*>(pInputs[i].dwID, pCursor));
						CWBMTObjectManager::GetIns_()->addMTCursor(pCursor);
					}
				}               
				else if ((pInputs[i].dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)// 触摸移动事件
				{
					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
					if (m_mapTouchCursor.end() != it)
					{
						it->second->update(MTTime::getSessionTime(),(float)pInputs[i].x, (float)pInputs[i].y);
						CWBMTObjectManager::GetIns_()->updateMTCursor(it->second); 
					}
				}               
				else if ((pInputs[i].dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)// 触摸弹起事件
				{
					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
					if (m_mapTouchCursor.end() != it)
					{
						it->second->update(MTTime::getSessionTime(),(float)pInputs[i].x, (float)pInputs[i].y);
						CWBMTObjectManager::GetIns_()->removeMTCursor(it->second);
						delete it->second;
						m_mapTouchCursor.erase(it);
					}
					if (m_bIsDBClick && m_mapTouchCursor.empty())
					{
						m_bIsDBClick = false;
						CWBObject* pObj = NULL;
						pObj = g_pWBPage->FindByLayerIndex(m_dDBClickobjlayer);
						if (pObj == NULL)
						{
							return S_OK;
						}
// 						int nIndex;
// 						POSITION pos;
// 						CWBObject *pSleObj = NULL;
// 						CObList *pPageList = g_pWBPage->GetWBList();
// 						bool bfind = false;
// 						if (pPageList != NULL)
// 						{
// 							for (nIndex =0;nIndex<pPageList->GetCount();++nIndex)
// 							{
// 								pos = pPageList->FindIndex(nIndex);
// 								if (pos == NULL)
// 								{
// 									continue;
// 								}
// 								pSleObj = (CWBObject *)pPageList->GetAt(pos);
// 								if (pSleObj == NULL)
// 								{
// 									continue;
// 								}
// 								if (pSleObj->IsBkg())
// 								{
// 									continue;
// 								}
// 								if (pSleObj->GetType() == WB_OBJ_DOCUMENT)
// 								{
// 									if (((CWBDocument *)pSleObj)->GetIsConvertTip())
// 									{
// 										continue;
// 									}
// 								}
// 								if (pSleObj->GetLayerIndex() == pObj->GetLayerIndex())
// 								{
// 									g_pWBPage->DeleteSelectFrame(pObj->GetLayerIndex());
// 									pPageList->RemoveAt(pos);							
// 									CWBMTObjectManager::GetIns_()->RemoveMTObject(pObj);
// 									bfind = true;
// 									Invalidate();
// 									UpdateWindow();
// 								}
// 							}
// 
// 						}
// 						if (bfind)
						{
							CWBPicture* pWBpic = NULL;
							if (pObj->GetType() == WB_OBJ_PICTURE)
							{
								pWBpic = new CWBPicture(m_strDBClickSavePath,((CWBPicture*)pObj)->m_pItem->rect);
								if (pWBpic == NULL)
								{
									delete pWBpic;
									pWBpic = NULL;
								}
								pWBpic->CopyAttribute(pObj);
								SetWBPictureEffect((CWBPicture *)pWBpic);
								CRect rcResGaller;
								((CWhiteBoardDlg*)g_whiteboardDlg)->GetResGalleryRect(rcResGaller);
								CPoint ptScren(rcResGaller.right,rcResGaller.top);
								CRect rcWBdlg;
								g_whiteboardDlg->GetWindowRect(&rcWBdlg);
								bool bgoright = true;
								int x = ((CWBPicture *)pObj)->m_pItem->rect.right;
								int y =rcWBdlg.right - rcWBdlg.Width()/3;
								x = x - ((CWBPicture *)pObj)->m_pItem->rect.Width()/2;
								if (x > y)
								{
									bgoright = false;
								}
								ProcessMediaPhotoEffect(ptScren,bgoright);
								if (m_strResourceUpdataPath.IsEmpty())
								{
									CString strFilename;
									CString strReadName;
									strReadName = ReadConfigText(_T("WhiteBoardMenuListText"),_T("MediaPhoto"));
									if (strReadName.IsEmpty())
									{
										strReadName = _T("视频拍照");
									}
									strFilename = g_strMediaPhoto + _T("\\") + strReadName;
									m_strResourceUpdataPath = strFilename;
								}
								WBRESWND::SwitchSpecialDirectoryEx(m_strResourceUpdataPath);
							}

							
						}
					}
				}
			}
		}
		if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(lParam))
		{
			OutputDebugString(_T("warning, the touch input handle was not closed successfully!"));
		}
	}

	delete [] pInputs;
#endif
	return S_OK;
}



void CWhiteBoardView::OnCallbackGesture( const unsigned int &nMessage, const unsigned int &nAppendMessage )
{	
	///如果不是当前状态不是选择，则不做任何处理
	if (g_nDrawStatus != DRAW_NORMAL)
	{
		return;
	}
	WORD lWord = LOWORD(nMessage);
	if (TGT_DIRECTION == lWord)
	{
		if( (MTOG_RIGHT == nAppendMessage)
			|| (MTOG_DOWN == nAppendMessage) )
		{
			NextPage();
		}
		else if( (MTOG_LEFT == nAppendMessage)
			|| (MTOG_UP == nAppendMessage) )
		{
			PrePage();
		}
	}
}


void CWhiteBoardView::OnGestureCombineHandle( const GestureCombinePar &gesPar )
{
	///老的手势库
	//暂不执行
	//if (GetTouchFingleNum() >= 2)
	//{
	//	if( 2 < (abs(gesPar.xtrans) + abs(gesPar.ytrans)) )
	//	{
	//		CPoint pt = GetScrollPosition();
	//		pt.y = max(pt.y - gesPar.ytrans, 0);
	//		pt.x = max(pt.x - gesPar.xtrans, 0);
	//		ScrollToPosition(pt);
	//	}
	//}
}

void CWhiteBoardView::SetRectSelect(CRect rectSelect)
{
	m_rectSelect.left = rectSelect.left;
	m_rectSelect.right = rectSelect.right;
	m_rectSelect.top = rectSelect.top;
	m_rectSelect.bottom = rectSelect.bottom;
}

unsigned int CWhiteBoardView::GetMapSizeOfSpecialDrawTypeObj(int nDrawType)
{
	unsigned int nSize = 0;
	std::map<DWORD, MTDrawPar*>::const_iterator it = m_mapMTDrawPar.cbegin();
	while ( m_mapMTDrawPar.cend() != it )
	{
		if ( nDrawType == it->second->GetType() )
		{
			++nSize;
		}
		++it;
	}
	return nSize;
}

void CWhiteBoardView::ClearAllMTDrawMap()
{
	if (m_mapMTDrawPar.empty())
		return ;
	else
		BroadCostReleastLimitObjs();

	std::map<DWORD, MTDrawPar*>::iterator itDrawPar = m_mapMTDrawPar.begin();
	while( itDrawPar != m_mapMTDrawPar.end() )
	{
		delete itDrawPar->second;
		++itDrawPar ;
	}
	m_mapMTDrawPar.clear();
	if (!m_map.empty())
	{
		m_map.clear();
	}
}

void CWhiteBoardView::ClearMTDrawMap( const DWORD &dwTouchID )
{
	std::map<DWORD, MTDrawPar*>::iterator itDrawPar = m_mapMTDrawPar.find(dwTouchID);
	if (m_mapMTDrawPar.end() != itDrawPar)
	{
		delete itDrawPar->second;
		m_mapMTDrawPar.erase(itDrawPar);
		m_map.erase(itDrawPar->first);
	}
}


HRESULT CWhiteBoardView::OnReceiveTouchResouce( WPARAM wPar, LPARAM lPar )
{
	LPCTSTR sPath = (LPCTSTR)(wPar);
	CPoint point(GET_X_LPARAM(lPar), GET_Y_LPARAM(lPar));
	ScreenToClient(&point);
	CPoint ptSource = point;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);
	if (PathFileExists(sPath))
	{
		std::string strExt = ToStr(PathFindExtension(sPath));
		std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);
		int nType = WBRESWND::GetResType(strExt.c_str());
		switch(nType)
		{
		case  WBRES_PICTURE:
			{
				//清除所有创建的选择框
				g_pWBPage->ClearSelectFrameList();

				//判断文件的文件夹是否是特殊的；
				CString strPath = sPath;
				strPath = strPath.Left(strPath.ReverseFind('\\'));
				strPath = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') -1);
				bool bSpecpic = false;
				CRect rcExpand = CRect(0,0,0,0);
				if (strPath.Compare(_T("视频拍照")) == 0/*||strPath.Compare(_T(""))*/)
				{
					bSpecpic = true;
					GetClientRect(&rcExpand);
					rcExpand.right = rcExpand.left + rcExpand.Width()*2/3;
					rcExpand.bottom = rcExpand.top + rcExpand.Height()*2/3;
				}
				//创建一个新对象
				CWBPicture *pWBPicture = new CWBPicture(sPath/*,rcExpand*/);			
				pWBPicture->SetAlpha((int)g_nTranspar);
				CPoint ptCenter;
				//获得对象中心点
				pWBPicture->GetCenterPoint(ptCenter);
				CRect viewRect;
				GetClientRect(&viewRect);
				if (bSpecpic)
				{
					CPoint pt(viewRect.Width()/2+getXnPos(),viewRect.Height()/2+getYnPos() -20);
					pWBPicture->MoveOffset(pt - ptCenter);
					pWBPicture->SetIsPrintScreen();
					pWBPicture->InitCom();
				}
				else
				{
					pWBPicture->MoveOffset(point - ptCenter);
					//控制图片的最大不能超出view的大小4-21金鹏
// 					if(rect.Width() > viewRect.Width() ||rect.Height() > viewRect.Height())
// 					{
// 
// 					}
// 					pWBPicture->SetPictureBigerSzie(viewRect);
				}
				
				g_pWBPage->AddWBObject(pWBPicture);

				if (m_pResourcePic)
				{
					//暂时不要，以后再说
// 					std::map<CString,VECTORSPECPIC>::iterator it = m_pResourcePic->find(sPath);
// 					if (it != m_pResourcePic->end())
// 					{
// 						VECTORSPECPIC vec = it->second;
// 						vec.push_back(pWBPicture->GetLayerIndex());
// 						it->second = vec;
// 						
// 					}
// 					else
// 					{
// 						VECTORSPECPIC vec;
// 						vec.push_back(pWBPicture->GetLayerIndex());
// 						m_pResourcePic->insert(pair<CString,VECTORSPECPIC>(sPath,vec));
// 					}
					
				}
				
				//m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
				//创建选择框，并移动对象到当前位置
				if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
				{
					m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
				}
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBPicture->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			break;
		case WBRES_FLASH:
			{
				//清除所有创建的选择框
				g_pWBPage->ClearSelectFrameList();
				//创建一个新对象
				CWBFlash *pWBFlash = new CWBFlash(sPath);			
				pWBFlash->SetAlpha((int)g_nTranspar);
				pWBFlash->init();

				//获得对象中心点
				CPoint ptCenter;
				pWBFlash->GetCenterPoint(ptCenter);
				pWBFlash->MoveOffset(point - ptCenter);
				g_pWBPage->AddWBObject(pWBFlash);
				//m_moveFirstPoint = ptCenter;//将移动起始点设为中心点

				//创建选择框，并移动对象到当前位置		
				if (g_pWBPage->AddSelectFrame(pWBFlash->GetLayerIndex()))
				{
					m_dwCurSelectObjLayerIndex = pWBFlash->GetLayerIndex();
				}
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBFlash->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			break;
		case WBRES_MOVIE:
			{
				//只能主动添加一个视频
				int nPage = g_pWBFile->GetPageCount();
				if (nPage >= 1)
				{
					for (int i =1 ; i <= nPage; ++i)
					{
						CWBPage *pWBPage = g_pWBFile->GetPage(i); 
						CObList* nMediaCameraList = pWBPage->GetMediaCameraList();
						if (nMediaCameraList->GetSize() > 0)
						{
							int nIndex;
							CWBObject * pObj = NULL;
							POSITION pos = NULL;
							for (nIndex = 0; nIndex < nMediaCameraList->GetSize(); ++nIndex)
							{
								pos = nMediaCameraList->FindIndex(nIndex);
								if (pos == NULL)
								{
									continue;
								}
								pObj = (CWBObject *)nMediaCameraList->GetAt(pos);

								if (pObj->GetType() == WB_OBJ_MEDIA &&((CWBMedia*)pObj)->IsActiveCreated())
								{
									CString strSupportOneMedia;
									strSupportOneMedia = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SupportOneMedia"));
									if (strSupportOneMedia.IsEmpty())
									{
										strSupportOneMedia = _T("目前支持主动添加一个视频");
									}
									if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
									{
										//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,strSupportOneMedia, MB_OK,true);
										g_pHPWBDataNotify->ShowTipWindow(strSupportOneMedia);
									}
									else
									{
										AfxMessageBox(strSupportOneMedia,MB_OK);
									}
									g_nDrawStatus = DRAW_NORMAL;
									return S_FALSE;
								}
								else
								{
									continue;
								}
							}
						}
					}
				}
				//清除所有创建的选择框
				g_pWBPage->ClearSelectFrameList();
				//创建一个新对象
				CRect WhiteBoardClientRt;
				((CWhiteBoardDlg*)GetParent())->GetWindowRect(&WhiteBoardClientRt);

				CRect rct = CRect( 74, 43, 580, 450 );
				rct.MoveToXY(ptSource.x - rct.Width()/2 , ptSource.y - rct.Height()/2);

				
				CString filepath = sPath;
				if (m_MedioVideoScreenDlg)
				{
					CString str = m_MedioVideoScreenDlg->GetCurrentFilePath();
					if (str.Compare(filepath) == 0)
					{
						if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
						{
							//g_pHPWBDataNotify->MessageBox(NULL,m_strPrompt,_T("该文件正在录制，不能打开！"), MB_OK,true);
							g_pHPWBDataNotify->ShowTipWindow(_T("该文件正在录制，不能打开！"));
						}
						else
						{
							AfxMessageBox(_T("该文件正在录制，不能打开！"),MB_OK);
						}
						break;
					}
				}
				CWBMedia *pWBMedia = new CWBMedia(filepath, rct,TRUE);
				//获得对象中心点
				CPoint ptCenter;
				pWBMedia->GetCenterPoint(ptCenter);
				pWBMedia->MoveOffset(ptSource - ptCenter);

				g_pWBPage->AddWBObject(pWBMedia);

				//创建选择框，并移动对象到当前位置		
				if (g_pWBPage->AddSelectFrame(pWBMedia->GetLayerIndex()))
				{
					m_dwCurSelectObjLayerIndex = pWBMedia->GetLayerIndex();
				}

				Invalidate();

				XMemIni xmlObj;
				xmlObj.Set(KEY_MEDIA_ADDSESSION,g_ulSessionID);
				pWBMedia->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			break;
		case WBRES_DOC:
			{
				if (m_bIsDocConvert) //文档还在转换过程中
				{
					CRect clientRC;
					((CWhiteBoardDlg*)GetParent())->GetClientRect(&clientRC);
					ClientToScreen(&clientRC);

					CDialogEx* pDialog = new CbuttonCloseDlg();
					pDialog->Create(IDD_DOCCONVERTTIP, this);
					pDialog->MoveWindow((clientRC.CenterPoint()).x - 120, (clientRC.CenterPoint()).y - 50, 257, 105, TRUE);
					pDialog->SetBackgroundImage(IDB_DOCCONVERTTIP);
					pDialog->ShowWindow(SW_SHOW);

					Sleep(3000);

					delete pDialog;
					pDialog = NULL;

					g_nDrawStatus = DRAW_NORMAL;
					break;
				}
				//清除所有创建的选择框
				g_pWBPage->ClearSelectFrameList();
				//创建一个新对象
				CWBDocument *pWBDocument = new CWBDocument(sPath);
				if (NULL == pWBDocument)
				{
					break;
				}
				if (g_bSupportU7OpenFile)
				{
					CreateFullScreen(pWBDocument);
				}
				pWBDocument->SetAlpha((int)g_nTranspar);
				g_pWBPage->AddWBObject(pWBDocument);

				int nPageIndex = (int)((g_pWBFile->GetCurPageIndex()));
				pWBDocument->SetPageIndex(nPageIndex);

				//获得对象中心点
				CPoint ptCenter;
				pWBDocument->GetCenterPoint(ptCenter);
				pWBDocument->MoveOffset(point - ptCenter);

				//创建选择框，并移动对象到当前位置		
				if (g_pWBPage->AddSelectFrame(pWBDocument->GetLayerIndex()))
				{
					m_dwCurSelectObjLayerIndex = pWBDocument->GetLayerIndex();
				}
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBDocument->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			break;
		case WBRES_DXF:
			{
				g_pWBPage->ClearSelectFrameList();
				//创建一个DXF新对象
				CWBDxf *pWBDxf = new CWBDxf(sPath);
				if (pWBDxf == NULL)
				{
					break;
				}

				pWBDxf->SetAlpha((int)g_nTranspar);
				pWBDxf->SetIsNeedReDraw(); //判断是否重画DXF

				//获得对象中心点
				CPoint ptCenter;
				pWBDxf->GetCenterPoint(ptCenter);
				pWBDxf->MoveOffset(point - ptCenter);
				g_pWBPage->AddWBObject(pWBDxf, WB_LAYER_NORMAL);
		
				//创建选择框，并移动对象到当前位置		
				if (g_pWBPage->AddSelectFrame(pWBDxf->GetLayerIndex()))
				{
					m_dwCurSelectObjLayerIndex = pWBDxf->GetLayerIndex();
				}
				Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBDxf->SetXmlValue(xmlObj);
				AddObj(xmlObj);
			}
			break;
		default:
			break;
		}
	}
	///当从资源栏中拖拽对象到view上时将状态设为normal
	g_nDrawStatus = DRAW_NORMAL;
	return S_OK;
}

afx_msg LRESULT CWhiteBoardView::OnProcessScreenDraw(WPARAM wParam, LPARAM lParam)
{
	BOOL bIsDrawScreen;
	BOOL bIsTransparent;
	//	BOOL bIsFullScreen;
	int nAlpha;
	XMemIni* pXMemIni = (XMemIni*)wParam;
	(*pXMemIni).Get(KEY_SCREENDRAW, bIsDrawScreen);
	(*pXMemIni).Get(KEY_SCREENDRAW_GUISE, bIsTransparent);
	//	m_XMemIni.Get(KEY_SCREENDRAW_FULLSCREEN, bIsFullScreen);
	(*pXMemIni).Get(KEY_SCREENDRAW_ALPHA, nAlpha);

	if (bIsDrawScreen)
	{
		if (!g_bIsDrawScreen)
		{
			DrawOnScreen();
		}
		ShiftGuise(bIsTransparent);
		m_bIsSDTransparent = bIsTransparent;
		//		FullScreenShift(bIsFullScreen);
		if (CScreenDrawMgr::GetIns())
		{
			CScreenDrawMgr::GetIns()->AlphaChange(nAlpha);
		}
	} 
	else
	{
		DestroyScreenDraw();
		m_bIsOpenSCDraw = FALSE;
	}
	delete pXMemIni;
	pXMemIni = NULL;
	return 0;
}

void CWhiteBoardView::SetResFilePath()//外部拖拽进白板的对象的地址
{
	m_sLastDragFileURL = ((CWhiteBoardDlg*)GetParent())->GetCurFilePath();
}

void  CWhiteBoardView::SetComObj(CWBObject *pObj, DWORD dwLayerIndex)
{
	m_pComObj = pObj;
	m_dwComLayerIndex = dwLayerIndex;
}
DWORD CWhiteBoardView::GetComLayerIndex()
{
	return m_dwComLayerIndex;
}

UINT CWhiteBoardView::GetPrePage()
{
	return m_nPrePage;
}

void CWhiteBoardView::SetPrePage(int nPage)
{
	m_nPrePage = nPage;
}

void  CWhiteBoardView::SetCurrentCursorPos()
{
	CRect viewRectInScreen;
	GetWindowRect(&viewRectInScreen);//得到白板在屏幕上的位置
	GetCursorPos(&m_CurrentCursorPos);//鼠标在屏幕中的坐标
	m_CurrentCursorPos.x -= viewRectInScreen.left;//鼠标在屏幕中的位置转化为在白板中的坐标
	m_CurrentCursorPos.y -= viewRectInScreen.top;//鼠标在屏幕中的位置转化为在白板中的坐标
	// 	CClientDC dc(this);
	// 	OnPrepareDC(&dc);//进行坐标原点的匹配
	// 	dc.DPtoLP(&m_CurrentCursorPos);//将视图坐标转换为文档作标
}

//获取光标坐标
CPoint CWhiteBoardView::GetCurrentCursorPos()
{
	SetCurrentCursorPos();
	return m_CurrentCursorPos;
}

//清除离线用户的摄像头对象, 返回删除个数
UINT CWhiteBoardView::ClearObjectOfOffLineUser(CString sUCCUserID)
{
	UINT nDelObject = 0;
	//遍历所有页，然后遍历各页中的所有对象
	UINT nPageCount = g_pWBFile->GetPageCount();
	CWBPage *pPage = NULL;
	for (UINT nIndex = 1; nIndex <= nPageCount; ++nIndex)
	{
		pPage = g_pWBFile->GetPage(nIndex);
		if (NULL != pPage)
		{
			//获取页中的对象链表
			CObList *pWBObjList = pPage->GetWBObjList();
			if (!pWBObjList)
			{
				continue;
			}

			//遍历对象列表
			POSITION pos, posTemp;
			posTemp = pos = pWBObjList->GetHeadPosition();
			CWBObject *pObj = NULL;
			for (; NULL != pos; )
			{
				pObj = static_cast<CWBObject*>(pWBObjList->GetNext(pos));

				if (pObj && m_pUserNameID != NULL && pObj->GetType() == WB_OBJ_PICTURE)
				{
					CWBPicture*picture = static_cast<CWBPicture*>(pObj);
					std::map<std::string,DWORD>::iterator it = m_pUserNameID->find(ToStr(sUCCUserID.GetBuffer(sUCCUserID.GetLength())));
					sUCCUserID.ReleaseBuffer();
					if (it != m_pUserNameID->end())
					{
						if (picture->GetLayerIndex() == it->second)
						{
							DWORD dwlayer = it->second;
							pWBObjList->RemoveAt(posTemp);
							m_pUserNameID->erase(it);
							DeleteCaminfo(dwlayer);
							RemoveObj(dwlayer);
							delete picture;
							picture = NULL;
							++nDelObject;
							Invalidate();
						}

					}
				}

				if ( pObj && (WB_OBJ_CAMERA == pObj->GetType()) )
				{
					CWBCamera *pCameraObj = static_cast<CWBCamera*>(pObj);
					if ( pCameraObj && !(pCameraObj->BCreatorOwner()) )
					{
						CString strUCCUserID = pCameraObj->GetUCCUserID();
						//if(!sUCCUserID.CompareNoCase(strUCCUserID))
						if(sUCCUserID.CompareNoCase(strUCCUserID) == 0)
						{
							CWBPage *page = NULL;
							int npage = g_pWBFile->GetPageCount();
							for(int i = 1; i <= npage; ++i)
							{
								page = g_pWBFile->GetPage(i);
								if (page)
								{
									page->RemoveMediaCameraObject(pCameraObj);
								}
							}
							//g_pWBPage->RemoveMediaCameraObject(pCameraObj);
							//从列表中移除
							pWBObjList->RemoveAt(posTemp);
							//发送同步删除服务器上的对象
							//RemoveObj(pCameraObj->GetLayerIndex());
							RemoveMediaObj(pCameraObj->GetLayerIndex());
							//RemoveMediaObj(pCameraObj->GetLayerIndex());
							//并删除对象(窗口应该在对象删除时自动关闭吧，如析构函数)
							delete pCameraObj;
							pCameraObj = NULL;
							nDelObject++;
							OutputDebugString(_T("/n--摄像头已删除摄像头已删除摄像头已删除--/n"));
						}
						else
						{
							OutputDebugString(_T("/n--摄像头没有删除摄像头没有删除摄像头没有删除--/n"));
						}
						Invalidate();
					}
				}
				if ( pObj && (WB_OBJ_MEDIA == pObj->GetType()) )
				{
					CWBMedia *pMediaObj = static_cast<CWBMedia*>(pObj);
					if ( pMediaObj && !(pMediaObj->IsActiveCreated()) )
					{
						CString strUCCUserID = pMediaObj->GetUCCUserID();
						//if(!sUCCUserID.CompareNoCase(strUCCUserID))//wangxiaobin
						if(sUCCUserID.CompareNoCase(strUCCUserID) == 0)
						{

							/*删除新视频中表示视频个数链表的数据*/

							g_pWhiteBoardView->DeleteMediaListDate(pMediaObj);

							CWBPage *page = NULL;
							int npage = g_pWBFile->GetPageCount();
							for(int i = 1; i <= npage; ++i)
							{
								page = g_pWBFile->GetPage(i);
								if (page)
								{
									page->RemoveMediaCameraObject(pMediaObj);
								}
							}
							//g_pWBPage->RemoveMediaCameraObject(pMediaObj);
							//从列表中移除
							pWBObjList->RemoveAt(posTemp);
							//发送同步删除服务器上的对象
							//RemoveObj(pMediaObj->GetLayerIndex());
							RemoveMediaObj(pMediaObj->GetLayerIndex());
							//并删除对象(窗口应该在对象删除时自动关闭吧，如析构函数)
							delete pMediaObj;
							pMediaObj = NULL;
							++nDelObject;
							Invalidate();
							OutputDebugString(_T("/n--视频已删除视频已删除视频已删除--/n"));
						}
						else
						{
							OutputDebugString(_T("/n--视频没有删除视频没有删除视频没有删除--/n"));
						}
					}
				}
				posTemp = pos;
			}
		}
	}
	return nDelObject;
}
void CWhiteBoardView::DeleteIntnetMediaToSin()
{
	POSITION pos;
	CWBObject *pObj = NULL;
	//遍历所有页，然后遍历各页中的所有对象
	UINT nPageCount = g_pWBFile->GetPageCount();
	CWBPage *pPage = NULL;
	for (UINT nIndex = 1; nIndex <= nPageCount; ++nIndex)
	{
		pPage = g_pWBFile->GetPage(nIndex);
		if (NULL != pPage)
		{
			//获取页中的对象链表
			CObList *pWBObjList = pPage->GetWBObjList();
			if (pWBObjList == NULL)
			{
				continue;
			}
			if (pWBObjList->GetCount() == 0)
			{
				continue;
			}
			for(int Index = 0;Index <pWBObjList->GetCount();++Index )
			{
				pos = pWBObjList->FindIndex(Index);
				if (pos == NULL)
				{
					continue;
				}
				pObj = (CWBObject *)pWBObjList->GetAt(pos);
				if (pObj == NULL)
				{
					continue;
				}
				if (pObj->GetType() == WB_OBJ_MEDIA)
				{
					if (!((CWBMedia*)pObj)->IsActiveCreated())
					{
						g_pWhiteBoardView->DeleteMediaListDate(pObj);
						g_pWBPage->RemoveMediaCameraObject(pObj);
						pWBObjList->RemoveAt(pos);
						delete pObj;
						pObj = NULL;
					}
				}

			}
		}
	}

}
void CWhiteBoardView::DeleteMediaListDate( CWBObject* pObj )
{
	if (pObj == NULL)
	{
		return;
	}
	if (pObj->GetType() == WB_OBJ_MEDIA)
	{
		//获得当前视频对象的视频窗口指针，并将向量中与之相同的值擦除
		CWBMediaWindow* pWnd = dynamic_cast<CWBMedia*>(pObj)->GetMediaWindow();
		std::vector<CWBMediaWindow*>::iterator	iter;
		for(iter = g_pWhiteBoardView->m_MediaList.begin(); iter < g_pWhiteBoardView->m_MediaList.end(); ++iter)
		{
			if (g_pWhiteBoardView->m_MediaList.size() == 0)
			{
				break;
			}

			if (*iter == pWnd)
			{
				g_pWhiteBoardView->m_MediaList.erase(iter);
				break;
			}
		}
	}
	else
	{
		return;
	}
}


void CWhiteBoardView::RemoveAllMediaListDate()
{
	if (g_pWhiteBoardView)
	{
		if (g_pWhiteBoardView->m_MediaList.size() == 0)
		{
			return;
		}
		g_pWhiteBoardView->m_MediaList.clear();
	}
}

afx_msg LRESULT CWhiteBoardView::OnProcessFileOperate(WPARAM wParam, LPARAM lParam)
{

	if (NULL == g_pWhiteBoardView)
	{
		return 1;
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

		g_pWBFile->ClearPageList();
		//CDataBridge::GetInstance()->NewFile();
		g_pWBFile->AddPage(this);
		g_pWBPage = g_pWBFile->GetCurPage();
		//DestroyBtns();
		Invalidate();
		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}

		g_strCurFilePath.Empty();
		g_nDrawStatus = DRAW_NORMAL;

		g_pObj = NULL;
		/*		g_pWhiteBoardView->m_IsNewfile = TRUE; */
		g_pWhiteBoardView->m_bLocalFileOperate = true;
		if (IsOpenPreviewPanel())
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();	
		}
		else
		{
			g_pWhiteBoardView->m_bLocalFileOperate = false;
		}
	}
	return 0;
}

void CWhiteBoardView::RotateByAngle(int angle)
{
	CWBObject *pObj = NULL;// 记录根据图层号找到的对象地址
	
	g_pWBPage = g_pWBFile->GetCurPage();

	if (0 == (g_pWBPage->GetWBSelectFrameList()).size())
	{
		return;
	}
	unsigned long ulEventID = GenerateSSRC();

	for(int i = 0; i < (int)(g_pWBPage->GetWBSelectFrameList()).size(); ++i)
	{
		pObj = g_pWBPage->FindByLayerIndex((g_pWBPage->GetWBSelectFrameList())[i]);
		if (pObj == NULL)
		{
			return;
		}
		else if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan//对象被锁定
		{
			return;
		}
		else if (pObj->GetType() == WB_OBJ_PICTURE && bIsInCamInfoMap(pObj))
		{
			continue;
		}
		((CWBImageButton*)pObj->GetImageButton())->ChangeRationCenterOperator();
	
		if (pObj->GetType() == WB_OBJ_COMBINE)//如果是组合
		{
			((CWBCombine *)pObj)->GetSrcLinePoint();
		}
		if (pObj->GetType() == WB_OBJ_DOCUMENT)//如果是文档
		{
			((CWBDocument *)pObj)->GetSrcAnnPt();
		}
		if (pObj->GetType() == WB_OBJ_PICTURE)//如果是图片
		{
			((CWBPicture *)pObj)->GetSrcAnnPt();
		}

		if (pObj->GetType() == WB_OBJ_COMBINE)//如果是组合
		{
			((CWBCombine *)pObj)->GetSrcCenterPoint();
			//pObj->GetCenterPoint(g_ptComRotCenter);
			pObj->GetDrawRotationCenter(g_ptComRotCenter);
			pObj->RotateByCenter(0);	
		}
		if (pObj->GetType() == WB_OBJ_DOCUMENT) //如果是文档
		{
			((CWBDocument *)pObj)->GetSrcCenterPt();
			//pObj->GetCenterPoint(g_ptDocRotCenter);
			pObj->GetDrawRotationCenter(g_ptDocRotCenter);
			pObj->RotateByPoint(g_ptDocRotCenter,0);
		}
		if (pObj->GetType() == WB_OBJ_PICTURE) //如果是图片
		{
			((CWBPicture *)pObj)->GetSrcCenterPt();
			//pObj->GetCenterPoint(g_ptDocRotCenter);
			pObj->GetDrawRotationCenter(g_ptDocRotCenter);
			pObj->RotateByPoint(g_ptDocRotCenter,0);
		}
		if (pObj->GetType() == WB_OBJ_PROTRACTOR)
		{	
			pObj->RotateByPoint(((CWBProtractor *)pObj)->GetCTpoint(),angle);
		}
		else if (pObj->GetType() == WB_OBJ_SETSQUARE)
		{
			pObj->RotateByPoint(((CWBSetSquare *)pObj)->GetSetSquareCT(),angle);
		}
        else if (pObj->GetType() == WB_OBJ_COMBINE||pObj->GetType() == WB_OBJ_DOCUMENT)//如果是组合或文档
		{
			pObj->RotateByCenter(angle);
		}
		else
		{
			CPoint ptRotationCenter;
			pObj->GetDrawRotationCenter(ptRotationCenter);
			pObj->RotateByPoint(ptRotationCenter,angle);
		}	


		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			Invalidate();
		}

		//发送同步消息
		XMemIni xmlObj;
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
		pObj->SetXmlValue(xmlObj);
		ChangeObj(xmlObj);
	}
}
void  CWhiteBoardView::RotateByHV(int HVAngle)
{
	CWBObject *pObj = NULL;// 记录根据图层号找到的对象地址

	g_pWBPage = g_pWBFile->GetCurPage();

	if (0 == (g_pWBPage->GetWBSelectFrameList()).size())
	{
		return;
	}
	unsigned long ulEventID = GenerateSSRC();

	for(int i = 0; i < (int)(g_pWBPage->GetWBSelectFrameList()).size(); ++i)
	{
		pObj = g_pWBPage->FindByLayerIndex((g_pWBPage->GetWBSelectFrameList())[i]);
		if (pObj == NULL)
		{
			return;
		}
		if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan//对象被锁定
		{
			return;
		}
		if (pObj->GetType() == WB_OBJ_PICTURE && bIsInCamInfoMap(pObj))
		{
			continue;
		}
		if (pObj->GetRotationAngle() == HVAngle)
		{
			return;
		}
		if (pObj->GetType() == WB_OBJ_COMBINE)//如果是组合
		{
			((CWBCombine *)pObj)->GetSrcLinePoint();
		}
		if (pObj->GetType() == WB_OBJ_DOCUMENT)//如果是文档
		{
			((CWBDocument *)pObj)->GetSrcAnnPt();
		}
		if (pObj->GetType() == WB_OBJ_PICTURE)//如果是图片
		{
			((CWBPicture *)pObj)->GetSrcAnnPt();
		}
		if (pObj->GetType() == WB_OBJ_COMBINE)//如果是组合
		{
			double angle =  pObj->GetRotationAngle();
			((CWBCombine *)pObj)->GetSrcCenterPoint();
			pObj->GetDrawRotationCenter(g_ptComRotCenter);
			pObj->RotateByCenter(-angle+HVAngle);
		}
		if (pObj->GetType() == WB_OBJ_DOCUMENT) //如果是文档
		{
			double angle =  pObj->GetRotationAngle();
			((CWBDocument *)pObj)->GetSrcCenterPt();
			pObj->GetDrawRotationCenter(g_ptDocRotCenter);
			pObj->RotateByCenter(-angle+HVAngle);
		}
		if (pObj->GetType() == WB_OBJ_PICTURE) //如果是图片
		{
			double angle = pObj->GetRotationAngle();
			((CWBPicture *)pObj)->GetSrcCenterPt();
			pObj->GetDrawRotationCenter(g_ptDocRotCenter);
			pObj->RotateByCenter(-angle+HVAngle);
		}
		if (pObj->GetType() == WB_OBJ_ANNOTATIONS) //如果是标注
		{
			double angle =  pObj->GetRotationAngle();
			pObj->RotateByCenter(-angle+HVAngle);
		}
		if (pObj->GetType() == WB_OBJ_PROTRACTOR)
		{
			pObj->RotateByPoint(((CWBProtractor *)pObj)->GetCTpoint(),0);
			pObj->SetRotarionAngle(HVAngle);
		} 
		else
		{
			//pObj->RotateByCenter(0);
			//pObj->SetRotarionAngle(HVAngle);
			CPoint pt;
			double angle;
			pObj->GetDrawRotationCenter(pt);
			angle = pObj->GetRotationAngle();
			double angel = 90;
			pObj->RotateByPoint(pt,HVAngle - angle);
			//pObj->SetRotarionAngle(HVAngle);
		}
		

		if (CScreenDrawMgr::GetIns() != NULL) 
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			Invalidate();
		}

		//发送同步消息
		XMemIni xmlObj;
		xmlObj.Set(KEY_EVENT_ID, ulEventID);
		pObj->SetXmlValue(xmlObj);
		ChangeObj(xmlObj);
	}
}

void CWhiteBoardView::DeleteAll()
{
	int nResult = 0;
	CString strAttention;
	CString strIsDelAllObj ;
	strAttention = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Attention"));
	if (strAttention.IsEmpty())
	{
		strAttention = _T("注意");
	}
	strIsDelAllObj = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsDeleteAllObj"));
	if (strIsDelAllObj.IsEmpty())
	{
		strIsDelAllObj = _T("是否要删除全部对象?");
	}
	if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
	{

		nResult = g_pHPWBDataNotify->MessageBox(this->m_hWnd,strAttention,strIsDelAllObj , MB_OKCANCEL,false);
	}
	else
	{
		nResult = MessageBox(strIsDelAllObj,strAttention, MB_OKCANCEL|MB_ICONWARNING);
	}
	if (nResult != IDOK)
	{
		return;
	}
	if(g_pWhiteBoardView == NULL)
	{
		return;
	}
	std::vector<DWORD *> vecLayerIndex;
	DWORD dwObjCount = g_pWBPage->GetAllObjLayerIndex(vecLayerIndex);
	CWBObject *pWBObj = NULL;
	g_pWBPage->ClearSelectFrameList();
	m_ulSendEventID = GenerateSSRC();
	if (g_pWBPage->DeleteAllObjects(m_ulSendEventID))
	{
		CObList *pdelList = g_pWBPage->GetSelectObjLst();
		int j = pdelList->GetCount();
		POSITION pos;
		if (j == 1)//删除所选列表只剩一个对象时,此即为全局对象
		{
			for (int k = 0;k < j;++k )//遍历选中对象列表
			{
				pos = pdelList->FindIndex(k);
				if (pos == NULL)
				{
					continue;
				}
				g_pObj = (CWBObject *)pdelList->GetAt(pos);
			}
		}
		if (NULL != g_pWhiteBoardView->m_pWBFunctionDlg)//控制属性面板显示相应的属性
		{
			if (::IsWindow(g_pWhiteBoardView->m_pWBFunctionDlg->GetSafeHwnd()))
			{						
				if (j == 0)//选中对象列表中无对象时
				{
					if (!g_bAttriDlginitialize)
					{
						::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_NOSELECTWBOBJ,0,0);
					}					
				}
				//else if (j == 1)//选中对象列表中只有一个对象时
				//{
				//	::PostMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,0,j);
				//}
				//else //选中对象列表中存在多个对象时
				//{			
				//	for (int k = 0;k < j;++k )//遍历选中对象列表
				//	{
				//		pos = pdelList->FindIndex(k);
				//		if (pos == NULL)
				//		{
				//			continue;
				//		}
				//		pObj = (CWBObject *)pdelList->GetAt(pos);
				//		if (!pObj)
				//		{
				//			continue;
				//		}
				//		int ntype = pObj->GetType();
				//		switch(ntype)
				//		{
				//		case WB_OBJ_FLASH:									
				//		case WB_OBJ_COMBINE:					
				//		case WB_OBJ_DOCUMENT:					
				//		case WB_OBJ_PICTURE:
				//			{
				//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_PICTURE,j);//隐藏线宽,线形,箭头
				//			}
				//			break;
				//		case WB_OBJ_ANNOTATIONS:
				//			{
				//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_ANNOTATIONS,j);//隐藏线宽,线形,箭头
				//			}
				//			break;
				//		case WB_OBJ_TEXT:
				//			{
				//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_TEXT,j);//隐藏线宽,线形,箭头
				//			}
				//			break;
				//		case WB_OBJ_RHOMBUS:					
				//		case WB_OBJ_TRIANGLE:					
				//		case WB_OBJ_ELLIPSE:					
				//		case WB_OBJ_RECT:		
				//		case WB_OBJ_PALLELOGAM: //平行四边形
				//		case WB_OBJ_ARBITRARYTRIANGLE:
				//		case WB_OBJ_PENTAGON://五边形
				//		case WB_OBJ_HEXAGON://六边形
				//		case WB_OBJ_FIVEPOINTSTART:
				//		case WB_OBJ_TRAPEZIUM:
				//		case WB_OBJ_POLYGON:
				//			{
				//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_POLYGON,j);//隐藏线形,箭头,字体相关属性
				//			}
				//			break;
				//		case WB_OBJ_CURVE:					
				//		case WB_OBJ_LINE:
				//			{
				//				::PostMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBOBJ,WB_OBJ_LINE,j);//隐藏线形,箭头,字体相关属性
				//			}
				//			break;
				//		default:
				//			break;
				//		}		
				//	}
				//}					
			}
		}
		if (g_pWhiteBoardView->m_hWnd == NULL)
		{
			return;
		}

		if (CScreenDrawMgr::GetIns() != NULL)
		{
			CScreenDrawMgr::GetIns()->Invalidate();
		}
		else
		{
			Invalidate();
		}
		UpdateWindow();
		RemovePageAllObj(g_pWBFile->GetCurPageIndex());

	}
	
	for (int i = 0; i < (int)vecLayerIndex.size(); ++i )
	{
		if (vecLayerIndex[i] != NULL)
		{
			delete vecLayerIndex[i];
		}
	}
	vecLayerIndex.clear();
	g_nDrawStatus = DRAW_NORMAL;
}
// afx_msg LRESULT CWhiteBoardView::OnWbDelAllAttribute(WPARAM wParam, LPARAM lParam)
// {
// 	g_pWBFile->ClearPageList();
// 	//CDataBridge::GetInstance()->NewFile();
// 	g_pWBFile->AddPage(this);
// 	g_pWBPage = g_pWBFile->GetCurPage();
// 	//DestroyBtns();
// 	g_strCurFilePath.Empty();
// 	//清除操作记录
// 	g_pWBFile->ClearOperationsList();
// 	g_pWBFile->AddOperation();
// 	//g_IsSelectObj = FALSE;//释放选择
// 	g_pObj = NULL; 
//   //关闭文件预览操作
// 	g_pWhiteBoardView->m_IsClosefile = TRUE; 
// 	if (g_pWhiteBoardView->IsOpenFunctionPanel())
// 	{
// 		g_pWhiteBoardView->m_wbFunctionDlg->OnBnClickedWbPreviewbtn();
// 
// 	}
// 	Invalidate();
// 	return 0;
// }


afx_msg LRESULT CWhiteBoardView::OnNoticePraviewRefresh(WPARAM wParam, LPARAM lParam)
{
	m_bRefurbishPreview = (BOOL)wParam;
	return 0;
}

void CWhiteBoardView::OnBroadcast_Data( XMemIni xmlObj )
{
	int nBroadcastCmd = -1;
	unsigned long ulSessionID = 0;
	if (xmlObj.Get(KEY_FILE_SENDER,ulSessionID) && ulSessionID == g_ulSessionID)
	{
		return;
	}
	//xmlObj.DeleteKey(KEY_FILE_SENDER);
	xmlObj.Get(KEY_BROADCAST_CMD,nBroadcastCmd);

	switch(nBroadcastCmd)  //判断广播类型
	{
	case _E_ROADCASTCMD_NONE:
		break;
	case _E_OBJ_MOVE:
		{
			string strPoints = "";
			if (xmlObj.Get(KEY_MOVE_OFFSET,strPoints) && strPoints != "")
			{
				MoveObj_Realtime_Synchronizatio(xmlObj);
			}
			else
			{
				ProcessBroadcastAttiObj(xmlObj);
			}
		}
		break;
	case _E_OBJDRAW_MOVE:
		{
			ProcessBroadcastAttiObj(xmlObj);
		}
		break;
	case _E_PROCESS_FILE:		//文件操作进展
		{
			int nCmdEvent = 0 ;
			if (xmlObj.Get(KEY_FILEPROC_MARCH,nCmdEvent))
			{
				switch(nCmdEvent)
				{
				case KEY_BEGIN_EVENT: //文件操作开始
					{
						m_bRefurbishPreview = FALSE;//在这里设置判断条件让在删除所有对象结束之前不响应删除命令 等到删除完了之后就一次性把全部内容删除 刷新预览和鹰眼
					}
					break;
				case KEY_END_EVENT: //文件操作结束
					{			
						SendMessage(WM_PRAVIEW_REFRESH);
						m_bRefurbishPreview = TRUE;
						break;
					}
				default:
					break;
				}
			}
			if (g_pHPWBDataNotify && g_bSupportI7)
			{
				MSG msg;
				memset(&msg,0,sizeof(MSG));
				msg.hwnd = NULL;
				msg.message = 4;
				g_pHPWBDataNotify->SetSelectObjStatue(msg);
			}
		}
		break;
	case	_E_CURSOR_MOVE:
		{
			if (m_bIsneedjionBroadcast == FALSE)
			{
				return;
			}
			int nbBroadcastCuror = -1;
			xmlObj.Get(KEY_BROADCASTUSERCURSOR,nbBroadcastCuror);
			string str_name = "";
			xmlObj.Get(KEY_STRAVCONUSERID,str_name);

			switch(nbBroadcastCuror)
			{
			case _E_MOVE_TRANARENT_CURSOR:
				{
					string strPoints = "";
					xmlObj.Get(KEY_CURSORPOINT,strPoints);
					int nstrlength = str_name.length();
					char* pzsName = new char[nstrlength + 1];
					if (pzsName == NULL) return;
					memset(pzsName,0,nstrlength +1);
					memcpy(pzsName,str_name.c_str(),nstrlength);
					pzsName[nstrlength] = '\0';
					nstrlength = strPoints.length();
					char* pzPoint = new char[nstrlength +1];
					if (pzPoint == NULL) return;
					memset(pzPoint,0,nstrlength +1);
					memcpy(pzPoint,strPoints.c_str(),nstrlength);
					pzPoint[nstrlength] = '\0';
					if (g_whiteboardDlg->IsWindowVisible())
					{
						PostMessage(WM_PROCESS_TRANDLG,(WPARAM)(pzsName),(LPARAM)(pzPoint));
					}
				}	
				break;
			case _E_CLOSE_TRANARENT_CURSOR:  //关闭光标
				{
					OnRejectBroadcastUserCursor(str_name);
				}
				break;

			default:
				break;
			}
		}
		break;
	case _E_WATER_SPECIAL_EFFICIENCY:
		{
			if (m_bIsneedjionBroadcast == FALSE)
			{
				return;
			}
			int nbBroadcastCuror = -1;
			xmlObj.Get(KEY_BROADCASTUSERWATER,nbBroadcastCuror);

			xmlObj.Get(KEY_WATER_USER,m_ReceiveWaterUserID);

			switch(nbBroadcastCuror)
			{
			case _E_SHOWWATER:
				{					
					string strPoints = "";
					xmlObj.Get(KEY_WATER_POINT,strPoints);											
					std::string strTemp = "";
					int nIndex = 0;
					strTemp = strPoints.substr(nIndex, 4);
					int _x = atoi(strTemp.c_str());
					nIndex += 4;
					strTemp = strPoints.substr(nIndex, 4);
					int _y = atoi(strTemp.c_str());
					m_ReceiveWaterPoint.x = _x;
					m_ReceiveWaterPoint.y = _y;
					if (g_reduce)
					{
						m_ReceiveWaterPoint.x = m_ReceiveWaterPoint.x*g_reduce;
						m_ReceiveWaterPoint.y = m_ReceiveWaterPoint.y*g_reduce;
					}
					if (g_whiteboardDlg->IsWindowVisible())
					{
						PostMessage(WM_PROCESS_WATERDLG);
					}
					
				}
				break;
			case _E_CLOSEWATER:
				{
					OnRejectBroadcasstUserWater(m_ReceiveWaterUserID);
				}
			default:
				break;

			}
		}
		break;
	case _E_BROADE_ERASE_LINE:
		{
			int left,right,top,button;
			if ( xmlObj.Get(KEY_ERASE_RECT_LEFT,left)&& xmlObj.Get(KEY_ERASE_RECT_RIGHT,right) &&
				xmlObj.Get(KEY_ERASE_RECT_TOP,top) && xmlObj.Get(KEY_ERASE_RECT_BOTTON,button))
			{
				CRect rect(left,top,right,button);
				if (g_pWBPage && g_pWhiteBoardView && g_pWhiteBoardView->m_hWnd)
				{
					g_pWBPage->EraseOccasionalObjects(rect);
					if (g_bIsDrawScreen && CScreenDrawMgr::GetIns())
					{
						CScreenDrawMgr::GetIns()->Invalidate();
					}
					else
					{
						Invalidate();
					}

				}
			}	
		}
		break;
	case _E_BROAD_ERASE_UP:
		{
			m_bEraseLineUp = TRUE;
		}
		break;
	case _E_RELEASE_LIMITOBJS:
		{
			OnReleaseLimitObjs();
		}
		break;
	case _E_BROAD_UNDO_REDO_MEDIA:
	case CMD_TOUCH_MOVE_OBJ:
		{
			ProcessObj(xmlObj,CMD_CHANGE_ATTRIBUTE);
		}
		break;
	 case _E_BROAD_UNDO_REDO_CAMERA:
		{
			m_bIsCameraUNDO = true;
			ProcessObj(xmlObj,CMD_CHANGE_ATTRIBUTE);
			m_bIsCameraUNDO = false;
		}
		break;
	case _E_BROAD_ROLL_CALL:
		{
			if (WB_SINGLERVER == g_WBType)
			{
				int nBroadStyle = 0;
				xmlObj.Get(KEY_BROADCASTUSERSTYLE ,nBroadStyle);
				if (nBroadStyle == 0)
				{
					m_nSendRollCallBroadCastID = ulSessionID;
					ProcessRollCallBroadCast();
				}
				else
				{
					xmlObj.Get(KEY_BROADCASTSENDROLLCALLUSER,m_nSendRollCallBroadCastID);
					if (m_nSendRollCallBroadCastID == g_ulSessionID)
					{
						string str;
						xmlObj.Get(KEY_BROADCASTROLLCALLOKUSERNAME,str);
						if (str != "")
						{
							ProcessRollCallLRSultBroadCast(str);
						}

					}
				}
			}
		}
		break;
	case _E_BROAD_UNDO_REDO_SPOLITE:
		{
			ProcessObj(xmlObj);
		}
		break;
	case _E_BOARD_CAMERA_NAME:  //摄像头名
		{
			std::string strAddUser = "";
			xmlObj.Get(KEY_CAMERA_ID_OF_UCC_LOGIN_NAME,strAddUser);
			if (!strAddUser.empty() && g_pWhiteBoardView)
			{
				g_pWhiteBoardView->SetUserCameraName(strAddUser);
			}
		}
		break;
	default:
		break;
	}
}
afx_msg LRESULT CWhiteBoardView::OnDelallobject(WPARAM wParam, LPARAM lParam)
{
	RemoveAllObj();
	return 0;
}

void  CWhiteBoardView::LoadPopupMenuBitmap()
{
	m_CheckedBmp.LoadBitmap(IDB_OBMENU_CHECKED);
	TranslateBitmapSizeToFitXP(&m_CheckedBmp);
	m_InfiniteCloneBmp.LoadBitmap(IDB_OBMENU_INFINITECLONE);//	无限克隆
	TranslateBitmapSizeToFitXP(&m_InfiniteCloneBmp);
	m_UnLockBmp.LoadBitmap(IDB_OBMENU_UNLOCK); //解锁
	TranslateBitmapSizeToFitXP(&m_UnLockBmp);
	m_LockBmp.LoadBitmap(IDB_OBMENU_LOCK); //上锁
	TranslateBitmapSizeToFitXP(&m_LockBmp);
	//视频
	m_MoviePlayBmp.LoadBitmap(IDB_OBMENU_MOVIE_PALY); // 视频播放
	TranslateBitmapSizeToFitXP(&m_MoviePlayBmp);
	m_MoviePauseBmp.LoadBitmap(IDB_OBMENU_MOVIE_PAUSE); //视频暂停
	TranslateBitmapSizeToFitXP(&m_MoviePauseBmp);
	m_MovieStopBmp.LoadBitmap(IDB_OBMENU_MOVIE_STOP); //视频停止
	TranslateBitmapSizeToFitXP(&m_MovieStopBmp);
	

	m_MediaCatchFrameBmp.LoadBitmap(IDB_OBMENU_MEDIA_CATCHFRAME); //截图
	TranslateBitmapSizeToFitXP(&m_MediaCatchFrameBmp);
	m_MediaSetmuteBmp.LoadBitmap(IDB_OBMENU_MEDIA_SETMUTE); //有音
	TranslateBitmapSizeToFitXP(&m_MediaSetmuteBmp);
	m_MediaNoSetmuteBmp.LoadBitmap(IDB_OBMENU_MEDIA_NOSETMUTE); //静音
	TranslateBitmapSizeToFitXP(&m_MediaNoSetmuteBmp);

	//文档
	m_DocPrePageBmp.LoadBitmap(IDB_OBMENU_DOC_PREPAGE); // 上一页
	TranslateBitmapSizeToFitXP(&m_DocPrePageBmp);
	m_DocFirstPageBmp.LoadBitmap(IDB_OBMENU_DOC_FIRSTPAGE); //首页
	TranslateBitmapSizeToFitXP(&m_DocFirstPageBmp);
	m_DocNextPageBmp.LoadBitmap(IDB_OBMENU_DOC_NEXTPAGE); //下一页
	TranslateBitmapSizeToFitXP(&m_DocNextPageBmp);
	m_DocLastPageBmp.LoadBitmap(IDB_OBMENU_DOC_LASTPAGE); //末页
	TranslateBitmapSizeToFitXP(&m_DocLastPageBmp);
	m_DocPrintCurBmp.LoadBitmap(IDB_OBMENU_DOC_PRINTCUR); //当前页
	TranslateBitmapSizeToFitXP(&m_DocPrintCurBmp);
	m_DocPrintAllBmp.LoadBitmap(IDB_OBMENU_DOC_PRINTALL); //所有页
	TranslateBitmapSizeToFitXP(&m_DocPrintAllBmp);

	m_SelectAllBmp.LoadBitmap(IDB_OBMENU_SELECTALL);	 //全选
	TranslateBitmapSizeToFitXP(&m_SelectAllBmp);
// 	m_SckdBmp.LoadBitmap(IDB_OBMENU_SELECTALL);
// 	TranslateBitmapSizeToFitXP(&m_SckdBmp);
	m_UnselectBmp.LoadBitmap(IDB_OBMENU_UNSELECT);	 //取消选择
	TranslateBitmapSizeToFitXP(&m_UnselectBmp);
	m_EqualScaleZoomBmp.LoadBitmap(IDB_OBMENU_EQUALSCALEZOOM); //等比例缩放
	TranslateBitmapSizeToFitXP(&m_EqualScaleZoomBmp);
	m_DeleteBackgroundBmp.LoadBitmap(IDB_OBMENU_DELETEBACKGROUND); //删除背景图片
	TranslateBitmapSizeToFitXP(&m_DeleteBackgroundBmp);
	m_SetAsBackgroundBmp.LoadBitmap(IDB_OBMENU_SETASBACKGROUND); //设为背景
	TranslateBitmapSizeToFitXP(&m_SetAsBackgroundBmp);

	m_EditBmp.LoadBitmap(IDB_OBMENU_EDIT); //编辑
	TranslateBitmapSizeToFitXP(&m_EditBmp);
	m_CutBmp.LoadBitmap(IDB_OBMENU_CUT); //剪切
	TranslateBitmapSizeToFitXP(&m_CutBmp);
	m_CopyBmp.LoadBitmap(IDB_OBMENU_COPY);	 //复制
	TranslateBitmapSizeToFitXP(&m_CopyBmp);
	m_DeleteBmp.LoadBitmap(IDB_OBMENU_DELETE);	 //删除
	TranslateBitmapSizeToFitXP(&m_DeleteBmp);
	m_DeleteAllBmp.LoadBitmap(IDB_OBMENU_DELALL);	 //删除全部
	TranslateBitmapSizeToFitXP(&m_DeleteAllBmp);
	m_PasteBmp.LoadBitmap(IDB_OBMENU_PASTE); //粘贴
	TranslateBitmapSizeToFitXP(&m_PasteBmp);
	m_CloneBmp.LoadBitmap(IDB_OBMENU_CLONE); //克隆
	TranslateBitmapSizeToFitXP(&m_CloneBmp);
	m_CombineBmp.LoadBitmap(IDB_OBMENU_COMBINE); //组合
	TranslateBitmapSizeToFitXP(&m_CombineBmp);
	m_UnCombineBmp.LoadBitmap(IDB_OBMENU_UNCOMBINE);	 //取消组合
	TranslateBitmapSizeToFitXP(&m_UnCombineBmp);
	m_TopLayerBmp.LoadBitmap(IDB_OBMENU_TOPLAYER);	 //顶层
	TranslateBitmapSizeToFitXP(&m_TopLayerBmp);
	m_PreLayerBmp.LoadBitmap(IDB_OBMENU_UPLAYER); //上一层
	TranslateBitmapSizeToFitXP(&m_PreLayerBmp);
	m_NextLayerBmp.LoadBitmap(IDB_OBMENU_UNDERLAYER); //下一层
	TranslateBitmapSizeToFitXP(&m_NextLayerBmp);
	m_BottomLayerBmp.LoadBitmap(IDB_OBMENU_BOTTPMLAYER); //底层
	TranslateBitmapSizeToFitXP(&m_BottomLayerBmp);
	m_AttributeBmp.LoadBitmap(IDB_OBMENU_ATTRIBUTE); //属性
	TranslateBitmapSizeToFitXP(&m_AttributeBmp);

	m_Deasil45Bmp.LoadBitmap(IDB_OBMENU_DEASIL45); //顺时针45度
	TranslateBitmapSizeToFitXP(&m_Deasil45Bmp);
	m_Deasil90Bmp.LoadBitmap(IDB_OBMENU_DEASIL90);	 //顺时针90度
	TranslateBitmapSizeToFitXP(&m_Deasil90Bmp);
	m_Widdershins45Bmp.LoadBitmap(IDB_OBMENU_WIDDERSHINS45);	 //逆时针45度
	TranslateBitmapSizeToFitXP(&m_Widdershins45Bmp);
	m_Widdershins90Bmp.LoadBitmap(IDB_OBMENU_WIDDERSHINS90); //逆时针90度
	TranslateBitmapSizeToFitXP(&m_Widdershins90Bmp);
	m_Rotation180Bmp.LoadBitmap(IDB_OBMENU_ROTATION180); //旋转180度
	TranslateBitmapSizeToFitXP(&m_Rotation180Bmp);
	m_HirizontalBmp.LoadBitmap(IDB_ROTATION_HIRIZONTAL); //水平放置
	TranslateBitmapSizeToFitXP(&m_HirizontalBmp);
	m_VerticalBmp.LoadBitmap(IDB_ROTATION_VERTICAL); //属性垂直放置
	TranslateBitmapSizeToFitXP(&m_VerticalBmp);

    //DXF
	m_DxfDragBmp.LoadBitmap(IDB_OBMENU_DXF_DRAG); //拖拽
	TranslateBitmapSizeToFitXP(&m_DxfDragBmp);
	m_DxfZoomInBmp.LoadBitmap(IDB_OBMENU_DXF_ZOOMIN); //放大
	TranslateBitmapSizeToFitXP(&m_DxfZoomInBmp);
	m_DxfZoomOutBmp.LoadBitmap(IDB_OBMENU_DXF_ZOOMOUT); //缩小
	TranslateBitmapSizeToFitXP(&m_DxfZoomOutBmp);
}

void CWhiteBoardView::processWBDTSData( AVDataInPacket& in )
{
	UINT nDataLen = 0;
	in >> nDataLen;	
	unsigned char* pData = NULL;
	pData = in.ReadData((int&)nDataLen);
	if (pData)
	{
		int iCmdType = -1;
		iCmdType = *((int*)pData);
		m_WBDataModel.ProcessDTSWbCmdData((eWbCmdOperation)iCmdType,(char*)pData,nDataLen); //处理从网络发过来的白板命令数据
	}
}

BOOL CWhiteBoardView::AddComponentToObj(CWBObject *pAddObj, unsigned long ulEventID)
{
	if (pAddObj == NULL)
	{
		return FALSE;
	}
	POSITION pos;
	CWBObject *pObj = NULL;
	CRgn objRgn;
	CRect objRc;
	if(!pAddObj->CWBObject::CreateRgn(&objRgn))
	{
		return FALSE;
	}
	if (!objRgn.m_hObject)
	{
		return FALSE;
	}
	objRgn.GetRgnBox(&objRc);//获取完全包含指定区域的最小矩形

	CObList *pWBObjList = g_pWBPage->GetWBObjList();
	pos = pWBObjList->GetTailPosition();//获取链表的尾元素位置
	pWBObjList->GetPrev(pos);
	for( ; pos != NULL; )//pos的循环
	{
		pObj = (CWBObject *)pWBObjList->GetPrev(pos);
		if (pObj == NULL)
		{
			continue;
		}
		if (((pObj->GetType() == WB_OBJ_PICTURE)
			|| (pObj->GetType() == WB_OBJ_MOVIE)
			|| (pObj->GetType() == WB_OBJ_MEDIA)
			|| (pObj->GetType() == WB_OBJ_FLASH)
			|| (pObj->GetType() == WB_OBJ_DOCUMENT)
			|| (pObj->GetType() == WB_OBJ_MIDI)
			|| (pObj->GetType() == WB_OBJ_CAMERA)
			|| ((pObj->IsSolid())
			&& ((pObj->GetType() == WB_OBJ_POLYGON)
			|| (pObj->GetType() == WB_OBJ_RECT)
			|| (pObj->GetType() == WB_OBJ_ELLIPSE)
			|| (pObj->GetType() == WB_OBJ_TRIANGLE)
			|| (pObj->GetType() == WB_OBJ_PALLELOGAM)
			|| (pObj->GetType() == WB_OBJ_ARBITRARYTRIANGLE)
			|| (pObj->GetType() == WB_OBJ_PENTAGON)
			|| (pObj->GetType() == WB_OBJ_HEXAGON)
			|| (pObj->GetType() == WB_OBJ_FIVEPOINTSTART)
			|| (pObj->GetType() == WB_OBJ_TRAPEZIUM)
			|| (pObj->GetType() == WB_OBJ_RHOMBUS)
			|| (pObj->GetType() == WB_OBJ_SECTOR)))))
		{
			//判断曲线是否直接画在文档或图片上
			CRgn rgn;
			if (!pObj->CreateRgn(&rgn))
			{
				continue;
			}
			if (!rgn.m_hObject)
			{
				continue;
			}
			if (rgn.RectInRegion(&objRc))
			{
				if (pObj->GetType() == WB_OBJ_DOCUMENT)
				{
					CWBDocument *doc = (CWBDocument*)pObj;
					if (pAddObj != doc && doc->ObjectIsComponent(pAddObj))
					{
						g_pWBPage->DelWBObj(pAddObj->GetLayerIndex());

						CPoint pt;
						pAddObj->GetCenterPoint(pt);
						pAddObj->SetRotationCenter(pt);

						doc->AddComponent(pAddObj);
						g_pWBPage->ClearSelectFrameList();
						Invalidate();

						//发送同步消息
						m_ulSendEventID = ulEventID;
						//RemoveObj(pAddObj->GetLayerIndex());
						RemoveDelComCurve(pAddObj->GetLayerIndex());
						XMemIni xmlObj;
						xmlObj.Set(KEY_EVENT_ID, m_ulSendEventID);
						doc->SetXmlValue(xmlObj);
						ChangeObj(xmlObj);

						//SendChangePage();
						rgn.DeleteObject();
						objRgn.DeleteObject();
						return TRUE;
					}
					else
					{
						rgn.DeleteObject();
						break;
					}
				}
				else if (pObj->GetType() == WB_OBJ_PICTURE)
				{
					CWBPicture *pic = (CWBPicture*)pObj;
					if (pAddObj != pic && pic->GetIsPrintScreen() && pic->ObjectIsComponent(pAddObj))
					{
						g_pWBPage->DelWBObj(pAddObj->GetLayerIndex());

						CPoint pt;
						pAddObj->GetCenterPoint(pt);
						pAddObj->SetRotationCenter(pt);

						pic->AddComponent(pAddObj);
						g_pWBPage->ClearSelectFrameList();
						Invalidate();

						//发送同步消息
						m_ulSendEventID = ulEventID;
						RemoveDelComCurve(pAddObj->GetLayerIndex());
						XMemIni xmlObj;
						xmlObj.Set(KEY_EVENT_ID, m_ulSendEventID);
						pic->SetXmlValue(xmlObj);
						ChangeObj(xmlObj);

						rgn.DeleteObject();
						objRgn.DeleteObject();
						return TRUE;
					}
					else
					{
						rgn.DeleteObject();
						break;
					}
				}
				else if (pObj->GetType() == WB_OBJ_MIDI)
				{
					CWBMidi* pwbmid = (CWBMidi*)pObj;
					if (pAddObj != pwbmid &&pwbmid->ObjectIsComponent(pAddObj))
					{
						g_pWBPage->DelWBObj(pAddObj->GetLayerIndex());

						CPoint pt;
						pAddObj->GetCenterPoint(pt);
						pAddObj->SetRotationCenter(pt);

						pwbmid->AddComponent(pAddObj);
						g_pWBPage->ClearSelectFrameList();

						SetPageIsNeedDraw(TRUE);
						Invalidate();
						UpdateWindow();
						SetPageIsNeedDraw(FALSE);


						m_ulSendEventID = ulEventID;
						RemoveDelComCurve(pAddObj->GetLayerIndex());

						XMemIni xmlobj;
						xmlobj.Set(KEY_EVENT_ID,m_ulSendEventID);
						pwbmid->SetXmlValue(xmlobj);
						ChangeObj(xmlobj);

						rgn.DeleteObject();
						objRgn.DeleteObject();
						return TRUE;

					}
				}
				else
				{
					rgn.DeleteObject();
					break;
				}
			}
		}
	}
	objRgn.DeleteObject();
	return FALSE;
}

void CWhiteBoardView::AddComListToObj()
{
	int nIndex;
	DWORD dwLen = 0;
	CWBObject *pObj = NULL;
	int nFlag = 0;
	std::vector<DWORD> vecSelectFrameList;

	vecSelectFrameList = g_pWBPage->GetWBSelectFrameList();
	int nCount = vecSelectFrameList.size();

	unsigned long ulEventID = GenerateSSRC(); //事件ID

	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{
		//查找选择的对象
		pObj = g_pWBPage->FindByLayerIndex(vecSelectFrameList[nIndex]);
		if (pObj == NULL)
		{
			continue;
		}
		if (pObj->IsLock() || pObj->IsLimit()) //huanglongquan//对象被锁定
		{
			continue;
		}
		if (pObj->IsBkg())
		{
			continue;
		}
		if (pObj->GetType() == WB_OBJ_CURVE || pObj->GetType() == WB_OBJ_ANNOTATIONS) //如果是曲线或者备注
		{
			if (AddComponentToObj(pObj, ulEventID))
			{
				nFlag ++;
			}
		}
	}
	/*if (nFlag > 1)
	{
	SendChangePage();
	}*/
}

BOOL CWhiteBoardView::TranslateBitmapSize(  
	IN HDC hCurScreemDC,   //当前屏幕DC，可通过GetDC获得  
	IN HBITMAP hBmpSrc,    //原BITMAP句柄  
	IN int nSrcWidth,      //原BITMAP的宽度  
	IN int nSrcHeight,     //原BITMAP的高度  
	OUT HBITMAP &hBmpDst,  //改变大小后的BITMAP的句柄  
	IN int nDstWidth,      //改变大小后的BITMAP的宽度  
	IN int nDstHeight      //改变大小后的BITMAP的高度  
	)  
{  
	if (hBmpSrc == NULL || hCurScreemDC == NULL)  
	{  
		return FALSE;  
	}  
	if (hBmpDst != NULL)  
	{  
		DeleteObject(hBmpDst);  
	}  
	HDC hOldSrcDC = NULL;  
	HDC hOldDstDC = NULL;  
	HDC hSrcDC = CreateCompatibleDC(hCurScreemDC);  
	HDC hDstDC = CreateCompatibleDC(hCurScreemDC);  
	hBmpDst = CreateCompatibleBitmap(hCurScreemDC,nDstWidth,nDstHeight);  
	hOldSrcDC = (HDC)SelectObject(hSrcDC,hBmpSrc);  
	hOldDstDC = (HDC)SelectObject(hDstDC,hBmpDst);  
	StretchBlt(hDstDC,0,0,nDstWidth,nDstHeight,hSrcDC,0,0,nSrcWidth,nSrcHeight,SRCCOPY);  
	SelectObject(hSrcDC,hOldSrcDC);  
	SelectObject(hDstDC,hOldDstDC);  
	DeleteDC(hSrcDC);  
	DeleteDC(hDstDC); 
	DeleteObject(hBmpDst); 
	return TRUE;      
}

bool CWhiteBoardView::TranslateBitmapSizeToFitXP( CBitmap *pBitmap )
{
	if (pBitmap == NULL)
	{
		return false;
	}
	if (g_systemVersion >= 6)
	{
		return false;
	}
	else
	{
		HBITMAP hBmpCopy = NULL;
		TranslateBitmapSize(::GetDC(NULL),  (HBITMAP)pBitmap->m_hObject, 30, 30, hBmpCopy, GetSystemMetrics(SM_CXMENUCHECK), GetSystemMetrics(SM_CYMENUCHECK));
		pBitmap->Attach(hBmpCopy);
		return true;
	}
	/*	bmpCopy.Attach(hBmpCopy);*/
}

BOOL CWhiteBoardView::GetPageIsNeedDraw()
{
	return m_pageIsNeedDraw;
}

void CWhiteBoardView::SetPageIsNeedDraw( BOOL PageIsNeedDraw )
{
	m_pageIsNeedDraw = PageIsNeedDraw;
}

void CWhiteBoardView::SetViewIsNeedDraw( BOOL ViewIsNeedDraw )
{
	m_viewIsNeedDraw = ViewIsNeedDraw;
}

void CWhiteBoardView::AddResToWBView( DRAW_STATE nDrawStatus,CString strResPath,RES_INPUT_METHOD nAddResMethod )//view中添加选中的资源
{
	CPoint nPoint;
	nPoint = GetCurrentCursorPos();
	CClientDC dc(this);
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&nPoint);//将视图坐标转换为文档作标

	if (nDrawStatus == ADDPICTUREFROMFOLDER)//从外部文件夹中拖拽图片进白板
	{
		if (!PathFileExists(strResPath))
		{
			return;
		}
		//清除所有创建的选择框
		g_pWBPage->ClearSelectFrameList();

		//创建一个新对象
		CWBPicture *pWBPicture = new CWBPicture(strResPath);			
		pWBPicture->SetAlpha((int)g_nTranspar);
		//控制图片的最大不能超出view的大小4-21金鹏
		CRect viewRect;
		GetClientRect(&viewRect);
		pWBPicture->SetPictureBigerSzie(viewRect);
		g_pWBPage->AddWBObject(pWBPicture);
		CPoint ptCenter,ptOffset;
		//获得对象中心点
		pWBPicture->GetCenterPoint(ptCenter);
		m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
		//创建选择框，并移动对象到当前位置
		if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
		{
			m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
		}
		ptOffset = nPoint - ptCenter;
		g_pWBPage->MoveSelectedObj(ptOffset);
		Invalidate();			
		//发送同步消息
		XMemIni xmlObj;
		pWBPicture->SetXmlValue(xmlObj);
		AddObj(xmlObj);
	}

	if (nDrawStatus == ADDPICTUREFROMRESDLG)//从资源面板中拖拽图片进白板
	{
		if (!PathFileExists(strResPath/*m_pResource->m_strFileName*/))
		{
			return;
		}
		//清除所有创建的选择框
		g_pWBPage->ClearSelectFrameList();

		//创建一个新对象
		CWBPicture *pWBPicture = new CWBPicture(strResPath);			
		pWBPicture->SetAlpha((int)g_nTranspar);
		//控制图片的最大不能超出view的大小4-21金鹏
		CRect viewRect;
		GetClientRect(&viewRect);
		pWBPicture->SetPictureBigerSzie(viewRect);
		g_pWBPage->AddWBObject(pWBPicture);
		if (nAddResMethod == DRAG_AND_DROP)
		{
			CPoint ptCenter,ptOffset;
			//获得对象中心点
			pWBPicture->GetCenterPoint(ptCenter);
			m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
			}
			ptOffset = nPoint - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
		}
		if (nAddResMethod == DOUBLE_CLK)
		{
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
			}
		}
		Invalidate();			
		//发送同步消息
		XMemIni xmlObj;
		pWBPicture->SetXmlValue(xmlObj);
		AddObj(xmlObj);
	}

	if (nDrawStatus == ADDDOCFROMFOLDER)//从外部文件夹中拖拽文档进白板
	{
		if (DocConvertTip()) //文档还在转换过程中
		{
			return;
		}
		
		if (!PathFileExists(strResPath))
		{
			return;
		}
		//清除所有创建的选择框
		g_pWBPage->ClearSelectFrameList();
		//创建一个新对象
		CWBDocument *pWBDocument = new CWBDocument(strResPath);
		if (pWBDocument == NULL)
		{
			return;
		}
		pWBDocument->SetAlpha((int)g_nTranspar);
		if (g_bSupportU7OpenFile)
		{
			CreateFullScreen(pWBDocument);
		}
		int nPageIndex;
		nPageIndex = (int)g_pWBFile->GetCurPageIndex();
		pWBDocument->SetPageIndex(nPageIndex);

		g_pWBPage->AddWBObject(pWBDocument);
		CPoint ptCenter,ptOffset;
		//获得对象中心点
		pWBDocument->GetCenterPoint(ptCenter);
		m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
		//创建选择框，并移动对象到当前位置
		if (g_pWBPage->AddSelectFrame(pWBDocument->GetLayerIndex()))
		{
			m_dwCurSelectObjLayerIndex = pWBDocument->GetLayerIndex();
		}
		ptOffset = nPoint - ptCenter;
		g_pWBPage->MoveSelectedObj(ptOffset);
		Invalidate();

		//发送同步消息
		XMemIni xmlObj;
		pWBDocument->SetXmlValue(xmlObj);
		AddObj(xmlObj);
	}

	if (nDrawStatus == ADDDOCFROMRESDLG)//从资源面板中拖拽文档进白板
	{
		if (DocConvertTip()) //文档还在转换过程中
		{
			return;
		}
		
		if (!PathFileExists(strResPath/*m_pResource->m_strFileName*/))
		{
			return;
		}
		//清除所有创建的选择框
		g_pWBPage->ClearSelectFrameList();
		//创建一个新对象
		CWBDocument *pWBDocument = new CWBDocument(strResPath);
		if (pWBDocument == NULL)
		{
			return;
		}
		pWBDocument->SetAlpha((int)g_nTranspar);
		if (g_bSupportU7OpenFile)
		{
			CreateFullScreen(pWBDocument);
		}
		int nPageIndex;
		nPageIndex = (int)g_pWBFile->GetCurPageIndex();
		pWBDocument->SetPageIndex(nPageIndex);

		g_pWBPage->AddWBObject(pWBDocument);
		if (nAddResMethod == DRAG_AND_DROP)
		{
			CPoint ptCenter,ptOffset;
			//获得对象中心点
			pWBDocument->GetCenterPoint(ptCenter);
			m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBDocument->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBDocument->GetLayerIndex();
			}
			ptOffset = nPoint - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
		}
		if (nAddResMethod == DOUBLE_CLK)
		{
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBDocument->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBDocument->GetLayerIndex();
			}
		}
		Invalidate();

		//发送同步消息
		XMemIni xmlObj;
		pWBDocument->SetXmlValue(xmlObj);
		AddObj(xmlObj);
	}

	if (nDrawStatus == ADDNEWMOVIEFROMFOLDER)//从外部文件夹中拖拽新视频进白板
	{
		if (!PathFileExists(strResPath))
		{
			return;
		}
		//清除所有创建的选择框
		g_pWBPage->ClearSelectFrameList();
		//创建一个新对象
		CRect rct = CRect( 74, 43, 580, 450 );
		rct.MoveToXY(nPoint.x - rct.Width()/2 - getXnPos(), nPoint.y - rct.Height()/2 - getYnPos());

		CWBMedia *pWBMedia = new CWBMedia(strResPath, rct);

		g_pWBPage->AddWBObject(pWBMedia);


		//创建选择框，并移动对象到当前位置		
		if (g_pWBPage->AddSelectFrame(pWBMedia->GetLayerIndex()))
		{
			m_dwCurSelectObjLayerIndex = pWBMedia->GetLayerIndex();
		}

		Invalidate();

		XMemIni xmlObj;
		pWBMedia->SetXmlValue(xmlObj);
		AddObj(xmlObj);
	}

	if (nDrawStatus == ADDNEWMOVIEFROMRESDLG)//从资源面板中拖拽新视频进白板
	{
		if (!PathFileExists(strResPath))
		{
			return;
		}
		//清除所有创建的选择框
		g_pWBPage->ClearSelectFrameList();

		if (nAddResMethod == DRAG_AND_DROP)
		{
			//创建一个新对象
			CRect rct = CRect( 74, 43, 580, 450 );
			rct.MoveToXY(nPoint.x - rct.Width()/2 - getXnPos(), nPoint.y - rct.Height()/2 - getYnPos());

			CWBMedia *pWBMedia = new CWBMedia(strResPath, rct);

			g_pWBPage->AddWBObject(pWBMedia);


			//创建选择框，并移动对象到当前位置		
			if (g_pWBPage->AddSelectFrame(pWBMedia->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBMedia->GetLayerIndex();
			}

			Invalidate();

			XMemIni xmlObj;
			pWBMedia->SetXmlValue(xmlObj);
			AddObj(xmlObj);
		}
		if (nAddResMethod == DOUBLE_CLK)
		{
			//创建一个新对象
			CRect rct = CRect( WB_CTRLBTNDLG_WIDTH + 2,WB_TOOLBOXBTN_SPEC_HEIGHT + 12, 580, 450);
			CWBMedia *pWBMedia = new CWBMedia(strResPath, rct);
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBMedia->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBMedia->GetLayerIndex();
			}
			g_pWBPage->AddWBObject(pWBMedia);
			Invalidate();
			//发送同步消息
			XMemIni xmlObj;
			pWBMedia->SetXmlValue(xmlObj);
			AddObj(xmlObj);
		}
	}

	if (nDrawStatus == ADDFLASHFROMFOLDER)//从外部文件夹中拖拽flash进白板
	{
		if (!PathFileExists(strResPath))
		{
			return;
		}
		//清除所有创建的选择框
		g_pWBPage->ClearSelectFrameList();
		//创建一个新对象
		CWBFlash *pWBFlash = new CWBFlash(strResPath);			
		pWBFlash->SetAlpha((int)g_nTranspar);
		g_pWBPage->AddWBObject(pWBFlash);
		pWBFlash->init();
		CPoint ptCenter,ptOffset;
		//获得对象中心点
		pWBFlash->GetCenterPoint(ptCenter);
		//创建选择框，并移动对象到当前位置
		if (g_pWBPage->AddSelectFrame(pWBFlash->GetLayerIndex()))
		{
			m_dwCurSelectObjLayerIndex = pWBFlash->GetLayerIndex();
		}
		ptOffset = nPoint - ptCenter;
		g_pWBPage->MoveSelectedObj(ptOffset);
		Invalidate();
		//发送同步消息
		XMemIni xmlObj;
		pWBFlash->SetXmlValue(xmlObj);
		AddObj(xmlObj);
	}

	if (nDrawStatus == ADDFLASHFROMRESDLG)//从资源面板中拖拽flash进白板
	{
		if (!PathFileExists(strResPath))
		{
			return;
		}
		//清除所有创建的选择框
		g_pWBPage->ClearSelectFrameList();
		//创建一个新对象
		CWBFlash *pWBFlash = new CWBFlash(strResPath);			
		pWBFlash->SetAlpha((int)g_nTranspar);
		g_pWBPage->AddWBObject(pWBFlash);
		pWBFlash->init();
		if (nAddResMethod == DRAG_AND_DROP)
		{
			CPoint ptCenter,ptOffset;
			//获得对象中心点
			pWBFlash->GetCenterPoint(ptCenter);
			m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBFlash->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBFlash->GetLayerIndex();
			}
			ptOffset = nPoint - ptCenter;
			g_pWBPage->MoveSelectedObj(ptOffset);
		}
		if (nAddResMethod == DOUBLE_CLK)
		{
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBFlash->GetLayerIndex()))
			{
				m_dwCurSelectObjLayerIndex = pWBFlash->GetLayerIndex();
			}
		}
		Invalidate();
		//发送同步消息
		XMemIni xmlObj;
		pWBFlash->SetXmlValue(xmlObj);
		AddObj(xmlObj);
	}
	g_nDrawStatus = DRAW_NORMAL;
	g_nMethodOfResToView = NORMAL;
	g_strSelectedResPath.Empty();
}

void CWhiteBoardView::OpenAboutDlg()
{
	((CWhiteBoardDlg*)GetParent())->CreateAboutDlg();
}

void CWhiteBoardView::UpdateViewBackGround(CWBObject *pObj)
{
	if (pObj)
	{
		//取消已有的背景对象
		CWBObject *pBkgObj = g_pWBPage->GetBkgObj();
		if (pBkgObj)
		{
			pBkgObj->SetBkg(FALSE);

			//发送同步信息
			XMemIni xmlObj;
			pBkgObj->SetXmlValue(xmlObj);
			ChangeObj(xmlObj);
		}
		//pObj->DrawAsBakground(g_pWBPage->GetMemDC(),this);
		//pObj->SetBkg(TRUE);
		Invalidate();
		//pObj->DrawAsBakground(, this);

		//发送同步信息
		XMemIni xmlObj;
		pObj->SetXmlValue(xmlObj);
		ChangeObj(xmlObj);
	}
}

CWBFunctionDlg * CWhiteBoardView::GetWBFunctionDlg()
{
	return m_pWBFunctionDlg;
}


CWBOptionDlg * CWhiteBoardView::GetWBOptionDlg()
{
	return m_pWBOptionDlg;
}

void CWhiteBoardView::DeleteFuncPad()
{

	if (m_pWBFunctionDlg != NULL)
	{
		delete m_pWBFunctionDlg;
		m_pWBFunctionDlg = NULL;
	}
	//if (m_pWBOptionDlg != NULL)
	//{
	//	//delete m_pWBOptionDlg;
	//	m_pWBOptionDlg = NULL;
	//}
}



afx_msg LRESULT CWhiteBoardView::OnPraviewRefresh(WPARAM wParam, LPARAM lParam)
{
	//文件打开新页
	g_pWhiteBoardView->m_bLocalFileOperate = true;
	if (IsOpenPreviewPanel())
	{
		g_pWhiteBoardView->GetWBFunctionDlg()->GetPreviewDLG()->RflashViewList();
	}
	else
	{
		g_pWhiteBoardView->m_bLocalFileOperate = false;
	}
	return 0;
}

//
//afx_msg LRESULT CWhiteBoardView::OnClosewbfunctiondlg(WPARAM wParam, LPARAM lParam)
//{
//	if (g_pWhiteBoardView == NULL)
//	{
//		return FALSE;
//	}
//	if (IsOpenFunctionDlg())
//	{
//		g_pWhiteBoardView->GetWBFunctionDlg()->OnBnClickedClosewbfunctiondlg();
//	}
//	return 0;
//}


//将接收到的同步数据存入缓冲队列中
void CWhiteBoardView::OnNETEC_NodeReceivedFromRouter(const char* pData,int nLen)
{
	if(nLen<=1 || pData==NULL)
	{
		return;
	}
	//TRACE("\n[SYS] RECV:%s\n",pData);
	int icmd = *((int*)pData);
	if (icmd == CMD_CONFERENCE_RECORDDATA)
	{
		m_bIsconfernce = true;
		SetIsMedialubo(TRUE);
		WB_AvconLock(true);
		int iWbcmd = *((int*)(pData + sizeof(int)));
		if ( iWbcmd == CMD_SAVE_STREAM )/*解析命令 判断文件内容是不是录像发过来的*/
		{
			int ndatalen = 0;
			ndatalen += sizeof(int) + sizeof(int);
			int nstr_path_size = 	*(int*)(pData +ndatalen);
			ndatalen += sizeof(int);
			string str_path = "";
			str_path = pData + ndatalen;
			ndatalen += nstr_path_size;
			int nFilePos = *(int*)(pData + ndatalen);
			ndatalen += sizeof(int);

			int npos = str_path.find_last_of("\\") +1;
			string str_name = str_path.substr(npos);
			str_path = ToStr(g_strAvconTempPath.GetBuffer(g_strAvconTempPath.GetLength()));
			str_path += "\\";
			g_strAvconTempPath.ReleaseBuffer();
			if (_access(str_path.c_str(),0) == -1)
			{
				int iPos = 0;
				iPos = str_path.find_first_of("\\",iPos);
				while (iPos != -1)
				{
					iPos = str_path.find_first_of("\\",iPos+1);
					string strPath = str_path.substr(0,iPos);
					int i = _mkdir(strPath.c_str());
				}
			}
			str_path += str_name;

			FILE*	fp = NULL;
			if(_access(str_path.c_str(),0) == -1)
				fopen_s(&fp,str_path.c_str(), "wb");
			else
				fopen_s(&fp,str_path.c_str(), "r+b");
			if	(fp)
			{
				fseek(fp, nFilePos, SEEK_SET);
				fwrite(pData + ndatalen, 1, nLen - ndatalen, fp);	
				fclose(fp);
			}
		}
		else
		{
			XDataBuffer*pXDataBuffer=new XDataBuffer();
			pXDataBuffer->StoreData((char *)(pData + sizeof(int)),nLen - sizeof(int));
			m_tXBufferPool.SetDataBuffer(pXDataBuffer);
		}
	}
	else
	{
		/*if ( *(pData+nLen-1)!='\0' )//对会议录像视频有影响
		{
		return;
		}*/

		XDataBuffer*pXDataBuffer=new XDataBuffer();
		pXDataBuffer->StoreData((char *)pData,nLen);
		m_tXBufferPool.SetDataBuffer(pXDataBuffer);
	}
}
void CWhiteBoardView::OnCallbackData(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime) //白板中拖拽的回调函数
{
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->_OnCallbackData();
	}
	return ;
}

afx_msg LRESULT CWhiteBoardView::OnWbDelPageAllObj(WPARAM wParam, LPARAM lParam)
{
	CWBPage *pObjPage = NULL;
	int nPageId = (int)wParam;
	unsigned long ulEventID = (unsigned long)lParam;

	pObjPage = g_pWBFile->GetPage(nPageId);
	if (pObjPage == NULL)
	{
		return 0;
	}
	XAutoLock lock(m_csOnWbDeleteObjLock);
	pObjPage->DeleteAllObjects(ulEventID);
	Invalidate();
	if (CScreenDrawMgr::GetIns() != NULL)
	{
		CScreenDrawMgr::GetIns()->Invalidate();
	}
	return 0;
}

void CWhiteBoardView::_OnCallbackData()
{

	XAutoLock lock(m_csDraw);
	XDataBuffer*pXDataBuffer=m_tXBufferPool.GetDataBuffer();
	if (pXDataBuffer == NULL )
		return;
	while (pXDataBuffer)
	{	
		if (pXDataBuffer->GetData())
		{
			int nCmdType = *((int*)(pXDataBuffer->GetData()));
			if (nCmdType == CMD_BROADCAST_DATA) 
			{
				ProcBroadcast_Data((char*)pXDataBuffer->GetData(),pXDataBuffer->GetLen());
			}
			else
			{
				//网络
				ProcessPacketData((char*)pXDataBuffer->GetData(),pXDataBuffer->GetLen());
				if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord) // 给会议录像发送内容
				{
					char* pbuf = (char*)pXDataBuffer->GetData();
					int iCmdType = *((int*)pbuf);
					if (CMD_CHANGE_ATTRIBUTE != iCmdType  &&  CMD_ADD_ATTRIBUTE != iCmdType)   //删除命令解析方式跟别的命令解析不一样
					{
						g_pConferenceRecordNotify->NotifyConferenceRecordData((char*)pXDataBuffer->GetData(),pXDataBuffer->GetLen(),true);
					}
				}
			}
			delete pXDataBuffer;
			pXDataBuffer = NULL;
			pXDataBuffer = m_tXBufferPool.GetDataBuffer();
		}


		if ( NULL == pXDataBuffer) 
		{
			//m_bChangeObjectList  = true;
			if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
			{
				CScreenDrawMgr::GetIns()->Invalidate();
			}
			Invalidate();	
		}
	}
}


void CWhiteBoardView::SetDocConvert(BOOL bIsDocConvert)
{
	m_bIsDocConvert = bIsDocConvert;
}
BOOL CWhiteBoardView::GetDocConvert()
{
	return m_bIsDocConvert;
}

void CWhiteBoardView::ViewWindowFinalTreat()
{
	if (g_pWhiteBoardView == NULL || !IsWindow(g_pWhiteBoardView->GetSafeHwnd()))
	{
		return;
	}
	CheckVectorBeforCloseWB();
	//12-2-1jinpeng 如果掉线或者对方宣布所有会议结束，同时还打开了白板中的空白处的右键菜单，则需要发送关闭菜单的模态消息
	g_pWhiteBoardView->SendMessage(WM_CANCELMODE, 0, 0);

	if (CWBRichEditMgr::GetInst_()->IsRichEditShow())
	{
		g_pWhiteBoardView->SendMessage(UWM_DESTROY_RICHEDIT, 0, 0); //加了这句，文本在编辑状态时退出白板，不会产生内存泄露
	}
	g_bMedioScreenWndEnd = FALSE;

	//屏幕录制窗口意外关闭，先关闭模态窗口
	if (g_hVedioHwnd)
	{
		::SendMessage(g_hVedioHwnd,WM_LBUTTONUP,0,0);

	}
	if (m_pTransparent_dlg)
	{
		m_pTransparent_dlg->SendMessage(WM_CLOSE);
	}
	DestroyScreenDraw();
	ReJectAllBroadcastUserCursor();
	ReJectAllBroadcastUserWater();
	ReJectAllProcessCamInfo();
	KillTimer(CHECKMEMORYSTATUS);
	if (m_nTimerID) //取消网络数据缓冲池定时器
	{
		::KillTimer(NULL,m_nTimerID);
		m_nTimerID=0;
	}
	KillProReliefTimer();
	if (m_pDlgPrintLoadingMsg)
	{
		m_pDlgPrintLoadingMsg->DestroyWindow();
		delete m_pDlgPrintLoadingMsg;
		m_pDlgPrintLoadingMsg = NULL;
	}
	//销毁触摸相关数据
	if(CWBMTObjectManager::GetIns_())
	{
		CWBMTObjectManager::GetIns_()->Destroy();
	}


	m_tXBufferPool.Flush();

	if (m_waterdlg)
	{
		delete m_waterdlg;
		m_waterdlg = NULL;
	}
	RemoveAllMediaListDate();
	DestroyFullScreen();

}

HWND CWhiteBoardView::GetSpotLightHWND()
{
	if (NULL != m_WBSpotlight)
	{
		return m_WBSpotlight->GetSafeHwnd();
	}
	return NULL;
}
void CWhiteBoardView::HideResBar()
{
	BOOL bIsResCreted = WBRESWND::IsGalleryWndCreated();
	if(bIsResCreted)
	{
		WBRESWND::HideGalleryWndEx();
	}

}
void CWhiteBoardView::ReductionPrimaryWB()
{
	//还原白板打开的开始状态
	/*	SendMessage(WM_CLOSEWBFUNCTIONDLG,0,0);*/
	setCurrentViewScroll(CPoint(0,0));
	BOOL bIsResCreted = WBRESWND::IsGalleryWndCreated();
	if(bIsResCreted)
	{
		WBRESWND::HideGalleryWndEx();
	}
	if (m_MedioVideoScreenDlg)
	{
		delete m_MedioVideoScreenDlg;
		m_MedioVideoScreenDlg = NULL;
	}
	if (NULL != m_WBSpotlight)
	{
		delete m_WBSpotlight;
		m_WBSpotlight =NULL;
	}
	if (NULL != m_pWBRectMaqn)
	{
		delete m_pWBRectMaqn;
		m_pWBRectMaqn = NULL;
	}
	WB_CloseAllChildBox();
}

bool CWhiteBoardView::CreateLoadingPictureDlg()
{
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->SendMessage(WM_CREATE_PRINT_LOADINGMSG,0,0);
	}
	return true;
}

bool CWhiteBoardView::DestoryLoadingPictureDlg()
{
	if (m_pDlgPrintLoadingMsg)
	{
		m_pDlgPrintLoadingMsg->DestroyWindow();
		delete m_pDlgPrintLoadingMsg;
		m_pDlgPrintLoadingMsg = NULL;
	}
	return true;
}

HWND CWhiteBoardView::GetDlgPrintLoadingMsgHWND()
{
	if (m_pDlgPrintLoadingMsg)
	{
		if (IsWindow(m_pDlgPrintLoadingMsg->m_hWnd))
		{
			return m_pDlgPrintLoadingMsg->m_hWnd;
		}
	}
	return NULL;
}

afx_msg LRESULT CWhiteBoardView::OnCreatePrintLoadingMsg(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgPrintLoadingMsg)
	{
		delete m_pDlgPrintLoadingMsg;
		m_pDlgPrintLoadingMsg =NULL;
	}
	m_pDlgPrintLoadingMsg = new CDlgPrintLoadingMsg(this);
	m_pDlgPrintLoadingMsg->Create(CDlgPrintLoadingMsg::IDD,this);

	return 0;
}



void CWhiteBoardView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return;
	__super::OnMouseHWheel(nFlags, zDelta, pt);
}


BOOL CWhiteBoardView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return FALSE;
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CWhiteBoardView::GetUserColor()
{
#ifdef HUBEI_MEDICAL_SDK

	string strAvconUserID = "";
	strAvconUserID = ToStr(g_strAvconUserID.GetBuffer(g_strAvconUserID.GetLength()));
	g_strAvconUserID.ReleaseBuffer();
	AVDataOutPacket outPacket;
	int nLen = sizeof(int) + strAvconUserID.length();
	m_IsGetAllObj = true;
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		int iCmd = CMD_ASKFOR_USERCOLOR;
		memset(pData,0,nLen);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		memcpy(pData + sizeof(int),strAvconUserID.c_str(),strAvconUserID.length());
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
	}

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
#endif
}

void CWhiteBoardView::SendDataToConferenceRecord( XMemIni xmlobj ,int iCmdType /*= CMD_ADD_ATTRIBUTE */,bool bkeyFrame /*= true*/ )
{
	if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord)
	{
		XAutoLock l(m_csConferenceRecord);
		std::string strValue = "";
		int nPageIndex;
		int nType;
		if (!xmlobj.Get(KEY_OBJ_PAGE, nPageIndex)) //如果没有填写页码，则填写当前页的页码
		{
			nPageIndex = (int)g_pWBFile->GetCurPageIndex();
			xmlobj.Set(KEY_OBJ_PAGE, nPageIndex); //填写页码
		}
		if (xmlobj.Get(KEY_OBJ_TYPE,nType))
		{
			if (nType == WB_OBJ_MEDIA)
			{
				xmlobj.Set(KEY_MEDIA_ADDSESSION,g_ulSessionID);
			}
			if (nType == WB_OBJ_CAMERA)
			{
				xmlobj.Set(KEY_CAREMA_ADDSESSION,g_ulSessionID);
			}
		}
		xmlobj.Set(KEY_FILE_SENDER,g_ulSessionID);
		xmlobj.Set(KEY_CONFERENCE_RECORD,1);//设置是录像数据
		xmlobj.GetString(strValue);
		if (strValue != "")
		{
			int nLen = sizeof(int) + sizeof(int) + strValue.size();
			//int nLen = sizeof(int)  + strValue.size();
			char* pData = new char[nLen+1];
			if (pData)
			{
				int iConFerenceRecord = CMD_CONFERENCE_RECORDDATA;
				int iCmd = iCmdType;
				memset(pData,0,nLen + 1);
				memcpy(pData,(char*)&iConFerenceRecord,sizeof(int));
				memcpy(pData + sizeof(int),(char*)&iCmd,sizeof(int));
 				memcpy(pData + sizeof(int) + sizeof(int),strValue.c_str(),strValue.size());
				//memcpy(pData,(char*)&iCmd,sizeof(int));
				//memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
				pData[nLen] = '\0';
			}
			g_pConferenceRecordNotify->NotifyConferenceRecordData(pData,nLen + 1,bkeyFrame);
			if (pData)
			{
				delete[]pData;
				pData = NULL;
			}
		}
		if (g_pWBPage)
		{
			g_pWBPage->SetbStartConferenceRecord(true);
		}
	}
}
void CWhiteBoardView::SenMediadDataToConferenceRecord(unsigned char*pData,int nLen,int nKeyFrame,DWORD dwLayer /*= -1*/ )//主动视频
{
	if (pData == NULL || nLen <= 0 )
	{
		return ;
	}
	if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord )
	{
		int iCmdType = CMD_MEDIA_OPERATION;
		int nDataLen = sizeof(int) + sizeof(DWORD) + nLen;
		char* pBuff = new char[nDataLen];

		if (dwLayer <= 0)
		{
			dwLayer = m_MediaIDIsActive;
		}
		memset(pBuff,0,nDataLen);
		memcpy(pBuff,(char*)&iCmdType,sizeof(int));
		memcpy(pBuff + sizeof(int),(char*)&dwLayer,sizeof(DWORD));
		memcpy(pBuff + sizeof(int) + sizeof(DWORD),pData,nLen);

		g_pConferenceRecordNotify->NotifyConferenceRecordData(pBuff,nDataLen,nKeyFrame);

		if (pBuff)
		{
			delete[] pBuff;
			pBuff = NULL;
		}

	}
}
void CWhiteBoardView::SenPlayMediadDataToConferenceRecord(unsigned char*pData,int nLen,int nKeyFrame,DWORD dwLayer /*= -1*/ )//被动视频
{
	if (pData == NULL || nLen <= 0 )
	{
		return ;
	}
	if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord )
	{
		int iCmdType = CMD_MEDIA_OPERATION;
		int nDataLen = sizeof(int) + sizeof(DWORD) + nLen;
		char* pBuff = new char[nDataLen];
		
		if (dwLayer <= 0)
		{
			dwLayer = m_MediaID;
		}
		memset(pBuff,0,nDataLen);
		memcpy(pBuff,(char*)&iCmdType,sizeof(int));
		memcpy(pBuff + sizeof(int),(char*)&dwLayer,sizeof(DWORD));
		memcpy(pBuff + sizeof(int) + sizeof(DWORD),pData,nLen);
		
		g_pConferenceRecordNotify->NotifyConferenceRecordData(pBuff,nDataLen,(bool)nKeyFrame);
		if (pBuff)
		{
			delete[] pBuff;
			pBuff = NULL;
		}

	}
}

void CWhiteBoardView::SenAudioDataToConferenceRecord(unsigned char*pData,int nLen,bool&bLoop,DWORD dwLayer /*= -1*/ )//主动音频
{
	if (pData == NULL || nLen <= 0 )
	{
		return ;
	}
	if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord )
	{
		int iCmdType = CMD_MEDIA_AUDIO_ID;
		int nDataLen = sizeof(int) + sizeof(DWORD) + nLen;
		char* pBuff = new char[nDataLen];
		
		if (dwLayer == -1)
		{
			dwLayer = m_MediaIDIsActive;
		}
		memset(pBuff,0,nDataLen);
		memcpy(pBuff,(char*)&iCmdType,sizeof(int));
		memcpy(pBuff + sizeof(int),(char*)&dwLayer,sizeof(DWORD));
		memcpy(pBuff + sizeof(int) + sizeof(DWORD),pData,nLen);
		
		g_pConferenceRecordNotify->NotifyConferenceRecordData(pBuff,nDataLen,bLoop);

		if (pBuff)
		{
			delete[] pBuff;
			pBuff = NULL;
		}

	}
}
void CWhiteBoardView::SenPlayAudioDataToConferenceRecord(unsigned char*pData,int nLen,bool&bLoop,DWORD dwLayer /*= -1*/ )//beidong
{
	if (pData == NULL || nLen <= 0 )
	{
		return ;
	}
	if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord )
	{
		int iCmdType = CMD_MEDIA_AUDIO_ID;
		int nDataLen = sizeof(int) + sizeof(DWORD) + nLen;
		char* pBuff = new char[nDataLen];
		if (dwLayer <= 0)
		{
			dwLayer = m_MediaID;
		}
		memset(pBuff,0,nDataLen);
		memcpy(pBuff,(char*)&iCmdType,sizeof(int));
		memcpy(pBuff + sizeof(int),(char*)&dwLayer,sizeof(DWORD));
		memcpy(pBuff + sizeof(int) + sizeof(DWORD),pData,nLen);
		
		g_pConferenceRecordNotify->NotifyConferenceRecordData(pBuff,nDataLen,bLoop);

		if (pBuff)
		{
			delete[] pBuff;
			pBuff = NULL;
		}

	}
}
bool CWhiteBoardView::SendFileToConferenceRecord( string str_path )
{
	if (str_path == "")
	{
		return false;
	}
	else
	{
		if (_access(str_path.c_str(),0) == 0)//文件存在
		{
			XAutoLock l(m_csConferenceRecord);
			int iConFerenceRecord = CMD_CONFERENCE_RECORDDATA;
			int iCmd = CMD_SAVE_STREAM;
			FILE* pFile = NULL;
			fopen_s(&pFile, str_path.c_str(), "rb");
			//在这里发送文件数据给会议录像
			if (pFile)
			{
				int nlpos = 0;
				int nstr_path_size = str_path.length();
				int iLength = sizeof(int) + sizeof(int) + sizeof(int) + nstr_path_size + sizeof(int) +  8192; //录像标识 + 命令类型 + 路径长度 + 文件路径 +  文件发送的位置 + 文件内容
				char* pStreamData = new char[iLength + 1];
				
				int nDataLen = 0;
				while(!feof(pFile))
				{
					memset(pStreamData,0,iLength +1);
					nDataLen = 0;
					memcpy(pStreamData,(char*)&iConFerenceRecord,sizeof(int)); //录播命令
					nDataLen += sizeof(int);
					memcpy(pStreamData + nDataLen ,(char*)&iCmd,sizeof(int)); //白板命令
					nDataLen += sizeof(int);
					memcpy(pStreamData + nDataLen,(char*)&nstr_path_size,sizeof(int));//文件路径长度	
					nDataLen += sizeof(int);
					memcpy(pStreamData + nDataLen,str_path.c_str(),nstr_path_size); //文件路径
					nDataLen += nstr_path_size;
					memcpy(pStreamData + nDataLen,(char*)&nlpos,sizeof(int)); //文件的位置
					nDataLen += sizeof(int);
					int nLen=fread(pStreamData + nDataLen,1,8192,pFile); // 文件内容
					nDataLen += nLen;
					if (nLen >= 0)
					{
						g_pConferenceRecordNotify->NotifyConferenceRecordData(pStreamData,nDataLen,false);
					}
					nlpos += nLen;
				}
				if (pStreamData)
				{
					delete []pStreamData;
					pStreamData = NULL;
				}
				
				fclose(pFile);
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool CWhiteBoardView::SendLocalDataToConferenceRecord( char* pData,int nLen,bool bKeyFrame /*= true*/ )
{
	if (pData == NULL || nLen <= 0 || *(pData + nLen - 1) != '\0')
	{
		return false;
	}
	int iCmdType = *((int*)pData);
	if (iCmdType == CMD_ADD_ATTRIBUTE || iCmdType == CMD_CHANGE_ATTRIBUTE)
	{
		char* pBuff = (char*)pData + sizeof(int);
		std::string strValue = pBuff;
		XMemIni xmlobj;
		xmlobj.ResetString(strValue);
		int nType;
		if (xmlobj.Get(KEY_OBJ_TYPE,nType))
		{
			if (nType == WB_OBJ_MEDIA || nType == WB_OBJ_CAMERA)
			{
				SendDataToConferenceRecord(xmlobj,iCmdType); //发送文件属性
				return true;
			}
		}
		string str_path = "";

		if (xmlobj.Get(KEY_FILE_PATH,str_path) && str_path != "")
		{
			if (iCmdType == CMD_ADD_ATTRIBUTE)
			{
				SendFileToConferenceRecord(str_path); //发送文件内容
			}
			Sleep(20);
			SendDataToConferenceRecord(xmlobj,iCmdType); //发送文件属性
			return true;
		}
	}
	g_pConferenceRecordNotify->NotifyConferenceRecordData(pData,nLen,bKeyFrame);
	return true;
}


afx_msg LRESULT CWhiteBoardView::OnSendPageDatatoConferenceRecord(WPARAM wParam, LPARAM lParam)
{
	if (g_pWBFile)
	{
		g_pWBFile->GetCurPage()->SendAllPageDataToConferenceRecord();
		if (wParam == 1)
		{
			//Sleep(300);
			//XMemIni xmlObj;
			//xmlObj.Set(KEY_OBJ_ID, 0);
			//xmlObj.Set(KEY_OBJ_TYPE, 0);
			//xmlObj.Set(KEY_EVENT_ID,0);
			//xmlObj.Set(KEY_FILE_SENDER, g_ulSessionID);
			//float fScrollScaleX = (float)g_pWBPage->GetPageViewScroll().x;
			//float fScrollScaleY = (float)g_pWBPage->GetPageViewScroll().y;
			//fScrollScaleX = fScrollScaleX/m_fWbRatioX;
			//fScrollScaleY = fScrollScaleY/m_fWbRatioY;
			//xmlObj.Set(KEY_PAGEINDEX, (int)(g_pWBFile->GetCurPageIndex()));
			//xmlObj.Set(KEY_PAGE_SCROLL_X, fScrollScaleX);
			//xmlObj.Set(KEY_PAGE_SCROLL_Y, fScrollScaleY);
			//SendDataToConferenceRecord(xmlObj,CMD_CHANGE_ATTRIBUTE,false);
		}
	}
	return 0;
}

void CWhiteBoardView::SetMoveIsNeedSendData( BOOL MoveIsNeedSendData )
{
	m_bMoveIsNeedSendData = MoveIsNeedSendData;
}

BOOL CWhiteBoardView::GetMoveIsNeedSendData()
{
	return m_bMoveIsNeedSendData;
}


void CWhiteBoardView::SendMoveSelectedObjOffset( CPoint cpoffset,unsigned long ulEventId )
{
	CWBObject *pObj = NULL;
	int nIndex = 0;
	CString strPointItem;
	strPointItem.Format(_T("%04d%04d"), 
		cpoffset.x,
		cpoffset.y);
	string strPoints = ToStr(strPointItem.GetBuffer(strPointItem.GetLength()));
	strPointItem.ReleaseBuffer();
	std::vector<DWORD> vecSelectFrameList;
	vecSelectFrameList = g_pWBPage->GetWBSelectFrameList();
	int nCount = vecSelectFrameList.size();
	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{
		pObj = g_pWBPage->FindByLayerIndex(vecSelectFrameList[nIndex]);
		if (pObj == NULL)
		{
			continue;
		}
		if (pObj->IsLock())
		{
			continue;
		}
		XMemIni xmlObj;
		xmlObj.Set(KEY_OBJ_ID, pObj->GetLayerIndex());
		xmlObj.Set(KEY_OBJ_TYPE, pObj->GetType());
		xmlObj.Set(KEY_MOVE_OFFSET, strPoints);
		xmlObj.Set(KEY_EVENT_OBJNUMBER, nCount);
		xmlObj.Set(KEY_EVENT_ID, ulEventId);
		xmlObj.Set(KEY_BROADCAST_CMD,_E_OBJ_MOVE);


		/************************************************************************/
		/*    8-26                                                                  */
		CPoint ptDrawCenter;
		pObj->GetDrawRotationCenter(ptDrawCenter);
		xmlObj.Set(KEY_DRAWRORATTION_CENTERX, ptDrawCenter.x);
		xmlObj.Set(KEY_DRAWRORATTION_CENTERY, ptDrawCenter.y);
		/************************************************************************/

		if (pObj->GetType() == WB_OBJ_DOCUMENT)
		{
			DWORD dwLayerIndex = 0;
			CWBObject *pSelectObj = ((CWBDocument *)pObj)->GetSelectComObj();
			if (pSelectObj != NULL)
			{
				dwLayerIndex = pSelectObj->GetLayerIndex();
			}
			xmlObj.Set(KEY_EVENT_DOCSELECTOBJ, dwLayerIndex);
		}
		if (pObj->GetType() == WB_OBJ_PICTURE)
		{
			if (((CWBPicture *)pObj)->GetIsPrintScreen())
			{
				DWORD dwLayerIndex = 0;
				CWBObject *pSelectObj = ((CWBPicture *)pObj)->GetSelectComObj();
				if (pSelectObj != NULL)
				{
					dwLayerIndex = pSelectObj->GetLayerIndex();
				}
				xmlObj.Set(KEY_EVENT_DOCSELECTOBJ, dwLayerIndex);
			}
		}
		if (pObj->GetType() == WB_OBJ_MIDI)
		{
			DWORD dwLayerIndex = 0;
			CWBObject *pSelectObj = ((CWBMidi*)pObj)->GetSelectComObj();
			if (pSelectObj != NULL)
			{
				dwLayerIndex = pSelectObj->GetLayerIndex();
			}
			xmlObj.Set(KEY_EVENT_DOCSELECTOBJ, dwLayerIndex);
		}
		Broadcast_Data(xmlObj);
	}
}

void CWhiteBoardView::MoveObj_Realtime_Synchronizatio( XMemIni xmlobj )
{
	unsigned long ulObjId = 0;
	int nPageIndex = 0;
	int nType = 0;
	unsigned long ulEventID; 
	string strPoints = "";
	if (!xmlobj.Get(KEY_OBJ_ID,ulObjId)
		|| !xmlobj.Get(KEY_MOVE_OFFSET,strPoints)
		|| !xmlobj.Get(KEY_OBJ_TYPE,nType)
		|| !xmlobj.Get(KEY_EVENT_ID,ulEventID)
		|| !xmlobj.Get(KEY_OBJ_PAGE,nPageIndex)
		)
	{
		return;
	}

	CPoint point;
	int nIndex = 0;
	string strTemp = "";
	strTemp = strPoints.substr(nIndex, 4);
	point.x = atoi(strTemp.c_str());
	nIndex += 4;
	strTemp = strPoints.substr(nIndex, 4);
	point.y = atoi(strTemp.c_str());

	CWBPage *pObjPage = g_pWBFile->GetPage(nPageIndex);
	if (pObjPage == NULL || g_pWhiteBoardView == NULL)
	{
		return ;
	}
	WB_OBJ_TYPE objType = (WB_OBJ_TYPE)nType;

	CWBObject *pObj = pObjPage->FindByLayerIndex(ulObjId, objType);
	if (pObj != NULL) 
	{

		/************************************************************************/
		/*                                                                      */
		int DrawptX = 0;
		int DrawPtY = 0;
		xmlobj.Get(KEY_DRAWRORATTION_CENTERX, DrawptX);
		xmlobj.Get(KEY_DRAWRORATTION_CENTERY, DrawPtY);
		pObj->SetDrawRotationCenter(CPoint(DrawptX, DrawPtY));
		/************************************************************************/


		// 让广播数据处理完了之后限制其操作，在接受到chang消息或add消息的时候才打开 huanglongquan add by 2012年12月14日
		unsigned long ulTmepObjId = pObj->GetLayerIndex();
		//pObj->Limit(false);
		RemoveLimitObjId((void*)pObj);
		
		CPoint ptRotationCenter;
		pObj->GetRotationCenter(ptRotationCenter);
		ptRotationCenter += point;
		pObj->RotateByPoint(ptRotationCenter,0);

		if (pObj->GetType() == WB_OBJ_DOCUMENT)
		{
			DWORD dwLayerIndex = 0;
			xmlobj.Get(KEY_EVENT_DOCSELECTOBJ, dwLayerIndex);
			((CWBDocument *)pObj)->SetSelectedObj(dwLayerIndex);
		}
		if (pObj->GetType() == WB_OBJ_PICTURE)
		{
			if (((CWBPicture *)pObj)->GetIsPrintScreen())
			{
				DWORD dwLayerIndex = 0;
				xmlobj.Get(KEY_EVENT_DOCSELECTOBJ, dwLayerIndex);
				((CWBPicture *)pObj)->SetSelectedObj(dwLayerIndex);
			}
		}
		if (pObj->GetType() == WB_OBJ_MIDI)
		{
			DWORD dwLayerIndex = 0;
			xmlobj.Get(KEY_EVENT_DOCSELECTOBJ, dwLayerIndex);
			((CWBMidi *)pObj)->SetSelectedObj(dwLayerIndex);
		}
		pObj->MoveOffset(point);
		int nbObjLimit = -1;
		xmlobj.Get(KEY_OBJ_LIMIT,nbObjLimit);
		bool bLimit =( nbObjLimit== 1 ? true : false );
		//pObj->Limit(bLimit);
		if(bLimit)
		{
			//加入限制类控制其对象操作
			AddLimitObjId((void*)pObj);
		}

		int nCounts = 1;
		xmlobj.Get(KEY_EVENT_OBJNUMBER,nCounts);
		if (m_nMoveObj_Realtime_Synchronizatio_Counts % nCounts == 0)
		{
			if (CScreenDrawMgr::GetIns() != NULL)
				CScreenDrawMgr::GetIns()->Invalidate();
			else
				this->Invalidate();
		}
		++m_nMoveObj_Realtime_Synchronizatio_Counts  ;
		if (m_nMoveObj_Realtime_Synchronizatio_Counts > nCounts)
		{
			m_nMoveObj_Realtime_Synchronizatio_Counts = 1;
		}


	}
}
void CWhiteBoardView::BroadcastUserCursor( CPoint pt,string str_username )
{
	if ( !m_bBroadcastUserCursor ||  !g_pHPWBDataNotify)
	{
		return;
	}
	XMemIni xmlObj;
	xmlObj.Set(KEY_FILE_SENDER,g_ulSessionID);
	xmlObj.Set(KEY_BROADCAST_CMD,_E_CURSOR_MOVE);
	xmlObj.Set(KEY_BROADCASTUSERCURSOR,_E_MOVE_TRANARENT_CURSOR);
	std::string str_temp = str_username;
	xmlObj.Set(KEY_STRAVCONUSERID, str_temp);
	char pData[256] = {0};
	sprintf_s(pData,"%04d%04d ",pt.x,pt.y);
	str_temp = pData;
	xmlObj.Set(KEY_CURSORPOINT,str_temp);
	string strValue ="";
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_BROADCAST_DATA;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}
		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());
		
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	
	}
}

void CWhiteBoardView::RejectBroadcastUserCursor(string str_name)
{
	if ( m_bBroadcastUserCursor || !g_pHPWBDataNotify || str_name == (""))
	{
		return;
	}
	XMemIni xmlObj;
	xmlObj.Set(KEY_BROADCAST_CMD,_E_CURSOR_MOVE);
	xmlObj.Set(KEY_FILE_SENDER,g_ulSessionID);
	xmlObj.Set(KEY_BROADCASTUSERCURSOR,_E_CLOSE_TRANARENT_CURSOR);
	xmlObj.Set(KEY_STRAVCONUSERID, str_name);
	std::string strValue = "";
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_BROADCAST_DATA;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}
		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());
		
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
		
	}
}

void CWhiteBoardView::OnRejectBroadcastUserCursor( string str_name )
{
	if (str_name == ToStr(g_strAvconUserID.GetBuffer(g_strAvconUserID.GetLength())))
	{
		if (m_pTransparent_dlg != NULL)
		{
			delete m_pTransparent_dlg;
			m_pTransparent_dlg = NULL;
		}
	}
	g_strAvconUserID.ReleaseBuffer();
	IT_MapCursorDlgPointer it = m_MapCursorDlgPointer.find(str_name);
	if (it != m_MapCursorDlgPointer.end())
	{
		CTransparent_Dlg* pDlg =  (CTransparent_Dlg*)it->second;
		pDlg->SendMessage(WM_CLOSE);
		m_MapCursorDlgPointer.erase(it);
	}
}


void CWhiteBoardView::ReJectAllBroadcastUserCursor()
{
	IT_MapCursorDlgPointer it = m_MapCursorDlgPointer.begin();
	while (it != m_MapCursorDlgPointer.end())
	{
		CTransparent_Dlg* pDlg =  (CTransparent_Dlg*)it->second;
		pDlg->SendMessage(WM_CLOSE);
		++it;
	}
	m_MapCursorDlgPointer.clear();
}


afx_msg LRESULT CWhiteBoardView::OnProcess_CurorTranDlg(WPARAM wParam, LPARAM lParam)
{
	string str_username = "";
	const char* _cpw = (char*)wParam;
	str_username = _cpw;

	string strPoints = "";
	const char* _cpl = (char*)lParam;
	strPoints = _cpl;
	std::string strTemp = "";
	int nIndex = 0;
	strTemp = strPoints.substr(nIndex, 4);
	int _x = atoi(strTemp.c_str());
	nIndex += 4;
	strTemp = strPoints.substr(nIndex, 4);
	int _y = atoi(strTemp.c_str());

	POINT point;
	point.x = _x;
	point.y = _y;
	CPoint pt(point);
	if (g_reduce>0.00)
	{
		pt.x = pt.x*g_reduce;
		pt.y = pt.y*g_reduce;
	}
	pt.x = pt.x - g_pWhiteBoardView->getXnPos();
	pt.y = pt.y - g_pWhiteBoardView->getYnPos();	
	CRect viewRect;
	GetClientRect(&viewRect);
	if (!viewRect.PtInRect(pt))
	{
		return 0;
	}
	IT_MapCursorDlgPointer it = m_MapCursorDlgPointer.find(str_username);
	if (it != m_MapCursorDlgPointer.end())
	{
		CTransparent_Dlg* pTranDlg = (CTransparent_Dlg*)it->second;
		CRect rect;
		pTranDlg->GetClientRect(&rect);
		POINT _ptBR = pt;
		pt.y -= 28;
		_ptBR.x += rect.Width() ;
		rect.SetRect(pt,_ptBR);
		ClientToScreen(&rect);
		pTranDlg->MoveWindow(&rect);
		pTranDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		CTransparent_Dlg* pTranDlg = new CTransparent_Dlg();
		if (pTranDlg == NULL)
		{
			delete _cpw;
			delete _cpl;
			return 0;
		}
		pTranDlg->SetStrname(str_username);
		pTranDlg->Create(CTransparent_Dlg::IDD,this);
		CRect rect;
		pTranDlg->GetClientRect(&rect);
		POINT _ptBR = pt;
		pt.y -= 28;
		_ptBR.x += rect.Width() ;
		rect.SetRect(pt,_ptBR);
		ClientToScreen(&rect);
		pTranDlg->MoveWindow(&rect);
		pTranDlg->ShowWindow(SW_SHOW);
		m_MapCursorDlgPointer.insert(MapCursorDlgPointer::value_type(str_username,pTranDlg));
	}

	delete _cpw;
	delete _cpl;
	return 0;
}

void CWhiteBoardView::ProcBroadcast_Data( const char*pData,int nlen )
{
	if (m_pEditObj != NULL)
	{
		return;
	}
	if (pData == NULL || nlen < sizeof(int))
	{
		return;
	}
	char* pXmlData = new char[nlen + 1];
	if (pXmlData == NULL)
	{
		return;
	}
	memcpy(pXmlData,pData,nlen);
	int nCmdType = *((int*)pXmlData);
	if (nCmdType == CMD_BROADCAST_DATA)
	{

		char* pBuff = (char*)pXmlData + sizeof(int); 
		std::string strValue = pBuff;
		XMemIni xmlPacket;
		xmlPacket.ResetString(strValue);
		OnBroadcast_Data(xmlPacket);
	}
	if (pXmlData)
	{
		delete pXmlData;
		pXmlData = NULL;
	}
}

void CWhiteBoardView::RejectBroadcastUserWater(string str_name)
{
	if (!g_pHPWBDataNotify || str_name == "")
	{
		return;
	}
	XMemIni xmlObj;
	xmlObj.Set(KEY_FILE_SENDER,g_ulSessionID);
	xmlObj.Set(KEY_BROADCAST_CMD,_E_WATER_SPECIAL_EFFICIENCY);
	xmlObj.Set(KEY_BROADCASTUSERWATER,_E_CLOSEWATER);
	string str_temp = str_name;
	xmlObj.Set(KEY_WATER_USER, str_temp);
	string strValue = "";
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_BROADCAST_DATA;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}
		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
}

void CWhiteBoardView::OnRejectBroadcasstUserWater( string str_name )
{
	IT_MapWaterDlgPointer itwater = m_MapWaterDlgPointer.find(str_name);
	if (itwater != m_MapWaterDlgPointer.end())
	{
		CWaterDlg *waterdlg = (CWaterDlg *)itwater->second;
		waterdlg->SendMessage(WM_CLOSE);
		itwater->second = NULL;
		m_MapWaterDlgPointer.erase(itwater);
	}
}

void CWhiteBoardView::ReJectAllBroadcastUserWater()
{
	IT_MapWaterDlgPointer it = m_MapWaterDlgPointer.begin();
	while (it != m_MapWaterDlgPointer.end())
	{
		CWaterDlg* pDlg =  (CWaterDlg*)it->second;
		pDlg->SendMessage(WM_CLOSE);
		//delete pDlg;
		it->second = NULL;
		++it;		
	}
	m_MapWaterDlgPointer.clear();
}


afx_msg LRESULT CWhiteBoardView::OnProcess_WaterDlg(WPARAM wParam, LPARAM lParam)
{

	if (!g_bProcessWater)
	{
		return 0;
	}
	CPoint ProWaterpoint;
	ProWaterpoint.x = m_ReceiveWaterPoint.x - g_pWhiteBoardView->getXnPos();
	ProWaterpoint.y = m_ReceiveWaterPoint.y - g_pWhiteBoardView->getYnPos();
	CRect viewRectInScreen;
	GetWindowRect(&viewRectInScreen);
	ProWaterpoint.x += viewRectInScreen.left;
	ProWaterpoint.y += viewRectInScreen.top;
	if (!viewRectInScreen.PtInRect(ProWaterpoint))
	{
		return 0;
	}
	HDC       hScrDC, hMemDC;       // 屏幕和内存设备描述表
	HBITMAP  hBitmap,	hOldBitmap;    // 位图句柄
	int       nX, nY;       
	int       nWidth, nHeight;
	CBitmap *bitmap;
	nHeight = WATERDLG_HEIGHT;
	nWidth = WATERDLG_WIDTH;
	//为屏幕创建设备描述表
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = ProWaterpoint.x - WATERDLG_WIDTH/2;
	nY = ProWaterpoint.y - WATERDLG_HEIGHT/2;
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight,hScrDC, nX, nY, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	bitmap = CBitmap::FromHandle(hBitmap);
	if (bitmap == NULL)
	{

		return -1;
	}
	IT_MapWaterDlgPointer itwater = m_MapWaterDlgPointer.find(m_ReceiveWaterUserID);
	if (itwater != m_MapWaterDlgPointer.end())
	{
		CWaterDlg* pTranDlg = (CWaterDlg*)itwater->second;
		pTranDlg->SetBackgroundBitmap(bitmap,ProWaterpoint);		
	}
	else
	{
		CWaterDlg* pTranDlg = new CWaterDlg(bitmap,ProWaterpoint);
		if (pTranDlg == NULL)
		{

			return -1;
		}
		pTranDlg->Create(CWaterDlg::IDD,this);		
		m_MapWaterDlgPointer.insert(MapWaterDlgPointer::value_type(m_ReceiveWaterUserID,pTranDlg));
	}
	DeleteObject(hBitmap);
	return 0;
}

LRESULT CWhiteBoardView::OnCreateWaterDlg( WPARAM wParam,LPARAM lParam )
{

	CPoint point;
	point.x = wParam;
	point.y = lParam;
	if (g_reduce>0.00)
	{
		point.x = point.x*g_reduce;
		point.y = point.y*g_reduce;
	}
	int nIndex;
	POSITION pos;
	CWBObject *pWBObj;
	CObList *pWBList = NULL;
	pWBList = g_pWBPage->GetWBObjList();
	if (pWBList == NULL)
	{
		return -1;
	}
	int nCount =pWBList->GetCount();
	for (nIndex = 0; nIndex < nCount; ++nIndex )
	{
		pos = pWBList->FindIndex(nIndex);
		if (pos == NULL)
		{
			continue;
		}
		pWBObj = (CWBObject *)pWBList->GetAt(pos);
		if (pWBObj == NULL)
		{
			continue;
		}
		if (pWBObj->GetType() ==WB_OBJ_MEDIA)
		{
			// 				int n = ((CWBMedia*)pWBObj)->IsInSelectArea(point);
			// 				if (n == WB_PS_SELECTFRAME ||n == WB_PS_INMEDIACTRL )
			// 				{
			// 					return;
			// 				}
			CRect rc = ((CWBMedia*)pWBObj)->GetMediaRect();
			rc.left = rc.left - WATERDLG_HEIGHT/2;
			rc.top = rc.top - WATERDLG_HEIGHT/2;
			rc.right = rc.right + WATERDLG_HEIGHT/2;
			rc.bottom = rc.bottom + WATERDLG_HEIGHT/2 + 95;
			if (rc.PtInRect(point))
			{
				return -1;
			}
		}
		else if (pWBObj->GetType() == WB_OBJ_FLASH)
		{
			CRect rc = ((CWBFlash*)pWBObj)->GetCurRect();
			rc.left = rc.left - WATERDLG_HEIGHT/2;
			rc.top = rc.top - WATERDLG_HEIGHT/2;
			rc.right = rc.right + WATERDLG_HEIGHT/2;
			rc.bottom = rc.bottom + WATERDLG_HEIGHT/2;
			if (rc.PtInRect(point))
			{
				return -1;
			}
		}

		else if (pWBObj->GetType() == WB_OBJ_CAMERA)
		{
			CRect rc = ((CWBCamera*)pWBObj)->GetCaremaRect();
			rc.left = rc.left - WATERDLG_HEIGHT/2;
			rc.top = rc.top - WATERDLG_HEIGHT/2;
			rc.right = rc.right + WATERDLG_HEIGHT/2;
			rc.bottom = rc.bottom + WATERDLG_HEIGHT/2;
			if (rc.PtInRect(point))
			{
				return -1;
			}
		}

	}
	
	CPoint ProWaterpoint;
	ProWaterpoint.x = point.x /*- g_pWhiteBoardView->getXnPos()*/;
	ProWaterpoint.y = point.y /*- g_pWhiteBoardView->getYnPos()*/;
	CRect viewRectInScreen;
	GetWindowRect(&viewRectInScreen);
	ProWaterpoint.x += viewRectInScreen.left;
	ProWaterpoint.y += viewRectInScreen.top;
	HDC       hScrDC, hMemDC;       // 屏幕和内存设备描述表
	HBITMAP  hBitmap,	hOldBitmap;    // 位图句柄
	int       nX, nY;       
	int       nWidth, nHeight;
	CBitmap *bitmap;
	nHeight = WATERDLG_HEIGHT;
	nWidth = WATERDLG_WIDTH;
	//为屏幕创建设备描述表
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = ProWaterpoint.x - WATERDLG_WIDTH/2;
	nY = ProWaterpoint.y - WATERDLG_HEIGHT/2;
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight,hScrDC, nX, nY, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	bitmap = CBitmap::FromHandle(hBitmap);
	if (bitmap == NULL)
	{
		return -1;
	}
	if (m_waterdlg == NULL)
	{
		m_waterdlg = new CWaterDlg(bitmap,ProWaterpoint);		
		m_waterdlg->Create(IDD_WATER_DIALOG,g_pWhiteBoardView);				
	}	
	else
	{	
		m_waterdlg->SetBackgroundBitmap(bitmap,ProWaterpoint);		
	}
	CPoint ptSend = CPoint(wParam,lParam);
	ptSend.x += getXnPos();
	ptSend.y += getYnPos();
    BroadcastUserWater(ptSend,ToStr(g_strAvconUserID.GetBuffer(g_strAvconUserID.GetLength())));
	g_strAvconUserID.ReleaseBuffer();
	DeleteObject(hBitmap);
	return 0;
}


bool CWhiteBoardView::AllowDeleteObj( DWORD dwObjId ,unsigned long EventID )
{
	if (dwObjId == m_dwCurOperationObjId)
	{
		return true;
	}


	//if (m_dwCurOperationObjId == 0)
	{
		std::vector<DWORD> vecSelectFrameList;
		vecSelectFrameList = g_pWBPage->GetWBSelectFrameList();
		std::vector<DWORD>::iterator it = vecSelectFrameList.begin();
		while (it != vecSelectFrameList.end())
		{
			if (*it == dwObjId)
			{
				//发送ADD组合成员对象
				//CWBObject *pObj = g_pWBPage->FindByLayerIndex(dwObjId);
				//if (pObj->GetType() == WB_OBJ_COMBINE)
				//{
				//	((CWBCombine *)pObj)->SetComXmlValue(EventID);
				//}
				return true;
			}
			++it;
		}
	}
	//触摸map
	if( !m_mapMTDrawPar.empty() )
	{
		std::map<DWORD, MTDrawPar*>::const_iterator itTouch = m_mapMTDrawPar.begin();
		while ( itTouch != m_mapMTDrawPar.end())
		{
			if ( dwObjId == itTouch->second->GetWBObject()->GetLayerIndex() )
				return true;
			++itTouch;
		}
	}

	return false;
}

void CWhiteBoardView::BroadCostReleastLimitObjs()
{
	if (g_pHPWBDataNotify == NULL)  return;
	XMemIni xmlObj;
	xmlObj.Set(KEY_BROADCAST_CMD,_E_RELEASE_LIMITOBJS);
	unsigned long ulSend = 0;
	if (!xmlObj.Get(KEY_FILE_SENDER,ulSend))
	{
		xmlObj.Set(KEY_FILE_SENDER,g_ulSessionID);
	}
	std::string strValue = "";
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned int)nLen;
		char* pData = new char[nLen+1];
		if (pData)
		{
			int iCmd = CMD_BROADCAST_DATA;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] = '\0';
			outPacket.WriteData((unsigned char*)pData,nLen+1);
		}
		SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
}

void CWhiteBoardView::OnReleaseLimitObjs()
{
	if(m_LimitLocalObj.LimitObjEmpty())
		return;
	g_pWBPage->ReleaseLimitObjs();
	m_LimitLocalObj._ReliefAllLimitObjs();
}

void CWhiteBoardView::SetPastePt(CPoint pt)
{
	m_ptPaste = pt;
}

void CALLBACK CWhiteBoardView::OnProRelief( HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime )
{
	if (g_pWhiteBoardView)
	{
		g_pWhiteBoardView->_OnProRelief();
	}
	return ;
}

void CWhiteBoardView::_OnProRelief()
{
	//huanglongquan add
	if( g_pHPWBDataNotify == NULL || m_LimitLocalObj.LimitObjEmpty()) 
		return;
	std::list<unsigned long>EraseList;
	m_LimitLocalObj._ReliefObjId(EraseList);
	if (EraseList.empty())
		return;
	std::list<unsigned long>::iterator _it = EraseList.begin();
	for(;_it != EraseList.end(); ++_it)
	{
		CWBObject* pObj = g_pWBPage->FindByLayerIndex(*_it);
		if (pObj == NULL)
			continue;
		pObj->Limit(false);
	}
	EraseList.clear();
}

void CWhiteBoardView::AddLimitObjId( unsigned long ulObjId )
{
	CWBObject* pWBObject = g_pWBPage->FindByLayerIndex(ulObjId);
	if (pWBObject == NULL)
		return;
	pWBObject->Limit(true);
	m_LimitLocalObj.AddLimitObjId(ulObjId);
}

void CWhiteBoardView::AddLimitObjId( void* pObject )
{
	if(pObject == NULL)
		return;
	CWBObject* pWBObject = (CWBObject*) pObject;
	pWBObject->Limit(true);
	m_LimitLocalObj.AddLimitObjId(pWBObject->GetLayerIndex());
}

void CWhiteBoardView::RemoveLimitObjId( unsigned long ulObjId )
{
	CWBObject* pWBObject = g_pWBPage->FindByLayerIndex(ulObjId);
	if (pWBObject == NULL)
		return;
	pWBObject->Limit(false);
	m_LimitLocalObj.RemoveLimitObjId(ulObjId);
}

void CWhiteBoardView::RemoveLimitObjId( void* pWBObj )
{
	if(pWBObj == NULL)
		return;
	CWBObject* pWBObject = (CWBObject*) pWBObj;
	pWBObject->Limit(false);
	m_LimitLocalObj.RemoveLimitObjId(pWBObject->GetLayerIndex());
}

void CWhiteBoardView::StartProReliefTimer()
{
	if (m_nTimerLimitObjPro == 0)
	{
		m_nTimerLimitObjPro=::SetTimer(NULL,10000,8000,(TIMERPROC)&OnProRelief);
	}
}

void CWhiteBoardView::KillProReliefTimer()
{
	if (m_nTimerLimitObjPro != 0)
	{
		::KillTimer(NULL,m_nTimerLimitObjPro);
		m_nTimerLimitObjPro = 0;
	}
}


void CWhiteBoardView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 	point.x = point.x -g_pWhiteBoardView->getXnPos();
	// 	point.y = point.y - g_pWhiteBoardView->getYnPos();
	// 	CRect viewRectInScreen;
	// 	GetWindowRect(&viewRectInScreen);
	// 	point.x += viewRectInScreen.left;
	// 	point.y += viewRectInScreen.top;
	if (g_bAvconLock) 
		return;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);
	int nTemp = g_pWBPage->PointInSelectFrame(point);
	
	DWORD nGetWBObjID = g_pWBPage->GetCurEditingObjLayerIndex();
	
	if (nTemp == WB_PS_SELECTFRAME)	//如果点在某个选择框内
	{
		CWBObject *pObj = g_pWBPage->FindByLayerIndex(nGetWBObjID);
		if (pObj != NULL)
		{
			if (pObj->GetType() == WB_OBJ_TEXT || pObj->GetType() == WB_OBJ_ANNOTATIONS)
			{
				m_dwMenuLayerIndex = nGetWBObjID;
				if (!pObj->IsLock() && !pObj->IsLimit() && !pObj->IsInfiniteClone())
				{
					m_bIsEditing = TRUE;
					m_pEditObj = pObj;
					if (pObj->GetType() == WB_OBJ_ANNOTATIONS)
					{
						m_bIsAnnotations = TRUE;
					}
					else if (pObj->GetType() == WB_OBJ_TEXT)
					{
						m_bIsAnnotations = FALSE;
					}
					g_pWBPage->DeleteSelectFrame(m_dwMenuLayerIndex);
					setFrameListAndObjLayerIndex();

					CRect rect = GetEditTextRect();
					if (!CWBRichEditMgr::GetInst_()->IsRichEditShow())
					{
						SendMessage( UWM_EDIT_RICHEDIT, (WPARAM)(MAKELONG(rect.left, rect.top)), (LPARAM)(MAKELONG(rect.right, rect.bottom)) );
						SetPageIsNeedDraw(TRUE);
						Invalidate();
					}
					m_bUseToControlDraw = true;
					g_pWBPage->DelWBObj(m_dwMenuLayerIndex);
					//RemoveObj(m_dwMenuLayerIndex);

					if (CScreenDrawMgr::GetIns() != NULL) //如果屏幕绘图打开
					{
						CScreenDrawMgr::GetIns()->Invalidate();
					}
					else
					{
						Invalidate();
					}

					if (m_bIsAnnotations)
					{
						g_nDrawStatus = ADD_ANNOTATIONS;
					} 
					else
					{
						g_nDrawStatus = DRAW_TEXT;
					}
					g_nDrawStatus = DRAWED_TEXT;

					DRAW_STATE nDrawStatus = g_nDrawStatus;
					g_nDrawStatus = DRAW_TEXT;
					if (NULL != m_pWBFunctionDlg)
					{
						if (::IsWindow(m_pWBFunctionDlg->GetSafeHwnd()))
						{
							::SendMessage(m_pWBFunctionDlg->m_hWnd,WM_SELECTWBPEN,0,0);
						}
					}
					g_nDrawStatus = nDrawStatus;
				
				}
			}
			if (pObj->GetType() == WB_OBJ_ARBITRARYTRIANGLE)
			{
				
				pObj->SetSelectFrameShow(FALSE);
				((CWBArbitraryTriangle*)pObj)->SetIsEdit(true);
				g_nDrawStatus = DRAW_NORMAL;
				Invalidate();
			}
			if (pObj->GetType() == WB_OBJ_PICTURE)
			{
				if (((CWBPicture *)pObj)->GetIsPrintScreen() && ((CWBPicture *)pObj)->IsEdited())
				{
					g_pWBPage->ClearSelectFrameList();
					m_strDBClickSavePath = ((CWBPicture *)pObj)->SavePictureToJpg();
					if (!m_strDBClickSavePath.IsEmpty())
					{
						m_dDBClickobjlayer = nGetWBObjID;
						m_bIsDBClick = true;
					}
					
				}
			}
		}
	}

	IT_MapDropCamInfo it = m_MapDropCamInfo.find(nGetWBObjID);
	if(it != m_MapDropCamInfo.end())
	{
		HPMC_DROP *pBuffer = new HPMC_DROP;
		DWORD nWBObjID = it->first;
		if (nWBObjID == nGetWBObjID)
		{
			m_nLDBKPoint = point;
			m_bCamaraMediaClk = true;
			HPMC_DROP* pData = it->second;
			XMemIni xmlObj;
			xmlObj.Set(KEY_CAMERA_ID_OF_UCC_LOGIN_NAME, pData->MemberName);
			xmlObj.Set(KEY_BROADCAST_CMD, _E_BOARD_CAMERA_NAME);//广播摄像头名
			Broadcast_Data(xmlObj);
			//比较当前打开的与将要打开的是否是同一个摄像头
			if (m_channelID == pData->ChannelID && m_strUserCurCameraName.compare(pData->MemberName) ==0)
			{
				if (m_pCamaraObj)
				{
					return;
				}
				else
				{
					memcpy(pBuffer,pData,sizeof(HPMC_DROP));
					SendDropNotifyToU7(pBuffer,false);
					m_channelID = pBuffer->ChannelID;
					m_strUserCurCameraName = pData->MemberName;
				}
			}
			else  //不是同一个摄像头
			{
				if (m_pCamaraObj && g_pWBPage)
				{
					DWORD layerId = m_pCamaraObj->GetLayerIndex();
					g_pWBPage->DeleteWBObject(layerId);
					RemoveObj(layerId);
				}

				memcpy(pBuffer,pData,sizeof(HPMC_DROP));
				SendDropNotifyToU7(pBuffer,false);
				m_channelID = pBuffer->ChannelID;
				m_strUserCurCameraName = pData->MemberName;
			}
		}
		delete pBuffer;
		pBuffer = NULL;
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
}


afx_msg LRESULT CWhiteBoardView::OnPopmenu(WPARAM wParam, LPARAM lParam)
{
	DWORD dwLayerIndex = wParam;
	CreateObjMenu(dwLayerIndex);
	return 0;
}

void CWhiteBoardView::SetIsLocalWhiteboard( bool isLocalWhiteboard )
{
	g_isLoacalWB = isLocalWhiteboard;
}

bool CWhiteBoardView::CombineFile( CWBFile* dst, CWBFile* src )
{
	if ((NULL == dst) || (NULL == src))
	{
		return false;
	}
	dst->DeleteLastContinuousEmptyPages();

	int pageCountSrc = src->GetPageCount();

	for (int i = 1; i<=pageCountSrc; ++i)
	{
		CWBPage* pCur = src->GetPage(i);

		if (NULL != pCur)
		{
			pCur->SetParent(this);

			dst->AddPage(pCur);

		}
	}


	return true;
}

void CWhiteBoardView::AddRuler()
{

	CRect viewRectInScreen;
	GetWindowRect(&viewRectInScreen);
	CPoint pt = CPoint(getXnPos()+400,getYnPos()+300);
	pt.x  +=  viewRectInScreen.left;
	pt.y  +=  viewRectInScreen.top;
	g_pWBPage->ClearSelectFrameList();

	CWBRuler *pWBRuler = new CWBRuler(pt);
	pWBRuler->SetAlpha(125);

	g_pWBPage->AddWBObject(pWBRuler,WB_LAYER_NORMAL);

	if (g_pWBPage->AddSelectFrame(pWBRuler->GetLayerIndex()))
	{
		m_dwCurSelectObjLayerIndex = pWBRuler->GetLayerIndex();
		g_pWBPage->SetObjBottomLayer(m_dwCurSelectObjLayerIndex);
	}
	Invalidate();
	g_nDrawStatus = DRAW_NORMAL;
}


void CWhiteBoardView::CreateAnnRule(CPoint &pt, int number )
{
	CString str,str1;
	str.Format(L"%d",number);
	str1 = _T(" 像素");
	str = str +str1;
	COLORREF clrCurTextColor = RGB(0,0,0);

	LOGFONT  fontItem;
	fontItem.lfItalic = 0;
	fontItem.lfStrikeOut = 0;
	fontItem.lfUnderline = 0;
	fontItem.lfWeight = 200;
	fontItem.lfWidth = 0;
	fontItem.lfHeight = 20;	
	fontItem.lfCharSet = 134;
	fontItem.lfEscapement = 0;
	fontItem.lfOrientation = 0;
	_tcscpy(fontItem.lfFaceName,_T("宋体"));
	CWBAnnotations *pRulerAnnotations = new CWBAnnotations(pt,fontItem,clrCurTextColor,str);
	CWBText *pText = pRulerAnnotations->GetText();
	pText->SetIsVertical(g_bIsVertical);
	pRulerAnnotations->SetAlpha((int)g_nTranspar);

	if (g_bIsDrawScreen)
	{
		m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pRulerAnnotations, WB_LAYER_LABEL);
	} 
	else
	{
		m_dwCurObjLayerIndex = g_pWBPage->AddWBObject(pRulerAnnotations);
	}
	Invalidate();
	//添加同步信息
	XMemIni xmlObj;
	pRulerAnnotations->SetXmlValue(xmlObj);
	AddObj(xmlObj);

}

/*********************************************************************/
void CWhiteBoardView::AddProtractor()
{
	if (m_WBProtractor == NULL)
	{
		CRect viewRectInScreen;
		GetWindowRect(&viewRectInScreen);
		CPoint pt = CPoint(getXnPos()+400,getYnPos()+300);
		pt.x  +=  viewRectInScreen.left;
		pt.y  +=  viewRectInScreen.top;
		CWBProtractor *pProtractor = new CWBProtractor(pt);
		g_pWBPage->AddWBObject(pProtractor,WB_LAYER_NORMAL);
		m_pageIsNeedDraw = TRUE;
		Invalidate();
	}
}
BOOL CWhiteBoardView::AddSetSquare()  //增加一个三角板
{
	static int s_nSetSquareAddX = 0;
	CRect viewRectInScreen;
	GetWindowRect(&viewRectInScreen);
	CPoint pt = CPoint(getXnPos()+70 + s_nSetSquareAddX ,getYnPos()+50);
	s_nSetSquareAddX +=20;//下一个三角板往X轴移10像素
	if (s_nSetSquareAddX>=200)
	{
		s_nSetSquareAddX = 0;
	}
	pt.x  +=  viewRectInScreen.left;
	pt.y  +=  viewRectInScreen.top;
	CWBSetSquare *pWBsetSquare = new CWBSetSquare(pt);//传入创建三角板的起点
	g_pWBPage->AddWBObject(pWBsetSquare,WB_LAYER_NORMAL);//添加对象
	/*	if (g_pWBPage->AddSelectFrame(pWBsetSquare->GetLayerIndex()))
	{
	m_dwCurSelectObjLayerIndex = pWBsetSquare->GetLayerIndex();
	g_pWBPage->SetObjBottomLayer(m_dwCurSelectObjLayerIndex);
	}
	*/
	m_pageIsNeedDraw = TRUE;

	Invalidate();
	g_nDrawStatus = DRAW_NORMAL;
	return TRUE;
}

BOOL CWhiteBoardView::AddCompass()  //增加一个圆规
{
	static int s_nCompassAddX = 0;
	CRect viewRectInScreen;
	GetWindowRect(&viewRectInScreen);
	CPoint pt = CPoint(getXnPos() + 70 + s_nCompassAddX, getYnPos() + 100);
	s_nCompassAddX += 20;//下一个三角板往X轴移动10像素
	if (s_nCompassAddX >= 200)
	{
		s_nCompassAddX = 0;
	}
	pt.x  += viewRectInScreen.left;
	pt.y  += viewRectInScreen.top;
	CWBCompass *pWBCompass = new CWBCompass(pt);//传入创建圆规的起点
	g_pWBPage->AddWBObject(pWBCompass,WB_LAYER_NORMAL);

	m_pageIsNeedDraw = TRUE;
	Invalidate();
	g_nDrawStatus = DRAW_NORMAL;
	return TRUE;
}

void CWhiteBoardView::AddAngle()
{
	if (m_WBAngle == NULL)
	{
		CPoint ptCT;
		CPoint ptRB;
		CPoint ptLT;
		double radian;
		CWBProtractor *pWBProtractor = (CWBProtractor*)g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
		ptCT = pWBProtractor->GetCTpoint();
		ptRB = pWBProtractor->GetRBpoint();
		ptLT = pWBProtractor->GetLTpoint();
		radian = pWBProtractor->GetRadian();
		CWBAngle *pAngle = new CWBAngle(ptLT,ptRB,ptCT,radian);
		g_pWBPage->AddWBObject(pAngle,WB_LAYER_NORMAL);
		m_pageIsNeedDraw = TRUE;
		if (g_pWBPage->AddSelectFrame(pAngle->GetLayerIndex()))
		{
			m_dwCurSelectObjLayerIndex = pAngle->GetLayerIndex();
		}
		Invalidate();
	}
	g_nDrawStatus = DRAW_NORMAL;
}

void CWhiteBoardView::PrintBmp(int count)
{
	CDC *pdc = GetDC();
	int FromPage,ToPage;
	CPrintDialog m_PrintDlg(FALSE,PD_ALLPAGES| PD_USEDEVMODECOPIES  ,NULL);
	FromPage = m_PrintDlg.m_pd.nMinPage = m_PrintDlg.m_pd.nFromPage = 1;
	ToPage = m_PrintDlg.m_pd.nMaxPage = m_PrintDlg.m_pd.nToPage = count;	
	PRINTDLG   pd; 
	LPDEVMODE   lpmode; 
	if(AfxGetApp()-> GetPrinterDeviceDefaults(&pd))
	{ 
		lpmode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpmode) 
		{ 
			lpmode-> dmOrientation=DMORIENT_PORTRAIT; 
			m_PrintDlg.m_pd.hDevMode = lpmode;
		} 
		GlobalUnlock(pd.hDevMode); 
	}

	if(m_PrintDlg.DoModal() == IDOK)
	{
		CDC pDC;
		if (!pDC.Attach(m_PrintDlg.GetPrinterDC()))
		{
			AfxMessageBox(_T("No printer found!")); 
			return; 

		}
		pDC.m_bPrinting = TRUE; 
		if(m_PrintDlg.m_pd.nFromPage>m_PrintDlg.m_pd.nMinPage)
			FromPage = m_PrintDlg.m_pd.nFromPage;
		if(m_PrintDlg.m_pd.nToPage < m_PrintDlg.m_pd.nMaxPage)
			ToPage =  m_PrintDlg.m_pd.nToPage;
		CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwMenuLayerIndex);
		if (pObj->GetType() != WB_OBJ_DOCUMENT)
		{
			/*((CWBDocument*)pObj)->PrintPage(FromPage,ToPage);*/
			return;
		}
		for(int PageContr = FromPage;PageContr <= ToPage; ++PageContr)
		{  
			((CWBDocument*)pObj)->GotoPage(PageContr);
			HBITMAP b= ((CWBDocument*)pObj)->GetCurPageBmp();

			CDC dc;
			dc.CreateCompatibleDC(pdc);
			//从外部文件导入

			CBitmap bitmap;
			bitmap.Attach(b);
			BITMAP bm;
			bitmap.GetBitmap(&bm);
			int width = bm.bmWidth;
			int height = bm.bmHeight;
			dc.SelectObject(&bitmap);
			//	pWBPrintPage = g_pWBFile->GetPage(PageContr);
			int m_PrintX,m_PrintY;
			//通过将图片放入内存DC来达到图片与设备不兼容问题 12-25添加jin
			CRect rect(0,0,width,height);
			CDC dcMem;         //以下是输出位图的标括准操作
			CBitmap *pOldBitmap = NULL;
			CBitmap BmpBuf;
			dcMem.CreateCompatibleDC(NULL);
			BmpBuf.CreateCompatibleBitmap(&pDC,rect.Width(),rect.Height());
			pOldBitmap = dcMem.SelectObject(&BmpBuf);
			BITMAP bmpinfo;
			BmpBuf.GetBitmap(&bmpinfo);
			dcMem.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));

			if( !1)
			{
				//ratex = ratey =1;
			}
			else
			{
				SetMapMode(pDC,MM_ISOTROPIC);	
				m_PrintX = pDC.GetDeviceCaps(LOGPIXELSX);
				m_PrintY = pDC.GetDeviceCaps(LOGPIXELSY);

				// 设置

				//CRect viewRect;
				// GetSystemMetrics(SM_CXSCREEN)是获取屏幕水平方向像素点个数 
				//showRage是设置view像素点个数和当前屏幕像素点个数比率

				//把设备坐标的原点移动到逻辑坐标的(X, Y)处
				pDC.SetWindowOrg((int)(-16),(int)(-20));
				//将1个逻辑单位映射成1/29英寸	
				pDC.SetWindowExt(29,26);		
				pDC.SetViewportExt(m_PrintX,m_PrintY);


				if (pDC.StartDoc(_T("printimage")) == SP_ERROR)// 启动打印工作 
				{  
					DeleteDC(pDC);
					return ;				//ExitProcess(0);
				}
			}	
			if (b)
			{  
				//pWBPrintPage->Draw(&dcMem);
				::BitBlt(dcMem.m_hDC,0, 0, width, height,dc, 0, 0, SRCCOPY);
				::StretchBlt(pDC.m_hDC,0,0,210 ,279 ,dcMem.m_hDC,0,0,width,height,SRCCOPY);
				dcMem.SelectObject(pOldBitmap);	
				BmpBuf.DeleteObject();
				dcMem.DeleteDC(); 
			}
			//
			if(pDC.IsPrinting())
			{               
				CPrintInfo* pInfo=NULL;
				CRect rectDraw2;
				pdc->SetMapMode(MM_ANISOTROPIC); //转化坐标映射模式
				int 
					nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //得到屏幕分辨率的宽和高(我的是1280*1024)
				int 
					nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
				CSize 
					size = CSize(nScreenWidth, nScreenHeight);
				pdc->SetWindowExt(size); //设置视窗大小
				int xLogPixPerInch = pdc->GetDeviceCaps(LOGPIXELSX);  
				//得到设备每逻辑英寸的像素数量
				int yLogPixPerInch = pdc->GetDeviceCaps(LOGPIXELSY);
				float fWidth = (float)xLogPixPerInch / 96 ; 
				//得到电脑屏幕映射到视窗大小比率
				float fHeight = (float)yLogPixPerInch / 96; //一般得到的fWidth = fHeight 


				long xExt = (long)(fWidth * size.cx); //得到视窗大小
				long yExt = (long)(fHeight * size.cy) ;
				pdc->SetViewportExt((int)xExt, (int)yExt); //设置视窗大小(由电脑屏幕映射到视窗大小)

				//PrintDraw(pDC, pInfo, fWidth, fHeight, rectDraw1,print_x,print_y); 
				CBitmap *pOldBmp = NULL;
				CRect rectDraw; //得到view中要绘图的区域(可能派生自CScrollView,所以不一定小于
				GetClientRect(&rectDraw); //屏幕大小)

				//得到要进行缩放的比例
				float 
					fw = 0.0;
				BOOL 
					bw = FALSE;
				float 
					fh = 0.0;
				BOOL 
					bh = FALSE;
				float 
					fr = 1.0;
				int PrintX = pdc->GetDeviceCaps(LOGPIXELSX);
				int PrintY = pdc->GetDeviceCaps(LOGPIXELSY);	
				if ((long)(fWidth * rectDraw.Width()) > m_PrintX) //如果长度由屏幕映射到打印机后大于打印机的可绘图区域
				{ 
					//(pInfo->m_rectDraw), 则需要进行缩放操作,缩放后不是图形变形
					fw = fWidth * rectDraw.Width() / m_PrintX;
					bw = TRUE;
				}
				if ((long)(fHeight * rectDraw.Height()) >m_PrintY) //同上
				{
					fh = fHeight * rectDraw.Height() / m_PrintY;
					bh = TRUE;
				}
				if (bw || bh) //缩放比率应使纸张能容纳下图片大小,且尽可能的按原图大小显示
					fr = fw > fh ? fw : fh;


				//Recalculate rect of draw
				//RecalculateRect(rectDraw, fr, rectDraw1); 
				rectDraw2 = rectDraw;
				if (fr == 0.0)
					return ;


				rectDraw2.left = (long)((float)rectDraw.left / fr);
				rectDraw2.top = (long)((float)rectDraw.top / fr);
				rectDraw2.right = (long)((float)rectDraw.right / fr);
				rectDraw2.bottom = (long)((float)rectDraw.bottom / fr);
				// pDC.StretchBlt(rectDraw2.left, rectDraw2.top, rectDraw2.Width(), rectDraw2.Height(),&dc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);

			}
			if (pDC == NULL)
			{
				return;
			}
			pDC.EndDoc();
			dcMem.SelectObject(pOldBitmap);
			dc.DeleteDC();
			dcMem.DeleteDC();
			BmpBuf.DeleteObject();
		}
		
		::ReleaseDC(this->m_hWnd, pDC.m_hDC);
		if (pObj != NULL)
		{
			((CWBDocument*)pObj)->GotoPage(FromPage);
		}

	}
	else
	{
		return;
	}
	ReleaseDC(pdc);
}

void CWhiteBoardView::PrintBmp( HBITMAP hbitmap )
{

	CDC *pdc = GetDC();
	UINT FromPage,ToPage;
	CPrintDialog m_PrintDlg(FALSE,PD_ALLPAGES| PD_USEDEVMODECOPIES  ,NULL);
	FromPage = m_PrintDlg.m_pd.nMinPage = m_PrintDlg.m_pd.nFromPage = 1;
	ToPage = m_PrintDlg.m_pd.nMaxPage = m_PrintDlg.m_pd.nToPage = 1;	
	PRINTDLG   pd; 
	LPDEVMODE   lpmode; 
	if(AfxGetApp()-> GetPrinterDeviceDefaults(&pd))
	{ 
		lpmode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpmode) 
		{ 
			lpmode-> dmOrientation=DMORIENT_PORTRAIT;
			//lpmode-> dmOrientation=DMORIENT_LANDSCAPE; 
			m_PrintDlg.m_pd.hDevMode = lpmode;
		} 
		GlobalUnlock(pd.hDevMode); 
	}

	if(m_PrintDlg.DoModal() == IDOK)
	{
		CDC pDC;
		if (!pDC.Attach(m_PrintDlg.GetPrinterDC()))
		{
			AfxMessageBox(_T("No printer found!")); 
			return; 

		}
		pDC.m_bPrinting = TRUE; 
		if(m_PrintDlg.m_pd.nFromPage>m_PrintDlg.m_pd.nMinPage)
			FromPage = m_PrintDlg.m_pd.nFromPage;
		if(m_PrintDlg.m_pd.nToPage < m_PrintDlg.m_pd.nMaxPage)
			ToPage =  m_PrintDlg.m_pd.nToPage;
		for(UINT PageContr = FromPage;PageContr <= ToPage; ++PageContr)
		{ 
			CDC dc;
			dc.CreateCompatibleDC(pdc);
			//从外部文件导入
			CBitmap bitmap;
			bitmap.Attach(hbitmap);
			BITMAP bm;
			bitmap.GetBitmap(&bm);
			int width = bm.bmWidth;
			int height = bm.bmHeight;
			dc.SelectObject(&bitmap);
			//	pWBPrintPage = g_pWBFile->GetPage(PageContr);
			//int m_PrintX,m_PrintY;
			//通过将图片放入内存DC来达到图片与设备不兼容问题 12-25添加jin
			CRect rect(0,0,width,height);
			CDC dcMem;         //以下是输出位图的标括准操作
			CBitmap *pOldBitmap = NULL;
			CBitmap BmpBuf;
			dcMem.CreateCompatibleDC(NULL);
			BmpBuf.CreateCompatibleBitmap(&pDC,rect.Width(),rect.Height());
			pOldBitmap = dcMem.SelectObject(&BmpBuf);
			BITMAP bmpinfo;
			BmpBuf.GetBitmap(&bmpinfo);
			dcMem.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));

			if( !1)
			{
			}
			else
			{
				SetMapMode(pDC,MM_ISOTROPIC);	
				m_PrintX = pDC.GetDeviceCaps(LOGPIXELSX);
				m_PrintY = pDC.GetDeviceCaps(LOGPIXELSY);

				// 设置

				//CRect viewRect;
				// GetSystemMetrics(SM_CXSCREEN)是获取屏幕水平方向像素点个数 
				//showRage是设置view像素点个数和当前屏幕像素点个数比率

				//把设备坐标的原点移动到逻辑坐标的(X, Y)处
				pDC.SetWindowOrg((int)(-16),(int)(-20));
				//将1个逻辑单位映射成1/29英寸	
				pDC.SetWindowExt(29,26);	
				pDC.SetViewportExt(m_PrintX,m_PrintY);


				if (pDC.StartDoc(_T("printimage")) == SP_ERROR)// 启动打印工作 
				{  
					DeleteDC(pDC);
					return ;				//ExitProcess(0);
				}
			}	
			if (hbitmap)
			{  
				::BitBlt(dcMem.m_hDC,0, 0, width, height,dc, 0, 0, SRCCOPY);
				::StretchBlt(pDC.m_hDC,0,0,210 ,279 ,dcMem.m_hDC,0,0,width,height,SRCCOPY);
				dcMem.SelectObject(pOldBitmap);	
				BmpBuf.DeleteObject();
				dcMem.DeleteDC(); 
			}

			if(pDC.IsPrinting())
			{               
				CPrintInfo* pInfo=NULL;
				CRect rectDraw2;
				pdc->SetMapMode(MM_ANISOTROPIC); //转化坐标映射模式
				int 
					nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //得到屏幕分辨率的宽和高(我的是1280*1024)
				int 
					nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
				CSize 
					size = CSize(nScreenWidth, nScreenHeight);
				pdc->SetWindowExt(size); //设置视窗大小
				int xLogPixPerInch = pdc->GetDeviceCaps(LOGPIXELSX);  
				//得到设备每逻辑英寸的像素数量
				int yLogPixPerInch = pdc->GetDeviceCaps(LOGPIXELSY);
				float fWidth = (float)xLogPixPerInch / 96 ; 
				//得到电脑屏幕映射到视窗大小比率
				float fHeight = (float)yLogPixPerInch / 96; //一般得到的fWidth = fHeight 


				long xExt = (long)(fWidth * size.cx); //得到视窗大小
				long yExt = (long)(fHeight * size.cy) ;
				pdc->SetViewportExt((int)xExt, (int)yExt); //设置视窗大小(由电脑屏幕映射到视窗大小)

				//PrintDraw(pDC, pInfo, fWidth, fHeight, rectDraw1,print_x,print_y); 
				CBitmap  *pOldBmp = NULL;
				CRect rectDraw; //得到view中要绘图的区域(可能派生自CScrollView,所以不一定小于
				GetClientRect(&rectDraw); //屏幕大小)

				//得到要进行缩放的比例
				float 
					fw = 0.0;
				BOOL 
					bw = FALSE;
				float 
					fh = 0.0;
				BOOL 
					bh = FALSE;
				float 
					fr = 1.0;
				int PrintX = pdc->GetDeviceCaps(LOGPIXELSX);
				int PrintY = pdc->GetDeviceCaps(LOGPIXELSY);	
				if ((long)(fWidth * rectDraw.Width()) > m_PrintX) //如果长度由屏幕映射到打印机后大于打印机的可绘图区域
				{ 
					//(pInfo->m_rectDraw), 则需要进行缩放操作,缩放后不是图形变形
					fw = fWidth * rectDraw.Width() / m_PrintX;
					bw = TRUE;
				}
				if ((long)(fHeight * rectDraw.Height()) >m_PrintY) //同上
				{
					fh = fHeight * rectDraw.Height() / m_PrintY;
					bh = TRUE;
				}
				if (bw || bh) //缩放比率应使纸张能容纳下图片大小,且尽可能的按原图大小显示
					fr = fw > fh ? fw : fh;


				//Recalculate rect of draw
				//RecalculateRect(rectDraw, fr, rectDraw1); 
				rectDraw2 = rectDraw;
				if (fr == 0.0)
					return ;


				rectDraw2.left = (long)((float)rectDraw.left / fr);
				rectDraw2.top = (long)((float)rectDraw.top / fr);
				rectDraw2.right = (long)((float)rectDraw.right / fr);
				rectDraw2.bottom = (long)((float)rectDraw.bottom / fr);
				//   pDC.StretchBlt(rectDraw2.left,rectDraw2.top,rectDraw2.Width(),rectDraw2.Height(),&dc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);

			}
			if (pDC == NULL)
			{
				return;
			}
			pDC.EndDoc();

		}
		::ReleaseDC(this->m_hWnd, pDC.m_hDC);

	}
	else
	{

		return;
	}
	ReleaseDC(pdc);
}

LRESULT CWhiteBoardView::OnProcessRectMagnClose( WPARAM wParam, LPARAM lParam )
{
	if (m_pWBRectMaqn != NULL)
	{
		//设置发送事件ID
		m_ulSendEventID = GenerateSSRC();
		RemoveObj(m_pWBRectMaqn->GetLayerIndex());

		delete m_pWBRectMaqn;
		m_pWBRectMaqn = NULL;
	}	
	return 1;
}

void CWhiteBoardView::SetDustbinTouchID()
{
	m_ntouchID = 0;
}



void CWhiteBoardView::MoveObjToDustbin( CPoint point,int id /*=0*/)
{
	if (NULL != m_pWBDustbin) //假如创建了垃圾箱
	{
		CWBObject *pObj = NULL;
		int mousepos = m_pWBDustbin->PointIn(point);
		CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
		int iSelectObjCounts = pSelectObjLst->GetSize();
		POSITION pos;
		if (m_ntouchID != 0 && m_ntouchID != id)
		{
			return;
		}
		if(mousepos != m_PreMousePos)//假如鼠标移入或者移出垃圾桶
		{
			
			if (MOUSE_OUTSIDE != mousepos)//鼠标移入垃圾桶
			{
				m_ntouchID = id;
				CPoint upDustbinOffset(0, -5);
				m_pWBDustbin->MoveOffset(upDustbinOffset);
				m_pWBDustbin->MoveRect(upDustbinOffset);
				Sleep(3);
				Invalidate();
				UpdateWindow();
				CPoint downDustbinOffset(0, 5);
				m_pWBDustbin->MoveOffset(downDustbinOffset);
				m_pWBDustbin->MoveRect(downDustbinOffset);

				int alpha = 255/2;
				for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
				{

					pos = pSelectObjLst->FindIndex(i);

					pObj = pSelectObjLst->GetAt(pos);
					//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
					if (pObj->IsLock() || 	pObj->IsLimit()) //huanglongquan add
					{
						continue;
					}
					else
					{
						if (pObj->GetType() == WB_OBJ_CURVE)
						{
							((CWBCurve*)pObj)->SetIsDraw();
						}
						if (pObj->GetType() == WB_OBJ_ICOCURVE)
						{
							((CWBIcoCurve*)pObj)->SetIsRefush(TRUE);
						}
						if (pObj->GetType() == WB_OBJ_PICTURE)
						{
							((CWBPicture *)pObj)->SetIsPath();
						}

						if(pObj->GetType() == WB_OBJ_DXF) //判断是否重画DXF
						{
							((CWBDxf *)pObj)->SetIsNeedReDraw(); 
						}

						if (pObj->GetType() != WB_OBJ_COMBINE)
						{
							if (pObj->GetType() == WB_OBJ_DOCUMENT)
							{
								CWBObject *pDocCom = ((CWBDocument *)pObj)->GetSelectComObj();
								if (pDocCom != NULL)
								{
									pDocCom->SetAlpha(alpha);
									if (pDocCom->GetType() == WB_OBJ_CURVE)
									{
										((CWBCurve*)pDocCom)->SetIsDraw();
									}
								} 
								else
								{
									pObj->SetAlpha(alpha);
								}
							} 
							else
							{
								pObj->SetAlpha(alpha);
							}
						}
					}
				}
			}
			else//鼠标移出垃圾桶
			{
				m_ntouchID = 0;
				CPoint upDustbinOffset(0, -5);
				m_pWBDustbin->MoveOffset(upDustbinOffset);
				m_pWBDustbin->MoveRect(upDustbinOffset);
				Sleep(3);
				Invalidate();
				UpdateWindow();
				CPoint downDustbinOffset(0, 5);
				m_pWBDustbin->MoveOffset(downDustbinOffset);
				m_pWBDustbin->MoveRect(downDustbinOffset);

				for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
				{
					pos = pSelectObjLst->FindIndex(i);

					pObj = pSelectObjLst->GetAt(pos);
					//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
					if (pObj->IsLock() || 	pObj->IsLimit()) //huanglongquan add
					{
						continue;
					}
					else
					{
						if (pObj->GetType() == WB_OBJ_CURVE)
						{
							((CWBCurve*)pObj)->SetIsDraw();
						}

						if (pObj->GetType() == WB_OBJ_PICTURE)
						{
							((CWBPicture *)pObj)->SetIsPath();
						}

						if(pObj->GetType() == WB_OBJ_DXF) //判断是否重画DXF
						{
							((CWBDxf *)pObj)->SetIsNeedReDraw(); 
						}

						if (pObj->GetType() != WB_OBJ_COMBINE)
						{
							if (pObj->GetType() == WB_OBJ_DOCUMENT)
							{
								CWBObject *pDocCom = ((CWBDocument *)pObj)->GetSelectComObj();
								if (pDocCom != NULL)
								{
									pDocCom->SetAlpha(pDocCom->GetPreAlpha());
									if (pDocCom->GetType() == WB_OBJ_CURVE)
									{
										((CWBCurve*)pDocCom)->SetIsDraw();
									}
								} 
								else
								{
									pObj->SetAlpha(pObj->GetPreAlpha());
								}
							} 
							else
							{
								pObj->SetAlpha(pObj->GetPreAlpha());
							}
						}
					}
				}
			}
		}

		m_PreMousePos = mousepos;// add by xiewenxiu 20111201
	}

}

void CWhiteBoardView::SetObjectoutDustbin()
{
	if (g_pWBPage == NULL)
	{
		return;
	}
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
	int iSelectObjCounts = pSelectObjLst->GetSize();
	if (iSelectObjCounts == 0)
	{
		return;
	}
	POSITION pos;
	CWBObject *pObj = NULL;
	for (int i = 0;i < iSelectObjCounts;++i )//遍历选中对象列表
	{
		pos = pSelectObjLst->FindIndex(i);

		pObj = pSelectObjLst->GetAt(pos);
		if (pObj == NULL)
		{
			continue;
		}
		//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
		if (pObj->IsLock() || 	pObj->IsLimit()) //huanglongquan add
		{
			continue;
		}
		
		else
		{
			if (pObj->GetType() == WB_OBJ_CURVE)
			{
				((CWBCurve*)pObj)->SetIsDraw();
			}
			if (pObj->GetType() == WB_OBJ_ICOCURVE)
			{
				((CWBIcoCurve*)pObj)->SetIsRefush(TRUE);
			}
			if (pObj->GetType() == WB_OBJ_PICTURE)
			{
				((CWBPicture *)pObj)->SetIsPath();
			}

			if(pObj->GetType() == WB_OBJ_DXF) //判断是否重画DXF
			{
				((CWBDxf *)pObj)->SetIsNeedReDraw(); 
			}

			if (pObj->GetType() != WB_OBJ_COMBINE)
			{
				if (pObj->GetType() == WB_OBJ_DOCUMENT)
				{
					CWBObject *pDocCom = ((CWBDocument *)pObj)->GetSelectComObj();
					if (pDocCom != NULL)
					{
						pDocCom->SetAlpha(pDocCom->GetPreAlpha());
						if (pDocCom->GetType() == WB_OBJ_CURVE)
						{
							((CWBCurve*)pDocCom)->SetIsDraw();
						}
					} 
					else
					{
						pObj->SetAlpha(pObj->GetPreAlpha());
					}
				} 
				else
				{
					pObj->SetAlpha(pObj->GetPreAlpha());
				}
			}
		}
	}
}

void CWhiteBoardView::SetBroadcastUserCursor( bool IsBroad )
{

	if(m_pWBFunctionDlg&& m_pWBFunctionDlg->GetSafeHwnd()&&m_pWBFunctionDlg->IsWindowVisible())
	{
		m_pWBFunctionDlg->ShowWindow(SW_HIDE);
	}

	if (!IsBroad)
	{
		m_bIsneedjionBroadcast = FALSE;
		if (m_pTransparent_dlg)
		{
			m_pTransparent_dlg->ShowWindow(SW_HIDE);
		}

		IT_MapCursorDlgPointer it = m_MapCursorDlgPointer.begin();
		while (it != m_MapCursorDlgPointer.end())
		{
			CTransparent_Dlg* pDlg =  (CTransparent_Dlg*)it->second;
			if (pDlg)
			{
				pDlg->ShowWindow(SW_HIDE);
			}
			++it;
		}
		if (m_MedioVideoScreenDlg)
		{
			m_MedioVideoScreenDlg->ShowWindow(SW_HIDE);
		}
		if (m_waterdlg)
		{
			m_waterdlg->ShowWindow(SW_HIDE);
		}
		IT_MapWaterDlgPointer itw = m_MapWaterDlgPointer.begin();
		while (itw != m_MapWaterDlgPointer.end())
		{
			CWaterDlg *pwater = (CWaterDlg*)itw->second;
			if (pwater)
			{
				pwater->ShowWindow(SW_HIDE);
			}
			++itw;
		}
	}
	else
	{
		if (m_bIsneedjionBroadcast)
		{
			return;
		}
		m_bIsneedjionBroadcast = TRUE;
		if (m_pTransparent_dlg)
		{
			m_pTransparent_dlg->ShowWindow(SW_SHOW);
		}
		if (m_MedioVideoScreenDlg)
		{
			m_MedioVideoScreenDlg->ShowWindow(SW_SHOW);
		}
		
	}
}



void CWhiteBoardView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return;
	__super::OnMButtonDown(nFlags, point);
}

void CWhiteBoardView::SetRichEditRect( CRect &rc )
{
	m_RichEditRect = rc;
}

CRect CWhiteBoardView::GetRichEditRect()
{
	return m_RichEditRect;
}


LRESULT CWhiteBoardView::OnDropNotify( WPARAM wParam, LPARAM lParam )
{
	HPMC_DROP*pData = (HPMC_DROP*)wParam;
	if (!pData)
	{
		return 0;
	}
	int nflag = 0;
	nflag = pData->Type;

	HPMC_DROP *pBuffer = new HPMC_DROP;
	memcpy(pBuffer,pData,sizeof(HPMC_DROP));
	switch(nflag)
	{
	case 2:
		{
			if (m_bIsAllowDropCamera)
			{
				if (g_isLoacalWB && nflag != 99)//屏蔽本地会议拖拽
				{
					if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
					{
						//g_pHPWBDataNotify->MessageBox(NULL, m_strPrompt, m_strLocalNotDropCameraToWB, MB_OK, true);
						g_pHPWBDataNotify->ShowTipWindow(m_strLocalNotDropCameraToWB);
					}
					else
					{
						AfxMessageBox(m_strLocalNotDropCameraToWB,MB_OK);
					}
					return 0;
				}

				CPoint point;
				::GetCursorPos(&point);

				ScreenToClient(&point);
				CClientDC dc(this);
				OnPrepareDC(&dc);
				dc.DPtoLP(&point);
				CRect rect;

				rect.left = point.x - 50;
				rect.top = point.y -29;
				rect.right = rect.left + 100;
				rect.bottom = rect.top +58;
				ProcessCaminfo(wParam,rect);
				//SendDropNotifyToU7(pBuffer,false);
				
			}
			else
			{
				SendDropNotifyU7Message(pBuffer,false);
				return NULL;
			}

		}
		break;
	case 99:
		{
			/*char chClassify = pBuffer->MemberID[0];
			int  nResult = -1;
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify && '-' == chClassify)
			{
			nResult = g_pHPWBDataNotify->MessageBox(this->m_hWnd,_T("提示"),_T("是否拖入") , MB_OKCANCEL,false);
			}
			if (nResult == IDOK)
			{
			pBuffer->Type = 2;
			SendDropNotifyToU7(pBuffer,false);
			}
			else
			{*/
			SendDropNotifyU7Message(pBuffer,false);
			/*}*/

		}
		break;
	default:
		break;
	}

	delete pBuffer;
	pBuffer = NULL;
	return NULL;
}


void CWhiteBoardView::SetTouchEventId( unsigned long ulEventId)
{
	m_ulTouchEventId = ulEventId;
}

BOOL CWhiteBoardView::MTInfiniteClone()
{
	CWBObject *pObj = g_pWBPage->FindByLayerIndex(m_dwCurSelectObjLayerIndex);
	if (pObj)
	{
		if (pObj->IsClone())
		{
			if (!pObj->IsLock() && !pObj->IsLimit())
			{
				g_pWBPage->ClearSelectFrameList();
				g_pWBPage->AddSelectFrame(pObj->GetLayerIndex());
				CloneObject();
				pObj->SetClone(FALSE);
			}
			Invalidate();
			UpdateWindow();
			return TRUE;
		}
	}
	return FALSE;
}

void CWhiteBoardView::SendDropNotifyToU7( HPMC_DROP *pBuffer,bool bflag /*= true*/ )
{
	AVDataOutPacket outPacket;		   				
	unsigned long	ChannelID;	
	std::string  StrPathTemp1 = "";

	StrPathTemp1 = pBuffer->MemberID;
	ChannelID = pBuffer->ChannelID;
	int iCmd = CMD_DROP_NOTIFY_TO_WB;

	int xxlen = StrPathTemp1.length();

	int nLen = sizeof(int) + sizeof(unsigned long)  + xxlen;
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		memset(pData,0,nLen+1);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		memcpy(pData+sizeof(int), (char *)&ChannelID, sizeof(unsigned long));
		memcpy(pData+sizeof(int) +sizeof(unsigned long),StrPathTemp1.c_str(),xxlen);

		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
	}
	SendPacketInfoOnlyToNet((char*)outPacket.GetData(), outPacket.GetLength(), bflag);
	if (pData)
	{
		delete pData;
		pData = NULL;
	}
}

void CWhiteBoardView::ProcessCaminfo( WPARAM wParam,CRect rc )
{
	HPMC_DROP *pData = (HPMC_DROP*)wParam;
	HPMC_DROP *hp = new HPMC_DROP;
	::ZeroMemory(hp,sizeof(HPMC_DROP));
	memcpy(hp,pData,sizeof(HPMC_DROP));
	if (pData->Type == CFSRC_AV)
	{
		
		unsigned long channelID = pData->ChannelID;
		CString str =GetCurrentPath() + L"\\SKIN\\WhiteBoard\\camera3.png"; 
		CRect rect(0,0,0,0);
		rect.SetRect(rc.TopLeft(),rc.BottomRight());
		CString str11 = _T("");
		char *strMen = NULL;
 		if (pData->MemberName != NULL)
 		{
 			strMen = pData->MemberName;
 			str11 = A2U(pData->MemberName);
 		}
 		else
		{
			strMen = pData->MemberID;
			str11 = ToDRAWStr(strMen).c_str();
		}

		IT_MapDropCamInfo it = m_MapDropCamInfo.begin();
		if (m_MapDropCamInfo.empty())
		{

			CWBPicture *pWBPicture = new CWBPicture(str,rect,str11);
			if (pWBPicture == NULL)
			{
				return;
			}
// 			if (!str11.IsEmpty())
// 			{
// 				pWBPicture->SetCurchannelID(channelID);
// 			}
			g_pWBPage->ClearSelectFrameList();
			pWBPicture->SetAlpha((int)g_nTranspar);
			//控制图片的最大不能超出view的大小
			CRect viewRect,viewRectInScreen;
			GetClientRect(&viewRect);
			GetWindowRect(&viewRectInScreen);//得到白板在屏幕上的位置
			if (!(rect.left || rect.bottom || rect.right || rect.top)) 
			{
				pWBPicture->SetPictureBigerSzie(viewRect);
			}	
			DWORD nWbObjID = g_pWBPage->AddWBObject(pWBPicture,WB_LAYER_NORMAL);
			if (pData->MemberName != NULL)
			{
				pWBPicture->SetstrCaramaPicture(pData->MemberName);
				m_pCaremaPicture.push_back(pData->MemberName);
			}
			else
			{
				pWBPicture->SetstrCaramaPicture(pData->MemberID);
				m_pCaremaPicture.push_back(pData->MemberID);
			}
			
			CPoint ptCenter;

			pWBPicture->GetCenterPoint(ptCenter);
			m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
			//创建选择框，并移动对象到当前位置
			if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
			{ 
				m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
				pWBPicture->SetSelectFrameShow(FALSE);
			}

			m_bIsdrop = TRUE;

			pWBPicture->SetIsPath();//图片是否重先加载wangxiaobin
			Invalidate();
			g_nDrawStatus = DRAW_NORMAL;
			m_MapDropCamInfo.insert(MapDropCamInfo::value_type(nWbObjID,(HPMC_DROP*)hp));
			std::string strName = "";
			std::string strMemberid = "";
			
			strName = hp->MemberID;
			strMemberid = hp->MemberID;
						
			m_pUserNameID->insert(pair<std::string,DWORD>(strName,nWbObjID));

			XMemIni xmlObj;
			pWBPicture->SetXmlValue(xmlObj);
			xmlObj.Set(KEY_PICTURE_SENDSTING,100);
			xmlObj.Set(KEY_PICTURE_CHANGNEL_ID,channelID);
			xmlObj.Set(KEY_CAMERA_ID_OF_UCC_LOGIN_NAME,strMemberid);
			AddObj(xmlObj);
		}
		else
		{
			BOOL bIsTrue = FALSE;
			while(it != m_MapDropCamInfo.end())
			{
				HPMC_DROP* HD = it->second;
				//CString str1 = A2U(strMen);
				char* str = NULL;
				str = pData->MemberID;
				CString str1 = ToDRAWStr(str).c_str();
				CString str2 = ToDRAWStr(HD->MemberID).c_str();
				CString strChannleID ;
				unsigned long nchannelID = pData->ChannelID;
				unsigned long nGetMapChannelID = HD->ChannelID;
				if ((str1 == str2) && (nchannelID == nGetMapChannelID))
				{
					bIsTrue = TRUE;
				}
				++it;
			}
			if (!bIsTrue)
			{

				CWBPicture *pWBPicture = new CWBPicture(str,rect,str11);
				if (pWBPicture == NULL)
				{
					return;
				}
				g_pWBPage->ClearSelectFrameList();
				pWBPicture->SetAlpha((int)g_nTranspar);
				//控制图片的最大不能超出view的大小
				CRect viewRect,viewRectInScreen;
				GetClientRect(&viewRect);
				GetWindowRect(&viewRectInScreen);//得到白板在屏幕上的位置
				if (!(rect.left || rect.bottom || rect.right || rect.top)) 
				{
					pWBPicture->SetPictureBigerSzie(viewRect);
				}	
				DWORD nWbObjID = g_pWBPage->AddWBObject(pWBPicture,WB_LAYER_NORMAL);
				if (pData->MemberName != NULL)//同时拖同一个的时候用到的
				{
					pWBPicture->SetstrCaramaPicture(pData->MemberName);
					m_pCaremaPicture.push_back(pData->MemberName);
				}
				else
				{
					pWBPicture->SetstrCaramaPicture(pData->MemberID);
					m_pCaremaPicture.push_back(pData->MemberID);
				}
				
				CPoint ptCenter;

				pWBPicture->GetCenterPoint(ptCenter);
				m_moveFirstPoint = ptCenter;	//将移动起始点设为中心点
				//创建选择框，并移动对象到当前位置
				if (g_pWBPage->AddSelectFrame(pWBPicture->GetLayerIndex()))
				{
					m_dwCurSelectObjLayerIndex = pWBPicture->GetLayerIndex();
					pWBPicture->SetSelectFrameShow(FALSE);
				}

				m_bIsdrop = TRUE;
				std::string strMemberid = "";
				strMemberid = hp->MemberID;
				pWBPicture->SetIsPath();//图片是否重先加载wangxiaobin
				Invalidate();
				g_nDrawStatus = DRAW_NORMAL;
				m_MapDropCamInfo.insert(MapDropCamInfo::value_type(nWbObjID,(HPMC_DROP*)hp));
				std::string strName = "";
				strName = hp->MemberID;
				m_pUserNameID->insert(pair<std::string,DWORD>(strName,nWbObjID));
				XMemIni xmlObj;
				pWBPicture->SetXmlValue(xmlObj);
				xmlObj.Set(KEY_PICTURE_SENDSTING,100);
				xmlObj.Set(KEY_PICTURE_CHANGNEL_ID,channelID);
				xmlObj.Set(KEY_CAMERA_ID_OF_UCC_LOGIN_NAME,strMemberid);
				AddObj(xmlObj);
			}
		}


	}


}

void CWhiteBoardView::ReJectAllProcessCamInfo()
{
	IT_MapDropCamInfo it = m_MapDropCamInfo.begin();
	while(it != m_MapDropCamInfo.end())
	{
		HPMC_DROP *HD = (HPMC_DROP*)it->second;
		delete HD;
		HD = NULL;
		it->second = NULL;

		++it;
	}
	m_MapDropCamInfo.clear();
}

void CWhiteBoardView::DeleteCaminfo( DWORD nWBObjID )
{
	IT_MapDropCamInfo it = m_MapDropCamInfo.find(nWBObjID);
	if (it != m_MapDropCamInfo.end())
	{
		HPMC_DROP *HD = it->second;
		it->second = NULL;
		delete HD;
		HD = NULL;
		m_MapDropCamInfo.erase(it);

	}
}

BOOL CWhiteBoardView::bIsInCamInfoMap( CWBObject* nDWBIndex )
{
	DWORD nlayID = ((CWBPicture*)nDWBIndex)->GetLayerIndex();
	IT_MapDropCamInfo it = m_MapDropCamInfo.find(nlayID);
	if (it != m_MapDropCamInfo.end())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
bool CWhiteBoardView::OpenCamaraFromU7( MediaCamaraU7Data *pMediaCamaraU7Data )
{
	MediaCamaraU7Data *pdata = pMediaCamaraU7Data;
	if (pdata == NULL)
	{
		return false;
	}
	//CRect rct = CRect(74, 43, 474, 343);//要改
	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);
	CRect rcCamara;
	rcCamara.left = point.x - 200;
	rcCamara.top = point.y -150;
	rcCamara.bottom = point.y +150;
	rcCamara.right = point.x + 200;
	m_rcCamaraMediaRect.SetRect(rcCamara.left,rcCamara.top,rcCamara.right,rcCamara.bottom);
	bool bIsCaremaLocal = false;
	MediaCamaraU7Data *pBuffer = new MediaCamaraU7Data;
	memcpy(pBuffer,pdata,sizeof(MediaCamaraU7Data));
	unsigned long videocid = pBuffer->nVideoID;
	unsigned long audiocid = pBuffer->nAudioID;
	unsigned short mcuport = pBuffer->nServerPort;
	unsigned short localport = pBuffer->nLocalPort;
	std::string nodeid(pBuffer->strNodeID);
	std::string mcuid (pBuffer->strMCUID);
	std::string mcuaddr(pBuffer->strMCUIP);
	std::string nataddr(pBuffer->strNATIP);
	std::string localaddr(pBuffer->strLocalIP);
	std::string strMemberid(pBuffer->m_strUCCUerID);
	bIsCaremaLocal = pBuffer->bIsCaptureCarema;
	//m_DropChanelID = pBuffer->channelID; //获取的是拖拽打开设备的ID

	CWBCamera *pCamera = NULL;
	pCamera = new CWBCamera(bIsCaremaLocal, rcCamara,bIsCaremaLocal);//被动创建者
	if (pCamera ==NULL || pCamera->GetCameraDlg() == NULL)
	{
		return false;
	}
	m_pCamaraObj = pCamera;
	CDlgCamera *pCdlgCamara = pCamera->GetCameraDlg();

	if (bIsCaremaLocal)
	{
		g_strAvconUserID = ToDRAWStr(strMemberid).c_str();
	}
	else
	{

		SCameraReceiveData  *data = new SCameraReceiveData(audiocid, videocid, ToDRAWStr(nodeid).c_str(),
			ToDRAWStr(nataddr).c_str(), 0, ToDRAWStr(localaddr).c_str(), localport, ToDRAWStr(mcuid).c_str(),
			ToDRAWStr(mcuaddr).c_str(), mcuport, ToDRAWStr(strMemberid).c_str());
		pCdlgCamara->UpdateRemoteData(data);
		pCdlgCamara->StartCamera();

		//录像
		///////////////////////////////////////////////////////////////
		DWORD layerIndex = pCamera->GetLayerIndex();
		if (pCamera && pCamera->GetCameraDlg()
			&& pCamera->GetCameraDlg()->GetPlayChanCallback_Empty())
		{
			pCamera->GetCameraDlg()->GetPlayChanCallback_Empty()->SetCaremaPlay(layerIndex);
		}
		/////////////////////////////////////////////////////////////////
		pCamera->SetCCUserID(strMemberid);

		if (data)
		{
			delete data;
			data = NULL;
		}
		
	}
	g_pWBPage->AddWBObject(pCamera);
	//if (m_ulUserCompure == g_ulSessionID)
	//{
	//std::string strMember = std::string(g_strAvconUserID.GetBuffer());
	XMemIni xmlObj;
	pCamera->SetXmlValue(xmlObj);
	if (!bIsCaremaLocal)
	{
		xmlObj.Set(KEY_DRAG_CAMERA_TO_WB,CMD_CAMARA_DROP_OBJ);
		//xmlObj.Set(KEY_FILE_SENDER,g_ulSessionID);
		xmlObj.Set(KEY_CAMERA_LOCAL_INFO_INTEGER_AUDIOID, audiocid);
		xmlObj.Set(KEY_CAMERA_LOCAL_INFO_INTEGER_DEVID, videocid);
		xmlObj.Set(KEY_CAMERA_REMOTE_INFO_STRING_NODEID, nodeid);
		xmlObj.Set(KEY_CAMERA_REMOTE_INFO_STRING_NATIP, nataddr);
		xmlObj.Set(KEY_CAMERA_REMOTE_INFO_INTEGER_NATPORT, 0);
		xmlObj.Set(KEY_CAMERA_REMOTE_INFO_STRING_LOCALIP, localaddr);
		xmlObj.Set(KEY_CAMERA_REMOTE_INFO_INTEGER_LOCALPORT, localport);
		xmlObj.Set(KEY_CAMERA_REMOTE_INFO_STRING_MCUID, mcuid);
		xmlObj.Set(KEY_CAMERA_REMOTE_INFO_STRING_MCUIP, mcuaddr);
		xmlObj.Set(KEY_CAMERA_REMOTE_INFO_INTERGER_SERVERPORT, mcuport);
		xmlObj.Set(KEY_CAMERA_ID_OF_UCC_LOGIN_USER, strMemberid);
		xmlObj.Set(KEY_DRAG_CAMERA_TO_WB,CMD_CAMARA_DROP_OBJ);
	}

	AddObj(xmlObj);
	//}
	if (pBuffer)
	{
		delete pBuffer;
		pBuffer = NULL;
	}
	return true;

}

bool CWhiteBoardView::OpenMediaFromU7( const char *pbuff )
{
	return true;
}

bool CWhiteBoardView::ProcessGetCamInfo( DWORD nObjID,CString str,string strname,unsigned long channelID )
{

	HPMC_DROP *hp = new HPMC_DROP;
	ZeroMemory(hp,sizeof(HPMC_DROP));
	CString strMemberID = str;
	CString strChannelID;
	/*int nPos;
	nPos = str.ReverseFind('#');
	if (nPos == -1)
	{
	int nPos;
	nPos = str.ReverseFind('_');
	strMemberID = str.Left(nPos);
	strChannelID = str.Right(str.GetLength() -1- str.Find(_T("_")));
	}
	else
	{
	int nPos;
	nPos = str.ReverseFind('#');
	strMemberID = str.Left(nPos);
	strChannelID = str.Right(str.GetLength() -1- str.Find(_T("#")));
	}

	strMemberID += '\0'; */
	std::string strmember = "";
	strmember = U2A(strMemberID);
	memcpy(hp->MemberName,strmember.c_str(),strmember.length());

	if (!m_pCaremaPicture.empty())
	{
		std::vector<string>::iterator it = m_pCaremaPicture.begin();
		if (it != m_pCaremaPicture.end())
		{
			if (strmember.compare(it->c_str()) == 0)
			{
				return false;
			}
		}
	}
	memcpy(hp->MemberID,strname.c_str(),strname.length());
	hp->ChannelID = /*_ttoi(strChannelID)*/channelID;
	hp->AreaID = 0;
	hp->Type = 2;
	hp->hWnd = NULL;
	if (m_MapDropCamInfo.empty())
	{
		m_MapDropCamInfo.insert(MapDropCamInfo::value_type(nObjID,(HPMC_DROP*)hp));	
		//std::string strName = hp->MemberID;
		//m_pUserNameID->insert(pair<std::string,DWORD>(strName,nObjID));
	}
	else
	{
		IT_MapDropCamInfo it = m_MapDropCamInfo.find(nObjID);
		if (it != m_MapDropCamInfo.end())
		{
			if (hp->MemberID == it->second->MemberID && hp->ChannelID == it->second->ChannelID)
			{
				return false;
			}

		}
		m_MapDropCamInfo.insert(MapDropCamInfo::value_type(nObjID,(HPMC_DROP*)hp));
		//std::string strName = hp->MemberID;
		//m_pUserNameID->insert(pair<std::string,DWORD>(strName,nObjID));
	}
	return true;

}
CWBCamera * CWhiteBoardView::GetCamaraObjU7()
{
	if (m_pCamaraObj)
	{
		return m_pCamaraObj;
	}
	return NULL;
}

void CWhiteBoardView::SetCamaraObjU7( CWBCamera *carama )
{
	m_pCamaraObj = carama;
}

void CWhiteBoardView::SendRollCallBroadCast( int UserStyle )
{
	if (!g_pHPWBDataNotify)
	{
		return;
	}
	XMemIni xmlObj;
	xmlObj.Set(KEY_FILE_SENDER,g_ulSessionID);
	xmlObj.Set(KEY_BROADCAST_CMD,_E_BROAD_ROLL_CALL);
	xmlObj.Set(KEY_BROADCASTUSERSTYLE , UserStyle);
	if (UserStyle != 0)
	{
		if (m_nSendRollCallBroadCastID != 0)
		{
			xmlObj.Set(KEY_BROADCASTSENDROLLCALLUSER, m_nSendRollCallBroadCastID);
			xmlObj.Set(KEY_BROADCASTROLLCALLOKUSERNAME,ToStr(g_strAvconUserID.GetBuffer(g_strAvconUserID.GetLength())));
			g_strAvconUserID.ReleaseBuffer();
		}
	}
	string strValue = "";
	xmlObj.GetString(strValue);
	if (strValue != "")
	{
		AVDataOutPacket outPacket;
		int nLen = sizeof(int) + strValue.size();
		outPacket<<(UINT)WB_NEW_COMMAND_DATA;
		outPacket<<(unsigned long)nLen;
		char* pData = new char[nLen + 1];
		if (pData)
		{
			int iCmd = CMD_BROADCAST_DATA;
			memset(pData,0,nLen);
			memcpy(pData,(char*)&iCmd,sizeof(int));
			memcpy(pData + sizeof(int),strValue.c_str(),strValue.size());
			pData[nLen] += '\0';
			outPacket.WriteData((unsigned char*)pData,nLen +1);
		}
		SendPacketData((char*)outPacket.GetData(),outPacket.GetLength());
		if (pData)
		{
			delete pData;
			pData = NULL;
		}

	}
	if (UserStyle == 0)
	{
		SetTimer(ROLLCALLSENDTIMER,1000,NULL);
		m_nRollCallTimer = 40;
	}

	m_nSendRollCallBroadCastID = 0;
	//ProcessRollCallBroadCast();

}

void CWhiteBoardView::ProcessRollCallBroadCast()
{
	if (!m_wbrcprodlg)
	{
		CRect viewRectInScreen;
		GetWindowRect(&viewRectInScreen);
		m_wbrcprodlg = new CWBRollCallDialog(viewRectInScreen.left,viewRectInScreen.top);

		m_wbrcprodlg->Create(CWBRollCallDialog::IDD,g_pWhiteBoardView);
	}
	else
	{
		CRect viewRectInScreen;
		GetWindowRect(&viewRectInScreen);
		m_wbrcprodlg->SetRollCallShow(viewRectInScreen.left,viewRectInScreen.top);

	}
}

void CWhiteBoardView::ProcessRollCallLRSultBroadCast(string str)
{
	m_RollCallNameList.push_back(str);
}

LRESULT CWhiteBoardView::ProcessMTUserCursor( WPARAM wParam,LPARAM lParam)
{
	CPoint point = CPoint(0,0);
	point.x = wParam;
	point.y = lParam;
	if (m_bBroadcastUserCursor)
	{			
		if (!g_strLocalUserName.IsEmpty())
		{
			if (m_pTransparent_dlg == NULL)
			{
				std::string strUserid = ToStr(g_strLocalUserName.GetBuffer(g_strLocalUserName.GetLength()));
				g_strLocalUserName.ReleaseBuffer();
				char*   pszAvconUserID = new char[strUserid.length() +1];
				strcpy(pszAvconUserID,strUserid.c_str());
				m_pTransparent_dlg = new CTransparent_Dlg(this,false);
				m_pTransparent_dlg->SetStrname(pszAvconUserID);
				m_pTransparent_dlg->Create(CTransparent_Dlg::IDD,this);

				if (pszAvconUserID)
				{
					delete []pszAvconUserID;
					pszAvconUserID = NULL;
				}
			}		
			else
			{
				if ((m_pTransparent_dlg->GetUsrName()) == "")
				{
					delete m_pTransparent_dlg;
					m_pTransparent_dlg = NULL;
					// 			char*   pszAvconUserID = new char[g_strAvconUserID.GetLength() +1];
					// 			_tcscpy(pszAvconUserID,g_strAvconUserID);
					// 			m_pTransparent_dlg->SetStrname(pszAvconUserID);
					// 			delete pszAvconUserID;
					// 			pszAvconUserID = NULL;
				}

			}
			if (g_reduce>0.00)
			{
				point.x = point.x*g_reduce;
				point.y = point.y*g_reduce;
			}
			CPoint pt = point;
			pt.x = pt.x /*- g_pWhiteBoardView->getXnPos()*/;
			pt.y = pt.y /*- g_pWhiteBoardView->getYnPos()*/;
			CRect viewClient;
			GetClientRect(&viewClient);
			if (!viewClient.PtInRect(pt))
			{
				return 0;
			}
			CRect viewRectInScreen;
			GetWindowRect(&viewRectInScreen);
			pt.x -= viewRectInScreen.left;
			pt.y -= viewRectInScreen.top;
			CRect rectImg;
			m_pTransparent_dlg->GetClientRect(&rectImg);
			POINT _ptBR = pt;
			POINT _ptpoint = pt;
			_ptBR.y -= 28;
			_ptpoint.x += rectImg.Width() ;
			_ptpoint.y += 40;
			rectImg.SetRect(_ptBR,_ptpoint);
			ClientToScreen(&rectImg);
			m_pTransparent_dlg->MoveWindow(&rectImg);
			m_pTransparent_dlg->ShowWindow(SW_SHOW);

			if (m_lmovecounts%10 == 0)
			{
				CPoint ptSend = CPoint(wParam,lParam);				
				CRect viewRectInScreen;
				GetWindowRect(&viewRectInScreen);
				ptSend.x -= viewRectInScreen.left;
				ptSend.y -= viewRectInScreen.top;
				ptSend.x += getXnPos();
				ptSend.y += getYnPos();
				BroadcastUserCursor(ptSend,ToStr(g_strLocalUserName.GetBuffer(g_strLocalUserName.GetLength())));
				g_strLocalUserName.ReleaseBuffer();
			}
			++m_lmovecounts ;
			if (m_lmovecounts > 10000)
			{
				m_lmovecounts = 0;
			}
		}

	}
	return 0;
}

void CWhiteBoardView::DeleteWaterCursorForAvcon6()
{
	if (m_waterdlg)
	{
		delete m_waterdlg;
		m_waterdlg = NULL;
	}
	if (m_pTransparent_dlg)
	{
		delete m_pTransparent_dlg;
		m_pTransparent_dlg = NULL;
	}
	ReJectAllBroadcastUserWater();
	ReJectAllBroadcastUserCursor();

}

void CWhiteBoardView::HideUserCursor()
{
	if (m_pTransparent_dlg)
	{
		m_pTransparent_dlg->ShowWindow(SW_HIDE);
	}
	ReJectAllBroadcastUserCursor();
// 	IT_MapCursorDlgPointer it = m_MapCursorDlgPointer.begin();
// 	while (it != m_MapCursorDlgPointer.end())
// 	{
// 		CTransparent_Dlg* pDlg =  (CTransparent_Dlg*)it->second;
// 		if (pDlg)
// 		{
// 			pDlg->ShowWindow(SW_HIDE);
// 		}
// 		it++;
// 	}
}


void CWhiteBoardView::ProcessCloseColorDlg()
{
	if (m_pWBFunctionDlg)
	{
		CWBFunctionAttributeDlg *pAttributeDlg = (CWBFunctionAttributeDlg*)m_pWBFunctionDlg->GetAttributeDlg();
		if (pAttributeDlg)
		{
			pAttributeDlg->CloseColorPaletteDlg();
		}
		
	}
}
CWBSavePictureDlg * CWhiteBoardView::GetWBSavePictureDlg()
{
	if(m_pWBSavePictureDlg == NULL)
	{
		m_pWBSavePictureDlg = new CWBSavePictureDlg(g_pWhiteBoardView);
		m_pWBSavePictureDlg->Create(IDD_SAVE_PICTURE,g_pWhiteBoardView);
		if (ZSYDZ)
		{
			m_pWBSavePictureDlg->PrintCurrentPage();
		}
	}
	else
	{
		if (ZSYDZ)
		{
			m_pWBSavePictureDlg->ShowWindow(SW_HIDE);
			m_pWBSavePictureDlg->PrintCurrentPage();
		}
		else
		{
			m_pWBSavePictureDlg->ShowWindow(SW_SHOW);
		}
		
	}
	return m_pWBSavePictureDlg;
}
void CWhiteBoardView::SetWBfunction( CWBFunctionDlg * fun )
{
	m_pWBFunctionDlg = fun;
}

void CWhiteBoardView::ChangeLeftTop_RightBottomPoint( CPoint &ptLeft, CPoint &ptRight )
{
	CPoint ptLeftTop;
	CPoint ptRightBottom;

	if ((ptLeft.x <= ptRight.x) && (ptLeft.y <= ptRight.y))
	{
		ptLeftTop = ptLeft;
		ptRightBottom = ptRight;
	}
	else if ((ptLeft.x > ptRight.x) && (ptLeft.y > ptRight.y))
	{
		ptLeftTop = ptRight;
		ptRightBottom = ptLeft;
	}
	else if ((ptLeft.x > ptRight.x) && (ptLeft.y <= ptRight.y))
	{
		ptLeftTop.x = ptRight.x;
		ptLeftTop.y = ptLeft.y;
		ptRightBottom.x = ptLeft.x;
		ptRightBottom.y = ptRight.y;
	}
	else if((ptLeft.x <= ptRight.x) && (ptLeft.y > ptRight.y))
	{
		ptLeftTop.x = ptLeft.x;
		ptLeftTop.y = ptRight.y;
		ptRightBottom.x = ptRight.x;
		ptRightBottom.y = ptLeft.y;
	}

	if ((ptRightBottom.x - ptLeftTop.x) < 30)
	{
		ptRightBottom.x = ptLeftTop.x + 30;
	}
	if ((ptRightBottom.y - ptLeftTop.y) < 30)
	{
		ptRightBottom.y = ptLeftTop.y + 30;
	}
	ptLeft = ptLeftTop;
	ptRight = ptRightBottom;
}

void CWhiteBoardView::SetRefMoreSelectFrameRange( CRect rect )
{
	m_rectRefresh = rect;
}

bool CWhiteBoardView::GetIsRectRefresh()
{
	return m_bflag;
}

void CWhiteBoardView::SetIsRectRefresh( bool bl )
{
	m_bflag = bl;
}
void CWhiteBoardView::ShowOrHideMediaCarema( UINT Index )
{
	if (g_pWBFile)
	{
		CWBPage *page = NULL;
		UINT nCountPage = g_pWBFile->GetPageCount();
		for(int i = 1;i <= nCountPage; ++i)
		{
			page = g_pWBFile->GetPage(i);
			if (page)
			{
				page->StartOrPauseMovieCur(false);
			}
		}
		page = g_pWBFile->GetPage(Index);
		if (page)
		{
			page->StartOrPauseMovieCur(true);
		}
	}
}

void CWhiteBoardView::ExitRoomDeleteStreamObj()
{
	int nPage = g_pWBFile->GetPageCount();
	for (int i =1 ; i <= nPage; ++i)
	{
		CWBPage *pWBPage = g_pWBFile->GetPage(i); 
		std::vector<DWORD *> vecPageOBjLayerIndex; //页所有对象的图层号
		pWBPage->GetAllObjLayerIndex(vecPageOBjLayerIndex);//得到页所有对象的图层号

		CWBObject * pObj = NULL;
		POSITION pos = NULL;
		for (int j = 0; j < vecPageOBjLayerIndex.size(); ++j)
		{
			if (vecPageOBjLayerIndex[j] == NULL)
			{
				continue;
			}
			pObj = pWBPage->FindByLayerIndex(*(vecPageOBjLayerIndex[j]));
			if (pObj == NULL)
			{
				continue;
			}
			if ((pObj->GetType() == WB_OBJ_MEDIA &&!((CWBMedia *)pObj)->IsActiveCreated())
				||(pObj->GetType() == WB_OBJ_CAMERA && !((CWBCamera *)pObj)->BCreatorOwner()) 
				)
			{
				pWBPage->DeleteWBObject(*(vecPageOBjLayerIndex[j]));
				
				g_pWhiteBoardView->Invalidate();
			}	
			delete vecPageOBjLayerIndex[j];
		}
		vecPageOBjLayerIndex.clear();
	}	
}



void CWhiteBoardView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
	//用于截取view滚动条的右键菜单消息
}

void CWhiteBoardView::ChangViewSize(CRect rect,double dReduceRate)
{
	
	g_reduce=dReduceRate;
	CWBPageDlg* pPageDlg=((CWhiteBoardDlg*)g_whiteboardDlg)->GetPageDlg();
	
	BOOL isSinFuncShow=((CWhiteBoardDlg*)g_whiteboardDlg)->GetSinFuncShow();
	if(g_reduce>0.00000000000000000)
	{
		
		double nViewWidth=(double)WB_VIEWHSCROLL*g_reduce;
		double nViewHight=(double)WB_VIEWVSCROLL*g_reduce;

		CRect rc;
		g_whiteboardDlg->GetClientRect(&rc);
		double nWindowWidth;
		double nWindowHight=rc.bottom;
		//此处判断功能面板是否打开
		if(isSinFuncShow)
		{
			nWindowWidth=rc.right-(WB_FUNCTION_DLG_WIDTH + WB_OPTIONDLG_WIDTH);
			rect.right=rect.right-(WB_FUNCTION_DLG_WIDTH + WB_OPTIONDLG_WIDTH);
			((CWhiteBoardDlg*)g_whiteboardDlg)->SetViewPos(rect);
			if (((CWhiteBoardView*)g_pWhiteBoardView)->GetWBFunctionDlg()&& ((CWhiteBoardView*)g_pWhiteBoardView)->GetWBFunctionDlg()->GetPreviewDLG())
			{
				((CWhiteBoardView*)g_pWhiteBoardView)->GetWBFunctionDlg()->GetPreviewDLG()->GetPreviewView()->SetRatio();//调整预览的显示比例
			}
			

		}
		else
		{
			 rect.right=rect.right-WB_OPTIONDLG_WIDTH ;
			 ((CWhiteBoardDlg*)g_whiteboardDlg)->SetViewPos(rect);
			if(((CWhiteBoardDlg*)g_whiteboardDlg)->GetSinOptShow())
			{
				nWindowWidth=rc.right- WB_OPTIONDLG_WIDTH;


			}
			else
			{
				nWindowWidth=rc.right;

			}
		}
		if((nViewHight<nWindowHight)&&(nViewWidth<nWindowWidth))
		{
			 if(((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg())
			 {
				 if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg()->GetPreviewSingleView())
				 {
					 ((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg()->GetPreviewSingleView()->OnEagleEyeClose();
				 }
				 
			 }
		}
	    if(((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg())
	    {
			if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg()->GetPreviewSingleView())
			{
				((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg()->GetPreviewSingleView()->SetRatio();//更新鹰眼中的矩形框
			}
	    }
	  

		if(((CWhiteBoardView*)g_pWhiteBoardView)->GetWBFunctionDlg())
		{
			if (((CWhiteBoardView*)g_pWhiteBoardView)->GetWBFunctionDlg()->GetPreviewDLG())
			{
				if (((CWhiteBoardView*)g_pWhiteBoardView)->GetWBFunctionDlg()->GetPreviewDLG()->GetPreviewView())
				{
					((CWhiteBoardView*)g_pWhiteBoardView)->GetWBFunctionDlg()->GetPreviewDLG()->GetPreviewView()->SetRatio();//调整预览的显示比例
				}
			}
		    CWBPreviewDlg *pre=((CWhiteBoardView*)g_pWhiteBoardView)->GetWBFunctionDlg()->GetPreviewDLG();
			TRACE("D");
		}
		
		

	}
	else
	{
		//g_pWhiteBoardView->SetWindowPos((CWnd *)dlg,0,0,1200,700,SWP_SHOWWINDOW);
		g_pWhiteBoardView->MoveWindow(0,0,rect.right,rect.bottom);
		pPageDlg->MoveWindow(rect.right - (WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70+ 30), 10,WB_TOOLBOXBTN_SPEC_WIDTH * 3 + 70
			,WB_TOOLBOXBTN_SPEC_HEIGHT); 
	}
	
}
void CWhiteBoardView::ChangeViewCoordinate(CDC *pDC)
{
	int nScrollX=g_pWhiteBoardView->getXnPos();//滚动条滑动的距离
	int nScrollY=g_pWhiteBoardView->getYnPos();
	
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetViewportOrg(-nScrollX,-nScrollY);//设置原点
	pDC->SetWindowOrg(0,0);

	pDC->SetViewportExt(WB_VIEWHSCROLL, WB_VIEWVSCROLL);
	pDC->SetWindowExt(WB_VIEWHSCROLL/g_reduce, WB_VIEWVSCROLL/g_reduce);
}

afx_msg void CWhiteBoardView::OnReduce(WPARAM wParam)
{
	double dReduceRate=0.00;
	m_dwCheckItem=wParam;
	switch(wParam)
	{
	case ID__20:
		dReduceRate=0.20;
		//((CWhiteBoardDlg*)g_whiteboardDlg)->GetMulWBFunctionDlg()->GetAttributeDlg()->m_ZoomCombx.SetCurSel(0);
		
		break;
	case ID__30:
		dReduceRate=0.30;
		
		break;
	case ID__40:
		dReduceRate=0.40;
		break;
	case ID__50:
		dReduceRate=0.50;
		break;
	case ID__60:
		dReduceRate=0.60;
		break;
	case ID__70:
		dReduceRate=0.70;
		break;
	case ID__80:
		dReduceRate=0.80;
		break;
	case ID__90:
		dReduceRate=0.90;
		break;
	case ID__100:
		dReduceRate=1.00;
		break;
	case ID__120:
		dReduceRate=1.20;
		break;
	case ID__140:
		dReduceRate=1.40;
		break;
	case ID__160:
		dReduceRate=1.60;
		break;
	case ID__180:
		dReduceRate=1.80;
		break;
	case ID__200:
		dReduceRate=2.00;
		break;
	case ID__230:
		dReduceRate=2.30;
		break;
	case ID__260:
		dReduceRate=2.60;
		break;
	case ID__290:
		dReduceRate=2.90;
		break;
	case ID__320:
		dReduceRate=3.20;
		break;
	
	default:
		break;


	}
	g_pWBPage->ClearSelectFrameList();//缩放的时候清掉选择框
	if(g_pWhiteBoardView->m_pWBFunctionDlg)
	{


		DWORD dwItem=g_pWhiteBoardView->GetCheckItem();
		//::SendMessage(g_pWhiteBoardView->m_pWBFunctionDlg->m_hWnd,WM_CHANGEZOOMCOMBXITEM,0,dwItem);
	}
	

	CRect rc;
	g_whiteboardDlg->GetClientRect(&rc);//获得白板的大小
	
	ChangeSize(/*CRect(0,0,1200,700)*/rc,dReduceRate);
	//SetPageIsNeedDraw(TRUE);

}

void CWhiteBoardView::ChangeSize(CRect rect,double dReduceRate )
{
	

		ChangViewSize(rect,dReduceRate);

		setViewScroll();//重新设置滚动条

		SetPageIsNeedDraw(TRUE);
		InvalidateRect(&rect,TRUE);
		//Invalidate();

	
}



void CWhiteBoardView::SetCheckItem(DWORD dItem)
{
	m_dwCheckItem=dItem;
}


DWORD CWhiteBoardView::GetCheckItem()
{
	return m_dwCheckItem;
}

void CWhiteBoardView::ProcessMTMoveRationalCenter( CPoint ptOffset )
{
// 	int nIndex;
// 	CWBObject *pObj = NULL;	
// 	POSITION pos;
// 	/*CWBPage *pPage = g_pWBPage;*/
// 	for (pos = g_pWBPage->m_lstSelectObject.GetHeadPosition(); NULL != pos;)
// 	{
// 		pObj = m_lstSelectObject.GetNext(pos);
// 		//如果对象被锁定 或 被限制操作（一般是在主动方操作该对象，互动方被限制）
// 		if (pObj == NULL || pObj->IsLock() || 	pObj->IsLimit()) //huanglongquan add
// 		{
// 			continue;
// 		}
// 		if (pObj->IsBkg())
// 		{
// 			continue;
// 		}
// 		if (pObj->GetType() == WB_OBJ_PROTRACTOR||pObj->GetType() ==WB_OBJ_SETSQUARE)
// 		{
// 			continue;
// 		}
// 		//使对象中心点偏移
// 		CPoint ptRotationCenter;
// 		pObj->GetRotationCenter(ptRotationCenter);
// 		ptRotationCenter += ptOffset;
// 		pObj->RotateByPoint(ptRotationCenter,0);
// 		(pObj->GetImageButton())->setOffsetCenter(ptOffset);
// 
// 
// 	}
// 	return;
}

//add by jiangchao 2013.06.06
void CWhiteBoardView::SendCurvePicFromFile(const char* strPath)
{
	CString strTemp = A2U(strPath); //发送文件路径

	m_ulIcoCurEvent = GenerateSSRC(); //同步事件
	m_dwIcoCurId = GenerateSSRC(); //曲线文件ID

	if(g_pHPWBDataNotify && !m_isLocalWhiteboard) //判断是否网络同步
	{
		XMemIni xmlObj;

		CFile File;
		if(!File.Open(strTemp, CFile::modeRead|CFile::shareDenyNone))
		{
			return;
		}
		unsigned long ulFileSize = (unsigned long)(File.GetLength()); //缓冲区总长度
		File.Close();

		CString strFileType = strTemp.Right(strTemp.GetLength() - strTemp.ReverseFind('.') - 1);

		xmlObj.Set(KEY_EVENT_ID,m_ulIcoCurEvent);
		xmlObj.Set(KEY_OBJ_ID, m_dwIcoCurId); //一定要有ID 不然不会接受文件
		xmlObj.Set(KEY_FILE_PATH, (std::string)ToStr(strTemp.GetBuffer(strTemp.GetLength())));
		xmlObj.Set(KEY_FILE_TYPE, (std::string)ToStr(strFileType.GetBuffer(strFileType.GetLength())));
		xmlObj.Set(KEY_FILE_POS, 0);
		xmlObj.Set(KEY_FILE_SIZE, ulFileSize);
		xmlObj.Set(KEY_SENDFILEMARK, 1); //发送指令

		AddObj(xmlObj); //发送到服务器
		strTemp.ReleaseBuffer();
		strFileType.ReleaseBuffer();
	}
	else //单机版
	{
		//不处理
	}
}

int CWhiteBoardView::GetCompassDrawShape()
{
	return m_nCompassDrawShape;
}

void CWhiteBoardView::SetCompassDrawShape( int num )
{
	m_nCompassDrawShape = num;
}

void CWhiteBoardView::CloseBroadcastUserCursor() //权限控制关闭水波纹和光标
{
	m_bBroadcastUserCursor = false;
	g_bRunWater = false;
	if (m_pTransparent_dlg)
	{
		m_pTransparent_dlg->ShowWindow(SW_HIDE);
	}	
	RejectBroadcastUserCursor(ToStr(g_strLocalUserName.GetBuffer(g_strLocalUserName.GetLength())));
	g_strLocalUserName.ReleaseBuffer();
	if (m_waterdlg)
	{
		m_waterdlg->ShowWindow(SW_HIDE);
	}

	if (g_hVedioHwnd)
	{
		::SendMessage(g_hVedioHwnd,WM_LBUTTONUP,0,0);

	}
	if (NULL != m_MedioVideoScreenDlg)
	{
		delete m_MedioVideoScreenDlg;
		m_MedioVideoScreenDlg = NULL;
	}
}

void CWhiteBoardView::ChangeUserCurorTranDlgPosition( CPoint ptoffset ) //处理U7中拖拽白板窗口后改变光标的位置
{
	CRect rc;
	if (!m_bIsneedjionBroadcast)
	{
		return;
	}
	if (m_bBroadcastUserCursor)
	{		
		if (m_pTransparent_dlg)
		{			
			m_pTransparent_dlg->GetWindowRect(&rc);	
			int width = rc.Width();
			int height = rc.Height();
			rc.left += ptoffset.x;
			rc.right += ptoffset.x;
			rc.top += ptoffset.y;
			rc.bottom += ptoffset.y;
			CRect WBrect;
			GetWindowRect(&WBrect);
			if (rc.left <= WBrect.left)
			{
				rc.left = WBrect.left;
				rc.right = rc.left + width;
			}
			else if (rc.right >= WBrect.right)
			{
				rc.right = WBrect.right;
				rc.left = rc.right - width;
			}

			if (rc.top <= WBrect.top)
			{
				rc.top = WBrect.top;
				rc.bottom = rc.top + height;
			}
			else if (rc.bottom >= WBrect.bottom)
			{
				rc.bottom = WBrect.bottom;
				rc.top = rc.bottom - height;
			}
			m_pTransparent_dlg->MoveWindow(&rc);
			m_pTransparent_dlg->ShowWindow(SW_SHOW);
		}

	}
	IT_MapCursorDlgPointer it = m_MapCursorDlgPointer.begin();
	while (it != m_MapCursorDlgPointer.end())
	{
		CTransparent_Dlg* pDlg =  (CTransparent_Dlg*)it->second;
		pDlg->GetWindowRect(&rc);			
		int width = rc.Width();
		int height = rc.Height();
		rc.left += ptoffset.x;
		rc.right += ptoffset.x;
		rc.top += ptoffset.y;
		rc.bottom += ptoffset.y;
		CRect WBrect;
		GetWindowRect(&WBrect);
		if (rc.left <= WBrect.left)
		{
			rc.left = WBrect.left;
			rc.right = rc.left + width;
		}
		else if (rc.right >= WBrect.right)
		{
			rc.right = WBrect.right;
			rc.left = rc.right - width;
		}

		if (rc.top <= WBrect.top)
		{
			rc.top = WBrect.top;
			rc.bottom = rc.top + height;
		}
		else if (rc.bottom >= WBrect.bottom)
		{
			rc.bottom = WBrect.bottom;
			rc.top = rc.bottom - height;
		}
		pDlg->MoveWindow(&rc);
		pDlg->ShowWindow(SW_SHOW);
		++it;
	}

}
/*处理视频拍照特效||||ptRes是资源库的位置，bRight是旋转从左还是从右*/
bool CWhiteBoardView::ProcessMediaPhotoEffect(CPoint ptRes,bool bRight)
{
	if (m_pWBPicture != NULL)
	{
		if (m_pWBPicture->IsLimit() || m_pWBPicture->IsLock())
		{
			return false;
		}

		//把图片水平放置
		CPoint ptRationCenter;
		m_pWBPicture->GetCenterPoint(ptRationCenter);
		double angle = m_pWBPicture->GetRotationAngle();
		m_pWBPicture->RotateByPoint(ptRationCenter, -angle);
		int width = m_pWBPicture->GetWidth();
		m_pWBPicture->SetZoomNeedCreateFram(true);

		CPoint newRationcenter;
		if (bRight)
		{
			newRationcenter.x = ptRationCenter.x + width;
			newRationcenter.y = ptRationCenter.y;
		}
		else
		{
			newRationcenter.x = ptRationCenter.x - width;
			newRationcenter.y = ptRationCenter.y;
		}
		int alpha = 255;
		int index = 0;
		int  stepNumber = 10;
		int initialAlpha = alpha;
		double zoomRate[10] = {1.0 ,1.0 ,1.0 ,0.9 ,0.8 ,0.7 ,0.75 ,0.65 ,0.6 ,0.5};
		while (index < stepNumber)
		{
			CPoint ptCenter;
			ptCenter.x = (int)(m_pWBPicture->m_pItem->rect.left + (m_pWBPicture->m_pItem->rect.Width()/2));
			ptCenter.y = (int)(m_pWBPicture->m_pItem->rect.top + (m_pWBPicture->m_pItem->rect.Height()/2));
			//m_pWBPicture->GetRotationCenter(ptCenter);
			m_pWBPicture->SetIsPath();
			m_pWBPicture->Zoom(ptCenter,zoomRate[index],zoomRate[index]);
 			CPoint ptOffset = CPoint(0,0);
			if (index < 3)
			{
				if (bRight)
				{
					ptOffset = CPoint((int)(width/3),0);
				}
				else
				{
					ptOffset = CPoint((int)(-width/3),0);
				}
			}
			else
			{
				int a = ptRes.x + getXnPos();
				a = (int)( a- newRationcenter.x)/5;
				ptOffset.x = (int)(a);
				int b =0;
				b = (ptRes.y + getYnPos() - newRationcenter.y)/5;
				ptOffset.y = (int)(b);
			}
			m_pWBPicture->MoveOffset(ptOffset);
			//m_pWBPicture->RotateByCenter(60);
			m_pWBPicture->SetSelectFrameShow(FALSE);
			if (stepNumber >1)
			{
				if (index != stepNumber -1)
				{
					alpha -= (initialAlpha/2)/(stepNumber -1);					
				}
				if (index == 9)
				{

					if (WBRESWND::IsGalleryWndCreated())
					{						
						WBRESWND::UpdateResGalleryEx();
					}
				}
			}
			index += 1;
			Invalidate();
			UpdateWindow();
			
		}
		if (m_pWBPicture != NULL)
		{
			#ifndef SUPPORTMT
			CWBMTObjectManager::GetIns_()->RemoveMTObject(m_pWBPicture);
#endif
			delete m_pWBPicture;
			m_pWBPicture = NULL;
		}
		Invalidate();
		UpdateWindow();
		return true;
	}
	return false;
}

void CWhiteBoardView::SetWBPictureEffect( CWBPicture *pWBObj )
{
	if (m_pWBPicture != NULL)
	{
		delete m_pWBPicture;
		m_pWBPicture = NULL;
	}
	m_pWBPicture = pWBObj;
}

void CWhiteBoardView::DeleteSpePicInMap( DWORD dwlayer )
{
	return;//暂时不要以后再说
	CWBObject *pObj = g_pWBPage->FindByLayerIndex(dwlayer);
	if ((pObj != NULL) &&(pObj->GetType() == WB_OBJ_PICTURE))
	{
// 		PWBPictureItem pItem;
// 		((CWBPicture*)pObj)->GetItem(pItem);
		CString strPath;
		strPath = ((CWBPicture*)pObj)->m_pItem->strFilePath;

		if (m_pResourcePic)
		{
			std::map<CString,VECTORSPECPIC>::iterator it = m_pResourcePic->find(strPath);
			if (it != m_pResourcePic->end())
			{
				VECTORSPECPIC vec = it->second;
				if (!vec.empty())
				{
					VECTORSPECPIC::iterator veit = find(vec.begin(),vec.end(),dwlayer);
					if (veit != vec.end())
					{
						vec.erase(veit);
					}
				}
				it->second = vec;
			}
		}
	}
}

bool CWhiteBoardView::DeleteSpePicFromResource( CString strPath )
{
	if (m_pResourcePic)
	{
		std::map<CString,VECTORSPECPIC>::iterator it = m_pResourcePic->find(strPath);
		if (it != m_pResourcePic->end())
		{
			VECTORSPECPIC vec = it->second;
			for (int i =0;i<vec.size();i++)
			{
				DWORD dwLayer = vec[i];
				CObList* PageList;
				PageList = g_pWBFile->GetWBPageList();
				POSITION pos;
				CWBPage *pPage;
				for (int i = 0;i<PageList->GetCount();i++)
				{
					pos = PageList->FindIndex(i);
					if (pos == NULL)
					{
						continue;
					}
					pPage = (CWBPage *)PageList->GetAt(pos);
					if (pPage == NULL)
					{
						continue;
					}
					pPage->DeleteWBObject(dwLayer);
				}
				
			}
			vec.clear();
			m_pResourcePic->erase(it);
			SetPageIsNeedDraw(TRUE);
			Invalidate(TRUE);
			SetPageIsNeedDraw(FALSE);			
		}
		
	}
	return true;
}

HRESULT CWhiteBoardView::ProcessResourceMessageBox( WPARAM wParam,LPARAM lParam )
{
	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("提示");
	}
	CString strIsSave;
	strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FileIsUsed"));
	if (strIsSave.IsEmpty())
	{
		strIsSave = _T("该文件已被占用了！");
	}	
	if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
	{
			
		//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_OK,true);
		g_pHPWBDataNotify->ShowTipWindow(strIsSave);
	}
	else
	{
		AfxMessageBox(strIsSave,MB_OK);
	}
	return S_OK;
}

void CWhiteBoardView::CheckDeleteObjInVector( CWBPicture* pWBPic )
{
	std::vector<CWBPicture*>::iterator it = m_pDropPicObj.begin();
	while (it != m_pDropPicObj.end())
	{
		if (m_pDropPicObj.size() == 0)
		{
			break;
		}
		if (*it == pWBPic)
		{
			m_pDropPicObj.erase(it);
			break;
		}		
		it++;
		
	}
}

void CWhiteBoardView::CheckVectorBeforCloseWB()
{
	if (!m_pDropPicObj.empty())
	{
// 		int nChoose;
// 		CString strPrompt;
// 		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
// 		if (strPrompt.IsEmpty())
// 		{
// 			strPrompt = _T("提示");
// 		}
// 		CString strIsSave;
// 		strIsSave = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("IsSaveEditPic"));
// 		if (strIsSave.IsEmpty())
// 		{
// 			strIsSave = _T("是否需要保存正在编辑的图片？");
// 		}
// 		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
// 		{
// 			nChoose = g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strIsSave, MB_YESNO,true);
// 		}
// 		else
// 		{
// 			nChoose = AfxMessageBox(strIsSave,MB_YESNO);
// 		}
// 		if (nChoose != IDNO)
		{
			std::vector<CWBPicture*>::iterator it ;
			for (it = m_pDropPicObj.begin();it<m_pDropPicObj.end();++it)
			{
				if (m_pDropPicObj.size() == 0)
				{
					break;
				}
				((CWBPicture*)*it)->SavePictureToJpg();
			}
		}
		
	}
}

void CWhiteBoardView::AddPicToVector( CWBPicture* pWbPic )
{
	if (m_pDropPicObj.empty())
	{
		m_pDropPicObj.push_back(pWbPic);
	}
	else
	{
		bool bfound = false;
		std::vector<CWBPicture*>::iterator it = m_pDropPicObj.begin();
		while (it != m_pDropPicObj.end())
		{
			if (m_pDropPicObj.size() == 0)
			{
				break;
			}
			if (*it == pWbPic)
			{
				bfound =true;
				break;
			}		
			it++;

		}
		if (!bfound)
		{
			m_pDropPicObj.push_back(pWbPic);
		}
	}
	
}

bool CWhiteBoardView::CheckDiskPlaceForPic()
{
	ULARGE_INTEGER   ulTotalSize;       //磁盘总空间大小       
	ULARGE_INTEGER   ulFreeSize;        //磁盘剩余空间大小
	ULARGE_INTEGER   ulAvailLableSize;  //

	CString csStr,strTemp;
	csStr = g_strMediaPhoto.Left(3);
	::GetDiskFreeSpaceEx(csStr,&ulTotalSize,&ulFreeSize,&ulAvailLableSize);
	strTemp.Format(_T("%ldM"),ulTotalSize.QuadPart/1024/1024);

	if (ulTotalSize.QuadPart / 1024 /1024 < 100)
	{
		CString strMessagePrompt;
		strMessagePrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("MessagePrompt"));
		if (strMessagePrompt.IsEmpty())
		{
			strMessagePrompt = _T("消息提示");
		}
		CString strDiskNotEnough;
		strDiskNotEnough = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("DiskNotEnough"));
		if (strDiskNotEnough.IsEmpty())
		{
			strDiskNotEnough = _T("保存文件时剩余磁盘空间不足！");
		}
		if (g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,strMessagePrompt,strDiskNotEnough, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strDiskNotEnough);
		}
		else
		{
			MessageBox(NULL,strDiskNotEnough,MB_OK);
		}
		return false;
	} 
	return true;
}

void CWhiteBoardView::AddMidi( CString strPath )
{
	if (strPath.IsEmpty())
	{
		return;
	}

	g_pWBPage->ClearSelectFrameList();

	CWBMidi* pwbmidi = new CWBMidi(strPath);
	g_pWBPage->AddWBObject(pwbmidi);
	SetPageIsNeedDraw(TRUE);
	Invalidate();
	UpdateWindow();
	g_nDrawStatus = DRAW_NORMAL;

	XMemIni xmlObj;
	pwbmidi->SetXmlValue(xmlObj);	
	AddObj(xmlObj);

	return;

}

DWORD CWhiteBoardView::GetdwMenuLayerIndex()
{
	return m_dwMenuLayerIndex;
}
void CWhiteBoardView::SetdwMenuLayerIndex(DWORD dwMenuLayerIndex)
{
	m_dwMenuLayerIndex = dwMenuLayerIndex;
}

int CWhiteBoardView::GetObjCopyListCount()
{
	return m_pObjCopyList.size();
}
bool CWhiteBoardView::SingleObjIsLock()
{
	if((NULL == g_pWBPage) && (1 != g_pWBPage->GetSelectObjNumber()))
	{
		return false;
	}
	else
	{
		CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();//获取选中对象列表
		POSITION pos;
		CWBObject *pObj = NULL;
		pos = pSelectObjLst->FindIndex(0);
		pObj = pSelectObjLst->GetAt(pos);
		if (pObj == NULL)
		{
			return false;;
		}
		if (pObj->IsLock() || pObj->IsLimit())
		{
			return true;
		}
		return false;
	}
}

int CWhiteBoardView::GetWBObjCount()
{
	return g_pWBPage->GetWBList()->GetCount();
}

void CWhiteBoardView::RefreshWB()
{
	SetTimer(ID_TIEMR_REFRESH, 100, NULL);
}

void CWhiteBoardView::ClearUndo()
{
	AVDataOutPacket outPacket;
	int nLen = 3;
	outPacket<<(UINT)WB_NEW_COMMAND_DATA;
	outPacket<<(unsigned int)nLen;
	char* pData = new char[nLen+1];
	if (pData)
	{
		int iCmd = CMD_CLEAR_UNDO_QUEUE;
		memset(pData,0,nLen);
		memcpy(pData,(char*)&iCmd,sizeof(int));
		pData[nLen] = '\0';
		outPacket.WriteData((unsigned char*)pData,nLen+1);
	}

	SendPacketData((char*)outPacket.GetData(), outPacket.GetLength());

	if (pData)
	{
		delete []pData;
		pData = NULL;
	}
}

void CWhiteBoardView::PostMessageToWhiteBoard(UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (NULL != m_hWnd)
	{
		::PostMessage(m_hWnd, msg, wparam, lparam);
	}
}