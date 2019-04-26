#pragma once
#include "IPlugin/IU7_Meeting/IXMLParser.h"

#define __USE_CHILD_NODE__

class CXMLFunc
{
public:
	CXMLFunc() : m_hDll(NULL)
#ifdef __USE_XMLPARSER__
		, fpLoadXMLFromFile(NULL)
		, fpGetRoot(NULL)
		, fpSave(NULL)
		, fpCreateXMLFile(NULL)
		, fpReleaseParser(NULL)
#endif
		, fpGetXMLString(NULL)
		, fpCreateNode(NULL)
		, fpCreateNodeByXMLStr(NULL)
		, fpReleaseNode(NULL)
		, fpGetNodeType(NULL)
		, fpSetNodeType(NULL)
		, fpGetNodeID(NULL)
		, fpSetAttribute(NULL)
		, fpGetAttribute(NULL)
		, fpSetAttributeInt(NULL)
		, fpGetAttributeInt(NULL)
		, fpSetAttributeUN(NULL)
		, fpGetAttributeUN(NULL)
		, fpSetAttributeBL(NULL)
		, fpGetAttributeBL(NULL)
		, fpSetAttributeDouble(NULL)
		, fpGetAttributeDouble(NULL)
#ifdef __USE_CHILD_NODE__
		, fpGetFirstChild(NULL)
		, fpGetFirstChildByType(NULL)
		, fpGetNextSibling(NULL)
		, fpGetNextSiblingByType(NULL)
		, fpGetLastChild(NULL)
		, fpGetLastChildByType(NULL)
		, fpAddChild(NULL)
		, fpGetChild(NULL)
		, fpDelChild(NULL)
		, fpDelAllChild(NULL)
#endif
	{
		LoadXMLLibrary();
	}

	~CXMLFunc()
	{
		ReleaseLibary();
	}

