// WBMainDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "Plugin_WB_Main.h"
#include "WBMainDlg.h"
#include "..\include\WB\WBDefine.h"
#include "..\includeUCC\IPlugin\IU7_MM\IPlugin_U7_MM_Member.h"
#include "..\includeUCC\IPlugin\IU7_MM\IPlugin_U7_MM_Function.h"
#include "AutoLockResource.h"


#define TIMER_WB_BROADCAST 10120   //��ʱ�� �¼� �Ƿ���ܵ��װ�㲥
// CWBMainDlg �Ի���

IMPLEMENT_DYNAMIC(CWBMainDlg, CDialog)

CWBMainDlg *CWBMainDlg::g_pMainDlg = NULL; //�װ�������ָ��
bool CWBMainDlg::g_isCreateWb = false;				  //�Ƿ��Ѿ������˰װ�
bool CWBMainDlg::g_isNetHPDTSConnect = false;

CWBMainDlg::CWBMainDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWBMainDlg::IDD, pParent)
, m_pHPNetNotify(NULL)
, m_pWhiteBoardDlg(NULL)
, m_hChildWnd(NULL)
, m_RectWB(NULL)
, m_pWBLoginDlg(NULL)
, m_isOnline(FALSE)
, m_isReceiveBoardcast(FALSE)
, m_isLocalBoardCastWB(FALSE)
, m_isNeedRealseTouchScreenAuthorize(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_onLineCurMeetingNumber = 0;
}

CWBMainDlg::~CWBMainDlg()
{
	if (m_pWBLoginDlg != NULL)
	{
		delete m_pWBLoginDlg;
		m_pWBLoginDlg = NULL;
	}
}

void CWBMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBERLIST_TREE, m_memberListTree);
}


BEGIN_MESSAGE_MAP(CWBMainDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(TVN_SELCHANGED, IDC_MEMBERLIST_TREE, &CWBMainDlg::OnTvnSelchangedMemberlistTree)
	ON_WM_TIMER()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CWBMainDlg ��Ϣ�������



BOOL CWBMainDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	UINT nItemID = LOWORD(wParam);
	switch (nItemID)
	{
		//�ļ��˵�
	case  IDC_LOAD_BTN : //��¼
		{
			OpenLoginDlg();
		}
		break;
	case  IDC_LOGOUT_BTN : //ע��
		{
			BOOL bl = TRUE;
			WB_SetMediaSinToIntnet(bl);
			CloseLoginDlg();
			WB_MediaSingelToNet();			
		}
		break;
	case  IDC_NEWFILE_BTN : //���ļ�
		{
			WB_Newfile();
			WB_CloseAllChildBox();
		}
		break;
	case  IDC_OPENFILE_BTN ://���ļ�
		{
			WB_Openfile();
			WB_CloseAllChildBox();
		}
		break;
	case  IDC_SAVEFILE_BTN : //�����ļ�
		{
			WB_SaveWBFile();
		}
		break;
	case  IDC_RESAVEFILE_BTN ://����ļ�
		{
			WB_Resavefile();
		}
		break;
	case  IDC_PRINTFILE_BTN : //��ӡ�ļ�
		{
			WB_Printfile();
		}
		break;
	case  IDC_CLOSEFILE_BTN ://�ر��ļ�
		{
			BOOL bIsExit = WB_SingleExit();
			WB_CloseAllChildBox();

			if (bIsExit)
			{
				if (NULL != m_pWBLoginDlg)
				{
					IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
					if (pPlugin_Function == NULL)
					{
						return FALSE;
					}
					IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
					if (NULL == pPlugin_Member)
					{
						return FALSE;
					}
					if (m_isOnline)
					{
// 						std::string strMemberID = pPlugin_Member->GetUserIDDomain();
// 						BOOL bPower =   CWBMainDlg::g_pMainDlg->LimitsOfPower();
// 						//-------------------------------------------------------------------------
// 						//�˳�����ʱ��������˳�����Ļص����� OnExitRoom
// 						//���˳������ʱ���ж��ǻ����ǲ������ˣ����û�еĻ��ͷ��͹رհװ�Ĺ㲥
// 						if (m_onLineCurMeetingNumber <= 1)
// 						{
// 							if (bPower)
// 							{
// 								//������������Ƶͨ��
// 								//������ط��жϻ�������
// 								//		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function *>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
// 								//if ( GetCurrentMeetingInfo()->roommode  == MEETING_MODE_NORMAL)
// 								pPlugin_Function->CloseBroadcast(strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
// 							}
// 						}
						IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
						if (NULL == pPlugin_Room)
						{
							return FALSE;
						}
						pPlugin_Room->ExitRoom();
						m_isReceiveBoardcast = FALSE;
						if (m_isNeedRealseTouchScreenAuthorize)//�ͷŴ�����Ȩ
						{
							pPlugin_Function->ReleaseTouchScreenAuthorize();
							m_isNeedRealseTouchScreenAuthorize = FALSE;
						}
						bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
						if (!bHardwareTouch) //Ӳ����֧�ִ���
						{
							WB_SetTouchAuthorization(FALSE);
						} 
						else
						{
							WB_SetTouchAuthorization(TRUE);
						}
						IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
						if (pPlugin_Login == NULL)
						{
							return FALSE;
						}
						m_onLineCurMeetingNumber--;
						m_pWBLoginDlg->DeleteMeetingListDlg();
						pPlugin_Login->Logout();
						SetIsOnline(FALSE);
					}
					pPlugin_Function->DelObserver(this);
					pPlugin_Member->DelObserver(this);

					delete m_pWBLoginDlg;
					m_pWBLoginDlg = NULL;
				}
				DestroyWhiteBoard();
				PostQuitMessage(0);
			}

			return TRUE;
		}
		break;
		//�༭�˵�
	case  IDC_SELECTEDIT_BTN : //ѡ��
		{
			WB_SetDrawStatus( DRAW_NORMAL);
		}
		break;
	case  IDC_UNDOEDIT_BTN ://����
		{
			WB_Undo(1);	
		}
		break;
	case  IDC_REDOEDIT_BTN : //����
		{
			WB_Redo();
		}
		break;
	case  IDC_CLONEEDIT_BTN ://��¡
		{
			WB_CloneObject();
		}
		break;
	case  IDC_INFINITECLONEEDIT_BTN : //���޿�¡
		{
			WB_InfiniteCloneObj();
		}
		break;
	case  IDC_CUTEDIT_BTN ://����
		{
			WB_CutObject();
		}
		break;
	case  IDC_COPYEDIT_BTN : //����
		{
			WB_CopyObject();
		}
		break;
	case  IDC_PASTEEDIT_BTN ://ճ��
		{
			WB_PasteObject();
		}
		break;
	case  IDC_DELETEEDIT_BTN : //ɾ��
		{
			WB_DeleteSelectedObjects();
		}
		break;
	case  IDC_DELETEALLEDIT_BTN ://ȫ��ɾ��
		{
			WB_DeleteAll();
		}
		break;
	case  IDC_PRELAYEREDIT_BTN : //��һͼ��
		{
			WB_SuperLayer();
		}
		break;
	case  IDC_NEXTLAYEREDIT_BTN ://��һͼ��
		{
			WB_UnderLayer();
		}
		break;
	case  IDC_TOPLAYER_BTN : //��һ��
		{
			WB_TopLayer();
		}
		break;
	case  IDC_BOTTOMLAYEREDIT_BTN ://�ײ�
		{
			WB_BottomLayer();
		}
		break;
	case  IDC_DEASIL45EDIT_BTN : //˳ʱ����ת45��
		{
			WB_RotateByAngle(-45);
		}
		break;
	case  IDC_DEASIL90EDIT_BTN ://˳ʱ����ת90��
		{
			WB_RotateByAngle(-90);
		}
		break;
	case  IDC_WIDDERSHINS45EDIT_BTN : //��ʱ����ת45��
		{
			WB_RotateByAngle(45);
		}
		break;
	case  IDC_WIDDERSHINS90EDIT_BTN ://��ʱ����ת90��
		{
			WB_RotateByAngle(90);
		}
		break;
	case  IDC_ROTATION180EDIT_BTN : //��ת180��
		{
			WB_RotateByAngle(180);
		}
		break;
	case  IDC_ROTATION_HIRIZONTALEDIT_BTN ://��ʱ����ת90��
		{
			WB_RotateByHV(0);
		}
		break;
	case  IDC_ROTATION_VERTICALEDIT_BTN : //��ת180��
		{
			WB_RotateByHV(-90);
		}
		break;
		//ҳ�˵�
	case  IDC_NEWPAGE_BTN ://�½�ҳ
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_AddNewPage();
		}
		break;
	case  IDC_PREPAGE_BTN : //��һҳ
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_PrePage();
		}
		break;
	case  IDC_NEXTPAGE_BTN ://��һҳ
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_NextPage();
		}
		break;
	case  IDC_FIRSTPAGE_BTN : //��һҳ
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_FirstPage();
		}
		break;
	case  IDC_LASTPAGE_EDIT ://���һҳ
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_LastPage();
		}
		break;
	case  IDC_INSERTPAGE_BTN : //����ҳ
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_InsertNewPage();
		}
		break;
	case  IDC_DELETEPAGE_BTN ://ɾ��ҳ
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_DeletePage();
		}
		break;
		//����˵�
	case  IDC_PICTUREINSERT_BTN ://ͼƬ
		{
			WB_AddPicture();
			SetFocus();
		}
		break;
	case  IDC_MEDIOINSERT_BTN : //��Ƶ
		{
			WB_AddMedia();
		}
		break;
	case  IDC_FLASHINSERT_BTN ://Flash
		{
			WB_AddFlash();
		}
		break;
	case  IDC_DOCUMENTINSERT_BTN : //�ĵ�
		{
			WB_AddDoc();
		}
		break;
	case  IDC_CAMERAINSERT_BTN ://����ͷ
		{
			WB_AddCamera();
		}
		break;
	case  IDC_TEXTINSERT_BTN : //�ı�
		{
			WB_SetDrawStatus( DRAW_TEXT);
		}
		break;
	case  IDC_FONTINSERT_BTN ://����
		{
			WB_SetCharfont();
		}
		break;
	case  IDC_ANNOTATIONSINSERT_BEN ://��ע
		{
			WB_SetDrawStatus(ADD_ANNOTATIONS);
		}
		break;
		//���Ʋ˵�
	case  IDC_LINEDRAW_BEN : //ֱ��
		{
			WB_SetDrawStatus( DRAW_LINE);
		}
		break;
	case  IDC_CURVEDRAW_BEN ://����
		{
			WB_SetDrawStatus( DRAW_POLYLINE);
		}
		break;
	case  IDC_RECTDRAW_BEN : //����
		{
			WB_SetDrawStatus( DRAW_RECT);
		}
		break;
	case  IDC_TRIANGLEDRAW_BEN ://������
		{
			WB_SetDrawStatus( DRAW_TRIANGLE);
		}
		break;
	case  IDC_ELLIPSEDRAW_BEN ://Բ
		{
			WB_SetDrawStatus( DRAW_ELLIPSE);
		}
		break;
	case  IDC_RHOMBUSDRAW_BEN : //����
		{
			WB_SetDrawStatus( DRAW_RHOMBUS);
		}
		break;
	case  IDC_POLYGONDRAW_BEN ://�����
		{
			WB_SetDrawStatus( DRAW_POLYGON);
		}
		break;
	case  IDC_BIGERRUBBERDRAW_BTN ://���
		{
			WB_SetDrawStatus( DRAW_ERASE);
			WB_SetBrushSize(10);
		}
		break;
