
#include "XmlDocumentWrapper.h"

namespace TinyXml
{

CXmlDocumentWrapper::CXmlDocumentWrapper(void)
{
}

CXmlDocumentWrapper::~CXmlDocumentWrapper(void)
{
}
bool CXmlDocumentWrapper::Parse(const char* pstrScr)
{
	m_XmlDocument.Parse(pstrScr);

	return true;
}
bool CXmlDocumentWrapper::LoadDocument(const std::string& strXmlFile)
{
	bool bOK=m_XmlDocument.LoadFile(strXmlFile.c_str());
	if(!bOK)
	{
		return false;
	}

	return true;
}

bool CXmlDocumentWrapper::LoadDocument( FILE* file )
{
	bool bOK=m_XmlDocument.LoadFile(file);
	if(!bOK)
	{
		return false;
	}

	return true;
}

HANDLE CXmlDocumentWrapper::GetRootNode()
{
	TiXmlElement *RootElement = m_XmlDocument.RootElement();

	return (void*)RootElement;
}
bool CXmlDocumentWrapper::Save(const std::string& strXmlFile)
{
	if(m_XmlDocument.SaveFile(strXmlFile.c_str()))
		return true;

	return false;
}
std::string CXmlDocumentWrapper::GetNodeName(HANDLE& hNode)
{
	if(!hNode)
		return false;

	const char* pName = ((TiXmlElement*)(void*)hNode)->Value();
	if(pName)
		return std::string(pName);

	return std::string("");
}
std::string CXmlDocumentWrapper::GetNodeText(HANDLE& hNode)
{
	if(!hNode)
		return false;

	const char* pName = ((TiXmlElement*)(void*)hNode)->GetText();
	if(pName)
		return std::string(pName);

	return std::string("");
}
bool CXmlDocumentWrapper::RemoveChild(HANDLE& hNode)
{
	if(!hNode)
		return false;

	HANDLE hParHandle = GetParentNode(hNode);

	return RemoveChild(hParHandle,hNode);
}
bool CXmlDocumentWrapper::RemoveChild(HANDLE& hParNode,HANDLE& hNode)
{
	if(!hParNode || !hNode)
		return false;

	if(!((TiXmlElement*)(void*)hParNode)->RemoveChild((TiXmlElement*)(void*)hNode))
		return false;

	return true;
}
HANDLE CXmlDocumentWrapper::CreateNewNode(HANDLE& hNode,const std::string& strNodeName)
{
	if(!hNode)
		return false;

	TiXmlElement* pNode=(TiXmlElement*)(void*)hNode;

	TiXmlElement xmlElement(strNodeName.c_str());
	TiXmlElement* pNewNode = (TiXmlElement*)(void*)pNode->InsertEndChild(xmlElement);

	if(!pNewNode)
		return NULL;

	return (void*)pNewNode;
}
bool CXmlDocumentWrapper::SetNodeAttribute(HANDLE& hNode,const std::string& strAttriName,const std::string& strAttriValue)
{
	if(!hNode)
		return false;

	TiXmlElement* pNode=(TiXmlElement*)(void*)hNode;

	pNode->SetAttribute(strAttriName.c_str(),strAttriValue.c_str());

	return true;
}
bool CXmlDocumentWrapper::GetChildrenNodes(HANDLE& hParaNode,std::vector<HANDLE>& vecNodes)
{
	if(!hParaNode)
		return false;

	TiXmlElement* pNode=(TiXmlElement*)(void*)hParaNode;

	TiXmlElement *pSubNode = pNode->FirstChildElement();
	while(pSubNode)
	{
		vecNodes.push_back((void*)pSubNode);

		pSubNode=pSubNode->NextSiblingElement();
	}

	return true;
}
bool CXmlDocumentWrapper::GetChildrenNodeByName(HANDLE& hParaNode,const std::string& strName,HANDLE& node)
{
	if(!hParaNode)
		return false;

	TiXmlElement* pNode=(TiXmlElement*)(void*)hParaNode;

	TiXmlElement *pSubNode = pNode->FirstChildElement();
	while(pSubNode)
	{
		if(pSubNode->Value()==strName)
		{
			node=(void*)pSubNode;
			return true;
		}

		pSubNode=pSubNode->NextSiblingElement();
	}

	return false;
}
bool CXmlDocumentWrapper::GetChildrenNodesByName(HANDLE& hParaNode,const std::string& strName,std::vector<HANDLE>& vecNodes)
{
	if(!hParaNode)
		return false;

	TiXmlElement* pNode=(TiXmlElement*)(void*)hParaNode;

	TiXmlElement *pSubNode = pNode->FirstChildElement();
	while(pSubNode)
	{
		if(pSubNode->Value()==strName)
			vecNodes.push_back((void*)pSubNode);

		pSubNode=pSubNode->NextSiblingElement();
	}

	return true;
}
HANDLE CXmlDocumentWrapper::GetFirstChildNode(HANDLE& hParaNode)
{
	if(!hParaNode)
		return NULL;

	TiXmlElement* pNode=(TiXmlElement*)(void*)hParaNode;

	TiXmlElement *pSubNode = pNode->FirstChildElement();
	return (void*)pSubNode;
}
HANDLE CXmlDocumentWrapper::GetParentNode(HANDLE& hChildNode)
{
	if(!hChildNode)
		return NULL;

	TiXmlElement* pNode=(TiXmlElement*)(void*)hChildNode;

	return (void*)(pNode->Parent());

	return NULL;
}
HANDLE CXmlDocumentWrapper::GetNextChild(HANDLE& hParaNode,HANDLE& hCurNode)
{
	return ((TiXmlElement*)hCurNode)->NextSiblingElement();
}
bool CXmlDocumentWrapper::GetNodeAttribute(HANDLE& hNode,const std::string& strAttriName,int& iResult)
{
	if(TIXML_NO_ATTRIBUTE==((TiXmlElement*)hNode)->QueryIntAttribute(strAttriName.c_str(),&iResult))
		return false;
	return true;
}
bool CXmlDocumentWrapper::GetNodeAttribute(HANDLE& hNode,const std::string& strAttriName,float& fResult)
{
	if(TIXML_NO_ATTRIBUTE==((TiXmlElement*)hNode)->QueryFloatAttribute(strAttriName.c_str(),&fResult))
		return false;
	return true;
}
bool CXmlDocumentWrapper::GetNodeAttribute(HANDLE& hNode,const std::string& strAttriName,std::string& strResult)
{
	const char* pResult=((TiXmlElement*)hNode)->Attribute(strAttriName.c_str());
	if(!pResult)
		return false;

	strResult=pResult;
	
	return true;
}	

}