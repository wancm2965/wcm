//VideoDraw.h
#ifndef __VIDEO_DRAW_H__
#define __VIDEO_DRAW_H__

#include <AVCONAVC/AVCONAVC_VideoDraw.h>

#include "VideoDrawFilter.h"

class VideoDraw
	: public AVCONAVC_VideoDraw
{
public:
	VideoDraw(void);
	virtual~VideoDraw(void);
public:
	virtual int Open(HWND hWndVideo,int nWidth,int nHeight,bool bNV12,bool& bTryEVR);
	virtual void Close(void);
	virtual void Repaint(void);
	virtual void SetWndPos(int x,int y,int cx,int cy);
	virtual int GetWidth(void);
	virtual int GetHeight(void);
	virtual bool GetNV12(void);
	virtual int Draw(unsigned char*pYUV,int nStride);
protected:
	HWND						m_hWndVideo;
	int							m_nWidth;
	int							m_nHeight;
	bool						m_bNV12;
	CComPtr<IGraphBuilder>		m_pGraph;
	IMFVideoDisplayControl*		m_pVideoDisplay;
	IVMRWindowlessControl*		m_pWindowless;
	VideoDrawFilter*			m_pVideoDrawFilter;
};

#endif
