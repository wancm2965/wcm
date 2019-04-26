#include "XImageSource.h"

XImageSource::XImageSource(VIDEC_ImageSource*pImageSource)
:m_pImageSource(pImageSource)
{

}

XImageSource::~XImageSource(void)
{

}

void XImageSource::Close(void)
{
	XAutoLock l(m_csListImageSink);
	for (XListPtr::iterator iter=m_ListImageSink.begin();
		iter!=m_ListImageSink.end();
		++iter)
	{
		VIDEC_ImageSink*pImageSink=(VIDEC_ImageSink*)*iter;
		pImageSink->OnDetachedFromImageSource(m_pImageSource);
	}
}
int XImageSource::PreSendImage(void)
{
	XAutoLock l(m_csListImageSink);
	for (XListPtr::iterator iter=m_ListImageSink.begin();
		iter!=m_ListImageSink.end();
		++iter)
	{
		int nSend=0;
		VIDEC_ImageSink*pImageSink=(VIDEC_ImageSink*)*iter;
		pImageSink->OnReceivedPreSend(nSend);
		if (nSend)
		{
			return 1;
		}
	}

	return 0;
}
int XImageSource::SendImage(VIDEC_Image*pImage)
{
	XAutoLock l(m_csListImageSink);
	for (XListPtr::iterator iter=m_ListImageSink.begin();
		iter!=m_ListImageSink.end();
		++iter)
	{
		VIDEC_ImageSink*pImageSink=(VIDEC_ImageSink*)*iter;
		int nSend=0;
		pImageSink->OnReceivedPreSend(nSend);
		if (nSend)
		{
			pImageSink->OnReceivedImage(pImage);
		}
	}

	return 0;
}

int XImageSource::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	if (pImageSink!=NULL)
	{
		pImageSink->OnAttachedToImageSource(m_pImageSource);
		XAutoLock l(m_csListImageSink);
		if (m_ListImageSink.find(pImageSink)==m_ListImageSink.end())
			m_ListImageSink.push_back(pImageSink);
		
		return 0;
	}

	return -1;
}

void XImageSource::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	if (pImageSink!=NULL)
	{
		pImageSink->OnDetachedFromImageSource(m_pImageSource);

		XAutoLock l(m_csListImageSink);
		m_ListImageSink.remove(pImageSink);
	}
}

int XImageSource::GetImageSinkCount(void)
{
	XAutoLock l(m_csListImageSink);
	return m_ListImageSink.size();
}

int XImageSource::SendMotionDetect(int*pResultList,int nResultCount)
{
	XAutoLock l(m_csListImageSink);
	for (XListPtr::iterator iter=m_ListImageSink.begin();
		iter!=m_ListImageSink.end();
		++iter)
	{
		VIDEC_ImageSink*pImageSink=(VIDEC_ImageSink*)*iter;
		pImageSink->OnReceivedMotionDetect(pResultList,nResultCount);
	}

	return 0;
}

int XImageSource::SendAudioSamples(short*pSamples,int nSamples,int nSampleRate)
{
	XAutoLock l(m_csListImageSink);
	for (XListPtr::iterator iter=m_ListImageSink.begin();
		iter!=m_ListImageSink.end();
		++iter)
	{
		VIDEC_ImageSink*pImageSink=(VIDEC_ImageSink*)*iter;
		pImageSink->OnReceivedAudioSamples(pSamples,nSamples,nSampleRate);
	}

	return 0;
}