	bool LoadXMLLibrary()
	{
		// 加载库
		m_hDll = LoadLibrary(_T("XMLParser.dll"));
		if (NULL == m_hDll)
		{
			m_strError = "LoadLibrary XMLParser.dll failed!";
			return false;
		}
		// 加载解析器相关操作
#ifdef __USE_XMLPARSER__
		fpLoadXMLFromFile = (_PLoadXMLFromFile)GetProcAddress(m_hDll, "LoadXMLFromFile");
		if (NULL == fpLoadXMLFromFile)
		{
			m_strError = "Load function LoadXMLFromFile failed!";
			return false;
		}
		fpGetRoot = (_PGetRoot)GetProcAddress(m_hDll, "GetRoot");
		if (NULL == fpGetRoot)
		{
			m_strError = "Load function GetRoot failed!";
			return false;
		}

		fpSave = (_PSave)GetProcAddress(m_hDll, "Save");
		if (NULL == fpSave)
		{
			m_strError = "Load function Save failed!";
			return false;
		}

		fpCreateXMLFile = (_PCreateXMLFile)GetProcAddress(m_hDll, "CreateXMLFile");
		if (NULL == fpCreateXMLFile)
		{
			m_strError = "Load function Create failed!";
			return false;
		}

		fpReleaseParser = (_PReleaseParser)GetProcAddress(m_hDll, "ReleaseParser");
		if (NULL == fpReleaseParser)
		{
			m_strError = "Load function ReleaseParser failed!";
			return false;
		}
#endif

		// 通用操作
		fpGetXMLString = (_PGetXMLString)GetProcAddress(m_hDll, "GetXMLString");
		if (NULL == fpGetXMLString)
		{
			m_strError = "Load function GetXMLString failed!";
			return false;
		}

		// 加载节点相关操作
		fpCreateNode = (_PCreateNode)GetProcAddress(m_hDll, "CreateNode");
		if (NULL == fpCreateNode)
		{
			m_strError = "Load function CreateNode failed!";
			return false;
		}

		fpCreateNodeByXMLStr = (_PCreateNodeByXMLStr)GetProcAddress(m_hDll, "CreateNodeByXMLStr");
		if (NULL == fpCreateNode)
		{
			m_strError = "Load function CreateNodeByXMLStr failed!";
			return false;
		}

		fpReleaseNode = (_PReleaseNode)GetProcAddress(m_hDll, "ReleaseNode");
		if (NULL == fpCreateNode)
		{
			m_strError = "Load function ReleaseNode failed!";
			return false;
		}

		fpGetNodeType = (_PGetNodeType)GetProcAddress(m_hDll, "GetNodeType");
		if (NULL == fpGetNodeType)
		{
			m_strError = "Load function GetNodeType failed!";
			return false;
		}

		fpSetNodeType = (_PSetNodeType)GetProcAddress(m_hDll, "SetNodeType");
		if (NULL == fpSetNodeType)
		{
			m_strError = "Load function SetNodeType failed!";
			return false;
		}

		fpGetNodeID = (_PGetNodeType)GetProcAddress(m_hDll, "GetNodeID");
		if (NULL == fpGetNodeID)
		{
			m_strError = "Load function GetNodeID failed!";
			return false;
		}

		fpSetAttribute = (_PSetAttribute)GetProcAddress(m_hDll, "SetAttribute");
		if (NULL == fpSetAttribute)
		{
			m_strError = "Load function SetAttribute failed!";
			return false;
		}

		fpGetAttribute = (_PGetAttribute)GetProcAddress(m_hDll, "GetAttribute");
		if (NULL == fpGetAttribute)
		{
			m_strError = "Load function GetAttribute failed!";
			return false;
		}

		fpSetAttributeInt = (_PSetAttributeInt)GetProcAddress(m_hDll, "SetAttributeInt");
		if (NULL == fpSetAttributeInt)
		{
			m_strError = "Load function SetAttributeInt failed!";
			return false;
		}

		fpGetAttributeInt = (_PGetAttributeInt)GetProcAddress(m_hDll, "GetAttributeInt");
		if (NULL == fpGetAttributeInt)
		{
			m_strError = "Load function GetAttributeInt failed!";
			return false;
		}

		fpSetAttributeUN = (_PSetAttributeUN)GetProcAddress(m_hDll, "SetAttributeUN");
		if (NULL == fpSetAttributeUN)
		{
			m_strError = "Load function SetAttributeUN failed!";
			return false;
		}

		fpGetAttributeUN = (_PGetAttributeUN)GetProcAddress(m_hDll, "GetAttributeUN");
		if (NULL == fpGetAttributeUN)
		{
			m_strError = "Load function GetAttributeUN failed!";
			return false;
		}

		fpSetAttributeBL = (_PSetAttributeBL)GetProcAddress(m_hDll, "SetAttributeBL");
		if (NULL == fpSetAttributeBL)
		{
			m_strError = "Load function SetAttributeBL failed!";
			return false;
		}

		fpGetAttributeBL = (_PGetAttributeBL)GetProcAddress(m_hDll, "GetAttributeBL");
		if (NULL == fpGetAttributeBL)
		{
			m_strError = "Load function GetAttributeBL failed!";
			return false;
		}

		fpSetAttributeDouble = (_PSetAttributeDouble)GetProcAddress(m_hDll, "SetAttributeDouble");
		if (NULL == fpSetAttributeDouble)
		{
			m_strError = "Load function SetAttributeDouble failed!";
			return false;
		}

		fpGetAttributeDouble = (_PGetAttributeDouble)GetProcAddress(m_hDll, "GetAttributeDouble");
		if (NULL == fpGetAttributeDouble)
		{
			m_strError = "Load function GetAttributeDouble failed!";
			return false;
		}

#ifdef __USE_CHILD_NODE__

		fpGetFirstChild = (_PGetFirstChild)GetProcAddress(m_hDll, "GetFirstChild");
		if (NULL == fpGetFirstChild)
		{
			m_strError = "Load function GetFirstChild failed!";
			return false;
		}

		fpGetFirstChildByType = (_PGetFirstChildByType)GetProcAddress(m_hDll, "GetFirstChildByType");
		if (NULL == fpGetFirstChildByType)
		{
			m_strError = "Load function GetFirstChildByType failed!";
			return false;
		}

		fpGetNextSibling = (_PGetNextSibling)GetProcAddress(m_hDll, "GetNextSibling");
		if (NULL == fpGetNextSibling)
		{
			m_strError = "Load function GetNextSibling failed!";
			return false;
		}

		fpGetNextSiblingByType = (_PGetNextSiblingByType)GetProcAddress(m_hDll, "GetNextSiblingByType");
		if (NULL == fpGetNextSiblingByType)
		{
			m_strError = "Load function GetNextSiblingByType failed!";
			return false;
		}

		fpGetLastChild = (_PGetLastChild)GetProcAddress(m_hDll, "GetLastChild");
		if (NULL == fpGetLastChild)
		{
			m_strError = "Load function GetLastChild failed!";
			return false;
		}

		fpGetLastChildByType = (_PGetLastChildByType)GetProcAddress(m_hDll, "GetLastChildByType");
		if (NULL == fpGetLastChildByType)
		{
			m_strError = "Load function GetLastChildByType failed!";
			return false;
		}

		fpAddChild = (_PAddChild)GetProcAddress(m_hDll, "AddChild");
		if (NULL == fpAddChild)
		{
			m_strError = "Load function AddChild failed!";
			return false;
		}

		fpGetChild = (_PGetChild)GetProcAddress(m_hDll, "GetChild");
		if (NULL == fpGetChild)
		{
			m_strError = "Load function GetChild failed!";
			return false;
		}

		fpDelChild = (_PDelChild)GetProcAddress(m_hDll, "DelChild");
		if (NULL == fpDelChild)
		{
			m_strError = "Load function DelChild failed!";
			return false;
		}

		fpDelAllChild = (_PDelAllChild)GetProcAddress(m_hDll, "DelAllChild");
		if (NULL == fpDelAllChild)
		{
			m_strError = "Load function DelAllChild failed!";
			return false;
		}

#endif

		return true;
	}

