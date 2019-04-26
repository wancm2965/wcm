
#include "D3DRender.h"

#include <tchar.h>
#include <wingdi.h>

#include <windowsx.h>
#include <dwmapi.h>
#include <mmsystem.h>


//#include "../../jpeglib/jmorecfg.h"
#include "../../jpeglib/jpegutils.h"

#pragma warning(disable : 4100)

#define HPAVC_D3D_REPAINT		(WM_USER+1010)


bool	g_bRegisterClass = false;


static int			g_nAdapterNum = 0;
static mfxIMPL		g_nAdapterIMPL = MFX_IMPL_UNSUPPORTED;
static bool			g_bInitialized=false;
static mfxStatus	g_nInitStatus = MFX_ERR_NONE;

int GetIntelAdapterNum(void)
{
	MFX_Init();
	return g_nAdapterNum;
}

mfxIMPL GetIntelAdapterIMPL(void)
{
	MFX_Init();
	return g_nAdapterIMPL;
}

mfxStatus MFX_Init(void)
{
	if (!g_bInitialized)
	{
		g_nAdapterNum = 0;
		g_nAdapterIMPL = MFX_IMPL_HARDWARE;
		{
			mfxSession auxSession;
			mfxVersion ver = { {0, 1 }};
			g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
			MFXClose(auxSession);
		}
		if (g_nInitStatus != MFX_ERR_NONE)
		{
			g_nAdapterNum = 1;
			g_nAdapterIMPL = MFX_IMPL_HARDWARE2;
			{
				mfxSession auxSession;
				mfxVersion ver = { {0, 1 }};
				g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
				MFXClose(auxSession);
			}
			if (g_nInitStatus != MFX_ERR_NONE)
			{
				g_nAdapterNum = 2;
				g_nAdapterIMPL = MFX_IMPL_HARDWARE3;
				{
					mfxSession auxSession;
					mfxVersion ver = { {0, 1 }};
					g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
					MFXClose(auxSession);
				}				
				if (g_nInitStatus != MFX_ERR_NONE)
				{
					g_nAdapterNum = 3;
					g_nAdapterIMPL = MFX_IMPL_HARDWARE4;
					{
						mfxSession auxSession;
						mfxVersion ver = { {0, 1 }};
						g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
						MFXClose(auxSession);
					}
					//if (g_nInitStatus != MFX_ERR_NONE)
					//{
					//	g_nAdapterNum = 4;
					//	g_nAdapterIMPL = MFX_IMPL_HARDWARE5;
					//	{
					//		mfxSession auxSession;
					//		mfxVersion ver = { {0, 1 }};
					//		g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
					//		MFXClose(auxSession);
					//	}
					//}
				}				
			}
		}
		if (g_nInitStatus != MFX_ERR_NONE)
		{
			g_nAdapterNum=0;
			g_nAdapterIMPL=MFX_IMPL_UNSUPPORTED;
		}
		DebugOutput( logcallstack ,"MFX_Init: g_nAdapterNum = %d g_nAdapterIMPL =%d\n",g_nAdapterNum,g_nAdapterIMPL);
		g_bInitialized = true;
	}
	return g_nInitStatus;
}


void*HPAVC_D3DRenderOpen(HPAVC_D3DRENDER_CB_DC cbdc,void*pUserData,void* hWnd,bool bCreateWnd,bool bD3D11)
{
	NEWRECORD3( CD3DRender*,pCD3DRender , new CD3DRender(cbdc,pUserData) )
	if (pCD3DRender)
	{
		pCD3DRender->Open(hWnd, bCreateWnd,bD3D11);
	}
	return pCD3DRender;
}

