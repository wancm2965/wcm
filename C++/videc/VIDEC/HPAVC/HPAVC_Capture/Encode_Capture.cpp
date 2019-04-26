#include "Encode_Capture.h"

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

//#include "../../VIDEC_FRE/VIDECFREHeader.h"
//
//void * FreMalloc(size_t size,char * fileName,char * func,int nLine)
//{
//	void * p=NULL;
//	p = malloc(size);
//	if(p == NULL)
//		DebugOutput("FreMalloc NULL,File:%s,FUN:%s,Line:%d \n", fileName,   func,  nLine);
//
//	return p;
//}

void*HPAVC_CaptureOpen(HPAVC_CAPTURE_CB_DC cbdc,HPAVC_CAPTURE_CB_DATA cbdata,void*pUserData,void* hWnd,int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nKeyIntMax)
{
	NEWRECORD3( Encode_Capture*,pEncode_Capture , new Encode_Capture(cbdc,cbdata,pUserData,hWnd) )
	if (pEncode_Capture)
	{
		pEncode_Capture->Open(nWidth, nHeight, nFrameRate, nProfile, nBitrate, nKeyIntMax);
	}
	return pEncode_Capture;
}

void HPAVC_CaptureClose(void*pHandle)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		pEncode_Capture->Close();
		delete pEncode_Capture;
		pEncode_Capture = NULL;
	}
}
int  HPAVC_CaptureEncodeData(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		return pEncode_Capture->EncodeData(pData, nLen,nRequestKeyFrame);
	}
	return 0;
}

int  HPAVC_CaptureGetVideoWidth(void*pHandle)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		return pEncode_Capture->GetVideoWidth();
	}
	return 0;
}

int  HPAVC_CaptureGetVideoHeigh(void*pHandle)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		return pEncode_Capture->GetVideoHeight();
	}
	return 0;
}

bool HPAVC_CaptureCaptureToBMPFile(void*pHandle,const char*cszFile)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		return pEncode_Capture->CaptureToBMPFile(cszFile);
	}
	return false;
}

bool HPAVC_CaptureSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		return pEncode_Capture->SetWindowPos(hWndInsertAfter/*HWND*/, x, y, cx, cy,nFlags);
	}
	return false;
}

int  HPAVC_CaptureSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		return pEncode_Capture->SetVideoParam(nBrightness,  nContrast,  nSaturation,  nHue, nDetail, nDenoise);
	}
	return 0;
}

#include <map>
static bool g_bIntelH264EncoderInitialized=false;
static bool g_bIntelH264EncoderHardware=false;
typedef std::map<void*,Encode_Capture*>	MapEncode_Capture;
static MapEncode_Capture g_MapEncode_Capture;
static unsigned int g_nMaxEncode_CaptureCount=20;

void HPAVC_EncoderSetMaxCount(unsigned int nMaxCount)
{
	//g_nMaxEncode_CaptureCount=nMaxCount;
}

int  HPAVC_EncoderTry(int nWidth,int nHeight)
{
	if (!g_bIntelH264EncoderInitialized)
	{
		g_bIntelH264EncoderInitialized=true;
		mfxSession tSession;
		mfxIMPL impl = MFX_IMPL_HARDWARE;
		mfxVersion ver = {0, 1};
		mfxStatus nStatus=MFXInit(MFX_IMPL_HARDWARE,&ver,&tSession);
		MFXClose(tSession);
		if (nStatus!=MFX_ERR_NONE)
		{
			return -1;
		}
		g_bIntelH264EncoderHardware=true;
	}

	if (g_bIntelH264EncoderHardware)
	{
		//if (nWidth==0 && nHeight==0)
		//{
		//	return 0;
		//}
		if (g_MapEncode_Capture.size()<g_nMaxEncode_CaptureCount)
		{
			return 0;
		}
	}
	return -1;
}

void*HPAVC_EncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,HPAVC_ENCODER_CB_DATA encoder_cb_data,HPAVC_ENCODER_CB_SF encoder_cb_sf,void*pUserData,int nKey)
{
	//unsigned long nTimestamp=timeGetTime();
	//if (nTimestamp-nKey>1000)
	//{
	//	return NULL;
	//}
	if (encoder_cb_data==NULL)
		return NULL;
	NEWRECORD3( Encode_Capture*,pEncode_Capture , new Encode_Capture(encoder_cb_data,encoder_cb_sf,pUserData,NULL) )
	if (pEncode_Capture)
	{
		if (pEncode_Capture->Open(nWidth, nHeight, nFrameRate, nProfile, nBitrate, nKeyIntMax) == -1)
		{
			pEncode_Capture->Close();
			delete pEncode_Capture;
			pEncode_Capture = NULL;
		}
	}
	return pEncode_Capture;
}

