#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "XVideoDisplay.h"
#include "XUtil.h"



#include <map>

#define WM_RESET_DS	(WM_USER+963)

typedef std::map<HWND,XVideoDisplay*>	MapDSDisplay;
static MapDSDisplay g_MapDSDisplay;

SDRDraw*				g_pDraw=NULL;
SDRDraw*				g_pDraw2=NULL;

void CreateOverLayWnd(void)
{
	if (g_pDraw==NULL)
	{
		NEWRECORD( g_pDraw,new SDRDraw(320, 240) )
		NULLReturn( g_pDraw )  
		if (0!=g_pDraw->Open(::GetDesktopWindow(),320, 240))
		{
			g_pDraw->Close();
			delete g_pDraw;
			g_pDraw=NULL;
		}
		else
		{
			if (g_pDraw)
			{
				g_pDraw->SetWindowPos(0,0,1, 1);
			}
		}
	}
	if (g_pDraw2==NULL)
	{
		NEWRECORD( g_pDraw2,new SDRDraw(320, 240) )
		NULLReturn( g_pDraw2 )  
		if (0!=g_pDraw2->Open(::GetDesktopWindow(),320, 240))
		{
			g_pDraw2->Close();
			delete g_pDraw2;
			g_pDraw2=NULL;
		}
		else
		{
			if (g_pDraw2)
			{
				g_pDraw2->SetWindowPos(0,0,1, 1);
			}
		}
	}
}

void DestoryOverLayWnd(void)
{
	if (g_pDraw)
	{
		g_pDraw->Close();
		delete g_pDraw;
		g_pDraw=NULL;
	}
	if (g_pDraw2)
	{
		g_pDraw2->Close();
		delete g_pDraw2;
		g_pDraw2=NULL;
	}
}

// C Callback that dispatch event to the right display
static LRESULT CALLBACK __DS__WndProcWindow(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = FALSE;
	BOOL resultSet = FALSE;

	MapDSDisplay::iterator iter=g_MapDSDisplay.find(hWnd);
	if (iter!=g_MapDSDisplay.end())
	{
		XVideoDisplay*pDSDisplay=iter->second;
		result = pDSDisplay->handleEvents(hWnd, uMsg, wParam, lParam);
		resultSet=TRUE;
	}

	return resultSet ? result : DefWindowProc(hWnd, uMsg, wParam, lParam);
}


VideoDisplay*VideoDisplay::Create(YNCD3D_CB_DC cbdc,void*pUserData)
{
	//return new XVideoDisplay(cbdc,pUserData);
	NEWRECORDret3(  VideoDisplay* ,p , new XVideoDisplay(cbdc,pUserData) )   
}


XVideoDisplay::XVideoDisplay(YNCD3D_CB_DC cbdc,void*pUserData)
:m_cbdc(cbdc)
,m_pUserData(pUserData)
{
	m_hVideoWnd=NULL;
	m_nWidth=0;
	m_nHeight=0;
	m_hD3DPlayer=NULL;
	m_nRedraw=0;
	m_pDraw=NULL;
	m_pParentWindowProc = NULL;

	m_bDSReset=true;
	m_bPause=false;
	m_bWTSSessionLock=false;
	m_bImageSizeChanged=false;
	m_bForceDestroyDraw=false;
	m_bRunning=false;

	m_nSkipFrameCount=0;
	m_nFrameCount=0;
	m_nLastTS=timeGetTime();

	m_bFirstDraw=false;
	m_bEnableD3D=true;
	m_nDrawErrorCount = 0;
	m_nDrawResetErrorCount =0;
}

XVideoDisplay::~XVideoDisplay(void)
{
}


int XVideoDisplay::Open(void*pParent)
{
	m_hVideoWnd = reinterpret_cast<HWND>(pParent);

	{
		m_pParentWindowProc = (WNDPROC)SetWindowLongPtr(m_hVideoWnd, GWLP_WNDPROC, (LONG_PTR)__DS__WndProcWindow);
		// Add this instance to the callback map
		g_MapDSDisplay[m_hVideoWnd]=this;
	}

	m_bRunning=true;
	if (!StartThread())
	{
		m_bRunning=false;
		return -1;
	}

	return 0;
}

void XVideoDisplay::Close(void)
{
	m_bRunning=false;
	WaitForStop();

	{
		// Remove this instance from the callback map
		g_MapDSDisplay.erase(m_hVideoWnd);
		// Restore parent Window procedure
		SetWindowLongPtr(m_hVideoWnd, GWLP_WNDPROC, (LONG_PTR) m_pParentWindowProc);
	}

	if (m_hD3DPlayer)
	{
		YNCD3DPlayerClose(m_hD3DPlayer);
		m_hD3DPlayer=NULL;
	}

	if (m_pDraw)
	{
		m_pDraw->Close();
		delete m_pDraw;
		m_pDraw=NULL;
	}

	{
		XAutoLock l(m_csListImage);
		while (m_listImage.size())
		{
			VIDEC_Image*pImage=(VIDEC_Image*)m_listImage.front();
			m_listImage.pop_front();

			pImage->Release();
			pImage=NULL;
		}
	}
}

