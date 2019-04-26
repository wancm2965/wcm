#include <WinSock2.h> 
#include "D3DDisplay.h"
#include "XUtil.h"


#include <map>

#define WM_RESET_D3D	(WM_USER+963)

typedef std::map<HWND,D3DDisplay*>	MapD3DDisplay;
static MapD3DDisplay g_MapD3DDisplay;


// C Callback that dispatch event to the right display
static LRESULT CALLBACK __D3D__WndProcWindow(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = FALSE;
	BOOL resultSet = FALSE;
	
	MapD3DDisplay::iterator iter=g_MapD3DDisplay.find(hWnd);
	if (iter!=g_MapD3DDisplay.end())
	{
		D3DDisplay*pD3DDisplay=iter->second;
		result = pD3DDisplay->handleEvents(hWnd, uMsg, wParam, lParam);
		resultSet=TRUE;
	}

	return resultSet ? result : DefWindowProc(hWnd, uMsg, wParam, lParam);
}


D3DDisplay::D3DDisplay(VideoDisplayCallback&rCallback)
:m_rCallback(rCallback)
{
	m_hWndParent = NULL;
	m_pParentWindowProc = NULL;
	m_nImageWidth = 176;
	m_nImageHeight = 144;

	m_hD3DPlayer=NULL;
	m_bD3DReset=true;

	m_bRunning=false;

	m_nTimerID=0;

	m_bWTSSessionLock=false;
	m_bForceDestroyDraw=false;
}

D3DDisplay::~D3DDisplay(void)
{
}

int D3DDisplay::Open(void*pParent)
{
	m_hWndParent = reinterpret_cast<HWND>(pParent);

	m_hD3DPlayer=D3DPlayerOpen(m_hWndParent,m_nImageWidth,m_nImageHeight);
	if (m_hD3DPlayer==NULL)
	{
		return -1;
	}

	{
		m_pParentWindowProc = (WNDPROC) SetWindowLongPtr(m_hWndParent, GWL_WNDPROC, (LONG) __D3D__WndProcWindow);
		// Add this instance to the callback map
		g_MapD3DDisplay[m_hWndParent]=this;
	}

	m_bRunning=true;
	StartThread();

	return 0;
}	

void D3DDisplay::Close(void)
{
	m_bRunning=false;
	WaitForStop();

	if (m_nTimerID)
	{
		::KillTimer(m_hWndParent,m_nTimerID);
		m_nTimerID=0;
	}

	{
		// Remove this instance from the callback map
		g_MapD3DDisplay.erase(m_hWndParent);
		// Restore parent Window procedure
		SetWindowLongPtr(m_hWndParent, GWL_WNDPROC, (LONG) m_pParentWindowProc);
	}

	if (m_hD3DPlayer)
	{
		D3DPlayerClose(m_hD3DPlayer);
		m_hD3DPlayer=NULL;
	}
}

int D3DDisplay::GetWidth(void)
{
	return m_nImageWidth;
}

int D3DDisplay::GetHeight(void)
{
	return m_nImageHeight;
}

int D3DDisplay::SetTitle(const char*cszTitle,unsigned long nColor)
{
	return -1;
}

void D3DDisplay::SetImageSizeChanged(void)
{
}

void D3DDisplay::ThreadProcMain(void)
{
	VIDEC_Image*pImage=NULL;
	while (m_bRunning)
	{
		m_rCallback.OnVideoDisplayCallbackGetImage(&pImage);
		if (pImage)
		{
			if (m_bWTSSessionLock==false && m_bD3DReset)
			{
				DisplayFrame((unsigned char*)pImage->GetData(),pImage->GetDataLen(),pImage->GetWidth(),pImage->GetHeight(),pImage->GetFrameRate());
			}

			pImage->Release();
			pImage=NULL;
		}
		else
		{
			XSleep(10);
		}
	}
}

void D3DDisplay::DisplayFrame(unsigned char*pYUV420Data,int nLen,int nWidth,int nHeight,int nFrameRate)
{
	if (m_nImageWidth != nWidth || m_nImageHeight != nHeight)
	{
		m_nImageWidth = nWidth;
		m_nImageHeight = nHeight;
	}
	
	if (m_bForceDestroyDraw)
	{
		m_bForceDestroyDraw=false;

		m_bD3DReset=false;
		if(m_hWndParent)
		{
			OutputDebugString("PostMessage\n");
			PostMessage(m_hWndParent, WM_RESET_D3D, 0, 0);
			if (m_nTimerID==0)
			{
				m_nTimerID=1774;
				::SetTimer(m_hWndParent,m_nTimerID,1,NULL);
				OutputDebugString("SetTimer\n");
			}
		}
	}
	else
	{
		if (m_bD3DReset && m_hD3DPlayer)
		{
			D3DPlayerPlayFrame(m_hD3DPlayer,pYUV420Data,nLen,nFrameRate,nWidth,nHeight);
		}
	}
}

void D3DDisplay::ResetD3D(void)
{
	if (m_bD3DReset)
	{
		return ;
	}

	RECT rect = {0};
	GetWindowRect(m_hWndParent, &rect);
	int nWndWidth=rect.right-rect.left;
	int nWndHeight=rect.bottom-rect.top;

	if (m_hD3DPlayer)
	{
		void*hD3DPlayer=m_hD3DPlayer;
		m_hD3DPlayer=NULL;

		OutputDebugString("D3DPlayerClose\n");
		D3DPlayerClose(hD3DPlayer);
	}

	m_hD3DPlayer=D3DPlayerOpen(m_hWndParent,m_nImageWidth,m_nImageHeight);
	if (m_hD3DPlayer)
	{
		m_bD3DReset=true;
	}
}

LRESULT D3DDisplay::handleEvents(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
	case WM_SIZE:
	case WM_MOVE:
		{
		}
		break;
	case WM_RESET_D3D:
		{
			OutputDebugString("WM_RESET_D3D\n");
			ResetD3D();
		}
		break;
	case WM_TIMER:
		{
			if (wParam==m_nTimerID)
			{
				if (m_nTimerID)
				{
					::KillTimer(m_hWndParent,m_nTimerID);
					m_nTimerID=0;

					OutputDebugString("WM_TIMER\n");
					ResetD3D();

					if (m_hD3DPlayer==NULL)
					{
						m_bD3DReset=false;
						m_nTimerID=1774;
						::SetTimer(m_hWndParent,m_nTimerID,500,NULL);
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

