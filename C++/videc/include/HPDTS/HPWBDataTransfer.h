#pragma once
class IHPNetNotify
{
public:
	//virtual void SendWbData(const char* pData,int iLen) = 0;
	virtual void SendWbData(const char* pData,int iLen,bool bToNet = true) = 0;
};