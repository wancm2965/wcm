#include "XImageSink.h"

XImageSink::XImageSink(MPVideo_ImageSink*pImageSink)
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

void XImageSink::OnAttachedToImageSource(MPVideo_ImageSource*pImageSource)
{
	XAutoLock l(m_csImageSource);
	if (m_pImageSource!=NULL)
	{
		m_pImageSource->RemoveImageSink(m_pImageSink);
	}
	m_pImageSource=pImageSource;
}

void XImageSink::OnDetachedFromImageSource(MPVideo_ImageSource*pImageSource)
{
	XAutoLock l(m_csImageSource);
	if (m_pImageSource==pImageSource)
	{
		m_pImageSource=NULL;
	}
}
