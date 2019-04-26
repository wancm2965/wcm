// AVMTSocketClient.h: interface for the AVMTSocketClient class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVMTSOCKETCLIENT_H__
#define __AVMTSOCKETCLIENT_H__

#include "HPBC.h"
#include "AVMTSocketTCP.h"
//#include "AVUtil.h"
#include "IHPNetIOQueue.h"

class AFX_EXT_CLASS AVMTSocketClient;
class AFX_EXT_CLASS AVMTSocketClientNotify
{
public:
	AVMTSocketClientNotify(void){};
	virtual~AVMTSocketClientNotify(void){};
public:
	virtual bool OnAVMTSocketClientNotifyConnected(AVMTSocketClient*pAVMTSocketClient,SOCKET hSocket,unsigned long ulType,const void*pData,int nLen)=0;
	virtual void OnAVMTSocketClientNotifyFailed(AVMTSocketClient*pAVMTSocketClient,unsigned long ulType)=0;
};

class AFX_EXT_CLASS AVMTSocketClient
	: public IHPNetIOEvent
{
public:
	AVMTSocketClient(AVMTSocketClientNotify&rNotify);
	virtual ~AVMTSocketClient(void);
public:
	bool Connect(SOCKET hSocket,unsigned long ulType,const void*pData,int nLen);
	void ReleaseConnections(void);
	bool IsExpired(void);

	operator SOCKET(void){return m_AVMTSocketTCP;};
	void OnRead(void);
	void OnWrite(void);
	void OnError(void);
protected:
	AVMTSocketClientNotify&	m_rNotify;
	void*					m_pData;
	int						m_nLen;
	bool					m_bSending;
	int						m_nSentBytes;
	unsigned long			m_ulType;
	unsigned long			m_ulStartTimestamp;
	AVMTSocketTCP			m_AVMTSocketTCP;
	char					m_pHeaderData[8];
	int						m_nHeaderLen;
	int						m_nBytes;
};

#endif