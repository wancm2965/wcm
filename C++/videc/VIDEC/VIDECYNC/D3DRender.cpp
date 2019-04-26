//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//
#include <winsock2.h>

#include <windowsx.h>

#include <dwmapi.h>
#include <mmsystem.h>

#include "D3DRender.h"
#include "sample_defs.h"
#include "atlbase.h"
#include "VIDECYNCAPI.h"
#include <cstdlib>
#include "../VIDEC_FRE/VIDECFREAPI.h"
#include "../jpeglib/jpegutils.h"

#pragma warning(disable : 4100)

const UINT DWM_BUFFER_COUNT  = 4;

int		g_nD3D11Count = 0;


CD3DRender::CD3DRender(YNCD3D_CB_DC cbdc,void*pUserData)
:m_cbdc(cbdc)
,m_pUserData(pUserData)
{
	m_pSurfRenderder=NULL;
	m_pMFXAllocator=NULL;
	m_pmfxAllocatorParams=NULL;

	m_hwdev=NULL;
	m_bD3D11 = false;

	m_bDwmEnabled = false;
	m_strBMPFile="";
	m_nWidth=0;
	m_nHeight=0;
	m_nPitch=0;
	m_hWndVideo=NULL;

	m_nFrameRate=60;

	m_nFlagSkipDisplay=0;

	m_nSkipFrameCount=0;
	m_nFrameCount=0;
	m_nLastTS=timeGetTime();
	m_nLastTimeStamp = m_nLastTS;

	m_pSurface	= NULL;
	m_pmfxAlloc = NULL;

	m_nCountFrameIndex=0;
	m_ulLastCountFrameTimestamp=0;
	m_nShowFrameRate=0;
	m_nShowFrameRateCount=0;
	m_nFrameRateCount=0;
	m_nFrameRateActual=60;

	m_bWaitStop=true;

	m_hEvent=NULL;
	m_pYUV420=NULL;
	m_nYUV420Len=0;

	m_pXSurfaceBuffer=NULL;
	g_nD3DDisplayCount = 0;
}


BOOL IsWin8(void)
{
	//only check once then cache the result
	static BOOL checked = FALSE;
	static BOOL result  = FALSE;
	OSVERSIONINFO osver;
	if (!checked)
	{
		checked = TRUE;

		osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

		if (GetVersionEx( &osver ) && osver.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if ((osver.dwMajorVersion == 6 && osver.dwMinorVersion > 1) || (osver.dwMajorVersion > 6))
			{
				result = TRUE;
			}
		}

		//DebugOutput("WinVersion:Major= %d Minor= %d\n",osver.dwMajorVersion,osver.dwMinorVersion);
	}
	return result;
}

int  GetD3D11Count(void)
{
	return g_nD3D11Count;
}

bool	g_bYNCRegisterClass = false;

LRESULT CALLBACK YNCRenderWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	CD3DRender *pRenderWindow = (CD3DRender *)GetWindowLongPtr(hwnd,0);
	if (pRenderWindow != NULL && msg == WM_ERASEBKGND)
	{
		pRenderWindow->Redraw();
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

mfxStatus CD3DRender::Init(HWND hWindow,int nWidth,int nHeight,bool bD3D11,bool bCreateWnd,bool bHardWareDecoder ,bool bDecoder)
{
	if (hWindow !=NULL)
	{
		if (bCreateWnd == false)
		{
			m_hWndVideo=hWindow;
		}
		else
		{
			if (g_bYNCRegisterClass == false)
			{
				WNDCLASS wndclass;
				wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
				wndclass.lpfnWndProc = YNCRenderWndProc;
				wndclass.cbClsExtra = 0;
				wndclass.cbWndExtra    = sizeof(CD3DRender *);
				wndclass.hInstance = NULL;
				wndclass.hIcon = NULL;
				wndclass.hCursor = NULL;
				wndclass.hbrBackground = NULL;
				wndclass.lpszMenuName = NULL;
				wndclass.lpszClassName = _T("YNCRenderClass");
				if (!RegisterClass(&wndclass)) {
					//OutputDebugStr(_T("Registering HPD3DRenderClass window failed"));
					return MFX_ERR_UNKNOWN;
				}
				g_bYNCRegisterClass = true;
			}
			//m_hWnd = CreateWindow(_T("HPD3DRenderClass"), _T("HP D3DRender Window"), WS_CHILD|WS_DISABLED, 0, 0, 0, 0, (HWND)hWnd, NULL, NULL, NULL);
			m_hWndVideo = CreateWindowEx(0,_T("YNCRenderClass"), _T("YNC Render Window"), WS_CHILD|WS_DISABLED, 0, 0,1920, 1080,hWindow, NULL, NULL, NULL);
			CD3DRender *pRenderWindow = this;
			SetWindowLongPtr(m_hWndVideo,0,(unsigned long)pRenderWindow);
		}
		if (m_hWndVideo == NULL)
		{
			return MFX_ERR_UNKNOWN;
		}
	}

#if MFX_D3D11_RENDER_SUPPORT
#else
	bD3D11 = false;
#endif
	//if (!IsWin8() || GetD3D11Count() >= FRE_GetD3D11Total() || nWidth < 1280 || nHeight < 720)
	if (!IsWin8() || FRE_GetD3D11Total() == 0/* GetD3D11Count() >= FRE_GetD3D11Total() || nWidth < 1280 || nHeight < 720*/)
	{
		bD3D11 = false;
	}
	m_bD3D11 = bD3D11;

	mfxStatus sts = MFX_ERR_NONE;

#if MFX_D3D11_RENDER_SUPPORT
	if (m_bD3D11)
	{
		NEWRECORD( m_hwdev , new CD3D11Device(*this,bHardWareDecoder) )
	}
	else
#endif
	{
		NEWRECORD( m_hwdev , new CD3D9Device(*this) )
	}

	//RECT rect,rect2;

	//GetClientRect(m_hWndVideo, &rect);
	//if (nWidth == 1920 && nHeight == 1080 && m_bD3D11)
	//{
	//	::SetWindowPos(m_hWndVideo,NULL,0,0,1920,1080,SWP_NOACTIVATE);
	//}
	//else
	//{
	////	::SetWindowPos(m_hWndVideo,NULL,rect.left,rect.top,rect.right-rect.left+2,rect.bottom-rect.top,SWP_NOACTIVATE);
	//}

	if (m_hwdev)
	{
		sts = m_hwdev->Init(m_hWndVideo);
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
					sts = m_hwdev->Init(m_hWndVideo);
					if (MFX_ERR_NONE > sts)
					{			
						delete m_hwdev;
						m_hwdev = NULL;
					}
				}
			}
		}
	}

	m_bDecoder = bDecoder;

	if (m_hwdev == NULL)
	{
		return sts;
	}
	if ((!m_bD3D11 || ((FRE_GetD3D11Debug() == 2 || FRE_GetD3D11Debug() == 3) && m_bDecoder == 1) || FRE_GetD3D11Debug() == 4) && FRE_GetPIT() == 0)
	{
		FrameRateEvent::Open();
	}

	if (m_bD3D11)
	{
		g_nD3D11Count++;
	}

	m_bHardWareDecoder = bHardWareDecoder;

	////DebugOutput("Render::Init_%02d:11Count=%d,Width=%d,Height=%d\n",m_bD3D11?11:9,g_nD3D11Count,nWidth,nHeight);

	//if (rect.right > 0 && rect.bottom > 0)
	//{
	//	::SetWindowPos(m_hWndVideo,NULL,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,SWP_NOACTIVATE);
	//}

	return MFX_ERR_NONE;
}

