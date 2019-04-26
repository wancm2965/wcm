// AVSource.h: interface for the AVSource class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVSOURCE_H__
#define __AVSOURCE_H__

class AVSink;

class AVCONEC40_AFX_EXT_CLASS AVSource  
{
public:
	AVSource();
	virtual ~AVSource();
public:
	virtual bool AddSink(AVSink*pAVSink);
	virtual bool RemoveSink(AVSink*pAVSink);
	virtual int	Deliver(char*szData,int nLen);
	virtual void RemoveAll(void)=0;
protected:
	AVCritSec				m_AVCritSec;
	ListPtr					m_listAVSink;
};

#endif 