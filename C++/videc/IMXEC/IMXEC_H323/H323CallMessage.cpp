#include "stdafx.h"
#include "H323CallMessage.h"
#include "H323Call.h"
#include "H323CallMgr.h"

#include <RVINC/cm.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


H323CallMessage::H323CallMessage(H323Call& rH323Call)
:m_rH323Call(rH323Call)
{
	m_strConfID="1001";
}

H323CallMessage::~H323CallMessage()
{
}




int H323CallMessage::SendCallH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	switch(nMsgType)
	{
	case h245CONcrqTerminalListRequest:
		SendTerminalListRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqSendThisSource:
		SendSendThisSourceRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqMakeMeChair:
		SendMakeMeChairRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqCancelMakeMeChair:
		SendCancelMakeMeChairRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqDropTerminal:
		SendDropTerminalRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqRequestTerminalID:
		SendTerminalIDRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqRequestChairTokenOwner:
		SendChairTokenOwnerRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqMakeTerminalBroadcaster:
		SendMakeTerminalBroadcasterRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqRequestAllTerminalIDs:
		SendAllTerminalIDsRequest(cszPacket,nPacketLen);
		break;
	case h245CONcrqRemoteMCRequest:
		SendRemoteMCRequest(cszPacket,nPacketLen);
		break;

	case h245CONcrsSendThisSourceResponse:
		SendSendThisSourceResponse(cszPacket,nPacketLen);
		break;
	case h245CONcrsTerminalIDResponse:
		SendTerminalIDResponse(cszPacket,nPacketLen);
		break;
	case h245CONcrsTerminalListResponse:
		SendTerminalListResponse(cszPacket,nPacketLen);
		break;
	case h245CONcrsMakeMeChairResponse:
		SendMakeMeChairResponse(cszPacket,nPacketLen);
		break;
	case h245CONcrsChairTokenOwnerResponse:
		SendChairTokenOwnerResponse(cszPacket,nPacketLen);
		break;
	case h245CONcrsMakeTerminalBroadcasterResponse:
		SendMakeTerminalBroadcasterResponse(cszPacket,nPacketLen);
		break;
	case h245CONcrsRequestAllTerminalIDsResponse:
		SendAllTerminalIDsResponse(cszPacket,nPacketLen);
		break;
	case h245CONcrsRemoteMCResponse:
		SendRemoteMCResponse(cszPacket,nPacketLen);
		break;


	case h245CONccSendThisSource:
		SendSendThisSourceCommand(cszPacket,nPacketLen);
		break;
	case h245CONccCancelSendThisSource:
		SendCancelSendThisSourceCommand(cszPacket,nPacketLen);
		break;
	case h245CONccDropConference:
		SendDropConferenceCommand(cszPacket,nPacketLen);
		break;
	case h245CONccMakeTerminalBroadcaster:
		SendMakeTerminalBroadcasterCommand(cszPacket,nPacketLen);
		break;
	case h245CONccCancelMakeTerminalBroadcaster:
		SendCancelMakeTerminalBroadcasterCommand(cszPacket,nPacketLen);
		break;

	case h245CONciTerminalNumberAssign:
		SendTerminalNumberAssignIndication(cszPacket,nPacketLen);
		break;
	case h245CONciTerminalJoinedConference:
		SendTerminalJoinedConferenceIndication(cszPacket,nPacketLen);
		break;
	case h245CONciTerminalLeftConference:
		SendTerminalLeftConferenceIndication(cszPacket,nPacketLen);
		break;
	case h245CONciSeenByAtLeastOneOther:
		SendSeenByAtLeastOneOtherIndication(cszPacket,nPacketLen);
		break;
	case h245CONciCancelSeenByAtLeastOneOther:
		SendCancelSeenByAtLeastOneOtherIndication(cszPacket,nPacketLen);
		break;
	case h245CONciSeenByAll:
		SendSeenByAllIndication(cszPacket,nPacketLen);
		break;
	case h245CONciCancelSeenByAll:
		SendCancelSeenByAllIndication(cszPacket,nPacketLen);
		break;

	case h245CONgrqGenericRequest:
		SendGenericRequestMessage(cszPacket,nPacketLen);
		break;
	case h245CONgrsGenericResponse:
		SendGenericResponseMessage(cszPacket,nPacketLen);
		break;
	case h245CONgcGenericCommand:
		SendGenericCommandMessage(cszPacket,nPacketLen);
		break;
	case h245CONgiGenericIndication:
		SendGenericIndicationMessage(cszPacket,nPacketLen);
		break;

	case h245CONcFlowControlCommand:
		SendFlowControlCommand(cszPacket,nPacketLen);
		break;
	case h245CONiFlowControlIndication:
		SendFlowControlIndication(cszPacket,nPacketLen);
		break;

	case h245CONciRequestForFloor:
		SendRequestForFloorIndication(cszPacket,nPacketLen);
		break;
	case h245CONciFloorRequested:
		SendFloorRequestedIndication(cszPacket,nPacketLen);
		break;
	case h245CONciTerminalYouAreSeeing:
		SendTerminalYouAreSeeingIndication(cszPacket,nPacketLen);
		break;
	case h245CONcVideoFastUpdatePicture:
		SendVideoFastUpdatePicture(cszPacket,nPacketLen);
		break;
	case h245CONcVideoFreezePicture: //Add VCF
		SendVideoFreezePicture(cszPacket,nPacketLen);
		break;
	case h245CONCameraControl:
		SendCameraControl(cszPacket,nPacketLen);
		break;
	case h245CONiLogicalChannelActive:
		SendMediaChannelActive(cszPacket,nPacketLen);
		break;
	case h245CONiLogicalChannelInActive:
		SendMediaChannelInActive(cszPacket,nPacketLen);
		break;

	default:
		break;
	}


	//char buf[1024]={0};
	//sprintf(buf,"\nType:%d,packet:%s,packet len:%d\n",nMsgType,cszPacket,nPacketLen);
	//OutputDebugString(buf);
	return 0;
}


void H323CallMessage::SetBuildPathValue(buildPathValue & pathValue,const char * path,int nValue,char * strValue)
{
	int & i=pathValue.m_nNum;
	strcpy(pathValue.m_BuildPVCaps[i].strPath,path);
	pathValue.m_BuildPVCaps[i].nValue=nValue;
	if(strValue!=NULL)
	{
		strcpy(pathValue.m_BuildPVCaps[i].strValue,strValue);
	}
	if(nValue <= 0) pathValue.m_BuildPVCaps[i].nIsString=0;
	else if(strValue ==NULL) pathValue.m_BuildPVCaps[i].nIsString=0;
	else pathValue.m_BuildPVCaps[i].nIsString=1;

	i++;
}

