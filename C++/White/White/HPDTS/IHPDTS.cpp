
//========================================================================================
//版权所有: 版权所有(C) 2008，上海华平信息技术股份有限公司
//系统名称: AVCON7
//作　　者: duquansheng
//创建日期: 2008-3-31
//功能说明: 数据传输服务器内核接口类定义
//

#include "stdafx.h"
#include <HPDTS/IHPDTS.h>
#include <HPDTS/DTSCmdType.h>
#include "HPDTS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------
static std::string		m_strFileTemPath = "";
static std::string		m_strFileSavePath = "";

//========================================================================================
//函数功能: 创建HPDTS实例
//作　　者: duquansheng
//创建日期: 2008-3-31
//参数说明: rIHPDTSNotify:回调引用，bUI：是否需要界面回调
//返回值  : HPDTS实例指针
//---------------------------------------------------------------------------------------
HPDTS_API IHPDTS* CreateIHPDTS(IHPDTSNotify& rIHPDTSNotify,bool bUI)
{
	return new HPDTS(rIHPDTSNotify,bUI);
}

//========================================================================================
//函数功能: WIN32环境下，删除指定目录
//作　　者: duquansheng
//创建日期: 2008-3-31
//参数说明: 目录路径
//返回值  : 是否成功
//---------------------------------------------------------------------------------------
#ifdef WIN32
bool DeleteDirectory(const std::string& DirName)
{
	if (DirName.length() == 0)
	{
		return false;
	}
	CFileFind tempFind;
	char tempFileFind[MAX_PATH];
	sprintf(tempFileFind,"%s\\*.*",DirName.c_str());
	bool IsFinded=(bool)tempFind.FindFile(tempFileFind);
	while(IsFinded)
	{
		IsFinded=(bool)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			char foundFileName[MAX_PATH];
			strcpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
			if(tempFind.IsDirectory())
			{
				char tempDir[MAX_PATH];
				sprintf(tempDir,"%s\\%s",DirName.c_str(),foundFileName);
				DeleteDirectory(tempDir);
			}
			else
			{
				char tempFileName[MAX_PATH];
				sprintf(tempFileName,"%s\\%s",DirName.c_str(),foundFileName);		
				DeleteFile(tempFileName);
			}
		}
	}
	tempFind.Close();
	if(!RemoveDirectory(DirName.c_str()))
	{
		TRACE(TEXT("删除目录失败！\n"));
		return false;
	}
	return true;
}
#endif

//========================================================================================
//函数功能: 初始化HPDTS
//作　　者: duquansheng
//创建日期: 2008-3-31
//参数说明: cszTemPath：HPDTS所使用的临时目录，注意：此目录在HPDTS释放是将被删除！！！
//返回值  : 
//---------------------------------------------------------------------------------------
HPDTS_API void HPDTSInit(const char* cszTemPath)
{
	printf("path=%s\n",cszTemPath);

	m_strFileTemPath=cszTemPath;
	//创建目录
#ifdef WIN32	
	m_strFileTemPath += "\\DTSTempDir";
	//清空目录
	HPDTSUnInit();
	//_mkdir(m_strFileTemPath.c_str());
	int iPos = 0;
	iPos = m_strFileTemPath.find_first_of("\\",iPos);
	while (iPos != -1)
	{
		iPos = m_strFileTemPath.find_first_of("\\",iPos+1);
		string strPath = m_strFileTemPath.substr(0,iPos);
		_mkdir(strPath.c_str());
	}
#else	
	m_strFileTemPath += "/DTSTempDir";
	//清空目录
	HPDTSUnInit();
	std::string mkdir="mkdir ";
	mkdir+=m_strFileTemPath.c_str();	
	system(mkdir.c_str());
	printf("file=%s\n",mkdir.c_str());
#endif

}

//========================================================================================
//函数功能: HPDTS释放
//作　　者: duquansheng
//创建日期: 2008-3-31
//参数说明: 
//返回值  : 
//---------------------------------------------------------------------------------------
HPDTS_API void HPDTSUnInit(void)
{
	if (m_strFileTemPath.length() == 0)
	{
		return;
	}

	int pos = m_strFileTemPath.find("DTSTempDir");
	if(pos < 1)
	{
		return;
	}
	//清空目录
#ifdef WIN32
	DeleteDirectory(m_strFileTemPath.c_str());	
#else
	std::string rmdir="rm -rf ";	
	rmdir+=m_strFileTemPath.c_str();	
	system(rmdir.c_str());	
#endif
}

//========================================================================================
//函数功能: 文件传输，文件分发时，客户端设置文件保存目录,可重复调用，替换原有设置。
//作　　者: duquansheng
//创建日期: 2008-3-31
//参数说明: cszSavePath：文件保存全路径，可不带‘\’结尾。
//返回值  : 
//---------------------------------------------------------------------------------------
HPDTS_API void HPDTSSetSavePath(const char* cszSavePath)
{
	m_strFileSavePath = cszSavePath;
	//创建目录
#ifdef WIN32	
	_mkdir(m_strFileSavePath.c_str());
#else	
	std::string mkdir="mkdir ";
	mkdir+=m_strFileSavePath.c_str();	
	system(mkdir.c_str());
#endif

}

//========================================================================================
//函数功能: 获取DTS临时目录
//作　　者: duquansheng
//创建日期: 2008-3-31
//参数说明: 
//返回值  : DTS临时目录全路径
//---------------------------------------------------------------------------------------
HPDTS_API const char* GetFileTemPath(void) 
{
	return m_strFileTemPath.c_str();
}

//========================================================================================
//函数功能: 获取DTS文件保存目录
//作　　者: duquansheng
//创建日期: 2008-3-31
//参数说明: 
//返回值  : DTS文件保存全路径
//---------------------------------------------------------------------------------------
HPDTS_API const char* GetFileSavePath(void)
{
	return m_strFileSavePath.c_str();
}

//---------------------------------------------------------------------------------------