// AVSink.h: interface for the AVSink class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVSINK_H__
#define __AVSINK_H__

class AVCONEC40_AFX_EXT_CLASS AVSource;
class AVCONEC40_AFX_EXT_CLASS AVSink  
{
public:
	AVSink();
	virtual ~AVSink();
public:
	virtual void OnDeliver(char*pData,int nLen);
	virtual void DisconnectFromAVSource(void);
	virtual void ConnectToAVSource(AVSource*pAVSource);
protected:
	AVSource*	m_pAVSource;
};

#endif 