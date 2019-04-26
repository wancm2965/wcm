
#ifndef __DECODE_PLAYER_H__
#define __DECODE_PLAYER_H__

#include "HPAVC_Player.h"
#include "../HPAVC_Util/VIDEC_Header.h"
#include "mfx_samples_config.h"

#define UsePipeline

#ifdef UsePipeline
#include "pipeline_decode.h"
#else
#include "IntelDecoderRenderer.h"

#endif // UsePipeline

#include <sstream>

#include "../HPAVC_Util/XThreadBase.h"
#include "../HPAVC_Util/XListPtr.h"
#include "../HPAVC_Util/XAutoLock.h"

struct XPacketBuffer
{
	XPacketBuffer(void)
		: pBuffer(NULL)
		, nLen(0)
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
};


class decode_Player
	:public HPAVC_Player
	,public XThreadBase	
	,public DecodingPipelineCallback
	,public HPAVC_D3DRenderCallback
{
public:
	decode_Player(HPAVC_PLAYER_CB_DC cbdc,void*pUserData);
	decode_Player(HPAVC_PlayerCallback*pCallback);
	virtual~decode_Player(void);
public:
	virtual int Open(void* hWnd/*HWND*/);
	virtual void Close(void);
	virtual int GetVideoWidth(void);
	virtual int GetVideoHeight(void);
	virtual bool SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags); 
	virtual void* GetWnd(void); 
	virtual int PlayData(void*pPacketData,int nPacketSize);
	virtual bool CaptureToBMPFile(const char*cszFile);
	virtual int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);

	virtual void ThreadProcMain(void);

	virtual void OnDecodingPipelineCallbackDrawDC(void* hDC/*HDC*/);
	virtual void OnHPAVC_D3DRenderCallbackDrawDC(void* hDC);

protected:
	int PlayerTry(unsigned long nWidth,unsigned long nHeight,unsigned short usType);


private:
	HPAVC_PlayerCallback*		m_pCallback;
	HPAVC_PLAYER_CB_DC			m_cbdc;
	void*						m_pUserData;
	HWND						m_hWnd;
	bool						m_bUseSoftwareDecoder;
	bool						m_bUseH263Decoder;
	void* m_hWndInsertAfter;int m_x, m_y, m_cx, m_cy, m_nFlags;
	unsigned long				m_nWidth;
	unsigned long				m_nHeight;
	
#ifdef UsePipeline
	CDecodingPipeline*			m_pPipeline;
#else
	IntelDecoderRenderer		m_Pipeline;
#endif

	HPAVC_D3DRender*			m_pD3DRender11;
	HPAVC_D3DRender*			m_pD3DRender9;
	

	bool						m_bWaitStop;
	XListPtr					m_listPacketData;
	XCritSec					m_csListPacketData;

	unsigned long				m_nLastTS;
	unsigned long				m_nFrameCount;


};



#endif // __DECODE_PLAYER_H__
