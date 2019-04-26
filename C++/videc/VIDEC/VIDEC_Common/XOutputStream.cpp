#include "XOutputStream.h"


XOutputStream::XOutputStream(VIDEC_OutputStream*pOutputStream)
:m_pOutputStream(pOutputStream)

,m_pEncoderStream(NULL)
{

}

XOutputStream::~XOutputStream(void)
{
	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->Close();
		delete m_pEncoderStream;
		m_pEncoderStream=NULL;
	}
}

int XOutputStream::GetBitrate(void)
{
	return m_pOutputStream->OnVIDEC_OutputStreamGetBitrate();

}

int XOutputStream::GetMaxBitrate(void)
{
	return m_pOutputStream->OnVIDEC_OutputStreamGetMaxBitrate();
}

int XOutputStream::GetFrameRate(void)
{
	return m_pOutputStream->OnVIDEC_OutputStreamGetFrameRate();
}

int XOutputStream::GetQuant(void)
{
	return m_pOutputStream->OnVIDEC_OutputStreamGetQuant();
}

int XOutputStream::GetQMin(void)
{
	return m_pOutputStream->OnVIDEC_OutputStreamGetQMin();
}

int XOutputStream::GetQMax(void)
{
	return m_pOutputStream->OnVIDEC_OutputStreamGetQMax();
}

VIDEC_CODEC_TYPE XOutputStream::GetCodecType(void)
{
	return m_pOutputStream->OnVIDEC_OutputStreamGetCodecType();
}


int XOutputStream::OutputImage(void*pData,int nLen,int nWidth,int nHeight)
{
	if (m_pEncoderStream==NULL)
	{
		m_pEncoderStream=VIDEC_EncoderStreamCreate(*this);
		if (m_pEncoderStream==NULL)
		{
			return -1;
		}
	}

	m_pEncoderStream->EncodeFrame(pData,nLen,nWidth,nHeight,GetFrameRate(),GetBitrate(),GetQuant(),GetQMin(),GetQMax(),GetCodecType());

	return 0;
}

void XOutputStream::OnVIDEC_EncoderStreamCallbackPacket(void*pPacketData,int nPacketSize)
{
	m_pOutputStream->OnVIDEC_OutputStreamOutputData(pPacketData,nPacketSize);
}


XOutputStreamMgr::XOutputStreamMgr(void)
{

}

XOutputStreamMgr::~XOutputStreamMgr(void)
{

}

void XOutputStreamMgr::Close(void)
{
	XAutoLock l(m_csMapXOutputStream);
	MapXOutputStream::iterator iter=m_MapXOutputStream.begin();
	while (iter!=m_MapXOutputStream.end())
	{
		XOutputStream*pXOutputStream=iter->second;
		++iter;

		delete pXOutputStream;
		pXOutputStream=NULL;
	}
	m_MapXOutputStream.clear();
}

int XOutputStreamMgr::AddOutputStream(VIDEC_OutputStream*pOutputStream)
{
	XAutoLock l(m_csMapXOutputStream);
	MapXOutputStream::iterator iter=m_MapXOutputStream.find(pOutputStream);
	if (iter==m_MapXOutputStream.end())
	{
		XOutputStream*pXOutputStream=new XOutputStream(pOutputStream);
		m_MapXOutputStream[pOutputStream]=pXOutputStream;

		return 0;
	}

	return -1;
}

void XOutputStreamMgr::RemoveOutputStream(VIDEC_OutputStream*pOutputStream)
{
	XAutoLock l(m_csMapXOutputStream);
	MapXOutputStream::iterator iter=m_MapXOutputStream.find(pOutputStream);
	if (iter!=m_MapXOutputStream.end())
	{
		XOutputStream*pXOutputStream=iter->second;
		m_MapXOutputStream.erase(iter);

		delete pXOutputStream;
		pXOutputStream=NULL;
	}
}

int XOutputStreamMgr::OutputImage(void*pData,int nLen,int nWidth,int nHeight)
{
	XAutoLock l(m_csMapXOutputStream);
	MapXOutputStream::iterator iter=m_MapXOutputStream.begin();
	while (iter!=m_MapXOutputStream.end())
	{
		XOutputStream*pXOutputStream=iter->second;
		pXOutputStream->OutputImage(pData,nLen,nWidth,nHeight);
		++iter;
	}

	return 0;
}



int XOutputStreamMgr::GetMaxFrameRate(void)
{
	int nMaxFrameRate=0;

	XAutoLock l(m_csMapXOutputStream);
	MapXOutputStream::iterator iter=m_MapXOutputStream.begin();
	while (iter!=m_MapXOutputStream.end())
	{
		XOutputStream*pXOutputStream=iter->second;
		if (pXOutputStream->GetFrameRate()>nMaxFrameRate)
		{
			nMaxFrameRate=pXOutputStream->GetFrameRate();
		}
		++iter;
	}

	return nMaxFrameRate;
}

int XOutputStreamMgr::GetOutputStreamCount(void)
{
	XAutoLock l(m_csMapXOutputStream);
	return m_MapXOutputStream.size();
}