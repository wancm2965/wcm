#include <winsock2.h>
#include <windows.h>
#include "XDenoise.h"
#include <math.h>
#include <mmintrin.h>
#include "../VIDEC_csp/fastmemcpy.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"

extern void * aligned_malloc( int size, int align );
extern void aligned_free( void *memory );

VIDEC_Denoise*VIDEC_Denoise::Create(void)
{
	//return new XDenoise();
	NEWRECORDret3(  VIDEC_Denoise* ,p , new XDenoise( )  )       
}

XDenoise::XDenoise(void)
{
	m_pLineBuf=NULL;

 	m_pPreFrame[0]=NULL;
	m_pPreFrame[1]=NULL;
	m_pPreFrame[2]=NULL;
}

XDenoise::~XDenoise(void)
{

}

int XDenoise::Open(int nWidth,int nHeight)
{
	m_pLineBuf=(unsigned char*)aligned_malloc(nWidth,128);

	m_nPreFrameStride[0]=(nWidth/16+2)*16;
	m_nPreFrameStride[1]=m_nPreFrameStride[2]=((nWidth>>1)/16+2)*16;
 	m_pPreFrame[0]=(unsigned char*)aligned_malloc(m_nPreFrameStride[0]*nHeight,128);
	m_pPreFrame[1]=(unsigned char*)aligned_malloc(m_nPreFrameStride[1]*(nHeight>>1),128);
	m_pPreFrame[2]=(unsigned char*)aligned_malloc(m_nPreFrameStride[2]*(nHeight>>1),128);
	
	m_nCurFrameStride[0]=nWidth;
	m_nCurFrameStride[1]=m_nCurFrameStride[2]=nWidth>>1;
	m_nCurFrameHeight[0]=nHeight;
	m_nCurFrameHeight[1]=m_nCurFrameHeight[2]=nHeight>>1;

	double LumSpac=4.00;
	double LumTmp=6.00;
	double ChromSpac=3.00;
	double ChromTmp=LumTmp*ChromSpac/LumSpac;
     //double LumSpac=0.0/100.0;
     //double LumTmp=2000.0/100.0;
     //double ChromSpac=0.0/100.0;
     //double ChromTmp=LumTmp*ChromSpac/LumSpac;


	precalcCoefs(0,LumSpac);
	precalcCoefs(1,LumTmp);
	precalcCoefs(2,ChromSpac);
	precalcCoefs(3,ChromTmp);
 

	return 0;
}

void XDenoise::Close(void)
{
	if (m_pLineBuf) aligned_free(m_pLineBuf);m_pLineBuf=NULL;
	if (m_pPreFrame[0]) aligned_free(m_pPreFrame[0]);m_pPreFrame[0]=NULL;
	if (m_pPreFrame[1]) aligned_free(m_pPreFrame[1]);m_pPreFrame[1]=NULL;
	if (m_pPreFrame[2]) aligned_free(m_pPreFrame[2]);m_pPreFrame[2]=NULL;

}

void XDenoise::precalcCoefs(int nCt, double Dist25)
{
	int *Ct=m_aCoefs[nCt];
	int i;
	double Gamma = log(0.25) / log(1.0 - Dist25/255.0);

	for (i = -256; i <= 255; i++)
	{
		double Simil = 1.0 - abs(i) / 255.0;
		double C = pow(Simil, Gamma) * (double)i;
		Ct[256+i] = int((C<0) ? (C-0.5) : (C+0.5));
	}
}

void XDenoise::deNoise(const unsigned char *Frame,    
                                        const unsigned char *FramePrev,
                                        unsigned char *FrameDest,     
                                        unsigned char *LineAnt,       
                                        int W, int H, int sStride, int pStride, int dStride,
                                        int *Horizontal, int *Vertical, int *Temporal)
{
 int sLineOffs = 0, pLineOffs = 0, dLineOffs = 0;
 unsigned char PixelAnt;

 /* First pixel has no left nor top neightbour. Only previous frame */
 LineAnt[0] = PixelAnt = Frame[0];
 FrameDest[0] = LowPass(FramePrev[0], LineAnt[0], Temporal);

 // Fist line has no top neightbour. Only left one for each pixel and last frame 
 for (int X = 1; X < W; X++)
  {
   PixelAnt = LowPass(PixelAnt, Frame[X], Horizontal);
   LineAnt[X] = PixelAnt;
   FrameDest[X] = LowPass(FramePrev[X], LineAnt[X], Temporal);
  }

 for (int Y = 1; Y < H; Y++)
  {
   sLineOffs += sStride, pLineOffs += pStride, dLineOffs += dStride;
   // First pixel on each line doesn't have previous pixel
   PixelAnt = Frame[sLineOffs];
   LineAnt[0] = LowPass(LineAnt[0], PixelAnt, Vertical);
   FrameDest[dLineOffs] = LowPass(FramePrev[pLineOffs], LineAnt[0], Temporal);

   for (int X = 1; X < W; X++)
    {
     // The rest are normal
     PixelAnt = LowPass(PixelAnt, Frame[sLineOffs+X], Horizontal);
     LineAnt[X] = LowPass(LineAnt[X], PixelAnt, Vertical);
     FrameDest[dLineOffs+X] = LowPass(FramePrev[pLineOffs+X], LineAnt[X], Temporal);
    }
  }
}

void asm_BitBlt_MMX(BYTE* dstp, int dst_pitch, const BYTE* srcp, int src_pitch, int row_size, int height) 
{
	if (height==0 || row_size==0) return;
	const unsigned char *esi=srcp+(height-1)*src_pitch;
	unsigned char *edi=dstp+(height-1)*dst_pitch;
	int row_size15=row_size-15;
	for (;height;height--,esi-=src_pitch,edi-=dst_pitch)
	{
		int ebx;
		for (ebx=0;ebx<row_size15;ebx+=16)
		{
			__m64 mm0=*(__m64*)(esi+ebx);
			__m64 mm1=*(__m64*)(esi+ebx+8);
			*(__m64*)(edi+ebx)=mm0;
			*(__m64*)(edi+ebx+8)=mm1;
		}
		for (;ebx<row_size;ebx++)
			edi[ebx]=esi[ebx];
	}
	_mm_empty();
}


void asm_BitBlt_C(BYTE* dstp, int dst_pitch, const BYTE* srcp, int src_pitch, int row_size, int height)
{
	if (dst_pitch == src_pitch && src_pitch == row_size) 
	{
		fastmemcpy(dstp, srcp, src_pitch * height);
	} 
	else 
	{
		for (int y=height; y>0; --y) 
		{
			fastmemcpy(dstp, srcp, row_size);
			dstp += dst_pitch;
			srcp += src_pitch;
		}
	}
}

int XDenoise::Denoise(unsigned char*src[],unsigned char*dst[])
{
	static const int coeffIdxs[3]={0,2,2};

	for (int i=0;i<3;i++)
	{
		deNoise(src[i],m_pPreFrame[i],dst[i],
				m_pLineBuf,m_nCurFrameStride[i],m_nCurFrameHeight[i],
				m_nCurFrameStride[i],m_nPreFrameStride[i],m_nCurFrameStride[i],
				m_aCoefs[coeffIdxs[i]  ] + 256,
				m_aCoefs[coeffIdxs[i]  ] + 256,
				m_aCoefs[coeffIdxs[i]+1] + 256);
		
		asm_BitBlt_MMX(m_pPreFrame[i],m_nPreFrameStride[i],dst[i],m_nCurFrameStride[i],m_nCurFrameStride[i],m_nCurFrameHeight[i]);        
	}

	return 0;
}
