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


// vncServer.cpp

// vncServer class implementation

// Includes
#include "stdhdrs.h"
#include <omnithread/omnithread.h>
#include <string.h>
#include <lmcons.h>

// Custom
#include "vncServer.h"

#include "vncClient.h"
#include "vncService.h"

// Constructor/destructor
vncServer::vncServer()
{
	m_socketConn = NULL;
	m_desktop = NULL;
	m_name = NULL;

	m_querysetting = 2;
	m_querytimeout = 10;
	m_queryaccept = FALSE;
	m_queryallownopass = FALSE;

	// Set the polling mode options
	m_poll_fullscreen = TRUE;
	m_poll_foreground = FALSE;
	m_poll_undercursor = TRUE;

	m_poll_oneventonly = FALSE;
	m_poll_consoleonly = TRUE;

	m_dont_set_hooks = FALSE;
	m_dont_use_driver = FALSE;
	m_driver_direct_access_en = TRUE;

	// Set the input options
	m_enable_remote_inputs = TRUE;
	m_disable_local_inputs = FALSE;

	// Clear the client mapping table
	for (int x=0; x<MAX_CLIENTS; x++)
		m_clientmap[x] = NULL;
	m_nextid = 0;

	// Signal set when a client quits
	m_clientquitsig = new omni_condition(&m_clientsLock);
	m_clients_disabled = FALSE;
	m_local_input_priority = TRUE;

	m_full_screen = TRUE;
	m_WindowShared= FALSE;
	m_hwndShared = NULL;
	m_screen_area = FALSE;
	m_primary_display_only_shared = FALSE;
	m_disable_time = 3;
	SetSharedRect(GetScreenRect());
	SetPollingCycle(300);
	PollingCycleChanged(false);
	m_cursor_pos.x = 0;
	m_cursor_pos.y = 0;

	// initialize
	m_enable_file_transfers = FALSE;
	m_remove_wallpaper = FALSE;
	m_blank_screen = FALSE;

	m_wallpaper_wait = FALSE;

	m_strPeerName="127.0.0.1";
}

vncServer::~vncServer()
{
	
	if (m_socketConn != NULL)
	{
		delete m_socketConn;
		m_socketConn = NULL;
	}

	// Remove any active clients!
	KillAuthClients();
	KillUnauthClients();

	// Wait for all the clients to die
	WaitUntilAuthEmpty();
	WaitUntilUnauthEmpty();

	// Don't free the desktop until no KillClient is likely to free it
	{	omni_mutex_lock l(m_desktopLock);

		if (m_desktop != NULL)
		{
			delete m_desktop;
			m_desktop = NULL;
		}
	}

	if (m_name != NULL)
	{
		free(m_name);
		m_name = NULL;
	}

	if (m_clientquitsig != NULL)
	{
		delete m_clientquitsig;
		m_clientquitsig = NULL;
	}

}

// Client handling functions
void
vncServer::DisableClients(BOOL state)
{
	m_clients_disabled = state;
}

BOOL
vncServer::ClientsDisabled()
{
	return m_clients_disabled;
}

BOOL vncServer::StartListen(UINT& nPort)
{
	// Is there a listening socket?
	if (m_socketConn == NULL)
	{
		m_socketConn = new vncSockConnect();
		if (m_socketConn == NULL)
			return FALSE;

		if (!m_socketConn->Init(this, nPort))
		{
			delete m_socketConn;
			m_socketConn = NULL;
			return FALSE;
		}
	}

	return TRUE;
}

void vncServer::StopListen(void)
{
	if (m_socketConn!=NULL)
	{
		delete m_socketConn;
		m_socketConn=NULL;
	}
}

vncClientId
vncServer::AddClient(VSocket *socket, BOOL reverse, BOOL shared)
{
	return AddClient(socket, reverse, shared, TRUE, TRUE);
}