void* CD3DRender::GetDeviceHandle(void)
{
	return m_hwdev;
}

void CD3DRender::Close()
{

	if (m_hEvent)
	{
		SetEvent(m_hEvent);
	}

	if (!m_bWaitStop)
	{
		m_bWaitStop = true;
		WaitForStop();
	}

	{
		XAutoLock l(m_csSurface);
		m_pSurface	= NULL;
		m_pmfxAlloc = NULL;
	}

	{
		XAutoLock l(m_csYUV420);
		if (m_pYUV420)
		{
			FREEMSG(m_pYUV420);
			m_pYUV420=NULL;
		}
	}
	{
		XAutoLock l(m_csSurfaceBuffer);
		if (m_pXSurfaceBuffer)
		{
			delete m_pXSurfaceBuffer;
			m_pXSurfaceBuffer=NULL;
		}
	}

	if ((!m_bD3D11 || ((FRE_GetD3D11Debug() == 2 || FRE_GetD3D11Debug() == 3) && m_bDecoder == 1) || FRE_GetD3D11Debug() == 4) && FRE_GetPIT() == 0)
	{
		FrameRateEvent::Close();
	}

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

	{
		//XAutoLock l(m_csSurface);
		if (m_hwdev)
		{
			delete m_hwdev;
			m_hwdev = NULL;
		}
	}

	if (m_bD3D11)
	{
		g_nD3D11Count--;
		if (g_nD3D11Count < 0)
		{
			g_nD3D11Count = 0;
		}
	}
}

CD3DRender::~CD3DRender()
{
	Close();

	if (m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent=NULL;
	}
}