void HPAVC_D3DRenderClose(void*pHandle)
{
	CD3DRender*pCD3DRender = (CD3DRender *)pHandle;  
	if (pCD3DRender)
	{
		pCD3DRender->Close();
		delete pCD3DRender;
		pCD3DRender = NULL;
	}
}
int  HPAVC_D3DRenderPlayFrame(void*pHandle, unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate)
{
	CD3DRender*pCD3DRender = (CD3DRender *)pHandle;  
	if (pCD3DRender)
	{
		return pCD3DRender->RenderFrame(pYV12Data,nWidth, nHeight,  nFrameRate);
	}
	return 0;
}
int  HPAVC_D3DRenderPlaySurface(void*pHandle, mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc)
{
	CD3DRender*pCD3DRender = (CD3DRender *)pHandle;  
	if (pCD3DRender)
	{
		return pCD3DRender->RenderFrameSurface(pSurface,pmfxAlloc);
	}
	return 0;
}
void* HPAVC_D3DRenderGetWnd(void*pHandle)
{
	CD3DRender*pCD3DRender = (CD3DRender *)pHandle;  
	if (pCD3DRender)
	{
		return pCD3DRender->GetWnd();
	}
	return NULL;
}
bool HPAVC_D3DRenderSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	CD3DRender*pCD3DRender = (CD3DRender *)pHandle;  
	if (pCD3DRender)
	{
		return pCD3DRender->SetWindowPos(hWndInsertAfter/*HWND*/, x, y, cx, cy, nFlags);
	}
	return false;
}
int  HPAVC_D3DRenderSkipDisplay(void*pHandle,int nSkipCount/*本次跳帧的数量*/)
{
	CD3DRender*pCD3DRender = (CD3DRender *)pHandle;  
	if (pCD3DRender)
	{
		pCD3DRender->SetSkipDisplay(nSkipCount);
	}
	return 0;
}
bool HPAVC_D3DRenderCaptureToBMPFile(void*pHandle,const char*cszFile)
{
	CD3DRender*pCD3DRender = (CD3DRender *)pHandle;  
	if (pCD3DRender)
	{
		return pCD3DRender->CaptureToBMPFile(cszFile);
	}
	return false;
}

HPAVC_D3DRender*HPAVC_D3DRender::Create(HPAVC_D3DRenderCallback*pCallback)
{
	return new CD3DRender(pCallback);
}

