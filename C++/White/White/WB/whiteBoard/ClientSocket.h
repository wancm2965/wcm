#pragma once

// CClientSocket ÃüÁîÄ¿±ê

class CClientSocket : public CSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	virtual void OnReceive(int nErrorCode);
	HWND getWBViewHwnd();
	void setWBViewHwnd(HWND tempHwnd = NULL);

private:
	HWND m_WBHwnd;
};


