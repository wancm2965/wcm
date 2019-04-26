//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//
#include <winsock2.h>

#include <tchar.h>
#include <windows.h>

#include "pipeline_decode.h"
#include "d3d_allocator.h"
#include "sysmem_allocator.h"

#include "mfxplugin.h"


#pragma warning(disable : 4100)

mfxStatus CDecodingPipeline::InitMfxParams(unsigned char*pData,int nLen)
{
	if (m_bDecodedHeader)
	{
		return MFX_ERR_NONE;
	}

	MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_NULL_PTR);
	mfxStatus sts = MFX_ERR_NONE;

	sts = ReadNextFrame(&m_mfxBS,pData,nLen);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	sts = m_pmfxDEC->DecodeHeader(&m_mfxBS, &m_mfxVideoParams);
	if (sts==MFX_ERR_NONE)
	{
		m_bDecodedHeader=true;	

		if ( m_mfxBS.PicStruct == MFX_PICSTRUCT_FIELD_TFF || m_mfxBS.PicStruct == MFX_PICSTRUCT_FIELD_BFF)
		{
			m_mfxVideoParams.mfx.FrameInfo.CropH *= 2;
			m_mfxVideoParams.mfx.FrameInfo.Height = MSDK_ALIGN16(m_mfxVideoParams.mfx.FrameInfo.CropH);
			m_mfxVideoParams.mfx.FrameInfo.PicStruct = m_mfxBS.PicStruct;
		}
	}

	MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	if (m_pD3DRender11)
	{
		m_mfxVideoParams.IOPattern = (mfxU16)(MFX_IOPATTERN_OUT_SYSTEM_MEMORY);
	}
	else
	{
		m_mfxVideoParams.IOPattern = (mfxU16)(MFX_IOPATTERN_OUT_VIDEO_MEMORY);
	}

	//reduce memory usage by allocation less surfaces
	m_mfxVideoParams.AsyncDepth = 2;

	return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::CreateHWDevice(YNCD3D_CB_DC cbdc,void*pUserData,int nWidth,int nHeight)
{  
	if (m_pD3DRender9 || m_pD3DRender11)
	{
		return MFX_ERR_NONE;
	}

	mfxStatus sts = MFX_ERR_NONE;
	NEWRECORD( m_pD3DRender9 , new CD3DRender(cbdc,pUserData) )
	if (NULL == m_pD3DRender9)
		return MFX_ERR_MEMORY_ALLOC;

	bool bD3D11=false;
#if MFX_D3D11_DECODE_SUPPORT

	//if (FRE_GetD3D11Decoder() > 0)
	{
		bD3D11=true;
	}	
#endif

	if (!IsWin8() || FRE_GetD3D11Total() == 0/* GetD3D11Count() >= FRE_GetD3D11Total() || nWidth < 1280 || nHeight < 720*/)
	{
		bD3D11 = false;
	}

	if (bD3D11)
	{
		//if (FRE_GetD3D11Decoder()==2)
		mfxIMPL impl = GetIntelAdapterIMPL();
		if (impl != 2)
		{
			NEWRECORD( m_pD3DRender11 , new CD3DRender(cbdc,pUserData) )
			if (NULL == m_pD3DRender11)
				return MFX_ERR_MEMORY_ALLOC;

			sts = m_pD3DRender11->Init(m_hWndVideo,nWidth, nHeight,true,false,false,true);
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			if(m_pD3DRender11->IsD3D11()==false)
			{
				delete m_pD3DRender11;
				m_pD3DRender11=NULL;

				sts = m_pD3DRender9->Init(m_hWndVideo,nWidth, nHeight,false,false,false,true);
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);			
			}
			else
			{
				delete m_pD3DRender9;
				m_pD3DRender9=NULL;
			}
		}
		else
		{
			sts = m_pD3DRender9->Init(m_hWndVideo,nWidth, nHeight,true,false,true,true);
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		}
	}
 	else
 	{
 		sts = m_pD3DRender9->Init(m_hWndVideo,nWidth, nHeight,false,false,false,true);
 		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
 	}

	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::ResetDevice()
{
	return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::AllocFrames()
{
	MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_NULL_PTR);

	mfxStatus sts = MFX_ERR_NONE;

	mfxFrameAllocRequest Request;

	mfxU16 nSurfNum = 0; // number of surfaces for decoder

	MSDK_ZERO_MEMORY(Request);

	sts = m_pmfxDEC->Query(&m_mfxVideoParams, &m_mfxVideoParams);
	MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM); 
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	// calculate number of surfaces required for decoder
	sts = m_pmfxDEC->QueryIOSurf(&m_mfxVideoParams, &Request);
	MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	nSurfNum = MSDK_MAX(Request.NumFrameSuggested, 1);

	// prepare allocation request
	Request.NumFrameMin = nSurfNum;
	Request.NumFrameSuggested = nSurfNum;
	//memcpy(&(Request.Info), &(m_mfxVideoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));


	//Request.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE; 

	// add info about memory type to request 
	//Request.Type |= m_pD3DRender11 ? MFX_MEMTYPE_SYSTEM_MEMORY:MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET  ; 

	// alloc frames for decoder
	sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &Request, &m_mfxResponse);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// prepare mfxFrameSurface1 array for decoder
	nSurfNum = m_mfxResponse.NumFrameActual;
	NEWRECORD( m_pmfxSurfaces , new mfxFrameSurface1 [nSurfNum] )
	MSDK_CHECK_POINTER(m_pmfxSurfaces, MFX_ERR_MEMORY_ALLOC);       

	for (int i = 0; i < nSurfNum; i++)
	{       
		memset(&(m_pmfxSurfaces[i]), 0, sizeof(mfxFrameSurface1));
		memcpy(&(m_pmfxSurfaces[i].Info), &(m_mfxVideoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));


		if (!m_pD3DRender11)
		{
			m_pmfxSurfaces[i].Data.MemId = m_mfxResponse.mids[i];    
		}
		else
		{
			sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, m_mfxResponse.mids[i], &(m_pmfxSurfaces[i].Data));
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		}
	}  

	return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::CreateAllocator()
{   
	mfxStatus sts = MFX_ERR_NONE;

	if (m_pD3DRender9==NULL && m_pD3DRender11==NULL)
	{
		return MFX_ERR_UNKNOWN;
	}

	if (m_pMFXAllocator)
	{
		return MFX_ERR_NONE;		
	}

	if (m_pD3DRender11 == NULL)
	{
		mfxHDL hdl = NULL;
		{

			CHWDevice* hwdev = (CHWDevice*)m_pD3DRender9->GetDeviceHandle();
			if (hwdev)
			{
				mfxHandleType hdl_t = MFX_HANDLE_D3D9_DEVICE_MANAGER;
#if MFX_D3D11_DECODE_SUPPORT
				if (m_pD3DRender9->IsD3D11())
				{
					hdl_t = MFX_HANDLE_D3D11_DEVICE;
				}
#endif // #if MFX_D3D11_DECODE_SUPPORT

				sts = hwdev->GetHandle(hdl_t, &hdl);
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
				sts = m_mfxSession.SetHandle(hdl_t, hdl);
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			}				
		}		

		// create D3D allocator
		if (m_pmfxAllocatorParams ==  NULL)
		{
#if MFX_D3D11_DECODE_SUPPORT
			if (m_pD3DRender9 && m_pD3DRender9->IsD3D11())
			{
				NEWRECORD( m_pMFXAllocator , new D3D11FrameAllocator )
				NEWRECORD3( D3D11AllocatorParams *,pd3dAllocParams , new D3D11AllocatorParams )
				MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
				pd3dAllocParams->pDevice = reinterpret_cast<ID3D11Device *>(hdl);
				m_pmfxAllocatorParams = pd3dAllocParams;
			}
			else
			{
				NEWRECORD( m_pMFXAllocator , new D3DFrameAllocator )
				NEWRECORD3( D3DAllocatorParams *,pd3dAllocParams , new D3DAllocatorParams )
				MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
				pd3dAllocParams->pManager = reinterpret_cast<IDirect3DDeviceManager9 *>(hdl);
				m_pmfxAllocatorParams = pd3dAllocParams;
			}
#else 
			NEWRECORD3( D3DAllocatorParams *,pd3dAllocParams , new D3DAllocatorParams )
			MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
			pd3dAllocParams->pManager = reinterpret_cast<IDirect3DDeviceManager9 *>(hdl);
			m_pmfxAllocatorParams = pd3dAllocParams;
#endif // #if MFX_D3D11_DECODE_SUPPORT
		}
		sts = m_mfxSession.SetFrameAllocator(m_pMFXAllocator);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}
	else
	{
		// create system memory allocator       
		NEWRECORD( m_pMFXAllocator , new SysMemFrameAllocator )
		MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

	}


	//if (!m_pmfxAllocatorParams)
	//{
	//	mfxAllocatorParams* allocatorParams = new mfxAllocatorParams;
	//	MSDK_CHECK_POINTER(allocatorParams, MFX_ERR_MEMORY_ALLOC);
	//	m_pmfxAllocatorParams = allocatorParams;
	//}
	// initialize general allocator
	sts = m_pMFXAllocator->Init(m_pmfxAllocatorParams);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return MFX_ERR_NONE;
}

