#include "stdafx.h"
#include "stkHandler.h"
#include "H323CallMgr.h"
#include <RVINC/md5c.h>
#include <RVINC/q931asn1.h>
#include "CallMgr.h"
#include <mstcpip.h>

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
#include "H323A7DAPCall.h"
#else 
#include "H323DAPCall.h"
#endif

#else
#include "H323MediaCall.h"
#endif

#include "H323RegUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef MD5_ID
//#define MD5_ID "1 2 840 113549 2 5"
#define MD5_ID "2 1872458839 1869769076 1751973888 0 0 0 0 0 0 0 0 0 0 0"
//第一个是科达用的,第二个是华为用的,暂定使用华为用的,如注册科达GK请使用第一个定义值
#endif

#define OutputPVTNode 1 

extern void SetDisConnectReason();
extern IMXEC_CALL_END_REASON GetCallDisconnectReason(char * szReason);
extern char * GetCallDisconnectReason(IMXEC_CALL_END_REASON nReason);
extern void LogFileChannel(H323MediaCall * pH323Call,void * p2, HCHAN hsChan,const char * cszUserID,const char * callID,const char * info,int type);
extern void LogFileCall(H323MediaCall * pH323Call,void * p2, HCHAN hsChan,const char * cszUserID,const char * callID,const char * info,int type);
extern int GetLocalFirstIP(std::string & strLocalIP);
extern const std::string& GetLocalIPByRemoteIP(const std::string&strHost,unsigned short nPort);
extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);

//notice FlowControlCommand use to terminal
void SetChannelFlowBitrate(H323Channel* pH323Channel,int nBitrate);
//modify by gsb 20140707
EM_ENTITY_TYPE GetTerminalType(H323MediaCall* pH323MediaCall,HPVT hVal,int nodeId,int type); //1 setup 2 connect

void CloseRasCall(HAPPRAS haRas,HRAS hsRas,IMXEC_CALL_END_REASON nEndReason,int reason);

//H323协议栈回调函数

void GetAutoRasRejectReason(int nodeId,int & nRasReason)
{
	typedef struct tagFieldReasons
	{
		int reason;
		int nameId;
	}fieldReasons;

	static fieldReasons rasRejectReasons[] =
	{
		{cmRASReasonResourceUnavailable ,             __q931(resourceUnavailable)},
		{cmRASReasonInsufficientResources ,           __q931(insufficientResources)},
		{cmRASReasonInvalidRevision ,                 __q931(invalidRevision)},
		{cmRASReasonInvalidCallSignalAddress ,        __q931(invalidCallSignalAddress)},
		{cmRASReasonInvalidRASAddress ,               __q931(invalidRASAddress)},
		{cmRASReasonInvalidTerminalType ,             __q931(invalidTerminalType)},
		{cmRASReasonInvalidPermission ,               __q931(invalidPermission)},
		{cmRASReasonInvalidConferenceID ,             __q931(invalidConferenceID)},
		{cmRASReasonInvalidEndpointID ,               __q931(invalidEndpointIdentifier)},
		{cmRASReasonCallerNotRegistered ,             __q931(callerNotRegistered)},
		{cmRASReasonCalledPartyNotRegistered ,        __q931(calledPartyNotRegistered)},
		{cmRASReasonDiscoveryRequired ,               __q931(discoveryRequired)},
		{cmRASReasonDuplicateAlias ,                  __q931(duplicateAlias)},
		{cmRASReasonTransportNotSupported ,           __q931(transportNotSupported)},
		{cmRASReasonCallInProgress ,                  __q931(callInProgress)},
		{cmRASReasonRouteCallToGatekeeper ,           __q931(routeCallToGatekeeper)},
		{cmRASReasonRequestToDropOther ,              __q931(requestToDropOther)},
		{cmRASReasonNotRegistered ,                   __q931(notRegistered)},
		{cmRASReasonUndefined ,                       __q931(undefinedReason)},
		{cmRASReasonTerminalExcluded ,                __q931(terminalExcluded)},
		{cmRASReasonNotBound ,                        __q931(notBound)},
		{cmRASReasonNotCurrentlyRegistered ,          __q931(notCurrentlyRegistered)},
		{cmRASReasonRequestDenied ,                   __q931(requestDenied)},
		{cmRASReasonLocationNotFound ,                __q931(requestDenied)},
		{cmRASReasonSecurityDenial ,                  __q931(securityDenial)},
		{cmRASReasonTransportQOSNotSupported ,        __q931(transportQOSNotSupported)},
		{cmRASResourceUnavailable ,                   __q931(resourceUnavailable)},
		{cmRASReasonInvalidAlias ,                    __q931(invalidAlias)},
		{cmRASReasonPermissionDenied ,                __q931(permissionDenied)},
		{cmRASReasonQOSControlNotSupported ,          __q931(qosControlNotSupported)},
		{cmRASReasonIncompleteAddress ,               __q931(incompleteAddress)},
		{cmRASReasonFullRegistrationRequired ,        __q931(fullRegistrationRequired)},
		{cmRASReasonRouteCallToSCN ,                  __q931(routeCallToSCN)},
		{cmRASReasonAliasesInconsistent ,             __q931(aliasesInconsistent)},
		{cmRASReasonAdditiveRegistrationNotSupported , __q931(additiveRegistrationNotSupported)},
		{cmRASReasonInvalidTerminalAliases ,          __q931(invalidTerminalAliases)},
		{cmRASReasonExceedsCallCapacity ,             __q931(exceedsCallCapacity)},
		{cmRASReasonCollectDestination ,              __q931(collectDestination)},
		{cmRASReasonCollectPIN ,                      __q931(collectPIN)},
		{cmRASReasonGenericData ,                     __q931(genericDataReason)},
		{cmRASReasonNeededFeatureNotSupported ,       __q931(neededFeatureNotSupported)},
		{cmRASReasonUnknownMessageResponse ,          0}, /* This one is invalid */
		{cmRASReasonHopCountExceeded ,                __q931(hopCountExceeded)},
		{cmRASReasonSecurityDHmismatch ,              __q931(securityDHmismatch)},
		{cmRASReasonNoRouteToDestination ,            __q931(noRouteToDestination)},
		{cmRASReasonUnallocatedNumber ,               __q931(unallocatedNumber)}

	};

	int nCount=sizeof(rasRejectReasons)/sizeof(fieldReasons);
	nRasReason = rasRejectReasons[nCount -1].reason +1;

	if(nodeId >=0)
	{
		HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
		int fieldID =0;
		int sub=pvtGetByPath(hVal,nodeId,"registrationReject.rejectReason",NULL,0,NULL);
		if(sub >=0) sub=pvtChild(hVal,sub);
		if(sub >=0) 
		{
			pvtGet(hVal,sub,&fieldID,NULL,NULL,NULL);
			for(int i=0;i<nCount;i++)
			{
				if(fieldID == rasRejectReasons[i].nameId)
				{
					nRasReason =rasRejectReasons[i].reason;
					break;
				}
			}
		}
	}
}

int RVCALLCONV cmEvRegEvent(HAPP hApp, cmRegState regState, cmRegEvent regEvent, RvPvtNodeId regEventHandle)
{
	H323CallMgr::Instance().m_bRegToGKStateOld = H323CallMgr::Instance().m_bRegisteredToGK;
	switch(regEvent)
	{
	case cmRegistrationReject:
		GetAutoRasRejectReason(regEventHandle,H323CallMgr::Instance().m_nREGRejectReason);
		break;
	default:
		break;
	}
	switch(regState)
	{
	case cmIdle:
		// reset the security sender ID
		if (regEvent != cmPermanentAlternateGatekeeperConfirmation)
			H323CallMgr::Instance().m_h235Mgr.H235SecurityAuthSetting();

		H323CallMgr::Instance().m_bRegisteredToGK = false;
		break;
	case cmDiscovered:
		H323CallMgr::Instance().m_bRegisteredToGK = false;
		if (H323CallMgr::Instance().m_nUseGK==1)//自动发现
		{
			HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
			int nodeId=pvtGetByPath(hVal,regEventHandle,"gatekeeperConfirm.rasAddress",NULL,0,NULL);
			if(nodeId >=0)
			{
				cmTransportAddress tranAddr;
				memset( &tranAddr, 0, sizeof(cmTransportAddress) );
				int status=cmVt2TransportAddress(hVal,nodeId,&tranAddr);
				if(status == 0 && tranAddr.type == cmTransportTypeIP )
				{
					struct in_addr		addr;
					addr.S_un.S_addr	=tranAddr.ip;
					H323CallMgr::Instance().m_strRegHost=inet_ntoa(addr);
					H323CallMgr::Instance().m_nRegGKPort=tranAddr.port;
				}
			}
			cmRegister(H323CallMgr::Instance().m_hApp);
		}
		break;
	case cmRegistered:
		{
			HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
			int nTimetoLive =0;
			int nKeepAlive =0 ;
			int nodeId=pvtGetByPath(hVal,regEventHandle,"registrationConfirm.keepAlive",NULL,&nKeepAlive,NULL);
			if(nodeId >=0 && nKeepAlive ==0)
			{
				nodeId=pvtGetByPath(hVal,regEventHandle,"registrationConfirm.timeToLive",NULL,&nTimetoLive,NULL);
				if(nodeId >=0 && nTimetoLive >0) H323CallMgr::Instance().m_nTimeToLive= nTimetoLive *1000;
			}
		}
		H323CallMgr::Instance().m_bRegisteredToGK = true;
		break;
	default:
		H323CallMgr::Instance().m_bRegisteredToGK = false;
		break;
	}
	{
		H323CallMgr::Instance().OnRegistrationState(H323CallMgr::Instance().m_bRegisteredToGK);
	}
	return 0;
}

void GetAlias(int nodeId,char * path,std::string & strE164ID,std::string & strH323ID,cmAlias & alias)
{
	HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	int status=-1;
	int adrNode = -1;
	int adrNodeBack=-1;
	cmAliasType type;

	if(nodeId >=0 )
	{
		int aliasNode=pvtGetNodeIdByPath(hVal,nodeId,path);

		__pvtGetNodeIdByFieldIds(adrNode, hVal, aliasNode, {_q931(aliasAddress) _anyField LAST_TOKEN});
		adrNodeBack=adrNode;
		//adrNode=aliasNode;
		adrNode=pvtChild(hVal,aliasNode);
		//{
		//OutputDebugString("Decode!\n");
		//OutputDebugString("Decode Error!\n");}
		type=cmAliasTypeE164;
		status=-1;
		while (RV_PVT_NODEID_IS_VALID(adrNode) && (status != RV_OK))
		{
			cmVt2Alias(hVal, &alias, adrNode);
			if (alias.type == type)
				status=RV_OK;
			else
				adrNode = pvtBrother(hVal, adrNode);
		}
		if(status == RV_OK)
		{
			strE164ID =alias.string;
		}

		adrNode=adrNodeBack;
		adrNode=pvtChild(hVal,aliasNode);
		type=cmAliasTypeH323ID;
		status=-1;
		while (RV_PVT_NODEID_IS_VALID(adrNode) && (status != RV_OK))
		{
			cmVt2Alias(hVal, &alias, adrNode);
			if (alias.type == type)
				status=RV_OK;
			else
				adrNode = pvtBrother(hVal, adrNode);
		}
		if(status == RV_OK)
		{
			char idBuf[288]={0};
			int length =utlBmp2Chr(idBuf,(RvUint8 *)alias.string,alias.length);
			if(length >=0) strH323ID =idBuf;
		}
	}
}

bool GetQ931CallPartyNumber(HCALL hsCall,cmCallParam callParam,std::string & strQ931CallNumber)
{
	bool bRet=false;
	strQ931CallNumber ="";
	cmAlias al;
	int nValue =0;
	char buff[288]={0};
	memset(&al,0,sizeof(al));
	al.string =buff;
	int status =cmCallGetParam(hsCall,callParam,0,&nValue,(char*)&al);
	if(status >=0 && al.type == cmAliasTypeE164 && al.pnType == cmPartyNumberPublicUnknown)
	{
		strQ931CallNumber =al.string;
		bRet=true;
	}
	return bRet;
}

int RVCALLCONV cmEvNewCall(HAPP hApp, HCALL hsCall, LPHAPPCALL lphaCall)
{
	LogFile(NULL,NULL,"","","cmEvNewCall         111");
	std::string strE164ID="";
	std::string strH323ID="";
	std::string strTermE164ID="";
	std::string strTermH323ID="";
	H323RegUser* pH323RegUser=NULL;
	H323MediaCall* pH323MediaCall =NULL;
	int nCallRate=0;
	cmCallGetParam(hsCall,cmParamRequestedRate, 0, &nCallRate,NULL);

	char msgbuf[128]={0};
	sprintf(msgbuf,"------> NewCall,callRate(kb):%d\n",nCallRate/1000);
	OutputDebugString(msgbuf);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	pH323MediaCall =  new H323MediaCall(H323CallMgr::Instance(), hsCall);
	int nodeId=cmCallGetMessage(hsCall,cmQ931setup);
	if(pH323MediaCall && nodeId >=0 )
	{
		cmAlias alias;
		char  buf[288]={0};
		alias.string = buf;
		char path[288]={0};//destinationAddress
		sprintf(path,"message.setup.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.setup.destinationAddress");
		GetAlias(nodeId,path,strE164ID,strH323ID,alias);
		if(strE164ID.size() >0)
			pH323RegUser=H323CallMgr::Instance().FindH323RegAccount(strE164ID);
		else if(strH323ID.size() >0)
		{
			pH323RegUser=H323CallMgr::Instance().FindH323RegAccount(strH323ID);
			if(pH323RegUser) strE164ID=pH323RegUser->GetRegPhoneNum();
		}
		memset(buf,0,sizeof(buf));
		sprintf(path,"message.setup.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.setup.sourceAddress");

		GetAlias(nodeId,path,strTermE164ID,strTermH323ID,alias);
		if(pH323RegUser)
		{
			pH323MediaCall->m_hsCall = hsCall;

			ConfMediaSet h235Set;
			std::string strTerminalIP="";
			memset(&h235Set,0,sizeof(h235Set));
			if(strTermE164ID.size() >0)
			{
				memcpy(h235Set.m_abyTerminalE164,strTermE164ID.c_str(),strTermE164ID.size() );
				h235Set.m_abyTerminalE164[strTermE164ID.size()]=0;
				pH323MediaCall->m_strE164ID = h235Set.m_abyTerminalE164;
			}
			if(strTermH323ID.size() >0)
			{
				memcpy(h235Set.m_abyTerminalH323ID,strTermH323ID.c_str(),strTermH323ID.size() );
				h235Set.m_abyTerminalH323ID[strTermH323ID.size()]=0;
				pH323MediaCall->m_strH323ID = h235Set.m_abyTerminalH323ID;
			}

			if( pH323MediaCall->GetRemoteUserIP(strTerminalIP) )
				strcpy(h235Set.m_abyTerminalIP,strTerminalIP.c_str());

			GetQ931CallPartyNumber(hsCall,cmParamCalledPartyNumber,pH323MediaCall->m_strQ931CalledNumber);
			GetQ931CallPartyNumber(hsCall,cmParamCallingPartyNumber,pH323MediaCall->m_strQ931CallerNumber);
			if(pH323MediaCall->m_strQ931CalledNumber.size() >0) strE164ID =pH323MediaCall->m_strQ931CalledNumber;

			if(pH323MediaCall->m_strQ931CallerNumber.size() >0)
			{
				std::string strCallerNumber =pH323MediaCall->m_strQ931CallerNumber;
				memcpy(h235Set.m_abyTerminalE164,strCallerNumber.c_str(),strCallerNumber.size() );
				h235Set.m_abyTerminalE164[strCallerNumber.size()]=0;
			}

			H323CallMgr::Instance().OnDAP_CallH235CapSet(pH323MediaCall,strE164ID.c_str(),h235Set);
			//bandWidth 设为零时则认为该会议不存在
			if(pH323MediaCall->m_nBandWidth <= 0)
			{
				LogFile(NULL,NULL,"","","cmEvNewCall Account m_nBandWidth <= 0 111\n");
				pH323RegUser->AccountCallRelease(pH323MediaCall,cmReasonTypeNoBandwidth);
				LogFile(NULL,NULL,"","","cmEvNewCall Account m_nBandWidth <= 0 222\n");
				return -1;
			}
			else
			{
				*lphaCall = (HAPPCALL)pH323MediaCall;
				pH323MediaCall->SeRegAccountCallMark();
				pH323RegUser->SetAccountARQSrcInfo(pH323MediaCall,&alias,h235Set.m_nUseFWNAT);
				int nRet=pH323RegUser->SendRegARQMessage(pH323MediaCall);
				if(nRet<0)
				{
					LogFile(NULL,NULL,"","","cmEvNewCall SendRegARQMessage<0 111\n");
					//notice wait test
					LogFile(NULL,NULL,"","","H323Stack SendARQ CallFunction Error.");
					IMXEC_CALL_END_REASON discReason=ModeDisconnectedError;
					pH323MediaCall->DisconnectReason()=GetCallDisconnectReason(discReason);
					H323CallMgr::Instance().H235SecurityCallEnd(pH323MediaCall);
					H323CallMgr::Instance().OnDAP_CallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),discReason,pH323MediaCall->DisconnectReason());
					cmCallClose(hsCall);
					LogFile(NULL,NULL,"","","cmEvNewCall SendRegARQMessage<0 222\n");
					delete pH323MediaCall;
					pH323MediaCall=NULL;
					return -1;
				}
			}
			LogFile(NULL,NULL,"","","cmEvNewCall         111222");
			return 0;
		}
		else //notice
		{
			//not support ip call incoming
		}
	}
	else return 0;

