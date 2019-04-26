
#include "stdafx.h"

#include "EncodeFrame.h"

#include "Demo_win32Dlg.h"

EncodeFrame::EncodeFrame(void)
:m_pIAVSender(NULL)
//,m_pHPAVC_Capture(NULL)
,m_pEncoderFrame(NULL)
,m_pPacketBuf(NULL)
{
	m_bWaitStop = false;
	m_nSeqNoRemoteMain = 1;

	m_ulLastCountFrameTimestamp=0;
	m_nSendFrameRateCount=0;
	m_nSendFrameRate=0;
}

EncodeFrame::~EncodeFrame(void)
{
}

void EncodeFrame::Open(int nFrameRate,int nBitrate)
{
// 	if (m_pHPAVC_Capture != NULL)
// 	{
// 		return;
// 	}

	m_nFrameRate	= nFrameRate;
	m_nBitrate		= nBitrate;


	//m_pHPAVC_Capture = HPAVC_Capture::Create(this,NULL);
	if (m_pIAVSender==NULL && GetLocalVideo()==0)
	{
		m_pIAVSender = NETEC_MediaSender::Create(*this);
		if (m_pIAVSender && 0==m_pIAVSender->Open())
		{
			unsigned long ulAudioID = GetTickCount();
			m_pIAVSender->StartAudio(ulAudioID);
			m_pIAVSender->StartVideo(ulAudioID+1);
		}
	}
	m_nEncoderType=0;
	m_bWaitStop = false;
	StartThread();
}

void EncodeFrame::Close(void)
{
	m_bWaitStop = true;
	WaitForStop();

	if (m_pEncoderFrame!=NULL)
	{
		m_pEncoderFrame->Close();
		delete m_pEncoderFrame;
		m_pEncoderFrame=NULL;
	}

// 	if (m_pHPAVC_Capture)
// 	{
// 		m_pHPAVC_Capture->Close();
// 		delete m_pHPAVC_Capture;
// 		m_pHPAVC_Capture = NULL;
// 	}

	NETEC_MediaSender* pIAVSender = m_pIAVSender;
	m_pIAVSender = NULL;

	if (pIAVSender)
	{	
		pIAVSender->StopAudio();
		pIAVSender->StopVideo();
		pIAVSender->Close();
		delete pIAVSender;
		pIAVSender = NULL;
	}

	if (m_pPacketBuf)
	{
		free(m_pPacketBuf);
		m_pPacketBuf=NULL;
	}
	{
		XAutoLock l(m_csListImage);
		while (m_listImage.size())
		{
			VIDEC_Image* pImage=(VIDEC_Image*)m_listImage.front();
			m_listImage.pop_front();
			pImage->Release();
			pImage=NULL;
		}
	}
}

