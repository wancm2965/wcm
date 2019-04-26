#include "ScreenCapChanRTP.h"

ScreenCapChanRTP::ScreenCapChanRTP(void)
{
	m_nScreenIndex=-1;
	m_pMPScreen=NULL;
	m_pCapChan=NULL;
	m_nCodecType=VIDEC_CODEC_H264;
}

ScreenCapChanRTP::~ScreenCapChanRTP(void)
{
}
	
int ScreenCapChanRTP::Open(int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate)
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
	}

	if (m_pMPScreen!=NULL && m_pCapChan==NULL)
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
		case VIDEC_CODEC_H263P://H.263
			nPayloadType=107;
			break;
		case VIDEC_CODEC_H263PP://H.263
			nPayloadType=108;
			break;
			//notice 20120116 update
		case VIDEC_CODEC_H264://H.264
		case VIDEC_CODEC_H264_SVC://H.264
			nPayloadType=106;
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
				m_pCapChan=pVideoCapChan;
				m_pCapChan->AddStreamSink(this);
				m_pMPScreen->AddImageSink(m_pCapChan);
			}
		}
	}
	return (m_pMPScreen!=NULL?0:-1);
}

void ScreenCapChanRTP::Close(void)
{
	if (m_pCapChan!=NULL)
	{
		m_pCapChan->Close();
		delete m_pCapChan;
		m_pCapChan=NULL;
	}

	if (m_pMPScreen!=NULL)
	{
		m_pMPScreen->Close();
		delete m_pMPScreen;
		m_pMPScreen=NULL;
	}
}

void ScreenCapChanRTP::SetScreenIndex(int nScreenIndex)
{
	m_nScreenIndex=nScreenIndex;
	if (m_pMPScreen)
	{
		m_pMPScreen->SetScreenIndex(m_nScreenIndex);
	}
}

void ScreenCapChanRTP::AddMediaCall(MediaCall*pMediaCall)
{
	XAutoLock l(m_csMapMediaCall);
	m_MapMediaCall[pMediaCall]=pMediaCall;

}

bool ScreenCapChanRTP::RemoveMediaCall(MediaCall*pMediaCall)
{
	XAutoLock l(m_csMapMediaCall);
	MapMediaCall::iterator iter=m_MapMediaCall.find(pMediaCall);
	if (iter!=m_MapMediaCall.end())
	{
		m_MapMediaCall.erase(iter);
		return true;
	}
	return false;
}

int ScreenCapChanRTP::GetMediaCallCount(void)
{
	XAutoLock l(m_csMapMediaCall);
	return m_MapMediaCall.size();
}

bool ScreenCapChanRTP::RequestKeyFrame(MediaCall*pMediaCall)
{
	if (pMediaCall)
	{
		XAutoLock l(m_csMapMediaCall);
		MapMediaCall::iterator iter=m_MapMediaCall.find(pMediaCall);
		if (iter==m_MapMediaCall.end())
		{
			return false;
		}

	}

	if (m_pCapChan)
	{
		m_pCapChan->RequestKeyFrame();
	}

	return true;
}

void ScreenCapChanRTPMain::OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)
{
	XAutoLock l(m_csMapMediaCall);
	MapMediaCall::iterator iter=m_MapMediaCall.begin();
	while (iter!=m_MapMediaCall.end())
	{
		MediaCall*pMediaCall=iter->second;
		pMediaCall->SendMainVideo((char*)pPacketData,nPacketLen);
		++iter;
	}
}

void ScreenCapChanRTPSecond::OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)
{
	XAutoLock l(m_csMapMediaCall);
	MapMediaCall::iterator iter=m_MapMediaCall.begin();
	while (iter!=m_MapMediaCall.end())
	{
		MediaCall*pMediaCall=iter->second;
		pMediaCall->SendSecondVideo((char*)pPacketData,nPacketLen);
		++iter;
	}
}