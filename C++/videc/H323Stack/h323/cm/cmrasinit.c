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

#include "rvhost.h"
#include "q931asn1.h"
#include "cmutils.h"
#include "cmAutoRas.h"
#include "cmAutoRasEP.h"
#include "cmAutoRasCall.h"
#include "cmrasinit.h"
#include "cmdebprn.h"
#include "cmCall.h"
#include "rvh323connection.h"
#include "cmaltep.h"
#include "rasdef.h"


#ifdef __cplusplus
extern "C" {
#endif




/************************************************************************
 *
 *                          Private functions
 *
 ************************************************************************/



/************************************************************************
 * autoRasSetPreGrantedARQParameters
 * purpose: This function extracts the PreGrantedARQ parameters from an
 *          incoming RCF message.
 * input  : hsRas   - RCF transaction handle
 * output : none
 * return : none
 ************************************************************************/
void RVCALLCONV autoRasSetPreGrantedARQParameters(
    IN HRAS             hsRas)
{
    HAPP hApp;
    cmElem* app;
    HPVT hVal;
    RvPvtNodeId responseId, chNodeId;

    hApp = (HAPP)emaGetInstance((EMAElement)hsRas);
    app=(cmElem*)hApp;
    hVal = app->hVal;
    
    responseId=cmiRASGetResponse(hsRas);
    responseId=pvtChild(app->hVal,responseId);

    app->rasGranted=0;
    if ((chNodeId = pvtGetChild(hVal, responseId, __q931(preGrantedARQ), NULL)) >= 0)
    {
        RvPvtNodeId tmpNodeId;
        int val;

        val=0;
        pvtGet(hVal,pvtGetChild(hVal,chNodeId,__q931(makeCall),NULL),NULL,NULL,&val,NULL);
        if (val) app->rasGranted |= (int)makeCall;

        val=0;
        pvtGet(hVal,pvtGetChild(hVal,chNodeId,__q931(useGKCallSignalAddressToMakeCall),NULL),NULL,NULL,&val,NULL);
        if (val) app->rasGranted |= (int)useGKCallSignalAddressToMakeCall;

        val=0;
        pvtGet(hVal,pvtGetChild(hVal,chNodeId,__q931(answerCall),NULL),NULL,NULL,&val,NULL);
        if (val) app->rasGranted |= (int)answerCall;

        val=0;
        pvtGet(hVal,pvtGetChild(hVal,chNodeId,__q931(useGKCallSignalAddressToAnswer),NULL),NULL,NULL,&val,NULL);
        if (val) app->rasGranted |= (int)useGKCallSignalAddressToAnswer;

        val=0;
        pvtGet(hVal,pvtGetChild(hVal,chNodeId,__q931(irrFrequencyInCall),NULL),NULL,NULL,&val,NULL);
        if (val) app->irrFrequencyInCall = val;

        tmpNodeId=pvtChild(hVal,pvtGetChild2(hVal,chNodeId,__q931(alternateTransportAddresses),__q931(annexE)));
        if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
        {
            cmVtToTA(hVal,tmpNodeId,&(app->gkAnnexEAddress));
        }

        chNodeId=pvtGetChild(hVal,chNodeId,__q931(useSpecifiedTransport),NULL);
        if (RV_PVT_NODEID_IS_VALID(chNodeId))
        {
            /* check if we must use annex E */
            if (pvtGetChild(hVal,chNodeId,__q931(annexE),NULL)>=0)
                app->rasGrantedProtocol = 1;

            /* check if we must use TCP */
            if (pvtGetChild(hVal,chNodeId,__q931(tcp),NULL)>=0)
                app->rasGrantedProtocol = 0;
        }
    }
}



/************************************************************************
 * handleAutoRasEvent
 * purpose: Callback function that handles Automatic RAS events
 * input  : hsCall  - Call handle of the transaction (NULL if not applicable)
 *          hsRas   - RAS transaction handle
 *          event   - Event that occurred
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int RVCALLCONV handleAutoRasEvent(
    IN HCALL            hsCall,
    IN HRAS             hsRas,
    IN cmiAutoRasEvent  event)
{
    callElem* call = (callElem *)hsCall;
    HAPP hApp;
    cmElem* app;
    HPVT hVal;

    if (hsCall == NULL)
        return RV_ERROR_NULLPTR;

    if (!emaLock((EMAElement)call))
        return RV_ERROR_DESTRUCTED;

    if (hsRas != NULL)
        hApp = (HAPP)emaGetInstance((EMAElement)hsRas);
    else
        hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    app=(cmElem*)hApp;

    hVal = app->hVal;

    switch (event)
    {
        case cmiAutoRasEvGotACF:
        {
            cmTransportAddress tpktAddress, annexEAddess;
            RvPvtNodeId tmpNodeId;
            RvPvtNodeId tpktAddressNodeId;
            RvPvtNodeId altEpRootNodeId;
            int bandwidth;

            RvPvtNodeId responseId = cmiRASGetResponse(hsRas);
            responseId = pvtChild(hVal, responseId);
            tpktAddressNodeId = pvtGetChild(hVal, responseId, __q931(destCallSignalAddress), NULL);

            /* This part of the code implements the CanMapAlias feature */
            /* When the stack receives an ACF with either one of the fields below, it copies the
               field to the setup message */
            tmpNodeId = pvtGetChild(hVal, responseId, __q931(destinationInfo), NULL);
            if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
                cmCallSetParam(hsCall, cmParamFullDestinationAddress, 0, tmpNodeId, NULL);
            tmpNodeId = pvtGetChild(hVal, responseId, __q931(destExtraCallInfo), NULL);
            if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
                cmCallSetParam(hsCall, cmParamFullDestExtraCallInfo, 0, tmpNodeId, NULL);
            __pvtGetByFieldIds(tmpNodeId, hVal, responseId, {_q931(remoteExtensionAddress) _anyField LAST_TOKEN}, NULL, NULL, NULL);
            if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
                cmCallSetParam(hsCall, cmParamFullExtension, 0, tmpNodeId, NULL);

            /* Here we play around with canMapSrcAlias, which is always TRUE for us */
            tmpNodeId = pvtGetChild(hVal, responseId, __q931(modifiedSrcInfo), NULL);
            if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
                cmCallSetParam(hsCall, cmParamFullSourceAddress, 0, tmpNodeId, NULL);

            if (cmCallGetOrigin(hsCall, NULL))
            {
                /* For Destination address, the addresses are scanned for an E-164 address, and if
                   found, it is set in the CalledPartyNumber param */
                if ((cmCallGetParam(hsCall, cmParamFullDestinationAddress, 0, &tmpNodeId, NULL) >= 0) &&
                    RV_PVT_NODEID_IS_VALID(tmpNodeId))
                {
                    cmRASAlias rasAlias;
                    RvChar alias[129]; /* e164 is 128 bytes long + NULL termination */
                    RvInt32 size = sizeof(rasAlias);
                    rasAlias.string = alias;

                    for (tmpNodeId = pvtChild(hVal, tmpNodeId);   /*start with first*/
                        RV_PVT_NODEID_IS_VALID(tmpNodeId);        /*as long as there are still children*/
                        tmpNodeId = pvtBrother(hVal, tmpNodeId))  /*go down the sequence*/
                    {
                        if (RV_PVT_NODEID_IS_VALID(pvtGetChild(hVal, tmpNodeId, __q931(e164), NULL)))
                        {
                            /* the child is of type E-164 */
                            vtToAlias(hVal, &rasAlias, tmpNodeId, RV_FALSE);
                            cmCallSetParam(hsCall, cmParamCalledPartyNumber, 0, size, (char *)&rasAlias);
                            break;
                        }
                    }
                }

                /* if there is a clear instruction from GK about the protocol to be used */
                tmpNodeId = pvtGetChild(hVal, responseId, __q931(useSpecifiedTransport), NULL);
                if (RV_PVT_NODEID_IS_VALID(tmpNodeId))
                {
                    /* check if we must use TCP */
                    if (pvtGetChild(hVal, tmpNodeId, __q931(tcp), NULL) >= 0)
                    {
                        cmCallSetParam(hsCall, cmParamAnnexE, 0, (RvInt32)cmTransNoAnnexE, NULL);
                    }
                    /* check if we must use annex E */
                    else if (pvtGetChild(hVal, tmpNodeId, __q931(annexE), NULL) >= 0)
                    {
                        cmCallSetParam(hsCall, cmParamAnnexE, 0, (RvInt32)cmTransUseAnnexE, NULL);
                        tmpNodeId = pvtChild(hVal, pvtGetChild2(hVal, responseId, __q931(alternateTransportAddresses), __q931(annexE)));
                        cmVtToTA(hVal, tmpNodeId, &annexEAddess);
                        cmCallSetParam(hsCall, cmParamDestinationAnnexEAddress, 0, sizeof(cmTransportAddress), (char*)&annexEAddess);
                    }
                }
                else
                {
                    /* GK doesn't specify the protocol to be used - assume TPKT, if AnnexE was not set */
                    cmAnnexEUsageMode annexEBehavior=cmTransNoAnnexE;
                    cmCallGetParam((HCALL)call,cmParamAnnexE,0,(int *)&annexEBehavior,NULL);

                    if (annexEBehavior != cmTransUseAnnexE)
                      cmCallSetParam(hsCall, cmParamAnnexE, 0, (RvInt32)cmTransNoAnnexE, (char*)NULL);
                }

                cmVtToTA(hVal, tpktAddressNodeId, &tpktAddress);
                cmCallSetParam(hsCall,cmParamDestinationIpAddress, 0, sizeof(cmTransportAddress), (char*)&tpktAddress);
                if (!RV_PVT_NODEID_IS_VALID(call->routeCallSignalAddress))
                {
                    cmCallSetParam(hsCall, cmParamDestCallSignalAddress, 0, sizeof(cmTransportAddress), (char*)&tpktAddress);
                }

                /* copy AltEP data from ACF into the CALL */
                altEpRootNodeId = pvtGetChild(hVal, responseId, __q931(alternateEndpoints), NULL);
                if (RV_PVT_NODEID_IS_VALID(altEpRootNodeId))
                    cmCallSetAlternateEndpoints(hsCall, altEpRootNodeId);
            }

            /* Get the bandwidth */
            pvtGetChildByFieldId(hVal, responseId, __q931(bandWidth), &bandwidth, NULL);
            call->rate = bandwidth * 50;

            /* Check and set the call model */
            {
                RvPstFieldId fieldId;
                pvtGet(hVal, pvtChild(hVal, pvtGetChild(hVal, responseId, __q931(callModel), NULL)), &fieldId, NULL, NULL, NULL);
                m_callset(call, gatekeeperRouted, (int)(fieldId != __q931(direct)));
            }

            callStartOK(call);
            break;
        }

        case cmiAutoRasEvCantRegister:
        {
            /* The endpoint couldn't register to a gatekeeper.
               We can only make this call if we have the DestCallSignalAddress and if
               the configuration allows IP calls without a gatekeeper */
            cmTransportAddress tpktAddress,annexEAddess;
            int size=sizeof(cmTransportAddress);
            m_callset(call,dummyRAS,RV_TRUE);
            if (pvtGetChild(hVal,app->rasConf,__q931(allowCallsWhenNonReg),NULL)>=0 &&
                (!cmCallGetOrigin(hsCall, NULL) ||
                 cmCallGetParam(hsCall,cmParamDestinationIpAddress,0,&size ,(char*)&tpktAddress)>=0 ||
                 cmCallGetParam(hsCall,cmParamDestinationAnnexEAddress,0, &size,(char*)&annexEAddess)>=0))
            {
                callStartOK(call);
            }
            else
            {
                callStartError(call, cmCallDisconnectionReasonNotRegistered);
                /* just in case the user didn't close the call yet, close the RAS part of it */
                cmiAutoRASCallClose(hsCall);
            }
            break;
        }

        case cmiAutoRasEvTimedout:
        case cmiAutoRasEvFailedOnARQ:
        {
            cmiAutoRASCallClose(hsCall);

            /* if we received the call, we must raise the disconnected and idle events.
            otherwise, we are the initiators, and cmCallMake/Dial will just fail */
            if (event == cmiAutoRasEvTimedout)
            {
                callStartError(call, cmCallDisconnectionReasonNoRasResponse);
            }
            else if (!m_callget(call, callInitiator))
            {
                /* If we're here then we were unable to dial out the call from the function itself - probably bad call
                   parameters got us to this ugly point */
                callStartError(call, cmCallDisconnectionReasonUnknown);
            }
            break;
        }

        case cmiAutoRasEvCallRejected:
        {
            int responseId = cmiRASGetResponse(hsRas);
            int rejectId;
            RvPstFieldId reasonType;

            /* Find the reason - we'll know what to do from there */
            rejectId = pvtChild(hVal, responseId);
            rejectId = pvtGetChild(hVal, rejectId, __q931(rejectReason), NULL);
            rejectId = pvtChild(hVal, rejectId);
            pvtGet(hVal, rejectId, &reasonType, NULL, NULL, NULL);

            /* See what's the reason */
            switch (reasonType)
            {
                case __q931(callerNotRegistered):
                    cmRegister((HAPP)hApp);
                    callDial((callElem*)hsCall);
                    break;

                case __q931(invalidEndpointIdentifier):
                    cmRegister((HAPP)hApp);
                    cmiAutoRASCallClose(hsCall);
                    callStartError(call, cmCallDisconnectionReasonReceivedARJ);
                    break;

                case __q931(routeCallToGatekeeper):
                    cmiAutoRASCallClose(hsCall);
                    callStartRoute(call);
                    break;

                case __q931(incompleteAddress):
                    if (cmCallGetOrigin(hsCall,NULL))
                    {
                        if(callIncompleteAddress(call) < 0)
                            cmiAutoRASCallClose(hsCall);
                    }
                    else
                    {
                        cmiAutoRASCallClose(hsCall);
                        callStartError(call, cmCallDisconnectionReasonReceivedARJ);
                    }
                    break;

                default:
                    cmiAutoRASCallClose(hsCall);
                    callStartError(call, cmCallDisconnectionReasonReceivedARJ);
                    break;
            }
            break;
        }

        case cmiAutoRasEvRateChanged:
        {
            /* BCF/BRQ received */
            int bandwidth;
            int rasMessageId=cmiRASGetResponse(hsRas);
            if (rasMessageId < 0)
            {
                /* We don't have a response - it's a BRQ... */
                rasMessageId=cmiRASGetRequest(hsRas);
            }
            rasMessageId=pvtChild(hVal,rasMessageId);
            if ((pvtGetChildByFieldId(hVal,rasMessageId,__q931(bandWidth),&bandwidth,NULL))>=0)
                call->rate=bandwidth*50;

            /* Notify the application about the change of rate */
            callNewRate(call);
            break;
        }

        case cmiAutoRasEvCallDropForced:
            rasCallDrop(call);
            break;

        case cmiAutoRasEvCallDropped:
            callStopOK(call);
            break;

        case cmiAutoRasEvPrepareIRR:
        {
            int requestId, responseId, perCallInfoId, nodeId;

            requestId  = pvtChild(hVal, cmiRASGetRequest(hsRas));
            responseId = pvtChild(hVal, cmiRASGetResponse(hsRas));
            __pvtBuildByFieldIds(perCallInfoId, hVal, responseId, {_q931(perCallInfo) _anyField LAST_TOKEN}, 0, NULL);

            pvtAdd(hVal, perCallInfoId, __q931(callReferenceValue), call->rascrv,NULL,NULL);
            pvtAdd(hVal,perCallInfoId,__q931(callIdentifier),16,(char*)call->callId,NULL);
            pvtAdd(hVal,perCallInfoId,__q931(conferenceID),16,(char*)call->cId,NULL);
            nodeId=pvtAddBranch(hVal,perCallInfoId,__q931(callSignaling));
            pvtAddBranch(hVal,perCallInfoId,__q931(h245));
            pvtAddBranch(hVal,perCallInfoId,__q931(substituteConfIDs));
            pvtAdd(hVal,perCallInfoId,__q931(originator),(RvInt32)m_callget(call,callInitiator),NULL,NULL);
            pvtAdd(hVal,perCallInfoId,__q931(bandWidth),call->rate/100,NULL,NULL);
            pvtSetTree(hVal,pvtAddBranch(hVal,perCallInfoId,__q931(callType)),
                       hVal,pvtGetChild(hVal,requestId,__q931(callType),NULL));
            pvtSetTree(hVal,pvtAddBranch(hVal,perCallInfoId,__q931(callModel)),
                       hVal,pvtGetChild(hVal,responseId,__q931(callModel),NULL));
            if (m_callget(call,callInitiator))
            {
                pvtSetTree(hVal,pvtAddBranch(hVal,nodeId,__q931(sendAddress)),
                           hVal,pvtGetChild(hVal,responseId,__q931(destCallSignalAddress),NULL));
                pvtSetTree(hVal,pvtAddBranch(hVal,nodeId,__q931(recvAddress)),
                           hVal,pvtGetChild(hVal,responseId,__q931(srcCallSignalAddress),NULL));
            }
            else
            {
                pvtSetTree(hVal,pvtAddBranch(hVal,nodeId,__q931(sendAddress)),
                           hVal,pvtGetChild(hVal,responseId,__q931(srcCallSignalAddress),NULL));
                pvtSetTree(hVal,pvtAddBranch(hVal,nodeId,__q931(recvAddress)),
                           hVal,pvtGetChild(hVal,responseId,__q931(destCallSignalAddress),NULL));
            }
            break;
        }
    }

    emaUnlock((EMAElement)call);
    return 0;
}


