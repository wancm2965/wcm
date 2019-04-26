#ifndef __INTEL_ENCODER_RENDERER_H__
#define __INTEL_ENCODER_RENDERER_H__

#include "sample_defs.h"
#include "mfx_plugin_uids.h"
#include "d3d_device.h"

#ifdef D3D_SURFACES_SUPPORT
#pragma warning(disable : 4201)
#endif

#include "sample_utils.h"
#include "base_allocator.h"

#include "mfxmvc.h"
#include "mfxvideo.h"
#include "mfxvideo++.h"
#include "mfxplugin.h"
#include "mfxplugin++.h"

#include "HPAVC_D3DRender.h"

#include <vector>
#include <memory>

#if defined(_WIN32) || defined(_WIN64)
    #define PLUGIN_NAME "sample_rotate_plugin.dll"
#else
    #define PLUGIN_NAME "libsample_rotate_plugin.so"
#endif

#include "YUVPin.h"


enum MemType {
    SYSTEM_MEMORY = 0x00,
    D3D9_MEMORY   = 0x01,
    D3D11_MEMORY  = 0x02,
};

struct sInputParams
{
	mfxU16 nTargetUsage;
	mfxU32 CodecId;
	mfxU32 CodecProfile;
	mfxU32 CodecLevel;
	mfxU32 ColorFormat;
	mfxU16 nPicStruct;
	mfxU16 nWidth; // source picture width
	mfxU16 nHeight; // source picture height
	mfxF64 dFrameRate;
	mfxU16 nBitRate;

	mfxU16 nDstWidth; // destination picture width, specified if resizing required
	mfxU16 nDstHeight; // destination picture height, specified if resizing required

	MemType memType;
	bool bUseHWLib; // true if application wants to use HW MSDK library


	mfxU32  HEVCPluginVersion;


	mfxU32 nMaxKeyFrameInterval;

	HWND hWnd;
};

class IntelH264EncoderCallback
{
public:
	virtual void OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)=0;
	virtual void OnIntelH264EncoderCallbackDraw(void* hDC)=0;
	virtual void OnIntelH264EncoderCallbackSurface(mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc)=0;
};


struct sTask
{
    mfxBitstream mfxBS;
    mfxSyncPoint EncSyncP;

	IntelH264EncoderCallback*	m_pCallback;

    sTask();
    mfxStatus WriteBitstream();
    mfxStatus Reset();
	mfxStatus Init(mfxU32 nBufferSize,IntelH264EncoderCallback*pCallback);
    mfxStatus Close();
};

class CEncTaskPool
{
public:
	CEncTaskPool(IntelH264EncoderCallback&rCallback);
    virtual ~CEncTaskPool();

	virtual mfxStatus Init(MFXVideoSession* pmfxSession, mfxU32 nPoolSize, mfxU32 nBufferSize);
    virtual mfxStatus GetFreeTask(sTask **ppTask);
    virtual mfxStatus SynchronizeFirstTask();
    virtual void Close();

protected:
    sTask* m_pTasks;
    mfxU32 m_nPoolSize;
    mfxU32 m_nTaskBufferStart;

    MFXVideoSession* m_pmfxSession;
	IntelH264EncoderCallback&	m_rCallback; 

    virtual mfxU32 GetFreeTaskIndex();
};

/* This class implements a pipeline with 2 mfx components: vpp (video preprocessing) and encode */
class IntelEncoderRenderer 
{
public:
    IntelEncoderRenderer(IntelH264EncoderCallback&rCallback);
    virtual ~IntelEncoderRenderer();
public:
    virtual mfxStatus Init(sInputParams *pParams,HPAVC_D3DRender*pD3DRender);
    virtual mfxStatus EncodeFrame(unsigned char*pYV12Data[],int nRequestKeyFrame);
    virtual void Close();
    virtual mfxStatus ResetMFXComponents(sInputParams* pParams);
    virtual mfxStatus ResetDevice();

	int GetVideoWidth(void);
	int GetVideoHeight(void);
	int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);

protected:
	virtual mfxStatus LoadNextFrame(mfxFrameSurface1* pSurface,unsigned char*pYV12Data[]);
protected:
	IntelH264EncoderCallback&	m_rCallback; 

    CEncTaskPool m_TaskPool;
    mfxU16 m_nAsyncDepth; // depth of asynchronous pipeline, this number can be tuned to achieve better performance
	mfxEncodeCtrl m_ctrl, *m_pCtrl;

    MFXVideoSession m_mfxSession;
    MFXVideoENCODE* m_pmfxENC;
    MFXVideoVPP* m_pmfxVPP;

    mfxVideoParam m_mfxEncParams;
    mfxVideoParam m_mfxVppParams;


    std::auto_ptr<MFXPlugin> m_pHEVC_plugin;
    std::auto_ptr<MFXVideoUSER>  m_pUserModule;
    const msdkPluginUID*     m_pUID;

    MFXFrameAllocator* m_pMFXAllocator;
    mfxAllocatorParams* m_pmfxAllocatorParams;
    MemType m_memType;
    bool m_bExternalAlloc; // use memory allocator as external for Media SDK

    mfxFrameSurface1* m_pEncSurfaces; // frames array for encoder input (vpp output)
    mfxFrameSurface1* m_pVppSurfaces; // frames array for vpp input
    mfxFrameAllocResponse m_EncResponse;  // memory allocation response for encoder
    mfxFrameAllocResponse m_VppResponse;  // memory allocation response for vpp

    // for disabling VPP algorithms
    mfxExtVPPDoNotUse m_VppDoNotUse;
    mfxExtCodingOption m_CodingOption;
	mfxExtVPPProcAmp		m_VppProcAmp;
	mfxExtVPPDetail			m_VppDetailConfig;
	mfxExtVPPDenoise		m_VppDenoiseConfig;

	bool					m_bVppParamsChanged;

    // external parameters for each component are stored in a vector
	mfxExtBuffer*           m_VppExtParams[4];
    std::vector<mfxExtBuffer*> m_EncExtParams;

    HPAVC_D3DRender *			m_pD3DRender;

	mfxFrameSurface1*			m_pSurfRenderder;


    virtual mfxStatus InitMfxEncParams(sInputParams *pParams);
    virtual mfxStatus InitMfxVppParams(sInputParams *pParams);


    virtual mfxStatus AllocAndInitVppDoNotUse();
    virtual void FreeVppDoNotUse();

    virtual mfxStatus CreateAllocator();
    virtual void DeleteAllocator();

    virtual mfxStatus CreateHWDevice();
    virtual void DeleteHWDevice();

    virtual mfxStatus AllocFrames();
    virtual void DeleteFrames();

    virtual mfxStatus AllocateSufficientBuffer(mfxBitstream* pBS);

    virtual mfxStatus GetFreeTask(sTask **ppTask);
    virtual mfxStatus SynchronizeFirstTask();
};

#endif // __PIPELINE_ENCODE_H__
