/*********************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2005-2014 Intel Corporation. All Rights Reserved.

**********************************************************************************/
//#include "stdafx.h"

#include "mfx_samples_config.h"

#if defined(_WIN32) || defined(_WIN64)
#include <tchar.h>
#include <windows.h>
#endif

#include <ctime>
#include <algorithm>
#include "IntelDecoderRenderer.h"
#include "sysmem_allocator.h"
#include "../../HPAVC_user_modules/plugin_api/plugin_loader.h"

#if defined(_WIN32) || defined(_WIN64)
#include "d3d_allocator.h"
#include "d3d_device.h"
#endif

#if defined LIBVA_SUPPORT
#include "vaapi_allocator.h"
#include "vaapi_device.h"
#endif

#pragma warning(disable : 4100)

#define __SYNC_WA // avoid sync issue on Media SDK side



#define DEFAULT_HUE				0.0
#define MAX_HUE					180.0
#define MIN_HUE					-180.0

#define DEFAULT_SATURATION		1.0
#define MAX_SATURATION			0.0
#define MIN_SATURATION			-10.0

#define DEFAULT_CONTRAST		1.0
#define MAX_CONTRAST			0.0
#define MIN_CONTRAST			-10.0

#define DEFAULT_BRIGHTNESS		1.0
#define MAX_BRIGHTNESS			100.0
#define MIN_BRIGHTNESS			-100.0

IntelDecoderRenderer::IntelDecoderRenderer(HPAVC_PlayerCallback&rCallback)
:m_rCallback(rCallback)
{
    m_pmfxDEC = NULL;
    m_pmfxVPP = NULL;
    m_pGeneralAllocator = NULL;
    m_pmfxAllocatorParams = NULL;
    m_memType = SYSTEM_MEMORY;

    m_pCurrentFreeSurface = NULL;
    m_pCurrentFreeVppSurface = NULL;
    m_pCurrentFreeOutputSurface = NULL;
    m_pCurrentOutputSurface = NULL;

    m_bExternalAlloc = false;
    m_bSysmemBetween = false;

	m_bStopDeliverLoop = false;
    m_fourcc = MFX_FOURCC_NV12;
    m_eWorkMode = MODE_RENDERING;
    m_pDeliverOutputSemaphore = NULL;
    m_pDeliveredEvent = NULL;
    m_error = MFX_ERR_NONE;


    m_hwdev = NULL;

    MSDK_ZERO_MEMORY(m_mfxVideoParams);
    MSDK_ZERO_MEMORY(m_mfxVppVideoParams);

    MSDK_ZERO_MEMORY(m_mfxResponse);
    MSDK_ZERO_MEMORY(m_mfxVppResponse);

    MSDK_ZERO_MEMORY(m_mfxBS);

    MSDK_ZERO_MEMORY(m_VppExtParams[0]);
    MSDK_ZERO_MEMORY(m_VppExtParams[1]);

    MSDK_ZERO_MEMORY(m_VppDoNotUse);
    m_VppDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
    m_VppDoNotUse.Header.BufferSz = sizeof(m_VppDoNotUse);

	m_nAsyncDepth=2;
	m_bInitialized=false;
	m_hWnd=NULL;
	m_pDeliverThread=NULL;

	m_nBrightness=DEFAULT_BRIGHTNESS;
	m_nHue=DEFAULT_HUE;
	m_nSaturation=DEFAULT_SATURATION;
	m_nContrast=DEFAULT_CONTRAST;

	m_pUID = NULL;
	m_nVideoCodecType=MFX_CODEC_AVC;
}

IntelDecoderRenderer::~IntelDecoderRenderer()
{
}

