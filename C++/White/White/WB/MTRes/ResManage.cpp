#include "stdafx.h"
#include <Windows.h>
#include "ResManage.h"
#include<algorithm>
#include "Shlwapi.h"
#include <string>
#pragma  comment(lib,"Shlwapi.lib") 
#pragma  warning(disable:4018) //warning C4018: “<”: 有符号/无符号不匹配

int CResManage::ReadResourcePathCount()//从WBConfig.ini中读取按钮的类型
{	
	tstring strPath = m_strAvconIniPath;
	strPath += CONFIG_FILEPATH_OF_TOOLBAR;
	LPCTSTR strSelection = TEXT("Resource");    //
	LPCTSTR Position = TEXT("Count");
	return GetPrivateProfileInt(strSelection, Position, -1/*默认值*/, strPath.c_str());
}
void CResManage::ReadResourcePath(int i,TCHAR* pathname,TCHAR* pathstr)//从WBConfig.ini中读取按钮的类型
{
	tstring strPath = m_strAvconIniPath;
	strPath += CONFIG_FILEPATH_OF_TOOLBAR;
	LPCTSTR strSelection = TEXT("Resource");    //
	TCHAR Position[MAX_PATH] = {0};
	_stprintf(Position,TEXT("path%d"),i);
	GetPrivateProfileString(strSelection, Position,TEXT("null")/*默认值*/,pathstr,255,strPath.c_str());
	_stprintf(Position,TEXT("path%dname"),i);
	GetPrivateProfileString(strSelection, Position,TEXT("null")/*默认值*/,pathname,255,strPath.c_str());
}

void CResManage::WriteResourcePathCount(int iValue)
{
	tstring strPath = m_strAvconIniPath;
	strPath += CONFIG_FILEPATH_OF_TOOLBAR;
	TCHAR strValue[MAX_PATH] = {0};
	_stprintf(strValue,TEXT("%d"),iValue);
	LPCTSTR strSelection = TEXT("Resource");
	LPCTSTR Position = TEXT("Count");
	WritePrivateProfileString(strSelection,Position,strValue, strPath.c_str());
}

void CResManage::WriteResourcePath(int i,const TCHAR* pathname,const TCHAR* pathstr)
{
	tstring strPath = m_strAvconIniPath;
	strPath += CONFIG_FILEPATH_OF_TOOLBAR;
	LPCTSTR strSelection = TEXT("Resource");
	TCHAR Position[MAX_PATH] = {0};
	_stprintf(Position,TEXT("path%d"),i);
	WritePrivateProfileString(strSelection,Position,pathstr, strPath.c_str());
	_stprintf(Position,TEXT("path%dname"),i);
	WritePrivateProfileString(strSelection,Position,pathname, strPath.c_str());
}

CResManage::CResManage()
	//: m_pResGally(NULL)
{
	//m_bResGallyShow = FALSE;
	if(m_strAvconIniPath.empty() || !::PathFileExists(m_strAvconIniPath.c_str()))    //这里要改要用白板那边传过来的路径
	{
		TCHAR path[MAX_PATH] = {0};
		GetModuleFileName(GetModuleHandle(NULL),path,MAX_PATH);
		_tcsrchr(path,TEXT('\\'))[0] = TEXT('\0');
		m_strAvconIniPath = path;
		m_strAvconIniPath += TEXT("\\config");
	}
	LoadWBResourceSetting();
}

CResManage::~CResManage()
{
	SaveWBResourceSetting();
	ClearResData();
}

CResManage * CResManage::GetInstance()
{
	return &m_inst;
}

void CResManage::LoadWBResourceSetting()
{
	for (ResMapArrayIter itt = m_resMap.begin(); m_resMap.end() != itt; ++itt)
	{
		delete (*itt);
	}
	m_resMap.clear();

	tstring sConfigFilePath = m_strAvconIniPath;
	sConfigFilePath += CONFIG_FILEPATH_OF_TOOLBAR;
	if (PathFileExists(sConfigFilePath.c_str()))
	{
		ClearResData();
		int n=ReadResourcePathCount();
		if(n >= 1 )
		{
			for(int i=1;i<=n;i++)
			{
				TCHAR tempPath[255]={0};
				TCHAR tempPathName[255]={0};
				ReadResourcePath(i,tempPathName,tempPath);

				SResourceMap *pRes = new SResourceMap;
				pRes->SetMapPath(tempPath);
				pRes->SetMapName(tempPathName);

				//过滤无效资源路径
				if(PathFileExists(tempPath))
				{
					m_resMap.push_back(pRes);
				}
			}
		}
	}
}