#else

	//限制同时只能有一个call在呼叫
	if(CallMgr::Instance().IsCanCall() ==false)
	{
		LogFile(NULL,NULL,"","","cmEvNewCall IsCanCall() ==false 111\n");
		cmCallDropParam(hsCall,cmReasonTypeNoPermision);
		cmCallClose(hsCall);
		LogFile(NULL,NULL,"","","cmEvNewCall IsCanCall() ==false 222\n");
		if (pH323MediaCall)
		{
			delete pH323MediaCall;
			pH323MediaCall=NULL;
		}
		return -1;
	}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#else

	CallMgr::Instance().SetCanCallNot();

#endif

#endif

	if(pH323MediaCall ==NULL ) pH323MediaCall =  new H323MediaCall(H323CallMgr::Instance(), hsCall);
	if(pH323MediaCall)
	{
		pH323MediaCall->m_hsCall = hsCall;
		//pH323MediaCall->SetCallNum(1);

//support ip call incoming
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

		std::string strTerminalIP="";
		ConfMediaSet h235Set;
		memset(&h235Set,0,sizeof(h235Set));
		if(strTermE164ID.size() >0)
		{
			memcpy(h235Set.m_abyTerminalE164,strTermE164ID.c_str(),strTermE164ID.size() );
			h235Set.m_abyTerminalE164[strTermE164ID.size()]=0;
			pH323MediaCall->m_strE164ID = h235Set.m_abyTerminalE164;
		}
		if(strTermH323ID.size() >0)
		{
			memcpy(h235Set.m_abyTerminalH323ID,strTermH323ID.c_str(),strTermH323ID.size() );
			h235Set.m_abyTerminalH323ID[strTermH323ID.size()]=0;
			pH323MediaCall->m_strH323ID = h235Set.m_abyTerminalH323ID;
		}
		if( pH323MediaCall->GetRemoteUserIP(strTerminalIP) )
			strcpy(h235Set.m_abyTerminalIP,strTerminalIP.c_str());

		GetQ931CallPartyNumber(hsCall,cmParamCalledPartyNumber,pH323MediaCall->m_strQ931CalledNumber);
		GetQ931CallPartyNumber(hsCall,cmParamCallingPartyNumber,pH323MediaCall->m_strQ931CallerNumber);
		if(pH323MediaCall->m_strQ931CalledNumber.size() >0) strE164ID =pH323MediaCall->m_strQ931CalledNumber;

		if(pH323MediaCall->m_strQ931CallerNumber.size() >0)
		{
			std::string strCallerNumber =pH323MediaCall->m_strQ931CallerNumber;
			memcpy(h235Set.m_abyTerminalE164,strCallerNumber.c_str(),strCallerNumber.size() );
			h235Set.m_abyTerminalE164[strCallerNumber.size()]=0;
		}

		H323CallMgr::Instance().OnDAP_CallH235CapSet(pH323MediaCall,strE164ID.c_str(),h235Set);
		if(pH323MediaCall->m_nBandWidth <= 0)
		{
		LogFile(NULL,NULL,"","","cmEvNewCall m_nBandWidth <= 0 111\n");
		cmCallDropParam(hsCall,cmReasonTypeNoBandwidth);
		cmCallClose(hsCall);
		LogFile(NULL,NULL,"","","cmEvNewCall m_nBandWidth <= 0 222\n");
		delete pH323MediaCall;
		pH323MediaCall=NULL;
		return -1;
		}

		//mcu 级联测试
		if( H323CallMgr::Instance().m_nTerminalCapsType ==1 && nCallRate > 1000)
			pH323MediaCall->SetBitrateFromCaller(nCallRate /1000);

#else

		int nodeId=cmCallGetMessage(hsCall,cmQ931setup);
		if(nodeId >=0 )
		{
			cmAlias alias;
			char  buf[288]={0};
			alias.string = buf;
			char path[288]={0};
			sprintf(path,"message.setup.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.setup.destinationAddress");
			GetAlias(nodeId,path,strE164ID,strH323ID,alias);

			memset(buf,0,sizeof(buf));
			sprintf(path,"message.setup.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.setup.sourceAddress");
			GetAlias(nodeId,path,strTermE164ID,strTermH323ID,alias);
			if(strTermE164ID.size() >0)
			{
				pH323MediaCall->m_strE164ID = strTermE164ID;
			}
			if(strTermH323ID.size() >0)
			{
				pH323MediaCall->m_strH323ID = strTermH323ID;
			}
		}

		//notice the set is caller bandwidth ?
		if(nCallRate > 1000)
		{
			H323CallMgr::Instance().m_nMainVideoMaxBitrate=nCallRate /1000 ;
			H323CallMgr::Instance().m_nSecondVideoMaxBitrate=nCallRate /1000 ;
		}

#endif

		*lphaCall = (HAPPCALL)pH323MediaCall;
		H323CallMgr::Instance().AddCall(pH323MediaCall);

	}

	LogFile(NULL,NULL,"","","cmEvNewCall         222");
	return 0;
}

int RVCALLCONV cmEvCallStateChanged(HAPPCALL haCall, HCALL hsCall, cmCallState_e state, cmCallStateMode_e stateMode)
{
	H323MediaCall* pH323MediaCall=(H323MediaCall*)haCall;
	if(!pH323MediaCall) return -1;

	LogFile(NULL,NULL,"","","cmEvCallStateChanged         111");

	char * str=NULL;
	char temp[288]={0};
	int nodeId=-1;
	int value=-1;
	HPVT hVal1;
	int field;
	RvBool bIsString=false;
	int error=-1;

	char szH225Version[288]={0};
	std::string strDisconnectReason ="";
	IMXEC_CALL_END_REASON discReason =ModeDisconnectedUnknown;

	if(CallMgr::Instance().m_nUseNETECSend > 0 ) pH323MediaCall->GetCallUserInfomation(hsCall,state);

	switch(state)
	{
	case cmCallStateDialtone:
		break;
	case cmCallStateRingBack:				//recv Alerting. cmCallAccept()
		{
			pH323MediaCall->OnCallRingOut();
		}
		break;
	case cmCallStateProceeding:				//recv Proceeding. cmCallSendCallProceeding()
		break;
	case cmCallStateOffering:				//recv Setup. cmCallMake()
		{
			RvBool bProductId=false;
			RvBool bVersionId=false;
			cmGetH225RemoteVersion(hsCall,szH225Version);
			pH323MediaCall->m_nH225Version=atoi(szH225Version);
			//因适应RADVision ViaIp MCU 720P作出改变
			error = cmCallGetParam(hsCall, cmParamFullSourceInfo, 0, &nodeId, (char*)&str);		
			hVal1 = cmGetValTree(H323CallMgr::Instance().m_hApp);
			sprintf(temp, "vendor.productId");
			error=pvtGetByPath(hVal1, nodeId, temp, &field, &value, &bIsString);
			if( error && bIsString && value>0 )
			{
				char szProductID[288]={0};
				error= pvtGetString( hVal1, error, value, szProductID ) ;	
				if (error>=0)
				{
					pH323MediaCall->m_strProductID=szProductID;

					if((strstr(szProductID,"RADVision")) && (strstr(szProductID,"ViaIp MCU") ))
					{
						bProductId=true;
					}

				}
			}

			bIsString=false;			
			sprintf(temp, "vendor.versionId"); //versionId: 7.1.2.8.2
			error=pvtGetByPath(hVal1, nodeId, temp, &field, &value, &bIsString);
			if( error && bIsString && value>0 )
			{
				char szVersionID[288]={0};
				error= pvtGetString( hVal1, error, value, szVersionID ) ;	
				if (error>=0)
				{
					pH323MediaCall->m_strVersionID=szVersionID;
					if((strstr(szVersionID,"7.1.2.8.2")) )
					{
						bVersionId=true;
					}
				}
			}

			int nFindPro=-1;
			int nFindVer=-1;

			nFindPro=pH323MediaCall->m_strProductID.find("Kdv7610");
			nFindVer= pH323MediaCall->m_strVersionID.find("5");
			if ((nFindPro>=0) && (nFindVer>=0) ) ; 

			//productId: Tandberg  versionId: 515
			nFindPro=-1; nFindVer=-1;
			nFindPro=pH323MediaCall->m_strProductID.find("Tandberg");
			nFindVer= pH323MediaCall->m_strVersionID.find("515");
			//if ((nFindPro>=0) && (nFindVer>=0) ) pH323MediaCall->m_nH239SessionId=0;

			//modify by gsb 20140707
			int setupId=cmCallGetMessage(hsCall,cmQ931setup);
			if(setupId >=0) GetTerminalType(pH323MediaCall,hVal1,setupId,1);

			pH323MediaCall->OnCallRingIn();
		}
		break;
	case cmCallStateConnected:				//recv Connect. CmCallAnswer()
		{
			RvBool bProductId=false;
			RvBool bVersionId=false;
			cmGetH225RemoteVersion(hsCall,szH225Version);
			pH323MediaCall->m_nH225Version=atoi(szH225Version);

			//因适应RADVision ViaIp MCU 720P作出改变
			error = cmCallGetParam(hsCall, cmParamFullDestinationInfo, 0, &nodeId, (char*)&str);		
			hVal1 = cmGetValTree(H323CallMgr::Instance().m_hApp);
			sprintf(temp, "vendor.productId");
			error=pvtGetByPath(hVal1, nodeId, temp, &field, &value, &bIsString);
			if( error && bIsString && value>0 )
			{
				char szProductID[288]={0};
				error= pvtGetString( hVal1, error, value, szProductID) ;	
				if (error>=0)
				{
					if(pH323MediaCall->m_strProductID.size()==0)
						pH323MediaCall->m_strProductID=szProductID;

					if((strstr(szProductID,"RADVision")) && (strstr(szProductID,"ViaIp MCU") ))
					{
						bProductId=true;
					}

				}
			}

			bIsString=false;			
			sprintf(temp, "vendor.versionId"); //versionId: 7.1.2.8.2
			error=pvtGetByPath(hVal1, nodeId, temp, &field, &value, &bIsString);
			if( error && bIsString && value>0 )
			{
				char szVersionID[288]={0};
				error= pvtGetString( hVal1, error, value, szVersionID ) ;	
				if (error>=0)
				{
					if(pH323MediaCall->m_strVersionID.size()==0)
						pH323MediaCall->m_strVersionID=szVersionID;
					if((strstr(szVersionID,"7.1.2.8.2")) )
					{
						bVersionId=true;
					}
				}
			}			
			if(bProductId && bVersionId)
			{
				pH323MediaCall->m_nRemoteTerminalType=2;  
			}
			int nFindPro=-1;
			int nFindVer=-1;

			nFindPro=pH323MediaCall->m_strProductID.find("Kdv7610");
			nFindVer= pH323MediaCall->m_strVersionID.find("5");
			if ((nFindPro>=0) && (nFindVer>=0) ) ; 

			//productId: Tandberg  versionId: 515
			nFindPro=-1; nFindVer=-1;
			nFindPro=pH323MediaCall->m_strProductID.find("Tandberg");
			nFindVer= pH323MediaCall->m_strVersionID.find("515");
			//if ((nFindPro>=0) && (nFindVer>=0) ) pH323MediaCall->m_nH239SessionId=0; 

			//if ((pH323MediaCall->m_strProductID.find("Kdv7610")>=0) || // && pH323MediaCall->m_strVersionID.find("5")>=0) ||
			//	(pH323MediaCall->m_strProductID.find("R480")>=0))// && pH323MediaCall->m_strVersionID.find("5")>=0))
			//{
			//}

			//modify by gsb 20140707
			if(pH323MediaCall->m_nCallDir == IMXEC_CALL_DIR_OUT && stateMode == cmCallStateModeConnectedCallSetup)
			{
				int connectId=cmCallGetMessage(hsCall,cmQ931connect);
				if(connectId >=0) GetTerminalType(pH323MediaCall,hVal1,connectId,2);
			}

			switch( stateMode )
			{
			case cmCallStateModeConnectedCallSetup:
				H323CallMgr::Instance().m_h235Mgr.H235SecurityCallGenerateSharedSecret(pH323MediaCall);
				break;
			case cmCallStateModeConnectedCall:
				{
					pH323MediaCall->m_bCallState =true;

LogFileCall(pH323MediaCall,NULL, 0,"","","OnCallEstablished",1);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
H323CallMgr::Instance().OnDAP_CallEstablished(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),pH323MediaCall->m_strRemoteUserNonstandardInfo);
#else
H323CallMgr::Instance().OnCallEstablished(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName());
#endif

				   //打开音频发送信道
					pH323MediaCall->OpenAudioOutChannel();
					//打开视频发送信道
					pH323MediaCall->OpenVideoOutChannel();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
					pH323MediaCall->OpenFECCOutChannel();
					pH323MediaCall->OpenH239VideoOutChannel();
#endif

#else
//打开远遥发送信道
pH323MediaCall->OpenFECCOutChannel();
//打开双流视频发送信道
pH323MediaCall->OpenH239VideoOutChannel();
#endif

				}
				break;

			case cmCallStateModeConnectedConference:
				break;
			}
		}
		break;
	case cmCallStateDisconnected:
		switch(stateMode)
		{
		case cmCallStateModeDisconnectedBusy:
			discReason =ModeDisconnectedBusy;
			break;
		case cmCallStateModeDisconnectedNormal:
			discReason =ModeDisconnectedNormal;
			break;
		case cmCallStateModeDisconnectedReject:
			discReason =ModeDisconnectedReject;
			break;
		case cmCallStateModeDisconnectedLocal:
			discReason =ModeDisconnectedLocal;
			break;
		case cmCallStateModeDisconnectedUnreachable:
			discReason =ModeDisconnectedUnreachable;
			break;
		case cmCallStateModeDisconnectedUnknown:
			discReason =ModeDisconnectedUnknown;
			break;
		default:
			discReason =ModeDisconnectedUnknown;
			break;
		}
		if(discReason == ModeDisconnectedNormal || discReason == ModeDisconnectedUnknown)
		{
			cmCallDisconnectionReason nDiscR=cmCallDisconnectionReasonUnknown;
			int nRet=cmCallGetParam(hsCall,cmParamDisconnectionReason,0,(RvInt32 *)&nDiscR,NULL);
			if(nRet >=0 && nDiscR == cmCallDisconnectionReasonRemoteDropped )
				discReason =ModeDisconnectedRemote;
		}
		if( pH323MediaCall->m_nLocalHungUP ==1) discReason =ModeDisconnectedLocal;     //本端主动挂断
		if( pH323MediaCall->m_nLocalHungUP ==2) discReason =ModeDisconnectedNet;
		if( pH323MediaCall->m_nLocalHungUP ==3) discReason =ModeDisconnectedRemote;    //远端正常挂断(接收到releaseComplete)
		if(discReason == ModeDisconnectedNormal) discReason = ModeDisconnectedUnknown; //仍为Normal则置Unknown OR 业务层排除掉

		pH323MediaCall->DisconnectReason()=GetCallDisconnectReason(discReason);
		//挂断本端的所有发送信道
		break;

	case cmCallStateIdle:
		{
			H323CallMgr::Instance().H235SecurityCallEnd(pH323MediaCall);
			cmCallClose(hsCall);

			LogFileCall(pH323MediaCall,NULL, 0,"",pH323MediaCall->DisconnectReason().c_str(),"OnCallDestroyed",1);

			if(H323CallMgr::Instance().DisconnectCheckRoundTripDalay(pH323MediaCall) )
			{
				discReason =ModeDisconnectedNet;
				pH323MediaCall->DisconnectReason()=GetCallDisconnectReason(discReason);
			}
			//仍为Normal则置Unknown OR 业务层排除掉
			if(discReason == ModeDisconnectedNormal)
			{
				discReason = ModeDisconnectedUnknown;
				pH323MediaCall->DisconnectReason()=GetCallDisconnectReason(discReason);
			}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
H323CallMgr::Instance().OnDAP_CallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName(),discReason,pH323MediaCall->DisconnectReason());

#else

//OutputDebugString("cmCallStateIdle OnCallDestroyed\n");
H323CallMgr::Instance().OnCallDestroyed(pH323MediaCall->GetCallID(),pH323MediaCall->GetUserID(),pH323MediaCall->GetUserFullName());

#endif
		}
		break;
	};

	LogFile(NULL,NULL,"","","cmEvCallStateChanged         222");
	return 0;
}