/************************************************************************
 * sendRasMessage
 * purpose: Callback function called when an outgoing RAS message should
 *          be sent.
 * input  : hApp            - Stack's instance handle
 *          index           - Index of the socket to send message on
 *          destAddress     - Address to send the message to
 *          messageBuf      - The message buffer to send
 *          messageLength   - The length of the message in bytes
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
static RvStatus RVCALLCONV sendRasMessage(
    IN HAPP                 hApp,
    IN RvInt                index,
    IN cmTransportAddress   *destAddress,
    IN RvUint8*             messageBuf,
    IN RvSize_t             messageLength)
{
    cmElem          *app;
    RasConnection   *connection;
    RvAddress       dest;
    RvStatus        status;
    RvSize_t        sent = 0;

    app = (cmElem *)hApp;

    connection = (RasConnection *)emaGetByIndex(app->rasListeningSockets, index-1);
    if (connection == NULL)
        return RV_ERROR_UNKNOWN;

    if (app->cmMyProtocolEvent.hookSendRawTo != NULL)
    {
        if (app->cmMyProtocolEvent.hookSendRawTo((HPROTCONN)connection, messageBuf, messageLength, destAddress))
            return RV_OK;
    }

    status = RvH323CmToCoreAddress(destAddress, &dest);
    if (status != RV_OK)
        return status;

    status = RvSocketSendBuffer(RvFdGetSocket(&connection->fd), messageBuf, messageLength, &dest, app->logMgr, &sent);
    if ((status != RV_OK) || (messageLength != sent))
    {
        return RV_ERROR_UNKNOWN;
    }

    return status;
}


/********************************************************************************************
 * RvRasSelectCb
 *
 * purpose : Callback that is executed when an event occurs on a file descriptor
 * input   : selectEngine   - Events engine of this fd
 *           fd             - File descriptor that this event occurred on
 *           selectEvent    - Event that happened
 *           error          - RV_TRUE if an error occurred
 * output  : None
 * return  : None
 ********************************************************************************************/
