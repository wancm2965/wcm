#include "inttypes.h"
#include <winsock2.h>
#include <windows.h>
typedef int stride_t;

#include "VIDEC_convert.h"
#include "convert_yv12.h"
#include "convert_yuy2.h"
//
//
//int yv12_to_rgb24(unsigned char*src[],int src_stride[],unsigned char*dst,int dst_stride,int width,int height)
//{
//	unsigned char*src_yuy2=(unsigned char*)MALLOCMSG(width*height*2);
//	if (src_yuy2==NULL)
//		return -1;
//	int src_yuy2_stride=width*2;
//	TconvertYV12<Tsse2>::yuv_to_yuy2(src[0],src[1],src[2],width,src_stride[0],src_stride[1],src_yuy2,src_yuy2_stride,height);
//
//    Tmmx_ConvertYUY2toRGB<0,0>::mmx_ConvertYUY2toRGB(src_yuy2,dst,src_yuy2+height*src_yuy2_stride,src_yuy2_stride,dst_stride,width*2,0);
//
//	FREEMSG(src_yuy2);
//
//	return 0;
//}
//
//int rgb24_to_yv12(unsigned char*src,unsigned char*dst[],int width,int height)
//{
//	unsigned char*src_yuy2=(unsigned char*)MALLOCMSG(width*height*2);
//	if (src_yuy2==NULL)
//		return -1;
//
//	int src_stride=-width*3;
//	src+=width*height*3;
//	int src_yuy2_stride=width*2;
//    Tmmx_ConvertRGBtoYUY2<true ,false>::mmx_ConvertRGBtoYUY2(src,src_yuy2,src_stride,src_yuy2_stride,width,height,0);
//
//	int dst_stride[3];
//	dst_stride[0]=width;
// 	dst_stride[1]=width>>1;
//	dst_stride[2]=width>>1;
//	TconvertYV12<Tsse2>::yuy2_to_yv12(src_yuy2,src_yuy2_stride,src_yuy2_stride,dst[0],dst[2],dst[1],dst_stride[0],dst_stride[1],height);
//	
//	FREEMSG(src_yuy2);
//
//	return 0;
//}


int yuv_to_yuy2(unsigned char*src[],int src_stride[],unsigned char*dst,int dst_stride,int width,int height)
{
	TconvertYV12<Tsse2>::yv12_to_yuy2(src[0],src[1],src[2],width,src_stride[0],src_stride[1],dst,dst_stride,height);
	return 0;
}



#define R_SRC edx
#define R_DST edi
#define R_XOFFSET eax
#define R_YLEFT ebx
#define R_SRC_PITCH ecx
#define R_DST_PITCH esi

#ifdef _WIN64
extern "C" void __stdcall mmx_process_frame2field_asm(const BYTE* srcp, int src_pitch, int row_size, BYTE* dstp, int dst_pitch, int height);
#endif



