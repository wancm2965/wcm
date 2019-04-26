#pragma once

#include "..\include\HttpServer\IHttpServer.h"

class CServer :public IHttpServerNotify
{
public:
	CServer();
	virtual ~CServer();

public:
	virtual bool OnAPItrigger(void*handler, const char*api, const char* param, char**pReply);

	bool StartServer();
	bool StopServer();

	void AsyncReply(void* handler,const char* reply);
private:
	IHttpServer* m_pHttpServer;
};