void RvRasSelectCb(
    IN RvSelectEngine*  selectEngine,
    IN RvSelectFd*      fd,
    IN RvSelectEvents   selectEvent,
    IN RvBool           error)
{
    RasConnection *connection = RV_GET_STRUCT(RasConnection, fd, fd);
    cmElem * app;

    RV_UNUSED_ARG(selectEngine);
    RV_UNUSED_ARG(selectEvent); /* Event should always be a read event */

    if (error)
    {
        /* We should just ignore errors - they might occur in high-load scenarios */
        return;
    }

    app = (cmElem *)emaGetInstance((EMAElement)connection);

    {
        RvUint8 * buffer;
        RvSize_t bytesReceived;
        RvAddress remoteAddress;
        RvStatus status;

        getEncodeDecodeBuffer(app->encodeBufferSize, &buffer);
        status = RvSocketReceiveBuffer(RvFdGetSocket(&connection->fd), buffer, (RvSize_t)app->encodeBufferSize, app->logMgr, &bytesReceived, &remoteAddress);
        if (status == RV_OK)
        {
            cmTransportAddress srcAddress;
            RvH323CoreToCmAddress(&remoteAddress, &srcAddress);
            cmCallPreCallBack((HAPP)app);
            cmiRASReceiveMessage(app->rasManager, connection->index, connection->isMulticast, &srcAddress, buffer, bytesReceived);
        }
    }
}


