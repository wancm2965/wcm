//IYUVDraw.h
#ifndef __IYUVDRAW_H__
#define __IYUVDRAW_H__

#include <VIDEC/VIDEC.h>

class IYUVDraw
{
public:
	static bool SaveToFileJPG(int nWidth,int nHeight,int nVirtualWidth,int nVirtualHeight,unsigned char*pData,int nLen,const char*cszFile);
};

#endif
