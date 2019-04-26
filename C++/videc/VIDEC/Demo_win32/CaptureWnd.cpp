// VideoWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "CaptureWnd.h"
#include "afxdialogex.h"
#include "Demo_win32Dlg.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
//#include "..\..\VIDEC\VIDEC_API\XImage.h"


// CCaptureWnd 对话框

void VidecHGRCallBackResult(PG_SDK_FrameResultC const & FrameResult,void*pUserData)
{
	CCaptureWnd*pWnd = (CCaptureWnd*)pUserData;
	if (pWnd)
	{
		pWnd->OnPG_SDKCallBackResult(FrameResult);
	}
}

IMPLEMENT_DYNAMIC(CCaptureWnd, CDialogEx)

CCaptureWnd::CCaptureWnd(int nShowType,bool bEncode,CWnd* pParent /*=NULL*/)
	: CDialogEx(CCaptureWnd::IDD, pParent)
	,m_pD3DRender(NULL)
	,m_pEncode(NULL)
	//,m_pXISRender(NULL)
	,m_pDraw(NULL)
	,m_pVidecD3D(NULL)
	,m_pAH2K(NULL)
	,m_pCamera(NULL)
	,m_bEncode(bEncode)	
	,m_pWnd(NULL)
{
	m_nShowType = (VIDEO_SHOW_TYPE)nShowType;
	m_bShowMax = false;

	m_pFile = NULL;
}

CCaptureWnd::~CCaptureWnd()
{
}

void CCaptureWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCaptureWnd, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CCaptureWnd 消息处理程序


