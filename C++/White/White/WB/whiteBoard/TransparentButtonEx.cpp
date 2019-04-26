// TransparentButtonEx.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "TransparentButtonEx.h"
#include "common\WBData.h"
#include "WBToolbarDlg.h"
#include "WBToolboxDlg.h"



// CTransparentButtonEx
#define TIMER_TOUCH_SHOW_TIP 1

MAP_PATH CTransparentButtonEx::m_mapPath;
eType CTransparentButtonEx::g_GraphicsbuttonType = TYPE_NONE;
eType CTransparentButtonEx::g_LineTypebuttonType = TYPE_NONE;
eType CTransparentButtonEx::g_RubberbuttonType = TYPE_NONE;
IMPLEMENT_DYNAMIC(CTransparentButtonEx, CButton)

CTransparentButtonEx::CTransparentButtonEx()
{
	m_bMouseIn = FALSE;
    m_pImg = NULL;
	m_pImgBG = NULL;
	m_pParentWnd = NULL;
	m_iTimerID = 0;
	m_bBTNStatus = FALSE;
}

CTransparentButtonEx::~CTransparentButtonEx()
{
	if (NULL != m_pImg )
	{
		delete m_pImg;
		m_pImg = NULL;
	}
	if (NULL != m_pImgBG)
	{
		delete m_pImgBG;
		m_pImgBG = NULL;
	}

	if (m_pParentWnd)
	{
		m_pParentWnd = NULL;
	}
}