LRESULT CALLBACK HPD3DRenderWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	CD3DRender *pRenderWindow = (CD3DRender *)GetWindowLongPtr(hwnd,0);
	if (pRenderWindow != NULL)
	{
		return pRenderWindow->OnReceiveMessage(hwnd,msg,wParam,lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

CD3DRender::CD3DRender(HPAVC_D3DRENDER_CB_DC cbdc,void*pUserData)
:m_pCallback(NULL)
,m_cbdc(cbdc)
,m_pUserData(pUserData)
,m_hwdev(NULL)
,m_hWnd(NULL)
,m_pSurfRenderder(NULL)
,m_pMFXAllocator(NULL)
,m_pmfxAllocatorParams(NULL)
,m_nSkipCount(0)
,m_hBrush(NULL)
,m_bD3D11(false)
{
	m_nWndWidth		= 176;
	m_nWndHeight	= 144;
	MSDK_ZERO_MEMORY(m_EncResponse);

	m_bDwmEnabled = false;
	m_nFrameRate = 0;

	m_strCaptureFilePath = "";
}

CD3DRender::CD3DRender(HPAVC_D3DRenderCallback*pCallback)
:m_pCallback(pCallback)
,m_cbdc(NULL)
,m_pUserData(NULL)
,m_hwdev(NULL)
,m_hWnd(NULL)
,m_pSurfRenderder(NULL)
,m_pMFXAllocator(NULL)
,m_pmfxAllocatorParams(NULL)
,m_nSkipCount(0)
,m_hBrush(NULL)
,m_bD3D11(false)
{
	m_nWndWidth		= 176;
	m_nWndHeight	= 144;

	MSDK_ZERO_MEMORY(m_EncResponse);

	m_bDwmEnabled = false;
	m_nFrameRate = 0;
	m_strCaptureFilePath = "";
}
CD3DRender::~CD3DRender()
{
}

//hWnd:视频显示窗口句柄，成功返回0，失败返回-1
int CD3DRender::Open(void* hWnd/*HWND*/,bool bCreateWnd,bool bD3D11)
{
	m_pSurface = NULL;
	m_pmfxAlloc= NULL;

#if MFX_D3D11_RENDER_SUPPORT
#else
 	bD3D11 = false;
 #endif

	m_bD3D11   = bD3D11;
	 m_hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	if (hWnd !=NULL)
	{
		if (bCreateWnd == false)
		{
			m_hWnd = (HWND)hWnd;
		}
		else
		{
			if (g_bRegisterClass == false)
			{
				WNDCLASS wndclass;
				wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
				wndclass.lpfnWndProc = HPD3DRenderWndProc;
				wndclass.cbClsExtra = 0;
				wndclass.cbWndExtra    = sizeof(CD3DRender *);
				wndclass.hInstance = NULL;
				wndclass.hIcon = NULL;
				wndclass.hCursor = NULL;
				wndclass.hbrBackground = NULL;
				wndclass.lpszMenuName = NULL;
				wndclass.lpszClassName = _T("HPD3DRenderClass");
				if (!RegisterClass(&wndclass)) {
					//OutputDebugStr(_T("Registering HPD3DRenderClass window failed"));
					return NULL;
				}
				g_bRegisterClass = true;
			}
			//m_hWnd = CreateWindow(_T("HPD3DRenderClass"), _T("HP D3DRender Window"), WS_CHILD|WS_DISABLED, 0, 0, 0, 0, (HWND)hWnd, NULL, NULL, NULL);
			m_hWnd = CreateWindowEx(0,_T("HPD3DRenderClass"), _T("HP D3DRender Window"), WS_CHILD|WS_DISABLED, 0, 0,1920, 1080, (HWND)hWnd, NULL, NULL, NULL);
			
		}
		if (m_hWnd == NULL)
		{
			return -1;
		}

		CD3DRender *pRenderWindow = this;
		SetWindowLongPtr(m_hWnd,0,(unsigned long)pRenderWindow);
	}
	mfxStatus sts = MFX_ERR_NONE;

#if MFX_D3D11_RENDER_SUPPORT
	if (m_bD3D11)
	{
		NEWRECORD( m_hwdev , new CD3D11Device(*this,false) )
	}
	else
#endif
	{
		NEWRECORD( m_hwdev , new CD3D9Device(*this) )
	}

	if (m_hwdev)
	{
		sts = m_hwdev->Init(m_hWnd);
		if (MFX_ERR_NONE > sts)
		{			
			delete m_hwdev;
			m_hwdev = NULL;
			if (m_bD3D11)
			{
				m_bD3D11 = false;
				NEWRECORD( m_hwdev , new CD3D9Device(*this) )
				if (m_hwdev)
				{
					sts = m_hwdev->Init(m_hWnd);
					if (MFX_ERR_NONE > sts)
					{			
						delete m_hwdev;
						m_hwdev = NULL;
					}
				}
			}
		}
	}
	if (m_hwdev == NULL)
	{
		return sts;
	}
	if (!m_bD3D11)
	{
		//FrameRateEvent::Open(nFrameRate);
	}
	m_nLastTS=GetTickCount();
	m_nFrameCount=0;
	m_nSkipFrameCount=0;

// 	m_bWaitStop = false;
// 	StartThread();

	return sts;
}


LRESULT CD3DRender::OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	if(!IsWindow(m_hWnd))
		return 0;
	switch (uMsg) {
	case WM_ERASEBKGND:
		{
			if (m_pSurface == NULL)
			{
				RECT rc;
				GetClientRect(m_hWnd,&rc);
				HDC pDC = GetDC(m_hWnd);
				if (pDC)
				{
					FillRect(pDC,&rc,m_hBrush);
					ReleaseDC(m_hWnd,pDC);
				}

			}
			else if (m_hwdev)
			{
				m_hwdev->RenderFrame(m_pSurface,m_pmfxAlloc,false);
			}
			return (LRESULT) 0;
		}
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}


//关闭显示窗口
void CD3DRender::Close(void)
{
	//m_bWaitStop = true;
	//WaitForStop();

	//FrameRateEvent::Close();	

	if (m_pSurfRenderder)
	{
		delete m_pSurfRenderder;
		m_pSurfRenderder = NULL;
	}
	// delete frames
	if (m_pMFXAllocator)
	{
		m_pMFXAllocator->Free(m_pMFXAllocator->pthis, &m_EncResponse);
		delete m_pMFXAllocator;
		m_pMFXAllocator = NULL;
	}

	MSDK_SAFE_DELETE(m_pmfxAllocatorParams);


	if (m_hwdev)
	{
		delete m_hwdev;
		m_hwdev = NULL;
	}
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);
		m_hBrush = NULL;
	}

	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	{
		XAutoLock l(m_csListPacketData);
		while (m_listPacketData.size())
		{
			XSurfaceBuffer*pXPacketBuffer=NULL;
			pXPacketBuffer=(XSurfaceBuffer*)m_listPacketData.front();
			m_listPacketData.pop_front();
			if (pXPacketBuffer)
			{			
				delete pXPacketBuffer;
				pXPacketBuffer=NULL;
			}
		}
	}
}

