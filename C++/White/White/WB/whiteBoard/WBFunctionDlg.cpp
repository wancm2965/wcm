// WBFunctionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFunctionDlg.h"
#include "afxdialogex.h"
#include "WhiteBoardView.h"
#include "WBTextAttributeDlg.h"
#include "../wb/whiteBoard/common/WBMedia/WBGlobalMediaCtrl.h"

//��������Ű���㣬������ֵwangxiaobin
#define FUNCTIONATTRIBUTE_X 21
#define FUNCTIONATTRIBUTE_WIDTH 160
// CWBFunctionDlg �Ի���

IMPLEMENT_DYNAMIC(CWBFunctionDlg, CDialogEx)

	CWBFunctionDlg::CWBFunctionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBFunctionDlg::IDD, pParent)
{
	m_dLeaveTime = 0;
	m_dLeaveTimeDiff = 0;
	m_wbAttributeDlg = NULL;
	m_wbBackgroundDlg = NULL;
	m_wbTeachToolDlg = NULL;
	m_wbPreviewDlg = NULL;
	m_wbResDlg = NULL;
	m_pParent = pParent;
	m_PreviewRect.SetRect(0,0,0,0);
	m_bIsClickSolidBtn = false;
	m_rectPreviewMenu.SetRect(0, 0, 0, 0); //���Ԥ���˵��ľ�������
	m_nShowOrHideCount = 0;
	m_bIshidewindow = true;
}

CWBFunctionDlg::~CWBFunctionDlg()
{
	if (NULL != m_wbPreviewDlg)
	{
		delete m_wbPreviewDlg;
		m_wbPreviewDlg = NULL;
	}
	if (NULL != m_wbAttributeDlg)
	{
		delete m_wbAttributeDlg;
		m_wbAttributeDlg = NULL;
	}
 	if (NULL != m_wbBackgroundDlg)
 	{
 		delete m_wbBackgroundDlg;
 		m_wbBackgroundDlg = NULL;
 	}
	if (NULL != m_wbTeachToolDlg)
	{
		delete m_wbTeachToolDlg;
		m_wbTeachToolDlg = NULL;
	}
	if (NULL != m_wbResDlg)
	{
		delete m_wbResDlg;
		m_wbResDlg = NULL;
	}
	//g_isFunctionDlgHide = FALSE;
}

void CWBFunctionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX,IDC_WB_HIDEFUNCDLG,m_cHideFuncdlgButton);
	//DDX_Control(pDX,IDC_WB_FUNCHIDESTATIC,m_cFuncdlgHideStatic);
	//DDX_Control(pDX,IDC_SINWB_CLOSEFUNCPAD,m_btnCloseFuncPad);
}

BEGIN_MESSAGE_MAP(CWBFunctionDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_SELECTWBOBJ,&CWBFunctionDlg::OnWM_SELECTWBOBJ)
	ON_MESSAGE(WM_SELECTWBERASER, &CWBFunctionDlg::OnWM_SELECTWBERASER)
	ON_MESSAGE(WM_SELECTWBPEN,&CWBFunctionDlg::OnWM_SELECTWBPEN)
	ON_MESSAGE(WM_NOSELECTWBOBJ,&CWBFunctionDlg::OnWM_NOSELECTWBOBJ)
	ON_MESSAGE(WM_OPENARRIBUTEDLG,&CWBFunctionDlg::OnWM_OPENARRIBUTEDLG)
	ON_MESSAGE(WM_PALETTECOLOR,&CWBFunctionDlg::OnWM_PALETTECOLOR)
	ON_MESSAGE(WM_OPENPREVIEWDLG,&CWBFunctionDlg::OnWM_OPENPREVIEWDLG)
	ON_MESSAGE(WM_OPENRESDLG,&CWBFunctionDlg::OnWM_OPENRESDLG)
	ON_MESSAGE(WM_OPENBACKGROUNDDLG,&CWBFunctionDlg::OnWM_OPENBACKGROUNDDLG)
	ON_MESSAGE(WM_OPENTEACHTOOLDLG,&CWBFunctionDlg::OnWM_OPENTEACHTOOLDLG)
	ON_BN_CLICKED(IDC_SINWB_CLOSEFUNCPAD, &CWBFunctionDlg::OnBnClickedClosewbfunctiondlg)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

 //����ѡ�����������Զ������������-caoyuqin
