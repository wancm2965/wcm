#pragma once
	
#include <string> 

using namespace std;

class CAttrString
{
public:
	CAttrString(void);
	CAttrString(const char* str);
	CAttrString& operator=(CAttrString& xstr);
	CAttrString& operator=(const char* str);

	~CAttrString(void);

	const char* GetBuffer(void);

	std::string AsString(void);
	unsigned long AsUnsignedLong(void);
	unsigned int AsUnsignedInt(void);
	unsigned short AsUnsignedShort(void);
	unsigned char AsUnsignedChar(void);
	bool AsBoolean(void);

private:
	const char* m_pstr;
};
