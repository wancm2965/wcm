/************************************************************************/
//H323RegUser.h
//Copyright(c):AVCON
//Author:Gao
//Destination:支持多用户注册到GK
//Date:2013-12-23
/************************************************************************/

#ifndef __H323REGUSER_H__
#define __H323REGUSER_H__

#include <string>
#include <RVINC/cm.h>

#define MaxH323RegUserCount  200

class H323CallMgr;
class H323MediaCall;
class H323Call;

class H323RegUser 
{
public:
	H323RegUser(H323CallMgr& rH323CallMgr);
	~H323RegUser();

public:
	H323CallMgr&						m_rH323CallMgr;

public:
	void SetRegisterAccount(const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword,int nLineID,int natFwOption);
	int  SendGRQMessage();
	int  SendRegMessage(cmRASTransaction rasTransact,int type=0);
	void SetAccountCallMakeParam(H323MediaCall* pH323MediaCall,const char * cszUserID,char * szPacket,int nUseNAT);
	int AccountMakeCall(H323MediaCall* pH323MediaCall);
	int AccountAnswerCall(H323MediaCall* pH323MediaCall,const char*cszCallID,char * szPacket=NULL,void *pConf=NULL);

	std::string & GetRegH323ID() {return m_strH323ID;};
	std::string & GetRegPhoneNum() {return m_strPhoneNum;};
	std::string & GetRegPassword() {return m_strPassword;};
	void SetRegEPID(HRAS hRas);
	void SetAccountARQSrcInfo(H323MediaCall* pH323MediaCall,cmAlias * alias,int nUseNAT);
	void SetAccountRegInfo(cmAlias * aliasGK,cmAlias * aliasEP,int timeToLive);
	void SetAccountSignalCallAddr(cmTransportAddress * signalAddr);

	int  SendRegARQMessage(H323MediaCall* pH323MediaCall);
	int AccountCall(H323MediaCall* pH323MediaCall);
	void AccountCallRelease(H323MediaCall* pH323MediaCall,int type = -1); //recv ARJ release call
	void GetLocalIP(std::string & strLocalIP);
	void GetLocalIPByGK(std::string & strLocalIP);

protected:
	std::string				m_strH323ID;
	std::string				m_strPhoneNum;
	std::string				m_strPassword;
	std::string				m_strHost;
	unsigned int			m_nLineID;						//账号ID
	int						m_nUseGK;						//1 自动查找GK服务器,2 注册到指定的GK服务器
	cmAlias					m_SrcTermAlias;
	char					m_szSrcAliasBuf[256];
	cmAlias					m_GKAlias;
	char					m_szGKAliasBuf[256];
	cmAlias					m_EPAlias;
	char					m_szEPAliasBuf[256];
	int						m_nTimeToLive;                  //TimeToLive   s  ,默认为 2s
	cmTransportAddress		m_SignalCallAddr;
	std::string				m_strNonPacket;

public:
	int						m_nUseFWNAT;					//是否启用H460 NAT穿透, 0 不启用 ,1 启用,默认为 0
	RvBool					m_nSendKeepAlive;
	bool                    m_bRegResult;                   //default false,reg success set true
	int						m_nRegStatusTimestamp;		    //账号注册结果时间计时
	bool                    m_bRegFirst;
};





#endif//__H323REGUSER_H__