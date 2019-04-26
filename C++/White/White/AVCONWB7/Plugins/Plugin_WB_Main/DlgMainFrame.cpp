#include "StdAfx.h"
#include "DlgMainFrame.h"
#include "tlhelp32.h"
#include "atlbase.h"
#include "atlstr.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"
#include "IPlugin/Plugin_WB_Login/IPlugin_WB_Login.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
#include "IPlugin/Plugin_Meeting_Member/IPlugin_Meeting_Member.h"
#include "IPlugin/IU7_IM/IPlugin_U7_IM_Login.h"
#include "IPlugin/Plugin_WB_Main/IPlugin_WB_Main.h"
#include "IPlugin/IU7_AV/IPlugin_U7_AV_Capture.h"
#include "IPlugin/IU7_AV/IPlugin_U7_AV_Player.h"
#include "IPlugin/IU7T_TipWindow/IU7T_TipWindow.h"


#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define TIMER_BROADCAST_WB		1888 //���ʹ򿪹㲥�Ķ�ʱ���¼�

CDlgMainFrame* g_pDlgMainFrame_ = NULL;
CDlgMainFrame* GetDlgMainFrame_(void)
{
	return g_pDlgMainFrame_;
}
CDlgMainFrame::CDlgMainFrame(void)
: m_bTouchScreenAuthorize(false)
, m_usMemberType (5) //0 ~4 ��ʾ��Ȩ���ڻ����д򿪰װ�㲥�� Ĭ��������� �����û��Ȩ�޴򿪰װ�㲥
, m_bTakeDataToNet(false)
, m_bAcceptBroadCast(false)
,m_bIsExitRoomCurTime(false)
{
	g_pDlgMainFrame_ = this;
	m_strUserMemberID.clear();
	m_pMenuBarDlg = NULL;
}

CDlgMainFrame::~CDlgMainFrame(void)
{
	g_pDlgMainFrame_ = NULL;
	
}

// �̳�WindowImplBase�ӿ�
void CDlgMainFrame::Init()
{
 	IPlugin_WB_FrameDlg* pPlugin_Wb_Dlg = dynamic_cast<IPlugin_WB_FrameDlg*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_FrameDlg"));
 	if (pPlugin_Wb_Dlg == NULL)
 		return;
 	if (pPlugin_Wb_Dlg->CreateWhiteBoard() == NULL)
 		return;
 	pPlugin_Wb_Dlg->AddObserver(this);
 	
 	RECT rcWorkArea;
 	int cx = GetSystemMetrics( SM_CXSCREEN ); 
 	int cy = GetSystemMetrics( SM_CYSCREEN );
 	rcWorkArea.left = 0;
 	rcWorkArea.top = 0;
 	rcWorkArea.right = cx;
 	rcWorkArea.bottom = cy;
 	HWND hWndAffter = HWND_TOP;
 	pPlugin_Wb_Dlg->SetWindowPos(hWndAffter,0,rcWorkArea.top ,rcWorkArea.right,rcWorkArea.bottom,SWP_SHOWWINDOW);
  	
	SetCursor(LoadCursor(NULL,IDC_ARROW));   //�������Ϊ����״̬
	IPlugin_U7_MM_Function* pPlugin_Funtion = dynamic_cast<IPlugin_U7_MM_Function*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Function"));
	if (pPlugin_Funtion)
	{
		pPlugin_Funtion->AddObserver(this);
	}
	IPlugin_U7_MM_Member* pPlugin_MM = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (pPlugin_MM)
	{
		pPlugin_MM->AddObserver(this);
	}
	IPlugin_U7_MM_Room* pPlugin_Room = dynamic_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pPlugin_Room)
	{
		pPlugin_Room->AddObserver(this);
	}
	//pPlugin_Wb_Dlg->OpenResource();
	ShowMenuBarDlg();
	
}

