/*********************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2005-2014 Intel Corporation. All Rights Reserved.

**********************************************************************************/

//#include "mfx_samples_config.h"

#include "pipeline_encode.h"
#include "sysmem_allocator.h"

#if D3D_SURFACES_SUPPORT
#include "d3d_allocator.h"
#include "d3d11_allocator.h"

#include "d3d_device.h"
#include "d3d11_device.h"
#endif

#ifdef LIBVA_SUPPORT
#include "vaapi_allocator.h"
#include "vaapi_device.h"
#endif

#include "../VIDEC_FRE/VIDECFREAPI.h"
#include "HPAVC.h"

int g_nEncCount =0;


void EncodingPrintCodeInfo(char * info,mfxStatus sts)
{
	//if(sts != MFX_ERR_NONE) //EncodingPrintCodeInfo("", sts);
	{
		char msg[256];
		sprintf(msg,"INL Encoding %s,code:%d \n",info,sts);
		DebugOutput( loghardenc ,msg);
	}
}

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

    m_pTasks = new sTask[m_nPoolSize];
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
        else if (MFX_ERR_ABORTED == sts)
        {
            while (!m_pTasks[m_nTaskBufferStart].DependentVppTasks.empty())
            {
                // find out if the error occurred in a VPP task to perform recovery procedure if applicable
                sts = m_pmfxSession->SyncOperation(*m_pTasks[m_nTaskBufferStart].DependentVppTasks.begin(), 0);

                if (MFX_ERR_NONE == sts)
                {
                    m_pTasks[m_nTaskBufferStart].DependentVppTasks.pop_front();
                    sts = MFX_ERR_ABORTED; // save the status of the encode task
                    continue; // go to next vpp task
                }
                else
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
    DependentVppTasks.clear();

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

    DependentVppTasks.clear();

    return MFX_ERR_NONE;
}

mfxStatus CEncodingPipeline::InitMfxEncParams(sInputParams *pInParams)
{
	m_mfxEncParams.mfx.CodecId = pInParams->CodecId;
	m_mfxEncParams.mfx.TargetUsage = pInParams->nTargetUsage; // trade-off between quality and speed
	m_mfxEncParams.mfx.RateControlMethod = pInParams->nRateControlMethod;
	m_mfxEncParams.mfx.GopRefDist = pInParams->nGopRefDist;
	m_mfxEncParams.mfx.GopPicSize = pInParams->nGopPicSize;
	m_mfxEncParams.mfx.NumRefFrame = pInParams->nNumRefFrame > 0 ? pInParams->nNumRefFrame : 1;
	m_mfxEncParams.mfx.IdrInterval = pInParams->nIdrInterval;

	if (m_mfxEncParams.mfx.RateControlMethod == MFX_RATECONTROL_CQP)
	{
		m_mfxEncParams.mfx.QPI = pInParams->nQPI;
		m_mfxEncParams.mfx.QPP = pInParams->nQPP;
		m_mfxEncParams.mfx.QPB = pInParams->nQPB;
	}
	else
	{
		m_mfxEncParams.mfx.TargetKbps = pInParams->nBitRate; // in Kbps
	}

	if (pInParams->enableQSVFF)
	{
		m_mfxEncParams.mfx.LowPower = MFX_CODINGOPTION_ON;
	}

	m_mfxEncParams.mfx.NumSlice = pInParams->nNumSlice;
	ConvertFrameRate(pInParams->dFrameRate, &m_mfxEncParams.mfx.FrameInfo.FrameRateExtN, &m_mfxEncParams.mfx.FrameInfo.FrameRateExtD);
	m_mfxEncParams.mfx.EncodedOrder = 0; // binary flag, 0 signals encoder to take frames in display order


	// specify memory type
	if (D3D9_MEMORY == pInParams->memType || D3D11_MEMORY == pInParams->memType)
	{
		m_mfxEncParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
	}
	else
	{
		m_mfxEncParams.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY;
	}

	// frame info parameters
	m_mfxEncParams.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
	m_mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	m_mfxEncParams.mfx.FrameInfo.PicStruct = pInParams->nPicStruct;

	// In case of JPEG there's fine tuning of ColorFormat and Chrome format
	if (MFX_CODEC_JPEG == pInParams->CodecId)
	{
		if (MFX_FOURCC_RGB4 == pInParams->ColorFormat)
		{
			m_mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV444;
			m_mfxEncParams.mfx.FrameInfo.FourCC = MFX_FOURCC_RGB4;
		}
		else if (MFX_FOURCC_YUY2 == pInParams->ColorFormat)
		{
			m_mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV422;
			m_mfxEncParams.mfx.FrameInfo.FourCC = MFX_FOURCC_YUY2;
		}
	}

	// set frame size and crops
	if (pInParams->CodecId == MFX_CODEC_HEVC && !memcmp(pInParams->pluginParams.pluginGuid.Data, MFX_PLUGINID_HEVCE_HW.Data, sizeof(MFX_PLUGINID_HEVCE_HW.Data)))
	{
		// In case of HW HEVC decoder width and height must be aligned to 32 pixels. This limitation is planned to be removed in later versions of plugin
		m_mfxEncParams.mfx.FrameInfo.Width = MSDK_ALIGN32(pInParams->nDstWidth);
		m_mfxEncParams.mfx.FrameInfo.Height = MSDK_ALIGN32(pInParams->nDstHeight);
	}
	else
	{
		// width must be a multiple of 16
		// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
		m_mfxEncParams.mfx.FrameInfo.Width = MSDK_ALIGN16(pInParams->nDstWidth);
		m_mfxEncParams.mfx.FrameInfo.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxEncParams.mfx.FrameInfo.PicStruct) ?
			MSDK_ALIGN16(pInParams->nDstHeight) : MSDK_ALIGN32(pInParams->nDstHeight);
	}

	m_mfxEncParams.mfx.FrameInfo.CropX = 0;
	m_mfxEncParams.mfx.FrameInfo.CropY = 0;
	m_mfxEncParams.mfx.FrameInfo.CropW = pInParams->nDstWidth;
	m_mfxEncParams.mfx.FrameInfo.CropH = pInParams->nDstHeight;

	// we don't specify profile and level and let the encoder choose those basing on parameters
	// we must specify profile only for MVC codec
	//if (MVC_ENABLED & m_MVCflags)
	//{
	//	m_mfxEncParams.mfx.CodecProfile = MFX_PROFILE_AVC_STEREO_HIGH;
	//}

	// configure and attach external parameters
	//if (MVC_ENABLED & pInParams->MVC_flags)
	//	m_EncExtParams.push_back((mfxExtBuffer *)&m_MVCSeqDesc);

	//if (MVC_VIEWOUTPUT & pInParams->MVC_flags)
	//{
	//	// ViewOuput option requested
	//	m_CodingOption.ViewOutput = MFX_CODINGOPTION_ON;
	//	m_EncExtParams.push_back((mfxExtBuffer *)&m_CodingOption);
	//}

	// configure the depth of the look ahead BRC if specified in command line
	if (pInParams->nLADepth || pInParams->nMaxSliceSize || pInParams->nBRefType)
	{
		m_CodingOption2.LookAheadDepth = pInParams->nLADepth;
		m_CodingOption2.MaxSliceSize = pInParams->nMaxSliceSize;
		m_CodingOption2.BRefType = pInParams->nBRefType;
		m_EncExtParams.push_back((mfxExtBuffer *)&m_CodingOption2);
	}


	// In case of HEVC when height and/or width divided with 8 but not divided with 16
	// add extended parameter to increase performance
	if ((!((m_mfxEncParams.mfx.FrameInfo.CropW & 15) ^ 8) ||
		!((m_mfxEncParams.mfx.FrameInfo.CropH & 15) ^ 8)) &&
		(m_mfxEncParams.mfx.CodecId == MFX_CODEC_HEVC))
	{
		m_ExtHEVCParam.PicWidthInLumaSamples = m_mfxEncParams.mfx.FrameInfo.CropW;
		m_ExtHEVCParam.PicHeightInLumaSamples = m_mfxEncParams.mfx.FrameInfo.CropH;
		m_EncExtParams.push_back((mfxExtBuffer*)&m_ExtHEVCParam);
	}

	if (!m_EncExtParams.empty())
	{
		m_mfxEncParams.ExtParam = &m_EncExtParams[0]; // vector is stored linearly in memory
		m_mfxEncParams.NumExtParam = (mfxU16)m_EncExtParams.size();
	}

	// JPEG encoder settings overlap with other encoders settings in mfxInfoMFX structure
	if (MFX_CODEC_JPEG == pInParams->CodecId)
	{
		m_mfxEncParams.mfx.Interleaved = 1;
		m_mfxEncParams.mfx.Quality = pInParams->nQuality;
		m_mfxEncParams.mfx.RestartInterval = 0;
		MSDK_ZERO_MEMORY(m_mfxEncParams.mfx.reserved5);
	}

	m_mfxEncParams.AsyncDepth = pInParams->nAsyncDepth;

	return MFX_ERR_NONE;
}