BEGIN_MESSAGE_MAP(CTransparentButtonEx, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_TIMER()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()



// CTransparentButtonEx 消息处理程序
 void CTransparentButtonEx::Init() //加载图片路径
{
	m_mapPath.clear();
	CString strPath = GetCurrentPath();
	strPath += L"\\SKIN\\WhiteBoard\\";
	m_mapPath[TYPE_DICOM] = strPath + L"Btn_Dicom.png";
	m_mapPath[TYPE_RECYCLE] = strPath + L"Recycle.png";  // 回收站
	m_mapPath[TYPE_SELECT] = strPath + L"Select.png";  // 选址
	m_mapPath[TYPE_PAGE] = strPath + L"Page.png";      //新建页
	m_mapPath[TYPE_FULLSCREEN] = strPath + L"FullSCreen.png";   //全屏
	m_mapPath[TYPE_BIGHT] = strPath + L"Curve.png";  //曲线
	m_mapPath[TYPE_LINE] = strPath + L"Beeline.png";  //直线
	m_mapPath[TYPE_ICOCURVE] = strPath + L"IcoCurve.png";  //图标曲线
	m_mapPath[TYPE_MAGCURVE] = strPath +L"MagicPen.png";
	m_mapPath[TYPE_IMAGE] = strPath + L"Image.png";   //图像
	m_mapPath[TYPE_VIDEO] = strPath + L"Video.png";   //视频
	m_mapPath[TYPE_DOCUMENT] = strPath + L"Document.png";  //文档
	m_mapPath[TYPE_FLASH] = strPath + L"flash.png";  //FLASH
	m_mapPath[TYPE_CAMERAHEAD] = strPath + L"CameraHead.png";  //摄像头
	m_mapPath[TYPE_MIDI] = strPath +L"Midi.png";
	m_mapPath[TYPE_ALLTOOL] = strPath + L"ToolboxBtn.png";  //工具箱
	m_mapPath[TYPE_RESOURCE] = strPath + L"ResourceList.png";   //资源库
	m_mapPath[TYPE_DEL] = strPath + L"Delete.png";
	m_mapPath[TYPE_PRE_PAGE] = strPath + L"PrePage.png";
	m_mapPath[TYPE_NEXT_PAGE] = strPath  + L"NextPage.png";
	m_mapPath[TYPE_T_H] = strPath + L"HorizontalText.png";   //水平文本
	m_mapPath[TYPE_T_V] = strPath + L"VerticalText.png";  //垂直文本
	m_mapPath[TYPE_REDO] = strPath + L"redo.png";
	m_mapPath[TYPE_UNDO] = strPath + L"undo.png";
	m_mapPath[TYPE_CANTOUCH] = strPath + L"CanTouch.png";
	m_mapPath[TYPE_RUBBER_BIG] = strPath + L"BigRubber.png";
	m_mapPath[TYPE_RUBBER_CURVE] = strPath + L"CurveRubber.png";
	m_mapPath[TYPE_RUBBER_OBJ] = strPath + L"ObjectRubber.png";
	m_mapPath[TYPE_TRIANGLE] = strPath + L"Triangle.png";
	m_mapPath[TYPE_RECTANGLE] = strPath + L"Rectangle.png";
	m_mapPath[TYPE_ROUNDNESS] = strPath + L"Roundness.png";
	m_mapPath[TYPE_RHOMB] = strPath + L"Rhombus.png";
	m_mapPath[TYPE_PENTAGON] = strPath + L"Pentagon.png";
	m_mapPath[TYPE_HEXAGON] = strPath + L"Hexagon.png";
	m_mapPath[TYPE_FIVEPOINTSTART] = strPath + L"Fivepointstart.png";
	m_mapPath[TYPE_ATRTRIANGLE] = strPath + L"AtrTriangle.png";
	m_mapPath[TYPE_PALLELOGAM] = strPath + L"Pallelogam.png";
	m_mapPath[TYPE_DYTRIANGLE] = strPath + L"Drtriangle.png";
	m_mapPath[TYPE_ZJTRIANGLE] = strPath + L"Zjtriangle.png";
	m_mapPath[TYPE_DYZJTRIANGLE] = strPath + L"Dyzjtriangle.png";
	m_mapPath[TYPE_TRAPEZIUM] = strPath + L"Trapezium.png";
	m_mapPath[TYPE_POLYGON] = strPath + L"Polygon.png";
	m_mapPath[TYPE_SCREENSHOTS] = strPath + L"CutScreen.png";	//截屏
	m_mapPath[TYPE_MAQN] = strPath + L"magnifier.png";  //放大镜
	m_mapPath[TYPE_SPOTLIGHT] = strPath + L"Spotlight.png";//聚光灯
	m_mapPath[TYPE_BAFFLEPLATE] = strPath + L"Baffle.png";	//挡板
	m_mapPath[TYPE_PROTRACTOR] = strPath + L"Protractor.png"; //量角器
	m_mapPath[TYPE_COMPASSES] = strPath + L"Compasses.png";   //圆规
	m_mapPath[TYPE_RULER] = strPath + L"Compasses.png";//直尺
	m_mapPath[TYPE_SCREENREC] = strPath + L"ScreenREC.png";	//屏幕录制	
	m_mapPath[TYPE_SAVE] = strPath + L"fileBK.png";  //文件
	m_mapPath[TYPE_CLOSE] = strPath + L"fileBK.png";
	m_mapPath[TYPE_PRINT] = strPath + L"fileBK.png";
	m_mapPath[TYPE_OPEN] = strPath + L"fileBK.png";
	m_mapPath[TYPE_COMBINE] = strPath + L"fileBK.png";
	m_mapPath[TYPE_RESAVE] = strPath + L"fileBK.png"; 
	m_mapPath[TYPE_DLG_FILE] = strPath + L"FileToolbar.png";//
	m_mapPath[TYPE_DLG_T] = strPath + L"Writing.png";//文字
	m_mapPath[TYPE_DLG_COLORPALETTE] = strPath + L"ColorPalette.png";//调色板
	m_mapPath[TYPE_FUNCTIONDLG] = strPath + L"FunctionDlg.png";//功能面板
    m_mapPath[TYPE_DDOPEN] = strPath + L"DrawDesktopOpen.png";
	m_mapPath[TYPE_DDFULLSCREEN] = strPath + L"DrawDesktopFullScreen.png";
	m_mapPath[TYPE_DDEFFECT] = strPath + L"DrawDesktopEffectSwitch.png";
	m_mapPath[TYPE_DDCTRL] = strPath + L"DrawDesktopCtrl.png";
	m_mapPath[TYPE_DDCLOSE] = strPath + L"DrawDesktopClose.png";		
	m_mapPath[TYPE_TOOLBAR_SHOW] = strPath + L"ToolbarShow.png";
	m_mapPath[TYPE_TOOLBAR_HIDE] = strPath + L"ToolbarHide.png";
	m_mapPath[TYPE_TOOLBOX_CLOSE] = strPath + L"ToolboxClose.png";
	m_mapPath[TYPE_BROADCAST] = strPath + L"CutScreen.png"; //广播 无图暂用
	m_mapPath[TYPE_BACKGROUND] = strPath + L"ToolbarBtnBG.png";
	m_mapPath[TYPE_CHILDBG] = strPath + L"ChildBtnBG.png";
	m_mapPath[TYPE_MEDIOVIDEOSCREEN] = strPath + L"ScreenRecord.png";
	m_mapPath[TYPE_FULLSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_AREASCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_WINDOWSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_PAUSESCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_STOPSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_PROPERTYSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_PATHSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_EAGLE_EYE] = strPath + L"EagleEye.png";
	m_mapPath[TYPE_DICOM] = strPath + L"Btn_Dicom.png";
	m_mapPath[TYPE_RECYCLE] = strPath + L"Recycle.png";  // 回收站
	m_mapPath[TYPE_SELECT] = strPath + L"Select.png";  // 选址
	m_mapPath[TYPE_PAGE] = strPath + L"Page.png";      //新建页
	m_mapPath[TYPE_FULLSCREEN] = strPath + L"FullSCreen.png";   //全屏
	m_mapPath[TYPE_BIGHT] = strPath + L"Curve.png";  //曲线
	m_mapPath[TYPE_LINE] = strPath + L"Beeline.png";  //直线
	m_mapPath[TYPE_ICOCURVE] = strPath + L"IcoCurve.png";  //图标曲线
	m_mapPath[TYPE_IMAGE] = strPath + L"Image.png";   //图像
	m_mapPath[TYPE_VIDEO] = strPath + L"Video.png";   //视频
	m_mapPath[TYPE_DOCUMENT] = strPath + L"Document.png";  //文档
	m_mapPath[TYPE_FLASH] = strPath + L"flash.png";  //FLASH
	m_mapPath[TYPE_CAMERAHEAD] = strPath + L"CameraHead.png";  //摄像头
	m_mapPath[TYPE_ALLTOOL] = strPath + L"ToolboxBtn.png";  //工具箱
	m_mapPath[TYPE_RESOURCE] = strPath + L"ResourceList.png";   //资源库
	m_mapPath[TYPE_DEL] = strPath + L"Delete.png";
	m_mapPath[TYPE_PRE_PAGE] = strPath + L"PrePage.png";
	m_mapPath[TYPE_NEXT_PAGE] = strPath  + L"NextPage.png";
	m_mapPath[TYPE_T_H] = strPath + L"HorizontalText.png";   //水平文本
	m_mapPath[TYPE_T_V] = strPath + L"VerticalText.png";  //垂直文本
	m_mapPath[TYPE_REDO] = strPath + L"redo.png";
	m_mapPath[TYPE_UNDO] = strPath + L"undo.png";
	m_mapPath[TYPE_CANTOUCH] = strPath + L"CanTouch.png";
	m_mapPath[TYPE_RUBBER_BIG] = strPath + L"BigRubber.png";
	m_mapPath[TYPE_RUBBER_CURVE] = strPath + L"CurveRubber.png";
	m_mapPath[TYPE_RUBBER_OBJ] = strPath + L"ObjectRubber.png";
	m_mapPath[TYPE_TRIANGLE] = strPath + L"Triangle.png";
	m_mapPath[TYPE_RECTANGLE] = strPath + L"Rectangle.png";
	m_mapPath[TYPE_ROUNDNESS] = strPath + L"Roundness.png";
	m_mapPath[TYPE_RHOMB] = strPath + L"Rhombus.png";
	/*m_mapPath[TYPE_PENTAGON] = strPath + L"Pentagon.png";*/
	m_mapPath[TYPE_HEXAGON] = strPath + L"Hexagon.png";
	m_mapPath[TYPE_FIVEPOINTSTART] = strPath + L"Fivepointstart.png";
	m_mapPath[TYPE_TRAPEZIUM] = strPath + L"Trapezium.png";
	m_mapPath[TYPE_POLYGON] = strPath + L"Polygon.png";
	m_mapPath[TYPE_SCREENSHOTS] = strPath + L"CutScreen.png";	//截屏
	m_mapPath[TYPE_MAQN] = strPath + L"magnifier.png";  //放大镜
	m_mapPath[TYPE_SPOTLIGHT] = strPath + L"Spotlight.png";//聚光灯
	m_mapPath[TYPE_BAFFLEPLATE] = strPath + L"Baffle.png";	//挡板
	m_mapPath[TYPE_PROTRACTOR] = strPath + L"Protractor.png"; //量角器
	m_mapPath[TYPE_COMPASSES] = strPath + L"Compasses.png";   //圆规
	m_mapPath[TYPE_RULER] = strPath + L"Compasses.png";//直尺
	m_mapPath[TYPE_SCREENREC] = strPath + L"ScreenREC.png";	//屏幕录制	
	m_mapPath[TYPE_SAVE] = strPath + L"fileBK.png";  //文件
	m_mapPath[TYPE_SAVE_PIC] = strPath + L"fileBK.png"; 
	m_mapPath[TYPE_CLOSE] = strPath + L"fileBK.png";
	m_mapPath[TYPE_PRINT] = strPath + L"fileBK.png";
	m_mapPath[TYPE_OPEN] = strPath + L"fileBK.png";
	m_mapPath[TYPE_COMBINE] = strPath + L"fileBK.png";
	m_mapPath[TYPE_RESAVE] = strPath + L"fileBK.png"; 
	m_mapPath[TYPE_DLG_FILE] = strPath + L"FileToolbar.png";//
	m_mapPath[TYPE_DLG_T] = strPath + L"Writing.png";//文字
	m_mapPath[TYPE_DLG_COLORPALETTE] = strPath + L"ColorPalette.png";//调色板
	m_mapPath[TYPE_FUNCTIONDLG] = strPath + L"FunctionDlg.png";//功能面板
    m_mapPath[TYPE_DDOPEN] = strPath + L"DrawDesktopOpen.png";
	m_mapPath[TYPE_DDFULLSCREEN] = strPath + L"DrawDesktopFullScreen.png";
	m_mapPath[TYPE_DDEFFECT] = strPath + L"DrawDesktopEffectSwitch.png";
	m_mapPath[TYPE_DDCTRL] = strPath + L"DrawDesktopCtrl.png";
	m_mapPath[TYPE_DDCLOSE] = strPath + L"DrawDesktopClose.png";		
	m_mapPath[TYPE_TOOLBAR_SHOW] = strPath + L"ToolbarShow.png";
	m_mapPath[TYPE_TOOLBAR_HIDE] = strPath + L"ToolbarHide.png";
	m_mapPath[TYPE_TOOLBOX_CLOSE] = strPath + L"ToolboxClose.png";
	m_mapPath[TYPE_BROADCAST] = strPath + L"CutScreen.png"; //广播 无图暂用
	m_mapPath[TYPE_BACKGROUND] = strPath + L"ToolbarBtnBG.png";
	m_mapPath[TYPE_CHILDBG] = strPath + L"ChildBtnBG.png";
	m_mapPath[TYPE_MEDIOVIDEOSCREEN] = strPath + L"ScreenRecord.png";
	m_mapPath[TYPE_FULLSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_AREASCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_WINDOWSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_PAUSESCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_STOPSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_PROPERTYSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_PATHSCREENRECORD] = strPath + L"ScreenRecordBtn.png";
	m_mapPath[TYPE_EAGLE_EYE] = strPath + L"EagleEye.png";

	m_mapPath[TYPE_DLG_LINETYPE] = strPath + L"LineType.png";
	m_mapPath[TYPE_DLG_RUBBER] = strPath + L"Rubber.png";
	m_mapPath[TYPE_DLG_GRAPHICS] = strPath + L"Graphics.png";
	m_mapPath[TYPE_DLG_RESOURCEFILE] = strPath + L"ResourceFile.png";
	m_mapPath[TYPE_DLG_TOOL] = strPath + L"Tool.png";
	m_mapPath[TYPE_DLG_DESKTOPDRAW] = strPath + L"DrawDesktop.png";

	m_mapPath[TYPE_LINECOLORBUTTON] = strPath + L"ColorButtonNormal.png";
	m_mapPath[TYPE_LINECOLORBUTTONDOWN] = strPath + L"ColorButtonDown.png";
	m_mapPath[TYPE_SOLIDCOLORBUTTONDOWN] = strPath + L"ColorButtonDown.png";
	m_mapPath[TYPE_SOLIDCOLORBUTTON] = strPath + L"ColorButtonNormal.png";
	//单机版时的联网和关机按钮
	m_mapPath[TYPE_SINGLE_NETWORK] = strPath + L"SingleNetwork.png";
	m_mapPath[TYPE_SINGLE_LOCAL] = strPath + L"SingleLocal.png";
	m_mapPath[TYPE_SINGLE_SHOTDOWN] = strPath + L"SingleShotdown.png";
	m_mapPath[TYPE_SINGLE_QUIT] = strPath + L"SingleQuit.png";
	m_mapPath[TYPE_DXF] = strPath + L"Btn_DXF.png";
	
}
void CTransparentButtonEx::SetButtonImage(CString filename)
{
	if(m_pImg)
	{
		delete m_pImg;
		m_pImg = NULL;
	}
	//wchar_t strFilename[MAX_PATH];
	CString picturePath = filename;
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImg = new Bitmap(picturePath);
	ASSERT(m_pImg);
}
void CTransparentButtonEx::SetType(eType enumType,bool isboxbtn/*=false*/)
{
	m_btnType = enumType;
	m_strPath = m_mapPath[enumType];
	SetButtonImage(m_strPath);
	if(m_pImgBG)
	{
		delete m_pImgBG;
		m_pImgBG = NULL;
	}
	
	if ( IDC_BTNONE == m_btnID ||IDC_BTNTWO == m_btnID ||IDC_BTNTHREE == m_btnID||IDC_BTNFOUR == m_btnID ||IDC_BTNFIVE == m_btnID
		||IDC_BTNSIX == m_btnID ||IDC_BTNSEVEN == m_btnID ||IDC_BTNEIGHT == m_btnID ||IDC_BTNNINE == m_btnID ||IDC_BTNTEN == m_btnID
		|| IDC_BTNELEVEN == m_btnID || TYPE_TOOLBAR_SHOW == enumType || TYPE_TOOLBAR_HIDE == enumType //控制工具条对话框中的显示隐藏按钮，因为是静态按钮所以不能通过ID来辨别
		)
	
	{
		m_strPath = m_mapPath[TYPE_BACKGROUND]; //主工具条按钮背景
	} 
	else if (TYPE_FULLSCREENRECORD == enumType || TYPE_AREASCREENRECORD == enumType || TYPE_WINDOWSCREENRECORD == enumType 
		|| TYPE_PAUSESCREENRECORD == enumType || TYPE_STOPSCREENRECORD == enumType ||TYPE_PROPERTYSCREENRECORD == enumType 
		|| TYPE_PATHSCREENRECORD == enumType )
	{
		m_strPath = m_mapPath[TYPE_MEDIOVIDEOSCREEN]; //屏幕录制按钮背景
	} 
	else
	{
		m_strPath = m_mapPath[TYPE_CHILDBG]; //其他按钮背景
	}
	if (isboxbtn)
	{
		m_strPath = m_mapPath[TYPE_CHILDBG];
	}
	if (TYPE_SOLIDCOLORBUTTON == enumType || TYPE_LINECOLORBUTTON == enumType)//
	{
		m_strPath = m_mapPath[TYPE_SOLIDCOLORBUTTON]; //其他按钮背景
	}
	if (TYPE_LINECOLORBUTTONDOWN == enumType)//
	{
		m_strPath = m_mapPath[TYPE_LINECOLORBUTTONDOWN]; //按下按钮背景
	}
	if (TYPE_SOLIDCOLORBUTTONDOWN == enumType)//
	{
		m_strPath = m_mapPath[TYPE_SOLIDCOLORBUTTONDOWN]; //按下按钮背景
	}
	//wchar_t strFilename[MAX_PATH];
	CString picturePath = m_strPath;
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImgBG = new Bitmap(picturePath);
	ASSERT(m_pImg);
    ShowWindow(SW_SHOW);

}
eType CTransparentButtonEx::GetType()
{
	return m_btnType;
}
void CTransparentButtonEx::OnLButtonDown(UINT nFlags, CPoint point)
{   
	if(g_bU7NewStyle)
	{
		SetTimer(TIMER_TOUCH_SHOW_TIP, 2000, NULL);
		if (CWBToolTip::GetIns())
		{
			CWBToolTip::GetIns()->Restore();
		}
		CButton::OnLButtonDown(nFlags, point);
		return;
	}
	if (WB_MULTIVER == g_WBType && g_bAvconLock)
	{
		return;
	}
	else
	{
		if (g_bAvconLock &&TYPE_SINGLE_NETWORK != GetType()  && TYPE_SINGLE_LOCAL != GetType()  && TYPE_SINGLE_SHOTDOWN != GetType()  && TYPE_SINGLE_QUIT != GetType() && TYPE_ALLTOOL != GetType()&&TYPE_TOOLBOX_CLOSE != GetType()) //如果会议中白板功能被锁定
		{
			return;
		}
	}
	if ((!g_bIsSupportMTTouch) && g_bNoSupportTouchInWB)//如果没有授权按钮就不响应触摸屏
	{
		if((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
		{
			return;
		}
	}
	
	if (TYPE_LINECOLORBUTTON == GetType())
	{
		SetType(TYPE_LINECOLORBUTTONDOWN);
	}
	if (TYPE_SOLIDCOLORBUTTON == GetType())
	{
		SetType(TYPE_SOLIDCOLORBUTTONDOWN);
	}
	SetTimer(TIMER_TOUCH_SHOW_TIP, 2000, NULL);
	if (CWBToolTip::GetIns())
	{
		CWBToolTip::GetIns()->Restore();
	}
	
	CButton::OnLButtonDown(nFlags, point);
}

void CTransparentButtonEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	
	if(g_bU7NewStyle)
	{
		KillTimer(TIMER_TOUCH_SHOW_TIP);
		CButton::OnLButtonUp(nFlags, point);
		return;
	}
	CRect BTNRect;
	if (WB_MULTIVER == g_WBType && g_bAvconLock)
	{
		return;
	}
	else
	{
		if (g_bAvconLock &&TYPE_SINGLE_NETWORK != GetType()  && TYPE_SINGLE_LOCAL != GetType()  && TYPE_SINGLE_SHOTDOWN != GetType()  && TYPE_SINGLE_QUIT != GetType() && TYPE_ALLTOOL != GetType()&&TYPE_TOOLBOX_CLOSE != GetType()) //如果会议中白板功能被锁定
		{
			return;
		}
	}
	if (TYPE_LINECOLORBUTTONDOWN == GetType())
	{
		SetType(TYPE_LINECOLORBUTTON);
	}
	if (TYPE_SOLIDCOLORBUTTONDOWN == GetType())
	{
		SetType(TYPE_SOLIDCOLORBUTTON);
	}
	KillTimer(TIMER_TOUCH_SHOW_TIP);
	GetWindowRect(&BTNRect);
	
	CWBToolbarDlg  *pToolbar = NULL;
	CWBToolboxDlg     *pToolbox = NULL;
	CWnd* upWnd = NULL;
	CWnd *pUpParent = NULL;
	CString downBtnText(_T("这是DOWN按钮的标题")),upBtnText(_T("这是UP按钮的标题"));
    CTransparentButtonEx *upBtn = NULL;  //拖拽移动按钮时UP的按钮指针
    m_upPoint = point;
    ClientToScreen(&m_upPoint);  //将UP的点坐标转换为屏幕坐标
	if (!BTNRect.PtInRect(m_upPoint))
	{
		((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg()->CloseAllChildToolBox();

	}
	upWnd = WindowFromPoint(m_upPoint);
	if (!upWnd)
	{
		return;
	}
	pUpParent = upWnd->GetParent();
	if ( !pUpParent)
	{
		return;
	}
	//如果弹起的窗口是工具条或是工具箱
	if ( (CWBToolbarDlg::g_toolBarHWND == pUpParent->m_hWnd) || (CWBToolboxDlg::g_toolBoxHWND == pUpParent->m_hWnd) )
	{	
		upBtn =(CTransparentButtonEx *)WindowFromPoint(m_upPoint);
		if (upBtn == NULL )
		{
			return;
		}
		UINT nID = upBtn->GetDlgCtrlID();
		if (nID == IDC_TOOLBOXSTATIC)
		{
			return;
		}
		GetWindowText(downBtnText);
		upBtn->GetWindowText(upBtnText);
		//如果是下面的按就钮什么都不做
		if((this->m_hWnd == upBtn->m_hWnd) //DOWN和UP是同一个按钮
			||(m_btnID == IDC_BTNTEN) ||(upBtn->m_btnID == IDC_BTNTEN) //工具箱按钮
			||(m_btnID == IDC_BTNNINE) ||(upBtn->m_btnID == IDC_BTNNINE) //资源目录按钮
			||(m_btnID == IDC_BTNONE) ||(upBtn->m_btnID == IDC_BTNONE) //选择按钮
			||(m_btnID == IDC_BTNELEVEN) ||(upBtn->m_btnID == IDC_BTNELEVEN) //单机的联网和本地按钮
			||(m_btnID == IDC_BTN_CLOSE) ||(upBtn->m_btnID == IDC_BTN_CLOSE) //工具箱中关闭按钮
			
			/*||(m_btnID == IDC_BTNEIGHT) ||(upBtn->m_btnID == IDC_BTNEIGHT)*/ //调色板按钮
			) 
		{

		}
		else
		{
			if (NULL ==m_pParentWnd || !IsWindow(m_pParentWnd->m_hWnd))
			{
				return;
			}
			//如果都是工具条中的按钮
			if( (m_pParentWnd->m_hWnd == CWBToolbarDlg::g_toolBarHWND) && (m_pParentWnd->m_hWnd == pUpParent->m_hWnd)  )
			{
			   CRect downBtnRt,upBtnRt,barRt;
			   int downPtRelativetoBarY,upPtRelativetoBarY;
			   pToolbar = (CWBToolbarDlg *)m_pParentWnd;
			   pToolbar->GetWindowRect(&barRt);
			   GetWindowRect(&downBtnRt);
			   upBtn->GetWindowRect(&upBtnRt);
			   downPtRelativetoBarY = downBtnRt.top - barRt.top;
			   upPtRelativetoBarY = upBtnRt.top - barRt.top;
			   pToolbar->CloseAllChildToolBox();
			   pToolbar->ChangButton(this,downPtRelativetoBarY,upBtn,upPtRelativetoBarY);
		  
			}
				//如果都是工具箱中的按钮
			else if(  (m_pParentWnd->m_hWnd == CWBToolboxDlg::g_toolBoxHWND) && (m_pParentWnd->m_hWnd == pUpParent->m_hWnd) )
			{
			   CRect downBtnRt,upBtnRt,boxRt;
			   int downPtRelativetoBarX,downPtRelativetoBarY,upPtRelativetoBarX,upPtRelativetoBarY;
			   pToolbox = (CWBToolboxDlg *)m_pParentWnd;
			   pToolbox->GetWindowRect(&boxRt);
			  // pToolbar = (CWBToolbarDlg *)pToolbox->GetParentWnd();
			   GetWindowRect(&downBtnRt);
			   upBtn->GetWindowRect(&upBtnRt);
			   downPtRelativetoBarX = downBtnRt.left - boxRt.left;
			   downPtRelativetoBarY = downBtnRt.top - boxRt.top;
			   upPtRelativetoBarX = upBtnRt.left - boxRt.left;
			   upPtRelativetoBarY = upBtnRt.top - boxRt.top;
			   pToolbox->ChangButton(this,downPtRelativetoBarX,downPtRelativetoBarY,upBtn,upPtRelativetoBarX,upPtRelativetoBarY);
		   //  pToolbar->CloseAllChildToolBox();
			}
			//如果DOWN是工具箱，UP是工具条
			else if((m_pParentWnd->m_hWnd == CWBToolboxDlg::g_toolBoxHWND) && ( pUpParent->m_hWnd == CWBToolbarDlg::g_toolBarHWND))
			{
				XAutoLock lock(m_ButtonUpWriteLock);
				CRect downBtnRt,upBtnRt,boxRt,barRt;
				int downPtRelativetoBoxX,downPtRelativetoBoxY,upPtRelativetoBarX,upPtRelativetoBarY;
				eType upBtnType;
				pToolbox = (CWBToolboxDlg *)m_pParentWnd;//如果DOWN是工具箱
				pToolbox->GetWindowRect(&boxRt);
				pToolbar = (CWBToolbarDlg *)(upBtn->m_pParentWnd);//UP是工具条
				pToolbar->GetWindowRect(&barRt);
				GetWindowRect(&downBtnRt); //DOWN按钮
				upBtn->GetWindowRect(&upBtnRt);//UP按钮
				downPtRelativetoBoxX = downBtnRt.left - boxRt.left;
				downPtRelativetoBoxY = downBtnRt.top - boxRt.top;
				upPtRelativetoBarX = upBtnRt.left - barRt.left;
				upPtRelativetoBarY = upBtnRt.top - barRt.top;
				upBtnType = upBtn->GetType();
				CString strSelection = _T("ButtonType");
				CString strValue = _T("");
				CString strText = _T("");
				//先执行UP的按钮
				//pToolbar->DelAndNewBtn(upBtn,upPtRelativetoBarY,m_btnType,downBtnText);//up的传down的类型和标题
				upBtn->SetWindowPos(&wndTop,WB_WIDTH_SPACE,upPtRelativetoBarY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
				upBtn->SetType(m_btnType);
				strText = pToolbar->GetCaptureFromType(m_btnType);
				upBtn->SetWindowText(strText);
				strValue.Format(_T("%d"),m_btnType);
				WriteWBConfigPathINI(strSelection,upBtn->GetName(),strValue);

				//pToolbox->DelAndNewBtn(this,downPtRelativetoBoxX,downPtRelativetoBoxY,upBtnType,upBtnText);    //down的up传的类型和标题  
				this->SetWindowPos(&wndTop,downPtRelativetoBoxX,downPtRelativetoBoxY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
				this->SetType(upBtnType,true);				
				strText = pToolbox->GetCaptureFromType(upBtnType);
				this->SetWindowText(strText);
				
				strValue.Format(_T("%d"),upBtnType);
				WriteWBConfigPathINI(strSelection,this->GetName(),strValue);
				Invalidate();
			}
			//如果DOWN是工具条，UP是工具箱
			else if((m_pParentWnd->m_hWnd == CWBToolbarDlg::g_toolBarHWND) && ( pUpParent->m_hWnd == CWBToolboxDlg::g_toolBoxHWND))
			{
				XAutoLock lock(m_ButtonUpWriteLock);
				CRect downBtnRt,upBtnRt,boxRt,barRt;
				int downPtRelativetoBoxX,downPtRelativetoBoxY,upPtRelativetoBarX,upPtRelativetoBarY;
				eType upBtnType;
				pToolbar = (CWBToolbarDlg *)m_pParentWnd;//如果DOWN是工具条
				pToolbar->GetWindowRect(&barRt);
				pToolbox = ( CWBToolboxDlg*)(upBtn->m_pParentWnd);//UP是工具箱
				pToolbox->GetWindowRect(&boxRt);
				GetWindowRect(&downBtnRt); //DOWN按钮
				upBtn->GetWindowRect(&upBtnRt);//UP按钮
				downPtRelativetoBoxX = downBtnRt.left - barRt.left;
				downPtRelativetoBoxY = downBtnRt.top - barRt.top;
				upPtRelativetoBarX = upBtnRt.left - boxRt.left;
				upPtRelativetoBarY = upBtnRt.top - boxRt.top;
				upBtnType = upBtn->GetType();


				CString strSelection = _T("ButtonType");
				CString strValue = _T("");
				CString strText = _T("");
				//先执行UP的按钮			
				upBtn->SetWindowPos(&wndTop,upPtRelativetoBarX,upPtRelativetoBarY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
				upBtn->SetType(m_btnType,true);				
				strText = pToolbox->GetCaptureFromType(m_btnType);
				upBtn->SetWindowText(strText);
				strValue.Format(_T("%d"),m_btnType);
				WriteWBConfigPathINI(strSelection,upBtn->GetName(),strValue);

				this->SetWindowPos(&wndTop,WB_WIDTH_SPACE,downPtRelativetoBoxY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
				this->SetType(upBtnType);
				strText = pToolbar->GetCaptureFromType(upBtnType);
				this->SetWindowText(strText);
				strValue.Format(_T("%d"),upBtnType);
				WriteWBConfigPathINI(strSelection,this->GetName(),strValue);
				Invalidate();

			}
		}
		
	}
	
	CButton::OnLButtonUp(nFlags, point);
	
}

void CTransparentButtonEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if (WB_MULTIVER == g_WBType && g_bAvconLock)
	{
		return;
	}
	else
	{
		if (g_bAvconLock &&TYPE_SINGLE_NETWORK != GetType()  && TYPE_SINGLE_LOCAL != GetType()  && TYPE_SINGLE_SHOTDOWN != GetType()  && TYPE_SINGLE_QUIT != GetType() && TYPE_ALLTOOL != GetType()&&TYPE_TOOLBOX_CLOSE != GetType()) //如果会议中白板功能被锁定
		{
			return;
		}
	}
	if(!m_bMouseIn)
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 200;
		tme.hwndTrack = m_hWnd;
		m_bMouseIn = TrackMouseEvent(&tme);
	}

	CButton::OnMouseMove(nFlags, point);
}

LRESULT  CTransparentButtonEx::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (TYPE_LINECOLORBUTTON == GetType() || TYPE_SOLIDCOLORBUTTON == GetType())
	{
		return 0;
	}  
	CRect rect;
	GetClientRect(&rect);
	CClientDC dc(this);
	Graphics graph(dc.GetSafeHdc()); 
	if (m_pImgBG)
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());

		graph.DrawImage(m_pImgBG, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片

	}
	if (m_pImg)
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());
		if (g_GraphicsbuttonType == m_btnType || g_LineTypebuttonType == m_btnType || g_RubberbuttonType == m_btnType)
		{
			graph.DrawImage(m_pImg, Rt, rect.Width()*1, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
		}
		else
		{
			if (m_bBTNStatus)
			{
				graph.DrawImage(m_pImg, Rt, rect.Width()*2, 0, rect.Width(), rect.Height(), UnitPixel);
			}
			else
			{
				graph.DrawImage(m_pImg, Rt, rect.Width()*0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
			}
		}
		
	}
	if (TYPE_DLG_COLORPALETTE == GetType())
	{
		SolidBrush brush(Color(255, GetRValue(g_clrWBCurObjColor), GetGValue(g_clrWBCurObjColor), GetBValue(g_clrWBCurObjColor)));
		graph.FillRectangle(&brush,35,35,15,15);
	}
	//在按钮上写文本
	if (m_btnType == TYPE_PRE_PAGE||m_btnType == TYPE_NEXT_PAGE||m_btnType == TYPE_REDO ||m_btnType == TYPE_UNDO||m_btnType == TYPE_CANTOUCH)
	{
		HRGN rgn = CreateRoundRectRgn(0,0,rect.Width(),rect.Height(),5,5);
		SetWindowRgn(rgn,TRUE);
		DeleteObject(rgn);
	}
	
	
	rect.top = rect.top + (rect.Height() - 13)/2;
	dc.SetBkMode( TRANSPARENT ); //设置文本写的时候底色透明
	HFONT hFont;
	hFont = CreateFont(15, 6, 0, 0, FW_NORMAL, 0, 0, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,_T("宋体"));
    switch(m_btnType)
	{
	    case TYPE_OPEN:
		case TYPE_PRINT:
		case TYPE_CLOSE:
		case TYPE_COMBINE:
		case TYPE_RESAVE:
		case TYPE_SAVE:
		
		case TYPE_FULLSCREENRECORD:
		case TYPE_AREASCREENRECORD:
		case TYPE_WINDOWSCREENRECORD:
		case TYPE_PAUSESCREENRECORD:
		case TYPE_STOPSCREENRECORD:
		case TYPE_PROPERTYSCREENRECORD:
		case TYPE_PATHSCREENRECORD:
			{
				dc.SelectObject(hFont);
				dc.DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
				
			}
			break;
		case TYPE_SAVE_PIC:
			{
				dc.SelectObject(hFont);
				dc.DrawText(m_strName,&rect, DT_LEFT);//字数多就从左开始
			}
			break;
		default:
			break;
	}
    m_bMouseIn = FALSE;
	if (CWBToolTip::GetIns())
	{
		CWBToolTip::GetIns()->Restore();
	}
	DeleteObject(hFont);
	return 0;
}
LRESULT  CTransparentButtonEx::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	if (TYPE_LINECOLORBUTTON == GetType() || TYPE_SOLIDCOLORBUTTON == GetType())
	{
		return 0;
	}
	CRect rect;
	GetClientRect(&rect);
	CClientDC dc(this);
	Graphics graph(dc.GetSafeHdc());
	if (m_pImgBG)
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());

		graph.DrawImage(m_pImgBG, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片

	}
	if (m_pImg)
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pImg, Rt, rect.Width()*1, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
	}
	if (m_btnType == TYPE_PRE_PAGE||m_btnType == TYPE_NEXT_PAGE||m_btnType == TYPE_REDO ||m_btnType == TYPE_UNDO||m_btnType == TYPE_CANTOUCH)
	{
		HRGN rgn = CreateRoundRectRgn(0,0,rect.Width(),rect.Height(),5,5);
		SetWindowRgn(rgn,TRUE);
		DeleteObject(rgn);
	}
	
	if (TYPE_DLG_COLORPALETTE == GetType())
	{
		SolidBrush brush(Color(255, GetRValue(g_clrWBCurObjColor), GetGValue(g_clrWBCurObjColor), GetBValue(g_clrWBCurObjColor)));
		graph.FillRectangle(&brush,32,32,12,12);
	}	
	//在按钮上写文本	
	rect.top = rect.top + (rect.Height() - 13)/2;
	dc.SetBkMode( TRANSPARENT );
	HFONT hFont;
	hFont = CreateFont(15, 6, 0, 0, FW_NORMAL, 0, 0, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,_T("宋体"));
	switch(m_btnType)
	{
	case TYPE_OPEN:
	case TYPE_PRINT:
	case TYPE_CLOSE:
	case TYPE_COMBINE:
	case TYPE_RESAVE:
	case TYPE_SAVE:	
	case TYPE_FULLSCREENRECORD:
	case TYPE_AREASCREENRECORD:
	case TYPE_WINDOWSCREENRECORD:
	case TYPE_PAUSESCREENRECORD:
	case TYPE_STOPSCREENRECORD:
	case TYPE_PROPERTYSCREENRECORD:
	case TYPE_PATHSCREENRECORD:
		{
			dc.SelectObject(hFont);
			dc.DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
		
		}
		break;
	case TYPE_SAVE_PIC:
		{
			dc.SelectObject(hFont);
			dc.DrawText(m_strName,&rect, DT_LEFT);//字数多就从左开始
		}
		break;
	default:
		break;

	}

	CString str;
	GetWindowText(str);
	CWBToolTip::GetIns()->SetText(str);
	RECT rcDlg = { 0 };
	::GetWindowRect(this->GetSafeHwnd(), &rcDlg);
	CWBToolTip::GetIns()->SetPosition(CPoint(rcDlg.right, rcDlg.bottom));
	CWBToolTip::GetIns()->Display();

	DeleteObject(hFont);
    return 0;
	//CButton::OnMouseHover(nFlags, point);
}

BOOL CTransparentButtonEx::OnEraseBkgnd(CDC* pDC)
{	
	return true;

	return CButton::OnEraseBkgnd(pDC);
}

void CTransparentButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rect;
	GetClientRect(&rect);
	Graphics graph(lpDrawItemStruct->hDC);
	CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (m_pImgBG) //按钮背景图
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());

		graph.DrawImage(m_pImgBG, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片

	}
	if (m_pImg)
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());
		if (IsWindowEnabled())//判断按钮是否是在可视状态
		{
			if (g_GraphicsbuttonType == m_btnType || g_LineTypebuttonType == m_btnType || g_RubberbuttonType == m_btnType)
			{
				graph.DrawImage(m_pImg, Rt, rect.Width()*1, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
			}
			
			else
			{
				if (m_bBTNStatus)
				{
					graph.DrawImage(m_pImg, Rt, rect.Width()*2, 0, rect.Width(), rect.Height(), UnitPixel);
				}
				else
				{
					graph.DrawImage(m_pImg, Rt, rect.Width()*0, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
				}
			}
		} 
		else
		{
			graph.DrawImage(m_pImg, Rt, rect.Width()*3, 0, rect.Width(), rect.Height(), UnitPixel);//按照模式中绘制按钮图片
		}
						
	}

	if (m_btnType == TYPE_PRE_PAGE||m_btnType == TYPE_NEXT_PAGE||m_btnType == TYPE_REDO ||m_btnType == TYPE_UNDO||m_btnType == TYPE_CANTOUCH)
	{
		HRGN rgn = CreateRoundRectRgn(0,0,rect.Width(),rect.Height(),5,5);
		SetWindowRgn(rgn,TRUE);
		DeleteObject(rgn);
	}
	


// 	HRGN rgnbtn;
// 	rgnbtn = CreateRoundRectRgn(0,0,rect.Width(),rect.Height(),5,5);
// 	SetWindowRgn(rgnbtn,TRUE);
// 	::DeleteObject(rgnbtn);
	switch(GetType())
	{
	case TYPE_LINECOLORBUTTON:
	case TYPE_LINECOLORBUTTONDOWN:
		{
			SolidBrush brush(Color(255, GetRValue(g_clrWBCurObjColor), GetGValue(g_clrWBCurObjColor), GetBValue(g_clrWBCurObjColor)));
			graph.FillRectangle(&brush,5,5,130,20);
		}
		break;
	case TYPE_SOLIDCOLORBUTTONDOWN:
	case TYPE_SOLIDCOLORBUTTON:
		{
			SolidBrush brush(Color(255, GetRValue(g_clrSolid), GetGValue(g_clrSolid), GetBValue(g_clrSolid)));
			graph.FillRectangle(&brush,5,5,130,20);
		}
		break;
	case TYPE_DLG_COLORPALETTE:
		{
			SolidBrush brush(Color(255, GetRValue(g_clrWBCurObjColor), GetGValue(g_clrWBCurObjColor), GetBValue(g_clrWBCurObjColor)));
			graph.FillRectangle(&brush,35,35,15,15);
		}
		break;
	default:
		break;
	}

	//在按钮上写文本

 	rect.top = rect.top + (rect.Height() - 13)/2;
	pDc->SetBkMode( TRANSPARENT );
	HFONT hFont;
	hFont = CreateFont(15, 6, 0, 0, FW_NORMAL, 0, 0, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,_T("宋体"));
	switch(m_btnType)
	{

	case TYPE_OPEN:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect,DT_CENTER);
			
		}
		break;
	case TYPE_PRINT:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect,DT_CENTER);
			
		}
		break;
	case TYPE_CLOSE:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect,DT_CENTER);
			
		}
		break;
	case TYPE_COMBINE:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect,DT_CENTER);
			
		}
		break;
	case TYPE_RESAVE:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect,DT_CENTER);
			
		}
		break;
	case TYPE_SAVE:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect,DT_CENTER);
			
		}
		break;
	case TYPE_SAVE_PIC:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect,DT_LEFT); //字数多就从左开始

		}
		break;
	case TYPE_FULLSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
			
		}
		break;
	case TYPE_AREASCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
			
		}
		break;
	case TYPE_WINDOWSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
			
		}
		break;
	case TYPE_PAUSESCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
			
		}
		break;
	case TYPE_STOPSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
			
		}
		break;
	case TYPE_PROPERTYSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
			
		}
		break;
	case TYPE_PATHSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//将文本画在按钮的中央
			
		}
		break;
	default:
		break;
	}
	DeleteObject(hFont);
}
BOOL CTransparentButtonEx::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	m_pParentWnd = pParentWnd;
	m_btnID = nID;
	return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}
