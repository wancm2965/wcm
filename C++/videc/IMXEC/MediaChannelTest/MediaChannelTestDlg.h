// MediaChannelTestDlg.h : 头文件
//

#pragma once

#include <AUDEC/AUDEC_Engine.h>
#include <VIDEC/VIDEC.h>
#include <IMXEC/IMXEC_MediaChannel.h>
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Node.h>
#include <NETEC/NETEC_Setting.h>
#include "afxwin.h"
#include "DlgVideo.h"
#include <string>

//#define  TEST_BK_MUSIC


// CMediaChannelTestDlg 对话框
class CMediaChannelTestDlg 
	: public CDialog
	, public IMXEC_CapChanCallback
	, public IMXEC_PlayChanCallback
	, public NETEC_Node
#ifdef TEST_BK_MUSIC
	, public IMXEC_BackgroundMusicCallback
#endif
{
// 构造
public:
	CMediaChannelTestDlg(CWnd* pParent = NULL);	// 标准构造函数
public:
	DlgVideo*			m_pDlgVideoLocal;
	DlgVideo*			m_pDlgVideoRemote;

	X_AUDIO_CODEC_ID	m_AudioCodecID;

	int					m_nDevID;
	VIDEC_CODEC_TYPE	m_VideoCodecType;
	int					m_nFrameRate;
	int					m_nBitrate;
	int					m_nWidth;
	int					m_nHeight;

	IMXEC_CapChanDev*	m_pCapChanDev;
	IMXEC_PlayChan*		m_pPlayChan;

	IMXEC_AudioMixerCapChan*	m_pAudioMixerCapChan;

#ifdef TEST_BK_MUSIC
	IMXEC_BackgroundMusic*	m_pBackgroundMusic;
#endif

public:
	//音频数据回调函数
	virtual void OnIMXEC_CapChanCallbackAudioData(unsigned char*pData,int nLen,bool&bLoop);
	virtual void OnIMXEC_CapChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_CapChanCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame);

	virtual void OnIMXEC_CapChanCallbackMotionDetected(int*pResultList,int nResultCount);
	virtual void OnIMXEC_CapChanCallbackAudioIDChanged(unsigned long nAudioID);

	//音频数据回调函数
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop);
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode);
	virtual void OnIMXEC_PlayChanCallbackRemoteSnapJPG(const char*cszPathName);

	/******************************************************************************
	* OnNETEC_NodeConnectStatusChanged
	*描述：网络连接状态的回调
	*输入：	cs					-网络连接状态
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

#ifdef TEST_BK_MUSIC
	virtual void OnIMXEC_BackgroundMusicCallbackPositionChanged(unsigned long ulSeconds){};
	virtual void OnIMXEC_BackgroundMusicCallbackEnd(void){};
#endif

public:
// 对话框数据
	enum { IDD = IDD_DIALOG_MEDIACHANNELTEST };

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
	CStatic m_btnStatus;
	CButton m_btnStart;
	CButton m_btnStop;
	CButton m_btnSubVideo;

	afx_msg void OnBnClickedButtonAudioSetting();
	afx_msg void OnBnClickedButtonVideoSetting();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedCheckSubVideo();
	CButton m_btnQSub;
	afx_msg void OnBnClickedCheckQsubVideo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedMedia();
};