void CDecodingPipeline::DeleteFrames()
{
	// delete surfaces array
	MSDK_SAFE_DELETE_ARRAY(m_pmfxSurfaces);    

	// delete frames
	if (m_pMFXAllocator)
	{        
		m_pMFXAllocator->Free(m_pMFXAllocator->pthis, &m_mfxResponse);
	}

	return;
}

void CDecodingPipeline::DeleteAllocator()
{    
	// delete allocator
	MSDK_SAFE_DELETE(m_pMFXAllocator);   
	MSDK_SAFE_DELETE(m_pmfxAllocatorParams);

	MSDK_SAFE_DELETE(m_pD3DRender9);
	MSDK_SAFE_DELETE(m_pD3DRender11);
}

CDecodingPipeline::CDecodingPipeline(bool bHardware)
:m_bHardware(bHardware)
{
	m_bDecodedHeader=false;
	m_bInited=false;
	m_nWidth=0;
	m_nHeight=0;

	m_hWndVideo=NULL;

	m_pmfxDEC = NULL;
	m_pMFXAllocator = NULL;
	m_pmfxAllocatorParams = NULL;

	m_pmfxSurfaces = NULL; 

	m_pD3DRender9 = NULL;
	m_pD3DRender11=NULL;	

	MSDK_ZERO_MEMORY(m_mfxVideoParams);
	MSDK_ZERO_MEMORY(m_mfxResponse);
	MSDK_ZERO_MEMORY(m_mfxBS);
}

