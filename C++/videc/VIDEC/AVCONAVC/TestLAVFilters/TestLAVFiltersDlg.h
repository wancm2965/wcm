
// TestLAVFiltersDlg.h : ͷ�ļ�
//

#pragma once

#include <AVCONAVC/AVCONAVC_MediaFile.h>
//#include <AVCONAVC/avpullplayer.h>
#include "DlgVideo.h"

// CTestLAVFiltersDlg �Ի���
class CTestLAVFiltersDlg 
	: public CDialog
	, public AVCONAVC_MediaFileCallback
{
// ����
public:
	CTestLAVFiltersDlg(CWnd* pParent = NULL);	// ��׼���캯��

	AVCONAVC_MediaFile*	m_pMediaFile;

	DlgVideo*			m_pDlgVideo;
	bool				m_bTryEVR;

#ifdef __AVPULLPLAYER_H__
	IAVPullPlayer*		m_pIAVPullPlayer;
#endif

	virtual void OnAVCONAVC_MediaFileCallbackPCMData(AVCONAVC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp);
	virtual void OnAVCONAVC_MediaFileCallbackYV12Buffer(unsigned char**ppBuffer,int nBufferLen);
	virtual void OnAVCONAVC_MediaFileCallbackYV12Data(AVCONAVC_MediaFile*pMediaFile,unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp);

// �Ի�������
	enum { IDD = IDD_TESTLAVFILTERS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedButton1();
};
