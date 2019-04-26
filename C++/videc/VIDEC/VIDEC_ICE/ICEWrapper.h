#ifndef __ICEWRAPPER_H__
#define __ICEWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

#include "../VIDECICE/VIDECICEAPI.h"

void ICE_init(void);
void ICE_terminate(void);

void*ICEOpen(int nWidth,int nHeight);
void ICEClose(void*pHandle);
int ICEProcess(void*pHandle,unsigned char*pU,unsigned char*pV);

class ICEWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	ICEWrapper(HINSTANCE hInstance);
	~ICEWrapper(void);
public:
	static ICEWrapper&Instance(void);
	bool IsOK(void);

	void*(*pVIDECICE_Open)(int nWidth,int nHeight,unsigned long nKey);
	void (*pVIDECICE_Close)(void*pHandle);
	int (*pVIDECICE_Process)(void*pHandle,unsigned char*pU,unsigned char*pV);
};

#endif
