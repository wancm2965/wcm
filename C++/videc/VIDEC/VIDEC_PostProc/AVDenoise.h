// AVDenoise.h: interface for the AVDenoise class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVDENOISE_H__
#define __AVDENOISE_H__

#include "AVMPlayer.h"
#include "libxpostproc.h"

class AVDenoise 
	: public IXDenoise 
{
public:
	AVDenoise();
	virtual ~AVDenoise();
public:
	bool Connect(int nWidth,int nHeight);
	void ReleaseConnections(void);

	bool Denoise(uint8_t * src[3], int srcStride[3], uint8_t * dst[3],int dstStride[3],int horizontalSize, int verticalSize);
protected:
	pp_context_t*	m_pContext;
	PPMode			m_PPMode;
	int				m_nWidth;
	int				m_nHeight;
};

#endif 