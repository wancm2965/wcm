#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "DSDisplay.h"
#include "XUtil.h"
#include "D3DDisplay.h"



#include <map>

#define WM_RESET_DS	(WM_USER+963)

typedef std::map<HWND,DSDisplay*>	MapDSDisplay;
static MapDSDisplay g_MapDSDisplay;


// C Callback that dispatch event to the right display
static LRESULT CALLBACK __DS__WndProcWindow(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = FALSE;
	BOOL resultSet = FALSE;

	MapDSDisplay::iterator iter=g_MapDSDisplay.find(hWnd);
	if (iter!=g_MapDSDisplay.end())
	{
		DSDisplay*pDSDisplay=iter->second;
		result = pDSDisplay->handleEvents(hWnd, uMsg, wParam, lParam);
		resultSet=TRUE;
	}

	return resultSet ? result : DefWindowProc(hWnd, uMsg, wParam, lParam);
}


VideoDisplay*VideoDisplay::Create(VideoDisplayCallback&rCallback)
{
	return new DSDisplay(rCallback);
}


DSDisplay::DSDisplay(VideoDisplayCallback&rCallback)
:m_rCallback(rCallback)
{
	m_hVideoWnd=NULL;
	m_nWidth=0;
	m_nHeight=0;
	m_hD3DPlayer=NULL;
	m_pDraw=NULL;
	m_nLastDrawTimestamp=0;
	m_nNextFrameInterval=1000/30;
	m_pParentWindowProc = NULL;

	m_bDSReset=true;

	m_bWTSSessionLock=false;
	m_bImageSizeChanged=false;
	m_bForceDestroyDraw=false;
}

DSDisplay::~DSDisplay(void)
{
}


int DSDisplay::Open(void*pParent)
{
	m_hVideoWnd = reinterpret_cast<HWND>(pParent);


	{
		m_pParentWindowProc = (WNDPROC) SetWindowLongPtr(m_hVideoWnd, GWL_WNDPROC, (LONG) __DS__WndProcWindow);
		// Add this instance to the callback map
		g_MapDSDisplay[m_hVideoWnd]=this;
	}


	return XWorkThread::Start();
}

void DSDisplay::Close(void)
{
	XWorkThread::Stop();

	{
		// Remove this instance from the callback map
		g_MapDSDisplay.erase(m_hVideoWnd);
		// Restore parent Window procedure
		SetWindowLongPtr(m_hVideoWnd, GWL_WNDPROC, (LONG) m_pParentWindowProc);
	}

	if (m_hD3DPlayer)
	{
		D3DPlayerClose(m_hD3DPlayer);
		m_hD3DPlayer=NULL;
	}

	if (m_pDraw)
	{
		m_pDraw->Close();
		delete m_pDraw;
		m_pDraw=NULL;
	}
}

int DSDisplay::GetWidth(void)
{
	return m_nWidth;
}

int DSDisplay::GetHeight(void)
{
	return m_nHeight;
}

int DSDisplay::SetTitle(const char*cszTitle,unsigned long nColor)
{
	return -1;
}

void DSDisplay::SetImageSizeChanged(void)
{
	if (m_hD3DPlayer==NULL)
	{
		m_bImageSizeChanged=true;
	}
}



bool DSDisplay::IsPending(void)
{
	if (::IsWindowVisible(m_hVideoWnd)==FALSE)
	{
		return false;
	}

	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp-m_nLastDrawTimestamp>=m_nNextFrameInterval)
	{
		return true;
	}
	return false;
}

