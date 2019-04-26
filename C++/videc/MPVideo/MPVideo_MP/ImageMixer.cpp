#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ImageMixer.h"
#include "../MPVIDEO_csp/fastmemcpy.h"
//#include <Windows.h>
#include "../MPVideo_convert/VDPWrapper.h"

void CopyImageRect(int nSrcWidth,int nSrcHeight,int nPointBytes,unsigned char*pSrc,int nFromLeft,int nFromTop,int nDstWidth,int nDstHeight,unsigned char*pDst)
{
	int nSrcLineBytes=nPointBytes*nSrcWidth;
	int nDstLineBytes=nPointBytes*nDstWidth;
	
	unsigned char*pSrc1=pSrc+nSrcLineBytes*nFromTop+nPointBytes*nFromLeft;
	unsigned char*pDst1=pDst;

	for (int i=0;i<nDstHeight;i++)
	{
		fastmemcpy(pDst1,pSrc1,nDstLineBytes);
		pSrc1+=nSrcLineBytes;
		pDst1+=nDstLineBytes;
	}
}

void ScaleLine(unsigned char*pSrcBuffer,unsigned char*pDstBuffer,unsigned int nSrcWidth,unsigned int nDstWidth)
{
	unsigned int nNumPixels		= nDstWidth;
	unsigned int nIntactPart	= nSrcWidth / nDstWidth;
	unsigned int nFractionPart	= nSrcWidth % nDstWidth;
	unsigned int i				= 0;

	while (nNumPixels-- > 0) 
	{
		*pDstBuffer++ = *pSrcBuffer;
		pSrcBuffer += nIntactPart;
		i += nFractionPart;
		if (i >= nDstWidth) 
		{
			i -= nDstWidth;
			pSrcBuffer++;
		}
	}
}

void ScaleRect(unsigned char*pSrcBuffer,unsigned int nSrcWidth,unsigned int nSrcHeight,unsigned char*pImageBuffer,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int nDstLeft,unsigned int nDstTop,unsigned int nDstWidth,unsigned int nDstHeight)
{
	unsigned int nNumRows			= nDstHeight;
	unsigned int nIntactPart		= (nSrcHeight / nDstHeight) * nSrcWidth;
	unsigned int nFractionPart		=  nSrcHeight % nDstHeight;
	unsigned int i					= 0;
	unsigned char* pPrevSrcBuffer	= NULL;
	unsigned char* pDstBuffer		= pImageBuffer + nImageWidth * nDstTop + nDstLeft;

	while (nNumRows-- > 0) 
	{
		if (pSrcBuffer == pPrevSrcBuffer) 
		{
			fastmemcpy(pDstBuffer, pDstBuffer - nImageWidth, nDstWidth);
		} 
		else 
		{
			ScaleLine(pSrcBuffer, pDstBuffer, nSrcWidth, nDstWidth);
			pPrevSrcBuffer = pSrcBuffer;
		}

		pDstBuffer += nImageWidth;
		pSrcBuffer += nIntactPart;
		i += nFractionPart;
		if (i >= nDstHeight) 
		{
			i -= nDstHeight;
			pSrcBuffer += nSrcWidth;
		}
	}
}




ImageMixer::ImageMixer(void)
{
	m_pImageBuffer	= NULL;
	m_nImageWidth	= 704;
	m_nImageHeight	= 576;
	m_nImageSize	= m_nImageWidth*m_nImageHeight*3/2;

	m_bVDResample=true;

}

ImageMixer::~ImageMixer(void)
{
	if (m_pImageBuffer)
	{
		free(m_pImageBuffer);
		m_pImageBuffer=NULL;
	}
}

void ImageMixer::SetImageSize(int nImageWidth,int nImageHeight)
{	
	if (m_nImageWidth != nImageWidth || m_nImageHeight	!= nImageHeight)
	{
		if (m_pImageBuffer!=NULL)
		{
			free(m_pImageBuffer);
			m_pImageBuffer=NULL;
		}

		m_nImageWidth	= nImageWidth;
		m_nImageHeight	= nImageHeight;
		m_nImageSize	= m_nImageWidth*m_nImageHeight*3/2;
		if (m_nImageSize>0)
		{
			m_pImageBuffer=(unsigned char*)malloc(m_nImageSize+(m_nImageSize>>2));
		}
	}
}