int IntelDecoderRenderer::Open(HWND hWnd)
{
	m_hWnd=hWnd;

	m_memType = D3D9_MEMORY;
	m_bSysmemBetween = false;

	mfxStatus sts = MFX_ERR_NONE;

    mfxVersion min_version;
    mfxVersion version;     // real API version with which library is initialized

    // we set version to 1.0 and later we will query actual version of the library which will got leaded
    min_version.Major = 1;
    min_version.Minor = 0;

    // Init session
	bool bUseHWLib=false;
    if (bUseHWLib)
    {
        // try searching on all display adapters
        mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;

        // if d3d11 surfaces are used ask the library to run acceleration through D3D11
        // feature may be unsupported due to OS or MSDK API version
        if (D3D11_MEMORY == m_memType)
            impl |= MFX_IMPL_VIA_D3D11;

        sts = m_mfxSession.Init(impl, &min_version);

        // MSDK API version may not support multiple adapters - then try initialize on the default
        if (MFX_ERR_NONE != sts)
            sts = m_mfxSession.Init((impl & !MFX_IMPL_HARDWARE_ANY) | MFX_IMPL_HARDWARE, &min_version);
    }
    else
        sts = m_mfxSession.Init(MFX_IMPL_SOFTWARE, &min_version);

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = MFXQueryVersion(m_mfxSession , &version); // get real API version of the loaded library
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // create decoder
    m_pmfxDEC = new MFXVideoDECODE(m_mfxSession);
    MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_MEMORY_ALLOC);

    //create VPP
    m_pmfxVPP = new MFXVideoVPP(m_mfxSession);
    if (!m_pmfxVPP) return MFX_ERR_MEMORY_ALLOC;

    // set video type in parameters
    m_mfxVideoParams.mfx.CodecId = MFX_CODEC_AVC;

    // prepare bit stream
    sts = InitMfxBitstream(&m_mfxBS, 1024 * 1024);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if (CheckVersion(&version, MSDK_FEATURE_PLUGIN_API))
    {
        /* Here we actually define the following codec initialization scheme:
         *  1. If plugin path specified: we load user-defined plugin (example: HEVC sample decoder plugin)
         *  2. If plugin path not specified:
         *    2.a) we check if codec is distributed as a mediasdk plugin and load it if yes
         *    2.b) if codec is not in the list of mediasdk plugins, we assume, that it is supported inside mediasdk library
         */
        // Load user plug-in, should go after CreateAllocator function (when all callbacks were initialized)

		msdk_char strPluginPath[MSDK_MAX_FILENAME_LEN];
		msdk_strcopy(strPluginPath, _T("./HPAVC_plugin_vp8.dll"));

        if (msdk_strlen(strPluginPath))
        {
            m_pUserModule.reset(new MFXVideoUSER(m_mfxSession));

			if (m_nVideoCodecType == MFX_CODEC_AVC || m_nVideoCodecType == MFX_CODEC_HEVC)
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
            if (bUseHWLib) {
                m_pUID = msdkGetPluginUID(MSDK_VDEC | MSDK_IMPL_HW, m_nVideoCodecType);
            }
            if (m_pUID) {
                sts = LoadPluginByUID(&session, m_pUID);
            }
            if ((MFX_ERR_NONE != sts) || !m_pUID) {
                m_pUID = msdkGetPluginUID(MSDK_VDEC | MSDK_IMPL_SW, m_nVideoCodecType);
                if (m_pUID) {
                    sts = LoadPluginByUID(&session, m_pUID);

                }
            }
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
    }

	return MFX_ERR_NONE;
}