void HPAVC_EncoderClose(void*pHandle)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		pEncode_Capture->Close();
		delete pEncode_Capture;
		pEncode_Capture = NULL;
	}
}

int  HPAVC_EncoderEncoding(void*pHandle,unsigned char*pData[],int nRequestKeyFrame)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		return pEncode_Capture->EncodeData(pData, nRequestKeyFrame);
	}
	return 0;
}

int  HPAVC_EncoderSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	Encode_Capture* pEncode_Capture = (Encode_Capture *)pHandle;  
	if (pEncode_Capture)
	{
		return pEncode_Capture->SetVideoParam(nBrightness,  nContrast,  nSaturation,  nHue, nDetail, nDenoise);
	}
	return 0;
}


HPAVC_Capture*HPAVC_Capture::Create(HPAVC_CaptureCallback*pCallback,void* hWnd/*HWND*/)
{
	return new Encode_Capture(pCallback,hWnd);
}

Encode_Capture::Encode_Capture(HPAVC_ENCODER_CB_DATA encoder_cb_data,HPAVC_ENCODER_CB_SF encoder_cb_sf,void*pUserData,void* hWnd)
:m_pCallback(NULL)
,m_encoder_cb_data(encoder_cb_data)
,m_encoder_cb_sf(encoder_cb_sf)
,m_cbdc(NULL)
,m_cbdata(NULL)
,m_pUserData(pUserData)
,m_plEncoderRenderer(NULL)
,m_bWaitStop(true)
{
	m_nWidth=0;
	m_nHeight=0;
	m_nY=0;
	m_nUV=0;
	m_pD3DRender = NULL;
	g_MapEncode_Capture[this]=this;

	m_pD3DRender = HPAVC_D3DRender::Create(this);
	if (m_pD3DRender)
	{
		m_pD3DRender->Open((HWND)hWnd,false,false);
	}
}

Encode_Capture::Encode_Capture(HPAVC_CAPTURE_CB_DC cbdc,HPAVC_CAPTURE_CB_DATA cbdata,void*pUserData,void* hWnd)
:m_pCallback(NULL)
,m_encoder_cb_data(NULL)
,m_encoder_cb_sf(NULL)
,m_cbdc(cbdc)
,m_cbdata(cbdata)
,m_pUserData(pUserData)
,m_plEncoderRenderer(NULL)
,m_bWaitStop(true)
{
	m_nWidth=0;
	m_nHeight=0;
	m_nY=0;
	m_nUV=0;
	m_pD3DRender = NULL;
	g_MapEncode_Capture[this]=this;

	m_pD3DRender = HPAVC_D3DRender::Create(this);
	if (m_pD3DRender)
	{
		m_pD3DRender->Open((HWND)hWnd,false,false);
	}
}
Encode_Capture::Encode_Capture(HPAVC_CaptureCallback*pCallback,void* hWnd)
:m_pCallback(pCallback)
,m_encoder_cb_data(NULL)
,m_encoder_cb_sf(NULL)
,m_cbdc(NULL)
,m_cbdata(NULL)
,m_pUserData(NULL)
,m_plEncoderRenderer(NULL)
,m_bWaitStop(true)
{
	m_nWidth=0;
	m_nHeight=0;
	m_nY=0;
	m_nUV=0;
	m_pD3DRender = NULL;
	g_MapEncode_Capture[this]=this;

	m_pD3DRender = HPAVC_D3DRender::Create(this);
	if (m_pD3DRender)
	{
		m_pD3DRender->Open((HWND)hWnd,false,false);
	}
}

Encode_Capture::~Encode_Capture(void)
{
	g_MapEncode_Capture.erase(this);
}

bool Encode_Capture::IsOpen(void)
{
	return (m_plEncoderRenderer !=NULL);
}

