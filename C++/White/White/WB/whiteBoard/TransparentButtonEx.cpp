// TransparentButtonEx.cpp : ʵ���ļ�
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



// CTransparentButtonEx ��Ϣ�������
 void CTransparentButtonEx::Init() //����ͼƬ·��
{
	m_mapPath.clear();
	CString strPath = GetCurrentPath();
	strPath += L"\\SKIN\\WhiteBoard\\";
	m_mapPath[TYPE_DICOM] = strPath + L"Btn_Dicom.png";
	m_mapPath[TYPE_RECYCLE] = strPath + L"Recycle.png";  // ����վ
	m_mapPath[TYPE_SELECT] = strPath + L"Select.png";  // ѡַ
	m_mapPath[TYPE_PAGE] = strPath + L"Page.png";      //�½�ҳ
	m_mapPath[TYPE_FULLSCREEN] = strPath + L"FullSCreen.png";   //ȫ��
	m_mapPath[TYPE_BIGHT] = strPath + L"Curve.png";  //����
	m_mapPath[TYPE_LINE] = strPath + L"Beeline.png";  //ֱ��
	m_mapPath[TYPE_ICOCURVE] = strPath + L"IcoCurve.png";  //ͼ������
	m_mapPath[TYPE_MAGCURVE] = strPath +L"MagicPen.png";
	m_mapPath[TYPE_IMAGE] = strPath + L"Image.png";   //ͼ��
	m_mapPath[TYPE_VIDEO] = strPath + L"Video.png";   //��Ƶ
	m_mapPath[TYPE_DOCUMENT] = strPath + L"Document.png";  //�ĵ�
	m_mapPath[TYPE_FLASH] = strPath + L"flash.png";  //FLASH
	m_mapPath[TYPE_CAMERAHEAD] = strPath + L"CameraHead.png";  //����ͷ
	m_mapPath[TYPE_MIDI] = strPath +L"Midi.png";
	m_mapPath[TYPE_ALLTOOL] = strPath + L"ToolboxBtn.png";  //������
	m_mapPath[TYPE_RESOURCE] = strPath + L"ResourceList.png";   //��Դ��
	m_mapPath[TYPE_DEL] = strPath + L"Delete.png";
	m_mapPath[TYPE_PRE_PAGE] = strPath + L"PrePage.png";
	m_mapPath[TYPE_NEXT_PAGE] = strPath  + L"NextPage.png";
	m_mapPath[TYPE_T_H] = strPath + L"HorizontalText.png";   //ˮƽ�ı�
	m_mapPath[TYPE_T_V] = strPath + L"VerticalText.png";  //��ֱ�ı�
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
	m_mapPath[TYPE_SCREENSHOTS] = strPath + L"CutScreen.png";	//����
	m_mapPath[TYPE_MAQN] = strPath + L"magnifier.png";  //�Ŵ�
	m_mapPath[TYPE_SPOTLIGHT] = strPath + L"Spotlight.png";//�۹��
	m_mapPath[TYPE_BAFFLEPLATE] = strPath + L"Baffle.png";	//����
	m_mapPath[TYPE_PROTRACTOR] = strPath + L"Protractor.png"; //������
	m_mapPath[TYPE_COMPASSES] = strPath + L"Compasses.png";   //Բ��
	m_mapPath[TYPE_RULER] = strPath + L"Compasses.png";//ֱ��
	m_mapPath[TYPE_SCREENREC] = strPath + L"ScreenREC.png";	//��Ļ¼��	
	m_mapPath[TYPE_SAVE] = strPath + L"fileBK.png";  //�ļ�
	m_mapPath[TYPE_CLOSE] = strPath + L"fileBK.png";
	m_mapPath[TYPE_PRINT] = strPath + L"fileBK.png";
	m_mapPath[TYPE_OPEN] = strPath + L"fileBK.png";
	m_mapPath[TYPE_COMBINE] = strPath + L"fileBK.png";
	m_mapPath[TYPE_RESAVE] = strPath + L"fileBK.png"; 
	m_mapPath[TYPE_DLG_FILE] = strPath + L"FileToolbar.png";//
	m_mapPath[TYPE_DLG_T] = strPath + L"Writing.png";//����
	m_mapPath[TYPE_DLG_COLORPALETTE] = strPath + L"ColorPalette.png";//��ɫ��
	m_mapPath[TYPE_FUNCTIONDLG] = strPath + L"FunctionDlg.png";//�������
    m_mapPath[TYPE_DDOPEN] = strPath + L"DrawDesktopOpen.png";
	m_mapPath[TYPE_DDFULLSCREEN] = strPath + L"DrawDesktopFullScreen.png";
	m_mapPath[TYPE_DDEFFECT] = strPath + L"DrawDesktopEffectSwitch.png";
	m_mapPath[TYPE_DDCTRL] = strPath + L"DrawDesktopCtrl.png";
	m_mapPath[TYPE_DDCLOSE] = strPath + L"DrawDesktopClose.png";		
	m_mapPath[TYPE_TOOLBAR_SHOW] = strPath + L"ToolbarShow.png";
	m_mapPath[TYPE_TOOLBAR_HIDE] = strPath + L"ToolbarHide.png";
	m_mapPath[TYPE_TOOLBOX_CLOSE] = strPath + L"ToolboxClose.png";
	m_mapPath[TYPE_BROADCAST] = strPath + L"CutScreen.png"; //�㲥 ��ͼ����
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
	m_mapPath[TYPE_RECYCLE] = strPath + L"Recycle.png";  // ����վ
	m_mapPath[TYPE_SELECT] = strPath + L"Select.png";  // ѡַ
	m_mapPath[TYPE_PAGE] = strPath + L"Page.png";      //�½�ҳ
	m_mapPath[TYPE_FULLSCREEN] = strPath + L"FullSCreen.png";   //ȫ��
	m_mapPath[TYPE_BIGHT] = strPath + L"Curve.png";  //����
	m_mapPath[TYPE_LINE] = strPath + L"Beeline.png";  //ֱ��
	m_mapPath[TYPE_ICOCURVE] = strPath + L"IcoCurve.png";  //ͼ������
	m_mapPath[TYPE_IMAGE] = strPath + L"Image.png";   //ͼ��
	m_mapPath[TYPE_VIDEO] = strPath + L"Video.png";   //��Ƶ
	m_mapPath[TYPE_DOCUMENT] = strPath + L"Document.png";  //�ĵ�
	m_mapPath[TYPE_FLASH] = strPath + L"flash.png";  //FLASH
	m_mapPath[TYPE_CAMERAHEAD] = strPath + L"CameraHead.png";  //����ͷ
	m_mapPath[TYPE_ALLTOOL] = strPath + L"ToolboxBtn.png";  //������
	m_mapPath[TYPE_RESOURCE] = strPath + L"ResourceList.png";   //��Դ��
	m_mapPath[TYPE_DEL] = strPath + L"Delete.png";
	m_mapPath[TYPE_PRE_PAGE] = strPath + L"PrePage.png";
	m_mapPath[TYPE_NEXT_PAGE] = strPath  + L"NextPage.png";
	m_mapPath[TYPE_T_H] = strPath + L"HorizontalText.png";   //ˮƽ�ı�
	m_mapPath[TYPE_T_V] = strPath + L"VerticalText.png";  //��ֱ�ı�
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
	m_mapPath[TYPE_SCREENSHOTS] = strPath + L"CutScreen.png";	//����
	m_mapPath[TYPE_MAQN] = strPath + L"magnifier.png";  //�Ŵ�
	m_mapPath[TYPE_SPOTLIGHT] = strPath + L"Spotlight.png";//�۹��
	m_mapPath[TYPE_BAFFLEPLATE] = strPath + L"Baffle.png";	//����
	m_mapPath[TYPE_PROTRACTOR] = strPath + L"Protractor.png"; //������
	m_mapPath[TYPE_COMPASSES] = strPath + L"Compasses.png";   //Բ��
	m_mapPath[TYPE_RULER] = strPath + L"Compasses.png";//ֱ��
	m_mapPath[TYPE_SCREENREC] = strPath + L"ScreenREC.png";	//��Ļ¼��	
	m_mapPath[TYPE_SAVE] = strPath + L"fileBK.png";  //�ļ�
	m_mapPath[TYPE_SAVE_PIC] = strPath + L"fileBK.png"; 
	m_mapPath[TYPE_CLOSE] = strPath + L"fileBK.png";
	m_mapPath[TYPE_PRINT] = strPath + L"fileBK.png";
	m_mapPath[TYPE_OPEN] = strPath + L"fileBK.png";
	m_mapPath[TYPE_COMBINE] = strPath + L"fileBK.png";
	m_mapPath[TYPE_RESAVE] = strPath + L"fileBK.png"; 
	m_mapPath[TYPE_DLG_FILE] = strPath + L"FileToolbar.png";//
	m_mapPath[TYPE_DLG_T] = strPath + L"Writing.png";//����
	m_mapPath[TYPE_DLG_COLORPALETTE] = strPath + L"ColorPalette.png";//��ɫ��
	m_mapPath[TYPE_FUNCTIONDLG] = strPath + L"FunctionDlg.png";//�������
    m_mapPath[TYPE_DDOPEN] = strPath + L"DrawDesktopOpen.png";
	m_mapPath[TYPE_DDFULLSCREEN] = strPath + L"DrawDesktopFullScreen.png";
	m_mapPath[TYPE_DDEFFECT] = strPath + L"DrawDesktopEffectSwitch.png";
	m_mapPath[TYPE_DDCTRL] = strPath + L"DrawDesktopCtrl.png";
	m_mapPath[TYPE_DDCLOSE] = strPath + L"DrawDesktopClose.png";		
	m_mapPath[TYPE_TOOLBAR_SHOW] = strPath + L"ToolbarShow.png";
	m_mapPath[TYPE_TOOLBAR_HIDE] = strPath + L"ToolbarHide.png";
	m_mapPath[TYPE_TOOLBOX_CLOSE] = strPath + L"ToolboxClose.png";
	m_mapPath[TYPE_BROADCAST] = strPath + L"CutScreen.png"; //�㲥 ��ͼ����
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
	//������ʱ�������͹ػ���ť
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
		|| IDC_BTNELEVEN == m_btnID || TYPE_TOOLBAR_SHOW == enumType || TYPE_TOOLBAR_HIDE == enumType //���ƹ������Ի����е���ʾ���ذ�ť����Ϊ�Ǿ�̬��ť���Բ���ͨ��ID�����
		)
	
	{
		m_strPath = m_mapPath[TYPE_BACKGROUND]; //����������ť����
	} 
	else if (TYPE_FULLSCREENRECORD == enumType || TYPE_AREASCREENRECORD == enumType || TYPE_WINDOWSCREENRECORD == enumType 
		|| TYPE_PAUSESCREENRECORD == enumType || TYPE_STOPSCREENRECORD == enumType ||TYPE_PROPERTYSCREENRECORD == enumType 
		|| TYPE_PATHSCREENRECORD == enumType )
	{
		m_strPath = m_mapPath[TYPE_MEDIOVIDEOSCREEN]; //��Ļ¼�ư�ť����
	} 
	else
	{
		m_strPath = m_mapPath[TYPE_CHILDBG]; //������ť����
	}
	if (isboxbtn)
	{
		m_strPath = m_mapPath[TYPE_CHILDBG];
	}
	if (TYPE_SOLIDCOLORBUTTON == enumType || TYPE_LINECOLORBUTTON == enumType)//
	{
		m_strPath = m_mapPath[TYPE_SOLIDCOLORBUTTON]; //������ť����
	}
	if (TYPE_LINECOLORBUTTONDOWN == enumType)//
	{
		m_strPath = m_mapPath[TYPE_LINECOLORBUTTONDOWN]; //���°�ť����
	}
	if (TYPE_SOLIDCOLORBUTTONDOWN == enumType)//
	{
		m_strPath = m_mapPath[TYPE_SOLIDCOLORBUTTONDOWN]; //���°�ť����
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
		if (g_bAvconLock &&TYPE_SINGLE_NETWORK != GetType()  && TYPE_SINGLE_LOCAL != GetType()  && TYPE_SINGLE_SHOTDOWN != GetType()  && TYPE_SINGLE_QUIT != GetType() && TYPE_ALLTOOL != GetType()&&TYPE_TOOLBOX_CLOSE != GetType()) //��������аװ幦�ܱ�����
		{
			return;
		}
	}
	if ((!g_bIsSupportMTTouch) && g_bNoSupportTouchInWB)//���û����Ȩ��ť�Ͳ���Ӧ������
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
		if (g_bAvconLock &&TYPE_SINGLE_NETWORK != GetType()  && TYPE_SINGLE_LOCAL != GetType()  && TYPE_SINGLE_SHOTDOWN != GetType()  && TYPE_SINGLE_QUIT != GetType() && TYPE_ALLTOOL != GetType()&&TYPE_TOOLBOX_CLOSE != GetType()) //��������аװ幦�ܱ�����
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
	CString downBtnText(_T("����DOWN��ť�ı���")),upBtnText(_T("����UP��ť�ı���"));
    CTransparentButtonEx *upBtn = NULL;  //��ק�ƶ���ťʱUP�İ�ťָ��
    m_upPoint = point;
    ClientToScreen(&m_upPoint);  //��UP�ĵ�����ת��Ϊ��Ļ����
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
	//�������Ĵ����ǹ��������ǹ�����
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
		//���������İ���ťʲô������
		if((this->m_hWnd == upBtn->m_hWnd) //DOWN��UP��ͬһ����ť
			||(m_btnID == IDC_BTNTEN) ||(upBtn->m_btnID == IDC_BTNTEN) //�����䰴ť
			||(m_btnID == IDC_BTNNINE) ||(upBtn->m_btnID == IDC_BTNNINE) //��ԴĿ¼��ť
			||(m_btnID == IDC_BTNONE) ||(upBtn->m_btnID == IDC_BTNONE) //ѡ��ť
			||(m_btnID == IDC_BTNELEVEN) ||(upBtn->m_btnID == IDC_BTNELEVEN) //�����������ͱ��ذ�ť
			||(m_btnID == IDC_BTN_CLOSE) ||(upBtn->m_btnID == IDC_BTN_CLOSE) //�������йرհ�ť
			
			/*||(m_btnID == IDC_BTNEIGHT) ||(upBtn->m_btnID == IDC_BTNEIGHT)*/ //��ɫ�尴ť
			) 
		{

		}
		else
		{
			if (NULL ==m_pParentWnd || !IsWindow(m_pParentWnd->m_hWnd))
			{
				return;
			}
			//������ǹ������еİ�ť
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
				//������ǹ������еİ�ť
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
			//���DOWN�ǹ����䣬UP�ǹ�����
			else if((m_pParentWnd->m_hWnd == CWBToolboxDlg::g_toolBoxHWND) && ( pUpParent->m_hWnd == CWBToolbarDlg::g_toolBarHWND))
			{
				XAutoLock lock(m_ButtonUpWriteLock);
				CRect downBtnRt,upBtnRt,boxRt,barRt;
				int downPtRelativetoBoxX,downPtRelativetoBoxY,upPtRelativetoBarX,upPtRelativetoBarY;
				eType upBtnType;
				pToolbox = (CWBToolboxDlg *)m_pParentWnd;//���DOWN�ǹ�����
				pToolbox->GetWindowRect(&boxRt);
				pToolbar = (CWBToolbarDlg *)(upBtn->m_pParentWnd);//UP�ǹ�����
				pToolbar->GetWindowRect(&barRt);
				GetWindowRect(&downBtnRt); //DOWN��ť
				upBtn->GetWindowRect(&upBtnRt);//UP��ť
				downPtRelativetoBoxX = downBtnRt.left - boxRt.left;
				downPtRelativetoBoxY = downBtnRt.top - boxRt.top;
				upPtRelativetoBarX = upBtnRt.left - barRt.left;
				upPtRelativetoBarY = upBtnRt.top - barRt.top;
				upBtnType = upBtn->GetType();
				CString strSelection = _T("ButtonType");
				CString strValue = _T("");
				CString strText = _T("");
				//��ִ��UP�İ�ť
				//pToolbar->DelAndNewBtn(upBtn,upPtRelativetoBarY,m_btnType,downBtnText);//up�Ĵ�down�����ͺͱ���
				upBtn->SetWindowPos(&wndTop,WB_WIDTH_SPACE,upPtRelativetoBarY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
				upBtn->SetType(m_btnType);
				strText = pToolbar->GetCaptureFromType(m_btnType);
				upBtn->SetWindowText(strText);
				strValue.Format(_T("%d"),m_btnType);
				WriteWBConfigPathINI(strSelection,upBtn->GetName(),strValue);

				//pToolbox->DelAndNewBtn(this,downPtRelativetoBoxX,downPtRelativetoBoxY,upBtnType,upBtnText);    //down��up�������ͺͱ���  
				this->SetWindowPos(&wndTop,downPtRelativetoBoxX,downPtRelativetoBoxY,WB_TOOLBARBTN_WIDTH,WB_TOOLBARBTN_HEIGHT,SWP_SHOWWINDOW);
				this->SetType(upBtnType,true);				
				strText = pToolbox->GetCaptureFromType(upBtnType);
				this->SetWindowText(strText);
				
				strValue.Format(_T("%d"),upBtnType);
				WriteWBConfigPathINI(strSelection,this->GetName(),strValue);
				Invalidate();
			}
			//���DOWN�ǹ�������UP�ǹ�����
			else if((m_pParentWnd->m_hWnd == CWBToolbarDlg::g_toolBarHWND) && ( pUpParent->m_hWnd == CWBToolboxDlg::g_toolBoxHWND))
			{
				XAutoLock lock(m_ButtonUpWriteLock);
				CRect downBtnRt,upBtnRt,boxRt,barRt;
				int downPtRelativetoBoxX,downPtRelativetoBoxY,upPtRelativetoBarX,upPtRelativetoBarY;
				eType upBtnType;
				pToolbar = (CWBToolbarDlg *)m_pParentWnd;//���DOWN�ǹ�����
				pToolbar->GetWindowRect(&barRt);
				pToolbox = ( CWBToolboxDlg*)(upBtn->m_pParentWnd);//UP�ǹ�����
				pToolbox->GetWindowRect(&boxRt);
				GetWindowRect(&downBtnRt); //DOWN��ť
				upBtn->GetWindowRect(&upBtnRt);//UP��ť
				downPtRelativetoBoxX = downBtnRt.left - barRt.left;
				downPtRelativetoBoxY = downBtnRt.top - barRt.top;
				upPtRelativetoBarX = upBtnRt.left - boxRt.left;
				upPtRelativetoBarY = upBtnRt.top - boxRt.top;
				upBtnType = upBtn->GetType();


				CString strSelection = _T("ButtonType");
				CString strValue = _T("");
				CString strText = _T("");
				//��ִ��UP�İ�ť			
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
		if (g_bAvconLock &&TYPE_SINGLE_NETWORK != GetType()  && TYPE_SINGLE_LOCAL != GetType()  && TYPE_SINGLE_SHOTDOWN != GetType()  && TYPE_SINGLE_QUIT != GetType() && TYPE_ALLTOOL != GetType()&&TYPE_TOOLBOX_CLOSE != GetType()) //��������аװ幦�ܱ�����
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

		graph.DrawImage(m_pImgBG, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ

	}
	if (m_pImg)
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());
		if (g_GraphicsbuttonType == m_btnType || g_LineTypebuttonType == m_btnType || g_RubberbuttonType == m_btnType)
		{
			graph.DrawImage(m_pImg, Rt, rect.Width()*1, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
		}
		else
		{
			if (m_bBTNStatus)
			{
				graph.DrawImage(m_pImg, Rt, rect.Width()*2, 0, rect.Width(), rect.Height(), UnitPixel);
			}
			else
			{
				graph.DrawImage(m_pImg, Rt, rect.Width()*0, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
			}
		}
		
	}
	if (TYPE_DLG_COLORPALETTE == GetType())
	{
		SolidBrush brush(Color(255, GetRValue(g_clrWBCurObjColor), GetGValue(g_clrWBCurObjColor), GetBValue(g_clrWBCurObjColor)));
		graph.FillRectangle(&brush,35,35,15,15);
	}
	//�ڰ�ť��д�ı�
	if (m_btnType == TYPE_PRE_PAGE||m_btnType == TYPE_NEXT_PAGE||m_btnType == TYPE_REDO ||m_btnType == TYPE_UNDO||m_btnType == TYPE_CANTOUCH)
	{
		HRGN rgn = CreateRoundRectRgn(0,0,rect.Width(),rect.Height(),5,5);
		SetWindowRgn(rgn,TRUE);
		DeleteObject(rgn);
	}
	
	
	rect.top = rect.top + (rect.Height() - 13)/2;
	dc.SetBkMode( TRANSPARENT ); //�����ı�д��ʱ���ɫ͸��
	HFONT hFont;
	hFont = CreateFont(15, 6, 0, 0, FW_NORMAL, 0, 0, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,_T("����"));
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
				dc.DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
				
			}
			break;
		case TYPE_SAVE_PIC:
			{
				dc.SelectObject(hFont);
				dc.DrawText(m_strName,&rect, DT_LEFT);//������ʹ���ʼ
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

		graph.DrawImage(m_pImgBG, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ

	}
	if (m_pImg)
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());
		graph.DrawImage(m_pImg, Rt, rect.Width()*1, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
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
	//�ڰ�ť��д�ı�	
	rect.top = rect.top + (rect.Height() - 13)/2;
	dc.SetBkMode( TRANSPARENT );
	HFONT hFont;
	hFont = CreateFont(15, 6, 0, 0, FW_NORMAL, 0, 0, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,_T("����"));
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
			dc.DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
		
		}
		break;
	case TYPE_SAVE_PIC:
		{
			dc.SelectObject(hFont);
			dc.DrawText(m_strName,&rect, DT_LEFT);//������ʹ���ʼ
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
	if (m_pImgBG) //��ť����ͼ
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());

		graph.DrawImage(m_pImgBG, Rt, 0, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ

	}
	if (m_pImg)
	{
		Rect Rt(0, 0, rect.Width(), rect.Height());
		if (IsWindowEnabled())//�жϰ�ť�Ƿ����ڿ���״̬
		{
			if (g_GraphicsbuttonType == m_btnType || g_LineTypebuttonType == m_btnType || g_RubberbuttonType == m_btnType)
			{
				graph.DrawImage(m_pImg, Rt, rect.Width()*1, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
			}
			
			else
			{
				if (m_bBTNStatus)
				{
					graph.DrawImage(m_pImg, Rt, rect.Width()*2, 0, rect.Width(), rect.Height(), UnitPixel);
				}
				else
				{
					graph.DrawImage(m_pImg, Rt, rect.Width()*0, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
				}
			}
		} 
		else
		{
			graph.DrawImage(m_pImg, Rt, rect.Width()*3, 0, rect.Width(), rect.Height(), UnitPixel);//����ģʽ�л��ư�ťͼƬ
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

	//�ڰ�ť��д�ı�

 	rect.top = rect.top + (rect.Height() - 13)/2;
	pDc->SetBkMode( TRANSPARENT );
	HFONT hFont;
	hFont = CreateFont(15, 6, 0, 0, FW_NORMAL, 0, 0, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,_T("����"));
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
			pDc->DrawText(m_strName,&rect,DT_LEFT); //������ʹ���ʼ

		}
		break;
	case TYPE_FULLSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
			
		}
		break;
	case TYPE_AREASCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
			
		}
		break;
	case TYPE_WINDOWSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
			
		}
		break;
	case TYPE_PAUSESCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
			
		}
		break;
	case TYPE_STOPSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
			
		}
		break;
	case TYPE_PROPERTYSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
			
		}
		break;
	case TYPE_PATHSCREENRECORD:
		{
			pDc->SelectObject(hFont);
			pDc->DrawText(m_strName,&rect, DT_CENTER);//���ı����ڰ�ť������
			
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
