// WBOptionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBOptionDlg.h"
#include "afxdialogex.h"
#include "afxdialogex.h"
#include "WhiteBoardView.h"
// CWBOptionDlg �Ի���

IMPLEMENT_DYNAMIC(CWBOptionDlg, CDialogEx)

CWBOptionDlg::CWBOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBOptionDlg::IDD, pParent)
{
	m_pParent = pParent;
	m_IsSet = false;
	m_bIsSetTimer = true; //�Ƿ�������ö�ʱ��
}

CWBOptionDlg::~CWBOptionDlg()
{
}

void CWBOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WB_PREVIEWBTN, m_cPreviewButton);
	DDX_Control(pDX, IDC_WB_ATTRIBUTEBTN, m_cAttributeButton);
	DDX_Control(pDX,IDC_WB_RESBTN,m_cResButton);
	DDX_Control(pDX,IDC_WB_BACKGROUNDBTN,m_cBGButton);
	DDX_Control(pDX, IDC_WB_TEACHTOOL, m_cTTButton);
}


BEGIN_MESSAGE_MAP(CWBOptionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_WB_PREVIEWBTN, &CWBOptionDlg::OnBnClickedWbPreviewbtn)
	ON_BN_CLICKED(IDC_WB_RESBTN, &CWBOptionDlg::OnBnClickedWbResbtn)
	ON_BN_CLICKED(IDC_WB_ATTRIBUTEBTN, &CWBOptionDlg::OnBnClickedWbAttributebtn)
	ON_BN_CLICKED(IDC_WB_BACKGROUNDBTN, &CWBOptionDlg::OnBnClickedWbBackgroundbtn)
	ON_MESSAGE(WM_SHOWOPTIONBTN,&CWBOptionDlg::OnWM_SHOWOPTIONBTN)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_WB_TEACHTOOL, &CWBOptionDlg::OnBnClickedWbTeachtool)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CWBOptionDlg ��Ϣ�������
void CWBOptionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	GetClientRect(&m_rect);
}

void CWBOptionDlg::OnOK()//���λس���
{
	return;
}

void CWBOptionDlg::OnCancel()//����esc��
{
	return;
}

BOOL CWBOptionDlg::OnEraseBkgnd(CDC* pDC)
{
	//CString CurPicPath = GetCurrentPath();	//��ȡ��ǰ����ִ�е�Ŀ¼
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	CString CurPicPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\image\\functionDlg\\OptionDlgBg.png";
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	

	return TRUE;
}

BOOL CWBOptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CSize psize;
	if (g_bU7NewStyle)
	{
		psize.cx = 0;
		psize.cy = 0;
	}
	else
	{
		m_cPreviewButton.MoveWindowBtn(m_rect.left, m_rect.top + 50);
		psize = m_cPreviewButton.GetBtnSizeHeight();
	}
	m_cAttributeButton.MoveWindowBtn(m_rect.left, m_rect.top + psize.cx + 50);
	//m_cResButton.MoveWindow(m_rect.left,m_rect.top + 124,30,60);
	CSize Asize;
		Asize.cx= m_cAttributeButton.GetBtnSizeHeight().cx + psize.cx;
	m_cBGButton.MoveWindowBtn(m_rect.left,m_rect.top + Asize.cx + 50);
	CSize Bsize;   //��Դ��ť
	Bsize.cx = m_cBGButton.GetBtnSizeHeight().cx + Asize.cx;
	m_cTTButton.MoveWindowBtn(m_rect.left,m_rect.top + Bsize.cx + 50);
	CSize Csize;
	Csize.cx= m_cTTButton.GetBtnSizeHeight().cx + Bsize.cx;
	m_cResButton.MoveWindowBtn(m_rect.left,m_rect.top + Csize.cx + 50);
	m_cResButton.ShowWindow(SW_HIDE);

	//����Ԥ����ťΪѡ��״̬
	if (g_bU7NewStyle)
	{
		m_cPreviewButton.SetIsSelect(FALSE);
		m_cAttributeButton.SetIsSelect(TRUE);

		m_cPreviewButton.ShowWindow(SW_HIDE);
	}
	else
	{
		m_cPreviewButton.SetIsSelect(TRUE);
		m_cAttributeButton.SetIsSelect(FALSE);
	}

	
	m_cResButton.SetIsSelect(FALSE);
	m_cBGButton.SetIsSelect(FALSE);
	m_cTTButton.SetIsSelect(FALSE);
	if (!g_bShowWBNewFun)
	{
		
		m_cBGButton.ShowWindow(SW_HIDE);
		m_cTTButton.ShowWindow(SW_HIDE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CWBOptionDlg::OnBnClickedWbPreviewbtn()
{
	OnOptionBtnClicked(IDC_WB_PREVIEWBTN);
}

void CWBOptionDlg::OnBnClickedWbAttributebtn()
{	
	OnOptionBtnClicked(IDC_WB_ATTRIBUTEBTN);
}

void CWBOptionDlg::OnBnClickedWbBackgroundbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	OnOptionBtnClicked(IDC_WB_BACKGROUNDBTN);
}

void CWBOptionDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CDialogEx::PostNcDestroy();
}

