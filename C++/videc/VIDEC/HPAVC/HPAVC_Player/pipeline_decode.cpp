/*********************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2005-2014 Intel Corporation. All Rights Reserved.

**********************************************************************************/

#include "mfx_samples_config.h"
#include "sample_defs.h"

#if defined(_WIN32) || defined(_WIN64)
#include <tchar.h>
#include <windows.h>
#endif

#include <ctime>
#include <algorithm>
#include "pipeline_decode.h"
#include "sysmem_allocator.h"
#include "../../HPAVC_user_modules/plugin_api/plugin_loader.h"

#if defined(_WIN32) || defined(_WIN64)
#include "d3d_allocator.h"
#include "d3d11_allocator.h"
#include "d3d_device.h"
#include "d3d11_device.h"
#endif

#if defined LIBVA_SUPPORT
#include "vaapi_allocator.h"
#include "vaapi_device.h"
#endif

#pragma warning(disable : 4100)

#define __SYNC_WA // avoid sync issue on Media SDK side


CDecodingPipeline::CDecodingPipeline(DecodingPipelineCallback&rCallback)
:m_rCallback(rCallback)
{
    m_pmfxDEC = NULL;
    m_pmfxVPP = NULL;
    m_pUID = NULL;
    m_pGeneralAllocator = NULL;
    m_pmfxAllocatorParams = NULL;

    m_pCurrentFreeSurface = NULL;
    m_pCurrentFreeVppSurface = NULL;
    m_pCurrentFreeOutputSurface = NULL;
    m_pCurrentOutputSurface = NULL;

	m_pD3DRender11  = NULL;
	m_pD3DRender9	= NULL;

	m_bDecodedHeader = false;
	m_bVppParamsChanged = false;


    MSDK_ZERO_MEMORY(m_mfxVideoParams);
    MSDK_ZERO_MEMORY(m_mfxVppVideoParams);

    MSDK_ZERO_MEMORY(m_mfxResponse);
    MSDK_ZERO_MEMORY(m_mfxVppResponse);

    MSDK_ZERO_MEMORY(m_mfxBS);

    MSDK_ZERO_MEMORY(m_VppExtParams[0]);
    MSDK_ZERO_MEMORY(m_VppExtParams[1]);
	MSDK_ZERO_MEMORY(m_VppExtParams[2]);
	MSDK_ZERO_MEMORY(m_VppExtParams[3]);

    MSDK_ZERO_MEMORY(m_VppDoNotUse);
    m_VppDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
    m_VppDoNotUse.Header.BufferSz = sizeof(m_VppDoNotUse);
}

