//  Copyright (C) 2003-2006 Constantin Kaplinsky. All Rights Reserved.
//  Copyright (C) 2000 Tridia Corporation. All Rights Reserved.
//  Copyright (C) 1999 AT&T Laboratories Cambridge. All Rights Reserved.
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// TightVNC distribution homepage on the Web: http://www.tightvnc.com/
//
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


// Many thanks to Randy Brown <rgb@inven.com> for providing the 3-button
// emulation code.

// This is the main source for a ClientConnection object.
// It handles almost everything to do with a connection to a server.
// The decoding of specific rectangle encodings is done in separate files.

#include "stdhdrs_client.h"

#include "vncviewer.h"

#ifdef UNDER_CE
#include "omnithreadce.h"
#define SD_BOTH 0x02
#else
#include <omnithread/omnithread.h>
#endif

#include "WinVNCClient.h"
#include "ClientConnection.h"

#include "commctrl.h"
#include "Exception.h"


#define INITIALNETBUFSIZE 4096
#define MAX_ENCODINGS 20
#define VWR_WND_CLASS_NAME _T("WINVNCCLIENT")

#define WM_REGIONUPDATED WM_USER+1

extern HINSTANCE g_AppInstance;


HotKeys g_HotKeys;

/*
 * Macro to compare pixel formats.
 */

#define PF_EQ(x,y)							\
	((x.bitsPerPixel == y.bitsPerPixel) &&				\
	 (x.depth == y.depth) &&					\
	 ((x.bigEndian == y.bigEndian) || (x.bitsPerPixel == 8)) &&	\
	 (x.trueColour == y.trueColour) &&				\
	 (!x.trueColour || ((x.redMax == y.redMax) &&			\
			    (x.greenMax == y.greenMax) &&		\
			    (x.blueMax == y.blueMax) &&			\
			    (x.redShift == y.redShift) &&		\
			    (x.greenShift == y.greenShift) &&		\
			    (x.blueShift == y.blueShift))))

const rfbPixelFormat vnc8bitFormat = {8, 8, 0, 1, 7,7,3, 0,3,6,0,0};
const rfbPixelFormat vnc16bitFormat = {16, 16, 0, 1, 63, 31, 31, 0,6,11,0,0};


// *************************************************************************
//  A Client connection involves two threads - the main one which sets up
//  connections and processes window messages and inputs, and a 
//  client-specific one which receives, decodes and draws output data 
//  from the remote server.
//  This first section contains bits which are generally called by the main
//  program thread.
// *************************************************************************

ClientConnection::ClientConnection(WinVNCClient&rWinVNCClient) 
:m_rWinVNCClient(rWinVNCClient)
,rcMask(NULL)
,rcSource(NULL)
{
	Init();
}

ClientConnection::ClientConnection(WinVNCClient&rWinVNCClient,SOCKET sock) 
:m_rWinVNCClient(rWinVNCClient)
,rcMask(NULL)
,rcSource(NULL)
{
	Init();
	m_sock = sock;
	m_serverInitiated = true;
}


void ClientConnection::Init(void)
{
	m_hwnd = NULL;
	m_hwnd1 = NULL;
	m_hwndscroll = NULL;

	m_desktopName = NULL;
	m_serverInitiated = false;
	m_netbuf = NULL;
	m_netbufsize = 0;
	m_zlibbuf = NULL;
	m_zlibbufsize = 0;
	m_hwndNextViewer = NULL;	

	m_dormant = false;
	m_hBitmapDC = NULL;
	m_hBitmap = NULL;
	m_hPalette = NULL;
	m_passwdSet = false;


	m_tightVncProtocol=false;
	m_enableFileTransfers = false;
	m_fileTransferDialogShown = false;

	m_sock = INVALID_SOCKET;
	m_bKillThread = false;
	m_threadStarted = true;
	m_running = false;
	m_pendingFormatChange = false;

	m_hScrollPos = 0; m_vScrollPos = 0;

	m_waitingOnEmulateTimer = false;
	m_emulatingMiddleButton = false;

	m_decompStreamInited = false;

	m_decompStreamRaw.total_in = ZLIBHEX_DECOMP_UNINITED;
	m_decompStreamEncoded.total_in = ZLIBHEX_DECOMP_UNINITED;

	for (int i = 0; i < 4; i++)
		m_tightZlibStreamActive[i] = false;

	prevCursorSet = false;
	rcCursorX = 0;
	rcCursorY = 0;

	// Create a buffer for various network operations
	CheckBufferSize(INITIALNETBUFSIZE);
}

void ClientConnection::InitCapabilities()
{
	// Supported authentication methods
	m_authCaps.Add(rfbAuthNone, rfbStandardVendor, sig_rfbAuthNone,
				   "No authentication");
	m_authCaps.Add(rfbAuthVNC, rfbStandardVendor, sig_rfbAuthVNC,
				   "Standard VNC password authentication");

	// Supported encoding types
	m_encodingCaps.Add(rfbEncodingCopyRect, rfbStandardVendor,
					   sig_rfbEncodingCopyRect, "Standard CopyRect encoding");
	m_encodingCaps.Add(rfbEncodingRRE, rfbStandardVendor,
					   sig_rfbEncodingRRE, "Standard RRE encoding");
	m_encodingCaps.Add(rfbEncodingCoRRE, rfbStandardVendor,
					   sig_rfbEncodingCoRRE, "Standard CoRRE encoding");
	m_encodingCaps.Add(rfbEncodingHextile, rfbStandardVendor,
					   sig_rfbEncodingHextile, "Standard Hextile encoding");
	m_encodingCaps.Add(rfbEncodingZlib, rfbTridiaVncVendor,
					   sig_rfbEncodingZlib, "Zlib encoding from TridiaVNC");
	m_encodingCaps.Add(rfbEncodingZlibHex, rfbTridiaVncVendor,
					   sig_rfbEncodingZlibHex, "ZlibHex encoding from TridiaVNC");
	//m_encodingCaps.Add(rfbEncodingTight, rfbTightVncVendor,
	//				   sig_rfbEncodingTight, "Tight encoding by Constantin Kaplinsky");

	// Supported "fake" encoding types
	m_encodingCaps.Add(rfbEncodingCompressLevel0, rfbTightVncVendor,
					   sig_rfbEncodingCompressLevel0, "Compression level");
	m_encodingCaps.Add(rfbEncodingQualityLevel0, rfbTightVncVendor,
					   sig_rfbEncodingQualityLevel0, "JPEG quality level");
	m_encodingCaps.Add(rfbEncodingXCursor, rfbTightVncVendor,
					   sig_rfbEncodingXCursor, "X-style cursor shape update");
	m_encodingCaps.Add(rfbEncodingRichCursor, rfbTightVncVendor,
					   sig_rfbEncodingRichCursor, "Rich-color cursor shape update");
	m_encodingCaps.Add(rfbEncodingPointerPos, rfbTightVncVendor,
					   sig_rfbEncodingPointerPos, "Pointer position update");
	m_encodingCaps.Add(rfbEncodingLastRect, rfbTightVncVendor,
					   sig_rfbEncodingLastRect, "LastRect protocol extension");
	m_encodingCaps.Add(rfbEncodingNewFBSize, rfbTightVncVendor,
					   sig_rfbEncodingNewFBSize, "Framebuffer size change");
}

// 
// Run() creates the connection if necessary, does the initial negotiations
// and then starts the thread running which does the output (update) processing.
// If Run throws an Exception, the caller must delete the ClientConnection object.
//

void ClientConnection::Run()
{
	SetSocketOptions();

	// Set up windows etc 
	CreateDisplay();

	// This starts the worker thread.
	// The rest of the processing continues in run_undetached.
	start_undetached();
}

static WNDCLASS wndclass;	// FIXME!