mfxStatus IntelDecoderRenderer::Init(unsigned char*pData,int nLen)
{
    mfxStatus sts = MFX_ERR_NONE;


    // Populate parameters. Involves DecodeHeader call
    sts = InitMfxParams(m_nAsyncDepth,pData,nLen);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    m_eWorkMode = MODE_RENDERING;
    if (m_eWorkMode == MODE_FILE_DUMP)
	{

    } 
	else if (m_eWorkMode == MODE_RENDERING) 
	{
    }
	else if (m_eWorkMode != MODE_PERFORMANCE) 
	{
        msdk_printf(MSDK_STRING("error: unsupported work mode\n"));
        sts = MFX_ERR_UNSUPPORTED;
    }
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

void IntelDecoderRenderer::Close()
{
	if (m_eWorkMode == MODE_RENDERING) {
		m_bStopDeliverLoop = true;
		if (m_pDeliverOutputSemaphore)
		{
			m_pDeliverOutputSemaphore->Post();
		}
		if (m_pDeliverThread)
		{
			m_pDeliverThread->Wait();
		}

		MSDK_SAFE_DELETE(m_pDeliverOutputSemaphore);
		MSDK_SAFE_DELETE(m_pDeliveredEvent);
		MSDK_SAFE_DELETE(m_pDeliverThread);
	}

	m_pPlugin.reset();
	if (m_pUID) MFXVideoUSER_UnLoad(m_mfxSession, &(m_pUID->mfx));

    WipeMfxBitstream(&m_mfxBS);
    MSDK_SAFE_DELETE(m_pmfxDEC);
    MSDK_SAFE_DELETE(m_pmfxVPP);

    DeleteFrames();

    m_mfxSession.Close();

    // allocator if used as external for MediaSDK must be deleted after decoder
    DeleteAllocator();

    return;
}

int IntelDecoderRenderer::DecodeFrame(unsigned char*pFrameData,int nLen)
{
	RunDecoding(pFrameData,nLen);
	return 0;
}

int IntelDecoderRenderer::GetWidth(void)
{
	return m_mfxVideoParams.mfx.FrameInfo.Width;
}

int IntelDecoderRenderer::GetHeight(void)
{
	return m_mfxVideoParams.mfx.FrameInfo.Height;
}

int IntelDecoderRenderer::SetAMP(int nBrightness,int nHue,int nSaturation,int nContrast)
{
	m_nBrightness=nBrightness;
	m_nHue=nHue;
	m_nSaturation=nSaturation;
	m_nContrast=nContrast;

	return 0;
}

mfxStatus IntelDecoderRenderer::ReadNextFrame(mfxBitstream *pBS,unsigned char*pData,int nLen)
{
	MSDK_CHECK_POINTER(pBS, MFX_ERR_NULL_PTR);
	pBS->DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;

	pBS->DataOffset=0;

	memcpy(pBS->Data+pBS->DataOffset, pData , nLen);
	pBS->DataLength += nLen;

	return MFX_ERR_NONE;
}

mfxStatus IntelDecoderRenderer::InitMfxParams(mfxU16 nAsyncDepth,unsigned char*pData,int nLen)
{
    MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_NULL_PTR);
    mfxStatus sts = MFX_ERR_NONE;

    // try to find a sequence header in the stream
    // if header is not found this function exits with error (e.g. if device was lost and there's no header in the remaining stream)
    for(;;)
    {
        // parse bit stream and fill mfx params
        sts = m_pmfxDEC->DecodeHeader(&m_mfxBS, &m_mfxVideoParams);
		if (m_pPlugin.get() && m_nVideoCodecType == MFX_CODEC_AVC && !sts) 
		{
			// force set format to nv12 as the vp8 plugin uses yv12
			m_mfxVideoParams.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
		}

        if (MFX_ERR_MORE_DATA == sts)
        {
            if (m_mfxBS.MaxLength == m_mfxBS.DataLength)
            {
                sts = ExtendMfxBitstream(&m_mfxBS, m_mfxBS.MaxLength * 2);
                MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
            }

			if (nLen<=0)
			{
				return sts;
			}

            // read a portion of data
            sts = ReadNextFrame(&m_mfxBS,pData,nLen);
			nLen=0;

            if (MFX_ERR_MORE_DATA == sts &&
                !(m_mfxBS.DataFlag & MFX_BITSTREAM_EOS))
            {
                m_mfxBS.DataFlag |= MFX_BITSTREAM_EOS;
                sts = MFX_ERR_NONE;
            }
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

            continue;
        }
        else
        {
			m_bInitialized=true;

            // if input is interlaced JPEG stream
            if ( m_mfxBS.PicStruct == MFX_PICSTRUCT_FIELD_TFF || m_mfxBS.PicStruct == MFX_PICSTRUCT_FIELD_BFF)
            {
                m_mfxVideoParams.mfx.FrameInfo.CropH *= 2;
                m_mfxVideoParams.mfx.FrameInfo.Height = MSDK_ALIGN16(m_mfxVideoParams.mfx.FrameInfo.CropH);
                m_mfxVideoParams.mfx.FrameInfo.PicStruct = m_mfxBS.PicStruct;
            }

            m_mfxVideoParams.mfx.Rotation = MFX_ROTATION_0;

            break;
        }
    }

    // check DecodeHeader status
    if (MFX_WRN_PARTIAL_ACCELERATION == sts)
    {
        msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    }
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if (!m_mfxVideoParams.mfx.FrameInfo.FrameRateExtN || !m_mfxVideoParams.mfx.FrameInfo.FrameRateExtD) {
        msdk_printf(MSDK_STRING("pretending that stream is 30fps one\n"));
        m_mfxVideoParams.mfx.FrameInfo.FrameRateExtN = 30;
        m_mfxVideoParams.mfx.FrameInfo.FrameRateExtD = 1;
    }
    if (!m_mfxVideoParams.mfx.FrameInfo.AspectRatioW || !m_mfxVideoParams.mfx.FrameInfo.AspectRatioH) {
        msdk_printf(MSDK_STRING("pretending that aspect ratio is 1:1\n"));
        m_mfxVideoParams.mfx.FrameInfo.AspectRatioW = 1;
        m_mfxVideoParams.mfx.FrameInfo.AspectRatioH = 1;
    }


    // specify memory type between Decoder and VPP
    m_mfxVideoParams.IOPattern = (mfxU16)( m_bSysmemBetween ?
            MFX_IOPATTERN_OUT_SYSTEM_MEMORY:
            MFX_IOPATTERN_OUT_VIDEO_MEMORY);

    m_mfxVideoParams.AsyncDepth = nAsyncDepth;

    return MFX_ERR_NONE;
}

