//  Copyright (C) 2001-2006 Constantin Kaplinsky. All Rights Reserved.
//  Copyright (C) 2002 Vladimir Vologzhanin. All Rights Reserved.
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


// vncClient.cpp

// The per-client object.  This object takes care of all per-client stuff,
// such as socket input and buffering of updates.

// vncClient class handles the following functions:
// - Recieves requests from the connected client and
//   handles them
// - Handles incoming updates properly, using a vncBuffer
//   object to keep track of screen changes
// It uses a vncBuffer and is passed the vncDesktop and
// vncServer to communicate with.

// Includes
#include "stdhdrs.h"
#include <omnithread/omnithread.h>
#include "resource.h"

// Custom
#include "vncClient.h"
#include "VSocket.h"
#include "vncDesktop.h"
#include "vncRegion.h"
#include "vncBuffer.h"
#include "vncService.h"
#include "vncKeymap.h"
#include "Windows.h"


//
// Normally, using macros is no good, but this macro saves us from
// writing constants twice -- it constructs signature names from codes.
// Note that "code_sym" argument should be a single symbol, not an expression.
//

#define SetCapInfo(cap_ptr, code_sym, vendor)			\
{														\
	rfbCapabilityInfo *pcap = (cap_ptr);				\
	pcap->code = Swap32IfLE(code_sym);					\
	memcpy(pcap->vendorSignature, (vendor),				\
	sz_rfbCapabilityInfoVendor);						\
	memcpy(pcap->nameSignature, sig_##code_sym,			\
	sz_rfbCapabilityInfoName);							\
}

// vncClient thread class

class vncClientThread : public omni_thread
{
public:
	// Init
	virtual BOOL Init(vncClient *client,
					  vncServer *server,
					  VSocket *socket,
					  BOOL reverse,
					  BOOL shared);

	// Sub-Init routines
	virtual void SendConnFailedMessage(const char *reasonString);
	virtual BOOL SendTextStringMessage(const char *str);
	virtual BOOL NegotiateTunneling();
	virtual BOOL NegotiateAuthentication(int authType);
	virtual BOOL AuthenticateNone();

	virtual BOOL ReadClientInit();
	virtual BOOL SendInteractionCaps();

	// The main thread function
	virtual void run(void *arg);

protected:
	virtual ~vncClientThread();

	// Fields
protected:
	VSocket *m_socket;
	vncServer *m_server;
	vncClient *m_client;
	BOOL m_reverse;
	BOOL m_shared;
};

vncClientThread::~vncClientThread()
{
	// If we have a client object then delete it
	if (m_client != NULL)
		delete m_client;
}

BOOL
vncClientThread::Init(vncClient *client, vncServer *server, VSocket *socket, BOOL reverse, BOOL shared)
{
	// Save the server pointer and window handle
	m_server = server;
	m_socket = socket;
	m_client = client;
	m_reverse = reverse;
	m_shared = shared;

	// Start the thread
	start();

	return TRUE;
}

//
// Send a "connection failed" message.
//

void
vncClientThread::SendConnFailedMessage(const char *reasonString)
{
	if (m_client->m_protocol_minor_version >= 7) {
		CARD8 zeroCount = 0;
		if (!m_socket->SendExact((char *)&zeroCount, sizeof(zeroCount)))
			return;
	} else {
		CARD32 authValue = Swap32IfLE(rfbSecTypeInvalid);
		if (!m_socket->SendExact((char *)&authValue, sizeof(authValue)))
			return;
	}
	SendTextStringMessage(reasonString);
}

//
// Send a text message preceded with a length counter.
//

BOOL
vncClientThread::SendTextStringMessage(const char *str)
{
	CARD32 len = Swap32IfLE(strlen(str));
	if (!m_socket->SendExact((char *)&len, sizeof(len)))
		return FALSE;
	if (!m_socket->SendExact(str, strlen(str)))
		return FALSE;

	return TRUE;
}

//
// Negotiate tunneling type (protocol versions 3.7t, 3.8t).
//

BOOL
vncClientThread::NegotiateTunneling()
{
	int nTypes = 0;

	// Advertise our tunneling capabilities (currently, nothing to advertise).
	rfbTunnelingCapsMsg caps;
	caps.nTunnelTypes = Swap32IfLE(nTypes);
	return m_socket->SendExact((char *)&caps, sz_rfbTunnelingCapsMsg);

	// Read tunneling type requested by the client (currently, not necessary).
	if (nTypes) {
		CARD32 tunnelType;
		if (!m_socket->ReadExact((char *)&tunnelType, sizeof(tunnelType)))
			return FALSE;
		tunnelType = Swap32IfLE(tunnelType);
		// We cannot do tunneling yet.
		return FALSE;
	}

	return TRUE;
}

//
// Negotiate authentication scheme (protocol versions 3.7t, 3.8t).
// NOTE: Here we always send en empty list for "no authentication".
//

BOOL
vncClientThread::NegotiateAuthentication(int authType)
{
	int nTypes = 0;

	if (authType == rfbAuthVNC) {
		nTypes++;
	} else if (authType != rfbAuthNone) {
		return FALSE;
	}

	rfbAuthenticationCapsMsg caps;
	caps.nAuthTypes = Swap32IfLE(nTypes);
	if (!m_socket->SendExact((char *)&caps, sz_rfbAuthenticationCapsMsg))
		return FALSE;

	if (authType == rfbAuthVNC) {
		// Inform the client about supported authentication types.
		rfbCapabilityInfo cap;
		SetCapInfo(&cap, rfbAuthVNC, rfbStandardVendor);
		if (!m_socket->SendExact((char *)&cap, sz_rfbCapabilityInfo))
			return FALSE;

		CARD32 type;
		if (!m_socket->ReadExact((char *)&type, sizeof(type)))
			return FALSE;
		type = Swap32IfLE(type);
		if (type != authType) {
			return FALSE;
		}
	}

	return TRUE;
}

//
// Handle security type for "no authentication".
//

