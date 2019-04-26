// CameraDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgCamera.h"
#include "DlgCameraVideoSetting.h"
#include "DlgCameraAudioSetting.h"
#include "ResourceWBMedia.h"
//#include "DlgShowConnectInfo.h"
#include "../WBData.h"
#include "../WB/whiteBoard/whiteBoard.h"
#include "WBWindows.h"
#include ".\common\WBMedia\Markup.h"
#include "ResData.h"
#include "CameraSettingIni.h"
// CameraDlg 对话框

IMPLEMENT_DYNAMIC(CDlgCamera, CDialogEx)
CDlgCamera::CDlgCamera(CWnd* pParent /*=NULL*/, BOOL bActiveCreated/* = TRUE*/,BOOL bLocal /*= FALSE*/)
	: CDialogEx(IDD_DIALOG_CAMERA_MAIN, pParent)
	, m_pPlayChan(NULL)
	, m_pCapChanDev(NULL)
	, m_bActiveCreated(bActiveCreated)
	, m_pAVCamera(NULL)
	, m_pCameraOwer(NULL)
	, m_bIsCameraOpen(FALSE)
	,m_dlgVideoSetting(NULL)
	,m_dlgAudioSetting(NULL)
	,m_bIsTouchDown(false)
	,m_pPlayChanCallbackCarema(NULL)
	,m_pVideoCapHandleCarema(NULL)
	,m_channelIDCarema(0)
	,m_bIsDrop(bLocal)
{
	//if (!bActiveCreated && pCamera)
	//{
	//	//待完整拷贝
	//	m_CameraReceiveData.nAudioID = pCamera->nAudioID;
	//	m_CameraReceiveData.nVideoID = pCamera->nVideoID;
	//}

	if (bLocal && g_pWhiteBoardView)
	{
		m_channelIDCarema = g_pWhiteBoardView->GetchannelID();
	}
	m_AudioSetBmp.LoadBitmap(IDB_OBMENU_AUDIOSET); //音频设置
	m_VideoSetBmp.LoadBitmap(IDB_OBMENU_VIDEOSET);//视频设置
	menu.CreatePopupMenu();
	if (!bActiveCreated)
	{
		//menu.AppendMenu(MF_STRING, IDC_UCOMMAND_CAMERA_SETTING_AUDIO, _T("音频设置")); 2012-3-14目前情况下，暂屏蔽被动方设置视频流的音频编码
	}
	else
	{
		CString strVideo;
		strVideo = ReadConfigText(_T("WhiteBoardMenuListText"), _T("camera_video"));
		if (strVideo.IsEmpty())
		{
			strVideo = _T("视频设置");
		}
		CString strAudio;
		strAudio = ReadConfigText(_T("WhiteBoardMenuListText"), _T("camera_audio"));
		if (strAudio.IsEmpty())
		{
			strAudio = _T("音频设置");
		}
		menu.AppendMenu(MF_STRING, IDC_UCOMMAND_CAMERA_SETTING_VIDEO, strVideo);
		menu.AppendMenu(MF_STRING, IDC_UCOMMAND_CAMERA_SETTING_AUDIO, strAudio);
	}
	menu.SetMenuItemBitmaps(IDC_UCOMMAND_CAMERA_SETTING_AUDIO, MF_BYCOMMAND, &m_AudioSetBmp,NULL);//音频设置
	menu.SetMenuItemBitmaps(IDC_UCOMMAND_CAMERA_SETTING_VIDEO, MF_BYCOMMAND, &m_VideoSetBmp,NULL);//视频设置
	//menu.AppendMenu(MF_STRING, IDC_UCOMMAND_GETLOCAL_CAMERA_SETTING, _T("获取视频连接信息"));
	//if (!m_bActiveCreated)
	//{
	//	menu.AppendMenu(MF_STRING, IDC_UCOMMAND_SETREMOTE_CAMERA_SETTING, _T("设置连接共享视频的连接参数"));
	//}
}

CDlgCamera::~CDlgCamera()
{
	if (m_pAVCamera)
	{
		delete m_pAVCamera;
		m_pAVCamera = NULL;
	}
	if (m_dlgVideoSetting != NULL)
	{
		delete m_dlgVideoSetting;
		m_dlgVideoSetting = NULL;
	}
	if (m_dlgAudioSetting != NULL)
	{
		delete m_dlgAudioSetting;
		m_dlgAudioSetting= NULL;
	}
	if (m_pPlayChanCallbackCarema != NULL)
	{
		delete m_pPlayChanCallbackCarema;
		m_pPlayChanCallbackCarema = NULL;
	}
	if (m_pVideoCapHandleCarema != NULL)
	{
		delete m_pVideoCapHandleCarema;
		m_pVideoCapHandleCarema = NULL;
	}
}

void CDlgCamera::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgCamera, CDialogEx)
	ON_MESSAGE(UWM_START_CONNECTED, OnStartCamera)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(IDC_UCOMMAND_CAMERA_SETTING_VIDEO, &CDlgCamera::OnVideoSetting)
	ON_COMMAND(IDC_UCOMMAND_CAMERA_SETTING_AUDIO, &CDlgCamera::OnAudioSetting)
	//ON_COMMAND(IDC_UCOMMAND_GETLOCAL_CAMERA_SETTING, &CDlgCamera::OnGetLocalVideoSetting)
	//ON_COMMAND(IDC_UCOMMAND_SETREMOTE_CAMERA_SETTING, &CDlgCamera::OnSetRemoteVideoSetting)
	ON_MESSAGE(WM_CAREMA_MOVE_TOUCH, &CDlgCamera::OnCaremaMoveTouch)
END_MESSAGE_MAP()
    

// CameraDlg 消息处理程序


