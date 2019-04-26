//XVideoDisplay.h
#ifndef __DS_DISPLAY_H__
#define __DS_DISPLAY_H__

#include "VideoDisplay.h"
#include "SDRDraw.h"
//#include "../VIDEC_Codecs/VIDECD3DWrapper.h"
#include "../VIDEC_Codecs/VIDECYNCWrapper.h"

#include "XAutoLock.h"
#include "XListPtr.h"
#include "XThreadBase.h"

class XVideoDisplay
	: public VideoDisplay
	, public XThreadBase
{
public:
	XVideoDisplay(YNCD3D_CB_DC cbdc,void*pUserData);
	virtual~XVideoDisplay(void);
public:

	virtual int Open(void*pParent);
	virtual void Close(void);

	virtual int GetWidth(void);
	virtual int GetHeight(void);

	virtual void DisplayImage(VIDEC_Image*pImage);
	virtual void SetEnableD3D(bool bEnabled);
	virtual void PausePlay(bool bPause,bool bClear);

	virtual int GetVideoShowFrameRate(void);
	virtual int GetVideoShowType(void);

	LRESULT handleEvents(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	virtual void ThreadProcMain(void);
protected:
	XListPtr				m_listImage;
	XCritSec				m_csListImage;
	bool					m_bRunning;
	bool					m_bFirstDraw;
	bool					m_bPause;
	HWND					m_hVideoWnd;
	int						m_nWidth;
	int						m_nHeight;

	int						m_nRedraw;
	void*					m_hD3DPlayer;
	SDRDraw*				m_pDraw;
	bool					m_bDSReset;

	bool					m_bWTSSessionLock;
	bool					m_bImageSizeChanged;
	bool					m_bForceDestroyDraw;
	int						m_nDrawErrorCount;
	int						m_nSkipFrameCount;
	int						m_nFrameCount;
	unsigned long			m_nLastTS;

	WNDPROC					m_pParentWindowProc;

	YNCD3D_CB_DC			m_cbdc;
	void*					m_pUserData;
	bool					    m_bEnableD3D;
	int                       m_nDrawResetErrorCount;
    unsigned long   m_nLastGetImageTimestamp;//

public:
	virtual void ScreenChangeReopenDraw(void);


};

#endif