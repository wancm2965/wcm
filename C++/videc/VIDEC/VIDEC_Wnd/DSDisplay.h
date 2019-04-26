//DSDisplay.h
#ifndef __DS_DISPLAY_H__
#define __DS_DISPLAY_H__

#include "VideoDisplay.h"
#include "XThreadBase.h"
#include "XWorkThread.h"
#include "SDRDraw.h"
#include "../VIDEC_Codecs/VIDECD3DWrapper.h"

class DSDisplay
	: public VideoDisplay
	, public XWorkThread
{
public:
	DSDisplay(VideoDisplayCallback&rCallback);
	virtual~DSDisplay(void);
public:
	virtual int Open(void*pParent);
	virtual void Close(void);

	virtual int GetWidth(void);
	virtual int GetHeight(void);
	virtual int SetTitle(const char*cszTitle,unsigned long nColor);
	virtual void SetImageSizeChanged(void);

	LRESULT handleEvents(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	virtual bool IsPending(void);
	virtual bool Running(void);
protected:
	VideoDisplayCallback&	m_rCallback;
	HWND					m_hVideoWnd;
	int						m_nWidth;
	int						m_nHeight;

	void*					m_hD3DPlayer;
	SDRDraw*				m_pDraw;
	bool					m_bDSReset;

	bool					m_bWTSSessionLock;
	bool					m_bImageSizeChanged;
	bool					m_bForceDestroyDraw;

	unsigned long			m_nLastDrawTimestamp;
	unsigned long			m_nNextFrameInterval;

	WNDPROC			m_pParentWindowProc;
};

#endif