unsigned int ImageMixer::GetImageWidth(void)
{
	return m_nImageWidth;
}

unsigned int ImageMixer::GetImageHeight(void)
{
	return m_nImageHeight;
}

unsigned char*ImageMixer::GetImageData(void)
{
	return m_pImageBuffer;
}

unsigned int ImageMixer::GetImageSize(void)
{
	return m_nImageSize;
}

bool ImageMixer::MixImage(unsigned char*pToImageData, int nToWidth, int nToHeight, unsigned char*pImageData,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int x,unsigned int y,unsigned int cx,unsigned int cy)
{
	if (cx>0 
		&& cy>0
		&& (nToWidth>=(x+cx))
		&& (nToHeight>=(y+cy)))
	{
		unsigned int nSrcYBufferSize = nImageWidth * nImageHeight;
		unsigned int nSrcUVBufferSize = nSrcYBufferSize >> 2;

		unsigned char*pSrcY = pImageData;
		unsigned char*pSrcU = pSrcY + nSrcYBufferSize;
		unsigned char*pSrcV = pSrcU + nSrcUVBufferSize;

		unsigned int nDstYBufferSize = nToWidth*nToHeight;
		unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;

		unsigned char*pDstY = pToImageData;
		unsigned char*pDstU = pDstY + nDstYBufferSize;
		unsigned char*pDstV = pDstU + nDstUVBufferSize;

		ScaleRect(pSrcY, nImageWidth,    nImageHeight,    pDstY, nToWidth,    nToHeight,    x,    y,    cx,    cy);
		ScaleRect(pSrcU, nImageWidth>>1, nImageHeight>>1, pDstU, nToWidth>>1, nToHeight>>1, x>>1, y>>1, cx>>1, cy>>1);
		ScaleRect(pSrcV, nImageWidth>>1, nImageHeight>>1, pDstV, nToWidth>>1, nToHeight>>1, x>>1, y>>1, cx>>1, cy>>1);

		/////////////////
		return true;
	}
	else
	{
		return false;
	}
}

bool ImageMixer::MixImage(unsigned char*pImageData,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int x,unsigned int y,unsigned int cx,unsigned int cy)
{
	if (cx>0 
		&& cy>0
		&& (m_nImageWidth>=(x+cx))
		&& (m_nImageHeight>=(y+cy)))
	{		
		unsigned int nSrcYBufferSize = nImageWidth * nImageHeight;
		unsigned int nSrcUVBufferSize = nSrcYBufferSize >> 2;

		//char msg2[128]={0};
		//sprintf(msg2,"MixImage(pImageData:%p, nImageWidth:%u,nImageHeight:%u,x:%u, y:%u, cx:%u, cy:%u)\n",pImageData, nImageWidth,nImageHeight,x, y, cx, cy);
		//OutputDebugString(msg2);

		if (!m_bVDResample)
		{
			unsigned char*pSrcY = pImageData;
			unsigned char*pSrcU = pSrcY + nSrcYBufferSize;
			unsigned char*pSrcV = pSrcU + nSrcUVBufferSize;

			unsigned int nDstYBufferSize = m_nImageWidth*m_nImageHeight;
			unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;

			unsigned char*pDstY = m_pImageBuffer;
			unsigned char*pDstU = pDstY + nDstYBufferSize;
			unsigned char*pDstV = pDstU + nDstUVBufferSize;

			ScaleRect(pSrcY, nImageWidth,    nImageHeight,    pDstY, m_nImageWidth,    m_nImageHeight,    x,    y,    cx,    cy);
			ScaleRect(pSrcU, nImageWidth>>1, nImageHeight>>1, pDstU, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1);
			ScaleRect(pSrcV, nImageWidth>>1, nImageHeight>>1, pDstV, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1);
		}
		else
		{
			unsigned char*pSrcY[3];
			unsigned char*pDstY[3];

			pSrcY[0] = pImageData;
			pSrcY[1] = pSrcY[0] + nSrcYBufferSize;
			pSrcY[2] = pSrcY[1] + nSrcUVBufferSize;

			unsigned int nDstYBufferSize = m_nImageWidth*m_nImageHeight;
			unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;
			int nYOffset = y*m_nImageWidth + x;
			int nUVOffset= y*m_nImageWidth / 4 + x / 2;

			pDstY[0] = m_pImageBuffer + nYOffset;
			pDstY[1] = m_pImageBuffer + nDstYBufferSize + nUVOffset;
			pDstY[2] = m_pImageBuffer + nDstYBufferSize + nDstUVBufferSize + nUVOffset;
			m_bVDResample = VDResampleGetFrame(nImageWidth, nImageHeight, nImageWidth, cx, cy, m_nImageWidth, pSrcY, pDstY);
		}
		/////////////////
		return true;
	}
	else
	{
		return false;
	}
}

