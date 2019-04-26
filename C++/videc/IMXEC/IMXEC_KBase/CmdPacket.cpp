#include "stdafx.h"
#include "CmdPacket.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//=======================================================================================
// 命令字符串编解码
//=======================================================================================

//---------------------------------------------------------------------------------------
void CommandStringEncode(const std::string& in,std::string& out)
{
	for(size_t i=0;i<in.length();i++)
	{
		std::string temp;
		switch(in[i])
		{
		case ' ':
			temp = "&space;";
			break;
		case '=':
			temp = "&equal;";
			break;
		case '\r':
			temp = "&return;";
			break;
		case '\n':
			temp = "&newline;";
			break;
		default:
			temp.append(1,in[i]);
		}
		out+=temp;
	}
}

//---------------------------------------------------------------------------------------
void CommandStringDecode(const std::string& in,std::string& out)
{
	out=in;
	while(true)
	{
		std::basic_string <char>::size_type index=out.find("&space;");
		if(index!=-1)
		{
			out.replace(index,7," ");
		}
		else
		{
			break;
		}
	}

	while(true)
	{
		std::basic_string <char>::size_type index=out.find("&equal;");
		if(index!=-1)
		{
			out.replace(index,7,"=");
		}
		else
		{
			break;
		}
	}

	while(true)
	{
		std::basic_string <char>::size_type index=out.find("&return;");
		if(index!=-1)
		{
			out.replace(index,8,"\r");
		}
		else
		{
			break;
		}
	}

	while(true)
	{
		std::basic_string <char>::size_type index=out.find("&newline;");
		if(index!=-1)
		{
			out.replace(index,9,"\n");
		}
		else
		{
			break;
		}
	}
}

//---------------------------------------------------------------------------------------
KCmdItem::KCmdItem(void)
{
}

//---------------------------------------------------------------------------------------
KCmdItem::KCmdItem(const std::string& data)
{
	STRING_MAP element=StrSplit(data," ");
	for(STRING_MAP::iterator it=element.begin();it!=element.end();it++)
	{
		std::string strElement=it->second;
		if(strElement.empty())
		{
			continue;
		}
		STRING_MAP attrib=StrSplit(strElement,"=");
		std::string name  = attrib[0];
		std::string value = attrib[1];
		m_mapAttribs[name] = value;
	}
}

//---------------------------------------------------------------------------------------
KCmdItem::KCmdItem(KCmdItem& rCmdItem)
{
	for(CMD_ATTRIB_MAP::iterator it=rCmdItem.m_mapAttribs.begin();it!=rCmdItem.m_mapAttribs.end();it++)
	{
		std::string name = it->first;
		std::string value = it->second;
		m_mapAttribs[name]=value;
	}
}

//---------------------------------------------------------------------------------------
KCmdItem::~KCmdItem(void)
{
	m_mapAttribs.clear();
}

//---------------------------------------------------------------------------------------
void KCmdItem::SetAttrib(const std::string& name,const std::string& value)
{
	if(value=="" || value.empty())
	{
		return;
	}
	std::string _value = "";
	CommandStringEncode(value,_value);
	m_mapAttribs[name] = _value;
}

//---------------------------------------------------------------------------------------
void KCmdItem::SetAttribUL(const std::string& name,unsigned long value)
{
	if(value)
	{
		SetAttrib(name,ULONG2STR(value));
	}
}

//---------------------------------------------------------------------------------------
void KCmdItem::SetAttribUN(const std::string& name,unsigned int value)
{
	if(value)
	{
		SetAttrib(name,UINT2STR(value));
	}
}

//---------------------------------------------------------------------------------------
void KCmdItem::SetAttribUS(const std::string& name,unsigned short value)
{
	if(value)
	{
		SetAttrib(name,USHORT2STR(value));
	}
}

//---------------------------------------------------------------------------------------
void KCmdItem::SetAttribUC(const std::string& name,unsigned char value)
{
	if(value)
	{
		SetAttrib(name,BYTE2STR(value));
	}
}