void CDlgCamera::StartCamera()
{
	if(m_bActiveCreated)
	{
		CRect rct;
		GetClientRect(rct);
		ASSERT(m_pCapChanDev);
		if (0!=m_pCapChanDev->Open(GetSafeHwnd(),0))
		{
			m_pCapChanDev->Close();
			delete m_pCapChanDev;
			m_pCapChanDev = NULL;
			m_bIsCameraOpen = FALSE;
		}
		else
		{
			if (m_bIsDrop)
			{
				m_pAVCamera->m_nDevID = m_channelIDCarema;
				m_bIsDrop = false;
			}
			m_pCapChanDev->OpenVideoDev(m_pAVCamera->m_nDevID,
				/*m_pAVCamera->m_nWidth*/rct.Width(),
				/*m_pAVCamera->m_nHeight*/rct.Height(),
				m_pAVCamera->m_nFrameRate,
				m_pAVCamera->m_nBitrate,
				VIDEC_NETWORK_SATELLITE,
				m_pAVCamera->m_VideoCodecType,
				0,0);
			m_bIsCameraOpen = TRUE;
			
			//m_pCapChanDev->OpenAudio(0,m_pAVCamera->m_AudioCodecID);
			if (g_bIsConferenceRecord)
			{
				m_pCapChanDev->SetForceEncodeAudio(1);
				m_pCapChanDev->SetForceEncodeVideo(1);
			}
			
			if (!m_pCapChanDev->IsOSDEnable())
			{
 				CString strPath = GetCurrentPath();
				strPath	+= _T("\\U7Config.ini");
 				TCHAR strValue[4096] = _T("");
				CString strDefPath = GetCurrentPath(); 
				strDefPath	+= _T("\\U7DefConfig.ini");
				int nValue = GetPrivateProfileInt(_T("EnabledOSD"),_T("bEnabled"),1,strDefPath);
				if (nValue == 0)
				{
					return;
				}
				nValue = GetPrivateProfileInt(_T("AVSet_Comm"),_T("EnableOSD"),0,strPath);
// 				if (nValue == 0)
// 				{
// 					return;
// 				}

				CString strDevID;
				strDevID.Format(_T("AVSet_%d"),m_pAVCamera->m_nDevID);
 				::GetPrivateProfileString(strDevID,_T("CardName"),NULL,strValue,4096,strPath);

 				CString cstrValue = strValue;

				if (cstrValue == _T(""))
				{
					CString strHD = _T("");
					VIDEC_DEV_TYPE type = VIDEC_DEV_SD;
					IMXEC_CapChanDev::GetDevType(0,type);
					if (type == VIDEC_DEV_SD)
					{
						strHD = _T("");
					}
					else
						strHD = _T("高清");
					cstrValue.Format(_T("%s#%s%d"),g_strLocalUserName,strHD,1);
				}
				
 				std::string strText = CW2A(cstrValue);
				int nPosition = GetPrivateProfileInt(_T("AVSet_Comm"),_T("OSDAlign"),0,strPath);
				unsigned long textcolor = GetPrivateProfileInt(_T("AVSet_Comm"),_T("OSDTextColor"),RGB(0,0,255),strPath);
				unsigned long textbkcolor = GetPrivateProfileInt(_T("AVSet_Comm"),_T("OSDBkColor"),RGB(0xFA,0xE6,0xE6),strPath);
				unsigned long fontsize = GetPrivateProfileInt(_T("AVSet_Comm"),_T("OSDlfHeight"),24,strPath);
				HDC hDC = ::GetDC(NULL);
				int logPixelsY = ::GetDeviceCaps(hDC, LOGPIXELSY);
				LOGFONTA logFontOSD;
				logFontOSD.lfHeight = -MulDiv(fontsize, logPixelsY, 72);
				logFontOSD.lfWidth = GetPrivateProfileInt(_T("AVSet_Comm"),_T("OSDlfWidth"),0,strPath);
				logFontOSD.lfEscapement = GetPrivateProfileInt(_T("AVSet_Comm"),_T("OSDlfEscapement"),0,strPath);
				logFontOSD.lfOrientation = GetPrivateProfileInt(_T("AVSet_Comm"),_T("OSDlfOrientation"),0,strPath);
				logFontOSD.lfWeight = GetPrivateProfileInt(_T("AVSet_Comm"),_T("OSDlfWeight"),0,strPath);
				logFontOSD.lfItalic			= GetPrivateProfileInt(_T("AVSet_Comm"), _T("OSDlfItalic"), 0,strPath);
				logFontOSD.lfUnderline		= GetPrivateProfileInt(_T("AVSet_Comm"), _T("OSDlfUnderline"), 0,strPath);
				logFontOSD.lfStrikeOut		= GetPrivateProfileInt(_T("AVSet_Comm"), _T("OSDlfStrikeOut"), 0,strPath);
				logFontOSD.lfCharSet			= GetPrivateProfileInt(_T("AVSet_Comm"), _T("OSDlfCharSet"), 0,strPath);
				logFontOSD.lfOutPrecision		= GetPrivateProfileInt(_T("AVSet_Comm"), _T("OSDlfOutPrecision"), 0,strPath);
				logFontOSD.lfClipPrecision	= GetPrivateProfileInt(_T("AVSet_Comm"), _T("OSDlfClipPrecision"), 0,strPath);
				logFontOSD.lfQuality			= GetPrivateProfileInt(_T("AVSet_Comm"), _T("OSDlfQuality"), 0,strPath);
				logFontOSD.lfPitchAndFamily	= GetPrivateProfileInt(_T("AVSet_Comm"), _T("OSDlfPitchAndFamily"), 0,strPath);
				TCHAR sValue[4096] = _T("");
				::GetPrivateProfileString(_T("AVSet_Comm"), _T("OSDlfFaceName"), _T("Tahoma"),sValue,4096,strPath);
				CString strFaceName				= sValue;
				strcpy_s(logFontOSD.lfFaceName, 32, CW2A(strFaceName));
				::ReleaseDC(NULL,hDC);
				m_pCapChanDev->SetOSDFont(logFontOSD,textcolor,textbkcolor);
 				m_pCapChanDev->SetOSDText(strText.c_str());

				m_pCapChanDev->EnableOSD(true);
			}

		}

	}
	else
	{
		ASSERT(m_pPlayChan);
		if (g_pWhiteBoardView && g_pWhiteBoardView->GetIsMedialubo())
		{
			if (0!=m_pPlayChan->Open(m_hWnd))
			{
				m_pPlayChan->Close();
				delete m_pPlayChan;
				m_pPlayChan=NULL;
				OutputDebugString(_T("连接失败"));
				m_bIsCameraOpen = FALSE;
			}
			else
			{
				//设置帧控制模式
				m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
				//启动音频，并设置是否接受
				m_pPlayChan->StartAudio();
				
				//启动视频，并设置是否接受
				m_pPlayChan->StartVideo();

				m_pPlayChan->EnableVideo(1);
				m_pPlayChan->EnableAudio(1);
				
				
				CRect rc;
				GetClientRect(rc);
				m_pPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
				m_bIsCameraOpen = TRUE;
			}
		}
		else
		{
			if ( 0 != m_pPlayChan->Open(m_hWnd,
				m_CameraReceiveData.nAudioID,
				m_CameraReceiveData.nVideoID, 
				m_CameraReceiveData.strNodeID.c_str(),
				m_CameraReceiveData.strNATIP.c_str(),
				m_CameraReceiveData.nNATPort,
				m_CameraReceiveData.strLocalIP.c_str(),
				m_CameraReceiveData.nLocalPort,
				m_CameraReceiveData.strMCUID.c_str(),
				m_CameraReceiveData.strMCUIP.c_str(),
				m_CameraReceiveData.nServerPort,0) )
			{
				m_pPlayChan->Close();
				delete m_pPlayChan;
				m_pPlayChan=NULL;
				OutputDebugString(_T("连接失败"));
				m_bIsCameraOpen = FALSE;
			}
			else
			{
				//设置帧控制模式
				m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
				//启动音频，并设置是否接受
				m_pPlayChan->StartAudio();
				
				//启动视频，并设置是否接受
				m_pPlayChan->StartVideo();
				if (g_bIsConferenceRecord)
				{
					m_pPlayChan->EnableVideo(1);
					m_pPlayChan->EnableAudio(1);
				}
				
				CRect rc;
				GetClientRect(rc);
				m_pPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
				m_bIsCameraOpen = TRUE;
			}
		}

	}
}

