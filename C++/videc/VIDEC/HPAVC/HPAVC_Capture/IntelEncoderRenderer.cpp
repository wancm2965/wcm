
#include "mfx_samples_config.h"

#include "IntelEncoderRenderer.h"
#include "sysmem_allocator.h"

#if D3D_SURFACES_SUPPORT
#include "d3d_allocator.h"
#include "d3d_device.h"
#endif

#if defined(_WIN32) || defined(_WIN64)
#include "d3d_allocator.h"
#include "d3d11_allocator.h"
#include "d3d_device.h"
#include "d3d11_device.h"
#endif

#ifdef LIBVA_SUPPORT
#include "vaapi_allocator.h"
#include "vaapi_device.h"
#endif

#include "../../VIDEC_FRE/VIDECFREHeader.h"


CEncTaskPool::CEncTaskPool(IntelH264EncoderCallback&rCallback)
:m_rCallback(rCallback)
{
	m_pTasks  = NULL;
	m_pmfxSession       = NULL;
	m_nTaskBufferStart  = 0;
	m_nPoolSize         = 0;
}

CEncTaskPool::~CEncTaskPool()
{
	Close();
}

mfxStatus CEncTaskPool::Init(MFXVideoSession* pmfxSession,  mfxU32 nPoolSize, mfxU32 nBufferSize)
{
	MSDK_CHECK_POINTER(pmfxSession, MFX_ERR_NULL_PTR);

	MSDK_CHECK_ERROR(nPoolSize, 0, MFX_ERR_UNDEFINED_BEHAVIOR);
	MSDK_CHECK_ERROR(nBufferSize, 0, MFX_ERR_UNDEFINED_BEHAVIOR);


	m_pmfxSession = pmfxSession;
	m_nPoolSize = nPoolSize;

	NEWRECORD( m_pTasks , new sTask[m_nPoolSize] )
	MSDK_CHECK_POINTER(m_pTasks, MFX_ERR_MEMORY_ALLOC);

	mfxStatus sts = MFX_ERR_NONE;

	for (mfxU32 i = 0; i < m_nPoolSize; i++)
	{
		sts = m_pTasks[i].Init(nBufferSize, &m_rCallback);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}

	return MFX_ERR_NONE;
}

mfxStatus CEncTaskPool::SynchronizeFirstTask()
{
	MSDK_CHECK_POINTER(m_pTasks, MFX_ERR_NOT_INITIALIZED);
	MSDK_CHECK_POINTER(m_pmfxSession, MFX_ERR_NOT_INITIALIZED);

	mfxStatus sts  = MFX_ERR_NONE;

	// non-null sync point indicates that task is in execution
	if (NULL != m_pTasks[m_nTaskBufferStart].EncSyncP)
	{
		sts = m_pmfxSession->SyncOperation(m_pTasks[m_nTaskBufferStart].EncSyncP, MSDK_WAIT_INTERVAL);

		if (MFX_ERR_NONE == sts)
		{
			sts = m_pTasks[m_nTaskBufferStart].WriteBitstream();
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

			sts = m_pTasks[m_nTaskBufferStart].Reset();
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

			// move task buffer start to the next executing task
			// the first transform frame to the right with non zero sync point
			for (mfxU32 i = 0; i < m_nPoolSize; i++)
			{
				m_nTaskBufferStart = (m_nTaskBufferStart + 1) % m_nPoolSize;
				if (NULL != m_pTasks[m_nTaskBufferStart].EncSyncP)
				{
					break;
				}
			}
		}


		return sts;
	}
	else
	{
		return MFX_ERR_NOT_FOUND; // no tasks left in task buffer
	}
}

mfxU32 CEncTaskPool::GetFreeTaskIndex()
{
	mfxU32 off = 0;

	if (m_pTasks)
	{
		for (off = 0; off < m_nPoolSize; off++)
		{
			if (NULL == m_pTasks[(m_nTaskBufferStart + off) % m_nPoolSize].EncSyncP)
			{
				break;
			}
		}
	}

	if (off >= m_nPoolSize)
		return m_nPoolSize;

	return (m_nTaskBufferStart + off) % m_nPoolSize;
}

mfxStatus CEncTaskPool::GetFreeTask(sTask **ppTask)
{
	MSDK_CHECK_POINTER(ppTask, MFX_ERR_NULL_PTR);
	MSDK_CHECK_POINTER(m_pTasks, MFX_ERR_NOT_INITIALIZED);

	mfxU32 index = GetFreeTaskIndex();

	if (index >= m_nPoolSize)
	{
		return MFX_ERR_NOT_FOUND;
	}

	// return the address of the task
	*ppTask = &m_pTasks[index];

	return MFX_ERR_NONE;
}

void CEncTaskPool::Close()
{
	if (m_pTasks)
	{
		for (mfxU32 i = 0; i < m_nPoolSize; i++)
		{
			m_pTasks[i].Close();
		}
	}

	MSDK_SAFE_DELETE_ARRAY(m_pTasks);

	m_pmfxSession = NULL;
	m_nTaskBufferStart = 0;
	m_nPoolSize = 0;
}

sTask::sTask(void)
: EncSyncP(0)
, m_pCallback(NULL)
{
	MSDK_ZERO_MEMORY(mfxBS);
}

mfxStatus sTask::Init(mfxU32 nBufferSize,IntelH264EncoderCallback*pCallback)
{
	Close();

	m_pCallback=pCallback;

	mfxStatus sts = Reset();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	sts = InitMfxBitstream(&mfxBS, nBufferSize);
	MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, sts, WipeMfxBitstream(&mfxBS));

	return sts;
}

