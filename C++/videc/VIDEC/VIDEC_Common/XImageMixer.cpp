#include <stdlib.h>
#include <memory.h>

#include "XImageMixer.h"


#define CalcY(Red, Green, Blue)     (unsigned char)((( 77*Red ) + (151*Green) + (28*Blue )) >>8)
#define CalcU(Red, Green, Blue)     (unsigned char)((((128*Blue) - ( 43*Red  ) - (84*Green)) >>8) + 128)
#define CalcV(Red, Green, Blue)     (unsigned char)((((128*Red ) - (107*Green) - (20*Blue )) >>8) + 128)

#define CalcR(Y, U, V)              (unsigned char)abs(((256*Y) + (358*(V-128))) >>8)
#define CalcG(Y, U, V)              (unsigned char)abs(((256*Y) - ( 87*(U-128)) - (182*(V-128))) >>8)
#define CalcB(Y, U, V)              (unsigned char)abs(((256*Y) + (451*(U-128))) >>8)


#define MixAlpha(Src, Dest, Alpha)	(unsigned char)((((Dest - Src) * Alpha) >> 8) + Src)


void CopyImageRect(int nSrcWidth,int nSrcHeight,int nPointBytes,unsigned char*pSrc,int nFromLeft,int nFromTop,int nDstWidth,int nDstHeight,unsigned char*pDst)
{
	int nSrcLineBytes=nPointBytes*nSrcWidth;
	int nDstLineBytes=nPointBytes*nDstWidth;
	
	unsigned char*pSrc1=pSrc+nSrcLineBytes*nFromTop+nPointBytes*nFromLeft;
	unsigned char*pDst1=pDst;

	for (int i=0;i<nDstHeight;i++)
	{
		memcpy(pDst1,pSrc1,nDstLineBytes);
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
			memcpy(pDstBuffer, pDstBuffer - nImageWidth, nDstWidth);
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




XImageMixer::XImageMixer(void)
{
	m_pImageBuffer	= NULL;
	m_nImageWidth	= 352;
	m_nImageHeight	= 288;
	m_nImageSize	= m_nImageWidth*m_nImageHeight*3/2;
}

XImageMixer::~XImageMixer(void)
{

}

int XImageMixer::Open(int nWidth,int nHeight)
{	
	m_nImageWidth	= nWidth;
	m_nImageHeight	= nHeight;
	m_nImageSize	= m_nImageWidth*m_nImageHeight*3/2;
	m_pImageBuffer	= (unsigned char*)malloc(m_nImageSize);
	if (m_pImageBuffer==NULL)
	{
		return -1;
	}

	return 0;
}

void XImageMixer::Close(void)
{
	if (m_pImageBuffer)
	{
		free(m_pImageBuffer);
		m_pImageBuffer=NULL;
	}
}

unsigned int XImageMixer::GetImageWidth(void)
{
	return m_nImageWidth;
}

unsigned int XImageMixer::GetImageHeight(void)
{
	return m_nImageHeight;
}

unsigned char*XImageMixer::GetImageData(void)
{
	return m_pImageBuffer;
}

unsigned int XImageMixer::GetImageSize(void)
{
	return m_nImageSize;
}

bool XImageMixer::MixImage(unsigned char*pToImageData, int nToWidth, int nToHeight, unsigned char*pImageData,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int x,unsigned int y,unsigned int cx,unsigned int cy)
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

bool XImageMixer::MixImage(unsigned char*pImageData,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int x,unsigned int y,unsigned int cx,unsigned int cy)
{
	if (cx>0 
		&& cy>0
		&& (m_nImageWidth>=(x+cx))
		&& (m_nImageHeight>=(y+cy)))
	{
		unsigned int nSrcYBufferSize = nImageWidth * nImageHeight;
		unsigned int nSrcUVBufferSize = nSrcYBufferSize >> 2;

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

		/////////////////
		return true;
	}
	else
	{
		return false;
	}
}

bool XImageMixer::FillRect(unsigned int x,unsigned int y,unsigned int cx,unsigned int cy,unsigned char ucRed,unsigned char ucGreen,unsigned char ucBlue)
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

 		FillRect(pDstY, m_nImageWidth,    m_nImageHeight,    x,    y,    cx,    cy,    CalcY(ucRed, ucGreen, ucBlue));
		FillRect(pDstU, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, CalcU(ucRed, ucGreen, ucBlue));
		FillRect(pDstV, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, CalcV(ucRed, ucGreen, ucBlue));

		return true;
	}
	else
	{
		return false;
	}
}

void XImageMixer::FillRect(unsigned char*pImageBuffer,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int nDstLeft,unsigned int nDstTop,unsigned int nDstWidth,unsigned int nDstHeight, unsigned char ucFilledValue)
{
	unsigned int nNumRows			= nDstHeight;
	unsigned char* pDstBuffer		= pImageBuffer + nImageWidth * nDstTop + nDstLeft;

	while (nNumRows-- > 0) 
	{
		memset(pDstBuffer, ucFilledValue, nDstWidth);
		pDstBuffer += nImageWidth;
	}
}

bool XImageMixer::DrawRect(unsigned int x,unsigned int y,unsigned int cx,unsigned int cy,unsigned char ucRed,unsigned char ucGreen,unsigned char ucBlue)
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

 		DrawRect(pDstY, m_nImageWidth,    m_nImageHeight,    x,    y,    cx,    cy,    CalcY(ucRed, ucGreen, ucBlue));
		DrawRect(pDstU, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, CalcU(ucRed, ucGreen, ucBlue));
		DrawRect(pDstV, m_nImageWidth>>1, m_nImageHeight>>1, x>>1, y>>1, cx>>1, cy>>1, CalcV(ucRed, ucGreen, ucBlue));

		return true;
	}
	else
	{
		return false;
	}
}

void XImageMixer::DrawRect(unsigned char*pImageBuffer,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int nDstLeft,unsigned int nDstTop,unsigned int nDstWidth,unsigned int nDstHeight, unsigned char ucFilledValue)
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
