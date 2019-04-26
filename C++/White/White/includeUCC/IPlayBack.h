// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
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
	 virtual void  SetPlayBackToCenter(BOOL bSet=TRUE)=0;		//�����Ƿ���ʾ�Ҽ������Ĳ���
	 virtual void  SetPlayerPath(CString PlayerPath)=0;			//���ò�����·��
	 virtual bool  Show(int nCmdShow)=0;		
};	

AFX_EXT_CLASS  IPlayBack*  CreatePlayBackFile(const char* cDir,CWnd* pParentWnd,IPlayBackNotify* pNotify);

