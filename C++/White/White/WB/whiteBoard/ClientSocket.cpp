// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "ClientSocket.h"


// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 成员函数

void CClientSocket::OnReceive(int nErrorCode)
{
//	char* buff = new char[2048];
	/*
	if (NULL == buff)
	{
		DWORD dwLen = 2048;
		pLine->Load(buff, dwLen);
		m_pWBPage->AddWBObject(pLine);	
	}
	*/
	HWND temp;
	temp = getWBViewHwnd();
	::SendMessage(getWBViewHwnd(),WM_WB_RECEIVERSOCKET,0,0);
	AsyncSelect(FD_READ); 
}

HWND CClientSocket::getWBViewHwnd()
{
	return m_WBHwnd;
}

void CClientSocket::setWBViewHwnd(HWND tempHwnd)
{
	m_WBHwnd = tempHwnd;
}