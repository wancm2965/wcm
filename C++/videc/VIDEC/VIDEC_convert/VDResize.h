#ifndef __VDRESIZEAPI_H__
#define __VDRESIZEAPI_H__

#ifdef VDRESIZE_EXPORT
#define VDRESIZE_API _declspec(dllexport)
#elif VDRESIZE_DLL
#define VDRESIZE_API _declspec(dllimport)
#else
#define VDRESIZE_API
#endif

VDRESIZE_API void*VDResize_Open(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride);
VDRESIZE_API void VDResize_GetFrame(void*hVDResize,unsigned char*srcData[], unsigned char*dstData[]);
VDRESIZE_API void VDResize_Close(void*hVDResize);
VDRESIZE_API bool VDResample_GetFrame(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride, unsigned char*srcData[], unsigned char*dstData[]);

#endif