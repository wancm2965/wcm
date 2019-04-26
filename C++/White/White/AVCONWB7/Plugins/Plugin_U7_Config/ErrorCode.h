#pragma once

#include <string>
#include <map>

class CErrorCode
{
public:
	CErrorCode(void);
	virtual ~CErrorCode(void);

	static CErrorCode& Instance(void);

public:
	void InitErrorCode(void);
	void Release(void);
	CString	GetErrCodeStr(unsigned long);

protected:
	std::map<unsigned long,CString> m_mapErrCode;

};
