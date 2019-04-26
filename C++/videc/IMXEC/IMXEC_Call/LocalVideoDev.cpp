#include "LocalVideoDev.h"
#include "CallMgr.h"

LocalVideoDev::LocalVideoDev(void)
{
	m_pVideoCapDev=NULL;
	m_pVideoCapChan=NULL;
	m_nVideoCodecType=VIDEC_CODEC_H264;

	m_nVideoBrightness=VIDEC_GetDefaultBrightness();
	m_nVideoContrast=VIDEC_GetDefaultContrast();
	m_nVideoSaturation=VIDEC_GetDefaultSaturation();
	m_nVideoHue=VIDEC_GetDefaultHue();
	m_nVideoGamma=VIDEC_GetDefaultGamma();
	m_nVideoSharpen=VIDEC_GetDefaultSharpen();
	m_bVideoSharpenLocalOnly=true;

	m_nVideoEnableOSD=0;
	m_strVideoOSD="";
	m_nVideoAlign=0;
	ZeroMemory(&m_fontVideoOSD, sizeof(m_fontVideoOSD));	
	m_fontVideoOSD.lfWeight = FW_HEAVY;//FW_BOLD;
	strcpy(m_fontVideoOSD.lfFaceName, "Arial");
	m_fontVideoOSD.lfHeight = -16;
	m_fontVideoOSD.lfQuality = NONANTIALIASED_QUALITY;
	m_crVideoText=(RGB(0,0,255));
	m_crVideoBk=(RGB(255,255,255));


	m_nScreenIndex=-1;
	m_pMPScreen=NULL;
}

LocalVideoDev::~LocalVideoDev(void)
{

}

