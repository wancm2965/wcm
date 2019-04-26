#ifndef __VDPWRAPPER_H__
#define __VDPWRAPPER_H__

#include "../MPVideo_Codecs/XDllLoadLibrary.h"
#include "VDResize.h"

void VDResizeInit(void);
void VDResizeUninit(void);

void* VDResizeOpen(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride);
void VDResizeGetFrame(void*pHandle,unsigned char*srcData[], unsigned char*dstData[]);
void VDResizeClose(void*pHandle);
bool VDResampleGetFrame(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride, unsigned char*srcData[], unsigned char*dstData[]);

class VDPWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	VDPWrapper(HINSTANCE hInstance);
	~VDPWrapper(void);
public:
	static VDPWrapper&Instance(void);
	bool IsOK(void);

	void* (*pVDResizeOpen)(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride);
	void (*pVDResizeGetFrame)(void*pHandle,unsigned char*srcData[], unsigned char*dstData[]);
	void (*pVDResizeClose)(void*pHandle);
	bool (*pVDResampleGetFrame)(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride, unsigned char*srcData[], unsigned char*dstData[]);

};

#endif
