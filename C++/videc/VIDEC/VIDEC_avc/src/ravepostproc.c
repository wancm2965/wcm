#include "postprocess.h"

 
void rave_pp_init(int mmx,int mmx2,int _3dnow,int _3dnowExt,int sse,int sse2)
{
}

void ImageCopy(uint8_t*pSrc,stride_t nSrcStride,uint8_t*pDst,stride_t nDstStride,int nWidth,int nHeight)
{
	int i=0;
	for (i=0;i<nHeight;i++)
	{
		memcpy(pDst,pSrc,nWidth);
		pSrc+=nSrcStride;
		pDst+=nDstStride;
	}
}

void  rave_pp_process(uint8_t * src[3], stride_t srcStride[3],
                     uint8_t * dst[3], stride_t dstStride[3],
                     int horizontalSize, int verticalSize,
                     QP_STORE_T *QP_store,  int QP_stride,
                     pp_mode_t *mode, pp_context_t *ppContext, int pict_type)
{
	ImageCopy(src[0],srcStride[0],dst[0],dstStride[0],horizontalSize,verticalSize);
	ImageCopy(src[1],srcStride[1],dst[1],dstStride[1],horizontalSize>>1,verticalSize>>1);
	ImageCopy(src[2],srcStride[2],dst[2],dstStride[2],horizontalSize>>1,verticalSize>>1);
}


pp_context_t *rave_pp_open(int width, int height, int flags)
{
	return 1;
}

void rave_pp_close(pp_context_t *ppContext)
{
}
