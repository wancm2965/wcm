#ifndef __XMEMINI_H__
#define __XMEMINI_H__

#include <map>
#include <string>

#define atoul(str)		strtoul(str,NULL,10)


typedef std::map<unsigned int,std::string>	MapNameToValue;
class XMemIni
{
public:
	XMemIni(void);
	XMemIni(const std::string& strText);
	~XMemIni(void);
public:
	void Set(unsigned int iKey,std::string strValue);
	void Set(unsigned int iKey,unsigned long ulValue);
	void Set(unsigned int iKey,unsigned short usValue);
	void Set(unsigned int iKey,float usValue);
	void Set(unsigned int iKey,int iValue);
	bool Get(unsigned int iKey,std::string& strValue);
	bool Get(unsigned int iKey,unsigned long& ulValue);
	bool Get(unsigned int iKey,unsigned short& usValue);
	bool Get(unsigned int iKey,float& usValue);
	bool Get(unsigned int iKey,int& iValue);
	void GetString(std::string& strText);
	void SetString(const std::string& strText);
	bool DeleteKey(unsigned int iKey);
	bool DeleteAllKey();
	void DeleteKeys(std::string strKey);
	void ResetPos();
	bool Next(unsigned int& iKey,std::string& strValue);
	int GetSize();
	void ResetString(const std::string& strText);
	void GetSubString(char* strKey,int iLen,std::string& strText);
protected:
	MapNameToValue	m_MapNameToValue;
	MapNameToValue::iterator m_it;
};

#endif