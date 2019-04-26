// TestDecoderDlg.h : 头文件
//

#pragma once
#include "../../include/VIDEC/VIDEC.h"
#include "../../include/VIDEC/VIDEC_Header.h"

#ifdef TEST_MJPG
#include "../../include/VIDEC/VIDEC_MJPGEncoder.h"
#include "../../include/VIDEC/VIDEC_MJPGDecoder.h"
#endif

#ifdef TEST_AH2K
#include "../../include/VIDEC/VIDEC_AH2KDev.h"
#endif
#ifdef TEST_CAMERA
#include "../../include/VIDEC/VIDEC_CameraDev.h"
#endif
#ifdef TEST_AH400
#include "../../include/VIDEC/VIDEC_AH400Dev.h"
#endif
#ifdef TEST_SCREEN
#include "../../include/VIDEC/VIDEC_Screen.h"
#endif
#ifdef TEST_DC
#include "../../include/VIDEC/VIDEC_DC.h"
#include "../../include/VIDEC/VIDEC_DCRecord.h"
#include "MemDC.h"
#endif
#ifdef TEST_MEDIAFILE
#include "../../include/VIDEC/VIDEC_MediaFile.h"
#endif
#ifdef TEST_IPCAMERA
#include "../../include/VIDEC/VIDEC_IPCamera.h"
#endif

#include "XBitrate.h"


#include "DlgFullScreen.h"
#include "DlgCapVideo.h"
#include "DlgDecoder.h"
#include "DlgBar.h"


// DlgMain 对话框
class DlgMain 
	: public CDialog
	, public VIDEC_StreamSink
	, public VIDEC_CapChanCallback
	, public VIDEC_ImageSink

#ifdef TEST_MEDIAFILE
	, public VIDEC_MediaFileCallback
#endif
#ifdef TEST_IPCAMERA
	, public VIDEC_IPCameraCallback
#endif

#ifdef TEST_MJPG
	, public VIDEC_MJPGEncoderCallback
	, public VIDEC_MJPGDecoderCallback
#endif
{
// 构造
public:
	DlgMain(CWnd* pParent = NULL);	// 标准构造函数


	void ResizeControls(void);
	void SetFullScreen(void);
	void DoStart(void);
	void DoStop(void);
	void DoSetting(void);

	DlgFullScreen*		m_pDlgFullScreen;
	DlgCapVideo*		m_pDlgCapVideo;
	DlgDecoder*			m_pDlgDecoder;
	DlgBar*				m_pDlgBar;

#ifdef TEST_MJPG
	VIDEC_MJPGEncoder*	m_pMJPGEncoder;
	VIDEC_MJPGDecoder*	m_pMJPGDecoder;

	virtual void OnVIDEC_MJPGEncoderCallbackStreamPacket(VIDEC_MJPGEncoder*pMJPGEncoder,unsigned char*pPacketData,int nPacketLen);
	virtual void OnVIDEC_MJPGEncoderCallbackPreEncode(VIDEC_MJPGEncoder*pMJPGEncoder,int&nSkip);
	virtual void OnVIDEC_MJPGDecoderCallbackImage(VIDEC_MJPGDecoder*pMJPGEncoder,VIDEC_Image*pImage);
#endif

#ifdef TEST_AH2K
	VIDEC_AH2KDev*		m_pDev;
#endif
#ifdef TEST_CAMERA
	VIDEC_CameraDev*	m_pDev;
#endif
#ifdef TEST_AH400
	VIDEC_AH400Dev*		m_pDev;
#endif
#ifdef TEST_SCREEN
	VIDEC_Screen*		m_pDev;
#endif
#ifdef TEST_DC
	VIDEC_DC*			m_pDev;
	CMemDC*				m_pMemDC;
	CDC*				m_pDC;
	VIDEC_DCRecord*		m_pDCRecord;
#endif
#ifdef TEST_MEDIAFILE
	VIDEC_MediaFile*	m_pDev;
	void OnVIDEC_MediaFileCallbackPositionChanged(VIDEC_MediaFile*pMediaFile,unsigned long ulSeconds){};
	void OnVIDEC_MediaFileCallbackEnd(VIDEC_MediaFile*pMediaFile){};
	void OnVIDEC_MediaFileCallbackPCMData(VIDEC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,unsigned long ulTimestamp);
#endif

#ifdef TEST_IPCAMERA
	VIDEC_IPCamera*		m_pIPCamera;
	void OnVIDEC_IPCameraCallbackVideoFramePacketMain(unsigned char*pPacketData,int nPacketLen);
	void OnVIDEC_IPCameraCallbackVideoFramePacketSub(unsigned char*pPacketData,int nPacketLen);
	void OnVIDEC_IPCameraCallbackVideoFramePacketQSub(unsigned char*pPacketData,int nPacketLen);
	void OnVIDEC_IPCameraCallbackVideoRTPPacketMain(unsigned char*pPacketData,int nPacketLen);
	void OnVIDEC_IPCameraCallbackVideoRTPPacketSub(unsigned char*pPacketData,int nPacketLen);
	void OnVIDEC_IPCameraCallbackVideoRTPPacketQSub(unsigned char*pPacketData,int nPacketLen);
#endif

	VIDEC_CapChan*		m_pCapChan;
	unsigned long		m_nLastSkipTimestamp;

	int					m_nWidth;
	int					m_nHeight;
	int					m_nDevID;
	int					m_nFormat;
	int					m_nFrameRate;
	int					m_nQuality;
	VIDEC_CODEC_TYPE	m_CodecType;
	BOOL				m_bEncode;
	BOOL				m_bDecode;
	DWORD				m_nDecoderCount;

	XBitrate			m_XBitrateVideo;

	void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource);
	void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource);
	void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);


	void OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag);

	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource){};
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource){};
	virtual void OnReceivedImage(VIDEC_Image*pImage);
	virtual void OnReceivedPreSend(int& nSend);


// 对话框数据
	enum { IDD = IDD_DIALOG_MAIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	afx_msg void OnTimer(UINT nIDEvent);
};
