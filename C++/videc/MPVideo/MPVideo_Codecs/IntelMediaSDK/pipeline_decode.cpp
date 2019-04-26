//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//

#include <tchar.h>
#include <windows.h>

#include "pipeline_decode.h"
#include "sysmem_allocator.h"
#include "d3d_allocator.h"
#include "XMemcpy.h"

#pragma warning(disable : 4100)

mfxStatus CDecodingPipeline::InitMfxParams(unsigned char*pData,int nLen)
{
	MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_NULL_PTR);
	mfxStatus sts = MFX_ERR_NONE;

	// try to find a sequence header in the stream
	// if header is not found this function exits with error (e.g. if device was lost and there's no header in the remaining stream)
	do
	{
		// parse bit stream and fill mfx params
		sts = m_pmfxDEC->DecodeHeader(&m_mfxBS, &m_mfxVideoParams);

		if (MFX_ERR_MORE_DATA == sts)
		{
			if (m_mfxBS.MaxLength == m_mfxBS.DataLength)
			{
				sts = ExtendMfxBitstream(&m_mfxBS, m_mfxBS.MaxLength * 2); 
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			}
			// read a portion of data             
			sts = ReadNextFrame(&m_mfxBS,pData,nLen);
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

			continue;
		}
		else
		{
			m_bDecodedHeader=true;
			m_nWidth=m_mfxVideoParams.mfx.FrameInfo.Width;
			m_nHeight=m_mfxVideoParams.mfx.FrameInfo.Height;

			if (m_pNV12Data)
			{
				_aligned_free(m_pNV12Data);
				m_pNV12Data=NULL;
				m_pUV=NULL;
			}

			// if input is interlaced JPEG stream
			if ( m_mfxBS.PicStruct == MFX_PICSTRUCT_FIELD_TFF || m_mfxBS.PicStruct == MFX_PICSTRUCT_FIELD_BFF)
			{
				m_mfxVideoParams.mfx.FrameInfo.CropH *= 2;
				m_mfxVideoParams.mfx.FrameInfo.Height = MSDK_ALIGN16(m_mfxVideoParams.mfx.FrameInfo.CropH);
				m_mfxVideoParams.mfx.FrameInfo.PicStruct = m_mfxBS.PicStruct;
			}

			break;
		}

		break;
	}while (1);

	// check DecodeHeader status
	MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);


	// specify memory type 
	m_mfxVideoParams.IOPattern = (mfxU16)(m_bd3dAlloc ? MFX_IOPATTERN_OUT_VIDEO_MEMORY : MFX_IOPATTERN_OUT_SYSTEM_MEMORY);

	//reduce memory usage by allocation less surfaces
	m_mfxVideoParams.AsyncDepth = 1;

	return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::CreateHWDevice()
{  
	mfxStatus sts = MFX_ERR_NONE;

	HWND window = NULL;

	{
		POINT point = {0, 0};
		window = WindowFromPoint(point);
	}

	m_hwdev = new CD3D9Device();
	if (NULL == m_hwdev)
		return MFX_ERR_MEMORY_ALLOC;

	sts = m_hwdev->Init(window);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return MFX_ERR_NONE;
}

