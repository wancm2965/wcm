#include "XImageSink.h"

XImageSink::XImageSink(VIDEC_ImageSink*pImageSink)
:m_pImageSink(pImageSink)
,m_pImageSource(NULL)
{

}

XImageSink::~XImageSink(void)
{

}

void XImageSink::Close(void)
{
	XAutoLock l(m_csImageSource);
	if (m_pImageSource!=NULL)
	{
		m_pImageSource->RemoveImageSink(m_pImageSink);
		m_pImageSource=NULL;
	}
}

void XImageSink::OnAttachedToImageSource(VIDEC_ImageSource*pImageSource)
{
	XAutoLock l(m_csImageSource);
	if (m_pImageSource!=NULL)
	{
		m_pImageSource->RemoveImageSink(m_pImageSink);
	}
	m_pImageSource=pImageSource;
}

void XImageSink::OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource)
{
	XAutoLock l(m_csImageSource);
	if (m_pImageSource==pImageSource)
	{
		m_pImageSource=NULL;
	}
}
