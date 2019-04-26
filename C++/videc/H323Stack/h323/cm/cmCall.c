/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#include "rvinternal.h"
#include "rvlog.h"
#include "rvhost.h"
#include "rvassert.h"
#include "rvstdio.h"
#include "msg.h"
#include "cm.h"
#include "cmintr.h"
#include "cmictrl.h"
#include "cmcallcid.h"
#include "cmCall.h"
#include "cmcallobj.h"
#include "q931asn1.h"
#include "cmdebprn.h"
#include "cmutils.h"
#include "cmCrossReference.h"
#include "cmQ931.h"
#include "cmiQ931.h"
#include "cmiFastStart.h"
#include "cmAutoRasCall.h"
#include "cmAutoRasEP.h"
#include "cmparam.h"
#include "cmControl.h"
#include "cmaltep.h"
#include "pvaltreeStackApi.h"
#include "psyntreeStackApi.h"
#include "rvh323mibstats.h"
#include "prnutils.h"
#include "oidutils.h"
#include "h245.h"

#ifdef __cplusplus
extern "C" {
#endif


static void enterCallInitState(IN callElem* call);
static void enterCallSetupState(IN callElem* call);
static void enterCallConnectedState(IN callElem* call);
static void initiateDialOnTransfer(callElem* call, cmElem* app);
static RvStatus disconnectionProcedure(IN  callElem* call);

int  cmcReadyEvent(IN H245Control* ctrl);
int cmCallStatusMsg2Struct(HPVT hVal,int handle,cmCallStatusMessage * callStatusMsg);


static RvBool controlDiedTimerEventsHandler(void*context);



/************************************************************************
 * simulatedMessageH245
 * purpose: define if message was simulated (was not sent actually)
 * input  : ctrl  - Stack handle for the H245 call control
 * output : none
 * return : true if simulated
 *          false - otherwise
 ************************************************************************/
RvBool simulatedMessageH245(IN HCONTROL ctrl)
{
    callElem*call=(callElem*)cmiGetByControl(ctrl);
    HSTRANSSESSION   hsTransSession=call->hsTransSession;

    return dummyControlSession(hsTransSession);
}


/************************************************************************
 * sendMessage
 * purpose: Send TCP (Q931/H245) message for a given call.
 *          This function also adds specific call-related information
 * input  : hsCall  - Stack handle for the call
 *          vNodeId - root node ID of the message to send
 *          type    - Type of channel to send on
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RvStatus sendMessage(IN HCALL hsCall, IN RvPvtNodeId vNodeId, IN cmTpktChanHandleType type)
{
    callElem*call=(callElem*)hsCall;
    TRANSERR result;

    /* Message should be processed - send it out */
    result = cmTransSendMessage(call->hsTransSession, vNodeId, (type==cmQ931TpktChannel)?cmTransQ931Type:cmTransH245Type);
    if (result == cmTransDummyHost)
        return RV_ERROR_NOTSUPPORTED;
    if ((result == cmTransWouldBlock) || ((int)result < 0))
        return RV_ERROR_UNKNOWN;
    return RV_OK;
}

/************************************************************************
 * sendCallMessage
 * purpose: Send Q931 message for a given call.
 *          This function also adds specific call-related information
 * input  : hsCall      - Stack handle for the Q931 call.
 *          message     - Root node ID of the message to send.
 *          q931type    - Message type we're dealing with.
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus sendCallMessage(
    IN HCALL                hsCall,
    IN RvPvtNodeId          message,
    IN cmCallQ931MsgType    q931type)
{
    cmElem      *app = (cmElem *)emaGetInstance((EMAElement)hsCall);
    callElem    *call = (callElem*)hsCall;
    RvPvtNodeId tmpNodeId;
    RvPvtNodeId msgNodeId;
    RvPstFieldId fieldId;
    HPVT        hVal = app->hVal;
    RvBool      replyToSetupMsg;
    cmExtendedFastConnectState_e efcState = cmExtendedFastConnectNo;

    /* We do our best not to try and put fields in messages where they're not found so we
       won't have any warnings in the log file later on */
    replyToSetupMsg =
       (q931type == cmQ931callProceeding) || (q931type == cmQ931alerting) ||
        (q931type == cmQ931connect) || (q931type == cmQ931progress) ||
        ((q931type == cmQ931facility) && app->gatekeeper);

    __pvtGetByFieldIds(msgNodeId, hVal, message, {_q931(message) _anyField _q931(userUser)
        _q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _anyField LAST_TOKEN}, &fieldId, NULL, NULL);

    /* Add to the message the identification parameters */

    /*CRV*/
    __pvtBuildByFieldIds(tmpNodeId,hVal, message, {_q931(callReferenceValue) _q931(twoBytes) LAST_TOKEN}, call->crv, NULL);

    if (fieldId != __q931(empty))
    {
        /*Call ID*/
        __pvtBuildByFieldIds(tmpNodeId, hVal, msgNodeId, {_q931(callIdentifier) _q931(guid) LAST_TOKEN}, 16, (char*)call->callId);
        RV_UNUSED_ARG(tmpNodeId);

        /*CID*/
        if (((q931type == cmQ931setup) || (q931type == cmQ931connect) || (q931type == cmQ931facility)) &&
            m_callget(call, overrideCID))
        {
            pvtAdd(hVal, msgNodeId, __q931(conferenceID), 16, (RvChar*)call->cId, NULL);
        }

        /*Add dest info field*/
        if (RV_PVT_NODEID_IS_VALID(call->destinationInfo) && replyToSetupMsg)
        {
            pvtSetTree(hVal, pvtAddBranch(hVal, msgNodeId, __q931(destinationInfo)),
                   hVal, call->destinationInfo);
        }

        cmCallGetParam(hsCall, cmParamExtendedFastConnectState, 0, (RvInt32*)&efcState, NULL);

        /* Add fast start messages in */
        if (replyToSetupMsg || (q931type == cmQ931information) || (q931type == cmQ931setup) ||
            ((efcState == cmExtendedFastConnectApproved) && (q931type != cmQ931releaseComplete)))
        {
            addFastStart((HCALL)call, message);
        }
    }

    /* Send the message on Q931 channel */
    return sendMessage(hsCall, message, cmQ931TpktChannel);
}

/************************************************************************
 * sendMessageForH245
 * purpose: Send H245 message for a given call.
 * input  : hsCall  - Stack handle for the call
 *          message - root node ID of the message to send
 * output : sentOnDummy - true iff the message was "sent" on a dummy host.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int sendMessageForH245(
    IN  HCALL    hsCall,
    IN  int      message,
    OUT RvBool*  sentOnDummy)
{
    callElem*call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    int ret;

    int nesting;

    if ((((cmElem*)hApp)->cmMyCallEvent.cmEvCallSendMessage != NULL) && (call->state != cmCallStateIdle))
    {
        cmiCBEnter((hApp, "cmEvCallSendMessage(haCall=%p,hsCall=%p,msg=%d)",
            (HAPPCALL)emaGetApplicationHandle((EMAElement)call), (HCALL)call, message));
        nesting = emaPrepareForCallback((EMAElement)call);
        ((cmElem*)hApp)->cmMyCallEvent.cmEvCallSendMessage(
            (HAPPCALL)emaGetApplicationHandle((EMAElement)call),
            (HCALL)call,
            message);
        emaReturnFromCallback((EMAElement)call, nesting);
        cmiCBExit((hApp, "cmEvCallSendMessage"));
    }

    ret = sendMessage(hsCall,message,cmH245TpktChannel);
    if (ret == RV_ERROR_NOTSUPPORTED)
    {
        if (sentOnDummy)
            *sentOnDummy = RV_TRUE;
        return RV_OK;
    }
    if (sentOnDummy)
        *sentOnDummy = RV_FALSE;
    return ret;
}


/************************************************************************
 * sendMessageForH245Chan
 * purpose: Send H245 message for a given channel.
 * input  : hsCall  - Stack handle for the call
 *          hsChan  - Channel handle for the H245 channel
 *          message - root node ID of the message to send
 * output : sentOnDummy - true iff the message was "sent" on a dummy host.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int sendMessageForH245Chan(
    IN  HCALL    hsCall,
    IN  HCHAN    hsChan,
    IN  int      message,
    OUT RvBool*  sentOnDummy)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    int nesting;

    if (((cmElem*)hApp)->cmMyChannelEvent.cmEvChannelSendMessage != NULL)
    {
        cmiCBEnter((hApp, "cmEvChannelSendMessage(haChan=%p,hsChan=%p,msg=%d)",
            (HAPPCHAN)emaGetApplicationHandle((EMAElement)hsChan), hsChan, message));
        nesting = emaPrepareForCallback((EMAElement)hsChan);
        ((cmElem*)hApp)->cmMyChannelEvent.cmEvChannelSendMessage(
            (HAPPCHAN)emaGetApplicationHandle((EMAElement)hsChan),
            hsChan,
            message);
        emaReturnFromCallback((EMAElement)hsChan, nesting);
        cmiCBExit((hApp, "cmEvChannelSendMessage"));
    }

    return sendMessageForH245(hsCall, message, sentOnDummy);
}


/************************************************************************
 * cmCallGetMessageContext
 * purpose: Get the message context for a given call. This function is
 *          used for checking security results of messages for a call.
 * input  : hsCall      - Stack handle for the call
 * output : hMsgContext - Message context for call (of the last message)
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallGetMessageContext(
    IN  HCALL            hsCall,
    OUT void**           hMsgContext)
{
    callElem*call=(callElem*)hsCall;
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
    HAPP hApp = (HAPP)emaGetInstance((EMAElement)call);
#endif

    cmiAPIEnter((hApp, "cmCallGetMessageContext: hsCall=%p",hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        if (hMsgContext)
            *hMsgContext=call->hMsgContext;
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit((hApp, "cmCallGetMessageContext=0"));

    return 0;
}


TRANSERR cmEvTransHostListen(
            IN HSTRANSHOST          hsTransHost,
            IN HATRANSHOST          haTransHost,
            IN TRANSCONNTYPE        type,
            IN cmTransportAddress   *address)
{
    HAPP hApp=(HAPP)cmTransGetHApp(hsTransHost);
    cmElem* app=(cmElem*)hApp;
    int addr;
    RvBool proceed=RV_TRUE;

    RV_UNUSED_ARG(type);
    RV_UNUSED_ARG(haTransHost);

    if ((hApp == NULL) || (hsTransHost == NULL))
        return cmTransOK;

    if (app->cmMyProtocolEvent.hookListen != NULL)
    {
        addr=pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);

        if (addr>=0 && address)
        {
            int nesting;
            cmTAToVt(app->hVal,addr,address);

            nesting = emaPrepareForCallback((EMAElement)hsTransHost);
            proceed = !app->cmMyProtocolEvent.hookListen((HPROTCONN)hsTransHost,addr);
            emaReturnFromCallback((EMAElement)hsTransHost, nesting);

            pvtDelete(app->hVal,addr);
        }
    }
    return (proceed)?cmTransOK:cmTransIgnoreMessage;
}

TRANSERR cmEvTransHostListening(
            IN HSTRANSHOST          hsTransHost,
            IN HATRANSHOST          haTransHost,
            IN TRANSCONNTYPE        type,
            IN cmTransportAddress   *address,
            IN RvBool               error)
{
    HAPP hApp=(HAPP)cmTransGetHApp(hsTransHost);
    cmElem* app=(cmElem*)hApp;
    int addr;
    RvBool proceed=RV_TRUE;

    RV_UNUSED_ARG(type);
    RV_UNUSED_ARG(haTransHost);

    if ((hApp == NULL) || (hsTransHost == NULL))
        return cmTransOK;

    if (app->cmMyProtocolEvent.hookListening != NULL)
    {
        addr=pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);

        if (addr>=0 && address)
        {
            int nesting;
            cmTAToVt(app->hVal,addr,address);

            nesting = emaPrepareForCallback((EMAElement)hsTransHost);
            proceed = !app->cmMyProtocolEvent.hookListening((HPROTCONN)hsTransHost,addr,error);
            emaReturnFromCallback((EMAElement)hsTransHost, nesting);

            pvtDelete(app->hVal,addr);
        }
    }
    return (proceed)?cmTransOK:cmTransIgnoreMessage;
}

TRANSERR cmEvTransHostConnecting(
            IN HSTRANSHOST          hsTransHost,
            IN HATRANSHOST          haTransHost,
            IN TRANSCONNTYPE        type,
            IN cmTransportAddress   *address)
{
    HAPP hApp=(HAPP)cmTransGetHApp(hsTransHost);
    cmElem* app=(cmElem*)hApp;
    int addr;
    RvBool proceed=RV_TRUE;

    RV_UNUSED_ARG(type);
    RV_UNUSED_ARG(haTransHost);

    /* See if we need this hook at all */
    if ((app->cmMyProtocolEvent.hookConnecting != NULL) && (address != NULL))
    {
        addr = pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);

        if (addr >= 0)
        {
            int nesting;
            cmTAToVt(app->hVal, addr, address);

            /* See if we should proceed with this connection - it's up to the application */
            nesting = emaPrepareForCallback((EMAElement)hsTransHost);
            proceed = !app->cmMyProtocolEvent.hookConnecting((HPROTCONN)hsTransHost,addr);
            emaReturnFromCallback((EMAElement)hsTransHost, nesting);

            /* Get rid of this node - we only needed it for the hook */
            pvtDelete(app->hVal,addr);
        }
    }

    return (proceed)?cmTransOK:cmTransIgnoreMessage;
}


TRANSERR cmEvTransHostConnected(IN HSTRANSHOST   hsTransHost,
                                IN HATRANSHOST   haTransHost,
                                IN TRANSCONNTYPE type,
                                IN RvBool        isOutgoing)
{
    HAPP hApp=(HAPP)cmTransGetHApp(hsTransHost);
    cmElem* app=(cmElem*)hApp;
    cmTransportAddress ta;
    int addrRemote,addrLocal;

    RV_UNUSED_ARG(type);
    RV_UNUSED_ARG(haTransHost);

    /* Make sure application wants to deal with these hooks at all */
    if ((isOutgoing && (app->cmMyProtocolEvent.hookOutConn == NULL)) ||
        (!isOutgoing && (app->cmMyProtocolEvent.hookInConn == NULL)))
        return cmTransOK;

    addrRemote=pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);
    addrLocal=pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);
    if (addrRemote>=0 && addrLocal>=0)
    {
        if (cmTransGetHostParam(hsTransHost, cmTransHostParam_remoteAddress, (void*)&ta)==cmTransOK)
        {
            int nesting;
            cmTAToVt(app->hVal,addrRemote,&ta);
            if (cmTransGetHostParam(hsTransHost, cmTransHostParam_localAddress, (void*)&ta)==cmTransOK)
                cmTAToVt(app->hVal,addrLocal,&ta);

            /* No need to check if hooks exist - we already did that at the beginning of this function */
            nesting = emaPrepareForCallback((EMAElement)hsTransHost);
            if (isOutgoing)
                app->cmMyProtocolEvent.hookOutConn((HPROTCONN)hsTransHost,addrLocal,addrRemote,RV_FALSE);
            else
                app->cmMyProtocolEvent.hookInConn((HPROTCONN)hsTransHost,addrRemote,addrLocal);
            emaReturnFromCallback((EMAElement)hsTransHost, nesting);
        }
    }
    pvtDelete(app->hVal,addrRemote);
    pvtDelete(app->hVal,addrLocal);
    return cmTransOK;
}


TRANSERR cmEvTransHostClosed(   IN HSTRANSHOST hsTransHost,
                                IN HATRANSHOST haTransHost,
                                IN RvBool      wasConnected)
{
    HAPP hApp=(HAPP)cmTransGetHApp(hsTransHost);
    cmElem* app=(cmElem*)hApp;
    int nesting;

    RV_UNUSED_ARG(haTransHost);

    if ((!wasConnected) && (app->cmMyProtocolEvent.hookOutConn != NULL))
    {
        cmTransportAddress ta;
        int addrRemote,addrLocal;

        addrRemote=pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);
        addrLocal=pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);
        if (addrRemote>=0 && addrLocal>=0)
        {
            if (cmTransGetHostParam(hsTransHost, cmTransHostParam_remoteAddress, (void*)&ta)==cmTransOK)
            {
                cmTAToVt(app->hVal,addrRemote,&ta);
                if (cmTransGetHostParam(hsTransHost, cmTransHostParam_localAddress, (void*)&ta)==cmTransOK)
                    cmTAToVt(app->hVal,addrLocal,&ta);

                nesting = emaPrepareForCallback((EMAElement)hsTransHost);
                app->cmMyProtocolEvent.hookOutConn((HPROTCONN)hsTransHost,addrLocal,addrRemote,RV_FALSE);
                emaReturnFromCallback((EMAElement)hsTransHost, nesting);
            }
        }
        pvtDelete(app->hVal,addrRemote);
        pvtDelete(app->hVal,addrLocal);
    }

    if (app->cmMyProtocolEvent.hookClose != NULL)
    {
        nesting = emaPrepareForCallback((EMAElement)hsTransHost);
        app->cmMyProtocolEvent.hookClose((HPROTCONN)hsTransHost);
        emaReturnFromCallback((EMAElement)hsTransHost, nesting);
    }

    cmTransCloseHost(hsTransHost);
    return cmTransOK;
}


TRANSERR cmEvTransNewRawMessage(IN  HSTRANSHOST             hsTransHost,
                                IN  HATRANSHOST             haTransHost,
                                IN  RvH323ConnectionType    type,
                                INOUT int                   pvtNode,
                                IN  RvUint8                 *msg,
                                IN  int                     msgSize,
                                OUT int                     *decoded,
                                OUT void                    **hMsgContext)
{
    HAPP hApp=(HAPP)cmTransGetHApp(hsTransHost);
    cmElem* app=(cmElem*)hApp;
    TRANSERR err;
    cmProtocol prot;
    int nesting;

    RV_UNUSED_ARG(haTransHost);

    cmCallPreCallBack(hApp);

    /* Decode the message. We call a callback function in Q931 case, since we might be using
       H.235v2 module in this part */
    if (app->cmEvCallNewRawMessage && (type == RvH323ConnectionQ931))
    {
        nesting = emaPrepareForCallback((EMAElement)hsTransHost);
        /* changed the following function to return TRANSERR directly */
        err = (TRANSERR)app->cmEvCallNewRawMessage(hApp, (HPROTCONN)hsTransHost, pvtNode, msg, msgSize, decoded, hMsgContext);
        emaReturnFromCallback((EMAElement)hsTransHost, nesting);
    }
    else
        err=(cmEmDecode(app->hVal, pvtNode, msg, msgSize, decoded)<0)?cmTransErr:cmTransOK;
    if (err != cmTransOK)
    {
        RvLogError(&app->logTPKT,
            (&app->logTPKT, "New message (channel %d) ignored (error)", emaGetIndex((EMAElement)hsTransHost)));
        RvH323CmPrintMessage(&app->logTPKTWIRE, "Bad message:", app->hVal, -1, msg, msgSize, RV_TRUE);
        return err;
    }

#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
    {
        char *name = cmGetProtocolMessageName((HAPP)app,pvtNode);
        if (RvLogIsSelected(&app->logTPKT, RV_LOGLEVEL_INFO))
        {
            RvLogTextInfo(&app->logTPKT,
                "New message (channel %d) recv <-- %s:", emaGetIndex((EMAElement)hsTransHost), nprn(name));
        }
        else
        {
            /* no TPKTCHAN Info log. it might be confusing to see the message without the <-- above it,
               so we make sure to print it to the TPKTWIRE, if it exists. */
            RvLogInfo(&app->logTPKTWIRE, (&app->logTPKTWIRE,
                "New message (channel %d) recv <-- %s:", emaGetIndex((EMAElement)hsTransHost), nprn(name)));
        }
    }
#endif
    RvLogDebug(&app->logTPKT, (&app->logTPKT, "Binary:"));
    printHexBuff(msg, msgSize, &app->logTPKT);
    RvH323CmPrintMessage(&app->logTPKTWIRE, NULL, app->hVal, pvtNode, NULL, 0, RV_TRUE);

    /* MIB - incoming message */
    if (type == RvH323ConnectionQ931)
        prot = cmProtocolQ931;
    else
        prot = cmProtocolH245;
    addStatistic(app->hStatistic, prot, app->hVal, pvtNode, RV_TRUE);

    if (app->cmMyProtocolEvent.hookRecv!=NULL)
    {
        nesting = emaPrepareForCallback((EMAElement)hsTransHost);
        err=(app->cmMyProtocolEvent.hookRecv((HPROTCONN)hsTransHost,pvtNode,RV_FALSE))?cmTransIgnoreMessage:err;
        emaReturnFromCallback((EMAElement)hsTransHost, nesting);
    }

    RvH323CmPrintMessage(&app->logTPKT, NULL, app->hVal, pvtNode, NULL, 0, RV_TRUE);

    return err;
}

TRANSERR cmEvTransSendRawMessage(   IN  HSTRANSHOST     hsTransHost,
                                    IN  HATRANSHOST     haTransHost,
                                    IN  HSTRANSSESSION  hsTransSession,
                                    IN  HATRANSSESSION  haTransSession,
                                    IN  int             pvtNode,
                                    IN  int             size,
                                    OUT RvUint8         *msg,
                                    OUT int             *msgSize)
{
    HAPP hApp=(HAPP)cmTransGetHApp(hsTransHost);
    callElem*call=(callElem*)haTransSession;
    cmElem* app=(cmElem*)hApp;
    TRANSERR err = cmTransOK;
    RvBool process=RV_TRUE;
    int nesting;

    RV_UNUSED_ARG(hsTransSession);
    RV_UNUSED_ARG(haTransHost);

#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
    {
        char *name = cmGetProtocolMessageName((HAPP)app,pvtNode);
        RvLogInfo(&app->logTPKT,
                (&app->logTPKT, "New message (channel %d) to send --> %s:", emaGetIndex((EMAElement)hsTransHost), nprn(name)));
    }
#endif

    RvH323CmPrintMessage(&app->logTPKT, "Message:",
        app->hVal, pvtNode, NULL, 0, RV_FALSE);

    getEncodeDecodeBuffer(-1, NULL);

    /* See if we have a hook and if the outgoing message should be send or not */
    if (app->cmMyProtocolEvent.hookSend != NULL)
    {
        nesting = emaPrepareForCallback((EMAElement)hsTransHost);
        process=!app->cmMyProtocolEvent.hookSend((HPROTCONN)hsTransHost,pvtNode,RV_FALSE);
        emaReturnFromCallback((EMAElement)hsTransHost, nesting);
    }
    if (process && (call == NULL || emaMarkLocked((EMAElement)call)))
    {
        cmProtocol prot = cmProtocolGetProtocol(hApp,pvtNode);

        if (call && (prot == cmProtocolQ931))
        {
            if ((app->cmMyCallEvent.cmEvCallSendMessage) && (call->state != cmCallStateIdle))
            {
                cmiCBEnter(((HAPP)app,"cmEvCallSendMessage(haCall=%p,hsCall=%p)",(HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call));
                nesting=emaPrepareForCallback((EMAElement)hsTransHost);
                (app->cmMyCallEvent.cmEvCallSendMessage)((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call, pvtNode);
                emaReturnFromCallback((EMAElement)hsTransHost,nesting);
                cmiCBExit(((HAPP)app, "cmEvCallSendMessage"));
            }
        }

        if ((call != NULL) && (emaWasDeleted((EMAElement)call)))
        {
#if (RV_LOGMASK & (RV_LOGLEVEL_INFO | RV_LOGLEVEL_DEBUG))
            char *name = cmGetProtocolMessageName((HAPP)app,pvtNode);
            RvLogInfo(&app->logTPKT,
                (&app->logTPKT, "New message (channel %d) Not sent (call deleted) --> %s:", emaGetIndex((EMAElement)hsTransHost), nprn(name)));
#endif
            emaReleaseLocked((EMAElement)call);
            return cmTransIgnoreMessage;
        }

        if (app->cmEvCallSendRawMessage)
        {
            nesting=emaPrepareForCallback((EMAElement)hsTransHost);
            err=(app->cmEvCallSendRawMessage(hApp,(HPROTCONN)hsTransHost,(HCALL)call, pvtNode, size, msg, msgSize)<0)?cmTransErr:cmTransOK;
            emaReturnFromCallback((EMAElement)hsTransHost,nesting);
        }
        else
            err=(cmEmEncode(app->hVal, pvtNode, msg, size, msgSize)<0)?cmTransErr:cmTransOK;
        if (err == cmTransOK)
        {
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
            if (!RvLogIsSelected(&app->logTPKT, RV_LOGLEVEL_INFO))
            {
                char *name = cmGetProtocolMessageName((HAPP)app,pvtNode);
                RvLogInfo(&app->logTPKTWIRE, (&app->logTPKTWIRE,
                    "New message (channel %d) sent --> %s:", emaGetIndex((EMAElement)hsTransHost), nprn(name)));
            }
#endif

            /* Make sure we notify the MIB component of the stack */
            addStatistic(app->hStatistic, prot, app->hVal, pvtNode, RV_FALSE);
            if (app->cmMyProtocolEvent.hookSendRaw != NULL)
            {
                process = !app->cmMyProtocolEvent.hookSendRaw((HPROTCONN)hsTransHost, msg, *msgSize);
            }
        }
        else
        {
#if (RV_LOGMASK & RV_LOGLEVEL_ERROR)
            char *name = cmGetProtocolMessageName((HAPP)app,pvtNode);
            RvLogError(&app->logTPKTWIRE,
                (&app->logTPKTWIRE, "New message (channel %d) Not sent (encoding error) --> %s:", emaGetIndex((EMAElement)hsTransHost), nprn(name)));
#endif
            *msgSize = -1;
        }

        emaReleaseLocked((EMAElement)call);
        RvH323CmPrintMessage(&app->logTPKTWIRE, "Message:", app->hVal, pvtNode, NULL, 0, RV_FALSE);
        RvLogDebug(&app->logTPKT, (&app->logTPKT, "Binary:"));
        printHexBuff(msg, *msgSize, &app->logTPKT);

    }
    if (!process)
        err = cmTransIgnoreMessage;

    return err;
}

//{{gaoshizhong add 打印结点信息
void printCapToBuffer(void* context, IN const char* line, ...)
{
	HCALL hsCall=(HCALL)context;
	HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
	cmiAPIEnter((hApp,"cmCallSimulateMessage:hsCall=%p,msg:%s",hsCall,line));
	cmiAPIExit((hApp,"cmCallSimulateMessage=0"));
	//OutputDebugString(line);
	//OutputDebugString("--cmCallSimulateMessage\n");
}
//}}gaoshizhong add 打印结点信息

/********************************************************************************************
 * cmCallSimulateMessage
 * purpose : "Push" a message into the stack as if it was received from the network.
 * input   : hsCall     - Call the message is received on
 *           message    - PVT node ID of the received message
 *                        This node must be a Q931 or H245 message.
 * output  : none
 * return  : Non-negative value on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallSimulateMessage(
                         IN HCALL               hsCall,
                         IN int                 message)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    HPVT hVal = ((cmElem *)hApp)->hVal;

	//{{gaoshizhong add 打印结点信息
	//pvtPrint(hVal,message,printCapToBuffer,hsCall);//gaoshizhong add
	//}}gaoshizhong add 打印结点信息

    cmiAPIEnter((hApp,"cmCallSimulateMessage: hsCall=%p, message=%d",hsCall,message));

    if (!emaLock((EMAElement)call))
    {
        cmiAPIExit((hApp,"cmCallSimulateMessage=-1"));
        return RV_ERROR_UNKNOWN;
    }

    switch(cmProtocolGetProtocol(hApp,message))
    {
        case cmProtocolQ931:
        {
            int msgNodeId,nodeId;
            RvPstFieldId fieldId;

            /* Find out the remote's side version if we don't know it yet */
            __pvtGetNodeIdByFieldIds(msgNodeId,hVal,message, {_q931(message) _anyField _q931(userUser)
                                                              _q931(h323_UserInformation) _q931(h323_uu_pdu)
                                                              _q931(h323_message_body) _anyField LAST_TOKEN});

            /* make sure the message isn't a call-proceeding message */
            pvtGet(hVal, msgNodeId, &fieldId, NULL, NULL, NULL);

            if ((m_callget(call,remoteVersion) == 0) &&
                ((nodeId=pvtGetChild(hVal,msgNodeId,__q931(protocolIdentifier),NULL)) >= 0) &&
                (fieldId != __q931(callProceeding)))
            {
                char buff[20];
                int object[6];
                int buffLen;
                buffLen=pvtGetString(hVal, nodeId, sizeof(buff), buff);
                if (oidDecodeOIDInt(buffLen, buff, sizeof(object)/sizeof(*object), object)>=6)
                    /*itu-t recommendation h 2250 version (version number)*/
                    /*0     1              2 3    4        5              */
                    m_callset(call,remoteVersion,object[5]);
            }

            q931ProcessMessage(cmiGetQ931((HCALL)call),message);
        }
        break;
        case cmProtocolH245:
        {
            if (cmiGetIsControl(hsCall))
            {
                cmExtendedFastConnectState_e efcState;
                /* check if we need to terminate EFC */
                cmCallGetParam(hsCall, cmParamExtendedFastConnectState, 0, (RvInt32*)&efcState, NULL);
                if (efcState != cmExtendedFastConnectNo)
                {
                    RvPstFieldId msgFieldId;
                    RvPvtNodeId tmpNode;

                    __pvtGetByFieldIds(tmpNode, cmGetValTree(hApp), message,
                        {_anyField _anyField LAST_TOKEN}, &msgFieldId, NULL, NULL);
                    if (RV_PVT_NODEID_IS_VALID(tmpNode) &&
                        ((msgFieldId == __h245(openLogicalChannel)) ||
                         (msgFieldId == __h245(requestChannelClose)) ||
                         (msgFieldId == __h245(closeLogicalChannel))))
                    {
                        /* cancel EFC */
                        cmCallSetParam(hsCall, cmParamExtendedFastConnectState, 0, cmExtendedFastConnectNo, NULL);
                    }
                }

                h245ProcessIncomingMessage(cmiGetControl((HCALL)call), message);
            }
        }
        break;
        default:
      break;
    }
    emaUnlock((EMAElement)call);
    cmiAPIExit((hApp,"cmCallSimulateMessage=0"));
    return 0;
}