CDecodingPipeline::~CDecodingPipeline()
{
}

int g_nDeCount =0;

int CDecodingPipeline::Open(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWndVideo,int nWidth,int nHeight)
{
	m_hWndVideo=hWndVideo;

	mfxStatus sts = MFX_ERR_NONE;

	// init session
	mfxIMPL impl = MFX_IMPL_SOFTWARE;
	if (m_bHardware)
	{
		impl = GetIntelAdapterIMPL();
	}
	// API version
	mfxVersion ver10 = {0, 1};
	mfxVersion version;
	version = ver10; 

	sts = CreateHWDevice(cbdc,pUserData,nWidth,nHeight);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

#if MFX_D3D11_DECODE_SUPPORT
	if (m_pD3DRender9 && m_pD3DRender9->IsD3D11())
	{
		impl |= MFX_IMPL_VIA_D3D11;
	}	
#endif

	sts = m_mfxSession.Init(impl, &version);
	
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	sts = MFXQueryVersion(m_mfxSession , &version); // get real API version of the loaded library
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// create decoder
	NEWRECORD( m_pmfxDEC , new MFXVideoDECODE(m_mfxSession) )
	MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_MEMORY_ALLOC);    

	// set video type in parameters
	m_mfxVideoParams.mfx.CodecId = MFX_CODEC_AVC; 

	//¼ÓÔØH265½âÂë²å¼þ
	if (MFX_CODEC_HEVC == m_mfxVideoParams.mfx.CodecId)
	{
		sts = MFXVideoUSER_Load(m_mfxSession, &MFX_PLUGINID_HEVCD_HW, 1);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}

	// prepare bit stream
	sts = InitMfxBitstream(&m_mfxBS, 1024 * 1024);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, -1);    

	// init allocator 
	sts = CreateAllocator();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, -1);


	return 0;
}

