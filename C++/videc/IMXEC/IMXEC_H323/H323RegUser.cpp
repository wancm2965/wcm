#include "stdafx.h"
#include "H323RegUser.h"
#include "H323CallMgr.h"
#include "CallMgr.h"
#include "XUtil.h"

extern const std::string& GetLocalIPByRemoteIP(const std::string&strHost,unsigned short nPort);
extern bool IsCallIPString(const char * str);
extern char * GetCallDisconnectReason(IMXEC_CALL_END_REASON nReason);
extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);

H323RegUser::H323RegUser(H323CallMgr& rH323CallMgr)
:m_rH323CallMgr(rH323CallMgr)
{
	m_nLineID=0;
	m_strH323ID ="";
	m_strPhoneNum="";
	m_strPassword="";
	m_strHost="";
	m_nTimeToLive=5 *1000;
	m_nUseGK=2;

	memset(&m_SrcTermAlias,0,sizeof(cmAlias) );
	memset(&m_szSrcAliasBuf,0,sizeof(m_szSrcAliasBuf));
	m_SrcTermAlias.string = m_szSrcAliasBuf;

	memset(&m_GKAlias,0,sizeof(cmAlias) );
	memset(&m_szGKAliasBuf,0,sizeof(m_szGKAliasBuf));
	m_GKAlias.string = m_szGKAliasBuf;

	memset(&m_EPAlias,0,sizeof(cmAlias) );
	memset(&m_szEPAliasBuf,0,sizeof(m_szEPAliasBuf));
	m_EPAlias.string = m_szEPAliasBuf;

	memset(&m_SignalCallAddr,0,sizeof(cmTransportAddress));
	m_nUseFWNAT =0;
	m_nSendKeepAlive = RV_FALSE;
	m_bRegResult=false;
	m_nRegStatusTimestamp=XGetTimestamp();
	m_bRegFirst=false;
}

H323RegUser::~H323RegUser()
{

}

void H323RegUser::SetRegisterAccount(const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword,int nLineID,int natFwOption)
{
	m_strH323ID =cszH323ID;
	m_strPhoneNum=cszTelNumber;
	m_strHost=cszHost;
	m_strPassword=cszRegPassword;
	m_nLineID =nLineID;
	m_nUseFWNAT=natFwOption;
}

int H323RegUser::SendGRQMessage()
{
	int nRet = -1;
	m_nUseGK=1;
	HRAS hRas = NULL;
	cmRASTransaction rasTransact=cmRASGatekeeper;
	char * cszH323ID=(char *)m_strH323ID.c_str();
	char * cszTelNumber=(char *)m_strPhoneNum.c_str();
	const char * cszHost="255.255.255.255";
	int nCastPort=m_rH323CallMgr.m_nRASCastPort;

	cmTransportAddress tranAddr;
	memset( &tranAddr, 0, sizeof(cmTransportAddress) );
	char addr[288]={0};
	sprintf(addr,"%s:%d",cszHost,nCastPort);
	cmString2TransportAddress(addr,&tranAddr);

	nRet = cmRASStartTransaction(m_rH323CallMgr.m_hApp,(HAPPRAS)this, &hRas, rasTransact, &tranAddr, NULL);
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}
	if(hRas == NULL) return -1;

	//TerminalAlias
	cmAlias termAlias;
	char bmpStr[288] = {0};
	if(m_rH323CallMgr.GetH323IDAlias(cszH323ID,termAlias,bmpStr) >=0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamTerminalAlias,0, sizeof(cmAlias), (char*)&termAlias );
	}
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}

	if(m_rH323CallMgr.GetE164IDAlias(cszTelNumber,termAlias,bmpStr) >=0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamTerminalAlias,1, sizeof(cmAlias), (char*)&termAlias );
	}
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}
	nRet = cmRASRequest( hRas );
	if(nRet < 0 )
	{
		cmRASClose(hRas);
		return -1;
	}

	return 0;
}