void ClientConnection::CreateDisplay() 
{
	// Create the window
	
	WNDCLASS wndclass;
	
	wndclass.style			= 0;
	wndclass.lpfnWndProc	= ClientConnection::Proc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= g_AppInstance;
	wndclass.hIcon			= (HICON)LoadIcon(g_AppInstance,
												MAKEINTRESOURCE(IDI_MAINICON));
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetSysColorBrush(COLOR_BTNFACE);
    wndclass.lpszMenuName	= (LPCTSTR)NULL;
	wndclass.lpszClassName	= VWR_WND_CLASS_NAME;

	RegisterClass(&wndclass);

	wndclass.style			= 0;
	wndclass.lpfnWndProc	= ClientConnection::ScrollProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= g_AppInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName	= (LPCTSTR)NULL;
	wndclass.lpszClassName	= "ScrollClass";

	RegisterClass(&wndclass);
	 
	wndclass.style			= 0;
	wndclass.lpfnWndProc	= ClientConnection::Proc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= g_AppInstance;
	wndclass.hIcon			= NULL;
	switch (m_opts.m_localCursor) {
	case NOCURSOR:
		wndclass.hCursor	= LoadCursor(g_AppInstance, 
										MAKEINTRESOURCE(IDC_NOCURSOR));
		break;
	case SMALLCURSOR:
		wndclass.hCursor	= LoadCursor(g_AppInstance, 
										MAKEINTRESOURCE(IDC_SMALLDOT));
		break;
	case NORMALCURSOR:
		wndclass.hCursor	=LoadCursor(NULL,IDC_ARROW);
		break;
	case DOTCURSOR:
	default:
		wndclass.hCursor	= LoadCursor(g_AppInstance, 
										MAKEINTRESOURCE(IDC_DOTCURSOR));
	}
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName	= (LPCTSTR)NULL;
	wndclass.lpszClassName	= "ChildClass";

	RegisterClass(&wndclass);
	
	m_hwnd1 = CreateWindow(VWR_WND_CLASS_NAME,
			      _T("VNCviewer"),
			      WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_SIZEBOX|
				  WS_MINIMIZEBOX|WS_MAXIMIZEBOX|
				  WS_CLIPCHILDREN,
			      CW_USEDEFAULT,
			      CW_USEDEFAULT,
			      CW_USEDEFAULT,       // x-size
			      CW_USEDEFAULT,       // y-size
			      NULL,                // Parent handle
			      NULL,                // Menu handle
			      g_AppInstance,
			      NULL);
	SetWindowLong(m_hwnd1, GWL_USERDATA, (LONG) this);
	SetWindowLong(m_hwnd1, GWL_WNDPROC, (LONG)ClientConnection::WndProc1);
	ShowWindow(m_hwnd1, SW_HIDE);

	m_hwndscroll = CreateWindow("ScrollClass",
			      NULL,
			      WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER,
			      CW_USEDEFAULT,
			      CW_USEDEFAULT,
			      CW_USEDEFAULT,       // x-size
			      CW_USEDEFAULT,       // y-size
			      m_hwnd1,                // Parent handle
			      NULL,                // Menu handle
			      g_AppInstance,
			      NULL);
	SetWindowLong(m_hwndscroll, GWL_USERDATA, (LONG) this);
	ShowWindow(m_hwndscroll, SW_HIDE);
	
	// Create a memory DC which we'll use for drawing to
	// the local framebuffer
	m_hBitmapDC = CreateCompatibleDC(NULL);

	// Set a suitable palette up
	if (GetDeviceCaps(m_hBitmapDC, RASTERCAPS) & RC_PALETTE) {
		BYTE buf[sizeof(LOGPALETTE)+216*sizeof(PALETTEENTRY)];
		LOGPALETTE *plp = (LOGPALETTE *) buf;
		int pepos = 0;
		for (int r = 5; r >= 0; r--) {
			for (int g = 5; g >= 0; g--) {
				for (int b = 5; b >= 0; b--) {
					plp->palPalEntry[pepos].peRed   = r * 255 / 5; 	
					plp->palPalEntry[pepos].peGreen = g * 255 / 5;
					plp->palPalEntry[pepos].peBlue  = b * 255 / 5;
					plp->palPalEntry[pepos].peFlags  = NULL;
					pepos++;
				}
			}
		}
		plp->palVersion = 0x300;
		plp->palNumEntries = 216;
		m_hPalette = CreatePalette(plp);
	}

	// Add stuff to System menu
	//HMENU hsysmenu = GetSystemMenu(m_hwnd1, FALSE);
	//AppendMenu(hsysmenu, MF_SEPARATOR, NULL, NULL);
	//AppendMenu(hsysmenu, MF_STRING, ID_FULLSCREEN,
	//		   _T("&Full screen\tCtrl-Alt-Shift-F"));
	//AppendMenu(hsysmenu, MF_SEPARATOR, NULL, NULL);
	//AppendMenu(hsysmenu, MF_STRING, ID_CONN_CTLALTDEL,
	//		   _T("Ctrl-Alt-&Del"));
	//AppendMenu(hsysmenu, MF_STRING, ID_CONN_CTLESC,
	//		   _T("Ctrl-Esc"));

	DrawMenuBar(m_hwnd1);


	m_hwnd = CreateWindow("ChildClass",
			      NULL,
			      WS_CHILD | WS_CLIPSIBLINGS,
			      CW_USEDEFAULT,
			      CW_USEDEFAULT,
			      CW_USEDEFAULT,       // x-size
			      CW_USEDEFAULT,	   // y-size
			      m_hwndscroll,             // Parent handle
			      NULL,                // Menu handle
			      g_AppInstance,
			      NULL);

	g_HotKeys.SetWindow(m_hwnd1);
    ShowWindow(m_hwnd, SW_HIDE);
		
	SetWindowLong(m_hwnd, GWL_USERDATA, (LONG) this);
	SetWindowLong(m_hwnd, GWL_WNDPROC, (LONG)ClientConnection::WndProc);
	
#ifndef _WIN32_WCE
	// We want to know when the clipboard changes, so
	// insert ourselves in the viewer chain. But doing
	// this will cause us to be notified immediately of
	// the current state.
	// We don't want to send that.
	m_initialClipboardSeen = false;
	m_hwndNextViewer = SetClipboardViewer(m_hwnd);
#endif
}


void ClientConnection::SwitchOffKey()
{
	SendKeyEvent(XK_Alt_L,     false);
	SendKeyEvent(XK_Control_L, false);
	SendKeyEvent(XK_Shift_L,   false);
	SendKeyEvent(XK_Alt_R,     false);
	SendKeyEvent(XK_Control_R, false);
	SendKeyEvent(XK_Shift_R,   false);
}

void ClientConnection::SetSocketOptions() {
	// Disable Nagle's algorithm
	BOOL nodelayval = TRUE;
	if (setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (const char *) &nodelayval, sizeof(BOOL)))
		throw WarningException("Error disabling Nagle's algorithm");
}

void ClientConnection::SendClientInit()
{
    rfbClientInitMsg ci;
	ci.shared = m_opts.m_Shared;

    WriteExact((char *)&ci, sz_rfbClientInitMsg);
}

void ClientConnection::ReadServerInit()
{
    ReadExact((char *)&m_si, sz_rfbServerInitMsg);

    m_si.framebufferWidth = Swap16IfLE(m_si.framebufferWidth);
    m_si.framebufferHeight = Swap16IfLE(m_si.framebufferHeight);
    m_si.format.redMax = Swap16IfLE(m_si.format.redMax);
    m_si.format.greenMax = Swap16IfLE(m_si.format.greenMax);
    m_si.format.blueMax = Swap16IfLE(m_si.format.blueMax);
    m_si.nameLength = Swap32IfLE(m_si.nameLength);
	
    m_desktopName = new TCHAR[m_si.nameLength + 2];

#ifdef UNDER_CE
    char *deskNameBuf = new char[m_si.nameLength + 2];

	ReadString(deskNameBuf, m_si.nameLength);
    
	MultiByteToWideChar( CP_ACP,   MB_PRECOMPOSED, 
			     deskNameBuf, m_si.nameLength,
			     m_desktopName, m_si.nameLength+1);
    delete deskNameBuf;
#else
    ReadString(m_desktopName, m_si.nameLength);
#endif

	std::string strText=m_rWinVNCClient.GetPeerUserName();
	if (strText.size()==0)
	{
		strText=m_desktopName;
	}
    
	SetWindowText(m_hwnd1, strText.c_str());	

	SetWindowText(m_hwnd1, strText.c_str());	

	SizeWindow(true);
}

//
// In protocols 3.7t/3.8t, the server informs us about supported
// protocol messages and encodings. Here we read this information.
//

void ClientConnection::ReadInteractionCaps()
{
	// Read the counts of list items following
	rfbInteractionCapsMsg intr_caps;
	ReadExact((char *)&intr_caps, sz_rfbInteractionCapsMsg);
	intr_caps.nServerMessageTypes = Swap16IfLE(intr_caps.nServerMessageTypes);
	intr_caps.nClientMessageTypes = Swap16IfLE(intr_caps.nClientMessageTypes);
	intr_caps.nEncodingTypes = Swap16IfLE(intr_caps.nEncodingTypes);

	// Read the lists of server- and client-initiated messages
	ReadCapabilityList(&m_serverMsgCaps, intr_caps.nServerMessageTypes);
	ReadCapabilityList(&m_clientMsgCaps, intr_caps.nClientMessageTypes);
	ReadCapabilityList(&m_encodingCaps, intr_caps.nEncodingTypes);
}

//
// Read the list of rfbCapabilityInfo structures and enable corresponding
// capabilities in the specified container. The count argument specifies how
// many records to read from the socket.
//

void ClientConnection::ReadCapabilityList(CapsContainer *caps, int count)
{
	rfbCapabilityInfo msginfo;
	for (int i = 0; i < count; i++) {
		ReadExact((char *)&msginfo, sz_rfbCapabilityInfo);
		msginfo.code = Swap32IfLE(msginfo.code);
		caps->Enable(&msginfo);
	}
}