/************************************************************************
 *
 *                          Public functions
 *
 ************************************************************************/


/************************************************************************
 * rasInit
 * purpose: Initialize the RAS module and all the network related with
 *          RAS.
 * input  : app     - Stack handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasInit(IN cmElem* app)
{
    RvInt32             inTx, outTx;
    RvInt32             maxCalls = 1;
    RvInt32             maxBuffSize = 2048;
    RvInt               i;
    RvInt               maxListeningAddresses;

    /* Make sure we can check these handles later on for validity */
    app->rasAddresses = RV_PVT_INVALID_NODEID;
    app->natrasAddresses = RV_PVT_INVALID_NODEID;

    maxListeningAddresses = app->maxListeningAddresses;

    /* Find out the number of calls - we might need that to calculate the number
       of transactions to support */
    if (ciGetValue(app->hCfg, "system.maxCalls", NULL, &maxCalls) != ERR_CI_NOERROR)
        if (ciGetValue(app->hCfg,"Q931.maxCalls",NULL, &maxCalls) != ERR_CI_NOERROR)
            maxCalls = 20;

    /* Check in configuration if we're a gatekeeper... */
    app->gatekeeper = RV_PVT_NODEID_IS_VALID(pvtGetChild(app->hVal, app->rasConf, __q931(gatekeeper), NULL));

    /* altEp automatically modify setup message. To be clarified what exactly should be modified */
    app->altEpModifySetup = (pvtGetChild(app->hVal, app->rasConf, __q931(altEpModifySetup), NULL) >= 0);

    /* Estimate the number of transactions by the number of calls */
    if (app->gatekeeper)
    {
        outTx = maxCalls;
        inTx = maxCalls * 2 + 10;
        if (app->maxListeningAddresses == 1)
            maxListeningAddresses = 2;
    }
    else
    {
        outTx = maxCalls + 10;
        inTx = maxCalls;
    }

    {
        /* Calculate the true value of incoming transactions - this is done by checking
           the amount of incoming transactions per second and multiplying that with the
           timeout necessary to support */
        int timeout = 4;
        int maxRetries = 3;

        ciGetValue(app->hCfg, "RAS.responseTimeOut",NULL, &timeout);

        ciGetValue(app->hCfg, "RAS.maxRetries",NULL, &maxRetries);
        if (maxRetries <= 0)
        {
            RvLogExcep(&app->log,
                (&app->log, "Invalid number of RAS.maxRetries in Stack configuration: %d. using 3.", maxRetries));
            maxRetries = 3;
        }

        timeout *= maxRetries;
        if (timeout == 0) timeout = 1;
        inTx = inTx * timeout + 1;
    }

    /* Check if we have explicit number of transactions supported in the configuration */
    ciGetValue(app->hCfg, "system.maxRasOutTransactions", NULL, &outTx);
    ciGetValue(app->hCfg, "system.maxRasInTransactions", NULL, &inTx);
    ciGetValue(app->hCfg, "system.allocations.maxBuffSize", NULL, &maxBuffSize);

    /* Initialize the RAS module */
    app->rasManager =
        cmiRASInitialize((HAPP)app,
                         app->hCat,
                         app->hVal,
                         app->rasConf,
                         (RvUint32)inTx,
                         (RvUint32)outTx,
                         (RvUint32)maxBuffSize,
                         app->gatekeeper,
                         app->logMgr);
    if (app->rasManager == NULL)
        return RV_ERROR_OUTOFRESOURCES;

    /* Initialize automatic RAS if we have to */
    if (!app->manualRAS)
    {
        app->hAutoRas = cmiAutoRASInit((HAPP)app,
                                       app->hTimers,
                                       app->hVal,
                                       app->rasConf,
                                       handleAutoRasEvent,
                                       app->logMgr);
        if (app->hAutoRas == NULL)
            return RV_ERROR_OUTOFRESOURCES;
    }

    cmiRASSetSendEventHandler(app->rasManager, sendRasMessage);
    cmiRASSetNewCallEventHandler((HAPP)app, cmEvRASNewCall);

    app->rasListeningSockets = emaConstruct(sizeof(RasConnection), maxListeningAddresses, emaNoLocks, "RAS HPROTCONN", RAS_PROTCONN, NULL, app, app->logMgr);
    for (i = 0; i < maxListeningAddresses; i++)
    {
        RasConnection *connection = (RasConnection *)emaAdd(app->rasListeningSockets, NULL);
        memset(connection, 0, sizeof(RasConnection));
    }

    return 0;
}