int H323RegUser::SendRegMessage(cmRASTransaction rasTransact,int type)
{
	if(m_bRegFirst && rasTransact == cmRASRegistration)
	{
		if(type == 1)
		{
			m_nSendKeepAlive =RV_FALSE;
			m_bRegResult =false;
			m_EPAlias.length =0;
			m_GKAlias.length =0;
		}
		int offsetTS=m_nTimeToLive  - 2000 - (m_nLineID +1) * 1000;  //m_nTimeToLive - 2s
		unsigned long nTimestamp=XGetTimestamp();
		if (m_nRegStatusTimestamp > nTimestamp)
		{
			m_nRegStatusTimestamp =nTimestamp;
		}
		else if ( (nTimestamp - m_nRegStatusTimestamp) >= m_nTimeToLive * 3 )
		{
			m_nRegStatusTimestamp=nTimestamp;
			//Reset Reg Status
			m_nSendKeepAlive =RV_FALSE;
			m_bRegResult =false;
			m_rH323CallMgr.OnDAP_H323RegistrationError(m_strH323ID.c_str(),m_strPhoneNum.c_str(),"REG TimeOut.");
		}
		else if ( (nTimestamp - m_nRegStatusTimestamp) < offsetTS )
		{
			//not to timer
			return 0;
		}
	}

	if(!m_bRegFirst && rasTransact == cmRASRegistration) m_bRegFirst=true;

	int nUseGK=m_nUseGK;
	int nFindGKAddr=m_rH323CallMgr.m_nFindGKAddr;
	cmTransportAddress * transGKAddr=&(m_rH323CallMgr.m_TransportAddr);
	int nGKPort=m_rH323CallMgr.m_nGateKeeperPort;

	int nRet = -1;
	HRAS hRas = NULL;
	char * cszH323ID=(char *)m_strH323ID.c_str();
	char * cszTelNumber=(char *)m_strPhoneNum.c_str();
	const char * cszHost=m_strHost.c_str();
	const char * cszRegPassword=m_strPassword.c_str();

	cmTransportAddress tranAddr;
	memset( &tranAddr, 0, sizeof(cmTransportAddress) );

	if( nUseGK == 1 && nFindGKAddr && transGKAddr)
	{
		memcpy( &tranAddr, transGKAddr, sizeof(cmTransportAddress) );
	}
	else
	{
		char addr[288]={0};
		sprintf(addr,"%s:%d",cszHost,nGKPort);
		cmString2TransportAddress(addr,&tranAddr);
	}

	nRet = cmRASStartTransaction(m_rH323CallMgr.m_hApp,(HAPPRAS)this, &hRas, rasTransact, &tranAddr, NULL);
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}
	if(hRas == NULL) return -1;

	//TerminalAlias
	cmAlias termAlias;
	char bmpStr[288] = {0};
	if(m_rH323CallMgr.GetH323IDAlias(cszH323ID,termAlias,bmpStr) >=0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamTerminalAlias,0, sizeof(cmAlias), (char*)&termAlias );
	}
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}

	if(m_rH323CallMgr.GetE164IDAlias(cszTelNumber,termAlias,bmpStr) >=0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamTerminalAlias,1, sizeof(cmAlias), (char*)&termAlias );
	}
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}

	if(rasTransact == cmRASRegistration)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamKeepAlive, 0, m_nSendKeepAlive, NULL);
		if(nRet < 0 )
		{
			cmRASClose( hRas );
			return -1;
		}
	}
	if( (m_nSendKeepAlive || rasTransact == cmRASUnregistration ) && m_EPAlias.length >0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamEndpointID,0, sizeof(cmAlias), (char*)&m_EPAlias);

	}
	if(m_GKAlias.length >0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamGatekeeperID,0, sizeof(cmAlias), (char*)&m_GKAlias);
	}

	nRet = cmRASRequest( hRas );
	if(nRet < 0 )
	{
		cmRASClose(hRas);
		return -1;
	}

	return 0;
}

void H323RegUser::SetRegEPID(HRAS hRas)
{
	int nStatus=0;
	if(m_EPAlias.length >0)
	{
		nStatus = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamEndpointID,0, sizeof(cmAlias), (char*)&m_EPAlias);
	}
	if(m_GKAlias.length >0)
	{
		nStatus = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamGatekeeperID,0, sizeof(cmAlias), (char*)&m_GKAlias);
	}
}

