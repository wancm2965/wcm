// LocalVideoWnd.h: interface for the LocalVideoWnd class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LOCALVIDEOWND_H__
#define __LOCALVIDEOWND_H__

#include "VIDEC.h"
#include "DoubleFrameRate.h"
#include "../VIDEC_API/XImageSink.h"
#include "../VIDEC_MP/ImageInput.h"
#include "MyBitmap.h"
#include "../VIDEC_ICE/ImageColorEnhance.h"
#include <AVCONAVC/AVCONAVC_VideoDraw.h>

class LocalVideoWnd  
	: public CWnd
	, public VIDEC_Wnd
	, public XImageSink
	, public DoubleFrameRate
	, public ImageInput
{
	DECLARE_DYNAMIC(LocalVideoWnd)
protected:
	//{{AFX_MSG(LocalVideoWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnImageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWTSessionChanged(WPARAM wParam, LPARAM lParam);
public:
	LocalVideoWnd(void);
	virtual ~LocalVideoWnd();
public:
	int Open(HWND hwndParent,bool bLocal,unsigned long*lpnVidFrameTimestamp,unsigned long*lpnAudFrameTimestamp);
	void Close(void);

	int GetVideoWidth(void);
	int GetVideoHeight(void);

	HWND GetWnd(void);
	bool IsWindowVisible(void);
	bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags);

	//设置视频属性
	virtual int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	virtual void SetLevelFixer(int nLevelFixer);
	virtual int GetLevelFixer(void);
	
	virtual int SetImageDisplayMode(int nMode);
	virtual int GetImageDisplayMode(void);

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

	virtual void EnablePreview(int nPreview);
	virtual int GetSupportPreview(void);
	virtual VIDEC_Image*GetImage(void);

	double GetAverageFPS(void);


	bool CaptureToBMPFile(const char*cszFile);
	
	void SetSynFlag(int nSynFlag);
	bool HasVideo(void);


	void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
	void OnReceivedImage(VIDEC_Image*pImage);
protected:
	void ResizeControls(void);
	void OnFrameData(XImage*pXImage);

	void OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage);
	virtual void Reset(void);

	bool DisplayImage(XImage*pXImage);
	
protected:
	HWND					m_hWndParent;

	AVCONAVC_VideoDraw*		m_pVideoDraw;
	VIDEC_Image*			m_pImage;
	XCritSec				m_csImage;

	int						m_nActualWidth;
	int						m_nActualHeight;
	int						m_nVirtualWidth;
	int						m_nVirtualHeight;
	int						m_nFrameRate;
	bool					m_bDoubleField;
	bool					m_bEvenField;
	bool					m_bOddField;

	bool					m_bImageChanged;

	bool					m_bShowWindow;

	int						m_nFrameCount;
	unsigned long			m_ulFrameCountTimestamp;


	bool					m_bGotFirstVideo;

	int						m_nScreenIndex;
	int						m_nWndIndex;
	int						m_nFullScreen;

	int						m_nEnablePreview;

	bool					m_bReset;


	int						m_nActualScreenIndex;
	bool					m_bActualScreenIndexChanged;
	int						m_nActualScreenWidth;
	int						m_nActualScreenHeight;


	bool					m_bDrawFailed;
	bool					m_bForceDestroy;
	bool					m_bWTLocked;
	int						m_nWTUnlockedDrawFailedCount;
	unsigned long			m_nLastWTUnLockedTimestamp;
	unsigned long			m_nFirstDrawFailedTimestamp;

	CMyBitMap*				m_pBitmapBG;

	unsigned long			m_nLastSizingTimestamp;

	ImageColorEnhance*		m_pICE;
};

#endif 