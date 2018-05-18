#include "stdafx.h"
#include "HttpAPIServer.h"
#include <string.h>
#include <string>




using namespace std;
CHttpAPIServer::CHttpAPIServer(CAIServerTestDemoDlg* pDlg)
{
	m_pDlg = pDlg;
	m_pServer = NULL;
}


CHttpAPIServer::~CHttpAPIServer()
{

}

bool CHttpAPIServer::OnAPItrigger(void* handler, const char* api, const char* param)
{
	std::string strApi(api);
	std::string strReply("ok");
	if (strApi.compare(CLBCK) == 0)
	{
		if (m_pDlg)
		{
			m_pDlg->ShowAPIBCKMessage(param);
		}
	}
	else if (strApi.compare(VERBCK) == 0)
	{
		if (m_pDlg)
		{
			m_pDlg->ShowAPIBCKMessage(param);
		}
	}
	m_pServer->AsyncReply(handler, strReply.c_str());
	return true;
}

void CHttpAPIServer::start()
{
	if (NULL == m_pServer)
	{
		m_pServer = IAPIServer::Create(this);
		if (m_pServer)
		{
			std::string strPath = CLBCK;
			m_pServer->RegAPIpath(strPath.c_str());
			strPath = VERBCK;
			m_pServer->RegAPIpath(strPath.c_str());
			std::string strIp = "0.0.0.0";
			int nPort = 4568;
			m_pServer->StartServer(strIp.c_str(), nPort);
		}

	}
}

void CHttpAPIServer::stop()
{
	if (NULL != m_pServer)
	{
		m_pServer->StopServer();
		delete m_pServer;
		m_pServer = NULL;
	}
}
