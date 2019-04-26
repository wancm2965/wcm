
#include "stdafx.h"

#include "XISRender.h"


XISRender::XISRender(void)
:m_hWnd(NULL)
,m_hRenderer(NULL)
,m_nFrameRate(30)
{
}

XISRender::~XISRender(void)
{
}

int XISRender::Open(HWND hWnd, XI_COLOR_FORMAT colorFormat, int nWidth, int nHeight, BOOL bUseOverlay, BOOL bWaitForVerticalBlank)
{
	if (m_hRenderer != NULL || hWnd == NULL)
	{
		return -1;
	}
	if (!XIS_GetMonitorGUIDFromWindow(hWnd, &m_guidMonitor))
		m_guidMonitor = GUID_NULL;
	// 		OSVERSIONINFO osvi;
	// 		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	// 		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	// 		GetVersionEx(&osvi);
	// 		DWORD dwMajorVersion = osvi.dwMajorVersion;
	m_hRenderer = XIS_CreateVideoRenderer(&m_guidMonitor, hWnd, colorFormat, nWidth,nHeight, bUseOverlay,bWaitForVerticalBlank);
	if (NULL == m_hRenderer)
		return -1;

	m_hWnd = hWnd;
	m_colorFormat = colorFormat;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_bWaitForVerticalBlank = bWaitForVerticalBlank;
	m_bUseOverlay = bUseOverlay;

	FrameRateEvent::Open();

	return 0;
}

void XISRender::Close(void)
{
	if (m_hRenderer == NULL || m_hWnd == NULL)
	{
		return;
	}
	XIS_DestroyVideoRenderer(m_hRenderer);
	m_hRenderer = NULL;
	FrameRateEvent::Close();
}

void XISRender::UpdateRender(CRect rcDest,GUID guidNew)
{
	if (m_hRenderer == NULL || m_hWnd == NULL)
	{
		return;
	}

	if (m_guidMonitor != guidNew )
	{
		m_guidMonitor = guidNew;
		XIS_ResetVideoRenderer(m_hRenderer, &m_guidMonitor, m_hWnd, m_colorFormat, m_nWidth, m_nHeight, m_bUseOverlay, m_bWaitForVerticalBlank);
	}
	m_rcDest = rcDest;
	XIS_SetVideoRendererPosition(m_hRenderer, NULL, rcDest);
}

BOOL IsTopDown(XI_COLOR_FORMAT color)
{
	if (color == XI_COLOR_RGB24 || color == XI_COLOR_RGB32)
		return FALSE;
	return TRUE;
}

int XISRender::RenderFrame(unsigned char*pYUV420,int cbStride, int nWidth,int nHeight, int nFrameRate)
{
	if (m_hRenderer == NULL || m_hWnd == NULL)
	{
		return -1;
	}
	if (nWidth != m_nWidth || m_nHeight != nHeight)
	{
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		XIS_ResetVideoRenderer(m_hRenderer, &m_guidMonitor, m_hWnd, m_colorFormat, m_nWidth, m_nHeight, m_bUseOverlay, m_bWaitForVerticalBlank);
		UpdateRender(m_rcDest);
	}

	if (nFrameRate!=m_nFrameRate)
	{
		m_nFrameRate=nFrameRate;
		FrameRateEvent::SetFrameRate(m_nFrameRate);
	}

	FrameRateEvent::WaitEvent();

	return XIS_VideoRendererDrawImage(m_hRenderer, pYUV420, cbStride, IsTopDown(m_colorFormat));
}
