#include "XUtil.h"
#include "XImage.h"
#include "../MPVideo_csp/fastmemcpy.h"

MPVideo_Image*MPVideo_Image::Create(unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate)
{
	if (pData==NULL || nLen<=0)
		return NULL;

	unsigned char*pImageData=(unsigned char*)malloc(nLen+(nLen>>2));
	if (pImageData!=NULL)
	{
		fastmemcpy(pImageData,pData,nLen);
		XImage*pXImage=new XImage(pImageData,nLen);
		pXImage->AddRef();

		pXImage->SetActualWidth(nWidth);
		pXImage->SetActualHeight(nHeight);
		pXImage->SetVirtualWidth(nWidth);
		pXImage->SetVirtualHeight(nHeight);
		pXImage->SetDoubleField(false);
		pXImage->SetEvenField(false);
		pXImage->SetOddField(false);
		pXImage->SetHardware(false);
		pXImage->SetTimestamp(XGetTimestamp());
		pXImage->SetFrameRate(nFrameRate);

		return pXImage;
	}
	return NULL;
}

XImage::XImage(void*pData,int nLen,int nBufOffset)
: m_cRef(0)
, m_nActualWidth(0)				//真实的图片大小
, m_nActualHeight(0)			//真实的图片大小
, m_nVirtualWidth(0)			//要显示的图片大小
, m_nVirtualHeight(0)			//要显示的图片大小
, m_bEvenFieldFirst(true)
, m_bDoubleField(false)
, m_bEvenField(false)
, m_bOddField(false)
, m_bHardware(false)
, m_ulTimestamp(0)				//采集到的时间
, m_ulTimestampOffset(0)				//采集到的时间

, m_nFrameRate(30)
, m_bVideoSignal(true)

, m_pData(pData)
, m_nDataLen(nLen)
, m_nBufOffset(nBufOffset)
{

	m_pBuf=(unsigned char*)m_pData-m_nBufOffset;
	m_nBufLen=m_nBufOffset+m_nDataLen;

	m_nStride=0;
	m_bNV12=false;
}

XImage::~XImage(void)
{
	if (m_pBuf!=NULL)
	{
		if (m_bNV12)
		{
			_aligned_free(m_pBuf);
		}
		else
		{
			free(m_pBuf);
		}
		m_pBuf=NULL;
	}
}

void XImage::AddRef(void)
{
    LONG lRef = InterlockedIncrement( &m_cRef );
}

void XImage::Release(void)
{
    LONG lRef = InterlockedDecrement( &m_cRef );
    if (lRef == 0) 
	{
        m_cRef++;

        delete this;
	}
}

int XImage::GetWidth(void)
{
	return m_nActualWidth;
}

int XImage::GetActualWidth(void)
{
	return m_nActualWidth;
}

void XImage::SetActualWidth(int nActualWidth)
{
	m_nActualWidth=nActualWidth;
}

int XImage::GetHeight(void)
{
	return m_nActualHeight;
}

int XImage::GetActualHeight(void)
{
	return m_nActualHeight;
}

void XImage::SetActualHeight(int nActualHeight)
{
	m_nActualHeight=nActualHeight;
}

int XImage::GetVirtualWidth(void)
{
	return m_nVirtualWidth;
}

void XImage::SetVirtualWidth(int nVirtualWidth)
{
	m_nVirtualWidth=nVirtualWidth;
}

int XImage::GetVirtualHeight(void)
{
	return m_nVirtualHeight;
}

void XImage::SetVirtualHeight(int nVirtualHeight)
{
	m_nVirtualHeight=nVirtualHeight;
}

bool XImage::GetEvenFieldFirst(void)
{
	return m_bEvenFieldFirst;
}

void XImage::SetEvenFieldFirst(bool bEvenFieldFirst)
{
	m_bEvenFieldFirst=bEvenFieldFirst;
}

bool XImage::GetDoubleField(void)
{
	return m_bDoubleField;
}

void XImage::SetDoubleField(bool bDoubleField)
{
	m_bDoubleField=bDoubleField;
}

bool XImage::GetEvenField(void)
{
	return m_bEvenField;
}

void XImage::SetEvenField(bool bEvenField)
{
	m_bEvenField=bEvenField;
}

bool XImage::GetOddField(void)
{
	return m_bOddField;
}

void XImage::SetOddField(bool bOddField)
{
	m_bOddField=bOddField;
}

bool XImage::GetHardware(void)
{
	return m_bHardware;
}

void XImage::SetHardware(bool bHardware)
{
	m_bHardware=bHardware;
}

unsigned long XImage::GetTimestamp(void)
{
	return m_ulTimestamp;
}

void XImage::SetTimestamp(unsigned long ulTimestamp)
{
	m_ulTimestamp=ulTimestamp;
}

unsigned long XImage::GetTimestampOffset(void)
{
	return m_ulTimestampOffset;
}

void XImage::SetTimestampOffset(unsigned long ulTimestampOffset)
{
	m_ulTimestampOffset=ulTimestampOffset;
}

int XImage::GetFrameRate(void)
{
	return m_nFrameRate;
}

void XImage::SetFrameRate(int nFrameRate)
{
	m_nFrameRate=nFrameRate;
}

void XImage::SetVideoSignal(bool bVideoSignal)
{
	m_bVideoSignal=bVideoSignal;
}

bool XImage::GetVideoSignal(void)
{
	return m_bVideoSignal;
}


void*XImage::GetData(void)
{
	return m_pData;
}

int XImage::GetDataLen(void)
{
	return m_nDataLen;
}

void XImage::SetDataLen(int nDataLen)
{
	m_nDataLen=nDataLen;
}


void XImage::SetStride(int nStride)
{
	m_nStride=nStride;
}

int XImage::GetStride(void)
{
	return m_nStride;
}

void XImage::SetNV12(bool bNV12)
{
	m_bNV12=bNV12;
}

bool XImage::GetNV12(void)
{
	return m_bNV12;
}

