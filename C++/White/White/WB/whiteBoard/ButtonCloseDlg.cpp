#include "stdafx.h"
#include "whiteBoard.h"
#include "ButtonCloseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CbuttonCloseDlg �Ի���




CbuttonCloseDlg::CbuttonCloseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CbuttonCloseDlg::IDD, pParent)
{
	pWBDoc = NULL;
}

void CbuttonCloseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTONCLOSE,m_btd);
}

BEGIN_MESSAGE_MAP(CbuttonCloseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONCLOSE, &CbuttonCloseDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CbuttonCloseDlg ��Ϣ�������

BOOL CbuttonCloseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    return 0;
}

void CbuttonCloseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CbuttonCloseDlg::OnPaint()
{
	CDialogEx::OnPaint();
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CbuttonCloseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CbuttonCloseDlg::OnBnClickedButtonClose()
{
	if (pWBDoc)
	{
		pWBDoc->ButtonClose();
	}
	g_pWhiteBoardView->DestroyFullScreen();
	
}
void CbuttonCloseDlg::setWBDoc( CWBDocument *pDoc )
{
	pWBDoc = pDoc;
}

