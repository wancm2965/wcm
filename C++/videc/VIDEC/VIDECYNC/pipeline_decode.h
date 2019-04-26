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

#include <winsock2.h>

#define D3D_SURFACES_SUPPORT

#ifdef D3D_SURFACES_SUPPORT
#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#include "D3DRender.h"
#endif

#include "sample_defs.h"
#include "base_allocator.h"

#include "mfxvideo.h"
#include "mfxvideo++.h"

class CDecodingPipeline
{
public:
    CDecodingPipeline(bool bHardware);
    virtual ~CDecodingPipeline();
public:
	virtual int Open(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWndVideo,int nWidth,int nHeight);
	virtual void Close(void);

	virtual int PlayFrame(const unsigned char*pFrameData, int nFrameLen,int nFrameRate,int&nFlagReset);
	virtual int GetActualWidth(void);
	virtual int GetActualHeight(void);
	virtual int Resize(void);
	virtual int SkipDisplay(int nFlagSkipDisplay);
	virtual int GetFrameRate(void);
	virtual int GetShowType(void);
	virtual bool CaptureToBMPFile(const char*cszFile);
protected:
	virtual mfxStatus ReadNextFrame(mfxBitstream *pBS,unsigned char*pData,int nLen);

	virtual mfxStatus Init(unsigned char*pData,int nLen);
	virtual mfxStatus RunDecoding(unsigned char*pData,int nLen,int nFrameRate,int&nFlagReset);
	virtual mfxStatus ResetDecoder(unsigned char*pData,int nLen);
	virtual mfxStatus ResetDevice();
protected:
	bool						m_bHardware;
	bool						m_bDecodedHeader;
	bool						m_bInited;
	int							m_nWidth;
	int							m_nHeight;	
	HWND					m_hWndVideo;
    mfxBitstream            m_mfxBS; // contains encoded data

    MFXVideoSession     m_mfxSession;
    MFXVideoDECODE*     m_pmfxDEC;
    mfxVideoParam       m_mfxVideoParams; 

    MFXFrameAllocator*      m_pMFXAllocator; 
    mfxAllocatorParams*     m_pmfxAllocatorParams;

    mfxFrameSurface1*       m_pmfxSurfaces; // frames array
    mfxFrameAllocResponse   m_mfxResponse;  // memory allocation response for decoder  

    CD3DRender*				 m_pD3DRender11;
	CD3DRender*				 m_pD3DRender9;
   UINT                     m_resetToken;
    
    virtual mfxStatus InitMfxParams(unsigned char*pData,int nLen);


    virtual mfxStatus CreateAllocator();
    virtual mfxStatus CreateHWDevice(YNCD3D_CB_DC cbdc,void*pUserData,int nWidth,int nHeight); 
    virtual mfxStatus AllocFrames();
    virtual void DeleteFrames();         
    virtual void DeleteAllocator();       
};

#endif // __PIPELINE_DECODE_H__ 