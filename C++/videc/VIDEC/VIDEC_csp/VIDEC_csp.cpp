// VIDEC_csp.cpp: implementation of the VIDEC_csp class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "VIDEC_csp.h"
#include "colorspace.h"
#include "fastmemcpy.h"
#include <memory.h>

#ifdef _WIN64
#include "apex_memmove.h"
#endif

extern "C"
{
	// cpu_flag detection helper functions 
	extern int check_cpu_features(void);
	extern void sse_os_trigger(void);
	extern void sse2_os_trigger(void);

#ifndef _WIN64
	FASTMEMCPY memcpy_x86;
	FASTMEMCPY memcpy_mmx;
	FASTMEMCPY memcpy_xmm;
	FASTMEMCPY memcpy_sse;
#endif

}

FASTMEMCPY* fastmemcpy;

class XColorspace
{
public:
	XColorspace(void)
	{
		int cpu_flags=check_cpu_features();
		xvid_colorspace_init(cpu_flags);

#ifdef _WIN64

		fastmemcpy = apex_memcpy;

#else 

		if      (cpu_flags&FF_CPU_SSE)		fastmemcpy=memcpy_sse;
		else if (cpu_flags&FF_CPU_MMXEXT)	fastmemcpy=memcpy_xmm;
		else if (cpu_flags&FF_CPU_MMX)		fastmemcpy=memcpy_mmx;
		else									fastmemcpy=memcpy_x86;
											fastmemcpy=memcpy;
#endif

	}
	virtual~XColorspace(void)
	{
	}
};

static XColorspace g_XColorspace;



int image_input(IMAGE * image,
				uint32_t width,
				int height,
				uint32_t edged_width,
				uint8_t * src[4],
				int src_stride[4],
				int csp,
				int interlaced);

int image_output(IMAGE * image,
				 uint32_t width,
				 int height,
				 uint32_t edged_width,
				 uint8_t * dst[4],
				 int dst_stride[4],
				 int csp,
				 int interlaced);

int avcon_convert(avcon_convert_t* convert);



/*
  perform safe packed colorspace conversion, by splitting
  the image up into an optimized area (pixel width divisible by 16),
  and two unoptimized/plain-c areas (pixel width divisible by 2)
*/

static void
safe_packed_conv(uint8_t * x_ptr, int x_stride,
				 uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
				 int y_stride, int uv_stride,
				 int width, int height, int vflip,
				 packedFunc * func_opt, packedFunc func_c, 
                 int size, int interlacing)
{
	int width_opt, width_c, height_opt;

    if (width<0 || width==1 || height==1) return; /* forget about it */

	if (func_opt != func_c && x_stride < size*((width+15)/16)*16)
	{
		width_opt = width & (~15);
		width_c = (width - width_opt) & (~1);
	}
	else if (func_opt != func_c && !(width&1) && (size==3))
	{
        /* MMX reads 4 bytes per pixel for RGB/BGR */
        width_opt = width - 2;
        width_c = 2;
    }
    else {
        /* Enforce the width to be divisable by two. */
		width_opt = width & (~1);
		width_c = 0;
	}

    /* packed conversions require height to be divisable by 2
       (or even by 4 for interlaced conversion) */
    if (interlacing)
        height_opt = height & (~3);
    else
        height_opt = height & (~1);

	func_opt(x_ptr, x_stride,
			y_ptr, u_ptr, v_ptr, y_stride, uv_stride,
			width_opt, height_opt, vflip);

	if (width_c)
	{
		func_c(x_ptr + size*width_opt, x_stride,
			y_ptr + width_opt, u_ptr + width_opt/2, v_ptr + width_opt/2,
			y_stride, uv_stride, width_c, height_opt, vflip);
	}
}

