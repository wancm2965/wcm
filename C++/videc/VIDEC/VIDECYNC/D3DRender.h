//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//


#ifndef __DECODE_D3D_RENDER_H__
#define __DECODE_D3D_RENDER_H__


#include "../VIDEC_FRE/VIDECFREAPI.h"

#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#include <dxva.h>
#include <windows.h>

#include "igfx_s3dcontrol.h"
#include "hw_device.h"
#include "d3d_device.h"
#include "sample_defs.h"
#include "base_allocator.h"
#include "d3d_allocator.h"
#include "d3d11_device.h"
#include "d3d11_allocator.h"


#include "../HPAVC/HPAVC_Util/XThreadBase.h"
#include "../HPAVC/HPAVC_Util/XListPtr.h"
#include "../HPAVC/HPAVC_Util/XAutoLock.h"

#include "./VIDECYNCAPI.h"

#define OVERLAY_BACKBUFFER_FORMAT D3DFMT_X8R8G8B8
#define VIDEO_MAIN_FORMAT D3DFMT_YUY2


/** Direct3D 9 device implementation.
@note Can be initilized for only 1 or two 2 views. Handle to 

@note Device always set D3DPRESENT_PARAMETERS::Windowed to TRUE.
*/

BOOL IsWin8(void);
int  GetD3D11Count(void);

struct XSurfaceBuffer
{
	XSurfaceBuffer(void)
		: pNV12Data(NULL)
		, nPitch(0)
		, nFrameRate(0)
		, CropW(0)
		, CropH(0)
		, CropX(0)
		, CropY(0)
	{
	}
	virtual~XSurfaceBuffer(void)
	{
		if(pNV12Data)
		{
			FREEMSG(pNV12Data);
			pNV12Data=NULL;
		}
	}
	unsigned char*			pNV12Data;
	int						nPitch;
	int						nFrameRate;
	int						CropW;
	int						CropH;
	int						CropX;
	int						CropY;
};


class CD3DRender 
	: public FrameRateEvent
	, public XThreadBase
	, public CD3D9DeviceCallback
{
public:
	CD3DRender(YNCD3D_CB_DC cbdc,void*pUserData);
	~CD3DRender();

	mfxStatus Init(HWND hWindow,int nWidth,int nHeight,bool bD3D11,bool bCreateWnd,bool bHardWareDecoder,bool bDecoder=false);
	void* GetDeviceHandle(void);
	virtual void Redraw(void);
	virtual bool SetWindowPos(HWND hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags); 
	mfxStatus DrawSurface(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc,int&nFlagReset);
	int DrawFrame(unsigned char*pFrameData,int nFrameLen,int nFrameRate,int nWidth,int nHeight);
	mfxStatus RenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc, int nFrameRate,int&nFlagReset,unsigned char*pNV12Data=NULL,int nWidth=0,int nHeight=0,int nPitch=0);
	mfxStatus DoRenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc,bool bSkip,unsigned char*pNV12Data=NULL,int nFrameRate=0,int nWidth=0,int nHeight=0,int nPitch=0);
	void Close() ;
	int SkipDisplay(int nFlagSkipDisplay);
	int GetFrameRate(void);
	int GetShowType(void);
	bool CaptureToBMPFile(const char*cszFile);
	void Resize(void);
	int GetWidth(void);
	int GetHeight(void);
	mfxStatus LoadNextFrame(mfxFrameSurface1* pSurface,unsigned char*pYV12Data[]);
	mfxStatus CreateSurfRenderder(int nWidth,int nHeight);
	bool EnableDwmQueuing(int nFrameRate);

	void ReSetFrameRate(void);
	bool IsD3D11(void){return m_bD3D11;};

	virtual void ThreadProcMain(void);

	virtual void OnCD3D9DeviceCallbackDraw(HDC hDC);
private:
	XCritSec						m_csHWdev;

	CHWDevice*						m_hwdev;
	
	mfxFrameSurface1*				m_pSurfRenderder;
	MFXFrameAllocator*				m_pMFXAllocator;
	mfxAllocatorParams*				m_pmfxAllocatorParams;
	mfxFrameAllocResponse			m_EncResponse;  // memory allocation response for encoder
	std::string						m_strBMPFile;
	bool							m_bDwmEnabled;
	int								m_nFlagSkipDisplay;

	HWND							m_hWndVideo;
	int								m_nFrameRate;

	int								m_nSkipFrameCount;
	int								m_nFrameCount;
	unsigned long					m_nLastTS;
	unsigned long					m_nLastTimeStamp;

	unsigned long					m_ulLastTime;
	bool							m_bDecoder;
	bool							m_bHardWareDecoder;
	UINT							m_nWidth;
	UINT							m_nHeight;
	UINT							m_nPitch;

	unsigned long					m_ulTimeLast;

	mfxFrameSurface1 *				m_pSurface;
	mfxFrameAllocator *				m_pmfxAlloc;
	bool							m_bD3D11;
	XCritSec						m_csSurface;

	int								m_nCountFrameIndex;
	unsigned long					m_ulLastCountFrameTimestamp;
	unsigned long					m_nShowFrameRate;
	unsigned long					m_nShowFrameRateCount;
	unsigned long					m_nFrameRateCount;
	unsigned long					m_nFrameRateActual;

	bool							m_bWaitStop;
	XListPtr						m_listPacketData;
	XCritSec						m_csListPacketData;
	XCritSec						m_csSurfaceBuffer;
	XSurfaceBuffer*					m_pXSurfaceBuffer;

	HANDLE							m_hEvent;
	unsigned char*					m_pYUV420;
	int								m_nYUV420Len;
	XCritSec						m_csYUV420;

	YNCD3D_CB_DC					m_cbdc;
	void*							m_pUserData;
	int                               g_nD3DDisplayCount ;

};


#endif // __DECODE_D3D_RENDER_H__