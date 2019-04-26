#ifndef __AVMTDataSESSION_H__
#define __AVMTDataSESSION_H__

#include <NETEC/XAutoLock.h>

class NETEC_API AVMTDataChannel;
class NETEC_API AVMTDataSession
{
public:
	AVMTDataSession(AVMTDataChannel&rAVMTDataChannel);
	virtual ~AVMTDataSession(void);
public:
	virtual bool Connect(void);
	virtual void ReleaseConnections(void);
	virtual uint32_t GetDataChannelID(void);
	virtual uint32_t GetDataSessionID(void);
	virtual void SendDataToChannel(const char*pData,int nLen);
	virtual void OnReceivedDataFromChannel(const char*pData,int nLen)=0;
	virtual bool IsDisconnected(void);

	virtual void OnDetachedFromAVMTDataChannel(void);
protected:
	XCritSec			m_csAVMTDataChannel;
	AVMTDataChannel*		m_pAVMTDataChannel;
	uint32_t		m_ulDataSessionID;
	uint32_t		m_ulDataChannelID;
};

#endif