void cmPfnApiEnterFunc(IN HAPP hApp, IN const char*line, ...)
{
}

void printLogInTestHandle(void* type, IN const char* line, ...)
{
	std::string msg="pvtTest";
	msg=msg+line;
	OutputDebugString(msg.c_str());
}

int RVCALLCONV cmEvCallControlStateChanged(HAPPCALL haCall,HCALL hsCall,cmControlState state,cmControlStateMode stateMode)
{
	HPVT valH =NULL;
	char tempbuf[100]={0};
	int error=0;
	int nodeId=-1;
	int param=0;

	H323MediaCall* pH323MediaCall = (H323MediaCall*)haCall;
	if(!pH323MediaCall)
		return -1;

	if(state==cmControlStateTransportConnected)
	{
		//H323CallMgr::Instance().OnDAP_CallCapAdd(pH323MediaCall->GetCallID() );
		nodeId=pH323MediaCall->BuildSendLocalCapabilitis();

		//std::string strContent="";
		//OutputPVTNodeContent(strContent,nodeId);
		int terminalType = H323CallMgr::Instance().GetMasterSlaveValue();
		if(pH323MediaCall->m_nMasterSlaveValue >0) terminalType = pH323MediaCall->m_nMasterSlaveValue;
		cmCallMasterSlaveDetermineExt(hsCall, terminalType, -1);
	}
	if(state==cmControlStateConnected)
	{
	}
	return 1;
}

int RVCALLCONV cmEvCallCapabilities(HAPPCALL haCall, HCALL hsCall, cmCapStruct*capabilities[])
{
	return 1;
}

int RVCALLCONV cmEvCallCapabilitiesExt(HAPPCALL haCall, HCALL hsCall, cmCapStruct***  capabilities[])
{
	H323MediaCall* pH323MediaCall = (H323MediaCall*)haCall;
	if(!pH323MediaCall)
		return -1;

	pH323MediaCall->OnRcvH245CapabilitiesTermUse( (void*)capabilities );

	return 0;
}

int RVCALLCONV cmEvCallCapabilitiesResponse(HAPPCALL haCall,HCALL hsCall,RvUint32 status)
{
	return 0;
}

int RVCALLCONV cmEvCallMasterSlave(HAPPCALL haCall,HCALL hsCall,RvUint32 terminalType,RvUint32 statusDeterminationNumber)
{
	return 0; 
}


//add adapter
int RVCALLCONV cmEvCallTerminalNumberAssign(HAPPCALL haCall,HCALL hsCall,RvUint8 mcuNumber, RvUint8 terminalNumber)
{
	H323MediaCall* pH323MediaCall = (H323MediaCall*)haCall;
	if(!pH323MediaCall)
		return -1;

	pH323MediaCall->RecvH245TerminalNumberAssign(mcuNumber, terminalNumber);
	return 0; 
}

int RVCALLCONV cmEvChannelNewRate(HAPPCHAN haChan, HCHAN hsChan, RvUint32 rate)
{
	H323Channel* pH323Channel = (H323Channel*)haChan;
	if(!pH323Channel)
		return -1;

	if(rate >0) rate = rate / 10;

	//notice FlowControlCommand
	SetChannelFlowBitrate(pH323Channel,rate);
	pH323Channel->m_rH323Call.RecvFlowControlCommand(pH323Channel,hsChan,rate);
	return 0; 
}

int RVCALLCONV cmEvCallUserInput(HAPPCALL haCall, HCALL hsCall, RvInt32 userInputId) /* userInputId User input node ID. */
{
	H323MediaCall* pH323MediaCall = (H323MediaCall*)haCall;
	if(!pH323MediaCall)
		return -1;
	//pH323MediaCall->RecvCallUserInput(hsCall,userInputId);
	return 0; 
}

int RVCALLCONV cmEvCallMasterSlaveStatus(HAPPCALL haCall,HCALL hsCall,RvUint32 status)
{
	return 0;
}

int RVCALLCONV cmEvCallControlMessage(HAPPCALL haCall, HCALL hsCall, HAPPCHAN haChan, HCHAN hsChan, RvPvtNodeId message, cmH245MessageType messageType)
{
	H323MediaCall * pH323MediaCall=(H323MediaCall*)haCall;
	H323Channel *pH323Channel = (H323Channel *)haChan;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if(pH323MediaCall ==NULL || pH323MediaCall ->m_hsCall ==NULL || pH323MediaCall ->m_hsCall != hsCall) return 0;
#endif

	int field;
	unsigned int bIsString;
	int i=0;
	int n1=0,n2=0,n3=0;
	int tag1;
	HPVT hVal1;
	char temp[288];
	int subMessageIdentifier = 0;

	if(pH323MediaCall)
	{
		pH323MediaCall->RecvH245ControlMessage(message,messageType);
	}

	switch(messageType)
	{
	case h245grqGenericRequest:
		{
			int terLabel = 0xff0000;
			int chID = 0xff0000;
			int symBreaking = 0xff0000;

			hVal1 = cmGetValTree(H323CallMgr::Instance().m_hApp);
			tag1 = pvtGetNodeIdByPath(hVal1,message, "");

			sprintf(temp, "subMessageIdentifier");
			if(pvtGetByPath(hVal1, message, temp, &field, &subMessageIdentifier, &bIsString) < 0)
			{
				break;
			}
			if(subMessageIdentifier != 3)
			{
				break;
			}

			//获取terminalLabel
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 1);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}

			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 1);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1==44)
			{
				terLabel = n2;
			}

			//获取channelID
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 2);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 2);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1 == 42)
			{
				chID = n2;
			}

			//获取symmetryBreaking
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 3);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 3);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1 == 43)
			{
				symBreaking = n2;
			}

			if(terLabel != 0xff0000 && chID != 0xff0000) //有些设备可能terLabel值为0，或chID值为0
			{
				if(pH323MediaCall)
				{
					pH323MediaCall->OnRecvGenericRequest(pH323Channel, terLabel, chID, symBreaking);
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
//					H323CallMgr::Instance().OnRecvGenericRequest(pH323MediaCall,terLabel,chID,symBreaking);
//#endif
//
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//					pH323MediaCall->RecvGenericMessage(messageType,0,terLabel,chID,symBreaking);
//#endif

}
			}
		}
		break;

	case h245grsGenericResponse:
		{
			int response = 0;
			int terLabel = 0xff0000;
			int chID = 0xff0000;
			int respValue = 0;

			hVal1 = cmGetValTree(H323CallMgr::Instance().m_hApp);
			tag1 = pvtGetNodeIdByPath(hVal1,message, "");

			//获取应答，response=126 为ACK，response=127为Reject
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 1);
			if(pvtGetByPath(hVal1, message, temp, &field, &response, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.logical", 1);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
				respValue=n2;
			}

			//获取terminalLabel
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 2);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 2);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1==44)
			{
				terLabel = n2;
			}

			//获取channelID
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 3);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 3);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1 == 42)
			{
				chID = n2;
			}

			if(terLabel != 0xff0000 && chID != 0xff0000)
			{
				if(pH323MediaCall)
				{
					pH323MediaCall->OnRecvGenericResponse(pH323Channel, response, terLabel, chID,respValue);

//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
//					H323CallMgr::Instance().OnRecvGenericResponse(pH323MediaCall,response,terLabel,chID);
//#endif
//
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//					pH323MediaCall->RecvGenericMessage(messageType,0,terLabel,chID,0,response,respValue);
//#endif
				}
			}
		}
		break;

	case h245gcGenericCommand:
		{
			int terLabel = 0xff0000;
			int chID = 0xff0000;

			hVal1 = cmGetValTree(H323CallMgr::Instance().m_hApp);
			tag1 = pvtGetNodeIdByPath(hVal1,message, "");

			//获取terminalLabel
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 1);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 1);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1==44)
			{
				terLabel = n2;
			}

			//获取channelID
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 2);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 2);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1 == 42)
			{
				chID = n2;
			}

			if(terLabel != 0xff0000 && chID != 0xff0000)
			{
				if(pH323MediaCall)
				{
					pH323MediaCall->OnRecvGenericCommand(pH323Channel, terLabel, chID);
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
//
//					H323CallMgr::Instance().OnRecvGenericCommand(pH323MediaCall,terLabel,chID);
//#endif
//
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//					pH323MediaCall->RecvGenericMessage(messageType,0,terLabel,chID);
//#endif

				}
			}
		}
		break;

	case h245giGenericIndication:
		{
			int terLabel = 0xff0000;
			int chID = 0xff0000;

			hVal1 = cmGetValTree(H323CallMgr::Instance().m_hApp);
			tag1 = pvtGetNodeIdByPath(hVal1,message, "");

			//获取terminalLabel
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 1);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 1);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1==44)
			{
				terLabel = n2;
			}

			//获取channelID
			n1 = n2 = 0;
			sprintf(temp, "messageContent.%d.parameterIdentifier.standard", 2);
			if(pvtGetByPath(hVal1, message, temp, &field, &n1, &bIsString) >=0)
			{
			}
			sprintf(temp, "messageContent.%d.parameterValue.unsignedMin", 2);
			if(pvtGetByPath(hVal1, message, temp, &field, &n2, &bIsString) >=0)
			{
			}
			if(n1 == 42)
			{
				chID = n2;
			}

			if(terLabel != 0xff0000 && chID != 0xff0000)
			{
				if(pH323MediaCall)
				{
					pH323MediaCall->OnRecvGenericIndication(pH323Channel, terLabel, chID);
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
//
//					H323CallMgr::Instance().OnRecvGenericIndication(pH323MediaCall,terLabel,chID);
//
//#endif
//
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//					pH323MediaCall->RecvGenericMessage(messageType,0,terLabel,chID);
//#endif

				}
			}
		}
		break;

	case h245mcEncryptionUpdateCommand:
		if(pH323MediaCall)
		{
			pH323MediaCall->OnRecvEncryptionUpdateCommand(pH323Channel,hsChan,message);
		}
		break;
	case h245mcEncryptionUpdateRequest:
		if(pH323MediaCall)
		{
			pH323MediaCall->OnRecvEncryptionUpdateRequest(pH323Channel,hsChan,message);
		}
		break;
	case h245mcEncryptionUpdateAck:
		if(pH323MediaCall)
		{
			pH323MediaCall->OnRecvEncryptionUpdateAck(pH323Channel,hsChan,message);
		}
		break;
	default:
		break;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
int RVCALLCONV cmEvCallNewChannel(HAPPCALL haCall, HCALL hsCall, HCHAN  hsChan, LPHAPPCHAN lphaChan)
{
	H323MediaCall* pH323MediaCall = (H323MediaCall*)haCall;

	if(pH323MediaCall)
	{
		pH323MediaCall->OnRcvH245OpenChannel( (HANDLE)hsChan, (HANDLE*)lphaChan );
	}
	return 0;
}

//cmEvChannelParameters()在cmEvChannelHandle()之前被调用
int RVCALLCONV cmEvChannelParameters(HAPPCHAN haChan, HCHAN hsChan, char* channelName,HAPPCHAN haChanSameSession, HCHAN hsChanSameSession, HAPPCHAN haChanAssociated, HCHAN hsChanAssociated, RvUint32 rate)
{
	unsigned int origin;
	int status;
	status = cmChannelGetOrigin(hsChan, &origin);//origin: 0 接收; 1发送
	if(origin == 1)//是发送信道
		return -1;

	H323Channel* pH323Channel = (H323Channel*)haChan;
	if (pH323Channel)
	{
		if( rate>0 )
		{
			pH323Channel->MaxBitrate()= rate/10;
		}

		if (channelName)
		{
			pH323Channel->ChannelName()=channelName;
		}
	}
	return 0;
}

//h235Media Get dataType and Get data nodeId	
void GetH235MediaTypeNodeId(HPVT pvtTree,int & dataTypeHandle ,cmCapDataType & dataType,H323Channel* pH323Channel)
{
	int nRet=-1;
	if( dataType!=cmCapH235) return;

	EncDecAlgorithmID algId;
	//get path value
	int  lenData=-1;
	RvBool  bIsString=RV_FALSE;
	unsigned char strData[256]={0};
	bool bGetOLCH235Cap=false;
	nRet=pvtGetByPath(pvtTree,dataTypeHandle,"encryptionAuthenticationAndIntegrity.encryptionCapability.1.algorithm",NULL,&lenData, &bIsString );
	if( nRet && bIsString && lenData>0 )
	{
		if( pvtGetString( pvtTree, nRet, lenData, (char*)strData ) > 0)
		{

			H323CallMgr::Instance().GetEncDecAlgorithmTypeFromOID(algId,strData,lenData);
		}
	}

	nRet=pvtGetNodeIdByPath(pvtTree,dataTypeHandle,"mediaType.videoData");
	if(nRet>=0) 
	{
		dataType=cmCapVideo;
		dataTypeHandle=pvtChild(pvtTree,nRet);
		bGetOLCH235Cap=true;
		goto next;
	}
	nRet=pvtGetNodeIdByPath(pvtTree,dataTypeHandle,"mediaType.audioData");
	if(nRet>=0) 
	{
		dataType=cmCapAudio;
		dataTypeHandle=pvtChild(pvtTree,nRet);
		bGetOLCH235Cap=true;
		goto next;
	}
	nRet=pvtGetNodeIdByPath(pvtTree,dataTypeHandle,"mediaType.data");
	if(nRet>=0) 
	{
		dataType=cmCapData;
		dataTypeHandle=pvtChild(pvtTree,pvtChild(pvtTree,nRet) );
		bGetOLCH235Cap=true;
		goto next;
	}

next:
	{
		char channelName[128]={0};
		int nameSize=sizeof(channelName);
		int dataTypeId=dataTypeHandle;
		strcpy(channelName, "null");
		HPST synConf = pvtGetSynTree(pvtTree, dataTypeId);
		int dataId = dataTypeId;//pvtChild(pvtTree, dataTypeId);
		RvPstFieldId fieldId=-1;
		pvtGet(pvtTree, dataId, &fieldId, NULL, NULL, NULL);
		pstGetFieldName(synConf, fieldId, nameSize, channelName);
		pH323Channel->ChannelName()=channelName;
		pH323Channel->AlgorithmId()=algId;
		pH323Channel->m_bH235OLCCap =bGetOLCH235Cap;

		return;
	}
}

