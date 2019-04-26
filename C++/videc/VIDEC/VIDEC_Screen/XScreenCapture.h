// XScreenCapture.h: interface for the XScreenCapture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XSCREENCAPTURE_H__
#define __XSCREENCAPTURE_H__

#include "XScreenCaptureSlice.h"
#include "XThreadBase.h"
#include "XUtil.h"
#include "VIDEC_Screen.h"
#include "../VIDEC_API/XImageSource.h"
#include "../VIDEC_API/XImage.h"
#include "OSD.h"

#include "../VIDEC_MP/ImageInput.h"

typedef struct tagXSCREEN_SLICE_YV12
{
	unsigned char*	pYV12[3];
	int				width;
	int				height;
}XSCREEN_SLICE_YV12,*PXSCREEN_SLICE_YV12;

class XScreenCapture 
	: public XThreadBase
	, public VIDEC_Screen
	, public XImageSource
	, public ImageInput
{
public:
	XScreenCapture(HDC hDC);
	virtual ~XScreenCapture(void);
public:
	int Open(int x,int y,int cx,int cy,int nFrameRate,int nBitCount);
	void Close(void);
	int Reset(int x,int y,int cx,int cy,int nFrameRate,int nBitCount);
	
	void SetInvert(int nInvert){};
	int GetInvert(void){return 0;};
	void SetDenoise(int nDenoise){};
	int GetDenoise(void){return 0;};
	void SetLevelFixer(int nLevelFixer){};
	int GetLevelFixer(void){return 0;};
	
	virtual void EnableOSD(bool bEnable);
	virtual bool IsOSDEnable(void);
	virtual void SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);
	
	int Move(int x,int y);

	int GetRect(int& x,int& y,int& cx,int& cy);

	int EnableCursor(bool bEnable);

	int RequestFrame(void);


	/******************************************************************************
	* SetScreenWnd
	*描述：设置所在屏幕窗口位置
	*输入：	nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);

	virtual VIDEC_Image*GetImage(void);
protected:
	int ResetMem(void);
	int InitMem(void);
	void ClearMem(void);

	bool InitDisplay(void);
	void ClearDisplay(void);
	void ThreadProcMain(void);
protected:
	HANDLE						m_evFrameInterval;

	XScreenCaptureSlice*		m_paXScreenCaptureSlice;
	PXSCREEN_SLICE_YV12			m_paXScreenSliceYV12;
	int							m_nSliceCount;
	int							m_nBitCount;

	int							m_nX;
	int							m_nY;
	int							m_nCX;
	int							m_nCY;
	unsigned long				m_ulFrameInterval;
	int							m_nFrameRate;

	PBYTE						m_pYV12Buffer;
	int							m_nYV12Size;

	HDC							m_hDDC;
	HDC							m_hNullDC;

	bool						m_bRunning;
	bool						m_bRequestFrame;

	bool						m_bEnableCursor;

	OSD							m_OSD;
	bool						m_bEnableOSD;
	CString						m_strOSD;
	int							m_nAlign;
	int							m_nTop;
	int							m_nLeft;
	COLORREF					m_crText;
	COLORREF					m_crBk;
	LOGFONTA					m_fontOSD;

	VIDEC_Image*				m_pImage;
	XCritSec					m_csImage;

	int							m_nSkipFrameCount;

	int							m_nSliceHeight;
	bool						m_bIsVistaOrAbove;

	HDC							m_hDC;
};

#endif 