CDecodingPipeline::~CDecodingPipeline()
{
	//Close();
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int  CDecodingPipeline::Open(HWND hWnd,mfxU32 videoType,bool bUseSoftwareDecoder,bool bUseH263Decoder,HPAVC_D3DRender*pD3DRender11,HPAVC_D3DRender*pD3DRender9)
{
	m_pD3DRender11			= pD3DRender11;
	m_pD3DRender9			= pD3DRender9;
	m_videoType				= videoType;
	m_hWnd					= hWnd;
	m_bUseSoftwareDecoder	= bUseSoftwareDecoder;
	m_bUseH263Decoder		= bUseH263Decoder;
	msdk_char strPluginPath[MSDK_MAX_FILENAME_LEN]={0};
	if (m_bUseH263Decoder)
	{
		msdk_strcopy(strPluginPath, _T("./HPAVC_plugin_h263.dll"));
	}
	mfxVersion min_version;
	mfxVersion version;     // real API version with which library is initialized
	min_version.Major = 1;
	min_version.Minor = 0;
	// Init session
	mfxStatus sts = MFX_ERR_NONE;

	mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
	if (m_bUseSoftwareDecoder)
	{
		impl = MFX_IMPL_SOFTWARE;
	}

#if MFX_D3D11_DECODE_SUPPORT
		impl |= MFX_IMPL_VIA_D3D11;
#endif

	sts = m_mfxSession.Init(impl, &min_version);
	if (MFX_ERR_NONE != sts)
		sts = m_mfxSession.Init((impl & !MFX_IMPL_HARDWARE_ANY) | MFX_IMPL_HARDWARE, &min_version);


	if (MFX_ERR_NONE != sts)
	{
		sts = m_mfxSession.Init(MFX_IMPL_SOFTWARE, &min_version);
	}

	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	sts = MFXQueryVersion(m_mfxSession , &version); // get real API version of the loaded library
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	// create decoder
	NEWRECORD( m_pmfxDEC , new MFXVideoDECODE(m_mfxSession) )
	MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_MEMORY_ALLOC);

	//create VPP
	NEWRECORD( m_pmfxVPP , new MFXVideoVPP(m_mfxSession) )
	if (!m_pmfxVPP) return MFX_ERR_MEMORY_ALLOC;

	// set video type in parameters
	m_mfxVideoParams.mfx.CodecId = m_videoType;

	// prepare bit stream
	sts = InitMfxBitstream(&m_mfxBS, 1024 * 1024);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, -1);  

	if (CheckVersion(&version, MSDK_FEATURE_PLUGIN_API)) 
	{
		if (msdk_strlen(strPluginPath))
		{
			m_pUserModule.reset(new MFXVideoUSER(m_mfxSession));
			if (m_videoType == MFX_CODEC_H263 ||m_videoType == MFX_CODEC_HEVC)
			{
				m_pPlugin.reset(LoadPlugin(MFX_PLUGINTYPE_VIDEO_DECODE, m_pUserModule.get(), strPluginPath));
			}
			if (m_pPlugin.get() == NULL) sts = MFX_ERR_UNSUPPORTED;
		}
		else
		{
			mfxSession session = m_mfxSession;
			// in case of HW library (-hw key) we will firstly try to load HW plugin
			// in case of failure - we will try SW one
			//if (pParams->bUseHWLib) {
			//	m_pUID = msdkGetPluginUID(MSDK_VDEC | MSDK_IMPL_HW, m_videoType);
			//}
			if (m_pUID) {
				sts = LoadPluginByUID(&session, m_pUID);
			}
			if ((MFX_ERR_NONE != sts) || !m_pUID) {
				m_pUID = msdkGetPluginUID(MSDK_VDEC | MSDK_IMPL_SW, m_videoType);
				if (m_pUID) {
					sts = LoadPluginByUID(&session, m_pUID);
				}
			}
		}
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}	

	sts = CreateHWDevice();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	//m_bWaitStop = false;
	//StartThread();

	return MFX_ERR_NONE;
}
mfxStatus CDecodingPipeline::Init(void)
{
	mfxStatus sts = MFX_ERR_NONE;

	sts = InitMfxParams();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// create device and allocator
	sts = CreateAllocator();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// in case of HW accelerated decode frames must be allocated prior to decoder initialization
	sts = AllocFrames();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	sts = m_pmfxDEC->Init(&m_mfxVideoParams);
	if (MFX_WRN_PARTIAL_ACCELERATION == sts)
	{
		msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
	}
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	sts = m_pmfxVPP->Init(&m_mfxVppVideoParams);
	if (MFX_WRN_PARTIAL_ACCELERATION == sts) {
		msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
	}
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	sts = m_pmfxDEC->GetVideoParam(&m_mfxVideoParams);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return MFX_ERR_NONE;
}

void CDecodingPipeline::Close()
{
	//m_bWaitStop = true;
	//WaitForStop();

	m_pD3DRender11 = NULL;
	m_pD3DRender9	= NULL;


	m_pPlugin.reset();

	WipeMfxBitstream(&m_mfxBS);
	MSDK_SAFE_DELETE(m_pmfxDEC);
	MSDK_SAFE_DELETE(m_pmfxVPP);


	DeleteFrames();

	m_mfxSession.Close();

	if (m_pUID)
		MFXVideoUSER_UnLoad(m_mfxSession, &(m_pUID->mfx));
	DeleteAllocator();

	return;
}

#if D3D_SURFACES_SUPPORT
bool operator < (const IGFX_DISPLAY_MODE &l, const IGFX_DISPLAY_MODE& r)
{
    if (r.ulResWidth >= 0xFFFF || r.ulResHeight >= 0xFFFF || r.ulRefreshRate >= 0xFFFF)
        return false;

         if (l.ulResWidth < r.ulResWidth) return true;
    else if (l.ulResHeight < r.ulResHeight) return true;
    else if (l.ulRefreshRate < r.ulRefreshRate) return true;

    return false;
}
#endif


