#include "stdafx.h"
#include "stdhdrs_client.h"
#include "WinVNCClient.h"
#include "VNCClientWnd.h"
#include "Exception.h"

#define WM_SOCKEVENT WM_USER+1

extern HINSTANCE g_AppInstance;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define VNCCLIENTWND_CLASS_NAME "VNCClientWnd"

VNCClientWnd::VNCClientWnd(WinVNCClient&rWinVNCClient,unsigned short&nPort)
:m_rWinVNCClient(rWinVNCClient)
{
	// Create a dummy window
	WNDCLASSEX wndclass;

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc	= VNCClientWnd::WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= g_AppInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= NULL;
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName	= (const char *) NULL;
	wndclass.lpszClassName	= VNCCLIENTWND_CLASS_NAME;
	wndclass.hIconSm		= NULL;

	RegisterClassEx(&wndclass);

	m_hWnd = CreateWindow(VNCCLIENTWND_CLASS_NAME,
				VNCCLIENTWND_CLASS_NAME,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				200, 200,
				NULL,
				NULL,
				g_AppInstance,
				NULL);
	
	// record which client created this window
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG) this);

	// Create a listening socket
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(nPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    m_hSocketListening = socket(AF_INET, SOCK_STREAM, 0);
	if (!m_hSocketListening)
		throw WarningException("Error creating VNCClientWnd socket");

	int one = 1, res = 0;

	res = bind(m_hSocketListening, (struct sockaddr *)&addr, sizeof(addr));
	if (res == SOCKET_ERROR) {
		closesocket(m_hSocketListening);
		m_hSocketListening = 0;
		throw WarningException("Error binding VNCClientWnd socket");
	}

#ifdef WIN32	
	int nLengthAddr = sizeof (sockaddr);
#else
	socklen_t nLengthAddr = sizeof (sockaddr);
#endif
	if (getsockname (m_hSocketListening, (sockaddr*)&addr, &nLengthAddr) != SOCKET_ERROR)
	{
		nPort=ntohs (addr.sin_port);
	}

	res = listen(m_hSocketListening, 5);
	if (res == SOCKET_ERROR) {
		closesocket(m_hSocketListening);
		m_hSocketListening = 0;
		throw WarningException("Error when VNCClientWnd listens");
	}

	// Send a message to specified window on an incoming connection
	WSAAsyncSelect (m_hSocketListening,  m_hWnd,  WM_SOCKEVENT, FD_ACCEPT);
}


// Process window messages
LRESULT CALLBACK VNCClientWnd::WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
	// This is a static method, so we don't know which instantiation we're 
	// dealing with. We have stored a pseudo-this in the window user data, 
	// though.
	VNCClientWnd *_this = (VNCClientWnd *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (iMsg) 
	{

	case WM_CREATE:
		{
			return 0;
		}

	case WM_SOCKEVENT:
		{
			switch(lParam) {
			case FD_ACCEPT:
				if(wParam == _this->m_hSocketListening)
				{
					SOCKET hNewSock;
					hNewSock = accept(_this->m_hSocketListening, NULL, NULL);
					WSAAsyncSelect(hNewSock, hwnd, 0, 0);
					unsigned long nbarg = 0;
					ioctlsocket(hNewSock, FIONBIO, &nbarg);

					_this->m_rWinVNCClient.OnSocketAccepted(hNewSock);
				}
				break;
			case FD_CLOSE:
				{
					if(wParam == _this->m_hSocketListening)
					{
						DestroyWindow(hwnd);
					}
				}
				break;
			}
			
			return 0;
		}
	}
	
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

VNCClientWnd::~VNCClientWnd()
{
	DestroyWindow(m_hWnd);
	shutdown(m_hSocketListening, SD_BOTH);
	closesocket(m_hSocketListening);
}