void* CD3DRender::GetWnd(void)
{
	return m_hWnd;
}
//设置窗口位置
bool CD3DRender::SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	m_nWndWidth		= cx;
	m_nWndHeight	= cy;

	nFlags|=SWP_SHOWWINDOW;

	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd,(HWND)hWndInsertAfter,x,y,cx,cy,nFlags);
	}
	return true;
} 

mfxStatus CD3DRender::LoadNextFrame(mfxFrameSurface1* pSurface,unsigned char*pYV12Data[])
{
	MSDK_CHECK_POINTER(pSurface, MFX_ERR_NULL_PTR);

	mfxU16 w, h, i, pitch;
	mfxU8 *ptr, *ptr2;
	mfxFrameInfo* pInfo = &pSurface->Info;
	mfxFrameData* pData = &pSurface->Data;

	// this reader supports only NV12 mfx surfaces for code transparency, 
	// other formats may be added if application requires such functionality
	if (MFX_FOURCC_NV12 != pInfo->FourCC && MFX_FOURCC_YV12 != pInfo->FourCC)  
	{
		return MFX_ERR_UNSUPPORTED;
	}

	if (pInfo->CropH > 0 && pInfo->CropW > 0) 
	{
		w = pInfo->CropW;
		h = pInfo->CropH;
	} 
	else 
	{
		w = pInfo->Width;
		h = pInfo->Height;
	}

	mfxU8*ptrY=(mfxU8*)pYV12Data[0];
	mfxU8*ptrU=(mfxU8*)pYV12Data[1];
	mfxU8*ptrV=(mfxU8*)pYV12Data[2];

	pitch = pData->Pitch;
	ptr = pData->Y + pInfo->CropX + pInfo->CropY * pData->Pitch;


	// read luminance plane
 	for (i=0;i<h;i++)
 	{
 		memcpy(ptr,ptrY,w);
 		ptr+=pitch;
 		ptrY+=w;
 	}

	// read chroma planes
	switch (pInfo->FourCC)
	{
	case MFX_FOURCC_NV12:

		mfxU32 j;
		w /= 2;
		h /= 2;            
		ptr = pData->UV + pInfo->CropX + (pInfo->CropY / 2) * pitch;
		// load U
		// load V
		for (i = 0; i < h; i++) 
		{
			for (j = 0; j < w; j++)
			{
				ptr[i * pitch + j * 2] = ptrU[j];
				ptr[i * pitch + j * 2 + 1] = ptrV[j];
			}
			ptrU+=w;
			ptrV+=w;
		}
		break;

	case MFX_FOURCC_YV12:
		w /= 2;
		h /= 2;
		pitch /= 2;

		ptr  = pData->U + (pInfo->CropX / 2) + (pInfo->CropY / 2) * pitch;
		ptr2 = pData->V + (pInfo->CropX / 2) + (pInfo->CropY / 2) * pitch;

		for (i=0;i<h;i++)
		{
			memcpy(ptr,ptrU,w);
			memcpy(ptr2,ptrV,w);
			ptrU+=w;
			ptrV+=w;
			ptr+=pitch;
			ptr2+=pitch;
		}

		break;
	default:
		return MFX_ERR_UNSUPPORTED;
	}

	return MFX_ERR_NONE;    
}

