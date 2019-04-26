#include "stdafx.h"

#include "WBWindows.h"
#include "../WBData.h"
#include "./ResourceWBMedia.h"
#include "../Define.h"
#include "../WB/whiteBoard/whiteBoard.h"
#include "WBGlobalMediaCtrl.h"
//#include "WB/WBDefine.h"
#include "../include/WB/WBDefine.h"
#include "../WB/whiteBoard/common/Define.h"

void  FillInputData(TOUCHINPUT* inData, DWORD cursor, DWORD eType, DWORD time, int x, int y);
void SetTabletInputServiceProperties(HWND hWnd)
{
	DWORD_PTR dwHwndTabletProperty = 
		TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
		TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
		TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
		TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)

	ATOM atom = ::GlobalAddAtom(MICROSOFT_TABLETPENSERVICE_PROPERTY);  
	SetProp(hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
	GlobalDeleteAtom(atom);
}

/*-----------------------------------------------------------------------------------
Begin:  definition of the class of CWBWindow
------------------------------------------------------------------------------------*/
IMPLEMENT_DYNAMIC(CWBWindow, CWnd)
	CWBWindow::CWBWindow()
{
}

CWBWindow::~CWBWindow()
{

}

BOOL CWBWindow::CreateWBWindows( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect,UINT nID, CWnd *pParent/* = NULL*/ )
{
	//主视图为其父窗口
	dwStyle |= WS_CHILD | WS_VISIBLE;
	BOOL bOK = Create(lpszClassName, lpszWindowName, dwStyle, rect, pParent, nID);
	return bOK;
}
/*-----------------------------------------------------------------------------------
End:  definition of the class of CWBWindow
------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------
Begin:  definition of the class of CWBMediaWindow
------------------------------------------------------------------------------------*/
unsigned long CWBMediaWindow::m_pCurCatchFrame = 0;
IMPLEMENT_DYNAMIC(CWBMediaWindow, CDialog)
	CWBMediaWindow::CWBMediaWindow( CString strFilePath, CMTObject *pWBObject, CWnd* pParent /*= NULL*/)
	: CDialog(IDD_DIALOG_WBMEDIA, pParent)
	, m_eMediaPlayState(Media_playing_before)
	, m_pWBMediaObject(pWBObject)
	, m_pCapture(NULL)
	, m_pVideoCapHandle(NULL)
	, strPathName(strFilePath)
	, m_bMediaLoop(FALSE)
	, m_pPlayChan(NULL)
	, m_pPlayChanCallback(NULL)
	, m_birate(512)
	,m_Controlbirate(0)
	, m_lButtonDownFlag(FALSE)
	, m_bMouseMove(FALSE)
	,m_bIsMediaMoveing(false)
{
	m_pCapture = NULL;
	m_eMediaPlayState = Media_playing_before;
	m_eCtrlState = CTRLHIDE;
	m_eVoiceState = VOICEOPEN;
	m_StrMediaDBclkPath = _T("");
	m_bFindFirst = true;
	m_pDBCCatchNum = 1;
}

CWBMediaWindow::~CWBMediaWindow()
{
	if (this->IsMediaOpen())
	{
		this->MediaClose();
	}

	if (NULL != m_pCapture)
	{
		m_pCapture->Close();
		delete m_pCapture;
		m_pCapture = NULL;
	}

	if (NULL != m_pPlayChan)
	{
		m_pPlayChan->Close();
		delete m_pPlayChan;
		m_pPlayChan = NULL;
	}
	if (NULL != m_pPlayChanCallback)
	{
		delete m_pPlayChanCallback;
		m_pPlayChanCallback = NULL;
	}
	if (NULL != m_pVideoCapHandle )
	{
		delete m_pVideoCapHandle;
		m_pVideoCapHandle= NULL;
	}
}
void CWBMediaWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWBMediaWindow, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_MEDIA_MOVE_TOUCH, &CWBMediaWindow::OnMediaMoveTouch)
	ON_MESSAGE(WM_MEDIA_CAPTURE, &CWBMediaWindow::OnMediaCapture)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



void CWBMediaWindow::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);
}

//改变视频窗口的大小，重新设置显示的视频的大小
void CWBMediaWindow::OnSize(UINT nType, int cx, int cy)
{
	SetInitSizeAndPos();

	CWnd::OnSize(nType, cx, cy);
}


//设置视频显示大小（让视频播放窗口大小和视频所在窗口大小一致）
void CWBMediaWindow::SetInitSizeAndPos()
{
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	MoveWindow(m_pOwnEr->m_rctMediaWindow.left, m_pOwnEr->m_rctMediaWindow.top, m_pOwnEr->m_rctMediaWindow.Width(), m_pOwnEr->m_rctMediaWindow.Height(), TRUE);
	this->ChangeMediaSize();
	if (!WBRESWND::IsGalleryWndCreated())
	{
		HRESULT hr =::SendMessage(g_whiteboardDlg->GetSafeHwnd(), UWM_CREATE_RESOURCE_DLG, 0, 0);
		
	}
	else if (!WBRESWND::IsGalleryWndShow())
	{
		WBRESWND::ShowGalleryWndEx();
	}
	
	
}


void CWBMediaWindow::InitialtMediaPlayingState(EMediaPlayState eMediaPlayState)
{
	m_eMediaPlayState = eMediaPlayState;
}


//根据路径打开视频
void CWBMediaWindow::OnOpenMedia(BOOL bActiveCreated /*= FALSE */)
{
	if (this->IsMediaOpen())
	{
		this->MediaPlay();
		InitialtMediaPlayingState(Media_playing);
	}
	else
	{	
		this->InitVedioCapture(CW2A(strPathName), bActiveCreated);
		InitialtMediaPlayingState(Media_playing);
	}
}