mfxStatus sTask::Close()
{
	WipeMfxBitstream(&mfxBS);
	EncSyncP = 0;

	return MFX_ERR_NONE;
}

mfxStatus sTask::WriteBitstream()
{
	if (!m_pCallback)
		return MFX_ERR_NOT_INITIALIZED;

	int nKeyFrame=0;
	mfxU16 nFrameType=mfxBS.FrameType;
	if (nFrameType&MFX_FRAMETYPE_I || 
		nFrameType&MFX_FRAMETYPE_IDR)
	{
		nKeyFrame=1;
	}
	m_pCallback->OnIntelH264EncoderCallback(mfxBS.Data + mfxBS.DataOffset, mfxBS.DataLength,nKeyFrame);


	// mark that we don't need bit stream data any more
	mfxBS.DataLength = 0;

	return MFX_ERR_NONE;
}

mfxStatus sTask::Reset()
{
	// mark sync point as FREEMSG
	EncSyncP = NULL;

	// prepare bit stream
	mfxBS.DataOffset = 0;
	mfxBS.DataLength = 0;

	return MFX_ERR_NONE;
}

mfxStatus IntelEncoderRenderer::AllocAndInitVppDoNotUse()
{
    m_VppDoNotUse.NumAlg = 1;

    NEWRECORD( m_VppDoNotUse.AlgList , new mfxU32 [m_VppDoNotUse.NumAlg] )
    MSDK_CHECK_POINTER(m_VppDoNotUse.AlgList,  MFX_ERR_MEMORY_ALLOC);

 //   m_VppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE; // turn off denoising (on by default)
    m_VppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS; // turn off scene analysis (on by default)
   // m_VppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL; // turn off detail enhancement (on by default)
    //m_VppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP; // turn off processing amplified (on by default)

    return MFX_ERR_NONE;

} // IntelEncoderRenderer::AllocAndInitVppDoNotUse()


void IntelEncoderRenderer::FreeVppDoNotUse()
{
    MSDK_SAFE_DELETE_ARRAY(m_VppDoNotUse.AlgList);
}

mfxStatus IntelEncoderRenderer::InitMfxEncParams(sInputParams *pInParams)
{
	m_mfxEncParams.mfx.CodecId                 = pInParams->CodecId;
	m_mfxEncParams.mfx.TargetUsage             = pInParams->nTargetUsage; // trade-off between quality and speed
	m_mfxEncParams.mfx.TargetKbps              = pInParams->nBitRate*4/5; // in Kbps
	m_mfxEncParams.mfx.MaxKbps				   = pInParams->nBitRate; // in Kbps
	m_mfxEncParams.mfx.RateControlMethod       = (mfxU16)MFX_RATECONTROL_VBR;
	ConvertFrameRate(pInParams->dFrameRate, &m_mfxEncParams.mfx.FrameInfo.FrameRateExtN, &m_mfxEncParams.mfx.FrameInfo.FrameRateExtD);
	m_mfxEncParams.mfx.EncodedOrder            = 0; // binary flag, 0 signals encoder to take frames in display order


    // specify memory type
//     if (D3D9_MEMORY == pInParams->memType || D3D11_MEMORY == pInParams->memType)
//     {
        m_mfxEncParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
//     }
//     else
//     {
//         m_mfxEncParams.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY;
//     }

	// frame info parameters
	m_mfxEncParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_NV12;
	m_mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	m_mfxEncParams.mfx.FrameInfo.PicStruct    = pInParams->nPicStruct;

	// set frame size and crops
	// width must be a multiple of 16
	// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
	m_mfxEncParams.mfx.FrameInfo.Width  = MSDK_ALIGN16(pInParams->nDstWidth);
	m_mfxEncParams.mfx.FrameInfo.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxEncParams.mfx.FrameInfo.PicStruct)?
		MSDK_ALIGN16(pInParams->nDstHeight) : MSDK_ALIGN32(pInParams->nDstHeight);

	m_mfxEncParams.mfx.FrameInfo.CropX = 0;
	m_mfxEncParams.mfx.FrameInfo.CropY = 0;
	m_mfxEncParams.mfx.FrameInfo.CropW = pInParams->nDstWidth;
	m_mfxEncParams.mfx.FrameInfo.CropH = pInParams->nDstHeight;


	if (!m_EncExtParams.empty())
	{
		m_mfxEncParams.ExtParam = &m_EncExtParams[0]; // vector is stored linearly in memory
		m_mfxEncParams.NumExtParam = (mfxU16)m_EncExtParams.size();
	}

	m_mfxEncParams.mfx.CodecProfile = pInParams->CodecProfile;
	m_mfxEncParams.mfx.CodecLevel = pInParams->CodecLevel;
	m_mfxEncParams.mfx.GopRefDist=1;
	m_mfxEncParams.mfx.GopPicSize=pInParams->nMaxKeyFrameInterval;

	m_mfxEncParams.mfx.CodecId                 = MFX_CODEC_AVC;//video conferencing features available only for AVC
	m_mfxEncParams.mfx.GopRefDist              = 1;//Distance between I- or P- key frames (1 means no B-frames)
	m_mfxEncParams.AsyncDepth                  = m_nAsyncDepth;//internal buffering should be disabled in encode

	/*Initializing Extcoding options with msxdecframebuffering = 1 to reduce decode latency*/
	m_CodingOption.MaxDecFrameBuffering               = 1;
	//turn on reference picture marking repetition SEI
	m_CodingOption.RefPicMarkRep = (mfxU16)(MFX_CODINGOPTION_ON);

	m_EncExtParams.push_back(reinterpret_cast<mfxExtBuffer*>(&m_CodingOption));

	//num ref frames doesn't affect latency
	//while ref lists management feature (for recovery from broken frames) needs at least several frames to be in the ref list, 5 is good enough
	m_mfxEncParams.mfx.NumRefFrame             = 5;

	if (!m_EncExtParams.empty())
	{
		m_mfxEncParams.NumExtParam = (mfxU16)m_EncExtParams.size();
		m_mfxEncParams.ExtParam    = &m_EncExtParams.front();
	}


    return MFX_ERR_NONE;
}

