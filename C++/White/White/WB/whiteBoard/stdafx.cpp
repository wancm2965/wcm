// stdafx.cpp : 只包括标准包含文件的源文件
// whiteBoard.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "whiteBoard.h"
#define CONFIG_FILEPATH_OF_TOOLBAR	_T("\\WBConfig.ini")

bool AvconParseFilePath( const char* pPath,string& str_Path,string& str_Title,string& str_ext )
{
	if (pPath == NULL)
	{
		return false;
	}
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(pPath,drive,dir,fname,ext);
	str_Path = drive;
	//strFilePath += dir;
	str_Title = fname;
	str_ext = ext;
	return true;
}

bool AvconRename( const char*pPath,string& str_name )
{
	if (pPath == NULL)
	{
		return false;
	}
	//得到文件属性
	struct stat buf;
	if (stat(pPath,&buf) < 0)		
	{
		return false;
	}
	//解析文件名
	char drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(pPath,drive,dir,fname,ext);
	string str_start_name = "Rename_";
	string str_name_temp = fname;
	int npos = str_name_temp.find(str_start_name);//查看该文件是不是已经重新命名过了
	if (npos != -1)
	{
		str_name = fname;
	}
	else
	{
		CTime modifyTime = buf.st_mtime; //创建时间
		CTime changeTime = buf.st_ctime;//最近修改时间
		str_name = str_start_name;
		str_name += fname;
		CString str_temp = modifyTime.Format(_T("_创建时间_%y_%m_%d_%H_%M_%S"));
		str_name += ToStr(str_temp.GetBuffer());
		str_temp.ReleaseBuffer();
		str_temp = changeTime.Format(_T("_最近修改__%y_%m_%d_%H_%M_%S"));
		str_name += ToStr(str_temp.GetBuffer());
		str_temp.ReleaseBuffer();
		str_temp.Format(_T("_Size_%d"),buf.st_size);
		str_name = ToStr(str_temp.GetBuffer());
		str_temp.ReleaseBuffer();
	}
	str_name += ext;
	int nSize = str_name.size();
	if (nSize > _MAX_FNAME)
	{
		str_name_temp = str_name;
		str_name.assign(str_name_temp,nSize - _MAX_FNAME + 1,_MAX_FNAME);
	}
	return true;
}

void AvconGet_path(const char* pStr_Path,char* pCrrent_Dir)
{
	if (pStr_Path == NULL)
	{
		return;
	}
	while(*(pCrrent_Dir ++) = *(pStr_Path ++));
	while(*(--pCrrent_Dir) != '\\');
	*pCrrent_Dir = '\0';
}

void AvconGet_name(const char* pStr_path,char* pFile_name)
{
	if (pStr_path == NULL)
	{
		return;
	}
	while(*(pStr_path ++));
	while(*(--pStr_path) != '\\');
	while(*(pFile_name++) = *(++pStr_path));
}

