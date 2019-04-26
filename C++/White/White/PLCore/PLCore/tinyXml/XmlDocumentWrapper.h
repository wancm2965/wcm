#pragma once

#include <string>
#include <vector>
#include "tinyxml.h"


typedef void* HANDLE;
namespace TinyXml
{
class CXmlDocumentWrapper
{
public:
	CXmlDocumentWrapper(void);
	~CXmlDocumentWrapper(void);
protected:
	TiXmlDocument m_XmlDocument;
public:
	bool Parse(const char* pstrScr);
	bool LoadDocument(const std::string& strXmlFile);
	bool LoadDocument(FILE* file);

	HANDLE GetRootNode();
	bool GetChildrenNodes(HANDLE& hParaNode,std::vector<HANDLE>& vecNodes);
	bool GetChildrenNodesByName(HANDLE& hParaNode,const std::string& strName,std::vector<HANDLE>& vecNodes);
	bool GetChildrenNodeByName(HANDLE& hParaNode,const std::string& strName,HANDLE& node);
	HANDLE GetFirstChildNode(HANDLE& hParaNode);
	HANDLE GetParentNode(HANDLE& hChildNode);
	HANDLE GetNextChild(HANDLE& hParaNode,HANDLE& hCurNode);
	bool GetNodeAttribute(HANDLE& hNode,const std::string& strAttriName,int& iResult);
	bool GetNodeAttribute(HANDLE& hNode,const std::string& strAttriName,float& fResult);
	bool GetNodeAttribute(HANDLE& hNode,const std::string& strAttriName,std::string& strResult);
	HANDLE CreateNewNode(HANDLE& hNode,const std::string& strNodeName);
	bool SetNodeAttribute(HANDLE& hNode,const std::string& strAttriName,const std::string& strAttriValue);
	bool Save(const std::string& strXmlFile);
	bool RemoveChild(HANDLE& hParNode,HANDLE& hNode);
	bool RemoveChild(HANDLE& hNode);

	std::string GetNodeName(HANDLE& hNode);
	std::string GetNodeText(HANDLE& hNode);
};

}