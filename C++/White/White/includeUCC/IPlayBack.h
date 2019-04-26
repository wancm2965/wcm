// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

class IPlayBackNotify
{
public:
	IPlayBackNotify(void){};
	virtual~IPlayBackNotify(void){};
public:
	virtual void OnIPlayBackNotifyVideoPath(const char* VideoFilePath)=0;
};

class IPlayBack
{
public:
	 virtual void  SetPlayBackLanguageIniPath(const char* strIniFullPath)=0;
	 virtual bool  SetPlayBackFile(int Type,const char* ServIp,int ServerPort,const char* cUserName,const char* cPassword,int Channel)=0;
	 virtual void  DestoryPlayBackFile()=0;
	 virtual void  SetPlayBackToCenter(BOOL bSet=TRUE)=0;		//设置是否显示右键到中心播放
	 virtual void  SetPlayerPath(CString PlayerPath)=0;			//设置播放器路径
	 virtual bool  Show(int nCmdShow)=0;		
};	

AFX_EXT_CLASS  IPlayBack*  CreatePlayBackFile(const char* cDir,CWnd* pParentWnd,IPlayBackNotify* pNotify);

