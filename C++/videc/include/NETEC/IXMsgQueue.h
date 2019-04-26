//IXMsgQueue.h

#ifndef __IXMSGQUEUE_H__
#define __IXMSGQUEUE_H__

#include <map>

#include <NETEC/XUtil.h>
#include <NETEC/XAutoLock.h>

class IXMsgHandle
{
public:
	IXMsgHandle(void);
	virtual~IXMsgHandle(void);
public:
	virtual void RegisterHandle(void);
	virtual void UnRegisterHandle(void);

	virtual void HandleMsg(uint32_t ulMsgID)=0;
	virtual void PostMsg(uint32_t ulMsgID,bool bOnce=true,uint32_t ulDelay=0);
	virtual void ClearMsg(void);
	virtual void RemoveMsg(uint32_t ulMsgID);

	virtual bool CheckMsg(void);
protected:
	struct XMsg
	{
		XMsg(uint32_t ulMsgID,bool bOnce,uint32_t ulDelay)
		:m_ulMsgID(ulMsgID)
		,m_bOnce(bOnce)
		,m_ulDelay(ulDelay)
		,m_ulTimestamp(XGetTimestamp())
		{
		}
		
		XMsg(void)
		:m_ulMsgID(0)
		,m_bOnce(true)
		,m_ulDelay(0)
		,m_ulTimestamp(XGetTimestamp())
		{
		}

		virtual~XMsg(void)
		{
		}
		
		XMsg operator=(const XMsg&tXMsg)
		{
			m_ulMsgID=tXMsg.m_ulMsgID;
			m_bOnce=tXMsg.m_bOnce;
			m_ulDelay=tXMsg.m_ulDelay;
			m_ulTimestamp=tXMsg.m_ulTimestamp;
			return *this;
		}
		
		uint32_t	m_ulMsgID;
		bool			m_bOnce;
		uint32_t	m_ulDelay;
		uint32_t	m_ulTimestamp;
	};

	typedef std::map<uint32_t,XMsg>	MapXMsg;
protected:
	MapXMsg				m_MapXMsg;
	XCritSec			m_csMapXMsg;
};

class  IXMsgQueue
{
public:
	IXMsgQueue(void){};
	virtual ~IXMsgQueue(void){};
public:
	static void Start(void);
	static void Stop(void);
};

#endif