mfxStatus IntelDecoderRenderer::CreateHWDevice()
{
#if D3D_SURFACES_SUPPORT
    mfxStatus sts = MFX_ERR_NONE;

    HWND window = m_hWnd;
    bool render = (m_eWorkMode == MODE_RENDERING);

#if MFX_D3D11_SUPPORT
    if (D3D11_MEMORY == m_memType)
        m_hwdev = new CD3D11Device();
        if (m_hwdev) {
            reinterpret_cast<CD3D11Device *>(m_hwdev)->DefineFormat((m_fourcc == MFX_FOURCC_A2RGB10) ? true: false);
        }
    else
#endif // #if MFX_D3D11_SUPPORT
        m_hwdev = new CD3D9Device(*this);
        if (m_hwdev) {
            reinterpret_cast<CD3D9Device *>(m_hwdev)->DefineFormat((m_fourcc == MFX_FOURCC_A2RGB10) ? true: false);
        }

    if (NULL == m_hwdev)
        return MFX_ERR_MEMORY_ALLOC;

    sts = m_hwdev->Init(window,MSDKAdapter::GetNumber(m_mfxSession));
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

#elif LIBVA_SUPPORT
    mfxStatus sts = MFX_ERR_NONE;
    m_hwdev = CreateVAAPIDevice();

    if (NULL == m_hwdev) {
        return MFX_ERR_MEMORY_ALLOC;
    }

    sts = m_hwdev->Init(NULL, (m_eWorkMode == MODE_RENDERING) ? 1 : 0, MSDKAdapter::GetNumber(m_mfxSession));
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
#endif
    return MFX_ERR_NONE;
}

mfxStatus IntelDecoderRenderer::ResetDevice()
{
    return m_hwdev->Reset();
}

mfxStatus IntelDecoderRenderer::AllocAndInitVppDoNotUse()
{
    m_VppDoNotUse.NumAlg = 3;

    m_VppDoNotUse.AlgList = new mfxU32 [m_VppDoNotUse.NumAlg];
    if (!m_VppDoNotUse.AlgList) return MFX_ERR_NULL_PTR;

    m_VppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE; // turn off denoising (on by default)
    m_VppDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS; // turn off scene analysis (on by default)
    m_VppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL; // turn off detail enhancement (on by default)
    //m_VppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP; // turn off processing amplified (on by default)

    return MFX_ERR_NONE;
}

mfxStatus IntelDecoderRenderer::InitVppParams()
{
    m_mfxVppVideoParams.IOPattern = (mfxU16)( m_bSysmemBetween?
            MFX_IOPATTERN_IN_SYSTEM_MEMORY:
            MFX_IOPATTERN_IN_VIDEO_MEMORY);

    m_mfxVppVideoParams.IOPattern |= (m_memType != SYSTEM_MEMORY)?
            MFX_IOPATTERN_OUT_VIDEO_MEMORY:
            MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    MSDK_MEMCPY_VAR(m_mfxVppVideoParams.vpp.In, &m_mfxVideoParams.mfx.FrameInfo, sizeof(mfxFrameInfo));
    MSDK_MEMCPY_VAR(m_mfxVppVideoParams.vpp.Out, &m_mfxVppVideoParams.vpp.In, sizeof(mfxFrameInfo));

    m_mfxVppVideoParams.vpp.Out.FourCC  = m_fourcc;

    m_mfxVppVideoParams.AsyncDepth = m_mfxVideoParams.AsyncDepth;

    AllocAndInitVppDoNotUse();
    m_VppExtParams[0] = (mfxExtBuffer*)&m_VppDoNotUse;


    m_mfxVppVideoParams.ExtParam = &m_VppExtParams[0];
    m_mfxVppVideoParams.NumExtParam = 1;

	m_VppProcAmp.Header.BufferId = MFX_EXTBUFF_VPP_PROCAMP;
	m_VppProcAmp.Header.BufferSz = sizeof(mfxExtVPPProcAmp);

	m_VppProcAmp.Hue        = 0.0;//[-180.0; 180.0]
	m_VppProcAmp.Saturation = 1.0;//[0.0; 10.0]
	m_VppProcAmp.Contrast   = 1.0;//[0.0; 10.0]
	m_VppProcAmp.Brightness = 0.0;//[-100.0; 100.0]
	m_VppProcAmp.Brightness = 0.0;//[-100.0; 100.0]

	m_mfxVppVideoParams.ExtParam[m_mfxVppVideoParams.NumExtParam++] = (mfxExtBuffer*)&(m_VppProcAmp);

    return MFX_ERR_NONE;
}