	void ReleaseLibary()
	{
		if (m_hDll != NULL)
		{
			FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}

	std::string GetLastError(void) { return m_strError;	}
public:
#ifdef __USE_XMLPARSER__

	_PLoadXMLFromFile fpLoadXMLFromFile;
	_PGetRoot fpGetRoot;
	_PSave fpSave;
	_PReleaseParser fpReleaseParser;
	_PCreateXMLFile fpCreateXMLFile;

#endif

	_PGetXMLString fpGetXMLString;

	// 节点相关操作
	_PCreateNode fpCreateNode;
	_PCreateNodeByXMLStr fpCreateNodeByXMLStr;
	_PReleaseNode fpReleaseNode;

	_PGetNodeType fpGetNodeType;
	_PSetNodeType fpSetNodeType;
	_PGetNodeID fpGetNodeID;

	// 获取属性
	_PSetAttribute fpSetAttribute;
	_PGetAttribute fpGetAttribute;
	_PSetAttributeInt fpSetAttributeInt;
	_PGetAttributeInt fpGetAttributeInt;
	_PSetAttributeUN fpSetAttributeUN;
	_PGetAttributeUN fpGetAttributeUN;
	_PSetAttributeBL fpSetAttributeBL;
	_PGetAttributeBL fpGetAttributeBL;
	_PSetAttributeDouble fpSetAttributeDouble;
	_PGetAttributeDouble fpGetAttributeDouble;

#ifdef __USE_CHILD_NODE__

	_PGetFirstChild fpGetFirstChild;
	_PGetFirstChildByType fpGetFirstChildByType;
	_PGetNextSibling fpGetNextSibling;
	_PGetNextSiblingByType fpGetNextSiblingByType;
	_PGetLastChild fpGetLastChild;
	_PGetLastChildByType fpGetLastChildByType;

	_PAddChild fpAddChild;
	_PGetChild fpGetChild;
	_PDelChild fpDelChild;
	_PDelAllChild fpDelAllChild;

#endif

private:
	std::string				m_strError;
	HMODULE					m_hDll;	// 库句柄
};

static CXMLFunc s_XMLFunc;