TRANSERR cmEvTransNewMessage(IN HSTRANSSESSION          hsTransSession,
                             IN HATRANSSESSION          haTransSession,
                             IN RvH323ConnectionType    type,
                             IN int                     message,
                             IN void                    *hMsgContext,
                             IN RvBool                  isTunnelled,
                             IN int                     index)
{
    callElem*call=(callElem*)haTransSession;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    cmElem* app=(cmElem*)hApp;

    RV_UNUSED_ARG(hsTransSession);
    RV_UNUSED_ARG(index);

    cmCallPreCallBack(hApp);
    /* we might change locks in one of the functions called from inside this callback, so better
       to lock the call, so that the new lock will be locked here as well */
    if (!emaLock((EMAElement)call))
        return cmTransErr;

    if (app->cmEvCallRecvRawMessage)
    {
        /* have H.235 check these messages */
        if (app->cmEvCallRecvRawMessage(hApp, (HCALL)call, message, hMsgContext) != RV_OK)
        {
            emaUnlock((EMAElement)call);
            return cmTransErr;
        }
    }

    if (app->cmMyCallEvent.cmEvCallRecvMessage && (call->state != cmCallStateIdle))
    {
        int nesting;
        cmiCBEnter(((HAPP)app, "cmEvCallRecvMessage(haCall=%p,hsCall=%p)",(HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call));
        nesting=emaPrepareForCallback((EMAElement)call);
        app->cmMyCallEvent.cmEvCallRecvMessage(
            (HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call, message);
        emaReturnFromCallback((EMAElement)call,nesting);
        cmiCBExit(((HAPP)app, "cmEvCallRecvMessage"));
    }

    if (isTunnelled)
    {
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
        if (!RvLogIsSelected(&app->logTPKTWIRE, RV_LOGLEVEL_DEBUG))
        {
            char *name = cmGetProtocolMessageName((HAPP)app, message);
            RvLogDebug(&app->logTPKT,
                (&app->logTPKT, "New TUNNELED message (call %d-%p) recv <-- %s:",
                    emaGetIndex((EMAElement)haTransSession), haTransSession, nprn(name)));
        }
#endif
        RvH323CmPrintMessage(&app->logTPKT, NULL, app->hVal, message, 0, 0, RV_TRUE);
    }

    if (!emaWasDeleted((EMAElement)call) && (type == RvH323ConnectionQ931))
    {
        /*Release old message context, if any */
        if ((call->hMsgContext != NULL) && app->cmEvCallReleaseMessageContext)
        {
            int nesting=emaPrepareForCallback((EMAElement)call);
            app->cmEvCallReleaseMessageContext(hMsgContext);
            emaReturnFromCallback((EMAElement)call,nesting);
        }
        call->hMsgContext=hMsgContext;
    }

    if (!emaWasDeleted((EMAElement)call))
        cmCallSimulateMessage((HCALL)call,message);
    emaUnlock((EMAElement)call);
    return cmTransOK;
}




/**************************************************************************************
 * cmEvTransBadMessage
 *
 * Purpose: To report to the user that a message for a session could not be decoded
 *          or encoded.
 *          Here, undecoded Q931 messages on a session cause a STATUS message to be
 *          sent in reply.
 *
 * Input:   hsTransSession  - The stack handle of the session.
 *          haTransSession  - The application handle of the session.
 *          type            - The type of the message (Q.931/H.245).
 *          msg             - The encoded message
 *          msgSize         - The encoded message size
 *          outgoing        - RV_TRUE: outgoing message, RV_FALSE-incoming message
 *
 **************************************************************************************/
TRANSERR cmEvTransBadMessage(IN HSTRANSSESSION          hsTransSession,
                             IN HATRANSSESSION          haTransSession,
                             IN RvH323ConnectionType    type,
                             RvUint8                    *msg,
                             int                        msgSize,
                             RvBool                     outgoing)
{
    RV_UNUSED_ARG(hsTransSession);
    RV_UNUSED_ARG(msg);
    RV_UNUSED_ARG(msgSize);

    if ((type == RvH323ConnectionQ931) && (!outgoing))
    {
        if (emaMarkLocked((EMAElement)haTransSession))
        {
            /* Q931... We should send a STATUS message for this one */
            q931DecodingFailure(cmiGetQ931((HCALL)haTransSession));
            emaReleaseLocked((EMAElement)haTransSession);
        }
    }

    return cmTransOK;
}


/************************************************************************
 * enqueueDummyStates
 * purpose: Put an artificial state inside an empty states queue.
 *          This is used for callbacks other than cmEvCallStateChange such
 *          as cmEvNewCall.
 * input  : call        - Stack's call object
 * output : none
 * return : none
 ************************************************************************/
static void enqueueDummyState(IN callElem* call)
{
    if (call->q_numStates != 0)
    {
        /* There's no reason to enqueue an Idle state if there are already states in this queue */
        return;
    }

    call->q_states[call->q_nextState] = RV_H323CALL_STATE_DUMMY;
    call->q_numStates = 1;
}


/************************************************************************
 * dequeueCallStates
 * purpose: Dequeue all pending states in the list of states
 * input  : app         - Stack instance
 *          call        - Stack's call object
 *          state       - State of call we want to process first
 *                        If set to cmCallStateIdle, then it is ignored
 *          stateMode   - Mode of the state we want to process first
 * output : none
 * return : none
 ************************************************************************/
static void dequeueCallStates(
    IN cmElem*              app,
    IN callElem*            call,
    IN cmCallState_e        state,
    IN cmCallStateMode_e    stateMode)
{
    int nesting;
    cmCallState_e oldState;

    cmiAPIEnter(((HAPP)app, ">>---------dequeueCallStates(app->numberOfConnectingCalls=%d)", app->numberOfConnectingCalls));

    call->q_numStates++;
    while ((call->q_numStates > 0) &&
           (call->state != cmCallStateIdle)) /* Idle is the "last stop" */
    {
        /* cmCallStateInit is reserved for the use of the queue as a state that should be
           in the queue, but always not-handled. This way, we can for example use cmEvNewCall()
           as if it is a queued state to make sure all event callbacks are handled after it.
           If we already raised cmCallStateDisconnected, we should only raise cmCallStateIdle next,
           so skip any other state. */
        if ((state != cmCallStateInit) &&
            ((call->state != cmCallStateDisconnected) || (state == cmCallStateIdle)))
        {
            /* handle the next state - its a real one */
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
             static const RvChar *stateModes[]=
               {"cmCallStateModeDisconnectedBusy",
                "cmCallStateModeDisconnectedNormal",
                "cmCallStateModeDisconnectedReject",
                "cmCallStateModeDisconnectedUnreachable",
                "cmCallStateModeDisconnectedUnknown",
                "cmCallStateModeDisconnectedLocal",
                "", /* no 6 */
                "cmCallStateModeConnectedCallSetup",
                "cmCallStateModeConnectedCall",
                "cmCallStateModeConnectedConference",

                "cmCallStateModeOfferingCreate",
                "cmCallStateModeOfferingInvite",
                "cmCallStateModeOfferingJoin",
                "cmCallStateModeOfferingCapabilityNegotiation",
                "cmCallStateModeOfferingCallIndependentSupplementaryService",

                "cmCallStateModeDisconnectedIncompleteAddress",

                "cmCallStateModeTransferingForwarded",
                "cmCallStateModeTransferingRouteToMC",
                "cmCallStateModeTransferingRouteToGatekeeper",

                "cmCallStateModeSetupNoReply",
                "cmCallStateModeSetupGotReleaseComplete"
};

            static const RvChar *states[]=
               {"cmCallStateDialtone",
                "cmCallStateProceeding",
                "cmCallStateRingBack",
                "cmCallStateConnected",
                "cmCallStateDisconnected",
                "cmCallStateIdle",
                "cmCallStateOffering",
                "cmCallStateTransfering",
                "", "",
                "cmCallStateIncompleteAddress",
                "cmCallStateWaitAddressAck"};

            const RvChar *stateModeStr = "";

            if ((state==cmCallStateDisconnected)||(state==cmCallStateConnected)||(state==cmCallStateOffering) ||
                ((stateMode == cmCallStateModeSetupNoReply) || (stateMode == cmCallStateModeSetupGotReleaseComplete)))
            {
                stateModeStr = stateModes[(int)stateMode];
            }

            cmiCBEnter(((HAPP)app,"cmEvCallStateChanged(haCall=%p,hsCall=%p,state=%s,stateMode=%s)",
                emaGetApplicationHandle((EMAElement)call), call, states[state], stateModeStr));
#endif
            oldState = call->state;
            call->state=state;
            call->stateMode=stateMode;

            if (call->state == cmCallStateDisconnected)
            {
                /* Make sure we decrement the number of currently connecting calls if we reach a
                   disconnected state on a non-connected call */
                if (m_callget(call,connectingCall))
                {
                    cmLock((HAPP)app);
                    app->numberOfConnectingCalls--;
                    m_callset(call,connectingCall,RV_FALSE);
                    cmUnlock((HAPP)app);
                }

                disconnectionProcedure(call);
            }

            if (state == cmCallStateIdle)
            {
                /* Don't raise the idle state from here. Tell EMA to raise it when all callbacks have exited. */
                emaRaiseIdle((EMAElement)call);
            }
            else if (app->cmMyCallEvent.cmEvCallStateChanged != NULL)
            {
                nesting=emaPrepareForCallback((EMAElement)call);
                app->cmMyCallEvent.cmEvCallStateChanged((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,state,stateMode);
                emaReturnFromCallback((EMAElement)call,nesting);
            }

            /* If callback didn't delete the call and it's a connected one, check
               if the control has already connected as well */
            if (!emaWasDeleted((EMAElement)call))
                notifyControlState((HCALL)call, controlGetState(cmiGetControl((HCALL)call)));

#if (RV_LOGMASK & RV_LOGLEVEL_LEAVE)
            cmiCBExit(((HAPP)app,"cmEvCallStateChanged"));
#endif
        }


        /* reduce state number */
        call->q_numStates--;
        if(call->q_numStates)
        {
            /* get next state from the queue */
            if (call->q_states[call->q_nextState] != RV_H323CALL_STATE_DUMMY)
            {
                state = (cmCallState_e) call->q_states[call->q_nextState];
                stateMode = (cmCallStateMode_e) call->q_stateModes[call->q_nextState];
            }
            else
                state = cmCallStateInit;
            call->q_nextState = (RvUint8) ( (call->q_nextState+1)%RV_H323CALL_STATE_MAX );
        }
    }

	cmiAPIExit(((HAPP)app, ">>---------dequeueCallStates(app->numberOfConnectingCalls=%d)", app->numberOfConnectingCalls));

}


/************************************************************************
 * notifyState
 * purpose: Notify the application about the state of the call
 * input  : hsCall      - Stack handle for the call
 *          state       - State of call
 *          stateMode   - Mode of the state
 * output : none
 * return : none
 ************************************************************************/
static void notifyState(IN HCALL hsCall, IN cmCallState_e state, IN cmCallStateMode_e stateMode)
{
    callElem*call=(callElem*)hsCall;
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);

    /* check if a state is already being handled */
    if (call->q_numStates != 0)
    {
        int newState;
        int oldState;
        if (call->q_numStates >= RV_H323CALL_STATE_MAX)
        {
            /* we may be in a state-loop - this shouldn't happen! */
            RvLogError(&app->logERR,
                (&app->logERR, "notifyState: We have a loop for hsCall=%p", hsCall));
            return;
        }

        /* place the new state in the state queue */
        newState = (call->q_nextState + call->q_numStates - 1) % RV_H323CALL_STATE_MAX;
        oldState = (newState+RV_H323CALL_STATE_MAX-1)%RV_H323CALL_STATE_MAX;
        if((call->q_states[oldState] != (RvUint8)state) || (call->q_stateModes[oldState] != (RvUint8)stateMode))
        {
            /* Seems like this state is not the same state as its predecessor - add it in */
            call->q_states[newState] = (RvUint8)state;
            call->q_stateModes[newState] = (RvUint8)stateMode;
            call->q_numStates++;
        }
        return;
    }
    else
    {
        /* just update the "current state" to the real current state */
        int curState;
        curState = (call->q_nextState + RV_H323CALL_STATE_MAX - 1)%RV_H323CALL_STATE_MAX;
        call->q_states[curState] = (RvUint8)state;
        call->q_stateModes[curState] = (RvUint8)stateMode;
    }

    /* No current states - handle it as is */

    /* don't report the same state twice */
    if ( (call->state == state) && (call->stateMode == stateMode) )
        return;

    /* enter state handle */
    dequeueCallStates(app, call, state, stateMode);
}


/************************************************************************
 * notifyControlState
 * purpose: Notify the application about the state of the control
 *          This function checks the control state and uses the call state
 *          notification function.
 * input  : hsCall         - Stack handle for the call
 *          state          - The control state
 * output : none
 * return : none
 ************************************************************************/
void notifyControlState(IN HCALL             hsCall,
                        IN H245ControlState  state)
{
    callElem*call=(callElem*)hsCall;

    if ((cmiGetIsControl(hsCall)) &&
        (call->state == cmCallStateConnected))
    {
        switch(state)
        {
            case ctrlConnected:
                if ((call->stateMode == cmCallStateModeConnectedCallSetup) ||
                    (call->stateMode == cmCallStateModeConnectedConference))
                {
                    notifyState((HCALL)call,cmCallStateConnected, cmCallStateModeConnectedCall);
                }
            break;
            case ctrlConference:
                if ((call->stateMode == cmCallStateModeConnectedCallSetup) ||
                    (call->stateMode == cmCallStateModeConnectedCall))
                {
                    notifyState((HCALL)call,cmCallStateConnected, cmCallStateModeConnectedConference);
                }
            break;
            default:
                /* Do nothing */
            break;
        }
    }
}

/************************************************************************
 * cmCallInAnsweringState
 * purpose: Check to see if we could be in the middle of answering a
 *          FS proposal
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : RV_TRUE - call state right, RV_FALSE - wrong call state
 ************************************************************************/
RvBool RVCALLCONV cmCallInAnsweringState(IN HCALL hsCall)
{
    RvBool bOrigin;
    callElem* call = (callElem*)hsCall;

    bOrigin = m_callget(call, callInitiator);

    if (!bOrigin && ((call->state == cmCallStateOffering) || (call->state == cmCallStateInit)))
        return RV_TRUE;

    return RV_FALSE;
}

/************************************************************************
 * callStartOK
 * purpose: Deal with an outgoing or incoming call in the Q931 side, after
 *          RAS was done (or skiped)
 * input  : call    - Stack handle for the call to dial
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int callStartOK(IN callElem* call)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    int nodeId;
    cmTransportAddress tpkt;
    cmTransportAddress annexE;
    int ret=0;
    catStruct      callObj;

    /* See what's the direction of the call */
    if (m_callget(call,callInitiator))
    {
        /* Outgoing call - make sure we add it to the CAT */
        if ((ret = callObjectCreate(app, (HCALL)call, Outgoing, &callObj))<0)
            return ret;
        if (call->hCatCall == NULL)
        {
            call->hCatCall = catAdd(app->hCat, &callObj, (HCALL)call);
            if (call->hCatCall == NULL)
                return RV_ERROR_OUTOFRESOURCES;
        }
        else
        {
            catUpdate(app->hCat, call->hCatCall, &callObj);
        }

        {
            cmAnnexEUsageMode annexEBehavior=cmTransNoAnnexE;
            cmCallGetParam((HCALL)call,cmParamAnnexE,0,(int *)&annexEBehavior,NULL);
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_annexEMode,&annexEBehavior);
        }

        /* Findout the address of the call's Q931 destination and try to connect to it */
        nodeId=((call->routeCallSignalAddress>=0))?
                    call->routeCallSignalAddress:
                    call->callSignalAddress;
        cmVtToTA(app->hVal,nodeId, &tpkt);

        if (call->annexECallSignalAddress>=0)
            cmVtToTA(app->hVal,call->annexECallSignalAddress, &annexE);
        else
            annexE=tpkt;

        cmTransSetAddress(call->hsTransSession, NULL, &tpkt, &annexE, NULL, cmTransQ931Conn, RV_FALSE);

        ret = (int)cmTransQ931Connect(call->hsTransSession);
    }
    else
    {
        /* Incoming call */
        RvBool incompleteAddress=0;
        {
            RVHCATCALL     hCatCall;

            /* CAT changes */
            if ((ret=callObjectCreate(app, (HCALL)call, Incoming, &callObj))<0)
                return ret;

            /* See if we already got this call */
            hCatCall = catFind(app->hCat, &callObj);
            if (hCatCall != NULL)
            {
                callElem * foundCall = (callElem*)catGetCallHandle(app->hCat, hCatCall);
                if (foundCall && m_callget(foundCall,callWithoutQ931))
                {
                    catUpdate(app->hCat, hCatCall, &callObj);
                    *foundCall=*call;
                    call=foundCall;
                }
            }
            else if (call->hCatCall == NULL)
            {
                /* No such call - add it to CAT as a new call */
                call->hCatCall = catAdd(app->hCat, &callObj, (HCALL)call);
                if (call->hCatCall == NULL)
                    return RV_ERROR_OUTOFRESOURCES;
            }
            else
            {
                /* update CAT information */
                catUpdate(app->hCat, call->hCatCall, &callObj);
            }

            /* Check if we've got a complete address or not */
            if (m_callget(call,remoteCanOverlapSend) && m_callget(call,enableOverlapSending))
                   incompleteAddress=1;
        }


        if (incompleteAddress)
        {
            /* Notify the application about the incomplete address */
            notifyState((HCALL)call,cmCallStateWaitAddressAck, (cmCallStateMode_e)0);
        }
        else
        {
            /* Continue to handle the incoming setup */
            enterCallSetupState(call);
        }

    }
    return ret;
}


/************************************************************************
 * reportDisconnectedState
 * purpose: Notify the application about a disconnected call
 * input  : call        - Stack handle for the call
 *          dropReason  - Reason the call was disconnected
 * output : none
 * return : none
 ************************************************************************/
static void reportDisconnectedState(
    IN callElem                     *call,
    IN cmCallDisconnectionReason    dropReason)
{
    cmCallState_e       state = cmCallStateDisconnected;
    cmCallStateMode_e   stateMode = cmCallStateModeDisconnectedNormal;

    /* Make sure we notify application about disconnections only if it knowns about this call */
    if (m_callget(call, notified))
    {
        /* when a GK crashes during a routed call, the EP will send DRQ and wait for the timeout
           to drop the call. if cmCallDrop is called at that time, the disconnected state is reported
           again, unless we do the following: */
        m_callset(call, notified, RV_FALSE);

        /* Remember why this call for dropped. This might be useful for the application some day */
        if (call->disconnectReason == cmCallDisconnectionReasonUnknown)
            call->disconnectReason = dropReason;

        switch (dropReason)
        {
            case cmCallDisconnectionReasonRemoteDropped:
            {
                /* Remote side dropped this call. Try and check the cause value and RC reasons if we can */
                RvInt32 causeValue = 0;
                RvInt32 reason = -1;
                cmCallGetParam((HCALL)call, cmParamReleaseCompleteCause, 0, &causeValue, NULL);
                cmCallGetParam((HCALL)call, cmParamReleaseCompleteReason, 0, &reason, NULL);
                if ((causeValue == 17) || ((cmReasonType)reason == cmReasonTypeInConf))
                    stateMode = cmCallStateModeDisconnectedBusy;
                else if (causeValue == 28)
                    stateMode = cmCallStateModeDisconnectedIncompleteAddress;

				//{{gaoshizhong add use reportDisconnectedState
				{
					HCALL hsCall=(HCALL)call;
					char msg[512]={0};
					HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
					cmiAPIEnter((hApp,"reportDisconnectedState info: hsCall=%p, dropReason=%d",hsCall,dropReason));
					cmiAPIExit((hApp,"reportDisconnectedState info=0"));
					sprintf(msg,"reportDisconnectedState info: hsCall=%p, dropReason=%d\n",hsCall,dropReason);
					//OutputDebugString(msg);
				}
				//}}gaoshizhong add use reportDisconnectedState
                break;
            }

            case cmCallDisconnectionReasonLocalDropped:
                stateMode = cmCallStateModeDisconnectedLocal;
				//{{gaoshizhong add use reportDisconnectedState
				{
					HCALL hsCall=(HCALL)call;
					char msg[512]={0};
					HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
					cmiAPIEnter((hApp,"reportDisconnectedState info: hsCall=%p, dropReason=%d",hsCall,dropReason));
					cmiAPIExit((hApp,"reportDisconnectedState info=0"));
					sprintf(msg,"reportDisconnectedState info: hsCall=%p, dropReason=%d\n",hsCall,dropReason);
					//OutputDebugString(msg);
				}
				//}}gaoshizhong add use reportDisconnectedState

                break;

            case cmCallDisconnectionReasonUnknown:
            case cmCallDisconnectionReasonProtocolError:
            case cmCallDisconnectionReasonNotRegistered:
            case cmCallDisconnectionReasonReceivedARJ:
            case cmCallDisconnectionReasonReceivedDRQ:
            case cmCallDisconnectionReasonNoRasResponse:
            case cmCallDisconnectionReasonNoDialResponse:
            case cmCallDisconnectionReasonQ931Closed:
            case cmCallDisconnectionReasonH245Closed:
            case cmCallDisconnectionReasonTimeoutT301:
            case cmCallDisconnectionReasonTimeoutT302:
            case cmCallDisconnectionReasonTimeoutT303:
            case cmCallDisconnectionReasonTimeoutT304:
            case cmCallDisconnectionReasonTimeoutT310:
                break;
        }

        if (stateMode == cmCallStateModeDisconnectedNormal)
        {
            /* The state mode still not determined properly */
            switch(call->state)
            {
                case cmCallStateInit:
                case cmCallStateDialtone:
                    /* make sure this side initiated the call */
                    if (m_callget(call, callInitiator))
                        stateMode = cmCallStateModeDisconnectedUnreachable;
                    break;

                case cmCallStateProceeding:
                case cmCallStateRingBack:
                    stateMode = cmCallStateModeDisconnectedReject;
                    break;

                case cmCallStateTransfering:
                case cmCallStateDisconnected:
                case cmCallStateIdle:
                    /* If we in one of this states, then we already reported the disconnected
                       state, so we can just skip the notification part. */
                    return;

                case cmCallStateConnected:
                case cmCallStateOffering:
                case cmCallStateWaitAddressAck:
                    /* For these, we do nothing special */
                    break;
            }
        }

        /* Change the state to disconnected */
        notifyState((HCALL)call, state, stateMode);
    }
}

/************************************************************************
 * dropControl
 * purpose: Drop the H245 Control connection for a call.
 *          This function is used when calls are disconnected
 * input  : call    - Stack handle for the call
 * output : none
 * return : none
 ************************************************************************/