bool IsDigitString(const char * strTemp)
{
	int i=0;
	bool bDigit =false;
	if(strTemp ==NULL) return bDigit;
	int nLen=strlen(strTemp);
	if(nLen <=0) return bDigit;
	bDigit =true;
	while( i < nLen)
	{
		if(strTemp[i] <'0' || strTemp[i] >'9' )
		{
			bDigit=false;
			break;
		}
		i++;
	}
	return bDigit;
}

void H323RegUser::SetAccountARQSrcInfo(H323MediaCall* pH323MediaCall,cmAlias * alias,int nUseNAT)
{
	memcpy(&m_SrcTermAlias,alias,sizeof(cmAlias));
	m_SrcTermAlias.string =m_szSrcAliasBuf;
	memset(m_SrcTermAlias.string,0,sizeof(m_SrcTermAlias.string));
	memcpy(m_SrcTermAlias.string,alias->string,alias->length);
	m_nUseFWNAT=nUseNAT;
	pH323MediaCall->m_nMasterCall=1;
	pH323MediaCall->m_nUseFWNAT=nUseNAT;
}

void H323RegUser::SetAccountRegInfo(cmAlias * aliasGK,cmAlias * aliasEP,int timeToLive)
{
	memcpy(&m_GKAlias,aliasGK,sizeof(cmAlias));
	m_GKAlias.string =m_szGKAliasBuf;
	memset(m_GKAlias.string,0,sizeof(m_GKAlias.string));
	memcpy(m_GKAlias.string,aliasGK->string,aliasGK->length);

	if( !m_nSendKeepAlive)
	{
		memcpy(&m_EPAlias,aliasEP,sizeof(cmAlias));
		m_EPAlias.string =m_szEPAliasBuf;
		memset(m_EPAlias.string,0,sizeof(m_EPAlias.string));
		memcpy(m_EPAlias.string,aliasEP->string,aliasEP->length);
		m_nSendKeepAlive = RV_TRUE;
	}

	m_bRegResult=true;
	if(timeToLive >0) m_nTimeToLive=timeToLive * 1000;
	else m_nTimeToLive=20 *1000;
	m_nRegStatusTimestamp=XGetTimestamp();

}

void H323RegUser::SetAccountSignalCallAddr(cmTransportAddress * signalAddr)
{
	//struct in_addr		addr;
	//addr.S_un.S_addr	=signalAddr->ip;
	//char * szIP = inet_ntoa(addr);
	//int port = signalAddr->port;
	memcpy(&m_SignalCallAddr,signalAddr,sizeof(cmTransportAddress));
}