int CDlgCamera::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(m_bActiveCreated)
	{
		//CString strCameraAudioFilePath = g_strAvconIniPath + WB_CARMERA_AUDIAO_FILENAME;
		//CString strCameraVideoFilePath = g_strAvconIniPath + WB_CARMERA_VIDEO_FILENAME;
		//CMarkup xml;

		if (!m_pCapChanDev)
		{
			if ( NULL == m_pVideoCapHandleCarema )
			{
				m_pVideoCapHandleCarema = new CCapChanPlayHandle_Empty();
			}
			m_pCapChanDev = IMXEC_CapChanDev::Create(*m_pVideoCapHandleCarema);

			if (m_pCapChanDev)
			{

				if (0!=m_pCapChanDev->Open(m_hWnd,0))
				{
					m_pCapChanDev->Close();
					delete m_pCapChanDev;
					m_pCapChanDev = NULL;
				}
				
				if (m_pCapChanDev && (!m_pAVCamera))
				{
					CRect rc;
					GetClientRect(&rc);
					//初始化数据
					m_pAVCamera = new CameraSet;
 				if (PathFileExists(g_strAvconIniPath))
 				{
 					CString strSetting = _T("CameraVideoSettingNum");
 					CString strTemp = _T("");
 					CString strNumber = _T("");
 					strTemp = _T("VideoCodec");
 					strNumber = ReadConfigTextVideoIni(strSetting,strTemp);
					if (strNumber.IsEmpty())
					{
						m_pAVCamera->m_VideoCodecType = VIDEC_CODEC_H264_SVC;
					}
					else
					{
						m_pAVCamera->m_VideoCodecType = (VIDEC_CODEC_TYPE)_ttoi(strNumber);
					}


					strTemp = _T("VideoFrameRate");
					strNumber = ReadConfigTextVideoIni(strSetting,strTemp);
					if (strNumber.IsEmpty())
					{
						m_pAVCamera->m_nFrameRate = 15;
					}
					else
					{
						m_pAVCamera->m_nFrameRate = (int)_ttoi(strNumber);
					}


					strTemp = _T("VideoBitrate");
					strNumber = ReadConfigTextVideoIni(strSetting,strTemp);
					if (strNumber.IsEmpty())
					{
						m_pAVCamera->m_nBitrate = 512;
					}
					else
					{
						m_pAVCamera->m_nBitrate = (int)_ttoi(strNumber);
					}

					strTemp = _T("VideoDevID");
					strNumber = ReadConfigTextVideoIni(strSetting,strTemp);
					if (strNumber.IsEmpty())
					{
						CString strPath = GetCurrentPath() + _T("\\U7Config.ini");
						CString strKey ;
						int DevID = -1;
						int nCount = 0;
						/*while(1)*/
// 						{							
// 							strKey.Format(_T("Dev_%d"),nCount);							
// 							DevID = GetPrivateProfileInt(strKey,_T("Published"),-1,strPath);
// 							if ( DevID != -1 )
// 							{
// 								break;
// 							}
// 							nCount++;
// 							if (nCount > 8)
// 							{
// 								break;
// 							}
// 						}
						strKey.Format(_T("Channel_%d"),nCount);	
						DevID = GetPrivateProfileInt(strKey,_T("DevID"),-1,strPath);
						
						if (DevID == -1)
						{
							m_pAVCamera->m_nDevID = 0;
						}
						else
						{
							m_pAVCamera->m_nDevID = DevID;
						}
						
					}
					else
					{
						m_pAVCamera->m_nDevID = _ttoi(strNumber);
					}

					CString strtance = _T("CameraAudioSettingINI");
					strTemp = _T("AudioCodecID");
					strNumber = ReadConfigTextVideoIni(strtance,strTemp);
					if (strNumber.IsEmpty())
					{
						m_pAVCamera->m_AudioCodecID = X_AUDIO_CODEC_G711U;
					}
					else
					{
						m_pAVCamera->m_AudioCodecID = (X_AUDIO_CODEC_ID)_ttoi(strNumber);
					}
 					
 				}
				}

			/*	xml.Load(strCameraVideoFilePath);
				while( xml.FindChildElem("CameraVideoSetting"))
				{
					xml.IntoElem();
					xml.FindChildElem("VideoDevID");
					m_pAVCamera->m_nDevID = atoi(xml.GetChildData());

 					xml.FindChildElem("VideoCodec");
 					m_pAVCamera->m_VideoCodecType = (VIDEC_CODEC_TYPE)atoi(xml.GetChildData());					

					xml.FindChildElem("VideoFrameRate");
					m_pAVCamera->m_nFrameRate = atoi(xml.GetChildData());

					xml.FindChildElem("VideoBitrate");
					m_pAVCamera->m_nBitrate = atoi(xml.GetChildData());
					xml.OutOfElem();
				}
				
				xml.Load(strCameraAudioFilePath);
				if (xml.FindChildElem("CameraAudioSetting"))
				{
					xml.IntoElem();
					xml.FindChildElem("AudioCodecID");
					m_pAVCamera->m_AudioCodecID = (X_AUDIO_CODEC_ID)atoi(xml.GetChildData());
				}*/
			}
		}
	}
	else
	{
		if ( NULL != m_pVideoCapHandleCarema )
		{
			delete m_pVideoCapHandleCarema;
			m_pVideoCapHandleCarema = NULL;
		}
		m_pPlayChanCallbackCarema = new IMXEC_PlayChanCallback_Empty();
		m_pPlayChan = IMXEC_PlayChan::Create(*m_pPlayChanCallbackCarema);
	//	m_pPlayChan = IMXEC_PlayChan::Create(*this);
	}

	if (m_bActiveCreated)
	{
		StartCamera();//如果是主动创建，则自动连接默认设备
	}
	return 0;
}

