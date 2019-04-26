#include "stdafx.h"
#include <math.h>
#include "PictProp.h"
#include "inttypes.h"
#include "simd.h"
#include "../VIDEC_csp/fastmemcpy.h"
#include "AVMPlayer.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"

#define DEFAULT_CONTRAST	128
#define DEFAULT_BRIGHTNESS	0
#define DEFAULT_HUE			0
#define DEFAULT_SATURATION	64

#define DEFAULT_GAMMA_CORRECTION	100
 
static inline uint8_t limit_uint8(int a)
{
	if (a&(~255)) return uint8_t((-a)>>31);
	else          return uint8_t(a);
}

template<class T> inline const T& limit(const T& val,const T& min,const T& max)
{
    if (val<min) {
        return min;
    } else if (val>max) {
        return max;
    } else {
        return val;
    }
}

extern void aligned_free( void *memory );
extern void * aligned_malloc( int size, int align );

VIDEC_PictProp*VIDEC_PictProp::Create(void)
{
	//return new PictProp();
	NEWRECORDret3(  VIDEC_PictProp* ,p , new PictProp( )  )  
}

PictProp::PictProp(void)
{
	m_nWidth=0;
	m_nHeight=0;
	m_nHeightUV=0;
	m_nStrideUV=0;

	m_nYLen=m_nWidth*m_nHeight;
	m_nUVLen=m_nYLen>>2;

	m_nContrast=DEFAULT_CONTRAST;
	m_nBrightness=DEFAULT_BRIGHTNESS;
	m_nHue=DEFAULT_HUE;
	m_nSaturation=DEFAULT_SATURATION;

	m_nSharpStrength=0;
	m_nOldSharpStrength=0;
    asharp_run=getAsharp();
    aline=NULL;

	oldAsharpT=6;
	oldAsharpD=8;
	oldAsharpB=0;
	oldAsharpHQBF=1;

	double t=oldAsharpT/100.0;
	asharpCfgT=(int)(t*(4<<7));
	asharpCfgT=limit(asharpCfgT,int(-(4<<7)),int(32*(4<<7))); // yes, negatives values are accepted

	double d=oldAsharpD/100.0;
	asharpCfgD=(int)(d*(4<<7));
	asharpCfgD=limit(asharpCfgD,0,int(16*(4<<7)));

	asharpCfgB=asharpCfgB2=256;

	if (AVMPlayer::m_cpu_flags&FF_CPU_SSE2)
	{
		chromaFc=&PictProp::chroma<Tsse2>;
		processLumaFc=&PictProp::processLuma<Tsse2>;
	}
	else
	{
		chromaFc=&PictProp::chroma<Tmmx>;
		processLumaFc=&PictProp::processLuma<Tmmx>;
	}
	processLumaFc=&PictProp::processLuma<Tmmx>;

	for (int i=-180,ii=0;i<=180;ii++,i++)
	{
		double Hue=(i*3.1415926)/180.0;
		m_anHueSin[ii]=short(sin(Hue)*128);
		m_anHueCos[ii]=short(cos(Hue)*128);
	}
 
	m_nOldGammaCorrection=-1;
	m_nGammaCorrection=DEFAULT_GAMMA_CORRECTION;
	
	m_nOldGammaR=-1;
	m_nGammaR=DEFAULT_GAMMA_CORRECTION;
	m_nOldGammaG=-1;
	m_nGammaG=DEFAULT_GAMMA_CORRECTION;
	m_nOldGammaB=-1;
	m_nGammaB=DEFAULT_GAMMA_CORRECTION;
}

PictProp::~PictProp(void)
{
    if (aline) {
        aligned_free(aline);
    }
    aline=NULL;
}


int PictProp::Open(int nWidth,int nHeight)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;

	m_nYLen=m_nWidth*m_nHeight;
	m_nUVLen=m_nYLen>>2;

	m_nStrideUV=m_nWidth>>1;
	m_nHeightUV=m_nHeight>>1;
	return 0;
}

void PictProp::Close(void)
{
    if (aline) {
        aligned_free(aline);
    }
    aline=NULL;

}

void PictProp::SetContrast(int nContrast)
{
	if (nContrast<0)
		nContrast=0;
	else if (nContrast>256)
		nContrast=256;
	m_nContrast=nContrast;
}

