//VIDEC_PictProp.h
#ifndef __VIDEC_PICTPROP_H__
#define __VIDEC_PICTPROP_H__

class VIDEC_PictProp
{
public:
	VIDEC_PictProp(void){};
	virtual~VIDEC_PictProp(void){};
public:
	virtual int Open(int nWidth,int nHeight)=0;
	virtual void Close(void)=0;
	virtual void SetContrast(int nContrast)=0;
	virtual void SetBrightness(int nBrightness)=0;
	virtual void SetHue(int nHue)=0;
	virtual void SetSaturation(int nSaturation)=0;
	virtual void SetGamma(int nGamma)=0;
	virtual void SetGammaRGB(int nGammaR,int nGammaG,int nGammaB)=0;
	virtual void SetSharpen(int nSharpen)=0;
	virtual bool NeedProcess(void)=0;
	virtual void DoProcess(unsigned char*src[3],unsigned char*dst[3])=0;

	static VIDEC_PictProp*Create(void);
};


#endif