void CDlgCamera::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (NULL != m_pCapChanDev)
	{
		m_pCapChanDev->Close();
		delete m_pCapChanDev;
		m_pCapChanDev = NULL;
	}
	if (NULL != m_pPlayChan)
	{
		m_pPlayChan->Close();
		delete m_pPlayChan;
		m_pPlayChan = NULL;
	}
	//
	//CWBCameraNetMng::DisConnect();
	//NETEC_Node::Stop();
	//NETEC_Core::Stop();

	//if (m_pCameraShowTopMost)
	//{
	//	delete m_pCameraShowTopMost;
	//	m_pCameraShowTopMost = NULL;
	//}

	CDialog::PostNcDestroy();
	delete this;
}

void CDlgCamera::OnSize( UINT nType, int cx, int cy )
{
	if((0==cx) || (0==cy))
		return;
	//CRect rctCamera;
	//GetClientRect(rctCamera);
	//if (m_pCameraShowTopMost == NULL)
	//{
	//	m_pCameraShowTopMost = new CWBCameraGestureWindows(this);
	//	m_pCameraShowTopMost->CreateWBWindows(NULL, _T(""), 0, rctCamera, IDC_WINDOWS_MEDIASHOW_TOPMOST, this);
	//	m_pCameraShowTopMost->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);	
	//}
	//if (m_pCameraShowTopMost)
	//{
	//	m_pCameraShowTopMost->MoveWindow(rctCamera.left, rctCamera.top, rctCamera.Width(), rctCamera.Height(), TRUE);
	//}

	if (m_bActiveCreated)
	{
		if (m_pCapChanDev)
		{
			CRect rect;
			GetClientRect(&rect);
			m_pCapChanDev->SetWindowPos(HWND_BOTTOM, 0, 0, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE);
		}
	}
	else
	{
		if (m_pPlayChan)
		{
			CRect rect;
			GetClientRect(&rect);
			m_pPlayChan->SetWindowPos(HWND_BOTTOM, 0, 0, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE);
		}
	}

	
	CDialogEx::OnSize(nType, cx, cy);
}

//监测网络连接状态
void CDlgCamera::OnConnectStatusChanged( BOOL bConnected )
{
	static BOOL bMark = FALSE;

	if (!bMark && bConnected)//之前没有连接上，则发送一次连接的消息
	{
		PostMessage(UWM_START_CONNECTED, 0, 0);
		bMark = TRUE;
	}
	else
	{
		bMark = FALSE;
	}
}


void CDlgCamera::StopCamera()
{
	if (m_pPlayChan)
	{
		m_pPlayChan->StopRecordToAVI();
	}

	if (m_pPlayChan!=NULL)
	{
		m_pPlayChan->Close();
	}

	if (m_pCapChanDev!=NULL)
	{
		m_pCapChanDev->Close();
	}
}

void CDlgCamera::OnRButtonDown( UINT nFlags, CPoint point )
{
	if (!g_bSupportU7OpenFile)
	{
		ClientToScreen(&point);
		menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	
	CDialog::OnRButtonDown(nFlags, point);
}

void CDlgCamera::OnVideoSetting()
{
    //修改为非模态主要因为模态对话框注销后，在进入会议没办法P屏wangxiaobin
	//CDlgCameraVideoSetting dlgVideoSetting(this,m_pAVCamera);
    //if (dlgVideoSetting.DoModal()==IDOK		
	BOOL bVideo = FALSE;
	if (m_dlgVideoSetting == NULL)
	{
		m_dlgVideoSetting	= new CDlgCameraVideoSetting(this,m_pAVCamera);
		if(m_dlgVideoSetting == NULL)
		{
			return;
		}
		bVideo = m_dlgVideoSetting->Create(IDD_DIALOG_CAMERA_VIDEO_SETTING,NULL);
	}
	else
	{
		if (!m_dlgVideoSetting->IsWindowVisible())
		{
			m_dlgVideoSetting->ShowWindow(SW_SHOW);
		}
		bVideo = TRUE;
	}
	

	if (bVideo)
	{
		if (!g_pWhiteBoardView)
		{
			return ;
		}
		if (m_dlgVideoSetting->IsVideoSetUpdated())//如果有更改
		{
			m_pAVCamera->m_nDevID = m_dlgVideoSetting->GetCameraPtr()->m_nDevID;
			m_pAVCamera->m_nFrameRate = m_dlgVideoSetting->GetCameraPtr()->m_nFrameRate;
			m_pAVCamera->m_nBitrate = m_dlgVideoSetting->GetCameraPtr()->m_nBitrate;
			m_pAVCamera->m_VideoCodecType = m_dlgVideoSetting->GetCameraPtr()->m_VideoCodecType;
			m_pAVCamera->m_nWidth = m_dlgVideoSetting->GetCameraPtr()->m_nWidth;
			m_pAVCamera->m_nHeight = m_dlgVideoSetting->GetCameraPtr()->m_nHeight;

			if (m_pCapChanDev!=NULL)
			{
				StartCamera();
			}
			//同步
		}
		//return   ;
	}
	
}

void CDlgCamera::OnAudioSetting()
{
	 //修改为非模态主要因为模态对话框注销后，在进入会议没办法P屏wangxiaobin
	//CDlgCameraAudioSetting dlgAudioSetting(this);
	//if (dlgAudioSetting.DoModal()==IDOK)
	BOOL bAudio = FALSE;
	if (m_dlgAudioSetting == NULL)
	{
		m_dlgAudioSetting = new CDlgCameraAudioSetting(this);
		bAudio = m_dlgAudioSetting->Create(IDD_DIALOG_CAMERA_AUDIO_SETTING,NULL);
	}
	else
	{
		m_dlgAudioSetting->ShowWindow(SW_SHOW);
		bAudio = TRUE;
	}
	if (bAudio)
	{
		if (!g_pWhiteBoardView || !m_pAVCamera || !m_dlgAudioSetting)
		{
			return ;
		}
		m_pAVCamera->m_AudioCodecID = m_dlgAudioSetting->GetCameraPtr()->m_AudioCodecID;
		if (m_dlgVideoSetting == NULL)
		{
			m_pAVCamera->m_nDevID = 0;
		}
		else
		{
			m_pAVCamera->m_nDevID = m_dlgVideoSetting->GetCameraPtr()->m_nDevID;
		}
		
		if (m_pCapChanDev!=NULL)
		{
			m_pCapChanDev->OpenAudio(m_pAVCamera->m_nDevID, m_pAVCamera->m_AudioCodecID);
		}
	}
}

LRESULT CDlgCamera::OnStartCamera( WPARAM wParam, LPARAM lParam )
{
	StartCamera();
	//发送连接信息同步
	WPARAM wPar = (WPARAM)m_pCameraOwer;
	::SendMessage(GetParent()->GetSafeHwnd(),WM_WBCAMERA_NET_CONNECT_SYNCHRONOUS, wPar, 0);
	return S_OK;
}

BOOL CDlgCamera::GetRemoteData( SCameraReceiveData &data )
{
	//首先更新
	m_CameraReceiveData.nAudioID = 0;//待补充
	m_CameraReceiveData.strNodeID = std::string(NETEC_Node::GetNodeID());
	m_CameraReceiveData.strNATIP = std::string(NETEC_Node::GetNATIP());
	m_CameraReceiveData.nNATPort = NETEC_Node::GetNATPort();
	m_CameraReceiveData.strLocalIP = std::string(NETEC_Node::GetLocalIP());
	m_CameraReceiveData.nLocalPort = NETEC_Node::GetLocalPort();
	m_CameraReceiveData.strMCUID = std::string(NETEC_Node::GetMCUID());
	m_CameraReceiveData.strMCUIP = std::string(NETEC_Node::GetMCUIP());
	m_CameraReceiveData.nServerPort = NETEC_Node::GetServerPort();
	data = m_CameraReceiveData;

	return TRUE;
}

BOOL CDlgCamera::GetLocalData( CameraSet &data )
{
	data = *m_pAVCamera;
	data.m_nAudioID=m_pCapChanDev->GetAudioID();
	data.m_nDevID=m_pCapChanDev->GetVideoID();
	return TRUE;
}


BOOL CDlgCamera::GetServerData( SCameraReceiveData &data )
{
	//都采用默认的参数信息，直接返回即可
	return TRUE;
}

void CDlgCamera::OnLButtonDown( UINT nFlags, CPoint point )
{
// 	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y)); 
// 	   my   code   end   
// 	CDialog::OnLButtonDown(nFlags,   point);

	if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH && !g_bIsSupportMTTouch )
	{
		return ;
	}

	if (DRAW_NORMAL == g_nDrawStatus)
	{
		g_pWBPage->SetObjTopLayer(m_pCameraOwer->GetLayerIndex());
	}  
	//将在鼠标在视频上的坐标转化为相对于白板视图的坐标，并将此动作交给白板试图处理

	m_bIsTouchDown = true;
	CRect rect;
	//dynamic_cast<CDlgCamera*>(GetParent())->GetWindowRect(&rect);
	this->GetWindowRect(&rect);
	g_pWhiteBoardView->ScreenToClient(&rect);
	point.x += rect.left;
	point.y += rect.top;
	nFlags |= MK_WB_ULBUTTON;
	SetCapture();
	g_pWhiteBoardView->OnLButtonDown(nFlags, point);
}