int
image_input(IMAGE * image,
			uint32_t width,
			int height,
			uint32_t edged_width,
			uint8_t * src[4],
			int src_stride[4],
			int csp,
			int interlacing)
{
	const int edged_width2 = edged_width/2;
	const int width2 = width/2;
	const int height2 = height/2;
#if 0
	const int height_signed = (csp & XVID_CSP_VFLIP) ? -height : height;
#endif

	switch (csp & ~XVID_CSP_VFLIP) {
	case XVID_CSP_RGB555:
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?rgb555i_to_yv12  :rgb555_to_yv12,
			interlacing?rgb555i_to_yv12_c:rgb555_to_yv12_c, 2, interlacing);
		break;

	case XVID_CSP_RGB565:
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?rgb565i_to_yv12  :rgb565_to_yv12,
			interlacing?rgb565i_to_yv12_c:rgb565_to_yv12_c, 2, interlacing);
		break;


	case XVID_CSP_BGR:
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?bgri_to_yv12  :bgr_to_yv12,
			interlacing?bgri_to_yv12_c:bgr_to_yv12_c, 3, interlacing);
		break;

	case XVID_CSP_BGRA:
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?bgrai_to_yv12  :bgra_to_yv12,
			interlacing?bgrai_to_yv12_c:bgra_to_yv12_c, 4, interlacing);
		break;

	case XVID_CSP_ABGR :
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?abgri_to_yv12  :abgr_to_yv12,
			interlacing?abgri_to_yv12_c:abgr_to_yv12_c, 4, interlacing);
		break;

	case XVID_CSP_RGB:
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?rgbi_to_yv12  :rgb_to_yv12,
			interlacing?rgbi_to_yv12_c:rgb_to_yv12_c, 3, interlacing);
		break;
            
	case XVID_CSP_RGBA :
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?rgbai_to_yv12  :rgba_to_yv12,
			interlacing?rgbai_to_yv12_c:rgba_to_yv12_c, 4, interlacing);
		break;
            
	case XVID_CSP_ARGB:
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?argbi_to_yv12  : argb_to_yv12,
			interlacing?argbi_to_yv12_c: argb_to_yv12_c, 4, interlacing);
		break;

	case XVID_CSP_YUY2:
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yuyvi_to_yv12  :yuyv_to_yv12,
			interlacing?yuyvi_to_yv12_c:yuyv_to_yv12_c, 2, interlacing);
		break;

	case XVID_CSP_YVYU:		/* u/v swapped */
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->v, image->u,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yuyvi_to_yv12  :yuyv_to_yv12,
			interlacing?yuyvi_to_yv12_c:yuyv_to_yv12_c, 2, interlacing);
		break;

	case XVID_CSP_UYVY:
		safe_packed_conv(
			src[0], src_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?uyvyi_to_yv12  :uyvy_to_yv12,
			interlacing?uyvyi_to_yv12_c:uyvy_to_yv12_c, 2, interlacing);
		break;

	case XVID_CSP_I420:	/* YCbCr == YUV == internal colorspace for MPEG */
		yv12_to_yv12(image->y, image->u, image->v, edged_width, edged_width2,
			src[0], src[0] + src_stride[0]*height, src[0] + src_stride[0]*height + (src_stride[0]/2)*height2,
			src_stride[0], src_stride[0]/2, width, height, (csp & XVID_CSP_VFLIP));
		break;

	case XVID_CSP_YV12: /* YCrCb == YVA == U and V plane swapped */
		yv12_to_yv12(image->y, image->v, image->u, edged_width, edged_width2,
			src[0], src[0] + src_stride[0]*height, src[0] + src_stride[0]*height + (src_stride[0]/2)*height2,
			src_stride[0], src_stride[0]/2, width, height, (csp & XVID_CSP_VFLIP));
		break;

	case XVID_CSP_PLANAR:  /* YCbCr with arbitrary pointers and different strides for Y and UV */
		yv12_to_yv12(image->y, image->u, image->v, edged_width, edged_width2,
			src[0], src[1], src[2], src_stride[0], src_stride[1],  /* v: dst_stride[2] not yet supported */
			width, height, (csp & XVID_CSP_VFLIP));
		break;

	case XVID_CSP_NULL:
		break;

	default :
		return -1;
	}


	/* pad out image when the width and/or height is not a multiple of 16 */

	if (width & 15)
	{
		int i;
		int pad_width = 16 - (width&15);
		for (i = 0; i < height; i++)
		{
			memset(image->y + i*edged_width + width,
				 *(image->y + i*edged_width + width - 1), pad_width);
		}
		for (i = 0; i < height/2; i++)
		{
			memset(image->u + i*edged_width2 + width2,
				 *(image->u + i*edged_width2 + width2 - 1),pad_width/2);
			memset(image->v + i*edged_width2 + width2,
				 *(image->v + i*edged_width2 + width2 - 1),pad_width/2);
		}
	}

	if (height & 15)
	{
		int pad_height = 16 - (height&15);
		int length = ((width+15)/16)*16;
		int i;
		for (i = 0; i < pad_height; i++)
		{
			memcpy(image->y + (height+i)*edged_width,
				   image->y + (height-1)*edged_width,length);
		}

		for (i = 0; i < pad_height/2; i++)
		{
			memcpy(image->u + (height2+i)*edged_width2,
				   image->u + (height2-1)*edged_width2,length/2);
			memcpy(image->v + (height2+i)*edged_width2,
				   image->v + (height2-1)*edged_width2,length/2);
		}
	}

