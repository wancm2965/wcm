#include "XOutputRTP.h"


XOutputRTP::XOutputRTP(VIDEC_OutputRTP*pOutputRTP)
:m_pOutputRTP(pOutputRTP)
,m_pEncoderRTP(NULL)

,m_nWidth(0)
,m_nHeight(0)
{

}

XOutputRTP::~XOutputRTP(void)
{
	if (m_pEncoderRTP!=NULL)
	{
		m_pEncoderRTP->Close();
		delete m_pEncoderRTP;
		m_pEncoderRTP=NULL;
	}
}

int XOutputRTP::GetBitrate(void)
{
	return m_pOutputRTP->OnVIDEC_OutputRTPGetBitrate();

}

int XOutputRTP::GetMaxBitrate(void)
{
	return m_pOutputRTP->OnVIDEC_OutputRTPGetMaxBitrate();
}

int XOutputRTP::GetFrameRate(void)
{
	return m_pOutputRTP->OnVIDEC_OutputRTPGetFrameRate();
}

int XOutputRTP::GetQuant(void)
{
	return m_pOutputRTP->OnVIDEC_OutputRTPGetQuant();
}

int XOutputRTP::GetQMin(void)
{
	return m_pOutputRTP->OnVIDEC_OutputRTPGetQMin();
}

int XOutputRTP::GetQMax(void)
{
	return m_pOutputRTP->OnVIDEC_OutputRTPGetQMax();
}

VIDEC_CODEC_TYPE XOutputRTP::GetCodecType(void)
{
	return m_pOutputRTP->OnVIDEC_OutputRTPGetCodecType();
}


int XOutputRTP::OutputImage(void*pData,int nLen,int nWidth,int nHeight)
{
	if (m_nWidth!=nWidth || m_nHeight!=nHeight)
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;

		if (m_pEncoderRTP!=NULL)
		{
			m_pEncoderRTP->Close();
			delete m_pEncoderRTP;
			m_pEncoderRTP=NULL;
		}
	}

	if (!m_pOutputRTP->OnVIDEC_OutputRTPIsValidImageSize(m_nWidth,m_nHeight))
	{
		return -1;
	}

	if (m_pEncoderRTP==NULL)
	{
		m_pEncoderRTP=VIDEC_EncoderRTPCreate(*this,nWidth,nHeight,GetFrameRate(),GetBitrate(),GetQuant(),GetQMin(),GetQMax(),96,GetCodecType());
		if (m_pEncoderRTP==NULL)
		{
			return -1;
		}
	}
	m_pEncoderRTP->EncodeFrame(pData,nLen);

	return 0;
}

void XOutputRTP::OnVIDEC_EncoderRTPCallbackPacket(void*pPacketData,int nPacketSize)
{
	m_pOutputRTP->OnVIDEC_OutputRTPOutputData(pPacketData,nPacketSize);
}


XOutputRTPMgr::XOutputRTPMgr(void)
{

}

XOutputRTPMgr::~XOutputRTPMgr(void)
{

}

void XOutputRTPMgr::Close(void)
{
	XAutoLock l(m_csMapXOutputRTP);
	MapXOutputRTP::iterator iter=m_MapXOutputRTP.begin();
	while (iter!=m_MapXOutputRTP.end())
	{
		XOutputRTP*pXOutputRTP=iter->second;
		++iter;

		delete pXOutputRTP;
		pXOutputRTP=NULL;
	}
	m_MapXOutputRTP.clear();
}

int XOutputRTPMgr::AddOutputRTP(VIDEC_OutputRTP*pOutputRTP)
{
	XAutoLock l(m_csMapXOutputRTP);
	MapXOutputRTP::iterator iter=m_MapXOutputRTP.find(pOutputRTP);
	if (iter==m_MapXOutputRTP.end())
	{
		XOutputRTP*pXOutputRTP=new XOutputRTP(pOutputRTP);
		m_MapXOutputRTP[pOutputRTP]=pXOutputRTP;

		return 0;
	}

	return -1;
}

void XOutputRTPMgr::RemoveOutputRTP(VIDEC_OutputRTP*pOutputRTP)
{
	XAutoLock l(m_csMapXOutputRTP);
	MapXOutputRTP::iterator iter=m_MapXOutputRTP.find(pOutputRTP);
	if (iter!=m_MapXOutputRTP.end())
	{
		XOutputRTP*pXOutputRTP=iter->second;
		m_MapXOutputRTP.erase(iter);

		delete pXOutputRTP;
		pXOutputRTP=NULL;
	}
}

int XOutputRTPMgr::OutputImage(void*pData,int nLen,int nWidth,int nHeight)
{
	XAutoLock l(m_csMapXOutputRTP);
	MapXOutputRTP::iterator iter=m_MapXOutputRTP.begin();
	while (iter!=m_MapXOutputRTP.end())
	{
		XOutputRTP*pXOutputRTP=iter->second;
		pXOutputRTP->OutputImage(pData,nLen,nWidth,nHeight);
		++iter;
	}

	return 0;
}


int XOutputRTPMgr::GetMaxFrameRate(void)
{
	int nMaxFrameRate=0;

	XAutoLock l(m_csMapXOutputRTP);
	MapXOutputRTP::iterator iter=m_MapXOutputRTP.begin();
	while (iter!=m_MapXOutputRTP.end())
	{
		XOutputRTP*pXOutputRTP=iter->second;
		if (pXOutputRTP->GetFrameRate()>nMaxFrameRate)
		{
			nMaxFrameRate=pXOutputRTP->GetFrameRate();
		}
		++iter;
	}

	return nMaxFrameRate;
}