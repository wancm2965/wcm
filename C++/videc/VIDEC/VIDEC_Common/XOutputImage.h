//XOutputImage.h
#ifndef __XOUTPUTIMAGE_H__
#define __XOUTPUTIMAGE_H__

#include <map>

#include "XAutoLock.h"
#include "VIDEC_OutputImage.h"


class XOutputImage
{
public:
	XOutputImage(VIDEC_OutputImage*pOutputImage);
	virtual~XOutputImage(void);
public:
	virtual int GetFrameRate(void);
	virtual int OutputImage(void*pData,int nLen,int nWidth,int nHeight);
protected:
	VIDEC_OutputImage*	m_pOutputImage;
};

typedef std::map<void*,XOutputImage*>	MapXOutputImage;

class XOutputImageMgr
{
public:
	XOutputImageMgr(void);
	virtual~XOutputImageMgr(void);
public:
	virtual void Close(void);

	virtual int AddOutputImage(VIDEC_OutputImage*pOutputImage);
	virtual void RemoveOutputImage(VIDEC_OutputImage*pOutputImage);

	virtual int OutputImage(void*pData,int nLen,int nWidth,int nHeight);

	virtual int GetMaxFrameRate(void);

	virtual int GetOutputImageCount(void);
protected:
	MapXOutputImage	m_MapXOutputImage;
	XCritSec		m_csMapXOutputImage;
};


#endif