BOOL
vncClientThread::AuthenticateNone()
{
	if (m_client->m_protocol_minor_version >= 8) {
		CARD32 secResult = Swap32IfLE(rfbAuthOK);
		if (!m_socket->SendExact((char *)&secResult, sizeof(secResult)))
			return FALSE;
	}
	return TRUE;
}


//
// Read client initialisation message
//

BOOL
vncClientThread::ReadClientInit()
{
	// Read the client's initialisation message
	rfbClientInitMsg client_ini;
	if (!m_socket->ReadExact((char *)&client_ini, sz_rfbClientInitMsg))
		return FALSE;

	// If the client wishes to have exclusive access then remove other clients
	if (!client_ini.shared && !m_shared)
	{
		m_server->KillAuthClients();
	}

	// Tell the server that this client is ok
	return m_server->Authenticated(m_client->GetClientId());
}

//
// Advertise our messaging capabilities (protocol version 3.7+).
//

BOOL
vncClientThread::SendInteractionCaps()
{
	// Update these constants on changing capability lists!
	const int MAX_SMSG_CAPS = 4;
	const int MAX_CMSG_CAPS = 6;
	const int MAX_ENC_CAPS = 14;

	int i;

	// Supported server->client message types
	rfbCapabilityInfo smsg_list[MAX_SMSG_CAPS];
	i = 0;


	int nServerMsgs = i;
	if (nServerMsgs > MAX_SMSG_CAPS) {
		return FALSE;
	}

	// Supported client->server message types
	rfbCapabilityInfo cmsg_list[MAX_CMSG_CAPS];
	i = 0;

	int nClientMsgs = i;
	if (nClientMsgs > MAX_CMSG_CAPS) {
		return FALSE;
	}

	// Encoding types
	rfbCapabilityInfo enc_list[MAX_ENC_CAPS];
	i = 0;
	SetCapInfo(&enc_list[i++],  rfbEncodingCopyRect,       rfbStandardVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingRRE,            rfbStandardVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingCoRRE,          rfbStandardVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingHextile,        rfbStandardVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingZlib,           rfbTridiaVncVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingZlibHex,        rfbTridiaVncVendor);
	//SetCapInfo(&enc_list[i++],  rfbEncodingTight,          rfbTightVncVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingCompressLevel0, rfbTightVncVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingQualityLevel0,  rfbTightVncVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingXCursor,        rfbTightVncVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingRichCursor,     rfbTightVncVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingPointerPos,     rfbTightVncVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingLastRect,       rfbTightVncVendor);
	SetCapInfo(&enc_list[i++],  rfbEncodingNewFBSize,      rfbTightVncVendor);
	int nEncodings = i;
	if (nEncodings > MAX_ENC_CAPS) {
		return FALSE;
	}

	// Create and send the header structure
	rfbInteractionCapsMsg intr_caps;
	intr_caps.nServerMessageTypes = Swap16IfLE(nServerMsgs);
	intr_caps.nClientMessageTypes = Swap16IfLE(nClientMsgs);
	intr_caps.nEncodingTypes = Swap16IfLE(nEncodings);
	intr_caps.pad = 0;
	if (!m_socket->SendExact((char *)&intr_caps, sz_rfbInteractionCapsMsg))
		return FALSE;

	// Send the capability lists
	if (nServerMsgs &&
		!m_socket->SendExact((char *)&smsg_list[0],
		sz_rfbCapabilityInfo * nServerMsgs))
		return FALSE;
	if (nClientMsgs &&
		!m_socket->SendExact((char *)&cmsg_list[0],
		sz_rfbCapabilityInfo * nClientMsgs))
		return FALSE;
	if (nEncodings &&
		!m_socket->SendExact((char *)&enc_list[0],
		sz_rfbCapabilityInfo * nEncodings))
		return FALSE;

	return TRUE;
}

