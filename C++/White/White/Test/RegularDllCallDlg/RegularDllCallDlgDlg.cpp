
// RegularDllCallDlgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RegularDllCallDlg.h"
#include "RegularDllCallDlgDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
public:
	afx_msg void On32772();
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

// CRegularDllCallDlgDlg �Ի���

CRegularDllCallDlgDlg::CRegularDllCallDlgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRegularDllCallDlgDlg::IDD, pParent)
{
	m_FullMenu = NULL;
}


CRegularDllCallDlgDlg::~CRegularDllCallDlgDlg()
{
}


void CRegularDllCallDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRegularDllCallDlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_BUTTON1, &CRegularDllCallDlgDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CRegularDllCallDlgDlg::OnBnClickedCancel)

	
	//ON_WM_MOUSEMOVE()
	ON_COMMAND(IDC_WB_NEWFILEB, &CRegularDllCallDlgDlg::OnWbNewfileb)
	ON_COMMAND(IDC_WB_OPENFILEB, &CRegularDllCallDlgDlg::OnWbOpenfileb)
	ON_COMMAND(IDC_WB_SAVEFILEB, &CRegularDllCallDlgDlg::OnWbSavefileb)
	ON_COMMAND(IDC_WB_RESAVEFILEB, &CRegularDllCallDlgDlg::OnWbResavefileb)
	ON_COMMAND(IDC_WB_PRINTFILEB, &CRegularDllCallDlgDlg::OnWbPrintfileb)
	ON_COMMAND(IDC_WB_CLOSEFILEB, &CRegularDllCallDlgDlg::OnWbClosefileb)

	ON_COMMAND(IDC_WB_NEWPAGEB, &CRegularDllCallDlgDlg::OnWbNewpageb)
	ON_COMMAND(IDC_WB_FOREPAGEB, &CRegularDllCallDlgDlg::OnWbForepageb)
	ON_COMMAND(IDC_WB_NEXTPAGEB, &CRegularDllCallDlgDlg::OnWbNextpageb)
	ON_COMMAND(IDC_WB_FIRSTPAGEB, &CRegularDllCallDlgDlg::OnWbFirstpageb)
	ON_COMMAND(IDC_WB_LASTPAGEB, &CRegularDllCallDlgDlg::OnWbLastpageb)


	ON_COMMAND(IDC_WB_FORELAYERB, &CRegularDllCallDlgDlg::OnWbForelayerb)
	ON_COMMAND(IDC_WB_NEXTLAYERB, &CRegularDllCallDlgDlg::OnWbNextlayerb)
	ON_COMMAND(IDC_WB_TOPLAYERB, &CRegularDllCallDlgDlg::OnWbToplayerb)
	ON_COMMAND(IDC_WB_BOTTOMLAYERB, &CRegularDllCallDlgDlg::OnWbBottomlayerb)
	ON_COMMAND(IDC_WB_LINEB, &CRegularDllCallDlgDlg::OnWbLineb)

	ON_COMMAND(IDC_WB_SELECTB, &CRegularDllCallDlgDlg::OnWbSelectb)
	ON_COMMAND(IDC_WB_CURVEB, &CRegularDllCallDlgDlg::OnWbCurveb)
	ON_COMMAND(IDC_WB_LITTERRUBBERB, &CRegularDllCallDlgDlg::OnWbLitterrubberb)
	ON_COMMAND(IDC_WB_BIGRUBBERB, &CRegularDllCallDlgDlg::OnWbBigrubberb)
	ON_COMMAND(IDC_WB_SINGLEOBJECTRUBBERB, &CRegularDllCallDlgDlg::OnWbSingleobjectrubberb)
	ON_COMMAND(IDC_WB_CURVESELECTRUBBERB, &CRegularDllCallDlgDlg::OnWbCurveselectrubberb)

	ON_COMMAND(IDC_WB_RECTANGLEB, &CRegularDllCallDlgDlg::OnWbRectangleb)
	ON_COMMAND(IDC_WB_ROUNDB, &CRegularDllCallDlgDlg::OnWbRoundb)
	ON_COMMAND(IDC_WB_TRIANGLEB, &CRegularDllCallDlgDlg::OnWbTriangleb)
	ON_COMMAND(IDC_WB_RHOMBUSB, &CRegularDllCallDlgDlg::OnWbRhombusb)
	ON_COMMAND(IDC_WB_POLYGONB, &CRegularDllCallDlgDlg::OnWbPolygonb)
	ON_COMMAND(IDC_WB_CHARFONTB, &CRegularDllCallDlgDlg::OnWbCharfontb)
	ON_COMMAND(IDC_WB_CHARTEXTB, &CRegularDllCallDlgDlg::OnWbChartextb)
	ON_COMMAND(IDC_WB_ANNOTATIONS, &CRegularDllCallDlgDlg::OnWbAnnotationsb)

	ON_COMMAND(IDC_WB_PICTUREB, &CRegularDllCallDlgDlg::OnWbPictureb)
	//ON_COMMAND(IDC_LINEWIDTHB1, &CRegularDllCallDlgDlg::OnLinewidthb1)
	ON_COMMAND(IDC_WB_DOCUMENTB, &CRegularDllCallDlgDlg::OnWbDocumentb)
	ON_COMMAND(IDC_WB_CUTSCREENB, &CRegularDllCallDlgDlg::OnWbCutscreenb)
	ON_COMMAND(IDC_WB_DELETEB, &CRegularDllCallDlgDlg::OnWbDeleteb)
	ON_COMMAND(IDC_WB_UCCB, &CRegularDllCallDlgDlg::OnWbUccb)
	ON_COMMAND(IDC_WB_RESOURCECATAGORYB, &CRegularDllCallDlgDlg::OnWbResourcecatagoryb)
	ON_COMMAND(IDC_RESOURCE_MANGER, &CFullMenuDlg::OnWbResourceManger)
	ON_COMMAND(IDC_WB_FLASHB, &CRegularDllCallDlgDlg::OnWbFlashb)
	
	ON_COMMAND(IDC_WB_FULLSCREENB, &CRegularDllCallDlgDlg::OnWbFullscreenb)
	ON_COMMAND(IDC_WB_UNDOB, &CRegularDllCallDlgDlg::OnWbUndob)
	ON_COMMAND(IDC_WB_REDOB, &CRegularDllCallDlgDlg::OnWbRedob)
	ON_COMMAND(IDC_WB_CLOSEB, &CRegularDllCallDlgDlg::OnWbCloseb)

	ON_COMMAND(IDC_WB_RECScreen, &CRegularDllCallDlgDlg::OnRecscreen)
	ON_COMMAND(ID_WB_RECBck, &CRegularDllCallDlgDlg::OnWbRecbck)
	ON_WM_CLOSE()
	ON_COMMAND(IDC_WB_FUNCTIONPANELB, &CRegularDllCallDlgDlg::OnWbFunctionpanelb)
	//ON_COMMAND(IDC_WB_INSERTPAGEB, &CRegularDllCallDlgDlg::OnInsertpageb)
	//ON_COMMAND(IDC_WB_DELETEPAGEB, &CRegularDllCallDlgDlg::OnDeletepageb)
	ON_COMMAND(ID_SCREENDRAW, &CRegularDllCallDlgDlg::OnScreenDraw)
	ON_COMMAND(ID_DESTORY, &CRegularDllCallDlgDlg::OnDestory)
	ON_COMMAND(ID_FULLSCREENSHIFT, &CRegularDllCallDlgDlg::OnFullscreenShift)
	ON_COMMAND(ID_GUISESHIFT, &CRegularDllCallDlgDlg::OnGuiseshift)
	ON_COMMAND(ID_CONTROL, &CRegularDllCallDlgDlg::OnCtrlDlg)
	ON_COMMAND(IDC_WB_MEDIA, &CRegularDllCallDlgDlg::OnWbMedia)
	ON_COMMAND(IDC_WB_CAMERA, &CRegularDllCallDlgDlg::OnWbCamera)
	ON_COMMAND(IDC_WB_DUSTBIN, &CRegularDllCallDlgDlg::OnWbDustbin)
	ON_COMMAND(ID_SINVER, &CRegularDllCallDlgDlg::OnSinver)
	ON_COMMAND(ID_MULTIVER, &CRegularDllCallDlgDlg::OnMultiver)
