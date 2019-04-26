#ifndef __VIDECUDAWRAPPER_H__
#define __VIDECUDAWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

#include "../VIDECUDA/VIDECUDAAPI.h"

int cudaSupportPlayer(void);

int cudaInit(void);
void cudaTerminate(void);

void cudaPlayerSetMaxCount(unsigned int nMaxPlayer);
int cudaPlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight);

void*cudaPlayerOpen(HWND hWnd, int nWidth, int nHeight);
void cudaPlayerClose(void*hPlayer);

int cudaPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate, int&nFlagReset);
int cudaPlayerGetWidth(void*hPlayer);
int cudaPlayerGetHeight(void*hPlayer);

int cudaPlayerResize(void*hPlayer);
int cudaPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay);

class VIDECUDAWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	VIDECUDAWrapper(HINSTANCE hInstance);
	~VIDECUDAWrapper(void);
public:
	static VIDECUDAWrapper&Instance(void);
	bool IsOK(void);

	int (*pVIDECUDA_Init)(unsigned long nKey);

	void (*pVIDECUDA_PlayerSetMaxCount)(unsigned int nMaxPlayer);

	int (*pVIDECUDA_PlayerTry)(void*hPlayer,unsigned long nWidth,unsigned long nHeight);

	void*(*pVIDECUDA_PlayerOpen)(HWND hWnd, int nWidth, int nHeight);
	void (*pVIDECUDA_PlayerClose)(void*hPlayer);

	int (*pVIDECUDA_PlayerPlayFrame)(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate, int&nFlagReset);
	int (*pVIDECUDA_PlayerGetWidth)(void*hPlayer);
	int (*pVIDECUDA_PlayerGetHeight)(void*hPlayer);
	int (*pVIDECUDA_PlayerResize)(void*hPlayer);
	int (*pVIDECUDA_PlayerSkipDisplay)(void*hPlayer,int nFlagSkipDisplay);
};

#endif