static RvStatus addRasAddress(
    IN      cmElem               *app,
    INOUT   cmTransportAddress   *addr,
    IN      RvInt                addrType)
{
    RvStatus status;
    RvAddress coreAddr;
    RvAddress bindAddress;
    RasConnection *connection;
    RvSocket sock;
    RvPortRange *portRange = NULL;
    RvPvtNodeId newAddrId;
    RvInt maxAddrs;

    maxAddrs = app->maxListeningAddresses;
    if (app->gatekeeper && (maxAddrs == 1))
        maxAddrs = 2;

    if (app->numRasListeningSockets == maxAddrs)
    {
        RvLogError(&app->logERR,
            (&app->logERR, "Too many RAS listening addresses - cannot add another one"));
        return RV_ERROR_OUTOFRANGE;
    }

    status = RvH323CmToCoreAddress(addr, &coreAddr);
    if (status != RV_OK)
        return status;
    status = RvH323CmToCoreAddress(&app->localAddress, &bindAddress);
    if (status != RV_OK)
        return status;

    if (addrType < 0)
    {
        /* We have to determine multicast on our own */
        addrType = (RvInt)RvAddressIsMulticastIp(&coreAddr);
    }

    /* Our bind address for this port should be considered now... */
    if (RvAddressGetType(&coreAddr) == RvAddressGetType(&bindAddress))
    {
        /* Bind using the local address and the given port */
        RvAddressSetIpPort(&bindAddress, RvAddressGetIpPort((&coreAddr)));
    }
    else
    {
        /* bind the requested socket to an arbitrary address with the given port */
        switch (RvAddressGetType(&coreAddr))
        {
        case RV_ADDRESS_TYPE_IPV4:
            RvAddressConstructIpv4(&bindAddress, RV_ADDRESS_IPV4_ANYADDRESS, RvAddressGetIpPort((&coreAddr)));
            break;
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
        case RV_ADDRESS_TYPE_IPV6:
            RvAddressConstructIpv6(&bindAddress, app->localAddress.addr.v6.ip, RvAddressGetIpPort((&coreAddr)), app->localAddress.addr.v6.scopeId);
            break;
#endif
        default:
            RvLogError(&app->logERR,
                (&app->logERR, "Bad RAS address type to bind"));
            return -13;
        }
    }

    connection = (RasConnection *)emaGetByIndex(app->rasListeningSockets, app->numRasListeningSockets);

    connection->isMulticast = (RvBool)(addrType != 0);

    status = RvSocketConstruct(RvAddressGetType(&coreAddr), RvSocketProtocolUdp, app->logMgr, &sock);
    if (status != RV_OK)
    {
        /* User wanted a RAS port to listen to, but it seems the port is a little bit taken... */
        return -13; /* Network problem */
    }

    /* We're always working with non-blocking sockets */
    RvSocketSetBlocking(&sock, RV_FALSE, app->logMgr);

    /* set buffer sizes and permit broadcasting */
    if (connection->isMulticast)
    {
        status = RvSocketBind(&sock, &bindAddress, NULL, app->logMgr);
        if (status != RV_OK)
        {
            /* User wanted a RAS port to listen to, but it seems the port is a little bit taken... */
            RvSocketDestruct(&sock, RV_FALSE, NULL, app->logMgr);
            RvLogError(&app->logERR,
                (&app->logERR, "RAS address is already used or invalid"));
            return -13; /* Network problem */
        }
    }
    else
    {
        /* bind the requested socket */
        portRange = app->portRange;
        status = RvSocketBind(&sock, &coreAddr, portRange, app->logMgr);
        if (status != RV_OK)
        {
            /* User wanted a RAS port to listen to, but it seems the port is a little bit taken... */
            RvSocketDestruct(&sock, RV_FALSE, NULL, app->logMgr);
            RvLogError(&app->logERR,
                (&app->logERR, "RAS address is already used or invalid"));
            return -13; /* Network problem */
        }

        /* Allow sending to multicast addresses */
        RvSocketSetBroadcast(&sock, RV_TRUE, app->logMgr);
    }

    /* Set the send and receive buffers for the socket - we need them for both unicast and multicast sockets */
    RvSocketSetBuffers(&sock, 8192, 8192, app->logMgr);

    /* Set the TTL value for multicast packets we might be sending out */
    {
        RvInt32 maxTtl;

        if (pvtGetChildByFieldId(app->hVal, app->rasConf, __q931(maxMulticastTTL), &maxTtl, NULL) < 0)
            maxTtl = 20;

        RvSocketSetMulticastTtl(&sock, maxTtl, app->logMgr);

        switch (RvAddressGetType(&coreAddr))
        {
        case RV_ADDRESS_TYPE_IPV4:
            if ((RvAddressIpv4GetIp(RvAddressGetIpv4(&coreAddr)) == RV_ADDRESS_IPV4_ANYADDRESS) || (addrType == RV_TRUE))
            {
                if (RvAddressIpv4GetIp(RvAddressGetIpv4(&bindAddress)) == RV_ADDRESS_IPV4_ANYADDRESS)
                {
                    RvUint32 numIp = 1;
                    RvHostLocalGetAddress(app->logMgr, &numIp, &bindAddress);
                }
            }
            else
                RvAddressCopy(&coreAddr, &bindAddress);
            break;

        case RV_ADDRESS_TYPE_IPV6:
            {
                /* coreAddr should be cool enough as it is (no any-address in IPv6) */
            }
            break;

        default:
            RvLogError(&app->logERR,
                (&app->logERR, "Bad RAS address - unable to check it"));
            RvSocketDestruct(&sock, RV_FALSE, portRange, app->logMgr);
            return -13;
        }

        if (addrType != RV_TRUE)
        {
            /* We are not dealing with a multicast address */
            status = RvSocketSetMulticastInterface(&sock, &bindAddress, app->logMgr);
            if (status != RV_OK)
            {
                /* User wanted a RAS port to listen to, but it seems the port is a little bit taken... */
                RvSocketDestruct(&sock, RV_FALSE, portRange, app->logMgr);
                RvLogError(&app->logERR,
                    (&app->logERR, "Unable to set the multicast interface"));
                return -13; /* Network problem */
            }
        }

        if (connection->isMulticast)
        {
            status = RvSocketJoinMulticastGroup(&sock, &coreAddr, &bindAddress, app->logMgr);
            /* todo: This should be included, but for now we closed it here for automatic tests with GK
            if (status != RV_OK)
            {
                RvSocketDestruct(&sock, RV_FALSE, NULL, app->logMgr);
                RvLogError(&app->logERR,
                    (&app->logERR, "Cannot join multicast group for a multicast RAS address"));
                return -13;
            }*/
        }
    }

    /* todo: replace with a generic function */
    /* Update the list of addresses the stack holds */
    {
        RvAddress localAddr;
        if (RvAddressGetIpPort(&coreAddr) == RV_ADDRESS_IPV4_ANYPORT)
        {
            /* Get the port right from the socket */
            RvSocketGetLocalAddress(&sock, app->logMgr, &localAddr);
            RvAddressSetIpPort(&coreAddr, RvAddressGetIpPort(&localAddr));
        }

        switch (RvAddressGetType(&coreAddr))
        {
        case RV_ADDRESS_TYPE_IPV4:
            if (RvAddressIpv4GetIp(RvAddressGetIpv4(&coreAddr)) == RV_ADDRESS_IPV4_ANYADDRESS)
            {
                RvUint32 numIp = 1;
                RvUint16 port = RvAddressGetIpPort(&coreAddr);
                RvHostLocalGetAddress(app->logMgr, &numIp, &coreAddr);
                RvAddressSetIpPort(&coreAddr, port);
            }
            break;

        case RV_ADDRESS_TYPE_IPV6:
            /* IPv6 addresses cannot be "any", so no need to check for a real address.
               We just use the one we have. */
            break;

#if defined(RV_DEBUG)
        default:
            RvLogError(&app->logERR,
                (&app->logERR, "Bad RAS address - unable to check it"));
            RvSocketDestruct(&sock, RV_FALSE, portRange, app->logMgr);
            return -13;
#endif
        }

        RvH323CoreToCmAddress(&coreAddr, addr);

        newAddrId = pvtAdd(app->hVal, app->rasAddresses, __anyField, 0, NULL, NULL);
        cmLTAToVt(app->hVal, newAddrId, addr);

        /*get NAT translation of added ras address*/
        if((app->cmNATMode == cmNatModeOnlyNAT || !app->gatekeeperInNat)  &&
            RV_PVT_NODEID_IS_VALID( app->natrasAddresses))
        {
            cmTransportAddress ta = *addr;
            RvChar tmpaddr[RV_TRANSPORT_ADDRESS_STRINGSIZE];
            if(app->cmMyEvent.cmEvNatRequest)
            {
                cmTransportAddress2String(&ta , sizeof(tmpaddr) , tmpaddr);
                /*get translation*/
                cmiCBEnter(((HAPP)app, "cmEvNATRequest(hApp=%p,AddressType=RAS,address: %s)",
                            app, tmpaddr));
                            app->cmMyEvent.cmEvNatRequest((HAPP)app,
                                                            cmProtocolRAS,
                                                            &ta);
                cmTransportAddress2String(&ta , sizeof(tmpaddr) , tmpaddr);
                cmiCBExit(((HAPP)app, "cmEvNATRequest(NatAddress: %s)", tmpaddr));
                /*add address to natrasAddresses*/
                newAddrId = pvtAdd(app->hVal, app->natrasAddresses, __anyField, 0, NULL, NULL);
                cmLTAToVt(app->hVal, newAddrId, &ta);
            }
        }
    }

    /* Wait for messages if we can */
    RvFdConstruct(&connection->fd, &sock, app->logMgr);
    RvSelectAdd(app->selectEngine, &connection->fd, RV_SELECT_READ, RvRasSelectCb);

    app->numRasListeningSockets++;
    connection->index = app->numRasListeningSockets;

    return RV_OK;
}



