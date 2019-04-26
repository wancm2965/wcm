// WBResDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBResDlg.h"
#include "afxdialogex.h"


// CWBResDlg 对话框

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


// CWBResDlg 消息处理程序

void CWBResDlg::OnOK()//屏蔽回车键
{
	return;
}