// TestAH2KDlg.h : 头文件
//

#pragma once
#include "../../include/VIDEC/VIDEC.h"
#include "../../include/VIDEC/VIDEC_Header.h"
//#include "../../include/VIDEC/VIDEC_Player.h"
#ifdef TEST_AH2K
#include "../../include/VIDEC/VIDEC_AH2KDev.h"
#endif
#ifdef TEST_DVC
#include "../../include/VIDEC/VIDEC_DVCDev.h"
#endif
#ifdef TEST_CAMERA
#include "../../include/VIDEC/VIDEC_CameraDev.h"
#endif
#ifdef TEST_AH410
#include "../../include/VIDEC/VIDEC_AH410Dev.h"
#endif
#ifdef TEST_AH210
#include "../../include/VIDEC/VIDEC_AH210Dev.h"
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

#ifdef TEST_RAW_FILE
#include "Reader.h"
#endif

#include "XBitrate.h"

//#define _TEST_DECODER
#ifdef _TEST_DECODER
#include "../../include/VIDEC/VIDEC_StreamDecoder.h"
#endif

#define H264				mmioFOURCC('H','2','6','4')
#define H263				mmioFOURCC('H','2','6','3')
#define FOURCC_VIDEC		mmioFOURCC('V','D','E','C')

#include "DlgFullScreen.h"
#include "DlgCapVideo.h"
#include "DlgPlayVideo.h"
#include "DlgBar.h"

#define _AVIFILE
#ifdef _AVIFILE
#include "../../include/VIDEC/VIDEC_AVIWriter.h"
#else
#include "AVCONEC_ASFWriter.h"
#endif

// DlgMain 对话框
class DlgMain 
	: public CDialog
	, public VIDEC_StreamSink
	, public VIDEC_CapChanCallback
	, public VIDEC_PlayerCallback

#ifdef TEST_MEDIAFILE
	, public VIDEC_MediaFileCallback
#endif
#ifdef TEST_RAW_FILE
	, public ReaderCallback
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
	DlgPlayVideo*		m_pDlgPlayVideo;
	DlgBar*				m_pDlgBar;
	CDialog*			m_pDlgVideo;

#ifdef TEST_AH2K
	VIDEC_AH2KDev*		m_pDev;
#endif
#ifdef TEST_DVC
	VIDEC_DVCDev*		m_pDev;
#endif
#ifdef TEST_CAMERA
	VIDEC_CameraDev*	m_pDev;
#endif
#ifdef TEST_AH410
	VIDEC_AH410Dev*		m_pDev;
#endif
#ifdef TEST_AH210
	VIDEC_AH210Dev*		m_pDev;
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

#ifdef TEST_RAW_FILE
	Reader*	m_pReader;
	void OnReaderCallbackFrameData(const unsigned char*pFrameData,int nFrameLen);
#endif

	VIDEC_CapChan*		m_pCapChan;
#ifdef _TEST_DECODER
	VIDEC_StreamDecoder*m_pDecoder;
#else
#ifdef TEST_PLAYER
#ifdef TEST_DXVA
	AVCONAVC_Player*		m_pPlayer;
#else
	VIDEC_Player*		m_pPlayer;
#endif
#else
	VIDEC_PlayChan*		m_pPlayChan;
#endif
#endif

	int					m_nWidth;
	int					m_nHeight;
	int					m_nDevID;
	int					m_nFormat;
	int					m_nFrameRate;
	int					m_nQuality;
	VIDEC_CODEC_TYPE	m_CodecType;
	BOOL				m_bEncode;
	BOOL				m_bDecode;

#ifdef _AVIFILE
	VIDEC_AVIWriter*	m_pAVIWriter;
#else
	AVCONEC_ASFWriter*	m_pASFWriter;
#endif

	XBitrate			m_XBitrateVideo;


	void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource);
	void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource);
	void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);


	void OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag){nSkipFlag=0;};
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