mfxStatus CD3DRender::CreateSurfRenderder(void)
{
	mfxStatus sts = MFX_ERR_NONE;
	if (m_pSurfRenderder==NULL && m_hwdev)
	{
		NEWRECORD( m_pSurfRenderder ,new mfxFrameSurface1 )
		memset(m_pSurfRenderder, 0, sizeof(mfxFrameSurface1));
		m_pSurfRenderder->Info.PicStruct	= MFX_PICSTRUCT_FIELD_BFF;
		m_pSurfRenderder->Info.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_pSurfRenderder->Info.FourCC		= MFX_FOURCC_NV12;
#if MFX_D3D11_RENDER_SUPPORT
		if (m_bD3D11)
		{
			NEWRECORD( m_pMFXAllocator , new D3D11FrameAllocator )
			MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);
			mfxHDL hdl = NULL;
			mfxHandleType hdl_t =MFX_HANDLE_D3D11_DEVICE;
			sts = m_hwdev->GetHandle(hdl_t, &hdl);
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			NEWRECORD3( D3D11AllocatorParams *,pd3dAllocParams , new D3D11AllocatorParams )
			MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
			pd3dAllocParams->pDevice = reinterpret_cast<ID3D11Device *>(hdl);
			m_pmfxAllocatorParams = pd3dAllocParams;
		}
		else
#endif
		{
			NEWRECORD( m_pMFXAllocator , new D3DFrameAllocator )
			MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);
			mfxHDL hdl = NULL;
			mfxHandleType hdl_t =MFX_HANDLE_D3D9_DEVICE_MANAGER;
			sts = m_hwdev->GetHandle(hdl_t, &hdl);
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			NEWRECORD3( D3DAllocatorParams*,pd3dAllocParams , new D3DAllocatorParams )
			MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
			pd3dAllocParams->pManager = reinterpret_cast<IDirect3DDeviceManager9 *>(hdl);
			m_pmfxAllocatorParams = pd3dAllocParams;
		}
		// initialize memory allocator
		sts = m_pMFXAllocator->Init(m_pmfxAllocatorParams);

		mfxFrameAllocRequest EncRequest;
		MSDK_ZERO_MEMORY(EncRequest);

		MSDK_MEMCPY_VAR(EncRequest.Info, &m_pSurfRenderder->Info, sizeof(mfxFrameInfo));	

		EncRequest.NumFrameMin = 1;
		EncRequest.NumFrameSuggested = 1;
		EncRequest.Info.CropH		= 1080;
		EncRequest.Info.CropW		= 1920;
		EncRequest.Info.Width		= 1920;
		EncRequest.Info.Height		= 1088;
		EncRequest.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_VPPIN | MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;
		// alloc frames for encoder
		sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &EncRequest, &m_EncResponse);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		m_pSurfRenderder->Info.FrameId.ViewId = 0;
		m_pSurfRenderder->Data.MemId = m_EncResponse.mids[0];
	}
	return sts;
}

//保持当前图象到BMP文件
bool CD3DRender::CaptureToBMPFile(const char*cszFile)
{
	m_strCaptureFilePath = cszFile;
	return true;
	//if (m_pSurface == NULL || m_pmfxAlloc == NULL)
	//{
	//	return false;
	//}
	//bool bR = false;


	//return bR;
}

void CD3DRender::OnCD3D9DeviceCallbackDraw(HDC hDC)
{
	if (m_pCallback)
	{
		m_pCallback->OnHPAVC_D3DRenderCallbackDrawDC(hDC);
	}
	if (m_cbdc)
	{
		m_cbdc(m_pUserData,hDC);
	}
}

void CD3DRender::SetSkipDisplay(int nSkipCount)
{
	m_nSkipCount = nSkipCount;
}

bool CD3DRender::EnableDwmQueuing(int nFrameRate)
{
	HRESULT hr;

	// DWM queuing is enabled already.
	if (m_bDwmEnabled)
	{
		if (m_nFrameRate != nFrameRate)
		{
			m_bDwmEnabled = false;
		}
		else
		{
			return true;
		}		
	}

	// Check to see if DWM is currently enabled.
	BOOL bDWM = FALSE;

	hr = DwmIsCompositionEnabled(&bDWM);

	if (FAILED(hr))
	{
		DebugOutput( logcallstack ,"DwmIsCompositionEnabled failed with error 0x%x.\n", hr);
		return false;
	}

	// DWM queuing is disabled when DWM is disabled.
	if (!bDWM)
	{
		m_bDwmEnabled = false;
		return false;
	}

	// Retrieve DWM refresh count of the last vsync.
	DWM_TIMING_INFO dwmti = {0};

	dwmti.cbSize = sizeof(dwmti);

	hr = DwmGetCompositionTimingInfo(NULL, &dwmti);

	if (FAILED(hr))
	{
		DebugOutput( logcoreexcep ,"CD3DRender_%d::EnableDwmQueuing FAILED DwmGetCompositionTimingInfo  error = 0x%x.nFrameRate = %u this = %u\n", m_bD3D11?11:9,hr,nFrameRate,this);
		return false;
	}

	// Enable DWM queuing from the next refresh.
	DWM_PRESENT_PARAMETERS dwmpp = {0};
	dwmpp.cbSize                    = sizeof(dwmpp);
	dwmpp.fQueue                    = TRUE;
	dwmpp.cRefreshStart             = dwmti.cRefresh + 1;
	dwmpp.cBuffer                   = 6; //maximum depth of DWM queue
	dwmpp.fUseSourceRate            = TRUE;
	dwmpp.cRefreshesPerFrame        = 1;
	dwmpp.eSampling                 = DWM_SOURCE_FRAME_SAMPLING_POINT;
	dwmpp.rateSource.uiDenominator  = 1;
	dwmpp.rateSource.uiNumerator    = nFrameRate;
	hr = DwmSetPresentParameters(m_hWnd, &dwmpp);

	if (FAILED(hr))
	{
		DebugOutput( logcoreexcep ,"CD3DRender_%d::EnableDwmQueuing FAILED DwmSetPresentParameters error = 0x%x.nFrameRate = %u this = %u\n", m_bD3D11?11:9,hr,nFrameRate,this);
		return false;
	}

	// DWM queuing is enabled.
	m_bDwmEnabled = true;

	m_nFrameRate = nFrameRate;

	DebugOutput( logcallstack ,"CD3DRender_%d::EnableDwmQueuing SUCCESS nFrameRate = %u this = %u\n",m_bD3D11?11:9,nFrameRate,this);

	return true;
}

