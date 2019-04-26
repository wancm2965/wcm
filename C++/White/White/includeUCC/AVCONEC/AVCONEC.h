//AVCONEC.h
#ifndef __AVCONEC_H__
#define __AVCONEC_H__

#include <AVCONEC/AVCONEC_Export.h>
#include <AVCONEC/libAVCONEC.h>
#include <AVCONEC/libXMediaFileCapture.h>
#include <AVCONProduct.h>
#include <VIDEC/VIDEC.h>

//nAECType回音消除强度 0-4 默认4
AVCONEC_API void AVCONECInit(int nAECType,int nUsedByProductType=AVCON_PRODUCT_NORMAL,unsigned long ulVIDECFlag=VIDEC_INIT_FLAG_ALL);
AVCONEC_API void AVCONECUninit(void);

AVCONEC_API int AVCONECVideoCaptureDeviceCount(void);
AVCONEC_API bool AVCONECVideoCaptureDeviceName(int nDevice,char*szDeviceName);


AVCONEC_API IXMediaFileCapture*CreateXMediaFileCapture(IXMediaFileCaptureNotify&rNotify,HWND hWnd,const char*cszMediaFile,int nFrameRate,int nQuality,VIDEC_CODEC_TYPE codecType,bool bLoop,int nNetworkType);

#endif