void PictProp::SetBrightness(int nBrightness)
{
	if (nBrightness<-256)
		nBrightness=-256;
	else if (nBrightness>256)
		nBrightness=256;
	m_nBrightness=nBrightness;
}

void PictProp::SetHue(int nHue)
{
	if (nHue<-180)
		nHue=-180;
	else if (nHue>180)
		nHue=180;
	m_nHue=nHue;
}

void PictProp::SetSaturation(int nSaturation)
{
	if (nSaturation<0)
		nSaturation=0;
	else if (nSaturation>128)
		nSaturation=128;
	m_nSaturation=nSaturation;
}

void PictProp::SetGamma(int nGamma)
{
	if (nGamma<1)
		nGamma=1;
	else if (nGamma>400)
		nGamma=400;
	m_nGammaCorrection=nGamma;
}

void PictProp::SetGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (nGammaR<1)
		nGammaR=1;
	else if (nGammaR>400)
		nGammaR=400;
	m_nGammaR=nGammaR;

	if (nGammaG<1)
		nGammaG=1;
	else if (nGammaG>400)
		nGammaG=400;
	m_nGammaG=nGammaG;

	if (nGammaB<1)
		nGammaB=1;
	else if (nGammaB>400)
		nGammaB=400;
	m_nGammaB=nGammaB;
}

void PictProp::SetSharpen(int nSharpen)
{
	if (nSharpen<0)
		nSharpen=0;
	else if (nSharpen>127)
		nSharpen=127;
	m_nSharpStrength=nSharpen;
}

bool PictProp::NeedProcess(void)
{
	return (m_nContrast!=DEFAULT_CONTRAST ||
		m_nBrightness!=DEFAULT_BRIGHTNESS ||
		m_nHue!=DEFAULT_HUE ||
		m_nSaturation!=DEFAULT_SATURATION ||
		(m_nGammaCorrection!=DEFAULT_GAMMA_CORRECTION) ||
		(m_nGammaR!=DEFAULT_GAMMA_CORRECTION) ||
		(m_nGammaG!=DEFAULT_GAMMA_CORRECTION) ||
		(m_nGammaB!=DEFAULT_GAMMA_CORRECTION) ||
		m_nSharpStrength!=0);
}

void PictProp::processGamma(unsigned char*pSrc,unsigned char*pDst)
{
	if (m_nOldGammaCorrection!=m_nGammaCorrection)
	{
		m_nOldGammaCorrection=m_nGammaCorrection;
		double gamma=100.0/(m_nOldGammaCorrection);
		for(int i=0;i<256;i++)
		{
			gammaTab[i]=(unsigned char)(255.0*pow((i/255.0),gamma));
			gammaTab8[i]=gammaTab[i]<<8;
			gammaTab16[i]=gammaTab8[i]<<8;
			gammaTab24[i]=gammaTab16[i]<<8;
		}
	}

	if (m_nGammaCorrection!=DEFAULT_GAMMA_CORRECTION)
	{
		for (unsigned int y=0;y<m_nHeight;y++)
		{
			unsigned char*srcY=pSrc;
			unsigned char*dstY=pDst;
			const unsigned char *srcTemp;unsigned char *dstTemp,*dstEnd;
			for (srcTemp=srcY+m_nWidth*y,dstTemp=dstY+m_nWidth*y,dstEnd=dstTemp+m_nWidth;dstTemp<dstEnd;srcTemp+=4,dstTemp+=4)
			{
				*(unsigned int*)dstTemp=gammaTab24[srcTemp[3]]|gammaTab16[srcTemp[2]]|gammaTab8[srcTemp[1]]|gammaTab[srcTemp[0]];
			}
		}
	}
}

