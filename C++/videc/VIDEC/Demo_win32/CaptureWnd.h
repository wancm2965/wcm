#pragma once
#include "stdafx.h"



#include <HPAVC_D3DRender.h>

//#include "imxec/IMXEC_MediaChannel.h"

#include "EncodeFrame.h"

#include "../../include/VIDECHGR/VIDECHGRAPI.h"

#include "XThreadBase.h"
#include "XCritSec.h"
#include "XListPtr.h"
#include "XAutoLock.h"
#include "OpAH2K.h"
#include "OpVidecCamera.h"

#include "Resource.h"

#include "..\..\include\VIDEC\VIDEC.h"

//#include "XISRender.h"

#include "SDRDraw.h"

#include "../VIDECYNC/VIDECYNCAPI.h"

#define AH2K_DEV_BEGIN		100

// CCaptureWnd 对话框

class CCaptureWnd
	: public CDialogEx
	, public HPAVC_D3DRenderCallback
	, public  XThreadBase	
	, public VideoParamNotify
	, public COpAH2KCallback
	, public COpVidecCameraCallback
	, public VIDEC_WndCallback
{
	DECLARE_DYNAMIC(CCaptureWnd)

public:
	CCaptureWnd(int nShowType,bool bEncode,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCaptureWnd();

// 对话框数据
	enum { IDD = IDD_DIALOG_CAPTURE };

private:
	HPAVC_D3DRender*		m_pD3DRender;

//	XISRender*				m_pXISRender;
	SDRDraw*				m_pDraw;
	void*					m_pVidecD3D;

	VIDEC_Wnd*				m_pWnd;

	EncodeFrame*			m_pEncode;

	COpAH2K*				m_pAH2K;
	COpVidecCamera*			m_pCamera;

	bool					m_bEncode;
	bool					m_bWaitStop;

	std::string				m_strFileName;
	int						m_nFrameRate;
	int						m_nBitrate;

	Video_Param				m_param;
	bool					m_bShowVideo;
	bool					m_bShowMax;

	unsigned long			m_nSkipFrameCount;
	unsigned long			m_nLastTS;
	unsigned long			m_nFrameCount;

	XListPtr			m_listImage;
	XCritSec			m_csListImage;

	int					m_nWidth;
	int					m_nHeight;

	FILE*				m_pFile;

	VIDEO_SHOW_TYPE		m_nShowType;

public:
	void OnCallbackDrawD3D9DC(void* hDC);
	virtual void ThreadProcMain(void);

	bool CreateRender(void);
	void GetAVParameters(unsigned long& ulAudioID,unsigned long& ulVideoID);

	bool Start(std::string strFileName,int nFrameRate,int nBitrate);
	void Stop(void);
	void SetVideoShow(bool bShowVideo);
	void StartPGSDK(void);
	bool StartCapture(int nDevID,int nWidth,int nHeight,int nFrameRate,int nBitrate);
	void Ah2kThreadProc();
public:
	virtual void OnOpAH2KCallbackImage(VIDEC_Image*pImage);
	virtual void OnOpAH2KCallbackAudioData(short*pSamples,int nSamples,int nSampleRate);

	virtual void OnOpVidecCameraaCallbackImageData(VIDEC_Image*pImage);    

	virtual void VideoParamNotifyNewParam(Video_Param param);

	void OnPG_SDKCallBackResult(PG_SDK_FrameResultC const & FrameResult);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
