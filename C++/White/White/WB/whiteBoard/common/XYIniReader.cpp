#include "StdAfx.h"
#include "XYIniReader.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
XYIniReader::XYIniReader()
{
}

XYIniReader::XYIniReader(const TCHAR* strFile)
{
	SetINIFileName(strFile);
}
	
XYIniReader::~XYIniReader()
{
}

// Used to retrieve a value give the section and key
void XYIniReader::GetKeyValue(const TCHAR* strKey, 
							  const TCHAR* strSection,
							  TCHAR* strRes,
							  DWORD iSize)
{
	// Get the info from the .ini file	
	GetPrivateProfileString(strSection,
		                    strKey,
							L"",
							strRes,
							iSize,
							m_strFileName);	
}

// Used to add or set a key value pair to a section
long XYIniReader::SetKey(const TCHAR* strValue, 
						const TCHAR* strKey, 
						const TCHAR* strSection)
{
	long lRetValue = WritePrivateProfileString (strSection, strKey, 
                             strValue, m_strFileName);

	return lRetValue;
}

// Used to find out if a given section exists
BOOL XYIniReader::SectionExists(const char* strSection)
{
	char ac_Result[100];	
	// Get the info from the .ini file	
	DWORD dwRetValue = GetPrivateProfileString((LPCTSTR)strSection,(LPCTSTR)NULL,
		(LPCTSTR)"",(LPTSTR)ac_Result, 90, (LPCTSTR)m_strFileName);
	// Return if we could retrieve any info from that section
	return (dwRetValue > 0);
}

// Used to retrieve all of the  section names in the ini file
BOOL XYIniReader::GetSectionNames(TCHAR*** buf, 
								  DWORD* pNum, 
								  DWORD* pBufSize)
{
	DWORD dwSize = 8191;
	wchar_t* ac_Result = new wchar_t[dwSize];	
	DWORD dwRetValue = 0;
	
	while (1)
	{
		dwRetValue = GetPrivateProfileSectionNames(ac_Result,
												   dwSize,
												   m_strFileName);
		if (dwRetValue < dwSize - 2)
		{
			break;
		}
		else
		{
			delete[] ac_Result;
			dwSize *= 2;
			ac_Result = new wchar_t[dwSize];
		}
	}

	
	if(dwRetValue > dwSize)
	{
		delete[] ac_Result;
		ac_Result = new wchar_t [dwRetValue+1];
	}
	

	
	BOOL res = RetriveStrings(ac_Result, 
							  dwRetValue, 							  
							  buf,
							  pNum,
							  pBufSize);
	delete[] ac_Result;
	return res;
}

// Used to retrieve all key/value pairs of a given section.  
int XYIniReader::GetSectionData(const TCHAR* strSection,
							//	char *buf,
								TCHAR*** buf,  
							    DWORD* pNum, 
							    DWORD* pBufSize)
{
	DWORD dwSize = 8191;
	wchar_t* ac_Result = new wchar_t[dwSize];
	DWORD dwRetValue = 0;
	while (1)
	{
		dwRetValue = GetPrivateProfileSection((LPCTSTR)strSection, 
											  (LPTSTR)ac_Result, 
											  dwSize, 
											  (LPCTSTR)m_strFileName);
		if (dwRetValue < dwSize - 2)
		{
			break;
		}
		else
		{
			delete[] ac_Result;
			dwSize *= 2;
			ac_Result = new wchar_t[dwSize];
		}
	}

	BOOL res = RetriveStrings(ac_Result, 
							  dwRetValue, 
							  buf,
							  pNum,
							  pBufSize);
	delete[] ac_Result;
	return res;
}

void XYIniReader::SetINIFileName(const TCHAR* strINIFile)
{
	if (strINIFile[1] == ':')
	{
		_tcscpy(m_strFileName, strINIFile);
	}
	else
	{
		char buf[MAX_PATH];
		if (GetCurrentDirectory(MAX_PATH, (LPTSTR)buf))
		{
			wchar_t* str = new wchar_t[strlen(buf)+100];
			wsprintfW(str, L"%s\\%s", buf, strINIFile);
			_tcscpy(m_strFileName, str);
			delete[] str;
		}
		else
		{
			_tcscpy(m_strFileName, strINIFile);
		}
	}	
}

//
// Pick up each string from a 'big' string to given buffer.
//
BOOL XYIniReader::RetriveStrings(const TCHAR* pString, 
							     DWORD dwStrLen, 
								 // char *pbuf,
								 TCHAR*** pppBuf,
							     DWORD* pNum,
							     DWORD* pBufSize)  
{
	DWORD dwCount = 0;
	// Calculate number of strings.
	for(DWORD i=0; i<dwStrLen; i++)
	{
		if (pString[i] == '\0')
		{
			dwCount++;
		}
	}

	// Find maximum string size.
	DWORD max = 0;
	const wchar_t* p = pString;
	for (DWORD i = 0; i < dwCount; i++)
	{
		DWORD len = _tcslen(p);
		if (max < len)
		{
			max = len;
		}
		p += len+1;
	}

	if (*pNum < dwCount || *pBufSize < max+1)
	{
		// Buffer size is too small.
		*pNum = dwCount;
		*pBufSize = max+1;
		return FALSE;
	}
	else
	{
		// Copy every string to buffer.
		p = pString;
		for (DWORD i = 0; i < dwCount; i++)
		{
			int len = _tcslen(p);
			_tcscpy((*pppBuf)[i], p);
	//		strcpy((*pbuf)[i], p);
			p += len+1;
		}
		return TRUE;
	}
}

//删除一个节
void XYIniReader::DelSection(const TCHAR* strSection)
{
	WritePrivateProfileString(strSection, NULL, NULL, m_strFileName);
}

//删除一个键
void XYIniReader::DelKey(const TCHAR* strKey, const TCHAR* strSection)
{
	WritePrivateProfileString(strSection, strKey, NULL, m_strFileName);
}

//得到一个键中的键名及键值，strTag用于在键值前面做标记防止键名存在"="，不能正确的找到键值，建议用"*"做标记
void XYIniReader::GetKeyNameValue(TCHAR* strName, TCHAR* strValue, CString strTag, TCHAR* strKeyData)
{
	CString str, strKeyName, strKeyValue;
	str = strKeyData;
	int nNameLength, nStrLength, nTagLength;	
	nNameLength = str.Find(strTag);	//判断标识符的位置
	nTagLength = strTag.GetLength();
	strKeyName = str.Left(nNameLength - 1);	//提取键名
	_tcscpy(strName, strKeyName);
	nStrLength = str.GetLength();		
	strKeyValue = str.Right(nStrLength - nNameLength - nTagLength);
	_tcscpy(strValue, strKeyValue);
}

//得到一个键中的键名及键值，默认用"="区分键名和键值，如果确定键名不存在"="，用此函数
void XYIniReader::GetKeyNameValue(TCHAR* strName, TCHAR* strValue, TCHAR* strKeyData)
{
	CString str, strKeyName, strKeyValue;
	str = strKeyData;
	int nNameLength, nStrLength;	
	nNameLength = str.Find(L"=");	//判断标识符的位置	
	strKeyName = str.Left(nNameLength - 1);	//提取键名
	_tcscpy(strName, strKeyName);
	nStrLength = str.GetLength();		
	strKeyValue = str.Right(nStrLength - nNameLength - 1);
	_tcscpy(strValue, strKeyValue);
}