void CDlgCamera::OnMouseMove( UINT nFlags, CPoint point )
{
	//将在鼠标在摄像头上的坐标转化为相对于白板视图的坐标，并将此动作交给白板试图处理

	if (!m_bIsTouchDown)//没有按下不移动
		return;
	
	if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH && !g_bIsSupportMTTouch )
	{
		return ;
	}
	CRect rect;
	this->GetWindowRect(&rect);
	g_pWhiteBoardView->ScreenToClient(&rect);
	point.x += rect.left;
	point.y += rect.top;
	nFlags |= MK_WB_ULBUTTON;
	g_pWhiteBoardView->OnMouseMove(nFlags, point);
}

void CDlgCamera::OnLButtonUp( UINT nFlags, CPoint point )
{
	if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH  && !g_bIsSupportMTTouch )
	{
		return ;
	}
	m_bIsTouchDown = false;
	CRect rect;
	this->GetWindowRect(&rect);
	g_pWhiteBoardView->ScreenToClient(&rect);
	point.x += rect.left;
	point.y += rect.top;
	nFlags |= MK_WB_ULBUTTON;
	ReleaseCapture();
	g_pWhiteBoardView->OnLButtonUp(nFlags, point);

	//发送同步消息 
	/*XMemIni xmlObj;
	dynamic_cast<CWBMediaWindow*>(GetParent())->GetWBMediaOwner()->SetXmlValue(xmlObj);

	g_pWhiteBoardView->ChangeObj(xmlObj);*/
}

BOOL CDlgCamera::UpdateRemoteData( SCameraReceiveData *data )
{
	//m_CameraReceiveData = *data;//可以赋值吗？

	m_CameraReceiveData.nAudioID = data->nAudioID;
	m_CameraReceiveData.nLocalPort = data->nLocalPort;
	m_CameraReceiveData.nNATPort = data->nNATPort;
	m_CameraReceiveData.nServerPort = data->nServerPort;
	m_CameraReceiveData.nVideoID = data->nVideoID;
	m_CameraReceiveData.strLocalIP = data->strLocalIP;
	m_CameraReceiveData.strMCUID = data->strMCUID;
	m_CameraReceiveData.strMCUIP = data->strMCUIP;
	m_CameraReceiveData.strNATIP = data->strNATIP;
	m_CameraReceiveData.m_strUCCUerID = data->m_strUCCUerID;
	m_CameraReceiveData.strNodeID = data->strNodeID;
	return TRUE;
}

//void CDlgCamera::OnGetLocalVideoSetting()
//{
//	unsigned long nAudioID=0;
//	unsigned long nVideoID;
//
//	std::string sNodeID;
//	std::string sNNATP;
//	unsigned short nNATPort;
//	std::string sLocalIP;
//	unsigned short nLocalPort;
//	std::string sMCUID;
//	std::string sMCUIP;
//	unsigned short nServerPort;
//	CDlgShowConnectInfo dlg;
//	if (m_bActiveCreated)
//	{
//		nAudioID=m_pCapChanDev->GetAudioID();
//		nVideoID=m_pCapChanDev->GetVideoID();
//
//		sNodeID = std::string(NETEC_Node::GetNodeID());
//		sNNATP = std::string(NETEC_Node::GetNATIP());
//		nNATPort = NETEC_Node::GetNATPort();
//		sLocalIP = std::string(NETEC_Node::GetLocalIP());
//		nLocalPort = NETEC_Node::GetLocalPort();
//		sMCUID = std::string(NETEC_Node::GetMCUID());
//		sMCUIP = std::string(NETEC_Node::GetMCUIP());
//		nServerPort = NETEC_Node::GetServerPort();
//	}
//	else
//	{
//		nVideoID = m_CameraReceiveData.nVideoID;
//		sNodeID = m_CameraReceiveData.strNodeID;
//		sNNATP = m_CameraReceiveData.strNATIP;
//		nNATPort = m_CameraReceiveData.nNATPort;
//		sLocalIP = m_CameraReceiveData.strLocalIP;
//		nLocalPort = m_CameraReceiveData.nLocalPort;
//		sMCUID = m_CameraReceiveData.strMCUID;
//		sMCUIP = m_CameraReceiveData.strMCUIP;
//		nServerPort = m_CameraReceiveData.nServerPort;
//	}
//	dlg.SetCreatorLable(m_bActiveCreated);
//
//	//dlg.SetWindowText(_T("获取本地共享的视频设置信息"));
//	dlg.nAudioID = 0;
//	dlg.nVideoID = nVideoID;
//	dlg.sNodeID = sNodeID;
//
//	dlg.sNATIP = sNNATP;
//	dlg.nNATPort = nNATPort;
//
//	dlg.sLocalIP = sLocalIP;
//	dlg.nLocalPort = nLocalPort;
//
//	dlg.sMCUID = sMCUID;
//	dlg.sMCUIP = sMCUIP;
//	dlg.nServerPort = nServerPort;
//
//	dlg.DoModal();
//}

