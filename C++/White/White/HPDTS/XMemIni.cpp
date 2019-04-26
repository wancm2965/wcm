#include "stdafx.h"
#include "XMemIni.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



XMemIni::XMemIni(void)
{
	ResetPos();
}

XMemIni::XMemIni(const std::string& strText)
{
	SetString(strText);
}

XMemIni::~XMemIni(void)
{
	m_MapNameToValue.clear();
}

bool XMemIni::DeleteKey(unsigned int iKey)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(iKey);
	if (it != m_MapNameToValue.end())
	{
// 		if (m_it == it)
// 		{
// 			m_it++;
// 			if (m_it == m_MapNameToValue.end())
// 			{
// 				m_it = m_MapNameToValue.begin();
// 			}
// 		}
		m_MapNameToValue.erase(it);
		return true;
	}
	return false;
}

void XMemIni::DeleteKeys(std::string strKey)
{
	int iDataLength = strKey.size();
	char* pBuffer = new char[iDataLength];
	if (pBuffer)
	{
		memset(pBuffer,0,iDataLength);
		memcpy(pBuffer,strKey.c_str(),iDataLength);
		int iPos = 0;
		while (iPos<iDataLength)
		{
			int iKey = *((int*)(pBuffer + iPos));
			DeleteKey(iKey);
			iPos += sizeof(int);
		}
		delete []pBuffer;
		pBuffer = NULL;
	}
}

bool XMemIni::DeleteAllKey()
{
	if (m_MapNameToValue.size() == 0)
	{
		return false;
	}
	else
	{
		m_MapNameToValue.clear();
		return true;
	}
	return true;
}

void XMemIni::Set(unsigned int iKey,std::string strValue)
{
	m_MapNameToValue[iKey]=strValue;
}

void XMemIni::Set(unsigned int iKey,unsigned long ulValue)
{
	char szValue[128]="";
	sprintf(szValue,"%u",ulValue);
	m_MapNameToValue[iKey]=szValue;
}

void XMemIni::Set(unsigned int iKey,int iValue)
{
	char szValue[128]="";
	sprintf(szValue,"%d",iValue);
	m_MapNameToValue[iKey]=szValue;
}
void XMemIni::Set(unsigned int iKey,unsigned short usValue)
{
	char szValue[128]="";
	sprintf(szValue,"%d",usValue);
	m_MapNameToValue[iKey]=szValue;
}

void XMemIni::Set(unsigned int iKey,float usValue)
{
	char szValue[128]="";
	sprintf(szValue,"%f",usValue);
	m_MapNameToValue[iKey]=szValue;
}
bool XMemIni::Get(unsigned int iKey,std::string& strValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(iKey);
	if (it!=m_MapNameToValue.end())
	{
		strValue=(*it).second;
		return true;
	}
	return false;
}

bool XMemIni::Get(unsigned int iKey,int& iValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(iKey);
	if (it!=m_MapNameToValue.end())
	{
		iValue=atoi((*it).second.c_str());
		return true;
	}
	return false;
}
bool XMemIni::Get(unsigned int iKey,unsigned long& ulValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(iKey);
	if (it!=m_MapNameToValue.end())
	{
		ulValue=atoul((*it).second.c_str());
		return true;
	}
	return false;
}

bool XMemIni::Get(unsigned int iKey,unsigned short& usValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(iKey);
	if (it!=m_MapNameToValue.end())
	{
		usValue=atoul((*it).second.c_str());
		return true;
	}
	return false;
}

bool XMemIni::Get(unsigned int iKey,float& usValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(iKey);
	if (it!=m_MapNameToValue.end())
	{
		usValue=atof((*it).second.c_str());
		return true;
	}
	return false;
}
void XMemIni::GetString(std::string&strText)
{
	strText="";
	MapNameToValue::iterator it=m_MapNameToValue.begin();
	while (it!=m_MapNameToValue.end())
	{
		char strKey[32] = {0};
		//itoa((*it).first,strKey,10);
		sprintf(strKey,"%u",(*it).first);

		strText+="<";
		strText+=strKey;
		strText+="=";
		strText+=(*it).second;
		strText+=">";
		++it;
	}
}

void XMemIni::GetSubString(char* strKey,int iLen,std::string& strText)
{
	strText = "";
	//char strBuffer[256] = {0};
	//memcpy(strBuffer,strKey.c_str(),strKey.size());
	int iSize = iLen;//strKey.size();
	int iPos = 0;
	int iKey = 0;
	std::string strValue = "";
	while(iPos<iSize)
	{
		iKey = *((int*)(strKey + iPos));
		if (Get(iKey,strValue))
		{
			char strCmd[32] = {0};
			//itoa(iKey,strCmd,10);
			sprintf(strCmd,"%u",iKey);

			strText+="<";
			strText+=strCmd;
			strText+="=";
			strText+=strValue;
			strText+=">";
		}
		iPos+=sizeof(int);
	}
}

void XMemIni::ResetString(const std::string& strText)
{
	int nStrLen=strText.length();
	if (nStrLen<=0)
		return;

	char*szText=(char*)malloc(nStrLen+2);
	if (szText==NULL)
		return;

	strcpy(szText,strText.c_str());

	char*szTemp=szText;
	m_MapNameToValue.clear();
	while(szTemp)
	{
		if (*szTemp=='<')
		{
			char*szName=szTemp+1;
			while(szTemp && *szTemp!='='){szTemp++;};
			if (*szTemp=='=')
			{
				char*szValue=szTemp+1;
				*szTemp='\0';
				szTemp=szValue;
				while(szTemp && *szTemp!='>'){szTemp++;};
				if (*szTemp=='>')
				{
					char*szNext=szTemp+1;
					*szTemp='\0';
					szTemp=szNext;

					Set(atoi(szName),szValue);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	free(szText);
	szText=NULL;
}

void XMemIni::SetString(const std::string& strText)
{
	int nStrLen=strText.length();
	if (nStrLen<=0)
		return;

	char*szText=(char*)malloc(nStrLen+2);
	if (szText==NULL)
		return;

	strcpy(szText,strText.c_str());

	char*szTemp=szText;
	//m_MapNameToValue.clear();
	while(szTemp)
	{
		if (*szTemp=='<')
		{
			char*szName=szTemp+1;
			while(szTemp && *szTemp!='='){szTemp++;};
			if (*szTemp=='=')
			{
				char*szValue=szTemp+1;
				*szTemp='\0';
				szTemp=szValue;
				while(szTemp && *szTemp!='>'){szTemp++;};
				if (*szTemp=='>')
				{
					char*szNext=szTemp+1;
					*szTemp='\0';
					szTemp=szNext;
					
					Set(atoi(szName),szValue);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	free(szText);
	szText=NULL;
}

void XMemIni::ResetPos()
{
	m_it = m_MapNameToValue.begin();
}

bool XMemIni::Next(unsigned int& strName,std::string& strValue)
{
	if (m_it != m_MapNameToValue.end())
	{
		strName = m_it->first;
		strValue = m_it->second;
		m_it++;
		return true;
	}
	else
	{
		m_it = m_MapNameToValue.begin();
	}
	return false;
}

int XMemIni::GetSize()
{
	return m_MapNameToValue.size();
}