mfxStatus IntelEncoderRenderer::InitMfxVppParams(sInputParams *pInParams)
{
    MSDK_CHECK_POINTER(pInParams,  MFX_ERR_NULL_PTR);

    // specify memory type
//     if (D3D9_MEMORY == pInParams->memType || D3D11_MEMORY == pInParams->memType)
//     {
        m_mfxVppParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
//     }
//     else
//     {
//         m_mfxVppParams.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
//     }

    // input frame info
    m_mfxVppParams.vpp.In.FourCC    = MFX_FOURCC_NV12;
    m_mfxVppParams.vpp.In.PicStruct = pInParams->nPicStruct;;
    ConvertFrameRate(pInParams->dFrameRate, &m_mfxVppParams.vpp.In.FrameRateExtN, &m_mfxVppParams.vpp.In.FrameRateExtD);

    // width must be a multiple of 16
    // height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
    m_mfxVppParams.vpp.In.Width     = MSDK_ALIGN16(pInParams->nWidth);
    m_mfxVppParams.vpp.In.Height    = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppParams.vpp.In.PicStruct)?
        MSDK_ALIGN16(pInParams->nHeight) : MSDK_ALIGN32(pInParams->nHeight);

    // set crops in input mfxFrameInfo for correct work of file reader
    // VPP itself ignores crops at initialization
    m_mfxVppParams.vpp.In.CropW = pInParams->nWidth;
    m_mfxVppParams.vpp.In.CropH = pInParams->nHeight;

    // fill output frame info
    MSDK_MEMCPY_VAR(m_mfxVppParams.vpp.Out,&m_mfxVppParams.vpp.In, sizeof(mfxFrameInfo));

    // only resizing is supported
    m_mfxVppParams.vpp.Out.Width = MSDK_ALIGN16(pInParams->nDstWidth);
    m_mfxVppParams.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppParams.vpp.Out.PicStruct)?
        MSDK_ALIGN16(pInParams->nDstHeight) : MSDK_ALIGN32(pInParams->nDstHeight);

    // configure and attach external parameters
    //AllocAndInitVppDoNotUse();
    //m_VppExtParams.push_back((mfxExtBuffer *)&m_VppDoNotUse);

    //m_mfxVppParams.ExtParam = &m_VppExtParams[0]; // vector is stored linearly in memory
    //m_mfxVppParams.NumExtParam = (mfxU16)m_VppExtParams.size();


	AllocAndInitVppDoNotUse();
	m_VppExtParams[0] = (mfxExtBuffer*)&m_VppDoNotUse;


	m_mfxVppParams.ExtParam = &m_VppExtParams[0];
	m_mfxVppParams.NumExtParam = 1;

	m_VppProcAmp.Header.BufferId = MFX_EXTBUFF_VPP_PROCAMP;
	m_VppProcAmp.Header.BufferSz = sizeof(mfxExtVPPProcAmp);

	m_VppProcAmp.Hue        = 0.0;//[-180.0; 180.0]
	m_VppProcAmp.Saturation = 1.0;//[0.0; 10.0]
	m_VppProcAmp.Contrast   = 1.0;//[0.0; 10.0]
	m_VppProcAmp.Brightness = 0.0;//[-100.0; 100.0]

	m_mfxVppParams.ExtParam[m_mfxVppParams.NumExtParam++] = (mfxExtBuffer*)&(m_VppProcAmp);

 	m_VppDetailConfig.DetailFactor = 0; //[0.0; 100.0]
 	m_VppDetailConfig.Header.BufferId = MFX_EXTBUFF_VPP_DETAIL;
 	m_VppDetailConfig.Header.BufferSz = sizeof(mfxExtVPPDetail);
 	m_mfxVppParams.ExtParam[m_mfxVppParams.NumExtParam++] = (mfxExtBuffer*)&(m_VppDetailConfig);

	m_VppDenoiseConfig.Header.BufferId = MFX_EXTBUFF_VPP_DENOISE;
	m_VppDenoiseConfig.Header.BufferSz = sizeof(mfxExtVPPDenoise);
	m_VppDenoiseConfig.DenoiseFactor   = 0;//[0.0; 100.0]
	m_mfxVppParams.ExtParam[m_mfxVppParams.NumExtParam++] = (mfxExtBuffer*)&(m_VppDenoiseConfig);


    return MFX_ERR_NONE;
}

mfxStatus IntelEncoderRenderer::CreateHWDevice()
{
	return MFX_ERR_NONE;
}

