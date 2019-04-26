
#ifndef __D3D_DISPLAY_H__
#define __D3D_DISPLAY_H__

#include "VIDEC.h"
#include "XThreadBase.h"
#include "../VIDEC_Codecs/VIDECD3DWrapper.h"
#include "VideoDisplay.h"



class D3DDisplay 
	: public XThreadBase
	, public VideoDisplay
{
public:
	D3DDisplay(VideoDisplayCallback&rCallback);
	virtual ~D3DDisplay(void);
public:
	virtual int Open(void*pParent);
	virtual void Close(void);

	virtual int GetWidth(void);
	virtual int GetHeight(void);

	virtual int SetTitle(const char*cszTitle,unsigned long nColor);

	virtual void SetImageSizeChanged(void);

	virtual void DisplayFrame(unsigned char*pYUV420Data,int nLen,int nWidth,int nHeight,int nFrameRate);

	LRESULT handleEvents(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	void ThreadProcMain(void);
	void ResetD3D(void);
private:
	VideoDisplayCallback&	m_rCallback;
	
	bool			m_bRunning;
	void*			m_hD3DPlayer;
	HWND			m_hWndParent;

	bool			m_bD3DReset;
	int				m_nImageWidth;
	int				m_nImageHeight;

	bool			m_bWTSSessionLock;
	bool			m_bForceDestroyDraw;


	UINT32			m_nTimerID;

	WNDPROC			m_pParentWindowProc;
};

#endif
