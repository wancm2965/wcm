#ifndef _PARAMAPI_H_
#define _PARAMAPI_H_

#ifdef _WIN32
#ifdef PARAMAPI_EXPORTS
#define PARAM_API __declspec(dllexport)
#else
#define PARAM_API __declspec(dllimport)
#endif // VOICE_ENGINE_EXPORT
#else //_WIN32
#define PARAM_API __attribute__ ((visibility("default")))
#endif

#ifdef _WIN32
#pragma warning(disable:4251)
#endif


class PARAM_API IPARAMAPI
{
public:
	IPARAMAPI() {};
	virtual ~IPARAMAPI() {};

	static IPARAMAPI* Create();

	virtual int SetParam(const char* key, ...) = 0;

	virtual void Close() = 0;
};



#endif