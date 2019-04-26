#pragma once
#include "XThreadBase.h"
#include "ServerSession.h"
#include "CServer.h"
class CServerSession;
class CTask : public XThreadBase
{
public:
	CTask();
	~CTask();

	void StartTask();
	void SaveCount(int nCount);
	void SaveSession(CServerSession* pse);
	void StopTask();

	virtual void ThreadProcMain(void);
	void SaveServer(CServer* pServer,void* handler);

private:
	bool m_bStartTask;
	int  m_nCount;
	CServerSession* m_psession;
	CServer* m_pServer;
	void*   m_phandler;
};