void dropControl(IN callElem* call)
{
    HCONTROL hCtrl;
    hCtrl = cmiGetControl((HCALL)call);

    /* First update the status so we don't get into stopControl() twice from 2 different
       threads: one on the incoming endSessionCommand and one for an outgoing one - done inside */
    m_callset(call,controlDisconnected,RV_TRUE);
    stopControl(hCtrl);

    /* Close all channels of this call */
    closeChannels(hCtrl);
}

/************************************************************************
 * dropRas
 * purpose: Notify the gatekeeper the call was disconnected before finishing
 *          off with this call
 * input  : call    - Stack handle for the call
 * output : none
 * return : none
 ************************************************************************/
void dropRas(callElem*call)
{
    int ret = 0;
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);

    if (!emaWasDeleted((EMAElement)call))
    {
        if (!app->manualRAS && !m_callget(call,dummyRAS))
        {
            /* Automatic RAS - send DRQ on call if necessary */
            ret = cmiAutoRASCallDrop((HCALL)call);
        }

        if (app->manualRAS || m_callget(call,dummyRAS) || (ret < 0))
        {
            if (!app->manualRAS)
                cmiAutoRASCallClose((HCALL)call);

            /* Manual RAS - continue to drop the call */
            callStopOK(call);
        }
    }
}


void callStartError(
    IN callElem                     *call,
    IN cmCallDisconnectionReason    reason)
{
    if (!m_callget(call, callInitiator))
        q931CallDrop(cmiGetQ931((HCALL)call), -1, -1);

    /* If GK has rejected our call then just close the call */
    /* No RAS drop required*/
    m_callset(call, dummyRAS, RV_TRUE);
    reportDisconnectedState(call, reason);
}

int callIncompleteAddress(callElem*call)
{
    /* This function is called for incoming ARJ with reason incompleteAddress */
    cmElem* app = (cmElem*)emaGetInstance((EMAElement)call);
    if (m_callget(call, callInitiator))
    {
        if (m_callget(call, enableOverlapSending))
        {
            if (app->cmMyCallEvent.cmEvCallIncompleteAddress != NULL)
            {
                HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)call);
                int nesting;
                cmiCBEnter(((HAPP)app, "cmEvCallIncompleteAddress(haCall=%p,hsCall=%p)", haCall, (HCALL)call));
                nesting = emaPrepareForCallback((EMAElement)call);
                app->cmMyCallEvent.cmEvCallIncompleteAddress(haCall, (HCALL)call);
                emaReturnFromCallback((EMAElement)call, nesting);
                cmiCBExit(((HAPP)app, "cmEvCallIncompleteAddress"));
            }
        }
        else
        {
            if (!m_callget(call, callInitiator))
                q931CallDrop(cmiGetQ931((HCALL)call), -1, -1);

            /* If GK has rejected our call then just close the call*/
            /* No RAS drop required */
            m_callset(call, dummyRAS, RV_TRUE);
            reportDisconnectedState(call, cmCallDisconnectionReasonReceivedARJ);
            return RV_ERROR_NOTSUPPORTED;
        }
    }
    return RV_OK;
}

int callStartRoute(callElem*call)
{
    /* Call should be routed to a gatekeeper */
    cmTransportAddress ta;
    if (!m_callget(call, callInitiator))
    {
        cmCallSetParam((HCALL)call, cmParamFacilityReason, 0, (RvInt32)cmReasonTypeRouteCallToGatekeeper, NULL);
        cmGetGKCallSignalAddress((HAPP)emaGetInstance((EMAElement)call), &ta);
        cmCallSetParam((HCALL)call, cmParamAlternativeAddress, 0, 0, (char*)&ta);
        q931CallFacility(cmiGetQ931((HCALL)call), -1);
    }
    else
    {
        /* Seems like we're the destination - this shouldn't happen... */
        callStartError(call, cmCallDisconnectionReasonProtocolError);
    }
    return 0;
}

/************************************************************************
 * callDial
 * purpose: Dials a call or answers incoming Setup requests.
 *          This function checks if we have to ARQ the GK on this call or
 *          not and continues the call setup process.
 * input  : call    - Stack handle for the call to dial
 * output : none
 * return : negative on error
 ************************************************************************/
int callDial(IN callElem* call)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    RvBool viaGk = RV_TRUE;
    cmTransportAddress ta;

    if (m_callget(call, automaticRAS))
    {
        m_callset(call, dummyRAS, RV_FALSE);

        if ((cmiAutoRASGetRegState((HAPP)app) == cmRegistered) &&
            ((int)call->preGrantedArqUse >= 0))
        {
            /* PreGrantedARQ is used */
            if (m_callget(call, callInitiator))
            {
                /* Let's see if we have a PreGranted call making on this outgoing call */
                if (app->rasGranted & (int)makeCall)
                {
                    if ( !(app->rasGranted & (int)useGKCallSignalAddressToMakeCall) && (call->preGrantedArqUse == cmPreGrantedArqDirect))
                    {
                        /*direct call without ARQ is allowed*/
                        if (cmCallGetParam((HCALL)call,cmParamDestCallSignalAddress,0,0,(char*)&ta)>=0)
                        {
                            cmCallSetParam((HCALL)call,cmParamDestinationIpAddress,0,0,(char*)&ta);
                            viaGk = RV_FALSE;
                        }
                    }

                    if (viaGk)
                    {
                        /* Routed call without ARQ through the GK */
                        if (app->rasGrantedProtocol == 1)
                        {
                            cmCallSetParam((HCALL)call,cmParamAnnexE,0,(RvInt32)cmTransUseAnnexE,NULL);
                            cmCallSetParam((HCALL)call,cmParamDestinationAnnexEAddress,0,0,(char*)&app->gkAnnexEAddress);
                        }
                        else
                        {
                            cmCallSetParam((HCALL)call,cmParamAnnexE,0,(RvInt32)cmTransNoAnnexE,NULL);
                            cmGetGKCallSignalAddress((HAPP)app,&ta);
                            cmCallSetParam((HCALL)call,cmParamDestinationIpAddress,0,0,(char*)&ta);
                        }
                    }

                    m_callset(call, dummyRAS, RV_TRUE);
                }
            }
            else if ((app->rasGranted & (int)answerCall) && !(app->rasGranted & (int)useGKCallSignalAddressToAnswer))
            {
                /* It's an incoming call and we can answer the Setup without an ARQ to the GK */
                m_callset(call, dummyRAS, RV_TRUE);
            }
        }
    }
    else
    {
        m_callset(call, dummyRAS, RV_TRUE);
    }

    call->rate = call->newRate;

    if (!m_callget(call, dummyRAS))
    {
        /*Automatic RAS*/
        return cmiAutoRASCallDial((HCALL)call);
    }
    else
    {
        /* set the source addresses from the configuration. in automatic RAS, the ARQ will be set */
        if (cmCallGetParam((HCALL)call,cmParamFullSourceAddress,0,NULL,NULL)<0)
            cmCallSetParam((HCALL)call,cmParamFullSourceAddress,0,
                pvtGetChild2(app->hVal,app->rasConf,__q931(registrationInfo),__q931(terminalAlias)),NULL);
        cmiAutoRASCallSetUnsolicitedIRR((HCALL)call, app->irrFrequencyInCall);

        /* No need to deal with any RAS messages here - continue with the call */
        return callStartOK(call);
    }
}


/**********************************************************************
 * cmSetupEnd
 * This function is responsible for the end of the scenario of receiving
 * a setup message. It was taken out of the cmSetup in order to give the
 * endpoint time to receive ACF before sending AutoConnect (when autoRas
 * and autoConnect are on).
 ************************************************************************/
int cmSetupEnd(IN callElem* call)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    HPVT hVal=app->hVal;
    int autoAnswer, ret = 0;

    if (pvtGetChild(hVal,app->q931Conf,__q931(autoAnswer),NULL)>=0)
    {
        /* Configuration set to automatically answer any incoming call */
        autoAnswer = 1;
    }
    else
    {
        autoAnswer = 0;

        /* Check if we automatically send Alerting for incoming calls */
        if (m_callget(call,sendAlerting) != 0)
            q931CallAccept(cmiGetQ931((HCALL)call),-1);
    }

    /*If we are configured to answer the call automatically then do it*/
    if (autoAnswer && call->state!=cmCallStateConnected)
        ret = callAnswer(call, RV_TRUE);
    return ret;
}

static void cmSetup(callElem*call, int message)
{
    int  ret;

    /* Check if we automatically send Call-Proceeding for incoming calls */
    if(m_callget(call,sendCallProceeding))
    {
        m_callset(call, sendCallProceeding, RV_FALSE);
        q931CallCallProceeding(cmiGetQ931((HCALL)call),-1);
        m_callset(call, enableOverlapSending, RV_FALSE);
    }

    /* We need to analyze the fast start before sending the automatic CONNECT message back */
    if ((message >=0) && !emaWasDeleted((EMAElement)call))
    {
        RvInt32 establish=1;
        cmCallGetParam((HCALL)call,cmParamEstablishH245,0,&establish,NULL);
        if (establish)
            analyzeFastStartMsg((HCALL)call, message);
    }

    if (!emaWasDeleted((EMAElement)call))
    {
        ret = callDial(call);
        if ((ret >= 0) && (m_callget(call,dummyRAS)) && (call->state != cmCallStateWaitAddressAck))
            cmSetupEnd(call);
    }
}

static void cmCallProceeding(callElem*call, int message)
{
    cmElem* app = (cmElem*)emaGetInstance((EMAElement)call);
    HPVT hVal = app->hVal;
    RvPvtNodeId tmpNodeId;

    __pvtGetNodeIdByFieldIds(tmpNodeId,hVal,message,
        {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
         _q931(h323_uu_pdu) _q931(h323_message_body) _anyField _q931(destinationInfo) LAST_TOKEN});

    cmCallSetParam((HCALL)call, cmParamFullDestinationInfo, 0, tmpNodeId, NULL);

    notifyState((HCALL)call, cmCallStateProceeding, (cmCallStateMode_e)0);

    {
        RvInt32 establish = 1;
        cmCallGetParam((HCALL)call, cmParamEstablishH245, 0, &establish, NULL);
        if (establish)
        {
            if (cmFastStartReply((HCALL)call, message, RV_FALSE) >= 0)
                deleteOutgoingFastStartOffer((HCALL)call);
        }
    }
}

static void cmAlerting(callElem*call, int message)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    HPVT hVal=app->hVal;
    RvPvtNodeId tmpNodeId;

    __pvtGetNodeIdByFieldIds(tmpNodeId,hVal,message,
        {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
         _q931(h323_uu_pdu) _q931(h323_message_body) _anyField _q931(destinationInfo) LAST_TOKEN});

    cmCallSetParam((HCALL)call,cmParamFullDestinationInfo,0,tmpNodeId,NULL);

    notifyState((HCALL)call,cmCallStateRingBack,(cmCallStateMode_e)0);

    {
        RvInt32 establish = 1;
        cmCallGetParam((HCALL)call, cmParamEstablishH245, 0, &establish, NULL);
        if (establish)
        {
            if (cmFastStartReply((HCALL)call, message, RV_FALSE) >= 0)
                deleteOutgoingFastStartOffer((HCALL)call);
        }
    }
}

static void cmConnect(callElem*call, int message)
{
    cmElem* app = (cmElem*)emaGetInstance((EMAElement)call);
    H245Control *ctrl=(H245Control *)cmiGetControl((HCALL)call);
    HPVT hVal = app->hVal;
    RvPvtNodeId destInfo;

    /* check if EFC was determined yet */
    if (cmiGetExtendedFastConnectStatus((HCALL)call, RV_TRUE) == cmExtendedFastConnectUndetermined)
    {
        /* not determined on connect - assume it is not supported */
        RvStatus ret = cmiSetRemoteExtendedFastConnectStatus((HCALL)call, m_callget(call,callInitiator), cmExtendedFastConnectNo);

        if (ret == RV_ERROR_NOTSUPPORTED)
        {
            /* drop the call */
            cmCallDropParam((HCALL)call, cmReasonTypeNeededFeatureNotSupported);
            return;
        }
    }

    __pvtGetNodeIdByFieldIds(destInfo, hVal, message,
        {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
        _q931(h323_uu_pdu) _q931(h323_message_body) _anyField _q931(destinationInfo) LAST_TOKEN});

    cmCallSetParam((HCALL)call,cmParamFullDestinationInfo,0,destInfo,NULL);

    enterCallConnectedState(call);
    if (!emaWasDeleted((EMAElement)call))
    {
        RvInt32 establish = 1;
        cmCallGetParam((HCALL)call, cmParamEstablishH245, 0, &establish, NULL);
        if (establish)
        {
            if (cmFastStartReply((HCALL)call, message, RV_TRUE) >= 0)
                deleteOutgoingFastStartOffer((HCALL)call);
        }

        if (emaWasDeleted((EMAElement)call) || (call->state == cmCallStateIdle))
            return;

        if (cmiGetExtendedFastConnectStatus((HCALL)call, RV_TRUE) != cmExtendedFastConnectApproved)
        {
            deleteOutgoingFastStartOffer((HCALL)call);
            cmiSetIsFastStartFinished((HCALL)call, RV_TRUE);
            cmcReadyEvent(ctrl);
        }
    }
}


static int cmReleaseComplete(callElem*call, int message)
{
    /* Incoming release complete message */
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    RV_UNUSED_ARG(message);

    /* try altEP if we have it and are the originator the call is in dialTone state */
    if ((RV_PVT_NODEID_IS_VALID(call->altEpProperty)) &&
        ((call->state == cmCallStateDialtone) || (call->state == cmCallStateProceeding)))
    {
        /* cancel setup timer, otherwise the altEp feature will call SetupNoReply callback */
        RvH323TimerCancel(app->hTimers, &call->timer);

        /* first time trying to connect Alt EP the calls altEpNum is set to -1 at call's init time */
        if (call->altEpNum < 0)
        {
            /* Set call's altEpNum to 0 and than increase it by one. */
            call->altEpNum = 0;
        }

        /* inform application about "no answer" from EP */
        notifyState((HCALL)call, call->state, cmCallStateModeSetupGotReleaseComplete);
    }
    else
    {
        /* Drop Transactions - put it in Idle state */
        cmTransDrop(call->hsTransSession);

        reportDisconnectedState(call, cmCallDisconnectionReasonRemoteDropped);
    }
    return 0;
}


static void cmStatus(IN callElem*call, IN RvPvtNodeId message)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    HPVT hVal=app->hVal;

    if (RV_PVT_NODEID_IS_VALID(message))
    {
        /* We actually received a STATUS message */
        if (app->cmMyCallEvent.cmEvCallStatus != NULL)
        {
            cmCallStatusMessage callStatusMsg;
            RvPvtNodeId handle;
            int nesting;

            handle=pvtGetChild2(hVal, message, __q931(message), __q931(status));
            cmCallStatusMsg2Struct(hVal, handle, &callStatusMsg);

            cmiCBEnter(((HAPP)app, "cmEvCallStatus(haCall=%p,hsCall=%p,handle=%d)",
                (HAPPCALL)emaGetApplicationHandle((EMAElement)call), (HCALL)call, handle));
            nesting = emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallStatus(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)call), (HCALL)call, &callStatusMsg);
            emaReturnFromCallback((EMAElement)call, nesting);
            cmiCBExit(((HAPP)app, "cmEvCallStatus"));
        }
    }
    else
    {
        /* StatusEnquiry has timed out... */
        if (app->cmMyCallEvent.cmEvCallStatusEnquiryTimeout != NULL)
        {
            int nesting;

            cmiCBEnter(((HAPP)app, "cmEvCallStatusEnquiryTimeout(haCall=%p,hsCall=%p)",
                (HAPPCALL)emaGetApplicationHandle((EMAElement)call), (HCALL)call));
            nesting = emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallStatusEnquiryTimeout(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)call), (HCALL)call);
            emaReturnFromCallback((EMAElement)call, nesting);
            cmiCBExit(((HAPP)app, "cmEvCallStatusEnquiryTimeout"));
        }
    }
}


static void cmFacility(callElem*call, int message)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    HPVT hVal=app->hVal;
    RvBool proceed = RV_TRUE;
    RvPvtNodeId tmp;
    RvPstFieldId fieldId=RV_ERROR_UNKNOWN;
    RvBool empty = RV_FALSE;
    int  i=0,j=0;
    RvBool firstE164 = RV_TRUE;

    __pvtGetNodeIdByFieldIds(tmp, hVal, message,
        {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
         _q931(h323_message_body) LAST_TOKEN});

    empty = (pvtGetChild(hVal,tmp,__q931(empty),NULL)>=0);
    tmp = pvtChild(hVal,tmp);

    {
        RvInt32 establish=1;
        cmCallGetParam((HCALL)call,cmParamEstablishH245,0,&establish,NULL);
        if (establish)
            if (cmFastStartReply((HCALL)call, message, RV_FALSE) >= 0)
                deleteOutgoingFastStartOffer((HCALL)call);
    }

    if (emaWasDeleted((EMAElement)call) || (call->state == cmCallStateIdle))
        return;

    if (!empty)
    {
        __pvtGetByFieldIds(tmp, hVal, tmp,
            {_q931(reason) _anyField LAST_TOKEN}, &fieldId, NULL, NULL);
    }

    if (app->cmMyCallEvent.cmEvCallFacility != NULL)
    {
        int nesting;

        cmiCBEnter(((HAPP)app, "cmEvCallFacility(haCall=%p,hsCall=%p,handle=%d,proceed=&)",
            (HAPPCALL)emaGetApplicationHandle((EMAElement)call), (HCALL)call, message));
        nesting=emaPrepareForCallback((EMAElement)call);
        app->cmMyCallEvent.cmEvCallFacility((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,message,&proceed);
        emaReturnFromCallback((EMAElement)call,nesting);
        cmiCBExit(((HAPP)app, "cmEvCallFacility proceed=%s", (proceed)?"TRUE":"FALSE"));
    }

    if (proceed && !empty && !emaWasDeleted((EMAElement)call))
    {
        cmCallStateMode_e stateMode = (cmCallStateMode_e)0;

        switch (fieldId)
        {
            case __q931(callForwarded):
            {
                /* remove the destination addresses before setting the new ones */
                i = cmCallGetNumOfParams((HCALL)call, cmParamDestinationAddress);
                for(; i>0; i--)
                {
                    cmCallDeleteParam((HCALL)call,cmParamDestinationAddress,i);
                }
                /* Also remove the source addresses if we are the call's destination */
                if (!m_callget(call,callInitiator))
                {
                    i = cmCallGetNumOfParams((HCALL)call, cmParamSourceAddress);
                    for(; i>0; i--)
                    {
                        cmCallDeleteParam((HCALL)call,cmParamSourceAddress,i);
                    }
                }
                i=0;
                m_callset(call,newCIDRequired,RV_TRUE);
                stateMode = cmCallStateModeTransferingForwarded;
            }
            /* Flows into next case statement */
            case __q931(routeCallToMC):
            {
                cmCallDeleteParam((HCALL)call,cmParamCalledPartyNumber,0);
                cmCallDeleteParam((HCALL)call,cmParamDestCallSignalAddress,0);

                if (fieldId==__q931(routeCallToMC))
                {
                    char cid[16];
                    RvInt32 size=16;

                    cmCallSetParam((HCALL)call,cmParamConferenceGoal,0,(RvInt32)cmJoin,NULL);
                    cmCallGetParam((HCALL)call,cmParamFacilityCID,0,&size,cid);
                    cmCallSetParam((HCALL)call,cmParamCID,0,size,cid);
                    stateMode = cmCallStateModeTransferingRouteToMC;
                }
            }
            /* Flows into next case statement */
            case __q931(routeCallToGatekeeper):
            {
                if (fieldId==__q931(routeCallToGatekeeper))
                {
                    stateMode = cmCallStateModeTransferingRouteToGatekeeper;
                }

                for (;;)
                {
                    char buffer[256];
                    cmAlias alias;
                    alias.string = buffer;
                    alias.length = sizeof(buffer);
                    buffer[0]=0;

                    if (cmCallGetParam((HCALL)call,cmParamAlternativeAliasAddress,i++,NULL,(char*)&alias)>=0)
                    {
                        /* if this is the first e164 alias */
                        if (alias.type==cmAliasTypeE164 && firstE164)
                        {
                            cmCallSetParam((HCALL)call,cmParamCalledPartyNumber,0,0,(char*)&alias);
                            firstE164 = RV_FALSE;
                        }
                        cmCallSetParam((HCALL)call,cmParamDestinationAddress,j++,0,(char*)&alias);
                    }
                    else
                        break;
                }

                cmCallDeleteParam((HCALL)call,cmParamCallSignalAddress,0);
                cmCallDeleteParam((HCALL)call,cmParamDestinationAnnexEAddress,0);
                cmCallSetParam((HCALL)call,cmParamAnnexE,0,(RvInt32)cmTransNoAnnexE,NULL);
                m_callset(call,remoteVersion,0);
                {
                    cmTransportAddress ta;

                    if ((cmCallGetParam((HCALL)call,cmParamAlternativeAddress,0,NULL,(char*)&ta)>=0) &&
                        cmTaHasIp(&ta) && (ta.port != 0))
                    {
                        if (fieldId==__q931(routeCallToGatekeeper))
                        {
                            cmCallSetParam((HCALL)call,cmParamRouteCallSignalAddress,0,0,(char*)&ta);
                        }
                        cmCallSetParam((HCALL)call,cmParamDestinationIpAddress,0,0,(char*)&ta);
                        if (fieldId!=__q931(routeCallToGatekeeper))
                        {
                            cmCallDeleteParam((HCALL)call,cmParamRouteCallSignalAddress,0);
                            cmCallSetParam((HCALL)call,cmParamDestCallSignalAddress,0,0,(char*)&ta);
                        }
                    }

                }
                dropControl(call);
                freeControl(cmiGetControl((HCALL)call));
                cmCallSetParam((HCALL)call,cmParamReleaseCompleteCause,0,16,NULL);
                cmCallSetParam((HCALL)call,cmParamReleaseCompleteReason,0,(RvInt32)cmReasonTypeFacilityCallDeflection,NULL);
                q931CallDrop(cmiGetQ931((HCALL)call), -1, -1);
                notifyState((HCALL)call,cmCallStateTransfering, stateMode);
                dropRas(call);
            }
            break;
            default: break;
        }
    }
}

static void cmProgress(callElem*call, int message)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    RvPvtNodeId tmpNodeId;

    RV_UNUSED_ARG(message);
    RV_UNUSED_ARG(call);

    __pvtGetNodeIdByFieldIds(tmpNodeId, app->hVal, message,
        {_q931(message) _anyField _q931(userUser) _q931(h323_UserInformation)
         _q931(h323_uu_pdu) _q931(h323_message_body) _anyField _q931(destinationInfo) LAST_TOKEN});

    cmCallSetParam((HCALL)call,cmParamFullDestinationInfo,0,tmpNodeId,NULL);
    {
        RvInt32 establish = 1;
        cmCallGetParam((HCALL)call,cmParamEstablishH245,0,&establish,NULL);
        if (establish)
        {
            if (cmFastStartReply((HCALL)call, message, RV_FALSE) >= 0)
                deleteOutgoingFastStartOffer((HCALL)call);
        }
    }

    if (emaWasDeleted((EMAElement)call) || (call->state == cmCallStateIdle))
        return;

    if (app->cmMyCallEvent.cmEvCallProgress)
    {
        int nesting;
        cmiCBEnter(((HAPP)app, "cmEvCallProgress(haCall=%p,hsCall=%p,handle=%ld)",(HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,message));
        nesting=emaPrepareForCallback((EMAElement)call);
        app->cmMyCallEvent.cmEvCallProgress((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,message);
        emaReturnFromCallback((EMAElement)call,nesting);
        cmiCBExit(((HAPP)app, "cmEvCallProgress"));
    }
}

static void cmSetupAcknowledge(callElem*call, int message)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);

    RV_UNUSED_ARG(message);

    if (m_callget(call,enableOverlapSending))
    {
        if (app->cmMyCallEvent.cmEvCallIncompleteAddress)
        {
            int nesting;
            cmiCBEnter(((HAPP)app, "cmEvCallIncompleteAddress(haCall=%p,hsCall=%p)",emaGetApplicationHandle((EMAElement)call),call));
            nesting=emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallIncompleteAddress((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call);
            emaReturnFromCallback((EMAElement)call,nesting);
            cmiCBExit(((HAPP)app, "cmEvCallIncompleteAddress"));
        }
    }

    else
    {
        HPVT hVal = app->hVal;
        RvPvtNodeId tmpNodeId,tmpNodeId1;
        RvPvtNodeId sMessage=callGetMessage((HCALL)call,cmQ931status);

        __pvtGetNodeIdByFieldIds(tmpNodeId,hVal,sMessage, {_q931(message) _anyField LAST_TOKEN});
        __pvtBuildByFieldIds(tmpNodeId1,hVal,tmpNodeId,
                             {_q931(cause) _q931(octet4) _q931(causeValue) LAST_TOKEN},98,NULL);
        __pvtBuildByFieldIds(tmpNodeId1,hVal,tmpNodeId,
                            {_q931(callState) _q931(callStateValue) LAST_TOKEN},
                            (RvInt32)q931GetCallState(cmiGetQ931((HCALL)call)),NULL);

        sendCallMessage((HCALL)call, sMessage, cmQ931status);
        callReleaseMessage((HCALL)call, cmQ931status);
        RV_UNUSED_ARG(tmpNodeId1);
    }
}