mfxStatus IntelDecoderRenderer::AllocFrames()
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

    sts = m_pmfxDEC->Query(&m_mfxVideoParams, &m_mfxVideoParams);
    MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // calculate number of surfaces required for decoder
    sts = m_pmfxDEC->QueryIOSurf(&m_mfxVideoParams, &Request);
    if (MFX_WRN_PARTIAL_ACCELERATION == sts)
    {
        msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
        m_bSysmemBetween = true;
    }
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // respecify memory type between Decoder and VPP
    m_mfxVideoParams.IOPattern = (mfxU16)( m_bSysmemBetween ?
            MFX_IOPATTERN_OUT_SYSTEM_MEMORY:
            MFX_IOPATTERN_OUT_VIDEO_MEMORY);

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

    Request.Type |= (m_bSysmemBetween)?
            MFX_MEMTYPE_SYSTEM_MEMORY:
            MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;

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
        if (m_bExternalAlloc) {
            m_pSurfaces[i].frame.Data.MemId = m_mfxResponse.mids[i];
        }
        else {
            sts = m_pGeneralAllocator->Lock(m_pGeneralAllocator->pthis, m_mfxResponse.mids[i], &(m_pSurfaces[i].frame.Data));
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
    }

    // prepare mfxFrameSurface1 array for VPP
    for (int i = 0; i < nVppSurfNum; i++) {
        MSDK_MEMCPY_VAR(m_pVppSurfaces[i].frame.Info, &(VppRequest[1].Info), sizeof(mfxFrameInfo));
        if (m_bExternalAlloc) {
            m_pVppSurfaces[i].frame.Data.MemId = m_mfxVppResponse.mids[i];
        }
        else {
            sts = m_pGeneralAllocator->Lock(m_pGeneralAllocator->pthis, m_mfxVppResponse.mids[i], &(m_pVppSurfaces[i].frame.Data));
            if (MFX_ERR_NONE != sts) {
                return sts;
            }
        }
    }
    return MFX_ERR_NONE;
}