vncClientId
vncServer::AddClient(VSocket *socket, BOOL reverse, BOOL shared,
					 BOOL keysenabled, BOOL ptrenabled)
{
	vncClient *client;

	omni_mutex_lock l(m_clientsLock);

	// Try to allocate a client id...
	vncClientId clientid = m_nextid;
	do
	{
		clientid = (clientid+1) % MAX_CLIENTS;
		if (clientid == m_nextid)
		{
			delete socket;
			return -1;
		}
	}
	while (m_clientmap[clientid] != NULL);

	// Create a new client and add it to the relevant client list
	client = new vncClient();
	if (client == NULL) {
		delete socket;
		return -1;
	}

	// Set the client's settings
	client->EnableKeyboard(keysenabled && m_enable_remote_inputs);
	client->EnablePointer(ptrenabled && m_enable_remote_inputs);

	// Start the client
	if (!client->Init(this, socket, reverse, shared, clientid))
	{
		// The client will delete the socket for us...
		delete client;
		return -1;
	}

	m_clientmap[clientid] = client;

	// Add the client to unauth the client list
	m_unauthClients.push_back(clientid);


	return clientid;
}

BOOL
vncServer::Authenticated(vncClientId clientid)
{
	vncClientList::iterator i;
	BOOL authok = TRUE;

	omni_mutex_lock l1(m_desktopLock);
	omni_mutex_lock l2(m_clientsLock);

	// Search the unauthenticated client list
	for (i = m_unauthClients.begin(); i != m_unauthClients.end(); i++)
	{
		// Is this the right client?
		if ((*i) == clientid)
		{
			vncClient *client = GetClient(clientid);

			// Yes, so remove the client and add it to the auth list
			m_unauthClients.erase(i);

			// Create the screen handler if necessary
			if (m_desktop == NULL)
			{
				if (RemoveWallpaperEnabled()) {
					m_wallpaper_wait = TRUE;
				}
				m_desktop = new vncDesktop();
				if (m_desktop == NULL)
				{
					client->Kill();
					authok = FALSE;
					break;
				}
				if (!m_desktop->Init(this))
				{

					client->Kill();
					authok = FALSE;

					delete m_desktop;
					m_desktop = NULL;

					break;
				}
			}

			// Create a buffer object for this client
			vncBuffer *buffer = new vncBuffer(m_desktop);
			if (buffer == NULL)
			{
				client->Kill();
				authok = FALSE;
				break;
			}

			// Tell the client about this new buffer
			client->SetBuffer(buffer);

			// Add the client to the auth list
			m_authClients.push_back(clientid);

			break;
		}
	}


	return authok;
}

void
vncServer::KillClient(vncClientId clientid)
{
	vncClientList::iterator i;
	BOOL done = FALSE;

	omni_mutex_lock l(m_clientsLock);

	// Find the client in one of the two lists
	for (i = m_unauthClients.begin(); i != m_unauthClients.end(); i++)
	{
		// Is this the right client?
		if ((*i) == clientid)
		{

			// Ask the client to die
			vncClient *client = GetClient(clientid);
			client->Kill();

			done = TRUE;
			break;
		}
	}
	if (!done)
	{
		for (i = m_authClients.begin(); i != m_authClients.end(); i++)
		{
			// Is this the right client?
			if ((*i) == clientid)
			{

				// Yes, so kill it
				vncClient *client = GetClient(clientid);
				client->Kill();

				done = TRUE;
				break;
			}
		}
	}

}

void
vncServer::KillAuthClients()
{
	vncClientList::iterator i;
	omni_mutex_lock l(m_clientsLock);

	// Tell all the authorised clients to die!
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{

		// Kill the client
		GetClient(*i)->Kill();
	}

}

void
vncServer::KillUnauthClients()
{
	vncClientList::iterator i;
	omni_mutex_lock l(m_clientsLock);

	// Tell all the authorised clients to die!
	for (i = m_unauthClients.begin(); i != m_unauthClients.end(); i++)
	{

		// Kill the client
		GetClient(*i)->Kill();
	}

}

UINT
vncServer::AuthClientCount()
{
	omni_mutex_lock l(m_clientsLock);

	return m_authClients.size();
}

UINT
vncServer::UnauthClientCount()
{
	omni_mutex_lock l(m_clientsLock);

	return m_unauthClients.size();
}

void
vncServer::WaitUntilAuthEmpty()
{
	omni_mutex_lock l(m_clientsLock);

	// Wait for all the clients to exit
	while (!m_authClients.empty())
	{
		// Wait for a client to quit
		m_clientquitsig->wait();
	}
}

