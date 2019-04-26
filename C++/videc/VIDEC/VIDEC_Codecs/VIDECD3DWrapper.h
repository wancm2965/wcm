#ifndef __VIDECD3DWRAPPER_H__
#define __VIDECD3DWRAPPER_H__

#include "../VIDEC_Codecs/XDllLoadLibrary.h"

#include "../VIDECD3D/VIDECD3DAPI.h"

int D3DSupportPlayer(void);

int D3DInit(void);
void D3DTerminate(void);


void*D3DPlayerOpen(HWND hWnd, int nWidth, int nHeight);
void D3DPlayerClose(void*hPlayer);

int D3DPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int nWidth,int nHeight);
int D3DPlayerGetWidth(void*hPlayer);
int D3DPlayerGetHeight(void*hPlayer);

int D3DPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay);


class VIDECD3DWrapper
{
private:
	XDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	VIDECD3DWrapper(HINSTANCE hInstance);
	~VIDECD3DWrapper(void);
public:
	static VIDECD3DWrapper&Instance(void);
	bool IsOK(void);

	int (*pVIDECD3D_Init)(unsigned long nKey);


	void*(*pVIDECD3D_PlayerOpen)(HWND hWnd, int nWidth, int nHeight);
	void (*pVIDECD3D_PlayerClose)(void*hPlayer);

	int (*pVIDECD3D_PlayerPlayFrame)(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int nWidth,int nHeight);
	int (*pVIDECD3D_PlayerGetWidth)(void*hPlayer);
	int (*pVIDECD3D_PlayerGetHeight)(void*hPlayer);
	int (*pVIDECD3D_PlayerSkipDisplay)(void*hPlayer,int nFlagSkipDisplay);
};

#endif
