// AVDeinterlace.h: interface for the AVDeinterlace class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVDEINTERLACE_H__
#define __AVDEINTERLACE_H__

#include "AVMPlayer.h"
#include "libxpostproc.h"

class AVDeinterlace 
	: public IXDeinterlace
{
public:
	AVDeinterlace();
	virtual ~AVDeinterlace();
public:
	bool Connect(int nWidth,int nHeight);
	void ReleaseConnections(void);

	bool Deinterlace(uint8_t * src[3], int srcStride[3], uint8_t * dst[3],int dstStride[3],int horizontalSize, int verticalSize);
protected:
	pp_context_t*					m_pContext;
	PPMode							m_PPMode;
	int								m_nWidth;
	int								m_nHeight;
};

#endif 