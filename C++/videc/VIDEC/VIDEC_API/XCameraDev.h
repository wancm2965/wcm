//XCameraDev.h
#ifndef __XCAMERADEV_H__
#define __XCAMERADEV_H__


#include <winsock2.h>
#include <windows.h>

#include "VIDEC_Camera.h"
#include "libXPreProc.h"
#include "XImageSource.h"
 
class XCameraDev
	: public VIDEC_CameraCallback
	, public IXPreProcessNotify
	, public XImageSource
	, public VIDEC_ImageSource
{
public:
	XCameraDev(unsigned int nDevIndex);
	virtual~XCameraDev(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat);
	void Close(void);
	int Reset(int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat);
	int SetVideoParam(int nBrightness/*����ֵ0-255*/, int nContrast/*�Աȶ�0-127*/, int nSaturation/*���Ͷ�0-127*/, int nHue/*ɫ��0-255*/,int nGamma/*٤��1-400*/,int nSharpen/*��0-127*/);
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);

	bool CanBeClosed(void);
	
	void OnVIDEC_CameraCallbackImageData(VIDEC_Image*pImage);

	void OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage);
	
	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);

	virtual void ShowAdjustVideoWnd(HWND hWnd);
	virtual void ShowVideoInputWnd(HWND hWnd);
	virtual bool HasVideoInputWnd(void);
	virtual int TVTunerPutChannel(long lChannel);

	virtual void SetInvert(int nInvert);
	virtual int GetInvert(void);
	virtual void SetDenoise(int nDenoise);
	virtual int GetDenoise(void);
	virtual void SetLevelFixer(int nLevelFixer);
	virtual int GetLevelFixer(void);
	
	virtual void EnableOSD(bool bEnable);
	virtual bool IsOSDEnable(void);
	virtual void SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);

	//�������ڲ���������
	virtual int SetupMask(RECT *RectList, int iRectCount);
	//ֹͣ����
	virtual int StopMask(void);
protected:
	VIDEC_Camera*		m_pCamera;
	IXPreProcess*		m_pIXPreProcess;

	unsigned int		m_nDevIndex;

	int					m_nWidth;
	int					m_nHeight;
	int					m_nFrameRate;
	bool				m_bDefaultFormat;

	bool				m_isUSBCamera;
	bool				m_bLifeCamera;
	bool				m_bAS120;

	int					m_nVirtualWidth;
	int					m_nVirtualHeight;
	char *              m_pConvertUV;
	bool                m_bRBSCamera;   //�촴��� RBS ��������ͷ 
};

#endif