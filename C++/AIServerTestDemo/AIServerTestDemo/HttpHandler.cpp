
#include "stdafx.h"
#include "HttpHandler.h"


size_t callback(void *ptr, size_t size, size_t nmemb, void *stream)  
{  
	CHttpHandler* pHandler = (CHttpHandler*)stream;
	if(pHandler)
	{
		std::string data((const char*) ptr, (size_t) size * nmemb);
		pHandler->ProcessData(data);
	}

	return size * nmemb; 
}

size_t callback_write_strdata(void *ptr, size_t size, size_t nmemb, void *stream)
{
	/*std::string data((const char*)ptr, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data << std::endl;*/
	return size * nmemb; 
}

CHttpHandler::CHttpHandler(IHttpHandlerNotify* pNotify)
	: m_pNotify(pNotify)
{

}

CHttpHandler::~CHttpHandler()
{

}

//必须在主线程初始化，否则线程不安全
//参考文档https://curl.haxx.se/libcurl/c/curl_global_init.html
void CHttpHandler::initHttp()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

void CHttpHandler::uninitHttp()
{
	curl_global_cleanup();
}

int CHttpHandler::post(const char* url, const char* data, unsigned long timeout)
{
	static int count = 0;
	struct curl_slist* header = NULL;
	CURL *curl;  
	CURLcode res = CURLE_FAILED_INIT;
	curl = curl_easy_init();  
	if (curl) {  
		header = curl_slist_append(header, "Content-Type: application/json");  
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
		curl_easy_setopt(curl, CURLOPT_URL, url);  
		curl_easy_setopt(curl, CURLOPT_POST, 1L);  
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 3000L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, /*CCommStringFunc::urlEncode*/(data));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write_strdata);
		res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
		}
		curl_slist_free_all(header);
		curl_easy_cleanup(curl);  
	} 
	
	return res;
}

int CHttpHandler::ProcessData(std::string data)
{
	if (m_pNotify)
	{
		m_pNotify->OnDataReply(data.c_str());
	}
	return 0;
}

//多线程使用curl时需要设置CURLOPT_NOSIGNAL为1 否则线程不安全...
//参考https://curl.haxx.se/libcurl/c/CURLOPT_NOSIGNAL.html
//curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
/*
int CHttpHandler::post_reply(const char* url, const char* data)
{
	CURL *curl;  
	CURLcode res = CURLE_FAILED_INIT;
	std::stringstream out;
	curl = curl_easy_init();  
	if (curl) {  
		curl_easy_setopt(curl, CURLOPT_URL, url);  
		curl_easy_setopt(curl, CURLOPT_POST, 1L);  
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 1000);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);  
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA,this);
		res = curl_easy_perform(curl);  
		curl_easy_cleanup(curl);  
	} 
	return res;
}

int CHttpHandler::get(const char* url, std::string& strJson)
{
	CURL *curl = NULL;  
	CURLcode res;
	std::stringstream out;
	curl = curl_easy_init(); 
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 1000);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write_strdata); //设置下载数据的回调函数  
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out); 
	res = curl_easy_perform(curl);
	if(res != CURLE_OK)  
	{  
		fprintf(stdout,"curl execute fail\n");  
	}
	strJson = out.str(); 

	curl_easy_cleanup(curl);  

	return 0;
}

int CHttpHandler::post(const char* url, const char* data)
{
	KAutoLock l(s_sec);
	::OutputDebugStringA("CHttpHandler::post_------------begin\r\n");
	CURL *curl;
	CURLcode res = CURLE_FAILED_INIT;
	std::stringstream out;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 1000);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	::OutputDebugStringA(("CHttpHandler::post_------------end\r\n"));
	return res;
}

int CHttpHandler::put(const char* url, const char* data)
{
	CURL *curl;
	CURLcode res = CURLE_FAILED_INIT;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return res;
}*/

int CHttpHandler::post_getreply(const char* url, const char* data,unsigned long timeout, std::string& reply)
{
	
	static int count = 0;
	
	CURL *curl;  
	CURLcode res = CURLE_FAILED_INIT;
	//std::stringstream out;
	curl = curl_easy_init();  
	if (curl) {  
		curl_easy_setopt(curl, CURLOPT_URL, url);  
		curl_easy_setopt(curl, CURLOPT_POST, 1L);  
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 200L); //200ms
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout); //200ms
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);  
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write_strdata);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
		res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			//reply = out.str();
		}
		curl_easy_cleanup(curl);
		if (res == CURLE_OPERATION_TIMEDOUT)
		{
			
		}
	} 
	
	return res;
}

CHttpHandler& CHttpHandler::Instance(IHttpHandlerNotify* pNotify)
{
	static CHttpHandler inst(pNotify);
	return inst;
}