CEncodingPipeline::CEncodingPipeline(IntelH264EncoderCallback&rCallback)
:m_rCallback(rCallback)
,m_TaskPool(rCallback)
{
    m_pmfxENC = NULL;

    m_pEncSurfaces = NULL;
    m_nAsyncDepth = 2;//??  


	m_pCtrl=NULL;
	MSDK_ZERO_MEMORY(m_ctrl);

    MSDK_ZERO_MEMORY(m_extCO);
    m_extCO.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
    m_extCO.Header.BufferSz = sizeof(m_extCO);

    MSDK_ZERO_MEMORY(m_CodingOption2);
    m_CodingOption2.Header.BufferId = MFX_EXTBUFF_CODING_OPTION2;
    m_CodingOption2.Header.BufferSz = sizeof(m_CodingOption2);

    MSDK_ZERO_MEMORY(m_mfxEncParams);
	m_nResetCount =0;
	memset(&m_sInputParams,0,sizeof(m_sInputParams));

	MSDK_ZERO_MEMORY(m_ExtHEVCParam);
	m_ExtHEVCParam.Header.BufferId = MFX_EXTBUFF_HEVC_PARAM;
	m_ExtHEVCParam.Header.BufferSz = sizeof(m_ExtHEVCParam);
}

CEncodingPipeline::~CEncodingPipeline()
{
    Close();
}