int H323CallMessage::SendBuildMessage(BuildMessageType nType,buildPathValue & pathValue)
{
	HPVT hVal;
	HPST hPst;
	int rootId;
	int nodeId;
	int error=-1;
	HPROTCONN h245ConHandle;
	int i=0;
	char * path;
	char * strValue;
	int nValue;
	int nIsString;
	char rootPath[128]={0};

	switch(nType)
	{
	case MessageConfReq:
		strcpy(rootPath, "request.conferenceRequest");
		break;
	case MessageConfResp:
		strcpy(rootPath, "response.conferenceResponse");
		break;
	case MessageConfCmd:
		strcpy(rootPath, "command.conferenceCommand");
		break;
	case MessageConfInd:
		strcpy(rootPath, "indication.conferenceIndication");
		break;
	case MessageCommand:
		strcpy(rootPath, "command");
		break;
	case MessageIndication:
		strcpy(rootPath, "indication");
		break;
	default:
		return -1;
	}

	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	hPst = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, hPst, 0, NULL);
	if(rootId>=0)
	{
		nodeId = pvtBuildByPath(hVal, rootId,rootPath, 0, NULL );
		i=0;
		while(i < pathValue.m_nNum)
		{
			path=pathValue.m_BuildPVCaps[i].strPath;
			strValue=pathValue.m_BuildPVCaps[i].strValue;
			nValue=pathValue.m_BuildPVCaps[i].nValue;
			nIsString=pathValue.m_BuildPVCaps[i].nIsString;
			if(nIsString <= 0)  strValue=NULL;
			error = pvtBuildByPath(hVal, nodeId,path,nValue,strValue);
			i++;
		}
		h245ConHandle = cmGetTpktChanHandle((HCALL)(m_rH323Call.m_hsCall), cmH245TpktChannel);
		if(h245ConHandle)
		{
			error=cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId);
		}
		pvtDelete(hVal,rootId);
		return 0;
	}

	pvtDelete(hVal,rootId);
	return -1;
}


void H323CallMessage::SetBuildPath(buildPathValue & pathValue,const char * path)
{
	int & i=pathValue.m_nNum;
	strcpy(pathValue.m_BuildPVCaps[i].strPath,path);
	i++;
}

int H323CallMessage::GetBuildPathValue(int nMessageID,buildPathValue & pathValue)
{
	HPVT hVal;
	int i=0;
	int  lenData;
	unsigned int  bIsString;
	int nodeData=0;
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	int nodeID= pvtGetNodeIdByPath(hVal,nMessageID, "");
	if(pathValue.m_nNum <= 0) return -1;

	while(i < pathValue.m_nNum)
	{
		pathValue.m_BuildPVCaps[i].nValue=0;
		pathValue.m_BuildPVCaps[i].nIsString=0;
		pathValue.m_BuildPVCaps[i].nGetPathValue=0;
		memset(pathValue.m_BuildPVCaps[i].strValue,0,sizeof(pathValue.m_BuildPVCaps[i].strValue) );
		nodeData= pvtGetByPath( hVal, nodeID, pathValue.m_BuildPVCaps[i].strPath, NULL, &lenData, &bIsString );
		if(nodeData>=0)
		{
			pathValue.m_BuildPVCaps[i].nGetPathValue=1;
			if(bIsString && lenData>0 )
			{
				if( pvtGetString( hVal, nodeData, lenData, (char*)pathValue.m_BuildPVCaps[i].strValue) >= 0)
				{
					pathValue.m_BuildPVCaps[i].nIsString=1;
					pathValue.m_BuildPVCaps[i].nValue=lenData;
				}
			}
			else 
			{
				pathValue.m_BuildPVCaps[i].nValue=lenData;
			}
		}
		i++;
	}
	return 0;
}

void H323CallMessage::RecvH245ControlMessage(int message,cmH245MessageType messageType)
{
	switch(messageType)
	{
	case h245crqTerminalListRequest:
		RecvTerminalListRequest(message);
		break;
	case h245crqSendThisSource:
		RecvSendThisSourceRequest(message);
		break;
	case h245crqMakeMeChair:
		RecvMakeMeChairRequest(message);
		break;
	case h245crqCancelMakeMeChair:
		RecvCancelMakeMeChairRequest(message);
		break;
	case h245crqDropTerminal:
		RecvDropTerminalRequest(message);
		break;
	case h245crqRequestTerminalID:
		RecvTerminalIDRequest(message);
		break;
	case h245crqRequestChairTokenOwner:
		RecvChairTokenOwnerRequest(message);
		break;
	case h245crqMakeTerminalBroadcaster:
		RecvMakeTerminalBroadcasterRequest(message);
		break;
	case h245crqRequestAllTerminalIDs:
		RecvAllTerminalIDsRequest(message);
		break;
	case h245crqRemoteMCRequest:
		RecvRemoteMCRequest(message);
		break;

	case h245crsSendThisSourceResponse:
		RecvSendThisSourceResponse(message);
		break;
	case h245crsTerminalIDResponse:
		RecvTerminalIDResponse(message);
		break;
	case h245crsTerminalListResponse:
		RecvTerminalListResponse(message);
		break;
	case h245crsMakeMeChairResponse:
		RecvMakeMeChairResponse(message);
		break;
	case h245crsChairTokenOwnerResponse:
		RecvChairTokenOwnerResponse(message);
		break;
	case h245crsMakeTerminalBroadcasterResponse:
		RecvMakeTerminalBroadcasterResponse(message);
		break;
	case h245crsRequestAllTerminalIDsResponse:
		RecvAllTerminalIDsResponse(message);
		break;
	case h245crsRemoteMCResponse:
		RecvRemoteMCResponse(message);
		break;


	case h245ccSendThisSource:
		RecvSendThisSourceCommand(message);
		break;
	case h245ccCancelSendThisSource:
		RecvCancelSendThisSourceCommand(message);
		break;
	case h245ccDropConference:
		RecvDropConferenceCommand(message);
		break;
	case h245ccMakeTerminalBroadcaster:
		RecvMakeTerminalBroadcasterCommand(message);
		break;
	case h245ccCancelMakeTerminalBroadcaster:
		RecvCancelMakeTerminalBroadcasterCommand(message);
		break;

		//case h245ciTerminalNumberAssign:
		//	break;

	case h245ciTerminalJoinedConference:
		RecvTerminalJoinedConferenceIndication(message);
		break;
	case h245ciTerminalLeftConference:
		RecvTerminalLeftConferenceIndication(message);
		break;
	case h245ciSeenByAtLeastOneOther:
		RecvSeenByAtLeastOneOtherIndication(message);
		break;
	case h245ciCancelSeenByAtLeastOneOther:
		RecvCancelSeenByAtLeastOneOtherIndication(message);
		break;
	case h245ciSeenByAll:
		RecvSeenByAllIndication(message);
		break;
	case h245ciCancelSeenByAll:
		RecvCancelSeenByAllIndication(message);
		break;
	case h245iFlowControlIndication:
		RecvFlowControlIndication(message);
		break;
	case h245CONciRequestForFloor:
		RecvRequestForFloorIndication(message);
		break;
	case h245CONciFloorRequested:
		RecvFloorRequestedIndication(message);
		break;
	case h245CONciTerminalYouAreSeeing:
		RecvTerminalYouAreSeeingIndication(message);
		break;

	default:
		break;
	}
}


void H323CallMessage::OnRecvH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	m_rH323Call.OnRecvH245ControlMessage(nMsgType,cszPacket,nPacketLen);
}

int H323CallMessage::SendSendThisSourceRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"sendThisSource.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"sendThisSource.terminalNumber", strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvSendThisSourceRequest(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"sendThisSource.mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"sendThisSource.terminalNumber"); //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqSendThisSource;
	packet.SetAttribUN2("CMD",1);packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","sendThisSource");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendSendThisSourceResponse(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strGranted=packet.GetAttrib("grantedSendThisSource");
	KString strDenied=packet.GetAttrib("deniedSendThisSource");

	buildPathValue pathValue;
	if(strGranted.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"sendThisSourceResponse.grantedSendThisSource",0);
	else if(strDenied.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"sendThisSourceResponse.deniedSendThisSource",0);
	else
		SetBuildPathValue(pathValue,"sendThisSourceResponse.grantedSendThisSource",0);
	return SendBuildMessage(MessageConfResp,pathValue);
}

