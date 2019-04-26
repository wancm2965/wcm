// VideoPlayerWnd.h: interface for the VideoPlayerWnd class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LOCALVIDEOWINDOW_H__
#define __LOCALVIDEOWINDOW_H__

#include <map>
#include "VIDEC.h"
#include "DoubleFrameRate.h"
#include "VideoDisplay.h"

#include "../VIDEC_API/XImageSink.h"
#include "VIDEC_convert.h"
#include "XThreadBase.h"
#include "libXPreProc.h"
#include "../VIDEC_MP/ImageInput.h"
#include "MyBitmap.h"

#include "../VIDEC_ICE/ImageColorEnhance.h"
#include "H264Player.h"

#define MAX_FRAME_INTERVAL_COUNT	60


class VideoPlayerWnd  
	: public CWnd
	, public DoubleFrameRate
	, public IXPreProcessNotify
	, public ImageInput
{
	DECLARE_DYNAMIC(VideoPlayerWnd)
protected:
	//{{AFX_MSG(VideoPlayerWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnImageChanged(WPARAM wParam, LPARAM lParam);
public:
	VideoPlayerWnd(YNCD3D_CB_DC cbdc,void*pUserData);
	virtual ~VideoPlayerWnd();
public:
	int Open(HWND hwndParent);
	void Close(void);
	
	int SetBgBmp(char* sFile);
	int SetBgRgb(int r, int g, int b);

	int GetVideoWidth(void);
	int GetVideoHeight(void);

	HWND GetWnd(void);
	bool IsWindowVisible(void);
	bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags);

	//设置视频属性
	virtual int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	

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
	void ResetImage(VIDEC_Image*pImage);

	double GetAverageFPS(void);


	bool CaptureToBMPFile(const char*cszFile);
	
	bool HasVideo(void);

	virtual void SetEnableVideo(int nEnable);
	virtual int GetEnableVideo(void);

	void OnReceivedImage(VIDEC_Image*pImage);
	void OnReceivedPreSend(int& nSend);
	void PausePlay(bool bPause,bool bClear);
	void SetEnableD3D(bool bEnabled);
	int GetVideoShowFrameRate(void);
	int GetVideoShowType(void);
	virtual void TurnOnGPU(int nWidth,int nHeight);
	virtual void TurnOffGPU(void);
	virtual bool CanUseGPU(int nWidth,int nHeight,int nVirtualWidth,int nVirtualHeight);
	virtual bool IsTryingGPU(void);
	virtual bool IsUsingGPU(void);
	virtual int PlayFrame(const unsigned char *pFrameData, int nFrameLen,int nKeyFrame,int nFrameRate);
	virtual void SkipOneFrame(void);
	virtual void RepeatFrame(void);
protected:
	void ResizeControls(void);
	void OnFrameData(XImage*pXImage);

	void OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage);
	virtual void Reset(void);

	bool DisplayImage(XImage*pXImage);
protected:
	HWND					m_hWndParent;
	VideoDisplay*			m_pVideoDisplay;
	VIDEC_Image*			m_pImage;
	XCritSec				m_csImage;
	int						m_nLastWidth;
	unsigned long			m_nLastGetImageTimestamp;

	int						m_nActualWidth;
	int						m_nActualHeight;
	int						m_nVirtualWidth;
	int						m_nVirtualHeight;
	int						m_nFrameRate;
	bool					m_bDoubleField;
	bool					m_bEvenField;
	bool					m_bOddField;

	bool					m_bFirstDraw;
	bool					m_bImageChanged;

	bool					m_bShowWindow;

	int						m_nFrameCount;
	unsigned long			m_ulFrameCountTimestamp;


	bool					m_bEnableD3D;
	bool					m_bGotFirstVideo;
	bool					m_bRunning;
	int						m_nRedraw;

	
	IXPreProcess*			m_pIXPreProcess;
	int						m_nBrightness;
	int						m_nContrast;
	int						m_nSaturation;
	int						m_nHue;
	int						m_nGamma;
	int						m_nSharpen;
	int						m_nGammaR;
	int						m_nGammaG;
	int						m_nGammaB;

	int						m_nScreenIndex;
	int						m_nWndIndex;
	int						m_nFullScreen;

	int						m_nEnablePreview;
#ifdef _DEMO
	unsigned long			m_ulFirstTimestamp;
#endif

	bool					m_bReset;


	int						m_nActualScreenIndex;
	bool					m_bActualScreenIndexChanged;


	unsigned long			m_nLastDrawTimestamp;

	CMyBitMap*				m_pBitmapBG;

	unsigned long			m_nLastFrameRateTimestamp;
	int						m_nLastFrameRate;
	unsigned long			m_nLastFrameTimestamp;
	unsigned long			m_nLastFrameCount;						

	int						m_nFrameIntervalCount;
	typedef std::map<unsigned long,int>	MapFrameInterval;
	MapFrameInterval		m_MapFrameInterval;
	unsigned long			m_nLastSizingTimestamp;

	ImageColorEnhance*		m_pICE;

	int						m_nEnableVideo;

	H264Player*				m_pH264Player;
	int						m_nWidth;
	int						m_nHeight;
	bool					m_bTryingGPU;
	bool					m_bUsingGPU;
	bool					m_bH264PlayerFailed;

	YNCD3D_CB_DC			m_cbdc;
	void*					m_pUserData;

	int						m_nOldScreenIndex;

	char					m_sBgBmp[256];
	int						m_nBgBmpWidth;
	int						m_nBgBmpHeight;
	int						m_nBgColorR;
	int						m_nBgColorG;
	int						m_nBgColorB;
};

#endif 