//void CDlgCamera::OnSetRemoteVideoSetting()
//{
//	if (m_bActiveCreated)
//	{
//		return;
//	}
//	//unsigned long nAudioID=m_pCapChanDev->GetAudioID();
//	//unsigned long nVideoID=m_pCapChanDev->GetVideoID();
//
//	//std::string sNodeID = std::string(NETEC_Node::GetNodeID());
//	//std::string sNNATP = std::string(NETEC_Node::GetNATIP());
//	//unsigned short nNATPort = NETEC_Node::GetNATPort();
//	//std::string sLocalIP = std::string(NETEC_Node::GetLocalIP());
//	//unsigned short nLocalPort = NETEC_Node::GetLocalPort();
//	//std::string sMCUID = std::string(NETEC_Node::GetMCUID());
//	//std::string sMCUIP = std::string(NETEC_Node::GetMCUIP());
//	//unsigned short nServerPort = NETEC_Node::GetServerPort();
//
//	CDlgShowConnectInfo dlg;
//	//dlg.SetWindowText(_T("设置待连接的远程视频设置信息"));
//	dlg.nAudioID = m_CameraReceiveData.nAudioID;
//	dlg.nVideoID = m_CameraReceiveData.nVideoID;
//	dlg.sNodeID = m_CameraReceiveData.strNodeID;
//
//	dlg.sNATIP = m_CameraReceiveData.strNATIP;
//	dlg.nNATPort = m_CameraReceiveData.nNATPort;
//
//	dlg.sLocalIP = m_CameraReceiveData.strLocalIP;
//	dlg.nLocalPort = m_CameraReceiveData.nLocalPort;
//
//	dlg.sMCUID = m_CameraReceiveData.strMCUID;
//	dlg.sMCUIP = m_CameraReceiveData.strLocalIP;
//	dlg.nServerPort = m_CameraReceiveData.nServerPort;
//
//	if (IDOK == dlg.DoModal())
//	{
//
//		if (!m_pPlayChan)
//		{
//			return;
//		}
//		m_pPlayChan->Close();
//
//		if (0!=m_pPlayChan->Open(GetSafeHwnd(), dlg.nAudioID, dlg.nVideoID, dlg.sNodeID.c_str(),
//			dlg.sNATIP.c_str(), dlg.nNATPort, dlg.sLocalIP.c_str(),
//			dlg.nLocalPort, dlg.sMCUID.c_str(),
//			dlg.sMCUIP.c_str(),dlg.nServerPort,0))
//		{
//			m_pPlayChan->Close();
//			//delete m_pPlayChan;
//			//m_pPlayChan=NULL;
//		}
//		else
//		{
//			//设置桢控制模式
//			m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
//			//启动音频，并设置是否接受
//			m_pPlayChan->StartAudio();
//			m_pPlayChan->EnableAudio(1);
//
//			//m_pPlayChan->SetAudioCast(true);
//			//m_pPlayChan->SetVideoCast(true);
//			//启动视频，并设置是否接受
//			m_pPlayChan->EnableVideo(1);
//			m_pPlayChan->StartVideo();
//			//m_pPlayChan->StartMJPG(FRAME_TYPE_CONTROL_MODE_MAIN);
//
//			//m_pPlayChan->StartRecordToAVI("d:\\TestP.avi");
//			CRect rc;
//			GetClientRect(rc);
//			m_pPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
//		}
//	}
//
//}

//void CDlgCamera::OnGestureCombineHandle( const GestureCombinePar &gesPar )
//{
//	//暂时仅处理平移操作
//	int dTransX = gesPar.xtrans;
//	int dTransY = gesPar.ytrans;
//
//	if (dTransX != 0 && dTransY != 0)
//	{
//		//rct.OffsetRect(dTransX, dTransY);
//		POINT  pt;
//		pt.x = dTransX;
//		pt.y = dTransY;
//		GetCameraOwner()->MoveOffset(pt);
//	}
//}
//
//void CDlgCamera::OnCallbackGesture( const unsigned int &nMessage, const unsigned int &nAppendMessage )
//{
//	WORD lWord = LOWORD(nMessage);
//	if (TGT_ACLICK == lWord)
//	{
//		WORD hWord = HIWORD(nMessage);
//		if ((TGC_TAP == hWord) || (TGC_CLICK == hWord))
//		{
//			BOOL bShow = GetCameraOwner()->GetSelectFrameShow();
//			bShow = !bShow;
//			GetCameraOwner()->SetSelectFrameShow(bShow);
//			GetCameraOwner()->GetFather()->AddSelectFrame(GetCameraOwner()->GetLayerIndex(), bShow);
//			g_pWhiteBoardView->SetFocus();
//			if (g_pWhiteBoardView == NULL)
//			{
//				return;
//			}
//			g_pWhiteBoardView->Invalidate();
//		}
//		else if (TGC_RCLICK == hWord)
//		{
//			//如有，则弹出右键菜单
//		}
//	}
//}
//
//int CDlgCamera::InRegion( float x, float y )
//{
//	return TRUE;
//}

void CDlgCamera::OnCancel()
{
	return;
}

void CDlgCamera::OnOK()
{
	return;
}

int CDlgCamera::PlayVideoDataCarema( const void*pData,int nLen )
{
	if (m_pPlayChan == NULL)
	{
		return -1;
	}

	int i = m_pPlayChan->PlayVideoData(pData,nLen);
	return i;
}

int CDlgCamera::PlayAudioDataCarema( const void*pData,int nLen )
{
	if (m_pPlayChan == NULL)
	{
		return -1;
	}
	int i = m_pPlayChan->PlayAudioData(pData,nLen);
	return i;
}

