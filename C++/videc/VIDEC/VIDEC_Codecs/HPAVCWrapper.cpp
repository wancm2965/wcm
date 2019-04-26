#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "HPAVCWrapper.h"
#include "XUtil.h"



static HPAVCWrapper* g_pHPAVCWrapper=NULL;
extern HINSTANCE g_hInstance;

extern BOOL IsVistaOrAbove();


HPAVCWrapper&HPAVCWrapper::Instance(void)
{
	return *g_pHPAVCWrapper;
}

HPAVCWrapper::HPAVCWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sHPAVC.dll",szTemp);


	pHPAVC_PlayerOpen=NULL;
	pHPAVC_PlayerClose=NULL;
	pHPAVC_PlayerPlayData=NULL;
	pHPAVC_PlayerGetVideoWidth=NULL;
	pHPAVC_PlayerGetVideoHeigh=NULL;
	pHPAVC_PlayerCaptureToBMPFile=NULL;
	pHPAVC_PlayerSetWindowPos=NULL;
	pHPAVC_PlayerGetWnd=NULL;
	pHPAVC_PlayerSetVideoParam=NULL;

	pHPAVC_CaptureOpen=NULL;
	pHPAVC_CaptureClose=NULL;
	pHPAVC_CaptureEncodeData=NULL;
	pHPAVC_CaptureGetVideoWidth=NULL;
	pHPAVC_CaptureGetVideoHeigh=NULL;
	pHPAVC_CaptureCaptureToBMPFile=NULL;
	pHPAVC_CaptureSetWindowPos=NULL;
	pHPAVC_CaptureSetVideoParam=NULL;

	pHPAVC_EncoderSetMaxCount=NULL;
	pHPAVC_EncoderTry=NULL;
	pHPAVC_EncoderOpen=NULL;
	pHPAVC_EncoderClose=NULL;
	pHPAVC_EncoderEncoding=NULL;
	pHPAVC_EncoderSetVideoParam=NULL;

	pHPAVC_D3DRenderOpen=NULL;
	pHPAVC_D3DRenderClose=NULL;
	pHPAVC_D3DRenderPlayFrame=NULL;
	pHPAVC_D3DRenderPlaySurface=NULL;
	pHPAVC_D3DRenderSetWindowPos=NULL;
	pHPAVC_D3DRenderGetWnd=NULL;
	pHPAVC_D3DRenderSkipDisplay=NULL;
	pHPAVC_D3DRenderCaptureToBMPFile=NULL;

	m_pXDllLoadLibrary=new XDllLoadLibrary(szPathName);

	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerOpen,"HPAVC_PlayerOpen");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerClose,"HPAVC_PlayerClose");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerPlayData,"HPAVC_PlayerPlayData");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerGetVideoWidth,"HPAVC_PlayerGetVideoWidth");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerGetVideoHeigh,"HPAVC_PlayerGetVideoHeigh");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerCaptureToBMPFile,"HPAVC_PlayerCaptureToBMPFile");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerSetWindowPos,"HPAVC_PlayerSetWindowPos");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerGetWnd,"HPAVC_PlayerGetWnd");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_PlayerSetVideoParam,"HPAVC_PlayerSetVideoParam");

		m_pXDllLoadLibrary->LoadFunction(pHPAVC_CaptureOpen,"HPAVC_CaptureOpen");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_CaptureClose,"HPAVC_CaptureClose");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_CaptureEncodeData,"HPAVC_CaptureEncodeData");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_CaptureGetVideoWidth,"HPAVC_CaptureGetVideoWidth");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_CaptureGetVideoHeigh,"HPAVC_CaptureGetVideoHeigh");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_CaptureCaptureToBMPFile,"HPAVC_CaptureCaptureToBMPFile");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_CaptureSetWindowPos,"HPAVC_CaptureSetWindowPos");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_CaptureSetVideoParam,"HPAVC_CaptureSetVideoParam");

		m_pXDllLoadLibrary->LoadFunction(pHPAVC_EncoderSetMaxCount,"HPAVC_EncoderSetMaxCount");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_EncoderTry,"HPAVC_EncoderTry");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_EncoderOpen,"HPAVC_EncoderOpen");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_EncoderClose,"HPAVC_EncoderClose");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_EncoderEncoding,"HPAVC_EncoderEncoding");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_EncoderSetVideoParam,"HPAVC_EncoderSetVideoParam");
		
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_D3DRenderOpen,"HPAVC_D3DRenderOpen");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_D3DRenderClose,"HPAVC_D3DRenderClose");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_D3DRenderPlayFrame,"HPAVC_D3DRenderPlayFrame");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_D3DRenderPlaySurface,"HPAVC_D3DRenderPlaySurface");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_D3DRenderSetWindowPos,"HPAVC_D3DRenderSetWindowPos");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_D3DRenderGetWnd,"HPAVC_D3DRenderGetWnd");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_D3DRenderSkipDisplay,"HPAVC_D3DRenderSkipDisplay");
		m_pXDllLoadLibrary->LoadFunction(pHPAVC_D3DRenderCaptureToBMPFile,"HPAVC_D3DRenderCaptureToBMPFile");	
	}
}

