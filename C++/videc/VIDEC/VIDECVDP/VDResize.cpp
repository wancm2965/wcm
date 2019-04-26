#include "stdafx.h"
#include "VDResize.h"

#include "./VirtualDub/h/vd2/Kasumi/resample.h"
#include "./VirtualDub\h\vd2\Kasumi\pixmap.h"


class VDResize
{
public:
	VDResize(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride);
	virtual~VDResize(void);
public:
	int Resize(unsigned char*srcData[], unsigned char*dstData[]);
protected:
	IVDPixmapResampler *m_pVDPixmapResampler;
	int					m_nResizeType;
	int					m_nSrcWidth;
	int					m_nSrcHeight;
	int					m_nSrcStride;
	int					m_nDstWidth;
	int					m_nDstHeight;
	int					m_nDstStride;
};

VDResize::VDResize(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride)
	:m_pVDPixmapResampler(NULL)
{
	m_nSrcWidth = nSrcWidth;
	m_nSrcHeight = nSrcHeight;
	m_nSrcStride = nSrcStride;
	m_nDstWidth = nDstWidth;
	m_nDstHeight = nDstHeight;
	m_nDstStride = nDstStride;
	m_nResizeType = GetPrivateProfileInt(("VIDEC"), ("ResizeType"), 1, "c:/Windows/VIDEC.ini");
	if (m_nResizeType < 1)
	{
		m_nResizeType = 1;
	}
	m_pVDPixmapResampler = VDCreatePixmapResampler();
	IVDPixmapResampler::FilterMode filter;
	filter = IVDPixmapResampler::FilterMode(m_nResizeType);
	m_pVDPixmapResampler->SetFilters(filter, filter, false);
	m_pVDPixmapResampler->Init(nDstWidth, nDstHeight, nsVDPixmap::kPixFormat_YUV420_Planar, nSrcWidth, nSrcHeight, nsVDPixmap::kPixFormat_YUV420_Planar);
}

VDResize::~VDResize(void)
{
	if (m_pVDPixmapResampler)
	{
		delete m_pVDPixmapResampler;
		m_pVDPixmapResampler = NULL;
	}
}

int VDResize::Resize(unsigned char*srcData[], unsigned char*dstData[])
{
	if (m_pVDPixmapResampler)
	{
		VDPixmap srcbm = { 0 };

		srcbm.data = srcData[0];
		srcbm.pitch = m_nSrcStride;
		srcbm.w = m_nSrcWidth;
		srcbm.h = m_nSrcHeight;
		srcbm.format = nsVDPixmap::kPixFormat_YUV420_Planar;
		srcbm.data2 = srcData[1];
		srcbm.pitch2 = m_nSrcStride / 2;
		srcbm.data3 = srcData[2];
		srcbm.pitch3 = m_nSrcStride / 2;

		VDPixmap dstpxm = { 0 };

		dstpxm.data = dstData[0];
		dstpxm.pitch = m_nDstStride;
		dstpxm.w = m_nDstWidth;
		dstpxm.h = m_nDstHeight;
		dstpxm.format = nsVDPixmap::kPixFormat_YUV420_Planar;
		dstpxm.data2 = dstData[1];
		dstpxm.pitch2 = m_nDstStride / 2;
		dstpxm.data3 = dstData[2];
		dstpxm.pitch3 = m_nDstStride / 2;

		m_pVDPixmapResampler->Process(dstpxm, srcbm);
	}
	return 0;
}



void*VDResize_Open(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride)
{
	VDResize* pVDResize = new VDResize(nSrcWidth, nSrcHeight, nSrcStride, nDstWidth, nDstHeight, nDstStride);
	return pVDResize;
}
void VDResize_GetFrame(void*hVDResize, unsigned char*srcData[], unsigned char*dstData[])
{
	VDResize* pVDResize = (VDResize*)hVDResize;
	if (pVDResize)
	{
		pVDResize->Resize(srcData, dstData);
	}
}
void VDResize_Close(void*hVDResize)
{
	VDResize* pVDResize = (VDResize*)hVDResize;
	if (pVDResize)
	{
		delete pVDResize;
		pVDResize = NULL;
	}
}

bool VDResample_GetFrame(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride, unsigned char*srcData[], unsigned char*dstData[])
{
	VDPixmap srcbm = { 0 };

	srcbm.data = srcData[0];
	srcbm.pitch = nSrcStride;
	srcbm.w = nSrcWidth;
	srcbm.h = nSrcHeight;
	srcbm.format = nsVDPixmap::kPixFormat_YUV420_Planar;
	srcbm.data2 = srcData[1];
	srcbm.pitch2 = nSrcStride / 2;
	srcbm.data3 = srcData[2];
	srcbm.pitch3 = nSrcStride / 2;

	VDPixmap dstpxm = { 0 };

	dstpxm.data = dstData[0];
	dstpxm.pitch = nDstStride;
	dstpxm.w = nDstWidth;
	dstpxm.h = nDstHeight;
	dstpxm.format = nsVDPixmap::kPixFormat_YUV420_Planar;
	dstpxm.data2 = dstData[1];
	dstpxm.pitch2 = nDstStride / 2;
	dstpxm.data3 = dstData[2];
	dstpxm.pitch3 = nDstStride / 2;

	return VDPixmapResample(dstpxm, srcbm, IVDPixmapResampler::kFilterLinear);
}