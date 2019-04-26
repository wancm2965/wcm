// DShowPicture.h: interface for the HPDShowPicture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HPDSHOWPICTURE_H__
#define __HPDSHOWPICTURE_H__


#include <streams.h>
#include <atlbase.h>
#include <initguid.h>

#include "VIDEC_Picture.H"

class DShowPicture 
	: public VIDEC_Picture  
{
public:
	DShowPicture(void);
	virtual ~DShowPicture(void);
public:
	int Open(HWND hWnd,const char*cszPathName);
	void Close(void);
	void ShowWindow(bool bShow);
	bool IsWindowVisible(void);
	void SetWindowPosition(int x,int y,int cx,int cy);
	int GetWidth(void);
	int GetHeight(void);
protected:
	CComPtr<IGraphBuilder>		m_pGraphBuilder;
	CComPtr<IMediaControl>		m_pMediaControl;
	CComPtr<IVideoWindow>		m_pVideoWindow;
	bool						m_bIsWindowVisible;
	int							m_nWidth;
	int							m_nHeight;
};

#endif 