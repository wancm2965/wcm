// WBResDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBResDlg.h"
#include "afxdialogex.h"


// CWBResDlg �Ի���

IMPLEMENT_DYNAMIC(CWBResDlg, CDialogEx)

	CWBResDlg::CWBResDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBResDlg::IDD, pParent)
{

}

CWBResDlg::~CWBResDlg()
{
}

void CWBResDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWBResDlg, CDialogEx)
END_MESSAGE_MAP()


// CWBResDlg ��Ϣ�������

void CWBResDlg::OnOK()//���λس���
{
	return;
}