int XVideoDisplay::GetWidth(void)
{
	return m_nWidth;
}

int XVideoDisplay::GetHeight(void)
{
	return m_nHeight;
}
int XVideoDisplay::GetVideoShowFrameRate(void)
{
	if (m_hD3DPlayer)
	{
		return YNCD3DPlayerGetFrameRate(m_hD3DPlayer);
	}
	else if (m_pDraw)
	{
		return m_pDraw->GetFrameRate();
	}
	return 0;
}
int XVideoDisplay::GetVideoShowType(void)
{
	if (m_hD3DPlayer)
	{
		return YNCD3DPlayerGetShowType(m_hD3DPlayer);
	}
	else if (m_pDraw)
	{
		return 1;
	}
	return 0;
}

void XVideoDisplay::PausePlay(bool bPause,bool bClear)
{
	//m_bPause=bPause;
	//if (bClear)
	{
		XAutoLock l(m_csListImage);
		while (m_listImage.size())
		{
			VIDEC_Image*pImage=(VIDEC_Image*)m_listImage.front();
			m_listImage.pop_front();

			pImage->Release();
			pImage=NULL;
		}
	}
}

void XVideoDisplay::SetEnableD3D(bool bEnabled)
{
	m_bEnableD3D = bEnabled;
}

void XVideoDisplay::DisplayImage(VIDEC_Image*pImage)
{
	int nFrameRate = 0;
	int nSize = 0;
	{
		XAutoLock l(m_csListImage);
		pImage->AddRef();
		m_listImage.push_back(pImage);
	}

	if (FRE_GetDebugLevel() > 0)
	{
		m_nFrameCount++;
		if (timeGetTime()-m_nLastTS > 20000)
		{
			char szDebug[1024];
			sprintf(szDebug,"XVideoDisplay::DisplayImage FrameCount=%u SkipFrameCount=%u this = %u\n",m_nFrameCount,m_nSkipFrameCount,this);
			DebugOutput( logthread ,szDebug);
			m_nSkipFrameCount=0;
			m_nFrameCount=0;
			m_nLastTS=timeGetTime();
		}
	}

}

void XVideoDisplay::ThreadProcMain(void)
{
	m_nLastGetImageTimestamp=XGetTimestamp();

	while (m_bRunning)
	{
		unsigned long nTempTimestamp=XGetTimestamp();
		if (nTempTimestamp - m_nLastGetImageTimestamp >= 60000 )
		{
			m_nLastGetImageTimestamp = nTempTimestamp;
			RECT rc;  char msg[256] ={0};  ::GetWindowRect(m_hVideoWnd,&rc);
			sprintf(msg,"--------------Video Display get image data timeout 60s ,hwnd:%p,pointer:%p,window width:%d, height:%d \n", m_hVideoWnd, this,  rc.right - rc.left , rc.bottom - rc.top );
			DebugOutput( logthread ,msg);
		}

		int nSize = 0;
		VIDEC_Image*pImage=NULL;
		{
			XAutoLock l(m_csListImage);
			nSize = m_listImage.size();
			if (nSize > 6 && m_hD3DPlayer == NULL)
			{				
				do
				{
					pImage=(VIDEC_Image*)m_listImage.front();
					m_listImage.pop_front();
					if (pImage)
					{			
						pImage->Release();
						pImage=NULL;
					}
					m_nSkipFrameCount++;
				}while (m_listImage.size() > 3);
			}
			if (nSize)
			{
				pImage=(VIDEC_Image*)m_listImage.front();
				m_listImage.pop_front();
			}
		}
		if (nSize > 6 && m_hD3DPlayer)
		{
			YNCD3DPlayerSkipDisplay(m_hD3DPlayer,1);
			m_nSkipFrameCount++;
		}		
		if (pImage==NULL)
		{
			if (m_nRedraw> 0)
			{
				if (m_nRedraw++ > 4 && m_hD3DPlayer)
				{
					m_nRedraw=0;
					YNCD3DPlayerReDraw(m_hD3DPlayer);	
				}
			}

			XSleep(1);
			continue;
		}
		m_nRedraw=0;

		if (m_bDSReset==false ||
			m_bWTSSessionLock)
		{
			pImage->Release();
			pImage=NULL;
			continue;
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
				continue;
			}
		}

		m_nLastGetImageTimestamp= nTempTimestamp;

		if (m_hD3DPlayer)
		{
			YNCD3DPlayerPlayFrame(m_hD3DPlayer,(unsigned char*)pImage->GetData(),pImage->GetDataLen(),pImage->GetFrameRate(),pImage->GetWidth(),pImage->GetHeight());
		}
		else if (m_pDraw)
		{
			if (!m_pDraw->Draw((unsigned char*)pImage->GetData(),pImage->GetDataLen(),pImage->GetFrameRate()))
			{	
				m_nDrawErrorCount++;

				if (m_nDrawErrorCount > 10)
				{
					DebugOutput( logcoreerror ,"=======  Draw ForceDestroyDraw =======\n");
					m_nDrawErrorCount = 0;
					m_bForceDestroyDraw=true;
					m_bDSReset=false;
					if(m_hVideoWnd)
					{
						PostMessage(m_hVideoWnd, WM_RESET_DS, 0, 0);
					}
				}

				pImage->Release();
				pImage=NULL;
				continue;
			}
			else
			{
				m_nDrawErrorCount = 0;
			}
		}
		else if (m_pDraw == NULL)
		{
			if (m_nDrawResetErrorCount >0 && m_nDrawResetErrorCount < 10) //ResetImageFormat ,max successive create 10 times 
			{
				DebugOutput( logcoreerror ,"=======  DrawResetError ForceDestroyDraw =======\n");
				m_bForceDestroyDraw=true;
				m_bDSReset=false;
				if(m_hVideoWnd)
				{
					PostMessage(m_hVideoWnd, WM_RESET_DS, 0, 0);
				}
				if(m_nDrawResetErrorCount >=9 )
				{
					DebugOutput( logcoreerror ,"========DrawResetError m_nDrawResetErrorCount >=9 Failed======\n");
				}
			}
			pImage->Release();
			pImage=NULL;
			continue;
		}


		
		if (!m_bFirstDraw)
		{
			m_bFirstDraw=true;
			::PostMessage(m_hVideoWnd,(WM_USER+100),102,NULL);
		}

		pImage->Release();
		pImage=NULL;
	}
}

