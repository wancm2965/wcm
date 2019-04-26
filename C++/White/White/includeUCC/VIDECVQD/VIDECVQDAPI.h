//VIDECVQDAPI.h
#ifndef __VIDECVQDAPI_H__
#define __VIDECVQDAPI_H__

#ifdef VIDECVQD_EXPORT
#define VIDECVQD_API _declspec(dllexport)
#elif VIDECVQD_DLL
#define VIDECVQD_API _declspec(dllimport)
#else
#define VIDECVQD_API
#endif

#define VQDS_DONE			0x00000000
#define VQDS_SIGNALLOST		0x00000001		//信号缺失；
#define VQDS_COLORCAST		0x00000002			//偏色；
#define VQDS_BLUR			0x00000004					//清晰度异常；
#define VQDS_BRIGHT			0x00000008				//亮度异常；
#define VQDS_FREEZE			0x00000010				//画面冻结；
#define VQDS_NOISE			0x00000020					//噪声、干扰；
#define VQDS_JITTER			0x00000040				//抖动、滚屏；
#define VQDS_DETECTING		0xFFFFFFFF

VIDECVQD_API void*VIDECVQD_Open(unsigned long nDetectFlags);	//打开一个质量诊断对象；
VIDECVQD_API void VIDECVQD_Close(void*pHandle);		//关闭打开的那个质量诊断对象；
VIDECVQD_API unsigned long VIDECVQD_DetectFrame(void*pHandle, unsigned char *pFrameData, int nWidth, int nHeight);	//帧检测函数；

/******************************************************************************************/
/*目前实现了nDetectFlag=【VQDS_BRIGHT，VQDS_BLUR，VQDS_NOISE】的范围设置nThreshold=【1-6】*/
/******************************************************************************************/
VIDECVQD_API void VIDECVQD_SetParams(unsigned long nDetectFlag,int nThreshold);	//阈值参数的设置,范围1-6，在函数VIDECVQD_DetectFrame之前调用；
#endif