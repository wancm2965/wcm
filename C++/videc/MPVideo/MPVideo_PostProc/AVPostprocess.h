#ifndef __AVPOSTPROCESS_H__
#define __AVPOSTPROCESS_H__

#include "AVMPlayer.h"
#include "libxpostproc.h"

class AVPostProcess
	: public IXPostProcess
{
public:
	AVPostProcess(void);
	virtual~AVPostProcess(void);
public:
	bool Connect(int nWidth,int nHeight,int nLevel);
	void ReleaseConnections(void);

	bool PostProcess(unsigned char * src[3], int srcStride[3], unsigned char * dst[3],int dstStride[3],int horizontalSize, int verticalSize,char*qscale_table,int qstride);
protected:
	void SetLevel(int nLevel);
protected:
	PPMode							m_PPMode;
	void*							m_pPPContext;
	int								m_nPPMode;
	int								m_nWidth;
	int								m_nHeight;
};




#endif