bool CD3DRender::CaptureToBMPFile(const char*cszFile)
{
	std::string strBMPFile = cszFile;
	bool bRet=true;
	if (strBMPFile.length() > 0)
	{
		m_strBMPFile = strBMPFile;
	}
	else if (m_strBMPFile.length() > 0)
	{
		SkipDisplay(1);
		if (m_pmfxAlloc == NULL || m_pSurface == NULL)
		{			
			unsigned char* pYUV=NULL;
			XAutoLock l(m_csYUV420);
			if (m_pYUV420==NULL)
			{		
				XAutoLock l(m_csSurfaceBuffer);
				if (m_pXSurfaceBuffer)
				{	
					m_nWidth = m_pXSurfaceBuffer->CropW;
					m_nHeight = m_pXSurfaceBuffer->CropH;
					m_nPitch = m_pXSurfaceBuffer->nPitch;
					pYUV=(unsigned char*)MALLOCMSG(m_nPitch*m_nHeight*3/2+1);
					if (pYUV)
					{
						memcpy(pYUV,m_pXSurfaceBuffer->pNV12Data,m_nPitch*m_nHeight*3/2+1);
					}					
				}
				else
				{
					return bRet;
				}
			}
			else
			{
				pYUV=m_pYUV420;
			}

			if (pYUV)
			{
				mfxU32 i, j, h, w;
				w = m_nWidth;
				h = m_nHeight;
				if (m_nPitch==0)
				{
					m_nPitch = m_nWidth;
				}
				int nYLen = w * h;
				unsigned char*pUV = pYUV + m_nPitch*h;
				unsigned char*pYUV420=(unsigned char*)MALLOCMSG(nYLen*2);
				if (pYUV420)
				{
					for (i = 0; i < m_nHeight; i++)
					{
						memcpy(pYUV420+i*m_nWidth,pYUV + i * m_nPitch,m_nWidth);
					}

					h = m_nHeight / 2;
					w = m_nWidth;
					int nPos = 0;
					for (i = 0; i < h; i++)
					{
						for (j = 0; j < w; j += 2)
						{
							memcpy(pYUV420+nYLen+nPos,pUV + i * m_nPitch + j,1);
							nPos++;
						}
					}
					for (i = 0; i < h; i++)
					{
						for (j = 1; j < w; j += 2)
						{
							memcpy(pYUV420+nYLen+nPos,pUV + i * m_nPitch + j,1);
							nPos++;
						}
					}
					unsigned char*pY=pYUV420;
					unsigned char*pU=pY+nYLen;
					unsigned char*pV=pU+(nYLen/4);				

					if (0==WriteYUVBytesIntoJpegFile(m_strBMPFile.c_str(),m_nWidth,m_nHeight,100,pY,pU,pV))
					{
						bRet=true;
					}
					else
					{
						bRet=false;
					}
					FREEMSG(pYUV420);
					pYUV420 = NULL;
				}

				if (m_pYUV420==NULL)
				{
					FREEMSG(pYUV);
					pYUV=NULL;
				}
			}
		}
		else
		{
			unsigned char*pYUV420=NULL;
			int nYLen=0;
			int nW=0;
			int nH=0;
			{
				XAutoLock l(m_csSurface);
				mfxStatus sts = m_pmfxAlloc->Lock(m_pmfxAlloc->pthis, m_pSurface->Data.MemId, &(m_pSurface->Data));
				if (MFX_ERR_NONE == sts) 
				{
					mfxFrameInfo &pInfo = m_pSurface->Info;
					mfxFrameData &pData = m_pSurface->Data;

					mfxU32 i, j, h, w;
					w = pInfo.CropW;
					h = pInfo.CropH;
					nYLen = w * h;
					nW=m_pSurface->Info.CropW;
					nH=m_pSurface->Info.CropH;
					pYUV420=(unsigned char*)MALLOCMSG(nYLen*2);
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
					}		
				}
				sts = m_pmfxAlloc->Unlock(m_pmfxAlloc->pthis, m_pSurface->Data.MemId, &(m_pSurface->Data));
			}
			if (pYUV420 && nYLen > 0)
			{
				unsigned char*pY=pYUV420;
				unsigned char*pU=pY+nYLen;
				unsigned char*pV=pU+(nYLen/4);				

				if (0==WriteYUVBytesIntoJpegFile(m_strBMPFile.c_str(),nW,nH,100,pY,pU,pV))
				{
					bRet=true;
				}
				else
				{
					bRet=false;
				}
				FREEMSG(pYUV420);
				pYUV420 = NULL;
			}
		}	
		m_strBMPFile="";
		SkipDisplay(1);
	}
	return bRet;
}

int CD3DRender::GetShowType(void)
{
	if (m_bD3D11)
	{
		return 3;
	}	
	return 2;
}

int CD3DRender::GetFrameRate(void)
{
	int nRate = m_nShowFrameRate;
	m_nShowFrameRate=0;
	return nRate;
}

int CD3DRender::SkipDisplay(int nFlagSkipDisplay)
{
	if (nFlagSkipDisplay==-1)
	{
		Redraw();
		return 0;
	}
 	m_nFlagSkipDisplay=FRE_GetSkipDisplay();
	return 0;
}

void CD3DRender::Resize(void)
{
	//if (m_hWndVideo)
	//{
	//	RECT rect,rect2;
	//	GetClientRect(m_hWndVideo, &rect);

	//	if (rect.right > 0 && rect.bottom > 0)
	//	{
	//		::SetWindowPos(m_hWndVideo,NULL,rect.left,rect.top,rect.right-rect.left+2,rect.bottom-rect.top+2,SWP_NOACTIVATE);
	//		::SetWindowPos(m_hWndVideo,NULL,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,SWP_NOACTIVATE);
	//	}
	//}
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

	//return MFX_ERR_NONE;
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

	int nOffsetLineTop =8; //notce add 
	if(nOffsetLineTop >0 && pInfo->FourCC ==MFX_FOURCC_NV12)
	{
	}

	return MFX_ERR_NONE;    
}

