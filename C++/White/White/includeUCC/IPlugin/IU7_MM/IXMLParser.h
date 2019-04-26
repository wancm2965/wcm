#ifndef __IXMLPARSER_H__
#define __IXMLPARSER_H__

#define PXMLPARSER void*
#define PNODE void*
enum
{
	XMLFILE_ENCODING_UNKNOWN,
	XMLFILE_ENCODING_UTF8,
	XMLFILE_ENCODING_LEGACY
};

const int XMLFILE_DEFAULT_ENCODING = XMLFILE_ENCODING_UNKNOWN;

#ifdef XMLPARSER_EXPORTS
#define XMLPARSER_API  extern "C"__declspec(dllexport) 

//////////////////////////////////////////////////////////////////////////
// ÿ���ļ���Ӧһ��������

/************************************************************************
* LoadXMLFromFile
* ���������ļ��н���XML
* ����: const char* strFilePath �ļ�·��
* ����: int encoding ���뷽ʽ
* ����ֵ: ������ָ��
************************************************************************/
XMLPARSER_API PXMLPARSER LoadXMLFromFile(const char* pstrFilePath, int encoding = XMLFILE_DEFAULT_ENCODING);


/************************************************************************
* LoadXMLFromString
* ���������ַ����н���XML
* ����:const char* pstrXML XML�ַ���
* ����ֵ: ������ָ��
************************************************************************/
XMLPARSER_API PXMLPARSER LoadXMLFromString(const char* pstrXML);


/************************************************************************
* CreateXML
* ����������һ��XML�ļ�������ʱָ��·����
* ����: const char * pstrVersion	�汾��
* ����: const char * pstrEncoding	���뷽ʽ("utf-8")
* ����: const char * pstrStandalone	�Ƿ������ͨ��Ϊ"yes"��
* ����ֵ: ������ָ��
************************************************************************/
XMLPARSER_API PXMLPARSER CreateXMLFile(const char* pstrVersion, const char* pstrEncoding, const char* pstrStandalone);


/************************************************************************
* Save
* ����������һ��XML�ļ�
* ����: PXMLPARSER pXMLParser	������ָ��
* ����: const char * pstrFilePath ·��
* ����ֵ: �Ƿ�ɹ�
************************************************************************/
XMLPARSER_API bool Save(PXMLPARSER pXMLParser, const char* pstrFilePath);



/************************************************************************
* GetParserLastError
* �������������һ��������Ϣ(ֻ�����ڽ���������)
* ����ֵ: ������Ϣ
************************************************************************/
XMLPARSER_API const char* GetParserLastError(void);


/************************************************************************
* Release
* �������ͷŽ�����(��Ҫ������)
* ����: PXMLPARSER pXMLParser	������ָ��
* ����ֵ: void
************************************************************************/
XMLPARSER_API void ReleaseParser(PXMLPARSER pXMLParser);


/************************************************************************
* GetRoot
* �������õ����ڵ�
* ����: PXMLPARSER pXMLParser	������ָ��
* ����ֵ: ���ڵ�ָ��
************************************************************************/
XMLPARSER_API PNODE GetRoot(PXMLPARSER pXMLParser);


/************************************************************************
* GetXMLString
* ��������ȡ�ڵ��XML��ʽ���ַ���
* ����: PNODE pNode	�ڵ�ָ��������ָ��
* ����ֵ: XML�ַ���
************************************************************************/
XMLPARSER_API const char* GetXMLString(PNODE pNode);


/************************************************************************
* GetXMLStringLength
* ��������ȡ�ڵ��XML��ʽ���ַ����ĳ���(����GetXMLString֮�������ȷ��ã�
* ����: PNODE pNode �ڵ�ָ��
* ����ֵ: �ڵ��ַ�������
************************************************************************/
XMLPARSER_API int GetXMLStringLength(PNODE pNode);


/************************************************************************
* CreateXMLParser
* ����������һ���ڵ�
* ����: const char * pstrNodeType	�ڵ����ͣ����ڵ��Զ���ɣ�
* ����: const char * pstrNodeID		�ڵ�ID
* ����ֵ: �ڵ�ָ��
************************************************************************/
XMLPARSER_API PNODE CreateNode(const char* pstrNodeType, const char* pstrNodeID);