HPAVCWrapper::~HPAVCWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool HPAVCWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void HPAVC_init(void)
{
	if (!IsVistaOrAbove())
		return;

	try{
		if (g_pHPAVCWrapper==NULL)
		{
			g_pHPAVCWrapper=new HPAVCWrapper(g_hInstance);
			if (!g_pHPAVCWrapper->IsOK())
			{
				delete g_pHPAVCWrapper;
				g_pHPAVCWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void HPAVC_uninit(void)
{
	try{
		if (g_pHPAVCWrapper)
		{
			delete g_pHPAVCWrapper;
			g_pHPAVCWrapper=NULL;
		}
	}catch(...){

	}
}

void*HPAVC_PlayerOpen(HPAVC_PLAYER_CB_DC cbdc,void*pUserData,void* hWnd)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return NULL;
		return g_pHPAVCWrapper->pHPAVC_PlayerOpen(cbdc,pUserData, hWnd);
	}
	catch(...)
	{
	}

	return NULL;
}

void HPAVC_PlayerClose(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return ;
		g_pHPAVCWrapper->pHPAVC_PlayerClose(pHandle);
	}
	catch(...)
	{
	}
}
int  HPAVC_PlayerPlayData(void*pHandle,void*pData,int nLen)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_PlayerPlayData(pHandle,pData,nLen);
	}
	catch(...)
	{
	}
}
int  HPAVC_PlayerGetVideoWidth(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_PlayerGetVideoWidth(pHandle);
	}
	catch(...)
	{
	}
}
int  HPAVC_PlayerGetVideoHeigh(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_PlayerGetVideoHeigh(pHandle);
	}
	catch(...)
	{
	}
}
bool HPAVC_PlayerCaptureToBMPFile(void*pHandle,const char*cszFile)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return false;
		return g_pHPAVCWrapper->pHPAVC_PlayerCaptureToBMPFile(pHandle,cszFile);
	}
	catch(...)
	{
	}
}
bool HPAVC_PlayerSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return false;
		return g_pHPAVCWrapper->pHPAVC_PlayerSetWindowPos(pHandle,hWndInsertAfter/*HWND*/, x, y, cx, cy, nFlags);
	}
	catch(...)
	{
	}
}
void* HPAVC_PlayerGetWnd(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return NULL;
		return g_pHPAVCWrapper->pHPAVC_PlayerGetWnd(pHandle);
	}
	catch(...)
	{
	}
}
int  HPAVC_PlayerSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_PlayerSetVideoParam(pHandle,nBrightness,  nContrast,  nSaturation,  nHue, nDetail, nDenoise);
	}
	catch(...)
	{
	}
}


void*HPAVC_CaptureOpen(HPAVC_CAPTURE_CB_DC cbdc,HPAVC_CAPTURE_CB_DATA cbdata,void*pUserData,void* hWnd,int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nKeyIntMax)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return NULL;
		return g_pHPAVCWrapper->pHPAVC_CaptureOpen(cbdc,cbdata,pUserData, hWnd, nWidth, nHeight, nFrameRate, nProfile, nBitrate, nKeyIntMax);
	}
	catch(...)
	{
	}

	return NULL;
}

void HPAVC_CaptureClose(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return ;
		g_pHPAVCWrapper->pHPAVC_CaptureClose(pHandle);
	}
	catch(...)
	{
	}
}
int  HPAVC_CaptureEncodeData(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_CaptureEncodeData(pHandle,pData,nLen,nRequestKeyFrame);
	}
	catch(...)
	{
	}
}
int  HPAVC_CaptureGetVideoWidth(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_CaptureGetVideoWidth(pHandle);
	}
	catch(...)
	{
	}
}
int  HPAVC_CaptureGetVideoHeigh(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_CaptureGetVideoHeigh(pHandle);
	}
	catch(...)
	{
	}
}
bool HPAVC_CaptureCaptureToBMPFile(void*pHandle,const char*cszFile)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return false;
		return g_pHPAVCWrapper->pHPAVC_CaptureCaptureToBMPFile(pHandle,cszFile);
	}
	catch(...)
	{
	}
}

