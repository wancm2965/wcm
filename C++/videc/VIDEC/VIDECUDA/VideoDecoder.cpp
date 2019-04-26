/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */
 
#include "VideoDecoder.h"

#include <cstring>
#include <cassert>
#include <string>

#define MaximumSize 20

VideoDecoder::VideoDecoder(unsigned long nWidth,unsigned long nHeight,unsigned long nDispWidth,unsigned long nDispHeight, 
						   CUcontext &rContext, 
						   cudaVideoCreateFlags eCreateFlags, 
						   CUvideoctxlock &vidCtxLock) 
	: m_VidCtxLock(vidCtxLock)
{
    // get a copy of the CUDA context
    m_Context          = rContext;
    m_VideoCreateFlags = eCreateFlags;

	// Fill the decoder-create-info struct from the given video-format struct.
    memset(&oVideoDecodeCreateInfo_, 0, sizeof(CUVIDDECODECREATEINFO));
            // Create video decoder
    oVideoDecodeCreateInfo_.CodecType           = cudaVideoCodec_H264;
    oVideoDecodeCreateInfo_.ulWidth             = nWidth;
    oVideoDecodeCreateInfo_.ulHeight            = nHeight;
    oVideoDecodeCreateInfo_.ulNumDecodeSurfaces = MaximumSize;
            // Limit decode memory to 24MB (16M pixels at 4:2:0 = 24M bytes)
    while (oVideoDecodeCreateInfo_.ulNumDecodeSurfaces * nWidth * nHeight > 16*1024*1024)
    {
        oVideoDecodeCreateInfo_.ulNumDecodeSurfaces--;
    }
    oVideoDecodeCreateInfo_.ChromaFormat        = cudaVideoChromaFormat_420;
    oVideoDecodeCreateInfo_.OutputFormat        = cudaVideoSurfaceFormat_NV12;
	oVideoDecodeCreateInfo_.DeinterlaceMode     = cudaVideoDeinterlaceMode_Adaptive;

            // No scaling
    oVideoDecodeCreateInfo_.ulTargetWidth       = nDispWidth;
    oVideoDecodeCreateInfo_.ulTargetHeight      = nDispHeight;
    oVideoDecodeCreateInfo_.ulNumOutputSurfaces = MAX_FRAME_COUNT;  // We won't simultaneously map more than 8 surfaces
    oVideoDecodeCreateInfo_.ulCreationFlags     = m_VideoCreateFlags;
    oVideoDecodeCreateInfo_.vidLock             = vidCtxLock;
            // create the decoder
    CUresult oResult = cuvidCreateDecoder(&oDecoder_, &oVideoDecodeCreateInfo_);
    assert(CUDA_SUCCESS == oResult);
}

VideoDecoder::~VideoDecoder()
{ 
    cuvidDestroyDecoder(oDecoder_);
}

cudaVideoCodec
VideoDecoder::codec()
const
{
    return oVideoDecodeCreateInfo_.CodecType;
}

cudaVideoChromaFormat
VideoDecoder::chromaFormat()
const
{
    return oVideoDecodeCreateInfo_.ChromaFormat;
}

unsigned long
VideoDecoder::maxDecodeSurfaces()
const
{
    return oVideoDecodeCreateInfo_.ulNumDecodeSurfaces;
}

unsigned long 
VideoDecoder::frameWidth()
const
{
    return oVideoDecodeCreateInfo_.ulWidth;
}

unsigned long 
VideoDecoder::frameHeight()
const
{
    return oVideoDecodeCreateInfo_.ulHeight;
}

unsigned long
VideoDecoder::targetWidth()
const
{
    return oVideoDecodeCreateInfo_.ulTargetWidth;
}

unsigned long
VideoDecoder::targetHeight()
const
{
    return oVideoDecodeCreateInfo_.ulTargetHeight;
}

void
VideoDecoder::decodePicture(CUVIDPICPARAMS * pPictureParameters, CUcontext * pContext)
{
	// Handle CUDA picture decode (this actually calls the hardware VP/CUDA to decode video frames)
	CUresult oResult = cuvidDecodePicture(oDecoder_, pPictureParameters);
}

void
VideoDecoder::mapFrame(int iPictureIndex, CUdeviceptr * ppDevice, unsigned int * pPitch, CUVIDPROCPARAMS * pVideoProcessingParameters)
{
    CUresult oResult = cuvidMapVideoFrame(oDecoder_, 
                                          iPictureIndex,
                                          ppDevice,
                                          pPitch, pVideoProcessingParameters);
}

void
VideoDecoder::unmapFrame(CUdeviceptr pDevice)
{
    CUresult oResult = cuvidUnmapVideoFrame(oDecoder_, pDevice);
}

