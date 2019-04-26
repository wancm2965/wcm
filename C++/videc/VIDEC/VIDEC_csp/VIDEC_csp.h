// VIDEC_CSP.h: interface for the VIDEC_CSP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VIDEC_CSP_H__
#define __VIDEC_CSP_H__

#include "stdint.h"

//#define int8_t  char
//#define uint8_t unsigned char
//#define int16_t  short
//#define uint16_t unsigned short
//#define int32_t  int
//#define uint32_t unsigned int
//#define int64_t  __int64
//#define uint64_t unsigned __int64
/* useful macros */

#define MIN(X, Y) ((X)<(Y)?(X):(Y))
#define MAX(X, Y) ((X)>(Y)?(X):(Y))
/* #define ABS(X)    (((X)>0)?(X):-(X)) */
#define SIGN(X)   (((X)>0)?1:-1)
#define CLIP(X,AMIN,AMAX)   (((X)<(AMIN)) ? (AMIN) : ((X)>(AMAX)) ? (AMAX) : (X))
#define DIV_DIV(a,b)    (((a)>0) ? ((a)+((b)>>1))/(b) : ((a)-((b)>>1))/(b))
#define SWAP(_T_,A,B)    { _T_ tmp = A; A = B; B = tmp; }

/* colorspace values */

#define XVID_CSP_PLANAR   (1<< 0) /* 4:2:0 planar (==I420, except for pointers/strides) */
#define XVID_CSP_USER	  XVID_CSP_PLANAR
#define XVID_CSP_I420     (1<< 1) /* 4:2:0 planar */
#define XVID_CSP_YV12     (1<< 2) /* 4:2:0 planar */
#define XVID_CSP_YUY2     (1<< 3) /* 4:2:2 packed */
#define XVID_CSP_UYVY     (1<< 4) /* 4:2:2 packed */
#define XVID_CSP_YVYU     (1<< 5) /* 4:2:2 packed */
#define XVID_CSP_RGB      (1<<16) /* 24-bit rgb packed */
#define XVID_CSP_BGRA     (1<< 6) /* 32-bit bgra packed */
#define XVID_CSP_ABGR     (1<< 7) /* 32-bit abgr packed */
#define XVID_CSP_RGBA     (1<< 8) /* 32-bit rgba packed */
#define XVID_CSP_ARGB     (1<<15) /* 32-bit argb packed */
#define XVID_CSP_BGR      (1<< 9) /* 24-bit bgr packed */
#define XVID_CSP_RGB555   (1<<10) /* 16-bit rgb555 packed */
#define XVID_CSP_RGB565   (1<<11) /* 16-bit rgb565 packed */
#define XVID_CSP_SLICE    (1<<12) /* decoder only: 4:2:0 planar, per slice rendering */
#define XVID_CSP_INTERNAL (1<<13) /* decoder only: 4:2:0 planar, returns ptrs to internal buffers */
#define XVID_CSP_NULL     (1<<14) /* decoder only: dont output anything */
#define XVID_CSP_VFLIP    (1<<31) /* vertical flip mask */

/* XVID_DEC_DECODE param1 */
/* general flags */
#define XVID_LOWDELAY      (1<<0) /* lowdelay mode  */
#define XVID_DISCONTINUITY (1<<1) /* indicates break in stream */
#define XVID_DEBLOCKY      (1<<2) /* perform luma deblocking */
#define XVID_DEBLOCKUV     (1<<3) /* perform chroma deblocking */
#define XVID_FILMEFFECT    (1<<4) /* adds film grain */
#define XVID_DERINGUV      (1<<5) /* perform chroma deringing, requires deblocking to work */
#define XVID_DERINGY       (1<<6) /* perform luma deringing, requires deblocking to work */

typedef struct
{
    uint8_t * y;
    uint8_t * u;
    uint8_t * v;
} IMAGE;

/* avcon_image_t
	for non-planar colorspaces use only plane[0] and stride[0]
	four plane reserved for alpha*/
typedef struct {
	int csp;				/* [in] colorspace; or with XVID_CSP_VFLIP to perform vertical flip */
	void * plane[4];		/* [in] image plane ptrs */
	int stride[4];			/* [in] image stride; "bytes per row"*/
} avcon_image_t;

/* XVID_GBL_CONVERT param1 */
typedef struct {
	avcon_image_t input;  /* [in] input image & colorspace */
	avcon_image_t output; /* [in] output image & colorspace */
	int width;           /* [in] width */
	int height;          /* [in] height */
	int interlacing;     /* [in] interlacing */
} avcon_convert_t;

int avcon_convert(avcon_convert_t* convert);

int RGB565_TO_YV12(unsigned char*pRGB,unsigned char*pYV12[3],int nWidth,int nHeight);
int YV12_TO_RGB565(unsigned char*pYV12[3],unsigned char*pRGB,int nWidth,int nHeight,int nStride,int nFlip);
int RGB24_TO_YV12(unsigned char*pRGB,unsigned char*pYV12[3],int nWidth,int nHeight,int nWidthOffset=0);
int YV12_TO_RGB24(unsigned char*pYV12[3],unsigned char*pRGB,int nWidth,int nHeight,int nStride,int nFlip);
int RGB32_TO_YV12(unsigned char*pRGB,unsigned char*pYV12[3],int nWidth,int nHeight);
int YV12_TO_RGB32(unsigned char*pYV12[3],unsigned char*pRGB,int nWidth,int nHeight,int nStride,int nFlip);

int YV12_TO_YV12(unsigned char*src[3],unsigned char*dst[3],int nWidth,int nHeight,int nStride);


#endif