mfxStatus CD3DRender::CreateSurfRenderder(int nWidth,int nHeight)
{
	mfxStatus sts = MFX_ERR_NONE;
	if (m_pSurfRenderder==NULL)
	{
		NEWRECORD( m_pSurfRenderder ,new mfxFrameSurface1 )
		memset(m_pSurfRenderder, 0, sizeof(mfxFrameSurface1));
		m_pSurfRenderder->Info.PicStruct	= MFX_PICSTRUCT_FIELD_BFF;
		m_pSurfRenderder->Info.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_pSurfRenderder->Info.FourCC		= MFX_FOURCC_NV12;
		m_pSurfRenderder->Info.CropH		=nHeight;
		m_pSurfRenderder->Info.CropW		=nWidth;
		m_pSurfRenderder->Info.Width		=MSDK_ALIGN16(nWidth);;
		m_pSurfRenderder->Info.Height		=MSDK_ALIGN16(nHeight);;

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
		EncRequest.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_VPPIN | MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;
		// alloc frames for encoder
		sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &EncRequest, &m_EncResponse);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		m_pSurfRenderder->Info.FrameId.ViewId = 0;
		m_pSurfRenderder->Data.MemId = m_EncResponse.mids[0];
	}
	return sts;
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
		//DebugOutput("DwmIsCompositionEnabled failed with error 0x%x.\n", hr);
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
		//DebugOutput("EnableDwmQueuing FAILED DwmGetCompositionTimingInfo  error = 0x%x.nFrameRate = %u this = %u\n", hr,nFrameRate,this);
		return false;
	}

	// Enable DWM queuing from the next refresh.
	DWM_PRESENT_PARAMETERS dwmpp = {0};
	dwmpp.cbSize                    = sizeof(dwmpp);
	dwmpp.fQueue                    = TRUE;
	dwmpp.cRefreshStart             = dwmti.cRefresh + 1;
	dwmpp.cBuffer                   = 8; //maximum depth of DWM queue
	dwmpp.fUseSourceRate            = TRUE;
	dwmpp.cRefreshesPerFrame        = 1;
	dwmpp.eSampling                 = DWM_SOURCE_FRAME_SAMPLING_POINT;
	dwmpp.rateSource.uiDenominator  = 1;
	dwmpp.rateSource.uiNumerator    = nFrameRate;
	hr = DwmSetPresentParameters(m_hWndVideo, &dwmpp);

	if (FAILED(hr))
	{
		//DebugOutput("EnableDwmQueuing FAILED DwmSetPresentParameters error = 0x%x.nFrameRate = %u this = %u\n", hr,nFrameRate,this);
		return false;
	}

	// DWM queuing is enabled.
	m_bDwmEnabled = true;

	m_nFrameRate = nFrameRate;

	//DebugOutput("EnableDwmQueuing SUCCESS nFrameRate = %u this = %u\n",nFrameRate,this);

	return true;
}

void CD3DRender::ReSetFrameRate(void)
{
	unsigned long ulCurTimestamp = timeGetTime();
	m_nFrameRateCount++;
	m_nShowFrameRateCount++;


	if (m_ulLastCountFrameTimestamp == 0)
	{
		m_ulLastCountFrameTimestamp = ulCurTimestamp;
	}
	unsigned long ulTemp = ulCurTimestamp - m_ulLastCountFrameTimestamp;
	if ((m_nShowFrameRateCount > 30 && ulTemp < 990) || (m_nShowFrameRateCount <= 30 && ulTemp < 980))
	{
		return;
	}
	m_ulLastCountFrameTimestamp = ulCurTimestamp;
	
	if (ulTemp > 1500)
	{
		//DebugOutput("CD3DRender::ReSetFrameRate m_nShowFrameRate=%d m_nShowFrameRateCount=%d ulTemp > 1500 this=%u=====================\n",m_nShowFrameRate,m_nShowFrameRateCount,this);

		float f = 1000.00/ulTemp;
		m_nShowFrameRate=m_nShowFrameRateCount*f;
		m_nShowFrameRateCount=0;
		m_nCountFrameIndex=0;
		m_nFrameRateCount=0;
		m_nFrameRateActual=60;
		return;
	}

	m_nShowFrameRate=m_nShowFrameRateCount;		
	m_nShowFrameRateCount=0;

	//DebugOutput("CD3DRender::ReSetFrameRate m_nShowFrameRate=%d this=%u\n",m_nShowFrameRate,this);


	if(m_nFrameRateCount < m_nFrameRateActual)
	{
		if (m_nCountFrameIndex < 10)
		{
			m_nCountFrameIndex++;
			m_nFrameRateCount=0;	
			return;
		}
		else
		{
			m_nCountFrameIndex=0;
		}
	}
	else
	{
		m_nCountFrameIndex=0;
	}
	
	m_nFrameRateActual = m_nFrameRateCount;
	m_nFrameRateCount=0;	

	if (m_nFrameRateActual>52)
	{
		m_nFrameRateActual=60;
	}
	else if (m_nFrameRateActual>32)
	{
		m_nFrameRateActual=50;
	}
	else if (m_nFrameRateActual>26)
	{
		m_nFrameRateActual=30;
	}
	else if (m_nFrameRateActual>21)
	{
		m_nFrameRateActual=25;
	}
	else if (m_nFrameRateActual>16)
	{
		m_nFrameRateActual=20;
	}
	else if (m_nFrameRateActual>11)
	{
		m_nFrameRateActual=15;
	}
	else if (m_nFrameRateActual>6)
	{
		m_nFrameRateActual=10;
	}
	else if (m_nFrameRateActual>2)
	{
		m_nFrameRateActual=5;
	}
	else
	{
		m_nFrameRateActual=1;
	}
}