bool HPAVC_CaptureSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return false;
		return g_pHPAVCWrapper->pHPAVC_CaptureSetWindowPos(pHandle,hWndInsertAfter/*HWND*/, x, y, cx, cy, nFlags);
	}
	catch(...)
	{
	}
}
int  HPAVC_CaptureSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_CaptureSetVideoParam(pHandle,nBrightness,  nContrast,  nSaturation,  nHue, nDetail, nDenoise);
	}
	catch(...)
	{
	}
}

void HPAVC_EncoderSetMaxCount(unsigned int nMaxCount)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return ;
		g_pHPAVCWrapper->pHPAVC_EncoderSetMaxCount(nMaxCount);
	}
	catch(...)
	{
	}
}
int  HPAVC_EncoderTry(int nWidth,int nHeight)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_EncoderTry(nWidth, nHeight);
	}
	catch(...)
	{
	}
}
void*HPAVC_EncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,HPAVC_ENCODER_CB_DATA encoder_cb_data,HPAVC_ENCODER_CB_SF encoder_cb_sf,void*pUserData)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return NULL;
		return g_pHPAVCWrapper->pHPAVC_EncoderOpen(nWidth, nHeight, nFrameRate, nProfile, nBitrate, nQMin, nQMax, nKeyIntMax,encoder_cb_data,encoder_cb_sf,pUserData,XGetTimestamp());
	}
	catch(...)
	{
	}
}
void HPAVC_EncoderClose(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return ;
		g_pHPAVCWrapper->pHPAVC_EncoderClose(pHandle);
	}
	catch(...)
	{
	}
}
int  HPAVC_EncoderEncoding(void*pHandle,unsigned char*pData[],int nRequestKeyFrame)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_EncoderEncoding(pHandle,pData,nRequestKeyFrame);
	}
	catch(...)
	{
	}
}
int  HPAVC_EncoderSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_EncoderSetVideoParam(pHandle,nBrightness,  nContrast,  nSaturation,  nHue, nDetail, nDenoise);
	}
	catch(...)
	{
	}
}

void*HPAVC_D3DRenderOpen(HPAVC_D3DRENDER_CB_DC cbdc,void*pUserData,void* hWnd,bool bCreateWnd,bool bD3D11)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return NULL;
		return g_pHPAVCWrapper->pHPAVC_D3DRenderOpen(cbdc,pUserData, hWnd,bCreateWnd,bD3D11);
	}
	catch(...)
	{
	}
}
void HPAVC_D3DRenderClose(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return ;
		g_pHPAVCWrapper->pHPAVC_D3DRenderClose(pHandle);
	}
	catch(...)
	{
	}
}
int  HPAVC_D3DRenderPlaySurface(void*pHandle, void* pSurface,void* pmfxAlloc)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_D3DRenderPlaySurface(pHandle,pSurface,pmfxAlloc);
	}
	catch(...)
	{
	}
}
int  HPAVC_D3DRenderPlayFrame(void*pHandle, unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_D3DRenderPlayFrame(pHandle,pYV12Data, nWidth, nHeight,  nFrameRate);
	}
	catch(...)
	{
	}
}
bool HPAVC_D3DRenderSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return false;
		return g_pHPAVCWrapper->pHPAVC_D3DRenderSetWindowPos(pHandle,hWndInsertAfter/*HWND*/, x, y, cx, cy, nFlags);
	}
	catch(...)
	{
	}
}
void* HPAVC_D3DRenderGetWnd(void*pHandle)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return NULL;
		return g_pHPAVCWrapper->pHPAVC_D3DRenderGetWnd(pHandle);
	}
	catch(...)
	{
	}
}
int  HPAVC_D3DRenderSkipDisplay(void*pHandle,int nSkipCount/*本次跳帧的数量*/)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_D3DRenderSkipDisplay(pHandle,nSkipCount);
	}
	catch(...)
	{
	}
}
bool HPAVC_D3DRenderCaptureToBMPFile(void*pHandle,const char*cszFile)
{
	try{
		if (g_pHPAVCWrapper==NULL)
			return 0;
		return g_pHPAVCWrapper->pHPAVC_D3DRenderCaptureToBMPFile(pHandle,cszFile);
	}
	catch(...)
	{
	}
}
