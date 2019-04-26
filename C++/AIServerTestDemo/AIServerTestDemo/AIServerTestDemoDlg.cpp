
// AIServerTestDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AIServerTestDemo.h"
#include "AIServerTestDemoDlg.h"
#include "afxdialogex.h"
#include <string>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "HttpHandler.h"

using namespace rapidjson;

using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


std::string INT2STR(int value)
{
	char tmp[100];
	sprintf_s(tmp, "%d", value);
	return tmp;
}

std::string U2A(CString csUnicodeString)
{
	int nUnicodeCharCount = WideCharToMultiByte(CP_UTF8, 0, csUnicodeString.GetBuffer(), -1, NULL, 0, NULL, NULL);
	char* szAnsiString = new char[nUnicodeCharCount + 1];
	WideCharToMultiByte(CP_UTF8, 0, (PWSTR)csUnicodeString.GetBuffer(), -1, szAnsiString, nUnicodeCharCount, NULL, NULL);
	std::string result = szAnsiString;
	delete[]szAnsiString;
	szAnsiString = NULL;
	return result;
}

CString A2U(const std::string& strAnsiString)
{
	int nUnicodeCharCont = MultiByteToWideChar(CP_UTF8, 0, strAnsiString.c_str(), -1, NULL, 0);
	WCHAR* szUnicodeString = new WCHAR[nUnicodeCharCont + 1];
	MultiByteToWideChar(CP_UTF8, 0, strAnsiString.c_str(), -1, szUnicodeString, nUnicodeCharCont);
	CString result(szUnicodeString);
	delete[]szUnicodeString;
	szUnicodeString = NULL;
	return result;
}
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAIServerTestDemoDlg �Ի���



CAIServerTestDemoDlg::CAIServerTestDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAIServerTestDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAPIServer = NULL;
}

void CAIServerTestDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_pFileEdit);
	DDX_Control(pDX, IDC_SHOWMSG, m_pShowMsg);
	DDX_Control(pDX, IDC_EDIT2, m_pCollectUsr);
	DDX_Control(pDX, IDC_EDIT5, m_pVerNum);
	DDX_Control(pDX, IDC_EDIT6, m_pVerthresold);
	DDX_Control(pDX, IDC_EDIT9, m_pEditServerAPIPath);
	DDX_Control(pDX, IDC_EDIT11, m_pEditVerPicPath);
	DDX_Control(pDX, IDC_EDIT12, m_p264verNum);
	DDX_Control(pDX, IDC_EDIT13, m_p264verthrold);
	DDX_Control(pDX, IDC_EDIT14, m_p264verAuidoID);
	DDX_Control(pDX, IDC_EDIT15, m_p264verVideoID);
	DDX_Control(pDX, IDC_EDIT16, m_p264verNodeID);
	DDX_Control(pDX, IDC_EDIT22, m_p264verMcuIp);
	DDX_Control(pDX, IDC_EDIT23, m_p264verMcuPort);
	DDX_Control(pDX, IDC_EDIT21, m_p264verMcuID);
	DDX_Control(pDX, IDC_EDIT20, m_p264verLocalport);
	DDX_Control(pDX, IDC_EDIT19, m_p264verLocalIp);
	DDX_Control(pDX, IDC_EDIT18, m_p264verNatport);
	DDX_Control(pDX, IDC_EDIT17, m_p264verNatIp);
	DDX_Control(pDX, IDC_EDIT10, m_pEditLocalHttpIP);
	DDX_Control(pDX, IDC_EDIT24, m_pEditStartVerHttpAPI);
	DDX_Control(pDX, IDC_EDIT25, m_pEditStopVerHttpAPI);
}

void CAIServerTestDemoDlg::ShowAPIBCKMessage(const char* strBCKMsg)
{
	SYSTEMTIME currentTime;
	::GetLocalTime(&currentTime);
	CString strOut;
	strOut.Format(_T("[%.2d:%.2d:%.2d:%.2d]\n"), currentTime.wHour, currentTime.wMinute, currentTime.wSecond, currentTime.wMilliseconds);
	CString strMsg = A2U(strBCKMsg);

	m_pShowMsg.SetWindowTextW(strOut+strMsg);

	m_pShowMsg.Invalidate();
}

BEGIN_MESSAGE_MAP(CAIServerTestDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CAIServerTestDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAIServerTestDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CAIServerTestDemoDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CAIServerTestDemoDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CAIServerTestDemoDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, &CAIServerTestDemoDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON6, &CAIServerTestDemoDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CAIServerTestDemoDlg ��Ϣ�������

BOOL CAIServerTestDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������


	if (NULL == m_pAPIServer)
	{
		m_pAPIServer = new CHttpAPIServer(this);
		if (m_pAPIServer)
		{
			m_pAPIServer->start();
		}
		
	}

	CHttpHandler::initHttp();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAIServerTestDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAIServerTestDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAIServerTestDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAIServerTestDemoDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
// 	if (m_pAPIServer)
// 	{
// 		m_pAPIServer->stop();
// 		delete m_pAPIServer;
// 		m_pAPIServer = NULL;
// 	}
	CDialogEx::OnClose();
}


