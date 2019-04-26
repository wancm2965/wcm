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
// 每个文件对应一个解析器

/************************************************************************
* LoadXMLFromFile
* 描述：从文件中解析XML
* 参数: const char* strFilePath 文件路径
* 参数: int encoding 编码方式
* 返回值: 解析器指针
************************************************************************/
XMLPARSER_API PXMLPARSER LoadXMLFromFile(const char* pstrFilePath, int encoding = XMLFILE_DEFAULT_ENCODING);


/************************************************************************
* LoadXMLFromString
* 描述：从字符串中解析XML
* 参数:const char* pstrXML XML字符串
* 返回值: 解析器指针
************************************************************************/
XMLPARSER_API PXMLPARSER LoadXMLFromString(const char* pstrXML);


/************************************************************************
* CreateXML
* 描述：创建一个XML文件（保存时指定路径）
* 参数: const char * pstrVersion	版本号
* 参数: const char * pstrEncoding	编码方式("utf-8")
* 参数: const char * pstrStandalone	是否独立（通常为"yes"）
* 返回值: 解析器指针
************************************************************************/
XMLPARSER_API PXMLPARSER CreateXMLFile(const char* pstrVersion, const char* pstrEncoding, const char* pstrStandalone);


/************************************************************************
* Save
* 描述：保存一个XML文件
* 参数: PXMLPARSER pXMLParser	解析器指针
* 参数: const char * pstrFilePath 路径
* 返回值: 是否成功
************************************************************************/
XMLPARSER_API bool Save(PXMLPARSER pXMLParser, const char* pstrFilePath);



/************************************************************************
* GetParserLastError
* 描述：返回最后一个错误信息(只作用于解析器操作)
* 返回值: 错误信息
************************************************************************/
XMLPARSER_API const char* GetParserLastError(void);


/************************************************************************
* Release
* 描述：释放解析器(必要操作！)
* 参数: PXMLPARSER pXMLParser	解析器指针
* 返回值: void
************************************************************************/
XMLPARSER_API void ReleaseParser(PXMLPARSER pXMLParser);


/************************************************************************
* GetRoot
* 描述：得到根节点
* 参数: PXMLPARSER pXMLParser	解析器指针
* 返回值: 根节点指针
************************************************************************/
XMLPARSER_API PNODE GetRoot(PXMLPARSER pXMLParser);


/************************************************************************
* GetXMLString
* 描述：获取节点的XML格式的字符串
* 参数: PNODE pNode	节点指针或解析器指针
* 返回值: XML字符串
************************************************************************/
XMLPARSER_API const char* GetXMLString(PNODE pNode);


/************************************************************************
* GetXMLStringLength
* 描述：获取节点的XML格式的字符串的长度(用在GetXMLString之后才能正确获得）
* 参数: PNODE pNode 节点指针
* 返回值: 节点字符串长度
************************************************************************/
XMLPARSER_API int GetXMLStringLength(PNODE pNode);


/************************************************************************
* CreateXMLParser
* 描述：创建一个节点
* 参数: const char * pstrNodeType	节点类型（根节点自动完成）
* 参数: const char * pstrNodeID		节点ID
* 返回值: 节点指针
************************************************************************/
XMLPARSER_API PNODE CreateNode(const char* pstrNodeType, const char* pstrNodeID);


/************************************************************************
* CreateNodeByXMLStr
* 描述：创建一个XML解析器（不用来保存文件，只用作数据读取）
* 参数: const char * pstrNodeType	第一个子节点类型（根节点自动完成）
* 参数: const char * pstrNodeID		子节点ID
* 返回值: 节点指针
************************************************************************/
XMLPARSER_API PNODE CreateNodeByXMLStr(const char* pstrXML);


/************************************************************************
* ReleaseNode
* 描述：释放一个节点（当节点通过Create创建，则此为必要操作！）
* 参数: PNODE pNode	节点指针
* 返回值: void
************************************************************************/
XMLPARSER_API void ReleaseNode(PNODE pNode);


/************************************************************************
* GetNodeType
* 描述：获取节点类型
* 参数: PNODE pNode	节点指针
* 返回值: 节点类型字符串
************************************************************************/
XMLPARSER_API const char* GetNodeType(PNODE pNode);


/************************************************************************
* SetNodeType
* 描述：设置节点类型
* 参数: PNODE pNode	节点指针
* 参数: const char* strType 节点类型
* 返回值: void
************************************************************************/
XMLPARSER_API void SetNodeType(PNODE pNode, const char* pstrType);


/************************************************************************
* GetNodeID
* 描述：获取节点ID
* 参数: PNODE pNode	节点指针
* 返回值: 节点ID
************************************************************************/
XMLPARSER_API const char* GetNodeID(PNODE pNode);