/************************************************************************
 * rasStart
 * purpose: Start the RAS module and all the network related with
 *          RAS.
 * input  : app     - Stack handle
 *          restart - Are we restarting the addresses only?
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasStart(IN cmElem* app, IN RvBool restart)
{
    cmTransportAddress  tmpAddr;
    RvStatus            status;
    RvRandom            randomValue;
    RvPvtNodeId         nodeId;
    RvBool              sendRegister = RV_TRUE;
    autorasEndpoint*    autoras;

    app->numRasListeningSockets = 0;
    autoras = (autorasEndpoint *)app->hAutoRas;

    /* Make sure we update the application about its RAS address */
    pvtDelete(app->hVal, app->rasAddresses);
    pvtDelete(app->hVal, app->natrasAddresses);

    /*get ras listening addresses from configuration*/
    app->rasAddresses = pvtAddRootByPath(app->hVal, app->synConfRAS, "multipleRASAddresses", 0, NULL);
    if (app->cmNATMode == cmNatModeOnlyNAT || !app->gatekeeperInNat)
        app->natrasAddresses = pvtAddRootByPath(app->hVal, app->synConfRAS, "multipleRASAddresses", 0, NULL);

    nodeId = pvtGetChild(app->hVal, app->rasConf, __q931(multipleRASAddresses), NULL);
    if (RV_PVT_NODEID_IS_VALID(nodeId))
    {
        /* We have multiple RAS listening addresses to handle */
        nodeId = pvtChild(app->hVal, nodeId);
        while (RV_PVT_NODEID_IS_VALID(nodeId))
        {
            cmVtToLTA(app->hVal, nodeId, &tmpAddr);
            status = addRasAddress(app, &tmpAddr, -1);
            if (status != RV_OK)
                return status;

            nodeId = pvtBrother(app->hVal, nodeId);
        }
    }
    else
    {
        /* Single address only */
        RvInt32 udpPort = 0;

        /* Deal with the unicast address first... */
        pvtGetChildByFieldId(app->hVal, app->rasConf, __q931(rasPort), &udpPort, NULL);
        memcpy(&tmpAddr, &app->localAddress, sizeof(tmpAddr));
        tmpAddr.port = (RvUint16)udpPort;
        status = addRasAddress(app, &tmpAddr, RV_FALSE);
        if (status != RV_OK)
            return status;

        /* Gatekeepers should also set a multicast address for listening */
        if (app->gatekeeper)
        {
            /* Get the multicast address the RAS is working with and set it */
            nodeId = pvtGetChild(app->hVal, app->rasConf, __q931(rasMulticastAddress), NULL);
            if (RV_PVT_NODEID_IS_VALID(nodeId))
            {
                /* Get the address and port of the multicast address from the configuration */
                cmVtToTA(app->hVal, nodeId, &tmpAddr);
                status = addRasAddress(app, &tmpAddr, RV_TRUE);
                if (status != RV_OK)
                    return status;
            }
        }
    }

    if(!restart)
    {
        /* Start the RAS and set the CS address */
        RvRandomGeneratorGetValue(&app->randomGenerator, &randomValue);
        if(app->cmNATMode == cmNatModeNoNAT ||  app->gatekeeperInNat) /*only "real" ras addresses required*/
            cmiRASStart(app->rasManager, app->rasAddresses, (RvUint32)randomValue);
        else
            cmiRASStart(app->rasManager, app->natrasAddresses, (RvUint32)randomValue);
    }
    else
    {   if(app->cmNATMode == cmNatModeNoNAT ||  app->gatekeeperInNat) /*only "real" ras addresses required*/
            cmiRASUpdateRasAddresses(app->rasManager, app->rasAddresses);
        else
        {
            cmiRASUpdateRasAddresses(app->rasManager, app->natrasAddresses);
        }

    }
    if (app->cmNATMode == cmNatModeNoNAT) /*only "real" q931 addresses required*/
        cmiRASUpdateCallSignalingAddress(app->rasManager, app->q931TpktAddresses, app->q931AnnexEAddresses);
    else
        cmiRASUpdateCallSignalingAddress(app->rasManager, app->natq931TpktAddresses, app->natq931AnnexEAddresses);

    /* Start automatic RAS */
    if (!app->manualRAS)
    {
        if(!restart)
            cmiAutoRASStart((HAPP)app);

        if (pvtGetChild(app->hVal, app->rasConf, __q931(manualRegistration), NULL) < 0)
        {
            if(restart)
            {
                /*if its a restart and endpoint was not registered or tried to register
                dont send RRQ*/
                if ((autoras->state != cmIdle) &&
                    (autoras->internalState == autorasRegNotTried))
                    sendRegister = RV_FALSE;
            }
            if(sendRegister)
            {
                /* Seems like our automatic RAS should try and register when starting */
                cmRegister((HAPP)app);
            }
        }
    }

    return RV_OK;
}


