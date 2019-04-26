// x264enc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "x264enc.h"
#include "avc.h"
#include "XUtil.h"
#include "IntelCodecWrapper.h"
#include "../VIDEC_Codecs/VIDECYNCWrapper.h"
#include "../VIDEC_Wnd/H264Player.h"
#include "../VIDEC_convert/VDPWrapper.h"


extern void AH400Play_init(void);
extern void AH400Play_terminate(void);
extern void UserPlay_init(void);
extern void UserPlay_terminate(void);

extern void VQDInit(void);
extern void VQDTerminate(void);


void VIDEC_CodecInit(void)
{
	UserPlay_init();
	AH400Play_init();

	avc_init();


	intel_codec_init();
	VQDInit();
	YNCD3DInit();
	H264Init();
	VDResizeInit();
}

void VIDEC_CodecTerminate(void)
{
	UserPlay_terminate();
	AH400Play_terminate();

	avc_uninit();


	intel_codec_uninit();
	VQDTerminate();
	YNCD3DTerminate();
	H264Terminate();
	VDResizeUninit();
}
