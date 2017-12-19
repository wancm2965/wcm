﻿#pragma once

#include "KBASE/KBASE_Export.h"
#include <string>

//---------------------------------------------------------------------------------------
// Guid类定义
// 生成128位GUID(Generate 128-bit globally unique identifier).
class KBASE_API Guid
{
public:
	Guid();
	Guid(const std::string& strGuid);
	Guid(unsigned char* pBuffer);
	~Guid();

	std::string GetGuid(void);
	const unsigned char* GetBuffer(void);

private:
	Guid(const Guid& rGuid){};
	Guid& operator=(const Guid& rGuid){return *this;};
	unsigned char m_pBuffer[16];
};