//---------------------------------------------------------------------------------------
void KCmdItem::SetAttribBL(const std::string& name,bool value)
{
	if(value)
	{
		SetAttrib(name,"true");
	}
	else
	{
		SetAttrib(name,"flase");
	}
}

//---------------------------------------------------------------------------------------
KString KCmdItem::GetAttrib(const std::string& name)
{
	std::string _value = "";
	CommandStringDecode(m_mapAttribs[name],_value);
	return _value;
}

//---------------------------------------------------------------------------------------
std::string KCmdItem::GetString(void)
{
	std::string attribs="";
	for(CMD_ATTRIB_MAP::iterator it=m_mapAttribs.begin();it!=m_mapAttribs.end();it++)
	{
		std::string name = it->first;
		std::string value = it->second;
		attribs += " " + name + "=" + value;
	}
	return TRIMSPACE(attribs);
}

//=======================================================================================
// 命令包类定义
//=======================================================================================

//---------------------------------------------------------------------------------------
KCmdPacket::KCmdPacket(const std::string& xns,int cmd,const std::string& userid)
:m_strXNS(xns)
,m_nCMD(cmd)
,m_strUserID(userid)
{
}

//---------------------------------------------------------------------------------------
KCmdPacket::KCmdPacket(const char* pData,int nLen)
:m_strXNS("ERR")
,m_strXMLData("")
{
	if(nLen<=1 || pData==NULL)
	{
		return;
	}
	std::string strData(pData);

	//获取第一行
	char* szFirstLine=strtok((char*)strData.c_str(),"\r\n");
	if(szFirstLine==NULL)
	{
		printf("[KBASE] invalid command packet format(errno=0).\n");
		return;
	}
	std::string strFirstLine(szFirstLine);

	//获取余下行
	while(true)
	{
		char* szTemp=strtok(NULL,"\r\n");
		if(szTemp==NULL)
		{
			break;
		}
		SetItem(szTemp);
	}

	//提取命令
	char* szXNS=strtok(szFirstLine," ");
	if(szXNS==NULL)
	{
		printf("[KBASE] invalid command packet format(errno=1).\n");
		return;
	}
	//获取XNS
	m_strXNS = szXNS;
	if (m_strXNS != "<?xml" && *(pData+nLen-1)!='\0')
	{
		//不是XML内容且没有\0
		printf("[KBASE] invalid command packet format(errno=2).\n");
		return;
	}

	if (m_strXNS == "<?xml")
	{
		//是XML内容
		printf("[KBASE] The format is xml.\n");
		m_strXMLData = pData;
		return;
	}

	//获取属性
	while(true)
	{
		char* szTemp=strtok(NULL," ");
		if(szTemp==NULL)
		{
			break;
		}
		std::string strElement(szTemp);
		int n=strElement.find("=");
		std::string name = strElement.substr(0,n);
		std::string value = strElement.substr(n+1,strElement.length()-n-1);
		if(name=="CMD")
		{
			m_nCMD = STR2INT(value);
		}
		else if(name=="USERID")
		{
			m_strUserID = value;
		}
		else
		{
			m_mapAttribs[name] = value;
		}
	}
}

//---------------------------------------------------------------------------------------
KCmdPacket::KCmdPacket(KCmdPacket& rCmdElement)
:m_strXNS(rCmdElement.GetXNS())
,m_nCMD(rCmdElement.GetCMD())
,m_strUserID(rCmdElement.GetUserID())
,m_strXMLData(rCmdElement.GetXMLString())
{
	for(CMD_ATTRIB_MAP::iterator it=rCmdElement.m_mapAttribs.begin();it!=rCmdElement.m_mapAttribs.end();it++)
	{
		std::string name = it->first;
		std::string value = it->second;
		m_mapAttribs[name]=value;
	}
	m_strItems = rCmdElement.GetItemString();
}

