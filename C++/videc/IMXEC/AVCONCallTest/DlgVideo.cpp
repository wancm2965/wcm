// DlgVideo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AVCONCallTest.h"
#include "DlgVideo.h"


// DlgVideo �Ի���

IMPLEMENT_DYNAMIC(DlgVideo, CDialog)

DlgVideo::DlgVideo(CWnd* pParent /*=NULL*/)
	: CDialog(DlgVideo::IDD, pParent)
{

}

DlgVideo::~DlgVideo()
{
}

void DlgVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgVideo, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// DlgVideo ��Ϣ�������

BOOL DlgVideo::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);

	pDC->FillRect(rc,&CBrush(RGB(0,0,0)));

	return TRUE;
}