void EncodeFrame::ThreadProcMain(void)
{
	while(!m_bWaitStop)
	{
		VIDEC_Image* pImage=NULL;
		{
			XAutoLock l(m_csListImage);
			int nSize = m_listImage.size();			
			if (nSize > 6)
			{				
				do
				{
					pImage=(VIDEC_Image*)m_listImage.front();
					m_listImage.pop_front();
					if (pImage)
					{			
						pImage->Release();
						pImage=NULL;
					}
				}while (m_listImage.size() > 3);
			}
			if (nSize)
			{
				pImage=(VIDEC_Image*)m_listImage.front();
				m_listImage.pop_front();
			}
		}
		if (pImage)
		{
// 			if (m_pHPAVC_Capture && m_pIAVSender)
// 			{
// 				m_nFrameRate= pImage->GetFrameRate();
// 
// 				if (m_pHPAVC_Capture && m_pHPAVC_Capture->IsOpen() == false)
// 				{
// 					int nKeyIntMax=m_nFrameRate*60*5;
// 					m_pHPAVC_Capture->Open(pImage->GetWidth(),pImage->GetHeight(),m_nFrameRate,HPAVC_HIGH_PROFILE,m_nBitrate,nKeyIntMax);
// 				}
// 
// 				int nRequestKeyFrame = m_pIAVSender->IsVideoRequestKeyFrame(0);
// 				m_pHPAVC_Capture->EncodeData((unsigned char*)pImage->GetData(),pImage->GetDataLen(),nRequestKeyFrame);
// 			}

			if (m_pEncoderFrame == NULL)
			{
				int  nMinQP = 28;
				int	 nMaxQP = 40;
				m_pEncoderFrame=VIDEC_EncoderCreate(*this,pImage->GetWidth(),pImage->GetHeight(),m_nFrameRate,VIDEC_CODEC_H264_SVC,false,m_nBitrate,nMinQP,nMaxQP,VIDEC_NETWORK_INTERNET,false);
			}

			if (m_pEncoderFrame!=NULL)
			{
				if (m_pIAVSender)
				{
					int nRequestKeyFrame = m_pIAVSender->IsVideoRequestKeyFrame(0);
					if (nRequestKeyFrame)
					{
						m_pEncoderFrame->RequestKeyFrame();
					}				
				}

				m_pEncoderFrame->EncodeFrame(pImage->GetData(),pImage->GetDataLen(),pImage->GetWidth(),pImage->GetHeight(),false,false,timeGetTime());
			}

			pImage->Release();
			pImage=NULL;
		}
		else
		{
			Sleep(1);
		}
	}
}

void EncodeFrame::GetAVParameters(unsigned long& ulAudioID,unsigned long& ulVideoID)
{
	if (m_pIAVSender)
	{
		m_pIAVSender->GetAudioID(ulAudioID);
		m_pIAVSender->GetVideoID(ulVideoID);
	}
}

void EncodeFrame::EncodeData(unsigned char*pData,int nLen)
{
// 	if (m_pHPAVC_Capture && m_pIAVSender)
// 	{
// 		if (m_pHPAVC_Capture->IsOpen() == false)
// 		{
// 			int nKeyIntMax=m_nFrameRate*60*5;
// 			m_pHPAVC_Capture->Open(1920,1080,m_nFrameRate,HPAVC_HIGH_PROFILE,m_nBitrate,nKeyIntMax);
// 		}
// 		int nRequestKeyFrame = m_pIAVSender->IsVideoRequestKeyFrame(0);
// 		m_pHPAVC_Capture->EncodeData(pData,nLen,nRequestKeyFrame);
// 	}
	if (m_pEncoderFrame == NULL)
	{
		int  nMinQP = 28;
		int	 nMaxQP = 40;
		m_pEncoderFrame=VIDEC_EncoderCreate(*this,1920,1080,m_nFrameRate,VIDEC_CODEC_H264_SVC,false,m_nBitrate,nMinQP,nMaxQP,VIDEC_NETWORK_PRIVATE,false);
	}

	if (m_pEncoderFrame!=NULL)
	{
		if (m_pIAVSender)
		{
			int nRequestKeyFrame = m_pIAVSender->IsVideoRequestKeyFrame(0);
			if (nRequestKeyFrame)
			{
				m_pEncoderFrame->RequestKeyFrame();
			}
		}
		m_pEncoderFrame->EncodeFrame(pData,nLen,1920,1080,false,false,timeGetTime());
	}
}

void EncodeFrame::EncodeImage(VIDEC_Image* pImage)
{
	XAutoLock l(m_csListImage);
	pImage->AddRef();
	m_listImage.push_back(pImage);
}

void EncodeFrame::SetVideoParam(Video_Param param)
{
// 	if (m_pHPAVC_Capture)
// 	{
// 		m_pHPAVC_Capture->SetVideoParam(param.iBrightness,param.iContrast,param.iSaturation,param.iHue,param.iDetail,param.iDenoise);
// 	}
}


int EncodeFrame::GetVideoSendFrameRate(void)
{
	int nRate = m_nSendFrameRate;
	m_nSendFrameRate=0;
	return nRate;
}


