#ifndef __STRPACKET_H__
#define __STRPACKET_H__

#include <map>
#include <string>

#define atoul(str)		strtoul(str,NULL,10)


class StrPacket
{
public:
	StrPacket(bool bA2U=false);
	StrPacket(const std::string& strText,bool bU2A=false);
	virtual~StrPacket(void);
public:
	void Set(const std::string& strName,const std::string& strValue);
	void Set(const std::string& strName,unsigned long nValue);
	void Set(const std::string& strName,unsigned int nValue);
	void Set(const std::string& strName,unsigned short nValue);
	void Set(const std::string& strName,unsigned char nValue);
	bool Get(const std::string& strName,std::string& strValue);
	bool Get(const std::string& strName,unsigned long& nValue);
	bool Get(const std::string& strName,unsigned int& nValue);
	bool Get(const std::string& strName,unsigned short& nValue);
	bool Get(const std::string& strName,unsigned char& nValue);
	void GetString(std::string& strText);
	void SetString(const std::string& strText);
protected:
	void Replace(std::string&strValue,const std::string&strToken,const std::string&strReplaced);
	void Encode(std::string&strValue);
	void Decode(std::string&strValue);
protected:
	typedef std::map<std::string,std::string>	MapNameToValue;

	MapNameToValue	m_MapNameToValue;
	bool			m_bU2A;
	bool			m_bA2U;
};

#endif