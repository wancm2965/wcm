#ifndef __STKHANDLER_H__
#define __STKHANDLER_H__


#include <RVINC/cm.h>
#include <RVINC/cmras.h>
#include <RVINC/stkutils.h>
#include <RVINC/cibuf.h>
#include <RVINC/rtp.h>
#include <RVINC/rtcp.h>
#include <string>

//----------------------- cmEvent -------------------------------
int RVCALLCONV cmEvNewCall(HAPP hApp, HCALL hsCall, LPHAPPCALL lphaCall);
int RVCALLCONV cmEvRegEvent(HAPP hApp, cmRegState regState, cmRegEvent regEvent, RvPvtNodeId regEventHandle);
//======================= cmEvent ===============================

//----------------------- cmCallEvent ---------------------------
int RVCALLCONV cmEvCallStateChanged(HAPPCALL haCall, HCALL hsCall, cmCallState_e state, cmCallStateMode_e stateMode);
//20120723
int RVCALLCONV cmEvCallUserInfo(HAPPCALL haCall, HCALL hsCall, int nodeId);

int RVCALLCONV cmEvCallFacility(HAPPCALL haCall,HCALL hsCall,int handle,RvBool *proceed);

//======================= cmCallEvent ===========================

//----------------------- cmControlEvent ------------------------
int RVCALLCONV cmEvCallNewChannel(HAPPCALL haCall, HCALL hsCall, HCHAN  hsChan, LPHAPPCHAN lphaChan);
int RVCALLCONV cmEvCallCapabilities(HAPPCALL haCall, HCALL hsCall, cmCapStruct*capabilities[]);
int RVCALLCONV cmEvCallCapabilitiesExt(HAPPCALL haCall, HCALL hsCall, cmCapStruct***  capabilities[]);
int RVCALLCONV cmEvCallCapabilitiesResponse(HAPPCALL haCall,HCALL hsCall,RvUint32 status);
int RVCALLCONV cmEvCallMasterSlaveStatus(HAPPCALL haCall,HCALL hsCall,RvUint32 status);
int RVCALLCONV cmEvCallControlMessage(HAPPCALL haCall, HCALL hsCall, HAPPCHAN haChan, HCHAN hsChan, RvPvtNodeId message, cmH245MessageType messageType);
int RVCALLCONV cmEvCallControlStateChanged(HAPPCALL haCall,HCALL hsCall,cmControlState state,cmControlStateMode stateMode);
int RVCALLCONV cmEvCallMasterSlave(HAPPCALL haCall,HCALL hsCall,RvUint32 terminalType,RvUint32 statusDeterminationNumber);

//add adapter
int RVCALLCONV cmEvCallTerminalNumberAssign(HAPPCALL haCall,HCALL hsCall,RvUint8 mcuNumber, RvUint8 terminalNumber);
int RVCALLCONV cmEvChannelNewRate(HAPPCHAN haChan, HCHAN hsChan, RvUint32 rate);
int RVCALLCONV cmEvCallUserInput(HAPPCALL haCall, HCALL hsCall, RvInt32 userInputId); /* userInputId User input node ID. */
//add
int RVCALLCONV cmEvCallRoundTripDelay(IN HAPPCALL haCall, IN  HCALL hsCall,IN RvInt32 delay); /* Positive: msec delay. Negative: expiration. */

//======================= cmControlEvent ========================

//----------------------- cmChannelEvent ------------------------
int RVCALLCONV cmEvChannelParameters(HAPPCHAN haChan, HCHAN hsChan, char* channelName,HAPPCHAN haChanSameSession, HCHAN hsChanSameSession, HAPPCHAN haChanAssociated, HCHAN hsChanAssociated, RvUint32 rate);
int RVCALLCONV cmEvChannelSetAddress(HAPPCHAN haChan, HCHAN hsChan, RvUint32 ip, RvUint16 port);
int RVCALLCONV cmEvChannelSetRTCPAddress(HAPPCHAN haChan, HCHAN hsChan, RvUint32 ip, RvUint16 port);
int RVCALLCONV cmEvChannelStateChanged(HAPPCHAN haChan, HCHAN hsChan, cmChannelState_e state, cmChannelStateMode_e stateMode);
int RVCALLCONV cmEvChannelHandle(HAPPCHAN  haChan, HCHAN  hsChan, RvInt dataTypeHandle, cmCapDataType dataType);
int RVCALLCONV cmEvChannelRequestCloseStatus(HAPPCHAN haChan, HCHAN hsChan, cmRequestCloseStatus  status);
int RVCALLCONV cmEvChannelMiscCommand(HAPPCHAN haChan,HCHAN hsChan,cmMiscellaneousCommand miscCommand);