//截去视频的某一帧
BOOL CWBMediaWindow::CaptureToBMPFile()
{
	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("提示");
	}
	if (IsMediaOpen())
	{
		EMediaPlayState playState = GetMediaPlayState();
		if (playState == Media_playing)
		{
			if ( NULL != m_pCapture || NULL != m_pPlayChan)
			{
				CString strFilename;
				strFilename = g_strAvconTempPath + /*"截图"*/_T("MediaCatchPicture");

				//判断HPDTS目录
				if(!PathIsDirectory(g_strAvconTempPath))
				{		
					if(!CreateDirectory(g_strAvconTempPath,NULL))
					{
						CString strInterimFolderCreateFail;
						strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
						if (strInterimFolderCreateFail.IsEmpty())
						{
							strInterimFolderCreateFail = _T("临时文件夹创建失败");
						} 
						if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
						{
							//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strInterimFolderCreateFail, MB_OK,true);
							g_pHPWBDataNotify->ShowTipWindow(strInterimFolderCreateFail);
						}
						else
						{
							AfxMessageBox(strInterimFolderCreateFail);
						}
						return FALSE;
					}
				}
				//判断资源库文件夹是否存在
				if(!PathIsDirectory(strFilename.Left(strFilename.GetLength() - 3)))
				{		
					if(!CreateDirectory(strFilename.Left(strFilename.GetLength() - 3), NULL))
					{
						CString strInterimFolderCreateFail;
						strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
						if (strInterimFolderCreateFail.IsEmpty())
						{
							strInterimFolderCreateFail = _T("临时文件夹创建失败");
						} 
						if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
						{
							//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strInterimFolderCreateFail, MB_OK,true);
							g_pHPWBDataNotify->ShowTipWindow(strInterimFolderCreateFail);
						}
						else
						{
							AfxMessageBox(strInterimFolderCreateFail);
						}
						return FALSE;
					}
				}
				//判断截图文件夹是否存在，没有则创建
				if(!PathIsDirectory(strFilename))
				{		
					if(!CreateDirectory(strFilename, NULL))
					{
						CString strScrshotCreateFail;
						strScrshotCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScrshotCreateFail"));
						if (strScrshotCreateFail.IsEmpty())
						{
							strScrshotCreateFail = _T("截图文件夹创建失败");
						}
						if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
						{
							//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strPrompt, strScrshotCreateFail, MB_OK,true);
							g_pHPWBDataNotify->ShowTipWindow(strScrshotCreateFail);
						}
						else
						{
							MessageBox(strScrshotCreateFail);
						}
						return FALSE;
					}
				}	
				//left, right是取字符左边还是右边部分，ReverseFind是
				if (GetWBMediaOwner()->IsActiveCreated())
				{
					strFilename += GetWBMediaOwner()->m_pItem->strFilePath.Right(GetWBMediaOwner()->m_pItem->strFilePath.GetLength() - GetWBMediaOwner()->m_pItem->strFilePath.ReverseFind('\\'));
					strFilename = strFilename.Left(strFilename.ReverseFind('.'));
				}
				else
				{
					strFilename += _T("\\");
				}


				if (GetVedioPosition() !=0 )
				{
					strFilename.Format(strFilename + "%d", m_pCurCatchFrame);
				}
				else
				{
					CString strCannotShot;
					strCannotShot = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CannotShot"));
					if (strCannotShot.IsEmpty())
					{
						strCannotShot = _T("视频尚未播放，无法拍照");
					}
					if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
					{
						//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strCannotShot, MB_OK,true);
						g_pHPWBDataNotify->ShowTipWindow(strCannotShot);
					}
					else
					{
						AfxMessageBox(strCannotShot);
					}
					return FALSE;
				}

				strFilename += _T(".bmp");
				BOOL capture;
				std::string strFilePath = "";
				strFilePath = ToStr(strFilename.GetBuffer());
				if (GetWBMediaOwner()->IsActiveCreated())
				{
					if (NULL == m_pCapture)
					{
						return FALSE;
					}
					capture = m_pCapture->GetCapChanDev()->CaptureToBMPFile(strFilePath.c_str());
				}
				else
				{
					if ( NULL == m_pPlayChan)
					{
						return FALSE;
					}
					capture = m_pPlayChan->CaptureToBMPFile(strFilePath.c_str());
				}

				if (capture)
				{

					if (g_pWhiteBoardView == NULL)
					{
						return FALSE;
					}
					CWBPicture *pPictrue = new CWBPicture(strFilename);
					//根据视频小重设截图大小

					WBMediaItem MediaItem;
					WBPictureItem PictrueItem;

					GetWBMediaOwner()->GetItem(&MediaItem);				
					pPictrue->GetItem(&PictrueItem);
					PictrueItem.rect.right = MediaItem.rect.Width() + g_pWhiteBoardView->getXnPos() ;
					PictrueItem.rect.bottom = MediaItem.rect.Height() + g_pWhiteBoardView->getYnPos();
					pPictrue->SetItem(&PictrueItem);				
					pPictrue->CreateSelecrtFrame();		

					g_pWBPage->AddWBObject(pPictrue, pPictrue->GetLayerType());
					CPoint FlashPoint(72, 44);
					pPictrue->MoveOffset(FlashPoint);
					//发送同步消息
					XMemIni xmlObj;
					pPictrue->SetXmlValue(xmlObj);
					g_pWhiteBoardView->AddObj(xmlObj);

					g_pWhiteBoardView->Invalidate(TRUE);

					m_pCurCatchFrame++;
				}
				strFilename.ReleaseBuffer();
				return capture;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			CString strCannotShot;
			strCannotShot = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CannotShot"));
			if (strCannotShot.IsEmpty())
			{
				strCannotShot = _T("视频尚未播放，无法截图");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strCannotShot, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strCannotShot);
			}
			else
			{
				AfxMessageBox(strCannotShot);
			}
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

int CWBMediaWindow::InRegion( float x, float y )
{
	return TRUE;
}

int CWBMediaWindow::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


void CWBMediaWindow::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::PostNcDestroy();
	delete this;
}

void CWBMediaWindow::OnCancel()
{	
	return;
}

void CWBMediaWindow::OnOK()
{
	return;
}

//调整视频大小
void CWBMediaWindow::ChangeMediaSize()
{
	if ( NULL != m_pCapture )
	{
		CRect rect;
		::GetClientRect(m_hWnd, &rect);
		m_pCapture->SetVideoWindowPos(HWND_BOTTOM, 0, 0, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE);
	}
	if ( NULL != m_pPlayChan)
	{
		CRect rect;
		::GetClientRect(m_hWnd, &rect);
		m_pPlayChan->SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}

CAVCapture * CWBMediaWindow::GetCapture() const
{
	return m_pCapture;
}


void CWBMediaWindow::InitVedioCapture(const char* strVedioFile /*= NULL */, BOOL bActiveCreated /*= TRUE */)
{
	if ( "" == strVedioFile  || !bActiveCreated )    //如果路径为空而且是被动创建
	{
		if ( NULL != m_pVideoCapHandle )
		{
			delete m_pVideoCapHandle;
			m_pVideoCapHandle = NULL;
		}
		if ( NULL != m_pCapture )
		{
			m_pCapture->Close();
			delete m_pCapture;
			m_pCapture = NULL;
		}
		m_pPlayChanCallback = new CPlayChanEmptyHandle();
		m_pPlayChan = IMXEC_PlayChan::Create(*m_pPlayChanCallback);

		if (m_pPlayChan!=NULL)
		{
			if (g_pWhiteBoardView )
			{
				if(g_pWhiteBoardView->GetIsMedialubo())
				{
					if (0!=m_pPlayChan->Open(m_hWnd))
					{
						m_pPlayChan->Close();
					}
					else
					{
						//设置桢控制模式
						m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
						//启动音频，并设置是否接受
						m_pPlayChan->StartAudio();
						m_pPlayChan->EnableAudio(1);
						//启动视频，并设置是否接受
						m_pPlayChan->EnableVideo(1);
						m_pPlayChan->StartVideo();
						CRect rc;
						this->GetClientRect(rc);
						m_pPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
					}

				}
				else
				{
					if (0!=m_pPlayChan->Open(m_hWnd,
						m_CameraReceiveData.nAudioID,
						m_CameraReceiveData.nVideoID,
						m_CameraReceiveData.strNodeID.c_str(),
						m_CameraReceiveData.strNATIP.c_str(),
						m_CameraReceiveData.nNATPort,
						m_CameraReceiveData.strLocalIP.c_str(),
						m_CameraReceiveData.nLocalPort,
						m_CameraReceiveData.strMCUID.c_str(),
						m_CameraReceiveData.strMCUIP.c_str(),
						m_CameraReceiveData.nServerPort,0))
					{
						m_pPlayChan->Close();
						//delete m_pPlayChan;
						//m_pPlayChan=NULL;
					}
					else
					{
						m_pPlayChan->StartAudio();
						m_pPlayChan->StartVideo();
						//设置桢控制模式
						m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
						//启动音频，并设置是否接受
						if (g_bIsConferenceRecord)
						{
							m_pPlayChan->EnableAudio(1);
							m_pPlayChan->EnableVideo(1);
						}
						
						//m_pPlayChan->SetAudioCast(true);
						//m_pPlayChan->SetVideoCast(true);
						//启动视频，并设置是否接受
						//m_pPlayChan->StartMJPG(FRAME_TYPE_CONTROL_MODE_MAIN);

						//m_pPlayChan->StartRecordToAVI("d:\\TestP.avi");
						CRect rc;
						this->GetClientRect(rc);
						m_pPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
					}
				}
			}

		}
		return;
	}
	if ("" != strVedioFile && bActiveCreated)
	{
		if ( NULL == m_pVideoCapHandle )
		{
			m_pVideoCapHandle = new CCapChanPlayHandle();
		}
		if ( NULL == m_pCapture )
		{
			m_pCapture = new CAVCapture(*m_pVideoCapHandle);
			m_pCapture->Create(m_hWnd);
			CRect rect;
			GetClientRect(&rect);
			//m_pCapture->GetCapChanDev()->OpenAudio(0, X_AUDIO_CODEC_G7221_14_24);
			//m_pCapture->GetCapChanDev()->OpenVideoDev(strVedioFile, 1, CBR_2400, 0, VIDEC_CODEC_H264_SVC, 1, 1);
			m_pCapture->GetCapChanDev()->OpenVideoDev(strVedioFile, 1, m_birate, m_Controlbirate, VIDEC_CODEC_H264_SVC, 1, 1);
			m_pCapture->SetVideoWindowPos(HWND_BOTTOM, 0, 0, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE );
			m_pCapture->SetVolume(40);
			if (g_bIsConferenceRecord)
			{
				m_pCapture->SetForceEncodeAudio(1);
				m_pCapture->SetForceEncodeVideo(1);
			}
			
			SetTimer(1, 50, NULL);
		}
	}
}

//如果视频为打开的话就播放视频
void CWBMediaWindow::MediaPlay()
{
	if (this->IsMediaOpen())
	{
		if (NULL == m_pCapture)
		{
			return;
		}
		if ( NULL != m_pCapture->GetCapChanDev() )
		{
			m_pCapture->GetCapChanDev()->Play();
			this->SetMediaPlayState(Media_playing);
		}
	}
}
//如果视频打开的话，就暂停视频
void CWBMediaWindow::MediaPause()
{
	if (this->IsMediaOpen())
	{
		if (m_pCapture == NULL)
		{
			return;
		}
		if ( NULL != m_pCapture->GetCapChanDev() )
		{
			m_pCapture->GetCapChanDev()->Pause();
			this->SetMediaPlayState(Media_pausing);
		}
	}
}

//循环播放状态的切换
void CWBMediaWindow::MediaLoop()
{
	EMediaPlayState eMediaPlayState = GetMediaPlayState();
	m_bMediaLoop= !m_bMediaLoop;
	if ( eMediaPlayState == Media_playing ) 
	{
		MediaPlay();
	}
	GetWBMediaOwner()->m_pItem->bIsMediaLoop = m_bMediaLoop;
}

//如果视频打开的话，停止视频
void CWBMediaWindow::MediaStop()
{
	if (this->IsMediaOpen())
	{
		if (NULL == m_pCapture)
		{
			return;
		}
		if ( NULL != m_pCapture->GetCapChanDev() )
		{			
			m_pCapture->GetCapChanDev()->Pause();
			m_pCapture->GetCapChanDev()->Seek(0);
		}
		this->SetMediaPlayState(Media_stop);
	}
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	Invalidate();
}

//关闭视频
void CWBMediaWindow::MediaClose()
{
	if (NULL != m_pCapture)
	{
		m_pCapture->CloseVideo();
		m_pCapture->CloseAudio();
	}

	if ( NULL != m_pCapture )
	{
		m_pCapture->Close();
		delete m_pCapture;
		m_pCapture = NULL;
	}

	if ( NULL != m_pVideoCapHandle )
	{
		delete m_pVideoCapHandle;
		m_pVideoCapHandle = NULL;
	}

	this->SetMediaPlayState(Media_stop);
	this->DestroyWindow();
}

IMXEC_CapChanDev* CWBMediaWindow::GetVedioCapChanDev() const
{
	if ( NULL != m_pCapture )
	{
		return m_pCapture->GetCapChanDev();
	}
	else
	{
		return NULL;
	}
}

//得到视频的总长度，以时间为单位
unsigned long CWBMediaWindow::GetVedioDuration() const
{
	if ( NULL != m_pCapture )
	{
		return m_pCapture->GetDuration();;
	}
	else
	{
		return 0;
	}	
}

//得到视频当前播放的进度
unsigned long CWBMediaWindow::GetVedioPosition() const
{
	if ( NULL != m_pCapture )
	{
		if (GetWBMediaOwner()->GetMediaPlayState() == Media_playing 
			|| GetWBMediaOwner()->GetMediaPlayState() == Media_pausing)
		{
			return m_pCapture->GetPosition();
		}
		else
		{
			return 0;
		}

	}
	else if (NULL != m_pPlayChan)
	{
		if (GetWBMediaOwner()->GetMediaPlayState() == Media_playing)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

//记录视频的播放状态
void CWBMediaWindow::SetMediaPlayState(EMediaPlayState mediaPlayState) 
{
	m_eMediaPlayState = mediaPlayState;
	GetWBMediaOwner()->m_pItem->playState = m_eMediaPlayState;
	m_ePrePlayState = m_eMediaPlayState;
	if (m_pPlayChan == NULL)
	{
		return;
	}
	if (!GetWBMediaOwner()->IsActiveCreated())
	{
		if (m_eMediaPlayState == Media_playing)
		{
			if (GetVoiceState() == VOICECLOSE)
			{
				//m_pPlayChan->EnableAudio(0);
				m_pPlayChan->StopAudio();
			}
			else
			{
				//m_pPlayChan->EnableAudio(1);
				m_pPlayChan->StartAudio();
			}
			//m_pPlayChan->EnableVideo(1);
			m_pPlayChan->StartVideo();

		}
		else if (m_eMediaPlayState == Media_stop || m_eMediaPlayState == Media_pausing)
		{
			//m_pPlayChan->EnableAudio(0);
			//m_pPlayChan->EnableVideo(0);
			m_pPlayChan->StopAudio();
			m_pPlayChan->StopVideo();
		}

	}
}

//记录视频下方的控制条的是否可见
void CWBMediaWindow::SetCtrlState(ECtrlState ctrlState) 
{ 
	m_eCtrlState = ctrlState;
	GetWBMediaOwner()->m_pItem->showState = m_eCtrlState; 
}

//视频定位
void CWBMediaWindow::SeekVedioPosition(unsigned long ulSeconds)
{
	if ( NULL != m_pCapture )
	{
		GetWBMediaOwner()->m_pItem->llCurTime = ulSeconds;
		m_pCapture->Seek(ulSeconds);
	}
}

//设置音量
int CWBMediaWindow::SetVedioVolume(int nVolume)
{ 
	if ( NULL != m_pCapture )
	{
		return m_pCapture->SetVolume(nVolume);
	}
	else if (NULL != m_pPlayChan)
	{
		return m_pPlayChan->SetVolume(nVolume);
	}
	return -1;
}

//静音或打开声音
void CWBMediaWindow::MediaSetMute(EVoiceState eVoiceState)
{
	if ( NULL != m_pCapture )
	{
		if (eVoiceState == VOICEOPEN)
		{
			m_pCapture->SetMute(false);
			m_eVoiceState = VOICEOPEN;
			GetWBMediaOwner()->m_pItem->voiceState = VOICEOPEN;
		}
		else
		{
			m_pCapture->SetMute(true);
			m_eVoiceState = VOICECLOSE;
			GetWBMediaOwner()->m_pItem->voiceState = VOICECLOSE;
		}
	}
}

//判断视频设备是否打开
BOOL CWBMediaWindow::IsMediaOpen()
{
	if ( NULL != m_pCapture )
	{
		return m_pCapture->GetCapChanDev()->IsVideoDevOpen();
	}
	if ( NULL != m_pPlayChan)
	{
		return TRUE;
	}
	return FALSE;
}

//得到视频是处于单步播放还是循环播放的状态
BOOL CWBMediaWindow::GetMediaLoopState() const
{
	return m_bMediaLoop;
}

//设置单步播放还是循环播放
void CWBMediaWindow::SetMediaLoopState(BOOL bMediaLoopState)
{ 
	m_bMediaLoop = bMediaLoopState; 
}

//判断音频是否打开
BOOL CWBMediaWindow::IsAudioOpen()
{
	if ( NULL != m_pCapture)
	{
		return TRUE;
	}
	return FALSE;
}

/*
*  在OnTimer中监视视频播放的进度，以便在必要的时候让视频停止
*/
void CWBMediaWindow::OnTimer( UINT_PTR nIDEvent )
{
	if (FALSE == m_bMediaLoop)
	{
		unsigned long nPosition;
		nPosition = this->GetVedioPosition();
		unsigned long nDuration;
		nDuration = this->GetVedioDuration();
		if (nDuration - 1 <= nPosition)//没有到结尾，就需要画进度条，改变时间
		{
			MediaStop();

			if (FALSE == GetWBMediaOwner()->GetIsHide())
			{
				GetWBMediaOwner()->GetMediaCtrl()->TimerDraw(TRUE,FALSE);
			}
			 
			
			//发送同步消息 
			XMemIni xmlObj;
			GetWBMediaOwner()->SetXmlValue(xmlObj);
			g_pWhiteBoardView->ChangeObj(xmlObj);
		}
	}
	BOOL isPause = FALSE;
	if (GetMediaPlayState() == Media_pausing)
	{
		isPause = TRUE;
	}
	if ( GetMediaPlayState() == Media_playing || GetMediaPlayState() == Media_pausing)
	{
		if (FALSE == GetWBMediaOwner()->GetIsHide())
		{
			GetWBMediaOwner()->GetMediaCtrl()->TimerDraw(FALSE,isPause);
		}
	}
	CWnd::OnTimer(nIDEvent);
}

//获取远程连接所需要的数据
BOOL CWBMediaWindow::GetRemoteData( SCameraReceiveData &data )
{
	if (m_pCapture == NULL)
	{
		return FALSE;
	}
	data.nAudioID = m_pCapture->GetAudioID();//待补充
	data.nVideoID = m_pCapture->GetVideoID();
	data.strNodeID = std::string(NETEC_Node::GetNodeID());
	data.strNATIP = std::string(NETEC_Node::GetNATIP());
	data.nNATPort = NETEC_Node::GetNATPort();
	data.strLocalIP = std::string(NETEC_Node::GetLocalIP());
	data.nLocalPort = NETEC_Node::GetLocalPort();
	data.strMCUID = std::string(NETEC_Node::GetMCUID());
	data.strMCUIP = std::string(NETEC_Node::GetMCUIP());
	data.nServerPort = NETEC_Node::GetServerPort();
	data.m_strUCCUerID = GetWBMediaOwner()->cameraData.m_strUCCUerID;

	return TRUE;
}

BOOL CWBMediaWindow::UpdateRemoteData( SCameraReceiveData &data )
{
	m_CameraReceiveData.nAudioID = data.nAudioID;
	m_CameraReceiveData.nLocalPort = data.nLocalPort;
	m_CameraReceiveData.nNATPort = data.nNATPort;
	m_CameraReceiveData.nServerPort = data.nServerPort;
	m_CameraReceiveData.nVideoID = data.nVideoID;
	m_CameraReceiveData.strLocalIP = data.strLocalIP;
	m_CameraReceiveData.strMCUID = data.strMCUID;
	m_CameraReceiveData.strMCUIP = data.strMCUIP;
	m_CameraReceiveData.strNATIP = data.strNATIP;
	m_CameraReceiveData.strNodeID = data.strNodeID;
	m_CameraReceiveData.m_strUCCUerID = data.m_strUCCUerID;
	return TRUE;
}

void CWBMediaWindow::SetBirtrate( int birtate )
{

	if (NULL != m_pCapture)
	{
		m_pCapture->Close();
		delete m_pCapture;
		m_pCapture = NULL;
	}

	m_birate = birtate;

	OnOpenMedia(TRUE);
}

void CWBMediaWindow::SetControlBirtrate( int controlbirtate )
{
	if (NULL != m_pCapture)
	{
		m_pCapture->Close();
		delete m_pCapture;
		m_pCapture = NULL;
	}

	m_Controlbirate = controlbirtate;

	OnOpenMedia(TRUE);
}

void CWBMediaWindow::IsPlanChan()
{
	if (m_pPlayChan != NULL)
	{
		m_pPlayChan->Close();
		delete m_pPlayChan;
		m_pPlayChan = NULL;
	}
	OnOpenMedia(FALSE);
}


/*-----------------------------------------------------------------------------------
End:  definition of the class of CWBMediaWindow
------------------------------------------------------------------------------------*/

void FillInputData(TOUCHINPUT* inData, DWORD cursor, DWORD eType, DWORD time, int x, int y)
{
	inData->dwID = cursor;
	inData->dwFlags = eType;
	inData->dwTime = time;
	inData->x = x;
	inData->y = y;
}


BOOL CWBMediaWindow::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return __super::OnEraseBkgnd(pDC);

}

void CWBMediaWindow::SetVoiceState( EVoiceState voiceState )
{
	if (m_pPlayChan == NULL)
	{
		return;
	}

	if (voiceState == VOICECLOSE)
	{
		m_pPlayChan->EnableAudio(0);
	}
	else
	{
		m_pPlayChan->EnableAudio(1);
		//被动接收关闭静音按钮无效  重新开启音频
		m_pPlayChan->StartAudio();
		//m_pPlayChan->StartVideo();
	}
	m_eVoiceState = voiceState;  
	
}
int CWBMediaWindow::PlayVideoData( const void*pData,int nLen )
{
	if (m_pPlayChan == NULL)
	{
		return -1;
	}

	int i = m_pPlayChan->PlayVideoData(pData,nLen);
	return i;

}
int CWBMediaWindow::PlayAudioData(const void*pData,int nLen)
{
	if (m_pPlayChan == NULL)
	{
		return -1;
	}
	int i = m_pPlayChan->PlayAudioData(pData,nLen);
	return i;
}



void CWBMediaWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH) && !g_bIsSupportMTTouch) //如果是手指按下并且没有授权，直接返回不处理
	{
		return ;
	}
	if (g_pWhiteBoardView == NULL || GetWBMediaOwner() == NULL)
	{
		return ;
	}
	if (DRAW_NORMAL == g_nDrawStatus)
	{
		g_pWBPage->SetObjTopLayer(GetWBMediaOwner()->GetLayerIndex());
	}
	m_lButtonDownFlag = TRUE;
	m_bIsMediaMoveing = false;
	//将在鼠标在视频上的坐标转化为相对于白板视图的坐标，并将此动作交给白板试图处理
	CRect rect;
	GetWindowRect(&rect);
	g_pWhiteBoardView->ScreenToClient(&rect);
	point.x += rect.left;
	point.y += rect.top;
	nFlags |= MK_WB_ULBUTTON;
	SetCapture();
	g_pWhiteBoardView->OnLButtonDown(nFlags, point);

}


void CWBMediaWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!m_lButtonDownFlag)//鼠标没有按下时候不响应MOVEwang
		return;
	if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH) && !g_bIsSupportMTTouch) //如果是手指按下并且没有授权，直接返回不处理
	{
		return ;
	}

	if (g_pWhiteBoardView == NULL || GetWBMediaOwner() == NULL)
	{
		return ;
	}
	if (GetMediaPlayState() == Media_playing)
	{
		MediaPlayToPause();
		m_bIsMediaMoveing = true;
	}
	
	if (m_lButtonDownFlag == TRUE)
	{
		GetWBMediaOwner()->SetMediaCtrlShow(TRUE);
		m_bMouseMove = TRUE;
	}
	//将在鼠标在视频上的坐标转化为相对于白板视图的坐标，并将此动作交给白板试图处理
	CRect rect;
	GetWindowRect(&rect);
	g_pWhiteBoardView->ScreenToClient(&rect);
	point.x += rect.left;
	point.y += rect.top;
	nFlags |= MK_WB_ULBUTTON;
	g_pWhiteBoardView->OnMouseMove(nFlags, point);

}


void CWBMediaWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH) && !g_bIsSupportMTTouch) //如果是手指按下并且没有授权，直接返回不处理
	{
		return ;
	}

	if (g_pWhiteBoardView == NULL || GetWBMediaOwner() == NULL)
	{
		return ;
	}
	if (GetMediaPlayState() == Media_pausing && m_bIsMediaMoveing)
	{
		MediaPauseToPlay();
		m_bIsMediaMoveing = false;
	}
	m_lButtonDownFlag = FALSE;
	m_bMouseMove = FALSE;
	//让视频控制窗口显示
	GetWBMediaOwner()->SetMediaCtrlShow(TRUE);
	
	//将在鼠标在视频上的坐标转化为相对于白板视图的坐标，并将此动作交给白板试图处理
	CRect rect;
	GetWindowRect(&rect);

	g_pWhiteBoardView->ScreenToClient(&rect);
	point.x += rect.left;
	point.y += rect.top;
	nFlags |= MK_WB_ULBUTTON;
	SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE/*|SWP_SHOWWINDOW*/);
	ReleaseCapture();
	g_pWhiteBoardView->OnLButtonUp(nFlags, point);
}


void CWBMediaWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_pWhiteBoardView == NULL)
	{
		return ;
	}
	CRect rect;
	GetWindowRect(&rect);
	g_pWhiteBoardView->ScreenToClient(&rect);
	point.x += rect.left;
	point.y += rect.top;
	g_pWhiteBoardView->OnRButtonUp(nFlags, point);

}

LRESULT CWBMediaWindow::OnMediaMoveTouch( WPARAM wParam, LPARAM lParam )
{
	if (GetWBMediaOwner() == NULL || g_pWhiteBoardView == NULL)
	{
		return 0;
	}
	CRect rect = GetWBMediaOwner()->GetMoveRect();
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
	GetWBMediaOwner()->SetMoveRect(rect);
	return 1;

}

LRESULT CWBMediaWindow::OnMediaCapture( WPARAM wParam, LPARAM lParam )
{
	BOOL bFlag = FALSE;
	m_bMediaCapture = FALSE;
	bFlag = CaptureToBMPFile();
	m_bMediaCapture = bFlag;
	return bFlag;
}

void CWBMediaWindow::MediaPlayToPause()
{
	SetMediaPlayState(Media_pausing);
	MediaPause();
}

void CWBMediaWindow::MediaPauseToPlay()
{
	SetMediaPlayState(Media_playing);
	MediaPlay();
}