//  	case  IDC_SMALLRUBBERDRAW_BEN : //С��
//  		{
//  			WB_SetDrawStatus( DRAW_ERASE);
//  			WB_SetBrushSize(BRUSH_SMALL);
//  		}
//  		break;
	case  IDC_CURVERUBBERDRAW_BEN ://����
		{
			WB_SetDrawStatus( DRAW_RGNDELETE);
		}
		break;
	case  IDC_OBJRUBBERDRAW_BTN ://������
		{
			WB_SetDrawStatus( DRAW_CLEANUP);
		}
		break;
		//���߲˵�
	case  IDC_BAFFLETOOL_BEN : //����
		{
			WB_Baffle();
		}
		break;
	case  IDC_SPOTLIGHTTOOL_BTN ://�۹��
		{
			WB_NewSpotlight();
		}
		break;
	case  IDC_MAQNTOOL_BTN ://�Ŵ�
		{
			WB_Maqnifier();
		}
		break;
	case  IDC_CUTSCREENTOOL_BTN : //����
		{
			WB_Cutscreen();
		}
		break;
	case  IDC_RECYCLE_BTN : //����վ
		{
			WB_Dustbin();
		}
		break;
	case  IDC_SCREENRECTOOL_BTN ://��Ļ¼��
		{
			WB_RECScreen();
			
		}
		break;
	case  IDC_RESOURCETOOL_BTN ://��ԴĿ¼
		{
			WB_OpenResource();
		}
		break;
