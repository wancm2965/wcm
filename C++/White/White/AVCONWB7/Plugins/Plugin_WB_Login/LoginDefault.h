#pragma  once

#define  DEFAULT_SERVER_NAME	_T("www.avms.com.cn")//Ĭ�ϵ�¼IP

#define CONFIG_FILEPATH_OF_LOGIN	_T("WBConfig.ini")	//�����ļ�·��

//���������ļ��ؼ���
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