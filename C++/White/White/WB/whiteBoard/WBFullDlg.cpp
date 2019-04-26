// WBFullDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFullDlg.h"
#include "afxdialogex.h"


// CWBFullDlg �Ի���

IMPLEMENT_DYNAMIC(CWBFullDlg, CDialogEx)

CWBFullDlg::CWBFullDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBFullDlg::IDD, pParent)
{

}

CWBFullDlg::~CWBFullDlg()
{
}

void CWBFullDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWBFullDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CWBFullDlg ��Ϣ�������


/*************************************************************************
*                         CWBFullDlg::OnInitDialog()                     *
*                             ȫ����ͼ��ʼ������                         *
*************************************************************************/
BOOL CWBFullDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��"���� "�˵�����ӵ�ϵͳ�˵��С�


	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//����������
/*	CBitmap bmp;
	UINT ButtonIDArray[2]={IDC_WB_FILEB, IDC_WB_FILEB};      //���尴ťID����

	//���ͼ��ͼ���б�
	for(int n=0; n<2; n++){
		bmp.LoadBitmap(IDB_WB_FILE + n);
		m_ImageList.Add(&bmp, RGB(255, 255, 255));
		bmp.DeleteObject();
	}
	m_Toolbar.Create(this);
	m_Toolbar.SetButtons(ButtonIDArray, 2);
	m_Toolbar.SetSizes(CSize(60,56),CSize(40, 40));
	m_Toolbar.GetToolBarCtrl().SetImageList(&m_ImageList);
	RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);*/


	//�Զ�������ͼ���򴴽�������
	m_whiteboard_fullview = (CWhiteBoardView*) RUNTIME_CLASS(CWhiteBoardView)->CreateObject();
	//��Ŀ��λ�ö�̬������ͼ
	if (NULL == m_whiteboard_fullview)  
	{  
		return FALSE;  
	}
	ShowWindow(SW_SHOWMAXIMIZED);
	//m_fullrect.top += 50;
	m_whiteboard_fullview->Create(NULL, NULL, WS_VISIBLE | AFX_WS_DEFAULT_VIEW, m_fullrect, this, NULL); 

	//ȫ�����������ɣ����ļ��е����ݶ��뵽ȫ������
	//	if ()//�ж�����Ϊ��ԭ�����Ƿ������ݣ��ж�ȡ���ݣ����򲻽��в���
	//	{
	//		m_whiteboard_dlg->SendDataFileFull(){}

	//	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻�TRUE
}

void CWBFullDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������

	GetClientRect(&m_fullrect);
}