// 	case  IDC_RESOURCEMANAGETOOL_BTN : //��Դ�����
// 		{
// 			//WB_OpenReManger();
// 		}
// 		break;
	case IDC_FUNCTIONTOOL_BTN: //�������
		{
			WB_FunctionPanel();
		}
		break;
	case  IDC_OPENSCREENDRAWTOOL_BTN ://�򿪱�ע
		{
			WB_DrawScreen(NULL);
		}
		break;
	case  IDC_CLOSESCREENDRAWTOOL_BTN ://�رձ�ע
		{
			//WB_DestroyWnd();
		}
		break;
	case  IDC_EFFECTSCREENDRAW_BTN : //Ч���л���ע
		{
			bool Trans = WB_IsScreenDrawTrans();
			WB_GuiseShift(!Trans );
		}
		break;
		//�����˵�
	case  IDC_ABOUTHELP_BTN ://����
		{
			//
			WB_AboutAvconWB();
		}
		break;
	case  IDC_LOOKHELP_BTN ://�װ�˵��
		{

		}

	}
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CWBMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	SetWindowText(_T("AVCONWB"));	   //���öԻ�������
	SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);		 //���ô������

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	m_memberAvatarLst.Create(16, 16, ILC_COLOR32, 2, 2);//����ͼ���б�
	CBitmap bm;
	bm.LoadBitmap(IDB_WBMEETINGMEMBER_ONLINE);//����Bitmap��Դ
	m_memberAvatarLst.Add(&bm, RGB(0, 0, 0));//��ӵ�ͼ���б�
	bm.DeleteObject();
	bm.LoadBitmap(IDB_WBMEETINGMEMBER_OFFLINE);
	m_memberAvatarLst.Add(&bm, RGB(0, 0, 0));
	bm.DeleteObject();
	bm.LoadBitmap(IDB_WBMEETINGROOMNAME);
	m_memberAvatarLst.Add(&bm, RGB(0, 0, 0));
	bm.DeleteObject();
	m_memberListTree.SetImageList(&m_memberAvatarLst,TVSIL_NORMAL);

	CMenu * tempSubMenw;
	//�ļ��Ӳ˵�	
	tempSubMenw = GetMenu()->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOGOUT_BTN,MF_GRAYED);

	LoginBmp.LoadBitmap(IDB_WBMENU_LOGINFILE);//��¼
	tempSubMenw->SetMenuItemBitmaps(IDC_LOAD_BTN, MF_BYCOMMAND, &LoginBmp,NULL);
	LogoutBmp.LoadBitmap(IDB_WBMENU_LOGOUTFILE);//ע��
	tempSubMenw->SetMenuItemBitmaps(IDC_LOGOUT_BTN, MF_BYCOMMAND, &LogoutBmp,NULL);
	NewFileBmp.LoadBitmap(IDB_WBMENU_NEWFILE);//�½��ļ�
	tempSubMenw->SetMenuItemBitmaps(IDC_NEWFILE_BTN, MF_BYCOMMAND, &NewFileBmp,NULL);
	OpenFileBmp.LoadBitmap(IDB_WBMENU_OPENFILE);//���ļ�
	tempSubMenw->SetMenuItemBitmaps(IDC_OPENFILE_BTN, MF_BYCOMMAND, &OpenFileBmp,NULL);
	SaveFileBmp.LoadBitmap(IDB_WBMENU_SAVEFILE);//�����ļ�
	tempSubMenw->SetMenuItemBitmaps(IDC_SAVEFILE_BTN, MF_BYCOMMAND, &SaveFileBmp,NULL);
	ResaveFileBmp.LoadBitmap(IDB_WBMENU_RESAVEFILE);//����ļ�
	tempSubMenw->SetMenuItemBitmaps(IDC_RESAVEFILE_BTN, MF_BYCOMMAND, &ResaveFileBmp,NULL);
	PrintFileBmp.LoadBitmap(IDB_WBMENU_PRINTFILE);//��ӡ�ļ�
	tempSubMenw->SetMenuItemBitmaps(IDC_PRINTFILE_BTN, MF_BYCOMMAND, &PrintFileBmp,NULL);
	CloseFileBmp.LoadBitmap(IDB_WBMENU_CLOSEFILE);//�˳��ļ�
	tempSubMenw->SetMenuItemBitmaps(IDC_CLOSEFILE_BTN, MF_BYCOMMAND, &CloseFileBmp,NULL);

	//ҳ�Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(1);
	NewPageBmp.LoadBitmap(IDB_WBMENU_NEWPAGE);//�½�ҳ
	tempSubMenw->SetMenuItemBitmaps(IDC_NEWPAGE_BTN, MF_BYCOMMAND, &NewPageBmp,NULL);
	PrePageBmp.LoadBitmap(IDB_WBMENU_PREPAGE);//��һҳ
	tempSubMenw->SetMenuItemBitmaps(IDC_PREPAGE_BTN, MF_BYCOMMAND, &PrePageBmp,NULL);
	NextPageBmp.LoadBitmap(IDB_WBMENU_NEXTPAGE);//��һҳ
	tempSubMenw->SetMenuItemBitmaps(IDC_NEXTPAGE_BTN, MF_BYCOMMAND, &NextPageBmp,NULL);
	FirstPageBmp.LoadBitmap(IDB_WBMENU_FIRSTPAGE);//��ҳ
	tempSubMenw->SetMenuItemBitmaps(IDC_FIRSTPAGE_BTN, MF_BYCOMMAND, &FirstPageBmp,NULL);
	LastPageBmp.LoadBitmap(IDB_WBMENU_LASTPAGE);//ĩҳ
	tempSubMenw->SetMenuItemBitmaps(IDC_LASTPAGE_EDIT, MF_BYCOMMAND, &LastPageBmp,NULL);
	INSERTPageBmp.LoadBitmap(IDB_WBMENU_INSERTPAGE);//����ҳ
	tempSubMenw->SetMenuItemBitmaps(IDC_INSERTPAGE_BTN, MF_BYCOMMAND, &INSERTPageBmp,NULL);
	DELETEPageBmp.LoadBitmap(IDB_WBMENU_DELETEPAGE);//ɾ��ҳ
	tempSubMenw->SetMenuItemBitmaps(IDC_DELETEPAGE_BTN, MF_BYCOMMAND, &DELETEPageBmp,NULL);

	//�༭�Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(2);
	SelectBmp.LoadBitmap(IDB_WBMENU_SELECTEDIT);//ѡ��
	tempSubMenw->SetMenuItemBitmaps(IDC_SELECTEDIT_BTN, MF_BYCOMMAND, &SelectBmp,NULL);
	UndoBmp.LoadBitmap(IDB_WBMENU_UNDOEDIT);//����
	tempSubMenw->SetMenuItemBitmaps(IDC_UNDOEDIT_BTN, MF_BYCOMMAND, &UndoBmp,NULL);
	RedoBmp.LoadBitmap(IDB_WBMENU_REDOEDIT);//����
	tempSubMenw->SetMenuItemBitmaps(IDC_REDOEDIT_BTN, MF_BYCOMMAND, &RedoBmp,NULL);
	CloneBmp.LoadBitmap(IDB_WBMENU_CLONEEDIT);//��¡
	tempSubMenw->SetMenuItemBitmaps(IDC_CLONEEDIT_BTN, MF_BYCOMMAND, &CloneBmp,NULL);
	InfinitCloneBmp.LoadBitmap(IDB_WBMENU_INTINITEEDIT);//���޿�¡
	tempSubMenw->SetMenuItemBitmaps(IDC_INFINITECLONEEDIT_BTN, MF_BYCOMMAND, &InfinitCloneBmp,NULL);
	CutBmp.LoadBitmap(IDB_WBMENU_CUTEDIT);//����
	tempSubMenw->SetMenuItemBitmaps(IDC_CUTEDIT_BTN, MF_BYCOMMAND, &CutBmp,NULL);
	CopyBmp.LoadBitmap(IDB_WBMENU_COPYEDIT);//����
	tempSubMenw->SetMenuItemBitmaps(IDC_COPYEDIT_BTN, MF_BYCOMMAND, &CopyBmp,NULL);
	PasteBmp.LoadBitmap(IDB_WBMENU_PASTEEDIT);//ճ��
	tempSubMenw->SetMenuItemBitmaps(IDC_PASTEEDIT_BTN, MF_BYCOMMAND, &PasteBmp,NULL);
	DeleteBmp.LoadBitmap(IDB_WBMENU_DELETEEDIT); //ɾ��
	tempSubMenw->SetMenuItemBitmaps(IDC_DELETEEDIT_BTN, MF_BYCOMMAND, &DeleteBmp,NULL);
	DeleteAllBmp.LoadBitmap(IDB_WBMENU_DELETEALLEDIT); //ȫ��ɾ��
	tempSubMenw->SetMenuItemBitmaps(IDC_DELETEALLEDIT_BTN, MF_BYCOMMAND, &DeleteAllBmp,NULL);
	//�༭�˵���ͼ���Ӳ˵�
	PreLayerBmp.LoadBitmap(IDB_WBMENU_PRELAYEREDIT); //��һ��
	tempSubMenw->SetMenuItemBitmaps(IDC_PRELAYEREDIT_BTN, MF_BYCOMMAND, &PreLayerBmp,NULL);
	NextLayerBmp.LoadBitmap(IDB_WBMENU_NEXTLAYEREDIT); //��һ��
	tempSubMenw->SetMenuItemBitmaps(IDC_NEXTLAYEREDIT_BTN, MF_BYCOMMAND, &NextLayerBmp,NULL);
	TopLayerBmp.LoadBitmap(IDB_WBMENU_TOPLAYEREDIT); //����
	tempSubMenw->SetMenuItemBitmaps(IDC_TOPLAYER_BTN, MF_BYCOMMAND, &TopLayerBmp,NULL);
	BottomLayerBmp.LoadBitmap(IDB_WBMENU_BOTTOMLAYEREDIT); //ĩ��
	tempSubMenw->SetMenuItemBitmaps(IDC_BOTTOMLAYEREDIT_BTN, MF_BYCOMMAND, &BottomLayerBmp,NULL);

	//�༭�˵�����ת�Ӳ˵�
	Deasil45Bmp.LoadBitmap(IDB_WBMENU_DEASIL45EDIT); //˳ʱ����ת45��
	tempSubMenw->SetMenuItemBitmaps(IDC_DEASIL45EDIT_BTN, MF_BYCOMMAND, &Deasil45Bmp,NULL);
	Deasil90Bmp.LoadBitmap(IDB_WBMENU_DEASIL90EDIT); //˳ʱ����ת90��
	tempSubMenw->SetMenuItemBitmaps(IDC_DEASIL90EDIT_BTN, MF_BYCOMMAND, &Deasil90Bmp,NULL);
	Widderstin45Bmp.LoadBitmap(IDB_WBMENU_WIDDERSHINS45EDIT); //��ʱ����ת45��
	tempSubMenw->SetMenuItemBitmaps(IDC_WIDDERSHINS45EDIT_BTN, MF_BYCOMMAND, &Widderstin45Bmp,NULL);
	Widderstin90Bmp.LoadBitmap(IDB_WBMENU_WIDDERSHINS90EDIT); //��ʱ����ת90��
	tempSubMenw->SetMenuItemBitmaps(IDC_WIDDERSHINS90EDIT_BTN, MF_BYCOMMAND, &Widderstin90Bmp,NULL);
	Rotation180Bmp.LoadBitmap(IDB_WBMENU_ROTATION180EDIT); //��ת180��
	tempSubMenw->SetMenuItemBitmaps(IDC_ROTATION180EDIT_BTN, MF_BYCOMMAND, &Rotation180Bmp,NULL);
	HirizontalBmp.LoadBitmap(IDB_WBMENU_HIRIZONTALEDIT); //ˮƽ����
	tempSubMenw->SetMenuItemBitmaps(IDC_ROTATION_HIRIZONTALEDIT_BTN, MF_BYCOMMAND, &HirizontalBmp,NULL);
	VerticalBmp.LoadBitmap(IDB_WBMENU_VERTICALEDIT); //��ֱ����
	tempSubMenw->SetMenuItemBitmaps(IDC_ROTATION_VERTICALEDIT_BTN, MF_BYCOMMAND, &VerticalBmp,NULL);

	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(3);
	PictureBmp.LoadBitmap(IDB_WBMENU_PICTUREINSERT); //ͼƬ
	tempSubMenw->SetMenuItemBitmaps(IDC_PICTUREINSERT_BTN, MF_BYCOMMAND, &PictureBmp,NULL);
	VedioBmp.LoadBitmap(IDB_WBMENU_VEDIOINSERT); //��Ƶ
	tempSubMenw->SetMenuItemBitmaps(IDC_MEDIOINSERT_BTN, MF_BYCOMMAND, &VedioBmp,NULL);
	FlashBmp.LoadBitmap(IDB_WBMENU_FLASHINSERT); //FLASH
	tempSubMenw->SetMenuItemBitmaps(IDC_FLASHINSERT_BTN, MF_BYCOMMAND, &FlashBmp,NULL);
	DocumentsBmp.LoadBitmap(IDB_WBMENU_DOCUMENTSINSERT); //�ĵ�
	tempSubMenw->SetMenuItemBitmaps(IDC_DOCUMENTINSERT_BTN, MF_BYCOMMAND, &DocumentsBmp,NULL);
	CameraheadBmp.LoadBitmap(IDB_WBMENU_CAMERAHEADSINSERT); //����ͷ
	tempSubMenw->SetMenuItemBitmaps(IDC_CAMERAINSERT_BTN, MF_BYCOMMAND, &CameraheadBmp,NULL);
	
	tempSubMenw->DeleteMenu(4,MF_BYPOSITION);//��������ͷ
	
	TextBmp.LoadBitmap(IDB_WBMENU_WRITINGSINSERT); //�ı�
	tempSubMenw->SetMenuItemBitmaps(IDC_TEXTINSERT_BTN, MF_BYCOMMAND, &TextBmp,NULL);
	AnnotationBmp.LoadBitmap(IDB_WBMENU_ANNOTATIONSINSERT); //��ע
	tempSubMenw->SetMenuItemBitmaps(IDC_ANNOTATIONSINSERT_BEN, MF_BYCOMMAND, &AnnotationBmp,NULL);

	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(4);
	BeelinesBmp.LoadBitmap(IDB_WBMENU_BEELINESDRAW); //ֱ��
	tempSubMenw->SetMenuItemBitmaps(IDC_LINEDRAW_BEN, MF_BYCOMMAND, &BeelinesBmp,NULL);
	CurvesBmp.LoadBitmap(IDB_WBMENU_CURVESDRAW); //����
	tempSubMenw->SetMenuItemBitmaps(IDC_CURVEDRAW_BEN, MF_BYCOMMAND, &CurvesBmp,NULL);
	RectBmp.LoadBitmap(IDB_WBMENU_RECTDRAW); //����
	tempSubMenw->SetMenuItemBitmaps(IDC_RECTDRAW_BEN, MF_BYCOMMAND, &RectBmp,NULL);
	TrianglesBmp.LoadBitmap(IDB_WBMENU_TRIANGLESDRAW); //������
	tempSubMenw->SetMenuItemBitmaps(IDC_TRIANGLEDRAW_BEN, MF_BYCOMMAND, &TrianglesBmp,NULL);
	RoundBmp.LoadBitmap(IDB_WBMENU_ROUNDDRAW); //Բ
	tempSubMenw->SetMenuItemBitmaps(IDC_ELLIPSEDRAW_BEN, MF_BYCOMMAND, &RoundBmp,NULL);
	RhombusBmp.LoadBitmap(IDB_WBMENU_RHOMBUSDRAW); //����
	tempSubMenw->SetMenuItemBitmaps(IDC_RHOMBUSDRAW_BEN, MF_BYCOMMAND, &RhombusBmp,NULL);
	BigRubberBmp.LoadBitmap(IDB_WBMENU_BIGRUBBERSDRAW); //���
	tempSubMenw->SetMenuItemBitmaps(IDC_BIGERRUBBERDRAW_BTN, MF_BYCOMMAND, &BigRubberBmp,NULL);