int RVCALLCONV cmEvChannelSendMessage(
									  IN      HAPPCHAN            haChan,
									  IN      HCHAN               hsChan,
									  IN      RvPvtNodeId         message)
{
	H323Channel * pChannel = NULL;
	pChannel=(H323Channel *)haChan;	
	RvBool        bInfo = RV_FALSE;
	RV_UNUSED_ARG(hsChan);
	RV_UNUSED_ARG(bInfo);

	if(pChannel!=NULL)
	{
		H323Call *   pH323MediaCall = &(pChannel->m_rH323Call);
		//0515 add mediaPacketization
		char * msgName = cmGetProtocolMessageName(H323CallMgr::Instance().m_hApp, message);
		if ( strcmp(msgName, "openLogicalChannel") == 0 )
		{
			pH323MediaCall->AddMediaPacketization(pChannel,message);
		}

		int msdStatus=0;
		//LockCall(call);
		msdStatus = cmCallMasterSlaveStatus(pH323MediaCall->m_hsCall); 
		if (msdStatus == 2)
		{
			if ( pChannel->m_bH235Cap && ( strcmp(msgName, "openLogicalChannel") == 0 ) )
			{
				int nRet=H323CallMgr::Instance().m_h235Mgr.H235SecurityChannelAddEncryptionSync(pH323MediaCall,pChannel,message);
				if(nRet >= 0 ) pChannel->EncryptionSecretStatus() =true;
			}
			else if (pChannel->m_bH235OLCCap && ( strcmp(msgName, "openLogicalChannelAck") == 0 ) )
			{
				int nRet=H323CallMgr::Instance().m_h235Mgr.H235SecurityChannelAddEncryptionSync(pH323MediaCall,pChannel,message);
				if(nRet >= 0 ) pChannel->EncryptionSecretStatus() =true;
			}
		}
		//UnlockCall(call);
	}


	return 0;
}

int RVCALLCONV cmEvChannelRecvMessage(   HAPPCHAN    haChan,HCHAN     hsChan,int   nodeId)
{
	H323Channel* pH323Channel = (H323Channel*)haChan;
	//RvUint8 temp[288];
	if(pH323Channel!=NULL)
	{
		H323Call *   pH323MediaCall = &(pH323Channel->m_rH323Call);
		int          msdStatus=0;
		//LockCall(call);
		msdStatus = cmCallMasterSlaveStatus(pH323MediaCall->m_hsCall);
		char * msgName = cmGetProtocolMessageName(H323CallMgr::Instance().m_hApp, nodeId);

		if (msdStatus != 2 )
		{
			if ( (strcmp(msgName, "openLogicalChannel") == 0 ) || (strcmp(msgName, "openLogicalChannelAck") == 0 ) )
			{
				int nRet=H323CallMgr::Instance().m_h235Mgr.H235SecurityChannelGetEncryptionSync(pH323MediaCall, pH323Channel,nodeId);
				if(nRet >= 0 ) pH323Channel->EncryptionSecretStatus() = true;
			}
		}

		//UnlockCall(call);

		if(strcmp(msgName, "openLogicalChannel") == 0)
		{
			HPVT pvtTree = cmGetValTree(H323CallMgr::Instance().m_hApp);
			char pathBuf[288]={"request.openLogicalChannel.forwardLogicalChannelParameters.multiplexParameters.h2250LogicalChannelParameters.mediaPacketization.rtpPayloadType.payloadDescriptor.rfc-number"};
			int nRFCNum=0;
			int nRet=pvtGetByPath(pvtTree,nodeId,pathBuf,NULL, &nRFCNum, NULL);
			if(nRet>=0)
			{
				pH323Channel->m_nRFCNumber=nRFCNum;
			}
		}
		if(strcmp(msgName, "closeLogicalChannel") == 0)
		{
#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 2) )

#else

			pH323MediaCall->OnRcvChannelClose(pH323Channel);

#endif

		}
	}

	if(pH323Channel)
	{
		pH323Channel->m_rH323Call.RecvChannelMessage(pH323Channel,nodeId);
	}

	if (H323CallMgr::Instance().m_hFwNat )
	{
		RvBool bInfo = FALSE;
		RvH323FwNatH46019traversalParams   traversalParams;
		cmTransportAddress      rtpMuxAddr, rctpMuxAddr, keepAliveAddr;
		memset(&traversalParams, 0, sizeof(traversalParams));
		memset(&rtpMuxAddr, 0, sizeof(rtpMuxAddr));
		memset(&rctpMuxAddr, 0, sizeof(rctpMuxAddr));
		memset(&keepAliveAddr, 0, sizeof(keepAliveAddr));
		traversalParams.rtpMuxAddr = &rtpMuxAddr;
		traversalParams.rctpMuxAddr = &rctpMuxAddr;
		traversalParams.keepAliveAddr = &keepAliveAddr;
		//RvH323FwNatChannelRecvMessage( H323CallMgr::Instance().m_hFwNat, hsChan, nodeId, FALSE, dirBoth, &traversalParams,&bInfo ); 
		//if(pH323Channel && bInfo == RV_TRUE)
		//{
		//	pH323Channel->SetKeepAliveAddress( traversalParams.keepAliveAddr->ip, traversalParams.keepAliveAddr->port);
		//}
		cmChannelDirection nDir =(cmChannelDirection) (pH323Channel->Direction() -1) ;
		RvH323FwNatChannelRecvMessage( H323CallMgr::Instance().m_hFwNat, hsChan, nodeId, FALSE,nDir, &traversalParams,&bInfo ); 

		unsigned int ip=traversalParams.keepAliveAddr->ip;
		unsigned short port=traversalParams.keepAliveAddr->port;
		if(pH323Channel && bInfo == RV_TRUE && ip>0 && port >0)
		{
			pH323Channel->SetKeepAliveAddress( ip,port);
			char msg[288]={0};
			sprintf(msg,"Channel SetKeepAliveAddress,ChannelDirection :%d,ip:%d port:%d\n",nDir,ip,port);
			OutputDebugString(msg);
		}
	}

	return 0;
}

//cmEvChannelHandle()在cmEvChannelParameters()之后被调用
int RVCALLCONV cmEvChannelHandle(HAPPCHAN  haChan, HCHAN  hsChan, RvInt dataTypeHandle, cmCapDataType dataType)
{
	unsigned int origin;
	int status;
	status = cmChannelGetOrigin(hsChan, &origin);//origin: 0 接收; 1发送
	if(origin == 1)//是发送信道
		return -1;

	//code add  Use:Get dynamicRTPPayloadType
	int nDynamicRTPPayload=96;
	cmChannelGetParam(hsChan,cmChannelParamH225DynamicRTPPayloadType,&nDynamicRTPPayload,NULL);

	H323Channel* pH323Channel = (H323Channel*)haChan;
	if (pH323Channel)
	{
		RvPstFieldId	nField;
		RvInt32			nValue;
		RvBool			bString;
		HPVT pvtTree = cmGetValTree(H323CallMgr::Instance().m_hApp);

		int nCapPrint=H323CallMgr::Instance().GetDAPCapPrintEnable();
		if(nCapPrint>0)
		{
			std::string strContent="";
			OutputPVTNodeContent(strContent,dataTypeHandle);
			H323CallMgr::Instance().OutPutCapContent(pH323Channel->m_rH323Call.GetCallID().c_str(),2,strContent.c_str());
		}

		GetH235MediaTypeNodeId(pvtTree,dataTypeHandle,dataType,pH323Channel);

		switch(dataType)
		{
		case cmCapAudio:
			pH323Channel->m_nMediaType=Enum_Channel_Media_Audio;

			if(!strcmp(pH323Channel->ChannelName().c_str(), "g711Ulaw64k"))
			{
				pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
				pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G711U;
				pH323Channel->PayloadType() = 0;
			}
			else if(!strcmp(pH323Channel->ChannelName().c_str(), "g711Alaw64k"))
			{
				pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
				pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G711A;
				pH323Channel->PayloadType() = 8;
			}
			else if(!strcmp(pH323Channel->ChannelName().c_str(), "g722-64k"))
			{
				pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
				pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G722_64;
				pH323Channel->PayloadType() = 9;
			}
			else if (!strcmp(pH323Channel->ChannelName().c_str(), "genericAudioCapability"))
			{
				int  lenData;
				RvBool  bIsString;
				int nodeData = pvtGetByPath( pvtTree, dataTypeHandle, "capabilityIdentifier.standard", NULL, &lenData, &bIsString );
				if( nodeData && bIsString && lenData>0 )
				{
					unsigned char strData[256];
					if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
					{
						//0.0.7.7221.1.0
						//0x00 0x07 0xb8 0x35 0x01 0x00
						if( lenData == 6 &&
							strData[0] == 0x00 &&
							strData[1] == 0x07 &&
							strData[2] == 0xb8 &&
							strData[3] == 0x35 &&
							strData[4] == 0x01 &&
							strData[5] == 0x00 )
						{
							int nValue=0;
							int nRet=0;
							nRet=pvtGetByPath(pvtTree,dataTypeHandle,"maxBitRate",&nField,&nValue,&bString);
							if(nRet>=0 && nValue==24000)
							{
								pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
								pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G7221_7_24;
								pH323Channel->PayloadType() = 121;
							}
							if(nRet>=0 && nValue==32000)
							{
								pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
								pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G7221_7_32;
								pH323Channel->PayloadType() = 122;
							}

						}
						//0.0.7.7221.1.1.0
						//0x00 0x07 0xb8 0x35 0x01 0x01 0x00
						if( lenData == 7 &&
							strData[0] == 0x00 &&
							strData[1] == 0x07 &&
							strData[2] == 0xb8 &&
							strData[3] == 0x35 &&
							strData[4] == 0x01 &&
							strData[5] == 0x01 &&
							strData[6] == 0x00 )
						{
							int nValue=0;
							int nRet=0;
							nRet=pvtGetByPath(pvtTree,dataTypeHandle,"maxBitRate",&nField,&nValue,&bString);
							if(nRet>=0 && nValue==240)
							{
								pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
								pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G7221_14_24;
								pH323Channel->PayloadType() = 123;
							}
							if(nRet>=0 && nValue==320)
							{
								pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
								pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G7221_14_32;
								pH323Channel->PayloadType() = 124;
							}
							if(nRet>=0 && nValue==480)
							{
								pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
								pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G7221_14_48;
								pH323Channel->PayloadType() = 125;
							}
						}
						//00 07 85 4F 01 00						
						//0x00 0x07 0x85 0x4F 0x01 0x00
						if( lenData == 6 &&
							strData[0] == 0x00 &&
							strData[1] == 0x07 &&
							strData[2] == 0x85 &&
							strData[3] == 0x4F &&
							strData[4] == 0x01 &&
							strData[5] == 0x00 )
						{
							int nValue=0;
							int nRet=0;
							nRet=pvtGetByPath(pvtTree,dataTypeHandle,"maxBitRate",&nField,&nValue,&bString);
							if(nRet>=0 && nValue==320)
							{
								pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
								pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G719_32;
								pH323Channel->PayloadType() = 126;
							}
							if(nRet>=0 && nValue==480)
							{
								pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
								pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G719_48;
								pH323Channel->PayloadType() = 127;
							}
							if(nRet>=0 && nValue==640)
							{
								pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
								pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G719_64;
								pH323Channel->PayloadType() = 120;
							}
						}


					}
				}									
			}
			else if(!strcmp(pH323Channel->ChannelName().c_str(), "g729AnnexA"))
			{
				pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
				pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G729;
				pH323Channel->PayloadType() = 18;
			}
			//code add use Kd MCU
			else if(!strcmp(pH323Channel->ChannelName().c_str(), "g729"))
			{
				pH323Channel->m_rH323Call.m_pAudioRecvChannel = pH323Channel;
				pH323Channel->AudioCodecID() = X_AUDIO_CODEC_G729;
				pH323Channel->PayloadType() = 18;
			}

			//add
			pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nAudioUpdatePT);
			break;

		case cmCapVideo:
			{
				pH323Channel->m_nMediaType=Enum_Channel_Media_Video;

				int nRet=0;
				if (!strcmp(pH323Channel->ChannelName().c_str(), "h261VideoCapability"))
				{
					pH323Channel->m_rH323Call.m_pMainVideoRecvChannel = pH323Channel;
					pH323Channel->VideoCodecType() = VIDEC_CODEC_H261;
					pH323Channel->PayloadType() = 31;
					pH323Channel->VideoFormatType()=VIDEO_FORMAT_CIF;
					//add
					pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nMainVideoUpdatePT);
				}
				else if (!strcmp(pH323Channel->ChannelName().c_str(), "h263VideoCapability"))
				{
					pH323Channel->m_rH323Call.m_pMainVideoRecvChannel = pH323Channel;
					int valueMaxBitRate=0;
					int nMaxBitrate = 2048;
					int nRet=pvtGetByPath(pvtTree, dataTypeHandle, "maxBitRate", NULL, &valueMaxBitRate, NULL);
					if(nRet >=0 && valueMaxBitRate >0)
					{
						nMaxBitrate = valueMaxBitRate/10 ;
						pH323Channel->MaxBitrate() = nMaxBitrate; //??
					}

					nRet=pvtGetByPath(pvtTree,dataTypeHandle,"h263Options.h263Version3Options.dataPartitionedSlices", NULL, &nValue, NULL);
					if(nRet>=0)//对方具有H.263++能力
					{
						pH323Channel->VideoCodecType() = VIDEC_CODEC_H263PP;
						pH323Channel->PayloadType() = nDynamicRTPPayload;
						pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
					}
					else
					{
						nRet=pvtGetByPath(pvtTree,dataTypeHandle,"h263Options.advancedIntraCodingMode", NULL, &nValue, NULL);
						if(nRet>=0)//对方具有H.263+能力
						{
							pH323Channel->VideoCodecType() = VIDEC_CODEC_H263P;
							pH323Channel->PayloadType() = nDynamicRTPPayload;
							pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
						}
						else
						{
							pH323Channel->VideoCodecType() = VIDEC_CODEC_H263;
							pH323Channel->PayloadType() = 34;
							pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
						}
					}

					if(pH323Channel->VideoCodecType() == VIDEC_CODEC_H263 && pH323Channel->m_nRFCNumber ==2429)
					{
						pH323Channel->VideoCodecType() = VIDEC_CODEC_H263P;
						pH323Channel->PayloadType() = nDynamicRTPPayload;
						pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
					}
					float nFrame= VideoFrameBase ;
					pH323Channel->VideoFrameRate()=nFrame;
					//add
					pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nMainVideoUpdatePT);

				}
				else if (!strcmp(pH323Channel->ChannelName().c_str(), "genericVideoCapability"))
				{
					int  lenData;
					RvBool  bIsString;

					int valueMaxBitRate=0;
					int nMaxBitrate = 2048;
					int nRet=pvtGetByPath(pvtTree, dataTypeHandle, "maxBitRate", NULL, &valueMaxBitRate, NULL);
					if(nRet >=0 && valueMaxBitRate >0)
					{
						nMaxBitrate = valueMaxBitRate/10 ;
						pH323Channel->MaxBitrate() = nMaxBitrate; //??
					}

					int nodeData = pvtGetByPath( pvtTree, dataTypeHandle, "capabilityIdentifier.standard", NULL, &lenData, &bIsString );
					if( nodeData && bIsString && lenData>0 )
					{
						unsigned char strData[256];
						if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
						{
							//0.0.8.241.0.0.1
							//0x00 0x08 0x81 0x71 0x00 0x00 0x01
							if( lenData == 7 &&
								strData[0] == 0x00 &&
								strData[1] == 0x08 &&
								strData[2] == 0x81 &&
								strData[3] == 0x71 &&
								strData[4] == 0x00 &&
								strData[5] == 0x00 &&
								strData[6] == 0x01 )
							{
								int nProfile=0;
								int nLevel=0;
								int nMaxFS=0;
								int nMBPS=0;
								int nDual =0;
								float nFrame= VideoFrameBase ;

								VIDEO_FORMAT_TYPE nFormatType =VIDEO_FORMAT_QCIF;
								nRet=pH323Channel->m_rH323Call.GetH264ProfileLevel(pvtTree,dataTypeHandle,"" ,nProfile,nLevel);
								if(nRet>=0)
								{
									pH323Channel->m_rH323Call.GetH264CapOptionParam(pvtTree,dataTypeHandle,nMaxFS,nMBPS,"",nDual);
									nFormatType=pH323Channel->m_rH323Call.GetCallFormatTypeFromH264Param(nProfile,nLevel,nMaxFS,nMBPS,nMaxBitrate);
									nFrame=pH323Channel->m_rH323Call.GetFrameRateFromH264Param(nMaxFS,nMBPS);
								}
								else nProfile=64;

								//error=pH323Channel->m_rH323Call.GetH264ProfileLevel(pvtTree,dataTypeHandle,"",nProfile,nLevel);
								//error=pvtGetByPath(pvtTree,dataTypeHandle,"collapsing.1.parameterValue.booleanArray", NULL, &nProfile, NULL);

								if(nProfile==64) pH323Channel->VideoCodecType() = VIDEC_CODEC_H264;
								if(nProfile==8) pH323Channel->VideoCodecType() = VIDEC_CODEC_H264_SVC;
								pH323Channel->H264Level() = nLevel;
								pH323Channel->m_rH323Call.m_pMainVideoRecvChannel = pH323Channel;
								pH323Channel->PayloadType() = nDynamicRTPPayload;
								pH323Channel->VideoFrameRate()=nFrame;
								pH323Channel->VideoFormatType()=nFormatType; //pH323Channel->m_rH323Call.GetFormatTypeFromLevel(nLevel);
							}
						}
					}
					//add
					pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nMainVideoUpdatePT);

				}

				//use to call kedacom
				else if (!strcmp(pH323Channel->ChannelName().c_str(), "nonStandard"))
				{
					int  lenData;
					RvBool  bIsString;
					int nodeData = pvtGetByPath( pvtTree, dataTypeHandle, "data", NULL, &lenData, &bIsString );
					if( nodeData && bIsString && lenData>0 )
					{
						unsigned char strData[256]={0};
						if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
						{
							//....kedacom_js_China............
							char * pName="kedacom_js_China";
							char * pName1=(char *) strData+4;
							if( lenData == 32 && strncmp(pName1,pName,strlen(pName)) ==0 )
							{
								if(nDynamicRTPPayload <=0) nDynamicRTPPayload =97; //kedacom not set pt ,the set 97
								pH323Channel->m_rH323Call.m_pMainVideoRecvChannel = pH323Channel;
								pH323Channel->VideoCodecType() = VIDEC_CODEC_MPEG4;
								pH323Channel->PayloadType() = nDynamicRTPPayload;
								pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
								pH323Channel->MaxBitrate() = 768;
								float nFrame= VideoFrameBase ;
								pH323Channel->VideoFrameRate() =nFrame;
								pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nMainVideoUpdatePT);
							}
						}
					}
				}

				//双流
				if (!strcmp(pH323Channel->ChannelName().c_str(), "extendedVideoCapability"))
				{
					pH323Channel->m_nMediaType=Enum_Channel_Media_DualVideo;

					INT32 valueMaxBitRate=0;
					int error = 0;

					error = pvtGetByPath(pvtTree, dataTypeHandle, "videoCapability.1.h261VideoCapability.maxBitRate", NULL, &valueMaxBitRate, NULL);
					if(error >= 0)
					{
						pH323Channel->VideoCodecType() = VIDEC_CODEC_H261;
						pH323Channel->PayloadType() = 31;
						pH323Channel->MaxBitrate()=valueMaxBitRate/10;
						pH323Channel->m_rH323Call.m_pSecondVideoRecvChannel = pH323Channel;
						pH323Channel->VideoFormatType()=VIDEO_FORMAT_CIF;
						//add
						pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nDualVideoUpdatePT);
					}

					error = pvtGetByPath(pvtTree, dataTypeHandle, "videoCapability.1.h263VideoCapability.maxBitRate", NULL, &valueMaxBitRate, NULL);
					if(error >= 0)
					{
						pH323Channel->MaxBitrate()=valueMaxBitRate/10;
						pH323Channel->m_rH323Call.m_pSecondVideoRecvChannel = pH323Channel;
						nRet=pvtGetByPath(pvtTree,dataTypeHandle,"videoCapability.1.h263VideoCapability.h263Options.h263Version3Options.dataPartitionedSlices", NULL, &nValue, NULL);
						if(nRet>=0)//对方具有H.263++双流能力
						{
							//pH323Channel->SetChannelName("h263v3Extend");//更换名称，便于与channels中的通道配置名称一致
							pH323Channel->VideoCodecType() = VIDEC_CODEC_H263PP;
							pH323Channel->PayloadType() = nDynamicRTPPayload;
							pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
						}
						else
						{
							nRet=pvtGetByPath(pvtTree,dataTypeHandle,"videoCapability.1.h263VideoCapability.h263Options.advancedIntraCodingMode", NULL, &nValue, NULL);
							if(nRet>=0)//对方具有H.263+双流能力
							{
								//pH323Channel->SetChannelName("h263v2Extend");//更换名称，便于与channels中的通道配置名称一致
								pH323Channel->VideoCodecType() = VIDEC_CODEC_H263P;
								pH323Channel->PayloadType() = nDynamicRTPPayload;
								pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
							}
							else
							{
								//pH323Channel->SetChannelName("h263Extend");//更换名称，便于与channels中的通道配置名称一致
								pH323Channel->VideoCodecType() = VIDEC_CODEC_H263;
								pH323Channel->PayloadType() = 34;
								pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
							}
						}

						if(pH323Channel->VideoCodecType() == VIDEC_CODEC_H263 && pH323Channel->m_nRFCNumber ==2429)
						{
							pH323Channel->VideoCodecType() = VIDEC_CODEC_H263P;
							pH323Channel->PayloadType() = nDynamicRTPPayload;
							pH323Channel->VideoFormatType()=VIDEO_FORMAT_4CIF;
						}
						//add
						pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nDualVideoUpdatePT);

					}

					error = pvtGetByPath(pvtTree, dataTypeHandle, "videoCapability.1.genericVideoCapability.maxBitRate", NULL, &valueMaxBitRate, NULL);
					if(error >= 0)
					{
						int  lenData;
						RvBool  bIsString;

						int nodeData = pvtGetByPath( pvtTree, dataTypeHandle, "videoCapability.1.genericVideoCapability.capabilityIdentifier.standard", NULL, &lenData, &bIsString );
						if( nodeData && bIsString && lenData>0 )
						{
							unsigned char strData[256];
							if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
							{
								//0.0.8.241.0.0.1
								//0x00 0x08 0x81 0x71 0x00 0x00 0x01
								if( lenData == 7 &&
									strData[0] == 0x00 &&
									strData[1] == 0x08 &&
									strData[2] == 0x81 &&
									strData[3] == 0x71 &&
									strData[4] == 0x00 &&
									strData[5] == 0x00 &&
									strData[6] == 0x01 )
								{
									//pH323Channel->SetChannelName("h264Extend");//更换名称，便于与channels中的通道配置名称一致
									int nProfile=0;
									int nLevel=0;
									int nMaxFS=0;
									int nMBPS=0;
									int nDual=1;
									int nMaxBitrate = valueMaxBitRate/10 ;
									if(nMaxBitrate <=0) nMaxBitrate =2048;
									float nFrame= VideoFrameBase ;
									VIDEO_FORMAT_TYPE nFormatType =VIDEO_FORMAT_QCIF;
									nRet=pH323Channel->m_rH323Call.GetH264ProfileLevel(pvtTree,dataTypeHandle,"videoCapability.1.genericVideoCapability." ,nProfile,nLevel);
									if(nRet>=0)
									{
										pH323Channel->m_rH323Call.GetH264CapOptionParam(pvtTree,dataTypeHandle,nMaxFS,nMBPS,"videoCapability.1.genericVideoCapability.",nDual);
										nFormatType=pH323Channel->m_rH323Call.GetCallFormatTypeFromH264Param(nProfile,nLevel,nMaxFS,nMBPS,nMaxBitrate);
										nFrame=pH323Channel->m_rH323Call.GetFrameRateFromH264Param(nMaxFS,nMBPS);
									}
									else nProfile=64;
									//error=pH323Channel->m_rH323Call.GetH264ProfileLevel(pvtTree,dataTypeHandle,"videoCapability.1.genericVideoCapability." ,nProfile,nLevel);
									pH323Channel->H264Level() = nLevel;
									//error=pvtGetByPath(pvtTree,dataTypeHandle,"videoCapability.1.genericVideoCapability.collapsing.1.parameterValue.booleanArray", NULL, &nProfile, NULL);
						
									if(nProfile==64)
										pH323Channel->VideoCodecType() = VIDEC_CODEC_H264;
									if(nProfile==8)
										pH323Channel->VideoCodecType() = VIDEC_CODEC_H264_SVC;

									pH323Channel->VideoFrameRate()=nFrame;
									pH323Channel->VideoFormatType()=nFormatType;
									pH323Channel->PayloadType() = nDynamicRTPPayload;
									pH323Channel->MaxBitrate() = nMaxBitrate;
									pH323Channel->m_rH323Call.m_pSecondVideoRecvChannel = pH323Channel;
								}
							}
						}
						//add
						pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nDualVideoUpdatePT);
					}
				}
			}
			break;
		case cmCapData:
			pH323Channel->m_nMediaType=Enum_Channel_Media_H224;

			if(!strcmp(pH323Channel->ChannelName().c_str(), "h224"))
			{
				pH323Channel->m_rH323Call.m_pH224RecvChannel = pH323Channel;
				pH323Channel->PayloadType() = 97;//?
			}
			//add
			pH323Channel->SetChannelUpdatePayloadType(pH323Channel->m_rH323Call.m_nFECCUpdatePT);
			break;
		}

		//code add  Use:Get dynamicRTPPayloadType
		if(nDynamicRTPPayload > 0)
			pH323Channel->PayloadType() = nDynamicRTPPayload;
	}
	return 0;
}