mfxStatus IntelEncoderRenderer::ResetDevice()
{
    //if (m_pD3DRender)
    //{
    //    return (mfxStatus)m_pD3DRender->Reset();
    //}
    return MFX_ERR_NONE;
}

mfxStatus IntelEncoderRenderer::AllocFrames()
{
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_NOT_INITIALIZED);

    mfxStatus sts = MFX_ERR_NONE;
    mfxFrameAllocRequest EncRequest;
    mfxFrameAllocRequest VppRequest[2];

    mfxU16 nEncSurfNum = 0; // number of surfaces for encoder
    mfxU16 nVppSurfNum = 0; // number of surfaces for vpp

    MSDK_ZERO_MEMORY(EncRequest);
    MSDK_ZERO_MEMORY(VppRequest[0]);
    MSDK_ZERO_MEMORY(VppRequest[1]);

    // Calculate the number of surfaces for components.
    // QueryIOSurf functions tell how many surfaces are required to produce at least 1 output.
    // To achieve better performance we provide extra surfaces.
    // 1 extra surface at input allows to get 1 extra output.

    sts = m_pmfxENC->QueryIOSurf(&m_mfxEncParams, &EncRequest);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if (m_pmfxVPP)
    {
        // VppRequest[0] for input frames request, VppRequest[1] for output frames request
        sts = m_pmfxVPP->QueryIOSurf(&m_mfxVppParams, VppRequest);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    // The number of surfaces shared by vpp output and encode input.
    // When surfaces are shared 1 surface at first component output contains output frame that goes to next component input
    nEncSurfNum = EncRequest.NumFrameSuggested + MSDK_MAX(VppRequest[1].NumFrameSuggested, 1) - 1 + (m_nAsyncDepth - 1);
	nEncSurfNum=3;

    // The number of surfaces for vpp input - so that vpp can work at async depth = m_nAsyncDepth
    nVppSurfNum = VppRequest[0].NumFrameSuggested + (m_nAsyncDepth - 1);
	nVppSurfNum=1;

    // prepare allocation requests
    EncRequest.NumFrameMin = nEncSurfNum;
    EncRequest.NumFrameSuggested = nEncSurfNum;
    MSDK_MEMCPY_VAR(EncRequest.Info, &(m_mfxEncParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
    if (m_pmfxVPP)
    {
        EncRequest.Type |= MFX_MEMTYPE_FROM_VPPOUT; // surfaces are shared between vpp output and encode input
    }

    // alloc frames for encoder
    sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &EncRequest, &m_EncResponse);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // alloc frames for vpp if vpp is enabled
    if (m_pmfxVPP)
    {
        VppRequest[0].NumFrameMin = nVppSurfNum;
        VppRequest[0].NumFrameSuggested = nVppSurfNum;
        MSDK_MEMCPY_VAR(VppRequest[0].Info, &(m_mfxVppParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

        sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &(VppRequest[0]), &m_VppResponse);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    // prepare mfxFrameSurface1 array for encoder
    NEWRECORD( m_pEncSurfaces , new mfxFrameSurface1 [m_EncResponse.NumFrameActual]  )
    MSDK_CHECK_POINTER(m_pEncSurfaces, MFX_ERR_MEMORY_ALLOC);

    for (int i = 0; i < m_EncResponse.NumFrameActual; i++)
    {
        memset(&(m_pEncSurfaces[i]), 0, sizeof(mfxFrameSurface1));
        MSDK_MEMCPY_VAR(m_pEncSurfaces[i].Info, &(m_mfxEncParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

        if (m_bExternalAlloc)
        {
            m_pEncSurfaces[i].Data.MemId = m_EncResponse.mids[i];
        }
        else
        {
            // get YUV pointers
            sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, m_EncResponse.mids[i], &(m_pEncSurfaces[i].Data));
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
    }

    // prepare mfxFrameSurface1 array for vpp if vpp is enabled
    if (m_pmfxVPP)
    {
        NEWRECORD( m_pVppSurfaces , new mfxFrameSurface1 [m_VppResponse.NumFrameActual] )
        MSDK_CHECK_POINTER(m_pVppSurfaces, MFX_ERR_MEMORY_ALLOC);

        for (int i = 0; i < m_VppResponse.NumFrameActual; i++)
        {
            MSDK_ZERO_MEMORY(m_pVppSurfaces[i]);
            MSDK_MEMCPY_VAR(m_pVppSurfaces[i].Info, &(m_mfxVppParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

            if (m_bExternalAlloc)
            {
                m_pVppSurfaces[i].Data.MemId = m_VppResponse.mids[i];
            }
            else
            {
                sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, m_VppResponse.mids[i], &(m_pVppSurfaces[i].Data));
                MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
            }
        }
    }

    return MFX_ERR_NONE;
}

mfxStatus IntelEncoderRenderer::CreateAllocator()
{
    mfxStatus sts = MFX_ERR_NONE;

  //  if (D3D9_MEMORY == m_memType || D3D11_MEMORY == m_memType)
    {
#if D3D_SURFACES_SUPPORT
        mfxHDL hdl = NULL;
#if MFX_D3D11_ENCODE_SUPPORT
		mfxHandleType hdl_t = MFX_HANDLE_D3D11_DEVICE;
#else
		mfxHandleType hdl_t = MFX_HANDLE_D3D9_DEVICE_MANAGER;
#endif // #if MFX_D3D11_ENCODE_SUPPORT

		if (m_pD3DRender)
		{				
			CHWDevice* hwdev = (CHWDevice*)m_pD3DRender->GetDeviceHandle();
			if (hwdev)
			{
				sts = hwdev->GetHandle(hdl_t, &hdl);
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			}				
			// handle is needed for HW library only
			mfxIMPL impl = 0;
			m_mfxSession.QueryIMPL(&impl);
			if (impl != MFX_IMPL_SOFTWARE)
			{
				sts = m_mfxSession.SetHandle(hdl_t, hdl);
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			}		
		}
        // create D3D allocator
#if MFX_D3D11_ENCODE_SUPPORT
		NEWRECORD( m_pMFXAllocator , new D3D11FrameAllocator )
		MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

		NEWRECORD3( D3D11AllocatorParams *,pd3dAllocParams , new D3D11AllocatorParams )
		MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
		pd3dAllocParams->pDevice = reinterpret_cast<ID3D11Device *>(hdl);

		m_pmfxAllocatorParams = pd3dAllocParams;
#else
		NEWRECORD( m_pMFXAllocator , new D3DFrameAllocator )
		MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

		NEWRECORD3( D3DAllocatorParams *,pd3dAllocParams , new D3DAllocatorParams )
		MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
		pd3dAllocParams->pManager = reinterpret_cast<IDirect3DDeviceManager9 *>(hdl);

		m_pmfxAllocatorParams = pd3dAllocParams;
#endif // #if MFX_D3D11_ENCODE_SUPPORT
        /* In case of video memory we must provide MediaSDK with external allocator
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to Media SDK */
        sts = m_mfxSession.SetFrameAllocator(m_pMFXAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        m_bExternalAlloc = true;
#endif
#ifdef LIBVA_SUPPORT
        sts = CreateHWDevice();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        /* It's possible to skip failed result here and switch to SW implementation,
        but we don't process this way */

        mfxHDL hdl = NULL;
        sts = m_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, &hdl);
        // provide device manager to MediaSDK
        sts = m_mfxSession.SetHandle(MFX_HANDLE_VA_DISPLAY, hdl);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // create VAAPI allocator
       NEWRECORD(  m_pMFXAllocator , new vaapiFrameAllocator )
        MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

        NEWRECORD3( vaapiAllocatorParams *,p_vaapiAllocParams , new vaapiAllocatorParams )
        MSDK_CHECK_POINTER(p_vaapiAllocParams, MFX_ERR_MEMORY_ALLOC);

        p_vaapiAllocParams->m_dpy = (VADisplay)hdl;
        m_pmfxAllocatorParams = p_vaapiAllocParams;

        /* In case of video memory we must provide MediaSDK with external allocator
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to mediasdk */
        sts = m_mfxSession.SetFrameAllocator(m_pMFXAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        m_bExternalAlloc = true;
#endif
    }
//     else
//     {
// #ifdef LIBVA_SUPPORT
//         //in case of system memory allocator we also have to pass MFX_HANDLE_VA_DISPLAY to HW library
//         mfxIMPL impl;
//         m_mfxSession.QueryIMPL(&impl);
// 
//         if(MFX_IMPL_HARDWARE == MFX_IMPL_BASETYPE(impl))
//         {
//             sts = CreateHWDevice();
//             MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
// 
//             mfxHDL hdl = NULL;
//             sts = m_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, &hdl);
//             // provide device manager to MediaSDK
//             sts = m_mfxSession.SetHandle(MFX_HANDLE_VA_DISPLAY, hdl);
//             MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
//         }
// #endif
// 
//         // create system memory allocator
//         m_pMFXAllocator = new SysMemFrameAllocator;
//         MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);
// 
//         /* In case of system memory we demonstrate "no external allocator" usage model.
//         We don't call SetAllocator, Media SDK uses internal allocator.
//         We use system memory allocator simply as a memory manager for application*/
//     }

    // initialize memory allocator
    sts = m_pMFXAllocator->Init(m_pmfxAllocatorParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE;
}

void IntelEncoderRenderer::DeleteFrames()
{
    // delete surfaces array
    MSDK_SAFE_DELETE_ARRAY(m_pEncSurfaces);
    MSDK_SAFE_DELETE_ARRAY(m_pVppSurfaces);

    // delete frames
    if (m_pMFXAllocator)
    {
        m_pMFXAllocator->Free(m_pMFXAllocator->pthis, &m_EncResponse);
        m_pMFXAllocator->Free(m_pMFXAllocator->pthis, &m_VppResponse);
    }
}

void IntelEncoderRenderer::DeleteHWDevice()
{
}

void IntelEncoderRenderer::DeleteAllocator()
{
    // delete allocator
    MSDK_SAFE_DELETE(m_pMFXAllocator);
    MSDK_SAFE_DELETE(m_pmfxAllocatorParams);

    DeleteHWDevice();
}

IntelEncoderRenderer::IntelEncoderRenderer(IntelH264EncoderCallback&rCallback)
:m_rCallback(rCallback)
,m_TaskPool(rCallback)
{
    m_pUID = NULL;
    m_pmfxENC = NULL;
    m_pmfxVPP = NULL;
    m_pMFXAllocator = NULL;
    m_pmfxAllocatorParams = NULL;
    m_memType = SYSTEM_MEMORY;
    m_bExternalAlloc = false;
    m_pEncSurfaces = NULL;
    m_pVppSurfaces = NULL;
    m_nAsyncDepth = 0;

	m_bVppParamsChanged = false;

	MSDK_ZERO_MEMORY(m_VppExtParams[0]);
	MSDK_ZERO_MEMORY(m_VppExtParams[1]);
	MSDK_ZERO_MEMORY(m_VppExtParams[2]);
	MSDK_ZERO_MEMORY(m_VppExtParams[3]);

    MSDK_ZERO_MEMORY(m_VppDoNotUse);
    m_VppDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
    m_VppDoNotUse.Header.BufferSz = sizeof(m_VppDoNotUse);

    MSDK_ZERO_MEMORY(m_CodingOption);
    m_CodingOption.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
    m_CodingOption.Header.BufferSz = sizeof(m_CodingOption);

	m_pCtrl=NULL;
	MSDK_ZERO_MEMORY(m_ctrl);

    m_pD3DRender = NULL;

    MSDK_ZERO_MEMORY(m_mfxEncParams);
    MSDK_ZERO_MEMORY(m_mfxVppParams);

    MSDK_ZERO_MEMORY(m_EncResponse);
    MSDK_ZERO_MEMORY(m_VppResponse);

	m_pSurfRenderder=NULL;
}

IntelEncoderRenderer::~IntelEncoderRenderer()
{
    Close();
}

mfxStatus IntelEncoderRenderer::Init(sInputParams *pParams,HPAVC_D3DRender*pD3DRender)
{
    MSDK_CHECK_POINTER(pParams, MFX_ERR_NULL_PTR);

	m_pD3DRender=pD3DRender;
    mfxStatus sts = MFX_ERR_NONE;
	sts = CreateHWDevice();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    mfxVersion min_version;
    mfxVersion version;     // real API version with which library is initialized

    // we set version to 1.0 and later we will query actual version of the library which will got leaded
    min_version.Major = 1;
    min_version.Minor = 0;

	//pParams->memType=SYSTEM_MEMORY;
	m_bExternalAlloc=true;

    //// Init session
    if (pParams->bUseHWLib)
    {
        // try searching on all display adapters
        mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;

        // if d3d11 surfaces are used ask the library to run acceleration through D3D11
        // feature may be unsupported due to OS or MSDK API version
#if MFX_D3D11_ENCODE_SUPPORT
            impl |= MFX_IMPL_VIA_D3D11;
#endif
        sts = m_mfxSession.Init(impl, &min_version);

        // MSDK API version may not support multiple adapters - then try initialize on the default
        if (MFX_ERR_NONE != sts)
           sts = m_mfxSession.Init((impl & (!MFX_IMPL_HARDWARE_ANY)) | MFX_IMPL_HARDWARE, &min_version);
    }
    else
        sts = m_mfxSession.Init(MFX_IMPL_SOFTWARE, &min_version);

   if (MFX_ERR_NONE != sts)
   {
	   sts = m_mfxSession.Init(MFX_IMPL_SOFTWARE, &min_version);
   }

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = MFXQueryVersion(m_mfxSession , &version); // get real API version of the loaded library
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if ((pParams->CodecId == MFX_CODEC_JPEG) && !CheckVersion(&version, MSDK_FEATURE_JPEG_ENCODE)) {
        msdk_printf(MSDK_STRING("error: Jpeg is not supported in the %d.%d API version\n"),
            version.Major, version.Minor);
        return MFX_ERR_UNSUPPORTED;
    }

    if (CheckVersion(&version, MSDK_FEATURE_PLUGIN_API)) {
        // we check if codec is distributed as a mediasdk plugin and load it if yes
        // else if codec is not in the list of mediasdk plugins, we assume, that it is supported inside mediasdk library

        // in case of HW library (-hw key) we will firstly try to load HW plugin
        // in case of failure - we will try SW one
        mfxSession session = m_mfxSession;

        if (pParams->bUseHWLib) {
            m_pUID = msdkGetPluginUID(MSDK_VENC | MSDK_IMPL_HW, pParams->CodecId);
        }
        if (m_pUID) {
            sts = LoadPluginByUID(&session, m_pUID);
        }
        if ((MFX_ERR_NONE != sts) || !m_pUID) {
            m_pUID = msdkGetPluginUID(MSDK_VENC | MSDK_IMPL_SW, pParams->CodecId);
            if (m_pUID) {
                sts = LoadPluginByUID(&session, m_pUID);
            }
        }
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    // set memory type
    m_memType = pParams->memType;

    // create and init frame allocator
    sts = CreateAllocator();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = InitMfxEncParams(pParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = InitMfxVppParams(pParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);


    // create encoder
    NEWRECORD( m_pmfxENC , new MFXVideoENCODE(m_mfxSession) )
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_MEMORY_ALLOC);

    // create preprocessor if resizing was requested from command line
    // or if different FourCC is set in InitMfxVppParams
    //if (pParams->nWidth  != pParams->nDstWidth ||
    //    pParams->nHeight != pParams->nDstHeight ||
    //    m_mfxVppParams.vpp.In.FourCC != m_mfxVppParams.vpp.Out.FourCC)
    {
        NEWRECORD( m_pmfxVPP , new MFXVideoVPP(m_mfxSession) )
        MSDK_CHECK_POINTER(m_pmfxVPP, MFX_ERR_MEMORY_ALLOC);
    }

    m_nAsyncDepth = 2; // this number can be tuned for better performance

    sts = ResetMFXComponents(pParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE;
}

void IntelEncoderRenderer::Close()
{
    MSDK_SAFE_DELETE(m_pmfxENC);
    MSDK_SAFE_DELETE(m_pmfxVPP);

    if (m_pUID) MFXVideoUSER_UnLoad(m_mfxSession, &(m_pUID->mfx));

    m_pHEVC_plugin.reset();

    FreeVppDoNotUse();

    DeleteFrames();
    // allocator if used as external for MediaSDK must be deleted after SDK components
    DeleteAllocator();

    m_TaskPool.Close();
    m_mfxSession.Close();

	if (m_pSurfRenderder)
	{
		delete m_pSurfRenderder;
		m_pSurfRenderder=NULL;
	}
}

mfxStatus IntelEncoderRenderer::ResetMFXComponents(sInputParams* pParams)
{
    MSDK_CHECK_POINTER(pParams, MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_NOT_INITIALIZED);

    mfxStatus sts = MFX_ERR_NONE;

    sts = m_pmfxENC->Close();
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_NOT_INITIALIZED);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if (m_pmfxVPP)
    {
        sts = m_pmfxVPP->Close();
        MSDK_IGNORE_MFX_STS(sts, MFX_ERR_NOT_INITIALIZED);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    // FREEMSG allocated frames
    DeleteFrames();

    m_TaskPool.Close();

    sts = AllocFrames();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = m_pmfxENC->Init(&m_mfxEncParams);
    if (MFX_WRN_PARTIAL_ACCELERATION == sts)
    {
        msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    }

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if (m_pmfxVPP)
    {
        sts = m_pmfxVPP->Init(&m_mfxVppParams);
        if (MFX_WRN_PARTIAL_ACCELERATION == sts)
        {
            msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
            MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
        }
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    mfxU32 nEncodedDataBufferSize = m_mfxEncParams.mfx.FrameInfo.Width * m_mfxEncParams.mfx.FrameInfo.Height * 4;
    sts = m_TaskPool.Init(&m_mfxSession, m_nAsyncDepth * 2, nEncodedDataBufferSize);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE;
}

mfxStatus IntelEncoderRenderer::AllocateSufficientBuffer(mfxBitstream* pBS)
{
    MSDK_CHECK_POINTER(pBS, MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_NOT_INITIALIZED);

    mfxVideoParam par;
    MSDK_ZERO_MEMORY(par);

    // find out the required buffer size
    mfxStatus sts = m_pmfxENC->GetVideoParam(&par);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // reallocate bigger buffer for output
    sts = ExtendMfxBitstream(pBS, par.mfx.BufferSizeInKB * 1000);
    MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, sts, WipeMfxBitstream(pBS));

    return MFX_ERR_NONE;
}

mfxStatus IntelEncoderRenderer::GetFreeTask(sTask **ppTask)
{
    mfxStatus sts = MFX_ERR_NONE;

    sts = m_TaskPool.GetFreeTask(ppTask);
    if (MFX_ERR_NOT_FOUND == sts)
    {
        sts = SynchronizeFirstTask();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // try again
        sts = m_TaskPool.GetFreeTask(ppTask);
    }

    return sts;
}

mfxStatus IntelEncoderRenderer::SynchronizeFirstTask()
{
    mfxStatus sts = m_TaskPool.SynchronizeFirstTask();

    return sts;
}


mfxStatus IntelEncoderRenderer::EncodeFrame(unsigned char*pYV12Data[],int nRequestKeyFrame)
{
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_NOT_INITIALIZED);

    mfxStatus sts = MFX_ERR_NONE;

	mfxFrameSurface1*pSurf=NULL;

    sTask *pCurrentTask = NULL; // a pointer to the current task
    mfxU16 nEncSurfIdx = 0;     // index of FREEMSG surface for encoder input (vpp output)
    mfxU16 nVppSurfIdx = 0;     // index of FREEMSG surface for vpp input

    mfxSyncPoint VppSyncPoint = NULL; // a sync point associated with an asynchronous vpp call


	m_pCtrl=NULL;
	m_ctrl.FrameType = MFX_FRAMETYPE_UNKNOWN;

	if (nRequestKeyFrame)
	{
		m_ctrl.FrameType = MFX_FRAMETYPE_I | MFX_FRAMETYPE_IDR | MFX_FRAMETYPE_REF;
		m_pCtrl = &m_ctrl;
	}

    sts = MFX_ERR_NONE;

	if (m_bVppParamsChanged && m_pmfxVPP)
	{
		m_bVppParamsChanged=false;		
		m_pmfxVPP->Init(&m_mfxVppParams);
	}

    // get a pointer to a FREEMSG task (bit stream and sync point for encoder)
    sts = GetFreeTask(&pCurrentTask);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // find FREEMSG surface for encoder input
    nEncSurfIdx = GetFreeSurface(m_pEncSurfaces, m_EncResponse.NumFrameActual);
    MSDK_CHECK_ERROR(nEncSurfIdx, MSDK_INVALID_SURF_IDX, MFX_ERR_MEMORY_ALLOC);

    // point pSurf to encoder surface
    pSurf = &m_pEncSurfaces[nEncSurfIdx];
    {
        // if vpp is enabled find FREEMSG surface for vpp input and point pSurf to vpp surface
        if (m_pmfxVPP)
        {
            nVppSurfIdx = GetFreeSurface(m_pVppSurfaces, m_VppResponse.NumFrameActual);
            MSDK_CHECK_ERROR(nVppSurfIdx, MSDK_INVALID_SURF_IDX, MFX_ERR_MEMORY_ALLOC);

            pSurf = &m_pVppSurfaces[nVppSurfIdx];
        }

        // load frame from file to surface data
        // if we share allocator with Media SDK we need to call Lock to access surface data and...
        if (m_bExternalAlloc)
        {
            // get YUV pointers
            sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, pSurf->Data.MemId, &(pSurf->Data));
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }

        pSurf->Info.FrameId.ViewId = 0;
		sts = LoadNextFrame(pSurf,pYV12Data);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // ... after we're done call Unlock
        if (m_bExternalAlloc)
        {
            sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, pSurf->Data.MemId, &(pSurf->Data));
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
    }

    // perform preprocessing if required
    if (m_pmfxVPP)
    {
        sts = m_pmfxVPP->RunFrameVPPAsync(&m_pVppSurfaces[nVppSurfIdx], &m_pEncSurfaces[nEncSurfIdx],
            NULL, &VppSyncPoint);

		sts = m_mfxSession.SyncOperation(VppSyncPoint, MSDK_VPP_WAIT_INTERVAL);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }


    {
        // at this point surface for encoder contains either a frame from file or a frame processed by vpp
        sts = m_pmfxENC->EncodeFrameAsync(m_pCtrl, &m_pEncSurfaces[nEncSurfIdx], &pCurrentTask->mfxBS, &pCurrentTask->EncSyncP);


        if (MFX_ERR_NONE < sts && !pCurrentTask->EncSyncP) // repeat the call if warning and no output
        {
        }
        else if (MFX_ERR_NONE < sts && pCurrentTask->EncSyncP)
        {
            sts = MFX_ERR_NONE; // ignore warnings if output is available
        }
        else if (MFX_ERR_NOT_ENOUGH_BUFFER == sts)
        {
            sts = AllocateSufficientBuffer(&pCurrentTask->mfxBS);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
        else
        {
            // get next surface and new task for 2nd bitstream in ViewOutput mode
            MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_BITSTREAM);
        }
    }

#if 0
	if (m_pSurfRenderder==NULL)
	{
		NEWRECORD( m_pSurfRenderder , new mfxFrameSurface1() )
	}
	memcpy(m_pSurfRenderder,&m_pEncSurfaces[nEncSurfIdx],sizeof(mfxFrameSurface1));
	if (m_pD3DRender)
	{
		m_pD3DRender->RenderFrameSurface(m_pSurfRenderder, m_pMFXAllocator);
	}

	//{
	//	m_rCallback.OnIntelH264EncoderCallbackSurface(m_pSurfRenderder, m_pMFXAllocator);
	//}
#endif

    // means that the input file has ended, need to go to buffering loops
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
    // exit in case of other errors
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);



    // get a FREEMSG task (bit stream and sync point for encoder)
    sts = GetFreeTask(&pCurrentTask);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    for (;;)
    {
        sts = m_pmfxENC->EncodeFrameAsync(NULL, NULL, &pCurrentTask->mfxBS, &pCurrentTask->EncSyncP);

        if (MFX_ERR_NONE < sts && !pCurrentTask->EncSyncP) // repeat the call if warning and no output
        {
            if (MFX_WRN_DEVICE_BUSY == sts)
                MSDK_SLEEP(1); // wait if device is busy
        }
        else if (MFX_ERR_NONE < sts && pCurrentTask->EncSyncP)
        {
            sts = MFX_ERR_NONE; // ignore warnings if output is available
            break;
        }
        else if (MFX_ERR_NOT_ENOUGH_BUFFER == sts)
        {
            sts = AllocateSufficientBuffer(&pCurrentTask->mfxBS);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
        else
        {
            // get new task for 2nd bitstream in ViewOutput mode
            MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_BITSTREAM);
            break;
        }
    }


    // MFX_ERR_MORE_DATA is the correct status to exit buffering loop with
    // indicates that there are no more buffered frames
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
    // exit in case of other errors
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);


    return sts;
}


mfxStatus IntelEncoderRenderer::LoadNextFrame(mfxFrameSurface1* pSurface,unsigned char*pYV12Data[])
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

	return MFX_ERR_NONE;    
}

int IntelEncoderRenderer::GetVideoWidth(void)
{
	if (m_mfxEncParams.mfx.FrameInfo.CropW != 0)
	{
		return m_mfxEncParams.mfx.FrameInfo.CropW;
	}
	return m_mfxEncParams.mfx.FrameInfo.Height;
}

int IntelEncoderRenderer::GetVideoHeight(void)
{
	if (m_mfxEncParams.mfx.FrameInfo.CropH != 0)
	{
		return m_mfxEncParams.mfx.FrameInfo.CropH;
	}
	return m_mfxEncParams.mfx.FrameInfo.Width;
}

int IntelEncoderRenderer::SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	if (m_VppProcAmp.Brightness == nBrightness && 
		m_VppProcAmp.Contrast == nContrast && 
		m_VppProcAmp.Hue == nHue && 
		m_VppProcAmp.Saturation	== nSaturation &&
		m_VppDetailConfig.DetailFactor	== nDetail &&
		m_VppDenoiseConfig.DenoiseFactor == nDenoise)
	{
		return 0;
	}
	
	m_VppProcAmp.Brightness 		= nBrightness;
	m_VppProcAmp.Contrast   		= nContrast;
	m_VppProcAmp.Hue				= nHue;
	m_VppProcAmp.Saturation			= nSaturation;
	m_VppDetailConfig.DetailFactor	= nDetail;
	m_VppDenoiseConfig.DenoiseFactor= nDenoise;

	m_bVppParamsChanged = true;

	return 0;
}