/*
	if (interlacing)
		image_printf(image, edged_width, height, 5,5, "[i]");
	image_dump_yuvpgm(image, edged_width, ((width+15)/16)*16, ((height+15)/16)*16, "\\encode.pgm");
*/
	return 0;
}



int
image_output(IMAGE * image,
			 uint32_t width,
			 int height,
			 uint32_t edged_width,
			 uint8_t * dst[4],
			 int dst_stride[4],
			 int csp,
			 int interlacing)
{
	const int edged_width2 = edged_width/2;
	int height2 = height/2;

/*
	if (interlacing)
		image_printf(image, edged_width, height, 5,100, "[i]=%i,%i",width,height);
	image_dump_yuvpgm(image, edged_width, width, height, "\\decode.pgm");
*/

	switch (csp & ~XVID_CSP_VFLIP) {
	case XVID_CSP_RGB555:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_rgb555i  :yv12_to_rgb555,
			interlacing?yv12_to_rgb555i_c:yv12_to_rgb555_c, 2, interlacing);
		return 0;

	case XVID_CSP_RGB565:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_rgb565i  :yv12_to_rgb565,
			interlacing?yv12_to_rgb565i_c:yv12_to_rgb565_c, 2, interlacing);
		return 0;

    case XVID_CSP_BGR:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_bgri  :yv12_to_bgr,
			interlacing?yv12_to_bgri_c:yv12_to_bgr_c, 3, interlacing);
		return 0;

	case XVID_CSP_BGRA:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_bgrai  :yv12_to_bgra,
			interlacing?yv12_to_bgrai_c:yv12_to_bgra_c, 4, interlacing);
		return 0;

	case XVID_CSP_ABGR:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_abgri  :yv12_to_abgr,
			interlacing?yv12_to_abgri_c:yv12_to_abgr_c, 4, interlacing);
		return 0;

	case XVID_CSP_RGB:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_rgbi  :yv12_to_rgb,
			interlacing?yv12_to_rgbi_c:yv12_to_rgb_c, 3, interlacing);
		return 0;

	case XVID_CSP_RGBA:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_rgbai  :yv12_to_rgba,
			interlacing?yv12_to_rgbai_c:yv12_to_rgba_c, 4, interlacing);
		return 0;

	case XVID_CSP_ARGB:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_argbi  :yv12_to_argb,
			interlacing?yv12_to_argbi_c:yv12_to_argb_c, 4, interlacing);
		return 0;

	case XVID_CSP_YUY2:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_yuyvi  :yv12_to_yuyv,
			interlacing?yv12_to_yuyvi_c:yv12_to_yuyv_c, 2, interlacing);
		return 0;

	case XVID_CSP_YVYU:		/* u,v swapped */
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->v, image->u,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_yuyvi  :yv12_to_yuyv,
			interlacing?yv12_to_yuyvi_c:yv12_to_yuyv_c, 2, interlacing);
		return 0;

	case XVID_CSP_UYVY:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_uyvyi  :yv12_to_uyvy,
			interlacing?yv12_to_uyvyi_c:yv12_to_uyvy_c, 2, interlacing);
		return 0;

	case XVID_CSP_I420: /* YCbCr == YUV == internal colorspace for MPEG */
		yv12_to_yv12(dst[0], dst[0] + dst_stride[0]*height, dst[0] + dst_stride[0]*height + (dst_stride[0]/2)*height2,
			dst_stride[0], dst_stride[0]/2,
			image->y, image->u, image->v, edged_width, edged_width2,
			width, height, (csp & XVID_CSP_VFLIP));
		return 0;

	case XVID_CSP_YV12:	/* YCrCb == YVU == U and V plane swapped */
		yv12_to_yv12(dst[0], dst[0] + dst_stride[0]*height, dst[0] + dst_stride[0]*height + (dst_stride[0]/2)*height2,
			dst_stride[0], dst_stride[0]/2,
			image->y, image->v, image->u, edged_width, edged_width2,
			width, height, (csp & XVID_CSP_VFLIP));
		return 0;

	case XVID_CSP_PLANAR:  /* YCbCr with arbitrary pointers and different strides for Y and UV */
		yv12_to_yv12(dst[0], dst[1], dst[2],
			dst_stride[0], dst_stride[1],	/* v: dst_stride[2] not yet supported */
			image->y, image->u, image->v, edged_width, edged_width2,
			width, height, (csp & XVID_CSP_VFLIP));
		return 0;

	case XVID_CSP_INTERNAL :
		dst[0] = image->y;
		dst[1] = image->u;
		dst[2] = image->v;
		dst_stride[0] = edged_width;
		dst_stride[1] = edged_width/2;
		dst_stride[2] = edged_width/2;
		return 0;

	case XVID_CSP_NULL:
	case XVID_CSP_SLICE:
		return 0;

	}

	return -1;
}