mfxStatus CDecodingPipeline::ResetDevice()
{
	return m_hwdev->Reset();
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
	memcpy(&(Request.Info), &(m_mfxVideoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
	Request.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE; 

	// add info about memory type to request 
	Request.Type |= m_bd3dAlloc ? MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET : MFX_MEMTYPE_SYSTEM_MEMORY; 
	// alloc frames for decoder
	sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &Request, &m_mfxResponse);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// prepare mfxFrameSurface1 array for decoder
	nSurfNum = m_mfxResponse.NumFrameActual;
	m_pmfxSurfaces = new mfxFrameSurface1 [nSurfNum];
	MSDK_CHECK_POINTER(m_pmfxSurfaces, MFX_ERR_MEMORY_ALLOC);       

	for (int i = 0; i < nSurfNum; i++)
	{       
		memset(&(m_pmfxSurfaces[i]), 0, sizeof(mfxFrameSurface1));
		memcpy(&(m_pmfxSurfaces[i].Info), &(m_mfxVideoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

		if (m_bExternalAlloc)
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

	if (m_bd3dAlloc)
	{    
		sts = CreateHWDevice();
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// provide device manager to MediaSDK
		IDirect3DDeviceManager9 *pd3dDeviceManager = NULL;
		sts = m_hwdev->GetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER, (mfxHDL*)&pd3dDeviceManager);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		sts = m_mfxSession.SetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER, pd3dDeviceManager);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);


		// crate D3D allocator
		m_pMFXAllocator = new D3DFrameAllocator;
		MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);
		D3DAllocatorParams *pd3dAllocParams = new D3DAllocatorParams;        
		MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);

		pd3dAllocParams->pManager = pd3dDeviceManager;
		m_pmfxAllocatorParams = pd3dAllocParams;

		/* In case of video memory we must provide MediaSDK with external allocator 
		thus we demonstrate "external allocator" usage model.
		Call SetAllocator to pass allocator to mediasdk */
		sts = m_mfxSession.SetFrameAllocator(m_pMFXAllocator);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		m_bExternalAlloc = true;  
	} 
	else
	{        
		// create system memory allocator       
		m_pMFXAllocator = new SysMemFrameAllocator;         
		MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

		/* In case of system memory we demonstrate "no external allocator" usage model.  
		We don't call SetAllocator, MediaSDK uses internal allocator. 
		We use system memory allocator simply as a memory manager for application*/           
	}   

	// initialize memory allocator
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

	MSDK_SAFE_DELETE(m_hwdev);
}

CDecodingPipeline::CDecodingPipeline(HWVideoDecoderCallback&rCallback)
:m_rCallback(rCallback)
{
	m_nFrameIndex = 0;
	m_pmfxDEC = NULL;
	m_pMFXAllocator = NULL;
	m_pmfxAllocatorParams = NULL;
	m_bd3dAlloc = false;
	m_bExternalAlloc = false;
	m_pmfxSurfaces = NULL; 
	m_bIsExtBuffers = false;
	m_ppExtBuffers = NULL;

	m_hwdev = NULL;

	m_bDecodedHeader=false;
	m_nWidth=0;
	m_nHeight=0;
	m_pNV12Data=NULL;
	m_pUV=NULL;
	m_nY=0;
	m_nUV=0;
	m_nStride=0;
	m_nNV12DataLen=0;

	MSDK_ZERO_MEMORY(m_mfxVideoParams);
	MSDK_ZERO_MEMORY(m_mfxResponse);
	MSDK_ZERO_MEMORY(m_mfxBS);
}

CDecodingPipeline::~CDecodingPipeline()
{
}

int CDecodingPipeline::Open(int nWidth,int nHeight)
{
	mfxStatus sts = MFX_ERR_NONE;
	sts=DoOpen();
	return (sts == MFX_ERR_NONE?0:-1);
}

mfxStatus CDecodingPipeline::DoOpen(void)
{
	mfxStatus sts = MFX_ERR_NONE;
	HRESULT hr = S_OK;


	// init session
	mfxIMPL impl = MFX_IMPL_HARDWARE;;//MFX_IMPL_SOFTWARE

	// API version
	mfxVersion ver10 = {0, 1};
	mfxVersion ver13 = {3, 1};    
	mfxVersion version;
	version = ver13; 

	sts = m_mfxSession.Init(impl, &version);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// create decoder
	m_pmfxDEC = new MFXVideoDECODE(m_mfxSession);
	MSDK_CHECK_POINTER(m_pmfxDEC, MFX_ERR_MEMORY_ALLOC);    

	// set video type in parameters
	m_mfxVideoParams.mfx.CodecId = MFX_CODEC_AVC; 
	// set memory type
	m_bd3dAlloc = true;


	// prepare bit stream
	sts = InitMfxBitstream(&m_mfxBS, 1024 * 1024);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);    

	// init allocator 
	sts = CreateAllocator();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return sts;
}

