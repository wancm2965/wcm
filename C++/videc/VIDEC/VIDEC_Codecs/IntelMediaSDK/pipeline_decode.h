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
   
#ifndef __PIPELINE_DECODE_H__
#define __PIPELINE_DECODE_H__

#define D3D_SURFACES_SUPPORT

#ifdef D3D_SURFACES_SUPPORT
#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#include "decode_render.h"
#endif

#include "sample_defs.h"
#include "sample_utils.h"
#include "base_allocator.h"

#include "mfxmvc.h"
#include "mfxjpeg.h"
#include "mfxvideo.h"
#include "mfxvideo++.h"

#include "../HWVideoDecoder.h"

struct sInputParams
{
    mfxU32 videoType;
    bool   bd3dAlloc; // true - frames in video memory (d3d surfaces),  false - in system memory
    bool   bUseHWLib; // true if application wants to use HW mfx library
    bool   bCalLatency; // latency calculation for synhronous mode

    sInputParams()
    {
        MSDK_ZERO_MEMORY(*this);
    }
};

class CDecodingPipeline
	: public HWVideoDecoder
{
public:
    CDecodingPipeline(HWVideoDecoderCallback&rCallback);
    virtual ~CDecodingPipeline();
public:
	virtual int Open(int nWidth,int nHeight);
	virtual void Close(void);
	virtual int Decode(const unsigned char*pData,int nLen);
	virtual void FreeBuffer(void*pBuffer);
	virtual int GetWidth(void){return m_nWidth;}
	virtual int GetHeight(void){return m_nHeight;}

	virtual mfxStatus DoOpen(void);
    virtual mfxStatus Init(unsigned char*pData,int nLen);
    virtual mfxStatus RunDecoding(unsigned char*pData,int nLen);
	virtual mfxStatus ReadNextFrame(mfxBitstream *pBS,unsigned char*pData,int nLen);
	virtual mfxStatus WriteNextFrame(mfxFrameSurface1 *pSurface);
    virtual mfxStatus ResetDecoder(unsigned char*pData,int nLen);
    virtual mfxStatus ResetDevice();

    void SetExtBuffersFlag()       { m_bIsExtBuffers = true; }
protected:
    mfxU32                  m_nFrameIndex; // index of processed frame
    mfxBitstream            m_mfxBS; // contains encoded data

    MFXVideoSession     m_mfxSession;
    MFXVideoDECODE*     m_pmfxDEC;
    mfxVideoParam       m_mfxVideoParams; 

    mfxExtBuffer** m_ppExtBuffers;
    mfxU16 m_nNumExtBuffers;
    
    MFXFrameAllocator*      m_pMFXAllocator; 
    mfxAllocatorParams*     m_pmfxAllocatorParams;
    bool                    m_bd3dAlloc; // use d3d surfaces
    bool                    m_bExternalAlloc; // use memory allocator as external for Media SDK
    mfxFrameSurface1*       m_pmfxSurfaces; // frames array
    mfxFrameAllocResponse   m_mfxResponse;  // memory allocation response for decoder  

    bool                    m_bIsExtBuffers; // indicates if external buffers were allocated
    
    IDirect3D9*              m_pd3d;    
    CHWDevice               *m_hwdev;
    UINT                     m_resetToken;

	HWVideoDecoderCallback&		m_rCallback;
	bool						m_bDecodedHeader;
	int							m_nWidth;
	int							m_nHeight;
	unsigned char*				m_pNV12Data;
	unsigned char*				m_pUV;
	int							m_nY;
	int							m_nUV;
	int							m_nStride;
	int							m_nNV12DataLen;
    
    virtual mfxStatus InitMfxParams(unsigned char*pData,int nLen);

    virtual void DeleteExtBuffers();

    virtual void AttachExtParam();

    virtual mfxStatus CreateAllocator();
    virtual mfxStatus CreateHWDevice(); 
    virtual mfxStatus AllocFrames();
    virtual void DeleteFrames();         
    virtual void DeleteAllocator();       
};

#endif // __PIPELINE_DECODE_H__ 