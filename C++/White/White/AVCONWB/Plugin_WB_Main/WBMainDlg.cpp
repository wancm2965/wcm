// WBMainDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "Plugin_WB_Main.h"
#include "WBMainDlg.h"
#include "..\include\WB\WBDefine.h"
#include "..\includeUCC\IPlugin\IU7_MM\IPlugin_U7_MM_Member.h"
#include "..\includeUCC\IPlugin\IU7_MM\IPlugin_U7_MM_Function.h"
#include "AutoLockResource.h"


#define TIMER_WB_BROADCAST 10120   //定时器 事件 是否接受到白板广播
// CWBMainDlg 对话框

IMPLEMENT_DYNAMIC(CWBMainDlg, CDialog)

CWBMainDlg *CWBMainDlg::g_pMainDlg = NULL; //白板主窗口指针
bool CWBMainDlg::g_isCreateWb = false;				  //是否已经创建了白板
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


// CWBMainDlg 消息处理程序



BOOL CWBMainDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	UINT nItemID = LOWORD(wParam);
	switch (nItemID)
	{
		//文件菜单
	case  IDC_LOAD_BTN : //登录
		{
			OpenLoginDlg();
		}
		break;
	case  IDC_LOGOUT_BTN : //注销
		{
			BOOL bl = TRUE;
			WB_SetMediaSinToIntnet(bl);
			CloseLoginDlg();
			WB_MediaSingelToNet();			
		}
		break;
	case  IDC_NEWFILE_BTN : //新文件
		{
			WB_Newfile();
			WB_CloseAllChildBox();
		}
		break;
	case  IDC_OPENFILE_BTN ://打开文件
		{
			WB_Openfile();
			WB_CloseAllChildBox();
		}
		break;
	case  IDC_SAVEFILE_BTN : //保存文件
		{
			WB_SaveWBFile();
		}
		break;
	case  IDC_RESAVEFILE_BTN ://另存文件
		{
			WB_Resavefile();
		}
		break;
	case  IDC_PRINTFILE_BTN : //打印文件
		{
			WB_Printfile();
		}
		break;
	case  IDC_CLOSEFILE_BTN ://关闭文件
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
// 						//退出会议时，不会进退出会议的回调函数 OnExitRoom
// 						//在退出会议的时候，判断是会议是不是有人，如果没有的话就发送关闭白板的广播
// 						if (m_onLineCurMeetingNumber <= 1)
// 						{
// 							if (bPower)
// 							{
// 								//发布本地音视频通道
// 								//在这个地方判断会议类型
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
						if (m_isNeedRealseTouchScreenAuthorize)//释放触摸授权
						{
							pPlugin_Function->ReleaseTouchScreenAuthorize();
							m_isNeedRealseTouchScreenAuthorize = FALSE;
						}
						bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
						if (!bHardwareTouch) //硬件不支持触摸
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
		//编辑菜单
	case  IDC_SELECTEDIT_BTN : //选择
		{
			WB_SetDrawStatus( DRAW_NORMAL);
		}
		break;
	case  IDC_UNDOEDIT_BTN ://撤销
		{
			WB_Undo(1);	
		}
		break;
	case  IDC_REDOEDIT_BTN : //重做
		{
			WB_Redo();
		}
		break;
	case  IDC_CLONEEDIT_BTN ://克隆
		{
			WB_CloneObject();
		}
		break;
	case  IDC_INFINITECLONEEDIT_BTN : //无限克隆
		{
			WB_InfiniteCloneObj();
		}
		break;
	case  IDC_CUTEDIT_BTN ://剪切
		{
			WB_CutObject();
		}
		break;
	case  IDC_COPYEDIT_BTN : //复制
		{
			WB_CopyObject();
		}
		break;
	case  IDC_PASTEEDIT_BTN ://粘贴
		{
			WB_PasteObject();
		}
		break;
	case  IDC_DELETEEDIT_BTN : //删除
		{
			WB_DeleteSelectedObjects();
		}
		break;
	case  IDC_DELETEALLEDIT_BTN ://全部删除
		{
			WB_DeleteAll();
		}
		break;
	case  IDC_PRELAYEREDIT_BTN : //上一图层
		{
			WB_SuperLayer();
		}
		break;
	case  IDC_NEXTLAYEREDIT_BTN ://下一图层
		{
			WB_UnderLayer();
		}
		break;
	case  IDC_TOPLAYER_BTN : //第一层
		{
			WB_TopLayer();
		}
		break;
	case  IDC_BOTTOMLAYEREDIT_BTN ://底层
		{
			WB_BottomLayer();
		}
		break;
	case  IDC_DEASIL45EDIT_BTN : //顺时针旋转45度
		{
			WB_RotateByAngle(-45);
		}
		break;
	case  IDC_DEASIL90EDIT_BTN ://顺时针旋转90都
		{
			WB_RotateByAngle(-90);
		}
		break;
	case  IDC_WIDDERSHINS45EDIT_BTN : //逆时针旋转45度
		{
			WB_RotateByAngle(45);
		}
		break;
	case  IDC_WIDDERSHINS90EDIT_BTN ://逆时针旋转90度
		{
			WB_RotateByAngle(90);
		}
		break;
	case  IDC_ROTATION180EDIT_BTN : //旋转180度
		{
			WB_RotateByAngle(180);
		}
		break;
	case  IDC_ROTATION_HIRIZONTALEDIT_BTN ://逆时针旋转90度
		{
			WB_RotateByHV(0);
		}
		break;
	case  IDC_ROTATION_VERTICALEDIT_BTN : //旋转180度
		{
			WB_RotateByHV(-90);
		}
		break;
		//页菜单
	case  IDC_NEWPAGE_BTN ://新建页
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_AddNewPage();
		}
		break;
	case  IDC_PREPAGE_BTN : //上一页
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_PrePage();
		}
		break;
	case  IDC_NEXTPAGE_BTN ://下一页
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_NextPage();
		}
		break;
	case  IDC_FIRSTPAGE_BTN : //第一页
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_FirstPage();
		}
		break;
	case  IDC_LASTPAGE_EDIT ://最后一页
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_LastPage();
		}
		break;
	case  IDC_INSERTPAGE_BTN : //插入页
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_InsertNewPage();
		}
		break;
	case  IDC_DELETEPAGE_BTN ://删除页
		{
			WB_SetDrawStatus( DRAW_NORMAL);
			WB_DeletePage();
		}
		break;
		//插入菜单
	case  IDC_PICTUREINSERT_BTN ://图片
		{
			WB_AddPicture();
			SetFocus();
		}
		break;
	case  IDC_MEDIOINSERT_BTN : //视频
		{
			WB_AddMedia();
		}
		break;
	case  IDC_FLASHINSERT_BTN ://Flash
		{
			WB_AddFlash();
		}
		break;
	case  IDC_DOCUMENTINSERT_BTN : //文档
		{
			WB_AddDoc();
		}
		break;
	case  IDC_CAMERAINSERT_BTN ://摄像头
		{
			WB_AddCamera();
		}
		break;
	case  IDC_TEXTINSERT_BTN : //文本
		{
			WB_SetDrawStatus( DRAW_TEXT);
		}
		break;
	case  IDC_FONTINSERT_BTN ://字体
		{
			WB_SetCharfont();
		}
		break;
	case  IDC_ANNOTATIONSINSERT_BEN ://备注
		{
			WB_SetDrawStatus(ADD_ANNOTATIONS);
		}
		break;
		//绘制菜单
	case  IDC_LINEDRAW_BEN : //直线
		{
			WB_SetDrawStatus( DRAW_LINE);
		}
		break;
	case  IDC_CURVEDRAW_BEN ://曲线
		{
			WB_SetDrawStatus( DRAW_POLYLINE);
		}
		break;
	case  IDC_RECTDRAW_BEN : //矩形
		{
			WB_SetDrawStatus( DRAW_RECT);
		}
		break;
	case  IDC_TRIANGLEDRAW_BEN ://三角形
		{
			WB_SetDrawStatus( DRAW_TRIANGLE);
		}
		break;
	case  IDC_ELLIPSEDRAW_BEN ://圆
		{
			WB_SetDrawStatus( DRAW_ELLIPSE);
		}
		break;
	case  IDC_RHOMBUSDRAW_BEN : //菱形
		{
			WB_SetDrawStatus( DRAW_RHOMBUS);
		}
		break;
	case  IDC_POLYGONDRAW_BEN ://多边形
		{
			WB_SetDrawStatus( DRAW_POLYGON);
		}
		break;
	case  IDC_BIGERRUBBERDRAW_BTN ://大擦
		{
			WB_SetDrawStatus( DRAW_ERASE);
			WB_SetBrushSize(10);
		}
		break;