void
ClearKeyState(BYTE key)
{
	// This routine is used by the VNC client handler to clear the
	// CAPSLOCK, NUMLOCK and SCROLL-LOCK states.

	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	if(keyState[key] & 1)
	{
		// Simulate the key being pressed
		keybd_event(key, 0, KEYEVENTF_EXTENDEDKEY, 0);

		// Simulate it being release
		keybd_event(key, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
}

void
vncClientThread::run(void *arg)
{
	// All this thread does is go into a socket-recieve loop,
	// waiting for stuff on the given socket

	// IMPORTANT : ALWAYS call RemoveClient on the server before quitting
	// this thread.


	// Save the handle to the thread's original desktop
	HDESK home_desktop = GetThreadDesktop(GetCurrentThreadId());

	// To avoid people connecting and then halting the connection, set a timeout
	m_socket->SetTimeout(30000);


	// READ CLIENT INITIALIZATION MESSAGE
	if (!ReadClientInit()) {
		m_server->RemoveClient(m_client->GetClientId());
		return;
	}

	m_socket->SetTimeout(m_server->AutoIdleDisconnectTimeout()*1000);

	// INIT PIXEL FORMAT

	// Get the screen format
	m_client->m_fullscreen = m_client->m_buffer->GetSize();

	// Get the name of this desktop
	char desktopname[MAX_COMPUTERNAME_LENGTH+1];
	DWORD desktopnamelen = MAX_COMPUTERNAME_LENGTH + 1;
	if (GetComputerName(desktopname, &desktopnamelen))
	{
		// Make the name lowercase
		for (size_t x=0; x<strlen(desktopname); x++)
		{
			desktopname[x] = tolower(desktopname[x]);
		}
	}
	else
	{
		strcpy(desktopname, "WinVNC");
	}

	// Send the server format message to the client
	rfbServerInitMsg server_ini;
	server_ini.format = m_client->m_buffer->GetLocalFormat();

	// Endian swaps
	RECT sharedRect;
	sharedRect = m_server->GetSharedRect();
	server_ini.framebufferWidth = Swap16IfLE(sharedRect.right- sharedRect.left);
	server_ini.framebufferHeight = Swap16IfLE(sharedRect.bottom - sharedRect.top);
	server_ini.format.redMax = Swap16IfLE(server_ini.format.redMax);
	server_ini.format.greenMax = Swap16IfLE(server_ini.format.greenMax);
	server_ini.format.blueMax = Swap16IfLE(server_ini.format.blueMax);
	server_ini.nameLength = Swap32IfLE(strlen(desktopname));

	if (!m_socket->SendExact((char *)&server_ini, sizeof(server_ini)))
	{
		m_server->RemoveClient(m_client->GetClientId());
		return;
	}
	if (!m_socket->SendExact(desktopname, strlen(desktopname)))
	{
		m_server->RemoveClient(m_client->GetClientId());
		return;
	}

	// Inform the client about our interaction capabilities (protocol 3.7t)
	if (m_client->m_protocol_tightvnc) {
		if (!SendInteractionCaps()) {
			m_server->RemoveClient(m_client->GetClientId());
			return;
		}
	}

	// UNLOCK INITIAL SETUP
	// Initial negotiation is complete, so set the protocol ready flag
	{
		omni_mutex_lock l(m_client->m_regionLock);
		m_client->m_protocol_ready = TRUE;
	}

	// Clear the CapsLock and NumLock keys
	if (m_client->IsKeyboardEnabled())
	{
		ClearKeyState(VK_CAPITAL);
		// *** JNW - removed because people complain it's wrong
		//ClearKeyState(VK_NUMLOCK);
		ClearKeyState(VK_SCROLL);
	}

	// MAIN LOOP

	BOOL connected = TRUE;
	while (connected)
	{
		rfbClientToServerMsg msg;

		// Ensure that we're running in the correct desktop
		if (!vncService::InputDesktopSelected())
			if (!vncService::SelectDesktop(NULL))
				break;

		// Try to read a message ID
		if (!m_socket->ReadExact((char *)&msg.type, sizeof(msg.type)))
		{
			connected = FALSE;
			break;
		}

		// What to do is determined by the message id
		switch(msg.type)
		{

		case rfbSetPixelFormat:
			// Read the rest of the message:
			if (!m_socket->ReadExact(((char *) &msg)+1, sz_rfbSetPixelFormatMsg-1))
			{
				connected = FALSE;
				break;
			}


			// Swap the relevant bits.
			msg.spf.format.redMax = Swap16IfLE(msg.spf.format.redMax);
			msg.spf.format.greenMax = Swap16IfLE(msg.spf.format.greenMax);
			msg.spf.format.blueMax = Swap16IfLE(msg.spf.format.blueMax);

			{
				omni_mutex_lock l(m_client->m_regionLock);

				// Tell the buffer object of the change
				if (!m_client->m_buffer->SetClientFormat(msg.spf.format))
				{

					connected = FALSE;
				}

				// Set the palette-changed flag, just in case...
				m_client->m_palettechanged = TRUE;
			}
			break;

		case rfbSetEncodings:
			// Read the rest of the message:
			if (!m_socket->ReadExact(((char *) &msg)+1, sz_rfbSetEncodingsMsg-1))
			{
				connected = FALSE;
				break;
			}


			m_client->m_buffer->SetQualityLevel(-1);
			m_client->m_buffer->SetCompressLevel(6);
			m_client->m_buffer->EnableXCursor(FALSE);
			m_client->m_buffer->EnableRichCursor(FALSE);
			m_client->m_buffer->EnableLastRect(FALSE);
			m_client->m_use_PointerPos = FALSE;
			m_client->m_use_NewFBSize = FALSE;

			m_client->m_cursor_update_pending = FALSE;
			m_client->m_cursor_update_sent = FALSE;
			m_client->m_cursor_pos_changed = FALSE;

			// Read in the preferred encodings
			msg.se.nEncodings = Swap16IfLE(msg.se.nEncodings);
			{
				int x;
				BOOL encoding_set = FALSE;
				BOOL shapeupdates_requested = FALSE;
				BOOL pointerpos_requested = FALSE;

				{
					omni_mutex_lock l(m_client->m_regionLock);
					// By default, don't use copyrect!
					m_client->m_copyrect_use = FALSE;
				}

				for (x = 0; x < msg.se.nEncodings; x++)
				{
					omni_mutex_lock l(m_client->m_regionLock);
					CARD32 encoding;

					// Read an encoding in
					if (!m_socket->ReadExact((char *)&encoding, sizeof(encoding)))
					{
						connected = FALSE;
						break;
					}

					// Is this the CopyRect encoding (a special case)?
					if (Swap32IfLE(encoding) == rfbEncodingCopyRect)
					{
						// Client wants us to use CopyRect
						m_client->m_copyrect_use = TRUE;
						continue;
					}

					// Is this an XCursor encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingXCursor) {
						m_client->m_buffer->EnableXCursor(TRUE);
						shapeupdates_requested = TRUE;
						continue;
					}

					// Is this a RichCursor encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingRichCursor) {
						m_client->m_buffer->EnableRichCursor(TRUE);
						shapeupdates_requested = TRUE;
						continue;
					}

					// Is this a CompressLevel encoding?
					if ((Swap32IfLE(encoding) >= rfbEncodingCompressLevel0) &&
						(Swap32IfLE(encoding) <= rfbEncodingCompressLevel9))
					{
						// Client specified encoding-specific compression level
						int level = (int)(Swap32IfLE(encoding) - rfbEncodingCompressLevel0);
						m_client->m_buffer->SetCompressLevel(level);
						continue;
					}

					// Is this a QualityLevel encoding?
					if ((Swap32IfLE(encoding) >= rfbEncodingQualityLevel0) &&
						(Swap32IfLE(encoding) <= rfbEncodingQualityLevel9))
					{
						// Client specified image quality level used for JPEG compression
						int level = (int)(Swap32IfLE(encoding) - rfbEncodingQualityLevel0);
						m_client->m_buffer->SetQualityLevel(level);
						continue;
					}

					// Is this a PointerPos encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingPointerPos) {
						pointerpos_requested = TRUE;
						continue;
					}

					// Is this a LastRect encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingLastRect) {
						m_client->m_buffer->EnableLastRect(TRUE);
						continue;
					}

					// Is this a NewFBSize encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingNewFBSize) {
						m_client->m_use_NewFBSize = TRUE;
						continue;
					}

					// Have we already found a suitable encoding?
					if (!encoding_set)
					{
						// omni_mutex_lock l(m_client->m_regionLock);

						// No, so try the buffer to see if this encoding will work...
						if (m_client->m_buffer->SetEncoding(Swap32IfLE(encoding))) {
							encoding_set = TRUE;
						}

					}
				}

				// Enable CursorPos encoding only if cursor shape updates were
				// requested by the client.
				if (shapeupdates_requested && pointerpos_requested) {
					m_client->m_use_PointerPos = TRUE;
					m_client->SetCursorPosChanged();
				}

				// If no encoding worked then default to RAW!
				// FIXME: Protocol extensions won't work in this case.
				if (!encoding_set)
				{
					omni_mutex_lock l(m_client->m_regionLock);


					if (!m_client->m_buffer->SetEncoding(Swap32IfLE(rfbEncodingRaw)))
					{

						connected = FALSE;
					}
				}
			}

			break;

		case rfbFramebufferUpdateRequest:
			// Read the rest of the message:
			if (!m_socket->ReadExact(((char *) &msg)+1, sz_rfbFramebufferUpdateRequestMsg-1))
			{
				connected = FALSE;
				break;
			}

			if (msg.fur.incremental) {
			} else {
			}

			{
				RECT update;
				RECT sharedRect;
				{
					omni_mutex_lock l(m_client->m_regionLock);

					sharedRect = m_server->GetSharedRect();
					// Get the specified rectangle as the region to send updates for.
					update.left = Swap16IfLE(msg.fur.x)+ sharedRect.left;
					update.top = Swap16IfLE(msg.fur.y)+ sharedRect.top;
					update.right = update.left + Swap16IfLE(msg.fur.w);

					_ASSERTE(Swap16IfLE(msg.fur.x) >= 0);
					_ASSERTE(Swap16IfLE(msg.fur.y) >= 0);

					//if (update.right > m_client->m_fullscreen.right)
					//	update.right = m_client->m_fullscreen.right;
					if (update.right > sharedRect.right)
						update.right = sharedRect.right;
					if (update.left < sharedRect.left)
						update.left = sharedRect.left;

					update.bottom = update.top + Swap16IfLE(msg.fur.h);
					//if (update.bottom > m_client->m_fullscreen.bottom)
					//	update.bottom = m_client->m_fullscreen.bottom;
					if (update.bottom > sharedRect.bottom)
						update.bottom = sharedRect.bottom;
					if (update.top < sharedRect.top)
						update.top = sharedRect.top;

					// Set the update-wanted flag to true
					m_client->m_updatewanted = TRUE;

					// Clip the rectangle to the screen
					if (IntersectRect(&update, &update, &sharedRect))
					{
						// Is this request for an incremental region?
						if (msg.fur.incremental)
						{
							// Yes, so add it to the incremental region
							m_client->m_incr_rgn.AddRect(update);
						}
						else
						{
							// No, so add it to the full update region
							m_client->m_full_rgn.AddRect(update);

							// Disable any pending CopyRect
							m_client->m_copyrect_set = FALSE;
						}
					}

					// Trigger an update
					m_server->RequestUpdate();
				}
			}
			break;

		case rfbKeyEvent:
			// Read the rest of the message:
			if (m_socket->ReadExact(((char *) &msg)+1, sz_rfbKeyEventMsg-1))
			{

				if (m_client->IsKeyboardEnabled() && !m_client->IsInputBlocked())
				{
					msg.ke.key = Swap32IfLE(msg.ke.key);
					// Get the keymapper to do the work
					vncKeymap::keyEvent(msg.ke.key, msg.ke.down != 0,
						m_client->m_server);
					m_client->m_remoteevent = TRUE;
				}
			}
			break;

		case rfbPointerEvent:
			// Read the rest of the message:
			if (m_socket->ReadExact(((char *) &msg)+1, sz_rfbPointerEventMsg-1))
			{

				if (m_client->IsPointerEnabled() && !m_client->IsInputBlocked())
				{
					// Convert the coords to Big Endian
					msg.pe.x = Swap16IfLE(msg.pe.x);
					msg.pe.y = Swap16IfLE(msg.pe.y);

					// Remember cursor position for this client
					m_client->m_cursor_pos.x = msg.pe.x;
					m_client->m_cursor_pos.y = msg.pe.y;

					// if we share only one window...

					RECT coord;
					{
						omni_mutex_lock l(m_client->m_regionLock);

						coord = m_server->GetSharedRect();
					}

					// to put position relative to screen
					msg.pe.x = (CARD16)(msg.pe.x + coord.left);
					msg.pe.y = (CARD16)(msg.pe.y + coord.top);

					// Work out the flags for this event
					DWORD flags = MOUSEEVENTF_ABSOLUTE;
					flags |= MOUSEEVENTF_MOVE;
					m_server->SetMouseCounter(1, m_client->m_cursor_pos, false );

					if ( (msg.pe.buttonMask & rfbButton1Mask) != 
						(m_client->m_ptrevent.buttonMask & rfbButton1Mask) )
					{
						if (GetSystemMetrics(SM_SWAPBUTTON))
							flags |= (msg.pe.buttonMask & rfbButton1Mask) 
							? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
						else
							flags |= (msg.pe.buttonMask & rfbButton1Mask) 
							? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
						m_server->SetMouseCounter(1, m_client->m_cursor_pos, false);
					}
					if ( (msg.pe.buttonMask & rfbButton2Mask) != 
						(m_client->m_ptrevent.buttonMask & rfbButton2Mask) )
					{
						flags |= (msg.pe.buttonMask & rfbButton2Mask) 
							? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
						m_server->SetMouseCounter(1, m_client->m_cursor_pos, false);
					}
					if ( (msg.pe.buttonMask & rfbButton3Mask) != 
						(m_client->m_ptrevent.buttonMask & rfbButton3Mask) )
					{
						if (GetSystemMetrics(SM_SWAPBUTTON))
							flags |= (msg.pe.buttonMask & rfbButton3Mask) 
							? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
						else
							flags |= (msg.pe.buttonMask & rfbButton3Mask) 
							? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
						m_server->SetMouseCounter(1, m_client->m_cursor_pos, false);
					}

					// Treat buttons 4 and 5 presses as mouse wheel events
					DWORD wheel_movement = 0;
					if ((msg.pe.buttonMask & rfbButton4Mask) != 0 &&
						(m_client->m_ptrevent.buttonMask & rfbButton4Mask) == 0)
					{
						flags |= MOUSEEVENTF_WHEEL;
						wheel_movement = (DWORD)+120;
					}
					else if ((msg.pe.buttonMask & rfbButton5Mask) != 0 &&
						(m_client->m_ptrevent.buttonMask & rfbButton5Mask) == 0)
					{
						flags |= MOUSEEVENTF_WHEEL;
						wheel_movement = (DWORD)-120;
					}

					// Generate coordinate values
// PRB: should it be really only primary rect?
					HWND temp = GetDesktopWindow();
					GetWindowRect(temp,&coord);

					unsigned long x = (msg.pe.x * 65535) / (coord.right - coord.left - 1);
					unsigned long y = (msg.pe.y * 65535) / (coord.bottom - coord.top - 1);

					// Do the pointer event
					::mouse_event(flags, (DWORD)x, (DWORD)y, wheel_movement, 0);
					// Save the old position
					m_client->m_ptrevent = msg.pe;

					// Flag that a remote event occurred
					m_client->m_remoteevent = TRUE;
					m_client->m_pointer_event_time = time(NULL);

					// Flag that the mouse moved
					// FIXME: It should not set m_cursor_pos_changed here.
					m_client->UpdateMouse();

					// Trigger an update
					m_server->RequestUpdate();
				}
			}
			break;

		case rfbClientCutText:
			// Read the rest of the message:
			if (m_socket->ReadExact(((char *) &msg)+1, sz_rfbClientCutTextMsg-1))
			{

				// Allocate storage for the text
				const UINT length = Swap32IfLE(msg.cct.length);
				char *text = new char [length+1];
				if (text == NULL)
					break;
				text[length] = 0;

				// Read in the text
				if (!m_socket->ReadExact(text, length)) {
					delete [] text;
					break;
				}

				// Get the server to update the local clipboard
				if (m_client->IsKeyboardEnabled() && m_client->IsPointerEnabled())
					m_server->UpdateLocalClipText(text);

				// Free the clip text we read
				delete [] text;
			}
			break;

		default:
			// Unknown message, so fail!
			connected = FALSE;
		}
	}

	// Move into the thread's original desktop
	vncService::SelectHDESK(home_desktop);

	// Quit this thread.  This will automatically delete the thread and the
	// associated client.

	// Remove the client from the server, just in case!
	m_server->RemoveClient(m_client->GetClientId());
}