static void cmInformation(callElem*call, int message)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    HPVT hVal=app->hVal;
    RvPvtNodeId tmpNodeId,tmpNodeId1,q931NodeId;
    int nesting;
    RvInt32 establish = 1;

    cmCallGetParam((HCALL)call, cmParamEstablishH245, 0, &establish, NULL);
    if (establish)
    {
        if (cmFastStartReply((HCALL)call, message, RV_FALSE) >= 0)
            deleteOutgoingFastStartOffer((HCALL)call);
    }

    if (emaWasDeleted((EMAElement)call) || (call->state == cmCallStateIdle))
        return;

    if ((call->state == cmCallStateWaitAddressAck) && app->cmMyCallEvent.cmEvCallAdditionalAddress)
    {
        char address[256];
        RvBool sendingComplete=RV_FALSE;
        address[0]=0;

        __pvtGetNodeIdByFieldIds(q931NodeId, hVal, message, {_q931(message) _anyField LAST_TOKEN});
        tmpNodeId=pvtGetChild(hVal, q931NodeId, __q931(calledPartyNumber), NULL);
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        {
            tmpNodeId1 = pvtGetChild(app->hVal,tmpNodeId,__q931(numberDigits),NULL);
            pvtGetString(app->hVal,tmpNodeId1,256,address);
        }

        if (pvtGetChild(hVal, q931NodeId,__q931(sendingComplete),NULL)>=0)
            sendingComplete=RV_TRUE;

        cmiCBEnter(((HAPP)app, "cmEvCallAdditionalAddress (haCall=%p,hsCall=%p,message=%d)",emaGetApplicationHandle((EMAElement)call),call,message));
        nesting=emaPrepareForCallback((EMAElement)call);
        app->cmMyCallEvent.cmEvCallAdditionalAddress((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,address,sendingComplete);
        emaReturnFromCallback((EMAElement)call,nesting);
        cmiCBExit(((HAPP)app, "cmEvCallAdditionalAddress"));
    }
    else if (app->cmMyCallEvent.cmEvCallUserInfo)
    {
        cmiCBEnter(((HAPP)app,"cmEvCallUserInfo(haCall=%p,hsCall=%p,message=%d)",emaGetApplicationHandle((EMAElement)call),call,message));
        nesting=emaPrepareForCallback((EMAElement)call);
        app->cmMyCallEvent.cmEvCallUserInfo((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,message);
        emaReturnFromCallback((EMAElement)call,nesting);
        cmiCBExit(((HAPP)app,"cmEvCallUserInfo"));
    }
}

static void cmNotify(callElem*call, int message)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    if (app->cmMyCallEvent.cmEvCallNotify)
    {
        int nesting;
        cmiCBEnter(((HAPP)app,"cmEvCallNotify(haCall=%p,hsCall=%p,handle=%ld)",(HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,message));
        nesting=emaPrepareForCallback((EMAElement)call);
        app->cmMyCallEvent.cmEvCallNotify((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,message);
        emaReturnFromCallback((EMAElement)call,nesting);
        cmiCBExit(((HAPP)app,"cmEvCallNotify"));
    }
}


/************************************************************************
 * cmIndicate
 * purpose: Indicate and process an incoming Q931 message
 * input  : call    - Stack handle for the call
 *          message - Message root node
 *          msgType - Type of the message
 * output : none
 * return : Non-negative value on success
 *          negative value on failure
 ************************************************************************/
int cmIndicate(IN HCALL call, IN RvPvtNodeId message, IN cmCallQ931MsgType msgType)
{
    callElem* callE=(callElem*)call;
    cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);

    /* Reset the call's timer - we got a message from the net
       unless we have disconnected the control and we wait just for releaseComplete using
       the timer as the postControlDisconnectionDelay timer */
    if (!(m_callget(callE,controlDisconnected) && (msgType != cmQ931releaseComplete)))
    {
        RvH323TimerCancel(app->hTimers, &callE->timer);
    }

    /* Make sure we ignore incoming messages if we're already in the idle state */
    if ((msgType != cmQ931releaseComplete) && (callE->state == cmCallStateIdle))
        return 0;

    /* Act by the message itself */
    switch ((int)msgType)
    {
        case cmQ931setup            : cmSetup(callE,message);            break;
        case cmQ931callProceeding   : cmCallProceeding(callE,message);   break;
        case cmQ931connect          : cmConnect(callE,message);          break;
        case cmQ931alerting         : cmAlerting(callE,message);         break;
        case cmQ931releaseComplete  : cmReleaseComplete(callE,message);  break;
        case cmQ931status           : cmStatus(callE,message);           break;
        case cmQ931facility         : cmFacility(callE,message);         break;
        case cmQ931setupAck         : cmSetupAcknowledge(callE,message); break;
        case cmQ931progress         : cmProgress(callE,message);         break;
        case cmQ931information      : cmInformation(callE,message);      break;
        case cmQ931notify           : cmNotify(callE,message);           break;

        /* Handle any timeouts we had */
        case 1301                   : reportDisconnectedState(callE, cmCallDisconnectionReasonTimeoutT301); break;
        case 1302                   : reportDisconnectedState(callE, cmCallDisconnectionReasonTimeoutT302); break;
        case 1303                   : reportDisconnectedState(callE, cmCallDisconnectionReasonTimeoutT303); break;
        case 1304                   : reportDisconnectedState(callE, cmCallDisconnectionReasonTimeoutT304); break;
        case 1310                   : reportDisconnectedState(callE, cmCallDisconnectionReasonTimeoutT310); break;

        default:
            /* We should never be here... */
            RvAssert(RV_FALSE);
            break;
    }
    return 0;
}

/************************************************************************
 * cmCallMemSet
 * purpose: Set a new call object information
 * input  : call    - Call object to initialize
 * output : none
 * return : none
 ************************************************************************/
static void cmCallMemSet(IN callElem* call)
{
    memset(call, 0xff, sizeof(callElem));

#ifdef RV_H323CALL_DEBUG_FLAGS
    memset(&call->flags, 0, sizeof(call->flags));
#else
    call->flags             = 0;
#endif

    call->rate              = 0;
    call->newRate           = 0;
    call->hopCount          = 0;

    call->hCatCall          = NULL;
    call->timer             = NULL;
    call->firstChan         = NULL;
    call->hMsgContext       = NULL;
    call->hsTransSession    = NULL;

    call->state             = cmCallStateInit;
    call->q_nextState       = 0;
    call->q_numStates       = 0;
}


/************************************************************************
 * cmEvRASNewCall
 * purpose: Handle a new call that was received by a RAS ARQ message
 * input  : hApp        - Application's handle for a stack instance
 *        : lphCatCall  - the pointer where to put the CAT handle
 *        : callObj     - call parameters
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmEvRASNewCall(
    IN HAPP         hApp,
    IN RVHCATCALL*  lphCatCall,
    IN catStruct*   callObj)
{
    callElem call,*ce;
    int someId;
    int maxCalls;
    RvBool earlyH245;
    cmElem*app=(cmElem*)hApp;
    HPVT hVal=app->hVal;
	RvInt32 nRet=-1;

    /* Check that it is possible to accept the call*/
    pvtGet(hVal,pvtGetChild(app->hVal,app->q931Conf,__q931(maxCalls),NULL),NULL,NULL,&(maxCalls),NULL);
    if (app->busy < maxCalls)
    {
        /* Set a new call object information */
        cmCallMemSet(&call);

        m_callset(&call, automaticRAS, !app->manualRAS);
        m_callset(&call, overrideCID, RV_TRUE);
        m_callset(&call, callWithoutQ931, RV_TRUE);

        /* transport layer defaults */
        m_callset(&call,h245Tunneling ,(pvtGetChild(hVal,app->q931Conf,__q931(h245Tunneling),NULL)>=0));
        m_callset(&call,notEstablishControl , (pvtGetChild(hVal,app->q931Conf,__q931(notEstablishControl),NULL)>=0));
        m_callset(&call,isMultiplexed ,RV_FALSE); /* Default for incoming calls is non-multiplexed */
        m_callset(&call,shutdownEmptyConnection ,RV_FALSE);
        earlyH245 = (pvtGetChild(hVal,app->q931Conf,__q931(earlyH245),NULL)>=0);
        m_callset(&call,h245Stage , (earlyH245)?cmTransH245Stage_early:cmTransH245Stage_connect);

        someId=pvtGetChild(app->hVal,app->rasConf,__q931(preGrantedArqUse),NULL);
        if (pvtGetChild(app->hVal,someId,__q931(routed),NULL) >= 0)  call.preGrantedArqUse=cmPreGrantedArqRouted;
        if (pvtGetChild(app->hVal,someId,__q931(direct),NULL) >= 0)  call.preGrantedArqUse=cmPreGrantedArqDirect;


        {
            int nodeId;
            int annexE=0;

            call.annexE=cmTransNoAnnexE;

            if ((nodeId=pvtGetChild(app->hVal,app->q931Conf,__q931(useAnnexE),NULL))>=0)
            {
                int usageMode = pvtGetSyntaxIndex(app->hVal,pvtChild(app->hVal,pvtGetChild(app->hVal,nodeId,__q931(protocolPreference),NULL)))-1;
                if (usageMode >= 0)
                    call.annexE = (cmAnnexEUsageMode)usageMode;

                if (((int)call.annexE) < 0)
                {
                    call.annexE=cmTransNoAnnexE;
                    pvtGetChildByFieldId(app->hVal,nodeId,__q931(defaultProtocol),&annexE,NULL);
                    if (annexE)
                        call.annexE=cmTransUseAnnexE;
                }
            }
        }

        /* Add the new call to the calls database */
        ce=(callElem *)emaAdd(app->hCalls,NULL);
        if (ce == NULL)
            return RV_ERROR_UNKNOWN;
        if (transParentNew(app->hTransport, (void*)ce) != RV_OK)
        {
            emaDelete((EMAElement*)ce);
            return RV_ERROR_UNKNOWN;
        }

        memcpy(ce,&call,sizeof(call));
        clearControl(cmiGetControl((HCALL)ce), !(callObj->answerCall), sizeof(callElem) + sizeof(q931Elem));
        fastStartCallInit((HCALL)ce);
        if (!app->manualRAS)
            cmiAutoRASCallCreate((HCALL)ce);
        q931CallCreate((HQ931)cmiGetQ931((HCALL)ce), -1, -1, -1, -1, -1, -1);

        /* Add this call to CAT */
        *lphCatCall=ce->hCatCall = catAdd(app->hCat,callObj,(HCALL)ce);
        {
            /* Set the call's parameters from the CAT struct */
            cmCallSetParam((HCALL)ce,cmParamCID,0,0,(char*)callObj->cid);
            cmCallSetParam((HCALL)ce,cmParamCallID,0,0,(char*)callObj->callID);
            cmCallSetParam((HCALL)ce,cmParamRASCRV,0,(int)(callObj->rasCRV),NULL);
        }

        /* Notify application about the new incoming call */
        m_callset(ce,notified,RV_TRUE);
        if (app->cmMyEvent.cmEvNewCall != NULL)
        {
            HAPPCALL haCall = NULL;
            enqueueDummyState(ce);
            cmiCBEnter(((HAPP)app, "cmEvNewCall(hApp=%p,hsCall=%p,lphaCall)", app, ce));
            nRet=app->cmMyEvent.cmEvNewCall((HAPP)app,(HCALL)ce,&haCall);
            emaSetApplicationHandle((EMAElement)ce,(void*)haCall);
            cmiCBExit(((HAPP)app, "cmEvNewCall(*lphaCall=%p)", emaGetApplicationHandle((EMAElement)ce)));
            dequeueCallStates(app, ce, cmCallStateInit, (cmCallStateMode_e)-1);

            if (app->mibEvent.h341AddNewCall)
                app->mibEvent.h341AddNewCall(app->mibHandle, (HCALL)ce);
        }
		//{{gaoshizhong add 20140801 process cmEvNewCall return value
		if(nRet < 0) return RV_ERROR_OUTOFRESOURCES;
		else
		//}}gaoshizhong add 20140801 process cmEvNewCall return value
		{
        /* We have a new call here... */
        cmLock((HAPP)app);
        app->busy++;
        cmUnlock((HAPP)app);
		}

        return RV_OK;
    }

    return RV_ERROR_UNKNOWN;
}

/**************************************************************************************
 * cmEvTransNewSession
 * Purpose: To report to the user that a new session was created due to a new
 *          incoming message. This callback is called only when a new message actually
 *          arrives and not when the connection is established.
 *          The CM creates a new incoming call object in this event
 * Input:   hsTrans         - The stack handle of the instance.
 *          haTrans         - The application handle of the instance.
 *          hsTransSession  - The stack handle of the session.
 *          pvtNode         - the node of the SETUP message that caused the creation.
 * Output:  haTransSession  - The application handle of the session.
 *          cause           - In case of rejection, the cause (as in RELEASE COMPLETE).
 *          reasonNameId    - In case of rejection, the reason name Id (as in RELEASE COMPLETE).
 **************************************************************************************/
TRANSERR cmEvTransNewSession(IN  HAPPTRANS        hsTrans,
                             IN  HAPPATRANS       haTrans,
                             IN  HSTRANSSESSION   hsTransSession,
                             OUT HATRANSSESSION   *haTransSession,
                             IN  int              message,
                             OUT int              *cause,
                             OUT RvPstFieldId     *reasonNameId)
{
    cmElem* app=(cmElem*)haTrans;
    callElem call,*ce = NULL;
    RvPvtNodeId someId;
    HPVT hVal=app->hVal;
    catStruct callObj;
    RVHCATCALL hCatCall;
    RvBool rasCall = RV_FALSE;
	RvInt32 nRet=-1;

    RV_UNUSED_ARG(hsTrans);

    /* Create a CAT key out of the incoming SETUP message */
    if (callObjectCreateFromMessage(app, message, &callObj) < 0)
        return cmTransErr;

    /* If we need to reject the call the cause and reason for the rejection are taken from the configuration */
    if (cause)
    {
        *cause=17;
        pvtGetChildByFieldId(hVal,app->q931Conf,__q931(busyCause),cause,NULL);
    }
    if (reasonNameId)
    {
        *reasonNameId=__q931(inConf);
        pvtGet(hVal,pvtChild(hVal, pvtGetChild(hVal,app->q931Conf,__q931(busyReason),NULL)),reasonNameId,NULL,NULL,NULL);
    }

    /*It is possible that there are already call started from RAS*/
    if ((hCatCall=catFind(app->hCat, &callObj))!=NULL)
    {
        /* Found a call - update it if it's a RAS call */
        ce = (callElem*)catGetCallHandle(app->hCat, hCatCall);
        if (ce && m_callget(ce,callWithoutQ931))
        {
            rasCall = RV_TRUE;
            catUpdate(app->hCat, hCatCall, &callObj);
        }
        else
        {
            /* call already has Q.931, it cannot be the same call (we just got a new setup, remember?) */
            ce = NULL;
        }
    }
    if (ce == NULL)
    {
        RvInt32 maxConnectingCalls;
        RvInt32 maxCalls;

        /* Check that it is possible to accept the call*/
        pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(maxCalls), &maxCalls, NULL);
        if (!RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(maxConnectingCalls), &maxConnectingCalls, NULL)))
            maxConnectingCalls = maxCalls;

        if ((app->busy < maxCalls) && (app->numberOfConnectingCalls < maxConnectingCalls))
        {
            /* Create the new call object */
            cmCallMemSet(&call);

            m_callset(&call, automaticRAS, !app->manualRAS);
            m_callset(&call, overrideCID, RV_TRUE);

            someId = pvtGetChild(app->hVal, app->rasConf, __q931(preGrantedArqUse), NULL);
            if (RV_PVT_NODEID_IS_VALID(someId))
            {
                if (pvtGetChild(app->hVal,someId,__q931(routed),NULL) >= 0)
                    call.preGrantedArqUse = cmPreGrantedArqRouted;
                else if (pvtGetChild(app->hVal,someId,__q931(direct),NULL) >= 0)
                    call.preGrantedArqUse = cmPreGrantedArqDirect;
            }

            ce = (callElem *)emaAdd(app->hCalls, NULL);
            if (ce != NULL)
            {
                memcpy(ce,&call,sizeof(call));
                clearControl(cmiGetControl((HCALL)ce), RV_FALSE, sizeof(callElem) + sizeof(q931Elem));
                fastStartCallInit((HCALL)ce);
                if (!app->manualRAS)
                    cmiAutoRASCallCreate((HCALL)ce);
            }
        }
        else
        {
            if (app->busy >= maxCalls)
            {
                RvLogWarning(&app->log, (&app->log, "Releasing incoming call due to Q931.maxCalls"));
            }
            else
            {
                RvLogWarning(&app->log, (&app->log, "Releasing incoming call due to Q931.maxConnectingCalls"));
            }
            return cmTransErr;
        }
    }

    if (ce == NULL)
        return cmTransErr;

    if (rasCall)
    {
        /* call already had a lock from the ARQ transaction. we should lock it before changing locks */
        emaLock((EMAElement)ce);
        transParentShare(app->hTransport, (void*)ce, (void*)hsTransSession);
        emaUnlock((EMAElement)ce);
    }
    else
    {
        /* call did not have a previous lock */
        transParentShare(app->hTransport, (void*)ce, (void*)hsTransSession);
    }

    if (emaMarkLocked((EMAElement)ce))
    {
        int q931NodeId,perNodeId;
        int tmpNodeId;
        RvInt32 t302 = 10, t322 = 4, responseTo;

        m_callset(ce,callWithoutQ931,RV_FALSE);
        ce->hsTransSession=hsTransSession;
        *haTransSession=(HATRANSSESSION)ce;
        /* set the application handle immediately, we might need it in case the application calls
           APIs inside cmEvNewCall */
        emaSetApplicationHandle((EMAElement)hsTransSession, (void*)*haTransSession);
        callInitParameters((HAPP)app, (HCALL)ce);
        __pvtGetNodeIdByFieldIds(someId,app->hVal,app->rasConf, {_q931(registrationInfo) _q931(terminalType) LAST_TOKEN});

        /* for a call that has just been created set the initial transport states are according to the
           config.val file, otherwise set it to what is written on the call (and might have been changed
           by setParam) */
        if (!rasCall)
        {
            RvBool earlyH245;

            /* Add as new call */
            cmLock((HAPP)app);
            app->busy++;
            cmUnlock((HAPP)app);

            m_callset(ce,h245Tunneling, (pvtGetChild(hVal,app->q931Conf,__q931(h245Tunneling),NULL)>=0));
            m_callset(ce,isParallelTunneling,RV_FALSE);
            m_callset(ce,isMultiplexed,RV_FALSE); /* Default for incoming calls is non-multiplexed */
            m_callset(ce,shutdownEmptyConnection,RV_FALSE);

            m_callset(ce,notEstablishControl ,(pvtGetChild(hVal,app->q931Conf,__q931(notEstablishControl),NULL)>=0));
            earlyH245 = (pvtGetChild(hVal,app->q931Conf,__q931(earlyH245),NULL)>=0);
            m_callset(ce,h245Stage ,(earlyH245)?cmTransH245Stage_early:cmTransH245Stage_connect);
            m_callset(ce,enableOverlapSending ,(pvtGet(app->hVal,pvtGetChild(app->hVal,app->q931Conf,__q931(overlappedSending),NULL),NULL,NULL,NULL,NULL) >= 0));
        }
        cmCallSetParam((HCALL)ce, cmParamH245Tunneling, 0, (RvInt32)m_callget(ce,h245Tunneling), NULL);
        cmCallSetParam((HCALL)ce, cmParamH245Parallel, 0, (RvInt32)m_callget(ce,isParallelTunneling), NULL);
        cmCallSetParam((HCALL)ce, cmParamIsMultiplexed, 0, (RvInt32)m_callget(ce,isMultiplexed), NULL);
        cmCallSetParam((HCALL)ce, cmParamShutdownEmptyConnection, 0, (RvInt32)m_callget(ce,shutdownEmptyConnection), NULL);
        cmCallSetParam((HCALL)ce, cmParamEstablishH245, 0, (RvInt32)(!m_callget(ce,notEstablishControl)), NULL);
        cmCallSetParam((HCALL)ce, cmParamH245Stage, 0, (RvInt32)m_callget(ce,h245Stage), NULL);
        cmCallSetParam((HCALL)ce, cmParamCanOverlapSending, 0, (RvInt32)m_callget(ce,enableOverlapSending), NULL);

        if (RV_PVT_NODEID_IS_VALID(someId))
        {
            cmCallSetParam((HCALL)ce,cmParamFullDestinationInfo,0,someId,NULL);
            cmCallSetParam((HCALL)ce,cmParamFullSourceInfo,0,someId,NULL);
        }
        else
        {
            __pvtBuildByFieldIds(someId, app->hVal, app->rasConf, {_q931(registrationInfo) _q931(terminalType) LAST_TOKEN}, 0, NULL);
            pvtAdd(app->hVal, someId, __q931(mc), 0, NULL, NULL);
            pvtAdd(app->hVal, someId, __q931(undefinedNode), 0, NULL, NULL);
            cmCallSetParam((HCALL)ce,cmParamFullDestinationInfo,0,someId,NULL);
            cmCallSetParam((HCALL)ce,cmParamFullSourceInfo,0,someId,NULL);
        }

        pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(responseTimeOut), &t302, NULL);
        responseTo = t302;
        pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(t302), &t302, NULL);
        if (t302 > 0) /* We don't tolerate no timeout on T302 */
            t302 *= 1000;
        else
            t302 = responseTo * 1000;
        pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(t322), &t322, NULL);
        t322 *= 1000;

        q931CallCreate((HQ931)cmiGetQ931((HCALL)ce), -1, t302, -1, -1, -1, t322);

        /* Get Call Identification Parameters from the message */
        /* CRV */
        ce->crv = callObj.crv;

        __pvtGetNodeIdByFieldIds(q931NodeId,hVal,message, {_q931(message) _anyField LAST_TOKEN});
        __pvtGetNodeIdByFieldIds(perNodeId,hVal,q931NodeId, {_q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu)
                                                             _q931(h323_message_body) _anyField LAST_TOKEN});

        /* CID*/
        memcpy(ce->cId,callObj.cid,16);

        /* CallID*/
        if (callObj.flags & catCallId)
            memcpy(ce->callId,callObj.callID,16);
        else
        {
            /* In the case call has no CallID generate one */
            RvH323UniqueIdGeneratorGet(app->guidGenerator, ce->callId);
        }


        /* Call may be accepted */

        {/*Process Call Rate*/
            int rateMult=0;
            int rate=0;
            /*Calculate the call rate from the bearerCapability IE if present */
            __pvtGetNodeIdByFieldIds(tmpNodeId,hVal,q931NodeId, {_q931(bearerCapability) _q931(octet4) LAST_TOKEN});
            pvtGet(hVal,pvtGetChild(hVal,tmpNodeId,__q931(rateMultiplier),NULL),NULL,NULL,&rateMult,NULL);
            pvtGet(hVal,pvtGetChild(hVal,tmpNodeId,__q931(informationTransferRate),NULL),NULL,NULL,&rate,NULL);
            switch(rate)
            {
                case 0:ce->newRate=0;break;
                case 16:ce->newRate=64000;break;
                case 17:ce->newRate=128000;break;
                case 19:ce->newRate=384000;break;
                case 21:ce->newRate=1536000;break;
                case 23:ce->newRate=1920000;break;
                case 24:ce->newRate=64000*rateMult;break;
                /*Set Rate to be 128000 as default*/
                default: ce->newRate=128000;break;
            }
            if (rateMult>1) m_callset(ce,multiRate,RV_TRUE);
        }
        {
            /* Process Overlap Sending */
            int remoteCOS;
            /* Remote should both specify canOverlapSend==RV_TRUE and omit sendingComplete, */
            /*  to be considered as able to Overlap Send                                 */
            pvtGetChildByFieldId(hVal,perNodeId,__q931(canOverlapSend),(RvInt32*)&remoteCOS,NULL);
            if (remoteCOS==RV_TRUE)
                m_callset(ce,remoteCanOverlapSend,!(pvtGetChild(hVal,q931NodeId,__q931(sendingComplete),NULL)>=0));
        }
        /* Get the Conference Goal */
        tmpNodeId = pvtGetChild(hVal, perNodeId, __q931(conferenceGoal), NULL);
        {
            int iConferenceGoal = pvtGetSyntaxIndex(hVal, pvtChild(hVal, tmpNodeId)) - 1;
            ce->conferenceGoal = (cmConferenceGoalType)iConferenceGoal;
            if (ce->conferenceGoal == cmCallIndependentSupplementaryService)
                cmCallSetParam((HCALL)ce, cmParamH245Stage, 0, (RvInt32)cmTransH245Stage_facility, NULL);
        }
        {
            /* Get the hopCount */
            RvInt32 value;
            if (pvtGetChildByFieldId(hVal, perNodeId, __q931(hopCount), &value, NULL) >= 0)
            {
                ce->hopCount = (RvUint8)value;
            }
        }

        /* CRV for specific calls should be between 1-32767 */
        cmLock((HAPP)app);
        app->crv %= 0x7fff;
        app->crv++;
        if ((ce->rascrv < 0) && (app->gatekeeper == RV_FALSE))
            ce->rascrv = app->crv;
        cmUnlock((HAPP)app);

        /*Getting the connection addresses*/
        {
            HSTRANSHOST hsTransHost;
            cmTransportAddress ta;
            cmTransGetSessionParam(hsTransSession,cmTransParam_host,(void*)&hsTransHost);
            cmTransGetHostParam(hsTransHost,cmTransHostParam_localAddress,&ta);
            if (ce->callSignalAddress<0)
                ce->callSignalAddress=pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);
            cmTAToVt(app->hVal,ce->callSignalAddress,&ta);

            cmTransGetHostParam(hsTransHost,cmTransHostParam_remoteAddress,&ta);
            if (ce->remoteCallSignalAddress<0)
                ce->remoteCallSignalAddress=pvtAddRoot(app->hVal,app->hAddrSyn,0,NULL);
            cmTAToVt(app->hVal,ce->remoteCallSignalAddress,&ta);
        }

        /* set the flags for call-proceeding and Alerting */
        {
            RvBool manualCallProceeding, manualAccept;
            manualCallProceeding = (pvtGet(hVal,pvtGetChild(hVal,app->q931Conf,__q931(manualCallProceeding),NULL),NULL,NULL,NULL,NULL)>=0);
            manualAccept         = (pvtGet(hVal,pvtGetChild(hVal,app->q931Conf,__q931(manualAccept),NULL),NULL,NULL,NULL,NULL)>=0);

            /* in case of automatic alerting we don't want to send the callProceeding, the
            alerting message will be enough */
            if((manualAccept) && (!manualCallProceeding))
            {
                m_callset(ce,sendCallProceeding,RV_TRUE);
            }
            if(!manualAccept)
            {
                m_callset(ce,sendAlerting,RV_TRUE);
            }
        }

        /*Processing the Setup message*/
        {
            RvPvtNodeId nodeId=pvtAddRoot(hVal,NULL,0,NULL);
            pvtSetTree(hVal,nodeId,hVal,message);

            /* put the new SETUP message into the call property */
            callSetMessage((HCALL)ce, cmQ931setup, nodeId);
            q931SimulateSetup(cmiGetQ931((HCALL)ce));

            pvtDelete(hVal,nodeId);
        }

        /* Notify application about the new incoming call */
        if(!m_callget(ce,notified))
        {
            m_callset(ce,notified,RV_TRUE);
            if(app->cmMyEvent.cmEvNewCall)
            {
                HAPPCALL haCall = NULL;
                enqueueDummyState(ce);
                cmiCBEnter(((HAPP)app, "cmEvNewCall(hApp=%p,hsCall=%p,lphaCall)", app, ce));
                nRet=app->cmMyEvent.cmEvNewCall((HAPP)app, (HCALL)ce, &haCall);
                emaSetApplicationHandle((EMAElement)ce, (void*)haCall);
                cmiCBExit(((HAPP)app, "cmEvNewCall(*lphaCall=%p)", emaGetApplicationHandle((EMAElement)ce)));
                dequeueCallStates(app, ce, cmCallStateInit, (cmCallStateMode_e)-1);

                if (app->mibEvent.h341AddNewCall)
                    app->mibEvent.h341AddNewCall(app->mibHandle, (HCALL)ce);
            }
        }

        emaReleaseLocked((EMAElement)ce);

		//{{gaoshizhong add 20140715 process cmEvNewCall return value
		if(nRet < 0) return cmTransErr;
		else
		//}}gaoshizhong add 20140715 process cmEvNewCall return value
		{
        /* Count this call... */
        cmLock((HAPP)app);
        app->numberOfConnectingCalls++;
        m_callset(ce,connectingCall,RV_TRUE);
        cmUnlock((HAPP)app);
		}

        return cmTransOK;
    }
    return cmTransErr;
}