/************************************************************************
 * rasStop
 * purpose: Stop the RAS module (if not a restart) and all the network related with
 *          RAS.
 *          This makes the RAS ports to be removed and the endpoint to be
 *          unregistered.
 * input  : app     - Stack handle
 *          restart - if RV_TRUE the ras module is not stopped and ras addresses
 *          tree is not destroyed.
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasStop(IN cmElem* app, IN RvBool restart)
{
    int status = 0;
    RasConnection *connection = emaGetByIndex(app->rasListeningSockets, 0);

    if (!app->manualRAS)
        status = cmUnregister((HAPP)app);


    while (connection && connection->index > 0)
    {
             RvSocket *sock;
             connection->index = 0;
             sock = RvFdGetSocket(&connection->fd);
             /* It's preferable to leave the multicast group properly before closing the socket...
                problem is, we do not keep the multicast addresses. */
             RvSelectRemove(app->selectEngine, &connection->fd);
             RvSocketShutdown(sock, RV_FALSE, app->logMgr);
             RvSocketDestruct(sock, RV_FALSE, app->portRange, app->logMgr);
             RvFdDestruct(&connection->fd);
             connection = emaGetNext(app->rasListeningSockets, connection);
    }

    if (!restart)
    {
        /*if restart we don't want to stop the ras instance*/
        cmiRASStop(app->rasManager);
    }
    pvtDelete(app->hVal, app->rasAddresses);
    app->rasAddresses = RV_PVT_INVALID_NODEID;
    pvtDelete(app->hVal, app->natrasAddresses);
    app->natrasAddresses = RV_PVT_INVALID_NODEID;

    return status;
}


