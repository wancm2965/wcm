//* ////////////////////////////////////////////////////////////////////////////// */
//*
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license  agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in  accordance  with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//
//
//*/

#ifndef __PIPELINE_ENCODE_H__
#define __PIPELINE_ENCODE_H__

#define D3D_SURFACES_SUPPORT

#ifdef D3D_SURFACES_SUPPORT
#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#endif

#include "sample_defs.h"
#include "sample_utils.h"
#include "base_allocator.h"

#include "mfxmvc.h"
#include "mfxvideo.h"
#include "mfxvideo++.h"

#include <vector>

struct sInputParams
{
    mfxU16 nTargetUsage;
    mfxU32 CodecId;
	mfxU16 CodecProfile;
	mfxU16 CodecLevel;
    mfxU32 ColorFormat;
    mfxU16 nPicStruct;
    mfxU16 nWidth; // source picture width
    mfxU16 nHeight; // source picture height
    mfxF64 dFrameRate;
    mfxU16 nBitRate;   
    bool   bIsMVC; // true if Multi-View-Codec is in use

    mfxU32 numViews; // number of views for Multi-View-Codec

    bool bd3dAlloc; // true - frames in video memory (d3d surfaces), false - in system memory
    bool bUseHWLib; // true if application wants to use HW mfx library

};

struct sTask
{
    mfxBitstream mfxBS;
    mfxSyncPoint EncSyncP;    
};

class IntelH264EncoderCallback
{
public:
	virtual void OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)=0;
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
class CEncodingPipeline
{
public:
    CEncodingPipeline(IntelH264EncoderCallback&rCallback);
    virtual ~CEncodingPipeline();

    virtual mfxStatus Init(sInputParams *pParams);
    virtual mfxStatus Run(unsigned char*pYV12Data[],int nRequestKeyFrame);
    virtual void Close();
    virtual mfxStatus ResetMFXComponents(void);
    virtual mfxStatus ResetDevice();

    void SetMultiView();    
    void SetNumView(mfxU32 numViews) { m_nNumView = numViews; }
protected:
	mfxStatus LoadNextFrame(mfxFrameSurface1* pSurface,unsigned char*pYV12Data[]);
protected:
	IntelH264EncoderCallback&	m_rCallback; 
    
    CEncTaskPool m_TaskPool; 
    mfxU16 m_nAsyncDepth; // depth of asynchronous pipeline, this number can be tuned to achieve better performance

    MFXVideoSession m_mfxSession;
    MFXVideoENCODE* m_pmfxENC;

    mfxVideoParam m_mfxEncParams; 
    
    bool m_bIsMVC; // MVC codec is in use   
    
    MFXFrameAllocator* m_pMFXAllocator; 
    mfxAllocatorParams* m_pmfxAllocatorParams;
    bool m_bd3dAlloc; // use d3d surfaces
    bool m_bExternalAlloc; // use memory allocator as external for Media SDK

    mfxFrameSurface1* m_pEncSurfaces; // frames array for encoder input (vpp output)
    mfxFrameAllocResponse m_EncResponse;  // memory allocation response for encoder  

    mfxU32 m_nNumView;

    // for MVC encoder and VPP configuration
    mfxExtMVCSeqDesc m_MVCSeqDesc;

    // external parameters for each component are stored in a vector
    std::vector<mfxExtBuffer*> m_EncExtParams;

#ifdef D3D_SURFACES_SUPPORT
    IDirect3D9*              m_pd3d;
    IDirect3DDeviceManager9* m_pd3dDeviceManager; 
    IDirect3DDevice9* m_pd3dDevice;
    UINT m_resetToken;
#endif  

    virtual mfxStatus InitMfxEncParams(sInputParams *pParams);
    
    virtual mfxStatus AllocAndInitMVCSeqDesc();
    virtual void FreeMVCSeqDesc();   
  
    virtual mfxStatus CreateAllocator();
    virtual void DeleteAllocator();    

    virtual mfxStatus CreateDeviceManager(); 
    virtual void DeleteDeviceManager();

    virtual mfxStatus AllocFrames();
    virtual void DeleteFrames();             
    
    virtual mfxStatus AllocateSufficientBuffer(mfxBitstream* pBS);
};

#endif // __PIPELINE_ENCODE_H__ 