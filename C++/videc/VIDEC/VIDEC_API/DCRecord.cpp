//DCRecord.cpp

#include "DCRecord.h"
#include "VIDEC_Header.h"
#include "XUtil.h"

VIDEC_DCRecord*VIDEC_DCRecord::Create(void)
{
	//return new DCRecord();
	NEWRECORDret3(  VIDEC_DCRecord* ,p , new DCRecord() ) 

}

DCRecord::DCRecord(void)
:m_pAVIWriter(NULL)
,m_pCapChan(NULL)
,m_pDC(NULL)
{
	m_ulFirstVideoTimestamp=XGetTimestamp();
	m_ulFrameInterval=100;
}

DCRecord::~DCRecord(void)
{

};

int DCRecord::Open(HDC hDC,int x,int y,int cx,int cy,const char*cszPathName,int nFrameRate,int nQuality,int nBitrateControlType)
{
	m_pAVIWriter=VIDEC_AVIWriter::Create();
	if (m_pAVIWriter==NULL)
	{
		return -1;
	}
	if (nFrameRate<1)
		nFrameRate=1;
	if (nFrameRate>30)
		nFrameRate=30;

	m_ulFrameInterval=1000/nFrameRate;

	if (0!=m_pAVIWriter->Open(cszPathName,1))
	{
		m_pAVIWriter->Close();
		delete m_pAVIWriter;
		m_pAVIWriter=NULL;
		return -1;
	}

	cx-=(cx%16);
	cy-=(cy%16);
	if (cx<16)
		cx=16;
	if (cy<16)
		cy=16;

	m_pDC=VIDEC_DC::Create();
	if (m_pDC==NULL)
	{
		return -1;
	}
	if (0!=m_pDC->Open(hDC,x,y,cx,cy))
	{
		m_pDC->Close();
		delete m_pDC;
		m_pDC=NULL;
	}


	m_pCapChan=VIDEC_CapChan::Create(*this);
	if (m_pCapChan==NULL)
	{
		return -1;
	}
	if (0!=m_pCapChan->Open(nQuality,nBitrateControlType,VIDEC_CODEC_H263P,0,0,0))
	{
		m_pCapChan->Close();
		delete m_pCapChan;
		m_pCapChan=NULL;
		return -1;
	}

	m_pDC->AddImageSink(m_pCapChan);
	m_pCapChan->AddStreamSink(this);

	return 0;
}

void DCRecord::Close(void)
{
	if (m_pDC!=NULL)
	{
		m_pDC->Close();
		delete m_pDC;
		m_pDC=NULL;
	}

	if (m_pCapChan!=NULL)
	{
		m_pCapChan->Close();
		delete m_pCapChan;
		m_pCapChan=NULL;
	}

	if (m_pAVIWriter!=NULL)
	{
		m_pAVIWriter->Close();
		delete m_pAVIWriter;
		m_pAVIWriter=NULL;
	}
}

void DCRecord::CaptureFrame(void)
{
	if (m_pDC!=NULL)
	{
		m_pDC->RequestFrame();
	}
}

bool DCRecord::IsDone(void)
{
	return true;
}

void DCRecord::OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource)
{

}

void DCRecord::OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource)
{

}

void DCRecord::OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
	if (m_pAVIWriter!=NULL)
	{
		m_ulFirstVideoTimestamp+=m_ulFrameInterval;
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pData,m_ulFirstVideoTimestamp);
		m_pAVIWriter->WriteVideo(pData,nLen);
	}
}