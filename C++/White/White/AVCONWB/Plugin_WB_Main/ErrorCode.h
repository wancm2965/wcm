#pragma once

#include <string>
#include <map>
#include "RWIniFile.h"
class CErrorCode
{
public:
	CErrorCode(void);
	~CErrorCode(void);

	static CErrorCode& Instance(void);

public:
	void InitErrorCode(void);
	void Release(void);
	CString	GetErrCodeStr(unsigned long);

protected:
	std::map<unsigned long,CString> m_mapErrCode;
};