END_MESSAGE_MAP()


// CRegularDllCallDlgDlg ��Ϣ�������

BOOL CRegularDllCallDlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//����ȫ���µĶԻ���
	m_FullMenu = new CFullMenuDlg;
	m_FullMenu->Create(IDD_FULLMENU);
	m_FullMenu->ShowWindow(SW_HIDE);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CRegularDllCallDlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRegularDllCallDlgDlg::OnPaint()
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
HCURSOR CRegularDllCallDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// void CRegularDllCallDlgDlg::OnBnClickedButton1()
// {
// 
// 	WB_ShowWhiteBoardDlg( NULL, NULL, &wndTop,CRect(0,0,1200,700),NULL);
// 	GetMenu()->EnableMenuItem(ID_SINVER,MF_DISABLED|MF_BYCOMMAND);
// 	GetMenu()->EnableMenuItem(ID_MULTIVER,MF_DISABLED|MF_BYCOMMAND);
// 	GetMenu()->EnableMenuItem(IDC_BUTTON1,MF_DISABLED|MF_BYCOMMAND);
// 	SetMenu(GetMenu());	
// } 

void CRegularDllCallDlgDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CDialogEx::OnCancel();
}

//��������
// void CRegularDllCallDlgDlg::OnOpenAttributeDlg()
// {  
// 	WB_OpenAttributeDlg();
// }
//�½�
void CRegularDllCallDlgDlg::OnWbNewfileb()
{	
	WB_Newfile();
}

