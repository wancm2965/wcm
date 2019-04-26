//* ////////////////////////////////////////////////////////////////////////////// */
//*
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license  agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in  accordance  with the terms of that agreement.
//        Copyright (c) 2005-2014 Intel Corporation. All Rights Reserved.
//
//
//*/

#ifndef __PIPELINE_ENCODE_H__
#define __PIPELINE_ENCODE_H__

#include "sample_defs.h"
//#include "mfx_plugin_uids.h"

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
#include "sample_params.h"

#include <vector>
#include <memory>

//struct sInputParams
//{
//    mfxU16 nTargetUsage;
//    mfxU32 CodecId;
//	mfxU32 CodecProfile;
//	mfxU32 CodecLevel;
//    mfxU32 ColorFormat;
//    mfxU16 nPicStruct;
//    mfxU16 nWidth; // source picture width
//    mfxU16 nHeight; // source picture height
//    mfxF64 dFrameRate;
//    mfxU16 nBitRate;
//    mfxU16 nQuality; // quality parameter for JPEG encoder
//
//    mfxU16 nDstWidth; // destination picture width, specified if resizing required
//    mfxU16 nDstHeight; // destination picture height, specified if resizing required
//
//    bool bUseHWLib; // true if application wants to use HW MSDK library
//
//
//    mfxU32  HEVCPluginVersion;
//
//
//	mfxU32 nMaxKeyFrameInterval;
//};

enum MemType {
	SYSTEM_MEMORY = 0x00,
	D3D9_MEMORY = 0x01,
	D3D11_MEMORY = 0x02,
};

struct sInputParams
{
	mfxU16 nTargetUsage;
	mfxU32 CodecId;
	mfxU32 ColorFormat;
	mfxU16 nPicStruct;
	mfxU16 nWidth; // source picture width
	mfxU16 nHeight; // source picture height
	mfxF64 dFrameRate;
	mfxU32 nNumFrames;
	mfxU16 nBitRate;
	mfxU16 MVC_flags;
	mfxU16 nGopPicSize;
	mfxU16 nGopRefDist;
	mfxU16 nNumRefFrame;
	mfxU16 nBRefType;
	mfxU16 nIdrInterval;
	mfxU16 reserved[4];

	mfxU16 nQuality; // quality parameter for JPEG encoder

	mfxU32 numViews; // number of views for Multi-View Codec

	mfxU16 nDstWidth; // destination picture width, specified if resizing required
	mfxU16 nDstHeight; // destination picture height, specified if resizing required

	MemType memType;
	bool bUseHWLib; // true if application wants to use HW MSDK library

	msdk_char strSrcFile[MSDK_MAX_FILENAME_LEN];

	sPluginParams pluginParams;

	std::vector<msdk_char*> srcFileBuff;
	std::vector<msdk_char*> dstFileBuff;

	mfxU32  HEVCPluginVersion;
	mfxU8 nRotationAngle; // if specified, enables rotation plugin in mfx pipeline
	msdk_char strPluginDLLPath[MSDK_MAX_FILENAME_LEN]; // plugin dll path and name

	mfxU16 nAsyncDepth; // depth of asynchronous pipeline, this number can be tuned to achieve better performance
	mfxU16 gpuCopy; // GPU Copy mode (three-state option)

	mfxU16 nRateControlMethod;
	mfxU16 nLADepth; // depth of the look ahead bitrate control  algorithm
	mfxU16 nMaxSliceSize; //maximum size of slice
	mfxU16 nQPI;
	mfxU16 nQPP;
	mfxU16 nQPB;

	bool enableQSVFF;

	mfxU16 nNumSlice;
	bool UseRegionEncode;

	bool isV4L2InputEnabled;

#if defined (ENABLE_V4L2_SUPPORT)
	msdk_char DeviceName[MSDK_MAX_FILENAME_LEN];
	msdk_char MipiModeName[MSDK_MAX_FILENAME_LEN];
	enum V4L2PixelFormat v4l2Format;
	int MipiPort;
	enum AtomISPMode MipiMode;
#endif

};

class IntelH264EncoderCallback
{
public:
	virtual void OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)=0;
};

struct sTask
{
    mfxBitstream mfxBS;
    mfxSyncPoint EncSyncP;
    std::list<mfxSyncPoint> DependentVppTasks;
    IntelH264EncoderCallback*	m_pCallback;

    sTask(void);
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

	IntelH264EncoderCallback&	m_rCallback; 

    MFXVideoSession* m_pmfxSession;

    virtual mfxU32 GetFreeTaskIndex();
};


class CEncodingPipeline
{
public:
    CEncodingPipeline(IntelH264EncoderCallback&rCallback);
    virtual ~CEncodingPipeline();

    virtual mfxStatus Init(sInputParams *pParams);
    virtual mfxStatus Run(unsigned char*pYV12Data[],int nRequestKeyFrame);
    virtual void Close();
    virtual mfxStatus ResetMFXComponents(sInputParams* pParams);

protected:
	mfxStatus LoadNextFrame(mfxFrameSurface1* pSurface,unsigned char*pYV12Data[]);
protected:
	IntelH264EncoderCallback&	m_rCallback; 

    CEncTaskPool m_TaskPool;
    mfxU16 m_nAsyncDepth; // depth of asynchronous pipeline, this number can be tuned to achieve better performance

    MFXVideoSession m_mfxSession;
    MFXVideoENCODE* m_pmfxENC;

    mfxVideoParam m_mfxEncParams;

    mfxFrameSurface1* m_pEncSurfaces; // frames array for encoder input (vpp output)

	std::vector<mfxExtBuffer*> m_extBuffers;
	mfxEncodeCtrl m_ctrl, *m_pCtrl;

    mfxExtCodingOption m_extCO;
    // for look ahead BRC configuration
    mfxExtCodingOption2 m_CodingOption2;

    virtual mfxStatus InitMfxEncParams(sInputParams *pParams);

    virtual mfxStatus AllocateSufficientBuffer(mfxBitstream* pBS);

    virtual mfxStatus GetFreeTask(sTask **ppTask);
    virtual mfxStatus SynchronizeFirstTask();

	sInputParams m_sInputParams; //??
	int          m_nResetCount;  //??

	mfxExtHEVCParam m_ExtHEVCParam;
	std::vector<mfxExtBuffer*> m_EncExtParams;

public:
	mfxStatus Reset(char * info,mfxStatus sts =MFX_ERR_NONE );//??


};

#endif // __PIPELINE_ENCODE_H__
