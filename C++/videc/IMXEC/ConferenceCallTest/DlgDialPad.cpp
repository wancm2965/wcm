// DlgDialPad.cpp : 实现文件
//

#include "stdafx.h"
#include "ConferenceCallTest.h"
#include "DlgDialPad.h"
#include <IMXEC/IMXEC_Call.h>


// DlgDialPad 对话框

IMPLEMENT_DYNAMIC(DlgDialPad, CDialog)

DlgDialPad::DlgDialPad(CWnd* pParent /*=NULL*/)
	: CDialog(DlgDialPad::IDD, pParent)
{
	m_strCallID = "";
}

DlgDialPad::~DlgDialPad()
{
}

void DlgDialPad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgDialPad, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &DlgDialPad::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &DlgDialPad::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &DlgDialPad::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &DlgDialPad::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &DlgDialPad::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &DlgDialPad::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &DlgDialPad::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &DlgDialPad::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &DlgDialPad::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON_STAR, &DlgDialPad::OnBnClickedButtonStar)
	ON_BN_CLICKED(IDC_BUTTON0, &DlgDialPad::OnBnClickedButton0)
	ON_BN_CLICKED(IDC_BUTTON12, &DlgDialPad::OnBnClickedButton12)
END_MESSAGE_MAP()


// DlgDialPad 消息处理程序

void DlgDialPad::OnBnClickedButton1()
{
	IMXEC_Call::SendDTMF(m_strCallID,'1');
}

void DlgDialPad::OnBnClickedButton2()
{
	IMXEC_Call::SendDTMF(m_strCallID,'2');
}

void DlgDialPad::OnBnClickedButton3()
{
	IMXEC_Call::SendDTMF(m_strCallID,'3');
}

void DlgDialPad::OnBnClickedButton4()
{
	IMXEC_Call::SendDTMF(m_strCallID,'4');
}

void DlgDialPad::OnBnClickedButton5()
{
	IMXEC_Call::SendDTMF(m_strCallID,'5');
}

void DlgDialPad::OnBnClickedButton6()
{
	IMXEC_Call::SendDTMF(m_strCallID,'6');
}

void DlgDialPad::OnBnClickedButton7()
{
	IMXEC_Call::SendDTMF(m_strCallID,'7');
}

void DlgDialPad::OnBnClickedButton8()
{
	IMXEC_Call::SendDTMF(m_strCallID,'8');
}

void DlgDialPad::OnBnClickedButton9()
{
	IMXEC_Call::SendDTMF(m_strCallID,'9');
}

void DlgDialPad::OnBnClickedButtonStar()
{
	IMXEC_Call::SendDTMF(m_strCallID,'*');
}

void DlgDialPad::OnBnClickedButton0()
{
	IMXEC_Call::SendDTMF(m_strCallID,'0');
}

void DlgDialPad::OnBnClickedButton12()
{
	IMXEC_Call::SendDTMF(m_strCallID,'#');
}
