#pragma once
#include "../include/IParamAPI.h"
class CParamAPi : public IPARAMAPI
{
public:
	CParamAPi();
	~CParamAPi();

	virtual int SetParam(const char* key, ...);

	virtual void Close();
};

