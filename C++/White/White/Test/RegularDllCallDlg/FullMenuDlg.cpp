// FullMenuDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RegularDllCallDlg.h"

#include "FullMenuDlg.h"
#include "afxdialogex.h"


// CFullMenuDlg �Ի���

IMPLEMENT_DYNAMIC(CFullMenuDlg, CDialog)

CFullMenuDlg::CFullMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFullMenuDlg::IDD, pParent)
{

}

CFullMenuDlg::~CFullMenuDlg()
{
}

void CFullMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFullMenuDlg, CDialog)

	ON_WM_NCHITTEST()

	ON_COMMAND(IDC_WB_NEWFILEB, &CFullMenuDlg::OnWbNewfileb)
	ON_COMMAND(IDC_WB_OPENFILEB, &CFullMenuDlg::OnWbOpenfileb)
	ON_COMMAND(IDC_WB_SAVEFILEB, &CFullMenuDlg::OnWbSavefileb)
	ON_COMMAND(IDC_WB_RESAVEFILEB, &CFullMenuDlg::OnWbResavefileb)
	ON_COMMAND(IDC_WB_PRINTFILEB, &CFullMenuDlg::OnWbPrintfileb)
	ON_COMMAND(IDC_WB_CLOSEFILEB, &CFullMenuDlg::OnWbClosefileb)

	ON_COMMAND(IDC_WB_NEWPAGEB, &CFullMenuDlg::OnWbNewpageb)
	ON_COMMAND(IDC_WB_FOREPAGEB, &CFullMenuDlg::OnWbForepageb)
	ON_COMMAND(IDC_WB_NEXTPAGEB, &CFullMenuDlg::OnWbNextpageb)
	ON_COMMAND(IDC_WB_FIRSTPAGEB, &CFullMenuDlg::OnWbFirstpageb)
	ON_COMMAND(IDC_WB_LASTPAGEB, &CFullMenuDlg::OnWbLastpageb)


	ON_COMMAND(IDC_WB_FORELAYERB, &CFullMenuDlg::OnWbForelayerb)
	ON_COMMAND(IDC_WB_NEXTLAYERB, &CFullMenuDlg::OnWbNextlayerb)
	ON_COMMAND(IDC_WB_TOPLAYERB, &CFullMenuDlg::OnWbToplayerb)
	ON_COMMAND(IDC_WB_BOTTOMLAYERB, &CFullMenuDlg::OnWbBottomlayerb)
	ON_COMMAND(IDC_WB_LINEB, &CFullMenuDlg::OnWbLineb)

	ON_COMMAND(IDC_WB_SELECTB, &CFullMenuDlg::OnWbSelectb)
	ON_COMMAND(IDC_WB_CURVEB, &CFullMenuDlg::OnWbCurveb)
	ON_COMMAND(IDC_WB_LITTERRUBBERB, &CFullMenuDlg::OnWbLitterrubberb)
	ON_COMMAND(IDC_WB_BIGRUBBERB, &CFullMenuDlg::OnWbBigrubberb)
	ON_COMMAND(IDC_WB_SINGLEOBJECTRUBBERB, &CFullMenuDlg::OnWbSingleobjectrubberb)
	ON_COMMAND(IDC_WB_CURVESELECTRUBBERB, &CFullMenuDlg::OnWbCurveselectrubberb)

	ON_COMMAND(IDC_WB_RECTANGLEB, &CFullMenuDlg::OnWbRectangleb)
	ON_COMMAND(IDC_WB_ROUNDB, &CFullMenuDlg::OnWbRoundb)
	ON_COMMAND(IDC_WB_TRIANGLEB, &CFullMenuDlg::OnWbTriangleb)
	ON_COMMAND(IDC_WB_RHOMBUSB, &CFullMenuDlg::OnWbRhombusb)
	ON_COMMAND(IDC_WB_POLYGONB, &CFullMenuDlg::OnWbPolygonb)
	ON_COMMAND(IDC_WB_CHARFONTB, &CFullMenuDlg::OnWbCharfontb)
	ON_COMMAND(IDC_WB_CHARTEXTB, &CFullMenuDlg::OnWbChartextb)

	ON_COMMAND(IDC_WB_PICTUREB, &CFullMenuDlg::OnWbPictureb)
	//ON_COMMAND(IDC_LINEWIDTHB1, &CFullMenuDlg::OnLinewidthb1)
	ON_COMMAND(IDC_WB_DOCUMENTB, &CFullMenuDlg::OnWbDocumentb)
	ON_COMMAND(IDC_WB_CUTSCREENB, &CFullMenuDlg::OnWbCutscreenb)
	ON_COMMAND(IDC_WB_DELETEB, &CFullMenuDlg::OnWbDeleteb)
	ON_COMMAND(IDC_WB_UCCB, &CFullMenuDlg::OnWbUccb)
	ON_COMMAND(IDC_WB_RESOURCECATAGORYB, &CFullMenuDlg::OnWbResourcecatagoryb)
	ON_COMMAND(IDC_RESOURCE_MANGER, &CFullMenuDlg::OnWbResourceManger)

	ON_COMMAND(IDC_WB_FLASHB, &CFullMenuDlg::OnWbFlashb)

	ON_COMMAND(IDC_WB_NORMALB, &CFullMenuDlg::OnWbNormalb)
	ON_COMMAND(IDC_WB_UNDOB, &CFullMenuDlg::OnWbUndob)
	ON_COMMAND(IDC_WB_REDOB, &CFullMenuDlg::OnWbRedob)
	ON_COMMAND(IDC_WB_DUSTBIN, &CFullMenuDlg::OnWbDustbinb)