LRESULT CWBFunctionDlg::OnWM_SELECTWBOBJ(WPARAM wParam, LPARAM lParam)
{
	CSize sized;
	if (g_pObj == NULL)
	{
		return 0;
	}
	if(NULL != m_wbAttributeDlg)
	{
		m_wbAttributeDlg->CloseColorPaletteDlg();
		//�ѹ����������ö�
		m_wbAttributeDlg->ResetVScroll(SB_TOP);

	}
	if (NULL == m_wbAttributeDlg /*|| !m_wbAttributeDlg->IsWindowVisible()*/)//�������Ϊ��ʱ
	{
		return 0;
	}
	m_wbAttributeDlg->m_cAttMTOperate.ShowWindow(SW_HIDE);
	m_wbAttributeDlg->m_btnMTRotate.ShowWindow(SW_HIDE);
	m_wbAttributeDlg->m_btnMTZoom.ShowWindow(SW_HIDE);
	m_wbAttributeDlg->m_BrushSizeCombx.ShowWindow(SW_HIDE);
	m_wbAttributeDlg->m_BrushSizeStatic.ShowWindow(SW_HIDE);
	m_wbAttributeDlg->m_DefaultBackGround.ShowWindow(SW_HIDE);

	if (lParam == 1)//��ѡʱ
	{
		CString strTranspar;
		strTranspar = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgTransparStatic"));
		if (strTranspar.IsEmpty())
		{
			strTranspar = _T("͸����");
		}
		m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_SHOW); //��ʾ�����ĵ���͸����
		m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparStatic.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_cReduceTransperBtn.MoveWindow(FUNCTIONATTRIBUTE_X,58,30,30);
		m_wbAttributeDlg->m_cAddTransparBtn.MoveWindow(145,58,30,30);
		m_wbAttributeDlg->m_TransparSlider.MoveWindow(53,58,90,30);
		m_wbAttributeDlg->m_TransparStatic.SetCaption(strTranspar);
		m_wbAttributeDlg->m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
		CSize nsize;
		nsize = m_wbAttributeDlg->m_TransparStatic.GetStringLength();
		m_wbAttributeDlg->m_TransparEdit.MoveWindow(FUNCTIONATTRIBUTE_X + nsize.cx + 5,31,40,20);
		m_wbAttributeDlg->m_PerStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + nsize.cx + 50,35);
		m_wbAttributeDlg->m_PerStatic.ShowWindow(SW_SHOW);

		switch(int ntype = g_pObj->GetType())
		{
		case WB_OBJ_LINE://ѡ��ֱ��ʱ
			{
				CWBLine *pCWBline=(CWBLine*)g_pObj;
				PWBLineItem pWBLineCurItem = new WBLineItem;
				pCWBline->GetItem(pWBLineCurItem);
				g_nTranspar = (float)pCWBline->GetAlpha();
				g_nWBCurObjWidth = (*pWBLineCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBLineCurItem).clrLine;
				g_nWBLineType = (*pWBLineCurItem).nLineType;
				g_nStartLineCap = (*pWBLineCurItem).nStartLineCap;
				g_nEndLineCap = (*pWBLineCurItem).nEndLineCap;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				functionAttribute.LineTypeStatic = 5;
				functionAttribute.LineTypeCombx = 5;
				functionAttribute.ArrowStatic = 5;
				functionAttribute.ArrowCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawLine();

				delete pWBLineCurItem;

				m_wbAttributeDlg->SetLineType(g_nWBLineType);
				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->SetLineCap(g_nStartLineCap, g_nEndLineCap);
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;
		case WB_OBJ_CURVE://ѡ������ʱ
			{
				CWBCurve *pCWBCurve=(CWBCurve*)g_pObj;
				PWBCurveItem pWBCureCurItem = pCWBCurve->GetItem();

				g_nTranspar = (float)pCWBCurve->GetAlpha();
				g_nStartLineCap = pWBCureCurItem->GetStartLineCap();
				g_nEndLineCap = pWBCureCurItem->GetEndLineCap();
				g_nWBLineType = pWBCureCurItem->GetType();
				g_nWBCurObjWidth = pWBCureCurItem->GetWidth();
				g_clrWBCurObjColor = pWBCureCurItem->GetColor();

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				functionAttribute.LineTypeStatic = 5;
				functionAttribute.LineTypeCombx = 5;
				functionAttribute.ArrowStatic = 5;
				functionAttribute.ArrowCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawLine();

				CWBCurve::ClearCurveItem(pWBCureCurItem);
				delete pWBCureCurItem;

				m_wbAttributeDlg->SetLineType(g_nWBLineType);
				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->SetLineCap(g_nStartLineCap,g_nEndLineCap);
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;
		case WB_OBJ_ICOCURVE://ѡ��ͼ������ʱ
			{
				CWBIcoCurve * pCWBIcoCurve=(CWBIcoCurve*)g_pObj;
				PWBIcoCurveItem pWBIcoCureCurItem = pCWBIcoCurve->GetItem();

				g_nTranspar = pCWBIcoCurve->GetAlpha();
				g_nWBCurObjWidth = pWBIcoCureCurItem->GetScaleWidth();
				g_nWBCurObjIcoFilePath = pWBIcoCureCurItem->GetIcoFilePath();

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				functionAttribute.IcoStyleComBox = 5;
				functionAttribute.IcoStyleStatic = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFuncitonAttributeMoveWindowDrawIcoCurve();

				delete pWBIcoCureCurItem;

				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->SetIcoStyle(g_nWBCurObjIcoFilePath);
				m_wbAttributeDlg->m_TransparSlider.SetPos((255-g_nTranspar)/2.55);
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;
		case WB_OBJ_PALLELOGAM: //ƽ���ı���
			{
				CWBPallelogam* pCWBPallelogam=(CWBPallelogam*)g_pObj;
				PWBPolygonItem pCWBPallelogamCurItem = new WBPolygonItem;
				pCWBPallelogam->GetItem(pCWBPallelogamCurItem);
				g_nTranspar = (float)pCWBPallelogam->GetAlpha();
				g_nWBCurObjWidth = (*pCWBPallelogamCurItem).nWidth;
				g_clrWBCurObjColor = (*pCWBPallelogamCurItem).clrLine;
				g_clrSolid = (*pCWBPallelogamCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();


				if (pCWBPallelogam->IsSolid())
				{
					g_bIsSolid = TRUE;
				} 
				else
				{
					g_bIsSolid = FALSE;    				
				}  
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				CWBPolygon::ClearItem(pCWBPallelogamCurItem);
				delete pCWBPallelogamCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_RECT://ѡ�о���ʱ
			{
				CWBRectangle* pCWBRectangle=(CWBRectangle*)g_pObj;
				PWBPolygonItem pCWBRectangleCurItem = new WBPolygonItem;
				pCWBRectangle->GetItem(pCWBRectangleCurItem);
				g_nTranspar = (float)pCWBRectangle->GetAlpha();
				g_nWBCurObjWidth = (*pCWBRectangleCurItem).nWidth;
				g_clrWBCurObjColor = (*pCWBRectangleCurItem).clrLine;
				g_clrSolid = (*pCWBRectangleCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();
				

				if (pCWBRectangle->IsSolid())
				{
					g_bIsSolid = TRUE;
				} 
				else
				{
					g_bIsSolid = FALSE;    				
				}  
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				CWBPolygon::ClearItem(pCWBRectangleCurItem);
				delete pCWBRectangleCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_ELLIPSE://ѡ����Բʱ
			{
				CWBEllipse* pCWBEllipse=(CWBEllipse*)g_pObj;
				PWBEllipseItem pPWBEllipseCurItem = new WBEllipseItem;
				pCWBEllipse->GetItem(pPWBEllipseCurItem);
				g_nTranspar = (float)pCWBEllipse->GetAlpha();
				g_nWBCurObjWidth = (*pPWBEllipseCurItem).nWidth;
				g_clrWBCurObjColor = (*pPWBEllipseCurItem).clrLine;
				g_clrSolid = (*pPWBEllipseCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBEllipse->IsSolid())
				{
					g_bIsSolid = TRUE;    				
				} 
				else
				{
					g_bIsSolid  = FALSE;    				
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				delete pPWBEllipseCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_ARBITRARYTRIANGLE://ѡ������������ʱ
			{
				CWBArbitraryTriangle* pCWBTriangle=(CWBArbitraryTriangle*)g_pObj;						
				PWBPolygonItem pWBPolygonCurItem = new WBPolygonItem;
				pCWBTriangle->GetItem(pWBPolygonCurItem);
				g_nTranspar = (float)pCWBTriangle->GetAlpha();
				g_nWBCurObjWidth = (*pWBPolygonCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBPolygonCurItem).clrLine;
				g_clrSolid = (*pWBPolygonCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBTriangle->IsSolid())
				{
					g_bIsSolid = TRUE;    				
				} 
				else
				{
					g_bIsSolid = FALSE;    				
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				//delete pCWBTriangle;
				CWBPolygon::ClearItem(pWBPolygonCurItem);
				delete pWBPolygonCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_TRIANGLE://ѡ��������ʱ
			{
				CWBTriangle* pCWBTriangle=(CWBTriangle*)g_pObj;						
				PWBPolygonItem pWBPolygonCurItem = new WBPolygonItem;
				pCWBTriangle->GetItem(pWBPolygonCurItem);
				g_nTranspar = (float)pCWBTriangle->GetAlpha();
				g_nWBCurObjWidth = (*pWBPolygonCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBPolygonCurItem).clrLine;
				g_clrSolid = (*pWBPolygonCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBTriangle->IsSolid())
				{
					g_bIsSolid = TRUE;    				
				} 
				else
				{
					g_bIsSolid = FALSE;    				
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				//delete pCWBTriangle;
				CWBPolygon::ClearItem(pWBPolygonCurItem);
				delete pWBPolygonCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_RHOMBUS://ѡ������ʱ
			{
				CWBRhombus* pCWBRhombus=(CWBRhombus*)g_pObj;						
				PWBPolygonItem pWBRhombusCurItem = new WBPolygonItem;
				pCWBRhombus->GetItem(pWBRhombusCurItem);
				g_nTranspar = (float)pCWBRhombus->GetAlpha();
				g_nWBCurObjWidth = (*pWBRhombusCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBRhombusCurItem).clrLine;
				g_clrSolid = (*pWBRhombusCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBRhombus->IsSolid())
				{
					g_bIsSolid = TRUE;
				} 
				else
				{
					g_bIsSolid = FALSE;
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				//delete pCWBRhombus;
				CWBPolygon::ClearItem(pWBRhombusCurItem);
				delete pWBRhombusCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_PENTAGON://ѡ�������ʱ
			{
				CWBPentagon* pCWBPentagon = (CWBPentagon*)g_pObj;						
				PWBPolygonItem pWBPentagonCurItem = new WBPolygonItem;
				pCWBPentagon->GetItem(pWBPentagonCurItem);
				g_nTranspar = (float)pCWBPentagon->GetAlpha();
				g_nWBCurObjWidth = (*pWBPentagonCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBPentagonCurItem).clrLine;
				g_clrSolid = (*pWBPentagonCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBPentagon->IsSolid())
				{
					g_bIsSolid = TRUE;
				} 
				else
				{
					g_bIsSolid = FALSE;
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				//delete pCWBRhombus;
				CWBPolygon::ClearItem(pWBPentagonCurItem);
				delete pWBPentagonCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_HEXAGON://ѡ��������ʱ
			{
				CWBHexagon* pCWBHexagon=(CWBHexagon*)g_pObj;						
				PWBPolygonItem pWBHexagonCurItem = new WBPolygonItem;
				pCWBHexagon->GetItem(pWBHexagonCurItem);
				g_nTranspar = (float)pCWBHexagon->GetAlpha();
				g_nWBCurObjWidth = (*pWBHexagonCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBHexagonCurItem).clrLine;
				g_clrSolid = (*pWBHexagonCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBHexagon->IsSolid())
				{
					g_bIsSolid = TRUE;
				} 
				else
				{
					g_bIsSolid = FALSE;
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				//delete pCWBRhombus;
				CWBPolygon::ClearItem(pWBHexagonCurItem);
				delete pWBHexagonCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_FIVEPOINTSTART://ѡ�������ʱ
			{
				CWBFivepointstart* pCWBFivepointstart = (CWBFivepointstart*)g_pObj;						
				PWBPolygonItem pWBFivepointstartCurItem = new WBPolygonItem;
				pCWBFivepointstart->GetItem(pWBFivepointstartCurItem);
				g_nTranspar = (float)pCWBFivepointstart->GetAlpha();
				g_nWBCurObjWidth = (*pWBFivepointstartCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBFivepointstartCurItem).clrLine;
				g_clrSolid = (*pWBFivepointstartCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBFivepointstart->IsSolid())
				{
					g_bIsSolid = TRUE;
				} 
				else
				{
					g_bIsSolid = FALSE;
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				//delete pCWBRhombus;
				CWBPolygon::ClearItem(pWBFivepointstartCurItem);
				delete pWBFivepointstartCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case  WB_OBJ_TRAPEZIUM://ѡ������ʱ
			{
				CWBTrapezium* pCWBTrapezium=(CWBTrapezium*)g_pObj;						
				PWBPolygonItem pWBTrapeziumCurItem = new WBPolygonItem;
				pCWBTrapezium->GetItem(pWBTrapeziumCurItem);
				g_nTranspar = (float)pCWBTrapezium->GetAlpha();
				g_nWBCurObjWidth = (*pWBTrapeziumCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBTrapeziumCurItem).clrLine;
				g_clrSolid = (*pWBTrapeziumCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBTrapezium->IsSolid())
				{
					g_bIsSolid = TRUE;
				} 
				else
				{
					g_bIsSolid = FALSE;
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				//delete pCWBRhombus;
				CWBPolygon::ClearItem(pWBTrapeziumCurItem);
				delete pWBTrapeziumCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case WB_OBJ_POLYGON://ѡ�ж����ʱ
			{
				CWBPolygon* pCWBPolygon = (CWBPolygon*)g_pObj;						
				PWBPolygonItem pWBPolygonCurItem = new WBPolygonItem;
				pCWBPolygon->GetItem(pWBPolygonCurItem);
				g_nTranspar = (float)pCWBPolygon->GetAlpha();
				g_nWBCurObjWidth = (*pWBPolygonCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBPolygonCurItem).clrLine;
				g_clrSolid = (*pWBPolygonCurItem).clrFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBPolygon->IsSolid())
				{
					g_bIsSolid = TRUE;    				
				} 
				else
				{
					g_bIsSolid = FALSE;
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				//delete pCWBPolygon;
				CWBPolygon::ClearItem(pWBPolygonCurItem);
				delete pWBPolygonCurItem;
				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
			}
			break;
		case WB_OBJ_PICTURE: //ͼƬ
			{
				CWBPicture *pPicture = (CWBPicture *)g_pObj;
				if (pPicture->GetIsPrintScreen())
				{
					g_bIsMTRotate = pPicture->GetIsMTRotate();
					m_wbAttributeDlg->m_btnMTRotate.SetIsSelect(g_bIsMTRotate);
					m_wbAttributeDlg->m_btnMTZoom.SetIsSelect(!g_bIsMTRotate);
				}

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);

				if (pPicture->GetIsPrintScreen())
				{
					m_wbAttributeDlg->m_cAttMTOperate.ShowWindow(SW_SHOW);
					m_wbAttributeDlg->m_cAttMTOperate.MoveWindow(FUNCTIONATTRIBUTE_X,100,100,15);
					m_wbAttributeDlg->m_btnMTRotate.ShowWindow(SW_SHOW);
					m_wbAttributeDlg->m_btnMTRotate.MoveWindow(FUNCTIONATTRIBUTE_X,115,64,28);
					m_wbAttributeDlg->m_btnMTZoom.ShowWindow(SW_SHOW);
					m_wbAttributeDlg->m_btnMTZoom.MoveWindow(95,115,64,28);
				}
				
				g_nTranspar = (float)pPicture->GetAlpha();
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;	
		case WB_OBJ_FLASH:
		case WB_OBJ_CAMERA:
		case WB_OBJ_MEDIA: //��Ƶ
			{
				CString strNoAttribute;
				strNoAttribute = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgNoAttribute"));
				if (strNoAttribute.IsEmpty())
				{
					strNoAttribute = _T("������");
				}
				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);

				if (!g_bU7NewStyle)
				{
					m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_HIDE);
				}
				
				m_wbAttributeDlg->m_TransparStatic.SetCaption(strNoAttribute);
				m_wbAttributeDlg->m_TransparStatic.SetWindowText(strNoAttribute);
				m_wbAttributeDlg->m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
				m_wbAttributeDlg->m_PerStatic.ShowWindow(SW_HIDE);
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;
		//case WB_OBJ_FLASH: //flash
		//	{
		//		WBFunctionAttribute functionAttribute;
		//		ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
		//		m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);

		//		CWBFlash *pFlash = (CWBFlash *)g_pObj;
		//		g_nTranspar = (float)pFlash->GetAlpha();
		//		m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
		//		m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
		//		m_wbAttributeDlg->UpdateData(FALSE);
		//	}
			break;
		case WB_OBJ_DOCUMENT: //�ĵ�
			{
				CWBDocument *pCWBDoc = (CWBDocument *)g_pObj;
				g_bIsMTRotate = pCWBDoc->GetIsMTRotate();
				m_wbAttributeDlg->m_btnMTRotate.SetIsSelect(g_bIsMTRotate);
				m_wbAttributeDlg->m_btnMTZoom.SetIsSelect(!g_bIsMTRotate);

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);

				m_wbAttributeDlg->m_cAttMTOperate.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_cAttMTOperate.MoveWindow(FUNCTIONATTRIBUTE_X,100,100,15);
				m_wbAttributeDlg->m_btnMTRotate.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_btnMTRotate.MoveWindow(FUNCTIONATTRIBUTE_X,115,64,28);
				m_wbAttributeDlg->m_btnMTZoom.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_btnMTZoom.MoveWindow(95,115,64,28);
				CWBDocument *pDocument = (CWBDocument *)g_pObj;
				g_nTranspar = (float)pDocument->GetAlpha();
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;
		case WB_OBJ_TEXT: //����
			{
				CWBText *pCWBText = (CWBText *)g_pObj;

				PWBTextItem pWBTextCurItem = new WBTextItem;
				pCWBText->GetItem(pWBTextCurItem);				
				g_clrWBCurObjColor = pWBTextCurItem->clrText;
				g_nWBCurObjFontFaceName = pWBTextCurItem->fontItem.lfFaceName;
				g_nWBCurObjFontSize = pWBTextCurItem->fontItem.lfHeight;
				g_isItalic = pWBTextCurItem->fontItem.lfItalic;
				g_isUnderline = pWBTextCurItem->fontItem.lfUnderline;
				g_bIsVertical = pCWBText->GetIsVertical();
				//CString nFontSizeText;
				//nFontSizeText.Format(_T("%d"),g_nWBCurObjFontSize);	
				g_nTranspar = (float)pCWBText->GetAlpha();	

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.FontStatic = 5;
				functionAttribute.FontFaceComBox = 5;
				functionAttribute.FontSizeComBox = 5;
				functionAttribute.FontBoldBtn = 5;
				functionAttribute.FontItalicBtn = 5;
				functionAttribute.FontUnderlineBtn = 5;
				functionAttribute.VerticalBtn = 5;
				functionAttribute.cTextStatic = 5;
				functionAttribute.TextEditBox = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->m_FontStatic.MoveWindow(FUNCTIONATTRIBUTE_X,152,FUNCTIONATTRIBUTE_WIDTH,58);
				m_wbAttributeDlg->m_FontFaceComBox.MoveWindow(FUNCTIONATTRIBUTE_X,154,100,400);
				m_wbAttributeDlg->m_FontSizeComBox.MoveWindow(121,154,52,400);
				m_wbAttributeDlg->m_FontBoldBtn.MoveWindow(30,181,28,28);
				m_wbAttributeDlg->m_FontItalicBtn.MoveWindow(68,181,28,28);	
				m_wbAttributeDlg->m_FontUnderlineBtn.MoveWindow(106,181,28,28);
				m_wbAttributeDlg->m_VerticalBtn.MoveWindow(144,181,28,28);				
				m_wbAttributeDlg->m_cTextStatic.MoveWindow(FUNCTIONATTRIBUTE_X,212,120,20);
				m_wbAttributeDlg->m_TextEditBox.MoveWindow(FUNCTIONATTRIBUTE_X,232,FUNCTIONATTRIBUTE_WIDTH,100);


				m_wbAttributeDlg->m_TextEditBox.LineScroll(0);

				m_wbAttributeDlg->m_Textstr = g_cTextstr = pWBTextCurItem->strText;
				//m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				//m_wbAttributeDlg->UpdateData(FALSE);
				int j = m_wbAttributeDlg->m_FontFaceComBox.FindStringExact(0,g_nWBCurObjFontFaceName);	
				m_wbAttributeDlg->m_FontFaceComBox.SetCurSel(j);
				//m_wbAttributeDlg->m_FontSizeComBox.SetWindowText(nFontSizeText);
				m_wbAttributeDlg->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
				m_wbAttributeDlg->UpdateData(FALSE);

				if (pWBTextCurItem->fontItem.lfWeight == FW_BOLD)
				{
					g_isBold = TRUE;					
				}
				if (pWBTextCurItem->fontItem.lfWeight == FW_NORMAL)
				{
					g_isBold = FALSE;					
				}
				m_wbAttributeDlg->m_FontBoldBtn.SetIsSelect(g_isBold);
				m_wbAttributeDlg->m_FontBoldBtn.Invalidate();
				m_wbAttributeDlg->m_FontItalicBtn.SetIsSelect(g_isItalic);
				m_wbAttributeDlg->m_FontItalicBtn.Invalidate();
				m_wbAttributeDlg->m_FontUnderlineBtn.SetIsSelect(g_isUnderline);
				m_wbAttributeDlg->m_FontUnderlineBtn.Invalidate();
				m_wbAttributeDlg->m_VerticalBtn.SetIsSelect(g_bIsVertical);
				m_wbAttributeDlg->m_VerticalBtn.Invalidate();

				delete pWBTextCurItem;
			}
			break;
		case WB_OBJ_COMBINE: //��϶���
			{
				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);

				CWBCombine *pCombine = (CWBCombine *)g_pObj;
				g_nTranspar = (float)pCombine->GetAlpha();
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);

			}
			break;
		case WB_OBJ_ANNOTATIONS://ѡ�б�עʱ
			{
				CWBAnnotations *pAnnotations = (CWBAnnotations *)g_pObj;

				CWBText *pText = pAnnotations->GetText();
				PWBTextItem pWBTextCurItem = new WBTextItem;
				pText->GetItem(pWBTextCurItem);				
				g_clrWBCurObjColor = pWBTextCurItem->clrText;
				g_nWBCurObjFontFaceName = pWBTextCurItem->fontItem.lfFaceName;
				g_nWBCurObjFontSize = pWBTextCurItem->fontItem.lfHeight;
				g_isItalic = pWBTextCurItem->fontItem.lfItalic;
				g_isUnderline = pWBTextCurItem->fontItem.lfUnderline;
				g_bIsVertical = pText->GetIsVertical();
				//CString nFontSizeText;
				//nFontSizeText.Format(_T("%d"),g_nWBCurObjFontSize);	

				CWBPolygon *pPolygon = pAnnotations->GetPolygon();
				PWBPolygonItem pWBPolygonCurItem = new WBPolygonItem;
				pPolygon->GetItem(pWBPolygonCurItem);
				g_clrSolid = (*pWBPolygonCurItem).clrFillSolid;

				g_nTranspar = (float)pAnnotations->GetAlpha();

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic = 5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.FontStatic = 5;
				functionAttribute.FontFaceComBox = 5;
				functionAttribute.FontSizeComBox = 5;
				functionAttribute.FontBoldBtn = 5;
				functionAttribute.FontItalicBtn = 5;
				functionAttribute.FontUnderlineBtn = 5;
				functionAttribute.VerticalBtn = 5;
				functionAttribute.cTextStatic = 5;
				functionAttribute.TextEditBox = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->m_SolidColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,150);
				sized = m_wbAttributeDlg->m_SolidColorStatic.GetStringLength();
				m_wbAttributeDlg->m_SolidCheckBtn.MoveWindow(FUNCTIONATTRIBUTE_X+ sized.cx + 5,145,20,20);
				m_wbAttributeDlg->m_SolidCheckStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X+ sized.cx + 30,150);
				m_wbAttributeDlg->m_SolidColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,170,FUNCTIONATTRIBUTE_WIDTH,30);
				m_wbAttributeDlg->m_FontStatic.MoveWindow(FUNCTIONATTRIBUTE_X,214,156,58);
				m_wbAttributeDlg->m_FontFaceComBox.MoveWindow(FUNCTIONATTRIBUTE_X,216,100,400);
				m_wbAttributeDlg->m_FontSizeComBox.MoveWindow(121,216,52,400);
				m_wbAttributeDlg->m_FontBoldBtn.MoveWindow(30,243,28,28);
				m_wbAttributeDlg->m_FontItalicBtn.MoveWindow(68,243,28,28);
				m_wbAttributeDlg->m_FontUnderlineBtn.MoveWindow(106,243,28,28);
				m_wbAttributeDlg->m_VerticalBtn.MoveWindow(144,243,28,28);
				m_wbAttributeDlg->m_cTextStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,274);
				m_wbAttributeDlg->m_TextEditBox.MoveWindow(FUNCTIONATTRIBUTE_X,294,FUNCTIONATTRIBUTE_WIDTH,100);

				m_wbAttributeDlg->m_TextEditBox.LineScroll(0);

				m_wbAttributeDlg->m_Textstr = g_cTextstr = pWBTextCurItem->strText;
				//m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				//m_wbAttributeDlg->UpdateData(FALSE);
				int j = m_wbAttributeDlg->m_FontFaceComBox.FindStringExact(0,g_nWBCurObjFontFaceName);	
				m_wbAttributeDlg->m_FontFaceComBox.SetCurSel(j);
				//m_wbAttributeDlg->m_FontSizeComBox.SetWindowText(nFontSizeText);
				m_wbAttributeDlg->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
				m_wbAttributeDlg->UpdateData(FALSE);

				if (pWBTextCurItem->fontItem.lfWeight == FW_BOLD)
				{
					g_isBold = TRUE;					
				}
				if (pWBTextCurItem->fontItem.lfWeight == FW_NORMAL)
				{
					g_isBold = FALSE;					
				}
				m_wbAttributeDlg->m_FontBoldBtn.SetIsSelect(g_isBold);
				m_wbAttributeDlg->m_FontBoldBtn.Invalidate();
				m_wbAttributeDlg->m_FontItalicBtn.SetIsSelect(g_isItalic);
				m_wbAttributeDlg->m_FontItalicBtn.Invalidate();
				m_wbAttributeDlg->m_FontUnderlineBtn.SetIsSelect(g_isUnderline);
				m_wbAttributeDlg->m_FontUnderlineBtn.Invalidate();
				m_wbAttributeDlg->m_VerticalBtn.SetIsSelect(g_bIsVertical);
				m_wbAttributeDlg->m_VerticalBtn.Invalidate();
				delete pWBTextCurItem;
				if (pAnnotations->IsSolid())
				{
					g_bIsSolid = TRUE;    				
				} 
				else
				{
					g_bIsSolid = FALSE;
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
				CWBPolygon::ClearItem(pWBPolygonCurItem);
				delete pWBPolygonCurItem;
			}
			break;
		case WB_OBJ_DXF: //dxf
			{
				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);

				CWBDxf *pDxf = (CWBDxf *)g_pObj;
				g_nTranspar = (float)pDxf->GetAlpha();
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;	
		case WB_OBJ_SECTOR://����
			{
				CWBSector* pCWBSector=(CWBSector*)g_pObj;
				PWBSectorItem pWBSectorCurItem = new WBSectorItem;
				pCWBSector->GetItem(pWBSectorCurItem);
				g_nTranspar = (float)pCWBSector->GetAlpha();
				g_nWBCurObjWidth = (*pWBSectorCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBSectorCurItem).clrLine;
				g_clrSolid = (*pWBSectorCurItem).clrFillSolid;
				g_bIsSolid = (*pWBSectorCurItem).bIsFillSolid;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

				if (pCWBSector->IsSolid())
				{
					g_bIsSolid = TRUE;    				
				} 
				else
				{
					g_bIsSolid  = FALSE;    				
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				delete pWBSectorCurItem;

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;
		case WB_OBJ_ARC://Բ��
			{
				CWBArc *pCWBArc =(CWBArc*)g_pObj;
				PWBArcItem pWBArcCurItem = new WBArcItem;
				pCWBArc->GetItem(pWBArcCurItem);
				g_nTranspar = (float)pCWBArc->GetAlpha();
				g_nWBCurObjWidth = (*pWBArcCurItem).nWidth;
				g_clrWBCurObjColor = (*pWBArcCurItem).clrLine;
				g_nWBLineType = (*pWBArcCurItem).nLineType;
				g_nStartLineCap = (*pWBArcCurItem).nStartLineCap;
				g_nEndLineCap = (*pWBArcCurItem).nEndLineCap;

				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				functionAttribute.LineTypeStatic = 5;
				functionAttribute.LineTypeCombx = 5;
				functionAttribute.ArrowStatic = 5;
				functionAttribute.ArrowCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawLine();

				delete pWBArcCurItem;

				m_wbAttributeDlg->SetLineType(g_nWBLineType);
				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->SetLineCap(g_nStartLineCap, g_nEndLineCap);
				m_wbAttributeDlg->SetLineWidth(g_nWBCurObjWidth);
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->UpdateData(FALSE);
			}
			break;
		}
		//m_wbAttributeDlg->m_FontSizeComBox.Invalidate();	
	}
	if (lParam > 1)//��ѡʱ
	{
		//��������������
		m_wbAttributeDlg->WBFunctionAttributeHideWindow();

		CString strTranspar;
		strTranspar = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgTransparStatic"));
		if (strTranspar.IsEmpty())
		{
			strTranspar = _T("͸����");
		}
		m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_SHOW); //��ʾ�����ĵ���͸����
		m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparStatic.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_cReduceTransperBtn.MoveWindow(FUNCTIONATTRIBUTE_X,58,30,30);
		m_wbAttributeDlg->m_cAddTransparBtn.MoveWindow(145,58,30,30);
		m_wbAttributeDlg->m_TransparSlider.MoveWindow(53,58,90,30);
		m_wbAttributeDlg->m_TransparStatic.SetCaption(strTranspar);
		m_wbAttributeDlg->m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
		CSize nsize;
		nsize = m_wbAttributeDlg->m_TransparStatic.GetStringLength();
		m_wbAttributeDlg->m_TransparEdit.MoveWindow(FUNCTIONATTRIBUTE_X + nsize.cx + 5,31,40,20);
		m_wbAttributeDlg->m_PerStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + nsize.cx + 50,35);
		m_wbAttributeDlg->m_PerStatic.ShowWindow(SW_SHOW);
		switch (wParam)
		{
		case WB_OBJ_DXF:
		case WB_OBJ_PICTURE:
		case WB_OBJ_DOCUMENT:
		case WB_OBJ_COMBINE:
		case WB_TRANS_ATTRIBUTE:
			{				
				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
			}
			break;
		case WB_OBJ_FLASH:
		case WB_OBJ_CAMERA:
		case WB_OBJ_MEDIA:
		case WB_NO_ATTRIBUTE:
			{		
				CString strNoAttribute;
				strNoAttribute = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgNoAttribute"));
				if (strNoAttribute.IsEmpty())
				{
					strNoAttribute = _T("������");
				}
				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);

				if (!g_bU7NewStyle)
				{
					m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_HIDE);
				}
				
				m_wbAttributeDlg->m_TransparStatic.SetCaption(strNoAttribute);
				m_wbAttributeDlg->m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
				m_wbAttributeDlg->m_PerStatic.ShowWindow(SW_HIDE);
			}
			break;
		case WB_ANNO_ATTRIBUTE:   //�����ע
			{
				CWBAnnotations *pAnnotations = (CWBAnnotations *)g_pObj;

				CWBText *pText = pAnnotations->GetText();
				PWBTextItem pWBTextCurItem = new WBTextItem;
				pText->GetItem(pWBTextCurItem);				
				g_clrWBCurObjColor = pWBTextCurItem->clrText;
				g_nWBCurObjFontFaceName = pWBTextCurItem->fontItem.lfFaceName;
				g_nWBCurObjFontSize = pWBTextCurItem->fontItem.lfHeight;
				g_isItalic = pWBTextCurItem->fontItem.lfItalic;
				g_isUnderline = pWBTextCurItem->fontItem.lfUnderline;
				g_bIsVertical = pText->GetIsVertical();

				CWBPolygon *pPolygon = pAnnotations->GetPolygon();
				PWBPolygonItem pWBPolygonCurItem = new WBPolygonItem;
				pPolygon->GetItem(pWBPolygonCurItem);
				g_clrSolid = (*pWBPolygonCurItem).clrFillSolid;

				g_nTranspar = (float)pAnnotations->GetAlpha();

				m_wbAttributeDlg->m_LineColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,96);
				m_wbAttributeDlg->m_LineColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,112,FUNCTIONATTRIBUTE_WIDTH,30);
				m_wbAttributeDlg->m_SolidColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,150);
				m_wbAttributeDlg->m_SolidColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,170,FUNCTIONATTRIBUTE_WIDTH,30);
				sized = m_wbAttributeDlg->m_SolidColorStatic.GetStringLength();
				m_wbAttributeDlg->m_SolidCheckBtn.MoveWindow(FUNCTIONATTRIBUTE_X + sized.cx + 5,145,20,20);
				m_wbAttributeDlg->m_SolidCheckStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + sized.cx + 30,150);
				m_wbAttributeDlg->m_FontStatic.MoveWindow(FUNCTIONATTRIBUTE_X,214,FUNCTIONATTRIBUTE_WIDTH,58);
				m_wbAttributeDlg->m_FontFaceComBox.MoveWindow(FUNCTIONATTRIBUTE_X,216,100,400);
				m_wbAttributeDlg->m_FontSizeComBox.MoveWindow(121,216,52,400);
				m_wbAttributeDlg->m_FontBoldBtn.MoveWindow(30,243,28,28);
				m_wbAttributeDlg->m_FontItalicBtn.MoveWindow(68,243,28,28);
				m_wbAttributeDlg->m_FontUnderlineBtn.MoveWindow(106,243,28,28);
				m_wbAttributeDlg->m_VerticalBtn.MoveWindow(144,243,28,28);
				m_wbAttributeDlg->m_SolidColorStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_SolidColorButton->ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_SolidCheckBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_SolidCheckStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineColorStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineColorButton->ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontFaceComBox.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontSizeComBox.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontBoldBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontItalicBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontUnderlineBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_VerticalBtn.ShowWindow(SW_SHOW);

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_SolidColorButton->Invalidate();
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				//m_wbAttributeDlg->UpdateData(FALSE);
				int j = m_wbAttributeDlg->m_FontFaceComBox.FindStringExact(0,g_nWBCurObjFontFaceName);	
				m_wbAttributeDlg->m_FontFaceComBox.SetCurSel(j);
				m_wbAttributeDlg->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
				m_wbAttributeDlg->UpdateData(FALSE);

				if (pWBTextCurItem->fontItem.lfWeight == FW_BOLD)
				{
					g_isBold = TRUE;					
				}
				if (pWBTextCurItem->fontItem.lfWeight == FW_NORMAL)
				{
					g_isBold = FALSE;					
				}
				m_wbAttributeDlg->m_FontBoldBtn.SetIsSelect(g_isBold);
				m_wbAttributeDlg->m_FontBoldBtn.Invalidate();
				m_wbAttributeDlg->m_FontItalicBtn.SetIsSelect(g_isItalic);
				m_wbAttributeDlg->m_FontItalicBtn.Invalidate();
				m_wbAttributeDlg->m_FontUnderlineBtn.SetIsSelect(g_isUnderline);
				m_wbAttributeDlg->m_FontUnderlineBtn.Invalidate();
				m_wbAttributeDlg->m_VerticalBtn.SetIsSelect(g_bIsVertical);
				m_wbAttributeDlg->m_VerticalBtn.Invalidate();
				delete pWBTextCurItem;
				if (pAnnotations->IsSolid())
				{
					g_bIsSolid = TRUE;    				
				} 
				else
				{
					g_bIsSolid = FALSE;
				}
				m_wbAttributeDlg->m_SolidColorButton->EnableWindow(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.SetIsSelect(g_bIsSolid);
				m_wbAttributeDlg->m_SolidCheckBtn.Invalidate();
				CWBPolygon::ClearItem(pWBPolygonCurItem);
				delete pWBPolygonCurItem;

			}
			break;
		case WB_TEXT_ATTRIBUTE:   //����ı� ���ı���עͬʱ����
			{
				CWBText *pCWBText = NULL;
				PWBTextItem pWBTextCurItem = NULL;
				if (g_pObj->GetType() != WB_OBJ_ANNOTATIONS)
				{
					pCWBText = (CWBText *)g_pObj;

					pWBTextCurItem = new WBTextItem;
					pCWBText->GetItem(pWBTextCurItem);				
					g_clrWBCurObjColor = pWBTextCurItem->clrText;
					g_nWBCurObjFontFaceName = pWBTextCurItem->fontItem.lfFaceName;
					g_nWBCurObjFontSize = pWBTextCurItem->fontItem.lfHeight;
					g_isItalic = pWBTextCurItem->fontItem.lfItalic;
					g_isUnderline = pWBTextCurItem->fontItem.lfUnderline;
					g_bIsVertical = pCWBText->GetIsVertical();
					g_nTranspar = (float)pCWBText->GetAlpha();	

					if (pWBTextCurItem->fontItem.lfWeight == FW_BOLD)
					{
						g_isBold = TRUE;					
					}
					if (pWBTextCurItem->fontItem.lfWeight == FW_NORMAL)
					{
						g_isBold = FALSE;					
					}
				}

				m_wbAttributeDlg->m_LineColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,96);
				m_wbAttributeDlg->m_LineColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,112,FUNCTIONATTRIBUTE_WIDTH,30);
				m_wbAttributeDlg->m_FontStatic.MoveWindow(FUNCTIONATTRIBUTE_X,152,FUNCTIONATTRIBUTE_WIDTH,58);
				m_wbAttributeDlg->m_FontFaceComBox.MoveWindow(FUNCTIONATTRIBUTE_X,154,100,400);
				m_wbAttributeDlg->m_FontSizeComBox.MoveWindow(121,154,52,400);
				m_wbAttributeDlg->m_FontBoldBtn.MoveWindow(30,181,28,28);
				m_wbAttributeDlg->m_FontItalicBtn.MoveWindow(68,181,28,28);
				m_wbAttributeDlg->m_FontUnderlineBtn.MoveWindow(106,181,28,28);
				m_wbAttributeDlg->m_VerticalBtn.MoveWindow(144,181,28,28);
				m_wbAttributeDlg->m_LineColorStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineColorButton->ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontFaceComBox.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontSizeComBox.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontBoldBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontItalicBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_FontUnderlineBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_VerticalBtn.ShowWindow(SW_SHOW);

				m_wbAttributeDlg->m_LineColorButton->Invalidate();
				m_wbAttributeDlg->m_TransparSlider.SetPos((int)((255-g_nTranspar)/2.55));
				m_wbAttributeDlg->m_nTranspar.Format(_T("%d"),(int)((255-g_nTranspar)/2.55));
				//m_wbAttributeDlg->UpdateData(FALSE);
				int j = m_wbAttributeDlg->m_FontFaceComBox.FindStringExact(0,g_nWBCurObjFontFaceName);	
				m_wbAttributeDlg->m_FontFaceComBox.SetCurSel(j);
				m_wbAttributeDlg->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
				m_wbAttributeDlg->UpdateData(FALSE);

				m_wbAttributeDlg->m_FontBoldBtn.SetIsSelect(g_isBold);
				m_wbAttributeDlg->m_FontBoldBtn.Invalidate();
				m_wbAttributeDlg->m_FontItalicBtn.SetIsSelect(g_isItalic);
				m_wbAttributeDlg->m_FontItalicBtn.Invalidate();
				m_wbAttributeDlg->m_FontUnderlineBtn.SetIsSelect(g_isUnderline);
				m_wbAttributeDlg->m_FontUnderlineBtn.Invalidate();
				m_wbAttributeDlg->m_VerticalBtn.SetIsSelect(g_bIsVertical);
				m_wbAttributeDlg->m_VerticalBtn.Invalidate();

				if (pWBTextCurItem != NULL)
				{
					delete pWBTextCurItem;
				}
			}
			break;
			//��������,��ͷ,�����������
		case WB_OBJ_POLYGON:
		case WB_OBJ_RECT:
		case WB_OBJ_PALLELOGAM:
		case WB_OBJ_ELLIPSE:
		case WB_OBJ_TRIANGLE:
		case WB_OBJ_PENTAGON://�����
		case WB_OBJ_HEXAGON://������
		case WB_OBJ_FIVEPOINTSTART:
		case WB_OBJ_TRAPEZIUM:
		case WB_OBJ_RHOMBUS:
		case WB_OBJ_SECTOR:
			{
				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.SolidColorStatic = 5;
				functionAttribute.SolidCheckBtn = 5;
				functionAttribute.SolidCheckStatic =5;
				functionAttribute.SolidColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();

			}
			break;
		case WB_OBJ_LINE://���,�����������
		case WB_OBJ_CURVE:
		case WB_OBJ_ARC:
			{
				WBFunctionAttribute functionAttribute;
				ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
				functionAttribute.LineColorStatic = 5;
				functionAttribute.LineColorButton = 5;
				functionAttribute.LineWidthStatic = 5;
				functionAttribute.LineWidthEdit = 5;
				functionAttribute.LineWidthCombx = 5;
				functionAttribute.LineTypeStatic = 5;
				functionAttribute.LineTypeCombx = 5;
				functionAttribute.ArrowStatic = 5;
				functionAttribute.ArrowCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawLine();

			}
			break;
		case WB_TRANS_COL://��ɫ ͸���� 
			{
				m_wbAttributeDlg->m_LineColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,96);
				m_wbAttributeDlg->m_LineColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,112,FUNCTIONATTRIBUTE_WIDTH,30);
				m_wbAttributeDlg->m_LineColorStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineColorButton->ShowWindow(SW_SHOW);

			}
			break;
		case WB_TRANS_COL_SOLID://��ɫ ͸���� ���
			{
				m_wbAttributeDlg->m_LineColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,96);
				m_wbAttributeDlg->m_LineColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,112,FUNCTIONATTRIBUTE_WIDTH,30);
				m_wbAttributeDlg->m_SolidColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,150);
				m_wbAttributeDlg->m_SolidColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,170,FUNCTIONATTRIBUTE_WIDTH,30);
				sized = m_wbAttributeDlg->m_SolidColorStatic.GetStringLength();
				m_wbAttributeDlg->m_SolidCheckBtn.MoveWindow(FUNCTIONATTRIBUTE_X + sized.cx + 5,145,20,20);
				m_wbAttributeDlg->m_SolidCheckStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + sized.cx + 30,150);
				m_wbAttributeDlg->m_LineColorStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineColorButton->ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_SolidColorStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_SolidColorButton->ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_SolidCheckBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_SolidCheckStatic.ShowWindow(SW_SHOW);
			}
			break;
		case WB_TRANS_COL_WIDTH://��ɫ ͸���� �߿�
			{
				m_wbAttributeDlg->m_LineColorStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,96);
				m_wbAttributeDlg->m_LineColorButton->MoveWindow(FUNCTIONATTRIBUTE_X,112,FUNCTIONATTRIBUTE_WIDTH,30);
				//m_wbAttributeDlg->m_LineWidthStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,150);
				//sized = m_wbAttributeDlg->m_LineWidthStatic.GetStringLength();
				//m_wbAttributeDlg->m_LineWidthEdit.MoveWindow(FUNCTIONATTRIBUTE_X + sized.cx + 5,150,40,20);
				//m_wbAttributeDlg->m_LineWidthCombx.MoveWindow(FUNCTIONATTRIBUTE_X,175,FUNCTIONATTRIBUTE_WIDTH,30);
				

				m_wbAttributeDlg->m_LineWidthStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,164);
				sized = m_wbAttributeDlg->m_LineWidthStatic.GetStringLength();
				m_wbAttributeDlg->m_LineWidthEdit.MoveWindow(FUNCTIONATTRIBUTE_X + sized.cx + 5,160,46,20);
				m_wbAttributeDlg->m_LineWidthCombx.MoveWindow(FUNCTIONATTRIBUTE_X,184,FUNCTIONATTRIBUTE_WIDTH,30);
				m_wbAttributeDlg->m_LineColorStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineColorButton->ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineWidthStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineWidthEdit.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_LineWidthCombx.ShowWindow(SW_SHOW);

			}
			break;
		default:
			break;
		}
		//m_wbAttributeDlg->m_FontSizeComBox.Invalidate();	
	}
	g_bAttriDlginitialize = FALSE;//�������ȥ��ʼ��

	if (g_bU7NewStyle)
	{
		m_wbAttributeDlg->m_FontStatic.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_FontFaceComBox.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_FontSizeComBox.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_FontBoldBtn.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_FontItalicBtn.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_FontUnderlineBtn.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_VerticalBtn.ShowWindow(SW_HIDE);
	}
	return 1;
}