//H235 Media use
int RVCALLCONV cmEvChannelSendMessage(HAPPCHAN haChan,HCHAN hsChan,RvPvtNodeId message);
int RVCALLCONV cmEvChannelRecvMessage(   HAPPCHAN    haChan,HCHAN     hsChan,int   nodeId);

//======================= cmChannelEvent ========================

//----------------------- cmProtocolEvent -----------------------
unsigned int RVCALLCONV cmHookListen(HPROTCONN hConn,int nodeId);
int RVCALLCONV cmHookListening(HPROTCONN hConn,int nodeId,unsigned int error);
int RVCALLCONV cmHookConnecting(HPROTCONN hConn,int nodeId);
int RVCALLCONV cmHookInConn(HPROTCONN hConn,int nodeIdFrom,int nodeIdTo);
int RVCALLCONV cmHookOutConn(HPROTCONN hConn,int nodeIdFrom,int nodeIdTo,unsigned int error);
unsigned int RVCALLCONV cmHookSend(HPROTCONN hConn,int nodeId,unsigned int error);
unsigned int RVCALLCONV cmHookRecv(HPROTCONN hConn,int nodeId,unsigned int error);
unsigned int RVCALLCONV cmHookSendTo(HPROTCONN hConn,int nodeId,int nodeIdTo,unsigned int error);
unsigned int RVCALLCONV cmHookRecvFrom(HPROTCONN hConn, int nodeId,int nodeIdFrom,unsigned int multicast,unsigned int error);
void RVCALLCONV cmHookClose(HPROTCONN hConn);
//======================= cmProtocolEvent =======================


//add adapter
//----------------------- cmRASEvent ----------------------------
int RVCALLCONV cmEvRASRequest( IN  HRAS             hsRas,
							  IN  HCALL            hsCall,
							  OUT LPHAPPRAS        lphaRas,
							  IN  cmRASTransaction transaction,
							  IN  cmRASTransport*  srcAddress, 
							  IN  HAPPCALL         haCall         OPTIONAL);

int RVCALLCONV cmEvRASConfirm( IN  HAPPRAS          haRas,
							  IN  HRAS             hsRas);

int RVCALLCONV cmEvRASReject( IN  HAPPRAS          haRas,
							 IN  HRAS             hsRas,
							 IN  cmRASReason      reason);

int RVCALLCONV cmEvRASTimeout( IN  HAPPRAS          haRas,
							  IN  HRAS             hsRas);

RvBool RVCALLCONV cmEvRASMessageSend(IN  HAPPRAS         haRas,
								  IN  HRAS            hsRas,
								  IN  cmRASTrStage    rasTrStage,
								  IN  RvPvtNodeId     messageNodeId);

int RVCALLCONV cmEvAutoRASConfirm(IN  HRAS             hsRas);

int RVCALLCONV cmEvAutoRASReject(IN  HRAS             hsRas,
							 IN  cmRASReason      reason);

int RVCALLCONV cmEvAutoRASTimeout(IN  HRAS             hsRas);

void GetRasRejectReason(cmRASReason reason,std::string & strReason);

//打印能力集信息
void printCapToBuffer(void* context, IN const char* line, ...);

void OutputPVTNodeContent(std::string & strOutPutBuf,int nodeId ,HPVT hPvt=NULL);

int AddCallSignalAddressNode(HPVT hVal,int nodeId ,std::string & strCallIP,int nPort,int nIndex=1);

int AddTerminalAliasNode(HPVT hVal,int nodeId,int nIndex =1);

void CheckAddSignalAddrAliasNode(int messageNodeId);

int SetCallSignalAddress(std::string strCallIP,int nPort,HCALL hsCall,cmCallParam callParam);

struct pvtPrintContext
{
	std::string & m_strPrint;

	pvtPrintContext(std::string & strPrint)
		:m_strPrint(strPrint)
	{
	}
};


//H460 Client 
int RVCALLCONV cmEvAutoRASRequest(HRAS hsRas, HCALL hsCall, cmRASTransaction transaction, cmRASTransport*  srcAddress, HAPPCALL haCall);
//int RVCALLCONV cmEvRASRequest( HRAS hsRas, HCALL hsCall, LPHAPPRAS lphaRas, cmRASTransaction transaction, cmTransportAddress* srcAddress, HAPPCALL haCall);
//RvBool RVCALLCONV cmEvRASMessageSend(HAPPRAS haRas, HRAS hsRas, cmRASTrStage rasTrStage, RvPvtNodeId messageNodeId);
int RVCALLCONV cmEvCallSendMessage(HAPPCALL haCall, HCALL hsCall, int message);
int RVCALLCONV cmEvCallRecvMessage(HAPPCALL haCall, HCALL hsCall, int message);

#endif	//#ifndef __STKHANDLER_H__
