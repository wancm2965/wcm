//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//

#include <tchar.h>
#include <windows.h>

#include "pipeline_encode.h"
#include "sysmem_allocator.h"

#ifdef D3D_SURFACES_SUPPORT
#include "d3d_allocator.h"
#endif

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

mfxStatus CEncTaskPool::Init(MFXVideoSession* pmfxSession, mfxU32 nPoolSize, mfxU32 nBufferSize)
{
    MSDK_CHECK_POINTER(pmfxSession, MFX_ERR_NULL_PTR);

    MSDK_CHECK_ERROR(nPoolSize, 0, MFX_ERR_NOT_FOUND);
    MSDK_CHECK_ERROR(nBufferSize, 0, MFX_ERR_NOT_FOUND);

    m_pmfxSession = pmfxSession;
    m_nPoolSize = nPoolSize;

    m_pTasks = new sTask [m_nPoolSize];  
    MSDK_CHECK_POINTER(m_pTasks, MFX_ERR_MEMORY_ALLOC);

    mfxStatus sts = MFX_ERR_NONE;

    mfxU32 i = 0;
    for (i = 0; i < m_nPoolSize; i++)
    {       
        MSDK_ZERO_MEMORY(m_pTasks[i].mfxBS);
        sts = InitMfxBitstream(&m_pTasks[i].mfxBS, nBufferSize);
        MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, MFX_ERR_NOT_INITIALIZED, WipeMfxBitstream(&m_pTasks[i].mfxBS));
                
        MSDK_ZERO_MEMORY(m_pTasks[i].EncSyncP);        
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
			int nKeyFrame=0;
			mfxU16 nFrameType=m_pTasks[m_nTaskBufferStart].mfxBS.FrameType;
			if (nFrameType&MFX_FRAMETYPE_I || 
				nFrameType&MFX_FRAMETYPE_IDR)
			{
				nKeyFrame=1;
			}
			m_rCallback.OnIntelH264EncoderCallback(m_pTasks[m_nTaskBufferStart].mfxBS.Data + m_pTasks[m_nTaskBufferStart].mfxBS.DataOffset, m_pTasks[m_nTaskBufferStart].mfxBS.DataLength,nKeyFrame);

			// mark that we don't need bit stream data any more
			m_pTasks[m_nTaskBufferStart].mfxBS.DataLength = 0;


            // mark sync point as free
            m_pTasks[m_nTaskBufferStart].EncSyncP = NULL;             

            // prepare bit stream
            m_pTasks[m_nTaskBufferStart].mfxBS.DataOffset = 0;
            m_pTasks[m_nTaskBufferStart].mfxBS.DataLength = 0;


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
    mfxU32 i = m_nPoolSize;

    if (m_pTasks)
    {
        for (i = 0; i < m_nPoolSize; i++)
        {            
            if (NULL == m_pTasks[i].EncSyncP)
            {                
                break;
            }
        }
    } 

    return i;
}

mfxStatus CEncTaskPool::GetFreeTask(sTask **ppTask)
{   
    MSDK_CHECK_POINTER(ppTask, MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(m_pTasks, MFX_ERR_NOT_INITIALIZED);

    mfxU32 index = m_nPoolSize; 
    mfxStatus sts;

    // if there is no free transform frame, wait for a task to complete
    if (GetFreeTaskIndex() >= m_nPoolSize)
    {
        sts = SynchronizeFirstTask();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }  

    index = GetFreeTaskIndex();

    if (index >= m_nPoolSize)
    {
        return MFX_ERR_NOT_ENOUGH_BUFFER;
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
            WipeMfxBitstream(&m_pTasks[i].mfxBS);                        
        }
    }

    MSDK_SAFE_DELETE_ARRAY(m_pTasks);

    m_pmfxSession = NULL;
    m_nTaskBufferStart = 0;
    m_nPoolSize = 0;        
}