/************************************************************************
* SetNodeID
* 描述：设置节点ID
* 参数: PNODE pNode	节点指针
* 参数: const char* strNodeID	节点类型
* 返回值: void
************************************************************************/
XMLPARSER_API void SetNodeID(PNODE pNode, const char* pstrNodeID);


/************************************************************************
* IsRoot
* 描述：判断节点是否是根节点
* 参数: PNODE pNode	节点指针
* 返回值: bool
************************************************************************/
XMLPARSER_API bool IsRoot(PNODE pNode);


/************************************************************************
* GetParentNode
* 描述：获取节点的父节点
* 参数: PNODE pNode	节点指针
* 返回值: PNODE		父节点指针
************************************************************************/
XMLPARSER_API PNODE GetParentNode(PNODE pNode);


/************************************************************************
* GetAttribute
* 描述：获取节点某个属性
* 参数: PNODE pNode	节点指针
* 参数: const char* strAttrName		属性名
* 参数: const char* strDefValue		默认属性值（如果无法读取到值，则返回默认值）
* 返回值: const char*	属性值
************************************************************************/
XMLPARSER_API const char* GetAttribute(PNODE pNode, const char* pstrAttrName, const char* pstrDefValue);


/************************************************************************
* SetAttribute
* 描述：设置节点的某个属性值(如果没有该属性则创建)
* 参数: PNODE pNode	节点指针
* 参数: const char* strAttrName		属性名
* 参数: const char* strAttrValue	属性值
* 返回值: void
************************************************************************/
XMLPARSER_API void SetAttribute(PNODE pNode, const char* pstrAttrName, const char* pstrAttrValue);


/************************************************************************
* SetAttribute
* 描述：设置节点的某个unsigned int类型的属性值(如果没有该属性则创建)
* 参数: PNODE pNode	节点指针
* 参数: const char* strAttrName		属性名
* 参数: unsigned int ulValue	属性值
* 返回值: void
************************************************************************/
XMLPARSER_API void SetAttributeUN(PNODE pNode, const char* pstrAttrName, unsigned long ulValue);


/************************************************************************
* GetAttribute
* 描述：获取节点某个unsigned int属性
* 参数: PNODE pNode	节点指针
* 参数: const char* strAttrName		属性名
* 参数: unsigned int unDefValue		默认属性值（如果无法读取到值，则返回默认值）
* 返回值: unsigned int unDefValue
************************************************************************/
XMLPARSER_API unsigned int GetAttributeUN(PNODE pNode, const char* pstrAttrName, unsigned int unDefValue);


/************************************************************************
* SetAttributeInt
* 描述：设置节点的某个int类型的属性值(如果没有该属性则创建)
* 参数: PNODE pNode	节点指针
* 参数: const char* strAttrName		属性名
* 参数: int nValue	属性值
* 返回值: void
************************************************************************/
XMLPARSER_API void SetAttributeInt(PNODE pNode, const char* pstrAttrName, int nValue);


/************************************************************************
* GetAttributeInt
* 描述：获取节点某个int属性
* 参数: PNODE pNode	节点指针
* 参数: const char * pstrAttrName 属性名
* 参数: int nDefValue	默认属性值
* 返回值: XMLPARSER_API int
************************************************************************/
XMLPARSER_API int GetAttributeInt(PNODE pNode, const char* pstrAttrName, int nDefValue);


/************************************************************************
* SetAttributeBL
* 描述：设置节点的某个bool类型的属性值(如果没有该属性则创建)
* 参数: PNODE pNode
* 参数: const char * pstrAttrName 属性名
* 参数: bool bValue	默认属性值
* 返回值: void
************************************************************************/
XMLPARSER_API void SetAttributeBL(PNODE pNode, const char* pstrAttrName, bool bValue);


/************************************************************************
* GetAttribute
* 描述：获取节点某个属性
* 参数: PNODE pNode	节点指针
* 参数: const char* strAttrName		属性名
* 参数: bool strDefValue		默认属性值（如果无法读取到值，则返回默认值）
* 返回值: bool	
************************************************************************/
XMLPARSER_API bool GetAttributeBL(PNODE pNode, const char* pstrAttrName, bool bDefValue);


/************************************************************************
* SetAttributeBL
* 描述：设置节点的某个bool类型的属性值(如果没有该属性则创建)
* 参数: PNODE pNode
* 参数: const char * pstrAttrName 属性名
* 参数: bool bValue	默认属性值
* 返回值: void
************************************************************************/
XMLPARSER_API void SetAttributeDouble(PNODE pNode, const char* pstrAttrName, double dValue);


/************************************************************************
* GetAttribute
* 描述：获取节点某个属性
* 参数: PNODE pNode	节点指针
* 参数: const char* strAttrName		属性名
* 参数: bool strDefValue		默认属性值（如果无法读取到值，则返回默认值）
* 返回值: bool	
************************************************************************/
XMLPARSER_API double GetAttributeDouble(PNODE pNode, const char* pstrAttrName, double dDefValue);