LRESULT CDlgMainFrame::OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	IPlugin_U7_MM_Function* pPlugin_Funtion = dynamic_cast<IPlugin_U7_MM_Function*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Function"));
	if (pPlugin_Funtion)
	{
		pPlugin_Funtion->DelObserver(this);
	}
	IPlugin_U7_MM_Member* pPlugin_MM = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (pPlugin_MM)
	{
		pPlugin_MM->DelObserver(this);
	}
	IPlugin_U7_MM_Room* pPlugin_Room = dynamic_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pPlugin_Room)
	{
		pPlugin_Room->DelObserver(this);
	}
	IPlugin_WB_FrameDlg* pPlugin_Wb_Dlg = dynamic_cast<IPlugin_WB_FrameDlg*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_FrameDlg"));
	if (pPlugin_Wb_Dlg != NULL)
	{
		pPlugin_Wb_Dlg->DelObserver(this);
		pPlugin_Wb_Dlg->DestroyWhiteBoard();
	}
	if (m_pMenuBarDlg != NULL)
	{	
		::DestroyWindow(m_pMenuBarDlg->GetHWND());
		delete m_pMenuBarDlg;
		m_pMenuBarDlg = NULL;		
	}
	return WindowImplBase::OnDestroy(uMsg,wParam,lParam,bHandled);
}
LRESULT CDlgMainFrame::OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (wParam == VK_ESCAPE)
	{
		PostQuitMessage(0);
	}
	return WindowImplBase::OnKeyDown( uMsg,  wParam,  lParam, bHandled);
}
LPCTSTR CDlgMainFrame::GetWindowClassName() const
{
	return _T("CDlgMainFrame");
}

CString CDlgMainFrame::GetSkinFile()
	{
	return _T("DlgMainFrame.xml");
	}

void CDlgMainFrame::Notify( TNotifyUI& msg )
{
	if (COUI_MSGTYPE_TIMER == msg.nType)
	{
		OnCoUITimer(msg);
	}
}

void CDlgMainFrame::ProcessWBToLocalData( char* pNotifyData )
{
	if (pNotifyData)
	{
		int iCmdType = *((int*)pNotifyData);
		switch (iCmdType)
		{
		case CMD_SINGLE_NETWORK://�ӵ�����������������¼��
			{
				IPlugin_WB_Meeting *pluginMeeting = static_cast<IPlugin_WB_Meeting*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Meeting"));
				if (pluginMeeting != NULL)
				{
					pluginMeeting->ShowUIMeetingList(false);
				}
				IPlugin_WB_Login *pluginlogin = dynamic_cast<IPlugin_WB_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Login"));
				if (pluginlogin != NULL)
				{
					pluginlogin->Show(true);
				} 
			} 
			break;
		case CMD_SINGLE_LOCAL: //���Ѿ���������£���ɵ����İװ�
			{
				OnExitInternet();
			} 
			break;
		case CMD_SINGLE_SHOTDOWN:   //�ڵ������ն���ʹ�ã��ػ�
			{
				//�û�������Ƶ��������������ɾ��������
				IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
				if (NULL == pMember)
					return;
				std::string strMemberID =pMember->GetUserIDDomain();
				WB_MemberOffline(strMemberID.c_str());

				ShutDown();
			} 
			break;
		case CMD_SINGLE_QUIT: //�˳����س���
			{		
 				IPlugin_U7T_MessageBox *pMessage = static_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
 				if (pMessage == NULL) 
 					return;
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("��ʾ");
				}
				CString strIsExit;
				strIsExit = ReadConfigText(_T("PluginText"), _T("MAIN_IsExit"));
				if (strIsExit.IsEmpty())
				{
					strIsExit = _T("ȷ���˳�����");
				}
 				int nResult = pMessage->MessageBox(GetHWND(),strPrompt,strIsExit,MB_OKCANCEL,false);
 				if (nResult == IDOK)
 				{
 					IPlugin_WB_Login* pPlugin_Login = dynamic_cast<IPlugin_WB_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Login"));
 					if (pPlugin_Login)
 					{
 						pPlugin_Login->Destroy();
 					}
				
					//PostQuitMessage(0);
					IPlugin_U7_MM_Room *pluginRoom = static_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
					if (pluginRoom != NULL)
					{
						pluginRoom->ExitRoom(false);
					}
                    WB_Close();
					//ȥ��ǿ�ƹر��߳��˳��������������˳���ܸĶ�����Ȼ����wangxiaobin
					SendMessage(WM_CLOSE);
					IPlugin_WB_Main *pluginMain = static_cast<IPlugin_WB_Main*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Main"));
					if (pluginMain != NULL)
					{
						pluginMain->Exit();
					}
					PostQuitMessage(0);
					//exit(0);
 					/*HANDLE hProcessSnap; 
 					TCHAR ProcessName[]=_T("AVCON_WB.exe"); 
 					DWORD ProcessID=0x00; 
 					hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
 					if(hProcessSnap==INVALID_HANDLE_VALUE) 
 					{ 
 						return ; 
 					} 
 					PROCESSENTRY32 pe32; 
 					pe32.dwSize = sizeof(pe32); 
 					BOOL bProcess = Process32First(hProcessSnap, &pe32); 
 					TCHAR buff[1024]; 
 					while(bProcess) 
 					{ 
 						std::string strName = U2A("AVCON_WB.exe");
 						if(strcmp(strName.c_str(),CW2A(pe32.szExeFile))==0) 
 						{ 
 							ProcessID= pe32.th32ProcessID; 
 							break; 
 						} 
 						memset(buff, 0x00, 1024); 
 						bProcess=Process32Next(hProcessSnap, &pe32); 
 					} 
 					if(ProcessID==0) 
 					{ 
 
 						return; 
 					} 
 
 					HANDLE hProcess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessID);
 					::TerminateProcess(hProcess,0);
 
 					CloseHandle(hProcess);*/
				
				}
			} 
			break;
		default:
			break;
		}
	}

}