int H323RegUser::SendRegARQMessage(H323MediaCall* pH323MediaCall)
{
	int nUseGK=m_nUseGK;
	int nFindGKAddr=m_rH323CallMgr.m_nFindGKAddr;
	cmTransportAddress * transGKAddr=&(m_rH323CallMgr.m_TransportAddr);
	int nGKPort=m_rH323CallMgr.m_nGateKeeperPort;

	int nRet = -1;
	HRAS hRas = NULL;
	char * cszH323ID=(char *)m_strH323ID.c_str();
	char * cszTelNumber=(char *)m_strPhoneNum.c_str();
	const char * cszHost=m_strHost.c_str();
	const char * cszRegPassword=m_strPassword.c_str();
	cmAlias termAlias;
	char bmpStr[288] = {0};
	char * gkIP="";
	RvUint16 port=m_rH323CallMgr.m_nGateKeeperPort;
	int nBandWidth= pH323MediaCall->m_nBandWidth *2; //notice modify 20140707
	std::string & strCalled = pH323MediaCall->m_strCalled ;

	cmTransportAddress tranAddr;
	memset( &tranAddr, 0, sizeof(cmTransportAddress) );


	//save to H323Call
	if(pH323MediaCall)
	{
		pH323MediaCall->m_strHost =m_strHost;
		pH323MediaCall->m_nRegUseGK =m_nUseGK;
		memcpy(&pH323MediaCall->m_GKAlias,&m_GKAlias,sizeof(cmAlias));
		memcpy(pH323MediaCall->m_GKAlias.string,m_GKAlias.string,m_GKAlias.length);

		memcpy(&pH323MediaCall->m_EPAlias,&m_EPAlias,sizeof(cmAlias));
		memcpy(pH323MediaCall->m_EPAlias.string,m_EPAlias.string,m_EPAlias.length);

	}

	if( nUseGK == 1 && nFindGKAddr && transGKAddr)
	{
		memcpy( &tranAddr, transGKAddr, sizeof(cmTransportAddress) );
		struct in_addr		addr;
		addr.S_un.S_addr	=tranAddr.ip;
		gkIP = inet_ntoa(addr);
		port = tranAddr.port;
	}
	else
	{
		char addr[288]={0};
		sprintf(addr,"%s:%d",cszHost,nGKPort);
		cmString2TransportAddress(addr,&tranAddr);
		gkIP=(char *)cszHost;
		port=nGKPort;
	}

	HCALL hsCall=pH323MediaCall->m_hsCall;

	pH323MediaCall->m_strLocalIP =GetLocalIPByRemoteIP(gkIP,port);

	nRet = cmRASStartTransaction(m_rH323CallMgr.m_hApp,(HAPPRAS)this, &hRas, cmRASAdmission, &tranAddr,hsCall);
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}
	if(hRas == NULL) return -1;

	//TerminalAlias
	cmRASParam rasFirstParam;
	cmRASParam rasSecondParam;
	if(pH323MediaCall->m_nMasterCall <= 0)
	{
		rasFirstParam=cmRASParamSrcInfo;
		rasSecondParam =cmRASParamDestInfo;
	}
	else
	{
		rasFirstParam=cmRASParamDestInfo;
		rasSecondParam =cmRASParamSrcInfo;
	}

	if(m_rH323CallMgr.GetH323IDAlias(cszH323ID,termAlias,bmpStr) >=0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, rasFirstParam,0, sizeof(cmAlias), (char*)&termAlias );
	}
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}

	if(m_rH323CallMgr.GetE164IDAlias(cszTelNumber,termAlias,bmpStr) >=0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, rasFirstParam,1, sizeof(cmAlias), (char*)&termAlias );
	}
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}

	if(pH323MediaCall->m_nMasterCall <= 0)
	{
		if(IsCallIPString(strCalled.c_str() ) ==true)
		{
			int nPort=m_rH323CallMgr.m_nH323ListenPort;
			cmTransportAddress dstCallSignalAddress;
			memset( &dstCallSignalAddress, 0, sizeof(cmTransportAddress) );
			char addr[288]={0};
			sprintf(addr,"%s:%u",strCalled.c_str(),nPort);
			nRet=cmString2TransportAddress(addr,&dstCallSignalAddress);
			if(nRet >=0)
				nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamDestCallSignalAddress,0, sizeof(cmTransportAddress), (char*)&dstCallSignalAddress);
			if(nRet < 0 )
			{
				cmRASClose( hRas );
				return -1;
			}
		}
		else if(IsDigitString(strCalled.c_str() ) == true)
		{
			cszTelNumber=(char *)strCalled.c_str();
			if(m_rH323CallMgr.GetE164IDAlias(cszTelNumber,termAlias,bmpStr) >=0)
			{
				nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamDestInfo,0, sizeof(cmAlias), (char*)&termAlias );
			}
			if(nRet < 0 )
			{
				cmRASClose( hRas );
				return -1;
			}
		}
		else
		{
			cszH323ID=(char *)strCalled.c_str();
			if(m_rH323CallMgr.GetH323IDAlias(cszH323ID,termAlias,bmpStr) >=0)
			{
				nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamDestInfo,0, sizeof(cmAlias), (char*)&termAlias );
			}
			if(nRet < 0 )
			{
				cmRASClose( hRas );
				return -1;
			}
		}
	}
	else
	{
		if(m_SrcTermAlias.length >0)
		{
			nRet = cmRASSetParam( hRas, cmRASTrStageRequest, rasSecondParam,0, sizeof(cmAlias), (char*)&m_SrcTermAlias);
		}
	}

	if(m_EPAlias.length >0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamEndpointID,0, sizeof(cmAlias), (char*)&m_EPAlias);
	}

	if(m_GKAlias.length >0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamGatekeeperID,0, sizeof(cmAlias), (char*)&m_GKAlias);
	}

	if(pH323MediaCall)
	{
		int len1=sizeof(pH323MediaCall->m_szConferenceID);
		int len2=sizeof(pH323MediaCall->m_szCallEndpointID);
		cmCallGetParam(pH323MediaCall->m_hsCall,cmParamCID,0,&len1,pH323MediaCall->m_szConferenceID);
		cmCallGetParam(pH323MediaCall->m_hsCall,cmParamCallID,0,&len2,pH323MediaCall->m_szCallEndpointID);
		cmCallGetParam(pH323MediaCall->m_hsCall,cmParamCRV,0,&pH323MediaCall->m_nParamCRV,NULL);
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamCID,0,len1,pH323MediaCall->m_szConferenceID);
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamCallID,0,len2,pH323MediaCall->m_szCallEndpointID);
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamCRV,0,pH323MediaCall->m_nParamCRV,NULL);
	}

	//cmTransportAddress srcCallSignalAddress;
	//cmCallGetParam(m_pH323MediaCall->m_hsCall,cmParamSrcCallSignalAddress,0,0,(char*)&srcCallSignalAddress);
	////nSrcPort get ?
	//if(m_nMasterCall <= 0)
	//{
	//	int nSrcPort=0;
	//	std::string strLocalIP=GetLocalIPByRemoteIP(gkIP,port);
	//	cmTransportAddress srcCallSignalAddress;
	//	memset( &srcCallSignalAddress, 0, sizeof(cmTransportAddress) );
	//	char addr[288]={0};
	//	sprintf(addr,"%s:%u",strLocalIP.c_str(),nSrcPort);
	//	nRet=cmString2TransportAddress(addr,&srcCallSignalAddress);
	//	if(nRet >=0) cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamSrcCallSignalAddress,0, sizeof(cmTransportAddress), (char*)&srcCallSignalAddress);
	//}

	nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamBandwidth,0, nBandWidth * 1000, NULL);

	nRet = cmRASRequest( hRas );
	if(nRet < 0 )
	{
		cmRASClose(hRas);
		return -1;
	}

	return 0;
}

