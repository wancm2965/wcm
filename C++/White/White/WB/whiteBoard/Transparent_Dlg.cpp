// Transparent_Dlg.cpp : ʵ���ļ�
//
#pragma once
#include "stdafx.h"
#include "Transparent_Dlg.h"
#include "afxdialogex.h"

// CTransparent_Dlg �Ի���

IMPLEMENT_DYNAMIC(CTransparent_Dlg, CDialog)

CTransparent_Dlg::CTransparent_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTransparent_Dlg::IDD, pParent)
	, m_pImage(NULL)
	, m_strname("")
	, m_bRawPencil(true)
{

}

CTransparent_Dlg::CTransparent_Dlg( CWnd* pParent,bool bRrawPencil )
	: CDialog(CTransparent_Dlg::IDD, pParent)
	, m_pImage(NULL)
	, m_strname("")
	, m_bRawPencil(bRrawPencil)
{

}

CTransparent_Dlg::~CTransparent_Dlg()
{
	m_cstw.DestroyTransWnd();
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage =NULL;
	}
}

void CTransparent_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTransparent_Dlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTransparent_Dlg ��Ϣ�������


BOOL CTransparent_Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_cstw.Create(200,200,0);
	m_cstw.Attach(this->GetSafeHwnd());
	OnInitImageEx();
	DrawSomething();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTransparent_Dlg::DrawSomething()
{
	CDC * pDC=m_cstw.GetDC();
	if(pDC)
	{
		HDC hdc = pDC->GetSafeHdc();
		PointF origin(10.0f, 40.0f);

		if (m_bRawPencil)
		{
			CRect crect;
			crect.SetRect(0,0,30,30);
			if (m_pImage)
			{
				m_pImage->Draw(hdc,crect);
			}
			origin.X = 30.0f;
			origin.Y = 10.0f;
		}
		Graphics graphics(hdc);
		WCHAR buf[256]; 
		USES_CONVERSION; // ���������
		wcscpy(buf, A2W(m_strname.c_str()));  // ת����Ҳ���� wcsncpy
		Gdiplus::Font myFont(L"����", 13);

		SolidBrush blackBrush(Color(255, 120, 0, 0));
		// DrawString �м��ֲ����跨������
		graphics.DrawString(
			buf,
			wcslen(buf),
			&myFont,
			origin,
			&blackBrush);

		m_cstw.Update();
	}
}

void CTransparent_Dlg::OnInitImageEx()
{
	//wchar_t strFilename[MAX_PATH];
	//CString PictureDir = GetCurrentPath();	//��ȡ��ǰ����ִ�е�Ŀ¼
	//CString picturePath ;

	//��ʼ��������ťͼƬ��Ĭ��Ϊ����״̬	
	CString picturePath = GetCurrentPath() + L"\\SKIN\\WhiteBoard\\image\\icon.png";			
	//MultiByteToWideChar(CP_ACP, 0, picturePath, -1, strFilename, MAX_PATH);
	m_pImage = new ImageEx(picturePath.GetBuffer());	
	picturePath.ReleaseBuffer();
}

void CTransparent_Dlg::SetStrname( string str_name )
{
	m_strname = str_name;
}

void CTransparent_Dlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_cstw.DestroyTransWnd();
	CDialog::OnClose();
	delete this;
}