void CD3DRender::ThreadProcMain(void)
{
	while(!m_bWaitStop)
	{
		XSurfaceBuffer*pXSurfaceBuffer=NULL;
		{
			XAutoLock l(m_csListPacketData);
			int nSize = m_listPacketData.size();
			if (nSize > 10)
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
				}while (m_listPacketData.size() > 5);
			}
			if (nSize)
			{
				pXSurfaceBuffer=(XSurfaceBuffer*)m_listPacketData.front();
				m_listPacketData.pop_front();		
			}
		}
		if (pXSurfaceBuffer!=NULL)
		{	
			int nFlagReset=100;
			DebugOutput( logthread ,"CD3DRender::ThreadProcMain RenderFrame 1 \n");
			RenderFrame(m_pSurfRenderder,m_pMFXAllocator,pXSurfaceBuffer->nFrameRate,nFlagReset,pXSurfaceBuffer->pNV12Data,pXSurfaceBuffer->CropW,pXSurfaceBuffer->CropH,pXSurfaceBuffer->nPitch);	
			DebugOutput( logthread ,"CD3DRender::ThreadProcMain RenderFrame 2\n");
			{
				XAutoLock l(m_csSurfaceBuffer);
				if (m_pXSurfaceBuffer)
				{
					delete m_pXSurfaceBuffer;
					m_pXSurfaceBuffer=NULL;
				}
				m_pXSurfaceBuffer=pXSurfaceBuffer;
			}
			//delete pXSurfaceBuffer;
			//pXSurfaceBuffer=NULL;
		}
		else
		{
			if (m_hEvent)
			{
				DebugOutput( logthread ,"CD3DRender::ThreadProcMain WaitForSingleObject 1 \n");
				WaitForSingleObject(m_hEvent,100);
				DebugOutput( logthread ,"CD3DRender::ThreadProcMain WaitForSingleObject 2 \n");
			}
			else
			{
				Sleep(1);
			}
		}
	}
}

mfxStatus CD3DRender::DrawSurface(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc,int&nFlagReset)
{
	ReSetFrameRate();


	if (FRE_GetD3D11Debug() == 1 || FRE_GetD3D11Debug() == 3 || FRE_GetD3D11Debug() == 4)
	{
		if (m_bD3D11 && m_bDecoder && m_hEvent == NULL && m_bWaitStop)
		{
			m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
			if (m_hEvent)
			{
				m_bWaitStop = false;
				StartThread();
			}
		}
	}

	if (m_hEvent)
	{
		nFlagReset=0;
		NEWRECORD3( XSurfaceBuffer*,pXPacketBuffer ,new XSurfaceBuffer )
		if (pXPacketBuffer)
		{
			mfxU16 w, h, i,j, pitch;
			mfxFrameInfo* pInfo		= &pSurface->Info;
			mfxFrameData* pSrcData	= &pSurface->Data;
			mfxU8 *ptr, *ptr2;
			pitch = pSrcData->Pitch;
			pXPacketBuffer->pNV12Data=(unsigned char*)MALLOCMSG(pitch*pInfo->CropH*3/2+1);
			if (pXPacketBuffer->pNV12Data)
			{
				memcpy(pXPacketBuffer->pNV12Data,pSrcData->Y,pitch*pInfo->CropH);			
				memcpy(pXPacketBuffer->pNV12Data+pitch*pInfo->CropH,pSrcData->UV,pitch*pInfo->CropH/2);

				pXPacketBuffer->nPitch=pitch;
				pXPacketBuffer->CropW=pInfo->CropW;
				pXPacketBuffer->CropH=pInfo->CropH;
				pXPacketBuffer->CropX=pInfo->CropX;
				pXPacketBuffer->CropY=pInfo->CropY;
				pXPacketBuffer->nFrameRate=m_nFrameRateActual;	
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

		SetEvent(m_hEvent);
	}
	else
	{
		{
			XAutoLock l(m_csYUV420);
			int nLen = pSurface->Data.Pitch*pSurface->Info.CropH*3/2+1;
			if (m_pYUV420 && nLen > m_nYUV420Len)
			{
				FREEMSG(m_pYUV420);
				m_pYUV420=NULL;
			}

			if (m_pYUV420==NULL)
			{
				m_nYUV420Len = nLen;
				m_pYUV420=(unsigned char*)MALLOCMSG(m_nYUV420Len);
			}

			if (m_pYUV420)
			{
				memcpy(m_pYUV420,pSurface->Data.Y,pSurface->Data.Pitch*pSurface->Info.CropH);			
				memcpy(m_pYUV420+pSurface->Data.Pitch*pSurface->Info.CropH,pSurface->Data.UV,pSurface->Data.Pitch*pSurface->Info.CropH/2);
			}
		}

		m_nWidth = pSurface->Info.CropW;
		m_nHeight = pSurface->Info.CropH;
		m_nPitch = pSurface->Data.Pitch;

		nFlagReset=100;
		RenderFrame(m_pSurfRenderder,m_pMFXAllocator,m_nFrameRateActual,nFlagReset,m_pYUV420,pSurface->Info.CropW,pSurface->Info.CropH,pSurface->Data.Pitch);	
	}

	return MFX_ERR_NONE;
}

int CD3DRender::DrawFrame(unsigned char*pFrameData,int nFrameLen,int nFrameRate,int nWidth,int nHeight)
{
	if(g_nD3DDisplayCount <=10 )
	{
		DebugOutput( logd3d ,"D3D DrawFrame \n");
		g_nD3DDisplayCount ++;
	}

	ReSetFrameRate();

	nFrameRate = m_nFrameRateActual;

	m_nWidth=nWidth;
	m_nHeight=nHeight;

	mfxStatus sts = MFX_ERR_NONE;

	//if (m_nFrameCount%5 == 0 && m_hwdev)
	//{
	//	sts = m_hwdev->CheckMonitor();
	//}

	if (m_pSurfRenderder)
	{
		if (sts == MFX_WRN_IN_EXECUTION || nWidth != m_pSurfRenderder->Info.CropW || nHeight!= m_pSurfRenderder->Info.CropH)
		{
			{
				XAutoLock l(m_csSurface);
				m_pSurface	= NULL;
				m_pmfxAlloc = NULL;
			}			
			
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
		}
	}

	mfxIMPL impl = GetIntelAdapterIMPL();
	if (/*FRE_GetD3D11Type() == 0*/impl != 2 && m_bD3D11)
	{
		{
			XAutoLock l(m_csYUV420);
			if (m_pYUV420==NULL)
			{
				m_pYUV420=(unsigned char*)MALLOCMSG(1920*1080*2);
			}
			memcpy(m_pYUV420,pFrameData,m_nWidth*m_nHeight);
			unsigned char*ptr = m_pYUV420+m_nWidth*m_nHeight;
			unsigned char*ptrU = pFrameData+m_nWidth*m_nHeight;
			unsigned char*ptrV = ptrU+m_nWidth*m_nHeight/4;
			for (int i = 0; i < m_nHeight/2; i++) 
			{
				for (int j = 0; j < m_nWidth/2; j++)
				{
					ptr[i * m_nWidth + j * 2] = ptrU[j];
					ptr[i * m_nWidth + j * 2 + 1] = ptrV[j];
				}
				ptrU+=m_nWidth/2;
				ptrV+=m_nWidth/2;
			}
		}

		int nFlagReset=100;
		m_nPitch = m_nWidth;
		return RenderFrame(m_pSurfRenderder,m_pMFXAllocator,nFrameRate,nFlagReset,m_pYUV420,m_nWidth,m_nHeight,m_nWidth);
	}
	else
	{
		if (m_pSurfRenderder==NULL)
		{
			sts = CreateSurfRenderder(nWidth, nHeight);
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		}
		m_pSurfRenderder->Info.FrameRateExtN = nFrameRate;
		m_pSurfRenderder->Info.FrameRateExtD = 1;

		sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, m_pSurfRenderder->Data.MemId, &(m_pSurfRenderder->Data));
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);		

		int nY=nWidth*nHeight;
		int nUV=nY/4;
		unsigned char*pYUV[3];
		pYUV[0]=(unsigned char*)pFrameData;
		pYUV[1]=(unsigned char*)pFrameData+nY;
		pYUV[2]=pYUV[1]+nUV;


		sts = LoadNextFrame(m_pSurfRenderder,pYUV);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, m_pSurfRenderder->Data.MemId, &(m_pSurfRenderder->Data));
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		int nFlagReset=100;
		return RenderFrame(m_pSurfRenderder,m_pMFXAllocator,nFrameRate,nFlagReset);
	}
	return 0;
}