void CDlgMainFrame::OnBroadcast( int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,const char* strCardName,int nChannelType,const MMS_MEMBER_TYPE& memberType)
{
	switch (nCardIndex)
	{
	case  HPWBCHANNEL_INDEX:			//�װ�
		{
			m_bAcceptBroadCast = true;
			IPlugin_U7T_TipWindow* pPluginTipWindow = dynamic_cast<IPlugin_U7T_TipWindow*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_TipWindow"));
			IPlugin_U7T_MessageBox* IPlugin_MessageBox = dynamic_cast<IPlugin_U7T_MessageBox*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7T_MessageBox"));
			IPlugin_WB_FrameDlg* pPlugin_Wb_Dlg = dynamic_cast<IPlugin_WB_FrameDlg*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_FrameDlg"));
			if (pPlugin_Wb_Dlg == NULL)
			{
				if (pPluginTipWindow)
				{
// 					CString strPrompt;
// 					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
// 					if (strPrompt.IsEmpty())
// 					{
// 						strPrompt = _T("��ʾ");
// 					}
					CString strOpenWBFail;
					strOpenWBFail = ReadConfigText(_T("PluginText"), _T("MAIN_OpenWBFail"));
					if (strOpenWBFail.IsEmpty())
					{
						strOpenWBFail = _T("�򿪰װ�ʧ�ܣ�");
					}
					pPluginTipWindow->ShowTipWindow(strOpenWBFail,3,this->GetHWND());
				}
// 				if (IPlugin_MessageBox)
// 				{
// 					CString strPrompt;
// 					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
// 					if (strPrompt.IsEmpty())
// 					{
// 						strPrompt = _T("��ʾ");
// 					}
// 					CString strOpenWBFail;
// 					strOpenWBFail = ReadConfigText(_T("PluginText"), _T("MAIN_OpenWBFail"));
// 					if (strOpenWBFail.IsEmpty())
// 					{
// 						strOpenWBFail = _T("�򿪰װ�ʧ�ܣ�");
// 					}
// 					IPlugin_MessageBox->MessageBox(this->GetHWND(),strPrompt,strOpenWBFail,MB_OK,true);
// 				}
				return;
			}
				//	������ ��������ڿ��ᣬ���ǰװ���飬��ɾ�����ذװ�����ж��� ��û���˵ľ��ϴ����ذװ����������
			if (!m_bTakeDataToNet) 
			{
				WB_CloseFuntion();//�ȹرձ��ذװ���ļ�
				WB_SingleCloseFile();		
				//���նԷ��İװ����ݣ�ͬʱ���ػ�ɾ��HPDTS�ļ��У�������Է��װ�û�����ݵĻ����ᷢ�����ݹ����Ӷ�����HPDTS�ļ���
				//���Ա��ش��´���
				std::string strDtsTempPath = CW2A(GetExePath());		
				strDtsTempPath += "HPDTS";
				_tmkdir(CA2W(strDtsTempPath.c_str()));
				strDtsTempPath = strDtsTempPath + "\\" + strMemberID;
				_tmkdir(CA2W(strDtsTempPath.c_str()));
				if (!strDtsTempPath.empty())
				{
					HPDTSInit(strDtsTempPath.c_str());				

				}
			}
			bool bRel = this->Connect();
			if (!bRel && !m_bIsExitRoomCurTime)
			{
				m_bIsExitRoomCurTime = false;
				if (pPluginTipWindow)
				{
					CString strConnectFail;
					strConnectFail = ReadConfigText(_T("PluginText"), _T("MAIN_ConnectFail"));
					if (strConnectFail.IsEmpty())
					{
						strConnectFail = _T("���ӻ���ʧ�ܣ����˻����µ��룡");
					}
					pPluginTipWindow->ShowTipWindow(strConnectFail,3,this->GetHWND());
				}
// 				if (IPlugin_MessageBox)
// 				{
// 					CString strPrompt;
// 					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
// 					if (strPrompt.IsEmpty())
// 					{
// 						strPrompt = _T("��ʾ");
// 					}
// 					CString strConnectFail;
// 					strConnectFail = ReadConfigText(_T("PluginText"), _T("MAIN_ConnectFail"));
// 					if (strConnectFail.IsEmpty())
// 					{
// 						strConnectFail = _T("���ӻ���ʧ�ܣ����˻����µ��룡");
// 					}
// 					IPlugin_MessageBox->MessageBox(this->GetHWND(),strPrompt,strConnectFail,MB_OK,true);
// 				}
				return;
			}
			WB_IsSuccessLoad(bRel);

			if (m_bTakeDataToNet)
			{
				if (m_usMemberType > 4)
				{
					WB_CloseFuntion();//�ȹرձ��ذװ���ļ�
					WB_SingleCloseFile();	
					//WB_AvconLock(true);
				}
				else
				{
					WB_SendAllObj();
				}
				if (pPluginTipWindow)
				{
					CString strConnectSuccess;
					strConnectSuccess = ReadConfigText(_T("PluginText"), _T("MAIN_ConnectSuccess"));
					if (strConnectSuccess.IsEmpty())
					{
						strConnectSuccess = _T("�������ӳɳɹ���");
					}
					pPluginTipWindow->ShowTipWindow(strConnectSuccess,3,this->GetHWND());
				}
// 				if (IPlugin_MessageBox)
// 				{
// 					CString strPrompt;
// 					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
// 					if (strPrompt.IsEmpty())
// 					{
// 						strPrompt = _T("��ʾ");
// 					}
// 					CString strConnectSuccess;
// 					strConnectSuccess = ReadConfigText(_T("PluginText"), _T("MAIN_ConnectSuccess"));
// 					if (strConnectSuccess.IsEmpty())
// 					{
// 						strConnectSuccess = _T("�������ӳɳɹ���");
// 					}
// 					IPlugin_MessageBox->MessageBox(this->GetHWND(),strPrompt,strConnectSuccess,MB_OK,true);
// 				}
				m_bTakeDataToNet = false;
			}
			else
			{
				if (m_usMemberType > 4)
				{
					//WB_AvconLock(true);
				}
				WB_ReqAllObj();//�������ж���
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
	if (nChannelType == HPAV_TYPE_AV|| nChannelType == HPAV_TYPE_AUDIO)
	{			
		IPlugin_U7_MM_Member* pMMMem = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if (pMMMem == NULL)
		{
			return;
		}
		if (strMemberID == pMMMem->GetUserIDDomain()&&nCardIndex <100)
		{
			IPlugin_U7_AV_CaptureMgr* pPluginAVCaptureMgr = dynamic_cast<IPlugin_U7_AV_CaptureMgr*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_AV_CaptureMgr"));
			if (pPluginAVCaptureMgr == NULL)
			{
				return;
			}
			IPlugin_U7_AV_Capture* pPluginAVCapture = pPluginAVCaptureMgr->GetU7AVCaptureByChnID(nCardIndex);
			if (pPluginAVCapture == NULL)
			{
				CAP_TYPE type = CAP_TYPE_AV;
				if (nCardIndex > 49 && nCardIndex < 99)
				{
					type = CAP_TYPE_IPCAM;
				}
				int nDevID = pPluginAVCaptureMgr->GetChannelDev(nCardIndex);
				pPluginAVCapture = pPluginAVCaptureMgr->CreateU7AVCapture(type,nDevID);
			}
			if (!pPluginAVCapture->IsAudioOpen())
			{
				pPluginAVCapture->OpenAudio();
			}
		}
		else
		{
			IPlugin_U7_AV_PlayerMgr* pU7PlayerMgr = dynamic_cast<IPlugin_U7_AV_PlayerMgr*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_AV_PlayerMgr"));
			if (pU7PlayerMgr == NULL)
			{
				return;
			}
			IPlugin_U7_AV_Player* pPluginU7AVPlayer = pU7PlayerMgr->GetU7AVPlayer(strMemberID,nCardIndex);
			if (pPluginU7AVPlayer == NULL)
			{
				pPluginU7AVPlayer = pU7PlayerMgr->CreateU7AVPlayer(strMemberID,nCardIndex);
				if (pPluginU7AVPlayer == NULL)
				{
					return;
				}
				pPluginU7AVPlayer->Open(false);
				pPluginU7AVPlayer->RecvAudio(true);

			}
		}
			
			
	}
}

void CDlgMainFrame::OnCloseBroadcast( const char* strMemberID,int nCardIndex,int nChannelType )
{
	if(nCardIndex == HPWBCHANNEL_INDEX/* || m_pSkinNotifyTransitionToNet == NULL*/)
	{
		WB_MemberOffline(strMemberID);
		return;
	}

	
	//ReleaseConnect();
	
	
	if (HPAV_TYPE_AV == nChannelType || HPAV_TYPE_AUDIO == nChannelType)
	{
		IPlugin_U7_MM_Member* pPluginU7MMmem = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if (pPluginU7MMmem == NULL)
		{
			return;
		}
		if (pPluginU7MMmem->GetUserIDDomain() != strMemberID)
		{
			IPlugin_U7_AV_PlayerMgr* pU7AVPlayerMgr = NULL;
			IPlugin_U7_AV_Player* pU7AvPlayer = NULL;
			
			pU7AVPlayerMgr = dynamic_cast<IPlugin_U7_AV_PlayerMgr*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_AV_PlayerMgr"));
			if (pU7AVPlayerMgr == NULL)
			{
				return;
			}
			pU7AvPlayer = pU7AVPlayerMgr->GetU7AVPlayer(strMemberID,nCardIndex);
			if (pU7AvPlayer == NULL)
			{
				return;
			}
			pU7AvPlayer->RecvAudio(false);
			pU7AVPlayerMgr->DestroyU7AVPlayer(strMemberID,nCardIndex);
		}
	}
	
}


void CDlgMainFrame::OnTouchScreenAuthorize( bool bAuthorize )
{
	//���ðװ崥����Ȩ
	m_bTouchScreenAuthorize = bAuthorize;
	WB_SetTouchAuthorization(bAuthorize);
}

bool CDlgMainFrame::Connect()
{
	IPlugin_WB_FrameDlg* pPlugin_Wb_Dlg = dynamic_cast<IPlugin_WB_FrameDlg*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_FrameDlg"));
	if (pPlugin_Wb_Dlg == NULL)
		return false;
	return pPlugin_Wb_Dlg->Connect();
}

bool CDlgMainFrame::ReleaseConnect()
{
	IPlugin_WB_FrameDlg* pPlugin_Wb_Dlg = dynamic_cast<IPlugin_WB_FrameDlg*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_FrameDlg"));
	if (pPlugin_Wb_Dlg == NULL)
		return false;
	pPlugin_Wb_Dlg->DisConnect();
	WB_AvconLock(false);
	return true;
}


void CDlgMainFrame::OnEnterRoomReady()
{
	bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
	if (bHardwareTouch)//����������봥����Ȩ
	{
		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if (pPlugin_Function == NULL)
		{
			return ;
		}
		pPlugin_Function->GetTouchScreenAuthorize();
	}
	
	IPlugin_U7_MM_Function* pPlugin_Funtion = dynamic_cast<IPlugin_U7_MM_Function*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Function"));
	if (pPlugin_Funtion == NULL)
	{
		return;
	}
	IPlugin_U7_MM_Member* pPlugin_MM = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (pPlugin_MM == NULL)
	{
		return;
	}
	
	IPlugin_U7_AV_CaptureMgr* pCaptureMgr = dynamic_cast<IPlugin_U7_AV_CaptureMgr*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_AV_CaptureMgr"));
	if (pCaptureMgr == NULL)
	{
		return;
	}
	
	IPlugin_U7_AV_Capture* pAVCapture = NULL;
	pAVCapture = pCaptureMgr->CreateU7AVCapture(CAP_TYPE_AV,0);
	if (pAVCapture)
	{
		if (!pAVCapture->IsAudioOpen())
		{
			pAVCapture->OpenAudio();
		}
	}
	if (pPlugin_Funtion)
	{
		pPlugin_Funtion->PublishLocalCard(0);
		pPlugin_Funtion->Broadcast(0,1,pPlugin_MM->GetUserIDDomain().c_str(),0,HPAV_TYPE_AUDIO,false);
	}
	
}

void CDlgMainFrame::OnEnterRoom( unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType )
{
	if (nErrCode != 0)
		return;
	m_usMemberType = usMemberType;


	IPlugin_U7_MM_Member* pUseMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (NULL == pUseMember)
		return;
	if (!m_strUserMemberID.empty())
	{
		m_strUserMemberID.clear(); 

	}
	std::string strMemberID =pUseMember->GetUserIDDomain();
	m_strUserMemberID = strMemberID;

	WB_IsSuccessLoad(true);
	IPlugin_U7_MM_Room* pU7MMRoom = dynamic_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pU7MMRoom == NULL)
		return;

	string strRoomId = strRoomID;
	std::map<std::string,bool> mapFilter; //��ʱmap����ɸѡ�ظ��Ļ���
	MMS_ROOM_LST& lstRoom = pU7MMRoom->GetRoomList();
	bool bFindRoom = false;
	MMS_ROOM_LST::iterator it = lstRoom.begin();
	for (; it != lstRoom.end(); it++)
	{
		PMMS_ROOM pRoomListItem = *it;
		if (strRoomId == pRoomListItem->roomid)
		{
			bFindRoom= true;
			break;
		}
	}
	if (!bFindRoom)
		return;
	if ((*it)->onlinesum == 0)
	{
		if (m_usMemberType >= 4) //û��Ȩ�޴򿪰װ� �����Ѿ������������ݣ�Ҳ����˵���ܵ��㲥�ˣ�
			return;
		//������������Ƶͨ��
		IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
		if (NULL == pMember)
			return;
		std::string strMemberID =pMember->GetUserIDDomain();
		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if (pPlugin_Function == NULL) 
			return;
		pPlugin_Function ->PublishLocalCard(HPWBCHANNEL_INDEX);	  //���Ͱװ�ͨ��
		int nScreenID = 0, nWindowID = 0;			 //�㲥�װ����
		pPlugin_Function->Broadcast(nScreenID,nWindowID,strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
		m_bTakeDataToNet = true;
	}
	else
	{
		CoUI_SetTimer(TIMER_BROADCAST_WB,15000); //15����û�н��ܵ��㲥��Ϣ��������ʱ��
	}
}

void CDlgMainFrame::OnExitRoom( unsigned int nErrCode )
{
	if (m_bTouchScreenAuthorize)//�ͷŷ�����������Ȩ
	{
		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if (pPlugin_Function)
		{
			pPlugin_Function->ReleaseTouchScreenAuthorize();
		}
	}
	else
	{
		//�ſ�������İװ�����Ȩ�����Ӳ��֧��
		bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
		WB_SetTouchAuthorization(bHardwareTouch);
	}
	IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (NULL == pMember)
		return;
	std::string strMemberID =pMember->GetUserIDDomain();
	WB_MemberOffline(strMemberID.c_str());
	m_bAcceptBroadCast = false;
	m_bIsExitRoomCurTime = true;
	m_bTakeDataToNet = false;	
	CoUI_KillTimer(TIMER_BROADCAST_WB);	
	WB_IsSuccessLoad(false);
	ReleaseConnect();
	
	IPlugin_U7_MM_Member* pU7MMMem = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
	if (pU7MMMem == NULL)
	{
		return;
	}
	IPlugin_U7_MM_Function* pU7MMFunc = dynamic_cast<IPlugin_U7_MM_Function*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Function"));
	if (pU7MMFunc)
	{
		pU7MMFunc->CloseBroadcast(pU7MMMem->GetUserIDDomain().c_str(),0,HPAV_TYPE_AUDIO);
	}
	IPlugin_U7_AV_Capture* pAVCapture = NULL;
	IPlugin_U7_AV_CaptureMgr* pAVCaptureMgr = dynamic_cast<IPlugin_U7_AV_CaptureMgr*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_AV_CaptureMgr"));
	if (pAVCaptureMgr != NULL)
	{
		pAVCapture = pAVCaptureMgr->CreateU7AVCapture(CAP_TYPE_AV,0);
		if (pAVCapture)
		{
			pAVCapture->CloseAudio();
		}
	}
	
}

void CDlgMainFrame::OnCoUITimer( TNotifyUI& msg )
{
	switch (msg.wParam)
	{
	case TIMER_BROADCAST_WB:
		{
			CoUI_KillTimer(TIMER_BROADCAST_WB);
			if (m_usMemberType >= 4 || m_bAcceptBroadCast) //û��Ȩ�޴򿪰װ� �����Ѿ������������ݣ�Ҳ����˵���ܵ��㲥�ˣ�
				return;
			//������������Ƶͨ��
			IPlugin_U7_MM_Member* pMember = dynamic_cast<IPlugin_U7_MM_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Member"));
			if (NULL == pMember)
				return;
			std::string strMemberID =pMember->GetUserIDDomain();

			IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Function"));
			if (pPlugin_Function == NULL) 
				return;
			pPlugin_Function ->PublishLocalCard(HPWBCHANNEL_INDEX);	  //���Ͱװ�ͨ��
			int nScreenID = 0, nWindowID = 0;			 //�㲥�װ����
			pPlugin_Function->Broadcast(nScreenID,nWindowID,strMemberID.c_str(),HPWBCHANNEL_INDEX,AVCONPlugin::HPAV_TYPE_AV);
			m_bTakeDataToNet = true;
		}
		break;
	default:
		break;
	}
}

void CDlgMainFrame::OnExitInternet()
{
	IPlugin_U7_MM_Room *pluginRoom = static_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pluginRoom != NULL)
	{
		pluginRoom->ExitRoom(false);
	}
	IPlugin_Meeting_Member *pluginMember = static_cast<IPlugin_Meeting_Member*>(AVCONPlugin::g_System->GetPluginByName("Plugin_Meeting_Member"));
	if (pluginMember != NULL)
	{
		pluginMember->DestroyUI();

	}
	IPlugin_U7_IM_Login *pImLogin = static_cast<IPlugin_U7_IM_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_IM_Login"));
	if (pImLogin != NULL)
	{
		pImLogin->Logout();
	}

	IPlugin_WB_Meeting *pluginMeeting = static_cast<IPlugin_WB_Meeting*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Meeting"));
	if (pluginMeeting != NULL)
	{
		pluginMeeting->ClosePlugin();
	}
	IPlugin_WB_Login *pluginloginWB = dynamic_cast<IPlugin_WB_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Login")); 
	if (pluginloginWB != NULL)
	{
		pluginloginWB->Destroy();
	}
	
	if (m_bTouchScreenAuthorize)//�ͷŷ�����������Ȩ
	{
		IPlugin_U7_MM_Function* pPlugin_Function = static_cast<IPlugin_U7_MM_Function*>(g_System->GetPluginByName("Plugin_U7_MM_Function"));
		if (pPlugin_Function)
		{
			pPlugin_Function->ReleaseTouchScreenAuthorize();
		}
	}
	else
	{
		//�ſ�������İװ�����Ȩ�����Ӳ��֧��
		bool bHardwareTouch = WB_BHardwareSupportMTTouch(); //��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
		WB_SetTouchAuthorization(bHardwareTouch);
	}

	ReleaseConnect();

}

void CDlgMainFrame::ShowMenuBarDlg()
{
	if (m_pMenuBarDlg == NULL)
	{
		m_pMenuBarDlg = new CMenuBarDlg;
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		m_pMenuBarDlg->Create(this->GetHWND(),_T("CMenuBarDlg"),UI_WNDSTYLE_FRAME,WS_EX_TOOLWINDOW&~WS_SIZEBOX,0,cy-31,cx,cy);
		::SetWindowPos(m_pMenuBarDlg->GetHWND(),HWND_TOPMOST,0,cy-31,cx,cy,SWP_SHOWWINDOW);
	}
	else
	{
		m_pMenuBarDlg->ShowWindow(true);
	}
}








