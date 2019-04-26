
#ifndef __ENCODE_CAPTURE_H__
#define __ENCODE_CAPTURE_H__

#include "HPAVC_Capture.h"
#include "HPAVC_Encoder.h"
#include "../HPAVC_Util/VIDEC_Header.h"
#include "mfx_samples_config.h"

#include "IntelEncoderRenderer.h"

#include <sstream>

#include "../HPAVC_Util/XThreadBase.h"
#include "../HPAVC_Util/XListPtr.h"
#include "../HPAVC_Util/XAutoLock.h"

#include "../../VIDEC_FRE/VIDECFREAPI.h"

#define D3DRENDER_DEBUG 0

struct XPacketBuffer
{
	XPacketBuffer(void)
		: pBuffer(NULL)
		, nLen(0)
		, nRequestKeyFrame(0)
	{
	}
	virtual~XPacketBuffer(void)
	{
		if(pBuffer)
		{
			FREEMSG(pBuffer);
			pBuffer=NULL;
		}
	}
	void*			pBuffer;
	int				nLen;
	int				nRequestKeyFrame;
};

class Encode_Capture
	:public HPAVC_Capture
	,public XThreadBase	
	,public IntelH264EncoderCallback
	,public HPAVC_D3DRenderCallback
{
public:
	Encode_Capture(HPAVC_ENCODER_CB_DATA encoder_cb_data,HPAVC_ENCODER_CB_SF encoder_cb_sf,void*pUserData,void* hWnd);
	Encode_Capture(HPAVC_CAPTURE_CB_DC cbdc,HPAVC_CAPTURE_CB_DATA cbdata,void*pUserData,void* hWnd);
	Encode_Capture(HPAVC_CaptureCallback*pCallback,void* hWnd);
	virtual~Encode_Capture(void);
public:
	virtual int  Open(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nKeyIntMax);
	virtual bool IsOpen(void);
	virtual void Close(void);
	virtual int	 EncodeData(unsigned char*pData,int nLen,int nRequestKeyFrame);
	virtual int	 EncodeData(unsigned char*pData[],int nRequestKeyFrame);
	virtual int  GetVideoWidth(void);
	virtual int  GetVideoHeight(void);
	virtual bool SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags); 
	virtual bool CaptureToBMPFile(const char*cszFile);
	virtual int  SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);
	virtual void ThreadProcMain(void);

	virtual void OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame);
	virtual void OnIntelH264EncoderCallbackDraw(void* hDC);
	virtual void OnIntelH264EncoderCallbackSurface(mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc);
	virtual void OnHPAVC_D3DRenderCallbackDrawDC(void* hDC/*HDC*/);
private:
	HPAVC_CaptureCallback*		m_pCallback;
	HPAVC_CAPTURE_CB_DC			m_cbdc;
	HPAVC_CAPTURE_CB_DATA		m_cbdata;
	HPAVC_ENCODER_CB_DATA		m_encoder_cb_data;
	HPAVC_ENCODER_CB_SF			m_encoder_cb_sf;
	void*						m_pUserData;
	IntelEncoderRenderer*		m_plEncoderRenderer;
	int							m_nWidth;
	int							m_nHeight;
	int							m_nY;
	int							m_nUV;
	int							m_nFrameRate;	
	bool						m_bWaitStop;
	XListPtr					m_listPacketData;
	XCritSec					m_csListPacketData;
	void* m_hWndInsertAfter;int m_x, m_y, m_cx, m_cy, m_nFlags;
	HPAVC_D3DRender*			m_pD3DRender;

	unsigned long				m_nLastTS;
	unsigned long				m_nFrameCount;

};



#endif // __DECODE_PLAYER_H__