void CAIServerTestDemoDlg::OnDestroy()
{
// 	if (m_pAPIServer)
// 	{
// 		m_pAPIServer->stop();
// 		delete m_pAPIServer;
// 		m_pAPIServer = NULL;
// 	}
	CDialogEx::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
}


void CAIServerTestDemoDlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString filter;
	filter = L"1080PͼƬ|*.jpg|*.png|*.bmp";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString strFile;
		strFile = dlg.GetPathName();
		m_pFileEdit.SetWindowTextW(strFile);
	}
}


//�ɼ�
void CAIServerTestDemoDlg::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	std::string strImageData = "";
	CString strFile;
	m_pFileEdit.GetWindowTextW(strFile);

	std::string strFilePath = U2A(strFile);

	Base64EncodeFromFile(strFilePath, strImageData);

	int nDataLen = strImageData.length();
	//
	string strDataLen = INT2STR(nDataLen);

	CString strEditUser;
	m_pCollectUsr.GetWindowTextW(strEditUser);
	std::string strUser = U2A(strEditUser);
	std::string strWidth;
	std::string strHeight;

	CString strLocalHttpIp;
	m_pEditLocalHttpIP.GetWindowTextW(strLocalHttpIp);
	std::string strLocalHttpIps = U2A(strLocalHttpIp);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	{
		writer.String("apikey");
		std::string strapikey = strLocalHttpIps;
		strapikey += ":4568";
		strapikey += CLBCK;
		writer.String(strapikey.c_str());
		writer.String("user_id");
		writer.String(strUser.c_str());
		writer.String("data");
		writer.String(strImageData.c_str());
		writer.String("datalen");
		writer.String(strDataLen.c_str());

		writer.String("width");
		writer.String("1920");

		writer.String("height");
		writer.String("1080");

		writer.String("aligned");
		writer.String("0");	
	}
	writer.EndObject();

	std::string strSendData = buffer.GetString();
	CString strHostPath;
	m_pEditServerAPIPath.GetWindowTextW(strHostPath);
	std::string strHost = U2A(strHostPath);
	SendCallBackMessage(strHost.c_str(), strSendData.c_str());
}

//ͼƬʶ��
void CAIServerTestDemoDlg::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	std::string strImageData = "";
	CString strFile;
	m_pEditVerPicPath.GetWindowTextW(strFile);

	std::string strFilePath = U2A(strFile);

	Base64EncodeFromFile(strFilePath, strImageData);
	int nDataLen = strImageData.length();
	//
	string strDataLen = INT2STR(nDataLen);

	CString strVerEdNum;
	m_pVerNum.GetWindowTextW(strVerEdNum);
	std::string strVerNum = U2A(strVerEdNum);

	CString strVerEdthro;
	m_pVerthresold.GetWindowTextW(strVerEdthro);
	std::string strVerthro = U2A(strVerEdthro);

	CString strLocalHttpIp;
	m_pEditLocalHttpIP.GetWindowTextW(strLocalHttpIp);
	std::string strLocalHttpIps = U2A(strLocalHttpIp);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	{
		writer.String("apikey");
		//std::string strapikey = "192.168.0.157:4568";
		std::string strapikey = strLocalHttpIps;
		strapikey += ":4568";
		strapikey += VERBCK;
		writer.String(strapikey.c_str());		
		writer.String("recnum");
		writer.String(strVerNum.c_str());
		writer.String("threshold");
		writer.String(strVerthro.c_str());

		writer.String("datatype");
		writer.String("pic");

		writer.String("dataparam");
		writer.StartObject();
		{
			writer.String("recdata");
			writer.String(strImageData.c_str());			
			writer.String("datalen");
			writer.String(strDataLen.c_str());

			writer.String("width");
			writer.String("1920");

			writer.String("height");
			writer.String("1080");
		}
		writer.EndObject();
	}
	writer.EndObject();

	std::string strSendData = buffer.GetString();
	CString strHostPath;
	m_pEditStartVerHttpAPI.GetWindowTextW(strHostPath);
	std::string strHost = U2A(strHostPath);
	SendCallBackMessage(strHost.c_str(), strSendData.c_str());
}


void CAIServerTestDemoDlg::SendCallBackMessage(const char* host, const char* message)
{
	CHttpHandler::Instance(NULL).post(host, message, 5000);
}

//ͼƬʶ���ļ���Ӱ�ť
void CAIServerTestDemoDlg::OnBnClickedButton4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString filter;
	filter = L"1080PͼƬ|*.jpg|*.png|*.bmp";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString strFile;
		strFile = dlg.GetPathName();
		m_pEditVerPicPath.SetWindowTextW(strFile);
	}
}