void
vncServer::WaitUntilUnauthEmpty()
{
	omni_mutex_lock l(m_clientsLock);

	// Wait for all the clients to exit
	while (!m_unauthClients.empty())
	{
		// Wait for a client to quit
		m_clientquitsig->wait();
	}
}

BOOL
vncServer::RemoteEventReceived()
{
	vncClientList::iterator i;
	BOOL result = FALSE;
	omni_mutex_lock l(m_clientsLock);

	// Iterate over the authorised clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		result = result || GetClient(*i)->RemoteEventReceived();
	}
	return result;
}

// Client info retrieval/setup
vncClient*
vncServer::GetClient(vncClientId clientid)
{
	if ((clientid >= 0) && (clientid < MAX_CLIENTS))
		return m_clientmap[clientid];
	return NULL;
}

vncClientList
vncServer::ClientList()
{
	vncClientList clients;

	omni_mutex_lock l(m_clientsLock);

	clients = m_authClients;

	return clients;
}

void
vncServer::BlockRemoteInput(BOOL block)
{
	omni_mutex_lock l(m_clientsLock);

	vncClientList::iterator i;
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
		GetClient(*i)->BlockInput(block);
}

void vncServer::SetRemoteViewOnly(BOOL bViewOnly)
{
	omni_mutex_lock l(m_clientsLock);

	vncClientList::iterator i;
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		vncClient*pClient=GetClient(*i);
		pClient->BlockInput(bViewOnly);
		pClient->EnableKeyboard(m_enable_remote_inputs);
		pClient->EnablePointer(m_enable_remote_inputs);
	}
}

const char*
vncServer::GetClientName(vncClientId clientid)
{
	omni_mutex_lock l(m_clientsLock);

	vncClient *client = GetClient(clientid);
	if (client != NULL)
		return client->GetClientName();
	return NULL;
}

// RemoveClient should ONLY EVER be used by the client to remove itself.
void
vncServer::RemoveClient(vncClientId clientid)
{
	{
		vncClientList::iterator i;
		BOOL done = FALSE;

		omni_mutex_lock l1(m_desktopLock);
		{	omni_mutex_lock l2(m_clientsLock);

			// Find the client in one of the two lists
			for (i = m_unauthClients.begin(); i != m_unauthClients.end(); i++)
			{
				// Is this the right client?
				if ((*i) == clientid)
				{

					// Yes, so remove the client and kill it
					m_unauthClients.erase(i);
					m_clientmap[clientid] = NULL;

					done = TRUE;
					break;
				}
			}
			if (!done)
			{
				for (i = m_authClients.begin(); i != m_authClients.end(); i++)
				{
					// Is this the right client?
					if ((*i) == clientid)
					{

						// Yes, so remove the client and kill it
						m_authClients.erase(i);
						m_clientmap[clientid] = NULL;

						done = TRUE;
						break;
					}
				}
			}

			// Signal that a client has quit
			m_clientquitsig->signal();

		} // Unlock the clientLock

		// Are there any authorised clients connected?
		if (m_authClients.empty() && (m_desktop != NULL))
		{

			// Delete the screen server
			delete m_desktop;
			m_desktop = NULL;
		}

	}

	OnClientDisconnected(clientid);
}



// Client->Desktop update signalling
void
vncServer::RequestUpdate()
{
	omni_mutex_lock l(m_desktopLock);
	if (m_desktop != NULL)
	{
		m_desktop->RequestUpdate();
	}
}

// Update handling
void
vncServer::TriggerUpdate()
{
	vncClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->TriggerUpdate();
	}
}

void
vncServer::UpdateRect(RECT &rect)
{
	vncClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdateRect(rect);
	}
}



void
vncServer::UpdateRegion(vncRegion &region)
{
	vncClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

		
	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
			GetClient(*i)->UpdateRegion(region);
	}
}

void
vncServer::CopyRect(RECT &dest, POINT &source)
{
	vncClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->CopyRect(dest, source);
	}
}

void
vncServer::UpdateMouse()
{
	vncClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this mouse update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdateMouse();
	}
}

void
vncServer::UpdateClipText(LPSTR text)
{
	vncClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdateClipText(text);
	}
}

