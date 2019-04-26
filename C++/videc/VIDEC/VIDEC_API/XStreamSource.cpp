#include "XStreamSource.h"
#include "VIDEC_Header.h"

XStreamSource::XStreamSource(VIDEC_StreamSource*pStreamSource)
:m_pStreamSource(pStreamSource)
{

}

XStreamSource::~XStreamSource(void)
{

}

void XStreamSource::Close(void)
{
	XAutoLock l(m_csMapStreamSink);
	for (MapStreamSink::iterator iter=m_MapStreamSink.begin();
		iter!=m_MapStreamSink.end();
		++iter)
	{
		VIDEC_StreamSink*pStreamSink=iter->second;
		pStreamSink->OnDetachedFromStreamSource(m_pStreamSource);
	}
}

int XStreamSource::SendStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen)
{
	bool bKeyFrame=(VIDEC_HEADER_EXT_GET_KEYFRAME(pData)!=0);

	XAutoLock l(m_csMapStreamSink);
	for (MapStreamSink::iterator iter=m_MapStreamSink.begin();
		iter!=m_MapStreamSink.end();
		++iter)
	{
		VIDEC_StreamSink*pStreamSink=iter->second;
		pStreamSink->OnReceivedStreamData(m_pStreamSource,pData,nLen,bKeyFrame,nWidth,nHeight,nHeaderLen);
	}

	return 0;
}

int XStreamSource::SendSubStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen)
{
	bool bKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pData)!=0;

	XAutoLock l(m_csMapStreamSink);
	for (MapStreamSink::iterator iter=m_MapStreamSink.begin();
		iter!=m_MapStreamSink.end();
		++iter)
	{
		VIDEC_StreamSink*pStreamSink=iter->second;
		pStreamSink->OnReceivedSubStreamData(m_pStreamSource,pData,nLen,bKeyFrame,nWidth,nHeight,nHeaderLen);
	}

	return 0;
}

int XStreamSource::SendAudioStreamData(unsigned char*pData,int nLen)
{
	XAutoLock l(m_csMapStreamSink);
	for (MapStreamSink::iterator iter=m_MapStreamSink.begin();
		iter!=m_MapStreamSink.end();
		++iter)
	{
		VIDEC_StreamSink*pStreamSink=iter->second;
		pStreamSink->OnReceivedAudioStreamData(m_pStreamSource,pData,nLen);
	}

	return 0;
}

int XStreamSource::AddStreamSink(VIDEC_StreamSink*pStreamSink)
{
	if (pStreamSink!=NULL)
	{
		pStreamSink->OnAttachedToStreamSource(m_pStreamSource);

		XAutoLock l(m_csMapStreamSink);
		m_MapStreamSink[pStreamSink]=pStreamSink;
		
		return 0;
	}

	return -1;
}

void XStreamSource::RemoveStreamSink(VIDEC_StreamSink*pStreamSink)
{
	if (pStreamSink!=NULL)
	{
		pStreamSink->OnDetachedFromStreamSource(m_pStreamSource);

		XAutoLock l(m_csMapStreamSink);
		m_MapStreamSink.erase(pStreamSink);
	}
}

int XStreamSource::GetStreamSinkCount(void)
{
	XAutoLock l(m_csMapStreamSink);
	return m_MapStreamSink.size();
}