void H323CallMessage::RecvSendThisSourceResponse(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"sendThisSourceResponse.grantedSendThisSource");      //pathValue index 0
	SetBuildPath(pathValue,"sendThisSourceResponse.deniedSendThisSource");       //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	std::string strAssent="0";//同意
	std::string strReject="0";//拒绝
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrsSendThisSourceResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","sendThisSourceResponse");
	packet.SetAttrib("CONFID",strConfID);
	if(pathValue.m_BuildPVCaps[0].nGetPathValue >0 ) strAssent="1";    //pathValue index 0
	packet.SetAttrib("grantedSendThisSource",strAssent);    
	if(pathValue.m_BuildPVCaps[1].nGetPathValue >0 ) strReject="1";    //pathValue index 1
	packet.SetAttrib("deniedSendThisSource",strReject);

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendMakeMeChairRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"makeMeChair",0);
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvMakeMeChairRequest(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqMakeMeChair;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeMeChair");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendMakeMeChairResponse(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strGranted=packet.GetAttrib("grantedChairToken");
	KString strDenied=packet.GetAttrib("deniedChairToken");

	buildPathValue pathValue;
	if(strGranted.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"makeMeChairResponse.grantedChairToken",0,NULL);
	else if(strDenied.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"makeMeChairResponse.deniedChairToken",0,NULL);
	else
		SetBuildPathValue(pathValue,"makeMeChairResponse.grantedChairToken",0,NULL);
	return SendBuildMessage(MessageConfResp,pathValue);
}

void H323CallMessage::RecvMakeMeChairResponse(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"makeMeChairResponse.grantedChairToken");      //pathValue index 0
	SetBuildPath(pathValue,"makeMeChairResponse.deniedChairToken");       //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	std::string strAssent="0";//同意
	std::string strReject="0";//拒绝
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrsMakeMeChairResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeMeChairResponse");
	packet.SetAttrib("CONFID",strConfID);
	if(pathValue.m_BuildPVCaps[0].nGetPathValue >0 ) strAssent="1";    //pathValue index 0
	packet.SetAttrib("grantedChairToken",strAssent);    
	if(pathValue.m_BuildPVCaps[1].nGetPathValue >0 ) strReject="1";    //pathValue index 1
	packet.SetAttrib("deniedChairToken",strReject);

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendTerminalListRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"terminalListRequest",0,NULL);
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvTerminalListRequest(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqTerminalListRequest;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalListRequest");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}


int H323CallMessage::GetBuildPathValueSetPacket(int nMessageID,buildPathValue & pathValue,KCmdUserDataPacket &packet,int nHasTermID)
{
	HPVT hVal;
	int i=0;
	int  lenData;
	unsigned int  bIsString;
	int nodeData=0;
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	int nodeID= pvtGetNodeIdByPath(hVal,nMessageID, "");
	char path[1024]={0};
	int index=0;
	char keyName[32]={0};
	while(1)
	{
		i=0;
		while(i < pathValue.m_nNum)
		{
			pathValue.m_BuildPVCaps[i].nValue=0;
			pathValue.m_BuildPVCaps[i].nIsString=0;
			pathValue.m_BuildPVCaps[i].nGetPathValue=0;
			memset(pathValue.m_BuildPVCaps[i].strValue,0,sizeof(pathValue.m_BuildPVCaps[i].strValue) );

			sprintf(path,pathValue.m_BuildPVCaps[i].strPath,index+1);
			nodeData = pvtGetByPath( hVal, nodeID,path, NULL, &lenData, &bIsString );
			if(nodeData>=0)
			{
				pathValue.m_BuildPVCaps[i].nGetPathValue=1;
				if(bIsString && lenData>0 )
				{
					if( pvtGetString( hVal, nodeData, lenData, (char*)pathValue.m_BuildPVCaps[i].strValue) >= 0)
					{
						pathValue.m_BuildPVCaps[i].nIsString=1;
						pathValue.m_BuildPVCaps[i].nValue=lenData;
					}
				}
				else 
				{
					pathValue.m_BuildPVCaps[i].nValue=lenData;
				}
			}
			else
			{
				break;
			}
			i++;
		}
		if(nodeData < 0) break;

		{
			KCmdUserDataPacket packetSub ;
			packet.SetAttribUN2("CMD",1);
			packetSub.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);
			packetSub.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);
			if(nHasTermID > 0 ) packetSub.SetAttrib("terminalID",pathValue.m_BuildPVCaps[2].strValue);
			std::string ListPacket1=packetSub.GetString();
			sprintf(keyName,"ListPacket%d",index+1);
			if(nHasTermID > 0 )
				sprintf(keyName,"IDPacket%d",index+1);
			packet.SetAttrib(keyName,ListPacket1.c_str());
		}

		index++;
	}
	return index;
}

int H323CallMessage::SendTerminalListResponse(const char * cszPacket,int nPacketLen)
{

	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strListNum=packet.GetAttrib("ListNum");

	int nCount=strListNum.AsUnsignedInt();
	int j=1;
	char strName[64]={0};
	char strPath[128]={0};
	if(nCount>0 && nCount<2100000)
	{
		buildPathValue pathValue;
		while(j<=nCount)
		{
			sprintf(strName,"ListPacket%d",j);
			KString strListPacket=packet.GetAttrib(strName);
			int nLen=strListPacket.AsString().length()+1;
			if(nLen <= 0) break;
			KCmdPacket ListPacket(strListPacket.AsString().c_str(),nLen);
			KString strMcuNum=ListPacket.GetAttrib("mcuNumber");
			KString strTermNum=ListPacket.GetAttrib("terminalNumber");

			sprintf(strPath,"terminalListResponse.%d.mcuNumber",j);
			SetBuildPathValue(pathValue,strPath,strMcuNum.AsUnsignedInt());
			sprintf(strPath,"terminalListResponse.%d.terminalNumber",j);
			SetBuildPathValue(pathValue,strPath,strTermNum.AsUnsignedInt());
			j++;
		}
		return SendBuildMessage(MessageConfResp,pathValue);
	}

	return 0;
}

void H323CallMessage::RecvTerminalListResponse(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"terminalListResponse.%d.mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalListResponse.%d.terminalNumber");       //pathValue index 1

	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrsTerminalListResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalListResponse");
	packet.SetAttrib("CONFID",strConfID);

	//get path value
	int num=0;
	num=GetBuildPathValueSetPacket(message,pathValue,packet,0);
	packet.SetAttribUN2("ListNum",num);

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendCancelMakeMeChairRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"cancelMakeMeChair",0,NULL);
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvCancelMakeMeChairRequest(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqCancelMakeMeChair;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelMakeMeChair");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendDropTerminalRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"dropTerminal.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"dropTerminal.terminalNumber", strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvDropTerminalRequest(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"dropTerminal.mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"dropTerminal.terminalNumber"); //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqSendThisSource;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","dropTerminal");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendTerminalIDRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"requestTerminalID.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"requestTerminalID.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvTerminalIDRequest(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"requestTerminalID.mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"requestTerminalID.terminalNumber"); //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqRequestTerminalID;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestTerminalID");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendTerminalIDResponse(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");
	KString strTermID=packet.GetAttrib("terminalID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"terminalIDResponse.terminalLabel.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"terminalIDResponse.terminalLabel.terminalNumber", strTermNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"terminalIDResponse.terminalID",strTermID.AsString().length(),(char *)strTermID.AsString().c_str());
	return SendBuildMessage(MessageConfResp,pathValue);
}

void H323CallMessage::RecvTerminalIDResponse(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"terminalIDResponse.terminalLabel.mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalIDResponse.terminalLabel.terminalNumber"); //pathValue index 1
	SetBuildPath(pathValue,"terminalIDResponse.terminalID");                   //pathValue index 2
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrsTerminalIDResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalIDResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	packet.SetAttrib("terminalID",pathValue.m_BuildPVCaps[2].strValue);       //pathValue index 2
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendChairTokenOwnerRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"requestChairTokenOwner",0,NULL);
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvChairTokenOwnerRequest(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqRequestChairTokenOwner;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestChairTokenOwner");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendChairTokenOwnerResponse(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");
	KString strTermID=packet.GetAttrib("terminalID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"chairTokenOwnerResponse.terminalLabel.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"chairTokenOwnerResponse.terminalLabel.terminalNumber", strTermNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"chairTokenOwnerResponse.terminalID",strTermID.AsString().length(),(char *)strTermID.AsString().c_str());
	return SendBuildMessage(MessageConfResp,pathValue);
}

