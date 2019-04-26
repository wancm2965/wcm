//XDenoise.h
#ifndef __XDENOISE_H__
#define __XDENOISE_H__


#include "VIDEC_convert.h"

class XDenoise
	: public VIDEC_Denoise
{
public:
	XDenoise(void);
	virtual~XDenoise(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	int Denoise(unsigned char*src[],unsigned char*dst[]);
private:
	static inline unsigned char LowPass(unsigned char Prev,unsigned char Curr,const int *Coef)
	{
		return unsigned char(Curr+Coef[Prev-Curr]);
	}
	static void deNoise(const unsigned char *Frame,    
						const unsigned char *FramePrev,
						unsigned char *FrameDest,     
						unsigned char *LineAnt,       
						int W, int H, int sStride, int pStride, int dStride,
						int *Horizontal, int *Vertical, int *Temporal);
protected:                     
	void precalcCoefs(int nCt, double Dist25);
protected:
	unsigned char*	m_pPreFrame[3];
	int				m_nPreFrameStride[3];
	int				m_nCurFrameStride[3];
	int				m_nCurFrameHeight[3];

	unsigned char*	m_pLineBuf;
	int				m_aCoefs[4][512];
};

#endif