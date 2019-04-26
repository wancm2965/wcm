#include <winsock2.h>
#include <windows.h>

#include "XDecoderFrameHW264.h"
#include "XAutoLock.h"
#include "../MPVideo_API/XImage.h"
#include "XUtil.h"
#include <VIDEC/VIDEC_Header.h>
#include <emmintrin.h>


XDecoderFrame*CreateXDecoderFrameHW264(MPVideo_DecoderCallback&rCallback)
{
	return new XDecoderFrameHW264(rCallback);
}


XDecoderFrameHW264::XDecoderFrameHW264(MPVideo_DecoderCallback&rCallback)
:XDecoderFrame(rCallback)
,m_pVideoDecoder(NULL)
{
}

XDecoderFrameHW264::~XDecoderFrameHW264(void)
{
}

int XDecoderFrameHW264::Open(int nWidth,int nHeight)
{
	m_pVideoDecoder=HWVideoDecoder::Create(*this);
	if (m_pVideoDecoder==NULL)
	{
		return -1;
	}

	if (0!=m_pVideoDecoder->Open(nWidth,nHeight))
	{
		m_pVideoDecoder->Close();
		delete m_pVideoDecoder;
		m_pVideoDecoder=NULL;

		return -1;
	}


	return 0;
}

void XDecoderFrameHW264::Close(void)
{
	if (m_pVideoDecoder!=NULL)
	{
		m_pVideoDecoder->Close();
		delete m_pVideoDecoder;
		m_pVideoDecoder=NULL;
	}
}

int XDecoderFrameHW264::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
	if (m_pVideoDecoder!=NULL)
	{
		m_pVideoDecoder->Decode((unsigned char*)pPacketData,nPacketSize);
	}

	return 0;
}

int XDecoderFrameHW264::DecodeRTPPacket(void*pPacketData,int nPacketSize)
{
	return 0;
}

extern void convert_nv12_yv12(unsigned char*pSrcY,unsigned char*pSrcUV,int nSrcStride,unsigned char*dst,int dstStride,int width,int height);


void XDecoderFrameHW264::OnHWVideoDecoderCallback(unsigned char*pData,int nLen,int nWidth,int nHeight,int nStride)
{
	int nImageDataLen=nWidth*nHeight*3/2;
	unsigned char*pImageData=(unsigned char*)malloc(nImageDataLen+(nImageDataLen>>2));
	if (pImageData==NULL)
	{
		_aligned_free(pData);
		return ;
	}

	memcpy(pImageData,pData,nImageDataLen);

	//unsigned char*pImageY=pData;
	//unsigned char*pImageUV=pImageY+nStride*nHeight;
	//convert_nv12_yv12(pImageY,pImageUV,nStride,pImageData,nWidth,nWidth,nHeight);


	XImage*pXImage=new XImage(pImageData,nImageDataLen);
	pXImage->AddRef();

	pXImage->SetActualWidth(nWidth);
	pXImage->SetActualHeight(nHeight);
	pXImage->SetVirtualWidth(nWidth);
	pXImage->SetVirtualHeight(nHeight);
	pXImage->SetDoubleField(false);
	pXImage->SetEvenField(false);
	pXImage->SetOddField(false);
	pXImage->SetTimestamp(0);
	pXImage->SetStride(nWidth);

	m_rCallback.OnMPVideo_DecoderCallbackFrame(this,pXImage);

	pXImage->Release();
	pXImage=NULL;

	_aligned_free(pData);
}



// Load 16 8-bit pixels into a register, using aligned memory access
// reg   - register to store pixels in
// src   - memory pointer of the source
#define PIXCONV_LOAD_PIXEL8_ALIGNED(reg,src) \
	reg = _mm_load_si128((const __m128i *)(src));      /* load (aligned) */

#define PIXCONV_LOAD_PIXEL8_UNALIGNED(reg,src) \
	reg = _mm_loadu_si128((const __m128i *)(src));      /* load (unaligned) */