//显示
int CD3DRender::RenderFrame(unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate)
{
	if (m_hWnd == NULL)
	{
		return 0;
	}

	if (m_strCaptureFilePath.length() > 0)
	{
		WriteYUVBytesIntoJpegFile(m_strCaptureFilePath.c_str(),nWidth,nHeight,100,pYV12Data[0],pYV12Data[1],pYV12Data[2]);
		m_strCaptureFilePath = "";
	}

	mfxStatus sts = MFX_ERR_NONE;
	if (m_pSurfRenderder==NULL)
	{
		sts = CreateSurfRenderder();
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}
	m_pSurfRenderder->Info.CropH		= nHeight;
	m_pSurfRenderder->Info.CropW		= nWidth;
	m_pSurfRenderder->Info.Width		= MSDK_ALIGN16(nWidth);
	m_pSurfRenderder->Info.Height		= MSDK_ALIGN16(nHeight);
	ConvertFrameRate(nFrameRate, &m_pSurfRenderder->Info.FrameRateExtN, &m_pSurfRenderder->Info.FrameRateExtD);

	sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, m_pSurfRenderder->Data.MemId, &(m_pSurfRenderder->Data));
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	sts = LoadNextFrame(m_pSurfRenderder,pYV12Data);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, m_pSurfRenderder->Data.MemId, &(m_pSurfRenderder->Data));
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return RenderFrameSurface(m_pSurfRenderder,m_pMFXAllocator);
}

int CD3DRender::RenderFrameSurface(mfxFrameSurface1* pSurface)
{
	if (m_hWnd == NULL)
	{
		return 0;
	}
	NEWRECORD3( XSurfaceBuffer*,pXPacketBuffer,new XSurfaceBuffer )
	if (pXPacketBuffer)
	{
		mfxU16 w, h, i,j, pitch;
		mfxFrameInfo* pInfo		= &pSurface->Info;
		mfxFrameData* pSrcData	= &pSurface->Data;
		mfxU8 *ptr, *ptr2;

		pitch = pSrcData->Pitch;
		pXPacketBuffer->nYLen =(pInfo->CropY * pitch + pInfo->CropX+ (pInfo->CropH - 1) * pitch) + pInfo->CropW;
		pXPacketBuffer->nUVLen = (pInfo->CropH/2-1)*pitch + (pInfo->CropW-1)*2+2 + pInfo->CropX + (pInfo->CropY / 2) * pitch;

		pXPacketBuffer->pYUVData=MALLOCMSG(pXPacketBuffer->nYLen + pXPacketBuffer->nUVLen+2);
		if (pXPacketBuffer->pYUVData)
		{
			memcpy((char*)pXPacketBuffer->pYUVData,pSrcData->Y,pXPacketBuffer->nYLen);			
			memcpy((char*)pXPacketBuffer->pYUVData+pXPacketBuffer->nYLen,pSrcData->UV,pXPacketBuffer->nUVLen);
			pXPacketBuffer->nPitch=pitch;
			pXPacketBuffer->CropW=pInfo->CropW;
			pXPacketBuffer->CropH=pInfo->CropH;
			pXPacketBuffer->CropX=pInfo->CropX;
			pXPacketBuffer->CropY=pInfo->CropY;
			pXPacketBuffer->nFrameRate=pInfo->FrameRateExtN/pInfo->FrameRateExtD;			
		}
		else
		{
			delete pXPacketBuffer;
			pXPacketBuffer=NULL;
		}
	}

	if (pXPacketBuffer)
	{
		XAutoLock l(m_csListPacketData);
		m_listPacketData.push_back(pXPacketBuffer);
	}

	return 0;	
}

