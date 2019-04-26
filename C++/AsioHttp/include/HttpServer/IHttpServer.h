#ifndef _IHTTPSERVER_H_
#define _IHTTPSERVER_H_


#ifdef WIN32
#ifdef HTTPSERVER_EXPORTS
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
#else
#define API
#endif

#if ( defined(_MSC_VER) && _MSC_VER < 1600)
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
#else
#include <stdint.h>
#endif // ( defined(_MSC_VER) && _MSC_VER > 1600)



	class IHttpServerNotify {
	public:
		virtual ~IHttpServerNotify() {};

		virtual bool OnAPItrigger(void*handler, const char*api, const char* param, char**pReply) = 0;
	};
	class  API IHttpServer {
	public:
		virtual ~IHttpServer() {};

		virtual int32_t StartHttpServer(const char* ip, int32_t port) = 0;
		virtual void StopHttpServer() = 0;
		virtual void RegAPIpath(const char* path) = 0;
		virtual void AsyncReply(void* handler, const char* reply) = 0;

		static IHttpServer*Create(IHttpServerNotify*pNotify);

	};



#endif