/**************************************************************************************
 * cmEvTransWantsToStartControl
 *
 * Purpose: To ask the CM if it is ok to connect control or send tunneled H.245
 *          messages. (will return RV_FALSE if the GK has not approved the call yet)
 *
 * Input:   hsTransSession      - The stack handle of the session.
 *          haTransSession      - The application handle of the session.
 *
 **************************************************************************************/
RvBool cmEvTransWantsToStartControl(IN HSTRANSSESSION    hsTransSession,
                                    IN HATRANSSESSION    haTransSession)
{
    callElem* call=(callElem*)haTransSession;
    cmElem* app;

    RV_UNUSED_ARG(hsTransSession);

    if (call == NULL)
        return RV_FALSE;

    app = (cmElem*)emaGetInstance((EMAElement)call);

    return (m_callget(call,callInitiator) || app->manualRAS || m_callget(call,dummyRAS) || (call->state != cmCallStateInit) || !m_callget(call, automaticRAS));
}


/**************************************************************************************
 * cmEvTransTunnelingWasRejected
 *
 * Purpose: To let the CM know that tunneling was rejected, and that the sent TCS/MSD
 *          messages will not be receiving acknowledgement soon.
 *
 * Input:   hsTransSession      - The stack handle of the session.
 *          haTransSession      - The application handle of the session.
 *
 **************************************************************************************/
TRANSERR cmEvTransTunnelingWasRejected(IN HSTRANSSESSION    hsTransSession,
                                       IN HATRANSSESSION    haTransSession,
                                       IN RvBool            restartTimers)
{
    callElem* call=(callElem*)haTransSession;

    RV_UNUSED_ARG(hsTransSession);

    if ((call == NULL) || !emaMarkLocked((EMAElement)call))
        return cmTransErr;

    if (restartTimers)
    {
        cmiStartControlTimers((HCALL)call);
    }
    else
    {
        cmiStopControlTimers((HCALL)call);
    }

    emaReleaseLocked((EMAElement)call);
    return cmTransOK;
}


/**************************************************************************************
 * cmEvTransExtFastConnestState
 *
 * Purpose: To let the CM know that replacement EFC flags were received.
 *
 * Input:   hsTransSession      - The stack handle of the session.
 *          haTransSession      - The application handle of the session.
 *
 **************************************************************************************/
TRANSERR cmEvTransExtFastConnestState(IN HSTRANSSESSION    hsTransSession,
                                      IN HATRANSSESSION    haTransSession,
                                      IN EFCSTATE          efcState)
{

    /* pass the information to the H.245 */
    callElem *call = (callElem*)haTransSession;
    cmElem   *app = (cmElem *)emaGetInstance((EMAElement)call);
    RvStatus ret;

    RV_UNUSED_ARG(hsTransSession);

    cmiCBEnter(((HAPP)app, "cmEvTransExtFastConnectState(hsCall=%p, efcState=%d)",
        emaGetApplicationHandle((EMAElement)call), efcState));

    ret = cmiSetRemoteExtendedFastConnectStatus((HCALL)haTransSession, m_callget(call,callInitiator), (cmExtendedFastConnectState_e)efcState);

    if (ret == RV_ERROR_NOTSUPPORTED)
    {
        /* drop the call */
        cmCallDropParam((HCALL)haTransSession, cmReasonTypeNeededFeatureNotSupported);
    }

    cmiCBExit(((HAPP)app, "cmEvTransExtFastConnestState: ret = %d",ret));
    RV_UNUSED_ARG(app);
    return cmTransOK;
}


/**************************************************************************************
 * cmEvTransNatAddressTranslation (CALLBACK)
 *
 * Purpose: To let the CM know that a NAT translation is needed.
 *
 * Input:   hsTransSession      - The stack handle of the session.
 *          haTransSession      - The application handle of the session.
 *          addrType            - The address type (ras , q931 or h245)
 *          address             - The local address to translate.
 * Output:  address             - The NAT translation for the local address.
 **************************************************************************************/
TRANSERR cmEvTransNatAddressTranslation(IN    HSTRANSSESSION      hsTransSession,
                                        IN    HATRANSSESSION      haTransSession,
                                        IN    int                 addrType,
                                        INOUT cmTransportAddress* address)
{
    callElem* call = (callElem*)haTransSession;
    cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);

    RvChar addrStr[RV_TRANSPORT_ADDRESS_STRINGSIZE];

    RV_UNUSED_ARG(hsTransSession);

    if (emaMarkLocked((EMAElement)call))
    {
        if (app->cmMyCallEvent.cmEvCallNatRequest)
        {
            int nesting;

            memset(addrStr , 0 , sizeof(addrStr));
            cmTransportAddress2String(address , sizeof(addrStr) , addrStr);

            cmiCBEnter(((HAPP)app, "cmEvCallNATRequest(haCall=%p,hsCall=%p,addrType=%d,address=%s)",
                emaGetApplicationHandle((EMAElement)call), call, addrType, addrStr));
            nesting = emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallNatRequest((HAPPCALL)emaGetApplicationHandle((EMAElement)call), (HCALL)call, (cmProtocol)addrType, address);
            emaReturnFromCallback((EMAElement)call,nesting);
            cmiCBExit(((HAPP)app, "cmEvCallNATRequest"));
        }
        emaReleaseLocked((EMAElement)call);
    }
    return cmTransOK;
}


/**************************************************************************************
 * cmEvTransParentChangedT (CALLBACK)
 *
 * Purpose: To change the parent lock of the call to that of the session.
 *
 * Input:   hsTransSession      - The stack handle of the session.
 *          haTransSession      - The application handle of the session.
 *
 **************************************************************************************/
TRANSERR cmEvTransParentChanged(IN HSTRANSSESSION hsTransSession,
                                IN HATRANSSESSION haTransSession)
{
    if (haTransSession != NULL)
    {
        RvStatus ret;
        callElem* call = (callElem*)haTransSession;
        cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);

        ret = transParentShare(app->hTransport, (void*)call, (void*)hsTransSession);
        if (ret != RV_OK)
            return cmTransErr;
    }
    return cmTransOK;
}


TRANSERR cmEvTransConnectionOnSessionClosed(IN HSTRANSSESSION hsTransSession,
                                            IN HATRANSSESSION haTransSession,
                                            IN TRANSCONNTYPE  type)
{
    callElem* call=(callElem*)haTransSession;
    cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);
    RV_UNUSED_ARG(hsTransSession);
    switch(type)
    {
        case cmTransQ931Conn:
        {
            if (emaMarkLocked((EMAElement)call))
            {
                /* see if configuration (keepCallOnQ931Close), the call state and the control state allow us
                to drop the call */
                if (call->state!=cmCallStateIdle &&
                    (pvtGetChild(app->hVal, app->q931Conf, __q931(keepCallOnQ931Close), NULL) < 0 ||
                    !cmiGetIsControl((HCALL)call)))
                {
                    if(!cmiGetIsControl((HCALL)call))
                    {
                        q931CallDrop(cmiGetQ931((HCALL)call), -1, -1);
                    }

                    reportDisconnectedState(call, cmCallDisconnectionReasonQ931Closed);
                }
                /* Let the world know that this call no longer has a Q931 session (no matter if we dropped it or
                not - the other side did) */
                m_callset(call,callWithoutQ931,RV_TRUE);
                emaReleaseLocked((EMAElement)call);
            }
        }
        break;
        case cmTransH245Conn:
        {
            if (emaMarkLocked((EMAElement)call))
            {
                /* We only have to handle this H.245 connection if the call hasn't reached the IDLE state */
                /* make sure we didn't already disconnect the call */
                if ((call->state != cmCallStateIdle) && !m_callget(call,controlDisconnected))
                {
                    dropControl(call);
                    RvH323TimerCancel(app->hTimers, &call->timer);

                    /* In case that this is a real call whose control has been disconnected, we
                       want to give a chance to the releaseComplete to arrive.
                       If it's just a dummy call for the control, no releaseComplete will ever
                       arrive */
                    if ((!emaWasDeleted((EMAElement)call)) && (call->state != cmCallStateInit))
                    {
                        if (m_callget(call,callWithoutQ931))
                        {
                            /* There is no Q931 - no use in waiting for RelComp */
                            call->timer = RvH323TimerStartWithType(app->hTimers,
                                controlDiedTimerEventsHandler, call, RvInt64Const(1, 0, 0), RV_TIMER_TYPE_ONESHOT);
                        }
                        else
                        {
                            if (app->postControlDisconnectionDelay)
                                call->timer = RvH323TimerStart(app->hTimers,
                                controlDiedTimerEventsHandler, call, app->postControlDisconnectionDelay);
                        }
                        m_callset(call,controlDisconnected,RV_TRUE);
                    }
                }
                emaReleaseLocked((EMAElement)call);
            }
        }
        break;
    }
    return cmTransOK;
}


TRANSERR cmEvTransAltEpConnection(IN HSTRANSSESSION hsTransSession,
                                  IN HATRANSSESSION haTransSession)
{
    callElem* call=(callElem*)haTransSession;
    HSTRANSHOST host = NULL;
    RvStatus status = RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)call))
    {
        cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
        cmTransportAddress tpkt;

        RV_UNUSED_ARG(app);
        RvLogInfo(&app->log, (&app->log, "AltEpNum=%d", call->altEpNum));

        /* first time trying to connect Alt EP the calls altEpNum is set to -1 at call's init time */
        if (call->altEpNum < 0)
        {
            /* Set call's altEpNum to 0 and than increase it by one. */
            call->altEpNum = 0;
        }

        /* get 'next' altEP address from the call */
        call->altEpNum++;
        status = cmCallGetAltEpCallSignalAddress(call, &tpkt);
        if (status == RV_OK)
        {
            cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);

            RvLogInfo(&app->log, (&app->log, "AltEpNum - handling next address"));

            /* set new destination into Setup if the host will be connected */
            if (app->altEpModifySetup)
                cmCallSetAltEpDestination((HCALL)call);

            cmTransGetSessionParam(hsTransSession, cmTransParam_host, (void *)&host);
            if (host)
            {
                cmTransHostTpktClose(host);

                /* set altEP address and try to connect it */
                cmTransHostTpktSetAddress(host, &tpkt);
                cmTransHostTpktConnect(call->hsTransSession);
            }
            else status=RV_ERROR_UNKNOWN;
        }
        emaUnlock((EMAElement)call);
    }

    if (status == RV_OK)
        return cmTransOK;
    else if (status==RV_ERROR_NOT_FOUND)
        return cmTransErrNoParty;
    else /* general error */
        return cmTransErr;
}


/************************************************************************
 * enterCallInitState
 * purpose: Set the state of the call to its init state.
 *          This state is the first the call enters
 * input  : call    - Call object
 * output : none
 * return : none
 ************************************************************************/
static void enterCallInitState(IN callElem* call)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    RvInt32 t301=180,t303,t304=4,t310,t322=4;

    /* Get the timeout values for call setup related timeouts */
    pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(responseTimeOut), &t304, NULL);
    t303 = t304 * 1000;
    pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(t304), &t304, NULL);
    if (t304 > 0) /* We do not tolerate no timeout on T304 */
        t304 *= 1000;
    else
        t304 = t303;
    pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(connectTimeOut), &t301, NULL);
    t301 *= 1000;
    pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(t322), &t322, NULL);
    t322 *= 1000;
    if (pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(t310), &t310, NULL) >= 0)
        t310 *= 1000;
    else
        t310 = t301;

    /* Create the Q931 "object" of the call */
    q931CallCreate((HQ931)cmiGetQ931((HCALL)call), t301, -1, t303, t304, t310, t322);

    /* Set the CID and CallID for the call */
    RvH323UniqueIdGeneratorGet(app->guidGenerator, call->callId);
    if (m_callget(call,newCIDRequired))
    {
        RvH323UniqueIdGeneratorGet(app->guidGenerator, call->cId);
        m_callset(call,newCIDRequired,0);
    }

    /* Set the CRV for the call and update it in the stack */
    /* CRV for specific calls should be between 1-32767 */
    cmLock((HAPP)app);
    app->crv %= 0x7fff;
    app->crv++;
    call->crv = app->crv;
    cmUnlock((HAPP)app);

    /* For non-gatekeeper applications, we set the Q931-CRV and the RAS-CRV to the same value */
    if (!app->gatekeeper)
        call->rascrv = call->crv;
    m_callset(call,callInitiator,RV_TRUE);
    cmTransCreateSession(app->hTransport, (HATRANSSESSION)call, NULL, &(call->hsTransSession));
    cmCallSetParam((HCALL)call, cmParamEstablishH245, 0, !m_callget(call,notEstablishControl), NULL);
}


TRANSERR cmEvTransSessionNewConnection( IN HSTRANSSESSION   hsTransSession,
                                        IN HATRANSSESSION   haTransSession,
                                        IN TRANSCONNTYPE    type)
{
    callElem* call=(callElem*)haTransSession;

    RV_UNUSED_ARG(hsTransSession);

    if (!emaMarkLocked((EMAElement)call))
        return cmTransErr;

    /* Make sure we are not currently in the process of starting to disconnect this call. */
    if ((call->state == cmCallStateDisconnected) || (call->state == cmCallStateIdle))
    {
        emaReleaseLocked((EMAElement)call);
        return cmTransErr;
    }

    switch(type)
    {
        case cmTransQ931Conn:
        {
            cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);

            int message=callGetMessage((HCALL)call,cmQ931setup);
            int tmpNodeId;

            __pvtBuildByFieldIds(tmpNodeId, app->hVal,message , {_q931(message) _q931(setup)
                _q931(userUser) _q931(h323_UserInformation) _q931(h323_uu_pdu) _q931(h323_message_body) _q931(setup)
                LAST_TOKEN},0,NULL);

            /* Set the hop count value if needed */
            if (call->hopCount > 0)
                pvtAdd(app->hVal, tmpNodeId, __q931(hopCount), call->hopCount, NULL, NULL);

            if (cmCallGetParam((HCALL)call, cmParamSrcCallSignalAddress, 0, NULL, NULL) < 0)
            {
                HSTRANSHOST host = NULL;
                int srcAddrNodeId;
                cmTransportAddress ta;
                memset(&ta, 0, sizeof(ta));

                cmTransGetSessionParam(call->hsTransSession, cmTransParam_host, (void *)&host);
                if (host)
                {
                    /*address in message should be taken from localAddressInMsg
                      field in case it is a translation*/
                    cmTransGetHostParam(host, cmTransHostParam_localAddressInMsg, (void *)&ta);
                }

                srcAddrNodeId = pvtAdd(app->hVal, tmpNodeId, __q931(sourceCallSignalAddress), 0, NULL, NULL);
                cmTAToVt(app->hVal, srcAddrNodeId, &ta);
            }

            if (m_callget(call, gatekeeperRouted))
            {
                char eId[256];
                int eIdLen=cmiAutoRASGetEndpointID((HAPP)app,eId);
                if (eIdLen>0)
                    pvtAdd(app->hVal,tmpNodeId,__q931(endpointIdentifier),eIdLen,eId,NULL);
            }

            __pvtBuildByFieldIds(tmpNodeId,app->hVal,message, {_q931(message) _q931(setup)
                _q931(bearerCapability) _q931(octet4) LAST_TOKEN},0,NULL);
            if (m_callget(call,multiRate))
            {
                pvtAdd(app->hVal,tmpNodeId,__q931(informationTransferRate),(call->rate<=64000)?16:24,NULL,NULL);
                if (call->rate>64000)
                    pvtAdd(app->hVal,tmpNodeId,__q931(rateMultiplier),(call->rate-1)/64000+1,NULL,NULL);

            }
            else
            {
                int rateVal=0;
                if (call->rate==0) rateVal=0;
                else if (call->rate<=64000) rateVal=16;
                else if (call->rate<=128000) rateVal=17;
                else if (call->rate<=384000) rateVal=19;
                else if (call->rate<=1536000) rateVal=21;
                else if (call->rate<=1920000) rateVal=23;
                pvtAdd(app->hVal, tmpNodeId, __q931(informationTransferRate), rateVal, NULL, NULL);
            }


            notifyState((HCALL)call,cmCallStateDialtone,(cmCallStateMode_e)0);
            if (!emaWasDeleted((EMAElement)call))
            {
                if (q931CallDial(cmiGetQ931((HCALL)call), message) != RV_OK)
                {
                    reportDisconnectedState(call, cmCallDisconnectionReasonUnknown);
                }
                if (app->mibEvent.h341AddNewCall)
                    app->mibEvent.h341AddNewCall(app->mibHandle, (HCALL)call);
            }
        }
        break;

        case cmTransH245Conn:
        {
            RvInt32 isParallel;
            HCONTROL ctrl = cmiGetControl((HCALL)call);
            RvInt32 terminalType;

            if (m_callget(call, setTerminalType))
                terminalType = call->msdTerminalType;
            else
                terminalType = -1;
            initControl(ctrl, RvH245ChannelParamsH225, terminalType);
            cmiSetIsControl((HCALL)call, RV_TRUE);

            cmCallGetParam((HCALL)call, cmParamH245Parallel, 0, &isParallel, NULL);
            if (isParallel != 0)
            {
                cmiSetIsFastStartFinished((HCALL)call, RV_FALSE);
            }

            cmiReportControl(ctrl, cmControlStateTransportConnected, (cmControlStateMode)0);
            startControl(ctrl);
        }
        break;
    }
    emaReleaseLocked((EMAElement)call);
    return cmTransOK;
}


/************************************************************************
 * cmCallNew
 * purpose: Creates a new call object that belongs to a particular Stack instance.
 *          This function does not launch any action on the network. It only causes
 *          the application and the Stack to exchange handles.
 * input  : hApp        - Stack handle for the application
 *          haCall      - Application's handle for the call
 * output : lphsCall    - Stack handle for the call to dial
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallNew(
        IN   HAPP        hApp,
        IN   HAPPCALL    haCall,
        OUT  LPHCALL     lphsCall)
{
    cmElem* app=(cmElem*)hApp;
    HPVT hVal;
    callElem call;
    int someId;
    RvBool boolParam;
    if (!hApp)
        return RV_ERROR_UNKNOWN;

    cmiAPIEnter(((HAPP)app, "cmCallNew(hApp=%p,haCall=%p,lphsCall)", hApp, haCall));

    hVal = app->hVal;

    /* Create a call struct and set the default values inside it */
    cmCallMemSet(&call);

    m_callset(&call, automaticRAS, !app->manualRAS);
    m_callset(&call, notified, RV_TRUE); /* No need to notify app about this call - app created it */
    m_callset(&call, overrideCID, RV_TRUE);
    m_callset(&call, newCIDRequired, RV_TRUE);
    m_callset(&call, callWithoutQ931, RV_FALSE);
    m_callset(&call, enableOverlapSending, (pvtGet(hVal,pvtGetChild(hVal,app->q931Conf,__q931(overlappedSending),NULL),NULL,NULL,NULL,NULL) >= 0));
    m_callset(&call, notEstablishControl, (pvtGetChild(hVal,app->q931Conf,__q931(notEstablishControl),NULL)>=0));
    m_callset(&call, multiRate, RV_TRUE);
    m_callset(&call, callSupportNat, (app->cmNATMode == cmNatModeOnlyNAT));

    {
        int nodeId;
        RvInt32 annexE=0;

        call.annexE = cmTransNoAnnexE;
        if ((nodeId=pvtGetChild(hVal,app->q931Conf,__q931(useAnnexE),NULL))>=0)
        {
            int iAnnexE = pvtGetSyntaxIndex(hVal,pvtChild(hVal,pvtGetChild(hVal,nodeId,__q931(protocolPreference),NULL)))-1;
            if (iAnnexE >= 0)
                call.annexE=(cmAnnexEUsageMode)iAnnexE;
            else
            {
                pvtGetChildByFieldId(hVal,nodeId,__q931(defaultProtocol),&annexE,NULL);
                if (annexE)
                    call.annexE=cmTransUseAnnexE;
            }
        }
    }

    someId=pvtGetChild(hVal,app->rasConf,__q931(preGrantedArqUse),NULL);
    if (pvtGetChild(hVal,someId,__q931(routed),NULL) >= 0)
        call.preGrantedArqUse=cmPreGrantedArqRouted;
    if (pvtGetChild(hVal,someId,__q931(direct),NULL) >= 0)
        call.preGrantedArqUse=cmPreGrantedArqDirect;

    {
        /* Make sure the application is not trying to open more calls than Q931.maxCalls */
        RvInt32 maxCalls;
        pvtGetChildByFieldId(hVal, app->q931Conf, __q931(maxCalls), &maxCalls, NULL);
        if (app->busy >= maxCalls)
        {
            cmiAPIExit(((HAPP)app, "cmCallNew: Too many calls opened"));
            return RV_ERROR_OUTOFRANGE;
        }
    }

    /* Allocate a call in EMA */
    *lphsCall = (HCALL)emaAdd(app->hCalls, (void*)haCall);
    if (*lphsCall == NULL)
    {
        cmiAPIExit(((HAPP)app, "cmCallNew=%d", RV_ERROR_OUTOFRESOURCES));
        return RV_ERROR_OUTOFRESOURCES;
    }
    if (transParentNew(app->hTransport, (void*)*lphsCall) != RV_OK)
    {
        emaDelete((EMAElement*)*lphsCall);
        cmiAPIExit(((HAPP)app, "cmCallNew=%d", RV_ERROR_OUTOFRESOURCES));
        return RV_ERROR_OUTOFRESOURCES;
    }
    memcpy((void*)*lphsCall, &call, sizeof(call));

    cmLock((HAPP)app);
    app->busy++;
    cmUnlock((HAPP)app);

    clearControl(cmiGetControl((HCALL)*lphsCall), RV_TRUE, sizeof(callElem) + sizeof(q931Elem));
    fastStartCallInit((HCALL)*lphsCall);
    if (!app->manualRAS)
        cmiAutoRASCallCreate((HCALL)*lphsCall);

    /* Initialize the property database for the call if we should */
    callInitParameters((HAPP)app, *lphsCall);

    /* Set the terminal type of the call */
    __pvtGetNodeIdByFieldIds(someId,hVal,app->rasConf, {_q931(registrationInfo) _q931(terminalType) LAST_TOKEN});
    if (someId >= 0)
        cmCallSetParam((HCALL)*lphsCall, cmParamFullSourceInfo, 0, someId ,NULL);
    else
    {
        __pvtBuildByFieldIds(someId, hVal, app->rasConf, {_q931(registrationInfo) _q931(terminalType) LAST_TOKEN}, 0, NULL);
        pvtAdd(hVal, someId, __q931(mc), 0, NULL, NULL);
        pvtAdd(hVal, someId, __q931(undefinedNode), 0, NULL, NULL);
        cmCallSetParam((HCALL)*lphsCall, cmParamFullSourceInfo, 0, someId, NULL);
    }

    /* Make sure call is in its init state */
    enterCallInitState((callElem*)*lphsCall);

    /* transport layer defaults - we set them after enterCallInitState() since only here we've
       got a session for them */
    boolParam = (pvtGetChild(hVal,app->q931Conf,__q931(h245Tunneling),NULL) >= 0);
    cmCallSetParam((HCALL)*lphsCall, cmParamH245Tunneling, 0, (RvInt32)boolParam, NULL);

    cmCallSetParam((HCALL)*lphsCall, cmParamH245Parallel, 0, RV_FALSE, NULL);
    cmCallSetParam((HCALL)*lphsCall, cmParamIsMultiplexed, 0, RV_FALSE, NULL);
    cmCallSetParam((HCALL)*lphsCall, cmParamShutdownEmptyConnection, 0, RV_TRUE, NULL);

    boolParam = (pvtGetChild(hVal,app->q931Conf,__q931(earlyH245),NULL)>=0);
    cmCallSetParam((HCALL)*lphsCall, cmParamH245Stage, 0,
        (RvInt32)((boolParam)?cmTransH245Stage_early:cmTransH245Stage_connect), NULL);

    cmCallSetParam((HCALL)*lphsCall, cmParamSupportNat, 0 , (app->cmNATMode == cmNatModeOnlyNAT), NULL);

    /* Make sure we notify the MIB about this call. */
    if (app->mibEvent.h341AddNewCall)
        app->mibEvent.h341AddNewCall(app->mibHandle, (HCALL)*lphsCall);

    cmiAPIExit(((HAPP)app, "cmCallNew(*lphsCall=%p)=0", *lphsCall));
    return 0;
}


/************************************************************************
 * callAnswer
 * purpose: Connect an incoming call.
 *          Set the display parameter to the host's name if we've got
 *          display information from the originator of the call
 * input  : call        - Stack handle for the call to dial
 *          bSetDisplay - false iff we do not want to set the hostname as
 *                        the display.
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int callAnswer(callElem*call, RvBool bSetDisplay)
{
    int status = RV_ERROR_UNKNOWN;

    if (bSetDisplay)
    {
        RvUint32 size=1;
        char display[2];
        strcpy(display, "");

        cmCallGetParam((HCALL)call, cmParamConnectDisplay, 0, (RvInt32*)&size, display);
        if (!display[0])
        {
            /* Set the display parameter for the Connect message to the local host's name */
            RvChar displayName[255];
            if (RvHostLocalGetName(sizeof(displayName), NULL, displayName) == RV_OK)
                cmCallSetParam((HCALL)call, cmParamConnectDisplay, 0,
                    (RvInt32)strlen(displayName), displayName);
        }
    }

    /* Answer with CONNECT */
    status = q931CallAnswer(cmiGetQ931((HCALL)call), RV_PVT_INVALID_NODEID);

    /* Make sure the state of the call is connected */
    if (status >= 0)
        enterCallConnectedState(call);

    return status;
}

