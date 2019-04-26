#ifndef _SUB_TITLE_H_
#define _SUB_TITLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

//#define USED_SUB_TITLE	1

class SubTitle
{
public:
	SubTitle(void);
	virtual~SubTitle(void);
public:
	static void LoadHZK(void);
	static void DrawTitle(unsigned char *pY,int nStride,int nWidth,int nHeight,int nSubTitle,const char*cszSubTitle);
private:
	static unsigned char	Ascii[];
	static unsigned char*	m_pHZK;
	static int				m_nHZKSize;
};

#endif