END_MESSAGE_MAP()


// CFullMenuDlg ��Ϣ�������


BOOL CFullMenuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//��������
// void CFullMenuDlg::OnPen()
// {  
// 	WB_OpenAttributeDlg();
// 
// }
//�½�
void CFullMenuDlg::OnWbNewfileb()
{	
	WB_Newfile();
}

//���ļ�
void CFullMenuDlg::OnWbOpenfileb()
{
	WB_Openfile();
}

//����
void CFullMenuDlg::OnWbSavefileb()
{
	WB_SaveWBFile();
}

//���
void CFullMenuDlg::OnWbResavefileb()
{
	WB_Resavefile();
}

//��ӡ
void CFullMenuDlg::OnWbPrintfileb()
{
	WB_Printfile();
}

//�ر�
void CFullMenuDlg::OnWbClosefileb()
{
	WB_Closefile();
}

//�½�ҳ
void CFullMenuDlg::OnWbNewpageb()
{  
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_AddNewPage();

}

//����ҳ
void CFullMenuDlg::OnInsertpageb()
{  
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_InsertNewPage();

}

//ɾ��ҳ
void CFullMenuDlg::OnDeletepageb()
{  
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_DeletePage();

}

//��һҳ
void CFullMenuDlg::OnWbForepageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_PrePage();
}

//��һҳ
void CFullMenuDlg::OnWbNextpageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_NextPage();
}

//��һҳ
void CFullMenuDlg::OnWbFirstpageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_FirstPage();
}

//���ҳ
void CFullMenuDlg::OnWbLastpageb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
	WB_LastPage();
}

//��һ��
void CFullMenuDlg::OnWbForelayerb()
{
	WB_SuperLayer();
}

//��һ��
void CFullMenuDlg::OnWbNextlayerb()
{
	WB_UnderLayer();
}

//����
void CFullMenuDlg::OnWbToplayerb()
{
	WB_TopLayer();
}

//�ײ�
void CFullMenuDlg::OnWbBottomlayerb()
{
	WB_BottomLayer();
}

//ֱ��
void CFullMenuDlg::OnWbLineb()
{
	WB_SetDrawStatus( DRAW_LINE);
}

