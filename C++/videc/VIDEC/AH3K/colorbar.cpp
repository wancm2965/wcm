#include "stdafx.h"
#include "colorbar.h"

#define BARS 8

void makecolorbar_yv12(unsigned char* pBuf,unsigned int x,unsigned int y)
{
	unsigned char* p;

	unsigned char* pY;
	unsigned char* pCr;
	unsigned char* pCb;

	/*
	Y00 Y01 Y02 Y03
	Y10 Y11 Y12 Y13
	Cb0     Cb1   
	Cr0     Cr1 
	4x2个像素,共10个字节
	使用空间 x*y+x*y/2 
	*/
	struct YCrCb
	{
		unsigned char y;
		unsigned char cb;
		unsigned char cr;
	}colorbar[BARS]=
	{
		{235,128,128}, //White
		{162,44,142},	//Yellow
		{131,156,44},	//Cyan
		{112,72,58},	//Green
		{84,184,198},	//Magenta
		{65,100,212},	//Read
		{35,212,114},	//Blue
		{16,128,128},	//Black	
	};

	p=pBuf;
	
	pY=p;
	pCb=pY+x*y;
	pCr=pCb+x*y/4;	
	
	if(p)
	{
		int bar=x/BARS;	//一个bar多少像素		
		int i;
		int j;
		//Y
		for(i=0;i<y;++i)
		{
			for(j=0;j<x;++j)
			{
				*p++=colorbar[j/bar].y;				
			}
		}

		ASSERT(pCb==p);
		for(i=0;i<y/2;++i)
		{
			for(j=0;j<x;++j)
			{
				if(j%2)
					*p++=colorbar[j/bar].cb;
			}
		}

		ASSERT(pCr==p);

		for(i=0;i<y/2;++i)
		{
			for(j=0;j<x;++j)
			{
				if(j%2)
					*p++=colorbar[j/bar].cr;
			}
		}			
	}
	
}


