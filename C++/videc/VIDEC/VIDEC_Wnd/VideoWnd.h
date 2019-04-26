//VideoWnd.h
#ifndef __VIDEO_WND_H__
#define __VIDEO_WND_H__

#include <VIDEC/VIDEC_VideoWnd.h>
#include "../VIDEC_MP/ImageInput.h"
#include "MyBitmap.h"

class VideoWnd
	: public CWnd
	, public ImageInput
{
	DECLARE_DYNAMIC(VideoWnd)
protected:
	//{{AFX_MSG(VideoWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	VideoWnd(void);
	virtual~VideoWnd(void);
public:
	virtual BOOL DestroyWindow();

	//取视频宽度
	virtual int GetVideoWidth(void);
	//取视频高度
	virtual int GetVideoHeight(void);

	//取视频窗口
	virtual HWND GetWnd(void);
	//same as CWnd::IsWindowVisible
	virtual bool IsWindowVisible(void);
	//same as CWnd::SetWindowPos
	virtual bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags); 

	//保持当前图象到BMP文件
	virtual bool CaptureToBMPFile(const char*cszFile);

	virtual double GetAverageFPS(void);

	//设置视频属性
	virtual int SetVideoParam(int nBrightness=128/*亮度值0-255*/, int nContrast=64/*对比度0-127*/, int nSaturation=64/*饱和度0-127*/, int nHue=0/*色调0-255*/,int nGamma=100/*伽马1-400*/,int nSharpen=0/*锐化0-127*/);
	//int nGammaR[1-400 100]
	//int nGammaG[1-400 100]
	//int nGammaB[1-400 100]
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	//设置图像显示模式

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

	virtual void ResizeControls(void)=0;
	virtual bool HasVideo(void)=0;
protected:
	CMyBitMap*				m_pBitmapBG;

};

#endif