void PictProp::processGammaRGB(unsigned char*srcOrg[3],unsigned char*dstOrg[3])
{
	if (m_nOldGammaR!=m_nGammaR || m_nOldGammaG!=m_nGammaG || m_nOldGammaB!=m_nGammaB)
	{
		m_nOldGammaR=m_nGammaR;m_nOldGammaG=m_nGammaG;m_nOldGammaB=m_nGammaB;
		double gammaR=m_nOldGammaR/100.0;
		double gammaG=m_nOldGammaG/100.0;
		double gammaB=m_nOldGammaB/100.0;
		double gY = 1.0/(0.299*gammaR+0.587*gammaG+0.114*gammaB);
		double gU = 1.0/sqrt (gammaB / gammaG);
		double gV = 1.0/sqrt (gammaR / gammaG);
		for(int i=0;i<256;i++)
		{
			gammaTabYUV[0][i]=(unsigned char)(255.0*pow((i/255.0),gY));
			gammaTabYUV[1][i]=(unsigned char)(255.0*pow((i/255.0),gU));
			gammaTabYUV[2][i]=(unsigned char)(255.0*pow((i/255.0),gV));
		}
	}

	int dy[3];
	dy[0]=m_nHeight;
	dy[1]=m_nHeightUV;
	dy[2]=m_nHeightUV;
	int stride[3];
	stride[0]=m_nWidth;
	stride[1]=m_nStrideUV;
	stride[2]=m_nStrideUV;
	int dx[3];
	dx[0]=m_nWidth;
	dx[1]=m_nStrideUV;
	dx[2]=m_nStrideUV;

	unsigned char*src[3];
	unsigned char*dst[3];
	src[0]=srcOrg[0];
	src[1]=srcOrg[1];
	src[2]=srcOrg[2];
	dst[0]=dstOrg[0];
	dst[1]=dstOrg[1];
	dst[2]=dstOrg[2];

	for (unsigned int i=0;i<3;i++)
	{
		for (const unsigned char *srcEnd=src[i]+dy[i]*stride[i];src[i]!=srcEnd;src[i]+=stride[i],dst[i]+=stride[i])
		{
			unsigned char *dstLn=dst[i];
			for (const unsigned char *srcLn=src[i],*srcLnEnd=src[i]+dx[i];srcLn<srcLnEnd;srcLn+=4,dstLn+=4)
			{
				*(unsigned int*)dstLn=(gammaTabYUV[i][srcLn[3]]<<24)|(gammaTabYUV[i][srcLn[2]]<<16)|(gammaTabYUV[i][srcLn[1]]<<8)|gammaTabYUV[i][srcLn[0]];
			}
		}
	}
}

