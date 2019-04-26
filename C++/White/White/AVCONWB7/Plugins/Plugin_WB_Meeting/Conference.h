#pragma once
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Function.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Room.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
using namespace AVCONPlugin;

//������תվ
class CConference  
	: public IPlugin_U7_MM_Function_Observer
	, public IPlugin_U7_MM_Room_Observer
	, public IPlugin_U7_MM_Member_Observer
{
public:	
	CConference(void);
	~CConference();
public:
	void EnterRoom(const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword);
	
	//�̳�IPlugin_U7_MM_Member_Observer�ӿ�
	virtual void OnRoomMemberItemEnd(void);
	
	//�̳�IPlugin_U7_MM_Room_Observer�ӿ�		
	virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType);
	virtual void OnEnterRoomReady();
	virtual void OnExitRoom(unsigned int nErrCode);
	// �̳�IPlugin_U7_MM_Function_Observer�ӿ�
	virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,const char* strCardName,int nChannelType);
	virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType);
	virtual void OnSetRight(int nRight, bool bEnable, const std::string& strMemberID);
public:
	std::string GetRoomName();
private:
	std::string m_strRoomName;
	unsigned short		m_usMemberType;//��������
};
CConference& GetConference(void);