//���ļ�
void CRegularDllCallDlgDlg::OnWbOpenfileb()
{

	WB_Openfile();
}

//����
void CRegularDllCallDlgDlg::OnWbSavefileb()
{

	WB_SaveWBFile();
}

//���
void CRegularDllCallDlgDlg::OnWbResavefileb()
{

	WB_Resavefile();
}

//��ӡ
void CRegularDllCallDlgDlg::OnWbPrintfileb()
{

	WB_Printfile();
}

//�ر�
void CRegularDllCallDlgDlg::OnWbClosefileb()
{

	WB_Closefile();
}

//�½�ҳ
void CRegularDllCallDlgDlg::OnWbNewpageb()
{  

	WB_SetDrawStatus( DRAW_NORMAL);
	WB_AddNewPage();
	
}

//��һҳ
void CRegularDllCallDlgDlg::OnWbForepageb()
{

	WB_SetDrawStatus( DRAW_NORMAL);
	WB_PrePage();
	
}

//��һҳ
void CRegularDllCallDlgDlg::OnWbNextpageb()
{

	WB_SetDrawStatus( DRAW_NORMAL);
	WB_NextPage();

}

//��һҳ
void CRegularDllCallDlgDlg::OnWbFirstpageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_FirstPage();
}

//���ҳ
void CRegularDllCallDlgDlg::OnWbLastpageb()
{	
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_LastPage();
}

//��һ��
void CRegularDllCallDlgDlg::OnWbForelayerb()
{

	WB_SuperLayer();
}

//��һ��
void CRegularDllCallDlgDlg::OnWbNextlayerb()
{

	WB_UnderLayer();
}

//����
void CRegularDllCallDlgDlg::OnWbToplayerb()
{

	WB_TopLayer();
}

//�ײ�
void CRegularDllCallDlgDlg::OnWbBottomlayerb()
{

	WB_BottomLayer();
}

//ֱ��
void CRegularDllCallDlgDlg::OnWbLineb()
{

	WB_SetDrawStatus( DRAW_LINE);
}

//ѡ��
void CRegularDllCallDlgDlg::OnWbSelectb()
{

	WB_SetDrawStatus( DRAW_NORMAL);
}

//����
void CRegularDllCallDlgDlg::OnWbCurveb()
{

	WB_SetDrawStatus( DRAW_POLYLINE);
}

//С��Ƥ��
void CRegularDllCallDlgDlg::OnWbLitterrubberb()
{

	WB_SetDrawStatus( DRAW_ERASE);
	WB_SetBrushSize(BRUSH_SMALL);

}

//����Ƥ��
void CRegularDllCallDlgDlg::OnWbBigrubberb()
{

	WB_SetDrawStatus( DRAW_ERASE);
	WB_SetBrushSize(BRUNSH_BIG);
}

//���
void CRegularDllCallDlgDlg::OnWbSingleobjectrubberb()
{
	
	WB_SetDrawStatus( DRAW_CLEANUP);
}

//����ɾ��
void CRegularDllCallDlgDlg::OnWbCurveselectrubberb()
{
	
	WB_SetDrawStatus( DRAW_RGNDELETE);
}

//����
void CRegularDllCallDlgDlg::OnWbRectangleb()
{

	WB_SetDrawStatus( DRAW_RECT);
}

//Բ
void CRegularDllCallDlgDlg::OnWbRoundb()
{

	WB_SetDrawStatus( DRAW_ELLIPSE);
}

//������
void CRegularDllCallDlgDlg::OnWbTriangleb()
{

	WB_SetDrawStatus( DRAW_TRIANGLE);
}

//����
void CRegularDllCallDlgDlg::OnWbRhombusb()
{
	WB_SetDrawStatus( DRAW_RHOMBUS);
}