void ClientConnection::SizeWindow(bool centered)
{
	// Find how large the desktop work area is
	RECT workrect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrect, 0);
	int workwidth = workrect.right -  workrect.left;
	int workheight = workrect.bottom - workrect.top;

	RECT fullwinrect;
	
	if (m_opts.m_scaling) {
		SetRect(&fullwinrect, 0, 0,
				m_si.framebufferWidth * m_opts.m_scale_num / m_opts.m_scale_den,
				m_si.framebufferHeight * m_opts.m_scale_num / m_opts.m_scale_den);
	} else {
		SetRect(&fullwinrect, 0, 0,
				m_si.framebufferWidth, m_si.framebufferHeight);
	}	

	AdjustWindowRectEx(&fullwinrect, 
			   GetWindowLong(m_hwnd, GWL_STYLE ), 
			   FALSE, GetWindowLong(m_hwnd, GWL_EXSTYLE));

	m_fullwinwidth = fullwinrect.right - fullwinrect.left;
	m_fullwinheight = fullwinrect.bottom - fullwinrect.top;

	AdjustWindowRectEx(&fullwinrect, 
			   GetWindowLong(m_hwndscroll, GWL_STYLE ) & ~WS_HSCROLL & 
			   ~WS_VSCROLL & ~WS_BORDER, 
			   FALSE, GetWindowLong(m_hwndscroll, GWL_EXSTYLE));
	AdjustWindowRectEx(&fullwinrect, 
			   GetWindowLong(m_hwnd1, GWL_STYLE ), 
			   FALSE, GetWindowLong(m_hwnd1, GWL_EXSTYLE));


	m_winwidth  = min(fullwinrect.right - fullwinrect.left,  workwidth);
	m_winheight = min(fullwinrect.bottom - fullwinrect.top, workheight);
	if ((fullwinrect.right - fullwinrect.left > workwidth) &&
		(workheight - m_winheight >= 16)) {
		m_winheight = m_winheight + 16;
	} 
	if ((fullwinrect.bottom - fullwinrect.top > workheight) && 
		(workwidth - m_winwidth >= 16)) {
		m_winwidth = m_winwidth + 16;
	}

	int x,y;
	WINDOWPLACEMENT winplace;
	winplace.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(m_hwnd1, &winplace);
	if (centered) {
		x = (workwidth - m_winwidth) / 2;		
		y = (workheight - m_winheight) / 2;		
	} else {
		// Try to preserve current position if possible
		GetWindowPlacement(m_hwnd1, &winplace);
		if ((winplace.showCmd == SW_SHOWMAXIMIZED) || (winplace.showCmd == SW_SHOWMINIMIZED)) {
			x = winplace.rcNormalPosition.left;
			y = winplace.rcNormalPosition.top;
		} else {
			RECT tmprect;
			GetWindowRect(m_hwnd1, &tmprect);
			x = tmprect.left;
			y = tmprect.top;
		}
		if (x + m_winwidth > workrect.right)
			x = workrect.right - m_winwidth;
		if (y + m_winheight > workrect.bottom)
			y = workrect.bottom - m_winheight;
	}
	winplace.rcNormalPosition.top = y;
	winplace.rcNormalPosition.left = x;
	winplace.rcNormalPosition.right = x + m_winwidth;
	winplace.rcNormalPosition.bottom = y + m_winheight;
	SetWindowPlacement(m_hwnd1, &winplace);
	SetForegroundWindow(m_hwnd1);
	PositionChildWindow();
}