/************************************************************************
* CreateNodeByXMLStr
* ����������һ��XML�������������������ļ���ֻ�������ݶ�ȡ��
* ����: const char * pstrNodeType	��һ���ӽڵ����ͣ����ڵ��Զ���ɣ�
* ����: const char * pstrNodeID		�ӽڵ�ID
* ����ֵ: �ڵ�ָ��
************************************************************************/
XMLPARSER_API PNODE CreateNodeByXMLStr(const char* pstrXML);


/************************************************************************
* ReleaseNode
* �������ͷ�һ���ڵ㣨���ڵ�ͨ��Create���������Ϊ��Ҫ��������
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: void
************************************************************************/
XMLPARSER_API void ReleaseNode(PNODE pNode);


/************************************************************************
* GetNodeType
* ��������ȡ�ڵ�����
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: �ڵ������ַ���
************************************************************************/
XMLPARSER_API const char* GetNodeType(PNODE pNode);


/************************************************************************
* SetNodeType
* ���������ýڵ�����
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strType �ڵ�����
* ����ֵ: void
************************************************************************/
XMLPARSER_API void SetNodeType(PNODE pNode, const char* pstrType);


/************************************************************************
* GetNodeID
* ��������ȡ�ڵ�ID
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: �ڵ�ID
************************************************************************/
XMLPARSER_API const char* GetNodeID(PNODE pNode);


/************************************************************************
* SetNodeID
* ���������ýڵ�ID
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strNodeID	�ڵ�����
* ����ֵ: void
************************************************************************/
XMLPARSER_API void SetNodeID(PNODE pNode, const char* pstrNodeID);


/************************************************************************
* IsRoot
* �������жϽڵ��Ƿ��Ǹ��ڵ�
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: bool
************************************************************************/
XMLPARSER_API bool IsRoot(PNODE pNode);


/************************************************************************
* GetParentNode
* ��������ȡ�ڵ�ĸ��ڵ�
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: PNODE		���ڵ�ָ��
************************************************************************/
XMLPARSER_API PNODE GetParentNode(PNODE pNode);


/************************************************************************
* GetAttribute
* ��������ȡ�ڵ�ĳ������
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strAttrName		������
* ����: const char* strDefValue		Ĭ������ֵ������޷���ȡ��ֵ���򷵻�Ĭ��ֵ��
* ����ֵ: const char*	����ֵ
************************************************************************/
XMLPARSER_API const char* GetAttribute(PNODE pNode, const char* pstrAttrName, const char* pstrDefValue);


/************************************************************************
* SetAttribute
* ���������ýڵ��ĳ������ֵ(���û�и������򴴽�)
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strAttrName		������
* ����: const char* strAttrValue	����ֵ
* ����ֵ: void
************************************************************************/
XMLPARSER_API void SetAttribute(PNODE pNode, const char* pstrAttrName, const char* pstrAttrValue);


/************************************************************************
* SetAttribute
* ���������ýڵ��ĳ��unsigned int���͵�����ֵ(���û�и������򴴽�)
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strAttrName		������
* ����: unsigned int ulValue	����ֵ
* ����ֵ: void
************************************************************************/
XMLPARSER_API void SetAttributeUN(PNODE pNode, const char* pstrAttrName, unsigned long ulValue);


/************************************************************************
* GetAttribute
* ��������ȡ�ڵ�ĳ��unsigned int����
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strAttrName		������
* ����: unsigned int unDefValue		Ĭ������ֵ������޷���ȡ��ֵ���򷵻�Ĭ��ֵ��
* ����ֵ: unsigned int unDefValue
************************************************************************/
XMLPARSER_API unsigned int GetAttributeUN(PNODE pNode, const char* pstrAttrName, unsigned int unDefValue);


/************************************************************************
* SetAttributeInt
* ���������ýڵ��ĳ��int���͵�����ֵ(���û�и������򴴽�)
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strAttrName		������
* ����: int nValue	����ֵ
* ����ֵ: void
************************************************************************/
XMLPARSER_API void SetAttributeInt(PNODE pNode, const char* pstrAttrName, int nValue);


/************************************************************************
* GetAttributeInt
* ��������ȡ�ڵ�ĳ��int����
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char * pstrAttrName ������
* ����: int nDefValue	Ĭ������ֵ
* ����ֵ: XMLPARSER_API int
************************************************************************/
XMLPARSER_API int GetAttributeInt(PNODE pNode, const char* pstrAttrName, int nDefValue);