void EncodeFrame::ReSetFrameRate(void)
{
	unsigned long ulCurTimestamp = timeGetTime();
	if (m_ulLastCountFrameTimestamp == 0)
	{
		m_ulLastCountFrameTimestamp = ulCurTimestamp;
	}
	unsigned long ulTemp = ulCurTimestamp - m_ulLastCountFrameTimestamp;
	if ((m_nSendFrameRateCount > 30 && ulTemp < 990) || (m_nSendFrameRateCount <= 30 && ulTemp < 980))
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


void EncodeFrame::OnVIDEC_EncoderCallbackFramePacket(void*pPacketData,int nPacketLen,bool bHardwareEncoding)
{
	if (bHardwareEncoding)
	{
		m_nEncoderType = 2;
	}
	else
	{
		m_nEncoderType = 1;
	}

	VIDEC_HEADER_EXT_SET_MAIN_FRAME(pPacketData,1);
	VIDEC_HEADER_EXT_SET_SUB_FRAME(pPacketData,1);
	VIDEC_HEADER_EXT_SET_QSUB_FRAME(pPacketData,1);
	
	m_nSendFrameRateCount++;
	ReSetFrameRate();
	
	if (m_pIAVSender)
	{
		int n = m_pIAVSender->SendVideo((unsigned char*)pPacketData,nPacketLen);
	}

	if (GetLocalVideo()==1)
	{
		CDemo_win32Dlg::Instance()->PlayData(pPacketData, nPacketLen);
	}
}

void EncodeFrame::OnHPAVC_CaptureCallbackPacketData(void*pData,int nLen,int nKeyFrame,int nWidth,int nHeight)
{
	if (m_pIAVSender)
	{
		if (m_pPacketBuf==NULL)
		{
			m_pPacketBuf=(unsigned char*)malloc(1920*1080*2);
			if (m_pPacketBuf==NULL)
				return;
		}

		int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
		int nPacketLen = nLen + nHeaderLen;
		if (m_pPacketBuf)
		{
			unsigned char*pPacketData=NULL;
			pPacketData=m_pPacketBuf+4+VIDEC_HEADER_EXT_KEY_FRAME_LEN;

			memcpy(pPacketData,pData,nLen);

			if (nKeyFrame)
			{
				unsigned char*pFrameHeader=(unsigned char*)pPacketData-4;
				memset(pFrameHeader,0,4);
				PAVCON_H264_HEADER pAVCONH264Header=(PAVCON_H264_HEADER)pFrameHeader;
				pAVCONH264Header->ID=0xFFFF;
				pAVCONH264Header->framerate=m_nFrameRate;
				nHeaderLen+=4;
			}
			pPacketData-=nHeaderLen;

			if (nKeyFrame)
			{
				VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pPacketData,nWidth);
				VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pPacketData,nHeight);
				VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pPacketData,nWidth);
				VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pPacketData,nHeight);
			}

			VIDEC_HEADER_EXT_RESET(pPacketData);
			VIDEC_HEADER_EXT_SET_CODEC_ID(pPacketData,4);
			VIDEC_HEADER_EXT_SET_EVEN_FIELD_FIRST(pPacketData,0);
			VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pPacketData,0);
			VIDEC_HEADER_EXT_SET_HARDWARE(pPacketData,1);
			VIDEC_HEADER_EXT_SET_KEYFRAME(pPacketData,(nKeyFrame!=0?1:0));
			VIDEC_HEADER_EXT_SET_BFRAME(pPacketData,0);

			VIDEC_HEADER_EXT_SET_SEQUENCE(pPacketData,m_nSeqNoRemoteMain++);
			VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,timeGetTime());

			VIDEC_HEADER_EXT_SET_MAIN_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_SUB_FRAME(pPacketData,1);
			VIDEC_HEADER_EXT_SET_QSUB_FRAME(pPacketData,1);

			int n = m_pIAVSender->SendVideo((unsigned char*)pPacketData,nPacketLen);
		}
	}
}