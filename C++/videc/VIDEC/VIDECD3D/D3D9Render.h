//D3D9Render.h
#ifndef __D3D9RENDER_H__
#define __D3D9RENDER_H__

#include <d3d9.h>

#include "../../VIDEC/VIDEC_csp/FrameRateEvent.h"

class D3D9Render
	: public FrameRateEvent
{
public:
	D3D9Render(void);
	virtual~D3D9Render(void);
public:
	int Open(HWND hWnd,int nWidth,int nHeight);
	void Close(void);
	int DrawFrame(unsigned char*pFrameData,int nFrameLen,int nFrameRate,int nWidth,int nHeight);
	int SkipDisplay(int nFlagSkipDisplay);
	int GetWidth(void);
	int GetHeight(void);
private:
	HRESULT ResetDevice(void);
	HRESULT CreateSwapChains(void);
private:
    HWND                    m_hWnd;
	bool					m_bResetImageFormat;

    IDirect3D9Ex*			m_pD3D;
    IDirect3DDevice9Ex*		m_pDevice;
	IDirect3DSwapChain9*	m_pSwapChain;

    D3DPRESENT_PARAMETERS   m_d3dpp;

    UINT                    m_nWidth;
    UINT                    m_nHeight;
	UINT					m_nAdapter;
	HMONITOR				m_hMonitor;
	int						m_nFrameLen;

	RECT					m_rcDst;
	int						m_nFlagSkipDisplay;
	int						m_nFrameRate;
	bool					m_bDeviceLost;
};

#endif