// The vncClient itself

vncClient::vncClient()
{

	m_socket = NULL;
	m_client_name = 0;
	m_server_name = 0;
	m_buffer = NULL;

	m_keyboardenabled = FALSE;
	m_pointerenabled = FALSE;
	m_inputblocked = FALSE;

	m_copyrect_use = FALSE;

	m_mousemoved = FALSE;
	m_ptrevent.buttonMask = 0;
	m_ptrevent.x = 0;
	m_ptrevent.y = 0;

	m_cursor_update_pending = FALSE;
	m_cursor_update_sent = FALSE;
	m_cursor_pos_changed = FALSE;
	m_pointer_event_time = (time_t)0;
	m_cursor_pos.x = -1;
	m_cursor_pos.y = -1;

	m_thread = NULL;
	m_updatewanted = FALSE;

	m_palettechanged = FALSE;

	m_copyrect_set = FALSE;

	m_remoteevent = FALSE;

	// IMPORTANT: Initially, client is not protocol-ready.
	m_protocol_ready = FALSE;
	m_fb_size_changed = FALSE;

	m_use_NewFBSize = FALSE;

	m_protocol_tightvnc=0;
}

vncClient::~vncClient()
{

	// We now know the thread is dead, so we can clean up
	if (m_client_name != 0) {
		free(m_client_name);
		m_client_name = 0;
	}
	if (m_server_name != 0) {
		free(m_server_name);
		m_server_name = 0;
	}

	// If we have a socket then kill it
	if (m_socket != NULL)
	{

		delete m_socket;
		m_socket = NULL;
	}

	// Kill the screen buffer
	if (m_buffer != NULL)
	{

		delete m_buffer;
		m_buffer = NULL;
	}
}