LRESULT CDlgCamera::OnCaremaMoveTouch( WPARAM wParam, LPARAM lParam )
{
	if (GetCameraOwner() == NULL)
	{
		return 0;
	}
	CRect rect = GetCameraOwner()->GetMoveRect();
	int nOffx = 0;
	int nOffy = 0;
	nOffx = (int)wParam;
	nOffy = (int)lParam;
	if (nOffx == 0 && nOffy == 0)
	{
		return 0;
	}
	rect.OffsetRect(nOffx, nOffy);
	MoveWindow(rect);
	GetCameraOwner()->SetMoveRect(rect);
	return 1;
}

/*-----------------------------------------------------------------------------------
Begin:  definition of the class of CWBCameraGestureWindows
------------------------------------------------------------------------------------*/
//IMPLEMENT_DYNAMIC(CWBCameraGestureWindows, CWBWindow)
//	CWBCameraGestureWindows::CWBCameraGestureWindows(CMTObject * pParent/* = NULL*/)
//	: CWBWindow()
//	, m_pParent(pParent)
//	, m_uNumContacts(0)
//{
//}
//
//CWBCameraGestureWindows::~CWBCameraGestureWindows()
//{
//}
//
//BEGIN_MESSAGE_MAP(CWBCameraGestureWindows, CWBWindow)
//	//ON_MESSAGE(WM_TOUCH, OnTouch)
//END_MESSAGE_MAP()
//
//
//BOOL CWBCameraGestureWindows::CreateWBWindows( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect,UINT nID, CWnd *pParent /*= NULL*/ )
//{
//	BOOL result = CWBWindow::CreateWBWindows(lpszClassName, lpszWindowName, dwStyle, rect, nID, pParent);
//	//result = RegisterTouchWindow(TRUE);
//	//result = CEncapsulateTouchAPI::U_RegisterTouchWindow(GetSafeHwnd());
//	//if (!m_pParent)
//	//{
//	//	SetProcessGesture(/*MT_ALL_BASIC_GESTURE|*/MT_TOUCH_EVENT_TAP);
//	//}
//
//	DWORD_PTR dwHwndTabletProperty = 
//		TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
//		TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
//		TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
//		TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)
//
//	ATOM atom = ::GlobalAddAtom(MICROSOFT_TABLETPENSERVICE_PROPERTY);  
//	SetProp(GetSafeHwnd(), MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
//	GlobalDeleteAtom(atom);
//
//	return result;
//}
////LRESULT CWBCameraGestureWindows::OnTouch( WPARAM wParam, LPARAM lParam )
////{
////	return S_OK;
////	int iNumContacts = LOWORD(wParam);
////	HTOUCHINPUT hInput = (HTOUCHINPUT)lParam;
////
////	PTOUCHINPUT pInputs = new (std::nothrow) TOUCHINPUT[iNumContacts];
////
////	if( pInputs != NULL )
////	{
////		//if (!GetTouchInputInfo(hInput, iNumContacts, pInputs, sizeof(TOUCHINPUT)))
////		if( !CEncapsulateTouchAPI::U_GetTouchInputInfo((HANDLE)hInput, iNumContacts, pInputs, sizeof(TOUCHINPUT)) )
////		{
////			return S_FALSE;
////		}
////		//获取每个触摸点信息，然后分发到各个处理容器中。
////		for(int i = 0; i < iNumContacts; i++)
////		{
////			pInputs[i].x = TOUCH_COORD_TO_PIXEL(pInputs[i].x);
////			pInputs[i].y = TOUCH_COORD_TO_PIXEL(pInputs[i].y);
////
////			if ((pInputs[i].dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)// 触摸按下事件
////			{
////				ProcessTouch(&pInputs[i]);
////			}               
////			else if ((pInputs[i].dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)// 触摸移动事件
////			{
////				ProcessTouch(&pInputs[i]);
////			}
////			else if ((pInputs[i].dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)// 触摸移动事件
////			{
////				ProcessTouch(&pInputs[i]);
////			}
////		}
////
////		//if (!CloseTouchInputHandle(hInput))
////		if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(hInput))
////		{
////			OutputDebugString(_T("warning, the touch input handle was not closed successfully!"));
////		}
////	}
////	delete pInputs;
////	return S_OK;
////}
//
//void CWBCameraGestureWindows::ProcessTouch( const TOUCHINPUT* inData )
//{
//	DWORD dwCursorID = inData->dwID;
//	DWORD dwEvent = inData->dwFlags;
//
//	//触摸点增一
//	if((dwEvent & TOUCHEVENTF_DOWN) && (dwCursorID != MOUSE_CURSOR_ID))
//	{
//		m_uNumContacts++;
//	}
//
//	//触摸时没有触摸点时直接退出
//	if((m_uNumContacts == 0) && (dwCursorID != MOUSE_CURSOR_ID))
//	{
//		return;
//	}
//	//有触摸点时处理鼠标时直接退出
//	else if((m_uNumContacts > 0) && (dwCursorID == MOUSE_CURSOR_ID))
//	{
//		return;
//	}
//
//	//触摸点减一
//	if((dwEvent & TOUCHEVENTF_UP) && (dwCursorID != MOUSE_CURSOR_ID))
//	{
//		m_uNumContacts--;
//	}
//
//	if (m_pParent)
//	{
//		///老的手势库
//		//m_pParent->ProcessTouchInput(inData);
//	}
//	else
//	{
//		///老的手势库
//		//ProcessTouchInput(inData);
//	}
//}
//
//LRESULT CWBCameraGestureWindows::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	switch (message)
//	{
//	//case WM_TOUCH:
//	//	{
//	//		int iNumContacts = LOWORD(wParam);
//	//		HTOUCHINPUT hInput = (HTOUCHINPUT)lParam;
//	//		PTOUCHINPUT pInputs = new (std::nothrow) TOUCHINPUT[iNumContacts];
//	//		if( pInputs != NULL )
//	//		{
//	//			//if (!GetTouchInputInfo(hInput, iNumContacts, pInputs, sizeof(TOUCHINPUT)))
//	//			if( !CEncapsulateTouchAPI::U_GetTouchInputInfo((HANDLE)hInput, iNumContacts, pInputs, sizeof(TOUCHINPUT)) )
//	//			{
//	//				return S_FALSE;
//	//			}
//	//			//获取每个触摸点信息，然后分发到各个处理容器中。
//	//			for(int i = 0; i < iNumContacts; i++)
//	//			{
//	//				pInputs[i].x = TOUCH_COORD_TO_PIXEL(pInputs[i].x);
//	//				pInputs[i].y = TOUCH_COORD_TO_PIXEL(pInputs[i].y);
//
//	//				if ((pInputs[i].dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)// 触摸按下事件
//	//				{
//	//					ProcessTouch(&pInputs[i]);
//	//				}               
//	//				else if ((pInputs[i].dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)// 触摸移动事件
//	//				{
//	//					ProcessTouch(&pInputs[i]);
//	//				}
//	//				else if ((pInputs[i].dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)// 触摸移动事件
//	//				{
//	//					ProcessTouch(&pInputs[i]);
//	//				}
//	//			}
//
//	//			//if (!CloseTouchInputHandle(hInput))
//	//			if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(hInput))
//	//			{
//	//				OutputDebugString(_T("warning, the touch input handle was not closed successfully!"));
//	//			}
//	//		}
//	//		delete pInputs;
//	//	}
//	//	break;
//	case WM_LBUTTONDOWN:
//		{
//			if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH )
//			{
//				return 0;
//			}
// 			//当鼠标左键按下时，让视频对象放在最上层，以便最优先被选取
//			if (DRAW_NORMAL == g_nDrawStatus)
//			{
//				g_pWBPage->SetObjTopLayer(dynamic_cast<CDlgCamera*>(GetParent())->GetCameraOwner()->GetLayerIndex());
//			}
//			//将在鼠标在视频上的坐标转化为相对于白板视图的坐标，并将此动作交给白板试图处理
//			CPoint point(LOWORD(lParam), HIWORD(lParam));
//			CRect rect;
//			dynamic_cast<CDlgCamera*>(GetParent())->GetWindowRect(&rect);
//			g_pWhiteBoardView->ScreenToClient(&rect);
//			point.x += rect.left;
//			point.y += rect.top;
//			wParam |= MK_WB_ULBUTTON;
//			g_pWhiteBoardView->OnLButtonDown((UINT)wParam, point);
//		}      
//		break;
//	case WM_MOUSEMOVE:
//		{
//
//			if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH )
//			{
//				return 0;
//			}
//			//将在鼠标在视频上的坐标转化为相对于白板视图的坐标，并将此动作交给白板试图处理
//			CPoint point(LOWORD(lParam), HIWORD(lParam));
//			CRect rect;
//			dynamic_cast<CDlgCamera*>(GetParent())->GetWindowRect(&rect);
//			g_pWhiteBoardView->ScreenToClient(&rect);
//			point.x += rect.left;
//			point.y += rect.top;
//			wParam |= MK_WB_ULBUTTON;
//			g_pWhiteBoardView->OnMouseMove((UINT)wParam, point);
//		}
//		break;
//	case WM_LBUTTONUP:
//		{
//
//			if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH )
//			{
//				return 0;
//			}
//			//将在鼠标在视频上的坐标转化为相对于白板视图的坐标，并将此动作交给白板试图处理
//			CPoint point(LOWORD(lParam), HIWORD(lParam));
//			CRect rect;
//			dynamic_cast<CDlgCamera*>(GetParent())->GetWindowRect(&rect);
//			g_pWhiteBoardView->ScreenToClient(&rect);
//			point.x += rect.left;
//			point.y += rect.top;
//			wParam |= MK_WB_ULBUTTON;
//			g_pWhiteBoardView->OnLButtonUp((UINT)wParam, point);
//
//			if (g_nDrawStatus != DRAW_CLEANUP  && g_nDrawStatus != TODUSTBIN_OVER)
//			{
//				//发送同步消息 
//				XMemIni xmlObj;
//				dynamic_cast<CDlgCamera*>(GetParent())->GetCameraOwner()->SetXmlValue(xmlObj);
//				g_pWhiteBoardView->ChangeObj(xmlObj);	
//			}
//		}
//		break;
//	case WM_RBUTTONUP:
//		{
//			if ( (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH )
//			{
//				return 0;
//			}
//			CPoint point(LOWORD(lParam), HIWORD(lParam));
//			CRect rect;
//			dynamic_cast<CDlgCamera*>(GetParent())->GetWindowRect(&rect);
//			g_pWhiteBoardView->ScreenToClient(&rect);
//			point.x += rect.left;
//			point.y += rect.top;
//			g_pWhiteBoardView->OnRButtonUp((UINT)wParam, point);
//		}
//		break;
//	default:
//		return __super::WindowProc(message, wParam, lParam);
//	}
//	
//	return 0;
//}