void H323CallMessage::RecvChairTokenOwnerResponse(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"chairTokenOwnerResponse.terminalLabel.mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"chairTokenOwnerResponse.terminalLabel.terminalNumber"); //pathValue index 1
	SetBuildPath(pathValue,"chairTokenOwnerResponse.terminalID");                   //pathValue index 2
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrsChairTokenOwnerResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","chairTokenOwnerResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	packet.SetAttrib("terminalID",pathValue.m_BuildPVCaps[2].strValue);       //pathValue index 2
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendMakeTerminalBroadcasterRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"makeTerminalBroadcaster.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"makeTerminalBroadcaster.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvMakeTerminalBroadcasterRequest(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"makeTerminalBroadcaster.mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"makeTerminalBroadcaster.terminalNumber"); //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqMakeTerminalBroadcaster;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeTerminalBroadcaster");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendMakeTerminalBroadcasterResponse(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strGranted=packet.GetAttrib("grantedMakeTerminalBroadcaster");
	KString strDenied=packet.GetAttrib("deniedMakeTerminalBroadcaster");

	buildPathValue pathValue;
	if(strGranted.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"makeTerminalBroadcasterResponse.grantedMakeTerminalBroadcaster",0,NULL);
	else if(strDenied.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"makeTerminalBroadcasterResponse.deniedMakeTerminalBroadcaster",0,NULL);
	else
		SetBuildPathValue(pathValue,"makeTerminalBroadcasterResponse.grantedMakeTerminalBroadcaster",0,NULL);
	return SendBuildMessage(MessageConfResp,pathValue);
}

void H323CallMessage::RecvMakeTerminalBroadcasterResponse(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"makeTerminalBroadcasterResponse.grantedMakeTerminalBroadcaster");      //pathValue index 0
	SetBuildPath(pathValue,"makeTerminalBroadcasterResponse.deniedMakeTerminalBroadcaster");       //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	std::string strAssent="0";//同意
	std::string strReject="0";//拒绝
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrsMakeTerminalBroadcasterResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeTerminalBroadcasterResponse");
	packet.SetAttrib("CONFID",strConfID);
	if(pathValue.m_BuildPVCaps[0].nGetPathValue >0 ) strAssent="1";    //pathValue index 0
	packet.SetAttrib("grantedMakeTerminalBroadcaster",strAssent);    
	if(pathValue.m_BuildPVCaps[1].nGetPathValue >0 ) strReject="1";    //pathValue index 1
	packet.SetAttrib("deniedMakeTerminalBroadcaster",strReject);

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendAllTerminalIDsRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"requestAllTerminalIDs",0,NULL);
	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvAllTerminalIDsRequest(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqRequestAllTerminalIDs;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestAllTerminalIDs");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendAllTerminalIDsResponse(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strIDNum=packet.GetAttrib("IDNum");

	int nCount=strIDNum.AsUnsignedInt();
	int j=1;
	char strName[64]={0};
	char strPath[128]={0};
	if(nCount>0 && nCount<2100000)
	{
		buildPathValue pathValue;
		while(j<=nCount)
		{
			sprintf(strName,"IDPacket%d",j);
			KString strIDPacket=packet.GetAttrib(strName);
			int nLen=strIDPacket.AsString().length()+1;
			if(nLen <= 0) break;

			KCmdPacket IDPacket(strIDPacket.AsString().c_str(),nLen);
			KString strMcuNum=IDPacket.GetAttrib("mcuNumber");
			KString strTermNum=IDPacket.GetAttrib("terminalNumber");
			KString strTermID=IDPacket.GetAttrib("terminalID");

			sprintf(strPath,"requestAllTerminalIDsResponse.terminalInformation.%d.terminalLabel.mcuNumber",j);
			SetBuildPathValue(pathValue,strPath,strMcuNum.AsUnsignedInt());
			sprintf(strPath,"requestAllTerminalIDsResponse.terminalInformation.%d.terminalLabel.terminalNumber",j);
			SetBuildPathValue(pathValue,strPath,strTermNum.AsUnsignedInt());
			sprintf(strPath,"requestAllTerminalIDsResponse.terminalInformation.%d.terminalID",j);
			SetBuildPathValue(pathValue,strPath,strTermID.AsString().length(),(char *)strTermID.AsString().c_str());
			j++;
		}
		return SendBuildMessage(MessageConfResp,pathValue);
	}

	return 0;
}

void H323CallMessage::RecvAllTerminalIDsResponse(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"requestAllTerminalIDsResponse.terminalInformation.%d.terminalLabel.mcuNumber");
	SetBuildPath(pathValue,"requestAllTerminalIDsResponse.terminalInformation.%d.terminalLabel.terminalNumber");
	SetBuildPath(pathValue,"requestAllTerminalIDsResponse.terminalInformation.%d.terminalID");

	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrsRequestAllTerminalIDsResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestAllTerminalIDsResponse");
	packet.SetAttrib("CONFID",strConfID);

	//get path value
	int num=0;
	num=GetBuildPathValueSetPacket(message,pathValue,packet,1);
	packet.SetAttribUN2("IDNum",num);

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendRemoteMCRequest(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMaster=packet.GetAttrib("masterActivate");
	KString strSlave=packet.GetAttrib("slaveActivate");
	KString strDeActivate=packet.GetAttrib("deActivate");

	buildPathValue pathValue;
	if(strMaster.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"remoteMCRequest.masterActivate",0,NULL);
	else if(strSlave.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"remoteMCRequest.slaveActivate",0,NULL);
	else if(strDeActivate.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"remoteMCRequest.deActivate",0,NULL);
	else 
		SetBuildPathValue(pathValue,"remoteMCRequest.slaveActivate",0,NULL);

	return SendBuildMessage(MessageConfReq,pathValue);
}

void H323CallMessage::RecvRemoteMCRequest(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"remoteMCRequest.masterActivate");      //pathValue index 0
	SetBuildPath(pathValue,"remoteMCRequest.slaveActivate");       //pathValue index 1
	SetBuildPath(pathValue,"remoteMCRequest.deActivate");          //pathValue index 2

	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	std::string strAssent="0";//同意
	std::string strReject="0";//拒绝
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrqRemoteMCRequest;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","remoteMCRequest");
	packet.SetAttrib("CONFID",strConfID);
	if(pathValue.m_BuildPVCaps[0].nGetPathValue >0 ) strAssent="1";    //pathValue index 0
	packet.SetAttrib("masterActivate",strAssent);    
	if(pathValue.m_BuildPVCaps[1].nGetPathValue >0 ) strReject="1";    //pathValue index 1
	packet.SetAttrib("slaveActivate",strReject);
	strReject="0";
	if(pathValue.m_BuildPVCaps[2].nGetPathValue >0 ) strReject="1";    //pathValue index 2
	packet.SetAttrib("deActivate",strReject);

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendRemoteMCResponse(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strAccept=packet.GetAttrib("accept");
	KString strReject=packet.GetAttrib("reject");
	KString strUnspecified=packet.GetAttrib("unspecified");
	KString strFunctionNotSupported=packet.GetAttrib("functionNotSupported");

	buildPathValue pathValue;
	if(strAccept.AsUnsignedInt() > 0)
		SetBuildPathValue(pathValue,"remoteMCResponse.accept",0,NULL);
	else if(strReject.AsUnsignedInt() > 0)
	{
		if(strUnspecified.AsUnsignedInt() > 0)
			SetBuildPathValue(pathValue,"remoteMCResponse.reject.unspecified",0,NULL);
		else if(strFunctionNotSupported.AsUnsignedInt() > 0)
			SetBuildPathValue(pathValue,"remoteMCResponse.reject.functionNotSupported",0,NULL);
		else
			SetBuildPathValue(pathValue,"remoteMCResponse.reject.unspecified",0,NULL);
	}
	else 
		SetBuildPathValue(pathValue,"remoteMCResponse.reject.unspecified",0,NULL);

	return SendBuildMessage(MessageConfResp,pathValue);
}


