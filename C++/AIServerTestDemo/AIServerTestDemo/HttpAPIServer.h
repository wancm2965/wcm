#pragma once
#include "APIServer/IAPIServer.h"
#include "AIServerTestDemoDlg.h"
using namespace apiserver;
class CAIServerTestDemoDlg;
class CHttpAPIServer : public IAPIServerNotify
{
public:
	CHttpAPIServer(CAIServerTestDemoDlg* pDlg);
	~CHttpAPIServer();

	virtual bool OnAPItrigger(void* handler, const char* api, const char* param);

	void start();

	void stop();

private:
	IAPIServer*  m_pServer;

	CAIServerTestDemoDlg* m_pDlg;
};

