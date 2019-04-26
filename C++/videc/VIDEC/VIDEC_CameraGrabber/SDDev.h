//SDDev.h
#ifndef __SDDEV_H__
#define __SDDEV_H__

#include "XOutputImage.h"
#include "XOutputStream.h"

class SDDev
	: public XOutputImageMgr
	, public XOutputStreamMgr
{
public:
	SDDev(unsigned int nDevIndex);
	virtual~SDDev(void);
public:
	virtual int Open(int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate)=0;
	virtual void Close()=0;
	virtual int SetDevCap(int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate)=0;

	virtual bool CanBeClosed(void);
protected:
	unsigned int		m_nDevIndex;

	int					m_nWidth;
	int					m_nHeight;
	int					m_nFrameRate;
	bool				m_bAutoFrameRate;
};

#endif