int RVCALLCONV cmEvChannelRequestCloseStatus(HAPPCHAN haChan, HCHAN hsChan, cmRequestCloseStatus  status)
{
	H323Channel* pH323Channel = (H323Channel*)haChan;
	if(pH323Channel)
	{
		return 0;
	}
	if (status==cmRequestCloseRequest)
	{
		//cmChannelAnswer(hsChan); //modify 20140703       
		cmChannelDrop(hsChan);
	}
	return 0;
}

int RVCALLCONV cmEvChannelMiscCommand(HAPPCHAN haChan,HCHAN hsChan,cmMiscellaneousCommand miscCommand)
{
	H323Channel* pH323Channel = (H323Channel*)haChan;
	switch(miscCommand)
	{
	case cmSwitchReceiveMediaOn:
		{
		}
		break;
	case cmSwitchReceiveMediaOff:
		{
		}
		break;
	case cmVideoFastUpdatePicture:
		{
			pH323Channel->OnChannelRequestKeyFrame();
		}
		break;
	case cmVideoFreezePicture:
		{
			pH323Channel->OnChannelRequestFreeze();
		}
		break;
	}
	return 0;
}

int RVCALLCONV cmEvChannelSetAddress(HAPPCHAN haChan, HCHAN hsChan, RvUint32 ip, RvUint16 port)
{
	H323Channel* pH323Channel = (H323Channel*)haChan;
	if (pH323Channel)
	{
		pH323Channel->SetRemoteRTPAddress(ip,port);
	}
	return 0;
}

int RVCALLCONV cmEvChannelSetRTCPAddress(HAPPCHAN haChan, HCHAN hsChan, RvUint32 ip, RvUint16 port)
{
	H323Channel* pH323Channel = (H323Channel*)haChan;
	if (pH323Channel)
	{
		pH323Channel->SetRemoteRTCPAddress(ip,port);
	}
	return 0;
}

int RVCALLCONV cmEvChannelStateChanged(HAPPCHAN haChan, HCHAN hsChan, cmChannelState_e state, cmChannelStateMode_e stateMode)
{
	H323Channel* pH323Channel = (H323Channel*)haChan;
	if(!pH323Channel)
		return -1;

	LogFile(NULL,NULL,"","","cmEvChannelStateChanged 111\n");

	switch(state)
	{
	case cmChannelStateOffering:
		{
			pH323Channel->OnChannelOffering();
		}
		break;
	case cmChannelStateConnected:
		{
			LogFileChannel((H323MediaCall *) (&(pH323Channel->m_rH323Call)),(void *)haChan, hsChan,"","","OnChannelConnected",1);
			SetChannelFlowBitrate(pH323Channel,1);//notice FlowControlCommand
			pH323Channel->OnChannelConnected(state,stateMode);
		}
		break;
	case cmChannelStateDisconnected:
		{
			LogFileChannel((H323MediaCall *) (&(pH323Channel->m_rH323Call)),(void *)haChan, hsChan,"","","OnChannelDisconnected",0);
			RvUint8 temp[288]={0};
			int nValue=0;
			cmChannelGetParam(hsChan,cmChannelParamRejectReason,&nValue,temp);
			if(nValue==cmRejectReasonInvalidSessionID) 
			{
				int count=pH323Channel->m_rH323Call.GetH239VideoOutChannelInvSIDCnt();
				if(count <=16 && pH323Channel == pH323Channel->m_rH323Call.m_pSecondVideoSendChannel)
				{
					pH323Channel->m_rH323Call.m_nH239SessionId=pH323Channel->m_rH323Call.GetH239VideoOutChannelInvSIDCnt();
					pH323Channel->OnChannelDisconnected();
					pH323Channel->m_rH323Call.OpenH239VideoOutChannel();
					pH323Channel->m_rH323Call.H239VideoOutChannelInvSIDCntAdd();
				}
			}
			else
			{
				pH323Channel->OnChannelDisconnected();
			}

			delete pH323Channel;
			pH323Channel=NULL;
		}
		break;
	case cmChannelStateIdle:
		{
			cmChannelClose(hsChan);
		}
		break;
	default:
		break;
	}

	LogFile(NULL,NULL,"","","cmEvChannelStateChanged 222\n");

	return 0;
}

unsigned int RVCALLCONV cmHookListen(HPROTCONN hConn,int nodeId)
{
	return 0;
}

int RVCALLCONV cmHookListening(HPROTCONN hConn,int nodeId,unsigned int error)
{
	return 0;
}

int RVCALLCONV cmHookConnecting(HPROTCONN hConn,int nodeId)
{
	return 0;
}

void SetSocketKeepAlive(HPROTCONN hConn,bool bKeepAlive,int nIdleTimer,int nIntervalTimer)
{
	int err= 0;
	void * sock=NULL;
	HAPP hApp=H323CallMgr::Instance().m_hApp;
	RvStatus status=cmProtocolGetSocket(hApp,hConn,&sock);
	if(bKeepAlive && status >=0 && sock)
	{
		int nEnable = 1; //1在终端机上再接上网线10多秒后会卡住
		if(nIdleTimer <=0) nIdleTimer =1000;
		if(nIntervalTimer <=0) nIntervalTimer =1000;

#ifdef _WIN32

		SOCKET hSocket=(SOCKET)sock;
		//err=setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE,(char *)&nEnable, sizeof(nEnable));
		//if (err != SOCKET_ERROR)
		{
			//KeepAlive实现，单位毫秒
			tcp_keepalive alive_in;
			tcp_keepalive alive_out;
			alive_in.keepalivetime = nIdleTimer;         //开始首次KeepAlive探测前的TCP空闭时间 多长时间没有数据就开始send心跳包
			alive_in.keepaliveinterval = nIntervalTimer; //两次KeepAlive探测间的时间间隔 每隔多长时间send一个心跳包
			alive_in.onoff = nEnable;
			unsigned long nReturn = 0;
			err = WSAIoctl(hSocket, SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in),&alive_out, sizeof(alive_out), &nReturn, NULL, NULL);
		}

#else

		//KeepAlive实现，单位秒 //notice the linux wait test
		int keepAlive = nEnable;                  //设定KeepAlive
		int keepIdle = nIdleTimer / 1000;         //开始首次KeepAlive探测前的TCP空闲时间
		int keepInterval = nIntervalTimer / 1000; //两次KeepAlive探测间的时间间隔
		int keepCount = 3;                        //判定断开前的KeepAlive探测次数
		setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,(void*)&keepAlive,sizeof(keepAlive));
		setsockopt(sock,SOL_TCP,TCP_KEEPIDLE,(void *)&keepIdle,sizeof(keepIdle));
		setsockopt(sock,SOL_TCP,TCP_KEEPINTVL,(void *)&keepInterval,sizeof(keepInterval));
		setsockopt(sock,SOL_TCP,TCP_KEEPCNT,(void *)&keepCount,sizeof(keepCount));

#endif

	}
}

int RVCALLCONV cmHookInConn(HPROTCONN hConn,int nodeIdFrom,int nodeIdTo)
{
	int service_type = SERVICETYPE_QUALITATIVE ;
	cmProtocolSetTypeOfService(H323CallMgr::Instance().m_hApp,hConn,service_type);
	return 0;
}

