// DlgShowConnectInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgShowConnectInfo.h"
#include "afxdialogex.h"
#include "ResourceWBMedia.h"

// CDlgShowConnectInfo �Ի���

IMPLEMENT_DYNAMIC(CDlgShowConnectInfo, CDialogEx)

CDlgShowConnectInfo::CDlgShowConnectInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TEST_CONNECT, pParent)
	, m_bActiveCreator(TRUE)
{

}

CDlgShowConnectInfo::~CDlgShowConnectInfo()
{
}

void CDlgShowConnectInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgShowConnectInfo, CDialogEx)
END_MESSAGE_MAP()


// CDlgShowConnectInfo ��Ϣ�������


void CDlgShowConnectInfo::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	//GetDlgCtrlID()
	CString sText;
	GetDlgItem(IDC_EDIT_AUDIOID)->GetWindowText(sText);
	nAudioID = atoi(sText);

	GetDlgItem(IDC_EDIT_DEVID)->GetWindowText(sText);
	nVideoID = atoi(sText);

	GetDlgItem(IDC_EDIT_STRING_NODEID)->GetWindowText(sText);
	sNodeID = std::string(sText.GetBuffer());

	GetDlgItem(IDC_EDIT_STRING_NATIP)->GetWindowText(sText);
	sNATIP = std::string(sText.GetBuffer());

	GetDlgItem(IDC_EDIT_NATPORT)->GetWindowText(sText);
	nNATPort = atoi(sText);

	GetDlgItem(IDC_EDIT_STRING_LOCALIP)->GetWindowText(sText);
	sLocalIP = std::string(sText.GetBuffer());

	GetDlgItem(IDC_EDIT_LOCALPORT)->GetWindowText(sText);
	nLocalPort = atoi(sText);

	GetDlgItem(IDC_EDIT_STRING_MCUID)->GetWindowText(sText);
	sMCUID = std::string(sText.GetBuffer());

	GetDlgItem(IDC_EDIT_STRING_MCUIP)->GetWindowText(sText);
	sMCUIP = std::string(sText.GetBuffer());

	GetDlgItem(IDC_EDIT_SERVERPORT)->GetWindowText(sText);
	nServerPort = atoi(sText);

	CDialogEx::OnOK();
}


BOOL CDlgShowConnectInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	//Video ID
	char temp[200];
	ultoa(nVideoID,temp,10);
	CString sDevID;
	sDevID.Format(_T("%ld"), nVideoID);
	//ultoa(nVideoID, sText.GetBuffer(),10);
	GetDlgItem(IDC_EDIT_DEVID)->SetWindowText(temp);

	//Audio ID
	CString sAudioID;
	sAudioID.Format(_T("%ld"), nAudioID);
	GetDlgItem(IDC_EDIT_AUDIOID)->SetWindowText(sAudioID);

	//node ID
	GetDlgItem(IDC_EDIT_STRING_NODEID)->SetWindowText(sNodeID.c_str());

	//NATIP
	GetDlgItem(IDC_EDIT_STRING_NATIP)->SetWindowText(sNATIP.c_str());
	//return TRUE;
	//
	CString sNATPort;
	sNATPort.Format(_T("%ld"), nNATPort);
	GetDlgItem(IDC_EDIT_NATPORT)->SetWindowText(sNATPort);

	//local ip
	GetDlgItem(IDC_EDIT_STRING_LOCALIP)->SetWindowText(sLocalIP.c_str());

	//
	CString sLocalPort;
	sLocalPort.Format(_T("%ld"), nLocalPort);
	GetDlgItem(IDC_EDIT_LOCALPORT)->SetWindowText(sLocalPort);

	//MCU ID
	GetDlgItem(IDC_EDIT_STRING_MCUID)->SetWindowText(sMCUID.c_str());

	//MCU IP
	GetDlgItem(IDC_EDIT_STRING_MCUIP)->SetWindowText(sMCUIP.c_str());
	//
	CString sServerPort;
	sServerPort.Format(_T("%ld"), nServerPort);
	GetDlgItem(IDC_EDIT_SERVERPORT)->SetWindowText(sServerPort);

	if (m_bActiveCreator)
	{
		SetWindowText(_T("���⹲�����Ƶ������Ϣ����"));
	}
	else
	{
		SetWindowText(_T("���ӹ���˵���Ƶ��Ϣ����"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
