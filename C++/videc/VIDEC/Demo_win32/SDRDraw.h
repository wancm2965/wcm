// SDRDraw.h: interface for the SDRDraw class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SDRDRAW_H__
#define __SDRDRAW_H__


#include "SDRDrawFilter.h"

#include <string>
#include "XAutoLock.h"
//#include "VIDEC_convert.h"
#include "IYUVDraw.h"

#include "../VIDEC_FRE/VIDECFREAPI.h"


class SDRDraw 
	: public IYUVDraw
	, public FrameRateEvent
{
public:
	SDRDraw(int nVirtualWidth,int nVirtualHeight);
	virtual ~SDRDraw(void);
public:
	virtual int Open(HWND hWnd,int nWidth,int nHeight);
	virtual void Close(void);
	
	virtual int ResetImageFormat(int nWidth,int nHeight);

	virtual void SetWindowPos(int x,int y,int cx,int cy); 
	virtual bool Draw(unsigned char*pData,int nLen, int nWidth,int nHeight,int nFrameRate);
	

	virtual void SetVirtualWidth(int nWidth){m_nVirtualWidth=nWidth;};
	virtual void SetVirtualHeight(int nHeight){m_nVirtualHeight=nHeight;};
	virtual int GetVirtualWidth(void){return m_nVirtualWidth;};
	virtual int GetVirtualHeight(void){return m_nVirtualHeight;};
	virtual int GetWidth(void){return m_nWidth;};
	virtual int GetHeight(void){return m_nHeight;};
protected:
	HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);
	IPin * GetInPin ( IBaseFilter *pFilter, int Num );
	IPin * GetOutPin( IBaseFilter *pFilter, int Num );
protected:
	IVideoWindow*			m_pivw;
	IFilterGraph*			m_pifg;
	IMediaControl*			m_pimc;
	SDRDrawFilter*			m_pSDRDrawFilter;
	IBaseFilter*			m_pColour;
	IBaseFilter*			m_pRenderer;

	int						m_nWidth;
	int						m_nHeight;
	HWND					m_hWnd;

	int						m_nVirtualWidth;
	int						m_nVirtualHeight;

	int						m_nFrameRate;
};	


#endif 