bool DSDisplay::Running(void)
{
	unsigned long nTimestamp=XGetTimestamp();
	VIDEC_Image*pImage=NULL;
	m_rCallback.OnVideoDisplayCallbackGetImage(&pImage);
	if (pImage==NULL)
	{
		m_nLastDrawTimestamp=nTimestamp+10;
		return false;
	}

	if (m_bDSReset==false ||
		m_bWTSSessionLock)
	{
		pImage->Release();
		pImage=NULL;
		m_nLastDrawTimestamp=nTimestamp+10;
		return false;
	}

	if (pImage->GetWidth()!=m_nWidth ||
		pImage->GetHeight()!=m_nHeight ||
		m_bImageSizeChanged)
	{
		m_nWidth=pImage->GetWidth();
		m_nHeight=pImage->GetHeight();

		m_bImageSizeChanged=false;
		if (m_hD3DPlayer==NULL)
		{
			m_bDSReset=false;

			if(m_hVideoWnd)
			{
				PostMessage(m_hVideoWnd, WM_RESET_DS, 0, 0);
			}

			pImage->Release();
			pImage=NULL;
			m_nLastDrawTimestamp=nTimestamp+10;
			return false;
		}
	}

	int nFrameRate=pImage->GetFrameRate();

	if (m_hD3DPlayer)
	{
		D3DPlayerPlayFrame(m_hD3DPlayer,(unsigned char*)pImage->GetData(),pImage->GetDataLen(),nFrameRate,pImage->GetWidth(),pImage->GetHeight());
	}
	else if (m_pDraw)
	{
		if (!m_pDraw->Draw((unsigned char*)pImage->GetData(),pImage->GetDataLen(),pImage->GetWidth()))
		{
			m_nLastDrawTimestamp=nTimestamp+10;

			pImage->Release();
			pImage=NULL;

			return false;
		}
	}

	pImage->Release();
	pImage=NULL;

	unsigned long nCurrentTimestamp=XGetTimestamp();
	unsigned long nDelta=nCurrentTimestamp-nTimestamp;
	if (nFrameRate<1)
		nFrameRate=1;

	unsigned long nFrameInterval=1000/nFrameRate;

	if (nFrameInterval>=60)
	{
		nFrameInterval=60;
	}
	else if (nFrameInterval>=38)
	{
		nFrameInterval=38;
	}
	else if (nFrameInterval>=30)
	{
		nFrameInterval=30;
	}
	else if (nFrameInterval>=18)
	{
		nFrameInterval=18;
	}
	else if (nFrameInterval>=14)
	{
		nFrameInterval=14;
	}
	else
	{
		nFrameInterval=10;
	}

	if (nFrameInterval>nDelta)
	{
		nFrameInterval-=nDelta;

		//char szDebug[1024];
		//sprintf(szDebug,"%u\n",nCurrentTimestamp-m_nLastDrawTimestamp);
		//OutputDebugString(szDebug);

		m_nLastDrawTimestamp=nCurrentTimestamp;
		m_nNextFrameInterval=nFrameInterval;
	}

	return true;
}



LRESULT DSDisplay::handleEvents(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
	case WM_SIZE:
	case WM_MOVE:
		{
			RECT rect = {0};
			GetWindowRect(hWnd, &rect);
			int nWndWidth=rect.right-rect.left;
			int nWndHeight=rect.bottom-rect.top;

			if (m_pDraw)
			{
				m_pDraw->SetWindowPos(0,0,nWndWidth,nWndHeight);
			}
		}
		break;
	case WM_RESET_DS:
		{
			RECT rect = {0};
			GetWindowRect(hWnd, &rect);
			int nWndWidth=rect.right-rect.left;
			int nWndHeight=rect.bottom-rect.top;


			if (D3DSupportPlayer() && m_hD3DPlayer==NULL)
			{
				m_hD3DPlayer=D3DPlayerOpen(m_hVideoWnd,m_nWidth,m_nHeight);
				if (m_hD3DPlayer)
				{
					m_bDSReset=true;
				}
			}

			if (m_bForceDestroyDraw)
			{
				m_bForceDestroyDraw=false;
				if (m_pDraw)
				{
					m_pDraw->Close();
					delete m_pDraw;
					m_pDraw=NULL;
				}
			}

			if (m_pDraw)
			{
				if (0!=m_pDraw->ResetImageFormat(m_nWidth,m_nHeight))
				{
					m_pDraw->Close();
					delete m_pDraw;
					m_pDraw=NULL;
				}
				else
				{
					m_bDSReset=true;

					RECT rect = {0};
					GetWindowRect(hWnd, &rect);
					int nWndWidth=rect.right-rect.left;
					int nWndHeight=rect.bottom-rect.top;

					if (m_pDraw)
					{
						m_pDraw->SetWindowPos(0,0,nWndWidth,nWndHeight);
					}
				}
			}
			else if (m_hD3DPlayer==NULL)
			{
				m_pDraw=new SDRDraw(m_nWidth,m_nHeight);
				if (0!=m_pDraw->Open(m_hVideoWnd,m_nWidth,m_nHeight))
				{
					m_pDraw->Close();
					delete m_pDraw;
					m_pDraw=NULL;
				}
				else
				{
					m_bDSReset=true;

					RECT rect = {0};
					GetWindowRect(hWnd, &rect);
					int nWndWidth=rect.right-rect.left;
					int nWndHeight=rect.bottom-rect.top;

					if (m_pDraw)
					{
						m_pDraw->SetWindowPos(0,0,nWndWidth,nWndHeight);
					}
				}
			}
		}
		break;
	case WM_WTSSESSION_CHANGE:
		{
			switch (wParam)
			{
			case WTS_SESSION_LOCK:
				m_bWTSSessionLock=true;
				if (m_hD3DPlayer)
				{
					D3DPlayerSkipDisplay(m_hD3DPlayer,1);
				}
				break;
			case WTS_SESSION_UNLOCK:
				if (m_hD3DPlayer)
				{
					D3DPlayerSkipDisplay(m_hD3DPlayer,0);
				}
				m_bForceDestroyDraw=true;
				m_bWTSSessionLock=false;
				break;
			}
		}
		break;
	}

	return CallWindowProc(m_pParentWindowProc, hWnd, uMsg, wParam, lParam);
}