int RVCALLCONV cmHookOutConn(HPROTCONN hConn,int nodeIdFrom,int nodeIdTo,unsigned int error)
{
	int service_type = SERVICETYPE_QUALITATIVE ;
	cmProtocolSetTypeOfService(H323CallMgr::Instance().m_hApp,hConn,service_type);
	return 0;
}

unsigned int RVCALLCONV cmHookSend(HPROTCONN hConn,int nodeId,unsigned int error)
{
	return 0;
}

unsigned int RVCALLCONV cmHookRecv(HPROTCONN hConn,int nodeId,unsigned int error)
{
	return 0;
}

unsigned int RVCALLCONV cmHookSendTo(HPROTCONN hConn,int nodeId,int nodeIdTo,unsigned int error)
{
	return 0;
}

unsigned int RVCALLCONV cmHookRecvFrom(HPROTCONN hConn, int nodeId,int nodeIdFrom,unsigned int multicast,unsigned int error)
{
	//return 0;
	int nRet =0;
	RvBool bInfo;
	if (H323CallMgr::Instance().m_hFwNat )
		nRet=RvH323FwNatHookRecvFrom( H323CallMgr::Instance().m_hFwNat, nodeId, nodeIdFrom, &bInfo );

	return nRet;
}

void RVCALLCONV cmHookClose(HPROTCONN hConn)
{
	//if(CallMgr::Instance().m_nUseCheckMedia >0 )
	{
		HCALL hsCall =NULL;
		H323MediaCall* pH323MediaCall =NULL;
		int type = -1;
		cmProtConnGetParam(H323CallMgr::Instance().m_hApp, hConn, cmProtConnParamType, &type, NULL);
		if (type != cmProtocolH245) return ;
		hsCall = cmGetCallByH245Chan(hConn);
		if (hsCall == NULL) return ;
		if(hsCall) cmCallGetHandle(hsCall,(HAPPCALL* )&pH323MediaCall );

		if(pH323MediaCall && pH323MediaCall->m_bCallState )
			H323CallMgr::Instance().HangupCallPointer(pH323MediaCall);
	}
}

int RVCALLCONV cmEvCallUserInfo(HAPPCALL haCall, HCALL hsCall, int nodeId)
{
	H323MediaCall* pH323MediaCall=(H323MediaCall*)haCall;
	if(!pH323MediaCall) return -1;
	pH323MediaCall->GetCallUserInfomation(hsCall,cmCallStateInit,nodeId);
	return 0;
}

//转呼控制
int RVCALLCONV cmEvCallFacility(HAPPCALL haCall,HCALL hsCall,int handle,RvBool *proceed)
{	
	H323MediaCall* pH323MediaCall=(H323MediaCall*)haCall;
	if(!pH323MediaCall) return -1;
	std::string	strLocalIP="";
	cmTransportAddress ta;
	unsigned long nLocal=H323CallMgr::Instance().GetLocalIPAddress(strLocalIP);
	if ((cmCallGetParam(hsCall,cmParamAlternativeAddress,0,NULL,(char*)&ta)>=0) && (ta.type == cmTransportTypeIP) && (ta.ip != 0) && (ta.port != 0))
	{
		unsigned long nDest=ta.ip;
		if(nLocal==nDest)
		{
			*proceed = RV_FALSE;
			pH323MediaCall->m_hsCall=NULL;
			H323CallMgr::Instance().H235SecurityCallEnd(pH323MediaCall);
			if (hsCall)
			{
				//不让转呼本机
				cmCallDrop(hsCall);
			}
		}
	}
	return 0;
}

//add adapter
int RVCALLCONV cmEvRASRequest( IN  HRAS             hsRas,
							  IN  HCALL            hsCall,
							  OUT LPHAPPRAS        lphaRas,
							  IN  cmRASTransaction transaction,
							  IN  cmRASTransport*  srcAddress, 
							  IN  HAPPCALL         haCall         OPTIONAL)
{
	int nRet = 0;

	RvBool bInfo = FALSE;
	RvBool bIgnore = FALSE;
	//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	std::string strE164ID="";
	std::string strH323ID="";
	H323RegUser* pH323RegUser=NULL;
	int paramNum=0;
	int i=0;
	int error=-1;
	bool findE164=false;
	bool findH323ID=false;
	cmAlias alias;
	char  buf[288]={0};
	alias.string = buf;
	//or use hsRas find reguser
	paramNum = cmRASGetNumOfParams(hsRas, cmRASTrStageRequest,cmRASParamEndpointAlias);
	for(i=0; i<paramNum; i++)
	{
		error = cmRASGetParam(hsRas,cmRASTrStageConfirm,cmRASParamTerminalAlias,i,0,(char*)&alias);
		int aliaslen=alias.length;
		if( (error >= 0) && (aliaslen >0) )
		{
			if(alias.type == cmAliasTypeE164)
			{
				if(!findE164) strE164ID = alias.string;
				findE164=true;
			}
			if(alias.type == cmAliasTypeH323ID)
			{
				if(!findH323ID)
				{
					char idbuf[288]={0};
					utlBmp2Chr(idbuf,(RvUint8*)alias.string,alias.length);
					strH323ID=idbuf;
				}
				findH323ID=true;
			}
		}
		if(findE164 && findH323ID ) break;
	}

	if(strE164ID.size() >0)
		pH323RegUser=H323CallMgr::Instance().FindH323RegAccount(strE164ID);
	else if(strH323ID.size() >0)
	{
		pH323RegUser=H323CallMgr::Instance().FindH323RegAccount(strH323ID);
		if(pH323RegUser) strE164ID=pH323RegUser->GetRegPhoneNum();
	}

	if(pH323RegUser)  * lphaRas = (HAPPRAS)pH323RegUser;

	if(pH323RegUser && pH323RegUser->m_nUseFWNAT >0 && H323CallMgr::Instance().m_hFwNat )
	{
		nRet = RvH323FwNatEvRASRequest(H323CallMgr::Instance().m_hFwNat, hsRas, &bIgnore, &bInfo);
		if(nRet <0 ) cmRASClose(hsRas);
		return nRet;
	}
	else if(transaction ==cmRASServiceControl  && H323CallMgr::Instance().m_hFwNat )
	{
		nRet = RvH323FwNatEvRASRequest(H323CallMgr::Instance().m_hFwNat, hsRas, &bIgnore, &bInfo);
		if(nRet <0 ) cmRASClose(hsRas);
		return nRet;
	}

	switch(transaction)
	{
	case cmRASRegistration:
		cmRASClose(hsRas);
		break;
	case cmRASUnregistration:
		cmRASClose(hsRas);
		break;
	case cmRASAdmission:
		cmRASClose(hsRas);
		break;
	case cmRASDisengage:
		cmRASConfirm(hsRas);
		cmRASClose(hsRas);
		break;
	case cmRASLocation:
		cmRASConfirm(hsRas);
		cmRASClose(hsRas);
		break;
	case cmRASBandwidth:
		cmRASClose(hsRas);
		break;
	case cmRASInfo:
		{
			//send IRR
			H323MediaCall* pH323MediaCall =NULL;
			HCALL hsCall=NULL;
			nRet = cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamCallHandle,0, NULL, (char*)&hsCall);
			if(nRet >=0 && hsCall) cmCallGetHandle(hsCall,(HAPPCALL* )&pH323MediaCall );
			if(pH323MediaCall )
			{
				pH323MediaCall->SendRegIRRMessage(hsRas);
			}
			else 
			{
				cmRASConfirm(hsRas);
			}
			cmRASClose(hsRas);
			break;
		}
	case cmRASGatekeeper:
	case cmRASNonStandard:
	case cmRASUnknown:
	case cmRASResourceAvailability:
	case cmRASUnsolicitedIRR:
		cmRASClose(hsRas);//notice 
		break;
	default:
		cmRASClose(hsRas);
		break;
	}

#else

	if(H323CallMgr::Instance().m_nUseTerminalFWNAT > 0 && H323CallMgr::Instance().m_hFwNat)
		nRet =RvH323FwNatEvRASRequest(H323CallMgr::Instance().m_hFwNat, hsRas, &bIgnore, &bInfo);

#endif

	cmRASClose(hsRas);
	return nRet;
}

int RVCALLCONV cmEvRASConfirm( IN  HAPPRAS          haRas,
							  IN  HRAS             hsRas)
{
	int error=0;
	int i=0;
	int paramNum=0;
	bool findE164=false;
	bool findH323ID=false;
	std::string strTelnum="";
	std::string strH323ID="";
	cmAlias alias;
	memset(&alias, 0, sizeof(cmAlias) );
	char tmp[288]={0};
	alias.string = tmp;

	alias.length = sizeof(tmp);
	cmRASTransaction transact;
	cmRASGetTransaction(hsRas,&transact);
	switch(transact)
	{
	case cmRASGatekeeper:
		{
			memset(&H323CallMgr::Instance().m_TransportAddr, 0, sizeof(cmTransportAddress) );
			int nRet = cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamRASAddress, 0,0
				,(char*)&(H323CallMgr::Instance().m_TransportAddr) );
			H323CallMgr::Instance().m_nFindGKAddr=true;
			//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
			H323RegUser * pH323RegUser =(H323RegUser *)haRas;
			if(pH323RegUser)
			{
				pH323RegUser->SendRegMessage(cmRASRegistration);
			}
#endif

		}
		break;
	case cmRASRegistration:
		{
//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

			H323RegUser * pH323RegUser =(H323RegUser *)haRas;
			if(pH323RegUser)
			{
				int timeToLive=0;
				int nRet=-1;
				cmAlias aliasEPId;
				memset(&aliasEPId, 0, sizeof(cmAlias) );
				char tmpEP[288]={0};
				aliasEPId.string = tmpEP;
				nRet =cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamEndpointID,0,0,(char*)&aliasEPId);

				cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamTimeToLive,0,&timeToLive,NULL);
				nRet =cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamGatekeeperID,0,NULL,(char*)&alias);
				if(nRet >=0)
				{
					pH323RegUser->SetAccountRegInfo(&alias,&aliasEPId,timeToLive);
					H323CallMgr::Instance().OnDAP_H323RegistrationSuccess(pH323RegUser->GetRegH323ID().c_str(),pH323RegUser->GetRegPhoneNum().c_str());
				}
			}
#else

			paramNum = cmRASGetNumOfParams(hsRas, cmRASTrStageConfirm, cmRASParamEndpointAlias);
			for(i=0; i<paramNum; i++)
			{
				//add
				//memset(&alias, 0, sizeof(cmAlias) );
				error = cmRASGetParam(hsRas,cmRASTrStageConfirm,cmRASParamTerminalAlias,i,0,(char*)&alias);
				int aliaslen=alias.length; //add
				if( (error >= 0) && (aliaslen >0) )
				{
					if(alias.type == cmAliasTypeE164)
					{
						if(!findE164) strTelnum=alias.string;
						findE164=true;
					}
					if(alias.type == cmAliasTypeH323ID)
					{
						//add
						if(!findH323ID)
						{
							char idbuf[288]={0};
							utlBmp2Chr(idbuf,(RvUint8*)alias.string,alias.length);
							strH323ID=idbuf;
						}
						findH323ID=true;
					}
				}
				if(findE164 && findH323ID ) break;
			}
			if(findE164 || findH323ID )
				H323CallMgr::Instance().OnDAP_H323RegistrationSuccess(strH323ID.c_str(),strTelnum.c_str());

#endif

		}
		break;
	case cmRASAdmission:
		{
//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

			H323MediaCall* pH323MediaCall =NULL;
			HCALL hsCall=NULL;
			int nRet = cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamCallHandle,0, NULL, (char*)&hsCall);
			if(nRet >=0 && hsCall) cmCallGetHandle(hsCall,(HAPPCALL* )&pH323MediaCall );

			H323RegUser * pH323RegUser =(H323RegUser *)haRas;
			if(pH323RegUser)
			{
				cmTransportAddress signalAddr;
				nRet = cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamDestCallSignalAddress,0, NULL, (char*)&signalAddr);
				if(nRet >=0)
				{
					pH323RegUser->SetAccountSignalCallAddr(&signalAddr);
				}
				LogFile(NULL,NULL,"","","ACF AccountCall.");
				if(pH323MediaCall) pH323RegUser->AccountCall(pH323MediaCall);
			}

			//int nIrrFreq =0; //notice AccountCall  OnDAP_CallDestroyed delete pH323MediaCall ?
			//nRet = cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamIrrFrequency,0,&nIrrFreq,NULL);
			//if(nRet >=0 && pH323MediaCall)
			//{
			//	//notice the not support temp
			//	//pH323MediaCall->SendRegIRRMessage(NULL,1);
			//}

#endif

		}
		break;
	default:
		break;
	}
	cmRASClose(hsRas);
	return 0;
}

void GetRasRejectReason(cmRASReason reason,std::string & strReason)
{
	switch(reason)
	{
	case cmRASReasonResourceUnavailable : strReason = "ResourceUnavailable"; break;
	case cmRASReasonInsufficientResources : strReason = "InsufficientResources"; break;
	case cmRASReasonInvalidRevision : strReason = "InvalidRevision"; break;
	case cmRASReasonInvalidCallSignalAddress : strReason = "InvalidCallSignalAddress"; break;
	case cmRASReasonInvalidRASAddress : strReason = "InvalidRASAddress"; break; 
	case cmRASReasonInvalidTerminalType : strReason = "InvalidTerminalType"; break;
	case cmRASReasonInvalidPermission : strReason = "InvalidPermission"; break;
	case cmRASReasonInvalidConferenceID : strReason = "InvalidConferenceID"; break;
	case cmRASReasonInvalidEndpointID : strReason = "InvalidEndpointID"; break;
	case cmRASReasonCallerNotRegistered : strReason = "CallerNotRegistered"; break;
	case cmRASReasonCalledPartyNotRegistered : strReason = "CalledPartyNotRegistered"; break;
	case cmRASReasonDiscoveryRequired : strReason = "DiscoveryRequired"; break;
	case cmRASReasonDuplicateAlias : strReason = "DuplicateAlias"; break;
	case cmRASReasonTransportNotSupported : strReason = "TransportNotSupported"; break;
	case cmRASReasonCallInProgress : strReason = "CallInProgress"; break;
	case cmRASReasonRouteCallToGatekeeper : strReason = "RouteCallToGatekeeper"; break;
	case cmRASReasonRequestToDropOther : strReason = "RequestToDropOther"; break;
	case cmRASReasonNotRegistered : strReason = "NotRegistered"; break;
	case cmRASReasonUndefined : strReason = "Undefined"; break;
	case cmRASReasonTerminalExcluded : strReason = "TerminalExcluded"; break;
	case cmRASReasonNotBound : strReason = "NotBound"; break;
	case cmRASReasonNotCurrentlyRegistered : strReason = "NotCurrentlyRegistered"; break;
	case cmRASReasonRequestDenied : strReason = "RequestDenied"; break;
	case cmRASReasonLocationNotFound : strReason = "LocationNotFound"; break;
	case cmRASReasonSecurityDenial : strReason = "SecurityDenial"; break;
	case cmRASReasonTransportQOSNotSupported : strReason = "TransportQOSNotSupported"; break;
	case cmRASResourceUnavailable : strReason = "cmRASResourceUnavailable"; break;
	case cmRASReasonInvalidAlias : strReason = "InvalidAlias"; break;
	case cmRASReasonPermissionDenied : strReason = "PermissionDenied"; break;
	case cmRASReasonQOSControlNotSupported : strReason = "QOSControlNotSupported"; break;
	case cmRASReasonIncompleteAddress : strReason = "IncompleteAddress"; break;
	case cmRASReasonFullRegistrationRequired : strReason = "FullRegistrationRequired"; break;
	case cmRASReasonRouteCallToSCN : strReason = "RouteCallToSCN"; break;
	case cmRASReasonAliasesInconsistent : strReason = "AliasesInconsistent"; break;
	case cmRASReasonAdditiveRegistrationNotSupported : strReason = "AdditiveRegistrationNotSupported"; break;
	case cmRASReasonInvalidTerminalAliases : strReason = "InvalidTerminalAliases"; break;      
	case cmRASReasonExceedsCallCapacity : strReason = "ExceedsCallCapacity"; break;
	case cmRASReasonCollectDestination : strReason = "CollectDestination"; break;
	case cmRASReasonCollectPIN : strReason = "CollectPIN"; break;
	case cmRASReasonGenericData : strReason = "GenericData"; break;
	case cmRASReasonNeededFeatureNotSupported : strReason = "NeededFeatureNotSupported"; break;
	case cmRASReasonUnknownMessageResponse : strReason = "UnknownMessageResponse"; break;     
	case cmRASReasonHopCountExceeded : strReason = "HopCountExceeded"; break;
	case cmRASReasonSecurityDHmismatch : strReason = "SecurityDHmismatch"; break;
	case cmRASReasonNoRouteToDestination : strReason = "NoRouteToDestination"; break;
	case cmRASReasonUnallocatedNumber : strReason = "UnallocatedNumber"; break;
	default : strReason = "Undown"; break;
	}
}