// Init
BOOL
vncClient::Init(vncServer *server,
				VSocket *socket,
				BOOL reverse,
				BOOL shared,
				vncClientId newid)
{
	// Save the server id;
	m_server = server;

	// Save the socket
	m_socket = socket;

	// Save the name/ip of the connecting client
	char *name = m_socket->GetPeerName();
	if (name != 0)
		m_client_name = strdup(name);
	else
		m_client_name = strdup("<unknown>");

	// Save the server name/ip
	name = m_socket->GetSockName();
	if (name != 0)
		m_server_name = strdup(name);
	else
		m_server_name = strdup("<unknown>");

	// Save the client id
	m_id = newid;

	// Spawn the child thread here
	m_thread = new vncClientThread;
	if (m_thread == NULL)
		return FALSE;
	return ((vncClientThread *)m_thread)->Init(this, m_server, m_socket, reverse, shared);

	return FALSE;
}

void
vncClient::Kill()
{
	// Close the socket
	if (m_socket != NULL)
		m_socket->Close();
}

// Client manipulation functions for use by the server
void
vncClient::SetBuffer(vncBuffer *buffer)
{
	// Until authenticated, the client object has no access
	// to the screen buffer.  This means that there only need
	// be a buffer when there's at least one authenticated client.
	m_buffer = buffer;
}


