#include <stdlib.h>
#include <memory.h>
#include "XEncoderStream.h"
#include "../MPVIDEO_API/XImage.h"
#include "XUtil.h"


MPVIDEO_API MPVideo_EncoderStream*MPVideo_EncoderStreamCreate(MPVideo_EncoderStreamCallback&rCallback,bool bRTPPacket)
{
	XEncoderStream*pXEncoderStream=new XEncoderStream(rCallback,bRTPPacket);
	if (0!=pXEncoderStream->Open())
	{
		pXEncoderStream->Close();
		delete pXEncoderStream;
		pXEncoderStream=NULL;
	}

	return pXEncoderStream;
}


XEncoderStream::XEncoderStream(MPVideo_EncoderStreamCallback&rCallback,bool bRTPPacket)
:m_rCallback(rCallback)
,m_bRTPPacket(bRTPPacket)

,m_nWidth(352)
,m_nHeight(288)
,m_codecType(VIDEC_CODEC_COUNT)

,m_nVirtualWidth(0)
,m_nVirtualHeight(0)
,m_bDoubleField(false)

,m_nBitrate(512)
,m_nMinQP(20)
,m_nMaxQP(30)
,m_nBitrateControlType(MPVIDEO_NETWORK_INTERNET)

,m_bRequestKeyFrame(true)
,m_nNetworkBitrate(0)

,m_pEncoderFrame(NULL)

,m_bRunning(false)

,m_nFrameRate(30)
{
#ifdef _DEMO
	m_ulFirstTimestamp=XGetTimestamp();
#endif
}

XEncoderStream::~XEncoderStream(void)
{

}

int XEncoderStream::Open(void)
{
	m_bRunning=true;
	if (XThreadBase::StartThread()==false)
	{
		m_bRunning=false;
		return -1;
	}

	return 0;
}

void XEncoderStream::Close(void)
{
	m_bRunning=false;
	XThreadBase::WaitForStop();

	if (m_pEncoderFrame!=NULL)
	{
		m_pEncoderFrame->Close();
		delete m_pEncoderFrame;
		m_pEncoderFrame=NULL;
	}

	{
		XAutoLock l(m_csListFrames);
		while (m_ListFrames.size())
		{
			PFRAME pFrame=(PFRAME)m_ListFrames.front();
			m_ListFrames.pop_front();

			delete pFrame;
			pFrame=NULL;
		}
	}
}

int XEncoderStream::EncodeFrame(MPVideo_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType)
{
#ifdef _DEMO
	unsigned long ulTimestamp=XGetTimestamp();
	if (ulTimestamp<m_ulFirstTimestamp)
	{
		m_ulFirstTimestamp=ulTimestamp;
	}
	else if (ulTimestamp-m_ulFirstTimestamp>60000*20)
	{
		return -1;
	}
#endif

	//最多缓冲一秒的数据
	{
		XAutoLock l(m_csListFrames);
		while (3<m_ListFrames.size())
		{
			PFRAME pFrame=(PFRAME)m_ListFrames.front();
			m_ListFrames.pop_front();

			delete pFrame;
			pFrame=NULL;

			//static int nLostCount=0;
			//char szDebug[1024]="";
			//sprintf(szDebug,"nLostCount=%d\n",++nLostCount);
			//OutputDebugString(szDebug);
		}
	}


	PFRAME pFrame=new FRAME(pImage,nBitrate,nMinQP,nMaxQP,nBitrateControlType,codecType);
	if (pFrame==NULL)
	{
		return -1;
	}

	{
		XAutoLock l(m_csListFrames);
		m_ListFrames.push_back(pFrame);
	}

	return 0;
}