#define mEMMS() __asm emms


int avcon_convert(avcon_convert_t* convert)
{
	try{
		int width;
		int height;
		int width2;
		int height2;
		IMAGE img;

		width = convert->width;
		height = convert->height;
		width2 = convert->width>>1;
		height2 = convert->height>>1;

		if ((convert->input.csp & ~XVID_CSP_VFLIP)==XVID_CSP_YV12)
		{
			img.y = (uint8_t*)convert->input.plane[0];
			img.u = (uint8_t*)convert->input.plane[1];
			img.v = (uint8_t*)convert->input.plane[2];

			image_output(&img, width, height, convert->input.stride[0],
						(uint8_t**)convert->output.plane, convert->output.stride,
						convert->output.csp, convert->interlacing);
		}
		else if ((convert->output.csp & ~XVID_CSP_VFLIP)==XVID_CSP_YV12)
		{
			img.y = (uint8_t*)convert->output.plane[0];
			if (!convert->output.plane[1] || !convert->output.plane[2])
			{
				img.u = (uint8_t*)convert->output.plane[0] + convert->output.stride[0]*height;
				img.v = (uint8_t*)convert->output.plane[0] + convert->output.stride[0]*height + (convert->output.stride[0]>>1)*height2;
			}
			else
			{
				img.u = (uint8_t*)convert->output.plane[1];
				img.v = (uint8_t*)convert->output.plane[2];
			}

			image_input(&img, width, height, width,
						(uint8_t**)convert->input.plane, convert->input.stride,
						convert->input.csp, convert->interlacing);
		}
		else
		{
			return -1;
		}

		mEMMS();
		return 0;
	}
	catch(...)
	{
		return -1;
	}
}

int YV12_TO_RGB565(unsigned char*pYV12[3],unsigned char*pRGB,int nWidth,int nHeight,int nStride,int nFlip)
{
	avcon_convert_t avconConvert;
	avconConvert.input.csp=XVID_CSP_YV12;
	avconConvert.input.plane[0]=pYV12[0];
	avconConvert.input.plane[1]=pYV12[1];
	avconConvert.input.plane[2]=pYV12[2];
	avconConvert.input.stride[0]=nWidth;
	avconConvert.input.stride[1]=nWidth>>1;
	avconConvert.input.stride[2]=nWidth>>1;	

	avconConvert.output.csp=XVID_CSP_RGB565;
	if (nFlip)
		avconConvert.output.csp|= XVID_CSP_VFLIP;

	avconConvert.output.plane[0]=pRGB;
	avconConvert.output.stride[0]=nStride;


	avconConvert.width=nWidth;
	avconConvert.height=nHeight;
	avconConvert.interlacing=0;
	return avcon_convert(&avconConvert);
}

int RGB565_TO_YV12(unsigned char*pRGB,unsigned char*pYV12[3],int nWidth,int nHeight)
{
	avcon_convert_t avconConvert;
	avconConvert.input.csp=XVID_CSP_RGB555 | XVID_CSP_VFLIP;
	avconConvert.input.plane[0]=pRGB;
	avconConvert.input.stride[0]=nWidth*2;

	avconConvert.output.csp=XVID_CSP_YV12;
	avconConvert.output.plane[0]=pYV12[0];
	avconConvert.output.plane[1]=pYV12[1];
	avconConvert.output.plane[2]=pYV12[2];
	avconConvert.output.stride[0]=nWidth;
	avconConvert.output.stride[1]=nWidth/2;
	avconConvert.output.stride[2]=nWidth/2;

	avconConvert.width=nWidth;
	avconConvert.height=nHeight;
	avconConvert.interlacing=0;
	return avcon_convert(&avconConvert);
}