//  	case  IDC_SMALLRUBBERDRAW_BEN : //小擦
//  		{
//  			WB_SetDrawStatus( DRAW_ERASE);
//  			WB_SetBrushSize(BRUSH_SMALL);
//  		}
//  		break;
	case  IDC_CURVERUBBERDRAW_BEN ://曲擦
		{
			WB_SetDrawStatus( DRAW_RGNDELETE);
		}
		break;
	case  IDC_OBJRUBBERDRAW_BTN ://擦对象
		{
			WB_SetDrawStatus( DRAW_CLEANUP);
		}
		break;
		//工具菜单
	case  IDC_BAFFLETOOL_BEN : //挡板
		{
			WB_Baffle();
		}
		break;
	case  IDC_SPOTLIGHTTOOL_BTN ://聚光灯
		{
			WB_NewSpotlight();
		}
		break;
	case  IDC_MAQNTOOL_BTN ://放大镜
		{
			WB_Maqnifier();
		}
		break;
	case  IDC_CUTSCREENTOOL_BTN : //截屏
		{
			WB_Cutscreen();
		}
		break;
	case  IDC_RECYCLE_BTN : //回收站
		{
			WB_Dustbin();
		}
		break;
	case  IDC_SCREENRECTOOL_BTN ://屏幕录制
		{
			WB_RECScreen();
			
		}
		break;
	case  IDC_RESOURCETOOL_BTN ://资源目录
		{
			WB_OpenResource();
		}
		break;
