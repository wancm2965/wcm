#include "StrPacket.h"
#ifdef _WIN32
#include <ATLCONV.H>
#endif


std::string A2U(const std::string&str)
{
#ifdef WIN32
	USES_CONVERSION;
	wchar_t*wszTemp=A2W(str.c_str());
	
	int nTempLen=WideCharToMultiByte(CP_UTF8,0,wszTemp,-1,NULL,0,NULL,NULL) ;
	char*szTemp=new char[nTempLen+1];
	WideCharToMultiByte(CP_UTF8,0,wszTemp,-1,szTemp,nTempLen,NULL,NULL) ;

	std::string strTemp=szTemp;

	delete[] szTemp;
	szTemp=NULL;

	return strTemp;
#else
	return str;
#endif
}

std::string U2A(const std::string&str)
{
#ifdef WIN32
	int nTempLen=MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,NULL,0);
	wchar_t* wszTemp = new wchar_t[nTempLen+1];
	MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,wszTemp,nTempLen);
	
	USES_CONVERSION;
	std::string strTemp=W2A(wszTemp);

	delete[]wszTemp;
	wszTemp=NULL;

	return strTemp;
#else
	return str;
#endif
}


StrPacket::StrPacket(bool bA2U)
:m_bA2U(bA2U)
,m_bU2A(false)
{
}

StrPacket::StrPacket(const std::string& strText,bool bU2A)
:m_bU2A(bU2A)
,m_bA2U(false)
{
	SetString(strText);
}

StrPacket::~StrPacket(void)
{
	m_MapNameToValue.clear();
}

void StrPacket::Set(const std::string& strName,const std::string& strValue)
{
	if (m_bA2U)
	{
		std::string strTemp=A2U(strValue);
		m_MapNameToValue[strName]=strTemp;
	}
	else
	{
		m_MapNameToValue[strName]=strValue;
	}
}

void StrPacket::Set(const std::string& strName,unsigned long nValue)
{
	char szValue[128]="";
	sprintf(szValue,"%u",nValue);
	m_MapNameToValue[strName]=szValue;
}

void StrPacket::Set(const std::string& strName,unsigned int nValue)
{
	char szValue[128]="";
	sprintf(szValue,"%u",nValue);
	m_MapNameToValue[strName]=szValue;
}

void StrPacket::Set(const std::string& strName,unsigned short nValue)
{
	char szValue[128]="";
	sprintf(szValue,"%u",nValue);
	m_MapNameToValue[strName]=szValue;
}

void StrPacket::Set(const std::string& strName,unsigned char nValue)
{
	char szValue[128]="";
	sprintf(szValue,"%u",nValue);
	m_MapNameToValue[strName]=szValue;
}

bool StrPacket::Get(const std::string& strName,std::string& strValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(strName);
	if (it!=m_MapNameToValue.end())
	{
		if (m_bU2A)
		{
			std::string strTemp=(*it).second;
			Decode(strTemp);

			strValue=U2A(strTemp);
		}
		else
		{
			strValue=(*it).second;

			Decode(strValue);
		}
		return true;
	}
	return false;
}

bool StrPacket::Get(const std::string& strName,unsigned long& nValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(strName);
	if (it!=m_MapNameToValue.end())
	{
		nValue=atoul((*it).second.c_str());
		return true;
	}
	return false;
}

bool StrPacket::Get(const std::string& strName,unsigned int& nValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(strName);
	if (it!=m_MapNameToValue.end())
	{
		nValue=atoul((*it).second.c_str());
		return true;
	}
	return false;
}

bool StrPacket::Get(const std::string& strName,unsigned short& nValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(strName);
	if (it!=m_MapNameToValue.end())
	{
		nValue=atoul((*it).second.c_str());
		return true;
	}
	return false;
}

bool StrPacket::Get(const std::string& strName,unsigned char& nValue)
{
	MapNameToValue::iterator it=m_MapNameToValue.find(strName);
	if (it!=m_MapNameToValue.end())
	{
		nValue=atoul((*it).second.c_str());
		return true;
	}
	return false;
}

void StrPacket::GetString(std::string&strText)
{
	strText="";
	MapNameToValue::iterator it=m_MapNameToValue.begin();
	while (it!=m_MapNameToValue.end())
	{
		strText+="<";

		std::string strName=(*it).first;
		Encode(strName);
		strText+=strName;

		strText+="=";

		std::string strValue=(*it).second;
		Encode(strValue);
		strText+=strValue;

		strText+=">";
		++it;
	}
}

void StrPacket::SetString(const std::string& strText)
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
					
					std::string strName=szName;
					Decode(strName);

					std::string strValue=szValue;
					Decode(strValue);

					Set(strName,strValue);
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

void StrPacket::Replace(std::string&strValue,const std::string&strToken,const std::string&strReplaced)
{
	std::string strDecoded=strValue;
	while(true)
	{
		size_t nPos=strValue.find(strToken);
		if(nPos!=-1)
		{
			strValue.replace(nPos,strToken.size(),strReplaced);
		}
		else
		{
			break;
		}
	}
}

void StrPacket::Encode(std::string&strValue)
{
	Replace(strValue,"=","&equal;");
	Replace(strValue,"<","&left;");
	Replace(strValue,">","&right;");
}

void StrPacket::Decode(std::string&strValue)
{
	Replace(strValue,"&equal;","=");
	Replace(strValue,"&left;","<");
	Replace(strValue,"&right;",">");
}

