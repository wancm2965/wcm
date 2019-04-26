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

	//ɾ��һ����
	virtual void DelKey(const TCHAR* strKey, const TCHAR* strSection);

	//ɾ��һ����
	virtual void DelSection(const TCHAR* strSection);

	//�õ�һ�����еļ�������ֵ��strTag�����ڼ�ֵǰ������Ƿ�ֹ��������"="��������ȷ���ҵ���ֵ��������"*"�����
	virtual void GetKeyNameValue(TCHAR* strName, TCHAR* strValue, CString strTag, TCHAR* strKeyData);

	//�õ�һ�����еļ�������ֵ��Ĭ����"="���ּ����ͼ�ֵ�����ȷ������������"="���ô˺���
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