// SSE2 Aligned memcpy
// dst - memory destination
// src - memory source
// len - size in bytes
#define PIXCONV_MEMCPY_ALIGNED(dst,src,len)     \
{                                             \
	__m128i reg;                                \
	__m128i *dst128 =  (__m128i *)(dst);        \
	for (int i = 0; i < len; i+=16) {           \
	PIXCONV_LOAD_PIXEL8_ALIGNED(reg,(src)+i); \
	_mm_stream_si128(dst128++, reg);          \
	}                                           \
}



void convert_nv12_yv12(unsigned char*pSrcY,unsigned char*pSrcUV,int nSrcStride,unsigned char*dst,int dstStride,int width,int height)
{
	const unsigned char *y  = pSrcY;
	const unsigned char *uv = pSrcUV;

	const int inStride = nSrcStride;
	const int outLumaStride = dstStride;
	const int outChromaStride = dstStride >> 1;

	const int chromaHeight = height >> 1;
	const int chromaWidth = width >>1;

	unsigned char *dstY = dst;
	unsigned char *dstU = dstY + height * outLumaStride;
	unsigned char *dstV = dstU + chromaHeight * outChromaStride;

	int line, i;
	__m128i xmm0,xmm1,xmm2,xmm3,xmm7;

	xmm7 = _mm_set1_epi16(0x00FF);

	_mm_sfence();

	// Copy the y
	if( (outLumaStride % 16) )
	{
		for( i = 0; i < height; i++ )
		{
			memcpy( dstY, y, width );
			y += inStride ;
			dstY += outLumaStride;
		}
	}
	else
	{
		for (line = 0; line < height; line++) {
			PIXCONV_MEMCPY_ALIGNED(dstY + outLumaStride * line, y, width);
			y += inStride;
		}
	}

	if( (outChromaStride % 16) )
	{

		for (line = 0; line < chromaHeight; line++) 
		{
			__m128i *dstV128 = (__m128i *)(dstV + outChromaStride * line);
			__m128i *dstU128 = (__m128i *)(dstU + outChromaStride * line);

			for (i = 0; i < width; i+=32) {
				PIXCONV_LOAD_PIXEL8_UNALIGNED(xmm0, uv+i+0);
				PIXCONV_LOAD_PIXEL8_UNALIGNED(xmm1, uv+i+16);
				xmm2 = xmm0;
				xmm3 = xmm1;

				// null out the high-order bytes to get the U values
				xmm0 = _mm_and_si128(xmm0, xmm7);
				xmm1 = _mm_and_si128(xmm1, xmm7);
				// right shift the V values
				xmm2 = _mm_srli_epi16(xmm2, 8);
				xmm3 = _mm_srli_epi16(xmm3, 8);
				// unpack the values
				xmm0 = _mm_packus_epi16(xmm0, xmm1);
				xmm2 = _mm_packus_epi16(xmm2, xmm3);

				_mm_storeu_si128( dstU128++, xmm0 );
				_mm_storeu_si128( dstV128++, xmm2 );
			}
			uv += inStride;
		}
	}
	else
	{
		for (line = 0; line < chromaHeight; line++) {
			__m128i *dstV128 = (__m128i *)(dstV + outChromaStride * line);
			__m128i *dstU128 = (__m128i *)(dstU + outChromaStride * line);

			for (i = 0; i < width; i+=32) {
				PIXCONV_LOAD_PIXEL8_ALIGNED(xmm0, uv+i+0);
				PIXCONV_LOAD_PIXEL8_ALIGNED(xmm1, uv+i+16);
				xmm2 = xmm0;
				xmm3 = xmm1;

				// null out the high-order bytes to get the U values
				xmm0 = _mm_and_si128(xmm0, xmm7);
				xmm1 = _mm_and_si128(xmm1, xmm7);
				// right shift the V values
				xmm2 = _mm_srli_epi16(xmm2, 8);
				xmm3 = _mm_srli_epi16(xmm3, 8);
				// unpack the values
				xmm0 = _mm_packus_epi16(xmm0, xmm1);
				xmm2 = _mm_packus_epi16(xmm2, xmm3);

				_mm_stream_si128(dstU128++, xmm0);
				_mm_stream_si128(dstV128++, xmm2);
			}
			uv += inStride;
		}
	}
}