// 	SmallRubberBmp.LoadBitmap(IDB_WBMENU_LITTERRUBBERSDRAW); //С��
// 	tempSubMenw->SetMenuItemBitmaps(IDC_SMALLRUBBERDRAW_BEN, MF_BYCOMMAND, &SmallRubberBmp,NULL);
	CurveRubbersBmp.LoadBitmap(IDB_WBMENU_CURVERUBBERSDRAW); //����
	tempSubMenw->SetMenuItemBitmaps(IDC_CURVERUBBERDRAW_BEN, MF_BYCOMMAND, &CurveRubbersBmp,NULL);
	ObjRubbersBmp.LoadBitmap(IDB_WBMENU_OBJRUBBERSDRAW); //������
	tempSubMenw->SetMenuItemBitmaps(IDC_OBJRUBBERDRAW_BTN, MF_BYCOMMAND, &ObjRubbersBmp,NULL);

	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(5);
	BaffleBmp.LoadBitmap(IDB_WBMENU_BAFFLESTOOL); //����
	tempSubMenw->SetMenuItemBitmaps(IDC_BAFFLETOOL_BEN, MF_BYCOMMAND, &BaffleBmp,NULL);
	SpotlightBmp.LoadBitmap(IDB_WBMENU_SPOTLIGHTSTOOL); //�۹��
	tempSubMenw->SetMenuItemBitmaps(IDC_SPOTLIGHTTOOL_BTN, MF_BYCOMMAND, &SpotlightBmp,NULL);
	MagnifierBmp.LoadBitmap(IDB_WBMENU_MAGNIFIERSTOOL); //�Ŵ�
	tempSubMenw->SetMenuItemBitmaps(IDC_MAQNTOOL_BTN, MF_BYCOMMAND, &MagnifierBmp,NULL);
	CutScreenBmp.LoadBitmap(IDB_WBMENU_CUTSCREENSTOOL); //����
	tempSubMenw->SetMenuItemBitmaps(IDC_CUTSCREENTOOL_BTN, MF_BYCOMMAND, &CutScreenBmp,NULL);
	RecycleBmp.LoadBitmap(IDB_WBMENU_RECYCLE); //����վ
	tempSubMenw->SetMenuItemBitmaps(IDC_RECYCLE_BTN, MF_BYCOMMAND, &RecycleBmp,NULL);
	ScreenRECBmp.LoadBitmap(IDB_WBMENU_SCREENRECSTOOL); //��Ļ¼��
	tempSubMenw->SetMenuItemBitmaps(IDC_SCREENRECTOOL_BTN, MF_BYCOMMAND, &ScreenRECBmp,NULL);
	DrawDesktopOpenBmp.LoadBitmap(IDB_WBMENU_DDOTOOL); //�򿪱�ע
	tempSubMenw->SetMenuItemBitmaps(IDC_OPENSCREENDRAWTOOL_BTN, MF_BYCOMMAND, &DrawDesktopOpenBmp,NULL);
	DrawDesktopEffectSwitchBmp.LoadBitmap(IDB_WBMENU_DDESTOOL); //Ч���л�
	tempSubMenw->SetMenuItemBitmaps(IDC_EFFECTSCREENDRAW_BTN, MF_BYCOMMAND, &DrawDesktopEffectSwitchBmp,NULL);
	DrawDesktopCloseBmp.LoadBitmap(IDB_WBMENU_DDCTOOLL); //�رձ�ע
	tempSubMenw->SetMenuItemBitmaps(IDC_CLOSESCREENDRAWTOOL_BTN, MF_BYCOMMAND, &DrawDesktopCloseBmp,NULL);
	ResourceListBmp.LoadBitmap(IDB_WBMENU_RRESOURCELISTSTOOL); //��ԴĿ¼
	tempSubMenw->SetMenuItemBitmaps(IDC_RESOURCETOOL_BTN, MF_BYCOMMAND, &ResourceListBmp,NULL);
	ResourceManageBmp.LoadBitmap(IDB_WBMENU_RESOURCEMANAGESTOOL); //��ԴĿ¼����
	//tempSubMenw->SetMenuItemBitmaps(IDC_RESOURCEMANAGETOOL_BTN, MF_BYCOMMAND, &ResourceManageBmp,NULL);
	FunctionDlgBmp.LoadBitmap(IDB_WBMENU_FUNCTIONDLGSTOOL); //�������
	tempSubMenw->SetMenuItemBitmaps(IDC_FUNCTIONTOOL_BTN, MF_BYCOMMAND, &FunctionDlgBmp,NULL);
	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(6);
	AboutBmp.LoadBitmap(IDB_WBMENU_ABOUTHELP); //����
	tempSubMenw->SetMenuItemBitmaps(IDC_ABOUTHELP_BTN, MF_BYCOMMAND, &AboutBmp,NULL);
	HelpBmp.LoadBitmap(IDB_WBMENU_HELP); //�鿴����
	tempSubMenw->SetMenuItemBitmaps(IDC_LOOKHELP_BTN, MF_BYCOMMAND, &HelpBmp,NULL);


	GetMenu()->GetSubMenu(1)->DeleteMenu(6,MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->DeleteMenu(5,MF_BYPOSITION);

	GetClientRect(&m_RectWB);
	SetWBStatic();


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CWBMainDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnCancel();
}
void CWBMainDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//	CDialog::OnOK();
}

BOOL CWBMainDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return CDialog::OnEraseBkgnd(pDC);
}

BOOL CWBMainDlg::PreTranslateMessage(MSG* pMsg)
{
	//���μ���Enter��Ϣ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return FALSE;
	}
	//���μ���Esc��Ϣ
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CWBMainDlg::SetHPDTSNotify( IHPNetNotify *pHPNetNotify )
{
	m_pHPNetNotify = (void*)pHPNetNotify;
	WB_SetDataNotify(m_pHPNetNotify);
}

void* CWBMainDlg::GetHPDTSNotify()
{
	return m_pHPNetNotify;
}
void CWBMainDlg::SetIsOnline( BOOL isOnline )
{
	m_isOnline = isOnline;
}

void CWBMainDlg::SetWBStatic()
{
	if (m_isOnline)
	{   //�����
		CMenu * tempSubMenw;
		//�ļ��Ӳ˵�	
		tempSubMenw = GetMenu()->GetSubMenu(0);
		tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_GRAYED);
		tempSubMenw->EnableMenuItem(IDC_LOGOUT_BTN,MF_ENABLED);
		ShowWindow(SW_SHOWMAXIMIZED);
		CRect rect;
		GetClientRect(&rect);
		m_RectWB = CRect((rect.left + 150),rect.top,rect.right,rect.bottom);
		m_memberListTree.MoveWindow(rect.left,rect.top,150,rect.bottom - rect.top);
		m_memberListTree.ShowWindow(SW_SHOW);
		// 		if (NULL == m_pWhiteBoardDlg )
		// 		{
		// 			HWND hwnd = CreateWhiteBoard();
		// 			UploadChildWnd(hwnd);
		// 		}
		// 		SetHPDTSNotify((IHPNetNotify*)m_pWhiteBoardDlg);		   //�趨ͨѶָ�� m_pWhiteBoardDlg�Ǽ̳нӿ���IHPNetNotify ����ʵ����sendData����
		// 		this->Connect();			 //�������ӵײ���ͨѶ ֻ�м�����һ�仰����������ʵ��ͨѶ
	} 
	else
	{
		//������
		CMenu * tempSubMenw;
		//�ļ��Ӳ˵�	
		tempSubMenw = GetMenu()->GetSubMenu(0);
		tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_ENABLED);
		tempSubMenw->EnableMenuItem(IDC_LOGOUT_BTN,MF_GRAYED);
		ModifyStyleEx(0,WS_EX_APPWINDOW); 
		ShowWindow(SW_SHOWMAXIMIZED);
		CRect rect;
		GetClientRect(&rect);
		m_RectWB = rect;
		m_memberListTree.ShowWindow(SW_HIDE);

		//	g_isCreateWb = WB_ShowWhiteBoardDlg( this, NULL, &wndTop,rect,NULL);
		HWND hwnd = CreateWhiteBoard();
		UploadChildWnd(hwnd);
	  // SetHPDTSNotify(NULL);  
		SetHPDTSNotify((IHPNetNotify*)m_pWhiteBoardDlg);	//add by  yang 2-28
		SetCursor(LoadCursor(NULL,IDC_ARROW));

	}
}

void CWBMainDlg::OpenLoginDlg()
{
	if (m_pWBLoginDlg == NULL)
	{
		m_pWBLoginDlg = new CWBLoginDlg();
		m_pWBLoginDlg->Create(CWBLoginDlg::IDD, NULL);
		m_pWBLoginDlg->ChangeLoginOrCancel();
		m_pWBLoginDlg->ShowWindow(SW_SHOW);
		WB_CloseAllChildBox();

		//��ӻ����Ա�۲���
		IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member*>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if (pPlugin_Member == NULL)
		{
			return ;
		}
		pPlugin_Member->AddObserver(this);

		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if (pPlugin_Function == NULL)
		{
			return ;
		}
		pPlugin_Function->AddObserver(this);
	}
}
void CWBMainDlg::CloseLoginDlg()
{
	if (NULL != m_pWBLoginDlg)
	{
		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if (pPlugin_Function == NULL)
		{
			return ;
		}
		IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if (NULL == pPlugin_Member)
		{
			return ;
		}
		if (m_isOnline)
		{
//			std::string strMemberID = pPlugin_Member->GetUserIDDomain();
//			BOOL bPower = LimitsOfPower();
			//-------------------------------------------------------------------------
			//�˳�����ʱ��������˳�����Ļص����� OnExitRoom
			//���˳������ʱ���ж��ǻ����ǲ������ˣ����û�еĻ��ͷ��͹رհװ�Ĺ㲥
// 			if (m_onLineCurMeetingNumber <= 1)
// 			{
// 				if (bPower)
// 				{
// 					//������������Ƶͨ��
// 					//������ط��жϻ�������
// 					//		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function *>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
// 					//if ( GetCurrentMeetingInfo()->roommode  == MEETING_MODE_NORMAL)
// 					pPlugin_Function->CloseBroadcast(strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
// 				}
// 			}

			IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
			if (NULL == pPlugin_Room)
			{
				return ;
			}
			pPlugin_Room->ExitRoom();
			m_isReceiveBoardcast = FALSE;
			if (m_isNeedRealseTouchScreenAuthorize)//�ͷŷ�����������Ȩ
			{
				pPlugin_Function->ReleaseTouchScreenAuthorize();
				m_isNeedRealseTouchScreenAuthorize = FALSE;
			}
			//�ſ�������İװ�����Ȩ�����Ӳ��֧��
			bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
			if (!bHardwareTouch) //Ӳ����֧�ִ���
			{
				WB_SetTouchAuthorization(FALSE);
			} 
			else
			{
                 WB_SetTouchAuthorization(TRUE);
			}
			IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
			if (pPlugin_Login == NULL)
			{
				return ;
			}
			m_onLineCurMeetingNumber--;
			m_pWBLoginDlg->DeleteMeetingListDlg();
			pPlugin_Login->Logout();
			SetIsOnline(FALSE);
		}
		pPlugin_Function->DelObserver(this);
		pPlugin_Member->DelObserver(this);

		delete m_pWBLoginDlg;
		m_pWBLoginDlg = NULL;
	}
	CString strItem;
	strItem =(CString)"AVCONWB���Ӱװ�";
	SetWindowText(strItem);//���ðװ�Ի���ı�������

	WB_CloseAllChildBox();//wangxiaobin
	m_isLocalBoardCastWB = FALSE;
  	m_pWhiteBoardDlg->ReleaseConnection();

	SetHPDTSNotify(NULL); 


	SetWBStatic();
}

void CWBMainDlg::DeleteLoginDlg()
{
	if (NULL != m_pWBLoginDlg)
	{
		delete m_pWBLoginDlg;
		m_pWBLoginDlg = NULL;
	}
	CMenu * tempSubMenw;
	//�ļ��Ӳ˵�	
	tempSubMenw = (CWBMainDlg::g_pMainDlg->GetMenu())->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOAD_BTN,MF_ENABLED);
}

HWND CWBMainDlg::GetHwnd()
{
	if (m_pWhiteBoardDlg)
	{
		return m_pWhiteBoardDlg->GetSafeHwnd();
	}
	return NULL;
}