// 	case  IDC_RESOURCEMANAGETOOL_BTN : //资源库管理
// 		{
// 			//WB_OpenReManger();
// 		}
// 		break;
	case IDC_FUNCTIONTOOL_BTN: //功能面板
		{
			WB_FunctionPanel();
		}
		break;
	case  IDC_OPENSCREENDRAWTOOL_BTN ://打开标注
		{
			WB_DrawScreen(NULL);
		}
		break;
	case  IDC_CLOSESCREENDRAWTOOL_BTN ://关闭标注
		{
			//WB_DestroyWnd();
		}
		break;
	case  IDC_EFFECTSCREENDRAW_BTN : //效果切换标注
		{
			bool Trans = WB_IsScreenDrawTrans();
			WB_GuiseShift(!Trans );
		}
		break;
		//帮助菜单
	case  IDC_ABOUTHELP_BTN ://关于
		{
			//
			WB_AboutAvconWB();
		}
		break;
	case  IDC_LOOKHELP_BTN ://白板说明
		{

		}

	}
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CWBMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	SetWindowText(_T("AVCONWB"));	   //设置对话框名字
	SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);		 //设置窗口最大化

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_memberAvatarLst.Create(16, 16, ILC_COLOR32, 2, 2);//创建图像列表
	CBitmap bm;
	bm.LoadBitmap(IDB_WBMEETINGMEMBER_ONLINE);//载入Bitmap资源
	m_memberAvatarLst.Add(&bm, RGB(0, 0, 0));//添加到图像列表
	bm.DeleteObject();
	bm.LoadBitmap(IDB_WBMEETINGMEMBER_OFFLINE);
	m_memberAvatarLst.Add(&bm, RGB(0, 0, 0));
	bm.DeleteObject();
	bm.LoadBitmap(IDB_WBMEETINGROOMNAME);
	m_memberAvatarLst.Add(&bm, RGB(0, 0, 0));
	bm.DeleteObject();
	m_memberListTree.SetImageList(&m_memberAvatarLst,TVSIL_NORMAL);

	CMenu * tempSubMenw;
	//文件子菜单	
	tempSubMenw = GetMenu()->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_LOGOUT_BTN,MF_GRAYED);

	LoginBmp.LoadBitmap(IDB_WBMENU_LOGINFILE);//登录
	tempSubMenw->SetMenuItemBitmaps(IDC_LOAD_BTN, MF_BYCOMMAND, &LoginBmp,NULL);
	LogoutBmp.LoadBitmap(IDB_WBMENU_LOGOUTFILE);//注销
	tempSubMenw->SetMenuItemBitmaps(IDC_LOGOUT_BTN, MF_BYCOMMAND, &LogoutBmp,NULL);
	NewFileBmp.LoadBitmap(IDB_WBMENU_NEWFILE);//新建文件
	tempSubMenw->SetMenuItemBitmaps(IDC_NEWFILE_BTN, MF_BYCOMMAND, &NewFileBmp,NULL);
	OpenFileBmp.LoadBitmap(IDB_WBMENU_OPENFILE);//打开文件
	tempSubMenw->SetMenuItemBitmaps(IDC_OPENFILE_BTN, MF_BYCOMMAND, &OpenFileBmp,NULL);
	SaveFileBmp.LoadBitmap(IDB_WBMENU_SAVEFILE);//保存文件
	tempSubMenw->SetMenuItemBitmaps(IDC_SAVEFILE_BTN, MF_BYCOMMAND, &SaveFileBmp,NULL);
	ResaveFileBmp.LoadBitmap(IDB_WBMENU_RESAVEFILE);//另存文件
	tempSubMenw->SetMenuItemBitmaps(IDC_RESAVEFILE_BTN, MF_BYCOMMAND, &ResaveFileBmp,NULL);
	PrintFileBmp.LoadBitmap(IDB_WBMENU_PRINTFILE);//打印文件
	tempSubMenw->SetMenuItemBitmaps(IDC_PRINTFILE_BTN, MF_BYCOMMAND, &PrintFileBmp,NULL);
	CloseFileBmp.LoadBitmap(IDB_WBMENU_CLOSEFILE);//退出文件
	tempSubMenw->SetMenuItemBitmaps(IDC_CLOSEFILE_BTN, MF_BYCOMMAND, &CloseFileBmp,NULL);

	//页子菜单
	tempSubMenw = GetMenu()->GetSubMenu(1);
	NewPageBmp.LoadBitmap(IDB_WBMENU_NEWPAGE);//新建页
	tempSubMenw->SetMenuItemBitmaps(IDC_NEWPAGE_BTN, MF_BYCOMMAND, &NewPageBmp,NULL);
	PrePageBmp.LoadBitmap(IDB_WBMENU_PREPAGE);//上一页
	tempSubMenw->SetMenuItemBitmaps(IDC_PREPAGE_BTN, MF_BYCOMMAND, &PrePageBmp,NULL);
	NextPageBmp.LoadBitmap(IDB_WBMENU_NEXTPAGE);//下一页
	tempSubMenw->SetMenuItemBitmaps(IDC_NEXTPAGE_BTN, MF_BYCOMMAND, &NextPageBmp,NULL);
	FirstPageBmp.LoadBitmap(IDB_WBMENU_FIRSTPAGE);//首页
	tempSubMenw->SetMenuItemBitmaps(IDC_FIRSTPAGE_BTN, MF_BYCOMMAND, &FirstPageBmp,NULL);
	LastPageBmp.LoadBitmap(IDB_WBMENU_LASTPAGE);//末页
	tempSubMenw->SetMenuItemBitmaps(IDC_LASTPAGE_EDIT, MF_BYCOMMAND, &LastPageBmp,NULL);
	INSERTPageBmp.LoadBitmap(IDB_WBMENU_INSERTPAGE);//插入页
	tempSubMenw->SetMenuItemBitmaps(IDC_INSERTPAGE_BTN, MF_BYCOMMAND, &INSERTPageBmp,NULL);
	DELETEPageBmp.LoadBitmap(IDB_WBMENU_DELETEPAGE);//删除页
	tempSubMenw->SetMenuItemBitmaps(IDC_DELETEPAGE_BTN, MF_BYCOMMAND, &DELETEPageBmp,NULL);

	//编辑子菜单
	tempSubMenw = GetMenu()->GetSubMenu(2);
	SelectBmp.LoadBitmap(IDB_WBMENU_SELECTEDIT);//选择
	tempSubMenw->SetMenuItemBitmaps(IDC_SELECTEDIT_BTN, MF_BYCOMMAND, &SelectBmp,NULL);
	UndoBmp.LoadBitmap(IDB_WBMENU_UNDOEDIT);//撤销
	tempSubMenw->SetMenuItemBitmaps(IDC_UNDOEDIT_BTN, MF_BYCOMMAND, &UndoBmp,NULL);
	RedoBmp.LoadBitmap(IDB_WBMENU_REDOEDIT);//重做
	tempSubMenw->SetMenuItemBitmaps(IDC_REDOEDIT_BTN, MF_BYCOMMAND, &RedoBmp,NULL);
	CloneBmp.LoadBitmap(IDB_WBMENU_CLONEEDIT);//克隆
	tempSubMenw->SetMenuItemBitmaps(IDC_CLONEEDIT_BTN, MF_BYCOMMAND, &CloneBmp,NULL);
	InfinitCloneBmp.LoadBitmap(IDB_WBMENU_INTINITEEDIT);//无限克隆
	tempSubMenw->SetMenuItemBitmaps(IDC_INFINITECLONEEDIT_BTN, MF_BYCOMMAND, &InfinitCloneBmp,NULL);
	CutBmp.LoadBitmap(IDB_WBMENU_CUTEDIT);//剪切
	tempSubMenw->SetMenuItemBitmaps(IDC_CUTEDIT_BTN, MF_BYCOMMAND, &CutBmp,NULL);
	CopyBmp.LoadBitmap(IDB_WBMENU_COPYEDIT);//复制
	tempSubMenw->SetMenuItemBitmaps(IDC_COPYEDIT_BTN, MF_BYCOMMAND, &CopyBmp,NULL);
	PasteBmp.LoadBitmap(IDB_WBMENU_PASTEEDIT);//粘贴
	tempSubMenw->SetMenuItemBitmaps(IDC_PASTEEDIT_BTN, MF_BYCOMMAND, &PasteBmp,NULL);
	DeleteBmp.LoadBitmap(IDB_WBMENU_DELETEEDIT); //删除
	tempSubMenw->SetMenuItemBitmaps(IDC_DELETEEDIT_BTN, MF_BYCOMMAND, &DeleteBmp,NULL);
	DeleteAllBmp.LoadBitmap(IDB_WBMENU_DELETEALLEDIT); //全部删除
	tempSubMenw->SetMenuItemBitmaps(IDC_DELETEALLEDIT_BTN, MF_BYCOMMAND, &DeleteAllBmp,NULL);
	//编辑菜单中图层子菜单
	PreLayerBmp.LoadBitmap(IDB_WBMENU_PRELAYEREDIT); //上一层
	tempSubMenw->SetMenuItemBitmaps(IDC_PRELAYEREDIT_BTN, MF_BYCOMMAND, &PreLayerBmp,NULL);
	NextLayerBmp.LoadBitmap(IDB_WBMENU_NEXTLAYEREDIT); //下一层
	tempSubMenw->SetMenuItemBitmaps(IDC_NEXTLAYEREDIT_BTN, MF_BYCOMMAND, &NextLayerBmp,NULL);
	TopLayerBmp.LoadBitmap(IDB_WBMENU_TOPLAYEREDIT); //顶层
	tempSubMenw->SetMenuItemBitmaps(IDC_TOPLAYER_BTN, MF_BYCOMMAND, &TopLayerBmp,NULL);
	BottomLayerBmp.LoadBitmap(IDB_WBMENU_BOTTOMLAYEREDIT); //末层
	tempSubMenw->SetMenuItemBitmaps(IDC_BOTTOMLAYEREDIT_BTN, MF_BYCOMMAND, &BottomLayerBmp,NULL);

	//编辑菜单中旋转子菜单
	Deasil45Bmp.LoadBitmap(IDB_WBMENU_DEASIL45EDIT); //顺时针旋转45度
	tempSubMenw->SetMenuItemBitmaps(IDC_DEASIL45EDIT_BTN, MF_BYCOMMAND, &Deasil45Bmp,NULL);
	Deasil90Bmp.LoadBitmap(IDB_WBMENU_DEASIL90EDIT); //顺时针旋转90度
	tempSubMenw->SetMenuItemBitmaps(IDC_DEASIL90EDIT_BTN, MF_BYCOMMAND, &Deasil90Bmp,NULL);
	Widderstin45Bmp.LoadBitmap(IDB_WBMENU_WIDDERSHINS45EDIT); //逆时针旋转45度
	tempSubMenw->SetMenuItemBitmaps(IDC_WIDDERSHINS45EDIT_BTN, MF_BYCOMMAND, &Widderstin45Bmp,NULL);
	Widderstin90Bmp.LoadBitmap(IDB_WBMENU_WIDDERSHINS90EDIT); //逆时针旋转90度
	tempSubMenw->SetMenuItemBitmaps(IDC_WIDDERSHINS90EDIT_BTN, MF_BYCOMMAND, &Widderstin90Bmp,NULL);
	Rotation180Bmp.LoadBitmap(IDB_WBMENU_ROTATION180EDIT); //旋转180度
	tempSubMenw->SetMenuItemBitmaps(IDC_ROTATION180EDIT_BTN, MF_BYCOMMAND, &Rotation180Bmp,NULL);
	HirizontalBmp.LoadBitmap(IDB_WBMENU_HIRIZONTALEDIT); //水平放置
	tempSubMenw->SetMenuItemBitmaps(IDC_ROTATION_HIRIZONTALEDIT_BTN, MF_BYCOMMAND, &HirizontalBmp,NULL);
	VerticalBmp.LoadBitmap(IDB_WBMENU_VERTICALEDIT); //垂直放置
	tempSubMenw->SetMenuItemBitmaps(IDC_ROTATION_VERTICALEDIT_BTN, MF_BYCOMMAND, &VerticalBmp,NULL);

	//插入子菜单
	tempSubMenw = GetMenu()->GetSubMenu(3);
	PictureBmp.LoadBitmap(IDB_WBMENU_PICTUREINSERT); //图片
	tempSubMenw->SetMenuItemBitmaps(IDC_PICTUREINSERT_BTN, MF_BYCOMMAND, &PictureBmp,NULL);
	VedioBmp.LoadBitmap(IDB_WBMENU_VEDIOINSERT); //视频
	tempSubMenw->SetMenuItemBitmaps(IDC_MEDIOINSERT_BTN, MF_BYCOMMAND, &VedioBmp,NULL);
	FlashBmp.LoadBitmap(IDB_WBMENU_FLASHINSERT); //FLASH
	tempSubMenw->SetMenuItemBitmaps(IDC_FLASHINSERT_BTN, MF_BYCOMMAND, &FlashBmp,NULL);
	DocumentsBmp.LoadBitmap(IDB_WBMENU_DOCUMENTSINSERT); //文档
	tempSubMenw->SetMenuItemBitmaps(IDC_DOCUMENTINSERT_BTN, MF_BYCOMMAND, &DocumentsBmp,NULL);
	CameraheadBmp.LoadBitmap(IDB_WBMENU_CAMERAHEADSINSERT); //摄像头
	tempSubMenw->SetMenuItemBitmaps(IDC_CAMERAINSERT_BTN, MF_BYCOMMAND, &CameraheadBmp,NULL);
	
	tempSubMenw->DeleteMenu(4,MF_BYPOSITION);//屏蔽摄像头
	
	TextBmp.LoadBitmap(IDB_WBMENU_WRITINGSINSERT); //文本
	tempSubMenw->SetMenuItemBitmaps(IDC_TEXTINSERT_BTN, MF_BYCOMMAND, &TextBmp,NULL);
	AnnotationBmp.LoadBitmap(IDB_WBMENU_ANNOTATIONSINSERT); //备注
	tempSubMenw->SetMenuItemBitmaps(IDC_ANNOTATIONSINSERT_BEN, MF_BYCOMMAND, &AnnotationBmp,NULL);

	//绘制子菜单
	tempSubMenw = GetMenu()->GetSubMenu(4);
	BeelinesBmp.LoadBitmap(IDB_WBMENU_BEELINESDRAW); //直线
	tempSubMenw->SetMenuItemBitmaps(IDC_LINEDRAW_BEN, MF_BYCOMMAND, &BeelinesBmp,NULL);
	CurvesBmp.LoadBitmap(IDB_WBMENU_CURVESDRAW); //曲线
	tempSubMenw->SetMenuItemBitmaps(IDC_CURVEDRAW_BEN, MF_BYCOMMAND, &CurvesBmp,NULL);
	RectBmp.LoadBitmap(IDB_WBMENU_RECTDRAW); //矩形
	tempSubMenw->SetMenuItemBitmaps(IDC_RECTDRAW_BEN, MF_BYCOMMAND, &RectBmp,NULL);
	TrianglesBmp.LoadBitmap(IDB_WBMENU_TRIANGLESDRAW); //三角形
	tempSubMenw->SetMenuItemBitmaps(IDC_TRIANGLEDRAW_BEN, MF_BYCOMMAND, &TrianglesBmp,NULL);
	RoundBmp.LoadBitmap(IDB_WBMENU_ROUNDDRAW); //圆
	tempSubMenw->SetMenuItemBitmaps(IDC_ELLIPSEDRAW_BEN, MF_BYCOMMAND, &RoundBmp,NULL);
	RhombusBmp.LoadBitmap(IDB_WBMENU_RHOMBUSDRAW); //菱形
	tempSubMenw->SetMenuItemBitmaps(IDC_RHOMBUSDRAW_BEN, MF_BYCOMMAND, &RhombusBmp,NULL);
	BigRubberBmp.LoadBitmap(IDB_WBMENU_BIGRUBBERSDRAW); //大擦
	tempSubMenw->SetMenuItemBitmaps(IDC_BIGERRUBBERDRAW_BTN, MF_BYCOMMAND, &BigRubberBmp,NULL);