void mmx_process_frame2field(const BYTE* srcp, int src_pitch, int row_size, BYTE* dstp, int dst_pitch, int height) {
  height--;
  static const __int64 add_2=0x0002000200020002;
  ///*
  if ((row_size&3)==0) {  // row width divideable with 4 (one dword per loop)

#ifdef _WIN64

	  mmx_process_frame2field_asm(srcp, src_pitch, row_size, dstp, dst_pitch, height);

#else

    __asm {
        push ebx  // avoid compiler bug
        movq mm7,[add_2];
        add [srcp],-4
        mov R_XOFFSET,0
        mov R_SRC,srcp
        mov R_DST,dstp
        mov R_SRC_PITCH,[src_pitch]
        mov R_DST_PITCH,[dst_pitch]
        mov R_YLEFT,[height]
loopback:
      pxor mm1,mm1
        punpckhbw mm0,[R_SRC]  // line0
        punpckhbw mm1,[R_SRC+R_SRC_PITCH]  // line1
        punpckhbw mm2,[R_SRC+R_SRC_PITCH*2]  // line2
        psrlw mm0,8
        psrlw mm1,7
        paddw mm0,mm7
        psrlw mm2,8
        paddw mm0,mm1
        paddw mm0,mm2
        psrlw mm0,2
        packuswb mm0,mm1
        movd [R_DST+R_XOFFSET],mm0
        add R_SRC,4
        add R_XOFFSET,4
        cmp  R_XOFFSET,[row_size]
        jl loopback						; Jump back
        add srcp, R_SRC_PITCH
        mov R_XOFFSET,0
        add srcp, R_SRC_PITCH
        add R_DST,R_DST_PITCH
        mov R_SRC,srcp
        dec R_YLEFT
        jnz loopback
        
        // last line 
loopback_last:
      pxor mm1,mm1
        punpckhbw mm0,[R_SRC]  // line0
        punpckhbw mm1,[R_SRC+R_SRC_PITCH]  // line1
        psrlw mm0,8
        movq mm2,mm1  // dupe line 1
        psrlw mm1,7
        paddw mm0,mm7
        psrlw mm2,8
        paddw mm0,mm1
        paddw mm0,mm2
        psrlw mm0,2
        packuswb mm0,mm1
        movd [R_DST+R_XOFFSET],mm0
        add R_XOFFSET,4
        add R_SRC,4
        cmp  R_XOFFSET,[row_size]
        jl loopback_last						; Jump back
        emms
        pop ebx
    }
#endif
  }
//*/
}

#undef R_SRC
#undef R_DST
#undef R_XOFFSET
#undef R_YLEFT
#undef R_SRC_PITCH
#undef R_DST_PITCH

void Frame2Field(unsigned char*pSrc,unsigned char*pDst,int nWidth,int nHeight)
{
	unsigned char*pSrcY=pSrc;
	unsigned char*pSrcU=pSrcY+nWidth*nHeight;
	unsigned char*pSrcV=pSrcU+((nWidth*nHeight)>>2);
		
	nHeight>>=1;
	unsigned char*pDstY=pDst;
	unsigned char*pDstU=pDstY+nWidth*nHeight;
	unsigned char*pDstV=pDstU+((nWidth*nHeight)>>2);

	mmx_process_frame2field(pSrcY,nWidth,nWidth,pDstY,nWidth,nHeight);
	mmx_process_frame2field(pSrcU,nWidth>>1,nWidth>>1,pDstU,nWidth>>1,nHeight>>1);
	mmx_process_frame2field(pSrcV,nWidth>>1,nWidth>>1,pDstV,nWidth>>1,nHeight>>1);
}


inline void WidthScaleFrame2Field(unsigned char*pSrc,unsigned char*pDst,int nWidth,int nHeight)
{
	int nHalfWidth=nWidth>>1;
	int nHalfHeight=nHeight>>1;
	unsigned char*pDstEven=pDst;
	unsigned char*pDstOdd=pDst+(nHalfWidth*nHalfHeight);
	for (int i=0;i<nHalfHeight;i++)
	{
		for (int j=0;j<nHalfWidth;j++)
		{
			*pDstEven++=*pSrc;
			pSrc+=2;
		}
		for (int k=0;k<nHalfWidth;k++)
		{
			*pDstOdd++=*pSrc;
			pSrc+=2;
		}
	}
}

//704X576->352X576
void HalfWidthScaleFrame2Field(unsigned char*pSrc,unsigned char*pDst,int nWidth,int nHeight)
{
	int nHalfWidth=nWidth>>1;
	int nHalfHeight=nHeight>>1;

	unsigned char*pSrcY=pSrc;
	unsigned char*pSrcU=pSrcY+nWidth*nHeight;
	unsigned char*pSrcV=pSrcU+nHalfWidth*nHalfHeight;
		
	unsigned char*pDstY=pDst;
	unsigned char*pDstU=pDstY+nHalfWidth*nHeight;
	unsigned char*pDstV=pDstU+(nHalfWidth*nHeight>>2);

	WidthScaleFrame2Field(pSrcY,pDstY,nWidth,nHeight);
	WidthScaleFrame2Field(pSrcU,pDstU,nHalfWidth,nHalfHeight);
	WidthScaleFrame2Field(pSrcV,pDstV,nHalfWidth,nHalfHeight);
}