LRESULT CWBOptionDlg::OnWM_SHOWOPTIONBTN(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case 0:
		{
			m_cPreviewButton.SetIsSelect(FALSE);
			m_cAttributeButton.SetIsSelect(FALSE);
			m_cResButton.SetIsSelect(FALSE);
			m_cBGButton.SetIsSelect(FALSE);
			m_cTTButton.SetIsSelect(FALSE);
			m_cBGButton.Invalidate();
			m_cResButton.Invalidate();
			m_cAttributeButton.Invalidate();
			m_cPreviewButton.Invalidate();
			m_cTTButton.Invalidate();
		}
		break;
	case 1:                  //Ԥ�����
		{

			m_cPreviewButton.SetIsSelect(TRUE);
			m_cAttributeButton.SetIsSelect(FALSE);
			m_cResButton.SetIsSelect(FALSE);
			m_cBGButton.SetIsSelect(FALSE);
			m_cTTButton.SetIsSelect(FALSE);
			m_cBGButton.Invalidate();
			m_cResButton.Invalidate();
			m_cAttributeButton.Invalidate();
			m_cPreviewButton.Invalidate();
			m_cTTButton.Invalidate();
		}
		break;
	case 2:                //������� 
		{
			m_cPreviewButton.SetIsSelect(FALSE);
			m_cAttributeButton.SetIsSelect(TRUE);
			m_cResButton.SetIsSelect(FALSE);
			m_cBGButton.SetIsSelect(FALSE);
			m_cTTButton.SetIsSelect(FALSE);
			m_cBGButton.Invalidate();
			m_cPreviewButton.Invalidate();
			m_cResButton.Invalidate();
			m_cAttributeButton.Invalidate();
			m_cTTButton.Invalidate();
		}
		break;
	case 3:              //��Դ���
		{
			m_cPreviewButton.SetIsSelect(FALSE);
			m_cAttributeButton.SetIsSelect(FALSE);
			m_cResButton.SetIsSelect(TRUE);
			m_cBGButton.SetIsSelect(FALSE);
			m_cTTButton.SetIsSelect(FALSE);
			m_cBGButton.Invalidate();
			m_cPreviewButton.Invalidate();
			m_cAttributeButton.Invalidate();
			m_cResButton.Invalidate();
			m_cTTButton.Invalidate();
		}
		break;
	case 4:               //�������
		{
			m_cPreviewButton.SetIsSelect(FALSE);
			m_cAttributeButton.SetIsSelect(FALSE);
			m_cResButton.SetIsSelect(FALSE);
			m_cBGButton.SetIsSelect(TRUE);
			m_cTTButton.SetIsSelect(FALSE);
			m_cBGButton.Invalidate();
			m_cPreviewButton.Invalidate();
			m_cAttributeButton.Invalidate();
			m_cResButton.Invalidate();
			m_cTTButton.Invalidate();
		}
		break;
	case 5:               //��ѧ�������
		{
			m_cPreviewButton.SetIsSelect(FALSE);
			m_cAttributeButton.SetIsSelect(FALSE);
			m_cResButton.SetIsSelect(FALSE);
			m_cBGButton.SetIsSelect(FALSE);
			m_cTTButton.SetIsSelect(TRUE);
			m_cBGButton.Invalidate();
			m_cPreviewButton.Invalidate();
			m_cAttributeButton.Invalidate();
			m_cResButton.Invalidate();
			m_cTTButton.Invalidate();
		}
		break;
	default:
		break;
	}
	return 1;
}

void CWBOptionDlg::OnBnClickedWbTeachtool()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	OnOptionBtnClicked(IDC_WB_TEACHTOOL);
}