void
vncServer::UpdatePalette()
{
	vncClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdatePalette();
	}
}

void
vncServer::UpdateLocalClipText(LPSTR text)
{
	omni_mutex_lock l(m_desktopLock);

	if (m_desktop != NULL)
		m_desktop->SetClipText(text);
}

// Changing hook settings

void
vncServer::DontSetHooks(BOOL enable)
{
	m_dont_set_hooks = enable;
	if (m_desktop != NULL)
		m_desktop->TryActivateHooks();
}

// Changing use driver settings

void
vncServer::DontUseDriver(BOOL enable)
{
	m_dont_use_driver = enable;
}

void
vncServer::DriverDirectAccess(BOOL enable)
{
	m_driver_direct_access_en = enable;
}

// Name and port number handling
void
vncServer::SetName(const char * name)
{
	// Set the name of the desktop
	if (m_name != NULL)
	{
		free(m_name);
		m_name = NULL;
	}
	
	m_name = strdup(name);
}



// Remote input handling
void
vncServer::EnableRemoteInputs(BOOL enable)
{
	m_enable_remote_inputs = enable;
}

BOOL vncServer::RemoteInputsEnabled()
{
	return m_enable_remote_inputs;
}

// Local input handling
void
vncServer::DisableLocalInputs(BOOL disable)
{
	if ( m_disable_local_inputs != disable )
	{
		m_disable_local_inputs = disable;
		if ( AuthClientCount() != 0 )
			m_desktop->SetLocalInputDisableHook(disable);
	}
}

BOOL vncServer::LocalInputsDisabled()
{
	return m_disable_local_inputs;
}

void vncServer::LocalInputPriority(BOOL disable)
{
	if( m_local_input_priority != disable )
	{
		m_local_input_priority = disable;
		m_remote_mouse = 0;
		m_remote_keyboard = 0;
		if ( AuthClientCount() != 0 )
			m_desktop->SetLocalInputPriorityHook(disable);
	}
	
}  

void
vncServer::GetScreenInfo(int &width, int &height, int &depth)
{
	rfbServerInitMsg scrinfo;

	omni_mutex_lock l(m_desktopLock);


	// Is a desktop object currently active?
	if (m_desktop == NULL)
	{
		vncDesktop desktop;

		// No, so create a dummy desktop and interrogate it
		if (!desktop.Init(this))
		{
			scrinfo.framebufferWidth = 0;
			scrinfo.framebufferHeight = 0;
			scrinfo.format.bitsPerPixel = 0;
		}
		else
		{
			desktop.FillDisplayInfo(&scrinfo);
		}
	}
	else
	{
		m_desktop->FillDisplayInfo(&scrinfo);
	}

	// Get the info from the scrinfo structure
	width = m_shared_rect.right - m_shared_rect.left;
	height = m_shared_rect.bottom - m_shared_rect.top;
	depth = scrinfo.format.bitsPerPixel;
}

inline BOOL
MatchStringToTemplate(const char *addr, size_t addrlen,
				      const char *filtstr, size_t filtlen) {
	if (filtlen == 0)
		return 1;
	if (addrlen < filtlen)
		return 0;
	for (size_t x = 0; x < filtlen; x++) {
		if (addr[x] != filtstr[x])
			return 0;
	}
	if ((addrlen > filtlen) && (addr[filtlen] != '.'))
		return 0;
	return 1;
}

void
vncServer::SetWindowShared(HWND hWnd)
{
	m_hwndShared=hWnd;
}

void  vncServer::SetMatchSizeFields(int left,int top,int right,int bottom)
{
	RECT trect = GetScreenRect();

/*	if ( right - left < 32 )
		right = left + 32;
	
	if ( bottom - top < 32)
		bottom = top + 32 ;*/

	if( right > trect.right )
		right = trect.right;
	if( bottom > trect.bottom )
		bottom = trect.bottom;
	if( left < trect.left)
		left = trect.left;
	if( top < trect.top)
		top = trect.top;

 
	m_screenarea_rect.left=left;
	m_screenarea_rect.top=top;
	m_screenarea_rect.bottom=bottom;
	m_screenarea_rect.right=right;
}