mfxStatus IntelDecoderRenderer::CreateAllocator()
{
    mfxStatus sts = MFX_ERR_NONE;

    m_pGeneralAllocator = new GeneralAllocator();

#if D3D_SURFACES_SUPPORT
        sts = CreateHWDevice();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // provide device manager to MediaSDK
        mfxHDL hdl = NULL;
        mfxHandleType hdl_t =
#if MFX_D3D11_SUPPORT
            D3D11_MEMORY == m_memType ? MFX_HANDLE_D3D11_DEVICE :
#endif // #if MFX_D3D11_SUPPORT
            MFX_HANDLE_D3D9_DEVICE_MANAGER;

        sts = m_hwdev->GetHandle(hdl_t, &hdl);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        sts = m_mfxSession.SetHandle(hdl_t, hdl);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // create D3D allocator
#if MFX_D3D11_SUPPORT
        if (D3D11_MEMORY == m_memType)
        {
            D3D11AllocatorParams *pd3dAllocParams = new D3D11AllocatorParams;
            MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
            pd3dAllocParams->pDevice = reinterpret_cast<ID3D11Device *>(hdl);

            m_pmfxAllocatorParams = pd3dAllocParams;
        }
        else
#endif // #if MFX_D3D11_SUPPORT
        {
            D3DAllocatorParams *pd3dAllocParams = new D3DAllocatorParams;
            MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
            pd3dAllocParams->pManager = reinterpret_cast<IDirect3DDeviceManager9 *>(hdl);

            m_pmfxAllocatorParams = pd3dAllocParams;
        }

        /* In case of video memory we must provide MediaSDK with external allocator
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to mediasdk */
        sts = m_mfxSession.SetFrameAllocator(m_pGeneralAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        m_bExternalAlloc = true;
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

        vaapiAllocatorParams *p_vaapiAllocParams = new vaapiAllocatorParams;
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
        mfxAllocatorParams* allocatorParams = new mfxAllocatorParams;
        MSDK_CHECK_POINTER(allocatorParams, MFX_ERR_MEMORY_ALLOC);
        m_pmfxAllocatorParams = allocatorParams;
    }
    // initialize general allocator
    sts = m_pGeneralAllocator->Init(m_pmfxAllocatorParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE;
}

void IntelDecoderRenderer::DeleteFrames()
{
    FreeBuffers();

	if (m_pCurrentFreeOutputSurface)
	{
		//FreeList(m_pCurrentFreeOutputSurface);
		m_pCurrentFreeOutputSurface=NULL;
	}

    // delete frames
    if (m_pGeneralAllocator)
    {
        m_pGeneralAllocator->FreeFrames(&m_mfxResponse);
        m_pGeneralAllocator->FreeFrames(&m_mfxVppResponse);
    }

	if (m_VppDoNotUse.AlgList)
	{
		delete [] m_VppDoNotUse.AlgList;
		m_VppDoNotUse.AlgList=NULL;
	}

    return;
}

void IntelDecoderRenderer::DeleteAllocator()
{
    // delete allocator
    MSDK_SAFE_DELETE(m_pGeneralAllocator);
    MSDK_SAFE_DELETE(m_pmfxAllocatorParams);
    MSDK_SAFE_DELETE(m_hwdev);
}


mfxStatus IntelDecoderRenderer::ResetDecoder(void)
{
    mfxStatus sts = MFX_ERR_NONE;

    // close decoder
    sts = m_pmfxDEC->Close();
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_NOT_INITIALIZED);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // free allocated frames
    DeleteFrames();
    m_pCurrentFreeSurface = NULL;
    m_pCurrentFreeOutputSurface = NULL;

    // initialize parameters with values from parsed header
    sts = InitMfxParams(m_nAsyncDepth,NULL,0);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // in case of HW accelerated decode frames must be allocated prior to decoder initialization
    sts = AllocFrames();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // init decoder
    sts = m_pmfxDEC->Init(&m_mfxVideoParams);
    if (MFX_WRN_PARTIAL_ACCELERATION == sts)
    {
        msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    }
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE;
}

mfxStatus IntelDecoderRenderer::DeliverOutput(mfxFrameSurface1* frame)
{
    CAutoTimer timer_fwrite(m_tick_fwrite);

    mfxStatus res = MFX_ERR_NONE, sts = MFX_ERR_NONE;

    if (!frame) {
        return MFX_ERR_NULL_PTR;
    }

    if (m_bExternalAlloc) {
        if (m_eWorkMode == MODE_FILE_DUMP) {
            res = m_pGeneralAllocator->Lock(m_pGeneralAllocator->pthis, frame->Data.MemId, &(frame->Data));
            if (MFX_ERR_NONE == res) 
			{
                sts = m_pGeneralAllocator->Unlock(m_pGeneralAllocator->pthis, frame->Data.MemId, &(frame->Data));
            }
            if ((MFX_ERR_NONE == res) && (MFX_ERR_NONE != sts)) {
                res = sts;
            }
        } 
		else if (m_eWorkMode == MODE_RENDERING) 
		{
#if D3D_SURFACES_SUPPORT
			RECT rect;
			GetClientRect(m_hWnd, &rect);
			if (IsRectEmpty(&rect))
				return MFX_ERR_UNKNOWN;

            res = m_hwdev->RenderFrame(frame, m_pGeneralAllocator);
#elif LIBVA_SUPPORT
            res = m_hwdev->RenderFrame(frame, m_pGeneralAllocator);
#endif
        }
    }
    else 
	{
    }

    return res;
}

mfxStatus IntelDecoderRenderer::DeliverLoop(void)
{
    mfxStatus res = MFX_ERR_NONE;

    while (!m_bStopDeliverLoop) {
        m_pDeliverOutputSemaphore->Wait();
        if (m_bStopDeliverLoop) {
            continue;
        }
        if (MFX_ERR_NONE != m_error) {
            continue;
        }
        msdkOutputSurface* pCurrentDeliveredSurface = m_DeliveredSurfacesPool.GetSurface();
        if (!pCurrentDeliveredSurface) {
            m_error = MFX_ERR_NULL_PTR;
            continue;
        }
        mfxFrameSurface1* frame = &(pCurrentDeliveredSurface->surface->frame);

        m_error = DeliverOutput(frame);
        ReturnSurfaceToBuffers(pCurrentDeliveredSurface);

        pCurrentDeliveredSurface = NULL;
        ++m_output_count;
        m_pDeliveredEvent->Signal();
    }
    return res;
}