// 	SmallRubberBmp.LoadBitmap(IDB_WBMENU_LITTERRUBBERSDRAW); //小擦
// 	tempSubMenw->SetMenuItemBitmaps(IDC_SMALLRUBBERDRAW_BEN, MF_BYCOMMAND, &SmallRubberBmp,NULL);
	CurveRubbersBmp.LoadBitmap(IDB_WBMENU_CURVERUBBERSDRAW); //曲擦
	tempSubMenw->SetMenuItemBitmaps(IDC_CURVERUBBERDRAW_BEN, MF_BYCOMMAND, &CurveRubbersBmp,NULL);
	ObjRubbersBmp.LoadBitmap(IDB_WBMENU_OBJRUBBERSDRAW); //擦对象
	tempSubMenw->SetMenuItemBitmaps(IDC_OBJRUBBERDRAW_BTN, MF_BYCOMMAND, &ObjRubbersBmp,NULL);

	//工具子菜单
	tempSubMenw = GetMenu()->GetSubMenu(5);
	BaffleBmp.LoadBitmap(IDB_WBMENU_BAFFLESTOOL); //挡板
	tempSubMenw->SetMenuItemBitmaps(IDC_BAFFLETOOL_BEN, MF_BYCOMMAND, &BaffleBmp,NULL);
	SpotlightBmp.LoadBitmap(IDB_WBMENU_SPOTLIGHTSTOOL); //聚光灯
	tempSubMenw->SetMenuItemBitmaps(IDC_SPOTLIGHTTOOL_BTN, MF_BYCOMMAND, &SpotlightBmp,NULL);
	MagnifierBmp.LoadBitmap(IDB_WBMENU_MAGNIFIERSTOOL); //放大镜
	tempSubMenw->SetMenuItemBitmaps(IDC_MAQNTOOL_BTN, MF_BYCOMMAND, &MagnifierBmp,NULL);
	CutScreenBmp.LoadBitmap(IDB_WBMENU_CUTSCREENSTOOL); //截屏
	tempSubMenw->SetMenuItemBitmaps(IDC_CUTSCREENTOOL_BTN, MF_BYCOMMAND, &CutScreenBmp,NULL);
	RecycleBmp.LoadBitmap(IDB_WBMENU_RECYCLE); //回收站
	tempSubMenw->SetMenuItemBitmaps(IDC_RECYCLE_BTN, MF_BYCOMMAND, &RecycleBmp,NULL);
	ScreenRECBmp.LoadBitmap(IDB_WBMENU_SCREENRECSTOOL); //屏幕录制
	tempSubMenw->SetMenuItemBitmaps(IDC_SCREENRECTOOL_BTN, MF_BYCOMMAND, &ScreenRECBmp,NULL);
	DrawDesktopOpenBmp.LoadBitmap(IDB_WBMENU_DDOTOOL); //打开标注
	tempSubMenw->SetMenuItemBitmaps(IDC_OPENSCREENDRAWTOOL_BTN, MF_BYCOMMAND, &DrawDesktopOpenBmp,NULL);
	DrawDesktopEffectSwitchBmp.LoadBitmap(IDB_WBMENU_DDESTOOL); //效果切换
	tempSubMenw->SetMenuItemBitmaps(IDC_EFFECTSCREENDRAW_BTN, MF_BYCOMMAND, &DrawDesktopEffectSwitchBmp,NULL);
	DrawDesktopCloseBmp.LoadBitmap(IDB_WBMENU_DDCTOOLL); //关闭标注
	tempSubMenw->SetMenuItemBitmaps(IDC_CLOSESCREENDRAWTOOL_BTN, MF_BYCOMMAND, &DrawDesktopCloseBmp,NULL);
	ResourceListBmp.LoadBitmap(IDB_WBMENU_RRESOURCELISTSTOOL); //资源目录
	tempSubMenw->SetMenuItemBitmaps(IDC_RESOURCETOOL_BTN, MF_BYCOMMAND, &ResourceListBmp,NULL);
	ResourceManageBmp.LoadBitmap(IDB_WBMENU_RESOURCEMANAGESTOOL); //资源目录管理
	//tempSubMenw->SetMenuItemBitmaps(IDC_RESOURCEMANAGETOOL_BTN, MF_BYCOMMAND, &ResourceManageBmp,NULL);
	FunctionDlgBmp.LoadBitmap(IDB_WBMENU_FUNCTIONDLGSTOOL); //功能面板
	tempSubMenw->SetMenuItemBitmaps(IDC_FUNCTIONTOOL_BTN, MF_BYCOMMAND, &FunctionDlgBmp,NULL);
	//帮助子菜单
	tempSubMenw = GetMenu()->GetSubMenu(6);
	AboutBmp.LoadBitmap(IDB_WBMENU_ABOUTHELP); //关于
	tempSubMenw->SetMenuItemBitmaps(IDC_ABOUTHELP_BTN, MF_BYCOMMAND, &AboutBmp,NULL);
	HelpBmp.LoadBitmap(IDB_WBMENU_HELP); //查看帮助
	tempSubMenw->SetMenuItemBitmaps(IDC_LOOKHELP_BTN, MF_BYCOMMAND, &HelpBmp,NULL);


	GetMenu()->GetSubMenu(1)->DeleteMenu(6,MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->DeleteMenu(5,MF_BYPOSITION);

	GetClientRect(&m_RectWB);
	SetWBStatic();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CWBMainDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
}
void CWBMainDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//	CDialog::OnOK();
}