/************************************************************************
 * enterCallSetupState
 * purpose: Handle an incoming setup message
 *          See if we're replying automatically and notify the application
 * input  : call    - Stack handle for the call to dial
 * output : none
 * return : none
 ************************************************************************/
static void enterCallSetupState(IN callElem* call)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);

    /*We enter offering state only once*/
    if ((call->state!=cmCallStateOffering) && (call->state!=cmCallStateIdle))
    {
        /* Notify the application about the incoming call's parameters */
        if (app->cmMyCallEvent.cmEvCallInfo)
        {
            char display[128];
            char userUser[128];
            RvInt32 displaySize=sizeof(display)-1;
            RvInt32 userUserSize=sizeof(userUser);
            int nesting;

            if (cmCallGetParam((HCALL)call,cmParamDisplay,0,&displaySize,display) < 0)
                displaySize = 0;
            if (displaySize < 128)
                display[displaySize] = 0;
            if (cmCallGetParam((HCALL)call,cmParamUserUser,0,&userUserSize,userUser) < 0)
                userUserSize = 0;
            if (userUserSize < 128)
                userUser[userUserSize] = 0;

            cmiCBEnter(((HAPP)app, "cmEvCallInfo(haCall=%p,hsCall=%p,display=%s,userUser=%s,userUserSize=%d)",
                emaGetApplicationHandle((EMAElement)call),
                call, nprn(display), nprn(userUser), userUserSize));
            nesting=emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallInfo((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,display,userUser,(int)userUserSize);
            emaReturnFromCallback((EMAElement)call,nesting);
            cmiCBExit(((HAPP)app, "cmEvCallInfo"));
        }

        /* Notify the change of state (=offering) */
        if ((!emaWasDeleted((EMAElement)call)) && (call->state != cmCallStateConnected) && (call->state != cmCallStateDisconnected))
        {
            /* The state mode of this state is the conference goal */
            static const cmCallStateMode_e confGoalStateModes[] = {
                cmCallStateModeOfferingCreate,
                cmCallStateModeOfferingJoin,
                cmCallStateModeOfferingInvite,
                cmCallStateModeOfferingCapabilityNegotiation,
                cmCallStateModeOfferingCallIndependentSupplementaryService};
            if ((((int)call->conferenceGoal) >= 0) && ((int)call->conferenceGoal < (int)cmLastCG))
                notifyState((HCALL)call, cmCallStateOffering, confGoalStateModes[call->conferenceGoal]);
        }

        if (!emaWasDeleted((EMAElement)call) && (call->hsTransSession != NULL))
        {
            if(m_callget(call,sendCallProceeding))
            {
                m_callset(call, sendCallProceeding, RV_FALSE);
                q931CallCallProceeding(cmiGetQ931((HCALL)call),-1);
            }
            /* trigger H.245 connection if nobody closed this call */
            cmTransTryControlAfterACF(call->hsTransSession);
        }
    }
}

static RvBool timerEventsHandler(void*context)
{
    callElem* call=(callElem*)context;
    cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);

    cmCallPreCallBack((HAPP)app);

    if (emaLock((EMAElement)call))
    {
        RvH323TimerClear(app->hTimers, &call->timer);

        /* do we have Alternate Endpoints to connect with ? */
        if (RV_PVT_NODEID_IS_VALID(call->altEpProperty))
        {
            /* first time trying to connect Alt EP the calls altEpNum is set to -1 at call's init time */
            if (call->altEpNum < 0)
            {
                /* Set call's altEpNum to 0 (it will be increased it by one to indicate the first). */
                call->altEpNum = 0;
            }

            /* If there were no TCP connection (call remains in Init state - try to connect AltEP */
            if (call->state == cmCallStateInit)
            {
                HSTRANSHOST host = NULL;
                RvStatus status = RV_ERROR_UNKNOWN;
                cmTransportAddress tpkt;

                /* get 'next' altEP address from the call */
                call->altEpNum++;

                /* try to connect with "next" AltEp */
                status = cmCallGetAltEpCallSignalAddress(call, &tpkt);
                if (status == RV_OK)
                {
                    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);

                    RvLogInfo(&app->log, (&app->log, "AltEpNum - handling next address"));

                    /* set new destination into Setup if the host will be connected */
                    if (app->altEpModifySetup)
                        cmCallSetAltEpDestination((HCALL)call);

                    cmTransGetSessionParam(call->hsTransSession, cmTransParam_host, (void *)&host);
                    if (host)
                    {
                        RvInt32 timeout = 3;

                        cmTransHostTpktClose(host);

                        /* set altEP address and try to connect it.
                           The Response timer must be set again */
                        pvtGet(app->hVal,pvtGetChild(app->hVal,app->q931Conf,__q931(responseTimeOut),NULL),NULL,NULL,&timeout,NULL);
                        timeout*=1000;
                        call->timer = RvH323TimerStart(app->hTimers, timerEventsHandler, call, timeout);

                        cmTransHostTpktSetAddress(host, &tpkt);
                        cmTransHostTpktConnect(call->hsTransSession);
                    }
                }
            }
            else
            {   /* inform application about "no answer" from EP */
                notifyState((HCALL)call, call->state, cmCallStateModeSetupNoReply);
            }
        }
        else
        {
            /* no AltEP */
            /* if no cause value, set cause value 102, recovery on timer expiry */
            if (cmCallGetParam((HCALL)call,cmParamReleaseCompleteCause,0,NULL,NULL)<0)
                cmCallSetParam((HCALL)call,cmParamReleaseCompleteCause,0,102,NULL);

            q931CallDrop(cmiGetQ931((HCALL)call), -1, -1);
            reportDisconnectedState(call, cmCallDisconnectionReasonNoDialResponse);
        }

        emaUnlock((EMAElement)call);
    }
    return RV_FALSE;
}




/************************************************************************
 * callNewRate
 * purpose: Notify the application about the rate of the call
 * input  : call      - Stack handle for the call
 * output : none
 * return : none
 ************************************************************************/
void callNewRate(IN callElem* call)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    if (app->cmMyCallEvent.cmEvCallNewRate)
    {
        int nesting;
        cmiCBEnter(((HAPP)app, "cmEvCallNewRate(haCall=%p,hsCall=%p,rate=%ld)",(HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,call->rate));
        nesting=emaPrepareForCallback((EMAElement)call);
        app->cmMyCallEvent.cmEvCallNewRate((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call, (RvUint32)call->rate);
        emaReturnFromCallback((EMAElement)call,nesting);
        cmiCBExit(((HAPP)app, "cmEvCallNewRate"));
    }
}


/************************************************************************
 * enterCallConnectedState
 * purpose: Raises the connected call state (called while locked).
 * input  : call      - Stack handle for the call
 * output : none
 * return : none
 ************************************************************************/
static void enterCallConnectedState(IN callElem* call)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);
    cmiAPIEnter(((HAPP)app, ">>---------enterCallConnectedState(app->numberOfConnectingCalls=%d)", app->numberOfConnectingCalls));
    emaMarkLocked((EMAElement)call);

    /* Reset the call's timer */
    RvH323TimerCancel(app->hTimers, &call->timer);

    if (m_callget(call,connectingCall))
    {
        cmLock((HAPP)app);
        app->numberOfConnectingCalls--;
        m_callset(call,connectingCall,RV_FALSE);
        cmUnlock((HAPP)app);
    }

    /* Notify application about the state */
    notifyState((HCALL)call, cmCallStateConnected, cmCallStateModeConnectedCallSetup);

    /* Application didn't drop the call in the callback - we continue notifying */
    if (!emaWasDeleted((EMAElement)call))
        callNewRate(call);
    emaReleaseLocked((EMAElement)call);
	cmiAPIExit(((HAPP)app, ">>---------enterCallConnectedState(app->numberOfConnectingCalls=%d)", app->numberOfConnectingCalls));
}


/************************************************************************
 * cmCallDial
 * purpose: Dials a call. This function together with cmCallSetParam()
 *          can be used instead of cmCallMake(). cmCallSetParam() should
 *          be called before cmCallDial() and the required parameters of
 *          the call should be set
 * input  : hsCall      - Stack handle for the call to dial
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallDial(
        IN  HCALL       hsCall)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    cmElem* app=(cmElem*)hApp;
    callElem*call=(callElem*)hsCall;
    RvInt32 timeout = 4;
    int ret = RV_ERROR_UNKNOWN;

    if (!hsCall || !hApp)
        return RV_ERROR_NULLPTR;

    cmiAPIEnter((hApp, "cmCallDial(hsCall=%p)", hsCall));

    if (emaLock((EMAElement)call))
    {
        RvInt32 maxConnectingCalls;
        RvInt32 maxCalls;

        /* Check that it is possible to dial out the call*/
        pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(maxCalls), &maxCalls, NULL);
        if (!RV_PVT_NODEID_IS_VALID(pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(maxConnectingCalls), &maxConnectingCalls, NULL)))
            maxConnectingCalls = maxCalls;

        if (app->numberOfConnectingCalls < maxConnectingCalls)
        {
            if (!m_callget(call,callDialInvoked) && m_callget(call,callInitiator))
            {
                RvBool hasTpktAddress;
                RvBool hasAnnexEAddress;

                ret = RV_OK;

                /* before we do anything stupid, let's check which addresses we have */
                hasTpktAddress = ((cmCallGetParam((HCALL)call,cmParamDestinationIpAddress,0,NULL,NULL) >= 0) ||
                    (cmCallGetParam((HCALL)call,cmParamDestCallSignalAddress,0,NULL,NULL) >= 0));
                hasAnnexEAddress = (cmCallGetParam((HCALL)call,cmParamDestinationAnnexEAddress,0,NULL,NULL) >= 0);

                if (hasTpktAddress && !hasAnnexEAddress) /* only TPKT */
                    cmCallSetParam((HCALL)call,cmParamAnnexE,0,(RvInt32)cmTransNoAnnexE,(char*)NULL);
                if (!hasTpktAddress && hasAnnexEAddress) /* only Annex E */
                    cmCallSetParam((HCALL)call,cmParamAnnexE,0,(RvInt32)cmTransUseAnnexE,(char*)NULL);

                if (!app->diffSrcAddressInSetupAndARQ)
                {
                    if ((cmCallGetParam((HCALL)call,cmParamSrcCallSignalAddress,0,NULL,NULL) < 0) &&
                        (hasTpktAddress || !hasAnnexEAddress))
                    {
                        cmTransportAddress ta;
                        memset(&ta, 0, sizeof(ta));
                        ta.type = cmTransportTypeIP;

                        if (cmTransSetAddress(call->hsTransSession, &ta, NULL, NULL, NULL, cmTransQ931Conn, RV_FALSE) != cmTransOK)
                            ret = RV_ERROR_UNKNOWN;
                        cmCallSetParam((HCALL)call,cmParamSrcCallSignalAddress,0,0,(char*)&ta);
                    }
                }

                cmCallSetParam((HCALL)call,cmParamSetupCanOverlapSending,0,(RvInt32)m_callget(call,enableOverlapSending),NULL);
                if (m_callget(call,enableOverlapSending))
                    cmCallSetParam((HCALL)call,cmParamSetupSendingComplete,0,RV_FALSE,NULL);
            }
            else
            {
                /* Seems like we tried to call this function twice or on the wrong side */
                RvLogError(&app->log, (&app->log, "Call %p already dialed or not originator", call));
                ret = RV_ERROR_UNKNOWN;
            }
        }
        else
        {
            /* Not enough resources... */
            RvLogWarning(&app->log, (&app->log, "Too many calls are currently being connected"));
            ret = RV_ERROR_OUTOFRESOURCES;
        }

        /* Dial the call */
        if (ret >= 0)
        {
            /* Set a timer for a response for the Setup */
            pvtGet(app->hVal,pvtGetChild(app->hVal,app->q931Conf,__q931(responseTimeOut),NULL),NULL,NULL,&timeout,NULL);
            timeout*=1000;
            RvH323TimerCancel(app->hTimers, &call->timer);
            call->timer = RvH323TimerStart(app->hTimers, timerEventsHandler, call, timeout);

            ret = callDial(call);

            if (ret >= 0)
            {
                /* Mark the call as a dialed one */
                m_callset(call,callDialInvoked,RV_TRUE);

                cmLock(hApp);
                app->numberOfConnectingCalls++;
                m_callset(call,connectingCall,RV_TRUE);
                cmUnlock(hApp);
            }
        }
        emaUnlock((EMAElement)call);
    }

    cmiAPIExit((hApp, "cmCallDial=%d", ret));
    return ret;
}


/************************************************************************
 * cmCallDialAltEp
 * purpose: Dials an alternate EP.
 * input  : hsCall   - Stack handle for the call to dial
 *        : altEpNum - The alt EP number from the alternateEndpoints list
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI  int RVCALLCONV cmCallDialAltEp(
    IN  HCALL       hsCall,
    IN  int         altEpNum)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    cmElem* app=(cmElem*)hApp;
    callElem*call=(callElem*)hsCall;
    int ret = RV_ERROR_UNKNOWN;
    
    if (!hsCall || !hApp)
        return RV_ERROR_NULLPTR;
    
    cmiAPIEnter((hApp, "cmCallDialAltEp(hsCall=%p, altEpNum=%d)", hsCall, altEpNum));
    
    if (emaLock((EMAElement)call))
    {
        ret = RV_OK;
        
        if (!app->diffSrcAddressInSetupAndARQ)
        {
            if (cmCallGetParam((HCALL)call,cmParamSrcCallSignalAddress,0,NULL,NULL) < 0)
            {
                cmTransportAddress ta;
                memset(&ta, 0, sizeof(ta));
                ta.type = cmTransportTypeIP;
                
                if (cmTransSetAddress(call->hsTransSession, &ta, NULL, NULL, NULL, cmTransQ931Conn, RV_FALSE) != cmTransOK)
                    ret = RV_ERROR_UNKNOWN;
                cmCallSetParam((HCALL)call,cmParamSrcCallSignalAddress,0,0,(char*)&ta);
            }
        }
        
        cmCallSetParam((HCALL)call,cmParamSetupCanOverlapSending,0,(RvInt32)m_callget(call,enableOverlapSending),NULL);
        if (m_callget(call,enableOverlapSending))
            cmCallSetParam((HCALL)call,cmParamSetupSendingComplete,0,RV_FALSE,NULL);
        
        /* "Dial" the call */
        if (ret == RV_OK)
        {
            HSTRANSHOST host = NULL;
            cmTransportAddress tpkt;
            
            call->altEpNum = altEpNum;
            
            /* modify the outgoing calledPartyNumber and destinationAddress (E164)
            in the H.225 setup message, based on the dialedDigits information
            in the aliasAddress parameter in the alternate endpoints field. */
            if (app->altEpModifySetup)
                cmCallSetAltEpDestination((HCALL)call);
            
            /* get 'next' altEP address from the CALL*/
            ret = cmCallGetAltEpCallSignalAddress(call, &tpkt);
            if (ret == RV_OK)
            {
                cmTransGetSessionParam(call->hsTransSession, cmTransParam_host, (void *)&host);
                if (host != NULL)
                {
                    RvInt32 timeout = 4;

                    /* close the old host */
                    cmTransHostTpktClose(host);

                    /* reset the call's timers */
                    q931CallClose((HQ931)cmiGetQ931((HCALL)call));
                    RvH323TimerCancel(app->hTimers, &call->timer);
                    
                    /* set altEP address ...     */
                    cmTransHostTpktSetAddress(host, &tpkt);

                    /* ... and try to connect to it, but before that set a
                    timer for a response for the Setup */
                    pvtGet(app->hVal,
                        pvtGetChild(app->hVal, app->q931Conf, __q931(responseTimeOut), NULL),
                        NULL, NULL, &timeout, NULL);

                    /* We also have a T303 timeout- responseTimeOut for outgoing
                    call after dial. We want that timerEventsHandler designated
                    for setup message will work first before T303 */

                    timeout*=1000;
                    call->timer = RvH323TimerStart(app->hTimers, timerEventsHandler, call, timeout);

                    cmTransHostTpktConnect(call->hsTransSession);
                }
                else
                    ret = RV_ERROR_UNKNOWN;
            }
        }
    }
    emaUnlock((EMAElement)call);
    cmiAPIExit((hApp, "cmCallDialAltEp=%d", ret));
    return ret;
}


/************************************************************************
 * cmCallAnswer
 * purpose: Answers an incoming call
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallAnswer(
        IN    HCALL       hsCall)
{
    HAPP hApp;
    RvStatus status = RV_ERROR_UNKNOWN;

    hApp = cmGetAHandle((HPROTOCOL)hsCall);
    if (hApp == NULL)
        return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallAnswer(hsCall=%p)", hsCall));
    status = cmCallAnswerExt(hsCall, NULL, NULL, 0);
    cmiAPIExit((hApp, "cmCallAnswer=%d", status));

    return status;
}

/************************************************************************
 * cmCallAccept
 * purpose: Accepts an incoming call and sends Q.931 ALERTING message to
 *          calling party
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallAccept(
        IN    HCALL       hsCall)
{
    int status = RV_ERROR_UNKNOWN;
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;

    if (!hsCall || !app) return RV_ERROR_NULLPTR;

    cmiAPIEnter(((HAPP)app, "cmCallAccept(hsCall=%p)", hsCall));

    if (emaLock((EMAElement)call))
    {
        if(call->state == cmCallStateInit)
        {
            /* call not really started yet. mark for send later. */
            m_callset(call, sendAlerting, RV_TRUE);
            status = RV_OK;
        }
        else
        {
            /* make sure to complete the address first */
            if(call->state == cmCallStateWaitAddressAck)
                cmCallAddressComplete(hsCall);

            /* Send Alerting message */
            status = q931CallAccept((HQ931)cmiGetQ931(hsCall),-1);

            /* Make sure we're in the right state (call setup) */
            if ((status >= 0) && (m_callget(call,dummyRAS)))
                enterCallSetupState(call);
        }

        emaUnlock((EMAElement)call);
    }

    cmiAPIExit(((HAPP)app, "cmCallAccept=%d", status));
    return status;
}

/************************************************************************
 * cmCallSendCallProceeding
 * purpose: Sends Q.931 CALL PROCEEDING message
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallSendCallProceeding(IN HCALL hsCall)
{
    int status = RV_ERROR_UNKNOWN;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallSendCallProceeding: hsCall=%p", hsCall));

    if (emaLock((EMAElement)call))
    {
        if(call->state == cmCallStateInit)
        {
            /* call not really started yet. mark for send later. */
            m_callset(call,sendCallProceeding,RV_TRUE);
            status = RV_OK;
        }
        else
        {
            /* make sure to complete the address first */
            if(call->state == cmCallStateWaitAddressAck)
                cmCallAddressComplete(hsCall);

            /* Send CallProceeding message */
            status = q931CallCallProceeding((HQ931)cmiGetQ931(hsCall),-1);

            /* Make sure we're in the right state (call setup) */
            if ((status >= 0) && (m_callget(call,dummyRAS)))
                enterCallSetupState(call);
        }

        emaUnlock((EMAElement)call);
    }

    cmiAPIExit((hApp, "cmCallSendCallProceeding=%d", status));
    return status;

}

/************************************************************************
 * callStopOK
 * purpose: We finished dropping the call.
 *          Notify the application we reached the Idle state on this call
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int callStopOK(IN callElem* call)
{
    cmElem *app = (cmElem*)emaGetInstance((EMAElement)call);

    RvH323TimerCancel(app->hTimers, &call->timer);

    if (call->state == cmCallStateTransfering)
    {
        /* On transfer, we initiate a new call dialing with the same handle */
        initiateDialOnTransfer(call, app);
    }
    else
    {
        notifyState((HCALL)call, cmCallStateIdle, (cmCallStateMode_e)0);
    }

    return 0;
}

/* The timeout was reached after H245 connection drop */
static RvBool controlDiedTimerEventsHandler(void*context)
{
    callElem* call=(callElem*)context;
    cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);

    cmCallPreCallBack((HAPP)app);
    if (emaLock((EMAElement)call))
    {
        RvH323TimerClear(app->hTimers, &call->timer);

        q931CallDrop(cmiGetQ931((HCALL)call), -1, -1);
        reportDisconnectedState(call, cmCallDisconnectionReasonH245Closed);
        emaUnlock((EMAElement)call);
    }
    return RV_FALSE;
}



/* On transfer, we initiate a new call dialing with the same handle */
static void initiateDialOnTransfer(callElem* call, cmElem* app)
{
    RvInt32 establish, tunneling, parallel, stage, multiplex, maintain;
    RvPvtNodeId keepH450 = RV_PVT_INVALID_NODEID;
    RvBool resetCall;
    RvStatus status;
    cmiAPIEnter(((HAPP)app, ">>---------initiateDialOnTransfer(app->numberOfConnectingCalls=%d)", app->numberOfConnectingCalls));

    /* Now that we're transferring, we better re-state the fact that this call was notified to the
       application, otherwise, we will never get a disconnected state on it... */
    m_callset(call, notified, RV_TRUE);

    resetCall = ((call->stateMode == cmCallStateModeTransferingForwarded) && !m_callget(call,callInitiator));

    /* Get parameters about this call before closing its session */
    cmCallGetParam((HCALL)call, cmParamEstablishH245,0,&establish,NULL);
    cmCallGetParam((HCALL)call, cmParamH245Parallel,0,&parallel,NULL);
    cmCallGetParam((HCALL)call, cmParamH245Tunneling,0,&tunneling,NULL);
    cmCallGetParam((HCALL)call, cmParamH245Stage,0,&stage,NULL);
    cmCallGetParam((HCALL)call, cmParamIsMultiplexed,0,&multiplex,NULL);
    cmCallGetParam((HCALL)call, cmParamShutdownEmptyConnection,0,&maintain,NULL);

    if (call->stateMode == cmCallStateModeTransferingRouteToGatekeeper)
    {
        int h4501SupplementaryService;

        __pvtGetByFieldIds(h4501SupplementaryService, app->hVal, cmGetProperty((HPROTOCOL)call),
            { _q931(setup) _q931(message) _q931(setup) _q931(userUser) _q931(h323_UserInformation)
              _q931(h323_uu_pdu) _q931(h4501SupplementaryService) LAST_TOKEN }, NULL, NULL, NULL);
        if(RV_PVT_NODEID_IS_VALID(h4501SupplementaryService))
        {
            keepH450 = pvtAddRoot(app->hVal, NULL, 0, NULL);
            pvtMoveTree(app->hVal, keepH450, h4501SupplementaryService);
        }
    }

    /* Close the transport session of the call */
    if (call->hsTransSession)
    {
        cmTransCloseSession(call->hsTransSession);
        call->hsTransSession=NULL;
    }

    if (resetCall)
    {
        /* we have to re-initialize the call */
        RvPvtNodeId sourceNode, addressNodeDest = RV_PVT_INVALID_NODEID, infoNodeDest = RV_PVT_INVALID_NODEID, addressNodeSrce = RV_PVT_INVALID_NODEID;
        RvInt32 sourceNodeParam;
        cmTransportAddress ta;
        cmAlias cpn;
        char cpnStr[128];

        /* remove the answering faststart, if it exists, and re-initialize it */
        deleteFastStart((HCALL)call);
        clearControl(cmiGetControl((HCALL)call), RV_TRUE, sizeof(callElem) + sizeof(q931Elem));
        fastStartCallInit((HCALL)call);

        /* get the destination aliases node */
        if (cmCallGetParam((HCALL)call, cmParamFullDestinationAddress, 0, &sourceNodeParam, (char*)NULL) >= 0)
        {
            sourceNode = (RvPvtNodeId)sourceNodeParam;
            addressNodeDest = pvtAddRoot(app->hVal, NULL, 0, NULL);
            pvtSetTree(app->hVal, addressNodeDest, app->hVal, sourceNode);
        }

        /* get the destination info node */
        if (cmCallGetParam((HCALL)call, cmParamFullDestinationInfo, 0, &sourceNodeParam, (char*)NULL) >= 0)
        {
            sourceNode = (RvPvtNodeId)sourceNodeParam;
            infoNodeDest = pvtAddRoot(app->hVal, NULL, 0, NULL);
            pvtSetTree(app->hVal, infoNodeDest, app->hVal, sourceNode);
        }

        /* get the dest IP address */
        if (cmCallGetParam((HCALL)call, cmParamDestinationIpAddress, 0, NULL, (char*)&ta) < 0)
        {
            ta.port = 0;
        }

        /* get the calling party number */
        cpn.string = cpnStr;
        cpn.length = sizeof(cpnStr);
        if (cmCallGetParam((HCALL)call, cmParamCalledPartyNumber, 0, NULL, (char*)&cpn) < 0)
        {
            cpn.length = 0;
        }

        /* Check if we have a source address */
        if (cmCallGetNumOfParams((HCALL)call, cmParamSourceAddress) > 0)
        {
            /* someone set the source address in cmCallStateTransfering - keep it */
            cmCallGetParam((HCALL)call, cmParamFullSourceAddress, 0, &sourceNodeParam, (char*)NULL);
            sourceNode = (RvPvtNodeId)sourceNodeParam;
            addressNodeSrce = pvtAddRoot(app->hVal, NULL, 0, NULL);
            pvtSetTree(app->hVal, addressNodeSrce, app->hVal, sourceNode);
        }

        /* Initialize the property database for the call if we should */
        callInitParameters((HAPP)app, (HCALL)call);

        /* Set the terminal type of the endpoint */
        __pvtGetNodeIdByFieldIds(sourceNode, app->hVal, app->rasConf,
            {_q931(registrationInfo) _q931(terminalType) LAST_TOKEN});
        if (RV_PVT_NODEID_IS_VALID(sourceNode))
            cmCallSetParam((HCALL)call, cmParamFullSourceInfo, 0, (RvInt32)sourceNode, NULL);
        else
        {
            __pvtBuildByFieldIds(sourceNode, app->hVal, app->rasConf, {_q931(registrationInfo) _q931(terminalType) LAST_TOKEN}, 0, NULL);
            pvtAdd(app->hVal, sourceNode, __q931(mc), 0, NULL, NULL);
            pvtAdd(app->hVal, sourceNode, __q931(undefinedNode), 0, NULL, NULL);
            cmCallSetParam((HCALL)call, cmParamFullSourceInfo, 0, (RvInt32)sourceNode, NULL);
        }

        /* set the destination aliases */
        if (addressNodeDest >= 0)
        {
            cmCallSetParam((HCALL)call, cmParamFullDestinationAddress, 0, addressNodeDest, (char *)NULL);
            pvtDelete(app->hVal, addressNodeDest);
        }

        /* set the destination info */
        if (infoNodeDest >= 0)
        {
            cmCallSetParam((HCALL)call, cmParamFullDestinationInfo, 0, infoNodeDest, (char *)NULL);
            pvtDelete(app->hVal, infoNodeDest);
        }

        /* set the dest IP address */
        if (ta.port != 0)
        {
            cmCallSetParam((HCALL)call, cmParamDestinationIpAddress, 0, sizeof(ta), (char *)&ta);
        }

        /* set the calling party number */
        if (cpn.length != 0)
        {
            cmCallSetParam((HCALL)call, cmParamCalledPartyNumber, 0, sizeof(cpn), (char *)&cpn);
        }

        /* Set the source address */
        if (addressNodeSrce >= 0)
        {
            /* someone set the source address in cmCallStateTransfering - set it */
            cmCallSetParam((HCALL)call, cmParamFullSourceAddress, 0, addressNodeSrce, (char *)NULL);
            pvtDelete(app->hVal, addressNodeSrce);
        }
    }
    else
    {
        H245Control* ctrl = (H245Control*)cmiGetControl((HCALL)call);
        /* just reset the fast connect state, if the call used fast connect */
        if (ctrl->fastStartState != fssNo)
            ctrl->fastStartState = fssPrplSend;
    }

    /* Delete CAT association of the call */
    if (call->hCatCall)
    {
        RVHCATCALL cat = call->hCatCall;
        call->hCatCall = NULL;
        catDelete(app->hCat, cat);
    }

    /* Close Q931 call object */
    q931CallClose((HQ931)cmiGetQ931((HCALL)call));

    /* Start the new call to finish off with the transfer procedure */
    enterCallInitState(call);
    cmCallSetParam((HCALL)call, cmParamEstablishH245,0,establish,NULL);
    cmCallSetParam((HCALL)call, cmParamH245Parallel,0,parallel,NULL);
    cmCallSetParam((HCALL)call, cmParamH245Tunneling,0,tunneling,NULL);
    cmCallSetParam((HCALL)call, cmParamH245Stage,0,stage,NULL);
    cmCallSetParam((HCALL)call, cmParamIsMultiplexed,0,multiplex,NULL);
    cmCallSetParam((HCALL)call, cmParamShutdownEmptyConnection,0,maintain,NULL);
    m_callset(call,controlDisconnected,RV_FALSE);

    /* unset the callDialInvoked param - we really do want to call cmCallDial again for this call */
    m_callset(call,callDialInvoked,RV_FALSE);
    if (RV_PVT_NODEID_IS_VALID(keepH450))
    {
        cmTransSetH450Element(call->hsTransSession, keepH450);
    }

    /* Since we are going to transfer this call, we better start by decreasing the number of connecting
       calls */
    if (m_callget(call,connectingCall))
    {
        cmLock((HAPP)app);
        app->numberOfConnectingCalls--;
        m_callset(call,connectingCall,RV_FALSE);
        cmUnlock((HAPP)app);
    }

    call->state = cmCallStateInit;
    status = cmCallDial((HCALL)call);
    if (status < 0)
    {
        /* This call will get disconnected soon enough and we should make sure the counter is valid */
        cmLock((HAPP)app);
        app->numberOfConnectingCalls++;
        m_callset(call,connectingCall,RV_TRUE);
        cmUnlock((HAPP)app);
    }

	cmiAPIExit(((HAPP)app, ">>---------initiateDialOnTransfer(app->numberOfConnectingCalls=%d)", app->numberOfConnectingCalls));

}