int YV12_TO_RGB24(unsigned char*pYV12[3],unsigned char*pRGB,int nWidth,int nHeight,int nStride,int nFlip)
{
	avcon_convert_t avconConvert;
	avconConvert.input.csp=XVID_CSP_YV12;
	avconConvert.input.plane[0]=pYV12[0];
	avconConvert.input.plane[1]=pYV12[1];
	avconConvert.input.plane[2]=pYV12[2];
	avconConvert.input.stride[0]=nWidth;
	avconConvert.input.stride[1]=nWidth>>1;
	avconConvert.input.stride[2]=nWidth>>1;	

	avconConvert.output.csp=XVID_CSP_BGR;
	if (nFlip)
		avconConvert.output.csp|= XVID_CSP_VFLIP;

	avconConvert.output.plane[0]=pRGB;
	avconConvert.output.stride[0]=nStride;


	avconConvert.width=nWidth;
	avconConvert.height=nHeight;
	avconConvert.interlacing=0;
	return avcon_convert(&avconConvert);
}

int RGB24_TO_YV12(unsigned char*pRGB,unsigned char*pYV12[3],int nWidth,int nHeight,int nWidthOffset)
{
	avcon_convert_t avconConvert;
	memset(&avconConvert,0,sizeof(avcon_convert_t));
	avconConvert.input.csp=XVID_CSP_BGR | XVID_CSP_VFLIP;
	avconConvert.input.plane[0]=pRGB;
	avconConvert.input.stride[0]=(nWidth+nWidthOffset)*3;

	avconConvert.output.csp=XVID_CSP_YV12;
	avconConvert.output.plane[0]=pYV12[0];
	avconConvert.output.plane[1]=pYV12[1];
	avconConvert.output.plane[2]=pYV12[2];
	avconConvert.output.stride[0]=nWidth;
	avconConvert.output.stride[1]=nWidth/2;
	avconConvert.output.stride[2]=nWidth/2;

	avconConvert.width=nWidth;
	avconConvert.height=nHeight;
	avconConvert.interlacing=0;
	return avcon_convert(&avconConvert);
}


int YV12_TO_RGB32(unsigned char*pYV12[3],unsigned char*pRGB,int nWidth,int nHeight,int nStride,int nFlip)
{
	avcon_convert_t avconConvert;
	avconConvert.input.csp=XVID_CSP_YV12;
	avconConvert.input.plane[0]=pYV12[0];
	avconConvert.input.plane[1]=pYV12[1];
	avconConvert.input.plane[2]=pYV12[2];
	avconConvert.input.stride[0]=nWidth;
	avconConvert.input.stride[1]=nWidth>>1;
	avconConvert.input.stride[2]=nWidth>>1;	

	avconConvert.output.csp=XVID_CSP_BGRA;
	if (nFlip)
		avconConvert.output.csp|=XVID_CSP_VFLIP;

	avconConvert.output.plane[0]=pRGB;
	avconConvert.output.stride[0]=nStride;


	avconConvert.width=nWidth;
	avconConvert.height=nHeight;
	avconConvert.interlacing=0;
	return avcon_convert(&avconConvert);
}

int RGB32_TO_YV12(unsigned char*pRGB,unsigned char*pYV12[3],int nWidth,int nHeight)
{
	avcon_convert_t avconConvert;
	avconConvert.input.csp=XVID_CSP_BGRA | XVID_CSP_VFLIP;
	avconConvert.input.plane[0]=pRGB;
	avconConvert.input.stride[0]=nWidth*4;

	avconConvert.output.csp=XVID_CSP_YV12;
	avconConvert.output.plane[0]=pYV12[0];
	avconConvert.output.plane[1]=pYV12[1];
	avconConvert.output.plane[2]=pYV12[2];
	avconConvert.output.stride[0]=nWidth;
	avconConvert.output.stride[1]=nWidth/2;
	avconConvert.output.stride[2]=nWidth/2;

	avconConvert.width=nWidth;
	avconConvert.height=nHeight;
	avconConvert.interlacing=0;
	return avcon_convert(&avconConvert);
}

int YV12_TO_YV12(unsigned char*src[3],unsigned char*dst[3],int nWidth,int nHeight,int nStride)
{
	try{
		IMAGE img;

		int src_stride[3];
		src_stride[0]=nWidth;
		src_stride[1]=nWidth>>1;
		src_stride[2]=nWidth>>1;

		img.y = dst[0];
		img.u = dst[1];
		img.v = dst[2];

		image_input(&img, nWidth, nHeight, nStride,
			(uint8_t**)src, src_stride,
			XVID_CSP_PLANAR, 0);

		mEMMS();
		return 0;
	}
	catch(...)
	{
		return -1;
	}
}