void H323CallMessage::RecvRemoteMCResponse(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"remoteMCResponse.accept");                            //pathValue index 0
	SetBuildPath(pathValue,"remoteMCResponse.reject.functionNotSupported");       //pathValue index 1
	SetBuildPath(pathValue,"remoteMCResponse.reject.unspecified");                //pathValue index 2

	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	std::string strAssent="0";//同意
	std::string strReject="0";//拒绝
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcrsRemoteMCResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","remoteMCResponse");
	packet.SetAttrib("CONFID",strConfID);
	if(pathValue.m_BuildPVCaps[0].nGetPathValue >0 ) strAssent="1";    //pathValue index 0
	packet.SetAttrib("accept",strAssent);  

	if(pathValue.m_BuildPVCaps[1].nGetPathValue >0 )               /*pathValue index 2*/
	{
		packet.SetAttrib("reject","1");
		packet.SetAttrib("functionNotSupported","1");
	}
	else 
	{
		packet.SetAttrib("reject","0");
		packet.SetAttrib("functionNotSupported","0");
	}
	if(pathValue.m_BuildPVCaps[2].nGetPathValue >0 )               /*pathValue index 2*/
	{
		packet.SetAttrib("reject","1");
		packet.SetAttrib("unspecified","1");
	}
	else 
	{
		packet.SetAttrib("reject","0");
		packet.SetAttrib("unspecified","0");
	}

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}


//发送广播终端命令 makeTerminalBroadcaster
int H323CallMessage::SendMakeTerminalBroadcasterCommand(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"makeTerminalBroadcaster.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"makeTerminalBroadcaster.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfCmd,pathValue);
}

void H323CallMessage::RecvMakeTerminalBroadcasterCommand(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalNumber"); //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONccMakeTerminalBroadcaster;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeTerminalBroadcaster");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送取消广播终端命令 cancelMakeTerminalBroadcaster
int H323CallMessage::SendCancelMakeTerminalBroadcasterCommand(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"cancelMakeTerminalBroadcaster",0);
	return SendBuildMessage(MessageConfCmd,pathValue);
}

void H323CallMessage::RecvCancelMakeTerminalBroadcasterCommand(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONccCancelMakeTerminalBroadcaster;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelMakeTerminalBroadcaster");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送切换源或设为发言人命令消息 sendThisSource
int H323CallMessage::SendSendThisSourceCommand(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"sendThisSource.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"sendThisSource.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfCmd,pathValue);
}

void H323CallMessage::RecvSendThisSourceCommand(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalNumber"); //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONccSendThisSource;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","sendThisSource");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送取消切换源或取消发言人命令消息 cancelSendThisSource
int H323CallMessage::SendCancelSendThisSourceCommand(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"cancelSendThisSource",0);
	return SendBuildMessage(MessageConfCmd,pathValue);
}

void H323CallMessage::RecvCancelSendThisSourceCommand(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONccCancelSendThisSource;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelSendThisSource");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送取消切换源或取消发言人命令消息 dropConference
int H323CallMessage::SendDropConferenceCommand(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"dropConference",0);
	return SendBuildMessage(MessageConfCmd,pathValue);
}

void H323CallMessage::RecvDropConferenceCommand(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONccDropConference;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","dropConference");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送终端号码分配指示消息 terminalNumberAssign
int H323CallMessage::SendTerminalNumberAssignIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"terminalNumberAssign.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"terminalNumberAssign.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvTerminalNumberAssignIndication(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"terminalNumberAssign.mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalNumberAssign.terminalNumber"); //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciTerminalNumberAssign;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalNumberAssign");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

void H323CallMessage::RecvTerminalNumberAssign(int mcuNumber, int terminalNumber)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciTerminalNumberAssign;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalNumberAssign");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",mcuNumber);
	packet.SetAttribUN2("terminalNumber",terminalNumber);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送终端加入会议指示消息 terminalJoinedConference