BOOL CCaptureWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_param.iBrightness		= 0;
	m_param.iContrast		= 1;
	m_param.iSaturation		= 1;
	m_param.iHue			= 0;
	m_param.iDetail			= 0;
	m_param.iDenoise		= 0;

	m_bShowVideo = true;

	ModifyStyle(WS_CHILD|WS_SYSMENU,WS_CAPTION|WS_DLGFRAME|WS_SIZEBOX);

	SetTimer(1000,1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


BOOL CCaptureWnd::DestroyWindow()
{
	KillTimer(1000);
	return CDialogEx::DestroyWindow();
}


BOOL CCaptureWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CCaptureWnd::OnEraseBkgnd(CDC* pDC)
{
	//if (!m_bShowVideo)
	//{
	//	CRect rc;
	//	GetClientRect(&rc);
	//	pDC->FillRect(rc, &CBrush(RGB(255, 0, 0)));	
	//}
	return TRUE;// CDialogEx::OnEraseBkgnd(pDC);
}


void CCaptureWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	if (m_pWnd)
	{
		m_pWnd->SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
	if (m_pVidecD3D)
	{
		VIDECYNC_D3DPlayerSetWindowPos(m_pVidecD3D,NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}
void CCaptureWnd::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

// 	if (m_pXISRender && m_bShowVideo)
// 	{
// 		SetVideoShow(m_bShowVideo);
// 	}
}

void CCaptureWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (1000 == nIDEvent)
	{
		CString strText;
		int nFps = 0;
		int nShowType = 0;

		if (m_pWnd)
		{
			nFps=m_pWnd->GetVideoShowFrameRate();
			nShowType = m_pWnd->GetVideoShowType();
		}
		else if (m_pVidecD3D)
		{
			nFps=VIDECYNC_D3DPlayerGetFrameRate(m_pVidecD3D);
			nShowType = VIDECYNC_D3DPlayerGetShowType(m_pVidecD3D);
		}

		CString strShowType;
		switch(nShowType)
		{
		case 0:
			strShowType = _T("Unknown");
			break;
		case 1:
			strShowType = _T("DirectShow");
			break;
		case 2:
			strShowType = _T("D3D9");
			break;
		case 3:
			strShowType = _T("D3D11");
			break;
		case 4:
			strShowType = _T("D3D12");
			break;
		};

		int nType=0;
		int nSendFrameRate=0; 
		if (m_pEncode)
		{
			nType = m_pEncode->m_nEncoderType;
			nSendFrameRate = m_pEncode->GetVideoSendFrameRate();
		}
		switch(nType)
		{
		case 0:
			strText.Format(_T("Unknown Encoder Show=%d Send=%d %s"),nFps,nSendFrameRate,strShowType);
			break;
		case 1:
			strText.Format(_T("SoftWare Encoder Show=%d Send=%d %s"),nFps,nSendFrameRate,strShowType);
			break;
		case 2:
			strText.Format(_T("Intel HardWare Encoder Show=%d Send=%d %s"),nFps,nSendFrameRate,strShowType);
			break;
		};
		SetWindowText(strText);
		//}
	}
	else if (nIDEvent == 1)
	{
		KillTimer(1);

		if (VIDECHGR_GetUserData() == NULL)
		{
			PGReturnCodeC RetCode = PG_C_OK;
			RetCode = VIDECHGR_Start(VidecHGRCallBackResult,this,m_nWidth,m_nHeight, m_nFrameRate);
			if (RetCode == PG_C_OK)
			{
				PG_SDK_DetectionParamsC params;
				params.m_bTwoHandsModeOn = FALSE;
				params.m_bSwipesFromShortRange = TRUE;
				params.m_bVerticalSwipeDetectionOn = TRUE;
				params.m_bHorizontalSwipeDetectionOn = TRUE;
				params.m_bLikeDetectionOn = TRUE;
				params.m_bFistDetectionOn = TRUE;
				params.m_bMuteDetectionOn = TRUE;
				params.m_bFaceDetectionOn = TRUE;
				params.m_bPalmDetectionOn = TRUE;
				params.m_bIndexFingerDetectionOn = TRUE;
				VIDECHGR_SetParams(params);
			}
		}	
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CCaptureWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//GetParent()->PostMessage(MSG_SET_FULLWND,m_nIndex,0);

	m_bShowMax = !m_bShowMax;

	if (m_bShowMax)
	{
		ModifyStyle(WS_CAPTION,0);
		ShowWindow(SW_MAXIMIZE);
	}
	else
	{
		ModifyStyle(0,WS_CAPTION);
		ShowWindow(SW_RESTORE);
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CCaptureWnd::GetAVParameters(unsigned long& ulAudioID,unsigned long& ulVideoID)
{
	if (m_pEncode)
	{
		m_pEncode->GetAVParameters(ulAudioID,ulVideoID);
	}
}

void CCaptureWnd_CallbackDrawD3D9DC(const void*pUserData,void* hDC)
{
	CCaptureWnd*pCCaptureWnd=(CCaptureWnd*)pUserData;
	if (pCCaptureWnd)
	{
		pCCaptureWnd->OnCallbackDrawD3D9DC(hDC);
	}
}


void CCaptureWnd::OnCallbackDrawD3D9DC(void* hDC)
{
	SetTextColor((HDC)hDC, RGB(255,0,0));
	SetBkMode((HDC)hDC, TRANSPARENT);

	int nType = 0;
	RECT rc;rc.left = 10;rc.top = 30;rc.bottom = 60;rc.right = 300;
	if (m_pEncode)
		nType = m_pEncode->m_nEncoderType;

	CString strText;
	switch(nType)
	{
	case 0:
		strText.Format(_T("Unknown Encoder"));
		break;
	case 1:
		strText.Format(_T("SoftWare Encoder"));		
		break;
	case 2:
		strText.Format(_T("Intel HardWare Encoder"));		
		break;
	};

	DrawText((HDC)hDC,strText,-1,&rc, DT_LEFT|DT_TOP|DT_SINGLELINE);
}

bool CCaptureWnd::CreateRender(void)
{
	if (m_nShowType == VIDEO_SHOW_D3D)
	{
 		if (m_pWnd != NULL || m_pVidecD3D != NULL)
 		{
 			return false;
 		}		

		if (m_pAH2K != NULL || m_pCamera!=NULL)
		{
			m_pWnd=VIDEC_Wnd::CreateLocalVideo(*this);
			if (m_pWnd==NULL)
			{
				return false;
			}

			if (0!=m_pWnd->Open(GetSafeHwnd(),true))
			{
				m_pWnd->Close();
				delete m_pWnd;
				m_pWnd=NULL;

				return false;
			}
		}
		else
		{
			m_pVidecD3D = VIDECYNC_D3DPlayerOpen(CCaptureWnd_CallbackDrawD3D9DC,this,GetSafeHwnd(),1920,1080,true);
		}

		//SetWindowText(_T("Encode - VIDEO_SHOW_D3D"));

		CRect rect;
		GetClientRect(&rect);
		if (m_pWnd)
		{
			m_pWnd->SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
 		if (m_pVidecD3D)
 		{
 			VIDECYNC_D3DPlayerSetWindowPos(m_pVidecD3D,NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
 		}

	}
	/*else if (m_nShowType == VIDEO_SHOW_D3D_9 || m_nShowType == VIDEO_SHOW_D3D_11)
	{
		if (m_pD3DRender != NULL)
		{
			return false;
		}
		m_pD3DRender= HPAVC_D3DRender::Create(this);	
		if (m_pD3DRender == NULL)
		{
			return false;
		}		
		m_pD3DRender->Open(GetSafeHwnd(),true,m_nShowType==VIDEO_SHOW_D3D_11);

		if (m_nShowType == VIDEO_SHOW_D3D_9)
		{
			SetWindowText(_T("Encode - VIDEO_SHOW_D3D_9"));
		}
		else
		{
			SetWindowText(_T("Encode - VIDEO_SHOW_D3D_11"));
		}
	}
	else if (VIDEO_SHOW_DRSHOW == m_nShowType)
	{
		m_pDraw=new SDRDraw(1920,1080);
		if (0!=m_pDraw->Open(GetSafeHwnd(),1920,1080))
		{
			m_pDraw->Close();
			delete m_pDraw;
			m_pDraw=NULL;
		}

		SetWindowText(_T("Encode - VIDEO_SHOW_DRSHOW"));

	}
	*/
// 	else if (VIDEO_SHOW_XIS_LIB== m_nShowType)
// 	{
// 		if (m_pXISRender != NULL)
// 		{
// 			return false;
// 		}
// 		
// 		m_pXISRender = new XISRender();
// 		if (m_pXISRender->Open(GetSafeHwnd(), XI_COLOR_I420, 1920,1080, FALSE,TRUE) != 0)
// 		{
// 			return false;
// 		}
// 		SetWindowText(_T("Encode - VIDEO_SHOW_XIS_LIB"));
// 	}
	return true;
}

bool CCaptureWnd::Start(std::string strFileName,int nFrameRate,int nBitrate)
{

	TCHAR str[512];
	_stprintf_s(str, 512, _T("Encode"));
	//OutputDebugStr(str);
	SetWindowText(str);

	m_strFileName	= strFileName;
	m_nFrameRate	= nFrameRate;
	m_nBitrate		= nBitrate;

	CreateRender();
	
	if (m_bEncode && m_pEncode == NULL)
	{
		m_pEncode = new EncodeFrame();
		m_pEncode->Open(nFrameRate,nBitrate);
	}

	m_bWaitStop = false;
	StartThread();


	return true;
}

void CCaptureWnd::StartPGSDK(void)
{
	SetTimer(1,2000,NULL);
}

bool CCaptureWnd::StartCapture(int nDevID,int nWidth,int nHeight,int nFrameRate,int nBitrate)
{
	
	m_nLastTS=GetTickCount();
	m_nFrameCount=0;
	m_nSkipFrameCount=0;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nFrameRate	= nFrameRate;
	m_nBitrate		= nBitrate;

	if (nDevID >= AH2K_DEV_BEGIN)
	{
		if (NULL == m_pAH2K)
		{
			m_pAH2K = COpAH2K::Create(this);
		}
		if (m_pAH2K != NULL)
		{
			m_pAH2K->Open(nDevID-AH2K_DEV_BEGIN,nWidth,nHeight,nFrameRate);
		}
	}
	else
	{
		if (NULL == m_pCamera)
		{
			m_pCamera = COpVidecCamera::Create(this);
		}
		if (m_pCamera != NULL)
		{
			m_pCamera->Open(nDevID,nWidth,nHeight,nFrameRate,0);
		}
	}

	CreateRender();


	if (m_bEncode && m_pEncode == NULL)
	{
		m_pEncode = new EncodeFrame();
		m_pEncode->Open(nFrameRate,nBitrate);
	}

	m_bWaitStop = false;
	StartThread();

	//m_pFile = fopen("c:/wlj.yuv", "wb");


	return false;
}



void CCaptureWnd::ThreadProcMain(void)
{
	if (m_pAH2K != NULL || m_pCamera!=NULL)
	{
		Ah2kThreadProc();
		return;
	}

	mfxFrameSurface1 Surface;
	Surface.Info.FourCC=MFX_FOURCC_NV12;
	Surface.Info.Width=1920;
	Surface.Info.Height=1080;

	Surface.Info.CropX=0;
	Surface.Info.CropY=0;
	Surface.Info.CropW=0;
	Surface.Info.CropH=0;

	Surface.Data.Pitch=1920;
	Surface.Data.Y=NULL;
	Surface.Data.UV=NULL;

	unsigned char*pYUV420=(unsigned char*)malloc(1920*1080*2);

	int nYUV420=1920*1080*3/2;


	FILE*fSource=NULL;
	fSource = fopen(m_strFileName.c_str(), "rb");

	unsigned long ulCurTime  = GetTickCount();
	unsigned long ulLastTime  = ulCurTime;
	while(!m_bWaitStop && fSource && pYUV420)
	{
		int nBytes=fread(pYUV420,1,nYUV420,fSource);
		if (nBytes==nYUV420)
		{
//			if (m_pD3DRender && m_bShowVideo)
//			{				
//				int nY=1920*1080;
//				int nUV=nY/4;
//				unsigned char*pYUV[3];
//				pYUV[0]=(unsigned char*)pYUV420;
//				pYUV[1]=(unsigned char*)pYUV420+nY;
//				pYUV[2]=pYUV[1]+nUV;
//				m_pD3DRender->RenderFrame(pYUV,1920,1080,m_nFrameRate);
//			}
//
//			if (m_pDraw && m_bShowVideo)
//			{
//				m_pDraw->Draw(pYUV420,nBytes,1920,1080,m_nFrameRate);
//			}
//
//// 			if (m_pXISRender && m_bShowVideo) 
//// 			{
//// 				m_pXISRender->RenderFrame(pYUV420, 1920,1920,1080, m_nFrameRate);
//// 			}
//
 			if (m_pVidecD3D && m_bShowVideo)
 			{
 				VIDECYNC_D3DPlayerPlayFrame(m_pVidecD3D, pYUV420, nBytes, m_nFrameRate,1920,1080);
 			}
//
			if (m_pEncode)
			{
				m_pEncode->EncodeData(pYUV420,nYUV420);
			}

/*			VIDEC_Image*pXImageNew=VIDEC_Image::CreateAutoFree(pYUV420,nYUV420,1920,1080,m_nFrameRate,0);
			pXImageNew->AddRef();

			if (m_pWnd)
			{
				m_pWnd->OnReceivedImage(pXImageNew);
			}

			if (m_pEncode)
			{
				m_pEncode->EncodeImage(pXImageNew);
			}

			pXImageNew->Release();
			pXImageNew=NULL;	*/		
		}
		else
		{
			fseek(fSource,0,0);
		}

		ulLastTime = ulCurTime;
		ulCurTime = timeGetTime();
		//TRACE(_T("ulCurTime = %u ulLastTime = %u ulCurTime - ulLastTime = %u\n"),ulCurTime,ulLastTime,ulCurTime - ulLastTime);
		if ( (ulCurTime - ulLastTime) < 10)
		{
			Sleep(1);
		}

		//Sleep(1000/(m_nFrameRate+2));
	}

	if (fSource)
	{
		fclose(fSource);
		fSource = NULL;
	}
	if (pYUV420)
	{
		free(pYUV420);
		pYUV420 = NULL;
	}

}

void CCaptureWnd::Ah2kThreadProc()
{
	while (!m_bWaitStop)
	{
		VIDEC_Image*pImage=NULL;
		{
			XAutoLock l(m_csListImage);
			int nSize = m_listImage.size();
			if (nSize)
			{
				pImage=(VIDEC_Image*)m_listImage.front();
				m_listImage.pop_front();
			}
		}
		if (pImage)
		{
			if (m_pWnd)
			{
				m_pWnd->OnReceivedImage(pImage);
			}

			pImage->Release();
			pImage=NULL;
		}
		else
		{
			Sleep(1);
		}

//		int nSize = 0;
//		VIDEC_Image*pImage=NULL;
//		{
//			XAutoLock l(m_csListImage);
//			nSize = m_listImage.size();
//			if (nSize > 6 && m_pVidecD3D == NULL)
//			{				
//				do
//				{
//					pImage=(VIDEC_Image*)m_listImage.front();
//					m_listImage.pop_front();
//					if (pImage)
//					{			
//						pImage->Release();
//						pImage=NULL;
//					}
//					m_nSkipFrameCount++;
//				}while (m_listImage.size() > 3);
//			}
//			if (nSize)
//			{
//				pImage=(VIDEC_Image*)m_listImage.front();
//				m_listImage.pop_front();
//			}
//		}
//		if (nSize > 6 && m_pVidecD3D)
//		{
//			VIDECYNC_D3DPlayerSkipDisplay(m_pVidecD3D,1);
//			m_nSkipFrameCount++;
//		}		
//
//		if (pImage)
//		{
//// 			if (m_pFile)
//// 			{
//// 				fwrite((unsigned char*)pImage->GetData(),1,1920*1080*3/2,m_pFile);
//// 			}
//
//			int nY=pImage->GetWidth()*pImage->GetHeight();
//			int nUV=nY/4;
//			unsigned char* pYUV420 = (unsigned char*)(pImage->GetData());
//			if (m_pD3DRender && m_bShowVideo)
//			{
//				unsigned char*pYUV[3];
//				pYUV[0]=(unsigned char*)pYUV420;
//				pYUV[1]=(unsigned char*)pYUV420+nY;
//				pYUV[2]=pYUV[1]+nUV;
//				m_pD3DRender->RenderFrame(pYUV,pImage->GetWidth(),pImage->GetHeight(),pImage->GetFrameRate());
//			}
//
//			if (m_pDraw && m_bShowVideo)
//			{
//				m_pDraw->Draw(pYUV420,pImage->GetDataLen(),pImage->GetWidth(),pImage->GetHeight(),pImage->GetFrameRate());
//			}
//
//// 			if (m_pXISRender && m_bShowVideo) 
//// 			{
//// 				m_pXISRender->RenderFrame(pYUV420, pImage->GetWidth(),pImage->GetWidth(),pImage->GetHeight(), pImage->GetFrameRate());
//// 			}
// 			
// 			if (m_pVidecD3D && m_bShowVideo)
// 			{
// 				VIDECYNC_D3DPlayerPlayFrame(m_pVidecD3D, pYUV420, pImage->GetDataLen(), pImage->GetFrameRate(),pImage->GetWidth(),pImage->GetHeight());
// 			}
//			pImage->Release();
//			pImage=NULL;
//		}
//		else
//		{
//			Sleep(1);
//		}
	}

	{
		XAutoLock l(m_csListImage);
		while (m_listImage.size())
		{
			VIDEC_Image* pImage=(VIDEC_Image*)m_listImage.front();
			m_listImage.pop_front();
			pImage->Release();
			pImage=NULL;
		}
	}
}

//---------------------------------------------------------------------------------------
// 停止服务
void CCaptureWnd::Stop(void)
{
	m_bWaitStop = true;
	WaitForStop();

	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	CCaptureWnd*pWnd = (CCaptureWnd*)VIDECHGR_GetUserData();
	if (pWnd == this)
	{
		VIDECHGR_Stop();
	}

	if (m_pD3DRender)
	{
		m_pD3DRender->Close();
		delete m_pD3DRender;
		m_pD3DRender = NULL;
	}
	if (m_pDraw)
	{
		m_pDraw->Close();
		delete m_pDraw;
		m_pDraw=NULL;
	}
// 	if (m_pXISRender) {
// 		m_pXISRender->Close();
// 		delete m_pXISRender;
// 		m_pXISRender = NULL;
// 	}
 	//if (m_pVidecD3D)
 	//{
 	//	VIDECYNC_D3DPlayerClose(m_pVidecD3D);
 	//	m_pVidecD3D = NULL;
 	//}

	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}

	if (m_pAH2K)
	{
		m_pAH2K->Close();
		delete m_pAH2K;
		m_pAH2K = NULL;
	}
	if (m_pCamera)
	{
		m_pCamera->Close();
		delete m_pCamera;
		m_pCamera = NULL;
	}



	if (m_pEncode)
	{
		m_pEncode->Close();
		delete m_pEncode;
		m_pEncode = NULL;
	}
}

void CCaptureWnd::OnOpAH2KCallbackImage(VIDEC_Image*pImage)
{
#if (0)
	{
		m_nFrameCount++;
		unsigned long ulTime = timeGetTime();
		if (ulTime-m_nLastTS > 20000)
		{
			char szDebug[1024];
			sprintf(szDebug,"CCaptureWnd::OnOpAH2KCallbackImage nFrameCount=%u SkipFrameCount=%u  this = %u listPacketData.size=%u\n",m_nFrameCount,m_nSkipFrameCount,this,m_listImage.size());
			OutputDebugStringA(szDebug);
			m_nSkipFrameCount=0;
			m_nFrameCount=0;
			m_nLastTS=ulTime;
		}
	}
#endif`	

	if(m_bShowVideo)
	{
		XAutoLock l(m_csListImage);
		pImage->AddRef();
		m_listImage.push_back(pImage);
	}

	if (m_pEncode)
	{
		m_pEncode->EncodeImage(pImage);
	}

	m_nWidth = pImage->GetWidth();
	m_nHeight= pImage->GetHeight();

	CCaptureWnd*pWnd = (CCaptureWnd*)VIDECHGR_GetUserData();
	if (pWnd == this)
	{
		VIDECHGR_DetectFrame((unsigned char *)pImage->GetData(),pImage->GetDataLen(), pImage->GetWidth(), pImage->GetHeight());
	}
}

void CCaptureWnd::OnOpAH2KCallbackAudioData(short*pSamples,int nSamples,int nSampleRate)
{

}

void CCaptureWnd::OnOpVidecCameraaCallbackImageData( VIDEC_Image*pImage )
{
	OnOpAH2KCallbackImage(pImage);
}

void CCaptureWnd::VideoParamNotifyNewParam(Video_Param param)
{
	m_param = param;
	if (m_pEncode)
	{
		m_pEncode->SetVideoParam(param);
	}
}

void CCaptureWnd::OnRButtonDown(UINT nFlags, CPoint point)
{

	CDlgVideoParam dlg(*this,m_param);
	dlg.DoModal();

	__super::OnRButtonDown(nFlags, point);
}


void CCaptureWnd::SetVideoShow(bool bShowVideo)
{
	m_bShowVideo = bShowVideo;	
}

void CCaptureWnd::OnPG_SDKCallBackResult(PG_SDK_FrameResultC const & FrameResult)
{
	PGShortRangeSwipeDirectionC SwipeDir = PG_C_SWIPE_DIRECTION_INVALID;
	BOOL bIsLike = FALSE;
	BOOL bIsFist = FALSE;
	BOOL bIsMute = FALSE;
	int nFrameIndex = FrameResult.m_FrameIndex;
	SwipeDir = FrameResult.m_ShortRangeResult.m_SwipeDirection;
	bIsLike = FrameResult.m_bIsLikeGesture;
	bIsFist = FrameResult.m_bIsFistGesture;
	bIsMute = FrameResult.m_bIsMuteGesture;
	// Update text for tracking:
	if (FrameResult.m_PrimaryPalmResult.m_bIsValid)
	{
			CString strTrackText;
		strTrackText.Format(_T("OnPG_SDKCallBackResult::[1: %d,%d  s:%4.2f %s %s]"), FrameResult.m_PrimaryPalmResult.m_CamFrameRefPointX,
			FrameResult.m_PrimaryPalmResult.m_CamFrameRefPointY,
			FrameResult.m_PrimaryPalmResult.m_Scale,
			FrameResult.m_PrimaryPalmResult.m_bGrabValid ? _T("GRAB") : _T(""),
			FrameResult.m_PrimaryPalmResult.m_bZoomValid ? _T("ZOOM") : _T("nozoom"));
		if (FrameResult.m_SecondaryPalmResult.m_bIsValid)
		{
			CString tmp;
			tmp.Format(_T("  [2: %d,%d  %s]"), FrameResult.m_SecondaryPalmResult.m_CamFrameRefPointX,
				FrameResult.m_SecondaryPalmResult.m_CamFrameRefPointY,
				FrameResult.m_SecondaryPalmResult.m_bGrabValid ? _T("GRAB") : _T(""));
			strTrackText += tmp;
		}
		if (FrameResult.m_TwoHandsExtendedInfo.m_bIsValid)
		{
			CString tmp;
			tmp.Format(_T("  [a:%4.2f d:%4.2f]"), FrameResult.m_TwoHandsExtendedInfo.m_RotationAngle, 
				FrameResult.m_TwoHandsExtendedInfo.m_Distance);
			strTrackText += tmp;
		}
		strTrackText += _T("\n");
		OutputDebugString(strTrackText);
	}

	// Add messages for short range result:
	if (SwipeDir != PG_C_SWIPE_DIRECTION_INVALID)
	{
		CString SwipeDirStr = _T("<UNKNOWN>");
		switch (SwipeDir)
		{
		case PG_C_SWIPE_DIRECTION_RIGHT_TO_LEFT: SwipeDirStr = _T("<-   (Right2Left)"); break;
		case PG_C_SWIPE_DIRECTION_LEFT_TO_RIGHT: SwipeDirStr = _T("->   (Left2Right)"); break;
		case PG_C_SWIPE_DIRECTION_BOTTOM_TO_TOP: SwipeDirStr = _T("^    (Bottom2Top)"); break;
		case PG_C_SWIPE_DIRECTION_TOP_TO_BOTTOM: SwipeDirStr = _T("v    (Top2Bottom)"); break;
		case PG_C_SWIPE_DIRECTION_TOP_THEN_BOTTOM: SwipeDirStr = _T("^V (TopThenBottom)"); break;
		default: break;
		}
		CString Msg = _T("OnPG_SDKCallBackResult::Swipe detected: ") + SwipeDirStr;
		Msg += _T("\n");
		OutputDebugString(Msg);
	}

	if (bIsLike)
	{
		OutputDebugString(_T("OnPG_SDKCallBackResult::'Like' detected !\n"));
	}

	if (bIsFist)
	{
		OutputDebugString(_T("OnPG_SDKCallBackResult::'Fist' detected !\n"));
	}

	if (bIsMute)
	{
		OutputDebugString(_T("OnPG_SDKCallBackResult::'Mute' detected !\n"));
	}
}