#include "stdafx.h"
//#include "KBASE/String.h"
#include "String.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//=======================================================================================
// KString类实现
//=======================================================================================

//---------------------------------------------------------------------------------------
KString::KString(void)
{
}

//---------------------------------------------------------------------------------------
KString::KString(const std::string& str)
{
	m_string = str;
}

//---------------------------------------------------------------------------------------
KString::~KString(void)
{
}

//---------------------------------------------------------------------------------------
KString& KString::operator=(KString& xstr)
{
	m_string = xstr.m_string;
	return *this;
}

//---------------------------------------------------------------------------------------
KString& KString::operator=(const std::string& str)
{
	m_string = str;
	return *this;
}

//---------------------------------------------------------------------------------------
std::string KString::AsString(void)
{
	return m_string;
}

//---------------------------------------------------------------------------------------
unsigned long KString::AsUnsignedLong(void)
{
	return STR2ULONG(m_string);
}

//---------------------------------------------------------------------------------------
unsigned int KString::AsUnsignedInt(void)
{
	return STR2UINT(m_string);
}

//---------------------------------------------------------------------------------------
unsigned short KString::AsUnsignedShort(void)
{
	return STR2USHORT(m_string);
}

//---------------------------------------------------------------------------------------
unsigned char KString::AsUnsignedChar(void)
{
	return STR2BYTE(m_string);
}

//---------------------------------------------------------------------------------------
bool KString::AsBoolean(void)
{
	return STR2BOOL(m_string);
}

//=======================================================================================
// KBitString类实现
//=======================================================================================

//---------------------------------------------------------------------------------------
KBitString::KBitString(void)
{
    memset(m_szPrivilegeString,0,sizeof(m_szPrivilegeString));
    memset(m_szPrivilegeString,'N',sizeof(m_szPrivilegeString)-1);
}

//---------------------------------------------------------------------------------------
KBitString::KBitString(const char* cszPrivilegeString)
{
    memset(m_szPrivilegeString,0,sizeof(m_szPrivilegeString));
    memcpy(m_szPrivilegeString,cszPrivilegeString,strlen(cszPrivilegeString));
}

//---------------------------------------------------------------------------------------
KBitString::~KBitString(void)
{
}

//---------------------------------------------------------------------------------------
void KBitString::SetPrivilege(int nBit,bool bEnable)
{
    if(nBit>255)
    {
        return;
    }
    char nBitValue=bEnable?'Y':'N';
    m_szPrivilegeString[nBit]=nBitValue;
}

//---------------------------------------------------------------------------------------
bool KBitString::GetPrivilege(int nBit)
{
    if(nBit<0 || nBit>255)
    {
        return false;
    }
    char nBitValue = m_szPrivilegeString[nBit];
    if(nBitValue=='Y')
    {
        return true;
    }
    else
    {
        return false;
    }
}

//---------------------------------------------------------------------------------------
const char* KBitString::GetString(void)
{
    return m_szPrivilegeString;
}