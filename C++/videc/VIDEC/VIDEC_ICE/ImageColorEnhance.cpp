#include "ImageColorEnhance.h"
#include "ICEWrapper.h"

ImageColorEnhance::ImageColorEnhance(void)
:m_pHandleICE(NULL)
,m_nWidth(0)
,m_nHeight(0)
{
}

ImageColorEnhance::~ImageColorEnhance(void)
{

}

int ImageColorEnhance::Open(void)
{
	return 0;
}
	
void ImageColorEnhance::Close(void)
{
	if (m_pHandleICE)
	{
		ICEClose(m_pHandleICE);
		m_pHandleICE=NULL;
	}
}
	
void ImageColorEnhance::Process(unsigned char*pY,unsigned char*pU,unsigned char*pV,int nWidth,int nHeight)
{
	if (nWidth!=m_nWidth || nHeight!=m_nHeight || m_pHandleICE==NULL)
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;

		if (m_pHandleICE)
		{
			ICEClose(m_pHandleICE);
			m_pHandleICE=NULL;
		}

		m_pHandleICE=ICEOpen(m_nWidth,m_nHeight);
	}

	if (m_pHandleICE)
	{
		ICEProcess(m_pHandleICE,pU,pV);
	}
}