int Encode_Capture::Open(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nKeyIntMax)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nY=nWidth*nHeight;
	m_nUV=m_nY/4;
	m_nFrameRate = nFrameRate;

	if (m_pD3DRender && m_cx > 0)
	{
		m_pD3DRender->SetWindowPos(m_hWndInsertAfter, m_x, m_y, m_cx, m_cy, m_nFlags);
	}	

	NEWRECORD( m_plEncoderRenderer , new IntelEncoderRenderer(*this) )

	if (!m_plEncoderRenderer)
	{
		return -1;
	}

	sInputParams tInputParams;   // input parameters from command line
	MSDK_ZERO_MEMORY(tInputParams);
	tInputParams.CodecId = MFX_CODEC_AVC;
	tInputParams.CodecProfile = (nProfile==HPAVC_BASELINE_PROFILE?MFX_PROFILE_AVC_BASELINE:MFX_PROFILE_AVC_HIGH);
	//tInputParams.CodecProfile = MFX_PROFILE_AVC_HIGH;
	if (tInputParams.CodecProfile==MFX_PROFILE_AVC_BASELINE)
	{
		if (nBitrate<=64)
		{
			tInputParams.CodecLevel=MFX_LEVEL_AVC_1;
		}
		else if (nBitrate<=192)
		{
			tInputParams.CodecLevel=MFX_LEVEL_AVC_11;
		}
		else if (nBitrate<=768)
		{
			tInputParams.CodecLevel=MFX_LEVEL_AVC_12;
		}
		else 
		{
			if (nWidth<=352)
			{
				tInputParams.CodecLevel=MFX_LEVEL_AVC_2;
			}
			else
			{
				tInputParams.CodecLevel=MFX_LEVEL_AVC_22;
			}
		}
	}
	else if (tInputParams.CodecProfile==MFX_PROFILE_AVC_HIGH)
	{
		if (nWidth>720)
		{
			tInputParams.CodecLevel = MFX_LEVEL_AVC_41;
		}
		else
		{
			tInputParams.CodecLevel = MFX_LEVEL_AVC_31;
		}
	}
	tInputParams.bUseHWLib = true;
	tInputParams.ColorFormat = MFX_FOURCC_NV12;
	tInputParams.nPicStruct = MFX_PICSTRUCT_FIELD_BFF;
	tInputParams.nTargetUsage = MFX_TARGETUSAGE_BALANCED;// MFX_TARGETUSAGE_BEST_QUALITY;//MFX_TARGETUSAGE_BEST_SPEED;//;
	tInputParams.nWidth = nWidth;
	tInputParams.nHeight = nHeight;
	tInputParams.nDstWidth = nWidth;
	tInputParams.nDstHeight = nHeight;
	tInputParams.dFrameRate = (mfxF64)nFrameRate;
	tInputParams.nBitRate =  nBitrate;
	tInputParams.nMaxKeyFrameInterval =  nKeyIntMax;

	tInputParams.ColorFormat = MFX_FOURCC_YV12;
	tInputParams.nPicStruct = MFX_PICSTRUCT_PROGRESSIVE;
	
	tInputParams.memType=D3D9_MEMORY;

	if (MFX_ERR_NONE != m_plEncoderRenderer->Init(&tInputParams,m_pD3DRender))
	{
		m_plEncoderRenderer->Close();
		delete m_plEncoderRenderer;
		m_plEncoderRenderer = NULL;
		return -1;
	}

	m_bWaitStop = false;
	StartThread();

	m_nLastTS=GetTickCount();
	m_nFrameCount=0;

	return 0;
}

void Encode_Capture::Close(void)
{
	m_bWaitStop = true;
	WaitForStop();

	{
		XAutoLock l(m_csListPacketData);
		while (m_listPacketData.size())
		{
			XPacketBuffer*pXPacketBuffer=NULL;
			pXPacketBuffer=(XPacketBuffer*)m_listPacketData.front();
			m_listPacketData.pop_front();
			if (pXPacketBuffer)
			{			
				delete pXPacketBuffer;
				pXPacketBuffer=NULL;
			}
		}
	}

	if (m_plEncoderRenderer)
	{
		m_plEncoderRenderer->Close();
		delete m_plEncoderRenderer;
		m_plEncoderRenderer = NULL;
	}

	if (m_pD3DRender)
	{
		m_pD3DRender->Close();
		delete m_pD3DRender;
		m_pD3DRender = NULL;
	}
}

void Encode_Capture::ThreadProcMain(void)
{
	while(!m_bWaitStop)
	{
		XPacketBuffer*pXPacketBuffer=NULL;
		{
			XAutoLock l(m_csListPacketData);
			if (m_listPacketData.size())
			{
				pXPacketBuffer=(XPacketBuffer*)m_listPacketData.front();
				m_listPacketData.pop_front();
			}
		}
		if (pXPacketBuffer!=NULL)
		{				
			unsigned char*pYUV[3];
			pYUV[0]=(unsigned char*)pXPacketBuffer->pBuffer;
			pYUV[1]=(unsigned char*)pXPacketBuffer->pBuffer+m_nY;
			pYUV[2]=pYUV[1]+m_nUV;

			EncodeData(pYUV,pXPacketBuffer->nRequestKeyFrame);

			delete pXPacketBuffer;
			pXPacketBuffer = NULL;
		}
		else
		{
			Sleep(1);
		}
	}
}

int Encode_Capture::GetVideoWidth(void)
{
	if (m_plEncoderRenderer)
	{
		return m_plEncoderRenderer->GetVideoWidth();
	}
	return m_nWidth;
}