mfxStatus CDecodingPipeline::Init(unsigned char*pData,int nLen)
{
	mfxStatus sts = MFX_ERR_NONE;
	sts = InitMfxParams(pData,nLen);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// if allocator is provided to MediaSDK as external, frames must be allocated prior to decoder initialization
	sts = AllocFrames();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// init decoder
	sts = m_pmfxDEC->Init(&m_mfxVideoParams);
	MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return MFX_ERR_NONE;
}

void CDecodingPipeline::Close()
{
	g_nDeCount =0;

	WipeMfxBitstream(&m_mfxBS);
	MSDK_SAFE_DELETE(m_pmfxDEC);   

	DeleteFrames();

	// allocator if used as external for MediaSDK must be deleted after decoder
	DeleteAllocator();

	m_mfxSession.Close();
}

int CDecodingPipeline::PlayFrame(const unsigned char*pFrameData, int nFrameLen,int nFrameRate,int&nFlagReset)
{
	mfxStatus sts = MFX_ERR_NONE;
	sts=RunDecoding((unsigned char*)pFrameData,nFrameLen,nFrameRate,nFlagReset);

	if (sts!=MFX_ERR_NONE)
	{
		nFlagReset=2;
	}

	return (sts==MFX_ERR_NONE?0:-1);
}

int CDecodingPipeline::GetActualWidth(void)
{
	return m_nWidth;
}

int CDecodingPipeline::GetActualHeight(void)
{
	return m_nHeight;
}

int CDecodingPipeline::Resize(void)
{
	if (m_pD3DRender11)
	{
		m_pD3DRender11->Resize();
	}
	else if (m_pD3DRender9)
	{
		m_pD3DRender9->Resize();
	}
	return 0;
}

int CDecodingPipeline::SkipDisplay(int nFlagSkipDisplay)
{
	if (m_pD3DRender11)
	{
		m_pD3DRender11->SkipDisplay(nFlagSkipDisplay);
	}
	else if (m_pD3DRender9)
	{
		m_pD3DRender9->SkipDisplay(nFlagSkipDisplay);
	}
	return 0;
}
int CDecodingPipeline::GetFrameRate(void)
{
	if (m_pD3DRender11)
	{
		return m_pD3DRender11->GetFrameRate();
	}
	else if (m_pD3DRender9)
	{
		return m_pD3DRender9->GetFrameRate();
	}
	return 0;
}

bool CDecodingPipeline::CaptureToBMPFile(const char*cszFile)
{
	if (m_pD3DRender11)
	{
		return m_pD3DRender11->CaptureToBMPFile(cszFile);
	}
	else if (m_pD3DRender9)
	{
		return m_pD3DRender9->CaptureToBMPFile(cszFile);
	}
	return false;
}

int CDecodingPipeline::GetShowType(void)
{
	if (m_pD3DRender11)
	{
		return m_pD3DRender11->GetShowType();
	}
	else if (m_pD3DRender9)
	{
		return m_pD3DRender9->GetShowType();
	}
	return 0;
}

