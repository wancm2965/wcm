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
	//����ͼΪ�丸����
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

//�ı���Ƶ���ڵĴ�С������������ʾ����Ƶ�Ĵ�С
void CWBMediaWindow::OnSize(UINT nType, int cx, int cy)
{
	SetInitSizeAndPos();

	CWnd::OnSize(nType, cx, cy);
}


//������Ƶ��ʾ��С������Ƶ���Ŵ��ڴ�С����Ƶ���ڴ��ڴ�Сһ�£�
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


//����·������Ƶ
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

//��ȥ��Ƶ��ĳһ֡
BOOL CWBMediaWindow::CaptureToBMPFile()
{
	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("��ʾ");
	}
	if (IsMediaOpen())
	{
		EMediaPlayState playState = GetMediaPlayState();
		if (playState == Media_playing)
		{
			if ( NULL != m_pCapture || NULL != m_pPlayChan)
			{
				CString strFilename;
				strFilename = g_strAvconTempPath + /*"��ͼ"*/_T("MediaCatchPicture");

				//�ж�HPDTSĿ¼
				if(!PathIsDirectory(g_strAvconTempPath))
				{		
					if(!CreateDirectory(g_strAvconTempPath,NULL))
					{
						CString strInterimFolderCreateFail;
						strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
						if (strInterimFolderCreateFail.IsEmpty())
						{
							strInterimFolderCreateFail = _T("��ʱ�ļ��д���ʧ��");
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
				//�ж���Դ���ļ����Ƿ����
				if(!PathIsDirectory(strFilename.Left(strFilename.GetLength() - 3)))
				{		
					if(!CreateDirectory(strFilename.Left(strFilename.GetLength() - 3), NULL))
					{
						CString strInterimFolderCreateFail;
						strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
						if (strInterimFolderCreateFail.IsEmpty())
						{
							strInterimFolderCreateFail = _T("��ʱ�ļ��д���ʧ��");
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
				//�жϽ�ͼ�ļ����Ƿ���ڣ�û���򴴽�
				if(!PathIsDirectory(strFilename))
				{		
					if(!CreateDirectory(strFilename, NULL))
					{
						CString strScrshotCreateFail;
						strScrshotCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScrshotCreateFail"));
						if (strScrshotCreateFail.IsEmpty())
						{
							strScrshotCreateFail = _T("��ͼ�ļ��д���ʧ��");
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
				//left, right��ȡ�ַ���߻����ұ߲��֣�ReverseFind��
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
						strCannotShot = _T("��Ƶ��δ���ţ��޷�����");
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
					//������ƵС�����ͼ��С

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
					//����ͬ����Ϣ
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
				strCannotShot = _T("��Ƶ��δ���ţ��޷���ͼ");
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
	// TODO: �ڴ����ר�ô����/����û���

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

//������Ƶ��С
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
	if ( "" == strVedioFile  || !bActiveCreated )    //���·��Ϊ�ն����Ǳ�������
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
						//���������ģʽ
						m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
						//������Ƶ���������Ƿ����
						m_pPlayChan->StartAudio();
						m_pPlayChan->EnableAudio(1);
						//������Ƶ���������Ƿ����
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
						//���������ģʽ
						m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
						//������Ƶ���������Ƿ����
						if (g_bIsConferenceRecord)
						{
							m_pPlayChan->EnableAudio(1);
							m_pPlayChan->EnableVideo(1);
						}
						
						//m_pPlayChan->SetAudioCast(true);
						//m_pPlayChan->SetVideoCast(true);
						//������Ƶ���������Ƿ����
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

//�����ƵΪ�򿪵Ļ��Ͳ�����Ƶ
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
//�����Ƶ�򿪵Ļ�������ͣ��Ƶ
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

//ѭ������״̬���л�
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

//�����Ƶ�򿪵Ļ���ֹͣ��Ƶ
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

//�ر���Ƶ
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

//�õ���Ƶ���ܳ��ȣ���ʱ��Ϊ��λ
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

//�õ���Ƶ��ǰ���ŵĽ���
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

//��¼��Ƶ�Ĳ���״̬
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

//��¼��Ƶ�·��Ŀ��������Ƿ�ɼ�
void CWBMediaWindow::SetCtrlState(ECtrlState ctrlState) 
{ 
	m_eCtrlState = ctrlState;
	GetWBMediaOwner()->m_pItem->showState = m_eCtrlState; 
}

//��Ƶ��λ
void CWBMediaWindow::SeekVedioPosition(unsigned long ulSeconds)
{
	if ( NULL != m_pCapture )
	{
		GetWBMediaOwner()->m_pItem->llCurTime = ulSeconds;
		m_pCapture->Seek(ulSeconds);
	}
}

//��������
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

//�����������
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

//�ж���Ƶ�豸�Ƿ��
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

//�õ���Ƶ�Ǵ��ڵ������Ż���ѭ�����ŵ�״̬
BOOL CWBMediaWindow::GetMediaLoopState() const
{
	return m_bMediaLoop;
}

//���õ������Ż���ѭ������
void CWBMediaWindow::SetMediaLoopState(BOOL bMediaLoopState)
{ 
	m_bMediaLoop = bMediaLoopState; 
}

//�ж���Ƶ�Ƿ��
BOOL CWBMediaWindow::IsAudioOpen()
{
	if ( NULL != m_pCapture)
	{
		return TRUE;
	}
	return FALSE;
}

/*
*  ��OnTimer�м�����Ƶ���ŵĽ��ȣ��Ա��ڱ�Ҫ��ʱ������Ƶֹͣ
*/
void CWBMediaWindow::OnTimer( UINT_PTR nIDEvent )
{
	if (FALSE == m_bMediaLoop)
	{
		unsigned long nPosition;
		nPosition = this->GetVedioPosition();
		unsigned long nDuration;
		nDuration = this->GetVedioDuration();
		if (nDuration - 1 <= nPosition)//û�е���β������Ҫ�����������ı�ʱ��
		{
			MediaStop();

			if (FALSE == GetWBMediaOwner()->GetIsHide())
			{
				GetWBMediaOwner()->GetMediaCtrl()->TimerDraw(TRUE,FALSE);
			}
			 
			
			//����ͬ����Ϣ 
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

//��ȡԶ����������Ҫ������
BOOL CWBMediaWindow::GetRemoteData( SCameraReceiveData &data )
{
	if (m_pCapture == NULL)
	{
		return FALSE;
	}
	data.nAudioID = m_pCapture->GetAudioID();//������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
		//�������չرվ�����ť��Ч  ���¿�����Ƶ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH) && !g_bIsSupportMTTouch) //�������ָ���²���û����Ȩ��ֱ�ӷ��ز�����
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
	//�����������Ƶ�ϵ�����ת��Ϊ����ڰװ���ͼ�����꣬�����˶��������װ���ͼ����
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (!m_lButtonDownFlag)//���û�а���ʱ����ӦMOVEwang
		return;
	if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH) && !g_bIsSupportMTTouch) //�������ָ���²���û����Ȩ��ֱ�ӷ��ز�����
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
	//�����������Ƶ�ϵ�����ת��Ϊ����ڰװ���ͼ�����꣬�����˶��������װ���ͼ����
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH) && !g_bIsSupportMTTouch) //�������ָ���²���û����Ȩ��ֱ�ӷ��ز�����
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
	//����Ƶ���ƴ�����ʾ
	GetWBMediaOwner()->SetMediaCtrlShow(TRUE);
	
	//�����������Ƶ�ϵ�����ת��Ϊ����ڰװ���ͼ�����꣬�����˶��������װ���ͼ����
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
		strPrompt = _T("��ʾ");
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
						strReadName = _T("��Ƶ����");
					}
					strFilename = g_strMediaPhoto + _T("\\") + strReadName;
					m_StrMediaDBclkPath = strFilename;
				}
				else
				{
					strFilename = m_StrMediaDBclkPath;
				}
				//�ж�HPDTSĿ¼
				if(!PathIsDirectory(g_strMediaPhoto))
				{		
					if(!CreateDirectory(g_strMediaPhoto,NULL))
					{
						CString strInterimFolderCreateFail;
						strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
						if (strInterimFolderCreateFail.IsEmpty())
						{
							strInterimFolderCreateFail = _T("��ʱ�ļ��д���ʧ��");
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
							strScrshotCreateFail = _T("��ͼ�ļ��д���ʧ��");
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
				//left, right��ȡ�ַ���߻����ұ߲��֣�ReverseFind��
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
						strCannotShot = _T("��Ƶ��δ���ţ��޷�����");
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
					//������ƵС�����ͼ��С

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
				strCannotShot = _T("��Ƶ��δ���ţ��޷���ͼ");
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	

	if (m_StrMediaDBclkPath.IsEmpty())
	{
		CString strFilename;
		CString strReadName;
		strReadName = ReadConfigText(_T("WhiteBoardMenuListText"),_T("MediaPhoto"));
		if (strReadName.IsEmpty())
		{
			strReadName = _T("��Ƶ����");
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
		BOOL bDirHasSlash = FALSE;//Ŀ¼�����Ƿ�Ҳ��б��
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
			//��һ���ж��Ƿ�����֧�ֵ��ļ�
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