mfxStatus CEncodingPipeline::Init(sInputParams *pParams)
{
	DebugOutput( logcallstack ,"Encoding Pipeline  Init \n");
	if(pParams ==NULL)  EncodingPrintCodeInfo("Init pParams Error ", MFX_ERR_MEMORY_ALLOC);

    MSDK_CHECK_POINTER(pParams, MFX_ERR_NULL_PTR);

    mfxStatus sts = MFX_ERR_NONE;

    mfxVersion min_version;
    mfxVersion version;     // real API version with which library is initialized

    // we set version to 1.0 and later we will query actual version of the library which will got leaded
    min_version.Major = 1;
    min_version.Minor = 0;

	mfxIMPL impl = GetIntelAdapterIMPL();
    sts = m_mfxSession.Init(impl, &min_version);

	if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("Init GetIntelAdapterIMPL", sts);

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = MFXQueryVersion(m_mfxSession , &version); // get real API version of the loaded library
	if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("Init MFXQueryVersion", sts);

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	m_pEncSurfaces=new mfxFrameSurface1;
	memset(m_pEncSurfaces, 0, sizeof(mfxFrameSurface1));
	m_pEncSurfaces->Info.PicStruct	= MFX_PICSTRUCT_FIELD_BFF;
	m_pEncSurfaces->Info.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	m_pEncSurfaces->Info.FourCC		= MFX_FOURCC_NV12;
	m_pEncSurfaces->Info.CropH		=pParams->nHeight;
	m_pEncSurfaces->Info.CropW		=pParams->nWidth;
	m_pEncSurfaces->Info.Width		=MSDK_ALIGN16(pParams->nWidth);;
	m_pEncSurfaces->Info.Height		=MSDK_ALIGN16(pParams->nHeight);;
	m_pEncSurfaces->Info.FrameId.ViewId = 0;
	m_pEncSurfaces->Data.Pitch = pParams->nWidth;
	m_pEncSurfaces->Data.Y = (mfxU8 *)MALLOCMSG(pParams->nWidth*pParams->nHeight*2);
	m_pEncSurfaces->Data.UV = (mfxU8 *)MALLOCMSG(pParams->nWidth*pParams->nHeight);
	if( (m_pEncSurfaces->Data.Y ==NULL )  || (m_pEncSurfaces->Data.UV==NULL )  )
	{
		DebugOutput( logcoreerror ,"EncodingPipeline::Init malloc faliled. \n");
		return MFX_ERR_MEMORY_ALLOC ;
	}

    // create and init frame allocator
   // sts = CreateAllocator();
   // MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	if (MFX_CODEC_HEVC == pParams->CodecId)
	{
		//如果是HEVC，就加载插件
		sts = MFXVideoUSER_Load(m_mfxSession, &MFX_PLUGINID_HEVCE_HW, 1);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}

    sts = InitMfxEncParams(pParams);
	if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("Init InitMfxEncParams", sts);

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // create encoder
    m_pmfxENC = new MFXVideoENCODE(m_mfxSession);
	if(m_pmfxENC ==NULL)  EncodingPrintCodeInfo("new MFXVideoENCODE Error ", MFX_ERR_MEMORY_ALLOC);

    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_MEMORY_ALLOC);

    m_nAsyncDepth = 2; // this number can be tuned for better performance //??  

    sts = ResetMFXComponents(pParams);
	if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("Init ResetMFXComponents", sts);

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	memcpy(&m_sInputParams,pParams,sizeof(m_sInputParams) );
	g_nEncCount =0;
    return MFX_ERR_NONE;
}

