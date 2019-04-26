#pragma  once

#define  DEFAULT_SERVER_NAME	_T("www.avms.com.cn")//默认登录IP

#define CONFIG_FILEPATH_OF_LOGIN	_T("WBConfig.ini")	//配置文件路径

//保存配置文件关键字
#define  SEVER_KEYNAME  _T("server")
#define  USER_KEYNAME   _T("accont")
#define  PASSWORD_KEYNAME _T("password")

#define  SAVEUSERINFOLONG   11
#define  SELECTIONNAME   _T("user")
#define  LASTSELECTIONNAME _T("lastUser")

typedef struct WBLoginSaveUserInfo
{
	CString server;
	CString userinfo;
	CString password;
}WBLoginInfo,*pWBLoginInfo;