// x264enc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "x264enc.h"
#include "avc.h"
#include "XUtil.h"
#include "./CUDA/CUDAEncWrapper.h"
#include "HWVideoDecoder.h"
#include "IntelCodecWrapper.h"
#include "../MPVideo_convert/VDPWrapper.h"


extern void DH264Play_init(void);
extern void DH264Play_terminate(void);
extern void AH400Play_init(void);
extern void AH400Play_terminate(void);
extern void UserPlay_init(void);
extern void UserPlay_terminate(void);




void VIDEC_CodecInit(void)
{
	UserPlay_init();
	DH264Play_init();
	AH400Play_init();

	avc_init();

	//if (0!=CUDAEncoderLoad())
	//{
	//	CUDAEncoderFree();
	//}

	VDResizeInit();
	intel_codec_init();
	HWVideoDecoder::Load();
}

void VIDEC_CodecTerminate(void)
{
	UserPlay_terminate();
	DH264Play_terminate();
	AH400Play_terminate();

	avc_uninit();

	//CUDAEncoderFree();

	HWVideoDecoder::Unload();
	intel_codec_uninit();

	VDResizeUninit();
}