void H323RegUser::SetAccountCallMakeParam(H323MediaCall* pH323MediaCall,const char * cszUserID,char * szPacket,int nUseNAT)
{
	pH323MediaCall->m_strCalled =cszUserID;
	if(szPacket) m_strNonPacket=szPacket;
	m_nUseFWNAT=nUseNAT;
	pH323MediaCall->m_nMasterCall=0;
	pH323MediaCall->m_nUseFWNAT=nUseNAT;
}

int H323RegUser::AccountMakeCall(H323MediaCall* pH323MediaCall)
{
	int ret=-1;
	LogFile(NULL,NULL,"","","H323RegUser AccountMakeCall 111.");
	if(pH323MediaCall)
	{
		char * cszUserID=NULL;
		RvUint16 port=m_rH323CallMgr.m_nH323ListenPort;
		char * szPacket=(char *)m_strNonPacket.c_str();
		if(m_strNonPacket.size() ==0) szPacket=NULL;
		int nBandWidth=pH323MediaCall->m_nBandWidth;
		ConfInfo *pConfInfo=&(pH323MediaCall->m_ConfInfo);
		HCALL hsCall=pH323MediaCall->m_hsCall;
		std::string & strCalled = pH323MediaCall->m_strCalled ;
		//cmCallSetHandle(hsCall,(HAPPCALL)pH323MediaCall);

		m_rH323CallMgr.H235SecurityCallInit(pH323MediaCall);

		pH323MediaCall->InitMediaRtpPortAllocate();
		LogFile(NULL,NULL,"","","H323RegUser InitMediaRtpPortAllocate.");
		//if(IsCallIPString(strCalled.c_str() ) ==true)
		//{
		//	port=m_rH323CallMgr.m_nH323ListenPort;
		//	cszUserID=(char *)strCalled.c_str();
		//}
		//else
		{
			struct in_addr addr;
			addr.S_un.S_addr =m_SignalCallAddr.ip;
			cszUserID=inet_ntoa(addr);//change remote ip
			port = m_SignalCallAddr.port;
		}

		char szDest[512]="";
		if ((!strchr(cszUserID,',') && strchr(cszUserID,'.')) || strchr(cszUserID,',')>strchr(cszUserID,'.') && strchr(cszUserID,'.'))
		{
			strcpy(szDest,"TA:");
			strcat(szDest, cszUserID);
			pH323MediaCall->m_strLocalIP=GetLocalIPByRemoteIP(cszUserID,port);
			if(port !=m_rH323CallMgr.m_nH323ListenPort) //change remote port
			{
				char szBufTemp[64]={0};
				sprintf(szBufTemp,":%u",port);
				strcat(szDest,szBufTemp);
			}
		}

		//notice the set is use NETEC Send
		if(CallMgr::Instance().m_nUseNETECSend > 0 )
		{
			//if (pConfInfo != NULL && pConfInfo->m_nUseFWNAT <= 0 )
			{
				m_rH323CallMgr.SetUserNonstandardInfo(pH323MediaCall,cmCallStateOffering,pConfInfo,szPacket);
				LogFile(NULL,NULL,"","","H323RegUser SetUserNonstandardInfo.");
			}
		}

		std::string strAVCON_H323=m_rH323CallMgr.m_strProName;
		std::string strAVCON_UserName=m_rH323CallMgr.m_strProName;
		//A7 add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
		strAVCON_UserName=m_rH323CallMgr.m_strProName;
		strAVCON_H323="Product of AVCON";
#endif

		if( (m_strH323ID.size()>0) && (m_strH323ID.size()<255) )
			strAVCON_H323=m_strH323ID;
		if( (m_rH323CallMgr.m_strUserName.size()>0) && (m_rH323CallMgr.m_strUserName.size()<255) )
			strAVCON_UserName=m_rH323CallMgr.m_strUserName;

		if( pConfInfo!=NULL && strlen(pConfInfo->m_abyConfName)>0 )
		{
			//strAVCON_UserName += pConfInfo->m_abyConfName;
			strAVCON_UserName = pConfInfo->m_abyConfName;
			strAVCON_H323=pConfInfo->m_abyConfName;
		}

		//A7 add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) 
		//added by jiangdingfeng 2013-7-25 如果携带了对端MCU会议E164号码(会议号码)
		if (pConfInfo != NULL && strlen(pConfInfo->m_abyConfE164)>0)
		{
			m_rH323CallMgr.AddQ931CallPartyNumber(hsCall,cszUserID,cmParamCalledPartyNumber,pConfInfo->m_abyConfE164);
			char szTemp[256] ={0};
			sprintf(szTemp,",TEL:%s", pConfInfo->m_abyConfE164);
			strcat(szDest, szTemp);
		}

		if (pConfInfo != NULL && strlen(pConfInfo->m_abyLocalConfE164)>0)
		{
			m_rH323CallMgr.AddQ931CallPartyNumber(hsCall,cszUserID,cmParamCallingPartyNumber,pConfInfo->m_abyLocalConfE164);
		}

#endif

		//set call param
		char * cszH323ID=(char *)m_strH323ID.c_str();
		char * cszTelNumber=(char *)m_strPhoneNum.c_str();
		cmAlias termAlias;
		char bmpStr[288] = {0};
		if(m_rH323CallMgr.GetE164IDAlias(cszTelNumber,termAlias,bmpStr) >=0)
		{
			cmCallSetParam(hsCall, cmParamSourceAddress,0,sizeof(cmAlias), (char*)&termAlias );
		}
		else if(m_rH323CallMgr.GetH323IDAlias(cszH323ID,termAlias,bmpStr) >=0)
		{
			cmCallSetParam(hsCall, cmParamSourceAddress,0,sizeof(cmAlias), (char*)&termAlias );
		}

		if(IsDigitString(strCalled.c_str() ) == true)
		{
			cszTelNumber=(char *)strCalled.c_str();
			if(m_rH323CallMgr.GetE164IDAlias(cszTelNumber,termAlias,bmpStr) >=0)
				cmCallSetParam(hsCall, cmParamDestinationAddress,0,sizeof(cmAlias), (char*)&termAlias );
		}
		else
		{
			cszH323ID=(char *)strCalled.c_str();
			if(m_rH323CallMgr.GetH323IDAlias(cszH323ID,termAlias,bmpStr) >=0)
				cmCallSetParam(hsCall, cmParamDestinationAddress,0,sizeof(cmAlias), (char*)&termAlias );
		}

		char szCallIDBack[288]={0}; strcpy(szCallIDBack,pH323MediaCall->GetCallID().c_str());
		ret=cmCallMake(hsCall, nBandWidth*1000, 0, szDest, NULL, strAVCON_UserName.c_str(),strAVCON_H323.c_str(), strAVCON_H323.length());
		if(ret < 0)
		{
			IMXEC_CALL_END_REASON discReason=ModeDisconnectedError;
			pH323MediaCall->DisconnectReason()=GetCallDisconnectReason(discReason);
			H323CallMgr::Instance().H235SecurityCallEnd(pH323MediaCall);
			H323CallMgr::Instance().OnDAP_CallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),discReason,pH323MediaCall->DisconnectReason());
			cmCallClose(hsCall);
			m_rH323CallMgr.OutPutStackLog(0,"H323Stack MakeCall Error.",szCallIDBack);
		}

		if(ret >=0) m_rH323CallMgr.OutPutStackLog(0,"H323Stack MakeCall OK.",szCallIDBack);

		{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)
			XAutoLock l(m_rH323CallMgr.m_csMapH323MediaCall);