LRESULT XVideoDisplay::handleEvents(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_ERASEBKGND:
		{
			m_nRedraw=1;
		}
		break;
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
			m_nRedraw=1;
		}
		break;
	case WM_RESET_DS:
		{
			RECT rect = {0};
			GetWindowRect(hWnd, &rect);
			int nWndWidth=rect.right-rect.left;
			int nWndHeight=rect.bottom-rect.top;


			if (m_bEnableD3D && m_hD3DPlayer==NULL)
			{
				m_bFirstDraw=false;
				//m_hD3DPlayer=D3DPlayerOpen(m_hVideoWnd,m_nWidth,m_nHeight);
				m_hD3DPlayer=YNCD3DPlayerOpen(m_cbdc,m_pUserData,m_hVideoWnd,m_nWidth,m_nHeight);
				if (m_hD3DPlayer)
				{
					m_bDSReset=true;
				}
				else
				{
					DebugOutput( logcoreerror ,"======= d3d create failed!\n");
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
					//add 20160923,possible bug.
					m_nDrawResetErrorCount ++;
					DebugOutput( logcoreerror ,"======Error ,SDRDraw ResetImageFormat Failed ,Close NULL.\n");
				}
				else
				{
					m_bDSReset=true;
					m_nDrawResetErrorCount =0;
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
				m_nDrawErrorCount = 0;
				NEWRECORD( m_pDraw,new SDRDraw(m_nWidth,m_nHeight) )
				NULLRecord( m_pDraw )  
				if (0!=m_pDraw->Open(m_hVideoWnd,m_nWidth,m_nHeight))
				{
					m_pDraw->Close();
					delete m_pDraw;
					m_pDraw=NULL;
					m_nDrawResetErrorCount ++;
					DebugOutput( logcoreerror ,"======Error ,SDRDraw Open Failed ,Close NULL.\n");
				}
				else
				{
					m_bDSReset=true;
					m_nDrawResetErrorCount =0;
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
	/*case WM_WTSSESSION_CHANGE:
		{
			switch (wParam)
			{
			case WTS_SESSION_LOCK:
				m_bWTSSessionLock=true;
				if (m_hD3DPlayer)
				{
					YNCD3DPlayerSkipDisplay(m_hD3DPlayer,1);
				}
				break;
			case WTS_SESSION_UNLOCK:
				if (m_hD3DPlayer)
				{
					YNCD3DPlayerSkipDisplay(m_hD3DPlayer,0);
				}
				m_bForceDestroyDraw=true;
				m_bWTSSessionLock=false;
				break;
			}
		}
		break;*/
	}

	return CallWindowProc(m_pParentWindowProc, hWnd, uMsg, wParam, lParam);
}

void XVideoDisplay::ScreenChangeReopenDraw(void)
{
	if(m_pDraw)
	{
		DebugOutput( logcallstack ,"======= ScreenChangeReopenDraw =======\n");
		m_bForceDestroyDraw=true;
		m_bDSReset=false;
		if(m_hVideoWnd)
		{
			PostMessage(m_hVideoWnd, WM_RESET_DS, 0, 0);
		}
	}
}