//�����
void CRegularDllCallDlgDlg::OnWbPolygonb()
{

	WB_SetDrawStatus( DRAW_POLYGON);
}

//ѡ������
void CRegularDllCallDlgDlg::OnWbCharfontb()
{

	WB_SetCharfont();
}

//�����ı�
void CRegularDllCallDlgDlg::OnWbChartextb()
{

	WB_SetDrawStatus( DRAW_TEXT);
}

//�����ע
void CRegularDllCallDlgDlg::OnWbAnnotationsb()
{
	WB_SetDrawStatus(ADD_ANNOTATIONS);
}

//����ͼƬ
void CRegularDllCallDlgDlg::OnWbPictureb()
{

	WB_AddPicture();
}

void CRegularDllCallDlgDlg::OnLinewidthb1()
{
	//MessageBox(_T("������ѡ�������ť"));
}

//�����ĵ�
void CRegularDllCallDlgDlg::OnWbDocumentb()
{

	WB_AddDoc();
}

//����
void CRegularDllCallDlgDlg::OnWbCutscreenb()
{

	WB_Cutscreen();
}



//ɾ��
void CRegularDllCallDlgDlg::OnWbDeleteb()
{

	WB_DeleteSelectedObjects();
}
//
void CRegularDllCallDlgDlg::OnWbUccb()
{
//	MessageBox(_T("UCC"));


}

//������Դ��
void CRegularDllCallDlgDlg::OnWbResourcecatagoryb()
{

	WB_OpenResource();
}

//������Դ����� 
void CRegularDllCallDlgDlg::OnWbResourceManger()
{

	WB_OpenReManger();
}

//����flash
void CRegularDllCallDlgDlg::OnWbFlashb()
{	

	WB_AddFlash();
}

//ȫ��
void CRegularDllCallDlgDlg::OnWbFullscreenb()
{

    WB_SetFullScreen( );

	//��ʾȫ���µĲ˵��Ի���
	m_FullMenu->ShowWindow(SW_SHOW);
}


//�˳�
void CRegularDllCallDlgDlg::OnWbCloseb()
{
	GetMenu()->EnableMenuItem(ID_SINVER,MF_ENABLED|MF_BYCOMMAND);
	GetMenu()->EnableMenuItem(ID_MULTIVER,MF_ENABLED|MF_BYCOMMAND);
	SetMenu(GetMenu());
	WB_Close( );
}

//Undo��ʱ�����һ�������˵�
void CRegularDllCallDlgDlg::OnWbUndob()
{   //12-25���jin
	CMenu PopupMenu;
	if (!PopupMenu.CreatePopupMenu())
	{
		return;
	}

	CStringList *pUndoNameList = new CStringList;

	//���UNDO�б����Ƶ�pUndoNameList��
	WB_GetUndoNameList(pUndoNameList);
	//ͳ��UNDO���ܵĲ���
	m_UndoSetpCount = pUndoNameList->GetCount();
	//��ӵ����˵��Ĳ˵���
	PopupMenu.AppendMenu(MF_STRING, IDC_UNDO, _T("����"));
	PopupMenu.AppendMenu(MF_SEPARATOR);
	PopupMenu.AppendMenu(MF_STRING, IDC_CANCEL, _T("ȡ��"));
	CString str ;
	POSITION position;
	UINT MenuId = IDC_UNDO1; //�˵����ID
	if (m_UndoSetpCount <= 10)  //���˵���С��10��ʱ��
	{
		for (position = pUndoNameList->GetHeadPosition(); NULL != position ;pUndoNameList->GetNext(position))
		{
			str = pUndoNameList->GetAt(position);
			if (""!=str)
			{
				PopupMenu.InsertMenu(1,MF_BYPOSITION|MF_STRING,MenuId,_T(str));
				MenuId++;
			}

		}
	}
	else
	{
		MenuId +=m_UndoSetpCount-11;
		for (position = pUndoNameList->FindIndex(m_UndoSetpCount-10); NULL != position ;pUndoNameList->GetNext(position))
		{
			str = pUndoNameList->GetAt(position);
			if (""!=str)
			{
				PopupMenu.InsertMenu(1,MF_BYPOSITION|MF_STRING,MenuId,_T(str));
				MenuId++;
			}

		}
	}

	CPoint point;
	GetCursorPos(&point);
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	PopupMenu.DestroyMenu();
}