#endif
			m_rH323CallMgr.m_MapH323MediaCall[pH323MediaCall->GetCallID()] = pH323MediaCall;
		}

	}
	LogFile(NULL,NULL,"","","H323RegUser AccountMakeCall 222.");
	return ret;
}


int H323RegUser::AccountAnswerCall(H323MediaCall* pH323MediaCall,const char*cszCallID,char * szPacket,void *pConf)
{
	int ret=-1;
	if(szPacket) m_strNonPacket=szPacket;
	if(pH323MediaCall)
	{
		ConfInfo *pConfInfo =(ConfInfo * )pConf;
		HCALL hsCall=pH323MediaCall->m_hsCall;
		const char * cszCallID=pH323MediaCall->GetCallID().c_str();

		if(CallMgr::Instance().m_nUseNETECSend > 0 )
		{
			//if (pConfInfo != NULL && pConfInfo->m_nUseFWNAT <= 0 )
			{
				m_rH323CallMgr.SetUserNonstandardInfo(pH323MediaCall,cmCallStateConnected,pConfInfo,szPacket);
			}
		}
		std::string strAVCON_H323=m_rH323CallMgr.m_strProName;
		std::string strAVCON_UserName=m_rH323CallMgr.m_strProName;
		if( (m_strH323ID.size()>0) && (m_strH323ID.size()<255) )
			strAVCON_H323=m_strH323ID;
		if( (m_rH323CallMgr.m_strUserName.size()>0) && (m_rH323CallMgr.m_strUserName.size()<255) )
			strAVCON_UserName=m_rH323CallMgr.m_strUserName;

		if( pConfInfo!=NULL && strlen(pConfInfo->m_abyConfName)>0 )
		{
			//strAVCON_UserName +=pConfInfo->m_abyConfName;
			strAVCON_UserName = pConfInfo->m_abyConfName;
			strAVCON_H323=pConfInfo->m_abyConfName;
		}
		if (pConfInfo != NULL)
		{
			pH323MediaCall->AcceptCallSetCapabilities(pConfInfo);
		}
		//set call param
		char * cszH323ID=(char *)m_strH323ID.c_str();
		char * cszTelNumber=(char *)m_strPhoneNum.c_str();
		cmAlias termAlias;
		char bmpStr[288] = {0};
		if(m_rH323CallMgr.GetE164IDAlias(cszTelNumber,termAlias,bmpStr) >=0)
		{
			cmCallSetParam(hsCall, cmParamConnectedAddress,0,sizeof(cmAlias), (char*)&termAlias );
		}
		else if(m_rH323CallMgr.GetH323IDAlias(cszH323ID,termAlias,bmpStr) >=0)
		{
			cmCallSetParam(hsCall, cmParamConnectedAddress,0,sizeof(cmAlias), (char*)&termAlias );
		}

		char szCallIDBack[288]={0}; strcpy(szCallIDBack,cszCallID);
		ret=cmCallAnswerExt(hsCall,strAVCON_UserName.c_str(),strAVCON_H323.c_str(),strAVCON_H323.length());
		if(ret < 0)
		{
			IMXEC_CALL_END_REASON discReason=ModeDisconnectedError;
			pH323MediaCall->DisconnectReason()=GetCallDisconnectReason(discReason);
			H323CallMgr::Instance().H235SecurityCallEnd(pH323MediaCall);
			H323CallMgr::Instance().OnDAP_CallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),discReason,pH323MediaCall->DisconnectReason());
			cmCallClose(hsCall);
			m_rH323CallMgr.OutPutStackLog(0,"H323Stack AcceptCall Error.",szCallIDBack);
		}

		if(ret >=0) m_rH323CallMgr.OutPutStackLog(0,"H323Stack AcceptCall OK.",szCallIDBack);
	}
	return ret;
}