/************************************************************************
* SetAttributeBL
* ���������ýڵ��ĳ��bool���͵�����ֵ(���û�и������򴴽�)
* ����: PNODE pNode
* ����: const char * pstrAttrName ������
* ����: bool bValue	Ĭ������ֵ
* ����ֵ: void
************************************************************************/
XMLPARSER_API void SetAttributeBL(PNODE pNode, const char* pstrAttrName, bool bValue);


/************************************************************************
* GetAttribute
* ��������ȡ�ڵ�ĳ������
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strAttrName		������
* ����: bool strDefValue		Ĭ������ֵ������޷���ȡ��ֵ���򷵻�Ĭ��ֵ��
* ����ֵ: bool	
************************************************************************/
XMLPARSER_API bool GetAttributeBL(PNODE pNode, const char* pstrAttrName, bool bDefValue);


/************************************************************************
* SetAttributeBL
* ���������ýڵ��ĳ��bool���͵�����ֵ(���û�и������򴴽�)
* ����: PNODE pNode
* ����: const char * pstrAttrName ������
* ����: bool bValue	Ĭ������ֵ
* ����ֵ: void
************************************************************************/
XMLPARSER_API void SetAttributeDouble(PNODE pNode, const char* pstrAttrName, double dValue);


/************************************************************************
* GetAttribute
* ��������ȡ�ڵ�ĳ������
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strAttrName		������
* ����: bool strDefValue		Ĭ������ֵ������޷���ȡ��ֵ���򷵻�Ĭ��ֵ��
* ����ֵ: bool	
************************************************************************/
XMLPARSER_API double GetAttributeDouble(PNODE pNode, const char* pstrAttrName, double dDefValue);


/************************************************************************
* DelAttribute
* ������ɾ���ڵ��ĳ������
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strAttrName	������
* ����ֵ: void
************************************************************************/
XMLPARSER_API void DelAttribute(PNODE pNode, const char* pstrAttrName);


/************************************************************************
* GetFirstChild
* ��������ȡ��һ���ӽڵ�
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: PNODE
************************************************************************/
XMLPARSER_API PNODE GetFirstChild(PNODE pNode);


/************************************************************************
* GetFirstChild
* ��������ȡĳ�����͵ĵ�һ���ӽڵ�
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strType		�ڵ�����
* ����ֵ: PNODE
************************************************************************/
XMLPARSER_API PNODE GetFirstChildByType(PNODE pNode, const char* pstrType);


/************************************************************************
* GetNextSibling
* ��������ȡĳ���ڵ����һ��ͬ���ڵ�
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: PNODE
************************************************************************/
XMLPARSER_API PNODE GetNextSibling(PNODE pNode);


/************************************************************************
* GetNextSibling
* ��������ȡĳ�����͵Ľڵ���һ��ͬ���ڵ�
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strType		�ڵ�����
* ����ֵ: int
************************************************************************/
XMLPARSER_API PNODE GetNextSiblingByType(PNODE pNode, const char* pstrType);


/************************************************************************
* GetFirstChild
* ��������ȡ���һ���ӽڵ�
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: PNODE
************************************************************************/
XMLPARSER_API PNODE GetLastChild(PNODE pNode);


/************************************************************************
* GetFirstChild
* ��������ȡĳ�����͵����һ���ӽڵ�
* ����: PNODE pNode	�ڵ�ָ��
* ����: const char* strType		�ڵ�����
* ����ֵ: PNODE
************************************************************************/
XMLPARSER_API PNODE GetLastChildByType(PNODE pNode, const char* pstrType);


/************************************************************************
* GetChild
* ���������ݽڵ����ͣ��ڵ�ID��ȡһ���ӽڵ�
* ����: PNODE pNode		�ڵ�ָ��
* ����: const char* strType		�ڵ�����
* ����: const char* strNodeID	�ڵ�ID
* ����ֵ: PNODE		�ӽڵ�ָ��
************************************************************************/
XMLPARSER_API PNODE GetChild(PNODE pNode, const char* pstrType, const char* pstrNodeID);