//��ʾ���������-caoyuqin11.4.27
LRESULT CWBFunctionDlg::OnWM_OPENARRIBUTEDLG(WPARAM wParam, LPARAM lParam)
{	
	//::SendMessage(g_pWhiteBoardView->GetWBOptionDlg()->m_hWnd,WM_SHOWOPTIONBTN,2,0);

	 if(NULL != m_wbAttributeDlg)
	{
		m_wbAttributeDlg->CloseColorPaletteDlg();
	}

	if (NULL != m_wbResDlg)
	{
		//if (m_wbResDlg->IsWindowVisible())
		{
			m_wbResDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbPreviewDlg)
	{
		//if (m_wbPreviewDlg->IsWindowVisible())
		{
			m_wbPreviewDlg->ShowWindow(SW_HIDE);
		}		
	}
	if (NULL != m_wbBackgroundDlg)
	{
		//if (m_wbBackgroundDlg->IsWindowVisible())
		{
			m_wbBackgroundDlg->ShowWindow(SW_HIDE);
		}		
	}
	if (NULL != m_wbTeachToolDlg)
	{
		//if (m_wbTeachToolDlg->IsWindowVisible())
		{
			m_wbTeachToolDlg->ShowWindow(SW_HIDE);
		}
	}

	if (NULL == m_wbAttributeDlg)
	{
		if (g_nDrawStatus != DRAW_ERASING && g_nDrawStatus != DRAW_ERASE)
		{
			CRect rc;
			((CWhiteBoardDlg*)GetParent())->GetClientRect(rc);
			m_wbAttributeDlg = new CWBFunctionAttributeDlg;
			m_wbAttributeDlg->Create(IDD_WB_ATTRIBUTEDLG,this);
			m_wbAttributeDlg->MoveWindow(10,30,WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - 60);
			m_wbAttributeDlg->ShowWindow(SW_SHOW);

			WBFunctionAttribute functionAttribute;
			ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
			functionAttribute.LineColorStatic = 5;
			functionAttribute.LineColorButton = 5;
			functionAttribute.SolidColorStatic = 5;
			functionAttribute.SolidCheckBtn = 5;
			functionAttribute.SolidCheckStatic = 5;
			functionAttribute.SolidColorButton = 5;
			functionAttribute.LineWidthStatic = 5;
			functionAttribute.LineWidthEdit = 5;
			functionAttribute.LineWidthCombx = 5;
			functionAttribute.LineTypeStatic = 5;
			functionAttribute.LineTypeCombx = 5;
			functionAttribute.ArrowStatic = 5;
			functionAttribute.ArrowCombx = 5;
			functionAttribute.FontStatic = 5;
			functionAttribute.FontFaceComBox = 5;
			functionAttribute.FontSizeComBox = 5;
			functionAttribute.FontBoldBtn = 5;
			functionAttribute.FontItalicBtn = 5;
			functionAttribute.FontUnderlineBtn = 5;
			functionAttribute.VerticalBtn = 5;
			m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
			m_wbAttributeDlg->WBFunctionAttributeMoveWindowUnSelect();

			m_wbAttributeDlg->m_cAttMTOperate.ShowWindow(SW_HIDE);
			m_wbAttributeDlg->m_btnMTRotate.ShowWindow(SW_HIDE);
			m_wbAttributeDlg->m_btnMTZoom.ShowWindow(SW_HIDE);
			g_bAttriDlginitialize = TRUE;
		}
		else
		{
			CRect rc;
			((CWhiteBoardDlg*)GetParent())->GetClientRect(rc);
		
			m_wbAttributeDlg = new CWBFunctionAttributeDlg;
			m_wbAttributeDlg->Create(IDD_WB_ATTRIBUTEDLG,this);	
			m_wbAttributeDlg->MoveWindow(10,30,WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - 60);
			m_wbAttributeDlg->ShowWindow(SW_SHOW);
			m_wbAttributeDlg->WBFunctionAttributeHideWindow();
			m_wbAttributeDlg->WBFunctionAttributeSelectEraser();
		}
	} 
	else if (!m_wbAttributeDlg->IsWindowVisible())
	{
		m_wbAttributeDlg->ShowWindow(SW_SHOW);		
	}
	//��ȡѡ�ж����б�
	CWBList *pSelectObjLst = g_pWBPage->GetSelectObjLst();
	DWORD selectObjCounts = pSelectObjLst->GetSize();//��ȡѡ�ж���ĸ���
	if (0 == selectObjCounts)
	{
		if (g_bAttriDlginitialize)
		{
			;
		}
		else
		{
			::SendMessage(this->m_hWnd,WM_NOSELECTWBOBJ,0,0);
		}			
	}
	if (1 == selectObjCounts)//����ѡ��Ϣ
	{
		::SendMessage(this->m_hWnd,WM_SELECTWBOBJ,0,selectObjCounts);
	}
	if (selectObjCounts > 1)//���Ͷ�ѡ��Ϣ
	{
		g_pWBPage->SelectMoreObjAttribute();
	}
	return 1;
}

//����ѡ�񻭱ʵ������Զ������������-caoyuqin
LRESULT CWBFunctionDlg::OnWM_SELECTWBPEN(WPARAM wParam, LPARAM lParam)
{
	//CSize nsize;
	if (NULL != m_wbAttributeDlg)
	{
		m_wbAttributeDlg->ResetVScroll(SB_TOP);
		CString strTranspar;
		strTranspar = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgTransparStatic"));
		if (strTranspar.IsEmpty())
		{
			strTranspar = _T("͸����");
		}
		m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_SHOW); //��ʾ�����ĵ���͸����
		m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_TransparStatic.SetCaption(strTranspar);
		m_wbAttributeDlg->m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
		m_wbAttributeDlg->m_TransparStatic.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_PerStatic.ShowWindow(SW_SHOW);
		m_wbAttributeDlg->m_cAttMTOperate.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_btnMTRotate.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_btnMTZoom.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_BrushSizeCombx.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_BrushSizeStatic.ShowWindow(SW_HIDE);
		m_wbAttributeDlg->m_DefaultBackGround.ShowWindow(SW_HIDE);

		switch(g_nDrawStatus)
		{
		case DRAW_NORMAL:
			{
				int i = 0;
				i = (g_pWBPage->GetSelectObjLst())->GetCount();//�õ�ѡ�ж����б��еĶ������
				if (i > 0)
				{
					break;
				} 
				else
				{
					
					//��������Ƿ��ʼ��
					if (g_bAttriDlginitialize)
					{
						break;
					} 
					else
					{
						::SendMessage(this->m_hWnd,WM_NOSELECTWBOBJ,0,0);
					}					
				}				
			}
			break;
		case DRAW_LINE://��ֱ��ʱ
		case DRAW_POLYLINE://������ʱ
			{
				WBFunctionAttribute FunctionAttribute;
				::ZeroMemory(&FunctionAttribute,sizeof(WBFunctionAttribute));
				FunctionAttribute.LineColorStatic = 5;
				FunctionAttribute.LineColorButton = 5;
				FunctionAttribute.LineWidthStatic = 5;
				FunctionAttribute.LineWidthEdit = 5;
				FunctionAttribute.LineWidthCombx = 5;
				FunctionAttribute.LineTypeStatic = 5;
				FunctionAttribute.LineTypeCombx = 5;
				FunctionAttribute.ArrowStatic = 5;
				FunctionAttribute.ArrowCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(FunctionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawLine();
				g_bAttriDlginitialize = FALSE;
			}
			break;
		case DRAW_POLYICON: //��ͼ������ʱ
			{
				WBFunctionAttribute FunctionAttribute;
				::ZeroMemory(&FunctionAttribute,sizeof(WBFunctionAttribute));
				FunctionAttribute.LineWidthCombx = 5;
				FunctionAttribute.LineWidthEdit = 5;
				FunctionAttribute.LineWidthStatic = 5;
				FunctionAttribute.IcoStyleComBox = 5;
				FunctionAttribute.IcoStyleStatic = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(FunctionAttribute);
				m_wbAttributeDlg->WBFuncitonAttributeMoveWindowDrawIcoCurve();
				//m_wbAttributeDlg->SetComboxItem();
				g_bAttriDlginitialize = FALSE;
			}
			break;
		case  DRAW_RECT://������ʱ
		case  DRAW_ELLIPSE://����Բʱ
		case  DRAW_TRIANGLE://��������ʱ
		case  DRAW_RHOMBUS://������ʱ
		case  DRAW_PENTAGON://�������ʱ
		case DRAW_HEXAGON://��������ʱ
		case DRAW_FIVEPOINTSTART:
		case DRAW_TRAPEZIUM:
		case DRAW_POLYGON://�������ʱ
			{
				WBFunctionAttribute FunctionAttribute;
				::ZeroMemory(&FunctionAttribute,sizeof(WBFunctionAttribute));
				FunctionAttribute.LineColorStatic = 5;
				FunctionAttribute.LineColorButton = 5;
				FunctionAttribute.SolidColorStatic = 5;
				FunctionAttribute.SolidCheckBtn = 5;
				FunctionAttribute.SolidCheckStatic = 5;
				FunctionAttribute.SolidColorButton = 5;
				FunctionAttribute.LineWidthStatic = 5;
				FunctionAttribute.LineWidthEdit = 5;
				FunctionAttribute.LineWidthCombx = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(FunctionAttribute);
				m_wbAttributeDlg->WBFunctionAttributeMoveWindowDrawPology();
				
				g_bAttriDlginitialize = FALSE;
			}
			break;
		case ADDDXF: //dxf add by jiangchao 2013.08.26
		case ADDPICTURE: //ͼƬ
		case ADDFLASH: //flash
			{
				WBFunctionAttribute FunctionAttribute;
				::ZeroMemory(&FunctionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(FunctionAttribute);
				g_bAttriDlginitialize = FALSE;
			}
			break;
		case ADDDOC: //�ĵ�
			{
				WBFunctionAttribute FunctionAttribute;
				::ZeroMemory(&FunctionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(FunctionAttribute);

				m_wbAttributeDlg->m_cAttMTOperate.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_cAttMTOperate.MoveWindow(FUNCTIONATTRIBUTE_X,100,100,15);
				m_wbAttributeDlg->m_btnMTRotate.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_btnMTRotate.MoveWindow(FUNCTIONATTRIBUTE_X,115,64,28);
				m_wbAttributeDlg->m_btnMTZoom.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_btnMTZoom.MoveWindow(95,115,64,28);
				g_bAttriDlginitialize = FALSE;
			}
			break;
		case ADD_ANNOTATIONS: //��ע
		case DRAW_TEXT: //����
			{
				WBFunctionAttribute FunctionAttribute;
				::ZeroMemory(&FunctionAttribute,sizeof(WBFunctionAttribute));
				FunctionAttribute.LineColorButton = 5;
				FunctionAttribute.LineColorStatic = 5;
				FunctionAttribute.FontBoldBtn = 5;
				FunctionAttribute.FontFaceComBox = 5;
				FunctionAttribute.FontItalicBtn = 5;
				FunctionAttribute.FontSizeComBox = 5;
				FunctionAttribute.FontStatic = 5;
				FunctionAttribute.FontUnderlineBtn = 5;
				FunctionAttribute.VerticalBtn = 5;
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(FunctionAttribute);
				m_wbAttributeDlg->m_FontStatic.MoveWindow(FUNCTIONATTRIBUTE_X,152,FUNCTIONATTRIBUTE_WIDTH,58);				
				m_wbAttributeDlg->m_FontFaceComBox.MoveWindow(FUNCTIONATTRIBUTE_X,154,100,400);
				m_wbAttributeDlg->m_FontSizeComBox.MoveWindow(121,154,52,400);
				m_wbAttributeDlg->m_FontBoldBtn.MoveWindow(30,181,28,28);
				m_wbAttributeDlg->m_FontItalicBtn.MoveWindow(68,181,28,28);
				m_wbAttributeDlg->m_FontUnderlineBtn.MoveWindow(106,181,28,28);
				m_wbAttributeDlg->m_VerticalBtn.MoveWindow(144,181,28,28);

				//m_wbAttributeDlg->m_FontSizeComBox.Invalidate();
				g_bAttriDlginitialize = FALSE;
			}
			break;
		case ADDCAMERA://����ͷ
		case ADDNEWMOVIE: //��Ƶ
			{
				CString strNoAttribute;
				strNoAttribute = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgNoAttribute"));
				if (strNoAttribute.IsEmpty())
				{
					strNoAttribute = _T("������");
				}
				WBFunctionAttribute FunctionAttribute;
				::ZeroMemory(&FunctionAttribute,sizeof(WBFunctionAttribute));
				m_wbAttributeDlg->WBFunctionAttributeShowWindow(FunctionAttribute);

				if (!g_bU7NewStyle)
				{
					m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_HIDE);
					m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_HIDE);
				}
				
				m_wbAttributeDlg->m_TransparStatic.SetCaption(strNoAttribute);
				m_wbAttributeDlg->m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
				m_wbAttributeDlg->m_PerStatic.ShowWindow(SW_HIDE);
				g_bAttriDlginitialize = FALSE;
			}
		default:
			break;
		}	
	
		if (g_bU7NewStyle)
		{
			m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_SHOW); 
			m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparStatic.ShowWindow(SW_SHOW);
		}
	}
	return 1;
}