//264ʶ��
void CAIServerTestDemoDlg::OnBnClickedButton5()
{
	CString strLocalHttpIp;
	m_pEditLocalHttpIP.GetWindowTextW(strLocalHttpIp);
	std::string strLocalHttpIps = U2A(strLocalHttpIp);
	std::string strapikey = strLocalHttpIps;
	strapikey += ":4568";
	strapikey += VERBCK;

	CString strc264verNum;
	m_p264verNum.GetWindowTextW(strc264verNum);
	CString strc264verthrold;
	m_p264verthrold.GetWindowTextW(strc264verthrold);
	CString strc264verAuidoID;
	m_p264verAuidoID.GetWindowTextW(strc264verAuidoID);
	CString strc264verVideoID;
	m_p264verVideoID.GetWindowTextW(strc264verVideoID);
	CString strc264verNodeID;
	m_p264verNodeID.GetWindowTextW(strc264verNodeID);
	CString strc264verMcuIp;
	m_p264verMcuIp.GetWindowTextW(strc264verMcuIp);
	CString strc264verMcuPort;
	m_p264verMcuPort.GetWindowTextW(strc264verMcuPort);
	CString strc264verMcuID;
	m_p264verMcuID.GetWindowTextW(strc264verMcuID);
	CString strc264verLocalport;
	m_p264verLocalport.GetWindowTextW(strc264verLocalport);
	CString strc264verLocalIp;
	m_p264verLocalIp.GetWindowTextW(strc264verLocalIp);
	CString strc264verNatport;
	m_p264verNatport.GetWindowTextW(strc264verNatport);
	CString strc264verNatIp;
	m_p264verNatIp.GetWindowTextW(strc264verNatIp);

	std::string str264verNum = U2A(strc264verNum);
	std::string str264verthrold = U2A(strc264verthrold);
	std::string str264verAuidoID = U2A(strc264verAuidoID);
	std::string str264verVideoID = U2A(strc264verVideoID);
	std::string str264verNodeID = U2A(strc264verNodeID);
	std::string str64verMcuIp = U2A(strc264verMcuIp);
	std::string str264verMcuPort = U2A(strc264verMcuPort);
	std::string str264verMcuID = U2A(strc264verMcuID);
	std::string str264verLocalport = U2A(strc264verLocalport);
	std::string str264verLocalIp = U2A(strc264verLocalIp);
	std::string str264verNatport = U2A(strc264verNatport);
	std::string str264verNatIp = U2A(strc264verNatIp);


	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	{
		writer.String("apikey");		
		writer.String(strapikey.c_str());
		writer.String("recnum");
		writer.String(str264verNum.c_str());
		writer.String("threshold");
		writer.String(str264verthrold.c_str());

		writer.String("datatype");
		writer.String("stream");

		writer.String("dataparam");
		writer.StartObject();
		{
			writer.String("audioid");
			writer.String(str264verAuidoID.c_str());
			writer.String("videoid");
			writer.String(str264verVideoID.c_str());

			writer.String("nodeid");
			writer.String(str264verNodeID.c_str());
			writer.String("natip");
			writer.String(str264verNatIp.c_str());

			writer.String("natport");
			writer.String(str264verNatport.c_str());
			writer.String("localip");
			writer.String(str264verLocalIp.c_str());


			writer.String("localport");
			writer.String(str264verLocalport.c_str());
			writer.String("mcuid");
			writer.String(str264verMcuID.c_str());


			writer.String("mcuip");
			writer.String(str64verMcuIp.c_str());
			writer.String("mcuport");
			writer.String(str264verMcuPort.c_str());

		}
		writer.EndObject();
	}
	writer.EndObject();

	std::string strSendData = buffer.GetString();
	CString strHostPath;
	m_pEditStartVerHttpAPI.GetWindowTextW(strHostPath);
	std::string strHost = U2A(strHostPath);
	SendCallBackMessage(strHost.c_str(), strSendData.c_str());
}


void CAIServerTestDemoDlg::OnBnClickedButton7()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strLocalHttpIp;
	m_pEditLocalHttpIP.GetWindowTextW(strLocalHttpIp);
	std::string strLocalHttpIps = U2A(strLocalHttpIp);
	std::string strapikey = strLocalHttpIps;
	strapikey += ":4568";
	strapikey += VERBCK;
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	{
		writer.String("apikey");
		writer.String(strapikey.c_str());
	}
	writer.EndObject();

	std::string strSendData = buffer.GetString();
	CString strHostPath;
	m_pEditStopVerHttpAPI.GetWindowTextW(strHostPath);
	std::string strHost = U2A(strHostPath);
	SendCallBackMessage(strHost.c_str(), strSendData.c_str());

}


void CAIServerTestDemoDlg::OnBnClickedButton6()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strLocalHttpIp;
	m_pEditLocalHttpIP.GetWindowTextW(strLocalHttpIp);
	std::string strLocalHttpIps = U2A(strLocalHttpIp);
	std::string strapikey = strLocalHttpIps;
	strapikey += ":4568";
	strapikey += VERBCK;

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	writer.StartObject();
	{
		writer.String("apikey");
		writer.String(strapikey.c_str());
	}
	writer.EndObject();

	std::string strSendData = buffer.GetString();
	CString strHostPath;
	m_pEditStopVerHttpAPI.GetWindowTextW(strHostPath);
	std::string strHost = U2A(strHostPath);
	SendCallBackMessage(strHost.c_str(), strSendData.c_str());
}
