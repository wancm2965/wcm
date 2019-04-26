#ifndef __HPAVCWRAPPER_H__
#define __HPAVCWRAPPER_H__

#include "XDllLoadLibrary.h"
#include <HPAVC/HPAVC_Player.h>
#include <HPAVC/HPAVC_Capture.h>
#include <HPAVC/HPAVC_Encoder.h>
#include <HPAVC/HPAVC_D3DRender.h>

void HPAVC_init(void);
void HPAVC_uninit(void);

void*HPAVC_PlayerOpen(HPAVC_PLAYER_CB_DC cbdc,void*pUserData,void* hWnd);
void HPAVC_PlayerClose(void*pHandle);
int  HPAVC_PlayerPlayData(void*pHandle,void*pData,int nLen);
int  HPAVC_PlayerGetVideoWidth(void*pHandle);
int  HPAVC_PlayerGetVideoHeigh(void*pHandle);
bool HPAVC_PlayerCaptureToBMPFile(void*pHandle,const char*cszFile);
bool HPAVC_PlayerSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
void* HPAVC_PlayerGetWnd(void*pHandle);
int  HPAVC_PlayerSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);

void*HPAVC_CaptureOpen(HPAVC_CAPTURE_CB_DC cbdc,HPAVC_CAPTURE_CB_DATA cbdata,void*pUserData,void* hWnd,int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nKeyIntMax);
void HPAVC_CaptureClose(void*pHandle);
int  HPAVC_CaptureEncodeData(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame);
int  HPAVC_CaptureGetVideoWidth(void*pHandle);
int  HPAVC_CaptureGetVideoHeigh(void*pHandle);
bool HPAVC_CaptureCaptureToBMPFile(void*pHandle,const char*cszFile);
bool HPAVC_CaptureSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
int  HPAVC_CaptureSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);

void HPAVC_EncoderSetMaxCount(unsigned int nMaxCount);
int  HPAVC_EncoderTry(int nWidth,int nHeight);
void*HPAVC_EncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,HPAVC_ENCODER_CB_DATA encoder_cb_data,HPAVC_ENCODER_CB_SF encoder_cb_sf,void*pUserData);
void HPAVC_EncoderClose(void*pHandle);
int  HPAVC_EncoderEncoding(void*pHandle,unsigned char*pData[],int nRequestKeyFrame);
int  HPAVC_EncoderSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);

void*HPAVC_D3DRenderOpen(HPAVC_D3DRENDER_CB_DC cbdc,void*pUserData,void* hWnd,bool bCreateWnd,bool bD3D11);
void HPAVC_D3DRenderClose(void*pHandle);
int  HPAVC_D3DRenderPlayFrame(void*pHandle, unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate);
int  HPAVC_D3DRenderPlaySurface(void*pHandle, void* pSurface,void* pmfxAlloc);
bool HPAVC_D3DRenderSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
void*HPAVC_D3DRenderGetWnd(void*pHandle);
int  HPAVC_D3DRenderSkipDisplay(void*pHandle,int nSkipCount/*本次跳帧的数量*/);
bool HPAVC_D3DRenderCaptureToBMPFile(void*pHandle,const char*cszFile);


class HPAVCWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	HPAVCWrapper(HINSTANCE hInstance);
	~HPAVCWrapper(void);
public:
	static HPAVCWrapper&Instance(void);
	bool IsOK(void);

	void* (*pHPAVC_PlayerOpen)(HPAVC_PLAYER_CB_DC cbdc,void*pUserData,void* hWnd);
	void (*pHPAVC_PlayerClose)(void*pHandle);
	int (*pHPAVC_PlayerPlayData)(void*pHandle,void*pData,int nLen);
	int (*pHPAVC_PlayerGetVideoWidth)(void*pHandle);
	int (*pHPAVC_PlayerGetVideoHeigh)(void*pHandle);
	bool(*pHPAVC_PlayerCaptureToBMPFile)(void*pHandle,const char*cszFile);
	bool (*pHPAVC_PlayerSetWindowPos)(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
	void*(*pHPAVC_PlayerGetWnd)(void*pHandle);
	int (*pHPAVC_PlayerSetVideoParam)(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);

	void* (*pHPAVC_CaptureOpen)(HPAVC_CAPTURE_CB_DC cbdc,HPAVC_CAPTURE_CB_DATA cbdata,void*pUserData,void* hWnd,int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nKeyIntMax);
	void (*pHPAVC_CaptureClose)(void*pHandle);
	int (*pHPAVC_CaptureEncodeData)(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame);
	int (*pHPAVC_CaptureGetVideoWidth)(void*pHandle);
	int (*pHPAVC_CaptureGetVideoHeigh)(void*pHandle);
	bool(*pHPAVC_CaptureCaptureToBMPFile)(void*pHandle,const char*cszFile);
	bool (*pHPAVC_CaptureSetWindowPos)(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
	int (*pHPAVC_CaptureSetVideoParam)(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);

	void (*pHPAVC_EncoderSetMaxCount)(unsigned int nMaxCount);
	int  (*pHPAVC_EncoderTry)(int nWidth,int nHeight);
	void*(*pHPAVC_EncoderOpen)(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,HPAVC_ENCODER_CB_DATA encoder_cb_data,HPAVC_ENCODER_CB_SF encoder_cb_sf,void*pUserData,int nKey);
	void (*pHPAVC_EncoderClose)(void*pHandle);
	int  (*pHPAVC_EncoderEncoding)(void*pHandle,unsigned char*pData[],int nRequestKeyFrame);
	int  (*pHPAVC_EncoderSetVideoParam)(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);

	void*(*pHPAVC_D3DRenderOpen)(HPAVC_D3DRENDER_CB_DC cbdc,void*pUserData,void* hWnd,bool bCreateWnd,bool bD3D11);
	void (*pHPAVC_D3DRenderClose)(void*pHandle);
	int  (*pHPAVC_D3DRenderPlayFrame)(void*pHandle, unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate);
	int  (*pHPAVC_D3DRenderPlaySurface)(void*pHandle, void* pSurface,void* pmfxAlloc);
	bool (*pHPAVC_D3DRenderSetWindowPos)(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
	void*(*pHPAVC_D3DRenderGetWnd)(void*pHandle);
	int  (*pHPAVC_D3DRenderSkipDisplay)(void*pHandle,int nFlagSkipDisplay);
	bool (*pHPAVC_D3DRenderCaptureToBMPFile)(void*pHandle,const char*cszFile);
};



#endif
