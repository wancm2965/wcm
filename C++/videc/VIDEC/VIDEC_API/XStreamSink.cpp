#include "XStreamSink.h"

XStreamSink::XStreamSink(VIDEC_StreamSink*pStreamSink)
:m_pStreamSink(pStreamSink)
,m_pStreamSource(NULL)
{

}

XStreamSink::~XStreamSink(void)
{

}

void XStreamSink::Close(void)
{
	XAutoLock l(m_csStreamSource);
	if (m_pStreamSource!=NULL)
	{
		m_pStreamSource->RemoveStreamSink(m_pStreamSink);
		m_pStreamSource=NULL;
	}
}

void XStreamSink::OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource)
{
	XAutoLock l(m_csStreamSource);
	if (m_pStreamSource!=NULL)
	{
		m_pStreamSource->RemoveStreamSink(m_pStreamSink);
	}
	m_pStreamSource=pStreamSource;
}

void XStreamSink::OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource)
{
	XAutoLock l(m_csStreamSource);
	if (m_pStreamSource==pStreamSource)
	{
		m_pStreamSource=NULL;
	}
}