int CD3DRender::RenderFrameSurface(mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc)
{
	if (m_hWnd == NULL)
	{
		return 0;
	}
	m_pSurface = pSurface;
	m_pmfxAlloc= pmfxAlloc;
	if (m_strCaptureFilePath.length() > 0)
	{
		mfxStatus sts = m_pmfxAlloc->Lock(pmfxAlloc->pthis, pSurface->Data.MemId, &(pSurface->Data));
		if (MFX_ERR_NONE == sts) {
			mfxFrameInfo &pInfo = pSurface->Info;
			mfxFrameData &pData = pSurface->Data;

			mfxU32 i, j, h, w;
			w = pInfo.CropW;
			h = pInfo.CropH;
			int nYLen = w * h;
			unsigned char*pYUV420=(unsigned char*)MALLOCMSG(nYLen*2);
			if (pYUV420)
			{
				for (i = 0; i < pInfo.CropH; i++)
				{
					memcpy(pYUV420+i*pInfo.CropW,pData.Y + (pInfo.CropY * pData.Pitch + pInfo.CropX)+ i * pData.Pitch,pInfo.CropW);
				}

				h = pInfo.CropH / 2;
				w = pInfo.CropW;
				int nPos = 0;
				for (i = 0; i < h; i++)
				{
					for (j = 0; j < w; j += 2)
					{
						memcpy(pYUV420+nYLen+nPos,pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX) + i * pData.Pitch + j,1);
						nPos++;
					}
				}
				for (i = 0; i < h; i++)
				{
					for (j = 1; j < w; j += 2)
					{
						memcpy(pYUV420+nYLen+nPos,pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX)+ i * pData.Pitch + j,1);
						nPos++;
					}
				}
				unsigned char*pY=pYUV420;
				unsigned char*pU=pY+nYLen;
				unsigned char*pV=pU+(nYLen/4);
				WriteYUVBytesIntoJpegFile(m_strCaptureFilePath.c_str(),pSurface->Info.CropW,pSurface->Info.CropH,100,pY,pU,pV);

				FREEMSG(pYUV420);
				pYUV420 = NULL;
			}		
		}
		sts = m_pmfxAlloc->Unlock(pmfxAlloc->pthis, pSurface->Data.MemId, &(pSurface->Data));
		m_strCaptureFilePath = "";
	}
	if (m_pSurface == NULL || m_pmfxAlloc == NULL)
	{
		return 0;
	}

	if (!IsWindowVisible(m_hWnd) || m_nWndWidth == 0 || m_nWndHeight == 0)
	{
		return 0;
	}

#if (1)
	{
		m_nFrameCount++;
		if (GetTickCount()-m_nLastTS > 20000)
		{
			char szDebug[1024];
			sprintf(szDebug,"CD3DRender_%d::RenderFrameSurface nFrameCount=%u SkipFrameCount=%u m_listPacketData.size=%u this = %u \n",m_bD3D11?11:9,m_nFrameCount,m_nSkipFrameCount,m_listPacketData.size(),this);
			DebugOutput( logd3d ,szDebug);

			m_nFrameCount=0;
			m_nSkipFrameCount=0;
			m_nLastTS=GetTickCount();
		}
	}
#endif

 	if (m_nSkipCount > 0)
 	{
 		m_nSkipCount--;
 		m_nSkipFrameCount++;
 		return 0;
 	}

	//if (!m_bD3D11)
	//{
	//	int nFrameRate = pSurface->Info.FrameRateExtN/pSurface->Info.FrameRateExtD;
	//	if (nFrameRate<40 && m_nRefreshRate>40)
	//	{
	//		WaitForSingleObject(m_hEventFrameRate,40);
	//	}

	//	//EnableDwmQueuing(nFrameRate);
	//}

	if (m_hwdev)
	{
		m_hwdev->RenderFrame(pSurface,pmfxAlloc,false);
	}	
	return 0;
}

