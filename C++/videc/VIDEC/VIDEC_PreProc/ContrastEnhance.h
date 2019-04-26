#ifdef __ENABLE_CONTRAST_ENHANCE__
 
//ContrastEnhance.h
#ifndef __CONTRAST_ENHANCE_SSE_S_LUT_H__
#define __CONTRAST_ENHANCE_SSE_S_LUT_H__

//define the fast calls for real functions
#define ContrastEnhanceOpen(nWidth,nHeight)		ContrastEnhanceOpen_sse_s_lut(nWidth,nHeight,15)
#define ContrastEnhanceClose(pHandle)				ContrastEnhanceClose_sse_s_lut(pHandle)
#define ContrastEnhanceProcess(pHandle,pY,nLevel)			ContrastEnhanceProcess_sse_s_lut(pHandle, pY,nLevel)

void*	ContrastEnhanceOpen_sse_s_lut(int nWidth,int nHeight,int enhanc_level=15);
void	ContrastEnhanceClose_sse_s_lut(void*pHandle);
void	ContrastEnhanceProcess_sse_s_lut(void*pHandle,unsigned char*pY,int enhanc_level=15);

#endif

#endif


