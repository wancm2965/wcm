#pragma once
#include "XMLFunc.h"

// 字符串类，此类本身没什么实际意义，就是为了兼容之前的代码
// 才重新写了这么一个类
class CAttrString
{
public:
	CAttrString(void) : m_pStr(NULL) {}

	CAttrString(const char* str) { this->m_pStr = str; }

	CAttrString& operator=(CAttrString& xstr)
	{	
		if (this != &xstr)
		{
			this->m_pStr = xstr.m_pStr;
		}
		return *this;
	}

	CAttrString& operator=(const char* str)
	{
		this->m_pStr = str;
		return *this;
	}

	~CAttrString(void){};

	const char* GetBuffer(void) { return m_pStr; }

	std::string AsString(void) 
	{ 
		if(m_pStr == NULL)
			return std::string("");

		return std::string(m_pStr); 
	}

	unsigned long AsUnsignedLong(void)
	{
		if(m_pStr == NULL)
			return 0;

		return (unsigned long)_strtoui64(m_pStr, NULL, 10);
	}

	unsigned int AsUnsignedInt(void)
	{
		if(m_pStr == NULL)
			return 0;

		return (unsigned int)_strtoui64(m_pStr, NULL, 10);
	}

	unsigned short AsUnsignedShort(void)
	{
		if(m_pStr == NULL)
			return 0;

		return (unsigned short)atoi(m_pStr);
	}

	unsigned char AsUnsignedChar(void)
	{
		if(m_pStr == NULL)
			return 0;

		return (unsigned char)AsUnsignedInt();
	}