int Encode_Capture::GetVideoHeight(void)
{
	if (m_plEncoderRenderer)
	{
		return m_plEncoderRenderer->GetVideoHeight();
	}
	return m_nHeight;
}

bool Encode_Capture::SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	if (m_pD3DRender)
	{
		m_pD3DRender->SetWindowPos(hWndInsertAfter, x, y, cx, cy,nFlags);
	}
	m_hWndInsertAfter = hWndInsertAfter; m_x = x; m_y = y; m_cx = cx; m_cy = cy; m_nFlags = nFlags;
	return false;
}

int Encode_Capture::EncodeData(unsigned char*pData[],int nRequestKeyFrame)
{
	mfxStatus sts = MFX_ERR_NONE;

#if D3DRENDER_DEBUG
	if (m_pD3DRender)
	{
		sts = (mfxStatus)m_pD3DRender->RenderFrame(pData,m_nWidth,m_nHeight,m_nFrameRate);
	}
#else
	if (m_plEncoderRenderer)
	{
		sts = m_plEncoderRenderer->EncodeFrame(pData,nRequestKeyFrame);
	}
#endif

	return sts;
}

int Encode_Capture::EncodeData(unsigned char*pData,int nLen,int nRequestKeyFrame)
{
	unsigned char*pYUV[3];
	pYUV[0]=(unsigned char*)pData;
	pYUV[1]=(unsigned char*)pData+m_nY;
	pYUV[2]=pYUV[1]+m_nUV;

	EncodeData(pYUV,nRequestKeyFrame);

	//if (m_bWaitStop)
	//{
	//	return -1;
	//}
	//XAutoLock l(m_csListPacketData);
	//XPacketBuffer*pXPacketBuffer=new XPacketBuffer;
	//if (pXPacketBuffer)
	//{
	//	pXPacketBuffer->pBuffer=MALLOCMSG(nLen);
	//	if (pXPacketBuffer->pBuffer)
	//	{
	//		memcpy(pXPacketBuffer->pBuffer,pData,nLen);
	//		pXPacketBuffer->nLen=nLen;
	//		pXPacketBuffer->nRequestKeyFrame=nRequestKeyFrame;
	//		m_listPacketData.push_back(pXPacketBuffer);
	//	}
	//	else
	//	{
	//		delete pXPacketBuffer;
	//		pXPacketBuffer=NULL;
	//	}
	//}

	return 0;
}

bool Encode_Capture::CaptureToBMPFile(const char*cszFile)
{
	if (m_pD3DRender)
	{
		return m_pD3DRender->CaptureToBMPFile(cszFile);
	}
	return false;
}

int Encode_Capture::SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	if (m_plEncoderRenderer)
	{
		return m_plEncoderRenderer->SetVideoParam(nBrightness, nContrast, nSaturation, nHue,nDetail,nDenoise);
	}
	return 0;
}

void Encode_Capture::OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)
{
	if (m_pCallback)
	{
		m_pCallback->OnHPAVC_CaptureCallbackPacketData(pData,nLen,nKeyFrame,GetVideoWidth(),GetVideoHeight());
	}
	if (m_cbdata)
	{
		m_cbdata(m_pUserData,pData,nLen,nKeyFrame,GetVideoWidth(),GetVideoHeight());
	}
	if (m_encoder_cb_data)
	{
		m_encoder_cb_data(m_pUserData,pData,nLen,nKeyFrame);
	}

#if (1)
	{
		m_nFrameCount++;
		if (GetTickCount()-m_nLastTS > 20000)
		{
			char szDebug[1024];
			sprintf(szDebug,"Encode_Capture::OnIntelH264EncoderCallback nFrameCount=%u  this = %u m_listPacketData.size = %u\n",m_nFrameCount,this,m_listPacketData.size());
			DebugOutput( logcallstack ,szDebug);

			m_nFrameCount=0;
			m_nLastTS=GetTickCount();
		}
	}
#endif

}
void Encode_Capture::OnIntelH264EncoderCallbackSurface(mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc)
{
	if (m_encoder_cb_sf)
	{
		m_encoder_cb_sf(m_pUserData,pSurface,pmfxAlloc);
	}
}
void Encode_Capture::OnIntelH264EncoderCallbackDraw(void* hDC)
{
	if (m_cbdc)
	{
		m_cbdc(m_pUserData,hDC);
	}
	if (m_pCallback)
	{
		m_pCallback->OnHPAVC_CaptureCallbackDrawDC(hDC);
	}
}
void Encode_Capture::OnHPAVC_D3DRenderCallbackDrawDC(void* hDC/*HDC*/)
{
	if (m_pCallback)
	{
		m_pCallback->OnHPAVC_CaptureCallbackDrawDC(hDC);
	}
	if (m_cbdc)
	{
		m_cbdc(m_pUserData,hDC);
	}
}