/************************************************************************
* DelAttribute
* 描述：删除节点的某个属性
* 参数: PNODE pNode	节点指针
* 参数: const char* strAttrName	属性名
* 返回值: void
************************************************************************/
XMLPARSER_API void DelAttribute(PNODE pNode, const char* pstrAttrName);


/************************************************************************
* GetFirstChild
* 描述：获取第一个子节点
* 参数: PNODE pNode	节点指针
* 返回值: PNODE
************************************************************************/
XMLPARSER_API PNODE GetFirstChild(PNODE pNode);


/************************************************************************
* GetFirstChild
* 描述：获取某个类型的第一个子节点
* 参数: PNODE pNode	节点指针
* 参数: const char* strType		节点类型
* 返回值: PNODE
************************************************************************/
XMLPARSER_API PNODE GetFirstChildByType(PNODE pNode, const char* pstrType);


/************************************************************************
* GetNextSibling
* 描述：获取某个节点的下一个同级节点
* 参数: PNODE pNode	节点指针
* 返回值: PNODE
************************************************************************/
XMLPARSER_API PNODE GetNextSibling(PNODE pNode);


/************************************************************************
* GetNextSibling
* 描述：获取某个类型的节点下一个同级节点
* 参数: PNODE pNode	节点指针
* 参数: const char* strType		节点类型
* 返回值: int
************************************************************************/
XMLPARSER_API PNODE GetNextSiblingByType(PNODE pNode, const char* pstrType);


/************************************************************************
* GetFirstChild
* 描述：获取最后一个子节点
* 参数: PNODE pNode	节点指针
* 返回值: PNODE
************************************************************************/
XMLPARSER_API PNODE GetLastChild(PNODE pNode);


/************************************************************************
* GetFirstChild
* 描述：获取某个类型的最后一个子节点
* 参数: PNODE pNode	节点指针
* 参数: const char* strType		节点类型
* 返回值: PNODE
************************************************************************/
XMLPARSER_API PNODE GetLastChildByType(PNODE pNode, const char* pstrType);


/************************************************************************
* GetChild
* 描述：根据节点类型，节点ID获取一个子节点
* 参数: PNODE pNode		节点指针
* 参数: const char* strType		节点类型
* 参数: const char* strNodeID	节点ID
* 返回值: PNODE		子节点指针
************************************************************************/
XMLPARSER_API PNODE GetChild(PNODE pNode, const char* pstrType, const char* pstrNodeID);


/************************************************************************
* AddChild
* 描述：增加一个子节点
* 参数: PNODE pNode		节点指针
* 参数: const char* strType		节点类型
* 参数: const char* strNodeID	节点ID
* 返回值: PNODE	子节点指针
************************************************************************/
XMLPARSER_API PNODE AddChild(PNODE pNode, const char* pstrType, const char* pstrNodeID);


/************************************************************************
* DelChild
* 描述：删除一个子节点
* 参数: PNODE pNode		节点指针
* 参数: const char* strType		节点类型
* 参数: const char* strNodeID	节点ID
* 返回值: bool
************************************************************************/
XMLPARSER_API bool DelChild(PNODE pNode, const char* pstrType, const char* pstrNodeID);


/************************************************************************
* DelAllChild
* 描述：删除所有子节点(不包括自己)
* 参数: PNODE pNode	节点指针
* 返回值: void
************************************************************************/
XMLPARSER_API void DelAllChild(PNODE pNode);

#else 
#define XMLPARSER_API  extern "C" __declspec(dllimport) 
#endif

//////////////////////////////////////////////////////////////////////////

#define __USE_FARPROC_DEFINE__

#ifdef __USE_FARPROC_DEFINE__  //函数指针类型定义

//////////////////////////////////////////////////////////////////////////
// 解析器相关操作
typedef PXMLPARSER (*_PLoadXMLFromFile)(const char*, int);

typedef PXMLPARSER (*_PLoadXMLFromString)(const char*);

typedef PXMLPARSER (*_PCreateXMLFile)(const char*, const char*, const char*);

typedef bool (*_PSave)(PXMLPARSER, const char*);

// 必要操作！！
typedef void (*_PReleaseParser)(PXMLPARSER);

typedef const char* (*_PGetLastError)(void);

typedef PNODE (*_PGetRoot)(PXMLPARSER);

//////////////////////////////////////////////////////////////////////////
// 节点相关操作

typedef PNODE (*_PCreateNode)(const char* , const char*);

typedef PNODE (*_PCreateNodeByXMLStr)(const char*);

// 释放一个节点（当节点通过Create创建，则此为必要操作！）
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