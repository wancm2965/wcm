#pragma once
class IHPWBDataNotify
{
public:
	virtual void ProcessWBDataOut(const char* pData,int iLen) = 0;
};