void
vncClient::TriggerUpdate()
{
	// Lock the updates stored so far
	omni_mutex_lock l(m_regionLock);
	if (!m_protocol_ready)
		return;

	if (m_updatewanted)
	{
		// Check if cursor shape update has to be sent
		m_cursor_update_pending = m_buffer->IsCursorUpdatePending();

		// Send an update if one is waiting
		if (!m_changed_rgn.IsEmpty() ||
			!m_full_rgn.IsEmpty() ||
			m_copyrect_set ||
			m_cursor_update_pending ||
			m_cursor_pos_changed ||
			(m_mousemoved && !m_use_PointerPos))
		{
			// Has the palette changed?
			if (m_palettechanged)
			{
				m_palettechanged = FALSE;
				if (!SendPalette())
					return;
			}

			// Now send the update
			m_updatewanted = !SendUpdate();
		}
	}
}

void
vncClient::UpdateMouse()
{
	if (!m_mousemoved && !m_cursor_update_sent)	{
		omni_mutex_lock l(m_regionLock);

		if (IntersectRect(&m_oldmousepos, &m_oldmousepos, &m_server->GetSharedRect()))
			m_changed_rgn.AddRect(m_oldmousepos);

		m_mousemoved = TRUE;
	} else if (m_use_PointerPos) {
		omni_mutex_lock l(m_regionLock);

		SetCursorPosChanged();
	}
}

void
vncClient::UpdateRect(RECT &rect)
{
	// Add the rectangle to the update region
	if (IsRectEmpty(&rect))
		return;

	omni_mutex_lock l(m_regionLock);

	if (IntersectRect(&rect, &rect, &m_server->GetSharedRect()))
		m_changed_rgn.AddRect(rect);
}

void
vncClient::UpdateRegion(vncRegion &region)
{
	// Merge our current update region with the supplied one
	if (region.IsEmpty())
		return;

	{
		omni_mutex_lock l(m_regionLock);

		// Merge the two
		vncRegion dummy;
		dummy.AddRect(m_server->GetSharedRect());
		region.Intersect(dummy);

		m_changed_rgn.Combine(region);
	}
}

void
vncClient::CopyRect(RECT &dest, POINT &source)
{
	// If CopyRect encoding is disabled or we already have a CopyRect pending,
	// then just redraw the region.
	if (!m_copyrect_use || m_copyrect_set) {
		UpdateRect(dest);
		return;
	}

	{
		omni_mutex_lock l(m_regionLock);

		// Clip the destination to the screen
		RECT destrect;
		if (!IntersectRect(&destrect, &dest, &m_server->GetSharedRect()))
			return;

		// Adjust the source correspondingly
		source.x = source.x + (destrect.left - dest.left);
		source.y = source.y + (destrect.top - dest.top);

		// Work out the source rectangle
		RECT srcrect;
		srcrect.left = source.x;
		srcrect.top = source.y;

		// And fill out the right & bottom using the dest rect
		srcrect.right = destrect.right-destrect.left + srcrect.left;
		srcrect.bottom = destrect.bottom-destrect.top + srcrect.top;

		// Clip the source to the screen
		RECT srcrect2;
		if (!IntersectRect(&srcrect2, &srcrect, &m_server->GetSharedRect()))
			return;

		// Correct the destination rectangle
		destrect.left += (srcrect2.left - srcrect.left);
		destrect.top += (srcrect2.top - srcrect.top);
		destrect.right = srcrect2.right-srcrect2.left + destrect.left;
		destrect.bottom = srcrect2.bottom-srcrect2.top + destrect.top;

		// Set the copyrect...
		m_copyrect_rect = destrect;
		m_copyrect_src.x = srcrect2.left;
		m_copyrect_src.y = srcrect2.top;

		m_copyrect_set = TRUE;
	}
}

void
vncClient::UpdateClipText(LPSTR text)
{
	if (!m_protocol_ready) return;

	// Don't send the clipboard contents to a view-only client
	if (!IsKeyboardEnabled() || !IsPointerEnabled())
		return;

	// Lock out any update sends and send clip text to the client
	omni_mutex_lock l(m_sendUpdateLock);

	rfbServerCutTextMsg message;
	message.length = Swap32IfLE(strlen(text));
	if (!SendRFBMsg(rfbServerCutText, (BYTE *) &message, sizeof(message)))
	{
		Kill();
		return;
	}
	if (!m_socket->SendQueued(text, strlen(text)))
	{
		Kill();
		return;
	}
}

void
vncClient::UpdatePalette()
{
	omni_mutex_lock l(m_regionLock);

	m_palettechanged = TRUE;
}

// Functions used to set and retrieve the client settings
const char*
vncClient::GetClientName()
{
	return (m_client_name != NULL) ? m_client_name : "[unknown]";
}

const char*
vncClient::GetServerName()
{
	return (m_server_name != NULL) ? m_server_name : "[unknown]";
}

// Internal methods
BOOL
vncClient::SendRFBMsg(CARD8 type, BYTE *buffer, int buflen)
{
	// Set the message type
	((rfbServerToClientMsg *)buffer)->type = type;

	// Send the message
	if (!m_socket->SendQueued((char *) buffer, buflen))
	{

		Kill();
		return FALSE;
	}
	return TRUE;
}


