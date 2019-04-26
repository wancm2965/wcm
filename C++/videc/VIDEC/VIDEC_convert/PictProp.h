//PictProp.h
#ifndef __PICTPROP_H__
#define __PICTPROP_H__

#include "VIDEC_PictProp.h"
#include "asharp.h"

class PictProp
	: public VIDEC_PictProp
{
public:
	PictProp(void);
	virtual~PictProp(void);
public:
	virtual int Open(int nWidth,int nHeight);
	virtual void Close(void);
	virtual void SetContrast(int nContrast);
	virtual void SetBrightness(int nBrightness);
	virtual void SetHue(int nHue);
	virtual void SetSaturation(int nSaturation);
	virtual void SetGamma(int nGamma);
	virtual void SetGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	virtual void SetSharpen(int nSharpen);
	virtual bool NeedProcess(void);
	virtual void DoProcess(unsigned char*src[3],unsigned char*dst[3]);
protected:
	void (PictProp::*chromaFc)(const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV);
	template<class _mm> void chroma(const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV);
	void (PictProp::*processLumaFc)(const unsigned char *srcY,unsigned char *dstY);
	template<class _mm> void processLuma(const unsigned char *srcY,unsigned char *dstY);

	void processGamma(unsigned char*pSrc,unsigned char*pDst);
	void processSharpen(unsigned char*pSrc);
		
	void processGammaRGB(unsigned char*src[3],unsigned char*dst[3]);
protected:
	int				m_nWidth;
	int				m_nHeight;

	int				m_nStrideUV;
	int				m_nHeightUV;

	int				m_nYLen;
	int				m_nUVLen;

	int				m_nContrast;
	int				m_nBrightness;
	int				m_nHue;
	int				m_nSaturation;


	short			m_anHueSin[360+1];
	short			m_anHueCos[360+1];

	 
	unsigned int	gammaTab[256];
	unsigned int	gammaTab8[256];
	unsigned int	gammaTab16[256];
	unsigned int	gammaTab24[256];
	int				m_nOldGammaCorrection;
	int				m_nGammaCorrection;//1-400

	int				m_nOldGammaR;
	int				m_nGammaR;
	int				m_nOldGammaG;
	int				m_nGammaG;
	int				m_nOldGammaB;
	int				m_nGammaB;
 
	unsigned int	gammaTabYUV[3][256];

	int				m_nSharpStrength;
	int				m_nOldSharpStrength;
	int asharpCfgT,asharpCfgD,asharpCfgB,asharpCfgB2;
	int oldAsharpT,oldAsharpD,oldAsharpB,oldAsharpHQBF;
	unsigned char *aline;
	asharp_run_fct *asharp_run;
};

#endif