mfxStatus CEncodingPipeline::AllocAndInitMVCSeqDesc()
{
    // a simple example of mfxExtMVCSeqDesc structure filling
    // actually equal to the "Default dependency mode" - when the structure fields are left 0,
    // but we show how to properly allocate and fill the fields

    mfxU32 i;       

    // mfxMVCViewDependency array
    m_MVCSeqDesc.NumView = m_nNumView; 
    m_MVCSeqDesc.NumViewAlloc = m_nNumView;
    m_MVCSeqDesc.View = new mfxMVCViewDependency[m_MVCSeqDesc.NumViewAlloc];
    MSDK_CHECK_POINTER(m_MVCSeqDesc.View, MFX_ERR_MEMORY_ALLOC);
    for (i = 0; i < m_MVCSeqDesc.NumViewAlloc; ++i)
    {
        MSDK_ZERO_MEMORY(m_MVCSeqDesc.View[i]);
        m_MVCSeqDesc.View[i].ViewId = (mfxU16) i; // set view number as view id
    }        

    // viewId array
    m_MVCSeqDesc.NumViewId = m_nNumView;    
    m_MVCSeqDesc.NumViewIdAlloc = m_nNumView;
    m_MVCSeqDesc.ViewId = new mfxU16[m_MVCSeqDesc.NumViewIdAlloc];
    MSDK_CHECK_POINTER(m_MVCSeqDesc.ViewId, MFX_ERR_MEMORY_ALLOC);
    for (i = 0; i < m_MVCSeqDesc.NumViewIdAlloc; ++i)
    {
        m_MVCSeqDesc.ViewId[i] = (mfxU16) i; 
    }   

    // create a single operation point containing all views
    m_MVCSeqDesc.NumOP = 1;
    m_MVCSeqDesc.NumOPAlloc = 1;  
    m_MVCSeqDesc.OP = new mfxMVCOperationPoint[m_MVCSeqDesc.NumOPAlloc];
    MSDK_CHECK_POINTER(m_MVCSeqDesc.OP, MFX_ERR_MEMORY_ALLOC);
    for (i = 0; i < m_MVCSeqDesc.NumOPAlloc; ++i)
    {
        MSDK_ZERO_MEMORY(m_MVCSeqDesc.OP[i]);
        m_MVCSeqDesc.OP[i].NumViews = (mfxU16) m_nNumView;
        m_MVCSeqDesc.OP[i].NumTargetViews = (mfxU16) m_nNumView;
        m_MVCSeqDesc.OP[i].TargetViewId = m_MVCSeqDesc.ViewId; // points to mfxExtMVCSeqDesc::ViewId
    } 

    return MFX_ERR_NONE;
}

void CEncodingPipeline::FreeMVCSeqDesc()
{

    MSDK_SAFE_DELETE_ARRAY(m_MVCSeqDesc.View);
    MSDK_SAFE_DELETE_ARRAY(m_MVCSeqDesc.ViewId);
    MSDK_SAFE_DELETE_ARRAY(m_MVCSeqDesc.OP);      
}

