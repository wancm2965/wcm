#ifndef __XPREPROSS_H__
#define __XPREPROSS_H__

#include "libXPreProc.h"
#include "libxpostproc.h"
#include "XAutoLock.h"
#include "VIDEC_convert.h"
#include "VIDEC_PictProp.h"
#include "../VIDEC_API/XImage.h"
#include "OSD.h"

#include <AVCONAVC/AVCONAVC.h>

class XPreProcess :
	public IXPreProcess
{
public:
	XPreProcess(IXPreProcessNotify&rIXPreProcessNotify);
	virtual ~XPreProcess(void);
public:
	bool Connect(void);
	void ReleaseConnections(void);
	void PreProcess(VIDEC_Image*pImage);
	void EnableI420(bool bEnable);
	bool IsI420Enable(void);
	void EnableInvert(bool bEnable);
	bool IsInvertEnable(void);
	void EnableDenoise(bool bEnable);
	bool IsDenoiseEnable(void);
	
	void EnableInterlaced(bool bEnable);
	bool IsInterlacedEnable(void);
	void EnableDeinterlaced(bool bEnable,bool bDoubleFrameRate);
	bool IsDeinterlacedEnable(void);

	void SetImageEnhance(bool bEnable);

	//设置遮掩并启动遮掩
	bool SetupMask(RECT *RectList, int iRectCount);
	//停止遮掩
	bool StopMask(void);

	void SetPictProp(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/);
	virtual void SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	
	virtual void EnableOSD(bool bEnable);
	virtual bool IsOSDEnable(void);
	virtual void SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);
protected:
	void OutputData(XImage*pXImage);
protected:
	IXPreProcessNotify&		m_rIXPreProcessNotify;
	int						m_nWidth;
	int						m_nHeight;
	unsigned char*			m_pBuffer;

	int						m_nYSize;
	int						m_nUVSize;
	int						m_naStride[3];

	bool					m_bI420Enable;
	bool					m_bInvertEnable;
	bool					m_bDenoiseEnable;

	bool					m_bInterlacedEnable;
	bool					m_bDeinterlacedEnable;
	bool					m_bDoubleFrameRate;

	VIDEC_Denoise*			m_pDenoise;
	IXDeinterlace*			m_pDeinterlace;


	XCritSec				m_csRectList;
	RECT *					m_pRectList;
	int						m_iRectCount;

	unsigned char*			m_pFrameBuf;
	int						m_nBufLen;

	VIDEC_PictProp*			m_pPictProp;
	int						m_nContrast;
	int						m_nBrightness;
	int						m_nHue;
	int						m_nSaturation;
	int						m_nGamma;
	int						m_nGammaR;
	int						m_nGammaG;
	int						m_nGammaB;
	int						m_nSharpen;
	bool					m_bResetPictProp;
	bool					m_bDoPictProp;

	OSD						m_OSD;
	bool					m_bEnableOSD;
	CString					m_strOSD;
	int						m_nAlign;
	COLORREF				m_crText;
	COLORREF				m_crBk;
	LOGFONTA				m_fontOSD;
	int						m_nTop;
	int						m_nLeft;

	bool					m_bEnableImageEnhance;
	void*					m_pContrastEnhance;
	int						m_nContrastEnhanceAlpha;
};


#endif