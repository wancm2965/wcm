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

	//ȡ��Ƶ���
	virtual int GetVideoWidth(void);
	//ȡ��Ƶ�߶�
	virtual int GetVideoHeight(void);

	//ȡ��Ƶ����
	virtual HWND GetWnd(void);
	//same as CWnd::IsWindowVisible
	virtual bool IsWindowVisible(void);
	//same as CWnd::SetWindowPos
	virtual bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags); 

	//���ֵ�ǰͼ��BMP�ļ�
	virtual bool CaptureToBMPFile(const char*cszFile);

	virtual double GetAverageFPS(void);

	//������Ƶ����
	virtual int SetVideoParam(int nBrightness=128/*����ֵ0-255*/, int nContrast=64/*�Աȶ�0-127*/, int nSaturation=64/*���Ͷ�0-127*/, int nHue=0/*ɫ��0-255*/,int nGamma=100/*٤��1-400*/,int nSharpen=0/*��0-127*/);
	//int nGammaR[1-400 100]
	//int nGammaG[1-400 100]
	//int nGammaB[1-400 100]
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	//����ͼ����ʾģʽ

	/******************************************************************************
	* SetScreenWnd
	*����������������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	virtual void EnablePreview(int nPreview);

	virtual void ResizeControls(void)=0;
	virtual bool HasVideo(void)=0;
protected:
	CMyBitMap*				m_pBitmapBG;

};

#endif
