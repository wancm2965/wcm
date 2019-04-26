//CapChan.cpp
#include "../IMXEC_win32/stdafx.h"

#include "CapChan.h"
#include <VIDEC/VIDEC_Header.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>
#include "XUtil.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CapChan::CapChan(IMXEC_CapChanCallback&rCallback)
:m_rCallback(rCallback)
,m_pMediaSender(NULL)
,m_hWndParent(NULL)
,m_pWnd(NULL)
,m_nOnlyPreview(0)
{
	m_nTotalBytes=0;

	m_ulLastCountFrameTimestamp=0;
	m_nSendFrameRateCount=0;
	m_nSendFrameRate=0;
	m_nVideoWnBkR = -1;
	m_nVideoWnBkG = -1;
	m_nVideoWnBkB = -1;
}

CapChan::~CapChan(void)
{

}


//打开通道
int CapChan::Open(HWND hwndParent,int nOnlyPreview)
{
	m_nOnlyPreview=nOnlyPreview;
	m_hWndParent=hwndParent;
	if (m_hWndParent!=NULL && m_pWnd==NULL)
	{
		m_pWnd=VIDEC_Wnd::CreateLocalVideo(*this);
		if (m_pWnd==NULL)
		{
			return -1;
		}

		if (0!=m_pWnd->Open(hwndParent,true, NULL, NULL, (char*)(m_strVideoWndBkImage[0]?m_strVideoWndBkImage.c_str():NULL), m_nVideoWnBkR, m_nVideoWnBkG, m_nVideoWnBkB))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;

			return -1;
		}
	}

	if (m_nOnlyPreview==0)
	{
		if (m_pMediaSender==NULL)
		{
			m_pMediaSender=NETEC_MediaSender::Create(*this);
			if (m_pMediaSender==NULL)
			{
				return -1;
			}

			if (0!=m_pMediaSender->Open())
			{
				m_pMediaSender->Close();
				delete m_pMediaSender;
				m_pMediaSender=NULL;

				return -1;
			}
		}
	}

	return 0;
}

//关闭通道
void CapChan::Close(void)
{
	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->Close();
		delete m_pMediaSender;
		m_pMediaSender=NULL;
	}


	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}
}

void CapChan::EnablePreview(int nPreview)
{
	if (m_pWnd!=NULL)
	{
		m_pWnd->EnablePreview(nPreview);
	}
}

//取视频窗口
HWND CapChan::GetVideoWnd(void)
{
	if (m_pWnd==NULL)
	{
		return NULL;
	}

	return m_pWnd->GetWnd();
}
//取视频窗口
int CapChan::GetVideoShowFrameRate(void)
{
	if (m_pWnd==NULL)
	{
		return 0;
	}

	return m_pWnd->GetVideoShowFrameRate();
}

int CapChan::GetVideoCaptureFrameRate(void)
{

	return 0;
}

int CapChan::GetVideoEncodeFrameRate(void)
{

	return 0;
}

int CapChan::GetVideoSendFrameRate(void)
{
	int nRate = m_nSendFrameRate;
	m_nSendFrameRate=0;
	return nRate;
}

int CapChan::GetVideoShowType(void)
{
	if (m_pWnd==NULL)
	{
		return 0;
	}

	return m_pWnd->GetVideoShowType();
}

//取视频宽度
int CapChan::GetVideoWidth(void)
{
	if (m_pWnd==NULL)
	{
		return 320;
	}

	return m_pWnd->GetVideoWidth();
}

//取视频高度
int CapChan::GetVideoHeight(void)
{
	if (m_pWnd==NULL)
	{
		return 240;
	}

	return m_pWnd->GetVideoHeight();
}

//same as CWnd::IsWindowVisible
bool CapChan::IsWindowVisible(void)
{
	if (m_pWnd==NULL)
	{
		return false;
	}

	return m_pWnd->IsWindowVisible();
}

//same as CWnd::SetWindowPos
bool CapChan::SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)
{
	if (m_pWnd!=NULL)
	{
		return m_pWnd->SetWindowPos(hWndInsertAfter,x,y,cx,cy,nFlags);
	}
	return false;
}

//保持当前图象到BMP文件
bool CapChan::CaptureToBMPFile(const char*cszFile)
{
	if (m_pWnd!=NULL)
	{
		return m_pWnd->CaptureToBMPFile(cszFile);
	}
	return false;
}




int CapChan::SetVolume(int nVolume)
{
	return 0;
}

int CapChan::GetLevel(int&nLevel)
{
	return 0;
}

