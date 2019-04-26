#pragma once
#include "ResData.h"
#include "tchar.h"
#define CONFIG_FILEPATH_OF_TOOLBAR	(TEXT("\\WBConfig.ini"))

class CResManage
{
protected:
	CResManage();
	~CResManage();

public:
	/*******************************************************************
	函数名称：GetInstance
	函数功能：【访问此对象的入口函数】获取实例对象
	参数说明：无
	返回值  ：CResManage的全局实例对象
	备注    ：
	*******************************************************************/
	static CResManage *GetInstance();

	/*******************************************************************
	函数名称：GetResType
	函数功能：【工具函数】判断资源类型
	参数说明：
	strFileFullPath，资源文件的路径
	返回值  ：
	WBRES_PICTURE, 图片文件
	WBRES_MOVIE,   电影文件
	WBRES_FLASH,   视频文件
	WBRES_DOC,     doc文档
	WBRES_MUISC    音乐文件
	备注    ：
	*******************************************************************/
	static int GetResType(const std::string &strFileFullPath);

	/*******************************************************************
	函数名称：IsResFileExtensionValid
	函数功能：【工具函数】资源文件扩展名是否有效(即：是否是我们所支持的类型)
	参数说明：strFileName，资源文件路径
	返回值  ：如我文件时白板支持的，返回TRUE，否则FALSE
	备注    ：
	*******************************************************************/
	BOOL IsResFileExtensionValid(LPCTSTR strFileName);

public:
	/*******************************************************************
	函数名称：GetResMap
	函数功能：获取资源数据对象指针
	参数说明：无
	返回值  ：返回一个ResMapArray *类型的指针
	备注    ：
	*******************************************************************/
	ResMapArray *GetResMap();

	////返回触摸资源窗口显示状态
	//const BOOL IsResWndShow();
	////显示影藏资源窗口
	//void ShowMTResourceWindow();
	////判断是否显示
	//BOOL IsMTResourceWindowShow();

	////设置资源窗口指针
	//void SetResWindows(CWBResGallery *pResGally);
	////设置资源库窗口的父窗口
	//void SetResWindowsParent(HWND hWnd){m_hResFather = hWnd;}

	/*******************************************************************
	函数名称：GetCurResourceIndex
	函数功能：获取当前使用的资源号索引(从零开始)
	参数说明：无
	返回值  ：返回当前使用的资源索引
	备注    ：
	*******************************************************************/
	unsigned int GetCurResourceIndex();

	/*******************************************************************
	函数名称：SetCurResourceIndex
	函数功能：设置当前使用的资源号索引(从零开始)
	参数说明：指定的资源索引
	返回值  ：无
	备注    ：
	*******************************************************************/
	void SetCurResourceIndex(unsigned int nIndex);

	/*******************************************************************
	函数名称：LoadWBResourceSetting
	函数功能：从配置文件中读取，写入配置信息
	参数说明：无
	返回值  ：无
	备注    ：配置文件保存在安装目录下的WBConfig.xml
	*******************************************************************/
	void LoadWBResourceSetting();

	/*******************************************************************
	函数名称：SaveWBResourceSetting
	函数功能：保存用户配置信息，包括资源文件名，文件路径，当前使用的资源索引号
	参数说明：无
	返回值  ：保存正确返回TRUE，否则返回FALSE
	备注    ：配置文件保存在安装目录下的WBConfig.xml
	*******************************************************************/
	BOOL SaveWBResourceSetting();
	
	/********************************************************************
	设置配置文件的写路径
	********************************************************************/
	void SetAvconIniPath(LPCTSTR strPath);
	tstring GetAvconIniPath();

private:
	/*******************************************************************
	函数名称：ClearResData
	函数功能：清空资源数据
	参数说明：无
	返回值  ：无
	备注    ：将资源链表的数据清空。
	*******************************************************************/
	void ClearResData();

	int CResManage::ReadResourcePathCount();
	void CResManage::ReadResourcePath(int i,TCHAR* pathname,TCHAR* pathstr);
	void WriteResourcePathCount(int iValue);
	void WriteResourcePath(int i,const TCHAR* pathname,const TCHAR* pathstr);

private:
	//资源目录配置信息
	ResMapArray m_resMap;

	//静态唯一实例对象
	static CResManage m_inst;

	//程序初始化路径
	tstring m_strAvconIniPath;
	////资源窗口
	//CWBResGallery *m_pResGally;
	//HWND m_hResFather;

	////资源窗口是否显示
	//BOOL m_bResGallyShow;
	//当前选中的资源索引
	unsigned int m_nIndexOfCurUsingResource;
};