void CEncodingPipeline::Close()
{
	//??
	DebugOutput( logcallstack ,"EncodingPipeline::Close 1\n");
    MSDK_SAFE_DELETE(m_pmfxENC);
	DebugOutput( logcallstack ,"EncodingPipeline::Close 2\n");
	if (m_pEncSurfaces)
	{
		if (m_pEncSurfaces->Data.Y)
		{
			FREEMSG(m_pEncSurfaces->Data.Y);
			m_pEncSurfaces->Data.Y=NULL;
		}
		if (m_pEncSurfaces->Data.UV)
		{
			FREEMSG(m_pEncSurfaces->Data.UV);
			m_pEncSurfaces->Data.UV=NULL;
		}
		delete m_pEncSurfaces;
		m_pEncSurfaces = NULL;
		DebugOutput( logcallstack ,"EncodingPipeline::Close 3\n");
	}

    m_TaskPool.Close();
	DebugOutput( logcallstack ,"EncodingPipeline::Close 4\n");
    m_mfxSession.Close();
	DebugOutput( logcallstack ,"EncodingPipeline::Close 5\n");
}

mfxStatus CEncodingPipeline::ResetMFXComponents(sInputParams* pParams)
{
    MSDK_CHECK_POINTER(pParams, MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_NOT_INITIALIZED);

    mfxStatus sts = MFX_ERR_NONE;

    sts = m_pmfxENC->Close();
    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_NOT_INITIALIZED);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    m_TaskPool.Close();

    sts = m_pmfxENC->Init(&m_mfxEncParams);
    if (MFX_WRN_PARTIAL_ACCELERATION == sts)
    {
        msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
    }

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    mfxU32 nEncodedDataBufferSize = m_mfxEncParams.mfx.FrameInfo.Width * m_mfxEncParams.mfx.FrameInfo.Height * 4  ;  //??
    sts = m_TaskPool.Init(&m_mfxSession, m_nAsyncDepth * 2 , nEncodedDataBufferSize); //?? 
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
    sts = ExtendMfxBitstream(pBS, par.mfx.BufferSizeInKB * 1000 ); //??
    MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, sts, WipeMfxBitstream(pBS));

    return MFX_ERR_NONE;
}

mfxStatus CEncodingPipeline::GetFreeTask(sTask **ppTask)
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

mfxStatus CEncodingPipeline::SynchronizeFirstTask()
{
    mfxStatus sts = m_TaskPool.SynchronizeFirstTask();

    return sts;
}

