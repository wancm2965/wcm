#ifndef _IAPISERVER_H_
#define _IAPISERVER_H_

#ifdef _WIN32
#ifdef _USEDLL
#ifdef API_EXPORT
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif 
#else
#define API
#endif
#else //_WIN32
#define API __attribute__ ((visibility("default")))
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

namespace apiserver{

	class API IAPIServerNotify
	{
	public:
		virtual ~IAPIServerNotify(){};

		//��ע���API����ʱ
		//handler: http request handler
		//api: webapi path
		//param: json data or string data
		virtual bool OnAPItrigger(void* handler, const char* api, const char* param) = 0;

	};


	class API IAPIServer
	{
	public:
		virtual ~IAPIServer(){}

		//Start Server
		//����ip��ַ�Լ��˿ںţ�һ��ip����Ϊ"0.0.0.0"
		//����ʱʹ��http://ip:port/xxx
		virtual int32_t StartServer(const char* ip,int32_t port) = 0;

		virtual void StopServer() = 0;

		//ע��api��ַ��ֻ��������ע����ĵ�ַ�Żᴥ��OnAPItrigger��Ϣ.��Ҫ��Ϊ�˷�ֹ����Ҫ���¼���ӦӰ��api�ĵ���
		virtual void RegAPIpath(const char* path) = 0;

		//�ظ�ʱ��Ҫʹ��handler
		virtual void AsyncReply(void* handler,const char* reply) = 0;

		//����IAPIServerʵ��
		static IAPIServer* Create(IAPIServerNotify* pNotify);
	};

}



#endif