void PictProp::DoProcess(unsigned char*src[3],unsigned char*dst[3])
{
	try
	{
		if ((m_nGammaR!=DEFAULT_GAMMA_CORRECTION) ||
			(m_nGammaG!=DEFAULT_GAMMA_CORRECTION) ||
			(m_nGammaB!=DEFAULT_GAMMA_CORRECTION))
		{
			if (m_nContrast!=DEFAULT_CONTRAST ||
				m_nBrightness!=DEFAULT_BRIGHTNESS ||
				m_nHue!=DEFAULT_HUE ||
				m_nSaturation!=DEFAULT_SATURATION ||
				(m_nGammaCorrection!=DEFAULT_GAMMA_CORRECTION) ||
				m_nSharpStrength!=0)		
			{
				if (m_nContrast!=DEFAULT_CONTRAST ||
					m_nBrightness!=DEFAULT_BRIGHTNESS)
				{
					(this->*processLumaFc)(src[0],dst[0]);
					if (m_nGammaCorrection!=DEFAULT_GAMMA_CORRECTION)
					{
						processGamma(dst[0],dst[0]);
					}
				}
				else
				{
					if (m_nGammaCorrection!=DEFAULT_GAMMA_CORRECTION)
					{
						processGamma(src[0],dst[0]);
					}
					else
					{
						fastmemcpy(dst[0],src[0],m_nYLen);
					}
				}
			 
				if (m_nHue!=DEFAULT_HUE ||
					m_nSaturation!=DEFAULT_SATURATION)
				{
					(this->*chromaFc)(src[1],src[2],dst[1],dst[2]);
				}
				else
				{
					fastmemcpy(dst[1],src[1],m_nUVLen);
					fastmemcpy(dst[2],src[2],m_nUVLen);
				}

				processGammaRGB(dst,dst);
			}
			else
			{
				processGammaRGB(src,dst);
			}
		}
		else
		{
			if (m_nContrast!=DEFAULT_CONTRAST ||
				m_nBrightness!=DEFAULT_BRIGHTNESS)
			{
				(this->*processLumaFc)(src[0],dst[0]);
				if (m_nGammaCorrection!=DEFAULT_GAMMA_CORRECTION)
				{
					processGamma(dst[0],dst[0]);
				}
			}
			else
			{
				if (m_nGammaCorrection!=DEFAULT_GAMMA_CORRECTION)
				{
					processGamma(src[0],dst[0]);
				}
				else
				{
					fastmemcpy(dst[0],src[0],m_nYLen);
				}
			}
		 
			if (m_nHue!=DEFAULT_HUE ||
				m_nSaturation!=DEFAULT_SATURATION)
			{
				(this->*chromaFc)(src[1],src[2],dst[1],dst[2]);
			}
			else
			{
				fastmemcpy(dst[1],src[1],m_nUVLen);
				fastmemcpy(dst[2],src[2],m_nUVLen);
			}

			if (m_nSharpStrength!=0)
			{
				if (m_nOldSharpStrength!=m_nSharpStrength)
				{
					int nAsharpOffset=0;//200;
					m_nOldSharpStrength=m_nSharpStrength;

					//oldAsharpT=nAsharpOffset+m_nOldSharpStrength*200/127;
					oldAsharpT=nAsharpOffset+m_nOldSharpStrength*508/127;
					if (oldAsharpT>3200)
						oldAsharpT=3200;
					oldAsharpD=400;//oldAsharpT+nAsharpOffset;
					if (oldAsharpD>1600)
						oldAsharpD=1600;
					oldAsharpB=0;
					oldAsharpHQBF=1;

					double t=oldAsharpT/100.0;
					asharpCfgT=(int)(t*(4<<7));
					asharpCfgT=limit(asharpCfgT,int(-(4<<7)),int(32*(4<<7))); // yes, negatives values are accepted

					double d=oldAsharpD/100.0;
					asharpCfgD=(int)(d*(4<<7));
					asharpCfgD=limit(asharpCfgD,0,int(16*(4<<7)));

					asharpCfgB=asharpCfgB2=256;
				}
				processSharpen(dst[0]);

				//unsigned char*pSrc=src[0];
				//unsigned char*pDst=dst[0];
				//for (int i=0;i<m_nHeight;i++)
				//{
				//	fastmemcpy(pDst,pSrc,m_nWidth/2);
				//	pDst+=m_nWidth;
				//	pSrc+=m_nWidth;
				//}
			}
		}
	}
	catch (...)
	{
		dst[0]=src[0];
		dst[1]=src[1];
		dst[2]=src[2];

		//DebugOutput("PictProp::DoProcess\n");
	}
}


template<class _mm> void PictProp::chroma(const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV)
{
	typename _mm::__m Sin64=_mm::set1_pi16(m_anHueSin[m_nHue+180]);
	typename _mm::__m Cos64=_mm::set1_pi16(m_anHueCos[m_nHue+180]);
	typename _mm::__m Sat64=_mm::set1_pi16((short)m_nSaturation);
	typename _mm::__m m0=_mm::setzero_si64(),m128=_mm::set1_pi16(128);
	for (const unsigned char *srcUend=srcU+m_nStrideUV*m_nHeightUV;srcU!=srcUend;srcU+=m_nStrideUV,srcV+=m_nStrideUV,dstU+=m_nStrideUV,dstV+=m_nStrideUV)
	{
		int x=0;
		const int dx113=m_nWidth-_mm::size/2+1;
		for (;x<dx113;x+=_mm::size/2)
		{
			typename _mm::__m u=_mm::subs_pi16(_mm::unpacklo_pi8(_mm::load2(srcU+x),m0),m128);
			typename _mm::__m v=_mm::subs_pi16(_mm::unpacklo_pi8(_mm::load2(srcV+x),m0),m128);
			typename _mm::__m u1=_mm::add_pi16(_mm::srai_pi16(_mm::mullo_pi16(_mm::add_pi16(_mm::srai_pi16(_mm::mullo_pi16(u,Cos64),7),_mm::srai_pi16(_mm::mullo_pi16(v,Sin64),7)),Sat64),6),m128);
			typename _mm::__m v1=_mm::add_pi16(_mm::srai_pi16(_mm::mullo_pi16(_mm::sub_pi16(_mm::srai_pi16(_mm::mullo_pi16(v,Cos64),7),_mm::srai_pi16(_mm::mullo_pi16(u,Sin64),7)),Sat64),6),m128);
			_mm::store2(dstU+x,_mm::packs_pu16(u1,m0));
			_mm::store2(dstV+x,_mm::packs_pu16(v1,m0));
		}
		for (;x<(int)m_nWidth;x++)
		{
			int u=srcU[x]-128;
			int v=srcV[x]-128;
			int u2=((u*m_anHueCos[m_nHue+180])>>7)+((v*m_anHueSin[m_nHue+180])>>7);
			int v2=((v*m_anHueCos[m_nHue+180])>>7)-((u*m_anHueSin[m_nHue+180])>>7);
			u=((u2*m_nSaturation)>>6)+128;
			v=((v2*m_nSaturation)>>6)+128;
			dstU[x]=limit_uint8(u);
			dstV[x]=limit_uint8(v);
		}
	}
	_mm::empty();
}