int XEncoderStream::DoEncodeFrame(MPVideo_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return -1;
	}

	bool bEncoderChanged=false;

	if (codecType!=m_codecType)
	{
		m_codecType=codecType;
		bEncoderChanged=true;
	}

	if (m_nBitrate!=nBitrate)
	{
		m_nBitrate=nBitrate;
		bEncoderChanged=true;
	}

	if (nMinQP!=m_nMinQP)
	{
		m_nMinQP=nMinQP;
		bEncoderChanged=true;
	}
	
	if (m_nMaxQP!=nMaxQP)
	{
		m_nMaxQP=nMaxQP;
		bEncoderChanged=true;
	}
	
	if (m_nBitrateControlType!=nBitrateControlType)
	{
		m_nBitrateControlType=nBitrateControlType;
		bEncoderChanged=true;
	}

	if (pXImage->GetFrameRate()!=m_nFrameRate)
	{
		m_nFrameRate=pXImage->GetFrameRate();
		if (codecType==VIDEC_CODEC_H264 || codecType==VIDEC_CODEC_H264_SVC)
		{
			bEncoderChanged=true;
		}
	}

	if (pXImage->GetActualWidth()!=m_nWidth)
	{
		m_nWidth=pXImage->GetActualWidth();
		bEncoderChanged=true;
	}

	if (pXImage->GetActualHeight()!=m_nHeight)
	{
		m_nHeight=pXImage->GetActualHeight();
		bEncoderChanged=true;
	}

	if (m_nVirtualWidth!=pXImage->GetVirtualWidth())
	{
		m_nVirtualWidth=pXImage->GetVirtualWidth();
	}
	if (m_nVirtualHeight!=pXImage->GetVirtualHeight())
	{
		m_nVirtualHeight=pXImage->GetVirtualHeight();
	}

	if (m_bDoubleField!=pXImage->GetDoubleField())
	{
		m_bDoubleField=pXImage->GetDoubleField();
		bEncoderChanged=true;
	}

	if (bEncoderChanged ||
		m_pEncoderFrame==NULL)
	{
		if (m_pEncoderFrame!=NULL)
		{
			m_pEncoderFrame->Close();
			delete m_pEncoderFrame;
			m_pEncoderFrame=NULL;
		}

		m_pEncoderFrame=MPVideo_EncoderCreate(*this,m_nWidth,m_nHeight,m_nFrameRate,m_codecType,m_bDoubleField,m_nBitrate,m_nMinQP,m_nMaxQP,m_nBitrateControlType,m_bRTPPacket);
	}

	if (m_pEncoderFrame!=NULL)
	{
		if (m_bRequestKeyFrame)
		{
			m_bRequestKeyFrame=false;
			m_pEncoderFrame->RequestKeyFrame();
		}


		return m_pEncoderFrame->EncodeFrame(pXImage->GetData(),pXImage->GetDataLen(),m_nVirtualWidth,m_nVirtualHeight,pXImage->GetEvenFieldFirst(),pXImage->GetHardware(),pXImage->GetTimestamp());
	}

	return -1;
}

void XEncoderStream::RequestKeyFrame(void)
{
	m_bRequestKeyFrame=true;
}

void XEncoderStream::OnMPVideo_EncoderCallbackFramePacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnMPVideo_EncoderStreamCallbackFramePacket(pPacketData,nPacketLen,m_nVirtualWidth,m_nVirtualHeight);
}

void XEncoderStream::OnMPVideo_EncoderCallbackRTPPacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnMPVideo_EncoderStreamCallbackRTPPacket(pPacketData,nPacketLen,m_nVirtualWidth,m_nVirtualHeight);
}

void XEncoderStream::ThreadProcMain(void)
{
	PFRAME pFrame=NULL;
	bool bRet=false;
	while (m_bRunning)
	{
		{
			XAutoLock l(m_csListFrames);
			if (m_ListFrames.size())
			{
				pFrame=(PFRAME)m_ListFrames.front();
				m_ListFrames.pop_front();
			}
		}

		if (m_pEncoderFrame!=NULL)
		{
			bRet=m_pEncoderFrame->DoRTPPacket();
		}
		else
		{
			bRet=false;
		}

		if (pFrame!=NULL)
		{
			int nSkipFlag=0;
			m_rCallback.OnMPVideo_EncoderStreamCallbackPreEncode(nSkipFlag);
			if (!nSkipFlag)
			{
				DoEncodeFrame(pFrame->m_pImage,pFrame->m_nBitrate,pFrame->m_nMinQP,pFrame->m_nMaxQP,pFrame->m_nBitrateControlType,pFrame->m_codecType);
			}

			delete pFrame;
			pFrame=NULL;
		}
		else if (!bRet)
		{
			XSleep(1);
		}
	}
}