/************************************************************************
 * rasCallDrop
 * purpose: Call drop was initiated by an incoming DRQ message from the
 *          gatekeeper
 * input  : call      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasCallDrop(IN callElem* call)
{
    /* Drop Q931 connection */
    q931CallDrop(cmiGetQ931((HCALL)call), -1, -1);

    /* Tell APP about disconnection */
    reportDisconnectedState(call, cmCallDisconnectionReasonReceivedDRQ);

    callStopOK(call);
    return 0;
}


/************************************************************************
 * cmCallDrop
 * purpose: Drops a call
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallDrop(IN  HCALL       hsCall)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    HQ931 q931Call=cmiGetQ931((HCALL)hsCall);
    int ret = RV_ERROR_UNKNOWN;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallDrop(hsCall=%p)", hsCall));
    if (emaLock((EMAElement)call))
    {
        RvBool tunnSuported = RV_FALSE;

        /* Update reason for this call's disconnection. Application asked for it... */
        call->disconnectReason = cmCallDisconnectionReasonLocalDropped;

        cmCallGetParam(hsCall, cmParamH245Tunneling, 0, (RvInt32*)&tunnSuported, NULL);
        if (tunnSuported)
        {
            /* as an exception, close the control here */
            cmTransSetSessionParam(call->hsTransSession, cmTransParam_callTerminating, (void*)&tunnSuported);
            dropControl(call);
        }

        /* Drop Q931 connection */
        /* q931CallDrop will do nothing if there was no Q.931 activity before,
           for example if it is called right after ARQ-ACF procedure */
        ret = q931CallDrop(q931Call, -1, -1);

        /* Drop Transactions - put it in Idle state */
        cmTransDrop(call->hsTransSession);

        /* Notify application about the disconnection */
        reportDisconnectedState(call, cmCallDisconnectionReasonLocalDropped);

        emaUnlock((EMAElement)call);
    }

    cmiAPIExit((hApp, "cmCallDrop=%d", ret));
    return ret;
}


static RvStatus disconnectionProcedure(IN  callElem* call)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)call);

    if (!call || !app) return RV_ERROR_UNKNOWN;

    RvLogEnter(&app->logAPI, (&app->logAPI, "disconnectionProcedure(hsCall=0x%p)", call));
    if (emaLock((EMAElement)call))
    {
        /* Reset the timer used by the call */
        RvH323TimerCancel(app->hTimers, &call->timer);

        /* Close H245 connection */
        dropControl(call);

        /* Get rid of RAS - this will also continue to drop the call */
        dropRas(call);

        emaUnlock((EMAElement)call);
    }
    RvLogLeave(&app->logAPI, (&app->logAPI, "disconnectionProcedure"));
    return RV_OK;
}


RVAPI
int RVCALLCONV cmCallFacility(
                            IN HCALL    hsCall,
                            IN int      message)
{
    int ret = 0;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    if (!hsCall ||  !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallFacility(hsCall=%p,handle=%d)",hsCall,message));
    if (emaLock((EMAElement)hsCall))
    {
        ret = q931CallFacility(cmiGetQ931(hsCall),message);
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit((hApp, "cmCallFacility=%d", ret));
    return ret;
}


/************************************************************************************
 *
 * cmCallProgress
 *
 * Purpose: To send PROGRESS message
 *
 * Input:   hsCall                  - The stack handle to the call
 *          message                 - The PVT nodeId of the PROGRESS message to send
 *
 * Output:  None.
 *
 * Returned value: A nonnegative value on success
 *
 **************************************************************************************/
RVAPI
int RVCALLCONV cmCallProgress(
                            IN HCALL        hsCall,
                            IN RvPvtNodeId  message)
{
    int status = RV_ERROR_UNKNOWN;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    if (!hsCall ||  !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallProgress(hsCall=%p,handle=%d)", hsCall, message));
    if (emaLock((EMAElement)hsCall))
    {
        status = q931CallProgress(cmiGetQ931(hsCall),message);
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit((hApp, "cmCallProgress=%d", status));
    return status;
}

/************************************************************************************
 *
 * cmCallNotify
 *
 * Purpose: To send NOTIFY message
 *
 * Input:   hsCall                  - The stack handle to the call
 *          message                 - The pvt nodeId of the NOTIFY message to send
 *
 * Output:  None.
 *
 * Returned value: A nonnegative value on success
 *
 **************************************************************************************/

RVAPI
int RVCALLCONV cmCallNotify(
                            IN HCALL        hsCall,
                            IN RvPvtNodeId  message)
{
    int result = RV_ERROR_UNKNOWN;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    if (!hsCall ||  !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallNotify(hsCall=%p,handle=%d)", hsCall, message));
    if (emaLock((EMAElement)hsCall))
    {
        result = q931CallNotify(cmiGetQ931(hsCall),message);
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit((hApp, "cmCallNotify=%d", result));
    return result;
}


RVAPI
int RVCALLCONV cmCallDropParam(
                             IN     HCALL           hsCall,
                             IN     cmReasonType    cmReason)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)hsCall);
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
    char *prnt= (char*)"<0";
#endif
    int ret = RV_ERROR_UNKNOWN;

    if (!hsCall) return RV_ERROR_UNKNOWN;
    if (!app) return RV_ERROR_UNKNOWN;

#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
    if (((int)cmReason) >= 0)
    {
        /* Set a reason string for the log */
        RvInt32 nameId;
        nameId = getParamFieldName(cmParamReleaseCompleteReason)[cmReason].nameId;
        prnt = pstGetFieldNamePtr(app->synProtRAS,nameId);
    }
#endif

    cmiAPIEnter(((HAPP)app, "cmCallDropParam(hsCall=%p,cmReason=%s)",hsCall,prnt));
    if (emaLock((EMAElement)hsCall))
    {
        /* Set the drop reason for this call */
        if (((int)cmReason) >= 0)
            cmCallSetParam((HCALL)hsCall,cmParamReleaseCompleteReason,0,(RvInt32)cmReason,NULL);

        /* Drop this call */
        ret = cmCallDrop(hsCall);
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit(((HAPP)app, "cmCallDropParam=%d", ret));
    return ret;
}

RVAPI
int RVCALLCONV cmCallClose(
                         IN     HCALL       hsCall)
{
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    int ret = RV_OK;

	RvLogWarning(&app->log, (&app->log, ">>---------cmCallClose Warning enter(app->busy=%d,ConnectingCalls=%d)", app->busy,app->numberOfConnectingCalls));

	{
		char msg[256]={0};
		sprintf(msg,">>---------cmCallClose enter(app->busy=%d,ConnectingCalls=%d)\n", app->busy,app->numberOfConnectingCalls);
		//OutputDebugString(msg);
	}

    if (!hsCall) return RV_ERROR_UNKNOWN;
    if (!app) return RV_ERROR_UNKNOWN;
    cmiAPIEnter(((HAPP)app, "cmCallClose(hsCall=%p)", hsCall));

    if (emaLock((EMAElement)call))
    {
#if (RV_CHECK_MASK & RV_CHECK_OTHER)
        if (emaWasDeleted((EMAElement)hsCall))
        {
            RvLogError(&app->logERR,
                (&app->logERR, "cmCallClose: Call handle %p already closed", hsCall));
        }
#endif

        if (app->mibEvent.h341DeleteControl)
            app->mibEvent.h341DeleteControl(app->mibHandle,hsCall);

        if (app->mibEvent.h341DeleteCall)
            app->mibEvent.h341DeleteCall(app->mibHandle,hsCall);

        if (call->hCatCall)
        {
            RVHCATCALL cat = call->hCatCall;
            call->hCatCall = NULL;
            catDelete(app->hCat, cat);
        }

        if (call->hsTransSession)
        {
            HSTRANSSESSION session = call->hsTransSession;
            call->hsTransSession = NULL;
            cmTransCloseSession(session);
        }

        deleteFastStart((HCALL)call);

        if (!app->manualRAS)
            cmiAutoRASCallClose((HCALL)call);

        /* make sure there are no leftover control */
        dropControl(call);

        if ((call->hMsgContext!=NULL) && (app->cmEvCallReleaseMessageContext))
        {
            int nesting=emaPrepareForCallback((EMAElement)call);
            app->cmEvCallReleaseMessageContext(call->hMsgContext);
            emaReturnFromCallback((EMAElement)call, nesting);
        }

        ret = q931CallClose((HQ931)cmiGetQ931(hsCall));

        RvH323TimerCancel(app->hTimers, &call->timer);

        /* Delete, Unlock and Release */
        emaDelete((EMAElement)call);
        emaUnlock((EMAElement)call);
    }
    else
    {
        RvLogError(&app->logERR,
            (&app->logERR, "cmCallClose: Call handle %p already closed", hsCall));
        ret = RV_ERROR_UNKNOWN;
    }

	RvLogWarning(&app->log, (&app->log, ">>---------cmCallClose Warning exit(app->busy=%d,ConnectingCalls=%d)", app->busy,app->numberOfConnectingCalls));

	{
		char msg[256]={0};
		sprintf(msg,">>---------cmCallClose exit(app->busy=%d,ConnectingCalls=%d)\n", app->busy,app->numberOfConnectingCalls);
		//OutputDebugString(msg);
	}
    cmiAPIExit(((HAPP)app, ">>---------cmCallClose=%d", ret));
    return ret;
}


void reallyCloseCall(
    IN  HAPP hApp,
    IN  HCALL hCall)
{
    cmElem *app = (cmElem *)hApp;
    callElem *call = (callElem *)hCall;

	{
		char msg[256]={0};
		sprintf(msg,">>---------reallyCloseCall enter(app->busy=%d,ConnectingCalls=%d)\n", app->busy,app->numberOfConnectingCalls);
		//OutputDebugString(msg);
	}

    cmiAPIEnter(((HAPP)app, ">>---------reallyCloseCall enter(hsCall=%p,app->busy=%d,ConnectingCalls=%d)", hCall, app->busy,app->numberOfConnectingCalls));

    freeControl(cmiGetControl(hCall));

    callEndParameters((HCALL)call);

    if(call->routeCallSignalAddress >= 0)
    {
        pvtDelete(app->hVal, call->routeCallSignalAddress);
        call->routeCallSignalAddress = RV_PVT_INVALID_NODEID;
    }

    /* delete AltEp Node */
    if (RV_PVT_NODEID_IS_VALID(call->altEpProperty))
    {
        pvtDelete(app->hVal, call->altEpProperty);
        call->altEpProperty = RV_PVT_INVALID_NODEID;
    }

    cmLock((HAPP)app);
    app->busy--;
    cmUnlock((HAPP)app);

    transParentRemove(app->hTransport, (void*)call);

    cmiAPIExit(((HAPP)app, ">>---------reallyCloseCall exit app->busy=%d,ConnectingCalls=%d", app->busy,app->numberOfConnectingCalls));

	{
		char msg[256]={0};
		sprintf(msg,">>---------reallyCloseCall exit(app->busy=%d,ConnectingCalls=%d)\n", app->busy,app->numberOfConnectingCalls);
		//OutputDebugString(msg);
	}
}


RVAPI
int RVCALLCONV cmCallGetOrigin(
                             IN     HCALL       hsCall,
                             OUT    RvBool*     origin)
{
    RvInt32 org=0;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmCallGetOrigin(hsCall=%p,origin)", hsCall));
    org = m_callget(call,callInitiator);
    if (origin != NULL)
        *origin = (RvBool)org;
    cmiAPIExit((hApp, "cmCallGetOrigin(origin=%d)=%d", nprnd((int*)origin), org));
    return (int)org;
}

/******************************************************************************
 * cmCallStatusEnquiry
 * ----------------------------------------------------------------------------
 * General: Builds and sends the STATUS ENQUIRY message to the other side.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall   - The Stack handle for the call.
 *         display  - The text part of the STATUS ENQUIRT message.
 *                    This parameter is a NULL terminated string.
 *                    Setting the display parameter to NULL will send out the
 *                    STATUS ENQUIRY message without the display Information
 *                    Element.
 * Output: None.
 *****************************************************************************/
RVAPI int RVCALLCONV cmCallStatusEnquiry(
    IN  HCALL           hsCall,
    IN  const RvUint8   *display)
{
    RvPvtNodeId id, message;
    cmElem* app=(cmElem*)emaGetInstance((EMAElement)hsCall);
    if (!hsCall) return RV_ERROR_UNKNOWN;
    if (!app) return RV_ERROR_UNKNOWN;

    cmiAPIEnter(((HAPP)app, "cmCallStatusEnquiry(hsCall=%p,display=%s)", hsCall, nprn(((RvChar*)display))));

    if (emaLock((EMAElement)hsCall))
    {
        message = callGetMessage(hsCall, cmQ931statusEnquiry);

        if (display != NULL)
        {
            __pvtBuildByFieldIds(id, app->hVal, message,
                {_q931(message) _q931(statusEnquiry) _q931(display) LAST_TOKEN},
                (RvInt32)strlen((RvChar*)display), (const RvChar*)display);
            if (!RV_PVT_NODEID_IS_VALID(id))
            {
                emaUnlock((EMAElement)hsCall);
                cmiAPIExit(((HAPP)app, "cmCallStatusEnquiry=%d", id));
                return (RvStatus)id;
            }
        }

        q931CallStatusEnquiry(cmiGetQ931(hsCall), message);
        emaUnlock((EMAElement)hsCall);
    }

    cmiAPIExit(((HAPP)app, "cmCallStatusEnquiry=0"));
    return RV_OK;
}


RVAPI
int RVCALLCONV cmCallIncompleteAddress(IN HCALL hsCall)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp,"cmCallIncompleteAddress(hsCall=%p)",hsCall));

    if (emaLock((EMAElement)call))
    {
        if (m_callget(call,remoteCanOverlapSend) && m_callget(call,enableOverlapSending))
            q931CallMoreInfo(cmiGetQ931(hsCall),-1);
        emaUnlock((EMAElement)call);
    }

    cmiAPIExit((hApp,"cmCallIncompleteAddress=1"));
    return RV_TRUE;

}

RVAPI
int RVCALLCONV cmCallUserInformationSend(IN HCALL hsCall, IN int nodeId)
{
    int res = RV_ERROR_DESTRUCTED;

    if ((hsCall == NULL) || !RV_PVT_NODEID_IS_VALID(nodeId))
        return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);

        cmiAPIEnter((hApp, "cmCallUserInformationSend(hsCall=%p,nodeId=%d)", hsCall, nodeId));

        res = q931CallInfo(cmiGetQ931(hsCall), nodeId);
        pvtDelete(((cmElem*)hApp)->hVal, nodeId);

        cmiAPIExit((hApp, "cmCallUserInformationSend=%d", res));
        emaUnlock((EMAElement)hsCall);
    }

    return res;
}


RVAPI
int RVCALLCONV cmCallAddressComplete(IN HCALL hsCall)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallAddressComplete(hsCall=%p)", hsCall));

    if (emaLock((EMAElement)call))
    {
        q931CallAddressComplete(cmiGetQ931(hsCall));
        enterCallSetupState(call);
        cmSetupEnd(call);
        emaUnlock((EMAElement)call);
    }

    cmiAPIExit((hApp, "cmCallAddressComplete=0"));
    return 0;
}

RVAPI
int RVCALLCONV cmCallOverlapSendingExt(
    IN HCALL        hsCall,
    IN const RvChar *address,
    IN RvBool       sendingComplete)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    HPVT hVal;
    int status = RV_ERROR_UNKNOWN;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;
    hVal=((cmElem*)hApp)->hVal;

    cmiAPIEnter((hApp, "cmCallOverlapSendingExt(hsCall=%p,address=%s,sendingComplete=%d)",
        hsCall, nprn(address), sendingComplete));

    if (emaLock((EMAElement)hsCall))
    {
        /* If this function has been called before Q931,then do overlap sending on RAS*/
        if (q931GetCallState(cmiGetQ931(hsCall))==cs_Null)
        {
            char buffer[257];
            cmAlias alias;
            alias.string = buffer;
            alias.length = sizeof(buffer);
            buffer[0]=0;

            if (cmCallGetParam(hsCall,cmParamCalledPartyNumber,0,0,(char *)&alias)>=0)
            {
                alias.string[alias.length] = '\0';
                strncat(alias.string,address,sizeof(buffer) - alias.length);
            }
            else
                strncpy(alias.string,address,sizeof(buffer));

            alias.length=(RvUint16)strlen(alias.string);
            cmCallSetParam(hsCall,cmParamCalledPartyNumber,0,sizeof(cmAlias),(char*)&alias);
            status = cmiAutoRASCallDial(hsCall);
            if ((status >= 0) && (sendingComplete))
                cmCallSetParam(hsCall,cmParamSetupSendingComplete,0,RV_TRUE,NULL);
        }
        /* If this function has been called during Q931 and in the correct state, then do overlap sending on Q931*/
        if (q931GetCallState(cmiGetQ931(hsCall))==cs_Overlap_sending)
        {
            int message=callGetMessage(hsCall,cmQ931information);
            int ieNodeId,tmpNodeId;
            int infoNodeId=pvtGetChild2(hVal,message, __q931(message), __q931(information));
            if (address!=NULL)
            {
                ieNodeId=pvtAddBranch(hVal,infoNodeId,__q931(calledPartyNumber));
                pvtAdd(hVal,ieNodeId,__q931(numberDigits),(RvInt32)strlen(address),address,NULL);
                tmpNodeId=pvtAddBranch(hVal,ieNodeId,__q931(octet3));
                pvtAdd(hVal,tmpNodeId,__q931(typeOfNumber),2,NULL,NULL);
                pvtAdd(hVal,tmpNodeId,__q931(numberingPlanIdentification),1,NULL,NULL);
            }
            if (sendingComplete)
                pvtAddBranch(hVal,infoNodeId,__q931(sendingComplete));

            status = q931CallInfo(cmiGetQ931(hsCall),message);
        }
        emaUnlock((EMAElement)hsCall);
    }

    cmiAPIExit((hApp, "cmCallOverlapSendingExt=%d", status));
    return status;

}




/*Sends/Recv H.450 Messages*/
TRANSERR cmEvTransNewH450Message(IN HSTRANSSESSION  hsTransSession,
                                 IN HATRANSSESSION  haTransSession,
                                 IN int             msg,
                                 IN int             msgSize,
                                 IN int             msgType)
{
    callElem*call=(callElem*)haTransSession;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    cmElem* app=(cmElem*)hApp;

    RV_UNUSED_ARG(hsTransSession);

    if (emaMarkLocked((EMAElement)call))
    {
        if ((app->cmMyCallEvent.cmEvCallH450SupplServ) && (call->state != cmCallStateIdle))
        {
            int nesting;
            cmiCBEnter((hApp, "cmEvCallH450SupplServ (haCall=%p,hsCall=%p)",emaGetApplicationHandle((EMAElement)call),call));
            nesting = emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallH450SupplServ((HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,(cmCallQ931MsgType)msgType,msg,msgSize);
            emaReturnFromCallback((EMAElement)call,nesting);
            cmiCBExit((hApp, "cmEvCallH450SupplServ"));
        }
        emaReleaseLocked((EMAElement)call);
    }

    return cmTransOK;
}


/************************************************************************
 * cmCallSendSuppServiceManually
 * purpose: Sends an H.450 APDU on the next Q.931 message that the
 *          application initiates. It will not initiate a FACILITY message
 *          on its own as cmCallSendSupplamentaryService() might send.
 * input  : hsCall      - Stack handle for the call
 *          buffer      - Buffer in which the APDU has been placed
 *          bufferSize  - Size of the buffer
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallSendSuppServiceManually(
    IN  HCALL       hsCall,
    IN  void*       buffer,
    IN  int         bufferSize)
{
    callElem*call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    TRANSERR err=cmTransOK;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,"cmCallSendSuppServiceManually(hsCall=%p)",hsCall));
    if (emaLock((EMAElement)hsCall))
    {
        err=cmTransSendH450Message(call->hsTransSession, (RvUint8 *)buffer, bufferSize, 0);
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit((hApp,"cmCallSendSuppServiceManually=%d", err));
    return (int)err;
}


/************************************************************************
 * cmCallSendSupplementaryService
 * purpose: Sends an H.450 APDU. This function takes a buffer of a
 *          Supplementary Service and puts an H.450 message in the buffer
 * input  : hsCall      - Stack handle for the call
 *          buffer      - Buffer in which the APDU has been placed
 *          bufferSize  - Size of the buffer
 *          flags:
 *             CM_H450_NO_FLAGS - will send on the next outgoing message, and will
 *                         initiate a facility messgae if call is connected.
 *             CM_H450_FORCE    - will force the stack to send a facility message
 *                         with the APDU.
 *             CM_H450_ALERTING - will send the APDU only with an alerting or a
 *                         connect message
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallSendSupplementaryService(
    IN  HCALL       hsCall,
    IN  void*       buffer,
    IN  int         bufferSize,
    IN  RvUint      flags)
{
    callElem*call=(callElem*)hsCall;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
    TRANSERR err=cmTransOK;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,"cmCallSendSupplementaryService(hsCall=%p)",hsCall));
    if (emaLock((EMAElement)hsCall))
    {
        callStateE state = q931GetCallState(cmiGetQ931(hsCall));
        if ((state == cs_Active) ||
            ((m_callget(call,callInitiator)) && (state != cs_Null)))
        {
            flags |= CM_H450_FORCE;
            flags &= ~CM_H450_ALERTING;
        }
        else if ((state == cs_Call_present) && (flags & CM_H450_FORCE))
        {
            /* this message is supposed to be sent on facility, so we'll send call proceeding and
            than immediately facility. waiting for the next message defeats the purpose of "FORCE" */
            cmCallSendCallProceeding(hsCall);
        }
        err=cmTransSendH450Message(call->hsTransSession, (RvUint8 *)buffer, bufferSize, flags);
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit((hApp,"cmCallSendSupplementaryService=%d", err));
    return (int)err;
}


/*
cmGetH225RemoteVersion - gets H225 remote side version.
IN parameters:
hsCall: Call handle
OUT parameters
version: user allocated string,pass version number retrieved from Q931 messages.
Return: RV_TRUE if succeeded,
RV_ERROR_UNKNOWN  if error is occured.
*/
RVAPI
int RVCALLCONV cmGetH225RemoteVersion(IN HCALL    hsCall,OUT char * version)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    int ver=1;
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmGetH225RemoteVersion: hsCall=%p", hsCall));
    if (emaLock((EMAElement)hsCall))
    {
        if (version)
        {
            ver=m_callget(call,remoteVersion);
            version[0]=(char)(ver+'0');
            version[1]=0;
        }
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit((hApp, "cmGetH225RemoteVersion: version %d", ver));
    return ver;
}