BOOL CWBMainDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CDialog::OnEraseBkgnd(pDC);
}

BOOL CWBMainDlg::PreTranslateMessage(MSG* pMsg)
{
	//屏蔽键盘Enter消息
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return FALSE;
	}
	//屏蔽键盘Esc消息
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
	{   //网络版
		CMenu * tempSubMenw;
		//文件子菜单	
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
		// 		SetHPDTSNotify((IHPNetNotify*)m_pWhiteBoardDlg);		   //设定通讯指针 m_pWhiteBoardDlg是继承接口类IHPNetNotify 并且实现了sendData方法
		// 		this->Connect();			 //重新连接底层库的通讯 只有加上这一句话才能真正的实现通讯
	} 
	else
	{
		//单机版
		CMenu * tempSubMenw;
		//文件子菜单	
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

		//添加会议成员观察者
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
			//退出会议时，不会进退出会议的回调函数 OnExitRoom
			//在退出会议的时候，判断是会议是不是有人，如果没有的话就发送关闭白板的广播
// 			if (m_onLineCurMeetingNumber <= 1)
// 			{
// 				if (bPower)
// 				{
// 					//发布本地音视频通道
// 					//在这个地方判断会议类型
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
			if (m_isNeedRealseTouchScreenAuthorize)//释放服务器触摸授权
			{
				pPlugin_Function->ReleaseTouchScreenAuthorize();
				m_isNeedRealseTouchScreenAuthorize = FALSE;
			}
			//放开单机版的白板内授权，如果硬件支持
			bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
			if (!bHardwareTouch) //硬件不支持触摸
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
	strItem =(CString)"AVCONWB电子白板";
	SetWindowText(strItem);//设置白板对话框的标题文字

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
	//文件子菜单	
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
		m_pWhiteBoardDlg->Create(CIndependent_WBDlg::IDD, this);	//把m_pWhiteBoardDlg的父窗口改成CWBMainDlg
		if (m_pWhiteBoardDlg->GetSafeHwnd() == NULL)
		{
			delete m_pWhiteBoardDlg;
			m_pWhiteBoardDlg = NULL;
			return NULL;
		}
		WB_SetTypeOfWB(WB_SINGLERVER);
		m_pWhiteBoardDlg->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
		//放开单机版的白板内授权，如果硬件支持
		bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
		if (!bHardwareTouch) //硬件不支持触摸
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
						if (m_isNeedRealseTouchScreenAuthorize)//释放触摸授权
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
	for (; it != lstMember.end(); it++)//设置在线成员列表
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
	m_lastOnlineMemberHitem = hItem;//得到最后一个在线成员项
	it = lstMember.begin();
	for (; it != lstMember.end(); it++)//设置离线成员列表
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
	m_lastOfflineMemberHitem = hItem;//得到最后一个离线成员项
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

			if (nStatus == 0)//退出会议
			{
				//m_lastOnlineMemberHitem = m_memberListTree.GetPrevSiblingItem(hItem);	
				pMember->status = 0;
				m_memberListTree.DeleteItem(hItem);
				m_lastOfflineMemberHitem = m_memberListTree.InsertItem(A2U(pMember->membername),1,1,m_memberlistTreeRoot,m_lastOfflineMemberHitem);
				m_memberListTree.SetItemData(m_lastOfflineMemberHitem,(DWORD)pMember);
				WB_MemberOffline(strMemberID.c_str());//wangxiaobin
				m_onLineCurMeetingNumber--;

			} 
			else//进入会议
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
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CWBMainDlg::OnRoomMemberItemEnd(void)
{
	InitMemberList();
	//当会议成员状态全部接受完毕，就判断会议是不是有人，如果没人的话就发送打开白板广播
	int nOnlineCurMeetingSum = CWBMainDlg::g_pMainDlg->GetOnlineMemberCounts();

	if (nOnlineCurMeetingSum <= 1)
	{
		// 		int nScreenID = 2, nWindowID = 0;
		// 		CUICmdSender::Instance().AVCast(GetDataHandle().GetUserID(), \
		// 			HPWBCHANNEL_INDEX, HPAV_TYPE_VIDEO,nScreenID, nWindowID);		
		//发布本地音视频通道
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
			AfxMessageBox(_T("您所在的用户组无权广播会议白板，详情请跟管理员联系！"));
			return;
		}
		IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if (NULL == pMember)
			return;
		std::string strMemberID =pMember->GetUserIDDomain();

		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		pPlugin_Function ->PublishLocalCard(HPWBCHANNEL_INDEX);	  //发送白板通道
		int nScreenID = 0, nWindowID = 0;			 //广播白板会议
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
	//宣布全部人员退会时进的回调
	CloseLoginDlg();
}

void CWBMainDlg::OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus)
{
	//更新成员
	UpdateMemberStatus(strMemberID, ucStatus);
}

