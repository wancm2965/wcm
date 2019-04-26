//IntelDecoderRenderer.h

#ifndef __INTEL_DECODER_RENDERER_H__
#define __INTEL_DECODER_RENDERER_H__

#include "mfx_samples_config.h"

#include "sample_defs.h"
#include "mfx_plugin_uids.h"

#if D3D_SURFACES_SUPPORT
#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#endif

#include <vector>
#include "d3d_device.h"
#include "mfx_buffering.h"
#include <memory>

#include "sample_defs.h"
#include "sample_utils.h"
#include "base_allocator.h"
#include "general_allocator.h"

#include "mfxplugin.h"
#include "mfxplugin++.h"
#include "mfxvideo.h"
#include "mfxvideo++.h"

#include "HPAVC_Player.h"


#ifndef MEM_TYPE
#define MEM_TYPE
enum MemType {
    SYSTEM_MEMORY = 0x00,
    D3D9_MEMORY   = 0x01,
    D3D11_MEMORY  = 0x02,
};
#endif

enum eWorkMode {
  MODE_PERFORMANCE,
  MODE_RENDERING,
  MODE_FILE_DUMP
};


struct CPipelineStatistics
{
    CPipelineStatistics():
        m_input_count(0),
        m_output_count(0),
        m_synced_count(0),
        m_tick_overall(0),
        m_tick_fread(0),
        m_tick_fwrite(0),
        m_timer_overall(m_tick_overall)
    {
    }
    virtual ~CPipelineStatistics(){}

    mfxU32 m_input_count;     // number of received incoming packets (frames or bitstreams)
    mfxU32 m_output_count;    // number of delivered outgoing packets (frames or bitstreams)
    mfxU32 m_synced_count;

    msdk_tick m_tick_overall; // overall time passed during processing
    msdk_tick m_tick_fread;   // part of tick_overall: time spent to receive incoming data
    msdk_tick m_tick_fwrite;  // part of tick_overall: time spent to deliver outgoing data

    CAutoTimer m_timer_overall; // timer which corresponds to m_tick_overall

private:
    CPipelineStatistics(const CPipelineStatistics&);
    void operator=(const CPipelineStatistics&);
};

class IntelDecoderRenderer
	: public CBuffering
    , public CPipelineStatistics
	, public CD3D9DeviceCallback

{
public:
    IntelDecoderRenderer(HPAVC_PlayerCallback&rCallback);
    virtual ~IntelDecoderRenderer();
public:
	virtual int Open(HWND hWnd);
	virtual void Close();

	virtual int DecodeFrame(unsigned char*pFrameData,int nLen);
	virtual int GetWidth(void);
	virtual int GetHeight(void);
	virtual int SetAMP(int nBrightness,int nHue,int nSaturation,int nContrast);

    virtual mfxStatus Init(unsigned char*pData,int nLen);
    virtual mfxStatus RunDecoding(unsigned char*pData,int nLen);
    virtual mfxStatus ResetDecoder(void);
    virtual mfxStatus ResetDevice();
protected: // functions
	virtual mfxStatus ReadNextFrame(mfxBitstream *pBS,unsigned char*pData,int nLen);

    virtual mfxStatus InitMfxParams(mfxU16 nAsyncDepth,unsigned char*pData,int nLen);
    virtual mfxStatus InitVppParams();
    virtual mfxStatus AllocAndInitVppDoNotUse();


    virtual mfxStatus CreateAllocator();
    virtual mfxStatus CreateHWDevice();
    virtual mfxStatus AllocFrames();
    virtual void DeleteFrames();
    virtual void DeleteAllocator();

    /** \brief Performs SyncOperation on the current output surface with the specified timeout.
     *
     * @return MFX_ERR_NONE Output surface was successfully synced and delivered.
     * @return MFX_ERR_MORE_DATA Array of output surfaces is empty, need to feed decoder.
     * @return MFX_WRN_IN_EXECUTION Specified timeout have elapsed.
     * @return MFX_ERR_UNKNOWN An error has occurred.
     */
    virtual mfxStatus SyncOutputSurface(mfxU32 wait);
    virtual mfxStatus DeliverOutput(mfxFrameSurface1* frame);

    virtual mfxStatus DeliverLoop(void);

    static unsigned int MSDK_THREAD_CALLCONVENTION DeliverThreadFunc(void* ctx);
	
	virtual void OnCD3D9DeviceCallbackDraw(HDC hDC);
protected: // variables
	mfxBitstream            m_mfxBS; // contains encoded data

    MFXVideoSession         m_mfxSession;
    MFXVideoDECODE*         m_pmfxDEC;
    MFXVideoVPP*            m_pmfxVPP;
    mfxVideoParam           m_mfxVideoParams;
    mfxVideoParam           m_mfxVppVideoParams;

	std::auto_ptr<MFXVideoUSER>  m_pUserModule;
	std::auto_ptr<MFXPlugin> m_pPlugin;
	const msdkPluginUID*     m_pUID;

    GeneralAllocator*       m_pGeneralAllocator;
    mfxAllocatorParams*     m_pmfxAllocatorParams;
    MemType                 m_memType;      // memory type of surfaces to use
    bool                    m_bExternalAlloc; // use memory allocator as external for Media SDK
    bool                    m_bSysmemBetween; // use system memory between Decoder and VPP, if false - video memory

    mfxFrameAllocResponse   m_mfxResponse;      // memory allocation response for decoder
    mfxFrameAllocResponse   m_mfxVppResponse;   // memory allocation response for vpp

    msdkFrameSurface*       m_pCurrentFreeSurface; // surface detached from free surfaces array
    msdkFrameSurface*       m_pCurrentFreeVppSurface; // VPP surface detached from free VPP surfaces array
    msdkOutputSurface*      m_pCurrentFreeOutputSurface; // surface detached from free output surfaces array
    msdkOutputSurface*      m_pCurrentOutputSurface; // surface detached from output surfaces array

    MSDKSemaphore*          m_pDeliverOutputSemaphore; // to access to DeliverOutput method
    MSDKEvent*              m_pDeliveredEvent; // to signal when output surfaces will be processed
    mfxStatus               m_error; // error returned by DeliverOutput method

    eWorkMode               m_eWorkMode; // work mode for the pipeline
    bool                    m_bStopDeliverLoop;
    mfxU32                  m_fourcc; // color format of vpp out, i420 by default

    mfxExtVPPDoNotUse       m_VppDoNotUse;      // for disabling VPP algorithms
    mfxExtBuffer*           m_VppExtParams[2];
	mfxExtVPPProcAmp		m_VppProcAmp;

    CHWDevice               *m_hwdev;

	mfxU16					m_nAsyncDepth;
	bool					m_bInitialized;
	HWND					m_hWnd;
	MSDKThread *			m_pDeliverThread;

	int						m_nBrightness;
	int						m_nHue;
	int						m_nSaturation;
	int						m_nContrast;

	mfxU32					m_nVideoCodecType;

	HPAVC_PlayerCallback&	m_rCallback;
private:
    IntelDecoderRenderer(const IntelDecoderRenderer&);
    void operator=(const IntelDecoderRenderer&);
};

#endif // __PIPELINE_DECODE_H__
