#pragma once

//#include "dll.h"
#include "windows.h"

class XYIniReader  
{
public:
	// default constructor
	XYIniReader();
	
	XYIniReader(const TCHAR* strFile);
	
	virtual ~XYIniReader();

	// method to set INI file name, if not already specified 
	virtual void SetINIFileName(const TCHAR* strINIFile);

	// methods to return the lists of section data and section names
	virtual int GetSectionData(const TCHAR* strSection,
						//		char* buf,
							   TCHAR*** buf,
							   DWORD* pNum, 
							   DWORD* pBufSize);
	virtual BOOL GetSectionNames(TCHAR*** buf,
								 DWORD* pNum, 
								 DWORD* pBufSize);
	
	// check if the section exists in the file
	virtual BOOL SectionExists(const char* strSection);

	// updates the key value, if key already exists, else creates a key-value pair
	virtual long SetKey(const TCHAR* strValue, 
				const TCHAR* strKey, 
				const TCHAR* strSection);

	// give the key value for the specified key of a section
	virtual void GetKeyValue(const TCHAR* strKey, 
					 const TCHAR* strSection,
					 TCHAR* strRes,
					 DWORD iSize);

	//删除一个键
	virtual void DelKey(const TCHAR* strKey, const TCHAR* strSection);

	//删除一个节
	virtual void DelSection(const TCHAR* strSection);

	//得到一个键中的键名及键值，strTag用于在键值前面做标记防止键名存在"="，不能正确的找到键值，建议用"*"做标记
	virtual void GetKeyNameValue(TCHAR* strName, TCHAR* strValue, CString strTag, TCHAR* strKeyData);

	//得到一个键中的键名及键值，默认用"="区分键名和键值，如果确定键名不存在"="，用此函数
	virtual void GetKeyNameValue(TCHAR* strName, TCHAR* strValue, TCHAR* strKeyData);

protected:
	virtual BOOL RetriveStrings(const TCHAR* pString, 
							    DWORD dwStrLen, 
//								char *buf,
							    TCHAR*** pppBuf,
							    DWORD* pNum,
							    DWORD* pBufSize);  
	
protected:
	// ini file name 
	wchar_t m_strFileName[512];	
};