template<class _mm> void PictProp::processLuma(const unsigned char *srcY,unsigned char *dstY)
{
	typedef typename _mm::__m __m;
	__m lumGain64=_mm::set1_pi16((short)m_nContrast);
	__m lumOffset64=_mm::set1_pi16((short)m_nBrightness);
	__m m0=_mm::setzero_si64(),m0040=_mm::set1_pi16(0x0040);
	int dx7=m_nWidth-_mm::size+1;
	int y=0;
	for (const unsigned char *srcYend=srcY+m_nHeight*m_nWidth;srcY!=srcYend;srcY+=m_nWidth,dstY+=m_nWidth,y++)
	{
		int x=0;
		if (_mm::align)
		{
			for (;intptr_t(dstY+x)&(_mm::align-1) && x<(int)m_nWidth;x++)
			{
				dstY[x]=limit_uint8((srcY[x]*m_nContrast>>7)+m_nBrightness);
			}
		}
		for (;x<dx7;x+=_mm::size)
		{
			__m mm1=*(__m*)(srcY+x);
			__m a=_mm::add_pi16(_mm::srli_pi16(_mm::add_pi16(_mm::mullo_pi16(_mm::unpacklo_pi8(mm1,m0),lumGain64),m0040),7),lumOffset64);
			__m b=_mm::add_pi16(_mm::srli_pi16(_mm::add_pi16(_mm::mullo_pi16(_mm::unpackhi_pi8(mm1,m0),lumGain64),m0040),7),lumOffset64);
			_mm::store2(dstY+x            ,_mm::packs_pu16(a,m0));
			_mm::store2(dstY+x+_mm::size/2,_mm::packs_pu16(b,m0));
		}
		for (;x<(int)m_nWidth;x++)
		{
			dstY[x]=limit_uint8((srcY[x]*m_nContrast>>7)+m_nBrightness);
		}
	}
	_mm::empty();
}


void PictProp::processSharpen(unsigned char*pSrc)
{
	try{
		if (aline==NULL)
		{
			aline=(unsigned char*)aligned_malloc(m_nWidth*2,128);
			if (aline==NULL)
				return;
		}

		unsigned char *lastX2,*lastX1;
		if (oldAsharpHQBF) 
		{
			lastX2=(unsigned char*)_alloca(m_nHeight);
			lastX1=(unsigned char*)_alloca(m_nHeight);
			for (unsigned int y=0; y<m_nHeight; y++) 
			{
				lastX2[y]=pSrc[y*m_nWidth+m_nWidth-2];
				lastX1[y]=pSrc[y*m_nWidth+m_nWidth-1];
			}
		} 
		else 
		{
			lastX2=lastX1=NULL;
		}

		asharp_run(pSrc,(int)m_nWidth,m_nHeight,m_nWidth,asharpCfgT,asharpCfgD,asharpCfgB,asharpCfgB2,oldAsharpHQBF,aline);
		_mm_empty();

		if (oldAsharpHQBF)
		{
			for (unsigned int y=0; y<m_nHeight; y++) 
			{
				pSrc[y*m_nWidth+m_nWidth-2]=lastX2[y];
				pSrc[y*m_nWidth+m_nWidth-1]=lastX1[y];
			}
		}
	}
	catch(...)
	{
		//DebugOutput("PictProp::processSharpen\n");
	}
}