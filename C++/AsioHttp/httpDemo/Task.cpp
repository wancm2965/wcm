#include "stdafx.h"
#include "Task.h"
#include <string>

CTask::CTask()
{
	m_bStartTask = false;
	m_nCount = 0;
	m_psession = NULL;
	m_pServer = NULL;
	m_phandler = NULL;
}


CTask::~CTask()
{
}

void CTask::StartTask()
{
	m_bStartTask = true;
	StartThread();
}

void CTask::SaveCount(int nCount)
{
	m_nCount = nCount;
}

void CTask::SaveSession(CServerSession* pse)
{
	m_psession = pse;
}

void CTask::StopTask()
{
	m_bStartTask = false;
	WaitForStop();

	if(m_psession)
		m_psession->StopWait();
}

void CTask::ThreadProcMain(void)
{
	while (m_bStartTask)
	{
		int nCount = m_nCount;
		for (int i = 0;i<nCount ;i++)
		{
			std::string strRel = "now processing";
			if (m_pServer)
			{
				//m_pServer->AsyncReply(m_phandler, strRel.c_str());
				int m = 0;
			}
			Sleep(1000);
		}
		//test
		m_bStartTask = false;
	}

	if (m_psession)
		m_psession->StopWait();
}

void CTask::SaveServer(CServer* pServer, void* handler)
{
	m_pServer = pServer;
	m_phandler = handler;
}
