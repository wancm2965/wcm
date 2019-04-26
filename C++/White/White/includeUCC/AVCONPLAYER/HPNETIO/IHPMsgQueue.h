#ifndef __IXMsgPUMP_H__
#define __IXMsgPUMP_H__

#include "HPBC.h"
#include <map>
#include "IHPEventsPump.h"
//#include "AVUtil.h"
//#include "AVAutoLock.h"


class AFX_EXT_CLASS IHPMsgQueue;
class AFX_EXT_CLASS IHPMsgHandle
{
public:
	IHPMsgHandle(void);
	virtual~IHPMsgHandle(void);
public:
	virtual void HandleMsg(unsigned long ulMsgID)=0;
	virtual void PostMsg(unsigned long ulMsgID,bool bOnce=true,unsigned long ulDelay=0);
	virtual void ClearMsg(void);
	virtual void RemoveMsg(unsigned long ulMsgID);

	virtual bool CheckMsg(void);
protected:
	struct XMsg
	{
		XMsg(unsigned long ulMsgID,bool bOnce,unsigned long ulDelay)
		:m_ulMsgID(ulMsgID)
		,m_bOnce(bOnce)
		,m_ulDelay(ulDelay)
		,m_ulTimestamp(AVGetTimestamp())
		{
		}
		
		XMsg(void)
		:m_ulMsgID(0)
		,m_bOnce(true)
		,m_ulDelay(0)
		,m_ulTimestamp(AVGetTimestamp())
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
		
		unsigned long	m_ulMsgID;
		bool			m_bOnce;
		unsigned long	m_ulDelay;
		unsigned long	m_ulTimestamp;
	};

	typedef std::map<unsigned long,XMsg>	MapXMsg;
protected:
	MapXMsg				m_MapXMsg;
	AVCritSec			m_csMapXMsg;
};

class AFX_EXT_CLASS  IHPMsgQueue
	: public IHPEventsQueue
{
public:
	IHPMsgQueue(void){};
	virtual ~IHPMsgQueue(void){};
public:
	static void CreateInstance(void);
	static void DestroyInstance(void);
	static IHPMsgQueue&Instance(void);
};

#endif