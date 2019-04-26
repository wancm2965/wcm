#ifndef __AVMTSCONNECTION_H__
#define __AVMTSCONNECTION_H__

#include "HPBC.h"
#include "AVMTSocketTCP.h"
#include "AVMTSFrame.h"
//#include "AVBufferPool.h"
#include "IHPNetIOQueue.h"
#include "IHPMsgQueue.h"

class AFX_EXT_CLASS AVMTSConnection
	: public IHPNetIOEvent
	, public AVMTSFrame
	, public AVBufferPool
	, public IHPMsgHandle
{
public:
	AVMTSConnection(SOCKET hSocket);
	~AVMTSConnection(void);
public:
	virtual void ReleaseConnections(void);
	virtual bool SendHeader(unsigned long ulType,const void*pData,int nLen);
	virtual bool SendHeader(const char*pHeader,int nHeaderLen);


	operator SOCKET(void){return m_AVMTSocketTCP;};
	void OnRead(void);
	void OnWrite(void);
	void OnError(void);

	virtual int SendData(const char*pData,int nLen);
	virtual void OnDisconnected(void);

	virtual void SetDataBuffer(AVDataBuffer*pAVDataBuffer);

	virtual void HandleMsg(unsigned long ulMsgID);
protected:
	AVMTSocketTCP				m_AVMTSocketTCP;
	bool						m_bDisconnected;
	unsigned long				m_ulWriteTimestamp;
	unsigned long				m_ulReadimestamp;
};


class AVMTTCPClientNotify
{
public:
	AVMTTCPClientNotify(void){};
	virtual~AVMTTCPClientNotify(void){};
public:
	virtual void OnAVMTTCPClientNotifyReceivedFrame(const char*pFrameData,int nLen)=0;
	virtual void OnAVMTTCPClientNotifyDisconnected(void)=0;
};

class AVMTTCPClient
	: public AVMTSConnection
{
public:
	AVMTTCPClient(AVMTTCPClientNotify&rNotify,SOCKET hSocket)
		: AVMTSConnection(hSocket)
		, m_rNotify(rNotify)
	{
	};
	virtual~AVMTTCPClient(void){};
public:
	virtual void OnReceivedFrame(const char*pFrameData,int nLen)
	{
		m_rNotify.OnAVMTTCPClientNotifyReceivedFrame(pFrameData,nLen);
	}
	virtual void OnDisconnected(void)
	{
		m_rNotify.OnAVMTTCPClientNotifyDisconnected();
		AVMTSConnection::OnDisconnected();
	}
protected:
	AVMTTCPClientNotify&	m_rNotify;
};

#endif