int RVCALLCONV cmEvRASReject( IN  HAPPRAS          haRas,
							 IN  HRAS             hsRas,
							 IN  cmRASReason      reason)
{
	int error=0;
	int i=0;
	int paramNum=0;
	bool findE164=false;
	bool findH323ID=false;
	std::string strTelnum="";
	std::string strH323ID="";
	std::string strReason="UndownReasonORNotSupported";
	cmAlias alias;
	char tmp[288]={0};
	alias.string = tmp;
	alias.length = sizeof(tmp);
	cmRASTransaction transact;
	cmRASGetTransaction(hsRas,&transact);
	switch(transact)
	{
	case cmRASRegistration:
		{
			GetRasRejectReason(reason,strReason);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
			H323RegUser * pH323RegUser =(H323RegUser *)haRas;
			if(pH323RegUser)
			{
				pH323RegUser->m_nSendKeepAlive =RV_FALSE;
				H323CallMgr::Instance().OnDAP_H323RegistrationError(pH323RegUser->GetRegH323ID().c_str(),pH323RegUser->GetRegPhoneNum().c_str(),(char *)strReason.c_str());
			}

#else

			paramNum = cmRASGetNumOfParams(hsRas, cmRASTrStageConfirm, cmRASParamEndpointAlias);
			for(i=0; i<paramNum; i++)
			{
				error = cmRASGetParam(hsRas,cmRASTrStageConfirm,cmRASParamTerminalAlias,i,0,(char*)&alias);
				int aliaslen=alias.length;
				if( (error >= 0) && (aliaslen >0) )
				{
					if(alias.type == cmAliasTypeE164)
					{
						if(!findE164) strTelnum=alias.string;
						findE164=true;
					}
					if(alias.type == cmAliasTypeH323ID)
					{
						if(!findH323ID)
						{
							char idbuf[288]={0};
							utlBmp2Chr(idbuf,(RvUint8*)alias.string,alias.length);
							strH323ID=idbuf;
						}
						findH323ID=true;
					}
				}
				if(findE164 && findH323ID ) break;
			}

			if(findE164 || findH323ID )
			{
			}

#endif

		}
		break;
	case cmRASAdmission:
		{
			//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

			LogFile(NULL,NULL,"","","ARJ cmCallClose.");
			CloseRasCall(haRas,hsRas,ModeDisconnectedReject,reason);
			H323RegUser * pH323RegUser =(H323RegUser *)haRas;
			if(pH323RegUser)
			{
				if(reason ==cmRASReasonCallerNotRegistered)
				{
					GetRasRejectReason(reason,strReason);
					H323CallMgr::Instance().OnDAP_H323RegistrationError(pH323RegUser->GetRegH323ID().c_str(),pH323RegUser->GetRegPhoneNum().c_str(),(char *)strReason.c_str());
					pH323RegUser->SendRegMessage(cmRASRegistration,1);
				}
			}
#endif
		}
		break;
	default:
		break;
	}
	cmRASClose(hsRas);
	return 0;
}

int RVCALLCONV cmEvRASTimeout( IN  HAPPRAS          haRas,
							  IN  HRAS             hsRas)
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	cmRASTransaction transact;
	cmRASGetTransaction(hsRas,&transact);
	switch(transact)
	{
	case cmRASAdmission:
		{
			LogFile(NULL,NULL,"","","cmEvRASTimeout ARQ.");
			CloseRasCall(haRas,hsRas,ModeDisconnectedReject,-1);
			break;
		}
	default:
		break;
	}

#endif

	cmRASClose(hsRas);
	return 0;
}

void CloseRasCall(HAPPRAS haRas,HRAS hsRas,IMXEC_CALL_END_REASON nEndReason,int rasReason)
{
	H323RegUser * pH323RegUser =(H323RegUser *)haRas;
	if(pH323RegUser)
	{
		H323MediaCall* pH323MediaCall =NULL;
		HCALL hsCall=NULL;
		int nRet = cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamCallHandle,0, NULL, (char*)&hsCall);
		if(nRet >=0 && hsCall) cmCallGetHandle(hsCall,(HAPPCALL* )&pH323MediaCall );
		if(pH323MediaCall)
		{
			if(rasReason ==cmRASReasonCalledPartyNotRegistered) nEndReason=ModeDisconnectedRemoteNotReg;
			cmCallClose(pH323MediaCall->m_hsCall);
			pH323MediaCall->m_strUserID=pH323MediaCall->m_strCalled;
			pH323MediaCall->m_strUserName=pH323MediaCall->m_strCalled;
			pH323MediaCall->DisconnectReason()=GetCallDisconnectReason(nEndReason);/*strDisconnectReason;*/
			H323CallMgr::Instance().OnDAP_CallDestroyed(pH323MediaCall);
			//pH323RegUser->AccountCallRelease(pH323MediaCall);
		}
	}
}

void SetNonStandardData(int nodeId,char * nonBuf)
{
	HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	int nodeSub=pvtChild(hVal,nodeId);
	if(nodeSub >=0)
	{
		int tempNodeId = pvtBuildByPath(hVal, nodeSub, "nonStandardData", 0, 0 );
		//tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.object", length, OID );
		int tmpError = pvtBuildByPath(hVal, tempNodeId, "nonStandardIdentifier.h221NonStandard.t35CountryCode",H323CallMgr::Instance().m_nT35Code, 0 );
		tmpError = pvtBuildByPath(hVal, tempNodeId, "nonStandardIdentifier.h221NonStandard.t35Extension", H323CallMgr::Instance().m_nT35Extension, 0 );
		tmpError = pvtBuildByPath(hVal, tempNodeId, "nonStandardIdentifier.h221NonStandard.manufacturerCode", H323CallMgr::Instance().m_nManufacturerCode, 0 );

		int paramLen=strlen(nonBuf);
		tmpError = pvtBuildByPath(hVal, tempNodeId, "data", paramLen, nonBuf);
	}
}


RvBool RVCALLCONV cmEvRASMessageSend(IN  HAPPRAS         haRas,
								  IN  HRAS            hsRas,
								  IN  cmRASTrStage    rasTrStage,
								  IN  RvPvtNodeId     messageNodeId)
{
	H323MediaCall* pH323MediaCall =NULL;
	HCALL hsCall=NULL;
	cmRASTransaction transaction;
	int nStatus=cmRASGetTransaction(hsRas,&transaction);

	if(nStatus >=0)
	{
		//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

		if(transaction==cmRASInfo || transaction==cmRASUnsolicitedIRR || transaction==cmRASDisengage) return RV_FALSE;

#else

		////check send URQ ,only send once
		//if(rasTrStage ==cmRASTrStageRequest && transaction==cmRASUnregistration)
		//	return H323CallMgr::Instance().CheckRASURQSend();

#endif

	}

	H323RegUser * pH323RegUser=NULL;
	std::string strPhoneNum =H323CallMgr::Instance().m_strTelNumber;
	std::string strPassword =H323CallMgr::Instance().m_strRegPassword;
	//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	pH323RegUser =(H323RegUser *)haRas;
	if(pH323RegUser)
	{
		strPhoneNum = pH323RegUser->GetRegPhoneNum();
		strPassword = pH323RegUser->GetRegPassword();
	}
#endif

	if(rasTrStage ==cmRASTrStageRequest )
	{
		//add by Jan for username and password [20131204]
		if(strPhoneNum.size() >0 && strPassword.size() >0 )
		{
			const char * userName=strPhoneNum.c_str();
			const char * passWord=strPassword.c_str();
			int nCryptoTokensId = -1;
			int nCryptoEPPwdHashId = -1;
			int nAliasId = -1;
			int nAliasNameId = -1;
			int nTimeStampId = -1;
			int nTokenId = -1;
			int nAlgorithmOIDId = -1;
			int nHashId = -1;
			int nParamsId = -1;
			char  OID_MD5[16*2] = {0};
			MD5_CTX mdc;
			unsigned char src[262] = {0};
			unsigned char md5hash[16] = {0};
			int pos = 0;
			unsigned char infoLen = 0;
			int nMD5len = utlEncodeOID(sizeof(OID_MD5), OID_MD5, MD5_ID);
			HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
			int nNodeId=messageNodeId;
			nNodeId=pvtChild(hVal,messageNodeId);
			__pvtBuildByFieldIds(nCryptoTokensId, hVal, messageNodeId, {_anyField _q931(cryptoTokens) _nul(1) LAST_TOKEN}, 0, NULL);

			__pvtBuildByFieldIds(nCryptoEPPwdHashId, hVal, nCryptoTokensId, {_q931(cryptoEPPwdHash) LAST_TOKEN}, 0, NULL);

			__pvtBuildByFieldIds(nAliasId, hVal, nCryptoEPPwdHashId, { _q931(alias) LAST_TOKEN }, 0, NULL );

			__pvtBuildByFieldIds(nAliasNameId, hVal, nAliasId, { _q931(e164) LAST_TOKEN }, strlen(userName), userName);

			__pvtBuildByFieldIds(nTimeStampId, hVal, nCryptoEPPwdHashId, { _q931(timeStamp) LAST_TOKEN }, 17, NULL);

			__pvtBuildByFieldIds(nTokenId, hVal, nCryptoEPPwdHashId, { _q931(token) LAST_TOKEN }, 0, NULL );

			__pvtBuildByFieldIds(nAlgorithmOIDId, hVal, nTokenId, { _q931(algorithmOID) LAST_TOKEN }, nMD5len, OID_MD5 );

			__pvtBuildByFieldIds(nParamsId, hVal, nTokenId, { _q931(paramS)/*_q931(null)*/ LAST_TOKEN }, 0, NULL );

			/*md5*/
			/*
			（1）标识符字段，1字节，固定为0x61；
			（2）时间戳字段，2字节，固定为0x0011；
			（3）密码长度字段，1字节；
			（4）密码字段，按照密码长度依次拷贝；
			（5）分隔符字段，1字节，固定为0x00；
			（6）E164别名长度字段，1字节；
			（7）E164别名，按照别名长度依次拷贝。
			262 = 1 + 2 + 1 + 128 + 1 + 1 +128
			*/
			pos = 0;
			src[pos++] = 0x61;
			src[pos++] = 0x00;
			src[pos++] = 0x11;
			infoLen = (unsigned char)strlen(passWord);
			src[pos++] = infoLen;
			memcpy(&src[pos], passWord, infoLen);
			pos += infoLen;
			src[pos++] = 0x00;
			infoLen = (unsigned char)strlen(userName);
			src[pos++] = infoLen;
			memcpy(&src[pos], userName, infoLen);
			pos += infoLen;

			MD5Init(&mdc);
			MD5Update(&mdc, (unsigned char *)src, pos);
			MD5Final(md5hash, &mdc);

			__pvtBuildByFieldIds(nHashId, hVal, nTokenId, { _q931(hash) LAST_TOKEN }, 128, (char *)md5hash);
		}
		//end [20131204]

		if(nStatus >=0 && transaction == cmRASRegistration)
		{
			char nonstandard[288]={0};
			std::string strLocalIP="";

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
			if(pH323RegUser) pH323RegUser->GetLocalIPByGK(strLocalIP);
			
			if(pH323RegUser && pH323RegUser->m_nSendKeepAlive ==RV_FALSE)
			{
				HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
				int nEPID=pvtGetByPath(hVal,messageNodeId,"registrationRequest.endpointIdentifier",NULL,NULL,NULL);
				if(nEPID >=0) pvtDelete(hVal, nEPID);
				int nGPID=pvtGetByPath(hVal,messageNodeId,"registrationRequest.gatekeeperIdentifier",NULL,NULL,NULL);
				if(nGPID >=0) pvtDelete(hVal, nGPID);
			}
#else

			//20140828 Add
			CheckAddSignalAddrAliasNode(messageNodeId);
			strLocalIP = GetLocalIPByRemoteIP(H323CallMgr::Instance().m_strRegHost.c_str(),H323CallMgr::Instance().m_nRegGKPort);

#endif

			sprintf(nonstandard,"[<ID=HPRRQNSM><USE=NATFW><LocalIP=%s>]",strLocalIP.c_str() );
			SetNonStandardData(messageNodeId,nonstandard);
		}

	}

	if(rasTrStage ==cmRASTrStageRequest && nStatus >=0 && transaction == cmRASAdmission)
	{
		int nRet = cmRASGetParam(hsRas, rasTrStage, cmRASParamCallHandle,0, NULL, (char*)&hsCall);
		if(nRet >=0 && hsCall) cmCallGetHandle(hsCall,(HAPPCALL* )&pH323MediaCall);
		if(pH323MediaCall)
		{
			std::string strUserNonstand ="";
			pH323MediaCall->GetUserNonstandardInfo(strUserNonstand);
			SetNonStandardData(messageNodeId,(char *)strUserNonstand.c_str());
		}
	}

	RvBool bStop = FALSE;
	RvBool bInfo = FALSE;
	//add gk more account support
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	RvBool bResult=RV_FALSE;
	pH323RegUser =(H323RegUser *)haRas;

	if(pH323RegUser && pH323RegUser->m_nUseFWNAT >0 && H323CallMgr::Instance().m_hFwNat )
	{
		bResult=RvH323FwNatEvRASMessageSend(H323CallMgr::Instance().m_hFwNat, hsRas, rasTrStage, messageNodeId, &bStop, &bInfo);
	}
 
	if(nStatus >=0 && transaction==cmRASUnregistration && pH323RegUser)
	{
		if(pH323RegUser->m_nSendKeepAlive ==RV_FALSE)
		{
			HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
			int nEPID=pvtGetByPath(hVal,messageNodeId,"unregistrationRequest.endpointIdentifier",NULL,NULL,NULL);
			if(nEPID >=0) pvtDelete(hVal, nEPID);
		}
		delete pH323RegUser;
		pH323RegUser=NULL;
		return bResult;

	}

#else

	if(H323CallMgr::Instance().m_nUseTerminalFWNAT > 0 && H323CallMgr::Instance().m_hFwNat )
		return RvH323FwNatEvRASMessageSend(H323CallMgr::Instance().m_hFwNat, hsRas, rasTrStage, messageNodeId, &bStop, &bInfo);

#endif

	return RV_FALSE;
}

int RVCALLCONV cmEvAutoRASConfirm(IN  HRAS             hsRas)
{
	H323MediaCall* pH323MediaCall =NULL;
	HCALL hsCall=NULL;
	cmRASTransaction transaction;
	char strData[256]={0};
	int nodeId =-1;
	int nStatus=cmRASGetTransaction(hsRas,&transaction);
	if(nStatus >=0 && transaction == cmRASAdmission)
	{
		int nRet = cmRASGetParam(hsRas, cmRASTrStageConfirm, cmRASParamCallHandle,0, NULL, (char*)&hsCall);
		if(nRet >=0 && hsCall) cmCallGetHandle(hsCall,(HAPPCALL* )&pH323MediaCall);
		if(pH323MediaCall)
		{
			nStatus = cmRASGetParam(hsRas,cmRASTrStageConfirm,cmRASParamNonStandardData,0,&nodeId,strData);
			if(nStatus >=0 && nodeId >=0) pH323MediaCall->GetCallUserInfomation(hsCall,(cmCallState_e)101,nodeId);
		}

	}
	//cmRASClose(hsRas);
	return 0;
}

int RVCALLCONV cmEvAutoRASReject(IN  HRAS             hsRas,
							 IN  cmRASReason      reason)
{
	cmRASClose(hsRas);
	return 0;
}