BOOL CResManage::SaveWBResourceSetting()
{
	int nIndex = 0; 
	tstring PathName;
	tstring Path;
	PResMapArray pResMap = CResManage::GetInstance()->GetResMap();
	ResMapArrayIter it = pResMap->begin();
	for (; it != pResMap->end(); it++, nIndex++)
	{
		Path = (*it)->GetMapPath();      //得到当前路径		
		PathName = (*it)->GetMapName();	
		WriteResourcePathCount(nIndex+1);
		WriteResourcePath(nIndex+1,PathName.c_str(),Path.c_str());
	}
	WriteResourcePathCount( nIndex );



	BOOL bSaveOK = TRUE;
	//CString sPath = g_strAvconIniPath + WB_RESOURCE_FILENAME;
	////首先确保文件夹存在
	//if (!PathFileExists(g_strAvconIniPath))
	//{
	//	bSaveOK = CreateDirectory(g_strAvconIniPath, NULL);
	//}
	//if(!bSaveOK)
	//{
	//	return bSaveOK;//文件路径创建失败
	//}

	//CMarkup xml;
	//xml.AddElem(WB_RESOURCE_XML_HEAD);
	//xml.IntoElem();
	///*-----------------------------保存示例----------------------------
	////xml.AddElem(WB_RESOURCE_ELEMENT_NAME);
	////xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_1, _T("图片"));
	////xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_2, _T("d:\\wbtemp"));
	////xml.AddElem(WB_RESOURCE_ELEMENT_NAME);
	////xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_1, _T("图片2"));
	////xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_2, _T("d:\\wbtemp23"));
	//------------------------------------------------------------------*/
	//SResourceMap * pRes = NULL;
	//ResMapArrayIter it = m_resMap.begin();
	//for (; it != m_resMap.end(); it++)
	//{
	//	xml.AddElem(WB_RESOURCE_ELEMENT_NAME);
	//	xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_1, (*it)->GetMapName());
	//	xml.SetAttrib(WB_RESOURCE_ELEMENT_ATTRIBUTE_2, (*it)->GetMapPath());
	//}
	////写入索引号
	//xml.AddChildElem(WB_RESOURCE_ELEMENT_LAST_USING_ITEM, m_nIndexOfCurUsingResource, 0);
	//xml.OutOfElem();
	//sPath = GetCurrentPath() + WB_RESOURCE_FILENAME;
	//bSaveOK = xml.Save(sPath);
	return bSaveOK;
}

BOOL CResManage::IsResFileExtensionValid( LPCTSTR strFileName )
{
	int nType = CResManage::GetResType(ToStr(strFileName).c_str());
	return (nType>=0);
}

ResMapArray * CResManage::GetResMap()
{
	return &m_resMap;
}

int CResManage::GetResType(const std::string &strFileFullPath)
{
	int type = -1;
	//解析
	std::string sExt = PathFindExtensionA(strFileFullPath.c_str());
	transform(sExt.begin(),sExt.end(),sExt.begin(),::tolower);
	if ( (sExt.compare(".bmp") == 0) || (sExt.compare(".jpg") == 0) || (sExt.compare(".gif") == 0) || (sExt.compare(".png") == 0) || (sExt.compare(".dib") == 0) 
		|| (sExt.compare(".tiff") == 0) || (sExt.compare(".jfif") == 0) || (sExt.compare(".ico") == 0) || (sExt.compare(".jpeg") == 0) || (sExt.compare(".tif") == 0) )
	{
		type = WBRES_PICTURE;
	}
	else if ((sExt.compare(".doc") == 0) || (sExt.compare(".ppt") == 0) || (sExt.compare(".xls") == 0) || (sExt.compare(".pdf") == 0)
				||(sExt.compare(".docx") == 0) ||(sExt.compare(".xlsx") == 0) || (sExt.compare(".pptx") == 0))
	{
		type = WBRES_DOC;
	}
	else if ((sExt.compare(".avi") == 0) || (sExt.compare(".wmv") == 0) || (sExt.compare(".mpeg") == 0) || (sExt.compare(".mpg") == 0))
// 		||(sExt.compare(".asf") == 0) || (sExt.compare(".mov") == 0) || (sExt.compare(".dat") == 0) || (sExt.compare(".rmvb") == 0)
// 		||(sExt.compare(".vob") == 0) || (sExt.compare(".mp4") == 0) || (sExt.compare(".f4v") == 0) || (sExt.compare(".mp3") == 0)
// 		||(sExt.compare(".wm") == 0) || (sExt.compare(".wma") == 0) || (sExt.compare(".rm") == 0) || (sExt.compare(".mts") == 0)||(sExt.compare(".mkv") == 0))
	{
		type = WBRES_MOVIE;
	}
	else if ((sExt.compare(".swf") == 0))
	{
		type = WBRES_FLASH;
	}

	return type;
}

void CResManage::ClearResData()
{
	ResMapArrayIter it = m_resMap.begin();
	for (;it != m_resMap.end();it++)
	{
		delete *it;
	}
	m_resMap.clear();
}

unsigned int CResManage::GetCurResourceIndex()
{
	//检验校准
	int nSize = m_resMap.size();
	if ( (m_nIndexOfCurUsingResource>nSize) || (m_nIndexOfCurUsingResource<0) )
	{
		m_nIndexOfCurUsingResource = 0;//置第0个
	}
	return m_nIndexOfCurUsingResource;
}

void CResManage::SetCurResourceIndex( unsigned int nIndex )
{
	int nSize = m_resMap.size();
	if ( (nIndex>nSize) || (nIndex<0) )
	{
		m_nIndexOfCurUsingResource = 0;//置第0个
	}
	else
	{
		m_nIndexOfCurUsingResource = nIndex;
	}
	//智能修正
	if ((0 == m_nIndexOfCurUsingResource) && nSize>0)
	{
		m_nIndexOfCurUsingResource = 1;//取第一个
	}
}

void CResManage::SetAvconIniPath( LPCTSTR strPath )
{
	if (::PathFileExists(strPath))
	{
		//tstring strOld = m_strAvconIniPath;
		//tstring strNew = tstring(strPath);
		//transform(strOld.begin(),strOld.end(),strOld.begin(),tolower);
		//transform(strNew.begin(),strNew.end(),strNew.begin(),tolower);
		//if( 0 == strOld.compare(strNew))
		//{
			m_strAvconIniPath = tstring(strPath);
			LoadWBResourceSetting();
		//}
	}
}

tstring CResManage::GetAvconIniPath()
{
	return m_strAvconIniPath;
}

//
//const BOOL CResManage::IsResWndShow()
//{
//	return m_bResGallyShow;
//}

CResManage CResManage::m_inst;



#pragma  warning(default:4018)