BOOL vncClient::SendUpdate()
{
#ifndef _DEBUG
	try
	{
#endif
		// First, check if we need to send pending NewFBSize message
		if (m_use_NewFBSize && m_fb_size_changed) {
			SetNewFBSize(TRUE);
			return TRUE;
		}

		vncRegion toBeSent;			// Region to actually be sent
		rectlist toBeSentList;		// List of rectangles to actually send
		vncRegion toBeDone;			// Region to check

		// Prepare to send cursor position update if necessary
		if (m_cursor_pos_changed) {
			POINT cursor_pos;
			if (!GetCursorPos(&cursor_pos)) {
				cursor_pos.x = 0;
				cursor_pos.y = 0;
			}
			RECT shared_rect = m_server->GetSharedRect();
			cursor_pos.x -= shared_rect.left;
			cursor_pos.y -= shared_rect.top;
			if (cursor_pos.x < 0) {
				cursor_pos.x = 0;
			} else if (cursor_pos.x >= shared_rect.right - shared_rect.left) {
				cursor_pos.x = shared_rect.right - shared_rect.left - 1;
			}
			if (cursor_pos.y < 0) {
				cursor_pos.y = 0;
			} else if (cursor_pos.y >= shared_rect.bottom - shared_rect.top) {
				cursor_pos.y = shared_rect.bottom - shared_rect.top - 1;
			}
			if (cursor_pos.x == m_cursor_pos.x && cursor_pos.y == m_cursor_pos.y) {
				m_cursor_pos_changed = FALSE;
			} else {
				m_cursor_pos.x = cursor_pos.x;
				m_cursor_pos.y = cursor_pos.y;
			}
		}

		toBeSent.Clear();
		if (!m_full_rgn.IsEmpty()) {
			m_incr_rgn.Clear();
			m_copyrect_set = false;
			toBeSent.Combine(m_full_rgn);
			m_changed_rgn.Clear();
			m_full_rgn.Clear();
		} else {
			if (!m_incr_rgn.IsEmpty()) {
				// Get region to send from vncDesktop
				toBeSent.Combine(m_changed_rgn);

				// Mouse stuff for the case when cursor shape updates are off
				if (!m_cursor_update_sent && !m_cursor_update_pending) {
					// If the mouse hasn't moved, see if its position is in the rect
					// we're sending. If so, make sure the full mouse rect is sent.
					if (!m_mousemoved) {
						vncRegion tmpMouseRgn;
						tmpMouseRgn.AddRect(m_oldmousepos);
						tmpMouseRgn.Intersect(toBeSent);
						if (!tmpMouseRgn.IsEmpty()) 
							m_mousemoved = TRUE;
					}
					// If the mouse has moved (or otherwise needs an update):
					if (m_mousemoved) {
						// Include an update for its previous position
						if (IntersectRect(&m_oldmousepos, &m_oldmousepos, &m_server->GetSharedRect())) 
							toBeSent.AddRect(m_oldmousepos);
						// Update the cached mouse position
						m_oldmousepos = m_buffer->GrabMouse();
						// Include an update for its current position
						if (IntersectRect(&m_oldmousepos, &m_oldmousepos, &m_server->GetSharedRect())) 
							toBeSent.AddRect(m_oldmousepos);
						// Indicate the move has been handled
						m_mousemoved = FALSE;
					}
				}
				m_changed_rgn.Clear();
			}
		}

		// Get the list of changed rectangles!
		int numrects = 0;
		if (toBeSent.Rectangles(toBeSentList))
		{
			// Find out how many rectangles this update will contain
			rectlist::iterator i;
			int numsubrects;
			for (i=toBeSentList.begin(); i != toBeSentList.end(); i++)
			{
				numsubrects = m_buffer->GetNumCodedRects(*i);

				// Skip remaining rectangles if an encoder will use LastRect extension.
				if (numsubrects == 0) {
					numrects = 0xFFFF;
					break;
				}
				numrects += numsubrects;
			}
		}

		if (numrects != 0xFFFF) {
			// Count cursor shape and cursor position updates.
			if (m_cursor_update_pending)
				numrects++;
			if (m_cursor_pos_changed)
				numrects++;
			// Handle the copyrect region
			if (m_copyrect_set)
				numrects++;
			// If there are no rectangles then return
			if (numrects != 0)
				m_incr_rgn.Clear();
			else
				return FALSE;
		}

		omni_mutex_lock l(m_sendUpdateLock);

		// Otherwise, send <number of rectangles> header
		rfbFramebufferUpdateMsg header;
		header.nRects = Swap16IfLE(numrects);
		if (!SendRFBMsg(rfbFramebufferUpdate, (BYTE *) &header, sz_rfbFramebufferUpdateMsg))
			return TRUE;

		// Send mouse cursor shape update
		if (m_cursor_update_pending) {
			if (!SendCursorShapeUpdate())
				return TRUE;
		}

		// Send cursor position update
		if (m_cursor_pos_changed) {
			if (!SendCursorPosUpdate())
				return TRUE;
		}

		// Encode & send the copyrect
		if (m_copyrect_set) {
			m_copyrect_set = FALSE;
			if(!SendCopyRect(m_copyrect_rect, m_copyrect_src))
				return TRUE;
		}

		// Encode & send the actual rectangles
		if (!SendRectangles(toBeSentList))
			return TRUE;

		// Send LastRect marker if needed.
		if (numrects == 0xFFFF) {
			if (!SendLastRect())
				return TRUE;
		}

		// Both lists should be empty when we exit
		_ASSERT(toBeSentList.empty());
#ifndef _DEBUG
	}
	catch (...)
	{
		throw;
	}
#endif

	return TRUE;
}

// Send a set of rectangles
BOOL
vncClient::SendRectangles(rectlist &rects)
{
	RECT rect;

	// Work through the list of rectangles, sending each one
	while(!rects.empty())
	{
		rect = rects.front();
		if (!SendRectangle(rect))
			return FALSE;

		rects.pop_front();
	}
	rects.clear();

	return TRUE;
}

// Tell the encoder to send a single rectangle
BOOL vncClient::SendRectangle(RECT &rect)
{
	RECT sharedRect;
	{
		omni_mutex_lock l(m_regionLock);
		sharedRect = m_server->GetSharedRect();
	}

	IntersectRect(&rect, &rect, &sharedRect);
	// Get the buffer to encode the rectangle
	UINT bytes = m_buffer->TranslateRect(
		rect,
		m_socket,
		sharedRect.left,
		sharedRect.top);

    // Send the encoded data
    return m_socket->SendQueued((char *)(m_buffer->GetClientBuffer()), bytes);
}