// δѡ���κζ���ʱ�����������-caoyuqin
LRESULT CWBFunctionDlg::OnWM_NOSELECTWBOBJ(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_wbAttributeDlg/* || !m_wbAttributeDlg->IsWindowVisible()*/)//�������Ϊ��ʱ �����������ʱ
	{
		return 0;
	}
	else//��ʼ���������
	{
		//�ѹ����������ö�
		if (NULL != m_wbAttributeDlg)
		{
			GetAttributeDlg()->ResetVScroll(SB_TOP);
		}

		if (g_nDrawStatus != DRAW_ERASE && g_nDrawStatus != DRAW_ERASING)
		{
			//m_wbAttributeDlg->WBFunctionAttributeHideWindow();
			CString strTranspar;
			strTranspar = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgTransparStatic"));
			if (strTranspar.IsEmpty())
			{
				strTranspar = _T("͸����");
			}
			m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_SHOW); //��ʾ�����ĵ���͸����
			m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparStatic.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparStatic.SetCaption(strTranspar);
			m_wbAttributeDlg->m_PerStatic.ShowWindow(SW_SHOW);
			if (g_pWBPage->GetIsExistBackGround())
			{
				m_wbAttributeDlg->m_DefaultBackGround.ShowWindow(SW_SHOW);
			}
			else
			{
				m_wbAttributeDlg->m_DefaultBackGround.ShowWindow(SW_HIDE);
			}

			WBFunctionAttribute functionAttribute;
			ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
			functionAttribute.LineColorStatic = 5;
			functionAttribute.LineColorButton = 5;
			functionAttribute.SolidColorStatic = 5;
			functionAttribute.SolidCheckBtn = 5;
			functionAttribute.SolidCheckStatic = 5;
			functionAttribute.SolidColorButton = 5;
			functionAttribute.LineWidthStatic = 5;
			functionAttribute.LineWidthEdit = 5;
			functionAttribute.LineWidthCombx = 5;
			functionAttribute.LineTypeStatic = 5;
			functionAttribute.LineTypeCombx = 5;
			functionAttribute.ArrowStatic = 5;
			functionAttribute.ArrowCombx = 5;
			functionAttribute.FontStatic = 5;
			functionAttribute.FontFaceComBox = 5;
			functionAttribute.FontSizeComBox = 5;
			functionAttribute.FontBoldBtn = 5;
			functionAttribute.FontItalicBtn = 5;
			functionAttribute.FontUnderlineBtn = 5;
			functionAttribute.VerticalBtn = 5;
			m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
			m_wbAttributeDlg->WBFunctionAttributeMoveWindowUnSelect();

			//m_wbAttributeDlg->m_FontSizeComBox.Invalidate();
			m_wbAttributeDlg->m_cAttMTOperate.ShowWindow(SW_HIDE);
			m_wbAttributeDlg->m_btnMTRotate.ShowWindow(SW_HIDE);
			m_wbAttributeDlg->m_btnMTZoom.ShowWindow(SW_HIDE);
			m_wbAttributeDlg->m_BrushSizeCombx.ShowWindow(SW_HIDE);
			m_wbAttributeDlg->m_BrushSizeStatic.ShowWindow(SW_HIDE);
			g_bAttriDlginitialize = TRUE;
			return 1;
		}
		else
		{
			m_wbAttributeDlg->WBFunctionAttributeHideWindow();
			m_wbAttributeDlg->WBFunctionAttributeSelectEraser();
			
		}

		if (g_bU7NewStyle)
		{
			m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_SHOW); //��ʾ�����ĵ���͸����
			m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_SHOW);
			m_wbAttributeDlg->m_TransparStatic.ShowWindow(SW_SHOW);
		}

	}
	return 1;
}
BOOL CWBFunctionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//m_cFuncdlgHideStatic.ShowWindow(SW_HIDE);
	//m_cHideFuncdlgButton.ShowWindow(SW_HIDE);
	//CString HideStatic;
	//HideStatic = ReadConfigText(_T("WhiteBoardFunctionBoardText"),_T("FunctiondlgHideStatic"));
	//if (HideStatic.IsEmpty())
	//{
	//	HideStatic = _T("�Զ�����");
	//} 
	//m_cFuncdlgHideStatic.SetCaption(HideStatic);
	//m_cFuncdlgHideStatic.SetTextPosition(TRUE);
	//CRect rc;
	//g_whiteboardDlg->GetWindowRect(rc);
	//MoveWindow(rc.right  
	//	,rc.top + 50
	//	,WB_FUNCTION_DLG_WIDTH
	//	,rc.bottom - rc.top - 265);

	if (g_bU7NewStyle)
	{
		if (NULL == m_wbAttributeDlg)
		{

			CRect rc;
			((CWhiteBoardDlg*)GetParent())->GetClientRect(rc);
			m_wbAttributeDlg = new CWBFunctionAttributeDlg;
			m_wbAttributeDlg->Create(IDD_WB_ATTRIBUTEDLG,this);
			m_wbAttributeDlg->MoveWindow(10,30,WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - 60);
			m_wbAttributeDlg->ShowWindow(SW_SHOW);

			WBFunctionAttribute functionAttribute;
			ZeroMemory(&functionAttribute,sizeof(WBFunctionAttribute));
			functionAttribute.LineColorStatic = 5;
			functionAttribute.LineColorButton = 5;
			functionAttribute.SolidColorStatic = 5;
			functionAttribute.SolidCheckBtn = 5;
			functionAttribute.SolidCheckStatic = 5;
			functionAttribute.SolidColorButton = 5;
			functionAttribute.LineWidthStatic = 5;
			functionAttribute.LineWidthEdit = 5;
			functionAttribute.LineWidthCombx = 5;
			functionAttribute.LineTypeStatic = 5;
			functionAttribute.LineTypeCombx = 5;
			functionAttribute.ArrowStatic = 5;
			functionAttribute.ArrowCombx = 5;
			functionAttribute.FontStatic = 5;
			functionAttribute.FontFaceComBox = 5;
			functionAttribute.FontSizeComBox = 5;
			functionAttribute.FontBoldBtn = 5;
			functionAttribute.FontItalicBtn = 5;
			functionAttribute.FontUnderlineBtn = 5;
			functionAttribute.VerticalBtn = 5;
			m_wbAttributeDlg->WBFunctionAttributeShowWindow(functionAttribute);
			m_wbAttributeDlg->WBFunctionAttributeMoveWindowUnSelect();

			m_wbAttributeDlg->m_cAttMTOperate.ShowWindow(SW_HIDE);
			m_wbAttributeDlg->m_btnMTRotate.ShowWindow(SW_HIDE);
			m_wbAttributeDlg->m_btnMTZoom.ShowWindow(SW_HIDE);
			g_bAttriDlginitialize = TRUE;
		}
	}
	else
	{
		if (NULL == m_wbPreviewDlg)
		{

			m_PreviewRect.SetRect(m_rect.left + 10,m_rect.top + 30,m_rect.left + WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - 30);
			m_wbPreviewDlg = new CWBPreviewDlg(m_PreviewRect,NULL);
			m_wbPreviewDlg->Create(IDD_WB_PREVIEWDLG,this);		
			m_wbPreviewDlg->ShowWindow(SW_SHOW);	
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

LRESULT CWBFunctionDlg::OnWM_OPENPREVIEWDLG(WPARAM wParam, LPARAM lParam)
{
	//::SendMessage(g_pWhiteBoardView->GetWBOptionDlg()->m_hWnd,WM_SHOWOPTIONBTN,1,0);
	if(NULL != m_wbAttributeDlg)
	{
		m_wbAttributeDlg->CloseColorPaletteDlg();
	}

	if (NULL != m_wbResDlg)
	{
		//if (m_wbResDlg->IsWindowVisible())
		{
			m_wbResDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbAttributeDlg)
	{
		//if (m_wbAttributeDlg->IsWindowVisible())
		{
			m_wbAttributeDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbBackgroundDlg)
	{
	//	if (m_wbBackgroundDlg->IsWindowVisible())
		{
			m_wbBackgroundDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbTeachToolDlg)
	{
		//if (m_wbTeachToolDlg->IsWindowVisible())
		{
			m_wbTeachToolDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL == m_wbPreviewDlg)
	{
		CRect rc;
		((CWhiteBoardDlg*)GetParent())->GetClientRect(rc);
		m_PreviewRect.SetRect(m_rect.left + 10,m_rect.top + 30,m_rect.left + WB_FUNCTION_DLG_WIDTH - 15,rc.bottom - 30);
		m_wbPreviewDlg = new CWBPreviewDlg(m_PreviewRect,NULL);
		m_wbPreviewDlg->Create(IDD_WB_PREVIEWDLG,this);
		m_wbPreviewDlg->MoveWindow(m_rect.left + 10,m_rect.top + 30,WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - 60);
		m_wbPreviewDlg->ShowWindow(SW_SHOW);		
	}
	else
	{
		if (!m_wbPreviewDlg->IsWindowVisible())
		{
			m_wbPreviewDlg->ShowWindow(SW_SHOW);		
			m_wbPreviewDlg->RefurbishPreview();
		}
	}
	return 1;
}

LRESULT CWBFunctionDlg::OnWM_OPENBACKGROUNDDLG( WPARAM wParam, LPARAM lParam )
{
	//::SendMessage(g_pWhiteBoardView->GetWBOptionDlg()->m_hWnd,WM_SHOWOPTIONBTN,4,0);
	if(NULL != m_wbAttributeDlg)
	{
		m_wbAttributeDlg->CloseColorPaletteDlg();
	}
	if (NULL != m_wbPreviewDlg)
	{
	//	if (m_wbPreviewDlg->IsWindowVisible())
		{
			m_wbPreviewDlg ->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbAttributeDlg)
	{
		//if (m_wbAttributeDlg->IsWindowVisible())
		{
			m_wbAttributeDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbTeachToolDlg)
	{
		//if (m_wbTeachToolDlg->IsWindowVisible())
		{
			m_wbTeachToolDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbResDlg)
	{
		//if (m_wbResDlg->IsWindowVisible())
		{
			m_wbResDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL == m_wbBackgroundDlg)
	{
		CRect rc;
		((CWhiteBoardDlg*)GetParent())->GetClientRect(rc);
		m_wbBackgroundDlg = new CWBBackgroundDlg;
		m_wbBackgroundDlg->Create(IDD_SINWB_BACKGROUNDDLG,this);
		m_wbBackgroundDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		if (!m_wbBackgroundDlg->IsWindowVisible())
		{
			m_wbBackgroundDlg->ShowWindow(SW_SHOW);
		}
	}			
	return 1;
}

void CWBFunctionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect nParentRc;
	((CWhiteBoardDlg *)GetParent())->GetClientRect(&nParentRc);
	CRect FuncRect;
	GetClientRect(FuncRect);
	m_rect = FuncRect;

	//m_rect.left = nParentRc.right - WB_FUNCTION_DLG_WIDTH - WB_OPTIONDLG_WIDTH - 20;
	//m_rect.top = nParentRc.top;
	//m_rect.right = m_rect.left + WB_FUNCTION_DLG_WIDTH;
	//m_rect.bottom = nParentRc.bottom;


	//if (m_btnCloseFuncPad.GetSafeHwnd())
	//{
	//	m_btnCloseFuncPad.MoveWindow(WB_FUNCTION_DLG_WIDTH - 36,2,30,28);
	//	m_btnCloseFuncPad.ShowWindow(SW_HIDE); //���ع������Ĺرհ�ť
	//}
	//if (m_cFuncdlgHideStatic.GetSafeHwnd())  //�Զ������ı�
	//{
	//	m_cFuncdlgHideStatic.MoveWindow(45,m_rect.bottom - 28,54,18);
	//}
	/*	if (m_cHideFuncdlgButton.GetSafeHwnd()) //�Զ����ذ�ť
	{
	m_cHideFuncdlgButton.MoveWindow(20,m_rect.bottom - 30,20,20);
	}*/	
	if (m_wbAttributeDlg != NULL)
	{
		m_wbAttributeDlg->MoveWindow(10,30 ,WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - 60);
	}
	if (m_wbResDlg != NULL)
	{
		m_wbResDlg->MoveWindow(10,30 ,WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - m_rect.top - 60);
	}
	if (m_wbBackgroundDlg != NULL)
	{
		m_wbBackgroundDlg->MoveWindow(10,30 ,WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - m_rect.top - 60);
	}
	if (m_wbPreviewDlg != NULL)
	{
		CRect Temprect;
		GetClientRect(Temprect);
		m_PreviewRect.left = Temprect.left +10;
		m_PreviewRect.right = Temprect.left + WB_FUNCTION_DLG_WIDTH - 15;
		m_PreviewRect.top = Temprect.top + 30;
		m_PreviewRect.bottom = Temprect.bottom - 30;

		m_wbPreviewDlg->SetPreviewRect(m_PreviewRect);
		m_wbPreviewDlg->MoveWindow(10,30 ,WB_FUNCTION_DLG_WIDTH - 15,m_PreviewRect.bottom - 30);
		m_wbPreviewDlg->RefurbishPreview();
	}
	if (m_wbTeachToolDlg != NULL)
	{
		m_wbTeachToolDlg->MoveWindow(10,30 ,WB_FUNCTION_DLG_WIDTH - 15,m_rect.bottom - m_rect.top - 60);
	}
}
void CWBFunctionDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

CWBFunctionAttributeDlg *CWBFunctionDlg::GetAttributeDlg()
{
	return m_wbAttributeDlg;
}

void CWBFunctionDlg::OnOK()//���λس���
{
	return;
}
CWBPreviewDlg* CWBFunctionDlg::GetPreviewDLG(void)//��ȡԤ���Ի���ָ��
{
	return m_wbPreviewDlg;
}

CWBBackgroundDlg* CWBFunctionDlg::GetBackGroundDlg()
{
	return m_wbBackgroundDlg;
}
BOOL CWBFunctionDlg::OnEraseBkgnd(CDC* pDC)
{
	CString CurPicPath = GetCurrentPath();	//��ȡ��ǰ����ִ�е�Ŀ¼
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	//if (g_WBType == WB_SINGLERVER)
	//{
		CurPicPath += "\\SKIN\\WhiteBoard\\image\\functionDlg\\SinWBFunctionDlg.png";
// 	}
// 	else
// 	{
// 		CurPicPath += "\\SKIN\\WhiteBoard\\image\\functionDlg\\m_wbFunctionDlg.png";
// 	}
	
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	

	return TRUE;
}
void CWBFunctionDlg::SetBtnState(int BtnId,WB_MOUSE_STATE MouseState)
{
	switch (BtnId)
	{
	//case IDC_SINWB_CLOSEFUNCPAD:
	//	m_btnCloseFuncPad.SetMouseState(MouseState);
	//	break;
	default:
		break;
	}
}
void CWBFunctionDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CDialogEx::PostNcDestroy();
}

//�������ϵĵ�ɫ����ɫ����������ϵ���ɫ��ť����һ��
LRESULT CWBFunctionDlg::OnWM_PALETTECOLOR(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_wbAttributeDlg)//�������Ϊ��ʱ
	{
		return 0;
	}
	if (!m_wbAttributeDlg->IsWindowVisible())//�����������ʱ
	{
		return 0;
	}
	m_wbAttributeDlg->m_LineColorButton->Invalidate();
	m_wbAttributeDlg->m_LineColorButton->UpdateData(FALSE);
	m_wbAttributeDlg->m_BrushSizeCombx.ShowWindow(SW_HIDE);
	m_wbAttributeDlg->m_BrushSizeStatic.ShowWindow(SW_HIDE);
	m_wbAttributeDlg->m_DefaultBackGround.ShowWindow(SW_HIDE);
	return 1;
}

void CWBFunctionDlg::OnBnClickedClosewbfunctiondlg()
{
	//((CWhiteBoardDlg *)GetParent())->DeleteFuncPad();
	// g_pWhiteBoardView->DeleteFuncPad();
	//((CWhiteBoardDlg *)(g_pWhiteBoardView->GetParent()))->MoveCWBView();
	return;
}

void CWBFunctionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnLButtonDown(nFlags, point);
	if (m_wbAttributeDlg)
	{
		m_wbAttributeDlg->CloseColorPaletteDlg();
	}
}

LRESULT CWBFunctionDlg::OnWM_OPENTEACHTOOLDLG( WPARAM wParam, LPARAM lParam )
{
	//::SendMessage(g_pWhiteBoardView->GetWBOptionDlg()->m_hWnd,WM_SHOWOPTIONBTN,5,0);
	if(NULL != m_wbAttributeDlg)
	{
		m_wbAttributeDlg->CloseColorPaletteDlg();
	}
	if (NULL != m_wbPreviewDlg)
	{
		//if (m_wbPreviewDlg->IsWindowVisible())
		{
			m_wbPreviewDlg ->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbBackgroundDlg)
	{
		//if (m_wbBackgroundDlg->IsWindowVisible())
		{
			m_wbBackgroundDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbResDlg)
	{
		//if (m_wbResDlg->IsWindowVisible())
		{
			m_wbResDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbAttributeDlg)
	{
		//if (m_wbAttributeDlg->IsWindowVisible())
		{
			m_wbAttributeDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL == m_wbTeachToolDlg)
	{
		CRect rc;
		((CWhiteBoardDlg*)GetParent())->GetClientRect(rc);
		m_wbTeachToolDlg = new CWBTeachToolDlg;
		m_wbTeachToolDlg->Create(IDD_WB_TEACHTOOLDLG,this);
		m_wbTeachToolDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		if (!m_wbTeachToolDlg->IsWindowVisible())
		{
			m_wbTeachToolDlg->ShowWindow(SW_SHOW);
		}
	}	
	return 1;
}

CWBTeachToolDlg * CWBFunctionDlg::GetTeachtoolDlg()
{
	return m_wbTeachToolDlg;
}

LRESULT CWBFunctionDlg::OnWM_SELECTWBERASER( WPARAM wParam, LPARAM lParam )
{
	if (NULL != m_wbAttributeDlg)
	{
		m_wbAttributeDlg->WBFunctionAttributeHideWindow();
		
		switch(g_nDrawStatus)
		{
		case DRAW_ERASE:
			{
				m_wbAttributeDlg->WBFunctionAttributeSelectEraser();
				CString str;
				str = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgBrushSizeStatic "));
				if (str.IsEmpty())
				{
					str = _T("���Ӵ�С");
				}
				m_wbAttributeDlg->m_BrushSizeStatic.SetCaption(str);
				m_wbAttributeDlg->m_BrushSizeStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_BrushSizeCombx.ShowWindow(SW_SHOW);
			}
			break;
		case DRAW_CLEANUP:
		case DRAW_RGNDELETE:
			{
				CString strTranspar;
				strTranspar = ReadConfigText(_T("WhiteBoardFunctionBoardText"), _T("AttributeDlgTransparStatic"));
				if (strTranspar.IsEmpty())
				{
					strTranspar = _T("͸����");
				}
				m_wbAttributeDlg->m_cReduceTransperBtn.ShowWindow(SW_SHOW); //��ʾ�����ĵ���͸����
				m_wbAttributeDlg->m_cAddTransparBtn.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_TransparSlider.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_TransparEdit.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_TransparStatic.ShowWindow(SW_SHOW);
				m_wbAttributeDlg->m_cReduceTransperBtn.MoveWindow(FUNCTIONATTRIBUTE_X,58,30,30);
				m_wbAttributeDlg->m_cAddTransparBtn.MoveWindow(145,58,30,30);
				m_wbAttributeDlg->m_TransparSlider.MoveWindow(53,58,90,30);
				m_wbAttributeDlg->m_TransparStatic.SetCaption(strTranspar);
				m_wbAttributeDlg->m_TransparStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X,35);
				CSize nsize;
				nsize = m_wbAttributeDlg->m_TransparStatic.GetStringLength();
				m_wbAttributeDlg->m_TransparEdit.MoveWindow(FUNCTIONATTRIBUTE_X + nsize.cx + 5,31,40,20);
				m_wbAttributeDlg->m_PerStatic.MoveStaticPos(FUNCTIONATTRIBUTE_X + nsize.cx + 50,35);
				m_wbAttributeDlg->m_PerStatic.ShowWindow(SW_SHOW);
				//����ˢ�Ӵ�С��ť
				m_wbAttributeDlg->m_BrushSizeStatic.ShowWindow(SW_HIDE);
				m_wbAttributeDlg->m_BrushSizeCombx.ShowWindow(SW_HIDE);
			}
			break;
		default:
			break;
		}
	}
	return 1;
}



void CWBFunctionDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	g_pWhiteBoardView->OnLButtonUp(nFlags, point);
	//CDialogEx::OnLButtonUp(nFlags, point);
}

LRESULT CWBFunctionDlg::OnWM_OPENRESDLG( WPARAM wParam, LPARAM lParam )
{
	//::SendMessage(g_pWhiteBoardView->GetWBOptionDlg()->m_hWnd,WM_SHOWOPTIONBTN,3,0);
	if (NULL != m_wbAttributeDlg)
	{
		m_wbAttributeDlg->CloseColorPaletteDlg();

	}
	if (NULL != m_wbPreviewDlg)
	{
		//if (m_wbPreviewDlg->IsWindowVisible())
		{
			m_wbPreviewDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbAttributeDlg)
	{
		//if (m_wbAttributeDlg->IsWindowVisible())
		{
			m_wbAttributeDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbBackgroundDlg)
	{
		//if (m_wbBackgroundDlg->IsWindowVisible())
		{
			m_wbBackgroundDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL != m_wbTeachToolDlg)
	{
		//if (m_wbTeachToolDlg->IsWindowVisible())
		{
			m_wbTeachToolDlg->ShowWindow(SW_HIDE);
		}
	}
	if (NULL == m_wbResDlg)
	{
		CRect rc;
		((CWhiteBoardDlg*)GetParent())->GetClientRect(rc);
		m_wbResDlg = new CWBFunctionResDlg;
		m_wbResDlg->Create(IDD_SINWB_RESDLG,this);
		m_wbResDlg->ShowWindow(SW_SHOW);
		if (m_wbResDlg->GetResTree() == NULL)
		{
			m_wbResDlg->CreateResTree();
			m_wbResDlg->CreateResListCtl();
		}
	}
	else
	{
		if (!m_wbResDlg->IsWindowVisible())
		{
			m_wbResDlg->ShowWindow(SW_SHOW);
		}
	}

	return 1;
}

void CWBFunctionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (g_bAvconLock)
	{
		return;
	}
	POINT pt;
	::GetCursorPos(&pt);
	CRect rect;
	CRect rc;
	CRect rcColorPlate;

	if (this->IsWindowVisible()   //��ȡ�����������ɫ�Ի���
		&& GetAttributeDlg() 
		&& GetAttributeDlg()->IsWindowVisible()
		&& GetAttributeDlg()->m_pAttriButeColorPatetteDlg
		&& GetAttributeDlg()->m_pAttriButeColorPatetteDlg->IsWindowVisible())
	{
		GetAttributeDlg()->m_pAttriButeColorPatetteDlg->GetWindowRect(rcColorPlate);
	}

	if (this->IsWindowVisible()    //��ȡ������������ɫ�Ի���
		&& GetAttributeDlg() 
		&& GetAttributeDlg()->IsWindowVisible()
		&& GetAttributeDlg()->m_pAttriButeSolidPatetteDlg
		&& GetAttributeDlg()->m_pAttriButeSolidPatetteDlg->IsWindowVisible())
	{
		GetAttributeDlg()->m_pAttriButeSolidPatetteDlg->GetWindowRect(rcColorPlate);
	}

	if (this->IsWindowVisible()   //��ȡ��������ɫ�Ի���
		&& m_wbBackgroundDlg
		&& m_wbBackgroundDlg->IsWindowVisible()
		&& m_wbBackgroundDlg->GetColorPaletteDlg()
		&& m_wbBackgroundDlg->GetColorPaletteDlg()->IsWindowVisible())
	{
		m_wbBackgroundDlg->GetColorPaletteDlg()->GetWindowRect(rcColorPlate);
	}

	GetWindowRect(rect);
	
	((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->GetWindowRect(rc);

	if (this->IsWindowVisible() &&  PtInRect(rect,pt))   //�ڹ��������
	{
		ShowWindow(SW_SHOW);
		m_dLeaveTime = GetTickCount();
		return;
	}

	else if (this->IsWindowVisible()  //�����е���ɫ�Ի���
		&& GetAttributeDlg() 
		&& GetAttributeDlg()->IsWindowVisible()
		&& GetAttributeDlg()->m_pAttriButeColorPatetteDlg
		&& GetAttributeDlg()->m_pAttriButeColorPatetteDlg->IsWindowVisible()
		&& PtInRect(rcColorPlate, pt))
	{
		ShowWindow(SW_SHOW);
		m_dLeaveTime = GetTickCount();
		return;
	}

	else if (this->IsWindowVisible()  //�����е������ɫ�Ի���
		&& GetAttributeDlg() 
		&& GetAttributeDlg()->IsWindowVisible()
		&& GetAttributeDlg()->m_pAttriButeSolidPatetteDlg
		&& GetAttributeDlg()->m_pAttriButeSolidPatetteDlg->IsWindowVisible()
		&& PtInRect(rcColorPlate, pt))
	{
		ShowWindow(SW_SHOW);
		m_dLeaveTime = GetTickCount();
		return;
	}

	else if (this->IsWindowVisible()   //�����е���ɫ�Ի���
		&& m_wbBackgroundDlg
		&& m_wbBackgroundDlg->IsWindowVisible()
		&& m_wbBackgroundDlg->GetColorPaletteDlg()
		&& m_wbBackgroundDlg->GetColorPaletteDlg()->IsWindowVisible()
		&& PtInRect(rcColorPlate, pt))
	{
		ShowWindow(SW_SHOW);
		m_dLeaveTime = GetTickCount();
		return;
	}
	else if (this->IsWindowVisible() && !PtInRect(rect, pt) && !PtInRect(rc, pt))
	{
		//�ж�Ԥ������е���˵���ť �����˵��� �����ڸ��ǹ������Ĳ˵����� ʹ��Show���� 
		if (m_wbPreviewDlg
			&& m_wbPreviewDlg->IsWindowVisible()
			&&PtInRect(m_rectPreviewMenu, pt))
		{
			ShowWindow(SW_SHOW);
			m_dLeaveTime = GetTickCount();
			return;
		}

		if (nIDEvent == ID_TIMER_HIDETIME)
		{
			DiffTimeHideFunctionDlg();
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CWBFunctionDlg::ShowOrHideFunctionDlg( bool bIsShow )
{
	if (NULL ==g_whiteboardDlg ||g_pWhiteBoardView == NULL 
		|| (((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()) == NULL||g_bAvconLock)
	{
		return;
	}

	CRect rct;
	//CRect ViewRect;
	g_pWhiteBoardView->GetWindowRect(rct);
	//g_whiteboardDlg->GetClientRect(rct);
	if(bIsShow)
	{
		if (++m_nShowOrHideCount < 5)
		{
			if (rct.Height() >= 250 && rct.Height() < 350)
			{
				if (((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg())
				{
					((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg()->ShowWindow(SW_HIDE);
				}
				
				SetWindowPos(&CWnd::wndTop,rct.right - m_nShowOrHideCount * 58 //�����ÿ�γ���53�Ǿ����Թ�������ѡ��Ŀ�ȡ����������صļ����
					,rct.top + 50
					,WB_FUNCTION_DLG_WIDTH
					,rct.bottom - rct.top - 65
					,SWP_SHOWWINDOW|SWP_NOACTIVATE);
			}
			else
			{
				SetWindowPos(&CWnd::wndTop,rct.right - m_nShowOrHideCount * 58 //�����ÿ�γ���53�Ǿ����Թ�������ѡ��Ŀ�ȡ����������صļ����
					,rct.top + 50
					,WB_FUNCTION_DLG_WIDTH
					,rct.bottom - rct.top - 219
					,SWP_SHOWWINDOW|SWP_NOACTIVATE);
			}
		}
		else
		{
			if (((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg())
			{
				((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->KillTimer(ID_TIMER_SHOW);
				((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->SetTime(true);
			}
			
			m_nShowOrHideCount = 0;
			//�����Զ����ض�ʱ������
			if (this->IsWindowVisible())
			{
				m_dLeaveTime = GetTickCount();
				SetTimer(ID_TIMER_HIDETIME, 10, NULL);
			}
		}
		//����ʾ������ˢ�� Ԥ��ҳ ��ֹԤ��ҳ����ʱ�����ҳû��ˢ��
			if ((GetPreviewDLG()&&GetPreviewDLG()->GetIsNeedRefessh())||(GetPreviewDLG() && GetPreviewDLG()->bLoadPreviews()))
			{
				GetPreviewDLG()->RefurbishPreview();
				GetPreviewDLG()->SetIsNeedRefesh(false);
			}
	}
	else
	{
		//�����ع������ǰ �����ɫ�Ի������ �ȹر���ɫ�Ի���
		if (m_wbAttributeDlg && m_wbAttributeDlg->IsWindowVisible())
		{
				m_wbAttributeDlg->SetFocus();
		}
		if (m_wbPreviewDlg && m_wbPreviewDlg->IsWindowVisible())
		{
			m_wbPreviewDlg->SendMessage(WM_CANCELMODE, 0, 0);
			
		}
		//�����ع������ǰ �����ɫ�Ի������ �ȹر���ɫ�Ի���
		else if (m_wbAttributeDlg 
			&& m_wbAttributeDlg->IsWindowVisible()   //�����е���ɫ�Ի���
			&& GetAttributeDlg()->m_pAttriButeColorPatetteDlg
			&& GetAttributeDlg()->m_pAttriButeColorPatetteDlg->IsWindowVisible())
		{
			m_wbAttributeDlg->CloseColorPaletteDlg();
		}

		else if (GetAttributeDlg()   //�����е������ɫ�Ի���
			&& GetAttributeDlg()->IsWindowVisible()
			&& GetAttributeDlg()->m_pAttriButeSolidPatetteDlg
			&& GetAttributeDlg()->m_pAttriButeSolidPatetteDlg->IsWindowVisible())
		{
			m_wbAttributeDlg->CloseColorPaletteDlg();
		}
		else if (m_wbBackgroundDlg    //d�����е������ɫ�Ի���
			&& m_wbBackgroundDlg->IsWindowVisible()
			&& m_wbBackgroundDlg->GetColorPaletteDlg()
			&& m_wbBackgroundDlg->GetColorPaletteDlg()->IsWindowVisible())
		{
			m_wbBackgroundDlg->CloseColorPaletteDlg();   
		}

		if (++m_nShowOrHideCount < 5)
		{
			if (rct.Height() >= 250 && rct.Height() < 350)
			{
				SetWindowPos(&CWnd::wndTop,rct.right - WB_FUNCTION_DLG_WIDTH - WB_OPTIONDLG_WIDTH - 21 + m_nShowOrHideCount * 58 //�����ÿ�γ���53�Ǿ����Թ�������ѡ��Ŀ�ȡ����������صļ����
					,rct.top + 50
					,WB_FUNCTION_DLG_WIDTH
					,rct.bottom - rct.top - 65
					,SWP_SHOWWINDOW|SWP_NOACTIVATE);
			}
			else
			{
				SetWindowPos(&CWnd::wndTop,rct.right - WB_FUNCTION_DLG_WIDTH - WB_OPTIONDLG_WIDTH - 21 + m_nShowOrHideCount * 58 //�����ÿ�γ���53�Ǿ����Թ�������ѡ��Ŀ�ȡ����������صļ����
					,rct.top + 50
					,WB_FUNCTION_DLG_WIDTH
					,rct.bottom - rct.top - 219
					,SWP_SHOWWINDOW|SWP_NOACTIVATE);
			}

		}
		else
		{
			ShowWindow(SW_HIDE);
			KillTimer(ID_TIMER_HIDETIME);
			if (((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg())
			{
				((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->KillTimer(ID_TIMER_HIDE);
				((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->SetTime(true);
			}
			
			m_nShowOrHideCount = 0;
			m_bIshidewindow = true;
			m_rectPreviewMenu.SetRect(0, 0, 0, 0);//���ϴε��Ԥ������еĲ˵�������������
		}
	}
}

void CWBFunctionDlg::DiffTimeHideFunctionDlg()
{
	DWORD dwDiffTime = GetTickCount() - m_dLeaveTime;
	if (dwDiffTime > 4000)
	{
		if (((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg())
		{
			((CWhiteBoardDlg *)g_whiteboardDlg)->GetOptionDlg()->SetTimer(ID_TIMER_HIDE, 10, NULL);
		}
		
	}

}

void CWBFunctionDlg::SetMouseLeaveTime( DWORD dw )
{
	m_dLeaveTime = dw;
}

void CWBFunctionDlg::SetPreviewMenuRect( CRect rect )
{
	m_rectPreviewMenu = rect;
}