static int nR = 166;
static int nG = 182;
static int nB = 255;

static int nY = 0.299*nR + 0.587*nG + 0.114*nB;
static int nU = 0.5 * nB + 128 - 0.1687 * nR - 0.3313 * nG; 
static int nV = 0.5 * nR - 0.4187 * nG - 0.0813 * nB + 128;


bool ImageMixer::FillRect(unsigned int x,unsigned int y,unsigned int cx,unsigned int cy)
{
	if (cx>0 
		&& cy>0
		&& (m_nImageWidth>=(x+cx))
		&& (m_nImageHeight>=(y+cy)))
	{
		unsigned int nDstYBufferSize = m_nImageWidth*m_nImageHeight;
		unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;

		unsigned char*pDstY = m_pImageBuffer;
		unsigned char*pDstU = pDstY + nDstYBufferSize;
		unsigned char*pDstV = pDstU + nDstUVBufferSize;

 	//	FillRect(pDstY, m_nImageWidth,    m_nImageHeight,    x,    y,    cx,    cy,    0);
		//FillRect(pDstU, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, 127);
		//FillRect(pDstV, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, 127);
		FillRect(pDstY, m_nImageWidth,    m_nImageHeight,    x,    y,    cx,    cy,    nY);
		FillRect(pDstU, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, nU);
		FillRect(pDstV, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, nV);

		return true;
	}
	else
	{
		return false;
	}
}

void ImageMixer::FillRect(unsigned char*pImageBuffer,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int nDstLeft,unsigned int nDstTop,unsigned int nDstWidth,unsigned int nDstHeight, unsigned char ucFilledValue)
{
	unsigned int nNumRows			= nDstHeight;
	unsigned char* pDstBuffer		= pImageBuffer + nImageWidth * nDstTop + nDstLeft;

	while (nNumRows-- > 0) 
	{
		memset(pDstBuffer, ucFilledValue, nDstWidth);
		pDstBuffer += nImageWidth;
	}
}

bool ImageMixer::DrawRect(unsigned int x,unsigned int y,unsigned int cx,unsigned int cy)
{
	if (cx>0 
		&& cy>0
		&& (m_nImageWidth>=(x+cx))
		&& (m_nImageHeight>=(y+cy)))
	{
		unsigned int nDstYBufferSize = m_nImageWidth*m_nImageHeight;
		unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;

		unsigned char*pDstY = m_pImageBuffer;
		unsigned char*pDstU = pDstY + nDstYBufferSize;
		unsigned char*pDstV = pDstU + nDstUVBufferSize;

 		DrawRect(pDstY, m_nImageWidth,    m_nImageHeight,    x,    y,    cx,    cy,    0);
		DrawRect(pDstU, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, 127);
		DrawRect(pDstV, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, 127);

		return true;
	}
	else
	{
		return false;
	}
}

void ImageMixer::DrawRect(unsigned char*pImageBuffer,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int nDstLeft,unsigned int nDstTop,unsigned int nDstWidth,unsigned int nDstHeight, unsigned char ucFilledValue)
{
	unsigned int nNumRows			= nDstHeight;
	unsigned char* pDstBuffer		= pImageBuffer + nImageWidth * nDstTop + nDstLeft;

	memset(pDstBuffer, ucFilledValue, nDstWidth);
	pDstBuffer += nImageWidth;
	nNumRows--;
	while (nNumRows-- > 1) 
	{
		*pDstBuffer=ucFilledValue;
		*(pDstBuffer+nDstWidth-1)=ucFilledValue;
		pDstBuffer += nImageWidth;
	}
	memset(pDstBuffer, ucFilledValue, nDstWidth);
}