unsigned int MSDK_THREAD_CALLCONVENTION IntelDecoderRenderer::DeliverThreadFunc(void* ctx)
{
    IntelDecoderRenderer* pipeline = (IntelDecoderRenderer*)ctx;

    mfxStatus sts;
    sts = pipeline->DeliverLoop();

    return 0;
}


mfxStatus IntelDecoderRenderer::SyncOutputSurface(mfxU32 wait)
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
        // we got completely decoded frame - pushing it to the delivering thread...
        ++m_synced_count;

        if (m_eWorkMode == MODE_PERFORMANCE) {
            m_output_count = m_synced_count;
            ReturnSurfaceToBuffers(m_pCurrentOutputSurface);
        } else if (m_eWorkMode == MODE_FILE_DUMP) {
            m_output_count = m_synced_count;
            sts = DeliverOutput(&(m_pCurrentOutputSurface->surface->frame));
            if (MFX_ERR_NONE != sts) {
                sts = MFX_ERR_UNKNOWN;
            }
            ReturnSurfaceToBuffers(m_pCurrentOutputSurface);
        } else if (m_eWorkMode == MODE_RENDERING) {
            m_DeliveredSurfacesPool.AddSurface(m_pCurrentOutputSurface);
            m_pDeliveredEvent->Reset();
            m_pDeliverOutputSemaphore->Post();
        }
        m_pCurrentOutputSurface = NULL;
    }

    if (MFX_ERR_NONE != sts) {
        sts = MFX_ERR_UNKNOWN;
    }

    return sts;
}