mfxStatus CEncodingPipeline::InitMfxEncParams(sInputParams *pInParams)
{
    m_mfxEncParams.mfx.CodecId                 = pInParams->CodecId;
    m_mfxEncParams.mfx.TargetUsage             = pInParams->nTargetUsage; // trade-off between quality and speed
    m_mfxEncParams.mfx.TargetKbps              = pInParams->nBitRate; // in Kbps
    m_mfxEncParams.mfx.MaxKbps				   = pInParams->nBitRate; // in Kbps
	m_mfxEncParams.mfx.GopPicSize=pInParams->dFrameRate*4;
	m_mfxEncParams.mfx.GopRefDist=1;

    m_mfxEncParams.mfx.RateControlMethod       = MFX_RATECONTROL_CBR; 
    ConvertFrameRate(pInParams->dFrameRate, &m_mfxEncParams.mfx.FrameInfo.FrameRateExtN, &m_mfxEncParams.mfx.FrameInfo.FrameRateExtD);    
    m_mfxEncParams.mfx.EncodedOrder            = 0; // binary flag, 0 signals encoder to take frames in display order

    // specify memory type
    if (pInParams->bd3dAlloc)
    {
        m_mfxEncParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
    }
    else
    {
        m_mfxEncParams.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY;
    }    

    // frame info parameters
    m_mfxEncParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_NV12;
    m_mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    m_mfxEncParams.mfx.FrameInfo.PicStruct    = pInParams->nPicStruct;

    // set frame size and crops
    // width must be a multiple of 16 
    // height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
    m_mfxEncParams.mfx.FrameInfo.Width  = MSDK_ALIGN16(pInParams->nWidth);
    m_mfxEncParams.mfx.FrameInfo.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxEncParams.mfx.FrameInfo.PicStruct)?
        MSDK_ALIGN16(pInParams->nHeight) : MSDK_ALIGN32(pInParams->nHeight);
    
    m_mfxEncParams.mfx.FrameInfo.CropX = 0; 
    m_mfxEncParams.mfx.FrameInfo.CropY = 0;
    m_mfxEncParams.mfx.FrameInfo.CropW = pInParams->nWidth;
    m_mfxEncParams.mfx.FrameInfo.CropH = pInParams->nHeight;
    
    // we don't specify profile and level and let the encoder choose those basing on parameters
    // we must specify profile only for MVC codec
    if (m_bIsMVC)
    {
        m_mfxEncParams.mfx.CodecProfile = MFX_PROFILE_AVC_STEREO_HIGH;
    }
	else
	{
		m_mfxEncParams.mfx.CodecProfile = MFX_PROFILE_AVC_BASELINE;
		m_mfxEncParams.mfx.CodecProfile = MFX_PROFILE_AVC_HIGH;
	}
	m_mfxEncParams.mfx.CodecProfile = pInParams->CodecProfile;
	m_mfxEncParams.mfx.CodecLevel = pInParams->CodecLevel;

    // configure and attach external parameters 
    
    if (pInParams->bIsMVC)
        m_EncExtParams.push_back((mfxExtBuffer *)&m_MVCSeqDesc);

    if (!m_EncExtParams.empty())
    {
        m_mfxEncParams.ExtParam = &m_EncExtParams[0]; // vector is stored linearly in memory
        m_mfxEncParams.NumExtParam = (mfxU16)m_EncExtParams.size();
    }
    
    return MFX_ERR_NONE;
}


mfxStatus CEncodingPipeline::CreateDeviceManager()
{    
#ifdef D3D_SURFACES_SUPPORT
   m_pd3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_pd3d)
        return MFX_ERR_NULL_PTR;

    POINT point = {0, 0};
    HWND window = WindowFromPoint(point);

    D3DPRESENT_PARAMETERS d3dParams;
    memset(&d3dParams, 0, sizeof(d3dParams));
    d3dParams.Windowed = TRUE;
    d3dParams.hDeviceWindow = window;
    d3dParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dParams.Flags = D3DPRESENTFLAG_VIDEO;
    d3dParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    d3dParams.BackBufferCount = 1;
    d3dParams.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dParams.BackBufferWidth = 0;
    d3dParams.BackBufferHeight = 0;
    
    HRESULT hr = m_pd3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
        &d3dParams,
        &m_pd3dDevice);
    if (FAILED(hr))
        return MFX_ERR_NULL_PTR;

    UINT resetToken = 0;    
    hr = DXVA2CreateDirect3DDeviceManager9(&resetToken, &m_pd3dDeviceManager);
    if (FAILED(hr))
        return MFX_ERR_NULL_PTR;

    hr = m_pd3dDeviceManager->ResetDevice(m_pd3dDevice, resetToken);
    if (FAILED(hr))
        return MFX_ERR_UNDEFINED_BEHAVIOR;
    
    m_resetToken = resetToken;   
#endif
    return MFX_ERR_NONE;
}

mfxStatus CEncodingPipeline::ResetDevice()
{
    if (m_bd3dAlloc)
    {
#ifdef D3D_SURFACES_SUPPORT
    HRESULT hr = m_pd3dDeviceManager->ResetDevice(m_pd3dDevice, m_resetToken);
    if (FAILED(hr))
        return MFX_ERR_UNDEFINED_BEHAVIOR;
    else
        return MFX_ERR_NONE;
#endif
    }
    else
    {
        return MFX_ERR_NONE;
    }
}