HWND CWBMainDlg::CreateWhiteBoard()
{
	if (NULL == m_pWhiteBoardDlg)
	{
		AutoLockResource l;
		m_pWhiteBoardDlg = new CIndependent_WBDlg(0);
		m_pWhiteBoardDlg->Create(CIndependent_WBDlg::IDD, this);	//��m_pWhiteBoardDlg�ĸ����ڸĳ�CWBMainDlg
		if (m_pWhiteBoardDlg->GetSafeHwnd() == NULL)
		{
			delete m_pWhiteBoardDlg;
			m_pWhiteBoardDlg = NULL;
			return NULL;
		}
		WB_SetTypeOfWB(WB_SINGLERVER);
		m_pWhiteBoardDlg->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
		//�ſ�������İװ�����Ȩ�����Ӳ��֧��
		bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
		if (!bHardwareTouch) //Ӳ����֧�ִ���
		{
			WB_SetTouchAuthorization(FALSE);
		} 
		else
		{
			WB_SetTouchAuthorization(TRUE);
		}
	}

	return m_pWhiteBoardDlg->GetSafeHwnd();
}

void CWBMainDlg::DestroyWhiteBoard()
{
	if (m_pWhiteBoardDlg)
	{
		//m_pWhiteBoardDlg->ShowWindow(SW_HIDE);
		//WB_Close();
		m_pWhiteBoardDlg->DestroyWindow();
		delete m_pWhiteBoardDlg;
		m_pWhiteBoardDlg = NULL;
	}
}

bool CWBMainDlg::Connect()
{
	if (NULL == m_pWhiteBoardDlg)
		return false;

	IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member");
	IPlugin_U7_MM_Member* pU7MMMember = dynamic_cast<IPlugin_U7_MM_Member*>(pIPlugin);
	if (NULL == pU7MMMember)
		return false;

	std::string strLocalUserID = pU7MMMember->GetUserIDDomain();
	PMMS_CARD pCardItem = pU7MMMember->GetCardItem(strLocalUserID.c_str(), 103);
	if (NULL == pCardItem)
		return false;

	m_pWhiteBoardDlg->SetLocalUserID(strLocalUserID);
	return m_pWhiteBoardDlg->Connect(pCardItem->videochnid, pCardItem->mcuid, pCardItem->mcuaddr, pCardItem->mcuport, "");
}

void CWBMainDlg::DisConnect()
{
	if (m_pWhiteBoardDlg)
	{
		m_pWhiteBoardDlg->ReleaseConnection();
	}
}

void CWBMainDlg::RequestAllObj()
{
	WB_ReqAllObj();
}

void CWBMainDlg::SendAllObj()
{
   WB_SendAllObj();
}


void CWBMainDlg::CloseWBFile()
{
	WB_Closefile();
}

void CWBMainDlg::SetLocalUserID(char *strUserID)
{
	WB_SetLocalUserID(strUserID);
}

void CWBMainDlg::MemberOffline(char *strUserID)
{
	WB_MemberOffline(strUserID);
}

void CWBMainDlg::Move(int x,int y,int cx,int cy,UINT nFlag)
{
	WB_MoveBorad(&CWnd::wndTop, CRect(x, y, cx, cy), nFlag);
}

void CWBMainDlg::ProcessWbData(const char *pData, int nLen)
{
	WB_ProcessDataIn(pData, nLen);
}

void CWBMainDlg::SendWbData(const char* pData,int iLen,bool bToNet)
{
	if (bToNet)
	{
		m_pWhiteBoardDlg->ProcessUIData(pData, iLen);
	}
	else
	{

	/*	int iCmd = *(int*)(pData + sizeof(UINT)+sizeof(unsigned int));
		switch (iCmd)
		{
		case CMD_SINGLE_QUIT:
			{
				if (NULL != m_pWBLoginDlg)
				{
					IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
					if (pPlugin_Function == NULL)
					{
						return ;
					}
					IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
					if (NULL == pPlugin_Member)
					{
						return ;
					}
					if (m_isOnline)
					{
						IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
						if (NULL == pPlugin_Room)
						{
							return ;
						}
						pPlugin_Room->ExitRoom();
						m_isReceiveBoardcast = FALSE;
						if (m_isNeedRealseTouchScreenAuthorize)//�ͷŴ�����Ȩ
						{
							pPlugin_Function->ReleaseTouchScreenAuthorize();
							m_isNeedRealseTouchScreenAuthorize = FALSE;
						}
						IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
						if (pPlugin_Login == NULL)
						{
							return ;
						}
						m_onLineCurMeetingNumber--;
						m_pWBLoginDlg->DeleteMeetingListDlg();
						pPlugin_Login->Logout();
						SetIsOnline(FALSE);
					}
					pPlugin_Function->DelObserver(this);
					pPlugin_Member->DelObserver(this);

					delete m_pWBLoginDlg;
					m_pWBLoginDlg = NULL;
				}
				DestroyWhiteBoard();
				PostQuitMessage(0);		
			}

			break;

		case CMD_SINGLE_NETWORK:
			{
				OpenLoginDlg();
			}
			break;
		default:
			;
		}*/
	}
	
}

void CWBMainDlg::UploadChildWnd( HWND HChildWnd )
{
	CWnd* pWnd = CWnd::FromHandle(HChildWnd);
	if (NULL != pWnd)
	{
		m_hChildWnd = HChildWnd;
		//			pWnd->SetOwner(this);
		LONG style = ::GetWindowLong(HChildWnd,GWL_STYLE);
		style &= ~WS_POPUP;
		style |= WS_CHILD;
		::SetWindowLong(HChildWnd, GWL_STYLE, style);
		::SetParent(HChildWnd,m_hWnd);
		pWnd->MoveWindow(&m_RectWB,true);
		pWnd->ShowWindow(SW_SHOW);
		//    	WB_SetDataNotify(NULL);
		//	WB_ReqAllObj();
		//	m_isOnline = true;
	}
}

void CWBMainDlg::InitMemberList()
{
	if (m_memberListTree.GetCount() != 0)
	{
		m_memberListTree.DeleteAllItems();
	}
	m_memberlistTreeRoot = m_memberListTree.InsertItem(*((m_pWBLoginDlg->GetMeetingListDlg())->GetSelectMeetingName()),2,2);

	IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (pPlugin_Member == NULL)
	{
		return;
	}

	MMS_MEMBER_LST lstMember = pPlugin_Member->GetMemberList();
	MMS_MEMBER_LST::iterator it = lstMember.begin();
	HTREEITEM hItem;
	for (; it != lstMember.end(); it++)//�������߳�Ա�б�
	{		
		PMMS_MEMBER_EX pMember = *it;
		if (pMember->status == 0)
		{
			continue;
		}
		else
		{
			hItem = m_memberListTree.InsertItem(A2U(pMember->membername),0,0,m_memberlistTreeRoot);
			m_onLineCurMeetingNumber ++; 
		}
		m_memberListTree.SetItemData(hItem, (DWORD)pMember);
	}
	/*	m_onLineCurMeetingNumber -= 1;*/
	m_lastOnlineMemberHitem = hItem;//�õ����һ�����߳�Ա��
	it = lstMember.begin();
	for (; it != lstMember.end(); it++)//�������߳�Ա�б�
	{		
		PMMS_MEMBER_EX pMember = *it;
		if (pMember->status == 1)
		{
			continue;
		}
		else
		{
			hItem = m_memberListTree.InsertItem(A2U(pMember->membername),1,1,m_memberlistTreeRoot);
		}
		m_memberListTree.SetItemData(hItem, (DWORD)pMember);
	}
	m_lastOfflineMemberHitem = hItem;//�õ����һ�����߳�Ա��
	m_memberListTree.Expand(m_memberlistTreeRoot, TVE_EXPAND);
}

void CWBMainDlg::UpdateMemberStatus( std::string strMemberID, unsigned char nStatus )
{
	//HTREEITEM hItem = m_memberListTree.GetFirstVisibleItem();
	HTREEITEM hItem = m_memberListTree.GetChildItem(m_memberlistTreeRoot);
	if (hItem == NULL)
	{
		return;
	}
	for (UINT i = 0; i < m_memberListTree.GetCount(); i++)
	{
		PMMS_MEMBER_EX pMember = (PMMS_MEMBER_EX)m_memberListTree.GetItemData(hItem);
		if (pMember->memberid == strMemberID)
		{
			// 			CString meetingname = m_memberListTree.GetItemText(m_memberlistTreeRoot);

			// 			MMS_ROOM_LST lstRoom = GetDataHandle().GetRoomList();
			// 			MMS_ROOM_LST::iterator it = lstRoom.begin();
			// /*			PMMS_ROOM pRoom;*/

			if (nStatus == 0)//�˳�����
			{
				//m_lastOnlineMemberHitem = m_memberListTree.GetPrevSiblingItem(hItem);	
				pMember->status = 0;
				m_memberListTree.DeleteItem(hItem);
				m_lastOfflineMemberHitem = m_memberListTree.InsertItem(A2U(pMember->membername),1,1,m_memberlistTreeRoot,m_lastOfflineMemberHitem);
				m_memberListTree.SetItemData(m_lastOfflineMemberHitem,(DWORD)pMember);
				WB_MemberOffline(strMemberID.c_str());//wangxiaobin
				m_onLineCurMeetingNumber--;

			} 
			else//�������
			{
				pMember->status = 1;
				int nImage;
				int nSelectedImage;
				m_memberListTree.GetItemImage(hItem,nImage,nSelectedImage);
				if (nImage == 0)
				{
					;
				}
				else
				{
					m_lastOfflineMemberHitem = m_memberListTree.GetPrevSiblingItem(hItem);
					m_memberListTree.DeleteItem(hItem);
					HTREEITEM nhitem;
					nhitem = m_memberListTree.InsertItem(A2U(pMember->membername),0,0,m_memberlistTreeRoot,m_memberListTree.GetChildItem(m_memberlistTreeRoot));
					m_memberListTree.SetItemData(nhitem,(DWORD)pMember);

				}

				CString strLoginID;
				strLoginID = *(m_pWBLoginDlg->GetLoginID());
				strLoginID = strLoginID + _T("@") + A2U(pMember->domain);

				if (A2U(strMemberID) != strLoginID)
				{
					m_onLineCurMeetingNumber ++;
				}
			}
			break;
		}
		hItem = m_memberListTree.GetNextItem(hItem,TVGN_NEXT);		
	}
}

int CWBMainDlg::GetOnlineMemberCounts()
{
	return m_onLineCurMeetingNumber;
}

