#include "XOutputImage.h"


XOutputImage::XOutputImage(VIDEC_OutputImage*pOutputImage)
:m_pOutputImage(pOutputImage)
{

}

XOutputImage::~XOutputImage(void)
{

}

int XOutputImage::GetFrameRate(void)
{
	return m_pOutputImage->OnVIDEC_OutputImageGetFrameRate();
}

int XOutputImage::OutputImage(void*pData,int nLen,int nWidth,int nHeight)
{
	m_pOutputImage->OnVIDEC_OutputImageOutputData(pData,nLen,nWidth,nHeight);
	return 0;
}


XOutputImageMgr::XOutputImageMgr(void)
{

}

XOutputImageMgr::~XOutputImageMgr(void)
{

}

void XOutputImageMgr::Close(void)
{
	XAutoLock l(m_csMapXOutputImage);
	MapXOutputImage::iterator iter=m_MapXOutputImage.begin();
	while (iter!=m_MapXOutputImage.end())
	{
		XOutputImage*pXOutputImage=iter->second;
		++iter;

		delete pXOutputImage;
		pXOutputImage=NULL;
	}
	m_MapXOutputImage.clear();
}

int XOutputImageMgr::AddOutputImage(VIDEC_OutputImage*pOutputImage)
{
	XAutoLock l(m_csMapXOutputImage);
	MapXOutputImage::iterator iter=m_MapXOutputImage.find(pOutputImage);
	if (iter==m_MapXOutputImage.end())
	{
		XOutputImage*pXOutputImage=new XOutputImage(pOutputImage);
		m_MapXOutputImage[pOutputImage]=pXOutputImage;

		return 0;
	}

	return -1;
}

void XOutputImageMgr::RemoveOutputImage(VIDEC_OutputImage*pOutputImage)
{
	XAutoLock l(m_csMapXOutputImage);
	MapXOutputImage::iterator iter=m_MapXOutputImage.find(pOutputImage);
	if (iter!=m_MapXOutputImage.end())
	{
		XOutputImage*pXOutputImage=iter->second;
		m_MapXOutputImage.erase(iter);

		delete pXOutputImage;
		pXOutputImage=NULL;
	}
}

int XOutputImageMgr::OutputImage(void*pData,int nLen,int nWidth,int nHeight)
{
	XAutoLock l(m_csMapXOutputImage);
	MapXOutputImage::iterator iter=m_MapXOutputImage.begin();
	while (iter!=m_MapXOutputImage.end())
	{
		XOutputImage*pXOutputImage=iter->second;
		pXOutputImage->OutputImage(pData,nLen,nWidth,nHeight);
		++iter;
	}

	return 0;
}

int XOutputImageMgr::GetMaxFrameRate(void)
{
	int nMaxFrameRate=0;

	XAutoLock l(m_csMapXOutputImage);
	MapXOutputImage::iterator iter=m_MapXOutputImage.begin();
	while (iter!=m_MapXOutputImage.end())
	{
		XOutputImage*pXOutputImage=iter->second;
		if (pXOutputImage->GetFrameRate()>nMaxFrameRate)
		{
			nMaxFrameRate=pXOutputImage->GetFrameRate();
		}
		++iter;
	}

	return nMaxFrameRate;
}


int XOutputImageMgr::GetOutputImageCount(void)
{
	XAutoLock l(m_csMapXOutputImage);
	return m_MapXOutputImage.size();
}
