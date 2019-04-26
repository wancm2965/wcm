/*****************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or
nondisclosure agreement with Intel Corporation and may not be copied
or disclosed except in accordance with the terms of that agreement.
Copyright(c) 2005-2014 Intel Corporation. All Rights Reserved.

*****************************************************************************/

#ifndef __PIPELINE_DECODE_H__
#define __PIPELINE_DECODE_H__

#include "sample_defs.h"
#include "mfx_plugin_uids.h"

#include "../HPAVC_Util/XThreadBase.h"
#include "../HPAVC_Util/XListPtr.h"
#include "../HPAVC_Util/XAutoLock.h"

#include "../HPAVC_D3DRender/D3DRender.h"

#if defined(_WIN32) || defined(_WIN64)

#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#include <dxva.h>
#include <windows.h>
#include "igfx_s3dcontrol.h"
#endif

#include "mfxstructures.h"
#include "mfxvideo.h"

#include "hw_device.h"

#if D3D_SURFACES_SUPPORT
#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#endif

#include <vector>
#include "hw_device.h"
//#include "decode_render.h"
#include "mfx_buffering.h"
#include <memory>

#include "sample_defs.h"
#include "sample_utils.h"
#include "base_allocator.h"
#include "general_allocator.h"

#include "mfxmvc.h"
#include "mfxjpeg.h"
#include "mfxplugin.h"
#include "mfxplugin++.h"
#include "mfxvideo.h"
#include "mfxvideo++.h"
#include "d3d_device.h"
#include "HPAVC_Player.h"
#include "HPAVC_D3DRender.h"

enum MemType {
    SYSTEM_MEMORY = 0x00,
    D3D9_MEMORY   = 0x01,
    D3D11_MEMORY  = 0x02,
};


typedef enum {
	VIDEC_CODEC_H261=0,		//unsupport	
	VIDEC_CODEC_H263,		
	VIDEC_CODEC_H263P,		
	VIDEC_CODEC_H263PP,		//same to H263P		
	VIDEC_CODEC_H264,
	VIDEC_CODEC_LSCC,		//unsupport
	VIDEC_CODEC_AH400,
	VIDEC_CODEC_MPEG4,		//unsupport
	VIDEC_CODEC_DH264,
	VIDEC_CODEC_HIKH,
	VIDEC_CODEC_H264_SVC,
	VIDEC_CODEC_HIKC,
	VIDEC_CODEC_MJPG,
	VIDEC_CODEC_IMKH,
	VIDEC_CODEC_DS81,
	VIDEC_CODEC_DS71,
	VIDEC_CODEC_X62MF,
	VIDEC_CODEC_MEGA,
	VIDEC_CODEC_DS80HD,
	VIDEC_CODEC_USER,
	VIDEC_CODEC_COUNT
}VIDEC_CODEC_TYPE;

class DecodingPipelineCallback
{
public:
	DecodingPipelineCallback(void){}
	virtual~DecodingPipelineCallback(void){};
public:
	virtual void OnDecodingPipelineCallbackDrawDC(void* hDC/*HDC*/){};
};

class CDecodingPipeline
	:public CBuffering
	,public XThreadBase
{
public:
    CDecodingPipeline(DecodingPipelineCallback&rCallback);
    virtual ~CDecodingPipeline();

    int  Open(HWND hWnd,mfxU32	videoType,bool bUseSoftwareDecoder,bool	bUseH263Decoder,HPAVC_D3DRender*pD3DRender11,HPAVC_D3DRender*pD3DRender9);
    void Close();
	mfxStatus DecodeFrame(void*pPacketData,int nPacketSize);
	int GetVideoWidth(void);
	int GetVideoHeight(void);
	int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);
protected: // functions
	virtual mfxStatus Init(void);
	virtual mfxStatus InitMfxParams(void);
    virtual mfxStatus InitVppParams();
    virtual mfxStatus AllocAndInitVppDoNotUse();

    virtual mfxStatus CreateAllocator();
    virtual mfxStatus CreateHWDevice();
    virtual mfxStatus AllocFrames();
    virtual void DeleteFrames();
    virtual void DeleteAllocator();
    virtual mfxStatus SyncOutputSurface(mfxU32 wait);

	virtual void ThreadProcMain(void);

protected: // variables
    mfxBitstream            m_mfxBS; // contains encoded data

    MFXVideoSession         m_mfxSession;
    MFXVideoDECODE*         m_pmfxDEC;
    MFXVideoVPP*            m_pmfxVPP;
    mfxVideoParam           m_mfxVideoParams;
    mfxVideoParam           m_mfxVppVideoParams;
    std::auto_ptr<MFXVideoUSER>  m_pUserModule;
    std::auto_ptr<MFXPlugin> m_pPlugin;
    const msdkPluginUID*     m_pUID;

    GeneralAllocator*       m_pGeneralAllocator;
    mfxAllocatorParams*     m_pmfxAllocatorParams;
	mfxU32					m_videoType;

    mfxFrameAllocResponse   m_mfxResponse;      // memory allocation response for decoder
    mfxFrameAllocResponse   m_mfxVppResponse;   // memory allocation response for vpp

    msdkFrameSurface*       m_pCurrentFreeSurface; // surface detached from FREEMSG surfaces array
    msdkFrameSurface*       m_pCurrentFreeVppSurface; // VPP surface detached from FREEMSG VPP surfaces array
    msdkOutputSurface*      m_pCurrentFreeOutputSurface; // surface detached from FREEMSG output surfaces array
    msdkOutputSurface*      m_pCurrentOutputSurface; // surface detached from output surfaces array


    mfxExtVPPDoNotUse       m_VppDoNotUse;      // for disabling VPP algorithms
    mfxExtBuffer*           m_VppExtParams[4];

	mfxExtVPPProcAmp		m_VppProcAmp;
	mfxExtVPPDetail			m_VppDetailConfig;
	mfxExtVPPDenoise		m_VppDenoiseConfig;

	bool					m_bVppParamsChanged;

	HPAVC_D3DRender*		m_pD3DRender11;

	HPAVC_D3DRender*		m_pD3DRender9;

    mfxU32 m_nTimeout;  //enables timeout for video playback, measured in seconds

	HWND					m_hWnd;

	bool					m_bWaitStop;
	bool					m_bDecodedHeader;
	DecodingPipelineCallback&	m_rCallback;

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

	XListPtr				m_listPacketData;
	XCritSec				m_csListPacketData;
	bool					m_bUseSoftwareDecoder;
	bool					m_bUseH263Decoder;

private:
    CDecodingPipeline(const CDecodingPipeline&);
    void operator=(const CDecodingPipeline&);
};

#endif // __PIPELINE_DECODE_H__