mfxStatus CDecodingPipeline::InitMfxParams(void)
{
	MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_NULL_PTR);
	mfxStatus sts = MFX_ERR_NONE;

	// try to find a sequence header in the stream
	// if header is not found this function exits with error (e.g. if device was lost and there's no header in the remaining stream)

	// parse bit stream and fill mfx params
	sts = m_pmfxDEC->DecodeHeader(&m_mfxBS, &m_mfxVideoParams);
	if (sts==MFX_ERR_NONE)
	{
		m_bDecodedHeader=true;	
	}

	// check DecodeHeader status
	MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	if (m_pPlugin.get() && m_videoType == MFX_CODEC_H263 && !sts) {
		// force set format to nv12 as the vp8 plugin uses yv12
		m_mfxVideoParams.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
	}

	// specify memory type 
	m_mfxVideoParams.IOPattern = (mfxU16)(MFX_IOPATTERN_OUT_VIDEO_MEMORY);

	//reduce memory usage by allocation less surfaces
	m_mfxVideoParams.AsyncDepth = 2;

	return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::CreateHWDevice()
{

    return MFX_ERR_NONE;
}


mfxStatus CDecodingPipeline::AllocAndInitVppDoNotUse()
{
    m_VppDoNotUse.NumAlg = 1;

   NEWRECORD(  m_VppDoNotUse.AlgList , new mfxU32 [m_VppDoNotUse.NumAlg] )
    if (!m_VppDoNotUse.AlgList) return MFX_ERR_NULL_PTR;

    m_VppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS; //turn off scene analysis (on by default)
   // m_VppDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_DETAIL; // turn off detail enhancement (on by default)
   // m_VppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DENOISE; //  turn off denoising (on by default)
   // m_VppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP; // turn off processing amplified (on by default)

    return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::InitVppParams()
{
    m_mfxVppVideoParams.IOPattern = (mfxU16)(MFX_IOPATTERN_IN_VIDEO_MEMORY);

    m_mfxVppVideoParams.IOPattern |= MFX_IOPATTERN_OUT_VIDEO_MEMORY;

    MSDK_MEMCPY_VAR(m_mfxVppVideoParams.vpp.In, &m_mfxVideoParams.mfx.FrameInfo, sizeof(mfxFrameInfo));	
    MSDK_MEMCPY_VAR(m_mfxVppVideoParams.vpp.Out, &m_mfxVppVideoParams.vpp.In, sizeof(mfxFrameInfo));


    m_mfxVppVideoParams.AsyncDepth = m_mfxVideoParams.AsyncDepth;

    AllocAndInitVppDoNotUse();
    m_VppExtParams[0] = (mfxExtBuffer*)&m_VppDoNotUse;

    m_mfxVppVideoParams.ExtParam = &m_VppExtParams[0];
    m_mfxVppVideoParams.NumExtParam = 1;

	m_VppProcAmp.Header.BufferId = MFX_EXTBUFF_VPP_PROCAMP;
	m_VppProcAmp.Header.BufferSz = sizeof(mfxExtVPPProcAmp);

	m_VppProcAmp.Hue        = 0.0;//[-180;180;0]
	m_VppProcAmp.Saturation = 1.0;//[0;10;1]
	m_VppProcAmp.Contrast   = 1.0;//[0;10;1]
	m_VppProcAmp.Brightness = 0.0;//[-100;100;0]

	m_mfxVppVideoParams.ExtParam[m_mfxVppVideoParams.NumExtParam++] = (mfxExtBuffer*)&(m_VppProcAmp);

  	m_VppDetailConfig.Header.BufferId = MFX_EXTBUFF_VPP_DETAIL;
  	m_VppDetailConfig.Header.BufferSz = sizeof(mfxExtVPPDetail);
  	m_VppDetailConfig.DetailFactor = 0; //[0.0; 100.0]
  	m_mfxVppVideoParams.ExtParam[m_mfxVppVideoParams.NumExtParam++] = (mfxExtBuffer*)&(m_VppDetailConfig);

	m_VppDenoiseConfig.Header.BufferId = MFX_EXTBUFF_VPP_DENOISE;
	m_VppDenoiseConfig.Header.BufferSz = sizeof(mfxExtVPPDenoise);
	m_VppDenoiseConfig.DenoiseFactor   = 0;//[0.0; 100.0]
	m_mfxVppVideoParams.ExtParam[m_mfxVppVideoParams.NumExtParam++] = (mfxExtBuffer*)&(m_VppDenoiseConfig);

    return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::AllocFrames()
{
    MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_NULL_PTR);

    mfxStatus sts = MFX_ERR_NONE;

    mfxFrameAllocRequest Request;
    mfxFrameAllocRequest VppRequest[2];

    mfxU16 nSurfNum = 0; // number of surfaces for decoder
    mfxU16 nVppSurfNum = 0; // number of surfaces for vpp

    MSDK_ZERO_MEMORY(Request);
    MSDK_ZERO_MEMORY(VppRequest[0]);
    MSDK_ZERO_MEMORY(VppRequest[1]);

	int nW = m_mfxVideoParams.mfx.FrameInfo.CropW;
	int nH = m_mfxVideoParams.mfx.FrameInfo.CropH;
    sts = m_pmfxDEC->Query(&m_mfxVideoParams, &m_mfxVideoParams);
    MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	m_mfxVideoParams.mfx.FrameInfo.CropW = nW;
	m_mfxVideoParams.mfx.FrameInfo.CropH = nH;

    // calculate number of surfaces required for decoder
    sts = m_pmfxDEC->QueryIOSurf(&m_mfxVideoParams, &Request);
    if (MFX_WRN_PARTIAL_ACCELERATION == sts)
    {
        msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    }
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // respecify memory type between Decoder and VPP
    m_mfxVideoParams.IOPattern = (mfxU16)(MFX_IOPATTERN_OUT_VIDEO_MEMORY);

    // recalculate number of surfaces required for decoder
    sts = m_pmfxDEC->QueryIOSurf(&m_mfxVideoParams, &Request);
    MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = InitVppParams();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = m_pmfxVPP->Query(&m_mfxVppVideoParams, &m_mfxVppVideoParams);
    MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // VppRequest[0] for input frames request, VppRequest[1] for output frames request
    sts = m_pmfxVPP->QueryIOSurf(&m_mfxVppVideoParams, VppRequest);
    if (MFX_WRN_PARTIAL_ACCELERATION == sts) {
        msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    }
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // The number of surfaces shared by vpp input and decode output
    nSurfNum = Request.NumFrameSuggested + MSDK_MAX(VppRequest[0].NumFrameSuggested, 1) - 1;

    // The number of surfaces for vpp output
    nVppSurfNum = VppRequest[1].NumFrameSuggested;

    // prepare allocation request
    Request.NumFrameMin = nSurfNum;
    Request.NumFrameSuggested = nSurfNum;

    // surfaces are shared between vpp input and decode output
    Request.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_VPPIN;

    Request.Type |= MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;

    // alloc frames for decoder
    sts = m_pGeneralAllocator->Alloc(m_pGeneralAllocator->pthis, &Request, &m_mfxResponse);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // alloc frames for VPP
    VppRequest[1].NumFrameMin = nVppSurfNum;
    VppRequest[1].NumFrameSuggested = nVppSurfNum;
    MSDK_MEMCPY_VAR(VppRequest[1].Info, &(m_mfxVppVideoParams.vpp.Out), sizeof(mfxFrameInfo));

    sts = m_pGeneralAllocator->Alloc(m_pGeneralAllocator->pthis, &(VppRequest[1]), &m_mfxVppResponse);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // prepare mfxFrameSurface1 array for decoder
    nVppSurfNum = m_mfxVppResponse.NumFrameActual;

    // AllocVppBuffers should call before AllocBuffers to set the value of m_OutputSurfacesNumber
    sts = AllocVppBuffers(nVppSurfNum);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // prepare mfxFrameSurface1 array for decoder
    nSurfNum = m_mfxResponse.NumFrameActual;

    sts = AllocBuffers(nSurfNum);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    for (int i = 0; i < nSurfNum; i++)
    {
        // initating each frame:
        MSDK_MEMCPY_VAR(m_pSurfaces[i].frame.Info, &(Request.Info), sizeof(mfxFrameInfo));
       // if (m_bExternalAlloc) {
            m_pSurfaces[i].frame.Data.MemId = m_mfxResponse.mids[i];
       /* }
        else {
            sts = m_pGeneralAllocator->Lock(m_pGeneralAllocator->pthis, m_mfxResponse.mids[i], &(m_pSurfaces[i].frame.Data));
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }*/
    }

    // prepare mfxFrameSurface1 array for VPP
    for (int i = 0; i < nVppSurfNum; i++) {
        MSDK_MEMCPY_VAR(m_pVppSurfaces[i].frame.Info, &(VppRequest[1].Info), sizeof(mfxFrameInfo));
       // if (m_bExternalAlloc) {
            m_pVppSurfaces[i].frame.Data.MemId = m_mfxVppResponse.mids[i];
        /*}
        else {
            sts = m_pGeneralAllocator->Lock(m_pGeneralAllocator->pthis, m_mfxVppResponse.mids[i], &(m_pVppSurfaces[i].frame.Data));
            if (MFX_ERR_NONE != sts) {
                return sts;
            }
        }*/
    }
    return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::CreateAllocator()
{
    mfxStatus sts = MFX_ERR_NONE;

	if (m_pGeneralAllocator)
	{
		return MFX_ERR_NONE;		
	}

	NEWRECORD( m_pGeneralAllocator , new GeneralAllocator() )
	

#if D3D_SURFACES_SUPPORT
        // provide device manager to MediaSDK
        mfxHDL hdl = NULL;
        mfxHandleType hdl_t =
#if MFX_D3D11_DECODE_SUPPORT
        MFX_HANDLE_D3D11_DEVICE;
#else 
		MFX_HANDLE_D3D9_DEVICE_MANAGER;
#endif // #if MFX_D3D11_DECODE_SUPPORT
		if (m_pD3DRender11)
		{				
			CHWDevice* hwdev = (CHWDevice*)m_pD3DRender11->GetDeviceHandle();
			if (hwdev)
			{
				sts = hwdev->GetHandle(hdl_t, &hdl);
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
				sts = m_mfxSession.SetHandle(hdl_t, hdl);
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			}				
		}		

        // create D3D allocator
		if (m_pmfxAllocatorParams ==  NULL)
		{
#if MFX_D3D11_DECODE_SUPPORT
            NEWRECORD3( D3D11AllocatorParams *,pd3dAllocParams , new D3D11AllocatorParams )
            MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
            pd3dAllocParams->pDevice = reinterpret_cast<ID3D11Device *>(hdl);
            m_pmfxAllocatorParams = pd3dAllocParams;
#else 
            NEWRECORD3( D3DAllocatorParams *,pd3dAllocParams , new D3DAllocatorParams )
            MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
            pd3dAllocParams->pManager = reinterpret_cast<IDirect3DDeviceManager9 *>(hdl);
            m_pmfxAllocatorParams = pd3dAllocParams;
#endif // #if MFX_D3D11_DECODE_SUPPORT
		}
        /* In case of video memory we must provide MediaSDK with external allocator
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to mediasdk */
        sts = m_mfxSession.SetFrameAllocator(m_pGeneralAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

#elif LIBVA_SUPPORT
        sts = CreateHWDevice();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        /* It's possible to skip failed result here and switch to SW implementation,
           but we don't process this way */

        // provide device manager to MediaSDK
        VADisplay va_dpy = NULL;
        sts = m_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, (mfxHDL *)&va_dpy);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        sts = m_mfxSession.SetHandle(MFX_HANDLE_VA_DISPLAY, va_dpy);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        NEWRECORD3( vaapiAllocatorParams *,p_vaapiAllocParams , new vaapiAllocatorParams )
        MSDK_CHECK_POINTER(p_vaapiAllocParams, MFX_ERR_MEMORY_ALLOC);

        p_vaapiAllocParams->m_dpy = va_dpy;
        m_pmfxAllocatorParams = p_vaapiAllocParams;

        /* In case of video memory we must provide MediaSDK with external allocator
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to mediasdk */
        sts = m_mfxSession.SetFrameAllocator(m_pGeneralAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        m_bExternalAlloc = true;
#endif
#ifdef LIBVA_SUPPORT
        //in case of system memory allocator we also have to pass MFX_HANDLE_VA_DISPLAY to HW library
        mfxIMPL impl;
        m_mfxSession.QueryIMPL(&impl);

        if(MFX_IMPL_HARDWARE == MFX_IMPL_BASETYPE(impl))
        {
            sts = CreateHWDevice();
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

            // provide device manager to MediaSDK
            VADisplay va_dpy = NULL;
            sts = m_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, (mfxHDL *)&va_dpy);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
            sts = m_mfxSession.SetHandle(MFX_HANDLE_VA_DISPLAY, va_dpy);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
#endif

    if (!m_pmfxAllocatorParams)
    {
        NEWRECORD3( mfxAllocatorParams* ,allocatorParams , new mfxAllocatorParams  )
        MSDK_CHECK_POINTER(allocatorParams, MFX_ERR_MEMORY_ALLOC);
        m_pmfxAllocatorParams = allocatorParams;
    }
    // initialize general allocator
    sts = m_pGeneralAllocator->Init(m_pmfxAllocatorParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE;
}

void CDecodingPipeline::DeleteFrames()
{
    FreeBuffers();

    // delete frames
    if (m_pGeneralAllocator)
    {
        m_pGeneralAllocator->FreeFrames(&m_mfxResponse);
        m_pGeneralAllocator->FreeFrames(&m_mfxVppResponse);
    }

    return;
}

void CDecodingPipeline::DeleteAllocator()
{
    // delete allocator
    MSDK_SAFE_DELETE(m_pGeneralAllocator);
    MSDK_SAFE_DELETE(m_pmfxAllocatorParams);
}

void CDecodingPipeline::ThreadProcMain(void)
{
	//while(!m_bWaitStop)
	//{


	//}
}

mfxStatus CDecodingPipeline::SyncOutputSurface(mfxU32 wait)
{
    if (!m_pCurrentOutputSurface) {
        m_pCurrentOutputSurface = m_OutputSurfacesPool.GetSurface();
    }
    if (!m_pCurrentOutputSurface) {
        return MFX_ERR_MORE_DATA;
    }

    mfxStatus sts = m_mfxSession.SyncOperation(m_pCurrentOutputSurface->syncp, wait);

    if (MFX_WRN_IN_EXECUTION == sts) {
        return sts;
    }
	
	if (MFX_ERR_NONE == sts) {
		mfxFrameSurface1* frame = &(m_pCurrentOutputSurface->surface->frame);
 //#if MFX_D3D11_DECODE_SUPPORT
 		if (m_pD3DRender11)
 		{
 			m_pD3DRender11->RenderFrameSurface(frame, m_pGeneralAllocator);
 		}
 //#else
	//	if (m_pD3DRender11)
	//	{			
	//		sts = m_pGeneralAllocator->Lock(m_pGeneralAllocator->pthis, frame->Data.MemId, &(frame->Data));
	//		if (MFX_ERR_NONE == sts) {
	//			m_pD3DRender11->RenderFrameSurface(frame);
	//		}
	//		sts = m_pGeneralAllocator->Unlock(m_pGeneralAllocator->pthis, frame->Data.MemId, &(frame->Data));
	//	}
 //#endif
	}

	ReturnSurfaceToBuffers(m_pCurrentOutputSurface);
	m_pCurrentOutputSurface = NULL;

    if (MFX_ERR_NONE != sts) {
        sts = MFX_ERR_UNKNOWN;
    }

    return sts;
}

mfxStatus CDecodingPipeline::DecodeFrame(void*pPacketData,int nPacketSize)
{
	mfxFrameSurface1*   pOutSurface = NULL;
	mfxBitstream*       pBitstream = &m_mfxBS;
	mfxStatus           sts = MFX_ERR_NONE;
	
	if (pBitstream) 
	{
		mfxU32 nBytesPacket = nPacketSize;
		if (pBitstream->MaxLength - pBitstream->DataLength >= nBytesPacket)
		{
			memmove(pBitstream->Data, pBitstream->Data + pBitstream->DataOffset, pBitstream->DataLength);
			pBitstream->DataOffset = 0;
			memcpy(pBitstream->Data + pBitstream->DataLength , (mfxU8*)pPacketData, nBytesPacket);
			pBitstream->DataLength += nBytesPacket;
		}
	}

	if (m_bVppParamsChanged && m_pmfxVPP)
	{
		m_bVppParamsChanged=false;		
		m_pmfxVPP->Init(&m_mfxVppVideoParams);
	}

	while (pBitstream && pBitstream->DataLength>0)         
	{
		//if (MFX_WRN_DEVICE_BUSY == sts)
		//{
		//	Sleep(1); // just wait and then repeat the same call to DecodeFrameAsync
		//	continue;
		//}

		if (!m_bDecodedHeader)
		{
			sts=Init();
			if (MFX_ERR_NONE != sts)
			{
				m_bDecodedHeader = false;
			}
			if (!m_bDecodedHeader)
			{
				continue;
			}
		}
		if (MFX_ERR_NONE != sts)
		{
			break;
		}
		
		if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) {
			SyncFrameSurfaces();
			SyncVppFrameSurfaces();
			if (!m_pCurrentFreeSurface) {
				m_pCurrentFreeSurface = m_FreeSurfacesPool.GetSurface();
			}
			if (!m_pCurrentFreeVppSurface) {
				m_pCurrentFreeVppSurface = m_FreeVppSurfacesPool.GetSurface();
			}
			if (!m_pCurrentFreeSurface || !m_pCurrentFreeVppSurface || (m_OutputSurfacesPool.GetSurfaceCount() == m_mfxVppVideoParams.AsyncDepth)) {
				// we stuck with no FREEMSG surface available, now we will sync...
				sts = SyncOutputSurface(MSDK_DEC_WAIT_INTERVAL);                
				continue;
			}

			if (!m_pCurrentFreeOutputSurface) {
				m_pCurrentFreeOutputSurface = GetFreeOutputSurface();
			}
			if (!m_pCurrentFreeOutputSurface) {
				sts = MFX_ERR_NOT_FOUND;
				break;
			}
		}
		if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) {
			pOutSurface = NULL;
			do {
				sts = m_pmfxDEC->DecodeFrameAsync(pBitstream, &(m_pCurrentFreeSurface->frame), &pOutSurface, &(m_pCurrentFreeOutputSurface->syncp));

				if (MFX_WRN_DEVICE_BUSY == sts) {
					mfxStatus _sts = SyncOutputSurface(MSDK_DEC_WAIT_INTERVAL);
					// note: everything except MFX_ERR_NONE are errors at this point
					if (MFX_ERR_NONE == _sts) {
						sts = MFX_WRN_DEVICE_BUSY;
					} else {
						sts = _sts;
						if (MFX_ERR_MORE_DATA == sts) {
							// we can't receive MFX_ERR_MORE_DATA and have no output - that's a bug
							sts = MFX_WRN_DEVICE_BUSY;//MFX_ERR_NOT_FOUND;
						}
					}
				}
			} while (MFX_WRN_DEVICE_BUSY == sts);

			if (sts > MFX_ERR_NONE) {
				// ignoring warnings...
				if (m_pCurrentFreeOutputSurface->syncp) {
					MSDK_SELF_CHECK(pOutSurface);
					// output is available
					sts = MFX_ERR_NONE;
				} else {
					// output is not available
					sts = MFX_ERR_MORE_SURFACE;
				}
			} else if ((MFX_ERR_MORE_DATA == sts) && !pBitstream) {
				// that's it - we reached end of stream; now we need to render bufferred data...
				do {
					sts = SyncOutputSurface(MSDK_DEC_WAIT_INTERVAL);
				} while (MFX_ERR_NONE == sts);

				if (MFX_ERR_MORE_DATA == sts) {
					sts = MFX_ERR_NONE;
				}
				break;
			} else if (MFX_ERR_INCOMPATIBLE_VIDEO_PARAM == sts || MFX_ERR_MORE_DATA == sts) {
				// need to go to the buffering loop prior to reset procedure
				pBitstream = NULL;
				sts = MFX_ERR_NONE;
				continue;
			}
		}
		if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) {
			// if current FREEMSG surface is locked we are moving it to the used surfaces array
			/*if (m_pCurrentFreeSurface->frame.Data.Locked)*/ {
				m_UsedSurfacesPool.AddSurface(m_pCurrentFreeSurface);
				m_pCurrentFreeSurface = NULL;
			}
		}

		if (MFX_ERR_NONE == sts) {
			do {
				if ((m_pCurrentFreeVppSurface->frame.Info.CropW == 0) ||
					(m_pCurrentFreeVppSurface->frame.Info.CropH == 0)) {
						m_pCurrentFreeVppSurface->frame.Info.CropW = pOutSurface->Info.CropW;
						m_pCurrentFreeVppSurface->frame.Info.CropH = pOutSurface->Info.CropH;
						m_pCurrentFreeVppSurface->frame.Info.CropX = pOutSurface->Info.CropX;
						m_pCurrentFreeVppSurface->frame.Info.CropY = pOutSurface->Info.CropY;
				}
				if (pOutSurface->Info.PicStruct != m_pCurrentFreeVppSurface->frame.Info.PicStruct) {
					m_pCurrentFreeVppSurface->frame.Info.PicStruct = pOutSurface->Info.PicStruct;
				}
				if ((pOutSurface->Info.PicStruct == 0) && (m_pCurrentFreeVppSurface->frame.Info.PicStruct == 0)) {
					m_pCurrentFreeVppSurface->frame.Info.PicStruct = pOutSurface->Info.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
				}

				sts = m_pmfxVPP->RunFrameVPPAsync(pOutSurface, &(m_pCurrentFreeVppSurface->frame), NULL, &(m_pCurrentFreeOutputSurface->syncp));

				//if (MFX_WRN_DEVICE_BUSY == sts) {
				//	MSDK_SLEEP(1); // just wait and then repeat the same call to RunFrameVPPAsync
				//}
			} while (MFX_WRN_DEVICE_BUSY == sts);

			// process errors
			if (MFX_ERR_MORE_DATA == sts) { // will never happen actually
				continue;
			}
			else if (MFX_ERR_NONE != sts) {
				break;
			}

			m_UsedVppSurfacesPool.AddSurface(m_pCurrentFreeVppSurface);
			msdk_atomic_inc16(&(m_pCurrentFreeVppSurface->render_lock));

			m_pCurrentFreeOutputSurface->surface = m_pCurrentFreeVppSurface;
			m_OutputSurfacesPool.AddSurface(m_pCurrentFreeOutputSurface);

			m_pCurrentFreeOutputSurface = NULL;
			m_pCurrentFreeVppSurface = NULL;
		}
	} //while processing   

    return sts; // ERR_NONE or ERR_INCOMPATIBLE_VIDEO_PARAM
}

int CDecodingPipeline::GetVideoWidth(void)
{
	if (m_mfxVideoParams.mfx.FrameInfo.CropW != 0)
	{
		return m_mfxVideoParams.mfx.FrameInfo.CropW;
	}
	return m_mfxVideoParams.mfx.FrameInfo.Width;
}

int CDecodingPipeline::GetVideoHeight(void)
{
	if (m_mfxVideoParams.mfx.FrameInfo.CropH != 0)
	{
		return m_mfxVideoParams.mfx.FrameInfo.CropH;
	}	
	return m_mfxVideoParams.mfx.FrameInfo.Height;
}

int CDecodingPipeline::SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	mfxF64   Brightness = nBrightness;//亮度
	mfxF64   Contrast	= nContrast;//对比度
	mfxF64   Hue		= nHue;		//色调
	mfxF64   Saturation = nSaturation;//饱和度
	if (Brightness > 100 || Contrast > 10 || Saturation > 10)
	{
		Brightness -=128;
		Contrast	= Contrast/10.0 - 5.4;	 
		Saturation  = Saturation/10.0 - 5.4; 
	}
	
	if (m_VppProcAmp.Brightness == Brightness && 
		m_VppProcAmp.Contrast == Contrast && 
		m_VppProcAmp.Hue == Hue && 
		m_VppProcAmp.Saturation	== Saturation
		)
	{
		if (nDetail != -1 && nDenoise != -1 && m_VppDetailConfig.DetailFactor	== nDetail && m_VppDenoiseConfig.DenoiseFactor == nDenoise)
		{
			return 0;
		}
		
		return 0;
	}

	m_VppProcAmp.Brightness 		= Brightness;
	m_VppProcAmp.Contrast   		= Contrast;
	m_VppProcAmp.Hue				= Hue;
	m_VppProcAmp.Saturation			= Saturation;

	if (nDetail != -1)
	{
		m_VppDetailConfig.DetailFactor	= nDetail;
	}	
	if (nDenoise != -1)
	{
		m_VppDenoiseConfig.DenoiseFactor= nDenoise;
	}	

	m_bVppParamsChanged = true;

	return 0;
}