void CWBMainDlg::OnTvnSelchangedMemberlistTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CWBMainDlg::OnRoomMemberItemEnd(void)
{
	InitMemberList();
	//�������Ա״̬ȫ��������ϣ����жϻ����ǲ������ˣ����û�˵Ļ��ͷ��ʹ򿪰װ�㲥
	int nOnlineCurMeetingSum = CWBMainDlg::g_pMainDlg->GetOnlineMemberCounts();

	if (nOnlineCurMeetingSum <= 1)
	{
		// 		int nScreenID = 2, nWindowID = 0;
		// 		CUICmdSender::Instance().AVCast(GetDataHandle().GetUserID(), \
		// 			HPWBCHANNEL_INDEX, HPAV_TYPE_VIDEO,nScreenID, nWindowID);		
		//������������Ƶͨ��
		if (m_isReceiveBoardcast)
		{
			m_isReceiveBoardcast = FALSE;
			return;
		}
		if (m_usMemberType >4)
		{
			HWND hwnd = CreateWhiteBoard();
			SetIsOnline(TRUE);
			SetWBStatic();
			UploadChildWnd(hwnd);
			AfxMessageBox(_T("�����ڵ��û�����Ȩ�㲥����װ壬�����������Ա��ϵ��"));
			return;
		}
		IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if (NULL == pMember)
			return;
		std::string strMemberID =pMember->GetUserIDDomain();

		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		pPlugin_Function ->PublishLocalCard(HPWBCHANNEL_INDEX);	  //���Ͱװ�ͨ��
		int nScreenID = 0, nWindowID = 0;			 //�㲥�װ����
		pPlugin_Function->Broadcast(nScreenID,nWindowID,strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
		m_isLocalBoardCastWB = TRUE;
	}
	else
	{
		HWND hwnd = CreateWhiteBoard();
		UploadChildWnd(hwnd);
		KillTimer(TIMER_WB_BROADCAST);
		SetTimer(TIMER_WB_BROADCAST,8000,NULL);
	}
}

void CWBMainDlg::OnMMUserData(const char* strFromID, const char* strCmdType, const char* strUserData)
{
	//����ȫ����Ա�˻�ʱ���Ļص�
	CloseLoginDlg();
}

void CWBMainDlg::OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus)
{
	//���³�Ա
	UpdateMemberStatus(strMemberID, ucStatus);
}

void CWBMainDlg::OnBroadcast( int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,int nChannelType )
{
	
	switch (nCardIndex)
	{
	case  HPWBCHANNEL_INDEX:			//�װ�
		{
			 m_isReceiveBoardcast = TRUE;
			 SetIsOnline(TRUE);
			 this->SetWBStatic();
			//������ ��������ڿ��ᣬ���ǰװ���飬��ɾ�����ذװ�����ж��� ��û���˵ľ��ϴ����ذװ����������
			if (NULL == m_pWhiteBoardDlg )
			{
				HWND hwnd = CreateWhiteBoard();
			}
			UploadChildWnd(m_pWhiteBoardDlg->m_hWnd);
 			if (m_isLocalBoardCastWB)	 									    //	 m_isReceiveBoardcast��ʼ����ʱ����FALSE���ڱ��ض˷��͹㲥��ʱ����TRUE������������
 			{
// 				if (m_usMemberType >=4)
// 				{
// 					SetHPDTSNotify(NULL);
// 					WB_CloseFuntion();
// 					WB_SingleCloseFile();			   //�ȹرձ��ذװ���ļ�													    
// 					WB_AvconLock(true);
// 					LockMenu();
// 				}
				CString cStrLoginId = _T("UserTemp");
				if (m_pWBLoginDlg)
				{
					CWBLoginDlg *pLoginDlg = (CWBLoginDlg *)m_pWBLoginDlg;
					cStrLoginId = *(pLoginDlg->GetLoginID());
				}
				SetHPDTSSavePath(cStrLoginId);
				std::string strTemp = _W2A(cStrLoginId);//��ʱ�ļ����浽HPDTS��
				WB_SetWBTempDir(strTemp.c_str());
				SetHPDTSNotify((IHPNetNotify*)m_pWhiteBoardDlg);//���ðװ�ͨѶָ��
				this->Connect();
				SendAllObj();		
				m_isLocalBoardCastWB = FALSE;
 			}
			else                              //�������Ļ��ͱ������Ǳ��ع㲥��
			{
				SetHPDTSNotify(NULL);
				WB_CloseFuntion();
				WB_SingleCloseFile();			   //�ȹرձ��ذװ���ļ�
				CString cStrLoginId = _T("UserTemp");
				if (m_pWBLoginDlg)
				{
					CWBLoginDlg *pLoginDlg = (CWBLoginDlg *)m_pWBLoginDlg;
					cStrLoginId = *(pLoginDlg->GetLoginID());
				}
				SetHPDTSSavePath(cStrLoginId);
				std::string strTemp = _W2A(cStrLoginId);
				WB_SetWBTempDir(strTemp.c_str());
				SetHPDTSNotify((IHPNetNotify*)m_pWhiteBoardDlg);//���ðװ�ͨѶָ��
				this->Connect(); //��������ʵ��ͬ��
				RequestAllObj();
// 				if (m_usMemberType >=4)
// 				{
// 					WB_AvconLock(true);
// 					LockMenu();
// 				}
			}

		}
		break;
	default:
		{
			IPlugin* pIPlugin = AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member");
			IPlugin_U7_MM_Member* pU7MMMember = dynamic_cast<IPlugin_U7_MM_Member*>(pIPlugin);
			if (pU7MMMember == NULL)
				break;

			if (strMemberID == pU7MMMember->GetUserIDDomain())
			{
				//����Ǳ��ط��͹㲥 �����¼�
			}
		}
	}
}

void CWBMainDlg::OnCloseBroadcast( const char* strMemberID,int nCardIndex,int nChannelType )
{
	if(nCardIndex != HPWBCHANNEL_INDEX)
	{
		return;
	}
 	m_pWhiteBoardDlg->ReleaseConnection();
 	SetHPDTSNotify(NULL);
	IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (NULL == pPlugin_Room)
	{
		return;
	}
	pPlugin_Room->ExitRoom();
	m_isReceiveBoardcast = FALSE;
	if (m_isNeedRealseTouchScreenAuthorize)//�ͷŴ�����Ȩ
	{
		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if (pPlugin_Function == NULL)
		{
			return ;
		}
		pPlugin_Function->ReleaseTouchScreenAuthorize();
		m_isNeedRealseTouchScreenAuthorize = FALSE;
	}
	bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
	if (!bHardwareTouch) //Ӳ����֧�ִ���
	{
		WB_SetTouchAuthorization(FALSE);
	} 
	else
	{
		WB_SetTouchAuthorization(TRUE);
	}

	IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
	if (pPlugin_Login == NULL)
	{
		return;
	}
	pPlugin_Login->Logout();
 	SetIsOnline(FALSE);
	m_pWBLoginDlg->DeleteMeetingListDlg();

	delete m_pWBLoginDlg;
	m_pWBLoginDlg = NULL;

	CString strItem;
	strItem =(CString)"AVCONWB���Ӱװ�";
	SetWindowText(strItem);//���ðװ�Ի���ı�������
 	SetWBStatic();
}

void CWBMainDlg::OnTouchScreenAuthorize(bool bAuthorize)
{
	m_isNeedRealseTouchScreenAuthorize = WB_SetTouchAuthorization(bAuthorize);
}

