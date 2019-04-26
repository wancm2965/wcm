// DrawCtl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "DrawCtl.h"
#include "afxdialogex.h"
#include "ScreenDrawMgr.h"


// CDrawCtl �Ի���

IMPLEMENT_DYNAMIC(CDrawCtl, CDialogEx)

CDrawCtl::CDrawCtl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDrawCtl::IDD, pParent)
{

}

CDrawCtl::~CDrawCtl()
{
}

void CDrawCtl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_transCtl);
}


BEGIN_MESSAGE_MAP(CDrawCtl, CDialogEx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_FULL, &CDrawCtl::OnBnClickedFull)
	ON_BN_CLICKED(IDC_EFFECTIVE, &CDrawCtl::OnBnClickedEffective)
	ON_BN_CLICKED(IDC_DESTORY, &CDrawCtl::OnBnClickedDestory)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CDrawCtl::OnNMCustomdrawSlider1)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CDrawCtl ��Ϣ�������

void CDrawCtl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    int m_nCur=m_transCtl.GetPos();
	int nAlpha = (int)(255-m_nCur*254/100);
	if(CScreenDrawMgr::GetIns())
	{
		CScreenDrawMgr::GetIns()->AlphaChange(nAlpha);
		g_pWhiteBoardView->SetSDAlpha(nAlpha);

		//����ͬ����Ϣ
		g_pWhiteBoardView->SendScreenDraw();
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDrawCtl::OnBnClickedFull()
{
	if (g_bAvconLock) //��������аװ幦�ܱ�����
	{
		return;
	}
	if(CScreenDrawMgr::GetIns())
	{
		if (g_pWhiteBoardView->FullScreenShift(!(CScreenDrawMgr::GetIns()->IsFullScreen())))
		{
// 			//����ͬ����Ϣ
// 			g_pWhiteBoardView->SendScreenDraw();
		}
	}
}


void CDrawCtl::OnBnClickedEffective()
{
	if (g_bAvconLock) //��������аװ幦�ܱ�����
	{
		return;
	}
	if(CScreenDrawMgr::GetIns())
	{
		if (g_pWhiteBoardView->ShiftGuise(!(CScreenDrawMgr::GetIns()->IsTransparent())))
		{
			//����ͬ����Ϣ
			g_pWhiteBoardView->SendScreenDraw();
		}
	}
}


void CDrawCtl::OnBnClickedDestory()
{
	if (g_bAvconLock) //��������аװ幦�ܱ�����
	{
		return;
	}
	if (g_pWhiteBoardView->DestroyScreenDraw())
	{
		//����ͬ����Ϣ
		g_pWhiteBoardView->SendScreenDraw();
	}
	this->DestroyWindow();
}

void CDrawCtl::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void CDrawCtl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (g_bAvconLock) //��������аװ幦�ܱ�����
	{
		return;
	}
	CRect rect;
	this->GetClientRect(&rect);

	if(point.x<=20&&point.y<=rect.Height())
	{	
		::SendMessageA(this->m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


BOOL CDrawCtl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	/*m_transCtl.SetTicFreq(5);
	m_transCtl.SetLineSize(5);*/
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_transCtl.SetPageSize(10);
	m_transCtl.SetPos(100);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
