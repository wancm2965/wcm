#ifndef __HTTP_HANDLER_H
#define __HTTP_HANDLER_H
#include <string>
#define CURL_STATICLIB
#include "curl/curl.h"

class IHttpHandlerNotify
{
public:
	virtual ~IHttpHandlerNotify(){}
	virtual void OnDataReply(const char* data) = 0;
};

class CHttpHandler
{
public:
	CHttpHandler(IHttpHandlerNotify* pNotify);
	~CHttpHandler();

	static void initHttp();
	static void uninitHttp();
	static int post(const char* url, const char* data, unsigned long timeout);
	static int post_getreply(const char* url, const char* data, unsigned long timeout, std::string& reply);
	int ProcessData(std::string data);
	
private:
	IHttpHandlerNotify* m_pNotify;

public:
	static CHttpHandler& Instance(IHttpHandlerNotify* pNotify);
};

#endif //__HTTP_HANDLER_H