void CWBMainDlg::OnTimer(UINT_PTR nIDEvent)
{																				  
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (TIMER_WB_BROADCAST == nIDEvent)
	{
		KillTimer(TIMER_WB_BROADCAST);
		if (m_isReceiveBoardcast)
		{
			m_isReceiveBoardcast = FALSE;
			 return;
		}
 		//������������Ƶͨ��
 		IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
 		if (NULL == pMember)
 			return;
 		std::string strMemberID =pMember->GetUserIDDomain();
 
 		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
 		pPlugin_Function ->PublishLocalCard(HPWBCHANNEL_INDEX);	  //���Ͱװ�ͨ��
 		int nScreenID = 0, nWindowID = 0;			 //�㲥�װ����
 		pPlugin_Function->Broadcast(nScreenID,nWindowID,strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
 		m_isLocalBoardCastWB = TRUE;
	}
	__super::OnTimer(nIDEvent);
}

void CWBMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if( (nID & 0xFFF0) == SC_CLOSE )
	{
		if (NULL != m_pWBLoginDlg)
		{
			IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
			if (pPlugin_Function == NULL)
			{
				return ;
			}
			IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
			if (NULL == pPlugin_Member)
			{
				return ;
			}
			if (m_isOnline)
			{
// 				std::string strMemberID = pPlugin_Member->GetUserIDDomain();
// 				BOOL bPower =   CWBMainDlg::g_pMainDlg->LimitsOfPower();
// 				//-------------------------------------------------------------------------
// 				//�˳�����ʱ��������˳�����Ļص����� OnExitRoom
// 				//���˳������ʱ���ж��ǻ����ǲ������ˣ����û�еĻ��ͷ��͹رհװ�Ĺ㲥
// 				if (m_onLineCurMeetingNumber <= 1)
// 				{
// 					if (bPower)
// 					{
// 						//������������Ƶͨ��
// 						//������ط��жϻ�������
// 						//		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function *>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
// 						//if ( GetCurrentMeetingInfo()->roommode  == MEETING_MODE_NORMAL)
// 						pPlugin_Function->CloseBroadcast(strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
// 					}
// 				}
				IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
				if (NULL == pPlugin_Room)
				{
					return ;
				}
				pPlugin_Room->ExitRoom();
				m_isReceiveBoardcast = FALSE;
				if (m_isNeedRealseTouchScreenAuthorize)//�ͷŴ�����Ȩ
				{
					pPlugin_Function->ReleaseTouchScreenAuthorize();
					m_isNeedRealseTouchScreenAuthorize = FALSE;
				}
				IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
				if (pPlugin_Login == NULL)
				{
					return ;
				}
				m_onLineCurMeetingNumber--;
				m_pWBLoginDlg->DeleteMeetingListDlg();
				pPlugin_Login->Logout();
				SetIsOnline(FALSE);
			}
			pPlugin_Function->DelObserver(this);
			pPlugin_Member->DelObserver(this);

			delete m_pWBLoginDlg;
			m_pWBLoginDlg = NULL;
		}
		DestroyWhiteBoard();
		PostQuitMessage(0);
	}
	else if( (nID & 0xFFF0) == SC_MOVE )
	{
		return;
	}
	else if( (nID & 0xFFF0) == SC_MINIMIZE ) //��С��
	{
		WB_SetIsMinSize(TRUE);

		CDialog::OnSysCommand(nID, lParam);
		return;
	}
	else if( (nID & 0xFFF0) == SC_RESTORE ) //����С���ָ�����
	{
		CDialog::OnSysCommand(nID, lParam);

		WB_SetIsMinSize(FALSE);
		WB_OpenSCDraw();
		return;
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CWBMainDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	return;
}

void CWBMainDlg::SetisLocalBoardCastWB( BOOL dispute )
{
	  m_isLocalBoardCastWB = dispute;
}

void CWBMainDlg::SetMMS_Member_Type(unsigned short usMemberType)
{
	 m_usMemberType = usMemberType;
}

void CWBMainDlg::LockMenu()
{
	CMenu * tempSubMenw;
	//�ļ��Ӳ˵�	
	tempSubMenw = GetMenu()->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_NEWFILE_BTN,MF_GRAYED);	 //�½��ļ�
	tempSubMenw->EnableMenuItem(IDC_OPENFILE_BTN,MF_GRAYED);	 //���ļ�
	tempSubMenw->EnableMenuItem(IDC_SAVEFILE_BTN,MF_GRAYED);	//�����ļ�
	tempSubMenw->EnableMenuItem(IDC_RESAVEFILE_BTN,MF_GRAYED);	 //����ļ�
	tempSubMenw->EnableMenuItem(IDC_PRINTFILE_BTN,MF_GRAYED);	 //��ӡ�ļ�

	//ҳ�Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(1);  
	tempSubMenw->EnableMenuItem(IDC_NEWPAGE_BTN, MF_GRAYED);  //�½�ҳ
	tempSubMenw->EnableMenuItem(IDC_PREPAGE_BTN, MF_GRAYED);  //��һҳ
	tempSubMenw->EnableMenuItem(IDC_NEXTPAGE_BTN, MF_GRAYED);//��һҳ
	tempSubMenw->EnableMenuItem(IDC_FIRSTPAGE_BTN, MF_GRAYED);   //��ҳ
	tempSubMenw->EnableMenuItem(IDC_LASTPAGE_EDIT, MF_GRAYED);   //ĩҳ
	//�༭�Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(2);
	tempSubMenw->EnableMenuItem(IDC_SELECTEDIT_BTN, MF_GRAYED);   //ѡ��
	tempSubMenw->EnableMenuItem(IDC_UNDOEDIT_BTN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_REDOEDIT_BTN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_CLONEEDIT_BTN, MF_GRAYED);   //��¡
	tempSubMenw->EnableMenuItem(IDC_INFINITECLONEEDIT_BTN, MF_GRAYED);   //���޿�¡
	tempSubMenw->EnableMenuItem(IDC_CUTEDIT_BTN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_COPYEDIT_BTN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_PASTEEDIT_BTN, MF_GRAYED);   //ճ��
	tempSubMenw->EnableMenuItem(IDC_DELETEEDIT_BTN, MF_GRAYED);   //ɾ��
	tempSubMenw->EnableMenuItem(IDC_DELETEALLEDIT_BTN, MF_GRAYED);   //ȫ��ɾ��
	//�༭�˵���ͼ���Ӳ˵�
	tempSubMenw->EnableMenuItem(IDC_PRELAYEREDIT_BTN, MF_GRAYED);   //��һ��
	tempSubMenw->EnableMenuItem(IDC_NEXTLAYEREDIT_BTN, MF_GRAYED);   //��һ��
	tempSubMenw->EnableMenuItem(IDC_TOPLAYER_BTN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_BOTTOMLAYEREDIT_BTN, MF_GRAYED);   //ĩ��
	//�༭�˵�����ת�Ӳ˵�
	tempSubMenw->EnableMenuItem(IDC_DEASIL45EDIT_BTN, MF_GRAYED);   //˳ʱ����ת45��
	tempSubMenw->EnableMenuItem(IDC_DEASIL90EDIT_BTN, MF_GRAYED);   //˳ʱ����ת90��
	tempSubMenw->EnableMenuItem(IDC_WIDDERSHINS45EDIT_BTN, MF_GRAYED);   //��ʱ����ת45��
	tempSubMenw->EnableMenuItem(IDC_WIDDERSHINS90EDIT_BTN, MF_GRAYED);   //��ʱ����ת90��
	tempSubMenw->EnableMenuItem(IDC_ROTATION180EDIT_BTN, MF_GRAYED);   //��ת180��
	tempSubMenw->EnableMenuItem(IDC_ROTATION_HIRIZONTALEDIT_BTN, MF_GRAYED);   //ˮƽ����
	tempSubMenw->EnableMenuItem(IDC_ROTATION_VERTICALEDIT_BTN, MF_GRAYED);   //��ֱ����

	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(3);
	tempSubMenw->EnableMenuItem(IDC_PICTUREINSERT_BTN, MF_GRAYED);   //ͼƬ
	tempSubMenw->EnableMenuItem(IDC_MEDIOINSERT_BTN, MF_GRAYED);   //��Ƶ
	tempSubMenw->EnableMenuItem(IDC_FLASHINSERT_BTN, MF_GRAYED);   //FLASH
	tempSubMenw->EnableMenuItem(IDC_DOCUMENTINSERT_BTN, MF_GRAYED);   //�ĵ�
	tempSubMenw->EnableMenuItem(IDC_CAMERAINSERT_BTN, MF_GRAYED);   //����ͷ
	tempSubMenw->EnableMenuItem(IDC_TEXTINSERT_BTN, MF_GRAYED);   //�ı�
	tempSubMenw->EnableMenuItem(IDC_ANNOTATIONSINSERT_BEN, MF_GRAYED);   //��ע
	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(4);
	tempSubMenw->EnableMenuItem(IDC_LINEDRAW_BEN, MF_GRAYED);   //ֱ��
	tempSubMenw->EnableMenuItem(IDC_CURVEDRAW_BEN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_RECTDRAW_BEN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_TRIANGLEDRAW_BEN, MF_GRAYED);   //������
	tempSubMenw->EnableMenuItem(IDC_ELLIPSEDRAW_BEN, MF_GRAYED);   //Բ
	tempSubMenw->EnableMenuItem(IDC_RHOMBUSDRAW_BEN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_BIGERRUBBERDRAW_BTN, MF_GRAYED);   //���
/*	tempSubMenw->EnableMenuItem(IDC_SMALLRUBBERDRAW_BEN, MF_GRAYED);   //С��*/
	tempSubMenw->EnableMenuItem(IDC_CURVERUBBERDRAW_BEN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_OBJRUBBERDRAW_BTN, MF_GRAYED);   //������


	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(5);
	tempSubMenw->EnableMenuItem(IDC_BAFFLETOOL_BEN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_SPOTLIGHTTOOL_BTN, MF_GRAYED);   //�۹��		
	tempSubMenw->EnableMenuItem(IDC_MAQNTOOL_BTN, MF_GRAYED);   //�Ŵ�
	tempSubMenw->EnableMenuItem(IDC_CUTSCREENTOOL_BTN, MF_GRAYED);   //����
	tempSubMenw->EnableMenuItem(IDC_RECYCLE_BTN, MF_GRAYED);   //����վ
	tempSubMenw->EnableMenuItem(IDC_SCREENRECTOOL_BTN, MF_GRAYED);   //��Ļ¼��
	tempSubMenw->EnableMenuItem(IDC_OPENSCREENDRAWTOOL_BTN, MF_GRAYED);   //�򿪱�ע
	tempSubMenw->EnableMenuItem(IDC_EFFECTSCREENDRAW_BTN, MF_GRAYED);   //Ч���л�
	tempSubMenw->EnableMenuItem(IDC_CLOSESCREENDRAWTOOL_BTN, MF_GRAYED);   //�رձ�ע
	tempSubMenw->EnableMenuItem(IDC_RESOURCETOOL_BTN, MF_GRAYED);   //��ԴĿ¼
	//tempSubMenw->EnableMenuItem(IDC_RESOURCEMANAGETOOL_BTN, MF_GRAYED);   //��ԴĿ¼����
	tempSubMenw->EnableMenuItem(IDC_FUNCTIONTOOL_BTN, MF_GRAYED);   //�������


}