mfxStatus IntelDecoderRenderer::RunDecoding(unsigned char*pData,int nLen)
{
    mfxFrameSurface1*   pOutSurface = NULL;
    mfxBitstream*       pBitstream = &m_mfxBS;
    mfxStatus           sts = MFX_ERR_NONE;
    bool                bErrIncompatibleVideoParams = false;

	time_t start_time;
    start_time = time(0);

	if (!m_bInitialized)
	{
		sts=Init(pData,nLen);
		if (!m_bInitialized)
		{
			return sts;
		}
	}
	else
	{
		sts = ReadNextFrame(&m_mfxBS,pData,nLen); // read more data to input bit stream
	}

	//m_VppProcAmp.Hue        = 0.0;//[-180.0; 180.0]
	//m_VppProcAmp.Saturation = 1.0;//[0.0; 10.0]
	//m_VppProcAmp.Contrast   = 1.0;//[0.0; 10.0]
	//m_VppProcAmp.Brightness = 0.0;//[-100.0; 100.0]

	//m_VppProcAmp.Brightness++;
	//m_VppExtParams[0] = (mfxExtBuffer*)&m_VppDoNotUse;


	//m_mfxVppVideoParams.ExtParam = &m_VppExtParams[0];
	//m_mfxVppVideoParams.NumExtParam = 1;

	//m_VppProcAmp.Header.BufferId = MFX_EXTBUFF_VPP_PROCAMP;
	//m_VppProcAmp.Header.BufferSz = sizeof(mfxExtVPPProcAmp);

	//m_VppProcAmp.Hue        = 0.0;//[-180.0; 180.0]
	//m_VppProcAmp.Saturation = 1.0;//[0.0; 10.0]
	//m_VppProcAmp.Contrast   = 1.0;//[0.0; 10.0]
	//m_VppProcAmp.Brightness = 0.0;//[-100.0; 100.0]

	static unsigned long nChangedCount=0;
	//nChangedCount++;

	if (nChangedCount>100)
	{
		nChangedCount=0;

		if (m_VppProcAmp.Brightness==MAX_BRIGHTNESS)
		{
			m_VppProcAmp.Brightness=MIN_BRIGHTNESS;
		}
		else
		{
			m_VppProcAmp.Brightness=MAX_BRIGHTNESS;
		}
		sts=m_pmfxVPP->Init(&m_mfxVppVideoParams);
	}

	//m_mfxVppVideoParams.ExtParam[m_mfxVppVideoParams.NumExtParam++] = (mfxExtBuffer*)&(m_VppProcAmp);


	//mfxFrameAllocRequest VppRequest[2];

	//MSDK_ZERO_MEMORY(VppRequest[0]);
	//MSDK_ZERO_MEMORY(VppRequest[1]);

	//sts = m_pmfxVPP->Query(&m_mfxVppVideoParams, &m_mfxVppVideoParams);
	//sts = m_pmfxVPP->QueryIOSurf(&m_mfxVppVideoParams, VppRequest);




	if (m_eWorkMode == MODE_RENDERING && m_pDeliverThread==NULL)
	{
		m_pDeliverOutputSemaphore = new MSDKSemaphore(sts);
		m_pDeliveredEvent = new MSDKEvent(sts, false, false);
		m_pDeliverThread = new MSDKThread(sts, DeliverThreadFunc, this);
		if (!m_pDeliverThread || !m_pDeliverOutputSemaphore || !m_pDeliveredEvent) {
			MSDK_SAFE_DELETE(m_pDeliverThread);
			MSDK_SAFE_DELETE(m_pDeliverOutputSemaphore);
			MSDK_SAFE_DELETE(m_pDeliveredEvent);
			return MFX_ERR_MEMORY_ALLOC;
		}
	}

    //while ((sts == MFX_ERR_NONE) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) {
	while ((sts == MFX_ERR_NONE) || (MFX_ERR_MORE_SURFACE == sts)) {
        if (MFX_ERR_NONE != m_error) {
            msdk_printf(MSDK_STRING("DeliverOutput return error = %d\n"),m_error);
            break;
        }
        if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) {
            // here we check whether output is ready, though we do not wait...
#ifndef __SYNC_WA
            mfxStatus _sts = SyncOutputSurface(0);
            if (MFX_ERR_UNKNOWN == _sts) {
                sts = _sts;
                break;
            } else if (MFX_ERR_NONE == _sts) {
                continue;
            }
#endif
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
#ifndef __SYNC_WA
            if (!m_pCurrentFreeSurface || !m_pCurrentFreeVppSurface) {
#else
            if (!m_pCurrentFreeSurface || !m_pCurrentFreeVppSurface || (m_OutputSurfacesPool.GetSurfaceCount() == m_mfxVppVideoParams.AsyncDepth)) {
#endif
                // we stuck with no free surface available, now we will sync...
                sts = SyncOutputSurface(MSDK_DEC_WAIT_INTERVAL);
                if (MFX_ERR_MORE_DATA == sts) {
                    if ((m_eWorkMode == MODE_PERFORMANCE) || (m_eWorkMode == MODE_FILE_DUMP)) {
                        sts = MFX_ERR_NOT_FOUND;
                    } else if (m_eWorkMode == MODE_RENDERING) {
                        if (m_synced_count != m_output_count) {
                            sts = m_pDeliveredEvent->TimedWait(MSDK_DEC_WAIT_INTERVAL);
                        } else {
                            sts = MFX_ERR_NOT_FOUND;
                        }
                    }
                    if (MFX_ERR_NOT_FOUND == sts) {
                        msdk_printf(MSDK_STRING("fatal: failed to find output surface, that's a bug!\n"));
                        break;
                    }
                }
                // note: MFX_WRN_IN_EXECUTION will also be treated as an error at this point
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

        if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) 
		{
            pOutSurface = NULL;
            do {
                sts = m_pmfxDEC->DecodeFrameAsync(pBitstream, &(m_pCurrentFreeSurface->frame), &pOutSurface, &(m_pCurrentFreeOutputSurface->syncp));

                if (MFX_WRN_DEVICE_BUSY == sts) 
				{
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

                while (m_synced_count != m_output_count) {
                    m_pDeliveredEvent->Wait();
                }

                if (MFX_ERR_MORE_DATA == sts) {
                    sts = MFX_ERR_NONE;
                }
                break;
            } else if (MFX_ERR_INCOMPATIBLE_VIDEO_PARAM == sts) {
                bErrIncompatibleVideoParams = true;
                // need to go to the buffering loop prior to reset procedure
                pBitstream = NULL;
                sts = MFX_ERR_NONE;
                continue;
            }
        }

        if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) {
            // if current free surface is locked we are moving it to the used surfaces array
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

                if (MFX_WRN_DEVICE_BUSY == sts) {
                    MSDK_SLEEP(1); // just wait and then repeat the same call to RunFrameVPPAsync
                }
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

void IntelDecoderRenderer::OnCD3D9DeviceCallbackDraw(HDC hDC)
{
	m_rCallback.OnHPAVC_CallbackDrawDC(hDC);
}