unsigned long CapChan::GetAudioID(void)
{
	unsigned long nAudioID=0;
	if (m_pMediaSender!=NULL)
	{
		if (0!=m_pMediaSender->GetAudioID((uint32_t &)nAudioID))
		{
			nAudioID=0;
		}
	}
	return nAudioID;
}

unsigned long CapChan::GetVideoID(void)
{
	unsigned long nVideoID=0;
	if (m_pMediaSender!=NULL)
	{
		if (0!=m_pMediaSender->GetVideoID((uint32_t &)nVideoID))
		{
			nVideoID=0;
		}
	}
	return nVideoID;
}



int CapChan::SendAudio(unsigned char*pData,int nLen)
{
	int nTempLen=nLen;
	if (AUDEC_HEADER_GET_USED_FEC(pData))
	{
		nTempLen=nTempLen>>1;
	}

	m_nTotalBytes+=nTempLen;

	m_XBitrateAudio.Calculate(nTempLen);

	if (m_pMediaSender!=NULL)
	{
		return m_pMediaSender->SendAudio(pData,nLen);
	}
	return -1;
}


void CapChan::ReSetFrameRate(void)
{
	unsigned long ulCurTimestamp = XGetTimestamp();
	if (m_ulLastCountFrameTimestamp == 0)
	{
		m_ulLastCountFrameTimestamp = ulCurTimestamp;
	}
	unsigned long ulTemp = ulCurTimestamp - m_ulLastCountFrameTimestamp;
	if (ulTemp < 990)
	{
		return;
	}

	if (ulTemp > 1500)
	{
		float f = 1000.00/ulTemp;
		m_nSendFrameRate=m_nSendFrameRateCount*f;
	}
	else
	{
		m_nSendFrameRate=m_nSendFrameRateCount;
	}
	m_ulLastCountFrameTimestamp = ulCurTimestamp;
	m_nSendFrameRateCount=0;
}

int CapChan::SendMainVideo(unsigned char*pData,int nLen)
{
	m_nTotalBytes+=nLen;

	m_XBitrateVideo.Calculate(nLen);

	m_rCallback.OnIMXEC_CapChanCallbackVideoData(pData,nLen,VIDEC_HEADER_EXT_GET_KEYFRAME(pData));

	if (m_pMediaSender!=NULL)
	{
		m_nSendFrameRateCount++;
		ReSetFrameRate();

		return m_pMediaSender->SendVideo(pData,nLen);
	}
	return -1;
}

int CapChan::SendSubVideo(unsigned char*pData,int nLen)
{
	m_nTotalBytes+=nLen;

	m_XBitrateVideo.Calculate(nLen);

	if (m_pMediaSender!=NULL)
	{
		return m_pMediaSender->SendVideo(pData,nLen);
	}
	return -1;
}

int CapChan::SendQSubVideo(unsigned char*pData,int nLen)
{
	m_nTotalBytes+=nLen;

	m_XBitrateVideo.Calculate(nLen);

	if (m_pMediaSender!=NULL)
	{
		return m_pMediaSender->SendVideo(pData,nLen);
	}
	return -1;
}


/******************************************************************************
* GetVideoPacketStatistics
*描述：获取视频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CapChan::GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	nBitrate=m_XBitrateVideo.GetBitrate();

	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->GetVideoPacketStatistics((uint32_t &)ulTotalPackets,(uint32_t &)ulLostPackets,AvgPacketLossRate,CurPacketLossRate);
	}
	else
	{
		ulTotalPackets=0;
		ulLostPackets=0;
		AvgPacketLossRate=0.00;
		CurPacketLossRate=0.00;
	}
	
	return 0;
}


/******************************************************************************
* GetAudioPacketStatistics
*描述：获取音频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CapChan::GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	nBitrate=m_XBitrateAudio.GetBitrate();

	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->GetAudioPacketStatistics((uint32_t &)ulTotalPackets,(uint32_t &)ulLostPackets,AvgPacketLossRate,CurPacketLossRate);
	}
	else
	{
		ulTotalPackets=0;
		ulLostPackets=0;
		AvgPacketLossRate=0.00;
		CurPacketLossRate=0.00;
	}
	
	return 0;
}

void CapChan::OnNETEC_MediaSenderCallbackRecvdCtrl(void*pData,int nLen)
{
	m_rCallback.OnIMXEC_CapChanCallbackRecvdCtrlData(pData,nLen);
}

unsigned long CapChan::DoGetTotalBytes(void)
{
	unsigned long nTotalBytes=m_nTotalBytes;
	m_nTotalBytes=0;
	return nTotalBytes;
}

void CapChan::OnVIDEC_WndCallbackDrawD3D9DC(void* hDC/*HDC*/)
{
	m_rCallback.OnIMXEC_CapChanCallbackDrawD3D9DC(hDC);
}
