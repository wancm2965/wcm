#include <stdlib.h>
#include <memory.h>
#include "XEncoderStream.h"
#include "../VIDEC_API/XImage.h"
#include "XUtil.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"


VIDEC_API VIDEC_EncoderStream*VIDEC_EncoderStreamCreate(VIDEC_EncoderStreamCallback&rCallback,bool bRTPPacket)
{
	NEWRECORD3( XEncoderStream*,pXEncoderStream ,new XEncoderStream(rCallback,bRTPPacket) )
	NULLReturnRet( pXEncoderStream , pXEncoderStream )  
	if (0!=pXEncoderStream->Open())
	{
		pXEncoderStream->Close();
		delete pXEncoderStream;
		pXEncoderStream=NULL;
	}

	return pXEncoderStream;
}


XEncoderStream::XEncoderStream(VIDEC_EncoderStreamCallback&rCallback,bool bRTPPacket)
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
,m_nBitrateControlType(VIDEC_NETWORK_INTERNET)

,m_bRequestKeyFrame(true)
,m_nNetworkBitrate(0)
,m_nEncodeErrorCount(0)
,m_pEncoderFrame(NULL)
,m_ulLastQKeyFrameTime(0)
,m_bRunning(false)
,m_nEncoderType(0)
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
	//DebugOutput("  EncoderStream Close   1\n");

	m_bRunning=false;
	XThreadBase::WaitForStop();

	DebugOutput( logcallstack ,"  EncoderStream Close   2\n");

	if (m_pEncoderFrame!=NULL)
	{
		//DebugOutput("  EncoderStream Close   3\n");
		m_pEncoderFrame->Close();
		//DebugOutput("  EncoderStream Close   4\n");
		delete m_pEncoderFrame;
		//DebugOutput("  EncoderStream Close   5\n");
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

int XEncoderStream::EncodeFrame(VIDEC_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType)
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

	NEWRECORD3( PFRAME ,pFrame,new FRAME(pImage,nBitrate,nMinQP,nMaxQP,nBitrateControlType,codecType) )
	//NULLReturnRet( pFrame , -1 )  
	if (pFrame==NULL)
	{
		return -1;
	}
	{
		XAutoLock l(m_csListFrames);
		m_ListFrames.push_back(pFrame);

		int nSize = m_ListFrames.size();
		if (nSize > 9)
		{				
			do
			{
				pFrame=(PFRAME)m_ListFrames.front();
				m_ListFrames.pop_front();
				if (pFrame)
				{			
					delete pFrame;
					pFrame=NULL;
				}
			}while (m_ListFrames.size() > 3);
		}
	}

	return 0;
}

int XEncoderStream::DoEncodeFrame(VIDEC_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType)
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

	if (pXImage->GetFrameRate()!=m_nFrameRate && abs(m_nFrameRate-pXImage->GetFrameRate())>=4)
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

		m_pEncoderFrame=VIDEC_EncoderCreate(*this,m_nWidth,m_nHeight,m_nFrameRate,m_codecType,m_bDoubleField,m_nBitrate,m_nMinQP,m_nMaxQP,m_nBitrateControlType,m_bRTPPacket);
	}
	int nR =-1;
	if (m_pEncoderFrame!=NULL)
	{
		if (m_bRequestKeyFrame)
		{
			unsigned long ulTempTime = XGetTimestamp();
			if (ulTempTime - m_ulLastQKeyFrameTime > 1000)
			{
				m_ulLastQKeyFrameTime = ulTempTime;
				m_bRequestKeyFrame=false;
				m_pEncoderFrame->RequestKeyFrame();
			}
		}

		nR = m_pEncoderFrame->EncodeFrame(pXImage->GetData(),pXImage->GetDataLen(),m_nVirtualWidth,m_nVirtualHeight,pXImage->GetEvenFieldFirst(),pXImage->GetHardware(),pXImage->GetTimestamp());
		if (nR !=0)
		{
			m_nEncodeErrorCount++;
			if (m_nEncodeErrorCount > 10)
			{
				DebugOutput( logsoftenc ,"m_pEncoderFrame->EncodeFrame Error!\n");
				m_bRequestKeyFrame=true;
				m_pEncoderFrame->Close();
				delete m_pEncoderFrame;
				m_pEncoderFrame=NULL;
			}
		}
		else
		{
			m_nEncodeErrorCount=0;
		}
	}

	return nR;
}

void XEncoderStream::RequestKeyFrame(void)
{
	m_bRequestKeyFrame=true;
}

int XEncoderStream::GetEncoderType(void)
{
	return m_nEncoderType;
}

void XEncoderStream::OnVIDEC_EncoderCallbackFramePacket(void*pPacketData,int nPacketLen,bool bHardwareEncoding)
{
	if (bHardwareEncoding)
	{
		m_nEncoderType = 2;
	}
	else
	{
		m_nEncoderType = 1;
	}
	m_rCallback.OnVIDEC_EncoderStreamCallbackFramePacket(pPacketData,nPacketLen,m_nVirtualWidth,m_nVirtualHeight);
}

void XEncoderStream::OnVIDEC_EncoderCallbackRTPPacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnVIDEC_EncoderStreamCallbackRTPPacket(pPacketData,nPacketLen,m_nVirtualWidth,m_nVirtualHeight);
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
			m_rCallback.OnVIDEC_EncoderStreamCallbackPreEncode(nSkipFlag);
			if (!nSkipFlag)
			{
				if (DoEncodeFrame(pFrame->m_pImage,pFrame->m_nBitrate,pFrame->m_nMinQP,pFrame->m_nMaxQP,pFrame->m_nBitrateControlType,pFrame->m_codecType) < 0)
				{
					m_nEncoderType=0;
				}
			}
			else
			{
				m_nEncoderType=0;
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