mfxStatus CD3DRender::RenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc, int nFrameRate,int&nFlagReset,unsigned char*pNV12Data,int nWidth,int nHeight,int nPitch)
{
	if (nFlagReset !=100)
	{
		ReSetFrameRate();
	}
	nFrameRate = m_nFrameRateActual;

	mfxStatus sts = MFX_ERR_NONE;
	int nKeyFrame=nFlagReset;
	nFlagReset=0;

	if (pNV12Data == NULL)
	{
		MSDK_CHECK_POINTER(pSurface, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(pmfxAlloc, MFX_ERR_NULL_PTR);

		nWidth=pSurface->Info.CropW;
		nHeight=pSurface->Info.CropH;
	}

	if (FRE_GetDebugLevel() > 0)
	{
		m_nFrameCount++;
		unsigned long nCurTS=timeGetTime();
		if (FRE_GetDebugLevel() == 100)
		{
			if (nFrameRate == 60)
			{
				int nInterval = nCurTS - m_nLastTimeStamp;
				if (nInterval > 20 || nInterval < 13)
				{
					DebugOutput( logd3d ,"VIDECYNC_%02d::RenderFrame  FrameInterval=%d FrameRate=%u this = %u \n",m_bD3D11?11:9,nInterval,nFrameRate,this);			
				}	
				m_nLastTimeStamp = nCurTS;
			}
			else if (nFrameRate == 30)
			{
				int nInterval = nCurTS - m_nLastTimeStamp;
				if (nInterval > 40 || nInterval < 25)
				{
					DebugOutput( logd3d ,"VIDECYNC_%02d::RenderFrame  FrameInterval=%d FrameRate=%u this = %u \n",m_bD3D11?11:9,nInterval,nFrameRate,this);			
				}	
				m_nLastTimeStamp = nCurTS;
			}
		}		
		
		{
			if (nCurTS-m_nLastTS >= 20000)
			{
				DebugOutput( logd3d ,"VIDECYNC_%02d::RenderFrame FrameCount=%u SkipFrameCount=%u  FrameRate=%u this = %u Hard_decode_render=%u Video_Width=%d Video_Height=%d\n",m_bD3D11?11:9,m_nFrameCount,m_nSkipFrameCount,nFrameRate,this,m_bDecoder?1:0,nWidth,nHeight);			
				m_nSkipFrameCount=0;
				m_nFrameCount=0;
				m_nLastTS=nCurTS;
			}
		}
	}		

	if (m_nFlagSkipDisplay < 0)
	{
		m_nFlagSkipDisplay=0;
	}

	bool bSkip = false;
	if (m_nFlagSkipDisplay)
	{
		m_nFlagSkipDisplay--;
		//m_nFlagSkipDisplay=0;
		m_nSkipFrameCount++;

		return MFX_ERR_NONE;

		//if (nFrameRate < 60)
		//{
		//	bSkip = true;
		//}
		//else
		//{
		//	return MFX_ERR_NONE;
		//}
	}
//DebugOutput("CD3DRender::RenderFrame IsWindowVisible 1 \n");
	if (!IsWindowVisible(m_hWndVideo))
	{
		//DebugOutput("CD3DRender::RenderFrame IsWindowVisible 2\n");
		return MFX_ERR_NONE;
	}
//DebugOutput("CD3DRender::RenderFrame IsWindowVisible 2\n");

	RECT rect;
	GetClientRect(m_hWndVideo, &rect);
	if (IsRectEmpty(&rect))
	{
		return MFX_ERR_NONE;
	}

	if (pSurface)
	{
		pSurface->Info.FrameRateExtN = nFrameRate;
		pSurface->Info.FrameRateExtD = 1;
	}

	sts = DoRenderFrame(pSurface,pmfxAlloc,bSkip,pNV12Data,nFrameRate,nWidth,nHeight,nPitch);

	if (!m_bD3D11 || ((FRE_GetD3D11Debug() == 2 || FRE_GetD3D11Debug() == 3 ) && m_bDecoder == 1) || FRE_GetD3D11Debug() == 4)
	{
		if (FRE_GetPIT() == 0)
		{
			if (m_nFrameRate!=nFrameRate)
			{
				m_nFrameRate=nFrameRate;
				FrameRateEvent::SetFrameRate(m_nFrameRate);
			}		
		}
		//else if (FRE_GetPIT() == 2)
		//{
		//	EnableDwmQueuing(nFrameRate);
		//}
	}

	return sts;
}
// 
// unsigned long XGenerateSSRC(void)
// {
// #ifdef WIN32
// 	LARGE_INTEGER frequence, privious;
// 	if(!QueryPerformanceFrequency( &frequence))//取高精度运行计数器的频率
// 	{
// 		return timeGetTime();
// 	}
// 
// 	if (!QueryPerformanceCounter( &privious ))
// 	{
// 		return timeGetTime();
// 	}
// 
// 	DWORD dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //换算到微秒数
// 
// 	return dwRet;//微秒
// #else
// 	struct timeval now;
// 	gettimeofday(&now,NULL);
// 	return now.tv_sec*1000+now.tv_usec; 
// #endif
// }

bool CD3DRender::SetWindowPos(HWND hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	if (m_hWndVideo)
	{
		return ::SetWindowPos(m_hWndVideo,hWndInsertAfter,x,y,cx,cy,nFlags);
	}
	return true;
}

void CD3DRender::Redraw(void)
{
//DebugOutput("CD3DRender::Redraw   begin\n");
	if (m_bD3D11)
	{
		DebugOutput( logcoreexcep ,"CD3DRender::Redraw   2\n");

		return;
	}

	if (!IsWindowVisible(m_hWndVideo))
	{
		DebugOutput( logcoreexcep ,"CD3DRender::Redraw   3\n");

		return;
	}

	RECT rect;
	GetClientRect(m_hWndVideo, &rect);
	if (IsRectEmpty(&rect))
	{
		DebugOutput( logcoreexcep ,"CD3DRender::Redraw   4\n");

		return;
	}

	{
		//XAutoLock l(m_csSurface);
		if (m_pSurface && m_pmfxAlloc)
		{
			DebugOutput( logcoreexcep ,"CD3DRender::Redraw   5\n");

			DoRenderFrame(m_pSurface, m_pmfxAlloc,true);
			DebugOutput( logcoreexcep ,"CD3DRender::Redraw   6\n");

		}
	}
	//DebugOutput("CD3DRender::Redraw   end\n");

}

mfxStatus CD3DRender::DoRenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc,bool bSkip,unsigned char*pNV12Data,int nFrameRate,int nWidth,int nHeight,int nPitch)
{
	HRESULT hr = S_OK;
	mfxStatus sts = MFX_ERR_NONE;

	if (bSkip)
	{
		XAutoLock l(m_csSurface);
		pSurface = m_pSurface;
		pmfxAlloc = m_pmfxAlloc;
	}

	if (pNV12Data==NULL)
	{
		MSDK_CHECK_POINTER(pSurface, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(pmfxAlloc, MFX_ERR_NULL_PTR);
	}

	if (/*bSkip == false &&*/ pSurface)
	{
		XAutoLock l(m_csSurface);
		m_pSurface = pSurface;
		m_pmfxAlloc = pmfxAlloc;
	}
	if (m_hwdev)
	{
		//DebugOutput("CD3DRender::DoRenderFrame if (m_hwdev) 1\n");
		//m_nShowFrameRateCount++;
		if (bSkip)
		{
			//XAutoLock l(m_csSurface);			
			sts = m_hwdev->RenderFrame(pSurface,pmfxAlloc,bSkip,pNV12Data,nFrameRate,nWidth,nHeight,nPitch);
		}
		else 
		{
			sts = m_hwdev->RenderFrame(pSurface,pmfxAlloc,bSkip,pNV12Data,nFrameRate,nWidth,nHeight,nPitch);
		}
//DebugOutput("CD3DRender::DoRenderFrame if (m_hwdev) 2\n");
		if (m_strBMPFile.length() > 0)
		{
			CaptureToBMPFile("");
		}
	}

	if ((!m_bD3D11 || ((FRE_GetD3D11Debug() == 2 || FRE_GetD3D11Debug() == 3) && m_bDecoder == 1) || FRE_GetD3D11Debug() == 4) && !bSkip && FRE_GetPIT() == 0)
	{
		//DebugOutput("CD3DRender::DoRenderFrame WaitEvent 1\n");
		FrameRateEvent::WaitEvent();
		//DebugOutput("CD3DRender::DoRenderFrame WaitEvent 2\n");
	}

	//if (m_hwdev && sts == MFX_ERR_NONE)
	//{
	//	if (bSkip)
	//	{
	//		XAutoLock l(m_csSurface);	
	//		sts = m_hwdev->Present();
	//	}
	//	else
	//	{
	//		sts = m_hwdev->Present();
	//	}
	//}
	

	//unsigned long ulTime = XGenerateSSRC();
	// 	if (ulTime - m_ulTimeLast  > 33339 || ulTime - m_ulTimeLast  < 33330)
	// 	{
	// 		unsigned long ulTime2 = XGenerateSSRC();
	// 		char szDebug[1024];
	// 		sprintf(szDebug,"CD3DRender::DoRenderFrame Time = %u PresentTime = %u this = %u\n",ulTime - m_ulTimeLast,ulTime2 - ulTime,this);
	// 		DebugOutput(szDebug);
	// 	}
	// 	m_ulTimeLast = ulTim
	return SUCCEEDED(hr) ? MFX_ERR_NONE : MFX_ERR_DEVICE_FAILED;
}

int CD3DRender::GetWidth(void)
{
	return m_nWidth;
}

int CD3DRender::GetHeight(void)
{
	return m_nHeight;
}

void CD3DRender::OnCD3D9DeviceCallbackDraw(HDC hDC)
{
	if (m_cbdc)
	{
		m_cbdc(m_pUserData,hDC);
	}
}

VIDECYNC_API void*VIDECYNC_D3DPlayerOpen(YNCD3D_CB_DC cbdc,void*pUserData,void* hWnd,int nWidth,int nHeight,bool bCreateWnd)
{
	if (FRE_GetUseD3D() == 0)
	{
		return NULL;
	}

	if (FRE_GetUseD3D() == 2)
	{
		mfxStatus nStatus = MFX_Init();
		if (nStatus != MFX_ERR_NONE)
		{
			return NULL;
		}	
	}

	NEWRECORD3( CD3DRender*,pD3D9Render ,new CD3DRender(cbdc,pUserData) )
	if (0!=pD3D9Render->Init((HWND)hWnd,nWidth,nHeight,true,bCreateWnd,false))
	{
		pD3D9Render->Close();
		delete pD3D9Render;
		pD3D9Render=NULL;
	}
	return pD3D9Render;
}

VIDECYNC_API void VIDECYNC_D3DPlayerClose(void*hPlayer)
{
	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		pD3D9Render->Close();
		delete pD3D9Render;
		pD3D9Render=NULL;
	}
}

