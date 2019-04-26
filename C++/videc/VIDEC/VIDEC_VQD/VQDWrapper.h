#ifndef __VQDWRAPPER_H__
#define __VQDWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

#include <VIDECVQD/VIDECVQDAPI.h>

void VQDInit(void);
void VQDTerminate(void);

void*VQDOpen(unsigned long nDetectFlags);
unsigned long VQDDetectFrame(void*pHandle, unsigned char *pFrameData, int nWidth, int nHeight);
void VQDClose(void*pHandle);
void VQDSetParams(unsigned long nDetectFlag,int nThreshold);

class VQDWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	VQDWrapper(HINSTANCE hInstance);
	~VQDWrapper(void);
public:
	static VQDWrapper&Instance(void);
	bool IsOK(void);

	void*(*pVIDECVQD_Open)(unsigned long nDetectFlags);
	unsigned long (*pVIDECVQD_DetectFrame)(void*pHandle, unsigned char *pFrameData, int nWidth, int nHeight);
	void (*pVIDECVQD_Close)(void*pHandle);
	void (*pVIDECVQD_SetParams)(unsigned long nDetectFlag,int nThreshold);
};

#endif