//ѡ��
void CFullMenuDlg::OnWbSelectb()
{
	WB_SetDrawStatus( DRAW_NORMAL);
}

//����
void CFullMenuDlg::OnWbCurveb()
{
	WB_SetDrawStatus( DRAW_POLYLINE);
}

//С��Ƥ��
void CFullMenuDlg::OnWbLitterrubberb()
{
	WB_SetDrawStatus( DRAW_ERASE);
	WB_SetBrushSize(BRUSH_SMALL);

}

//����Ƥ��
void CFullMenuDlg::OnWbBigrubberb()
{
	WB_SetDrawStatus( DRAW_ERASE);
	WB_SetBrushSize(BRUNSH_BIG);
}

//���
void CFullMenuDlg::OnWbSingleobjectrubberb()
{
	WB_SetDrawStatus( DRAW_CLEANUP);
}

//����ɾ��
void CFullMenuDlg::OnWbCurveselectrubberb()
{
	WB_SetDrawStatus( DRAW_RGNDELETE);
}

//����
void CFullMenuDlg::OnWbRectangleb()
{WB_SetDrawStatus( DRAW_RECT);
}

//Բ
void CFullMenuDlg::OnWbRoundb()
{
	WB_SetDrawStatus( DRAW_ELLIPSE);
}

//������
void CFullMenuDlg::OnWbTriangleb()
{
	WB_SetDrawStatus( DRAW_TRIANGLE);
}

//����
void CFullMenuDlg::OnWbRhombusb()
{
	WB_SetDrawStatus( DRAW_RHOMBUS);
}

//�����
void CFullMenuDlg::OnWbPolygonb()
{
	WB_SetDrawStatus( DRAW_POLYGON);
}

//ѡ������
void CFullMenuDlg::OnWbCharfontb()
{
	WB_SetCharfont();
}

//�����ı�
void CFullMenuDlg::OnWbChartextb()
{
	WB_SetDrawStatus( DRAW_TEXT);
}

//����ͼƬ
void CFullMenuDlg::OnWbPictureb()
{
	WB_AddPicture();
}

void CFullMenuDlg::OnLinewidthb1()
{
	//MessageBox(_T("������ѡ�������ť"));
}


//�����ĵ�
void CFullMenuDlg::OnWbDocumentb()
{
	WB_AddDoc();
}

//����
void CFullMenuDlg::OnWbCutscreenb()
{
	WB_Cutscreen();
}



//ɾ��
void CFullMenuDlg::OnWbDeleteb()
{
	WB_DeleteSelectedObjects();
}
//
void CFullMenuDlg::OnWbUccb()
{
	//	MessageBox(_T("UCC"));


}

//������Դ��
void CFullMenuDlg::OnWbResourcecatagoryb()
{
	WB_OpenResource();
}

//������Դ����� 
void CFullMenuDlg::OnWbResourceManger()
{
	 WB_OpenReManger();
}


//����flash
void CFullMenuDlg::OnWbFlashb()
{	
	WB_AddFlash();
}


//��ԭ�װ壬ȥ��ȫ��
void CFullMenuDlg::OnWbNormalb()
{
	WB_SetNormalScreen( );
	ShowWindow(SW_HIDE);
}

void CFullMenuDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}


void CFullMenuDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	//CDialog::OnCancel();
}

//�϶��Ի���
LRESULT CFullMenuDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);

	//return CDialog::OnNcHitTest(point);
}
//Undo��ʱ�����һ�������˵�
void CFullMenuDlg::OnWbUndob()
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


void CFullMenuDlg::OnWbRedob()
{
	WB_Redo( );
}


BOOL CFullMenuDlg::OnCommand(WPARAM wParam, LPARAM lParam)
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
	return CDialog::OnCommand(wParam, lParam);
}


void CFullMenuDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::PostNcDestroy();
	delete this;
}


void CFullMenuDlg::OnWbDustbinb()
{
	WB_AddPicture();
}