// Send a single CopyRect message
BOOL vncClient::SendCopyRect(RECT &dest, POINT &source)
{
	RECT rc_shr = m_server->GetSharedRect();

	// Create the message header
	rfbFramebufferUpdateRectHeader copyrecthdr;
	copyrecthdr.r.x = Swap16IfLE(dest.left - rc_shr.left);
	copyrecthdr.r.y = Swap16IfLE(dest.top - rc_shr.top);

	copyrecthdr.r.w = Swap16IfLE(dest.right-dest.left);
	copyrecthdr.r.h = Swap16IfLE(dest.bottom-dest.top);
	copyrecthdr.encoding = Swap32IfLE(rfbEncodingCopyRect);

	// Create the CopyRect-specific section
	rfbCopyRect copyrectbody;
	copyrectbody.srcX = Swap16IfLE(source.x - rc_shr.left);
	copyrectbody.srcY = Swap16IfLE(source.y - rc_shr.top);

	// Now send the message;
	if (!m_socket->SendQueued((char *)&copyrecthdr, sizeof(copyrecthdr)))
		return FALSE;
	if (!m_socket->SendQueued((char *)&copyrectbody, sizeof(copyrectbody)))
		return FALSE;

	return TRUE;
}

// Send LastRect marker indicating that there are no more rectangles to send
BOOL
vncClient::SendLastRect()
{
	// Create the message header
	rfbFramebufferUpdateRectHeader hdr;
	hdr.r.x = 0;
	hdr.r.y = 0;
	hdr.r.w = 0;
	hdr.r.h = 0;
	hdr.encoding = Swap32IfLE(rfbEncodingLastRect);

	// Now send the message;
	if (!m_socket->SendQueued((char *)&hdr, sizeof(hdr)))
		return FALSE;

	return TRUE;
}

// Send the encoder-generated palette to the client
// This function only returns FALSE if the SendQueued fails - any other
// error is coped with internally...
BOOL
vncClient::SendPalette()
{
	rfbSetColourMapEntriesMsg setcmap;
	RGBQUAD *rgbquad;
	UINT ncolours = 256;

	// Reserve space for the colour data
	rgbquad = new RGBQUAD[ncolours];
	if (rgbquad == NULL)
		return TRUE;

	// Get the data
	if (!m_buffer->GetRemotePalette(rgbquad, ncolours))
	{
		delete [] rgbquad;
		return TRUE;
	}

	// Compose the message
	omni_mutex_lock l(m_sendUpdateLock);

	setcmap.type = rfbSetColourMapEntries;
	setcmap.firstColour = Swap16IfLE(0);
	setcmap.nColours = Swap16IfLE(ncolours);

	if (!m_socket->SendQueued((char *) &setcmap, sz_rfbSetColourMapEntriesMsg))
	{
		delete [] rgbquad;
		return FALSE;
	}

	// Now send the actual colour data...
	for (UINT i=0; i<ncolours; i++)
	{
		struct _PIXELDATA {
			CARD16 r, g, b;
		} pixeldata;

		pixeldata.r = Swap16IfLE(((CARD16)rgbquad[i].rgbRed) << 8);
		pixeldata.g = Swap16IfLE(((CARD16)rgbquad[i].rgbGreen) << 8);
		pixeldata.b = Swap16IfLE(((CARD16)rgbquad[i].rgbBlue) << 8);

		if (!m_socket->SendQueued((char *) &pixeldata, sizeof(pixeldata)))
		{
			delete [] rgbquad;
			return FALSE;
		}
	}

	// Delete the rgbquad data
	delete [] rgbquad;

	return TRUE;
}

BOOL
vncClient::SendCursorShapeUpdate()
{
	m_cursor_update_pending = FALSE;

	if (!m_buffer->SendCursorShape(m_socket)) {
		m_cursor_update_sent = FALSE;

		return m_buffer->SendEmptyCursorShape(m_socket);
	}

	m_cursor_update_sent = TRUE;
	return TRUE;
}

BOOL
vncClient::SendCursorPosUpdate()
{
	m_cursor_pos_changed = FALSE;

	rfbFramebufferUpdateRectHeader hdr;
	hdr.encoding = Swap32IfLE(rfbEncodingPointerPos);
	hdr.r.x = Swap16IfLE(m_cursor_pos.x);
	hdr.r.y = Swap16IfLE(m_cursor_pos.y);
	hdr.r.w = Swap16IfLE(0);
	hdr.r.h = Swap16IfLE(0);

	return m_socket->SendQueued((char *)&hdr, sizeof(hdr));
}

// Send NewFBSize pseudo-rectangle to notify the client about
// framebuffer size change
BOOL
vncClient::SetNewFBSize(BOOL sendnewfb)
{
	rfbFramebufferUpdateRectHeader hdr;
	RECT sharedRect;

	sharedRect = m_server->GetSharedRect();

	m_full_rgn.Clear();
	m_incr_rgn.Clear();
	m_full_rgn.AddRect(sharedRect);

	if (!m_use_NewFBSize) {
		// We cannot send NewFBSize message right now, maybe later
		m_fb_size_changed = TRUE;

	} else if (sendnewfb) {
		hdr.r.x = 0;
		hdr.r.y = 0;
		hdr.r.w = Swap16IfLE(sharedRect.right - sharedRect.left);
		hdr.r.h = Swap16IfLE(sharedRect.bottom - sharedRect.top);
		hdr.encoding = Swap32IfLE(rfbEncodingNewFBSize);

		rfbFramebufferUpdateMsg header;
		header.nRects = Swap16IfLE(1);
		if (!SendRFBMsg(rfbFramebufferUpdate, (BYTE *)&header,
			sz_rfbFramebufferUpdateMsg))
            return FALSE;

		// Now send the message
		if (!m_socket->SendQueued((char *)&hdr, sizeof(hdr)))
			return FALSE;

		// No pending NewFBSize anymore
		m_fb_size_changed = FALSE;
	}

	return TRUE;
}

void
vncClient::UpdateLocalFormat()
{
	m_buffer->UpdateLocalFormat();
}

