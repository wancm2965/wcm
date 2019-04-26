#include "stdafx.h"
#include "CParamAPi.h"

void PrintInfo(const char* key, va_list vArgs)
{
	int nF = va_arg(vArgs, int);
	const char* sS = va_arg(vArgs, const char*);
	double nT = va_arg(vArgs, double);
	nT = 10;
}
IPARAMAPI* IPARAMAPI::Create()
{
	IPARAMAPI*pParam = nullptr;
	pParam = new CParamAPi();
	return pParam;
}

CParamAPi::CParamAPi()
{

}


CParamAPi::~CParamAPi()
{

}

int CParamAPi::SetParam(const char* key, ...)
{
	va_list args;
	va_start(args, key);
	PrintInfo(key, args);
	va_end(args);
	return 0;
}

void CParamAPi::Close()
{
	delete this;
}