void 
vncServer::SetKeyboardCounter(int count)
{
		
	omni_mutex_lock l(m_clientsLock);
	if (LocalInputPriority() && vncService::IsWin95())
	{
		m_remote_keyboard += count;
		if (count == 0)
			m_remote_keyboard = 0;
	}       
	
}

void 
vncServer::SetMouseCounter(int count, POINT &cursor_pos, BOOL mousemove)
{
	if( (mousemove) && ( abs (m_cursor_pos.x - cursor_pos.x)==0 
		&&  abs (m_cursor_pos.y - cursor_pos.y)==0 ) ) 
		return;
	
	omni_mutex_lock l(m_clientsLock);
	if (LocalInputPriority() && vncService::IsWin95())
	{
		m_remote_mouse += count;
		if (count == 0)
			m_remote_mouse = 0;

		m_cursor_pos.x = cursor_pos.x;
		m_cursor_pos.y = cursor_pos.y;
		
	
	}
	
}

void 
vncServer::SetNewFBSize(BOOL sendnewfb)
{
	vncClientList::iterator i;
	omni_mutex_lock l(m_clientsLock);

	// Post new framebuffer size update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->SetNewFBSize( sendnewfb);
	}
}


BOOL 
vncServer::FullRgnRequested()
{
	vncClientList::iterator i;
	omni_mutex_lock l(m_clientsLock);

	// Iterate over the authorised clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		if (GetClient(*i)->FullRgnRequested())
			return TRUE;
	}
	return FALSE;
}

BOOL 
vncServer::IncrRgnRequested()
{
	vncClientList::iterator i;
	omni_mutex_lock l(m_clientsLock);

	// Iterate over the authorised clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		if (GetClient(*i)->IncrRgnRequested())
			return TRUE;
	}
	return FALSE;
}

void 
vncServer::UpdateLocalFormat()
{
	vncClientList::iterator i;
	omni_mutex_lock l(m_clientsLock);

	// Iterate over the authorised clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		GetClient(*i)->UpdateLocalFormat();
			
	}
	return;
}

void 
vncServer::SetPollingCycle(UINT msec)
{
	if (m_polling_cycle != msec && msec > 10) {
		m_polling_cycle = msec;
		PollingCycleChanged(true);
	}
}

BOOL
vncServer::checkPointer(vncClient *pClient)
{
  vncClientList::iterator i;
  for (i = m_authClients.begin(); i != m_authClients.end(); i++)
  {
    if (GetClient(*i) == pClient) return TRUE;
  }
  return FALSE;
}


typedef HMONITOR (WINAPI* pMonitorFromPoint)(POINT,DWORD);
typedef BOOL (WINAPI* pGetMonitorInfo)(HMONITOR,LPMONITORINFO);

BOOL vncServer::SetShareMonitorFromPoint(POINT pt)
{
	HINSTANCE  hInstUser32 = LoadLibrary("User32.DLL");
	if (!hInstUser32) return FALSE;  
	pMonitorFromPoint pMFP = (pMonitorFromPoint)GetProcAddress(hInstUser32, "MonitorFromPoint");
	pGetMonitorInfo pGMI = (pGetMonitorInfo)GetProcAddress(hInstUser32, "GetMonitorInfoA");
	if (!pMFP || !pGMI)
	{
		FreeLibrary(hInstUser32);
		return FALSE;
	}

	HMONITOR hm = pMFP(pt, MONITOR_DEFAULTTONEAREST);
	if (!hm)
	{
		FreeLibrary(hInstUser32);
		return FALSE;
	}
	MONITORINFO	moninfo;
	moninfo.cbSize = sizeof(moninfo);
	if (!pGMI(hm, &moninfo))
	{
		FreeLibrary(hInstUser32);
		return FALSE;
	}

	FullScreen(FALSE);
	WindowShared(FALSE);
	ScreenAreaShared(TRUE);
	PrimaryDisplayOnlyShared(FALSE);

	SetMatchSizeFields(
		moninfo.rcMonitor.left,
		moninfo.rcMonitor.top,
		moninfo.rcMonitor.right,
		moninfo.rcMonitor.bottom);

	FreeLibrary(hInstUser32);
	return TRUE;
}

const std::string&vncServer::GetPeerName(void)
{
	return m_strPeerName;
}