/************************************************************************
* AddChild
* ����������һ���ӽڵ�
* ����: PNODE pNode		�ڵ�ָ��
* ����: const char* strType		�ڵ�����
* ����: const char* strNodeID	�ڵ�ID
* ����ֵ: PNODE	�ӽڵ�ָ��
************************************************************************/
XMLPARSER_API PNODE AddChild(PNODE pNode, const char* pstrType, const char* pstrNodeID);


/************************************************************************
* DelChild
* ������ɾ��һ���ӽڵ�
* ����: PNODE pNode		�ڵ�ָ��
* ����: const char* strType		�ڵ�����
* ����: const char* strNodeID	�ڵ�ID
* ����ֵ: bool
************************************************************************/
XMLPARSER_API bool DelChild(PNODE pNode, const char* pstrType, const char* pstrNodeID);


/************************************************************************
* DelAllChild
* ������ɾ�������ӽڵ�(�������Լ�)
* ����: PNODE pNode	�ڵ�ָ��
* ����ֵ: void
************************************************************************/
XMLPARSER_API void DelAllChild(PNODE pNode);

#else 
#define XMLPARSER_API  extern "C" __declspec(dllimport) 
#endif

//////////////////////////////////////////////////////////////////////////

#define __USE_FARPROC_DEFINE__

#ifdef __USE_FARPROC_DEFINE__  //����ָ�����Ͷ���

//////////////////////////////////////////////////////////////////////////
// ��������ز���
typedef PXMLPARSER (*_PLoadXMLFromFile)(const char*, int);

typedef PXMLPARSER (*_PLoadXMLFromString)(const char*);

typedef PXMLPARSER (*_PCreateXMLFile)(const char*, const char*, const char*);

typedef bool (*_PSave)(PXMLPARSER, const char*);

// ��Ҫ��������
typedef void (*_PReleaseParser)(PXMLPARSER);

typedef const char* (*_PGetLastError)(void);

typedef PNODE (*_PGetRoot)(PXMLPARSER);

//////////////////////////////////////////////////////////////////////////
// �ڵ���ز���

typedef PNODE (*_PCreateNode)(const char* , const char*);

typedef PNODE (*_PCreateNodeByXMLStr)(const char*);

// �ͷ�һ���ڵ㣨���ڵ�ͨ��Create���������Ϊ��Ҫ��������
typedef void (*_PReleaseNode)(PNODE);

typedef PNODE (*_PGetParentNode)(PNODE);

typedef const char* (*_PGetXMLString)(PNODE);
typedef int (*_PGetXMLStringLength)(PNODE);

typedef void (*_PSetNodeType)(PNODE, const char*);
typedef const char* (*_PGetNodeType)(PNODE);

typedef void (*_PSetNodeID)(PNODE, const char*);
typedef const char* (*_PGetNodeID)(PNODE);

typedef void (*_PSetAttribute)(PNODE, const char*, const char*);
typedef const char* (*_PGetAttribute)(PNODE, const char*, const char*);

typedef void (*_PSetAttributeInt)(PNODE, const char*, int);
typedef int (*_PGetAttributeInt)(PNODE, const char*, int);

typedef void (*_PSetAttributeUN)(PNODE, const char*, unsigned int);
typedef unsigned int (*_PGetAttributeUN)(PNODE, const char*, unsigned int);

typedef void (*_PSetAttributeBL)(PNODE, const char*, bool);
typedef bool (*_PGetAttributeBL)(PNODE, const char*, bool);

typedef void (*_PSetAttributeDouble)(PNODE, const char*, double);
typedef double (*_PGetAttributeDouble)(PNODE, const char*, double);

typedef void (*_PDelAttribute)(PNODE, const char*);

typedef bool (*_PIsRoot)(PNODE);

typedef PNODE (*_PGetFirstChild)(PNODE);

typedef PNODE (*_PGetFirstChildByType)(PNODE, const char*);

typedef PNODE (*_PGetNextSibling)(PNODE);

typedef PNODE (*_PGetNextSiblingByType)(PNODE, const char*);

typedef PNODE (*_PGetLastChild)(PNODE);

typedef PNODE (*_PGetLastChildByType)(PNODE, const char*);

typedef PNODE (*_PGetChild)(PNODE, const char*, const char*);

typedef PNODE (*_PAddChild)(PNODE, const char*, const char*);

typedef bool (*_PDelChild)(PNODE, const char*, const char*);

typedef void (*_PDelAllChild)(PNODE);


#endif

#endif