	bool AsBoolean(void)
	{
		if(m_pStr == NULL)
			return false;

		if (strcmp(m_pStr, "true") == 0 
			|| strcmp(m_pStr, "yes") == 0
			|| strcmp(m_pStr, "1") == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	const char* m_pStr;
};


class CCmdPacket
{
public:
	// 用于发送命令
	CCmdPacket(unsigned long ulCmdID)
		: m_pNode(NULL)
		, m_bCreateFromNode(false)
	{
		m_pNode = s_XMLFunc.fpCreateNode("UCCSDK", "");
		s_XMLFunc.fpSetAttributeUN(m_pNode, "nodeid", ulCmdID);
	}

	CCmdPacket(const char* pstrType, unsigned long ulCmdID)
		: m_pNode(NULL)
		, m_bCreateFromNode(false)
	{
		m_pNode = s_XMLFunc.fpCreateNode(pstrType, "");
		s_XMLFunc.fpSetAttributeUN(m_pNode, "nodeid", ulCmdID);
	}

	// 用于数据结构 
	CCmdPacket(const char* pstrType, const char* pstrName)
		: m_pNode(NULL)
		, m_bCreateFromNode(false)
	{
		m_pNode = s_XMLFunc.fpCreateNode(pstrType, pstrName);
	}

	// 用于解析XML， pstrXML：标准XML格式的字符串
	CCmdPacket(const char* pstrXML)
		: m_pNode(NULL)
		, m_bCreateFromNode(false)
	{
		m_pNode = s_XMLFunc.fpCreateNodeByXMLStr(pstrXML);
	}

	~CCmdPacket(void)
	{
		if (!m_bCreateFromNode)
		{
			s_XMLFunc.fpReleaseNode(m_pNode);
		}

		if (m_pNode != NULL)
		{
			m_pNode = NULL;
		}
	}

	// 设置属性
	void SetCMD(const std::string& strCmd);

	void SetAttrib(const char* name,const char* value)
	{
		s_XMLFunc.fpSetAttribute(m_pNode, name, value);
	}

	void SetAttrib(const char* name, int value)
	{
		s_XMLFunc.fpSetAttributeInt(m_pNode, name, value);
	}

	void SetAttrib(const char* name, double value)
	{
		s_XMLFunc.fpSetAttributeDouble(m_pNode, name, value);
	}

	void SetAttrib(const std::string& name, const std::string& value)
	{
		s_XMLFunc.fpSetAttribute(m_pNode, name.c_str(), value.c_str());
	}

	void SetAttribUL(const char* name,unsigned long value)
	{
		s_XMLFunc.fpSetAttributeUN(m_pNode, name, (unsigned int)value);
	}

	void SetAttribUN(const char* name,unsigned int value)
	{
		s_XMLFunc.fpSetAttributeUN(m_pNode, name, value);
	}

	void SetAttribUS(const char* name,unsigned short value)
	{
		s_XMLFunc.fpSetAttributeUN(m_pNode, name, (unsigned int)value);
	}

	void SetAttribUC(const char* name,unsigned char value)
	{
		s_XMLFunc.fpSetAttributeUN(m_pNode, name, (int)value);
	}

	void SetAttribBL(const char* name,bool value)
	{
		s_XMLFunc.fpSetAttributeBL(m_pNode, name, value);
	}


	// 获取属性
	unsigned long GetCmdIDUL(void)
	{
		return (unsigned long)s_XMLFunc.fpGetAttributeUN(m_pNode, "nodeid", 0);
	}

	CAttrString GetAttrib(const char* name, const char* defvalue = "")
	{
		return s_XMLFunc.fpGetAttribute(m_pNode, name, defvalue);
	}

	int GetAttribInt(const char* name, int defvalue = 0)
	{
		return s_XMLFunc.fpGetAttributeInt(m_pNode, name, defvalue);
	}

	double GetAttributeDouble(const char* name, double defvalue = 0)
	{
		return s_XMLFunc.fpGetAttributeDouble(m_pNode, name, defvalue);
	}

	unsigned long GetAttribUL(const char* name,unsigned long defvalue = 0)
	{
		return (unsigned long)s_XMLFunc.fpGetAttributeUN(m_pNode, name, defvalue);
	}

	unsigned int GetAttribUN(const char* name,unsigned int defvalue = 0)
	{
		return s_XMLFunc.fpGetAttributeUN(m_pNode, name, defvalue);
	}

	unsigned short GetAttribUS(const char* name,unsigned short defvalue = 0)
	{
		return (unsigned short)s_XMLFunc.fpGetAttributeUN(m_pNode, name, defvalue);
	}

	unsigned char GetAttribUC(const char* name,unsigned char defvalue = ' ')
	{
		return (unsigned char) s_XMLFunc.fpGetAttributeUN(m_pNode, name, defvalue);
	}

	bool GetAttribBL(const char* name,bool defvalue = false)
	{
		return s_XMLFunc.fpGetAttributeBL(m_pNode, name, defvalue);
	}

	// 获取XML字符串
	const std::string GetString(void)
	{
		return std::string(s_XMLFunc.fpGetXMLString(m_pNode));
	}

	const char* GetXMLString(void)
	{
		return s_XMLFunc.fpGetXMLString(m_pNode);
	}

	//////////////////////////////////////////////////////////////////////////
	// 子节点操作

	// 获取一个子节点
	CCmdPacket GetChild(const char* pstrType, const char* pstrName)
	{
		return CCmdPacket(s_XMLFunc.fpGetChild(m_pNode, pstrType, pstrName));
	}

	// 获取第一个子节点
	CCmdPacket GetFristChild(void)
	{
		return CCmdPacket(s_XMLFunc.fpGetFirstChild(m_pNode));
	}

	CCmdPacket GetFirstChildByType(const char* pstrType)
	{
		return CCmdPacket(s_XMLFunc.fpGetFirstChildByType(m_pNode, pstrType)); 
	}

	// 获取兄弟节点
	CCmdPacket GetNextSiblingItem(void)
	{
		return CCmdPacket(s_XMLFunc.fpGetNextSibling(m_pNode));
	}

	CCmdPacket GetNextSiblingByType(const char* pstrType)
	{
		return CCmdPacket(s_XMLFunc.fpGetNextSiblingByType(m_pNode, pstrType));
	}

	// 获取最后一个子节点
	CCmdPacket GetLastChild(void)
	{
		return CCmdPacket(s_XMLFunc.fpGetLastChild(m_pNode));
	}

	CCmdPacket GetLastChildByType(const char* pstrType)
	{
		return CCmdPacket(s_XMLFunc.fpGetLastChildByType(m_pNode, pstrType));
	}

	// 添加一个子节点
	CCmdPacket AddChild(const char* pstrType, const char* pstrName)
	{
		return CCmdPacket(s_XMLFunc.fpAddChild(m_pNode, pstrType, pstrName));
	}

	// 删除一个子节点
	bool DelChild(const char* pstrType, const char* pstrName)
	{
		return s_XMLFunc.fpDelChild(m_pNode, pstrType, pstrName);
	}

	// 删除全部子节点
	void DelAllChild(void)
	{
		return s_XMLFunc.fpDelAllChild(m_pNode);
	}

	// 此命令包是否有效
	bool IsValid(void)
	{
		return m_pNode != NULL;
	}

	// 获取命令包类型
	const char* GetType(void)
	{
		return s_XMLFunc.fpGetNodeType(m_pNode);
	}

	// 获取命令包名字，命令包标识符
	const char* GetName(void)
	{
		return s_XMLFunc.fpGetNodeID(m_pNode);
	}

protected:
	CCmdPacket(PNODE pNode)
		: m_pNode(pNode)
		, m_bCreateFromNode(true)
	{
	}

	PNODE m_pNode;
	bool m_bCreateFromNode;
};