IMXEC_PlayChan* CWBMediaWindow::GetPlayChan() const
{
	return m_pPlayChan;
}

void CWBMediaWindow::MediaPhotoEditFun()
{
	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("提示");
	}
	if (IsMediaOpen())
	{
		EMediaPlayState playState = GetMediaPlayState();
		if (playState == Media_playing)
		{
			if ( NULL != m_pCapture || NULL != m_pPlayChan)
			{
				if (g_strMediaPhoto.IsEmpty())
				{
					g_strMediaPhoto = GetCurrentPath() + _T("\\user");
				}
				CString strFilename;
				if (m_StrMediaDBclkPath.IsEmpty())
				{
					CString strFilename;
					CString strReadName;
					strReadName = ReadConfigText(_T("WhiteBoardMenuListText"),_T("MediaPhoto"));
					if (strReadName.IsEmpty())
					{
						strReadName = _T("视频拍照");
					}
					strFilename = g_strMediaPhoto + _T("\\") + strReadName;
					m_StrMediaDBclkPath = strFilename;
				}
				else
				{
					strFilename = m_StrMediaDBclkPath;
				}
				//判断HPDTS目录
				if(!PathIsDirectory(g_strMediaPhoto))
				{		
					if(!CreateDirectory(g_strMediaPhoto,NULL))
					{
						CString strInterimFolderCreateFail;
						strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
						if (strInterimFolderCreateFail.IsEmpty())
						{
							strInterimFolderCreateFail = _T("临时文件夹创建失败");
						} 
						if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
						{
							//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strInterimFolderCreateFail, MB_OK,true);
							g_pHPWBDataNotify->ShowTipWindow(strInterimFolderCreateFail);
						}
						else
						{
							AfxMessageBox(strInterimFolderCreateFail);
						}
						return ;
					}
				}		
				if(!PathIsDirectory(strFilename))
				{		
					if(!CreateDirectory(strFilename, NULL))
					{
						CString strScrshotCreateFail;
						strScrshotCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScrshotCreateFail"));
						if (strScrshotCreateFail.IsEmpty())
						{
							strScrshotCreateFail = _T("截图文件夹创建失败");
						}
						if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
						{
							//g_pHPWBDataNotify->MessageBox(this->m_hWnd,strPrompt, strScrshotCreateFail, MB_OK,true);
							g_pHPWBDataNotify->ShowTipWindow(strScrshotCreateFail);
						}
						else
						{
							MessageBox(strScrshotCreateFail);
						}
						return ;
					}
				}	
				//left, right是取字符左边还是右边部分，ReverseFind是
				if (GetWBMediaOwner()->IsActiveCreated())
				{
					strFilename += GetWBMediaOwner()->m_pItem->strFilePath.Right(GetWBMediaOwner()->m_pItem->strFilePath.GetLength() - GetWBMediaOwner()->m_pItem->strFilePath.ReverseFind('\\'));
					strFilename = strFilename.Left(strFilename.ReverseFind('.'));
				}
				else
				{
					CString strName = GetWBMediaOwner()->GetSaveGetMediaName();
					strFilename += strName.Right(strName.GetLength() - strName.ReverseFind('\\'));
					strFilename = strFilename.Left(strFilename.ReverseFind('.'));
					CString strUser(m_CameraReceiveData.m_strUCCUerID.c_str());
					if (!strUser.IsEmpty())
					{
						strUser = strUser.Left(strUser.ReverseFind('@'));
						strFilename += strUser;
					}
					/*strFilename += _T("\\");*/
				}


				if (GetVedioPosition() !=0 )
				{
					int a = m_pDBCCatchNum/1000;
					int b = (m_pDBCCatchNum - a*1000)/100;
					int c = (m_pDBCCatchNum - a*1000 - b*100)/10;
					int d = m_pDBCCatchNum - a*1000 - b*100 - c*10;
					strFilename.Format(strFilename +_T("_")+ "%d%d%d%d", a,b,c,d);
				}
				else
				{
					CString strCannotShot;
					strCannotShot = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CannotShot"));
					if (strCannotShot.IsEmpty())
					{
						strCannotShot = _T("视频尚未播放，无法拍照");
					}
					if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
					{
						//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strCannotShot, MB_OK,true);
						g_pHPWBDataNotify->ShowTipWindow(strCannotShot);
					}
					else
					{
						AfxMessageBox(strCannotShot);
					}
					return ;
				}

				strFilename += _T(".jpg");
				BOOL capture;
				std::string strFilePath = "";
				strFilePath = ToStr(strFilename.GetBuffer());
				if (GetWBMediaOwner()->IsActiveCreated())
				{
					if (NULL == m_pCapture)
					{
						return ;
					}
					capture = m_pCapture->GetCapChanDev()->CaptureToBMPFile(strFilePath.c_str());
				}
				else
				{
					if ( NULL == m_pPlayChan)
					{
						return ;
					}
					capture = m_pPlayChan->CaptureToBMPFile(strFilePath.c_str());
				}

				if (capture)
				{					
					if (g_pWhiteBoardView == NULL)
					{
						return ;
					}
					CWBPicture *pPictrue = new CWBPicture(strFilename);
					//根据视频小重设截图大小

					WBMediaItem MediaItem;
					WBPictureItem PictrueItem;

					GetWBMediaOwner()->GetItem(&MediaItem);				
					pPictrue->GetItem(&PictrueItem);
					PictrueItem.rect.left = MediaItem.rect.left + g_pWhiteBoardView->getXnPos() ;
					PictrueItem.rect.top = MediaItem.rect.top + g_pWhiteBoardView->getYnPos() ;
					PictrueItem.rect.right = MediaItem.rect.right + g_pWhiteBoardView->getXnPos() ;
					PictrueItem.rect.bottom = MediaItem.rect.bottom + g_pWhiteBoardView->getYnPos();
					pPictrue->SetItem(&PictrueItem);				
					pPictrue->CreateSelecrtFrame();	
					CRect rcWBdlg;
					g_whiteboardDlg->GetWindowRect(&rcWBdlg);
					bool bgoright = true;
					if (MediaItem.rect.right > rcWBdlg.right - rcWBdlg.Width()/3)
					{
						bgoright = false;
					}
					CRect rcResGaller;
					((CWhiteBoardDlg*)g_whiteboardDlg)->GetResGalleryRect(rcResGaller);
					CPoint pt(rcResGaller.right,rcResGaller.top);
					g_pWhiteBoardView->SetWBPictureEffect(pPictrue);
					bool bok = g_pWhiteBoardView->ProcessMediaPhotoEffect(pt,bgoright);
					

					m_pDBCCatchNum++;
				}
				strFilename.ReleaseBuffer();
				return ;
			}
			else
			{
				return ;
			}
		}
		else
		{
			CString strCannotShot;
			strCannotShot = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CannotShot"));
			if (strCannotShot.IsEmpty())
			{
				strCannotShot = _T("视频尚未播放，无法截图");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strCannotShot, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strCannotShot);
			}
			else
			{
				AfxMessageBox(strCannotShot);
			}
			return ;
		}
	}
	else
	{
		return ;
	}
}

void CWBMediaWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	

	if (m_StrMediaDBclkPath.IsEmpty())
	{
		CString strFilename;
		CString strReadName;
		strReadName = ReadConfigText(_T("WhiteBoardMenuListText"),_T("MediaPhoto"));
		if (strReadName.IsEmpty())
		{
			strReadName = _T("视频拍照");
		}
		strFilename = g_strMediaPhoto + _T("\\") + strReadName;
		m_StrMediaDBclkPath = strFilename;
	}
	bool bok = g_pWhiteBoardView->CheckDiskPlaceForPic();
	if (!bok)
	{
		return;
	}
	if (m_bFindFirst)
	{
		m_bFindFirst = false;
		BOOL fContinue = TRUE;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		tstring wstrSearchPath;
		tstring sNewDir = tstring(m_StrMediaDBclkPath);
		int nDirSize = sNewDir.size();
		tstring Temp1 = sNewDir.substr(nDirSize-1,1);
		BOOL bDirHasSlash = FALSE;//目录名称是否也有斜杠
		if ( tstring(TEXT("\\")) == Temp1)
		{
			wstrSearchPath = sNewDir + tstring(TEXT("*.*"));
			bDirHasSlash = TRUE;
		}
		else
		{
			wstrSearchPath = sNewDir + tstring(TEXT("\\")) + tstring(TEXT("*.*"));
		}

		hFind = FindFirstFile(wstrSearchPath.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			fContinue = FALSE;
		}

		CString strFilename;
		if (GetWBMediaOwner()->IsActiveCreated())
		{
			strFilename= GetWBMediaOwner()->m_pItem->strFilePath.Right(GetWBMediaOwner()->m_pItem->strFilePath.GetLength() - GetWBMediaOwner()->m_pItem->strFilePath.ReverseFind('\\') -1);
			strFilename = strFilename.Left(strFilename.ReverseFind('.'));
		}
		else
		{
			strFilename= GetWBMediaOwner()->GetSaveGetMediaName().Right(GetWBMediaOwner()->GetSaveGetMediaName().GetLength() - GetWBMediaOwner()->GetSaveGetMediaName().ReverseFind('\\') -1);
			strFilename = strFilename.Left(strFilename.ReverseFind('.'));
			CString strUser(m_CameraReceiveData.m_strUCCUerID.c_str());
			if (!strUser.IsEmpty())
			{
				strUser = strUser.Left(strUser.ReverseFind('@'));
				strFilename += strUser;
			}
		}
		
		
		CString strFindName = _T("");
		int nMax = 0;
		for (int iTimerId = 1; fContinue; iTimerId++)
		{
			if(m_StrMediaDBclkPath == TEXT("/")) break;
			BOOL bSupportFiles = !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)     &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)     &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY);
			HRESULT hr = S_OK;
			//进一步判定是否是所支持的文件
			tstring wstrPath;
			if (bDirHasSlash)
			{
				wstrPath = sNewDir + tstring( FindFileData.cFileName);
			}
			else
			{
				wstrPath = sNewDir + tstring(TEXT("\\")) + tstring( FindFileData.cFileName);
			}
			CString strGetNumber = _T("");
			strFindName = wstrPath.c_str();			
			strFindName = strFindName.Right(strFindName.GetLength() - strFindName.ReverseFind('\\')-1);
			strGetNumber = strFindName;
			strFindName = strFindName.Left(strFindName.ReverseFind('.') -5);
			if (strFindName == strFilename)
			{
				strGetNumber = strGetNumber.Left(strGetNumber.ReverseFind('.'));
				strGetNumber = strGetNumber.Right(4);
				long number = 0;
				number =_ttoi(strGetNumber);
				if ((int)number >nMax )
				{
					nMax = number;
				}
			}
			fContinue = FindNextFile(hFind, &FindFileData);
		}
		m_pDBCCatchNum = nMax +1;
	}
	if (!WBRESWND::IsGalleryWndCreated())
	{
		HRESULT hr =::SendMessage(g_whiteboardDlg->GetSafeHwnd(), UWM_CREATE_RESOURCE_DLG, 0, 0);
		if (SUCCEEDED(hr))
		{
			MediaPhotoEditFun();
			WBRESWND::SwitchSpecialDirectoryEx(m_StrMediaDBclkPath);
		}
	}
	else if (!WBRESWND::IsGalleryWndShow())
	{
		WBRESWND::ShowGalleryWndEx();
		MediaPhotoEditFun();
		WBRESWND::SwitchSpecialDirectoryEx(m_StrMediaDBclkPath);
	}
	else
	{
		MediaPhotoEditFun();
		WBRESWND::SwitchSpecialDirectoryEx(m_StrMediaDBclkPath);
	}
	
	
	__super::OnLButtonDblClk(nFlags, point);
}