mfxStatus CEncodingPipeline::AllocFrames()
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

    // The number of surfaces shared by vpp output and encode input.
    // When surfaces are shared 1 surface at first component output contains output frame that goes to next component input
    nEncSurfNum = EncRequest.NumFrameSuggested + MSDK_MAX(VppRequest[1].NumFrameSuggested, 1) - 1 + (m_nAsyncDepth - 1); 

    // The number of surfaces for vpp input - so that vpp can work at async depth = m_nAsyncDepth
    nVppSurfNum = VppRequest[0].NumFrameSuggested + (m_nAsyncDepth - 1); 

    // prepare allocation requests
    EncRequest.NumFrameMin = nEncSurfNum;
    EncRequest.NumFrameSuggested = nEncSurfNum;
    memcpy(&(EncRequest.Info), &(m_mfxEncParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
    EncRequest.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_ENCODE;
    // add info about memory type to request 
    EncRequest.Type |= m_bd3dAlloc ? MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET : MFX_MEMTYPE_SYSTEM_MEMORY; 

    // alloc frames for encoder
    sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &EncRequest, &m_EncResponse);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // prepare mfxFrameSurface1 array for encoder    
    m_pEncSurfaces = new mfxFrameSurface1 [m_EncResponse.NumFrameActual];
    MSDK_CHECK_POINTER(m_pEncSurfaces, MFX_ERR_MEMORY_ALLOC);

    for (int i = 0; i < m_EncResponse.NumFrameActual; i++)
    {       
        memset(&(m_pEncSurfaces[i]), 0, sizeof(mfxFrameSurface1));
        memcpy(&(m_pEncSurfaces[i].Info), &(m_mfxEncParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

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

    return MFX_ERR_NONE;
}

mfxStatus CEncodingPipeline::CreateAllocator()
{   
    mfxStatus sts = MFX_ERR_NONE;

    if (m_bd3dAlloc)
    {
#ifdef D3D_SURFACES_SUPPORT       
        sts = CreateDeviceManager();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // provide device manager to MediaSDK
        sts = m_mfxSession.SetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER, m_pd3dDeviceManager);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // crate D3D allocator
        m_pMFXAllocator = new D3DFrameAllocator; 
        MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

        D3DAllocatorParams *pd3dAllocParams = new D3DAllocatorParams; 
        MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);

        pd3dAllocParams->pManager = m_pd3dDeviceManager;
        m_pmfxAllocatorParams = pd3dAllocParams;

        /* In case of video memory we must provide Media SDK with external allocator 
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to Media SDK */
        sts = m_mfxSession.SetFrameAllocator(m_pMFXAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        m_bExternalAlloc = true;  
#endif
    } 
    else
    {        
        // create system memory allocator       
        m_pMFXAllocator = new SysMemFrameAllocator; 
        MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

        /* In case of system memory we demonstrate "no external allocator" usage model.  
        We don't call SetAllocator, Media SDK uses internal allocator. 
        We use system memory allocator simply as a memory manager for application*/           
    }   

    // initialize memory allocator
    sts = m_pMFXAllocator->Init(m_pmfxAllocatorParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE; 
}

void CEncodingPipeline::DeleteFrames()
{
    // delete surfaces array
    MSDK_SAFE_DELETE_ARRAY(m_pEncSurfaces);    

    // delete frames
    if (m_pMFXAllocator)
    {        
        m_pMFXAllocator->Free(m_pMFXAllocator->pthis, &m_EncResponse);
    }    
}

void CEncodingPipeline::DeleteDeviceManager()
{
#ifdef D3D_SURFACES_SUPPORT 
    MSDK_SAFE_RELEASE(m_pd3dDevice);
    MSDK_SAFE_RELEASE(m_pd3dDeviceManager);
    MSDK_SAFE_RELEASE(m_pd3d);  
#endif      
}

void CEncodingPipeline::DeleteAllocator()
{    
    // delete allocator
    MSDK_SAFE_DELETE(m_pMFXAllocator);   
    MSDK_SAFE_DELETE(m_pmfxAllocatorParams);

    DeleteDeviceManager();
}

CEncodingPipeline::CEncodingPipeline(IntelH264EncoderCallback&rCallback)
:m_rCallback(rCallback)
,m_TaskPool(rCallback)
{    
    m_pmfxENC = NULL;
    m_pMFXAllocator = NULL;
    m_pmfxAllocatorParams = NULL;
    m_bd3dAlloc = false;
    m_bExternalAlloc = false;
    m_pEncSurfaces = NULL;   
    m_nAsyncDepth = 0;

    m_bIsMVC = false;    
    m_nNumView = 0;

    MSDK_ZERO_MEMORY(m_MVCSeqDesc);

    m_MVCSeqDesc.Header.BufferId = MFX_EXTBUFF_MVC_SEQ_DESC;
    m_MVCSeqDesc.Header.BufferSz = sizeof(mfxExtMVCSeqDesc);

#ifdef D3D_SURFACES_SUPPORT
    m_pd3d = NULL;
    m_pd3dDeviceManager = NULL;  
    m_pd3dDevice        = NULL;
    m_resetToken        = 0;
#endif 

    MSDK_ZERO_MEMORY(m_mfxEncParams);

    MSDK_ZERO_MEMORY(m_EncResponse); 
}

CEncodingPipeline::~CEncodingPipeline()
{
    Close();
}

void CEncodingPipeline::SetMultiView()
{
    m_bIsMVC = true;
}

mfxStatus CEncodingPipeline::Init(sInputParams *pParams)
{
    MSDK_CHECK_POINTER(pParams, MFX_ERR_NULL_PTR);

    mfxStatus sts = MFX_ERR_NONE;


    // init session
    mfxIMPL impl = MFX_IMPL_HARDWARE;// : MFX_IMPL_SOFTWARE;
    
    mfxVersion ver = {0, 1};
    mfxVersion verMVC = {3, 1};    
    mfxVersion version;
    version = (m_bIsMVC) ? verMVC : ver; // API 1.0 is enough for this pipeline if NOT MVC
    sts = m_mfxSession.Init(impl, &version);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // set memory type
    m_bd3dAlloc = pParams->bd3dAlloc;     
    
    // create and init frame allocator 
    sts = CreateAllocator();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = InitMfxEncParams(pParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);    

    sts = AllocAndInitMVCSeqDesc();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);    

    // MVC specific options
    if (m_bIsMVC)
    {
        sts = AllocAndInitMVCSeqDesc();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);      
    }

    // create encoder
    m_pmfxENC = new MFXVideoENCODE(m_mfxSession); 
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_MEMORY_ALLOC);

    m_nAsyncDepth = 4; // this number can be tuned for better performance

    sts = ResetMFXComponents(); 
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    
    return MFX_ERR_NONE;
}

