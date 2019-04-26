//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//


#ifndef __DECODE_D3D_RENDER_H__
#define __DECODE_D3D_RENDER_H__

#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#include <dxva.h>
#include <windows.h>

#include "mfxstructures.h"
#include "mfxvideo.h"

#define OVERLAY_BACKBUFFER_FORMAT D3DFMT_X8R8G8B8
#define VIDEO_MAIN_FORMAT D3DFMT_YUY2

typedef void* WindowHandle;

enum {
    MFX_HANDLE_GFXS3DCONTROL = 0x100 /* A handle to the IGFXS3DControl instance */
}; //mfxHandleType

class CHWDevice
{
public:
    virtual ~CHWDevice(){}
    /** Initializes device for requested processing.
    @param[in] hWindow Window handle to bundle device to.
    @param[in] nViews Number of views to process.
    @param[in] Protected Protection type, should be equal to mfxVideoParam::Protection.
    */
    virtual mfxStatus Init(WindowHandle hWindow) = 0;
    /// Reset device.
    virtual mfxStatus Reset() = 0;
    /// Get handle can be used for MFX session SetHandle calls
    virtual mfxStatus GetHandle(mfxHandleType type, mfxHDL *pHdl) = 0;
    virtual void      Close() = 0;
};


/** Direct3D 9 device implementation.
@note Can be initilized for only 1 or two 2 views. Handle to 
MFX_HANDLE_GFXS3DCONTROL must be set prior if initializing for 2 views.

@note Device always set D3DPRESENT_PARAMETERS::Windowed to TRUE.
*/
class CD3D9Device : public CHWDevice
{
public:
    CD3D9Device();
    ~CD3D9Device();

    virtual mfxStatus Init(WindowHandle hWindow);
    virtual mfxStatus Reset();
    virtual mfxStatus GetHandle(mfxHandleType type, mfxHDL *pHdl);
    virtual void Close() ;
private:
	IDirect3D9*					m_pD3D9;
    IDirect3DDevice9*			m_pD3DD9;
    IDirect3DDeviceManager9*    m_pDeviceManager9;
    D3DPRESENT_PARAMETERS       m_D3DPP;
    UINT                        m_resetToken;


    D3DSURFACE_DESC                 m_backBufferDesc;

    // service required to create video processors
    IDirectXVideoProcessorService*  m_pDXVAVPS;
    //left channel processor
    IDirectXVideoProcessor*         m_pDXVAVP_Left; 
    // right channel processor
    IDirectXVideoProcessor*         m_pDXVAVP_Right;
    
    // target rectangle
    RECT                            m_targetRect;

    bool                            m_overlaySupported;

    // various structures for DXVA2 calls
    DXVA2_VideoDesc                 m_VideoDesc;
    DXVA2_VideoProcessBltParams     m_BltParams; 
    DXVA2_VideoSample               m_Sample;
};


#endif // __DECODE_D3D_RENDER_H__