int H323RegUser::AccountCall(H323MediaCall* pH323MediaCall)
{
	int nRet=-1;
	if(pH323MediaCall)
	{
		pH323MediaCall->m_pH323RegUser=this;

		if(pH323MediaCall->m_nMasterCall <= 0)
		{
			nRet=AccountMakeCall(pH323MediaCall);
		}
		else
		{
			m_rH323CallMgr.AddCall(pH323MediaCall);
			pH323MediaCall->OnCallRecvACF(this);
		}
	}
	return nRet;
}

void H323RegUser::AccountCallRelease(H323MediaCall* pH323MediaCall,int type)
{
	if(pH323MediaCall)
	{
		if(type >=0)
		{
			cmCallDropParam(pH323MediaCall->m_hsCall,(cmReasonType)type);
		}
		else
		{
			cmCallClose(pH323MediaCall->m_hsCall);
		}
		delete pH323MediaCall;
		pH323MediaCall=NULL;
	}
}

void H323RegUser::GetLocalIP(std::string & strLocalIP)
{
	struct in_addr		addr;
	addr.S_un.S_addr	=m_SignalCallAddr.ip;
	char * szIP=inet_ntoa(addr);
	RvUint16 port = m_SignalCallAddr.port;
	strLocalIP==GetLocalIPByRemoteIP(szIP,port);
}

void H323RegUser::GetLocalIPByGK(std::string & strLocalIP)
{
	int nUseGK=m_nUseGK;
	int nFindGKAddr=m_rH323CallMgr.m_nFindGKAddr;
	cmTransportAddress * transGKAddr=&(m_rH323CallMgr.m_TransportAddr);
	int nGKPort=m_rH323CallMgr.m_nGateKeeperPort;
	const char * cszHost=m_strHost.c_str();
	char * gkIP="";
	RvUint16 port=m_rH323CallMgr.m_nGateKeeperPort;
	if( nUseGK == 1 && nFindGKAddr && transGKAddr)
	{
		struct in_addr		addr;
		addr.S_un.S_addr	=transGKAddr->ip;
		gkIP = inet_ntoa(addr);
		port = transGKAddr->port;
	}
	else
	{
		gkIP=(char *)cszHost;
	}
	strLocalIP = GetLocalIPByRemoteIP(gkIP,port);

}