void CWBMainDlg::OnBroadcast( int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,int nChannelType )
{
	
	switch (nCardIndex)
	{
	case  HPWBCHANNEL_INDEX:			//白板
		{
			 m_isReceiveBoardcast = TRUE;
			 SetIsOnline(TRUE);
			 this->SetWBStatic();
			//先联网 如果有人在开会，且是白板会议，就删除本地白板的所有对象 如没有人的就上传本地白板的所有内容
			if (NULL == m_pWhiteBoardDlg )
			{
				HWND hwnd = CreateWhiteBoard();
			}
			UploadChildWnd(m_pWhiteBoardDlg->m_hWnd);
 			if (m_isLocalBoardCastWB)	 									    //	 m_isReceiveBoardcast初始化的时候是FALSE，在本地端发送广播的时候是TRUE，则条件成立
 			{
// 				if (m_usMemberType >=4)
// 				{
// 					SetHPDTSNotify(NULL);
// 					WB_CloseFuntion();
// 					WB_SingleCloseFile();			   //先关闭本地白板的文件													    
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
				std::string strTemp = _W2A(cStrLoginId);//临时文件保存到HPDTS中
				WB_SetWBTempDir(strTemp.c_str());
				SetHPDTSNotify((IHPNetNotify*)m_pWhiteBoardDlg);//设置白板通讯指针
				this->Connect();
				SendAllObj();		
				m_isLocalBoardCastWB = FALSE;
 			}
			else                              //不成立的话就表明不是本地广播的
			{
				SetHPDTSNotify(NULL);
				WB_CloseFuntion();
				WB_SingleCloseFile();			   //先关闭本地白板的文件
				CString cStrLoginId = _T("UserTemp");
				if (m_pWBLoginDlg)
				{
					CWBLoginDlg *pLoginDlg = (CWBLoginDlg *)m_pWBLoginDlg;
					cStrLoginId = *(pLoginDlg->GetLoginID());
				}
				SetHPDTSSavePath(cStrLoginId);
				std::string strTemp = _W2A(cStrLoginId);
				WB_SetWBTempDir(strTemp.c_str());
				SetHPDTSNotify((IHPNetNotify*)m_pWhiteBoardDlg);//设置白板通讯指针
				this->Connect(); //连接网络实现同步
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
				//如果是本地发送广播 处理事件
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
	if (m_isNeedRealseTouchScreenAuthorize)//释放触摸授权
	{
		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if (pPlugin_Function == NULL)
		{
			return ;
		}
		pPlugin_Function->ReleaseTouchScreenAuthorize();
		m_isNeedRealseTouchScreenAuthorize = FALSE;
	}
	bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
	if (!bHardwareTouch) //硬件不支持触摸
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
	strItem =(CString)"AVCONWB电子白板";
	SetWindowText(strItem);//设置白板对话框的标题文字
 	SetWBStatic();
}

void CWBMainDlg::OnTouchScreenAuthorize(bool bAuthorize)
{
	m_isNeedRealseTouchScreenAuthorize = WB_SetTouchAuthorization(bAuthorize);
}

void CWBMainDlg::OnTimer(UINT_PTR nIDEvent)
{																				  
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (TIMER_WB_BROADCAST == nIDEvent)
	{
		KillTimer(TIMER_WB_BROADCAST);
		if (m_isReceiveBoardcast)
		{
			m_isReceiveBoardcast = FALSE;
			 return;
		}
 		//发布本地音视频通道
 		IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
 		if (NULL == pMember)
 			return;
 		std::string strMemberID =pMember->GetUserIDDomain();
 
 		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
 		pPlugin_Function ->PublishLocalCard(HPWBCHANNEL_INDEX);	  //发送白板通道
 		int nScreenID = 0, nWindowID = 0;			 //广播白板会议
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
// 				//退出会议时，不会进退出会议的回调函数 OnExitRoom
// 				//在退出会议的时候，判断是会议是不是有人，如果没有的话就发送关闭白板的广播
// 				if (m_onLineCurMeetingNumber <= 1)
// 				{
// 					if (bPower)
// 					{
// 						//发布本地音视频通道
// 						//在这个地方判断会议类型
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
				if (m_isNeedRealseTouchScreenAuthorize)//释放触摸授权
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
	else if( (nID & 0xFFF0) == SC_MINIMIZE ) //最小化
	{
		WB_SetIsMinSize(TRUE);

		CDialog::OnSysCommand(nID, lParam);
		return;
	}
	else if( (nID & 0xFFF0) == SC_RESTORE ) //从最小化恢复窗口
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
	//文件子菜单	
	tempSubMenw = GetMenu()->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_NEWFILE_BTN,MF_GRAYED);	 //新建文件
	tempSubMenw->EnableMenuItem(IDC_OPENFILE_BTN,MF_GRAYED);	 //打开文件
	tempSubMenw->EnableMenuItem(IDC_SAVEFILE_BTN,MF_GRAYED);	//保存文件
	tempSubMenw->EnableMenuItem(IDC_RESAVEFILE_BTN,MF_GRAYED);	 //另存文件
	tempSubMenw->EnableMenuItem(IDC_PRINTFILE_BTN,MF_GRAYED);	 //打印文件

	//页子菜单
	tempSubMenw = GetMenu()->GetSubMenu(1);  
	tempSubMenw->EnableMenuItem(IDC_NEWPAGE_BTN, MF_GRAYED);  //新建页
	tempSubMenw->EnableMenuItem(IDC_PREPAGE_BTN, MF_GRAYED);  //上一页
	tempSubMenw->EnableMenuItem(IDC_NEXTPAGE_BTN, MF_GRAYED);//下一页
	tempSubMenw->EnableMenuItem(IDC_FIRSTPAGE_BTN, MF_GRAYED);   //首页
	tempSubMenw->EnableMenuItem(IDC_LASTPAGE_EDIT, MF_GRAYED);   //末页
	//编辑子菜单
	tempSubMenw = GetMenu()->GetSubMenu(2);
	tempSubMenw->EnableMenuItem(IDC_SELECTEDIT_BTN, MF_GRAYED);   //选择
	tempSubMenw->EnableMenuItem(IDC_UNDOEDIT_BTN, MF_GRAYED);   //撤销
	tempSubMenw->EnableMenuItem(IDC_REDOEDIT_BTN, MF_GRAYED);   //重做
	tempSubMenw->EnableMenuItem(IDC_CLONEEDIT_BTN, MF_GRAYED);   //克隆
	tempSubMenw->EnableMenuItem(IDC_INFINITECLONEEDIT_BTN, MF_GRAYED);   //无限克隆
	tempSubMenw->EnableMenuItem(IDC_CUTEDIT_BTN, MF_GRAYED);   //剪切
	tempSubMenw->EnableMenuItem(IDC_COPYEDIT_BTN, MF_GRAYED);   //复制
	tempSubMenw->EnableMenuItem(IDC_PASTEEDIT_BTN, MF_GRAYED);   //粘贴
	tempSubMenw->EnableMenuItem(IDC_DELETEEDIT_BTN, MF_GRAYED);   //删除
	tempSubMenw->EnableMenuItem(IDC_DELETEALLEDIT_BTN, MF_GRAYED);   //全部删除
	//编辑菜单中图层子菜单
	tempSubMenw->EnableMenuItem(IDC_PRELAYEREDIT_BTN, MF_GRAYED);   //上一层
	tempSubMenw->EnableMenuItem(IDC_NEXTLAYEREDIT_BTN, MF_GRAYED);   //下一层
	tempSubMenw->EnableMenuItem(IDC_TOPLAYER_BTN, MF_GRAYED);   //顶层
	tempSubMenw->EnableMenuItem(IDC_BOTTOMLAYEREDIT_BTN, MF_GRAYED);   //末层
	//编辑菜单中旋转子菜单
	tempSubMenw->EnableMenuItem(IDC_DEASIL45EDIT_BTN, MF_GRAYED);   //顺时针旋转45度
	tempSubMenw->EnableMenuItem(IDC_DEASIL90EDIT_BTN, MF_GRAYED);   //顺时针旋转90度
	tempSubMenw->EnableMenuItem(IDC_WIDDERSHINS45EDIT_BTN, MF_GRAYED);   //逆时针旋转45度
	tempSubMenw->EnableMenuItem(IDC_WIDDERSHINS90EDIT_BTN, MF_GRAYED);   //逆时针旋转90度
	tempSubMenw->EnableMenuItem(IDC_ROTATION180EDIT_BTN, MF_GRAYED);   //旋转180度
	tempSubMenw->EnableMenuItem(IDC_ROTATION_HIRIZONTALEDIT_BTN, MF_GRAYED);   //水平放置
	tempSubMenw->EnableMenuItem(IDC_ROTATION_VERTICALEDIT_BTN, MF_GRAYED);   //垂直放置

	//插入子菜单
	tempSubMenw = GetMenu()->GetSubMenu(3);
	tempSubMenw->EnableMenuItem(IDC_PICTUREINSERT_BTN, MF_GRAYED);   //图片
	tempSubMenw->EnableMenuItem(IDC_MEDIOINSERT_BTN, MF_GRAYED);   //视频
	tempSubMenw->EnableMenuItem(IDC_FLASHINSERT_BTN, MF_GRAYED);   //FLASH
	tempSubMenw->EnableMenuItem(IDC_DOCUMENTINSERT_BTN, MF_GRAYED);   //文档
	tempSubMenw->EnableMenuItem(IDC_CAMERAINSERT_BTN, MF_GRAYED);   //摄像头
	tempSubMenw->EnableMenuItem(IDC_TEXTINSERT_BTN, MF_GRAYED);   //文本
	tempSubMenw->EnableMenuItem(IDC_ANNOTATIONSINSERT_BEN, MF_GRAYED);   //备注
	//绘制子菜单
	tempSubMenw = GetMenu()->GetSubMenu(4);
	tempSubMenw->EnableMenuItem(IDC_LINEDRAW_BEN, MF_GRAYED);   //直线
	tempSubMenw->EnableMenuItem(IDC_CURVEDRAW_BEN, MF_GRAYED);   //曲线
	tempSubMenw->EnableMenuItem(IDC_RECTDRAW_BEN, MF_GRAYED);   //矩形
	tempSubMenw->EnableMenuItem(IDC_TRIANGLEDRAW_BEN, MF_GRAYED);   //三角形
	tempSubMenw->EnableMenuItem(IDC_ELLIPSEDRAW_BEN, MF_GRAYED);   //圆
	tempSubMenw->EnableMenuItem(IDC_RHOMBUSDRAW_BEN, MF_GRAYED);   //菱形
	tempSubMenw->EnableMenuItem(IDC_BIGERRUBBERDRAW_BTN, MF_GRAYED);   //大擦
/*	tempSubMenw->EnableMenuItem(IDC_SMALLRUBBERDRAW_BEN, MF_GRAYED);   //小擦*/
	tempSubMenw->EnableMenuItem(IDC_CURVERUBBERDRAW_BEN, MF_GRAYED);   //曲擦
	tempSubMenw->EnableMenuItem(IDC_OBJRUBBERDRAW_BTN, MF_GRAYED);   //擦对象


	//工具子菜单
	tempSubMenw = GetMenu()->GetSubMenu(5);
	tempSubMenw->EnableMenuItem(IDC_BAFFLETOOL_BEN, MF_GRAYED);   //挡板
	tempSubMenw->EnableMenuItem(IDC_SPOTLIGHTTOOL_BTN, MF_GRAYED);   //聚光灯		
	tempSubMenw->EnableMenuItem(IDC_MAQNTOOL_BTN, MF_GRAYED);   //放大镜
	tempSubMenw->EnableMenuItem(IDC_CUTSCREENTOOL_BTN, MF_GRAYED);   //截屏
	tempSubMenw->EnableMenuItem(IDC_RECYCLE_BTN, MF_GRAYED);   //回收站
	tempSubMenw->EnableMenuItem(IDC_SCREENRECTOOL_BTN, MF_GRAYED);   //屏幕录制
	tempSubMenw->EnableMenuItem(IDC_OPENSCREENDRAWTOOL_BTN, MF_GRAYED);   //打开标注
	tempSubMenw->EnableMenuItem(IDC_EFFECTSCREENDRAW_BTN, MF_GRAYED);   //效果切换
	tempSubMenw->EnableMenuItem(IDC_CLOSESCREENDRAWTOOL_BTN, MF_GRAYED);   //关闭标注
	tempSubMenw->EnableMenuItem(IDC_RESOURCETOOL_BTN, MF_GRAYED);   //资源目录
	//tempSubMenw->EnableMenuItem(IDC_RESOURCEMANAGETOOL_BTN, MF_GRAYED);   //资源目录管理
	tempSubMenw->EnableMenuItem(IDC_FUNCTIONTOOL_BTN, MF_GRAYED);   //功能面板


}

void CWBMainDlg::UnLockMenu()
{
	CMenu * tempSubMenw;
	//文件子菜单	
	tempSubMenw = GetMenu()->GetSubMenu(0);
	tempSubMenw->EnableMenuItem(IDC_NEWFILE_BTN,MF_ENABLED);	 //新建文件
	tempSubMenw->EnableMenuItem(IDC_OPENFILE_BTN,MF_ENABLED);	 //打开文件
	tempSubMenw->EnableMenuItem(IDC_SAVEFILE_BTN,MF_ENABLED);	//保存文件
	tempSubMenw->EnableMenuItem(IDC_RESAVEFILE_BTN,MF_ENABLED);	 //另存文件
	tempSubMenw->EnableMenuItem(IDC_PRINTFILE_BTN,MF_ENABLED);	 //打印文件

	//页子菜单
	tempSubMenw = GetMenu()->GetSubMenu(1);  
	tempSubMenw->EnableMenuItem(IDC_NEWPAGE_BTN, MF_ENABLED);  //新建页
	tempSubMenw->EnableMenuItem(IDC_PREPAGE_BTN, MF_ENABLED);  //上一页
	tempSubMenw->EnableMenuItem(IDC_NEXTPAGE_BTN, MF_ENABLED);//下一页
	tempSubMenw->EnableMenuItem(IDC_FIRSTPAGE_BTN, MF_ENABLED);   //首页
	tempSubMenw->EnableMenuItem(IDC_LASTPAGE_EDIT, MF_ENABLED);   //末页
	//编辑子菜单
	tempSubMenw = GetMenu()->GetSubMenu(2);
	tempSubMenw->EnableMenuItem(IDC_SELECTEDIT_BTN, MF_ENABLED);   //选择
	tempSubMenw->EnableMenuItem(IDC_UNDOEDIT_BTN, MF_ENABLED);   //撤销
	tempSubMenw->EnableMenuItem(IDC_REDOEDIT_BTN, MF_ENABLED);   //重做
	tempSubMenw->EnableMenuItem(IDC_CLONEEDIT_BTN, MF_ENABLED);   //克隆
	tempSubMenw->EnableMenuItem(IDC_INFINITECLONEEDIT_BTN, MF_ENABLED);   //无限克隆
	tempSubMenw->EnableMenuItem(IDC_CUTEDIT_BTN, MF_ENABLED);   //剪切
	tempSubMenw->EnableMenuItem(IDC_COPYEDIT_BTN, MF_ENABLED);   //复制
	tempSubMenw->EnableMenuItem(IDC_PASTEEDIT_BTN, MF_ENABLED);   //粘贴
	tempSubMenw->EnableMenuItem(IDC_DELETEEDIT_BTN, MF_ENABLED);   //删除
	tempSubMenw->EnableMenuItem(IDC_DELETEALLEDIT_BTN, MF_ENABLED);   //全部删除
	//编辑菜单中图层子菜单
	tempSubMenw->EnableMenuItem(IDC_PRELAYEREDIT_BTN, MF_ENABLED);   //上一层
	tempSubMenw->EnableMenuItem(IDC_NEXTLAYEREDIT_BTN, MF_ENABLED);   //下一层
	tempSubMenw->EnableMenuItem(IDC_TOPLAYER_BTN, MF_ENABLED);   //顶层
	tempSubMenw->EnableMenuItem(IDC_BOTTOMLAYEREDIT_BTN, MF_ENABLED);   //末层
	//编辑菜单中旋转子菜单
	tempSubMenw->EnableMenuItem(IDC_DEASIL45EDIT_BTN, MF_ENABLED);   //顺时针旋转45度
	tempSubMenw->EnableMenuItem(IDC_DEASIL90EDIT_BTN, MF_ENABLED);   //顺时针旋转90度
	tempSubMenw->EnableMenuItem(IDC_WIDDERSHINS45EDIT_BTN, MF_ENABLED);   //逆时针旋转45度
	tempSubMenw->EnableMenuItem(IDC_WIDDERSHINS90EDIT_BTN, MF_ENABLED);   //逆时针旋转90度
	tempSubMenw->EnableMenuItem(IDC_ROTATION180EDIT_BTN, MF_ENABLED);   //旋转180度
	tempSubMenw->EnableMenuItem(IDC_ROTATION_HIRIZONTALEDIT_BTN, MF_ENABLED);   //水平放置
	tempSubMenw->EnableMenuItem(IDC_ROTATION_VERTICALEDIT_BTN, MF_ENABLED);   //垂直放置

	//插入子菜单
	tempSubMenw = GetMenu()->GetSubMenu(3);
	tempSubMenw->EnableMenuItem(IDC_PICTUREINSERT_BTN, MF_ENABLED);   //图片
	tempSubMenw->EnableMenuItem(IDC_MEDIOINSERT_BTN, MF_ENABLED);   //视频
	tempSubMenw->EnableMenuItem(IDC_FLASHINSERT_BTN, MF_ENABLED);   //FLASH
	tempSubMenw->EnableMenuItem(IDC_DOCUMENTINSERT_BTN, MF_ENABLED);   //文档
	tempSubMenw->EnableMenuItem(IDC_CAMERAINSERT_BTN, MF_ENABLED);//摄像头
	tempSubMenw->EnableMenuItem(IDC_TEXTINSERT_BTN, MF_ENABLED);   //文本
	tempSubMenw->EnableMenuItem(IDC_ANNOTATIONSINSERT_BEN, MF_ENABLED);   //备注
	//绘制子菜单
	tempSubMenw = GetMenu()->GetSubMenu(4);
	tempSubMenw->EnableMenuItem(IDC_LINEDRAW_BEN, MF_ENABLED);   //直线
	tempSubMenw->EnableMenuItem(IDC_CURVEDRAW_BEN, MF_ENABLED);   //曲线
	tempSubMenw->EnableMenuItem(IDC_RECTDRAW_BEN, MF_ENABLED);   //矩形
	tempSubMenw->EnableMenuItem(IDC_TRIANGLEDRAW_BEN, MF_ENABLED);   //三角形
	tempSubMenw->EnableMenuItem(IDC_ELLIPSEDRAW_BEN, MF_ENABLED);   //圆
	tempSubMenw->EnableMenuItem(IDC_RHOMBUSDRAW_BEN, MF_ENABLED);   //菱形
	tempSubMenw->EnableMenuItem(IDC_BIGERRUBBERDRAW_BTN, MF_ENABLED);   //大擦
/*	tempSubMenw->EnableMenuItem(IDC_SMALLRUBBERDRAW_BEN, MF_ENABLED);   //小擦*/
	tempSubMenw->EnableMenuItem(IDC_CURVERUBBERDRAW_BEN, MF_ENABLED);   //曲擦
	tempSubMenw->EnableMenuItem(IDC_OBJRUBBERDRAW_BTN, MF_ENABLED);   //擦对象


	//工具子菜单
	tempSubMenw = GetMenu()->GetSubMenu(5);
	tempSubMenw->EnableMenuItem(IDC_BAFFLETOOL_BEN, MF_ENABLED);   //挡板
	tempSubMenw->EnableMenuItem(IDC_SPOTLIGHTTOOL_BTN, MF_ENABLED);   //聚光灯		
	tempSubMenw->EnableMenuItem(IDC_MAQNTOOL_BTN, MF_ENABLED);   //放大镜
	tempSubMenw->EnableMenuItem(IDC_CUTSCREENTOOL_BTN, MF_ENABLED);   //截屏
	tempSubMenw->EnableMenuItem(IDC_RECYCLE_BTN, MF_ENABLED);   //回收站
	tempSubMenw->EnableMenuItem(IDC_SCREENRECTOOL_BTN, MF_ENABLED);   //屏幕录制
	tempSubMenw->EnableMenuItem(IDC_OPENSCREENDRAWTOOL_BTN, MF_ENABLED);   //打开标注
	tempSubMenw->EnableMenuItem(IDC_EFFECTSCREENDRAW_BTN, MF_ENABLED);   //效果切换
	tempSubMenw->EnableMenuItem(IDC_CLOSESCREENDRAWTOOL_BTN, MF_ENABLED);   //关闭标注
	tempSubMenw->EnableMenuItem(IDC_RESOURCETOOL_BTN, MF_ENABLED);   //资源目录
	//tempSubMenw->EnableMenuItem(IDC_RESOURCEMANAGETOOL_BTN, MF_ENABLED);   //资源目录管理
	tempSubMenw->EnableMenuItem(IDC_FUNCTIONTOOL_BTN, MF_ENABLED);   //功能面板
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
							// 						//退出会议时，不会进退出会议的回调函数 OnExitRoom
							// 						//在退出会议的时候，判断是会议是不是有人，如果没有的话就发送关闭白板的广播
							// 						if (m_onLineCurMeetingNumber <= 1)
							// 						{
							// 							if (bPower)
							// 							{
							// 								//发布本地音视频通道
							// 								//在这个地方判断会议类型
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
							if (m_isNeedRealseTouchScreenAuthorize)//释放触摸授权
							{
								pPlugin_Function->ReleaseTouchScreenAuthorize();
								m_isNeedRealseTouchScreenAuthorize = FALSE;
							}
							bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
							if (!bHardwareTouch) //硬件不支持触摸
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
						if (m_isNeedRealseTouchScreenAuthorize)//释放触摸授权
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