int RVCALLCONV cmEvAutoRASTimeout(IN  HRAS             hsRas)
{
	cmRASClose(hsRas);
	return 0;
}

//打印能力集信息
void printCapToBuffer(void* context, IN const char* line, ...)
{
	pvtPrintContext * pContext=(pvtPrintContext *)context;
	if(pContext) pContext->m_strPrint=pContext->m_strPrint + line;
}

//add
int RVCALLCONV cmEvCallRoundTripDelay(IN HAPPCALL haCall,IN HCALL hsCall,IN RvInt32 delay) /* Positive: msec delay. Negative: expiration. */
{
	H323MediaCall* pH323MediaCall = (H323MediaCall*)haCall;
	if(pH323MediaCall)
	{
		if(delay >=0)
		{
			pH323MediaCall->OnRecvRoundTripResponse(hsCall,delay);
		}
	}
	return 0; 
}

//H460 Client 
int RVCALLCONV cmEvAutoRASRequest(HRAS hsRas, HCALL hsCall, cmRASTransaction transaction, cmRASTransport*  srcAddress, HAPPCALL haCall)
{
	RvBool bInfo = FALSE;
	RvBool bIgnore = FALSE;
	int nRet=-1;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	nRet=0;

#else

	if(H323CallMgr::Instance().m_nUseTerminalFWNAT > 0 && H323CallMgr::Instance().m_hFwNat )
	{
		nRet= RvH323FwNatEvAutoRASRequest(H323CallMgr::Instance().m_hFwNat, hsRas, &bIgnore, &bInfo);
	}
	else if(transaction ==cmRASDisengage )
	{
		//notice recv DRQ hungup
		H323MediaCall* pH323MediaCall = (H323MediaCall* )haCall;
		if(pH323MediaCall  && pH323MediaCall->m_bCallState ) H323CallMgr::Instance().HangupCallPointer(pH323MediaCall,0);
		cmRASConfirm(hsRas);
		cmRASClose(hsRas);
		nRet =0;
	}
	else
	{
		cmRASConfirm(hsRas);
		cmRASClose(hsRas);
		nRet =0;
	}

#endif

	return nRet;

}

int RVCALLCONV cmEvCallSendMessage(HAPPCALL haCall, HCALL hsCall, int message)
{
	H323MediaCall * pH323MediaCall=(H323MediaCall*)haCall;
	RvBool bInfo = FALSE;
	int nRet =-1;
	HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);

	if(pH323MediaCall)
	{
	//set setup destinationAddress or sourceAddress
	int nSetup=0;
	int nDestinationAddr=0;
	nSetup =pvtGetNodeIdByPath(hVal,message,"message.setup.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.setup");
	if(nSetup >=0)
	{
		//support more net card
		int nSourceCallAddr=pvtBuildByPath(hVal,nSetup,"sourceCallSignalAddress",0,NULL);
		if(pH323MediaCall->m_strMakeSourceIP.size() >0 && nSourceCallAddr >=0 )
		{
			int  nSourceCallPort=0;
			pvtGetByPath(hVal,nSourceCallAddr,"ipAddress.port",NULL,&nSourceCallPort,NULL);
			if(nSourceCallPort >0)
			{
				cmTransportAddress tranAddr;
				memset( &tranAddr, 0, sizeof(cmTransportAddress) );
				char addr[288]={0};
				sprintf(addr,"%s:%d",pH323MediaCall->m_strMakeSourceIP.c_str(),nSourceCallPort);
				cmString2TransportAddress(addr,&tranAddr);
				int temp=pvtGetNodeIdByPath(hVal,nSourceCallAddr,"ipAddress");
				if(temp >=0) pvtDelete(hVal,temp);
				cmTransportAddress2Vt(hVal,&tranAddr,nSourceCallAddr);
			}
		}

		cmAlias termAlias;
		char bmpStr[288] = {0};
		memset(&termAlias, 0, sizeof(termAlias) );
		termAlias.string=bmpStr;
		nDestinationAddr=pvtGetNodeIdByPath(hVal,nSetup,"destinationAddress");
		if(nDestinationAddr >=0 )
		{
			int temp=pvtGetNodeIdByPath(hVal,nDestinationAddr,"1");
			if(temp >=0)
			{
				cmVt2Alias(hVal,&termAlias,temp);
			}
		}
		//int nSourceAddr=pvtBuildByPath(hVal,nSetup,"sourceAddress.1",0,NULL);
	}
	//set connect h245Address
	std::string strExternalIPAddress=pH323MediaCall->GetMgrExternalIPAddressString();
	if (strExternalIPAddress.size() >0)
	{
		int  nH245Port=0;
		int  nH245Addr=0;
		nH245Addr=pvtGetNodeIdByPath(hVal,message,"message.connect.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.connect.h245Address");
		if(nH245Addr >=0 )
		{
			pvtGetByPath(hVal,nH245Addr,"ipAddress.port",NULL,&nH245Port,NULL);
			if(nH245Port >0)
			{
				cmTransportAddress tranAddr;
				memset( &tranAddr, 0, sizeof(cmTransportAddress) );
				char addr[288]={0};
				sprintf(addr,"%s:%d",strExternalIPAddress.c_str(),nH245Port);
				cmString2TransportAddress(addr,&tranAddr);
				int temp=pvtGetNodeIdByPath(hVal,nH245Addr,"ipAddress");
				if(temp >=0) pvtDelete(hVal,temp);
				cmTransportAddress2Vt(hVal,&tranAddr,nH245Addr);
			}
		}

	}

}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	if ( pH323MediaCall && pH323MediaCall->m_nUseFWNAT >0 && H323CallMgr::Instance().m_hFwNat )
	{
		nRet= RvH323FwNatCallSendMessage( H323CallMgr::Instance().m_hFwNat, hsCall,  NULL, message, &bInfo);
	}

#else

	if(H323CallMgr::Instance().m_nUseTerminalFWNAT > 0 && H323CallMgr::Instance().m_hFwNat )
		nRet= RvH323FwNatCallSendMessage( H323CallMgr::Instance().m_hFwNat, hsCall,  NULL, message, &bInfo);
#endif

	return nRet;
}

//notice FlowControlCommand
void SetChannelFlowBitrate(H323Channel* pH323Channel,int nBitrate)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else

	H323MediaCall* pH323MediaCall =(H323MediaCall* )(&pH323Channel->m_rH323Call);
	if (pH323Channel==pH323MediaCall->m_pAudioSendChannel)
	{
		//pH323MediaCall->m_nAudioFlowCCBitRate =nBitrate;
		pH323MediaCall->ReSetMainAudioCapBitrate(nBitrate);
	}
	else if (pH323Channel==pH323MediaCall->m_pMainVideoSendChannel)
	{
		//pH323MediaCall->m_nMainVideoFlowCCBitRate =nBitrate;
		pH323MediaCall->ReSetMainVideoCapBitrate(nBitrate);
	}
	else if (pH323Channel==pH323MediaCall->m_pSecondVideoSendChannel)
	{
		//pH323MediaCall->m_nDualVideoFlowCCBitRate =nBitrate;
		pH323MediaCall->ReSetSecondVideoCapBitrate(nBitrate);
	}

#endif

}

//modify by gsb 20140707
EM_ENTITY_TYPE GetTerminalType(H323MediaCall* pH323MediaCall,HPVT hVal,int nodeId,int type) //1 setup 2 connect
{
	LogFile(NULL,NULL,"","","GetTerminalType 111\n");

	EM_ENTITY_TYPE emEntityType = EM_ENTITY_UNKNOWN;
	if(nodeId >=0)
	{
		char path[288]={0};
		IMXEC_CALL_DIRECTION nCallDir =IMXEC_CALL_DIR_UNKOWN;
		if(type==1)
		{
			nCallDir =IMXEC_CALL_DIR_IN;
			sprintf(path,"message.setup.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.setup.sourceInfo");
		}
		else if(type==2)
		{
			nCallDir =IMXEC_CALL_DIR_OUT;
			sprintf(path,"message.connect.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.connect.destinationInfo");
		}

		nodeId=pvtGetNodeIdByPath(hVal,nodeId,path);
		if(nodeId >=0)
		{
			if (pvtGetNodeIdByPath( hVal, nodeId, "mcu") > 0)
			{
				emEntityType = EM_ENTITY_MCU;
			} 
			else if(pvtGetNodeIdByPath( hVal, nodeId, "terminal") > 0)
			{
				emEntityType = EM_ENTITY_TERMINAL;
			}
			else
			{
				emEntityType = EM_ENTITY_UNKNOWN;
			}

			CallParamSet callParam;
			memset(&callParam,0,sizeof(callParam));
			callParam.m_nEntityType=emEntityType;
			callParam.m_nCallDir=nCallDir;
			pH323MediaCall->m_nEntityType =emEntityType;
			memcpy(callParam.m_strCallID,pH323MediaCall->GetCallID().c_str(),pH323MediaCall->GetCallID().length());
			memcpy(callParam.m_strUserID,pH323MediaCall->GetUserID().c_str(),pH323MediaCall->GetUserID().length());
			memcpy(callParam.m_strProductID,pH323MediaCall->m_strProductID.c_str(),pH323MediaCall->m_strProductID.length());

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) 
#else 

			callParam.m_nMainVideoBitrate = H323CallMgr::Instance().m_nMainVideoMaxBitrate ;
			callParam.m_nDualVideoBitrate = H323CallMgr::Instance().m_nSecondVideoMaxBitrate ;

#endif

			CallMgr::Instance().OnPeerEntityInfo(&callParam);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	int nFindPro=-1;
	nFindPro=pH323MediaCall->m_strProductID.find("Polycom RMX");
	if (nFindPro>=0) pH323MediaCall->m_nMasterSlaveValue = 50;
#endif

		}
	}

	LogFile(NULL,NULL,"","","GetTerminalType 222\n");

	return emEntityType;
}

//输出结点树信息
void OutputPVTNodeContent(std::string & strOutPutBuf,int nodeId,HPVT hPvt)
{
#if defined(OutputPVTNode)

	if(nodeId >=0)
	{
		HPVT pvtTree =hPvt;
		if(pvtTree ==NULL)
		{
			pvtTree = cmGetValTree(H323CallMgr::Instance().m_hApp);
		}
		struct pvtPrintContext PrintContext(strOutPutBuf);
		pvtPrint(pvtTree,nodeId,printCapToBuffer,&PrintContext);
		OutputDebugString("\n\n-------------------------------");
		OutputDebugString(PrintContext.m_strPrint.c_str());
	}

#endif

}

int AddCallSignalAddressNode(HPVT hVal,int nodeId,std::string & strCallIP,int nPort,int nIndex)
{
	int nRet=-1;
	int error =-1;
	int nodeSub=-1;
	char strTmp[288]={0};
	char addr[288]={0};
	cmTransportAddress CallSignalAddress;
	memset( &CallSignalAddress, 0, sizeof(cmTransportAddress) );
	if(nodeId <0) return nRet;
	int nTempId=pvtGetNodeIdByPath(hVal,nodeId,"callSignalAddress");
	if(nTempId >0) pvtDelete(hVal,nTempId);

	sprintf(addr,"%s:%u",strCallIP.c_str(),nPort);
	error=cmString2TransportAddress(addr,&CallSignalAddress);
	if(error <0) return nRet;
	sprintf(strTmp, "callSignalAddress.%d", nIndex);
	nodeSub=pvtBuildByPath(hVal, nodeId,strTmp, 0, NULL );
	error=cmTransportAddress2Vt(hVal,&CallSignalAddress,nodeSub);
	nRet =1;
	return nRet;
}

int AddTerminalAliasNode(HPVT hVal,int nodeId,int nIndex)
{
	int nRet=-1;
	int len =0;
	int error =-1;
	int nCount=nIndex;
	char strTmp[288]={0};
	char tempbuf[288]={0};
	if(nodeId <0) return nRet;
	//设置注册信息
	int nTempId=pvtGetNodeIdByPath(hVal,nodeId,"terminalAlias");
	if(nTempId >0) pvtDelete(hVal,nTempId);
	if(H323CallMgr::Instance().m_strH323ID.size()>0)
	{
		sprintf(strTmp, "terminalAlias.%d.h323-ID", nCount);
		len = utlChr2Bmp(H323CallMgr::Instance().m_strH323ID.c_str(), (RvUint8*)tempbuf);
		error = pvtBuildByPath(hVal, nodeId, strTmp,  len, tempbuf);
		if(error >=0) nCount++;
	}
	if(H323CallMgr::Instance().m_strTelNumber.size()>0)
	{
		sprintf(strTmp, "terminalAlias.%d.e164", nCount);
		len=H323CallMgr::Instance().m_strTelNumber.size();
		if(len >=256) return nRet;
		strcpy(tempbuf,H323CallMgr::Instance().m_strTelNumber.c_str());
		error = pvtBuildByPath(hVal, nodeId,strTmp,len, tempbuf);
		if(error >=0) nCount++;
	}
	nRet =1;
	return nRet;
}

void CheckAddSignalAddrAliasNode(int messageNodeId)
{
	std::string strContent="";
	//OutputPVTNodeContent(strContent,messageNodeId);

	HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	int nTempNodeId=pvtBuildByPath(hVal, messageNodeId, "registrationRequest", 0, NULL );
	bool bFind=false;
	for(int i=0;i<32;i++)
	{
		int nValue=0;
		RvBool isString =RV_FALSE;
		char strTmp[288]={0};
		sprintf(strTmp, "callSignalAddress.%d.ipAddress.port",i+1);
		int nTempId=pvtGetByPath(hVal,nTempNodeId,strTmp,NULL,&nValue,&isString);
		if(nTempId >=0 && nValue >0)
		{
			bFind=true;
			break;
		}
	}
	if( !bFind )
	{
		int nPort=H323CallMgr::Instance().m_nRegGKPort;
		std::string strLocalIP = GetLocalIPByRemoteIP(H323CallMgr::Instance().m_strRegHost.c_str(),nPort);
		nPort=H323CallMgr::Instance().m_nH323ListenPort;
		AddCallSignalAddressNode(hVal,nTempNodeId,strLocalIP,nPort,1);
	}

	bFind=false;
	for(int i=0;i<32;i++)
	{
		char strTmp[288]={0};
		sprintf(strTmp, "terminalAlias.%d.h323-ID",i+1);
		int nTempId=pvtGetNodeIdByPath(hVal,nTempNodeId,strTmp);
		sprintf(strTmp, "terminalAlias.%d.e164",i+1);
		int nTempId2=pvtGetNodeIdByPath(hVal,nTempNodeId,strTmp);
		if(nTempId >= 0 || nTempId2 >=0)
		{
			bFind=true;
			break;
		}
	}
	if( !bFind )
	{
		AddTerminalAliasNode(hVal,nTempNodeId,1);
	}
}

int RVCALLCONV cmEvCallRecvMessage(HAPPCALL haCall, HCALL hsCall, int message)
{
	int nRet =0;
	H323MediaCall * pH323MediaCall=(H323MediaCall*)haCall;
	HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	//get releaseComplete
	int nNodeID=0;
	nNodeID =pvtGetNodeIdByPath(hVal,message,"message.releaseComplete.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body.releaseComplete");
	if(nNodeID >=0 && pH323MediaCall)
	{
		int nError= pvtGetNodeIdByPath(hVal,nNodeID,"reason.unreachableDestination");
		if(nError >=0) pH323MediaCall->m_nLocalHungUP =2;
		else pH323MediaCall->m_nLocalHungUP =3;
		//std::string strContent="releaseComplete\n";
		//OutputPVTNodeContent(strContent,message);
	}
	return nRet;
}

int SetCallSignalAddress(std::string strCallIP,int nPort,HCALL hsCall,cmCallParam callParam)
{
	int nRet=0;
	cmTransportAddress CallSignalAddress;
	memset( &CallSignalAddress, 0, sizeof(cmTransportAddress) );
	char addr[288]={0};
	sprintf(addr,"%s:%u",strCallIP.c_str(),nPort);
	nRet=cmString2TransportAddress(addr,&CallSignalAddress);
	if(nRet <0) return -1;
	nRet =cmCallSetParam(hsCall, callParam,0, sizeof(cmTransportAddress), (char*)&CallSignalAddress);
	if(nRet <0) return -1;
	return nRet;
}