int LocalVideoDev::Open(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate)
{
	m_nVideoCodecType=codecType;

	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(nCallType);
	if (nCallType==IMXEC_PERSONAL_CALL)
	{
		if (nDevID<0)
		{
			Close();
			return -1;
		}

		VIDEC_CapDev*pOldCapDev=m_pVideoCapDev;
		while (1)
		{
			int nTempDevCnt=VIDEC_AH2KDev::GetDevCount();
			if (nDevID < nTempDevCnt)
			{
				if (nDevID>=0)
				{
					VIDEC_AH2KDev*pAH2KDev=NULL;
					if (m_pVideoCapDev!=NULL)
					{
						pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pVideoCapDev);
					}
					if (pAH2KDev==NULL)
					{
						if (m_pVideoCapDev!=NULL)
						{
							m_pVideoCapDev->Close();
							delete m_pVideoCapDev;
							m_pVideoCapDev=NULL;

							pOldCapDev=NULL;
						}

						pAH2KDev=VIDEC_AH2KDev::Create();
						if (pAH2KDev!=NULL)
						{
							if (0!=pAH2KDev->Open(nDevID,nWidth,nHeight,nFrameRate))
							{
								pAH2KDev->Close();
								delete pAH2KDev;
								pAH2KDev=NULL;
							}
							else
							{
								m_pVideoCapDev=pAH2KDev;
							}
						}
					}
					else
					{
						pAH2KDev->Reset(nDevID,nWidth,nHeight,nFrameRate);
					}

					break ;
				}
			}

			nDevID-=nTempDevCnt;
			nTempDevCnt=VIDEC_CameraDev::GetDevCount();
			if (nDevID < nTempDevCnt)
			{
				if (nDevID>=0)
				{
					VIDEC_CameraDev*pCameraDev=NULL;
					if (m_pVideoCapDev!=NULL)
					{
						pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pVideoCapDev);
					}
					if (pCameraDev==NULL)
					{
						if (m_pVideoCapDev!=NULL)
						{
							m_pVideoCapDev->Close();
							delete m_pVideoCapDev;
							m_pVideoCapDev=NULL;

							pOldCapDev=NULL;
						}

						pCameraDev=VIDEC_CameraDev::Create();
						if (pCameraDev!=NULL)
						{
							if (0!=pCameraDev->Open(nDevID,nWidth,nHeight,nFrameRate))
							{
								pCameraDev->Close();
								delete pCameraDev;
								pCameraDev=NULL;
							}
							else
							{
								m_pVideoCapDev=pCameraDev;
							}
						}
					}
					else
					{
						pCameraDev->Reset(nDevID,nWidth,nHeight,nFrameRate);
					}
					break ;
				}
			}

			nDevID-=nTempDevCnt;
			nTempDevCnt=VIDEC_AH400Dev::GetDevCount();
			if (nDevID < nTempDevCnt)
			{
				if (nDevID>=0)
				{
					VIDEC_AH400Dev*pAH400Dev=NULL;
					if (m_pVideoCapDev!=NULL)
					{
						pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pVideoCapDev);
					}
					if (pAH400Dev==NULL)
					{
						if (m_pVideoCapDev!=NULL)
						{
							m_pVideoCapDev->Close();
							delete m_pVideoCapDev;
							m_pVideoCapDev=NULL;

							pOldCapDev=NULL;
						}

						pAH400Dev=VIDEC_AH400Dev::Create();
						if (pAH400Dev!=NULL)
						{
							if (0!=pAH400Dev->Open(nDevID,nWidth,nHeight,nFrameRate))
							{
								pAH400Dev->Close();
								delete pAH400Dev;
								pAH400Dev=NULL;
							}
							else
							{
								m_pVideoCapDev=pAH400Dev;
							}
						}
					}
					else
					{
						pAH400Dev->Reset(nDevID,nWidth,nHeight,nFrameRate);
					}
					break ;
				}
			}

			break;
		}

		if (pOldCapDev!=m_pVideoCapDev && 
			m_pVideoCapDev!=NULL)
		{
			m_pVideoCapDev->EnableOSD(m_nVideoEnableOSD);
			m_pVideoCapDev->SetOSDFont(m_fontVideoOSD,m_crVideoText,m_crVideoBk);
			m_pVideoCapDev->SetOSDText(m_strVideoOSD.c_str(),m_nVideoAlign);

			int nSharpen=m_nVideoSharpen;
			if (m_bVideoSharpenLocalOnly)
			{
				nSharpen=VIDEC_GetDefaultSharpen();
			}

			if (m_pVideoCapDev)
			{
				VIDEC_AH400Dev*pAH400Dev=dynamic_cast<VIDEC_AH400Dev*>(m_pVideoCapDev);
				if (pAH400Dev!=NULL)
				{
					pAH400Dev->SetVideoParam(m_nVideoBrightness,m_nVideoContrast,m_nVideoSaturation,m_nVideoHue,m_nVideoGamma,nSharpen);
				}
				else
				{
					VIDEC_AH2KDev*pAH2KDev=dynamic_cast<VIDEC_AH2KDev*>(m_pVideoCapDev);
					if (pAH2KDev!=NULL)
					{
						pAH2KDev->SetVideoParam(m_nVideoBrightness,m_nVideoContrast,m_nVideoSaturation,m_nVideoHue,m_nVideoGamma,nSharpen);
					}
					else
					{
						VIDEC_CameraDev*pCameraDev=dynamic_cast<VIDEC_CameraDev*>(m_pVideoCapDev);
						if (pCameraDev!=NULL)
						{
							pCameraDev->SetVideoParam(m_nVideoBrightness,m_nVideoContrast,m_nVideoSaturation,m_nVideoHue,m_nVideoGamma,nSharpen);
						}
					}
				}
			}
		}

		if (m_pVideoCapDev!=NULL)
		{
			if (m_pVideoCapChan==NULL)
			{
				int nPayloadType=0;
				switch (codecType)
				{
				case VIDEC_CODEC_H261://H.261
					nPayloadType=31;
					break;
				case VIDEC_CODEC_H263://H.263
					nPayloadType=34;
					break;
				case VIDEC_CODEC_H264://H.264
					nPayloadType=97;
					break;
				}			


				VIDEC_CapChanRTP*pVideoCapChan=VIDEC_CapChanRTP::Create(*this);
				if (pVideoCapChan!=NULL)
				{
					if (0!=pVideoCapChan->Open(nPayloadType,nWidth,nHeight,nFrameRate,nBitrate,codecType))
					{
						pVideoCapChan->Close();
						delete pVideoCapChan;
						pVideoCapChan=NULL;
					}
					else
					{
						m_pVideoCapChan=pVideoCapChan;
						m_pVideoCapChan->AddStreamSink(this);
						m_pVideoCapDev->AddImageSink(m_pVideoCapChan);
					}
				}
			}
		}

		return (m_pVideoCapDev!=NULL?0:-1);
	}
	else if (nCallType==IMXEC_CONFERENCE_CALL)
	{
		if (m_pMPScreen==NULL)
		{
			m_pMPScreen=VIDEC_MPScreen::Create();
			if (m_pMPScreen!=NULL)
			{
				if (0!=m_pMPScreen->Open(nWidth,nHeight,nFrameRate))
				{
					m_pMPScreen->Close();
					delete m_pMPScreen;
					m_pMPScreen=NULL;
				}
			}
		}

		if (m_pMPScreen!=NULL)
		{
			if (m_nScreenIndex>=0)
			{
				m_pMPScreen->SetScreenIndex(m_nScreenIndex);
			}

			if (m_pVideoCapChan==NULL)
			{
				int nPayloadType=0;
				switch (codecType)
				{
				case VIDEC_CODEC_H261://H.261
					nPayloadType=31;
					break;
				case VIDEC_CODEC_H263://H.263
					nPayloadType=34;
					break;
				case VIDEC_CODEC_H264://H.264
					nPayloadType=97;
					break;
				}			


				VIDEC_CapChanRTP*pVideoCapChan=VIDEC_CapChanRTP::Create(*this);
				if (pVideoCapChan!=NULL)
				{
					if (0!=pVideoCapChan->Open(nPayloadType,nWidth,nHeight,nFrameRate,nBitrate,codecType))
					{
						pVideoCapChan->Close();
						delete pVideoCapChan;
						pVideoCapChan=NULL;
					}
					else
					{
						m_pVideoCapChan=pVideoCapChan;
						m_pVideoCapChan->AddStreamSink(this);
						m_pMPScreen->AddImageSink(m_pVideoCapChan);
					}
				}
			}
		}

		return (m_pMPScreen!=NULL?0:-1);
	}

	return -1;
}

void LocalVideoDev::RequestKeyFrame(void)
{
	if (m_pVideoCapChan!=NULL)
	{
		m_pVideoCapChan->RequestKeyFrame();
	}
}

void LocalVideoDev::Close(void)
{
	if (m_pVideoCapChan!=NULL)
	{
		m_pVideoCapChan->Close();
		delete m_pVideoCapChan;
		m_pVideoCapChan=NULL;
	}

	if (m_pVideoCapDev!=NULL)
	{
		m_pVideoCapDev->Close();
		delete m_pVideoCapDev;
		m_pVideoCapDev=NULL;
	}

	if (m_pMPScreen!=NULL)
	{
		m_pMPScreen->Close();
		delete m_pMPScreen;
		m_pMPScreen=NULL;
	}
}


void LocalVideoDev::OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
}

void LocalVideoDev::OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)
{
	//m_rCallback.OnCapChanRTPCallbackSendVideo(this,(void*)pPacketData,nPacketLen);
}