VIDECYNC_API int VIDECYNC_D3DPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int nWidth,int nHeight)
{

	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->DrawFrame((unsigned char*)pFrameData,nFrameLen,nFrameRate,nWidth,nHeight);
	}

	return -1;
}

VIDECYNC_API void VIDECYNC_D3DPlayerRedraw(void*hPlayer)
{
	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		pD3D9Render->Redraw();
	}
}

VIDECYNC_API bool VIDECYNC_D3DPlayerSetWindowPos(void*hPlayer,void* hWndInsertAfter,int x,int y,int cx,int cy,unsigned int nFlags)
{
	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->SetWindowPos((HWND)hWndInsertAfter, x, y, cx, cy,nFlags);
	}
	return false;
}

VIDECYNC_API int VIDECYNC_D3DPlayerGetWidth(void*hPlayer)
{
	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->GetWidth();
	}

	return 1280;
}

VIDECYNC_API int VIDECYNC_D3DPlayerGetHeight(void*hPlayer)
{
	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->GetHeight();
	}

	return 720;
}

VIDECYNC_API int VIDECYNC_D3DPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay)
{
	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->SkipDisplay(nFlagSkipDisplay);
	}

	return -1;
}

VIDECYNC_API int VIDECYNC_D3DPlayerGetFrameRate(void*hPlayer)
{
	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->GetFrameRate();
	}

	return 0;
}
VIDECYNC_API int VIDECYNC_D3DPlayerGetShowType(void*hPlayer)
{
	CD3DRender*pD3D9Render=(CD3DRender*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->GetShowType();
	}

	return 0;
}