void CWBMainDlg::UnLockMenu()
{
	CMenu * tempSubMenw;
	//�ļ��Ӳ˵�	
	tempSubMenw = GetMenu()->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_NEWFILE_BTN,MF_ENABLED);	 //�½��ļ�
	tempSubMenw->EnableMenuItem(IDC_OPENFILE_BTN,MF_ENABLED);	 //���ļ�
	tempSubMenw->EnableMenuItem(IDC_SAVEFILE_BTN,MF_ENABLED);	//�����ļ�
	tempSubMenw->EnableMenuItem(IDC_RESAVEFILE_BTN,MF_ENABLED);	 //����ļ�
	tempSubMenw->EnableMenuItem(IDC_PRINTFILE_BTN,MF_ENABLED);	 //��ӡ�ļ�

	//ҳ�Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(1);  
	tempSubMenw->EnableMenuItem(IDC_NEWPAGE_BTN, MF_ENABLED);  //�½�ҳ
	tempSubMenw->EnableMenuItem(IDC_PREPAGE_BTN, MF_ENABLED);  //��һҳ
	tempSubMenw->EnableMenuItem(IDC_NEXTPAGE_BTN, MF_ENABLED);//��һҳ
	tempSubMenw->EnableMenuItem(IDC_FIRSTPAGE_BTN, MF_ENABLED);   //��ҳ
	tempSubMenw->EnableMenuItem(IDC_LASTPAGE_EDIT, MF_ENABLED);   //ĩҳ
	//�༭�Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(2);
	tempSubMenw->EnableMenuItem(IDC_SELECTEDIT_BTN, MF_ENABLED);   //ѡ��
	tempSubMenw->EnableMenuItem(IDC_UNDOEDIT_BTN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_REDOEDIT_BTN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_CLONEEDIT_BTN, MF_ENABLED);   //��¡
	tempSubMenw->EnableMenuItem(IDC_INFINITECLONEEDIT_BTN, MF_ENABLED);   //���޿�¡
	tempSubMenw->EnableMenuItem(IDC_CUTEDIT_BTN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_COPYEDIT_BTN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_PASTEEDIT_BTN, MF_ENABLED);   //ճ��
	tempSubMenw->EnableMenuItem(IDC_DELETEEDIT_BTN, MF_ENABLED);   //ɾ��
	tempSubMenw->EnableMenuItem(IDC_DELETEALLEDIT_BTN, MF_ENABLED);   //ȫ��ɾ��
	//�༭�˵���ͼ���Ӳ˵�
	tempSubMenw->EnableMenuItem(IDC_PRELAYEREDIT_BTN, MF_ENABLED);   //��һ��
	tempSubMenw->EnableMenuItem(IDC_NEXTLAYEREDIT_BTN, MF_ENABLED);   //��һ��
	tempSubMenw->EnableMenuItem(IDC_TOPLAYER_BTN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_BOTTOMLAYEREDIT_BTN, MF_ENABLED);   //ĩ��
	//�༭�˵�����ת�Ӳ˵�
	tempSubMenw->EnableMenuItem(IDC_DEASIL45EDIT_BTN, MF_ENABLED);   //˳ʱ����ת45��
	tempSubMenw->EnableMenuItem(IDC_DEASIL90EDIT_BTN, MF_ENABLED);   //˳ʱ����ת90��
	tempSubMenw->EnableMenuItem(IDC_WIDDERSHINS45EDIT_BTN, MF_ENABLED);   //��ʱ����ת45��
	tempSubMenw->EnableMenuItem(IDC_WIDDERSHINS90EDIT_BTN, MF_ENABLED);   //��ʱ����ת90��
	tempSubMenw->EnableMenuItem(IDC_ROTATION180EDIT_BTN, MF_ENABLED);   //��ת180��
	tempSubMenw->EnableMenuItem(IDC_ROTATION_HIRIZONTALEDIT_BTN, MF_ENABLED);   //ˮƽ����
	tempSubMenw->EnableMenuItem(IDC_ROTATION_VERTICALEDIT_BTN, MF_ENABLED);   //��ֱ����

	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(3);
	tempSubMenw->EnableMenuItem(IDC_PICTUREINSERT_BTN, MF_ENABLED);   //ͼƬ
	tempSubMenw->EnableMenuItem(IDC_MEDIOINSERT_BTN, MF_ENABLED);   //��Ƶ
	tempSubMenw->EnableMenuItem(IDC_FLASHINSERT_BTN, MF_ENABLED);   //FLASH
	tempSubMenw->EnableMenuItem(IDC_DOCUMENTINSERT_BTN, MF_ENABLED);   //�ĵ�
	tempSubMenw->EnableMenuItem(IDC_CAMERAINSERT_BTN, MF_ENABLED);//����ͷ
	tempSubMenw->EnableMenuItem(IDC_TEXTINSERT_BTN, MF_ENABLED);   //�ı�
	tempSubMenw->EnableMenuItem(IDC_ANNOTATIONSINSERT_BEN, MF_ENABLED);   //��ע
	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(4);
	tempSubMenw->EnableMenuItem(IDC_LINEDRAW_BEN, MF_ENABLED);   //ֱ��
	tempSubMenw->EnableMenuItem(IDC_CURVEDRAW_BEN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_RECTDRAW_BEN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_TRIANGLEDRAW_BEN, MF_ENABLED);   //������
	tempSubMenw->EnableMenuItem(IDC_ELLIPSEDRAW_BEN, MF_ENABLED);   //Բ
	tempSubMenw->EnableMenuItem(IDC_RHOMBUSDRAW_BEN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_BIGERRUBBERDRAW_BTN, MF_ENABLED);   //���
/*	tempSubMenw->EnableMenuItem(IDC_SMALLRUBBERDRAW_BEN, MF_ENABLED);   //С��*/
	tempSubMenw->EnableMenuItem(IDC_CURVERUBBERDRAW_BEN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_OBJRUBBERDRAW_BTN, MF_ENABLED);   //������


	//�����Ӳ˵�
	tempSubMenw = GetMenu()->GetSubMenu(5);
	tempSubMenw->EnableMenuItem(IDC_BAFFLETOOL_BEN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_SPOTLIGHTTOOL_BTN, MF_ENABLED);   //�۹��		
	tempSubMenw->EnableMenuItem(IDC_MAQNTOOL_BTN, MF_ENABLED);   //�Ŵ�
	tempSubMenw->EnableMenuItem(IDC_CUTSCREENTOOL_BTN, MF_ENABLED);   //����
	tempSubMenw->EnableMenuItem(IDC_RECYCLE_BTN, MF_ENABLED);   //����վ
	tempSubMenw->EnableMenuItem(IDC_SCREENRECTOOL_BTN, MF_ENABLED);   //��Ļ¼��
	tempSubMenw->EnableMenuItem(IDC_OPENSCREENDRAWTOOL_BTN, MF_ENABLED);   //�򿪱�ע
	tempSubMenw->EnableMenuItem(IDC_EFFECTSCREENDRAW_BTN, MF_ENABLED);   //Ч���л�
	tempSubMenw->EnableMenuItem(IDC_CLOSESCREENDRAWTOOL_BTN, MF_ENABLED);   //�رձ�ע
	tempSubMenw->EnableMenuItem(IDC_RESOURCETOOL_BTN, MF_ENABLED);   //��ԴĿ¼
	//tempSubMenw->EnableMenuItem(IDC_RESOURCEMANAGETOOL_BTN, MF_ENABLED);   //��ԴĿ¼����
	tempSubMenw->EnableMenuItem(IDC_FUNCTIONTOOL_BTN, MF_ENABLED);   //�������
}

BOOL CWBMainDlg::LimitsOfPower()
{
	  if (m_usMemberType > 4)
	  {
		  return FALSE;
	  }
	  return TRUE;
}

void CWBMainDlg::SetIsNeedRealseTouchScreenAuthorize(bool bIsNeedRealseTouchScreenAuthorize)
{
	m_isNeedRealseTouchScreenAuthorize = bIsNeedRealseTouchScreenAuthorize;
}

bool CWBMainDlg::GetIsNeedRealseTouchScreenAuthorize()
{
	return m_isNeedRealseTouchScreenAuthorize;
}

CWBLoginDlg *CWBMainDlg::GetLoginDlg()
{
	return m_pWBLoginDlg;
}

void CWBMainDlg::SetIsReceiveBoardcast(bool bIsReceiveBoardcast)
{
	m_isReceiveBoardcast = bIsReceiveBoardcast;
}

void CWBMainDlg::ProcessWBToLocalData( char* pNotifyData )
{
	if (NULL != pNotifyData)
	{
		int iCmd = *(int*)pNotifyData;
		switch (iCmd)
		{
		case CMD_SINGLE_QUIT:
			{
				BOOL bIsExit = WB_SingleExit();
				WB_CloseAllChildBox();

				if (bIsExit)
				{
					if (NULL != m_pWBLoginDlg)
					{
						IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
						if (pPlugin_Function == NULL)
						{
							return ;
						}
						IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
						if (NULL == pPlugin_Member)
						{
							return ;
						}
						if (m_isOnline)
						{
							// 						std::string strMemberID = pPlugin_Member->GetUserIDDomain();
							// 						BOOL bPower =   CWBMainDlg::g_pMainDlg->LimitsOfPower();
							// 						//-------------------------------------------------------------------------
							// 						//�˳�����ʱ��������˳�����Ļص����� OnExitRoom
							// 						//���˳������ʱ���ж��ǻ����ǲ������ˣ����û�еĻ��ͷ��͹رհװ�Ĺ㲥
							// 						if (m_onLineCurMeetingNumber <= 1)
							// 						{
							// 							if (bPower)
							// 							{
							// 								//������������Ƶͨ��
							// 								//������ط��жϻ�������
							// 								//		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function *>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
							// 								//if ( GetCurrentMeetingInfo()->roommode  == MEETING_MODE_NORMAL)
							// 								pPlugin_Function->CloseBroadcast(strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
							// 							}
							// 						}
							IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
							if (NULL == pPlugin_Room)
							{
								return ;
							}
							pPlugin_Room->ExitRoom();
							m_isReceiveBoardcast = FALSE;
							if (m_isNeedRealseTouchScreenAuthorize)//�ͷŴ�����Ȩ
							{
								pPlugin_Function->ReleaseTouchScreenAuthorize();
								m_isNeedRealseTouchScreenAuthorize = FALSE;
							}
							bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
							if (!bHardwareTouch) //Ӳ����֧�ִ���
							{
								WB_SetTouchAuthorization(FALSE);
							} 
							else
							{
								WB_SetTouchAuthorization(TRUE);
							}
							IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
							if (pPlugin_Login == NULL)
							{
								return ;
							}
							m_onLineCurMeetingNumber--;
							m_pWBLoginDlg->DeleteMeetingListDlg();
							pPlugin_Login->Logout();
							SetIsOnline(FALSE);
						}
						pPlugin_Function->DelObserver(this);
						pPlugin_Member->DelObserver(this);

						delete m_pWBLoginDlg;
						m_pWBLoginDlg = NULL;
					}
					DestroyWhiteBoard();
					PostQuitMessage(0);
			/*	if (NULL != m_pWBLoginDlg)
				{
					IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
					if (pPlugin_Function == NULL)
					{
						return ;
					}
					IPlugin_U7_MM_Member* pPlugin_Member = static_cast<IPlugin_U7_MM_Member *>(g_System->GetPluginByName("Plugin_U7_MM_Member"));
					if (NULL == pPlugin_Member)
					{
						return ;
					}
					if (m_isOnline)
					{
						IPlugin_U7_MM_Room* pPlugin_Room = static_cast<IPlugin_U7_MM_Room*>(g_System->GetPluginByName("Plugin_U7_MM_Room"));
						if (NULL == pPlugin_Room)
						{
							return ;
						}
						pPlugin_Room->ExitRoom();
						m_isReceiveBoardcast = FALSE;
						if (m_isNeedRealseTouchScreenAuthorize)//�ͷŴ�����Ȩ
						{
							pPlugin_Function->ReleaseTouchScreenAuthorize();
							m_isNeedRealseTouchScreenAuthorize = FALSE;
						}
						IPlugin_U7_IM_Login* pPlugin_Login = (IPlugin_U7_IM_Login*)g_System->GetPluginByName("Plugin_U7_IM_Login");
						if (pPlugin_Login == NULL)
						{
							return ;
						}
						m_onLineCurMeetingNumber--;
						m_pWBLoginDlg->DeleteMeetingListDlg();
						pPlugin_Login->Logout();
						SetIsOnline(FALSE);
					}
					pPlugin_Function->DelObserver(this);
					pPlugin_Member->DelObserver(this);

					delete m_pWBLoginDlg;
					m_pWBLoginDlg = NULL;
				}
				DestroyWhiteBoard();
				PostQuitMessage(0);	*/	


				}
		}

			break;
		

		case CMD_SINGLE_NETWORK:
			{
				OpenLoginDlg();
			}
			break;
		case CMD_SINGLE_LOCAL:
			{
				BOOL bl = TRUE;
				WB_SetMediaSinToIntnet(bl);
				CloseLoginDlg();
				WB_MediaSingelToNet();

				WB_IsSuccessLoad(false);
			}
			break;
		default:
			;
		}
	}
}