/************************************************************************
 * cmCallGetHandle
 * purpose: Returns the application handle for a call from the call handle.
 * input  : hsCall      - Stack handle for the call
 * output : haCall      - Application handle for the call
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallGetHandle(IN HCALL hsCall, OUT HAPPCALL* haCall)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    if (haCall != NULL)
    {
        cmiAPIEnter((hApp, "cmCallGetHandle: hsCall=%p", hsCall));
        *haCall=(HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);
        cmiAPIExit((hApp, "cmCallGetHandle: 1 (haCall=%p)", *haCall));
    }
    return RV_TRUE;
}


RVAPI
int RVCALLCONV cmSetCallHandle(
                             IN HCALL                hsCall,
                             IN HAPPCALL             haCall)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp,"cmCallSetHandle(hsCall=%p,haCall=%p)",hsCall,haCall));
    emaSetApplicationHandle((EMAElement)hsCall,(void*)haCall);
    cmiAPIExit((hApp, "cmCallSetHandle=0"));
    return 0;
}


/************************************************************************************
 *
 * cmCallMultiplex
 *
 * Purpose: To specify call's Q.931 multiplex behavior.
 *          This function call marks the call as supporting the Q.931 multiplexing
 *
 * Input:   hsCall                  - The stack handle to the call
 *          hsSameConnectionCall    - The stack handle to the call which Q.931
 *                                    connection is the prefered connection for the hsCall
 *                                    If this parameter is not NULL and hsSameConnectionCall
 *                                    has Q.931 connection, which remote address equal to the
 *                                    hsCall's desireable Q.931 connection remote address then
 *                                    the same Q.931 connection will be used for both calls.
 *
 * Output:  None.
 *
 * Returned value: A nonnegative value on success
 *
 **************************************************************************************/

RVAPI
int RVCALLCONV cmCallMultiplex(
                        IN      HCALL               hsCall,
                        IN      HCALL               hsSameConnectionCall)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    callElem*sameConnectionCall=(callElem*)hsSameConnectionCall;
    HSTRANSHOST host;
    int value=RV_TRUE;
    TRANSERR status = cmTransErr;
    if (!hsCall || !hsSameConnectionCall || !hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallMultiplex(hsCall=%p,hsSameConnectionCall=%p)",hsCall,hsSameConnectionCall));

    if (emaLock((EMAElement)hsCall))
    {
        if (emaLock((EMAElement)hsSameConnectionCall))
        {
            cmTransSetSessionParam(call->hsTransSession,cmTransParam_isMultiplexed,&value);
            if (hsSameConnectionCall)
            {
                /* just in case someone decided to call this with a same-connection-call which no
                   longer has or never had a Q.931 host, we check the status */
                status = cmTransGetSessionParam(sameConnectionCall->hsTransSession,cmTransParam_host,(void*)&host);
                if (status == cmTransOK)
                {
                    status = cmTransSetAddress(call->hsTransSession, NULL, NULL, NULL, host, cmTransQ931Conn, RV_FALSE);
                }
            }
            emaUnlock((EMAElement)hsSameConnectionCall);
        }
        emaUnlock((EMAElement)hsCall);
    }
    cmiAPIExit((hApp, "cmCallMultiplex = %d", status));
    if (status == cmTransOK)
        return RV_OK;
    return RV_ERROR_UNKNOWN;
}




TRANSERR cmEvTransNewAnnexLMessage(IN HSTRANSSESSION    hsTransSession,
                                   IN HATRANSSESSION    haTransSession,
                                   IN int               annexLElement,
                                   IN int               msgType)
{
    callElem*call=(callElem*)haTransSession;
    RV_UNUSED_ARG(hsTransSession);

    if (emaMarkLocked((EMAElement)call))
    {
        HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
        cmElem* app=(cmElem*)hApp;

        if ((app->cmMyCallEvent.cmEvCallNewAnnexLMessage != NULL) && (call->state != cmCallStateIdle))
        {
            int nesting;
            cmiCBEnter((hApp, "cmEvCallNewAnnexLMessage(haCall=%p,hsCall=%p, annexL=%d,msgType=%d)",
                (HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,annexLElement,msgType));
            nesting=emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallNewAnnexLMessage(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,annexLElement,msgType);
            emaReturnFromCallback((EMAElement)call,nesting);
            cmiCBExit((hApp, "cmEvCallNewAnnexLMessage"));
        }
        emaReleaseLocked((EMAElement)call);
    }
    return cmTransOK;
}

TRANSERR cmEvTransNewAnnexMMessage(IN HSTRANSSESSION    hsTransSession,
                                   IN HATRANSSESSION    haTransSession,
                                   IN int               annexMElement,
                                   IN int               msgType)
{
    callElem*call=(callElem*)haTransSession;
    RV_UNUSED_ARG(hsTransSession);

    if (emaMarkLocked((EMAElement)call))
    {
        HAPP hApp=(HAPP)emaGetInstance((EMAElement)call);
        cmElem* app=(cmElem*)hApp;

        if ((app->cmMyCallEvent.cmEvCallNewAnnexMMessage != NULL) && (call->state != cmCallStateIdle))
        {
            int nesting;
            cmiCBEnter((hApp, "cmEvCallNewAnnexMMessage(haCall=%p,hsCall=%p, annexM=%d,msgType=%d)",
                (HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,annexMElement,msgType));
            nesting=emaPrepareForCallback((EMAElement)call);
            app->cmMyCallEvent.cmEvCallNewAnnexMMessage(
                (HAPPCALL)emaGetApplicationHandle((EMAElement)call),(HCALL)call,annexMElement,msgType);
            emaReturnFromCallback((EMAElement)call,nesting);
            cmiCBExit((hApp, "cmEvCallNewAnnexMMessage"));
        }
        emaReleaseLocked((EMAElement)call);
    }
    return cmTransOK;
}


/********************************************************************************************
 * cmCallCreateAnnexLMessage
 * purpose : This function creates an Annex L message. It actually creates a PVT node of
 *           type StimulusControl, and allows setting the values of this type.
 * input   : hApp           - Stack instance handle
 *           isText         - RV_TRUE if it's a text message
 *                            RV_FALSE otherwise
 *                          - This is a field inside the structure of StimulusControl (=Annex L)
 *           h248Message    - The buffer of the Annex L message
 *                            NULL if not needed
 *           h248MessageLength  - Length of the message in bytes
 *           nonStandard    - Non standard data associated with the Annex L message
 *                            NULL if not needed
 * output  : none
 * return  : PVT node of the annex L message on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallCreateAnnexLMessage(
    IN HAPP                 hApp,
    IN RvBool               isText,
    IN const RvChar         *h248Message,
    IN int                  h248MessageLength,
    IN cmNonStandardParam   *nonStandard)
{
    HPVT hVal=((cmElem*)hApp)->hVal;

    int nodeId=pvtAddRoot(hVal,((cmElem*)hApp)->synAnnexL, 0,NULL);
    int tmpNodeId;

    cmiAPIEnter((hApp, "cmCallCreateAnnexLMessage(hApp=%p,isText=%d,h248Message=%.50s,h248MessageLength=%d)",
        hApp,isText,h248Message,h248MessageLength));
    if (nodeId<0)
    {
        cmiAPIExit((hApp, "cmCallCreateAnnexLMessage=-1"));
        return RV_ERROR_UNKNOWN;
    }

    if (isText)
        pvtAdd(hVal,nodeId,__q931(isText),0,NULL,NULL);
    if (h248Message)
        pvtAdd(hVal,nodeId,__q931(h248Message),h248MessageLength,h248Message,NULL);
    if (nonStandard)
    {
        tmpNodeId=pvtAdd(hVal,nodeId,__q931(nonStandard),0,NULL,NULL);
        setNonStandardParam(hVal,tmpNodeId,nonStandard);
    }
    cmiAPIExit((hApp, "cmCallCreateAnnexLMessage=%d", nodeId));
    return nodeId;
}


/********************************************************************************************
 * cmCallSendAnnexLMessage
 * purpose : This function sends an Annex L message on the network.
 * input   : hsCall     - Call to send the message in
 *           message    - PVT node of the message to send (of type StimulusControl)
 *           force      - RV_TRUE if the message must be sent immediately on a Facility message
 *                        RV_FALSE if we want to send it on the next Q931 message
 * output  : none
 * return  : Non-negative value on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallSendAnnexLMessage(
    IN HCALL                hsCall,
    IN int                  message,
    IN RvBool               force)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    int ret = RV_ERROR_UNKNOWN;

    if (!hApp) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmCallSendAnnexLMessage(hsCall=%p,message=%d,force=%d)",
        hsCall, message, force));
    if (message<0 || !hsCall)
    {
        cmiAPIExit((hApp, "cmCallSendAnnexLMessage=-1"));
        return RV_ERROR_UNKNOWN;
    }

    if (emaLock((EMAElement)hsCall))
    {
        TRANSERR trans=cmTransSendTunneledMessage(call->hsTransSession,message,cmTransAnnexLType,force);
        emaUnlock((EMAElement)hsCall);
        if (trans==cmTransOK)
            ret = 0;
    }

    cmiAPIExit((hApp, "cmCallSendAnnexLMessage=%d", ret));
    return ret;
}


/********************************************************************************************
 * cmCallGetAnnexLMessage
 * purpose : This function checks the values of an Annex L message.
 *           Annex L messages are piggy-backed on top of Q931 messages, inside a field
 *           called StimulusControl.
 *           This function should be used when cmEvCallNewAnnexLMessage is called.
 * input   : hApp           - Stack instance handle
 *           nodeId         - PVT Node ID of the Annex L message. It's of type StimulusControl
 *           h248MessageLength  - Maximum length of the message buffer in bytes
 * output  : isText         - RV_TRUE if it's a text message
 *                            RV_FALSE otherwise
 *                          - This is a field inside the structure of StimulusControl (=Annex L)
 *           h248Message    - The buffer of the Annex L message
 *           h248MessageLength  - Length of the message in bytes
 *           nonStandard    - Non standard data associated with the Annex L message
 * return  : Non-negative value on success
 *           Negative value on failure
 * Note    : Any of the output fields may be set to NULL.
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallGetAnnexLMessage(
    IN    HAPP                hApp,
    IN    int                 nodeId,
    OUT   RvBool*               isText,
    OUT   char*               h248Message,
    INOUT int*                h248MessageLength,
    OUT   cmNonStandardParam* nonStandard)
{
    HPVT hVal=((cmElem*)hApp)->hVal;
    int tmpNodeId;

    cmiAPIEnter((hApp, "cmCallGetAnnexLMessage(hApp=%p,nodeId=%d)", hApp, nodeId));
    if (nodeId < 0)
    {
        cmiAPIExit((hApp, "cmCallGetAnnexLMessage=-1"));
        return RV_ERROR_UNKNOWN;
    }

    if (isText)
    {
        *isText=(pvtGetChild(hVal,nodeId,__q931(isText),NULL)>=0);
    }

    if (h248MessageLength)
    {
        int len=0;
        tmpNodeId=pvtGetChildByFieldId(hVal,nodeId,__q931(h248Message),&len,NULL);
        if (h248Message)
            pvtGetString(hVal,tmpNodeId,RvMin(len,*h248MessageLength),h248Message);
        *h248MessageLength=len;
    }
    if (nonStandard)
    {
        tmpNodeId=pvtGetChild(hVal,nodeId,__q931(nonStandard),NULL);
        getNonStandardParam(hVal,tmpNodeId,nonStandard);
    }
    cmiAPIExit((hApp, "cmCallGetAnnexLMessage=0"));
    return 0;
}



/********************************************************************************************
 * cmCallCreateAnnexMMessage
 * purpose : This function creates an Annex M message. It actually creates a PVT node of
 *           type tunnelledSignallingMessage, and allows setting the values of this type.
 * input   : hApp               - Stack instance handle
 *           tunnellingRequired - RV_TRUE if tunneling of the messages is required
 *                                RV_FALSE otherwise
 *           tunnelledProtocolID- The identifier of the protocol being tunneled
 *                                This is a structure of type cmTunnelledProtocolID.
 *           messageContent     - The message being tunneled. It is an array of strings with
 *                                variable length. Last element in this array must point
 *                                to a NULL string.
 *           nonStandard        - Non standard data associated with the Annex M message
 * output  : none
 * return  : PVT node of the annex M message on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallCreateAnnexMMessage(
    IN HAPP                     hApp,
    IN RvBool                   tunnellingRequired,
    IN cmTunnelledProtocolID*   tunnelledProtocolID,
    IN cmOctetString*           messageContent,
    IN cmNonStandardParam*      nonStandard)
{
    HPVT hVal=((cmElem*)hApp)->hVal;

    int nodeId=pvtAddRoot(hVal,((cmElem*)hApp)->synAnnexM, 0,NULL);
    int protIdNodeId;
    int tmpNodeId;
    if (nodeId<0) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallCreateAnnexMMessage(hApp=%p)", hApp));
    if (tunnelledProtocolID)
    {
        protIdNodeId=pvtAdd(hVal,nodeId,__q931(tunnelledProtocolID),0,NULL,NULL);
        if (tunnelledProtocolID->subIdentifierLength>0)
        {

            pvtAdd(hVal,protIdNodeId,__q931(subIdentifier),tunnelledProtocolID->subIdentifierLength,tunnelledProtocolID->subIdentifier,NULL);
        }
        tmpNodeId=pvtAddBranch(hVal,protIdNodeId,__q931(id));
        if (tunnelledProtocolID->tunnelledProtocolObjectID[0])
        {
            char buff[128];
            int buffLen;

            buff[0]=0;

            buffLen = oidEncodeOID(sizeof(buff), buff, tunnelledProtocolID->tunnelledProtocolObjectID);
            pvtAdd(hVal,tmpNodeId,__q931(tunnelledProtocolObjectID), buffLen, buff,NULL);
        }
        else
        {
            tmpNodeId=pvtAddBranch(hVal,tmpNodeId,__q931(tunnelledProtocolAlternateID));
            pvtAdd(hVal,tmpNodeId,__q931(protocolType), tunnelledProtocolID->protocolTypeLength, tunnelledProtocolID->protocolType,NULL);
            if (tunnelledProtocolID->protocolVariantLength>0)
            {
                pvtAdd(hVal,tmpNodeId,__q931(protocolVariant), tunnelledProtocolID->protocolVariantLength, tunnelledProtocolID->protocolVariant,NULL);
            }
        }
    }
    if (tunnellingRequired)
        pvtAddBranch(hVal,nodeId,__q931(tunnellingRequired));
    if (nonStandard)
    {
        tmpNodeId=pvtAdd(hVal,nodeId,__q931(nonStandardData),0,NULL,NULL);
        setNonStandardParam(hVal,tmpNodeId,nonStandard);
    }
    {
        int i=0;
        tmpNodeId=pvtAddBranch(hVal,nodeId,__q931(messageContent));
        while(messageContent[i].message)
        {
            pvtAdd(hVal,tmpNodeId,-800,messageContent[i].size,messageContent[i].message,NULL);
            i++;
        }
    }
    cmiAPIExit((hApp, "cmCallCreateAnnexMMessage=%d", nodeId));
    return nodeId;
}


/********************************************************************************************
 * cmCallSendAnnexMMessage
 * purpose : This function sends an Annex M message on the network.
 * input   : hsCall     - Call to send the message in
 *           message    - PVT node of the message to send (of type tunnelledSignallingMessage)
 *           force      - RV_TRUE if the message must be sent immediately on a Facility message
 *                        RV_FALSE if we want to send it on the next Q931 message
 * output  : none
 * return  : Non-negative value on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallSendAnnexMMessage(
    IN HCALL                hsCall,
    IN int                  message,
    IN RvBool               force)
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    callElem*call=(callElem*)hsCall;
    int ret = RV_ERROR_UNKNOWN;

    if (!hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmCallSendAnnexMMessage(hsCall=%p,message=%d,force=%d)",hApp,message,force));
    if (message<0 || !hsCall)
    {
        cmiAPIExit((hApp, "cmCallSendAnnexMMessage=-1"));
        return RV_ERROR_UNKNOWN;
    }

    if (emaLock((EMAElement)hsCall))
    {
        TRANSERR trans=cmTransSendTunneledMessage(call->hsTransSession,message,cmTransAnnexMType,force);
        emaUnlock((EMAElement)hsCall);
        if (trans==cmTransOK)
            ret = 0;
    }

    cmiAPIExit((hApp, "cmCallSendAnnexMMessage=%d", ret));
    return ret;
}


/********************************************************************************************
 * cmCallGetAnnexMMessage
 * purpose : This function checks the values of an Annex M message.
 *           Annex M messages are piggy-backed on top of Q931 messages, inside a field
 *           called tunnelledSignallingMessage.
 *           This function should be used when cmEvCallNewAnnexMMessage is called.
 *           To get the actual tunneled message, use cmCallGetAnnexMMessageContent
 * input   : hApp           - Stack instance handle
 *           nodeId         - PVT Node ID of the Annex M message.
 * output  : tunnellingRequired - RV_TRUE if tunneling of the messages is required
 *                                RV_FALSE otherwise
 *           tunnelledProtocolID- The identifier of the protocol being tunneled
 *                                This is a structure of type cmTunnelledProtocolID.
 *           nonStandard        - Non standard data associated with the Annex M message
 * return  : Non-negative value on success
 *           Negative value on failure
 * Note    : Any of the output fields may be set to NULL.
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallGetAnnexMMessage(
    IN  HAPP                    hApp,
    IN  int                     nodeId,
    OUT RvBool*                   tunnellingRequired,
    OUT cmTunnelledProtocolID*  tunnelledProtocolID,
    OUT cmNonStandardParam*     nonStandard)
{
    HPVT hVal=((cmElem*)hApp)->hVal;

    RvPvtNodeId tmpNodeId, protIdNodeId;
    cmiAPIEnter((hApp, "cmCallGetAnnexMMessage(hsCall=%p,nodeId=%d)",hApp,nodeId));
    if (nodeId<0)
    {
        cmiAPIExit((hApp, "cmCallGetAnnexMMessage=-1"));
        return RV_ERROR_UNKNOWN;
    }

    if (tunnellingRequired)
    {
        *tunnellingRequired=(pvtGetChild(hVal,nodeId,__q931(tunnellingRequired),NULL)>=0);
    }
    if (tunnelledProtocolID)
    {
        protIdNodeId = pvtGetChild(hVal,nodeId,__q931(tunnelledProtocolID),NULL);
        tmpNodeId = pvtGetChildByFieldId(hVal, protIdNodeId, __q931(subIdentifier), NULL, NULL);
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        {
            tunnelledProtocolID->subIdentifierLength =
                pvtGetString(hVal, tmpNodeId, sizeof(tunnelledProtocolID->subIdentifier),
                    tunnelledProtocolID->subIdentifier);
        }
        else
        {
            tunnelledProtocolID->subIdentifierLength = 0;
            tunnelledProtocolID->subIdentifier[0] = '\0';
        }

        protIdNodeId = pvtChild(hVal, pvtGetChild(hVal, protIdNodeId, __q931(id), NULL));

        tunnelledProtocolID->tunnelledProtocolObjectID[0] = '\0';

        switch(pvtGetSyntaxIndex(hVal, protIdNodeId))
        {
            case 1/*tunnelledProtocolObjectID*/:
                {
                    char buff[128];
                    int buffLen;

                    buff[0] = '\0';
                    buffLen = pvtGetString(hVal, protIdNodeId, sizeof(buff), buff);
                    oidDecodeOID(buffLen, buff, sizeof(tunnelledProtocolID->tunnelledProtocolObjectID), tunnelledProtocolID->tunnelledProtocolObjectID, numberForm);

                }
                break;
            case 2/*tunnelledProtocolAlternateID*/:
                {
                    tmpNodeId = pvtGetChildByFieldId(hVal, protIdNodeId, __q931(protocolType), NULL, NULL);
                    if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
                    {
                        tunnelledProtocolID->protocolTypeLength =
                            pvtGetString(hVal, tmpNodeId, sizeof(tunnelledProtocolID->protocolType),
                                tunnelledProtocolID->protocolType);
                    }
                    else
                    {
                        tunnelledProtocolID->protocolTypeLength = 0;
                        tunnelledProtocolID->protocolType[0] = '\0';
                    }

                    tmpNodeId = pvtGetChildByFieldId(hVal,protIdNodeId, __q931(protocolVariant), NULL, NULL);
                    if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
                    {
                        tunnelledProtocolID->protocolVariantLength =
                            pvtGetString(hVal, tmpNodeId, sizeof(tunnelledProtocolID->protocolVariant),
                                tunnelledProtocolID->protocolVariant);
                    }
                    else
                    {
                        tunnelledProtocolID->protocolVariantLength = 0;
                        tunnelledProtocolID->protocolVariant[0] = '\0';
                    }
                }
                break;
            default: break;
        }
    }
    if (nonStandard)
    {
        tmpNodeId=pvtGetChild(hVal,nodeId,__q931(nonStandardData),NULL);
        getNonStandardParam(hVal,tmpNodeId,nonStandard);
    }
    cmiAPIExit((hApp, "cmCallGetAnnexMMessage=0"));
    return 0;
}


/********************************************************************************************
 * cmCallGetAnnexMMessageContent
 * purpose : This function checks the values of an Annex M message.
 *           Annex M messages are piggy-backed on top of Q931 messages, inside a field
 *           called tunnelledSignallingMessage.
 *           This function should be used when cmEvCallNewAnnexMMessage is called.
 *           To get more information about the specific message being tunneled, use
 *           cmCallGetAnnexMMessage.
 * input   : hApp                   - Stack instance handle
 *           nodeId                 - PVT Node ID of the Annex M message.
 *           index                  - Specific tunneled message to get (1-based)
 *           messageContentLength   - Maximum length of buffer in bytes
 * output  : messageContentLength   - Length of message
 *           messageContent         - The message buffer itself
 * return  : Non-negative value on success
 *           Negative value on failure
 * Note    : Any of the output fields may be set to NULL.
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallGetAnnexMMessageContent(
    IN    HAPP                hApp,
    IN    int                 nodeId,
    IN    int                 index,
    INOUT int*                messageContentLength,
    OUT   char*               messageContent)
{
    HPVT hVal=((cmElem*)hApp)->hVal;
    int ret = 0;

    int tmpNodeId;
    cmiAPIEnter((hApp, "cmCallGetAnnexMMessageContent(hsCall=%p,nodeId=%d,index=%d)",hApp,nodeId,index));
    if (nodeId<0)
    {
        cmiAPIExit((hApp, "cmCallGetAnnexMMessageContent=-1"));
        return RV_ERROR_UNKNOWN;
    }

    tmpNodeId=pvtGetByIndex(hVal,pvtGetChild(hVal,nodeId,__q931(messageContent),NULL),index,NULL);

    if (tmpNodeId>=0)
    {
        RvInt32 len;
        pvtGet(hVal,tmpNodeId,NULL,NULL,&len,NULL);
        if (messageContentLength)
        {
            if (messageContent)
                pvtGetString(hVal,tmpNodeId,RvMin(len,*messageContentLength),messageContent);
            *messageContentLength=len;
        }
    }
    else
        ret = tmpNodeId;

    cmiAPIExit((hApp, "cmCallGetAnnexMMessageContent=%d", ret));
    return ret;

}

/******************************************************************************
 * cmCallCatAddCall
 * ----------------------------------------------------------------------------
 * General: Adds a call to the CAT, or updates the CAT information.
 * Return Value: RV_OK on success/ negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall       - Call handle in the stack.
 *         bIncoming    - Is this the incoming side?
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmCallCatAddCall(IN HCALL       hsCall,
                                     IN RvBool      bIncoming)
{
    callElem    *call = (callElem *)hsCall;
    cmElem      *app = (cmElem *)emaGetInstance((EMAElement)call);
    catStruct   callObj;
    int status;

    if ((app == NULL) || (call == NULL))
        return RV_ERROR_NULLPTR;

    status = callObjectCreate(app, hsCall, bIncoming?Incoming:Outgoing, &callObj);

    if (status < 0)
        return status;

    if (call->hCatCall != NULL)
    {
        catUpdate(app->hCat, call->hCatCall, &callObj);
        return RV_OK;
    }

    call->hCatCall = catAdd(app->hCat, &callObj, hsCall);

    if (call->hCatCall == NULL)
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}

/******************************************************************************
 * cmCallCatGetCall
 * ----------------------------------------------------------------------------
 * General: Retrieves a call handle from the CAT.
 * Return Value: RV_OK on success/ negative value if call not found.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hH323        - Stack handle.
 *         callId       - The CallIdentifier of the call.
 *         bIncoming    - Is this the incoming side?
 * Output: hsCall       - Call handle in the stack.
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmCallCatGetCall(IN  HAPP       hH323,
                                     IN  RvChar *   callId,
                                     IN  RvBool     bIncoming,
                                     OUT HCALL *    hsCall)
{
    catStruct  key;
    cmElem     *app = (cmElem *)hH323;
    RVHCATCALL  hCatCall;

    memset(&key, 0, sizeof(catStruct));
    memcpy(key.callID, callId, sizeof(key.cid));
    key.answerCall = bIncoming;
    key.flags = catCallId | catAnswerCall;

    /*find CAT call*/
    hCatCall = catFind(app->hCat, &key);

    *hsCall = catGetCallHandle(app->hCat, hCatCall);

    if (*hsCall == NULL)
        return RV_ERROR_UNKNOWN;

    return RV_OK;

}

/******************************************************************************
 * cmCallSetConnection
 * ----------------------------------------------------------------------------
 * General: Replaces the call's connection with the given one.
 * Return Value: RV_OK on success/ negative value if call not found.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   hH323        - Stack handle.
 *          hsCall       - Call handle in the stack.
 *          bIsQ931Conn  - RV_TRUE if hConn is a Q931 host .
 *          hConn        - The protConn connection to set on the call.
 * Output:
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmCallSetConnection(IN HAPP                     hH323,
                                        IN HCALL                    hsCall,
                                        IN RvBool                   bIsQ931Conn,
                                        IN HPROTCONN                hConn)
{
    cmElem      *app = (cmElem *)hH323;
    callElem    *call = (callElem *)hsCall;
    RvStatus    status = RV_ERROR_UNKNOWN;

    if (emaLock((EMAElement)call))
    {
        status = cmTransSetConnection(app->hTransport, call->hsTransSession, (HSTRANSHOST)hConn, bIsQ931Conn);
        emaUnlock((EMAElement)call);
    }

    if (status != RV_OK)
        return RV_ERROR_UNKNOWN;
    return status;
}

/************************************************************************
 * callMibGetSession
 * purpose: Get the session of a handle for the MIB.
 *          This function is used to access the transport layer for MIB
 *          specific information
 * input  : call    - Stack handle for the call
 * output : none
 * return : Session of the call on success
 *          NULL on failure
 ************************************************************************/
HSTRANSSESSION callMibGetSession(IN HCALL call)
{
    return ((callElem *)call)->hsTransSession;
}


/************************************************************************
 * cmIsRoutedCall
 * purpose: Check if the call is routed or direct. Used to create perCallInfo
 *          inside IRR messages by the ras module.
 * input  : hsCall  - Call handle to check
 * output : none
 * return : RV_TRUE if call is routed
 ************************************************************************/
RvBool cmIsRoutedCall(IN HCALL hsCall)
{
    callElem* call = (callElem *)hsCall;

    return (m_callget(call, gatekeeperRouted) != 0);
}




#ifdef __cplusplus
}
#endif

