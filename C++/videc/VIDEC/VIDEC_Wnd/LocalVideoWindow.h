// LocalVideoWindow.h: interface for the LocalVideoWindow class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LOCALVIDEOWINDOW_H__
#define __LOCALVIDEOWINDOW_H__

#include "VIDEC.h"
#include "DoubleFrameRate.h"
#include "../VIDEC_API/XImageSink.h"
#include "../VIDEC_MP/ImageInput.h"
#include "MyBitmap.h"
#include "../VIDEC_ICE/ImageColorEnhance.h"
//#include "../VIDEC_Codecs/VIDECD3DWrapper.h"
#include "VideoDisplay.h"
#include "libXPreProc.h"
class LocalVideoWindow  
	: public CWnd
	, public VIDEC_Wnd
	, public XImageSink
	, public DoubleFrameRate
	, public ImageInput
	, public IXPreProcessNotify
{
	DECLARE_DYNAMIC(LocalVideoWindow)
protected:
	//{{AFX_MSG(LocalVideoWindow)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnImageChanged(WPARAM wParam, LPARAM lParam);
public:
	LocalVideoWindow(VIDEC_WndCallback&rCallback);
	virtual ~LocalVideoWindow();
public:
	int Open(HWND hwndParent, bool bLocal, unsigned long*lpnVidFrameTimestamp, unsigned long*lpnAudFrameTimestamp, char* pBgBmp = NULL, int bgcolr = -1, int bgcolg = -1, int bgcolb = -1);
	void Close(void);

	int GetVideoWidth(void);
	int GetVideoHeight(void);

	HWND GetWnd(void);
	bool IsWindowVisible(void);
	bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags);


	//设置视频属性
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

	virtual void SetEnableVideo(int nEnable);
	virtual int GetEnableVideo(void);

	void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
	void OnReceivedImage(VIDEC_Image*pImage);	
	void OnReceivedPreSend(int& nSend);	


	virtual void SupportPreProcess(void);
	virtual void EnableOSD(bool bEnable);
	virtual bool IsOSDEnable(void);
	virtual void SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);
	virtual void SetDenoise(int nDenoise);
	virtual int GetDenoise(void);
	int SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/);
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	void OnCallbackDrawD3D9DC(void* hDC);
	virtual int GetVideoShowType(void);
	virtual int GetVideoShowFrameRate(void);
protected:
	void ResizeControls(void);
	void OnFrameData(XImage*pXImage);

	void OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage);
	virtual void Reset(void);

	bool DisplayImage(XImage*pXImage);

private:
	int SetBgBmp(char* sFile);
	int SetBgRgb(int r, int g, int b);

protected:
	HWND					m_hWndParent;
	bool					m_bReSizeMax;
	bool					m_bReSizeMin;
	int						m_nLastWidth;
	VideoDisplay*			m_pVideoDisplay;
	IXPreProcess*			m_pIXPreProcess;
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



	CMyBitMap*				m_pBitmapBG;

	unsigned long			m_nLastSizingTimestamp;

	ImageColorEnhance*		m_pICE;

	int						m_nEnableVideo;


	int						m_nNoPreviewWidth;
	int						m_nNoPreviewHeight;

	VIDEC_WndCallback&		m_rCallback;

	char					m_sBgBmp[256];
	int						m_nBgBmpWidth;
	int						m_nBgBmpHeight;
	int						m_nBgColorR;
	int						m_nBgColorG;
	int						m_nBgColorB;
};

#endif 