mfxStatus CDecodingPipeline::ResetDecoder(unsigned char*pData,int nLen)
{
	mfxStatus sts = MFX_ERR_NONE;    

	// close decoder
	sts = m_pmfxDEC->Close();
	MSDK_IGNORE_MFX_STS(sts, MFX_ERR_NOT_INITIALIZED);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// FREEMSG allocated frames
	DeleteFrames();

	// initialize parameters with values from parsed header 
	sts = InitMfxParams(pData,nLen);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// allocate frames prior to decoder initialization (if allocator used as external)
	sts = AllocFrames();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// init decoder
	sts = m_pmfxDEC->Init(&m_mfxVideoParams);
	MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::RunDecoding(unsigned char*pData,int nLen,int nFrameRate,int&nFlagReset)
{   
	mfxSyncPoint        syncp;
	mfxFrameSurface1    *pmfxOutSurface = NULL;
	mfxStatus           sts = MFX_ERR_NONE;
	mfxU16              nIndex = 0; // index of FREEMSG surface  

	if(g_nDeCount <=10 )
	{
		DebugOutput( logharddec ,("VIDECYNC_Decoding \n") );
		g_nDeCount ++;
	}

	if (!m_bInited)
	{
		sts=Init(pData,nLen);
		if (sts == MFX_ERR_NONE)
		{
			m_bInited=true;
		}
	}
	else
	{
		sts = ReadNextFrame(&m_mfxBS,pData,nLen); // read more data to input bit stream
	}
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_NULL_PTR);

	//DebugOutput("DecodeFrameAsync 0\n");
	while (m_mfxBS.DataLength>0)         
	{
		if (MFX_ERR_MORE_DATA == sts)
		{
			break;
		}

		if (MFX_ERR_MORE_SURFACE == sts || MFX_ERR_NONE == sts)
		{
			nIndex = GetFreeSurfaceIndex(m_pmfxSurfaces, m_mfxResponse.NumFrameActual); // find new working surface 
			if (MSDK_INVALID_SURF_IDX == nIndex)
			{
				return MFX_ERR_MEMORY_ALLOC;            
			}
		}

		pmfxOutSurface = NULL;

		//DebugOutput("DecodeFrameAsync 1\n");
		sts = m_pmfxDEC->DecodeFrameAsync(&m_mfxBS, &(m_pmfxSurfaces[nIndex]), &pmfxOutSurface, &syncp);
		//DebugOutput("DecodeFrameAsync 2 :%d\n",sts);

		if(pmfxOutSurface ==NULL)
		{
			//DebugOutput("DecodeFrameAsync 2121 break err code:%d\n",sts);
			break;
		}

		// ignore warnings if output is available, 
		// if no output and no action required just repeat the same call
		if (MFX_ERR_NONE < sts && syncp) 
		{
			sts = MFX_ERR_NONE;
		}

		if (MFX_ERR_NONE == sts)
		{
			sts = m_mfxSession.SyncOperation(syncp, MSDK_DEC_WAIT_INTERVAL);             
		}          
		//DebugOutput("DecodeFrameAsync 3\n");

		if (MFX_ERR_NONE == sts)
		{
			//DebugOutput("DecodeFrameAsync 30\n");
			MSDK_CHECK_POINTER(pmfxOutSurface, MFX_ERR_NULL_PTR);

			//DebugOutput("DecodeFrameAsync 31\n");
			if (m_pD3DRender11)
			{				
				//sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
				if (MFX_ERR_NONE == sts) {
					//DebugOutput("DecodeFrameAsync 4\n");

					sts=m_pD3DRender11->DrawSurface(pmfxOutSurface, m_pMFXAllocator,nFlagReset);
					//DebugOutput("DecodeFrameAsync 5\n");

				}
				//sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
				if (sts == MFX_ERR_NULL_PTR)
					sts = MFX_ERR_NONE;
				MSDK_BREAK_ON_ERROR(sts);
			}
			else if (m_pD3DRender9)
			{
				//DebugOutput("m_pD3DRender9->RenderFrame 1\n");
				sts=m_pD3DRender9->RenderFrame(pmfxOutSurface, m_pMFXAllocator,nFrameRate,nFlagReset);
				//DebugOutput("m_pD3DRender9->RenderFrame 2\n");
				if (sts == MFX_ERR_NULL_PTR)
					sts = MFX_ERR_NONE;
				MSDK_BREAK_ON_ERROR(sts);			
			}
			m_nWidth=pmfxOutSurface->Info.CropW;
			m_nHeight=pmfxOutSurface->Info.CropH;			
		}

	} //while processing    

	//DebugOutput("DecodeFrameAsync 33\n");
	// means that file has ended, need to go to buffering loop
	MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
	// incompatible video parameters detected, 
	//need to go to the buffering loop prior to reset procedure 
	MSDK_IGNORE_MFX_STS(sts, MFX_ERR_INCOMPATIBLE_VIDEO_PARAM); 
	//DebugOutput("DecodeFrameAsync 6\n");

	int nBusy=0;
	// loop to retrieve the buffered decoded frames
	while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_SURFACE == sts)        
	{ 
		if (MFX_WRN_DEVICE_BUSY == sts)
		{
			if (nBusy == 1)
			{
				DebugOutput( logcoreexcep , ("Decode busy skip this frame.\n"));
				break;
			}
 			DebugOutput( logcoreexcep ,("Decode busy Sleep(1).\n"));
 			Sleep(1);
 			nBusy=1;
		}
		//DebugOutput("DecodeFrameAsync 7\n");

		mfxU16 nIndex = GetFreeSurfaceIndex(m_pmfxSurfaces, m_mfxResponse.NumFrameActual);

		if (MSDK_INVALID_SURF_IDX == nIndex)
		{
			return MFX_ERR_MEMORY_ALLOC;            
		}
		int nDecodeCycleCount = 0;
		do {
			sts = m_pmfxDEC->DecodeFrameAsync(NULL, &(m_pmfxSurfaces[nIndex]), &pmfxOutSurface, &syncp);
			//if (sts != MFX_ERR_NONE/* && sts != MFX_ERR_MORE_DATA*/)
			//{
			//	DebugOutput("***************************************\n m_pmfxDEC->DecodeFrameAsync sts = %d nDecodeCycleCount=%d\n**********************************\n",sts,nDecodeCycleCount);
			//}
			//DebugOutput("DecodeFrameAsync 8\n");

			if (MFX_WRN_DEVICE_BUSY == sts) {
				mfxStatus _sts = m_mfxSession.SyncOperation(syncp, MSDK_DEC_WAIT_INTERVAL);
				// note: everything except MFX_ERR_NONE are errors at this point
				if (MFX_ERR_NONE == _sts) {
					sts = MFX_WRN_DEVICE_BUSY;
				} else {
					sts = _sts;
					if (MFX_ERR_MORE_DATA == sts) {
						// we can't receive MFX_ERR_MORE_DATA and have no output - that's a bug
						sts = MFX_WRN_DEVICE_BUSY;//MFX_ERR_NOT_FOUND;
					}
				}
			}
			nDecodeCycleCount++;

			if(pmfxOutSurface ==NULL)
			{
				//DebugOutput("DecodeFrameAsync 2121 break err code:%d\n",sts);
				break;
			}
		} while (MFX_WRN_DEVICE_BUSY == sts && nDecodeCycleCount < 100);

		// ignore warnings if output is available, 
		// if no output and no action required just repeat the same call        
		if (MFX_ERR_NONE < sts && syncp) 
		{
			sts = MFX_ERR_NONE;
		}

		if (MFX_ERR_NONE == sts)
		{
			sts = m_mfxSession.SyncOperation(syncp, MSDK_DEC_WAIT_INTERVAL);
		}
		//DebugOutput("DecodeFrameAsync 9\n");

		if (MFX_ERR_NONE == sts)
		{
			MSDK_CHECK_POINTER(pmfxOutSurface, MFX_ERR_NULL_PTR);

			if (m_pD3DRender11)
			{
				//sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
				if (MFX_ERR_NONE == sts) {
					sts=m_pD3DRender11->DrawSurface(pmfxOutSurface, m_pMFXAllocator,nFlagReset);
				}
				//sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
				if (sts == MFX_ERR_NULL_PTR)
					sts = MFX_ERR_NONE;
				MSDK_BREAK_ON_ERROR(sts);
			}
			else if (m_pD3DRender9)
			{
				//DebugOutput("DecodeFrameAsync 10\n");

				sts=m_pD3DRender9->RenderFrame(pmfxOutSurface, m_pMFXAllocator,nFrameRate,nFlagReset);
				if (sts == MFX_ERR_NULL_PTR)
					sts = MFX_ERR_NONE;
				MSDK_BREAK_ON_ERROR(sts);			
			}

			m_nWidth=pmfxOutSurface->Info.CropW;
			m_nHeight=pmfxOutSurface->Info.CropH;			
			//DebugOutput("DecodeFrameAsync 11\n");

			break;
		}
	} 

	// MFX_ERR_MORE_DATA is the correct status to exit buffering loop with
	MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
	// exit in case of other errors
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return sts; // ERR_NONE or ERR_INCOMPATIBLE_VIDEO_PARAM
}


mfxStatus CDecodingPipeline::ReadNextFrame(mfxBitstream *pBS,unsigned char*pData,int nLen)
{
	MSDK_CHECK_POINTER(pBS, MFX_ERR_NULL_PTR);
	pBS->DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;

	pBS->DataOffset=0;

	memcpy(pBS->Data+pBS->DataOffset, pData , nLen);
	pBS->DataLength += nLen;

	return MFX_ERR_NONE;
}