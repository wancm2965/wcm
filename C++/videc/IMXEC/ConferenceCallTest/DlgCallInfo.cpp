// DlgCallInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "ConferenceCallTest.h"
#include "DlgCallInfo.h"
#include <IMXEC/IMXEC_Call.h>


// DlgCallInfo 对话框

IMPLEMENT_DYNAMIC(DlgCallInfo, CDialog)

DlgCallInfo::DlgCallInfo(CWnd* pParent /*=NULL*/)
	: CDialog(DlgCallInfo::IDD, pParent)
	, m_strCallID("")
	, m_nRefreshTimerID(0)
{

}

DlgCallInfo::~DlgCallInfo()
{
}

void DlgCallInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LOCAL_VIDEO_INFO, m_btnLocalVideoInfo);
	DDX_Control(pDX, IDC_REMOTE_LOCAL_VIDEO_INFO, m_btnRemoteVideoInfo);
	DDX_Control(pDX, IDC_SECOND_VIDEO_INFO, m_btnSecondVideoInfo);
}


BEGIN_MESSAGE_MAP(DlgCallInfo, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DlgCallInfo 消息处理程序

void DlgCallInfo::OnTimer(UINT_PTR nIDEvent)
{
	if (m_nRefreshTimerID==nIDEvent)
	{
		ShowInfo();
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL DlgCallInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	ShowInfo();

	m_nRefreshTimerID=2000;
	SetTimer(m_nRefreshTimerID,2000,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL DlgCallInfo::DestroyWindow()
{
	if (m_nRefreshTimerID!=0)
	{
		KillTimer(m_nRefreshTimerID);
		m_nRefreshTimerID=NULL;
	}

	return CDialog::DestroyWindow();
}

void DlgCallInfo::ShowInfo(void)
{
	{
		int nWidth=0;
		int nHeight=0;
		VIDEC_CODEC_TYPE nVideoCodecType=VIDEC_CODEC_H264;
		HWND hWnd=NULL;
		CallChannelInfo chinfo;
		chinfo.m_nFrame =0;
		IMXEC_Call::GetLocalMainVideoInfo(m_strCallID,nWidth,nHeight,nVideoCodecType,chinfo,hWnd);

		CString strCodecName="H264";
		switch (nVideoCodecType)
		{
		case VIDEC_CODEC_H261:
			strCodecName="H261";
			break;
		case VIDEC_CODEC_H263:
			strCodecName="H263";
			break;
		case VIDEC_CODEC_H263P:
			strCodecName="H263+";
			break;
		case VIDEC_CODEC_H263PP:
			strCodecName="H263++";
			break;
		case VIDEC_CODEC_H264:
			strCodecName="H264";
			break;
		};

		CString strText="";
		strText.Format("视频格式:\t%d X %d\r\n视频编码:\t%s",nWidth,nHeight,strCodecName);
		m_btnLocalVideoInfo.SetWindowText(strText);
	}
	{
		int nWidth=0;
		int nHeight=0;
		VIDEC_CODEC_TYPE nVideoCodecType=VIDEC_CODEC_H264;
		HWND hWnd=NULL;
		CallChannelInfo chinfo;
		chinfo.m_nFrame =0;
		IMXEC_Call::GetRemoteMainVideoInfo(m_strCallID,nWidth,nHeight,nVideoCodecType,chinfo,hWnd);


		CString strCodecName="H264";
		switch (nVideoCodecType)
		{
		case VIDEC_CODEC_H261:
			strCodecName="H261";
			break;
		case VIDEC_CODEC_H263:
			strCodecName="H263";
			break;
		case VIDEC_CODEC_H263P:
			strCodecName="H263+";
			break;
		case VIDEC_CODEC_H263PP:
			strCodecName="H263++";
			break;
		case VIDEC_CODEC_H264:
			strCodecName="H264";
			break;
		};

		CString strText="";
		strText.Format("视频格式:\t%d X %d\r\n视频编码:\t%s",nWidth,nHeight,strCodecName);
		m_btnRemoteVideoInfo.SetWindowText(strText);
	}
	{
		int nWidth=0;
		int nHeight=0;
		VIDEC_CODEC_TYPE nVideoCodecType=VIDEC_CODEC_H264;
		HWND hWnd=NULL;
		CallChannelInfo chinfo;
		chinfo.m_nFrame =0;
		IMXEC_Call::GetSecondVideoInfo(m_strCallID,nWidth,nHeight,nVideoCodecType,chinfo,hWnd);

		CString strCodecName="H264";
		switch (nVideoCodecType)
		{
		case VIDEC_CODEC_H261:
			strCodecName="H261";
			break;
		case VIDEC_CODEC_H263:
			strCodecName="H263";
			break;
		case VIDEC_CODEC_H263P:
			strCodecName="H263+";
			break;
		case VIDEC_CODEC_H263PP:
			strCodecName="H263++";
			break;
		case VIDEC_CODEC_H264:
			strCodecName="H264";
			break;
		};

		CString strText="";
		strText.Format("视频格式:\t%d X %d\r\n视频编码:\t%s",nWidth,nHeight,strCodecName);
		m_btnSecondVideoInfo.SetWindowText(strText);
	}
}