/*-----------------------------------------------------------------------------------
End:  definition of the class of CWBCameraGestureWindows
------------------------------------------------------------------------------------*/

void IMXEC_PlayChanCallback_Empty::OnIMXEC_PlayChanCallbackAudChanData( unsigned char*pData,int nLen,bool&bLoop )
{
	if (g_pWhiteBoardView)
	{
		if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord)
		{
			g_pWhiteBoardView->SenPlayAudioDataToConferenceRecord(pData,nLen,bLoop);

		}
	}
}

void IMXEC_PlayChanCallback_Empty::OnIMXEC_PlayChanCallbackVidChanData( unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode )
{
	if (g_pWhiteBoardView)
	{
		if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord)
		{
			g_pWhiteBoardView->SenPlayMediadDataToConferenceRecord(pData,nLen,nKeyFrame);

		}
	}
}

IMXEC_PlayChanCallback_Empty::IMXEC_PlayChanCallback_Empty()
{
	m_dwPlayCarema = -1;
}

IMXEC_PlayChanCallback_Empty::~IMXEC_PlayChanCallback_Empty()
{

}

void CCapChanPlayHandle_Empty::OnIMXEC_CapChanCallbackAudioData( unsigned char*pData,int nLen,bool&bLoop )
{
	if (g_pWhiteBoardView)
	{
		if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord)
		{
			g_pWhiteBoardView->SenAudioDataToConferenceRecord(pData,nLen,bLoop,m_dwCarema);

		}
	}
}

CCapChanPlayHandle_Empty::CCapChanPlayHandle_Empty()
{
	m_dwCarema = -1;
}

CCapChanPlayHandle_Empty::~CCapChanPlayHandle_Empty()
{

}

void CCapChanPlayHandle_Empty::OnIMXEC_CapChanCallbackVideoData( unsigned char*pData,int nLen,int nKeyFrame )
{
	if (g_pWhiteBoardView)
	{
		if (g_pConferenceRecordNotify != NULL && g_bIsConferenceRecord)
		{
			g_pWhiteBoardView->SenMediadDataToConferenceRecord(pData,nLen,nKeyFrame,m_dwCarema);

		}
	}
}

void CCapChanPlayHandle_Empty::OnIMXEC_CapChanCallbackVideoDevOpen( void )
{
	int a = 10;
}

void CCapChanPlayHandle_Empty::OnIMXEC_CapChanCallbackFilePlay( void )
{
	int a = 10;
}