int H323CallMessage::SendTerminalJoinedConferenceIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"terminalJoinedConference.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"terminalJoinedConference.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvTerminalJoinedConferenceIndication(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalNumber"); //pathValue index 1
	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciTerminalJoinedConference;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalJoinedConference");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送终端离开会议指示消息 terminalLeftConference
int H323CallMessage::SendTerminalLeftConferenceIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"terminalLeftConference.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"terminalLeftConference.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvTerminalLeftConferenceIndication(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalNumber"); //pathValue index 1

	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciTerminalLeftConference;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalLeftConference");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送至少选看一个其他视频指示消息 seenByAtLeastOneOther
int H323CallMessage::SendSeenByAtLeastOneOtherIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"seenByAtLeastOneOther",0);
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvSeenByAtLeastOneOtherIndication(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciSeenByAtLeastOneOther;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","seenByAtLeastOneOther");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送取消至少选看一个其他视频指示消息 cancelSeenByAtLeastOneOther
int H323CallMessage::SendCancelSeenByAtLeastOneOtherIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"cancelSeenByAtLeastOneOther",0);
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvCancelSeenByAtLeastOneOtherIndication(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciCancelSeenByAtLeastOneOther;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelSeenByAtLeastOneOther");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送选看所有视频指示消息 seenByAll
int H323CallMessage::SendSeenByAllIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"seenByAll",0);
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvSeenByAllIndication(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciSeenByAll;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","seenByAll");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送取消选看所有视频指示消息 cancelSeenByAll
int H323CallMessage::SendCancelSeenByAllIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");

	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"cancelSeenByAll",0);
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvCancelSeenByAllIndication(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciCancelSeenByAll;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelSeenByAll");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int H323CallMessage::SendRequestForFloorIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"requestForFloor",0);
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvRequestForFloorIndication(int message)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciRequestForFloor;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestForFloor");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int	H323CallMessage::SendFloorRequestedIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");
	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"floorRequested.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"floorRequested.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvFloorRequestedIndication(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalNumber"); //pathValue index 1

	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciFloorRequested;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","floorRequested");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int	H323CallMessage::SendTerminalYouAreSeeingIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	KString strMcuNum=packet.GetAttrib("mcuNumber");
	KString strTermNum=packet.GetAttrib("terminalNumber");
	buildPathValue pathValue;
	SetBuildPathValue(pathValue,"terminalYouAreSeeing.mcuNumber",strMcuNum.AsUnsignedInt());
	SetBuildPathValue(pathValue,"terminalYouAreSeeing.terminalNumber",strTermNum.AsUnsignedInt());
	return SendBuildMessage(MessageConfInd,pathValue);
}

void H323CallMessage::RecvTerminalYouAreSeeingIndication(int message)
{
	//set path 
	buildPathValue pathValue;
	SetBuildPath(pathValue,"mcuNumber");      //pathValue index 0
	SetBuildPath(pathValue,"terminalNumber"); //pathValue index 1

	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONciTerminalYouAreSeeing;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalYouAreSeeing");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("mcuNumber",pathValue.m_BuildPVCaps[0].nValue);        //pathValue index 0
	packet.SetAttribUN2("terminalNumber",pathValue.m_BuildPVCaps[1].nValue);   //pathValue index 1
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int	H323CallMessage::SendVideoFastUpdatePicture(const char * cszPacket,int nPacketLen)
{
	int ret=-1;
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	std::string strMedia=packet.GetAttrib("MediaName").AsString();
	//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	if(strMedia.compare("VIDEO") == 0) ret=m_rH323Call.RequestMainVideoKeyFrame();
	if(strMedia.compare("DUALVIDEO") == 0) ret=m_rH323Call.RequestSecondVideoKeyFrame();
	return ret;
}

int	H323CallMessage::SendVideoFreezePicture(const char * cszPacket,int nPacketLen)
{
	int ret=-1;
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	std::string strMedia=packet.GetAttrib("MediaName").AsString();
	//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	if(strMedia.compare("VIDEO") == 0) ret=m_rH323Call.RequestMainVideoFreeze();
	if(strMedia.compare("DUALVIDEO") == 0) ret=m_rH323Call.RequestSecondVideoFreeze();
	return ret;
}

void H323CallMessage::RecvVideoFastUpdatePicture(std::string strMedia)
{
	//set packet
	std::string strConfID =m_strConfID;
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcVideoFastUpdatePicture;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","videoFastUpdatePicture");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("MediaName",strMedia);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

void H323CallMessage::RecvVideoFreezePicture(std::string strMedia)
{
	//set packet
	std::string strConfID =m_strConfID;
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcVideoFreezePicture;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","videoFreezePicture");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("MediaName",strMedia);
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

//发送逻辑通道流控消息 flowControlCommand
int H323CallMessage::SendFlowControlCommand(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	std::string strMedia=packet.GetAttrib("logicalChannel").AsString();
	int nResourceID=packet.GetAttrib("resourceID").AsUnsignedInt();
	int nMaximumBitRate=packet.GetAttrib("maximumBitRate").AsUnsignedInt();
	nMaximumBitRate =nMaximumBitRate * 10 ; //-- units 100 bit/s --
	//get from strMedia
	int nLogicalNum=0;
	nLogicalNum=m_rH323Call.GetH323ChannelLogicalNumber(1,strMedia);

	buildPathValue pathValue;

	if(nLogicalNum > 0)
		SetBuildPathValue(pathValue,"flowControlCommand.scope.logicalChannelNumber",nLogicalNum);
	else if(nResourceID >0)
		SetBuildPathValue(pathValue,"flowControlCommand.scope.resourceID",nResourceID);
	else 
		SetBuildPathValue(pathValue,"flowControlCommand.scope.resourceID",0);

	if(nMaximumBitRate >= 0)
		SetBuildPathValue(pathValue,"flowControlCommand.restriction.maximumBitRate",nMaximumBitRate);
	else 
		SetBuildPathValue(pathValue,"flowControlCommand.restriction.noRestriction",0);

	return SendBuildMessage(MessageCommand,pathValue);
}

//发送逻辑通道流控消息 flowControlIndication
int H323CallMessage::SendFlowControlIndication(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	std::string strMedia=packet.GetAttrib("logicalChannel").AsString();
	int nResourceID=packet.GetAttrib("resourceID").AsUnsignedInt();
	int nMaximumBitRate=packet.GetAttrib("maximumBitRate").AsUnsignedInt();
	nMaximumBitRate =nMaximumBitRate * 10 ; //-- units 100 bit/s --

	//get from strMedia
	int nLogicalNum=0;
	nLogicalNum=m_rH323Call.GetH323ChannelLogicalNumber(1,strMedia);

	buildPathValue pathValue;

	if(nLogicalNum > 0)
		SetBuildPathValue(pathValue,"flowControlIndication.scope.logicalChannelNumber",nLogicalNum);
	else if(nResourceID >0)
		SetBuildPathValue(pathValue,"flowControlIndication.scope.resourceID",nResourceID);
	else 
		SetBuildPathValue(pathValue,"flowControlIndication.scope.resourceID",0);

	if(nMaximumBitRate >= 0)
		SetBuildPathValue(pathValue,"flowControlIndication.restriction.maximumBitRate",nMaximumBitRate);
	else 
		SetBuildPathValue(pathValue,"flowControlIndication.restriction.noRestriction",0);

	return SendBuildMessage(MessageIndication,pathValue);
}

void H323CallMessage::RecvFlowControlIndication(int message)
{
	//set path 
	buildPathValue pathValue;
	//SetBuildPath(pathValue,"flowControlIndication.scope.logicalChannelNumber");      //pathValue index 0
	//SetBuildPath(pathValue,"flowControlIndication.scope.resourceID");                //pathValue index 1
	//SetBuildPath(pathValue,"flowControlIndication.restriction.maximumBitRate");      //pathValue index 2

	SetBuildPath(pathValue,"scope.logicalChannelNumber");      //pathValue index 0
	SetBuildPath(pathValue,"scope.resourceID");                //pathValue index 1
	SetBuildPath(pathValue,"restriction.maximumBitRate");      //pathValue index 2

	//get path value
	GetBuildPathValue(message,pathValue);
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONiFlowControlIndication;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","flowControlIndication");
	packet.SetAttrib("CONFID",strConfID);
	std::string strMedia="NOMEDIA";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	m_rH323Call.GetH323ChannelLogicalNumMediaName(0,pathValue.m_BuildPVCaps[0].nValue,strMedia);
	if(pathValue.m_BuildPVCaps[0].nValue > 0)
		packet.SetAttrib("logicalChannel",strMedia);
	else 
		packet.SetAttribUN2("resourceID",pathValue.m_BuildPVCaps[1].nValue );

	packet.SetAttribUN2("maximumBitRate",pathValue.m_BuildPVCaps[2].nValue );
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

void H323CallMessage::RecvGenericMessage(cmH245MessageType messageType,int nBitrate, int terLabel, int chID, int symBreaking,int response,int respValue)
{
	std::string strConfID=m_strConfID;
	H245ControlMessageType nMsgType=h245CONgiGenericIndication;
	std::string strH245CMD="genericIndication";
	switch(messageType)
	{
	case h245grqGenericRequest:
		nMsgType=h245CONgrqGenericRequest;
		strH245CMD="genericRequest";
		break;
	case h245grsGenericResponse:
		nMsgType=h245CONgrsGenericResponse;
		strH245CMD="genericResponse";
		break;
	case h245gcGenericCommand:
		nMsgType=h245CONgcGenericCommand;
		strH245CMD="genericCommand";
		break;
	case h245giGenericIndication:
		nMsgType=h245CONgiGenericIndication;
		strH245CMD="genericIndication";
		break;
	default:
		break;
	}

	//set packet
	KCmdUserDataPacket packet;
	packet.SetAttribUN2("CMD",1);
	packet.SetAttrib("H245CMD",strH245CMD);
	packet.SetAttrib("CONFID",strConfID);

	packet.SetAttribUN2("BitRate",nBitrate);
	packet.SetAttribUN2("ChannelId",chID);
	packet.SetAttribUN2("SymBreak",symBreaking);
	packet.SetAttribUN2("TerminalLabel",terLabel);
	if(response==126)
		packet.SetAttrib("Response","2");
	else if(response==127)
		packet.SetAttrib("Response","1");
	else 
		packet.SetAttrib("Response","0");
	packet.SetAttribUN2("RespValue",respValue);

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);

}

void H323CallMessage::RecvFlowControlCommand(std::string strMediaName,unsigned int rate)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONcFlowControlCommand;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","flowControlCommand");
	packet.SetAttrib("CONFID",strConfID);

	packet.SetAttrib("logicalChannel",strMediaName);
	packet.SetAttribUN2("maximumBitRate",rate);

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

int	H323CallMessage::SendCameraControl(const char * cszPacket,int nPacketLen)
{
	int ret=-1;
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	std::string strControl=packet.GetAttrib("Control").AsString();
	int nControlValue=packet.GetAttrib("ControlValue").AsUnsignedInt();
	ret=m_rH323Call.SendCameraControl(strControl,nControlValue);
	return ret;
}

void H323CallMessage::RecvRemoteCameraControl(std::string strAction,int nValue)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONCameraControl;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","MediaCameraControl");
	packet.SetAttrib("CONFID",strConfID);

	packet.SetAttrib("Control",strAction);
	packet.SetAttribUN2("ControlValue",nValue);//nValue 视频源ID或预置点

	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}


void H323CallMessage::SendGenericRequestMessage(const char * cszPacket,int nPacketLen)
{
	KCmdPacket packet(cszPacket,nPacketLen);
	int nSymBreking=packet.GetAttrib("SymBreak").AsUnsignedInt();
	if(nSymBreking >0) nSymBreking=128;
	m_rH323Call.SendGenericRequest(nSymBreking);
}

void H323CallMessage::SendGenericResponseMessage(const char * cszPacket,int nPacketLen)
{
	bool bAccept=false;
	KCmdPacket packet(cszPacket,nPacketLen);
	int nValue=packet.GetAttrib("Response").AsUnsignedInt();
	if(nValue==2) bAccept=true;
	m_rH323Call.SendGenericResponse(bAccept);
}

void H323CallMessage::SendGenericCommandMessage(const char * cszPacket,int nPacketLen)
{
	m_rH323Call.SendGenericCommand();
}

void H323CallMessage::SendGenericIndicationMessage(const char * cszPacket,int nPacketLen)
{
	m_rH323Call.SendGenericIndication();
}

int H323CallMessage::SendMediaChannelActive(const char * cszPacket,int nPacketLen)
{
	int ret=-1;
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	std::string strMedia=packet.GetAttrib("MediaName").AsString();
	ret = m_rH323Call.SendMediaChannelActive(strMedia);
	return ret;
}

int H323CallMessage::SendMediaChannelInActive(const char * cszPacket,int nPacketLen)
{
	int ret=-1;
	KCmdPacket packet(cszPacket,nPacketLen);
	KString strConfID=packet.GetAttrib("CONFID");
	std::string strMedia=packet.GetAttrib("MediaName").AsString();
	ret = m_rH323Call.SendMediaChannelInActive(strMedia);
	return ret;
}

void H323CallMessage::MediaChannelActiveStateMessage(std::string strMediaName)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONiLogicalChannelActive;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","logicalChannelActive");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("MediaName",strMediaName);
	packet.SetAttrib("state","Active");
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

void H323CallMessage::MediaChannelInActiveStateMessage(std::string strMediaName)
{
	std::string strConfID =m_strConfID;
	//set packet
	KCmdUserDataPacket packet;
	H245ControlMessageType nMsgType=h245CONiLogicalChannelInActive;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","logicalChannelInactive");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("MediaName",strMediaName);
	packet.SetAttrib("state","InActive");
	std::string strPacket=packet.GetString();
	//callback
	OnRecvH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
}

void  H323CallMessage::SendH245Message()
{

	//H245会控信令字段设置定义
	//mcuNumber      mcu标记值 取值为整数 0-192
	//terminalNumber 终端标记值 取值为整数 0-192
	//terminalID     终端ID值 取值为字符串 长度为1--128
	//CONFID         会议ID 取值为字符串 长度为1--32

	std::string strMcuNum1="12";               //整数 0-192
	std::string strTermNum1="65";              //整数 0-192
	std::string strMcuNum2="22";               //整数 0-192
	std::string strTermNum2="26";              //整数 0-192
	std::string strTermID="1";                 //字符串 长度为1--128
	int nListCount = 2;
	int nIDCount = 2;
	std::string strTermID1="1";                //字符串 长度为1--128
	std::string strTermID2="2";                //字符串 长度为1--128
	std::string strValue="0";
	std::string strValue2="1";
	std::string strConfID="1001";			   //字符串 长度为1--32
	std::string strMcuNum="12";                //整数 0-192
	std::string strTermNum="65";               //整数 0-192
	H245ControlMessageType nMsgType;
	char * cszPacket;
	int nPacketLen;

	//发送切换源或视频选看请求消息 crq
	{
	KCmdUserDataPacket packet;
	nMsgType=h245CONcrqSendThisSource;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","sendThisSource");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();
	SendCallH245ControlMessage(nMsgType,strPacket.c_str(),strPacket.length()+1);
	}


	//发送切换源或视频选看响应消息 crs
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrsSendThisSourceResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","sendThisSourceResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("grantedSendThisSource",strValue);   //同意设为1否则设为0
	packet.SetAttrib("deniedSendThisSource",strValue2);    //拒绝设为1否则设为0
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送申请主席请求消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqMakeMeChair;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeMeChair");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送申请主席响应消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrsMakeMeChairResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeMeChairResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("grantedChairToken",strValue);   //同意设为1否则设为0
	packet.SetAttrib("deniedChairToken",strValue2);    //拒绝设为1否则设为0
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送终端列表请求消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqTerminalListRequest;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalListRequest");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送终端列表响应消息 复合信令设置
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrsTerminalListResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalListResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("ListNum",nListCount);//列表项数量
	{
		KCmdUserDataPacket packetSub ;
		packetSub.SetAttrib("mcuNumber",strMcuNum1);
		packetSub.SetAttrib("terminalNumber",strTermNum1);
		std::string ListPacket1=packetSub.GetString();
		packet.SetAttrib("ListPacket1",ListPacket1.c_str());
	}
	{
		KCmdUserDataPacket packetSub ;
		packetSub.SetAttrib("mcuNumber",strMcuNum2);
		packetSub.SetAttrib("terminalNumber",strTermNum2);
		std::string ListPacket2=packetSub.GetString();
		packet.SetAttrib("ListPacket2",ListPacket2.c_str());
	}
	//........
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}





	//发送取消申请主席请求消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqCancelMakeMeChair;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelMakeMeChair");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送终端离会请求消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqDropTerminal;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","dropTerminal");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送终端ID请求消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqRequestTerminalID;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestTerminalID");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送终端ID响应消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrsTerminalIDResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalIDResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	packet.SetAttrib("terminalID",strTermID);        //字符串 长度为1--128
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送请求主席令牌消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqRequestChairTokenOwner;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestChairTokenOwner");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送主席令牌响应消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrsChairTokenOwnerResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","chairTokenOwnerResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	packet.SetAttrib("terminalID",strTermID);        //字符串 长度为1--128
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送请求广播终端会场消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqMakeTerminalBroadcaster;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeTerminalBroadcaster");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送广播终端会场响应消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrsMakeTerminalBroadcasterResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeTerminalBroadcasterResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("grantedMakeTerminalBroadcaster",strValue);   //同意设为1否则设为0
	packet.SetAttrib("deniedMakeTerminalBroadcaster",strValue2);    //拒绝设为1否则设为0
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送请求所有终端ID消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqRequestAllTerminalIDs;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestAllTerminalIDs");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送所有终端ID响应消息 复合信令
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrsRequestAllTerminalIDsResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestAllTerminalIDsResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttribUN2("IDNum",nIDCount);//终端ID项数量
	{
		KCmdUserDataPacket packetSub ;
		packetSub.SetAttrib("mcuNumber",strMcuNum1);
		packetSub.SetAttrib("terminalNumber",strTermNum1);
		packetSub.SetAttrib("terminalNumber",strTermNum1);
		packetSub.SetAttrib("terminalID",strTermID1);
		std::string IDPacket1=packetSub.GetString();
		packet.SetAttrib("IDPacket1",IDPacket1.c_str());
	}
	{
		KCmdUserDataPacket packetSub ;
		packetSub.SetAttrib("mcuNumber",strMcuNum2);
		packetSub.SetAttrib("terminalNumber",strTermNum2);
		packetSub.SetAttrib("terminalID",strTermID2);
		std::string IDPacket2=packetSub.GetString();
		packet.SetAttrib("IDPacket2",IDPacket2.c_str());
	}
	//........
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送请求远端MC设置消息 MCU级联用
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrqRemoteMCRequest;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","remoteMCRequest");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("masterActivate",strValue);   //主MC 激活设为1否则设为0
	packet.SetAttrib("slaveActivate",strValue2);    //从MC 激活设为1否则设为0
	packet.SetAttrib("deActivate",strValue);       //MC 失活设为1否则设为0
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送远端MC设置响应消息 MCU级联用
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcrsRemoteMCResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","remoteMCResponse");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("accept",strValue);          //同意设为1否则设为0
	packet.SetAttrib("reject",strValue2);          //拒绝设为1否则设为0
	packet.SetAttrib("unspecified",strValue);     //如是拒绝原因:未知 设为1 否则设为0
	packet.SetAttrib("functionNotSupported",strValue2); //如是拒绝原因:此功能不支持 设为1 否则设为0
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送广播终端命令 makeTerminalBroadcaster   cc
	{ KCmdUserDataPacket packet ;nMsgType=h245CONccMakeTerminalBroadcaster;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","makeTerminalBroadcaster");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送取消广播终端命令 cancelMakeTerminalBroadcaster
	{ KCmdUserDataPacket packet ;nMsgType=h245CONccCancelMakeTerminalBroadcaster;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelMakeTerminalBroadcaster");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送切换源或设为发言人命令消息 sendThisSource
	{ KCmdUserDataPacket packet ;nMsgType=h245CONccSendThisSource;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","sendThisSource");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送取消切换源或取消发言人命令消息 cancelSendThisSource
	{ KCmdUserDataPacket packet ;nMsgType=h245CONccCancelSendThisSource;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelSendThisSource");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送取消切换源或取消发言人命令消息 dropConference
	{ KCmdUserDataPacket packet ;nMsgType=h245CONccDropConference;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","dropConference");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送终端号码分配指示消息 terminalNumberAssign   ci
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciTerminalNumberAssign;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalNumberAssign");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送终端加入会议指示消息 terminalJoinedConference
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciTerminalJoinedConference;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalJoinedConference");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送终端离开会议指示消息 terminalLeftConference
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciTerminalLeftConference;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalLeftConference");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送至少选看一个其他视频指示消息 seenByAtLeastOneOther
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciSeenByAtLeastOneOther;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","seenByAtLeastOneOther");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送取消至少选看一个其他视频指示消息 cancelSeenByAtLeastOneOther
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciCancelSeenByAtLeastOneOther;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelSeenByAtLeastOneOther");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}



	//发送选看所有视频指示消息 seenByAll
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciSeenByAll;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","seenByAll");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}




	//发送取消选看所有视频指示消息 cancelSeenByAll
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciCancelSeenByAll;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","cancelSeenByAll");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送请求发言消息 TERM-->MCU
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciRequestForFloor;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","requestForFloor");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送请求发言消息   MCU-->Chair
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciFloorRequested;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","floorRequested");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送指定为观看会场消息   terminalYouAreSeeing
	{ KCmdUserDataPacket packet ;nMsgType=h245CONciTerminalYouAreSeeing;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","terminalYouAreSeeing");
	packet.SetAttrib("CONFID",strConfID);
	packet.SetAttrib("mcuNumber",strMcuNum);
	packet.SetAttrib("terminalNumber",strTermNum);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送H239令牌genericRequest消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONgrqGenericRequest;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","genericRequest");
	packet.SetAttrib("CONFID",strConfID);
	int nSymBreaking =128;//0 释放，>0 其它
	packet.SetAttribUN2("SymBreak",nSymBreaking);

	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送H239令牌genericCommand消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONgcGenericCommand;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","genericCommand");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送H239令牌genericIndication消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONgiGenericIndication;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","genericIndication");
	packet.SetAttrib("CONFID",strConfID);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送H239令牌genericResponse消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONgrsGenericResponse;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","genericResponse");
	packet.SetAttrib("CONFID",strConfID);
	int nAccept=2;//2 允许 1 拒绝 0 不处理
	packet.SetAttribUN2("Response",nAccept);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送逻辑通道流控消息 flowControlCommand 消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcFlowControlCommand;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","flowControlCommand");
	packet.SetAttrib("CONFID",strConfID);
	std::string strMedia="VIDEO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	int nResourceID=0;//大于0设置值  0 不设置
	packet.SetAttrib("logicalChannel",strMedia);
	packet.SetAttribUN2("resourceID",nResourceID);
	int nMaximumBitRate=3840;//0不设置 大于0设置 单位 100 bit/s
	packet.SetAttribUN2("maximumBitRate",nMaximumBitRate);

	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送逻辑通道流控消息 flowControlIndication 消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONiFlowControlIndication;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","flowControlIndication");
	packet.SetAttrib("CONFID",strConfID);
	std::string strMedia="AUDIO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	int nResourceID=0;//大于0设置值  0 不设置
	packet.SetAttrib("logicalChannel",strMedia);
	packet.SetAttribUN2("resourceID",nResourceID);
	int nMaximumBitRate=3200;//0不设置 大于0设置 单位 100 bit/s
	packet.SetAttribUN2("maximumBitRate",nMaximumBitRate);

	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}


	//发送请求关键帧消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcVideoFastUpdatePicture;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","videoFastUpdatePicture");
	packet.SetAttrib("CONFID",strConfID);
	std::string strMedia="VIDEO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	packet.SetAttrib("MediaName",strMedia);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送请求图像冻结消息videoFreezePicture //add VCF
	{ KCmdUserDataPacket packet ;nMsgType=h245CONcVideoFreezePicture;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","videoFreezePicture");
	packet.SetAttrib("CONFID",strConfID);
	std::string strMedia="VIDEO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	packet.SetAttrib("MediaName",strMedia);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送媒体通道激活消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONiLogicalChannelActive;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","logicalChannelActive");
	packet.SetAttrib("CONFID",strConfID);
	std::string strMedia="VIDEO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	packet.SetAttrib("MediaName",strMedia);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

	//发送媒体通道激活消息
	{ KCmdUserDataPacket packet ;nMsgType=h245CONiLogicalChannelInActive;
	packet.SetAttribUN2("CMD",1);packet.SetAttrib("H245CMD","logicalChannelInactive");
	packet.SetAttrib("CONFID",strConfID);
	std::string strMedia="VIDEO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	packet.SetAttrib("MediaName",strMedia);
	std::string strPacket=packet.GetString();cszPacket=(char * ) strPacket.c_str();
	nPacketLen=strPacket.length()+1;
	SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);}

}