void CTransparentButtonEx::SetParentWnd(CWnd* pWnd)
{
	m_pParentWnd = pWnd;
}
void CTransparentButtonEx::SetName(CString strName)
{
	m_strName = strName;
}
CString CTransparentButtonEx::GetName()
{
	return m_strName;
}

afx_msg void CTransparentButtonEx::OnTimer(UINT nIDEvent)
{
	RECT rcDlg = { 0 };
	CString str;
	switch (nIDEvent)
	{
	case TIMER_TOUCH_SHOW_TIP:
		if (CWBToolTip::GetIns())
		{
			GetWindowText(str);
			CWBToolTip::GetIns()->SetText(str);
			::GetWindowRect(this->GetSafeHwnd(), &rcDlg);
			CWBToolTip::GetIns()->SetPosition(CPoint(rcDlg.right, rcDlg.bottom));
			CWBToolTip::GetIns()->Display();
		}
	
		break;
	default:
		break;
	}

	KillTimer(nIDEvent);
}

BOOL CTransparentButtonEx::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		return TRUE;
	}
	return CButton::PreTranslateMessage(pMsg);
}

void CTransparentButtonEx::SetButtonStatus( BOOL bStatus, eType ntype /*= TYPE_NONE*/ )
{
	m_bBTNStatus = bStatus;
	
	switch(ntype)
	{
	case TYPE_BIGHT:
		m_strPath = m_mapPath[TYPE_BIGHT];
		SetButtonImage(m_strPath);
		break;
	case TYPE_LINE:
		m_strPath = m_mapPath[TYPE_LINE];
		SetButtonImage(m_strPath);
		break;
	case TYPE_POLYGON:
		m_strPath = m_mapPath[TYPE_POLYGON];
		SetButtonImage(m_strPath);
		break;
	case TYPE_RECTANGLE:
		m_strPath = m_mapPath[TYPE_RECTANGLE];
		SetButtonImage(m_strPath);
		break;
	case TYPE_RHOMB:
		m_strPath = m_mapPath[TYPE_RHOMB];
		SetButtonImage(m_strPath);
		break;
	case TYPE_PENTAGON:
		m_strPath = m_mapPath[TYPE_PENTAGON];
		SetButtonImage(m_strPath);
		break;
	case TYPE_HEXAGON:
		m_strPath = m_mapPath[TYPE_HEXAGON];
		SetButtonImage(m_strPath);
		break;
	case TYPE_FIVEPOINTSTART:
		m_strPath = m_mapPath[TYPE_FIVEPOINTSTART];
		SetButtonImage(m_strPath);
		break;
	case TYPE_TRAPEZIUM:
		m_strPath = m_mapPath[TYPE_TRAPEZIUM];
		SetButtonImage(m_strPath);
		break;
	case TYPE_ROUNDNESS:
		m_strPath = m_mapPath[TYPE_ROUNDNESS];
		SetButtonImage(m_strPath);
		break;
	case TYPE_TRIANGLE:
		m_strPath = m_mapPath[TYPE_TRIANGLE];
		SetButtonImage(m_strPath);
		break;
	case TYPE_RUBBER_BIG:
		m_strPath = m_mapPath[TYPE_RUBBER_BIG];
		SetButtonImage(m_strPath);
		break;
	case TYPE_RUBBER_CURVE:
		m_strPath = m_mapPath[TYPE_RUBBER_CURVE];
		SetButtonImage(m_strPath);
		break;
	case TYPE_RUBBER_OBJ:
		m_strPath = m_mapPath[TYPE_RUBBER_OBJ];
		SetButtonImage(m_strPath);
		break;
	default:
		break;
	}
	

}



void CTransparentButtonEx::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_btnType == TYPE_TOOLBAR_SHOW||TYPE_TOOLBAR_HIDE == m_btnType)
	{
		CWBAboutDlg wbaboutdlg;
		wbaboutdlg.DoModal();
	}
	CButton::OnRButtonDblClk(nFlags, point);
}

void CTransparentButtonEx::OnRButtonUp( UINT nFlags, CPoint point )
{
	__super::OnRButtonUp(nFlags,point);
	if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg())
	{
		((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg()->CloseAllChildToolBox();
	}
	
}