void ClientConnection::PositionChildWindow()
{	
	RECT rparent;
	GetClientRect(m_hwnd1, &rparent);
	
	int parentwidth = rparent.right - rparent.left;
	int parentheight = rparent.bottom - rparent.top; 
				
	
	SetWindowPos(m_hwndscroll, HWND_TOP, rparent.left - 1, rparent.top - 1,
					parentwidth + 2, parentheight + 2, SWP_SHOWWINDOW);
	if (!m_opts.m_FitWindow) {
		if (InFullScreenMode()) {				
			ShowScrollBar(m_hwndscroll, SB_HORZ, FALSE);
			ShowScrollBar(m_hwndscroll, SB_VERT, FALSE);
		} else {
			ShowScrollBar(m_hwndscroll, SB_VERT, parentheight < m_fullwinheight);
			ShowScrollBar(m_hwndscroll, SB_HORZ, parentwidth  < m_fullwinwidth);
			GetClientRect(m_hwndscroll, &rparent);	
			parentwidth = rparent.right - rparent.left;
			parentheight = rparent.bottom - rparent.top;
			ShowScrollBar(m_hwndscroll, SB_VERT, parentheight < m_fullwinheight);
			ShowScrollBar(m_hwndscroll, SB_HORZ, parentwidth  < m_fullwinwidth);
			GetClientRect(m_hwndscroll, &rparent);	
			parentwidth = rparent.right - rparent.left;
			parentheight = rparent.bottom - rparent.top;		
		}
	} else {
		if (!IsIconic(m_hwnd1)) {
			ShowScrollBar(m_hwndscroll, SB_HORZ, FALSE);
			ShowScrollBar(m_hwndscroll, SB_VERT, FALSE);
			GetClientRect(m_hwndscroll, &rparent);	
			parentwidth = rparent.right - rparent.left;
			parentheight = rparent.bottom - rparent.top;
			if ((parentwidth < 1) || (parentheight < 1))
				return;
			RECT fullwinrect;		
			int den = max(m_si.framebufferWidth * 100 / parentwidth,
							m_si.framebufferHeight * 100 / parentheight);
			SetRect(&fullwinrect, 0, 0, (m_si.framebufferWidth * 100 + den - 1) / den,
					(m_si.framebufferHeight * 100 + den - 1) / den);						
			while ((fullwinrect.right - fullwinrect.left > parentwidth) ||
					(fullwinrect.bottom - fullwinrect.top > parentheight)) {
				den++;
				SetRect(&fullwinrect, 0, 0, (m_si.framebufferWidth * 100 + den - 1) / den,
					(m_si.framebufferHeight * 100 + den - 1) / den);								
			}

			m_opts.m_scale_num = 100;
			m_opts.m_scale_den = den;
				
			m_opts.FixScaling();

			m_fullwinwidth = fullwinrect.right - fullwinrect.left;
			m_fullwinheight = fullwinrect.bottom - fullwinrect.top;
		}
	}

	
	int x, y;
	if (parentwidth  > m_fullwinwidth) {
		x = (parentwidth - m_fullwinwidth) / 2;
	} else {
		x = rparent.left;
	}
	if (parentheight > m_fullwinheight) {
		y = (parentheight - m_fullwinheight) / 2;
	} else {
		y = rparent.top;
	}
	
	SetWindowPos(m_hwnd, HWND_TOP, x, y,
					min(parentwidth, m_fullwinwidth),
					min(parentheight, m_fullwinheight),
					SWP_SHOWWINDOW);

	m_cliwidth = min( (int)parentwidth, (int)m_fullwinwidth);
	m_cliheight = min( (int)parentheight, (int)m_fullwinheight);

	m_hScrollMax = m_fullwinwidth;
	m_vScrollMax = m_fullwinheight;
           
		int newhpos, newvpos;
	if (!m_opts.m_FitWindow) {
		newhpos = max(0, min(m_hScrollPos, 
								 m_hScrollMax - max(m_cliwidth, 0)));
		newvpos = max(0, min(m_vScrollPos, 
				                 m_vScrollMax - max(m_cliheight, 0)));
	} else {
		newhpos = 0;
		newvpos = 0;
	}
	RECT clichild;
	GetClientRect(m_hwnd, &clichild);
	ScrollWindowEx(m_hwnd, m_hScrollPos-newhpos, m_vScrollPos-newvpos,
					NULL, &clichild, NULL, NULL,  SW_INVALIDATE);
								
	m_hScrollPos = newhpos;
	m_vScrollPos = newvpos;
	if (!m_opts.m_FitWindow) {
		UpdateScrollbars();
	} else {
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
	UpdateWindow(m_hwnd);
}

void ClientConnection::CreateLocalFramebuffer() {
	omni_mutex_lock l(m_bitmapdcMutex);

	// Remove old bitmap object if it already exists
	bool bitmapExisted = false;
	if (m_hBitmap != NULL) {
		DeleteObject(m_hBitmap);
		bitmapExisted = true;
	}

	// We create a bitmap which has the same pixel characteristics as
	// the local display, in the hope that blitting will be faster.
	
	TempDC hdc(m_hwnd);
	m_hBitmap = ::CreateCompatibleBitmap(hdc, m_si.framebufferWidth,
										 m_si.framebufferHeight);
	
	if (m_hBitmap == NULL)
		throw WarningException("Error creating local image of screen.");
	
	// Select this bitmap into the DC with an appropriate palette
	ObjectSelector b(m_hBitmapDC, m_hBitmap);
	PaletteSelector p(m_hBitmapDC, m_hPalette);
	
	// Put a "please wait" message up initially
	RECT rect;
	SetRect(&rect, 0,0, m_si.framebufferWidth, m_si.framebufferHeight);
	COLORREF bgcol = RGB(0,0,0);//xmc RGB(0xcc, 0xcc, 0xcc);
	FillSolidRect(&rect, bgcol);
	
	//xmc
	//if (!bitmapExisted) {
	//	COLORREF oldbgcol  = SetBkColor(m_hBitmapDC, bgcol);
	//	COLORREF oldtxtcol = SetTextColor(m_hBitmapDC, RGB(255,0,64));
	//	rect.right = m_si.framebufferWidth / 2 + 10;
	//	rect.bottom = m_si.framebufferHeight / 2 + 100;
	//	rect.top = rect.bottom -20;
	//	rect.left = rect.right -100;
	//
	//	DrawText (m_hBitmapDC, _T("Loading ..."), -1, &rect,
	//			  DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	//	SetBkColor(m_hBitmapDC, oldbgcol);
	//	SetTextColor(m_hBitmapDC, oldtxtcol);
	//}
	//
	
	InvalidateRect(m_hwnd, NULL, FALSE);
}

void ClientConnection::SetupPixelFormat() {
	// Have we requested a reduction to 8-bit?
    if (m_opts.m_Use8Bit) {		
      
		m_myFormat = vnc8bitFormat;
    
		// We don't support colormaps so we'll ask the server to convert
    } else if (!m_si.format.trueColour) {
        
        // We'll just request a standard 16-bit truecolor
        m_myFormat = vnc16bitFormat;
        
    } else {

		// Normally we just use the sever's format suggestion
		m_myFormat = m_si.format;

		// It's silly requesting more bits than our current display has, but
		// in fact it doesn't usually amount to much on the network.
		// Windows doesn't support 8-bit truecolour.
		// If our display is palette-based, we want more than 8 bit anyway,
		// unless we're going to start doing palette stuff at the server.
		// So the main use would be a 24-bit true-colour desktop being viewed
		// on a 16-bit true-colour display, and unless you have lots of images
		// and hence lots of raw-encoded stuff, the size of the pixel is not
		// going to make much difference.
		//   We therefore don't bother with any restrictions, but here's the
		// start of the code if we wanted to do it.

		if (false) {
		
			// Get a DC for the root window
			TempDC hrootdc(NULL);
			int localBitsPerPixel = GetDeviceCaps(hrootdc, BITSPIXEL);
			int localRasterCaps	  = GetDeviceCaps(hrootdc, RASTERCAPS);
			
			// If we're using truecolor, and the server has more bits than we do
			if ( (localBitsPerPixel > m_myFormat.depth) && 
				! (localRasterCaps & RC_PALETTE)) {
				m_myFormat.depth = localBitsPerPixel;

				// create a bitmap compatible with the current display
				// call GetDIBits twice to get the colour info.
				// set colour masks and shifts
				
			}
		}
	}

	// The endian will be set before sending
}

void ClientConnection::SetFormatAndEncodings()
{
	// Set pixel format to myFormat

	rfbSetPixelFormatMsg spf;

	spf.type = rfbSetPixelFormat;
	spf.format = m_myFormat;
	spf.format.redMax = Swap16IfLE(spf.format.redMax);
	spf.format.greenMax = Swap16IfLE(spf.format.greenMax);
	spf.format.blueMax = Swap16IfLE(spf.format.blueMax);
	spf.format.bigEndian = 0;

	WriteExact((char *)&spf, sz_rfbSetPixelFormatMsg);

	// The number of bytes required to hold at least one pixel.
	m_minPixelBytes = (m_myFormat.bitsPerPixel + 7) >> 3;

	// Set encodings
	char buf[sz_rfbSetEncodingsMsg + MAX_ENCODINGS * 4];
	rfbSetEncodingsMsg *se = (rfbSetEncodingsMsg *)buf;
	CARD32 *encs = (CARD32 *)(&buf[sz_rfbSetEncodingsMsg]);
	int len = 0;

	se->type = rfbSetEncodings;
	se->nEncodings = 0;

	bool useCompressLevel = false;
	int i;

	// Put the preferred encoding first, and change it if the
	// preferred encoding is not actually usable.
	for (i = LASTENCODING; i >= rfbEncodingRaw; i--)
	{
		if (m_opts.m_PreferredEncoding == i) {
			if (m_opts.m_UseEnc[i]) {
				encs[se->nEncodings++] = Swap32IfLE(i);
				if ( i == rfbEncodingZlib ||
					 //i == rfbEncodingTight ||
					 i == rfbEncodingZlibHex ) {
					useCompressLevel = true;
				}
			} else {
				m_opts.m_PreferredEncoding--;
			}
		}
	}

	// Now we go through and put in all the other encodings in order.
	// We do rather assume that the most recent encoding is the most
	// desirable!
	for (i = LASTENCODING; i >= rfbEncodingRaw; i--)
	{
		if ( (m_opts.m_PreferredEncoding != i) &&
			 (m_opts.m_UseEnc[i]))
		{
			encs[se->nEncodings++] = Swap32IfLE(i);
			if ( i == rfbEncodingZlib ||
				 //i == rfbEncodingTight ||
				 i == rfbEncodingZlibHex ) {
				useCompressLevel = true;
			}
		}
	}

	// Request desired compression level if applicable
	if ( useCompressLevel && m_opts.m_useCompressLevel &&
		 m_opts.m_compressLevel >= 0 &&
		 m_opts.m_compressLevel <= 9) {
		encs[se->nEncodings++] = Swap32IfLE( rfbEncodingCompressLevel0 +
											 m_opts.m_compressLevel );
	}

	// Request cursor shape updates if enabled by user
	if (m_opts.m_requestShapeUpdates) {
		encs[se->nEncodings++] = Swap32IfLE(rfbEncodingXCursor);
		encs[se->nEncodings++] = Swap32IfLE(rfbEncodingRichCursor);
		if (!m_opts.m_ignoreShapeUpdates)
			encs[se->nEncodings++] = Swap32IfLE(rfbEncodingPointerPos);
	}

	// Request JPEG quality level if JPEG compression was enabled by user
	if ( m_opts.m_enableJpegCompression &&
		 m_opts.m_jpegQualityLevel >= 0 &&
		 m_opts.m_jpegQualityLevel <= 9) {
		encs[se->nEncodings++] = Swap32IfLE( rfbEncodingQualityLevel0 +
											 m_opts.m_jpegQualityLevel );
	}

	// Notify the server that we support LastRect and NewFBSize encodings
	encs[se->nEncodings++] = Swap32IfLE(rfbEncodingLastRect);
	encs[se->nEncodings++] = Swap32IfLE(rfbEncodingNewFBSize);

	len = sz_rfbSetEncodingsMsg + se->nEncodings * 4;

	se->nEncodings = Swap16IfLE(se->nEncodings);

	WriteExact((char *) buf, len);
}

// Closing down the connection.
// Close the socket, kill the thread.
void ClientConnection::KillThread()
{
	m_bKillThread = true;
	m_running = false;

	if (m_sock != INVALID_SOCKET) {
		shutdown(m_sock, SD_BOTH);
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}

void ClientConnection::DestroyWindow(void)
{
	if (m_hwnd1 != 0)
	{
		::DestroyWindow(m_hwnd1);
		m_hwnd1=NULL;
	}
}

void ClientConnection::CloseWindow(void)
{
	if (m_hwnd1 != 0)
	{
		::PostMessage(m_hwnd1,WM_CLOSE,0,0);
	}
}

// Get the RFB options from another connection.
void ClientConnection::CopyOptions(ClientConnection *source)
{
	this->m_opts = source->m_opts;
}

ClientConnection::~ClientConnection()
{
	m_rWinVNCClient.OnClientConnectionReleased();

	if (m_hwnd1 != 0)
		::DestroyWindow(m_hwnd1);

	if (m_sock != INVALID_SOCKET) {
		shutdown(m_sock, SD_BOTH);
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}

	if (m_desktopName != NULL)
	{
		delete [] m_desktopName;
		m_desktopName=NULL;
	}

	if (m_netbuf!=NULL)
	{
		delete [] m_netbuf;
		m_netbuf=NULL;
	}

	DeleteDC(m_hBitmapDC);
	if (m_hBitmap != NULL)
		DeleteObject(m_hBitmap);
	if (m_hPalette != NULL)
		DeleteObject(m_hPalette);

	if (rcMask!=NULL)
	{
		delete [] rcMask;
		rcMask=NULL;
	}
	if (rcSource!=NULL)
	{
		delete [] rcSource;
		rcSource=NULL;
	}
}

// You can specify a dx & dy outside the limits; the return value will
// tell you whether it actually scrolled.
bool ClientConnection::ScrollScreen(int dx, int dy) 
{
	dx = max(dx, -m_hScrollPos);
	//dx = min(dx, m_hScrollMax-(m_cliwidth-1)-m_hScrollPos);
	dx = min(dx, m_hScrollMax-(m_cliwidth)-m_hScrollPos);
	dy = max(dy, -m_vScrollPos);
	//dy = min(dy, m_vScrollMax-(m_cliheight-1)-m_vScrollPos);
	dy = min(dy, m_vScrollMax-(m_cliheight)-m_vScrollPos);
	if (dx || dy) {
		m_hScrollPos += dx;
		m_vScrollPos += dy;
		RECT clirect;
		GetClientRect(m_hwnd, &clirect);
		ScrollWindowEx(m_hwnd, -dx, -dy,
				NULL, &clirect, NULL, NULL,  SW_INVALIDATE);
		UpdateScrollbars();
		UpdateWindow(m_hwnd);
		return true;
	}
	return false;
}

// Process windows messages
LRESULT CALLBACK ClientConnection::ScrollProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{	// This is a static method, so we don't know which instantiation we're 
	// dealing with.  But we've stored a 'pseudo-this' in the window data.
	ClientConnection *_this = (ClientConnection *) GetWindowLong(hwnd, GWL_USERDATA);
		
	switch (iMsg) {
	case WM_HSCROLL:
		{				
			int dx = 0;
			int pos = HIWORD(wParam);
			switch (LOWORD(wParam)) {
			case SB_LINEUP:
				dx = - 2; break;
			case SB_LINEDOWN:
				dx = 2; break;
			case SB_PAGEUP:
				dx = _this->m_cliwidth * -1/4; break;
			case SB_PAGEDOWN:
				dx = _this->m_cliwidth * 1/4; break;
			case SB_THUMBPOSITION:
				dx = pos - _this->m_hScrollPos;
			case SB_THUMBTRACK:
				dx = pos - _this->m_hScrollPos;
			}
			if (!_this->m_opts.m_FitWindow) 
				_this->ScrollScreen(dx,0);
			return 0;
		}
	case WM_VSCROLL:
		{
			int dy = 0;
			int pos = HIWORD(wParam);
			switch (LOWORD(wParam)) {
			case SB_LINEUP:
				dy =  - 2; break;
			case SB_LINEDOWN:
				dy = 2; break;
			case SB_PAGEUP:
				dy =  _this->m_cliheight * -1/4; break;
			case SB_PAGEDOWN:
				dy = _this->m_cliheight * 1/4; break;
			case SB_THUMBPOSITION:
				dy = pos - _this->m_vScrollPos;
			case SB_THUMBTRACK:
				dy = pos - _this->m_vScrollPos;
			}
			if (!_this->m_opts.m_FitWindow) 
				_this->ScrollScreen(0,dy);
			return 0;
		}
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
LRESULT CALLBACK ClientConnection::WndProc1(HWND hwnd, UINT iMsg, 
					   WPARAM wParam, LPARAM lParam) 
{
	
	// This is a static method, so we don't know which instantiation we're 
	// dealing with.  But we've stored a 'pseudo-this' in the window data.
	ClientConnection *_this = (ClientConnection *) GetWindowLong(hwnd, GWL_USERDATA);
		
	switch (iMsg) {
	case WM_SETFOCUS:		
		g_HotKeys.SetWindow(hwnd);
		SetFocus(_this->m_hwnd);
		return 0;
	case WM_COMMAND:
	case WM_SYSCOMMAND:
		switch (LOWORD(wParam)) {
		case SC_MINIMIZE:
			_this->SetDormant(true);
			break;
		case SC_RESTORE:			
			_this->SetDormant(false);
			break;
		case ID_FULLSCREEN:
			// Toggle full screen mode
			_this->SetFullScreenMode(!_this->InFullScreenMode());
			return 0;
		case ID_CONN_CTLESC:
			_this->SendKeyEvent(XK_Control_L, true);
			_this->SendKeyEvent(XK_Escape,     true);
			_this->SendKeyEvent(XK_Escape,     false);
			_this->SendKeyEvent(XK_Control_L, false);
			return 0;
		case ID_CONN_CTLALTDEL:
			_this->SendKeyEvent(XK_Control_L, true);
			_this->SendKeyEvent(XK_Alt_L,     true);
			_this->SendKeyEvent(XK_Delete,    true);
			_this->SendKeyEvent(XK_Delete,    false);
			_this->SendKeyEvent(XK_Alt_L,     false);
			_this->SendKeyEvent(XK_Control_L, false);
			return 0;
		}
		break;		
	case WM_KILLFOCUS:
		if ( _this->m_opts.m_ViewOnly) return 0;
		_this->SwitchOffKey();
		return 0;
	case WM_SIZE:		
		_this->PositionChildWindow();			
		return 0;	
	case WM_CLOSE:		
		// Close the worker thread as well
		_this->KillThread();
		::DestroyWindow(hwnd);
		return 0;					  
	case WM_DESTROY: 			
#ifndef UNDER_CE
		// Remove us from the clipboard viewer chain
		BOOL res = ChangeClipboardChain( _this->m_hwnd, _this->m_hwndNextViewer);
#endif
		if (_this->m_waitingOnEmulateTimer) {
			
			KillTimer(_this->m_hwnd, _this->m_emulate3ButtonsTimer);
			_this->m_waitingOnEmulateTimer = false;
		}
			
		_this->m_hwnd1 = 0;
		_this->m_hwnd = 0;

		// We are currently in the main thread.
		// The worker thread should be about to finish if
		// it hasn't already. Wait for it.
		try {
			void *p;
			_this->join(&p);  // After joining, _this is no longer valid
		} catch (omni_thread_invalid) {
		// The thread probably hasn't been started yet,
		}	
		return 0;						 
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}	

LRESULT CALLBACK ClientConnection::Proc(HWND hwnd, UINT iMsg,
										WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

LRESULT CALLBACK ClientConnection::WndProc(HWND hwnd, UINT iMsg, 
					   WPARAM wParam, LPARAM lParam) 
{	
	// This is a static method, so we don't know which instantiation we're 
	// dealing with.  But we've stored a 'pseudo-this' in the window data.
	ClientConnection *_this = (ClientConnection *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (iMsg) {
	case WM_REGIONUPDATED:
		_this->DoBlit();
		_this->SendAppropriateFramebufferUpdateRequest();		
		return 0;
	case WM_PAINT:
		_this->DoBlit();		
		return 0;
	case WM_TIMER:
		if (wParam == _this->m_emulate3ButtonsTimer) {
			_this->SubProcessPointerEvent( 
										_this->m_emulateButtonPressedX,
										 _this->m_emulateButtonPressedY,
										_this->m_emulateKeyFlags);
			KillTimer(hwnd, _this->m_emulate3ButtonsTimer);
			 _this->m_waitingOnEmulateTimer = false;
		}
		return 0; 
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		{
			if (!_this->m_running)
				return 0;
			if (GetFocus() != hwnd && GetFocus() != _this->m_hwnd1)
				return 0;
			SetFocus(hwnd);

			POINT coords;
			coords.x = LOWORD(lParam);
			coords.y = HIWORD(lParam);

			if (iMsg == WM_MOUSEWHEEL) {
				// Convert coordinates to position in our client area,
				// make sure the pointer is inside the client area.
				if ( WindowFromPoint(coords) != hwnd ||
					 !ScreenToClient(hwnd, &coords) ||
					 coords.x < 0 || coords.y < 0 ||
					 coords.x >= _this->m_cliwidth ||
					 coords.y >= _this->m_cliheight ) {
					return 0;
				}
			} else {
				// Make sure the high-order word in wParam is zero.
				wParam = MAKEWPARAM(LOWORD(wParam), 0);
			}

			if (_this->InFullScreenMode()) {
				if (_this->BumpScroll(coords.x, coords.y))
					return 0;
			}
			if ( _this->m_opts.m_ViewOnly)
				return 0;

			_this->ProcessPointerEvent(coords.x, coords.y, wParam, iMsg);
			return 0;
		}

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		{
			if (!_this->m_running) return 0;
			if ( _this->m_opts.m_ViewOnly) return 0;
            _this->ProcessKeyEvent((int) wParam, (DWORD) lParam);
			return 0;
		}
	case WM_CHAR:
	case WM_SYSCHAR:
#ifdef UNDER_CE
        {
            int key = wParam;
            // Control keys which are in the Keymap table will already
            // have been handled.
            if (key == 0x0D  ||  // return
                key == 0x20 ||   // space
                key == 0x08)     // backspace
                return 0;

            if (key < 32) key += 64;  // map ctrl-keys onto alphabet
            if (key > 32 && key < 127) {
                _this->SendKeyEvent(wParam & 0xff, true);
                _this->SendKeyEvent(wParam & 0xff, false);
            }
            return 0;
        }
#endif
	case WM_DEADCHAR:
	case WM_SYSDEADCHAR:
	  return 0;
	case WM_SETFOCUS:
		if (_this->InFullScreenMode())
			SetWindowPos(hwnd, HWND_TOPMOST, 0,0,100,100, SWP_NOMOVE | SWP_NOSIZE);
		return 0;
	// Cacnel modifiers when we lose focus
	case WM_KILLFOCUS:
		{
			if (!_this->m_running) return 0;
			if (_this->InFullScreenMode()) {
				// We must top being topmost, but we want to choose our
				// position carefully.
				HWND foreground = GetForegroundWindow();
				HWND hwndafter = NULL;
				if ((foreground == NULL) || 
					(GetWindowLong(foreground, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
					hwndafter = HWND_NOTOPMOST;
				} else {
					hwndafter = GetNextWindow(foreground, GW_HWNDNEXT); 
				}

				SetWindowPos(_this->m_hwnd1, hwndafter, 0,0,100,100, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			}
			return 0;
		}	
    case WM_QUERYNEWPALETTE:
        {
			TempDC hDC(hwnd);
			
			// Select and realize hPalette
			PaletteSelector p(hDC, _this->m_hPalette);
			InvalidateRect(hwnd, NULL, FALSE);
			UpdateWindow(hwnd);

			return TRUE;
        }

	case WM_PALETTECHANGED:
		// If this application did not change the palette, select
		// and realize this application's palette
		if ((HWND) wParam != hwnd)
		{
			// Need the window's DC for SelectPalette/RealizePalette
			TempDC hDC(hwnd);
			PaletteSelector p(hDC, _this->m_hPalette);
			// When updating the colors for an inactive window,
			// UpdateColors can be called because it is faster than
			// redrawing the client area (even though the results are
			// not as good)
				#ifndef UNDER_CE
				UpdateColors(hDC);
				#else
				InvalidateRect(hwnd, NULL, FALSE);
				UpdateWindow(hwnd);
				#endif

		}
        break;

#ifndef UNDER_CE 
		
	case WM_SETCURSOR:
		{
			// if we have the focus, let the cursor change as normal
			if (GetFocus() == hwnd) 
				break;

			// if not, set to default system cursor
			SetCursor( LoadCursor(NULL, IDC_ARROW));
			return 0;
		}

	case WM_DRAWCLIPBOARD:
		_this->ProcessLocalClipboardChange();
		return 0;

	case WM_CHANGECBCHAIN:
		{
			// The clipboard chain is changing
			HWND hWndRemove = (HWND) wParam;     // handle of window being removed 
			HWND hWndNext = (HWND) lParam;       // handle of next window in chain 
			// If next window is closing, update our pointer.
			if (hWndRemove == _this->m_hwndNextViewer)  
				_this->m_hwndNextViewer = hWndNext;  
			// Otherwise, pass the message to the next link.  
			else if (_this->m_hwndNextViewer != NULL) 
				::SendMessage(_this->m_hwndNextViewer, WM_CHANGECBCHAIN, 
				(WPARAM) hWndRemove,  (LPARAM) hWndNext );  
			return 0;
		}
	
#endif
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}


// ProcessPointerEvent handles the delicate case of emulating 3 buttons
// on a two button mouse, then passes events off to SubProcessPointerEvent.

void
ClientConnection::ProcessPointerEvent(int x, int y, DWORD keyflags, UINT msg) 
{
	if (m_opts.m_Emul3Buttons) {
		// XXX To be done:
		// If this is a left or right press, the user may be 
		// about to press the other button to emulate a middle press.
		// We need to start a timer, and if it expires without any
		// further presses, then we send the button press. 
		// If a press of the other button, or any release, comes in
		// before timer has expired, we cancel timer & take different action.
		if (m_waitingOnEmulateTimer) {
			if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP ||
				abs(x - m_emulateButtonPressedX) > m_opts.m_Emul3Fuzz ||
				abs(y - m_emulateButtonPressedY) > m_opts.m_Emul3Fuzz) {
				// if button released or we moved too far then cancel.
				// First let the remote know where the button was down
				SubProcessPointerEvent(
					m_emulateButtonPressedX, 
					m_emulateButtonPressedY, 
					m_emulateKeyFlags);
				// Then tell it where we are now
				SubProcessPointerEvent(x, y, keyflags);
			} else if (
				(msg == WM_LBUTTONDOWN && (m_emulateKeyFlags & MK_RBUTTON))
				|| (msg == WM_RBUTTONDOWN && (m_emulateKeyFlags & MK_LBUTTON)))	{
				// Triggered an emulate; remove left and right buttons, put
				// in middle one.
				DWORD emulatekeys = keyflags & ~(MK_LBUTTON|MK_RBUTTON);
				emulatekeys |= MK_MBUTTON;
				SubProcessPointerEvent(x, y, emulatekeys);
				
				m_emulatingMiddleButton = true;
			} else {
				// handle movement normally & don't kill timer.
				// just remove the pressed button from the mask.
				DWORD keymask = m_emulateKeyFlags & (MK_LBUTTON|MK_RBUTTON);
				DWORD emulatekeys = keyflags & ~keymask;
				SubProcessPointerEvent(x, y, emulatekeys);
				return;
			}
			
			// if we reached here, we don't need the timer anymore.
			KillTimer(m_hwnd, m_emulate3ButtonsTimer);
			m_waitingOnEmulateTimer = false;
		} else if (m_emulatingMiddleButton) {
			if ((keyflags & MK_LBUTTON) == 0 && (keyflags & MK_RBUTTON) == 0)
			{
				// We finish emulation only when both buttons come back up.
				m_emulatingMiddleButton = false;
				SubProcessPointerEvent(x, y, keyflags);
			} else {
				// keep emulating.
				DWORD emulatekeys = keyflags & ~(MK_LBUTTON|MK_RBUTTON);
				emulatekeys |= MK_MBUTTON;
				SubProcessPointerEvent(x, y, emulatekeys);
			}
		} else {
			// Start considering emulation if we've pressed a button
			// and the other isn't pressed.
			if ( (msg == WM_LBUTTONDOWN && !(keyflags & MK_RBUTTON))
				|| (msg == WM_RBUTTONDOWN && !(keyflags & MK_LBUTTON)))	{
				// Start timer for emulation.
				m_emulate3ButtonsTimer = 
					SetTimer(
					m_hwnd, 
					IDT_EMULATE3BUTTONSTIMER, 
					m_opts.m_Emul3Timeout, 
					NULL);
				
				if (!m_emulate3ButtonsTimer) {
					PostMessage(m_hwnd1, WM_CLOSE, 0, 0);
					return;
				}
				
				m_waitingOnEmulateTimer = true;
				
				// Note that we don't send the event here; we're batching it for
				// later.
				m_emulateKeyFlags = keyflags;
				m_emulateButtonPressedX = x;
				m_emulateButtonPressedY = y;
			} else {
				// just send event noramlly
				SubProcessPointerEvent(x, y, keyflags);
			}
		}
	} else {
		SubProcessPointerEvent(x, y, keyflags);
	}
}

// SubProcessPointerEvent takes windows positions and flags and converts 
// them into VNC ones.

inline void
ClientConnection::SubProcessPointerEvent(int x, int y, DWORD keyflags)
{
	int mask;
  
	if (m_opts.m_SwapMouse) {
		mask = ( ((keyflags & MK_LBUTTON) ? rfbButton1Mask : 0) |
				 ((keyflags & MK_MBUTTON) ? rfbButton3Mask : 0) |
				 ((keyflags & MK_RBUTTON) ? rfbButton2Mask : 0) );
	} else {
		mask = ( ((keyflags & MK_LBUTTON) ? rfbButton1Mask : 0) |
				 ((keyflags & MK_MBUTTON) ? rfbButton2Mask : 0) |
				 ((keyflags & MK_RBUTTON) ? rfbButton3Mask : 0) );
	}

	if ((short)HIWORD(keyflags) > 0) {
		mask |= rfbButton4Mask;
	} else if ((short)HIWORD(keyflags) < 0) {
		mask |= rfbButton5Mask;
	}
	
	try {
		int x_scaled =
			(x + m_hScrollPos) * m_opts.m_scale_den / m_opts.m_scale_num;
		int y_scaled =
			(y + m_vScrollPos) * m_opts.m_scale_den / m_opts.m_scale_num;

		SendPointerEvent(x_scaled, y_scaled, mask);

		if ((short)HIWORD(keyflags) != 0) {
			// Immediately send a "button-up" after mouse wheel event.
			mask &= !(rfbButton4Mask | rfbButton5Mask);
			SendPointerEvent(x_scaled, y_scaled, mask);
		}
	} catch (Exception &e) {
		e.Report();
		PostMessage(m_hwnd1, WM_CLOSE, 0, 0);
	}
}

//
// SendPointerEvent.
//

inline void
ClientConnection::SendPointerEvent(int x, int y, int buttonMask)
{
    rfbPointerEventMsg pe;

    pe.type = rfbPointerEvent;
    pe.buttonMask = buttonMask;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
	SoftCursorMove(x, y);
    pe.x = Swap16IfLE(x);
    pe.y = Swap16IfLE(y);
	WriteExact((char *)&pe, sz_rfbPointerEventMsg);
}

//
// ProcessKeyEvent
//
// Normally a single Windows key event will map onto a single RFB
// key message, but this is not always the case.  Much of the stuff
// here is to handle AltGr (=Ctrl-Alt) on international keyboards.
// Example cases:
//
//    We want Ctrl-F to be sent as:
//      Ctrl-Down, F-Down, F-Up, Ctrl-Up.
//    because there is no keysym for ctrl-f, and because the ctrl
//    will already have been sent by the time we get the F.
//
//    On German keyboards, @ is produced using AltGr-Q, which is
//    Ctrl-Alt-Q.  But @ is a valid keysym in its own right, and when
//    a German user types this combination, he doesn't mean Ctrl-@.
//    So for this we will send, in total:
//
//      Ctrl-Down, Alt-Down,   
//                 (when we get the AltGr pressed)
//
//      Alt-Up, Ctrl-Up, @-Down, Ctrl-Down, Alt-Down 
//                 (when we discover that this is @ being pressed)
//
//      Alt-Up, Ctrl-Up, @-Up, Ctrl-Down, Alt-Down
//                 (when we discover that this is @ being released)
//
//      Alt-Up, Ctrl-Up
//                 (when the AltGr is released)

inline void ClientConnection::ProcessKeyEvent(int virtkey, DWORD keyData)
{
    bool down = ((keyData & 0x80000000l) == 0);

    // if virtkey found in mapping table, send X equivalent
    // else
    //   try to convert directly to ascii
    //   if result is in range supported by X keysyms,
    //      raise any modifiers, send it, then restore mods
    //   else
    //      calculate what the ascii would be without mods
    //      send that

#ifdef _DEBUG
#ifdef UNDER_CE
	char *keyname = "";
#else
    char keyname[32];
    if (GetKeyNameText(  keyData,keyname, 31)) {
    };
#endif
#endif

	try {
		KeyActionSpec kas = m_keymap.PCtoX(virtkey, keyData);    
		
		if (kas.releaseModifiers & KEYMAP_LCONTROL) {
			SendKeyEvent(XK_Control_L, false );
		}
		if (kas.releaseModifiers & KEYMAP_LALT) {
			SendKeyEvent(XK_Alt_L, false );
		}
		if (kas.releaseModifiers & KEYMAP_RCONTROL) {
			SendKeyEvent(XK_Control_R, false );
		}
		if (kas.releaseModifiers & KEYMAP_RALT) {
			SendKeyEvent(XK_Alt_R, false );
		}
		
		for (int i = 0; kas.keycodes[i] != XK_VoidSymbol && i < MaxKeysPerKey; i++) {
			SendKeyEvent(kas.keycodes[i], down );
		}
		
		if (kas.releaseModifiers & KEYMAP_RALT) {
			SendKeyEvent(XK_Alt_R, true );
		}
		if (kas.releaseModifiers & KEYMAP_RCONTROL) {
			SendKeyEvent(XK_Control_R, true );
		}
		if (kas.releaseModifiers & KEYMAP_LALT) {
			SendKeyEvent(XK_Alt_L, false );
		}
		if (kas.releaseModifiers & KEYMAP_LCONTROL) {
			SendKeyEvent(XK_Control_L, false );
		}
	} catch (Exception &e) {
		e.Report();
		PostMessage(m_hwnd1, WM_CLOSE, 0, 0);
	}

}

//
// SendKeyEvent
//

inline void
ClientConnection::SendKeyEvent(CARD32 key, bool down)
{
    rfbKeyEventMsg ke;

    ke.type = rfbKeyEvent;
    ke.down = down ? 1 : 0;
    ke.key = Swap32IfLE(key);
    WriteExact((char *)&ke, sz_rfbKeyEventMsg);
}

#ifndef UNDER_CE
//
// SendClientCutText
//

void ClientConnection::SendClientCutText(char *str, size_t len)
{
    rfbClientCutTextMsg cct;

    cct.type = rfbClientCutText;
    cct.length = Swap32IfLE(len);
    WriteExact((char *)&cct, sz_rfbClientCutTextMsg);
	WriteExact(str, len);
}
#endif

// Copy any updated areas from the bitmap onto the screen.

inline void ClientConnection::DoBlit() 
{
	if (m_hBitmap == NULL) return;
	if (!m_running) return;
				
	// No other threads can use bitmap DC
	omni_mutex_lock l(m_bitmapdcMutex);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hwnd, &ps);

	// Select and realize hPalette
	PaletteSelector p(hdc, m_hPalette);
	ObjectSelector b(m_hBitmapDC, m_hBitmap);
			
	
	if (m_opts.m_scaling) {
		int n = m_opts.m_scale_num;
		int d = m_opts.m_scale_den;
		
		// We're about to do some scaling on these values in the StretchBlt
		// We want to make sure that they divide nicely by n so we round them
		// down and up appropriately.
		ps.rcPaint.left =   ((ps.rcPaint.left   + m_hScrollPos) / n * n)         - m_hScrollPos;
		ps.rcPaint.right =  ((ps.rcPaint.right  + m_hScrollPos + n - 1) / n * n) - m_hScrollPos;
		ps.rcPaint.top =    ((ps.rcPaint.top    + m_vScrollPos) / n * n)         - m_vScrollPos;
		ps.rcPaint.bottom = ((ps.rcPaint.bottom + m_vScrollPos + n - 1) / n * n) - m_vScrollPos;
		
		// This is supposed to give better results.  I think my driver ignores it?
		SetStretchBltMode(hdc, HALFTONE);
		// The docs say that you should call SetBrushOrgEx after SetStretchBltMode, 
		// but not what the arguments should be.
		SetBrushOrgEx(hdc, 0,0, NULL);
		
		if (!StretchBlt(
			hdc, 
			ps.rcPaint.left, 
			ps.rcPaint.top, 
			ps.rcPaint.right-ps.rcPaint.left, 
			ps.rcPaint.bottom-ps.rcPaint.top, 
			m_hBitmapDC, 
			(ps.rcPaint.left+m_hScrollPos)     * d / n, 
			(ps.rcPaint.top+m_vScrollPos)      * d / n,
			(ps.rcPaint.right-ps.rcPaint.left) * d / n, 
			(ps.rcPaint.bottom-ps.rcPaint.top) * d / n, 
			SRCCOPY)) 
		{
			// throw ErrorException("Error in blit!\n");
		};
	} else {
		if (!BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, 
			ps.rcPaint.right-ps.rcPaint.left, ps.rcPaint.bottom-ps.rcPaint.top, 
			m_hBitmapDC, ps.rcPaint.left+m_hScrollPos, ps.rcPaint.top+m_vScrollPos, SRCCOPY)) 
		{
			// throw ErrorException("Error in blit!\n");
		}
	}
	
	EndPaint(m_hwnd, &ps);
}

inline void ClientConnection::UpdateScrollbars() 
{
	// We don't update the actual scrollbar info in full-screen mode
	// because it causes them to flicker.
	bool setInfo = !InFullScreenMode();

	SCROLLINFO scri;
	scri.cbSize = sizeof(scri);
	scri.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
	scri.nMin = 0;
	scri.nMax = m_hScrollMax; 
	scri.nPage= m_cliwidth;
	scri.nPos = m_hScrollPos; 
	
	if (setInfo) 
		SetScrollInfo(m_hwndscroll, SB_HORZ, &scri, TRUE);
	
	scri.cbSize = sizeof(scri);
	scri.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
	scri.nMin = 0;
	scri.nMax = m_vScrollMax;     
	scri.nPage= m_cliheight;
	scri.nPos = m_vScrollPos; 
	
	if (setInfo) 
		SetScrollInfo(m_hwndscroll, SB_VERT, &scri, TRUE);
}

// ********************************************************************
//  Methods after this point are generally called by the worker thread.
//  They finish the initialisation, then chiefly read data from the server.
// ********************************************************************


void* ClientConnection::run_undetached(void* arg) {

	m_threadStarted = true;

	try {
		SendClientInit();
		ReadServerInit();

		// Only for protocol version 3.7t
		if (m_tightVncProtocol) {
			// Determine which protocol messages and encodings are supported.
			ReadInteractionCaps();
		}

		CreateLocalFramebuffer();
		
		SetupPixelFormat();
		
		SetFormatAndEncodings();

		SendFullFramebufferUpdateRequest();

		RealiseFullScreenMode(false);

		m_running = true;
		UpdateWindow(m_hwnd1);
		
		while (!m_bKillThread) {
			
			// Look at the type of the message, but leave it in the buffer 
			CARD8 msgType;
			{
			  omni_mutex_lock l(m_readMutex);  // we need this if we're not using ReadExact
			  int bytes = recv(m_sock, (char *) &msgType, 1, MSG_PEEK);
			  if (bytes == 0) 
			  {
			    throw WarningException(_T("Connection closed"));
			  }
			  if (bytes < 0) 
			  {
			    throw WarningException("Error while waiting for server message");
			  }
			}
				
			switch (msgType) {
			case rfbFramebufferUpdate:
				ReadScreenUpdate();
				break;
			case rfbSetColourMapEntries:
				ReadSetColourMapEntries();
				break;
			case rfbBell:
				ReadBell();
				break;
			case rfbServerCutText:
				ReadServerCutText();
				break;
			default:
				throw WarningException("Unhandled message type received!\n");
			}

		}
        

	} catch (WarningException &e) {
		m_running = false;
		PostMessage(m_hwnd1, WM_CLOSE, 0, 0);
		if (!m_bKillThread) {
			e.Report();
		}
	} catch (QuietException &e) {
		m_running = false;
		e.Report();
		PostMessage(m_hwnd1, WM_CLOSE, 0, 0);
	} 
	return this;
}


//
// Requesting screen updates from the server
//

inline void
ClientConnection::SendFramebufferUpdateRequest(int x, int y, int w, int h, bool incremental)
{
    rfbFramebufferUpdateRequestMsg fur;

    fur.type = rfbFramebufferUpdateRequest;
    fur.incremental = incremental ? 1 : 0;
    fur.x = Swap16IfLE(x);
    fur.y = Swap16IfLE(y);
    fur.w = Swap16IfLE(w);
    fur.h = Swap16IfLE(h);

    WriteExact((char *)&fur, sz_rfbFramebufferUpdateRequestMsg);
}

inline void ClientConnection::SendIncrementalFramebufferUpdateRequest()
{
    SendFramebufferUpdateRequest(0, 0, m_si.framebufferWidth,
					m_si.framebufferHeight, true);
}

inline void ClientConnection::SendFullFramebufferUpdateRequest()
{
    SendFramebufferUpdateRequest(0, 0, m_si.framebufferWidth,
					m_si.framebufferHeight, false);
}


void ClientConnection::SendAppropriateFramebufferUpdateRequest()
{
	if (m_pendingFormatChange) {
		rfbPixelFormat oldFormat = m_myFormat;
		SetupPixelFormat();
		SoftCursorFree();
		SetFormatAndEncodings();
		m_pendingFormatChange = false;
		// If the pixel format has changed, request whole screen
		if (!PF_EQ(m_myFormat, oldFormat)) {
			SendFullFramebufferUpdateRequest();	
		} else {
			SendIncrementalFramebufferUpdateRequest();
		}
	} else {
		if (!m_dormant)
			SendIncrementalFramebufferUpdateRequest();
	}
}


// A ScreenUpdate message has been received

void ClientConnection::ReadScreenUpdate() {

	rfbFramebufferUpdateMsg sut;
	ReadExact((char *) &sut, sz_rfbFramebufferUpdateMsg);
    sut.nRects = Swap16IfLE(sut.nRects);
	if (sut.nRects == 0) return;
	
	for (int i=0; i < sut.nRects; i++) {

		rfbFramebufferUpdateRectHeader surh;
		ReadExact((char *) &surh, sz_rfbFramebufferUpdateRectHeader);

		surh.encoding = Swap32IfLE(surh.encoding);
		surh.r.x = Swap16IfLE(surh.r.x);
		surh.r.y = Swap16IfLE(surh.r.y);
		surh.r.w = Swap16IfLE(surh.r.w);
		surh.r.h = Swap16IfLE(surh.r.h);

		if (surh.encoding == rfbEncodingLastRect)
			break;
		if (surh.encoding == rfbEncodingNewFBSize) {
			ReadNewFBSize(&surh);
			break;
		}

		if ( surh.encoding == rfbEncodingXCursor ||
			 surh.encoding == rfbEncodingRichCursor ) {
			ReadCursorShape(&surh);
			continue;
		}

		if (surh.encoding == rfbEncodingPointerPos) {
			ReadCursorPos(&surh);
			continue;
		}

		// If *Cursor encoding is used, we should prevent collisions
		// between framebuffer updates and cursor drawing operations.
		SoftCursorLockArea(surh.r.x, surh.r.y, surh.r.w, surh.r.h);

		switch (surh.encoding) {
		case rfbEncodingRaw:
			ReadRawRect(&surh);
			break;
		case rfbEncodingCopyRect:
			ReadCopyRect(&surh);
			break;
		case rfbEncodingRRE:
			ReadRRERect(&surh);
			break;
		case rfbEncodingCoRRE:
			ReadCoRRERect(&surh);
			break;
		case rfbEncodingHextile:
			ReadHextileRect(&surh);
			break;
		case rfbEncodingZlib:
			ReadZlibRect(&surh);
			break;
		//case rfbEncodingTight:
		//	ReadTightRect(&surh);
		//	break;
		case rfbEncodingZlibHex:
			ReadZlibHexRect(&surh);
			break;
		default:
			break;
		}

		// Tell the system to update a screen rectangle. Note that
		// InvalidateScreenRect member function knows about scaling.
		RECT rect;
		SetRect(&rect, surh.r.x, surh.r.y,
				surh.r.x + surh.r.w, surh.r.y + surh.r.h);
		InvalidateScreenRect(&rect);

		// Now we may discard "soft cursor locks".
		SoftCursorUnlockScreen();
	}	

	// Inform the other thread that an update is needed.
	PostMessage(m_hwnd, WM_REGIONUPDATED, NULL, NULL);
}	

void ClientConnection::SetDormant(bool newstate)
{
	m_dormant = newstate;
	if (!m_dormant)
		SendIncrementalFramebufferUpdateRequest();
}

// The server has copied some text to the clipboard - put it 
// in the local clipboard too.

void ClientConnection::ReadServerCutText()
{
	rfbServerCutTextMsg sctm;
	ReadExact((char *) &sctm, sz_rfbServerCutTextMsg);
	size_t len = Swap32IfLE(sctm.length);

	CheckBufferSize(len);
	if (len == 0) {
		m_netbuf[0] = '\0';
	} else {
		ReadString(m_netbuf, len);
	}
	UpdateLocalClipboard(m_netbuf, len);
}

void ClientConnection::ReadSetColourMapEntries()
{
	// Currently, we read and silently ignore SetColourMapEntries.

	rfbSetColourMapEntriesMsg msg;
	ReadExact((char *)&msg, sz_rfbSetColourMapEntriesMsg);
	int numEntries = Swap16IfLE(msg.nColours);

	if (numEntries > 0) {
		size_t nBytes = 6 * numEntries;
		CheckBufferSize(nBytes);
		ReadExact(m_netbuf, nBytes);
	}
}

void ClientConnection::ReadBell() {
	rfbBellMsg bm;
	ReadExact((char *) &bm, sz_rfbBellMsg);
}


// General utilities -------------------------------------------------

// Reads the number of bytes specified into the buffer given
void ClientConnection::ReadExact(char *inbuf, int wanted)
{
	if (m_sock == INVALID_SOCKET && m_bKillThread)
		throw QuietException("Connection closed.");

	omni_mutex_lock l(m_readMutex);

	int offset = 0;
	
	while (wanted > 0) {

		int bytes = recv(m_sock, inbuf+offset, wanted, 0);
		if (bytes == 0) throw WarningException("Connection closed.");
		if (bytes == SOCKET_ERROR) {
			int err = ::GetLastError();
			m_running = false;
			throw WarningException("ReadExact: Socket error while reading.");
		}
		wanted -= bytes;
		offset += bytes;

	}
}

// Read the number of bytes and return them zero terminated in the buffer 
inline void ClientConnection::ReadString(char *buf, int length)
{
	if (length > 0)
		ReadExact(buf, length);
	buf[length] = '\0';
}


// Sends the number of bytes specified from the buffer
inline void ClientConnection::WriteExact(char *buf, int bytes)
{
	if (bytes == 0 || m_sock == INVALID_SOCKET)
		return;

	omni_mutex_lock l(m_writeMutex);

	int i = 0;
    int j;

    while (i < bytes) {

		j = send(m_sock, buf+i, bytes-i, 0);
		if (j == SOCKET_ERROR || j==0) {
			m_running = false;

			throw WarningException("WriteExact: Socket error while writing.");
		}
		i += j;
    }
}

// Read the string describing the reason for a connection failure.
// This function reads the data into m_netbuf, and returns that pointer
// as the beginning of the reason string.
char *ClientConnection::ReadFailureReason()
{
	CARD32 reasonLen;
	ReadExact((char *)&reasonLen, sizeof(reasonLen));
	reasonLen = Swap32IfLE(reasonLen);

	CheckBufferSize(reasonLen + 1);
	ReadString(m_netbuf, reasonLen);

	return m_netbuf;
}

// Makes sure netbuf is at least as big as the specified size.
// Note that netbuf itself may change as a result of this call.
// Throws an exception on failure.
void ClientConnection::CheckBufferSize(size_t bufsize)
{
	if (m_netbufsize > bufsize) return;

	// Don't try to allocate more than 2 gigabytes.
	if (bufsize >= 0x80000000) {
		throw WarningException("Requested buffer size is too big.");
	}

	omni_mutex_lock l(m_bufferMutex);

	char *newbuf = new char[bufsize + 256];
	if (newbuf == NULL) {
		throw ErrorException("Insufficient memory to allocate network buffer.");
	}

	if (m_netbuf != NULL) {
		delete[] m_netbuf;
	}
	m_netbuf = newbuf;
	m_netbufsize = bufsize + 256;
}

// Makes sure zlibbuf is at least as big as the specified size.
// Note that zlibbuf itself may change as a result of this call.
// Throws an exception on failure.
void ClientConnection::CheckZlibBufferSize(size_t bufsize)
{
	if (m_zlibbufsize > bufsize) return;

	// Don't try to allocate more than 2 gigabytes.
	if (bufsize >= 0x80000000) {
		throw WarningException("Requested zlib buffer size is too big.");
	}

	// omni_mutex_lock l(m_bufferMutex);

	unsigned char *newbuf = new unsigned char[bufsize + 256];
	if (newbuf == NULL) {
		throw ErrorException("Insufficient memory to allocate zlib buffer.");
	}

	if (m_zlibbuf != NULL) {
		delete[] m_zlibbuf;
	}
	m_zlibbuf = newbuf;
	m_zlibbufsize = bufsize + 256;
}

//
// Invalidate a screen rectangle respecting scaling set by user.
//

void ClientConnection::InvalidateScreenRect(const RECT *pRect) {
	RECT rect;

	// If we're scaling, we transform the coordinates of the rectangle
	// received into the corresponding window coords, and invalidate
	// *that* region.

	if (m_opts.m_scaling) {
		// First, we adjust coords to avoid rounding down when scaling.
		int n = m_opts.m_scale_num;
		int d = m_opts.m_scale_den;
		int left   = (pRect->left / d) * d;
		int top    = (pRect->top  / d) * d;
		int right  = (pRect->right  + d - 1) / d * d; // round up
		int bottom = (pRect->bottom + d - 1) / d * d; // round up

		// Then we scale the rectangle, which should now give whole numbers.
		rect.left   = (left   * n / d) - m_hScrollPos;
		rect.top    = (top    * n / d) - m_vScrollPos;
		rect.right  = (right  * n / d) - m_hScrollPos;
		rect.bottom = (bottom * n / d) - m_vScrollPos;
	} else {
		rect.left   = pRect->left   - m_hScrollPos;
		rect.top    = pRect->top    - m_vScrollPos;
		rect.right  = pRect->right  - m_hScrollPos;
		rect.bottom = pRect->bottom - m_vScrollPos;
	}
	InvalidateRect(m_hwnd, &rect, FALSE);
}

//
// Processing NewFBSize pseudo-rectangle. Create new framebuffer of
// the size specified in pfburh->r.w and pfburh->r.h, and change the
// window size correspondingly.
//

void ClientConnection::ReadNewFBSize(rfbFramebufferUpdateRectHeader *pfburh)
{
	m_si.framebufferWidth = pfburh->r.w;
	m_si.framebufferHeight = pfburh->r.h;

	CreateLocalFramebuffer();

	SizeWindow(false);
	RealiseFullScreenMode(true);
}

int ClientConnection::SetFullScreen(int nFullScreen)
{
	SetFullScreenMode(nFullScreen!=0);
	return 0;
}

int ClientConnection::SendCtrlEsc(void)
{
	SendKeyEvent(XK_Control_L, true);
	SendKeyEvent(XK_Escape,     true);
	SendKeyEvent(XK_Escape,     false);
	SendKeyEvent(XK_Control_L, false);
	return 0;
}

int ClientConnection::SendCtrlAltDel(void)
{
	SendKeyEvent(XK_Control_L, true);
	SendKeyEvent(XK_Alt_L,     true);
	SendKeyEvent(XK_Delete,    true);
	SendKeyEvent(XK_Delete,    false);
	SendKeyEvent(XK_Alt_L,     false);
	SendKeyEvent(XK_Control_L, false);
	return 0;
}

int ClientConnection::SetUse8Bit(int nUse8Bit)
{
	m_opts.m_Use8Bit=(nUse8Bit!=0);
	m_pendingFormatChange=true;
	return 0;
}
