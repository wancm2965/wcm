//
#ifndef __XISRENDER_H__
#define __XISRENDER_H__

#include "stdafx.h"
#include <string.h>

#include "./LibXI\LibXIStream\XIStream.h"
#include "./LibXI\DeviceDefs.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"


class XISRender:public FrameRateEvent
{
public:
	XISRender(void);
	virtual~XISRender(void);
public:
	int Open(HWND hWnd, XI_COLOR_FORMAT colorFormat, int nWidth, int nHeight, BOOL bUseOverlay, BOOL bWaitForVerticalBlank);
	void Close(void);
	void UpdateRender(CRect rcDest,GUID guidNew=GUID_NULL);
	int RenderFrame(unsigned char*pYUV420,int cbStride, int nWidth,int nHeight, int nFrameRate);
	
protected:
	void*					m_hRenderer;
	GUID					m_guidMonitor;
	int						m_nFrameRate;
	HWND					m_hWnd;
	CRect					m_rcDest;
	int						m_nWidth;
	int						m_nHeight;
	BOOL					m_bUseOverlay;
	BOOL					m_bWaitForVerticalBlank;
	XI_COLOR_FORMAT			m_colorFormat;
};	


#endif