int CDecodingPipeline::Decode(const unsigned char*pData,int nLen)
{
	mfxStatus sts = MFX_ERR_NONE;
	sts=RunDecoding((unsigned char*)pData,nLen);
	return (sts==MFX_ERR_NONE?0:-1);
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

void CDecodingPipeline::AttachExtParam()
{
	m_mfxVideoParams.ExtParam = m_ppExtBuffers;     
	m_mfxVideoParams.NumExtParam = m_nNumExtBuffers; 
}

void CDecodingPipeline::DeleteExtBuffers()
{
	if (m_ppExtBuffers != NULL)
	{
		mfxU32 i;
		for (i = 0; i < m_nNumExtBuffers; ++i)
			if (m_ppExtBuffers[i] != NULL)
				MSDK_SAFE_DELETE(m_ppExtBuffers[i]);
	}

	MSDK_SAFE_DELETE_ARRAY(m_ppExtBuffers);
}


void CDecodingPipeline::Close()
{
	WipeMfxBitstream(&m_mfxBS);
	MSDK_SAFE_DELETE(m_pmfxDEC);   

	DeleteFrames();

	// allocator if used as external for MediaSDK must be deleted after decoder
	DeleteAllocator();

	if (m_bIsExtBuffers)
	{
		DeleteExtBuffers();
	}

	m_mfxSession.Close();

	if (m_pNV12Data)
	{
		_aligned_free(m_pNV12Data);
		m_pNV12Data=NULL;
		m_pUV=NULL;
	}

	return;
}

void CDecodingPipeline::FreeBuffer(void*pBuffer)
{
	if (pBuffer)
	{
		_aligned_free(pBuffer);
		pBuffer=NULL;
	}
}

mfxStatus CDecodingPipeline::ResetDecoder(unsigned char*pData,int nLen)
{
	mfxStatus sts = MFX_ERR_NONE;    

	// close decoder
	sts = m_pmfxDEC->Close();
	MSDK_IGNORE_MFX_STS(sts, MFX_ERR_NOT_INITIALIZED);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// free allocated frames
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

mfxStatus CDecodingPipeline::RunDecoding(unsigned char*pData,int nLen)
{   
	mfxSyncPoint        syncp;
	mfxFrameSurface1    *pmfxOutSurface = NULL;
	mfxStatus           sts = MFX_ERR_NONE;
	mfxU16              nIndex = 0; // index of free surface   

	if (!m_bDecodedHeader)
	{
		sts=Init(pData,nLen);
		if (!m_bDecodedHeader)
		{
			return sts;
		}
	}
	else
	{
		sts = ReadNextFrame(&m_mfxBS,pData,nLen); // read more data to input bit stream
	}


	while (m_mfxBS.DataLength>0)         
	{
		if (MFX_WRN_DEVICE_BUSY == sts)
		{
			Sleep(1); // just wait and then repeat the same call to DecodeFrameAsync
			continue;
		}


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

		sts = m_pmfxDEC->DecodeFrameAsync(&m_mfxBS, &(m_pmfxSurfaces[nIndex]), &pmfxOutSurface, &syncp);

		// ignore warnings if output is available, 
		// if no output and no action required just repeat the same call
		if (MFX_ERR_NONE < sts && syncp) 
		{
			sts = MFX_ERR_NONE;
		}

		if (MFX_ERR_NONE == sts)
		{
			sts = m_mfxSession.SyncOperation(syncp, 0xFFFF);  
		}          

		if (MFX_ERR_NONE == sts)
		{                
			//unsigned long nStart=timeGetTime();
			if (m_bExternalAlloc) 
			{
				sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
				MSDK_BREAK_ON_ERROR(sts);

				sts = WriteNextFrame(pmfxOutSurface);
				MSDK_BREAK_ON_ERROR(sts);

				sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
				MSDK_BREAK_ON_ERROR(sts);
			}
			else 
			{
				sts = WriteNextFrame(pmfxOutSurface);
				MSDK_BREAK_ON_ERROR(sts);
			}            
			//unsigned long nEnd=timeGetTime();
			//char szDebug[1024];
			//sprintf(szDebug,"1 nDuration=%u\n",nEnd-nStart);
			//OutputDebugString(szDebug);
		}
	}; //while processing    

	//save the main loop exit status (required for the case of ERR_INCOMPATIBLE_PARAMS)
	mfxStatus mainloop_sts = sts; 

	// means that file has ended, need to go to buffering loop
	MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
	// incompatible video parameters detected, 
	//need to go to the buffering loop prior to reset procedure 
	MSDK_IGNORE_MFX_STS(sts, MFX_ERR_INCOMPATIBLE_VIDEO_PARAM); 
	// exit in case of other errors
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);          

	// loop to retrieve the buffered decoded frames
	while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_SURFACE == sts)        
	{        
		//char szDebug[1024];
		//sprintf(szDebug,"2 nLen=%d %u\n",nLen,timeGetTime());
		//OutputDebugString(szDebug);

		if (MFX_WRN_DEVICE_BUSY == sts)
		{
			Sleep(1);
		}

		mfxU16 nIndex = GetFreeSurfaceIndex(m_pmfxSurfaces, m_mfxResponse.NumFrameActual);

		if (MSDK_INVALID_SURF_IDX == nIndex)
		{
			return MFX_ERR_MEMORY_ALLOC;            
		}

		sts = m_pmfxDEC->DecodeFrameAsync(NULL, &(m_pmfxSurfaces[nIndex]), &pmfxOutSurface, &syncp);

		// ignore warnings if output is available, 
		// if no output and no action required just repeat the same call        
		if (MFX_ERR_NONE < sts && syncp) 
		{
			sts = MFX_ERR_NONE;
		}

		if (MFX_ERR_NONE == sts)
		{
			sts = m_mfxSession.SyncOperation(syncp, 0xFFFF);
		}

		if (MFX_ERR_NONE == sts)
		{
			if (m_bExternalAlloc) 
			{
				sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
				MSDK_BREAK_ON_ERROR(sts);

				sts = WriteNextFrame(pmfxOutSurface);
				MSDK_BREAK_ON_ERROR(sts);

				sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
				MSDK_BREAK_ON_ERROR(sts);
			}
			else 
			{
				sts = WriteNextFrame(pmfxOutSurface);
				MSDK_BREAK_ON_ERROR(sts);
			}            
		}
	} 

	// MFX_ERR_MORE_DATA is the correct status to exit buffering loop with
	MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
	// exit in case of other errors
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// if we exited main decoding loop with ERR_INCOMPATIBLE_PARAM we need to send this status to caller
	if (MFX_ERR_INCOMPATIBLE_VIDEO_PARAM == mainloop_sts) 
	{
		sts = mainloop_sts; 
	}

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

#if (0)
__declspec(align(16)) static __int64 mask1 = 0x00ff00ff00ff00ff;
__declspec(align(16)) static __int64 mask2 = 0xff00ff00ff00ff00;

void NV12ToYV12_SSE2(unsigned char * y_out,
					 unsigned char * u_out, 
					 unsigned char * v_out, 
					 const unsigned char * const y_src, 
					 const unsigned char * const uv_src, 
					 const int width, 
					 const int height, 
					 const int stride)

{
	__asm {

		push ebx
		push ecx
		push esi
		push edi
		push ebp
		push eax
		push edx

		//put Y
		mov eax, height
		mov esi, y_src
		mov edi, y_out


		mov ecx, width
		mul	ecx
		shr eax, 2
		mov edx, eax

		mov eax, height

yloop1:
		mov ebx, width
		shr ebx,4

xloop1:
		movdqu xmm0, [esi]
		movdqu [edi], xmm0

		add esi, 16
		add edi, 16

		sub ebx, 1
		jnz xloop1

		mov	ecx, stride
		sub ecx, width
		add edi, ecx

		sub eax, 1
		jnz yloop1


		//Put UV
		movq mm6,mask2
		movq mm7,mask1
		movq2dq xmm6,mm6
		movq2dq xmm7,mm7
		punpcklqdq xmm6,xmm6 //mask U
		punpcklqdq xmm7,xmm7 //mask V
		//align 16

		mov	eax, height
		shr eax, 1
		mov esi, uv_src
		mov edi, u_out
		mov edx, v_out

yloop2:
		mov ebx, width
		shr ebx, 5

xloop2:
		movdqu xmm2, [esi]
		movdqu xmm3, [esi + 16]

		movdqu xmm0, xmm2
		movdqu xmm1, xmm3

		pand xmm2, xmm6
		pand xmm3, xmm6

		pand xmm0, xmm7
		pand xmm1, xmm7

		psrlw xmm2, 8
		psrlw xmm3, 8
		packuswb xmm2, xmm3
		movdqu [edx], xmm2

		;psrlw xmm0, 8
		;psrlw xmm1, 8

		packuswb xmm0, xmm1
		movdqu [edi], xmm0

		add esi, 32
		add edi, 16
		add edx, 16

		sub ebx, 1
		jnz xloop2

		mov ecx, stride
		sub ecx, width
		shr ecx, 1
		add edi, ecx
		add edx, ecx

		sub eax, 1
		jnz yloop2

		pop edx
		pop eax
		pop ebp
		pop edi
		pop esi
		pop ecx
		pop ebx
	}
}


void YV12ToNV12_SSE2(   unsigned char * const y_out,
					 unsigned char * const u_out,
					 unsigned char * const v_out,
					 const unsigned char * const src,
					 const int width,
					 const int height,
					 const int stride)
{
	__asm {
		push ebx
		push ecx
		push esi
		push edi
		push ebp
		push eax
		push edx

		//Put Y	
		mov eax, height
		mov esi, src
		mov edi, y_out

		mov ecx, width
		mul	ecx
		shr eax, 2
		mov edx, eax

		mov eax, height

yloop1:
		mov ebx, width
			shr ebx,4

xloop1:
		movdqu xmm0, [esi]
		movdqu [edi], xmm0

		add esi, 16
		add edi, 16

		sub ebx, 1
		jnz xloop1

		mov	ecx, stride
		sub ecx, width
		add edi, ecx

		sub eax, 1
		jnz yloop1


		//Put UV 
		mov	eax, height
		shr eax, 1
		mov	edi, u_out

yloop2:
		mov ebx, width
		shr ebx, 4

xloop2:
		movq mm0, [esi]
		movq mm1, [esi + edx]
		movq2dq xmm0, mm0
		movq2dq xmm1, mm1
		PUNPCKLBW xmm0, xmm1

		movdqu [edi], xmm0

		add esi, 8
		add edi, 16

		sub ebx, 1
		jnz xloop2

		mov ecx, stride
		sub ecx, width
		add edi, ecx

		sub eax, 1
		jnz yloop2

		pop edx
		pop eax
		pop ebp
		pop edi
		pop esi
		pop ecx
		pop ebx
	}
}

#endif


mfxStatus CDecodingPipeline::WriteNextFrame(mfxFrameSurface1 *pSurface)
{
	MSDK_CHECK_POINTER(pSurface,         MFX_ERR_NULL_PTR);

	mfxFrameInfo *pInfo = &pSurface->Info;
	mfxFrameData *pData = &pSurface->Data;
	MSDK_CHECK_POINTER(pData, MFX_ERR_NULL_PTR);

	if (pInfo->FourCC!=MFX_FOURCC_NV12)
	{
		return MFX_ERR_UNSUPPORTED;
	}

	m_nStride=pData->Pitch;
	if (m_pNV12Data==NULL)
	{
		m_nY=m_nStride*pInfo->CropH;
		m_nUV=m_nStride*pInfo->CropH/2;
		m_nNV12DataLen=m_nY+m_nUV;
		m_pNV12Data=(unsigned char*)_aligned_malloc(m_nNV12DataLen,128);
		m_pUV=m_pNV12Data+m_nY;
		MSDK_CHECK_POINTER(m_pNV12Data,MFX_ERR_NULL_PTR);
	}

	gpu_memcpy(m_pNV12Data,pData->Y,m_nY);
	gpu_memcpy(m_pUV,pData->UV,m_nUV);

	m_rCallback.OnHWVideoDecoderCallback(m_pNV12Data,m_nNV12DataLen,m_nWidth,m_nHeight,m_nStride);

	m_pNV12Data=NULL;
	m_pUV=NULL;

	return MFX_ERR_NONE;
}