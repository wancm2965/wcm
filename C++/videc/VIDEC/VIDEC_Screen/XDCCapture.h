// XDCCapture.h: interface for the XDCCapture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XDCCAPTURE_H__
#define __XDCCAPTURE_H__

#include "XDCCaptureSlice.h"
#include "XThreadBase.h"
#include "XUtil.h"
#include "VIDEC_DC.h"
#include "../VIDEC_API/XImageSource.h"
#include "../VIDEC_API/XImage.h"
#include "XOSDFilter.h"

typedef struct tagXDC_SLICE_YV12
{
	unsigned char*	pYV12[3];
	int				width;
	int				height;
}XDC_SLICE_YV12,*PXDC_SLICE_YV12;

class XDCCapture 
	: public VIDEC_DC
	, public XImageSource
{
public:
	XDCCapture(void);
	virtual ~XDCCapture(void);
public:
	int Open(HDC hDC,int x,int y,int cx,int cy);
	void Close(void);
	int Reset(HDC hDC,int x,int y,int cx,int cy);
	
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
	
	int RequestFrame(void);

	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);
protected:
	int ResetMem(void);
	int InitMem(void);
	void ClearMem(void);

	void ThreadProcMain(void);
protected:
	XDCCaptureSlice*			m_paXDCCaptureSlice;
	PXDC_SLICE_YV12				m_paXDCSliceYV12;
	int							m_nSliceCount;
	int							m_nBitCount;

	int							m_nX;
	int							m_nY;
	int							m_nCX;
	int							m_nCY;

	PBYTE						m_pYV12Buffer;
	int							m_nYV12Size;

	HDC							m_hDC;
	HDC							m_hNullDC;

	XOSDFilter					m_XOSDFilter;
	bool						m_bEnableOSD;
	CString						m_strOSD;
	int							m_nAlign;
	COLORREF					m_crText;
	COLORREF					m_crBk;
	LOGFONTA					m_fontOSD;
	int							m_nTop;
	int							m_nLeft;

	int							m_nSliceHeight;
	bool						m_bIsVistaOrAbove;
};

#endif 