/************************************************************************
 * rasEnd
 * purpose: End the RAS module and all the network related with
 *          RAS.
 * input  : app     - Stack handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasEnd(IN cmElem* app)
{
    if (app->rasManager != NULL)
        cmiRASEnd(app->rasManager);
    if (app->hAutoRas)
        cmiAutoRASEnd((HAPP)app);
    if (app->rasListeningSockets != NULL)
    {
        emaClear(app->rasListeningSockets);
        emaDestruct(app->rasListeningSockets);
    }
    return 0;
}


/************************************************************************
 *
 *                        Public API functions
 *
 ************************************************************************/


/************************************************************************
 * cmGetRASConfigurationHandle
 * purpose: Gets the root Node ID of the RAS configuration tree.
 *          The application can then get and change configuration parameters
 *          for the control procedures.
 * input  : hApp     - Application's stack handle
 * output : none
 * return : The PVT Node ID of the RASConfiguration subtree on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV cmGetRASConfigurationHandle(
        IN  HAPP             hApp)
{
    cmElem* app = (cmElem *)hApp;
    if (!hApp) return RV_ERROR_UNKNOWN;

    cmiAPIEnter((hApp, "cmGetRASConfigurationHandle: hApp(%p)", hApp));

    cmiAPIExit((hApp, "cmGetRASConfigurationHandle=%d", app->rasConf));

    return app->rasConf;
}


/************************************************************************
 * cmGetLocalRASAddress
 * purpose: Gets the local RAS address
 * input  : hApp    - Application's stack handle
 * output : tr      - The local RAS signal transport address.
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV cmGetLocalRASAddress(
                IN  HAPP                hApp,
                OUT cmTransportAddress* tr)
{
    cmElem* app = (cmElem *)hApp;
    int status;

    if (!hApp) return RV_ERROR_UNKNOWN;
    cmiAPIEnter((hApp, "cmGetLocalRASAddress(hApp=%p)", hApp));
    status = cmVtToLTA(app->hVal, pvtChild(app->hVal, app->rasAddresses), tr);
    cmiAPIExit((hApp, "cmGetLocalRASAddress=%d", status));
    return status;
}



#ifdef __cplusplus
}
#endif


