//VNCClientWnd.h

#ifndef __VNCCLIENTWND_H__
#define __VNCCLIENTWND_H__

#pragma once

#include "stdhdrs_client.h"

class WinVNCClient;
class VNCClientWnd  
{
public:
	VNCClientWnd(WinVNCClient&rWinVNCClient,unsigned short&nPort);
	virtual ~VNCClientWnd();
public:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
protected:
	WinVNCClient&	m_rWinVNCClient;
	SOCKET			m_hSocketListening;
	HWND			m_hWnd;
};

#endif // __VNCCLIENTWND_H__