void CWBOptionDlg::OnOptionBtnClicked( int BtnId )
{
	CWBFunctionDlg *pFuncDlf = ((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg();
	if (pFuncDlf == NULL)
	{
		return;
	}
	switch(BtnId)
	{
	case  IDC_WB_PREVIEWBTN:
		{
			if (NULL == (pFuncDlf)->GetPreviewDLG())
			{
				::SendMessage(this->m_hWnd, WM_SHOWOPTIONBTN, 1, 0);
				::SendMessage(pFuncDlf->m_hWnd,WM_OPENPREVIEWDLG,0,0);

			}
			else
			{
				if (!pFuncDlf->GetPreviewDLG()->IsWindowVisible())
				{
					::SendMessage(this->m_hWnd, WM_SHOWOPTIONBTN, 1, 0);
					::SendMessage(pFuncDlf->m_hWnd,WM_OPENPREVIEWDLG,0,0);
				}
			}
		}
		break;

	case IDC_WB_ATTRIBUTEBTN:
		{
			if (NULL == pFuncDlf->GetAttributeDlg())
			{
				::SendMessage(this->m_hWnd, WM_SHOWOPTIONBTN, 2, 0);
				::SendMessage(pFuncDlf->m_hWnd,WM_OPENARRIBUTEDLG,0,0);
			}
			else
			{
				if (!pFuncDlf->GetAttributeDlg()->IsWindowVisible())
				{
					::SendMessage(this->m_hWnd, WM_SHOWOPTIONBTN, 2, 0);
					::SendMessage(pFuncDlf->m_hWnd,WM_OPENARRIBUTEDLG,0,0);
				}
			}		
		}
		break;

	case IDC_WB_BACKGROUNDBTN:
		{
			if (NULL == pFuncDlf->GetBackGroundDlg())
			{
				::SendMessage(this->m_hWnd, WM_SHOWOPTIONBTN, 4, 0);
				::SendMessage(pFuncDlf->m_hWnd,WM_OPENBACKGROUNDDLG,0,0);
			}
			else
			{
				if (!pFuncDlf->GetBackGroundDlg()->IsWindowVisible())
				{
					::SendMessage(this->m_hWnd, WM_SHOWOPTIONBTN, 4, 0);
					::SendMessage(pFuncDlf->m_hWnd,WM_OPENBACKGROUNDDLG,0,0);
				}
			}
					
		}
		break;

	case IDC_WB_TEACHTOOL:
		{
			if (NULL == pFuncDlf->GetTeachtoolDlg())
			{
				::SendMessage(this->m_hWnd, WM_SHOWOPTIONBTN, 5, 0);
				::SendMessage(pFuncDlf->m_hWnd,WM_OPENTEACHTOOLDLG,0,0);
			}
			else
			{
				if (!(((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg())->GetTeachtoolDlg()->IsWindowVisible())
				{
					::SendMessage(this->m_hWnd, WM_SHOWOPTIONBTN, 5, 0);
					::SendMessage(pFuncDlf->m_hWnd,WM_OPENTEACHTOOLDLG,0,0);
				}
			}
		}
		break;

	default:
		break;
	}

}


void CWBOptionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (((CWhiteBoardDlg *)g_whiteboardDlg)->GetFuntDlg())
	{
		if (ID_TIMER_SHOW == nIDEvent)
		{
			KillTimer(ID_TIMER_HIDE);
			((CWhiteBoardDlg *)g_whiteboardDlg)->GetFuntDlg()->ShowOrHideFunctionDlg(true);
		}
		if (ID_TIMER_HIDE == nIDEvent)
		{
			KillTimer(ID_TIMER_SHOW);
			((CWhiteBoardDlg *)g_whiteboardDlg)->GetFuntDlg()->ShowOrHideFunctionDlg(false);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CWBOptionDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//����ѡ��ĳ���
	if (!m_bIsSetTimer)
	{
		return;
	}
	if (((CWhiteBoardDlg*)g_whiteboardDlg) &&
		((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg())
	{
		if (!((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->IsWindowVisible())
		{
			SetTimer(ID_TIMER_SHOW, 10, NULL);	
			m_bIsSetTimer = false;
			((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->SetWindowIsHide(false);
		}
		else if(((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->GetWindowIsHide())
		{
			SetTimer(ID_TIMER_SHOW, 10, NULL);	
			m_bIsSetTimer = false;
			((CWhiteBoardDlg*)g_whiteboardDlg)->GetFuntDlg()->SetWindowIsHide(false);
		}
		else
		{
			((CWhiteBoardDlg *) g_whiteboardDlg)->GetFuntDlg()->SetMouseLeaveTime(GetTickCount());
		}
	}
	//ʹOnMouseHover����ʹ�� 
		//if (!m_IsSet)
		//{
		//	TRACKMOUSEEVENT tme = {0};
		//	tme.cbSize = sizeof(TRACKMOUSEEVENT);
		//	tme.dwFlags = TME_LEAVE;
		//	tme.dwHoverTime = 1;
		//	tme.hwndTrack = m_hWnd;
		//	m_IsSet = TrackMouseEvent(&tme);
		//}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CWBOptionDlg::OnBnClickedWbResbtn()
{

}