//---------------------------------------------------------------------------------------
KCmdPacket::~KCmdPacket(void)
{
	m_mapAttribs.clear();
	ClearAllItems();
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetXNS(const std::string& xns)
{
	m_strXNS = xns;
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetCMD(int cmd)
{
	m_nCMD = cmd;
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetUserID(const std::string& userid)
{
	m_strUserID = userid;
}

//---------------------------------------------------------------------------------------
std::string KCmdPacket::GetXNS(void)
{
	return m_strXNS;
}

//---------------------------------------------------------------------------------------
int KCmdPacket::GetCMD(void)
{
	return m_nCMD;
}

//---------------------------------------------------------------------------------------
std::string KCmdPacket::GetUserID(void)
{
	return m_strUserID;
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetItem(const std::string& item)
{
	m_strItems += item + "\r\n";
}

//---------------------------------------------------------------------------------------
std::string KCmdPacket::GetItemString(void)
{
	return m_strItems;
}

//---------------------------------------------------------------------------------------
CMD_ITEM_LST& KCmdPacket::GetItemList(void)
{
	m_lstItems.clear();
	STRING_MAP line=StrSplit(m_strItems,"\r\n");
	int nCount = line.size();
	for(int i=0;i<nCount;i++)
	{
		std::string strElement=line[i];
		if(strElement.empty())
		{
			continue;
		}
		m_lstItems.push_back(strElement);
	}
	return m_lstItems;
}

//---------------------------------------------------------------------------------------
void KCmdPacket::ClearAllItems(void)
{
	m_strItems = "";
	m_lstItems.clear();
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetAttrib(const std::string& name,const std::string& value)
{
	if(value=="" || value.empty())
	{
		return;
	}

	std::string _value = "";
	CommandStringEncode(value,_value);

	if(name=="CMD")
	{
		m_nCMD = STR2INT(value);
	}
	else if(name=="USERID")
	{
		m_strUserID = _value;
	}
	else
	{
		m_mapAttribs[name] = _value;
	}
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetAttribUL(const std::string& name,unsigned long value)
{
	if(value)
	{
		SetAttrib(name,ULONG2STR(value));
	}
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetAttribUN(const std::string& name,unsigned int value)
{
	if(value)
	{
		SetAttrib(name,UINT2STR(value));
	}
}


//---------------------------------------------------------------------------------------
void KCmdPacket::SetAttribUS(const std::string& name,unsigned short value)
{
	if(value)
	{
		SetAttrib(name,USHORT2STR(value));
	}
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetAttribUC(const std::string& name,unsigned char value)
{
	if(value)
	{
		SetAttrib(name,BYTE2STR(value));
	}
}

//---------------------------------------------------------------------------------------
void KCmdPacket::SetAttribBL(const std::string& name,bool value)
{
	if(value)
	{
		SetAttrib(name,"true");
	}
}

//---------------------------------------------------------------------------------------
KString KCmdPacket::GetAttrib(const std::string& name)
{
	std::string value = "";

	if(name=="CMD")
	{
		CommandStringDecode(INT2STR(m_nCMD),value);
	}
	else if(name=="USERID")
	{
		CommandStringDecode(m_strUserID,value);
	}
	else
	{
		CommandStringDecode(m_mapAttribs[name],value);
	}
	return value;
}

//---------------------------------------------------------------------------------------
std::string KCmdPacket::GetString(void)
{
	//获取命令包字符串
	std::string strAttribs = m_strXNS;
	strAttribs += " CMD=" + INT2STR(m_nCMD);
	if(!m_strUserID.empty())
	{
		strAttribs += " USERID=" + m_strUserID;
	}

	for(CMD_ATTRIB_MAP::iterator it=m_mapAttribs.begin();it!=m_mapAttribs.end();it++)
	{
		std::string name = it->first;
		std::string value = it->second;
		strAttribs += " " + name + "=" + value;
	}
	strAttribs = TRIMSPACE(strAttribs) + "\r\n";

	//获取项目字符串
	strAttribs += m_strItems;

	//返回
	return TRIMSPACE(strAttribs);
}

//---------------------------------------------------------------------------------------
std::string KCmdPacket::GetXMLString(void)
{
	return TRIMSPACE(m_strXMLData);
}

//add 
void KCmdPacket::SetAttribUN2(const std::string& name,unsigned int value)
{
	//if(value)
	{
		SetAttrib(name,UINT2STR(value));
	}
}