bool AvconRename( const char*pPath, string& str_new_name,string& str_Path,string& str_title,string& str_ext )
{
	if (pPath == NULL)
	{
		return false;
	}
	//得到文件属性
	struct stat buf;
	if (stat(pPath,&buf) < 0)		
	{
		return false;
	}
	//解析文件名
	char drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(pPath,drive,dir,fname,ext);
	str_Path = drive;
	//strFilePath += dir;
	str_title = fname;
	str_ext = ext;
	string str_start_name = "Rename_";
	string str_name_temp = fname;
	int npos = str_name_temp.find(str_start_name);//查看该文件是不是已经重新命名过了
	if (npos != -1)
	{
		str_new_name = fname;
	}
	else
	{
		CTime modifyTime = buf.st_mtime; //创建时间
		CTime changeTime = buf.st_ctime;//最近修改时间
		str_new_name = str_start_name;
		str_new_name += fname;
		CString str_temp = modifyTime.Format(_T("_创建时间_%y_%m_%d_%H_%M_%S"));
		str_new_name += ToStr(str_temp.GetBuffer());
		str_temp.ReleaseBuffer();
		str_temp = changeTime.Format(_T("_最近修改__%y_%m_%d_%H_%M_%S"));
		str_new_name += ToStr(str_temp.GetBuffer());
		str_temp.ReleaseBuffer();
		str_temp.Format(_T("_Size_%d"),buf.st_size);
		str_new_name =ToStr(str_temp.GetBuffer());
		str_temp.ReleaseBuffer();
	}
	str_new_name += ext;
	int nSize = str_new_name.size();
	if (nSize > _MAX_FNAME)
	{
		str_name_temp = str_new_name;
		str_new_name.assign(str_name_temp,nSize - _MAX_FNAME + 1,_MAX_FNAME);
	}
	return true;
}
//========================================================================================
//函数功能: WIN32环境下，删除指定目录
//参数说明: 目录路径
//返回值  : 是否成功
//---------------------------------------------------------------------------------------
bool RemoveDir(const char* pDir_name)
{
	if (pDir_name == NULL)
	{
		return false;
	}
	CFileFind Temp_Find;
	char tempFileFind[MAX_PATH];
	sprintf_s(tempFileFind,"%s\\*.*",pDir_name);
	wchar_t *strTemp = new wchar_t[MAX_PATH];
	wcscpy_s(strTemp,MAX_PATH,ToDRAWStr(tempFileFind).c_str());      
	bool IsFinded=(bool)Temp_Find.FindFile((LPCTSTR)strTemp);
	while(IsFinded)
	{
		IsFinded=(bool)Temp_Find.FindNextFile();
		if(!Temp_Find.IsDots())
		{
			char foundFileName[MAX_PATH];
			strcpy_s(foundFileName,ToStr(Temp_Find.GetFileName().GetBuffer(MAX_PATH)).c_str());
			if(Temp_Find.IsDirectory())
			{
				char tempDir[MAX_PATH];
				sprintf_s(tempDir,"%s\\%s",pDir_name,foundFileName);
				RemoveDir(tempDir);
			}
			else
			{
				char tempFileName[MAX_PATH];
				sprintf_s(tempFileName,"%s\\%s",pDir_name,foundFileName);		
				DeleteFileW(ToDRAWStr(tempFileName).c_str());
			}
		}
	}
	delete[]strTemp;
	Temp_Find.Close();
	if(!RemoveDirectoryW(ToDRAWStr(pDir_name).c_str()))
	{
		TRACE(TEXT("删除目录失败！\n"));
		return false;
	}
	return true;
}
DWORD CreateALLDirectory(LPCTSTR Directory)    //创建文件夹
{
	DWORD ret = ERROR_SUCCESS; 
	CString Target_Directory;
	Target_Directory = Directory;

	if(Target_Directory.Right(1) != _T('\\'))
		Target_Directory += _T("\\");

	int i = Target_Directory.Find( _T(":\\"),0);
	if(i != -1)
		i++;        

	while(ret == ERROR_SUCCESS && i < Target_Directory.GetLength() )
	{        
		CString Direct_char;    
		Direct_char = Target_Directory;

		i = Direct_char.Find( _T('\\'),i+1);
		if(i==-1)
			break;
		Direct_char.GetBufferSetLength(i);

		CString Find_char;
		Find_char.Format( _T("%s\\*.*"),Direct_char);

		CFileFind finder;        
		if(!finder.FindFile(Find_char))//未找到目录
		{
			if(!CreateDirectory(Direct_char,NULL))//建立目录
			{
				ret = GetLastError();
			}
		}
		finder.Close();        
	}

	return(ret);
}

CString ReadConfigText( CString strSelection, LPCTSTR ControlName )
{
	CString ControlText;
	wchar_t Text[256] = {0};
	GetPrivateProfileString(strSelection, ControlName, _T(""),Text,256,g_strAvconLanguageFile);
	ControlText = Text;
	return ControlText;
}

int ReadWBConfigPathINI( CString strSelection,LPCTSTR Position )
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	return  GetPrivateProfileInt(strSelection,Position,-1,strPath);
}

void WriteWBConfigPathINI( CString strSelection,LPCTSTR Postion,LPCTSTR strValue )
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;	
	WritePrivateProfileString(strSelection,Postion,strValue,strPath);
}
CString A2U( std::string sstring )
{
	if (sstring.empty())
	{
		return _T("");
	}
	int cchStr = (int) sstring.length() + 1;
	wchar_t* pwstr = new wchar_t[cchStr];
	if( pwstr != NULL )
		::MultiByteToWideChar(CP_UTF8, 0, sstring.c_str(), -1, pwstr, cchStr);

	CString strTmp(pwstr);
	delete []pwstr;

	return strTmp;
}

std::string U2A( CString strUnicodeString )
{
	ASSERT(!::IsBadStringPtrW(strUnicodeString,-1));
	int cchStr = ::WideCharToMultiByte(CP_UTF8, 0, strUnicodeString, wcslen(strUnicodeString), NULL, 0, NULL, NULL);
	char* pstr = new char[cchStr + 1];
	if( pstr != NULL )
		::WideCharToMultiByte(CP_UTF8, 0, strUnicodeString, -1, pstr, cchStr, NULL, NULL);
	pstr[cchStr] = '\0';
	std::string str(pstr);
	delete pstr;
	return str;
}
