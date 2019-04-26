#pragma once

// CDllClientSocket ÃüÁîÄ¿±ê

class AFX_EXT_CLASS CDllClientSocket : public CSocket
{
public:
	CDllClientSocket();
	virtual ~CDllClientSocket();
	virtual void OnReceive(int nErrorCode);
	HWND getWBViewHwnd();
	void setWBViewHwnd(HWND tempHwnd = NULL);

private:
	HWND m_WBHwnd;

};