//Redo
void CRegularDllCallDlgDlg::OnWbRedob()
{

	WB_Redo( );
}
//�����еľ��岽��
BOOL CRegularDllCallDlgDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{   //12-25���jin
	UINT   nItemID=LOWORD(wParam);
	if(IDC_CANCEL == nItemID)
	{			      
	}
	else if(IDC_UNDO == nItemID) 
	{
	
		WB_Undo(1 );
	}
	//����ʱ�����Զ�ǰ200��ͨ������������������У���200�Ժ�Ļ�����ͨ��������������������
	else if((nItemID > IDC_CANCEL)&(nItemID < IDC_CANCEL+200))
	{
	
		int needStepCount = m_UndoSetpCount - (nItemID-IDC_CANCEL) ;
		WB_Undo((int)needStepCount);
	}		
	return CDialogEx::OnCommand(wParam, lParam);
}



void CRegularDllCallDlgDlg::OnRecscreen()
{
	
	WB_RECScreen();
}


void CRegularDllCallDlgDlg::OnWbRecbck()
{
	WB_Baffle();
}


void CRegularDllCallDlgDlg::OnClose()
{
	OnWbCloseb();
	CDialogEx::OnClose();
}

void CRegularDllCallDlgDlg::OnWbFunctionpanelb()
{
	WB_FunctionPanel();
}


void CRegularDllCallDlgDlg::OnInsertpageb()
{
	// TODO: �ڴ���������������
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_InsertNewPage();
}

void CRegularDllCallDlgDlg::OnDeletepageb()
{
	// TODO: �ڴ���������������
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_DeletePage();
}
//��Ļ��ͼ
void CRegularDllCallDlgDlg::OnScreenDraw()
{
	WB_DrawScreen(NULL);
}
//������Ļ��ͼ����
void CRegularDllCallDlgDlg::OnDestory()
{
	// TODO: �ڴ���������������
	WB_DestroyWnd();
}

//ȫ���л�����bool��������������true��ȫ������������false�ص�ȫ��ǰ��λ�úʹ�С��������ݵõ��ĵ�ǰ״̬����ʵ���뵱ǰ�෴��״̬
void CRegularDllCallDlgDlg::OnFullscreenShift()
{
	// TODO: �ڴ���������������
	bool isFull=this->IsFullScreen();
	WB_FullScreenShift(!isFull);
}

//�л�Ч������bool��������������true��͸��������������false��������������ݵõ��ĵ�ǰ״̬����ʵ���뵱ǰ�෴��״̬
void CRegularDllCallDlgDlg::OnGuiseshift()
{
	// TODO: �ڴ���������������
	bool Trans=this->IsScreenDrawTrans();
	WB_GuiseShift(!Trans );
}
//�õ��Ƿ�͸����͸������true����������false
bool CRegularDllCallDlgDlg::IsScreenDrawTrans()
{
	return WB_IsScreenDrawTrans();
}
//�õ��Ƿ�ȫ����ȫ������true�����򷵻�false
bool CRegularDllCallDlgDlg::IsFullScreen()
{
	return WB_IsFullScreen();
}
void CRegularDllCallDlgDlg::OnCtrlDlg()
{
	WB_OnCtrlDlg();
}


void CRegularDllCallDlgDlg::OnWbMedia()
{
	// TODO: �ڴ���������������
	WB_AddMedia();
}

void CRegularDllCallDlgDlg::OnWbCamera()
{
	// TODO: �ڴ���������������
	WB_AddCamera();
}


void CRegularDllCallDlgDlg::OnWbDustbin()
{
	// TODO: �ڴ���������������
	//WB_Maqnifier();
	WB_Dustbin();
}


void CRegularDllCallDlgDlg::OnSinver()
{
	// TODO: �ڴ���������������
	WB_SetTypeOfWB(WB_SINGLERVER);
	WB_ShowWhiteBoardDlg( NULL, NULL, &wndTop,CRect(0,0,1200,700),NULL);
	GetMenu()->EnableMenuItem(ID_SINVER,MF_DISABLED|MF_BYCOMMAND);
	GetMenu()->EnableMenuItem(ID_MULTIVER,MF_DISABLED|MF_BYCOMMAND);
	SetMenu(GetMenu());
}


void CRegularDllCallDlgDlg::OnMultiver()
{
	// TODO: �ڴ���������������
	WB_SetTypeOfWB(WB_MULTIVER);
	WB_ShowWhiteBoardDlg( NULL, NULL, &wndTop,CRect(0,0,1200,700),NULL);
	GetMenu()->EnableMenuItem(ID_SINVER,MF_DISABLED|MF_BYCOMMAND);
	GetMenu()->EnableMenuItem(ID_MULTIVER,MF_DISABLED|MF_BYCOMMAND);
	SetMenu(GetMenu());
}