void* CD3DRender::GetDeviceHandle(void)
{
	return m_hwdev;
}

/*
void CD3DRender::ThreadProcMain(void)
{
	mfxStatus sts = MFX_ERR_NONE;
	if (m_pSurfRenderder==NULL)
	{
		sts = CreateSurfRenderder();
		if (MFX_ERR_NONE > sts)
		{
			return;
		}
	}

	while(!m_bWaitStop)
	{
		XSurfaceBuffer*pXSurfaceBuffer=NULL;
		{
			XAutoLock l(m_csListPacketData);

			int nSize = m_listPacketData.size();
			if (nSize > 6)
			{
				do
				{
					XSurfaceBuffer*pXPacketBuffer=(XSurfaceBuffer*)m_listPacketData.front();
					m_listPacketData.pop_front();
					if (pXPacketBuffer)
					{			
						delete pXPacketBuffer;
						pXPacketBuffer=NULL;
					}
					m_nSkipFrameCount++;
				}while (m_listPacketData.size() > 3);
			}
			pXSurfaceBuffer=(XSurfaceBuffer*)m_listPacketData.front();
			m_listPacketData.pop_front();		
		}
		if (pXSurfaceBuffer!=NULL)
		{
			m_pSurfRenderder->Info.CropH		= pXSurfaceBuffer->CropH;
			m_pSurfRenderder->Info.CropW		= pXSurfaceBuffer->CropW;
			m_pSurfRenderder->Info.CropX		= pXSurfaceBuffer->CropX;
			m_pSurfRenderder->Info.CropY		= pXSurfaceBuffer->CropY;
			m_pSurfRenderder->Info.Width		= MSDK_ALIGN16(pXSurfaceBuffer->CropW);
			m_pSurfRenderder->Info.Height		= MSDK_ALIGN16(pXSurfaceBuffer->CropH);
			ConvertFrameRate(pXSurfaceBuffer->nFrameRate, &m_pSurfRenderder->Info.FrameRateExtN, &m_pSurfRenderder->Info.FrameRateExtD);
			mfxStatus sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, m_pSurfRenderder->Data.MemId, &(m_pSurfRenderder->Data));
			if (sts ==  MFX_ERR_NONE)
			{
				mfxU16 w, h, i,j, pitch;
				mfxU8 *ptr, *ptrUV;
				mfxFrameInfo* pInfo = &m_pSurfRenderder->Info;
				mfxFrameData* pData = &m_pSurfRenderder->Data;
				w = pInfo->CropW;
				h = pInfo->CropH;
				pitch = pData->Pitch;
				ptr = pData->Y + pInfo->CropX + pInfo->CropY * pData->Pitch;
				mfxU8*ptrY=(mfxU8*)pXSurfaceBuffer->pYUVData;
				//load Y
				for (i=0;i<h;i++)
				{
					memcpy(ptr,ptrY,w);
					ptr+=pitch;
					ptrY+=pXSurfaceBuffer->nPitch;
				}
          
				ptr = pData->UV + pInfo->CropX + (pInfo->CropY / 2) * pitch;
				ptrUV = (mfxU8*)pXSurfaceBuffer->pYUVData+pXSurfaceBuffer->nYLen + pXSurfaceBuffer->CropX + (pXSurfaceBuffer->CropY / 2) * pXSurfaceBuffer->nPitch;
				// load U
				// load V				
				w /= 2;
				h /= 2; 
				for (i = 0; i < h; i++) 
				{
					for (j = 0; j < w; j++)
					{
						ptr[i * pitch + j * 2] = ptrUV[i * pXSurfaceBuffer->nPitch + j * 2];
						ptr[i * pitch + j * 2 + 1] = ptrUV[i * pXSurfaceBuffer->nPitch + j * 2+1];
					}
				}
				sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, m_pSurfRenderder->Data.MemId, &(m_pSurfRenderder->Data));
				if (sts ==  MFX_ERR_NONE)
				{
					RenderFrameSurface(m_pSurfRenderder,m_pMFXAllocator);
				}
			}
			delete pXSurfaceBuffer;
			pXSurfaceBuffer=NULL;
		}
		else
		{
			Sleep(1);
		}
	}
}
*/