void CEncodingPipeline::Close()
{    
    MSDK_SAFE_DELETE(m_pmfxENC);

    FreeMVCSeqDesc();

    DeleteFrames();    
    // allocator if used as external for MediaSDK must be deleted after SDK components
    DeleteAllocator();

    m_TaskPool.Close();
    m_mfxSession.Close();
}

mfxStatus CEncodingPipeline::ResetMFXComponents(void)
{
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_NOT_INITIALIZED);

    mfxStatus sts = MFX_ERR_NONE;    

    sts = m_pmfxENC->Close();
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_NOT_INITIALIZED);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // free allocated frames
    DeleteFrames();

    m_TaskPool.Close();   
    
    sts = AllocFrames();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);   

    sts = m_pmfxENC->Init(&m_mfxEncParams);
    MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);    

    mfxU32 nEncodedDataBufferSize = m_mfxEncParams.mfx.FrameInfo.Width * m_mfxEncParams.mfx.FrameInfo.Height * 4;    
    sts = m_TaskPool.Init(&m_mfxSession, m_nAsyncDepth, nEncodedDataBufferSize);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE;
}

mfxStatus CEncodingPipeline::AllocateSufficientBuffer(mfxBitstream* pBS)
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

mfxStatus CEncodingPipeline::Run(unsigned char*pYV12Data[],int nRequestKeyFrame)
{   
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_NOT_INITIALIZED);

    mfxStatus sts = MFX_ERR_NONE;    
    
    mfxFrameSurface1* pSurf = NULL; // dispatching pointer    

    sTask *pCurrentTask = NULL; // a pointer to the current task
    mfxU16 nEncSurfIdx = 0; // index of free surface for encoder input (vpp output)
    
    // Since in sample we support just 2 views 
    // we will change this value between 0 and 1 in case of MVC
    mfxU16 currViewNum = 0;     

	mfxEncodeCtrl EncodeCtrl;
	memset(&EncodeCtrl, 0, sizeof(mfxEncodeCtrl));
	EncodeCtrl.FrameType = MFX_FRAMETYPE_I | MFX_FRAMETYPE_REF | MFX_FRAMETYPE_IDR;

	mfxEncodeCtrl* pEncodeCtrl=NULL;
	if (nRequestKeyFrame)
	{
		pEncodeCtrl=&EncodeCtrl;
	}
    
    sts = MFX_ERR_NONE;       

    // main loop, preprocessing and encoding
	do
    {        
        // get a pointer to a free task (bit stream and sync point for encoder)
        sts = m_TaskPool.GetFreeTask(&pCurrentTask);
        MSDK_BREAK_ON_ERROR(sts);

        // find free surface for encoder input
        nEncSurfIdx = GetFreeSurface(m_pEncSurfaces, m_EncResponse.NumFrameActual);
        MSDK_CHECK_ERROR(nEncSurfIdx, MSDK_INVALID_SURF_IDX, MFX_ERR_MEMORY_ALLOC);

        // point pSurf to encoder surface
        pSurf = &m_pEncSurfaces[nEncSurfIdx];        

        {
            // load frame from file to surface data
            // if we share allocator with Media SDK we need to call Lock to access surface data and after we're done call Unlock
            if (m_bExternalAlloc) 
            {
                // get YUV pointers
                sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, pSurf->Data.MemId, &(pSurf->Data));
                MSDK_BREAK_ON_ERROR(sts);

                pSurf->Info.FrameId.ViewId = currViewNum;
                sts = LoadNextFrame(pSurf,pYV12Data);
                MSDK_BREAK_ON_ERROR(sts);
                if (m_bIsMVC) currViewNum ^= 1; // Flip between 0 and 1 for ViewId

                sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, pSurf->Data.MemId, &(pSurf->Data));
                MSDK_BREAK_ON_ERROR(sts);
            }
            else 
            {
                pSurf->Info.FrameId.ViewId = currViewNum;
                sts = LoadNextFrame(pSurf,pYV12Data);
                MSDK_BREAK_ON_ERROR(sts);
                if (m_bIsMVC) currViewNum ^= 1; // Flip between 0 and 1 for ViewId
            }
        }        
               
        
        for (;;)
        {
            // at this point surface for encoder contains either a frame from file or a frame processed by vpp        
            sts = m_pmfxENC->EncodeFrameAsync(pEncodeCtrl, &m_pEncSurfaces[nEncSurfIdx], &pCurrentTask->mfxBS, &pCurrentTask->EncSyncP);
            
            if (MFX_ERR_NONE < sts && !pCurrentTask->EncSyncP) // repeat the call if warning and no output
            {
                if (MFX_WRN_DEVICE_BUSY == sts)                
                    Sleep(1); // wait if device is busy                
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
                break;
            }
        }            
    }while(0);

    // means that the input file has ended, need to go to buffering loops
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
    // exit in case of other errors
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

 

    // loop to get buffered frames from encoder
    while (MFX_ERR_NONE <= sts)
    {       
        // get a free task (bit stream and sync point for encoder)
        sts = m_TaskPool.GetFreeTask(&pCurrentTask);
        MSDK_BREAK_ON_ERROR(sts);
        
        for (;;)
        {                
            sts = m_pmfxENC->EncodeFrameAsync(NULL, NULL, &pCurrentTask->mfxBS, &pCurrentTask->EncSyncP);            

            if (MFX_ERR_NONE < sts && !pCurrentTask->EncSyncP) // repeat the call if warning and no output
            {
                if (MFX_WRN_DEVICE_BUSY == sts)                
                    Sleep(1); // wait if device is busy                
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
                break;
            }
        }            
        MSDK_BREAK_ON_ERROR(sts); 
    }    

    // MFX_ERR_MORE_DATA is the correct status to exit buffering loop with
    // indicates that there are no more buffered frames
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
    // exit in case of other errors
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // synchronize all tasks that are left in task pool
    while (MFX_ERR_NONE == sts)
    {
        sts = m_TaskPool.SynchronizeFirstTask();
    }  

    // MFX_ERR_NOT_FOUND is the correct status to exit the loop with
    // EncodeFrameAsync and SyncOperation don't return this status
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_NOT_FOUND); 
    // report any errors that occurred in asynchronous part
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);     

    return sts; 
}


mfxStatus CEncodingPipeline::LoadNextFrame(mfxFrameSurface1* pSurface,unsigned char*pYV12Data[])
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