mfxStatus CEncodingPipeline::Run(unsigned char*pYV12Data[],int nRequestKeyFrame)
{

    MSDK_CHECK_POINTER(m_pmfxENC, MFX_ERR_NOT_INITIALIZED);

    mfxStatus sts = MFX_ERR_NONE;

    mfxFrameSurface1* pSurf = NULL; // dispatching pointer

    sTask *pCurrentTask = NULL; // a pointer to the current task
    mfxU16 nEncSurfIdx = 0;     // index of FREEMSG surface for encoder input (vpp output)

    sts = MFX_ERR_NONE;

	m_pCtrl=NULL;
	m_ctrl.FrameType = MFX_FRAMETYPE_UNKNOWN;

	if(g_nEncCount <=10 )
	{
		DebugOutput( loghardenc ,"VIDECINL_EncoderEncoding \n");
		g_nEncCount ++;
	}

	if (nRequestKeyFrame)
	{
		m_ctrl.FrameType = MFX_FRAMETYPE_I | MFX_FRAMETYPE_IDR | MFX_FRAMETYPE_REF;
		m_pCtrl = &m_ctrl;
	}

	if (!m_extBuffers.empty())
	{
		m_ctrl.NumExtParam = (mfxU16)m_extBuffers.size();
		m_ctrl.ExtParam    = &m_extBuffers.front();
		m_pCtrl            = &m_ctrl;
	}

    // get a pointer to a FREEMSG task (bit stream and sync point for encoder)
	sts = GetFreeTask(&pCurrentTask);
	if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("GetFreeTask", sts);

	 MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
       
	 sts = LoadNextFrame(m_pEncSurfaces,pYV12Data);
	 if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("LoadNextFrame", sts);

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	int nBusy = 0;

    for (;;)
    {
        // at this point surface for encoder contains either a frame from file or a frame processed by vpp
        sts = m_pmfxENC->EncodeFrameAsync(m_pCtrl, m_pEncSurfaces, &pCurrentTask->mfxBS, &pCurrentTask->EncSyncP);
		if(sts == MFX_WRN_DEVICE_BUSY )
		{
			DebugOutput( logcoreexcep ,"MFX_WRN_DEVICE_BUSY \n");
			//return Reset("EncodeFrameAsync Reset ", sts);
		}

		if(sts == MFX_ERR_NOT_ENOUGH_BUFFER )
		{
			DebugOutput( logcoreexcep ,"MFX_ERR_NOT_ENOUGH_BUFFER \n");
			//return Reset("EncodeFrameAsync Reset ", sts);
		}

        if (MFX_ERR_NONE < sts && !pCurrentTask->EncSyncP) // repeat the call if warning and no output
        {
            if (MFX_WRN_DEVICE_BUSY == sts)
			{
				if (nBusy == 1)
				{
					DebugOutput( logcoreexcep ,"Encode busy skip this frame.\n");
					break;
				}
 				DebugOutput( logcoreexcep ,"Encode busy Sleep(1).\n");
 				Sleep(1);
 				nBusy=1;				
			}
        }
        else if (MFX_ERR_NONE < sts && pCurrentTask->EncSyncP)
        {
            sts = MFX_ERR_NONE; // ignore warnings if output is available
            break;
        }
        else if (MFX_ERR_NOT_ENOUGH_BUFFER == sts)
        {
            sts = AllocateSufficientBuffer(&pCurrentTask->mfxBS);
			if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("EncodeFrameAsync reverse 2", sts);

            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
        else
        {
            // get next surface and new task for 2nd bitstream in ViewOutput mode
            MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_BITSTREAM);
            break;
        }
    }

	if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("EncodeFrameAsync reverse 1", sts);

    // means that the input file has ended, need to go to buffering loops
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
	if(sts != MFX_ERR_NONE)  EncodingPrintCodeInfo("EncodeFrameAsync end", sts);
	if(sts == MFX_ERR_DEVICE_FAILED )
	{
		DebugOutput( logcoreexcep ,"MFX_ERR_DEVICE_FAILED \n");
		//return Reset("EncodeFrameAsync Reset ", sts);
	}

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

mfxStatus CEncodingPipeline::Reset(char * info,mfxStatus sts)
{
	if(m_nResetCount <10)
	{
		EncodingPrintCodeInfo(info,sts);
		Close();
		sts =Init(&m_sInputParams);
		//sts = ResetMFXComponents(&m_sInputParams);
		EncodingPrintCodeInfo("Reset End", sts);

		if(sts !=MFX_ERR_NONE) m_nResetCount ++;
		else m_nResetCount =0;
	}
	return sts;
}
