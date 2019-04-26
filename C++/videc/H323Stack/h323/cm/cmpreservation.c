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

#include "cm.h"
#include "cmintr.h"
#include "cmutils.h"
#include "cmCall.h"
#include "cmControl.h"
#include "transportint.h"
#include "transutil.h"
#include "transnet.h"
#include "q931asn1.h"
#include "cmQ931.h"
#include "cmiQ931.h"
#include "h245.h"
#include "cmH245Object.h"
#include "cmcallobj.h"
#include "cmpreservation.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


static RvBool preservationTimeout(void* context);
RvBool channelTimeoutEventsHandler(void*hsChan);

/************************************************************************
 * encodeIntoBuffer
 * purpose: encode a nodeID into the buffer, adding to it's length.
 * input  : hVal      - pvt thingie
 *          nodeId    - nodeId to encode
 *          bufferLen - Size of the buffer
 *          length    - pointer to the length done so far
 *          ret       - current status
 * output : buffer    - The buffer with the encoded nodeId
 *          length    - updated length done
 *          ret       - updated return value
 * return : none
 ************************************************************************/
static void encodeIntoBuffer(
    IN    HPVT             hVal,
    IN    int              nodeId,
    OUT   RvUint8        * buffer,
    IN    RvSize_t         bufferLen,
    INOUT RvSize_t       * length,
    INOUT RvH323HaResult * ret
    )
{
    int encLen = 0;
    int len = bufferLen - *length - sizeof(int);

    if( (nodeId < 0) || ((int)(*ret) < 0) )
        return;

    if( (len > 0) &&
        (cmEmEncodeTolerant(hVal, nodeId, buffer+*length+sizeof(int), len, &encLen) >= 0) )
    {
        /* copy the length before the encoded buffer */
        memcpy((void*)(buffer+*length), (void*)&encLen, sizeof(int));
        *length += encLen+sizeof(int);
    }
    else
    {
        *length += encLen;
        if (len <= encLen)
            *ret = RvH323HaNotEnoughBuffer;
        else
            *ret = RvH323HaError;
    }
}

/************************************************************************
* decodeFromBuffer
* purpose: decode the buffer into a node Id.
* input  : hVal      - pvt thingie
*          nodeId    - pointer to nodeId to decode to
*          length    - Size of the buffer
*          lenDone   - pointer to the length done so far
*          buffer    - The buffer with the encoded nodeId
* output : lenDone   - updated length done
* return : negative on error
************************************************************************/
static RvStatus decodeFromBuffer(
    IN    HPVT             hVal,
    IN    HPST             hSyn,
    IN    int            * nodeId,
    IN    RvUint8        * buffer,
    IN    RvSize_t         length,
    INOUT RvSize_t       * lenDone,
    INOUT RvStatus       * ret
    )
{
    int decLen = 0;
    int decoded = 0;

    if ((*ret != RV_OK) || !RV_PVT_NODEID_IS_VALID(*nodeId))
        return *ret;

    if((length-*lenDone) >= sizeof(int))
        memcpy((void*)&decLen, (void*)(buffer+*lenDone), sizeof(int));
    else
    {
        *nodeId = -1;
        *ret = RV_ERROR_UNKNOWN;
        return RV_ERROR_UNKNOWN;
    }
    *lenDone += sizeof(int);

    *nodeId = pvtAddRoot(hVal,hSyn,0,NULL);
    if((cmEmDecode(hVal, *nodeId, buffer+*lenDone, decLen, &decoded) < 0) ||
        (decLen != decoded))
    {
        pvtDelete(hVal, *nodeId);
        *nodeId = -1;
        *ret = RV_ERROR_UNKNOWN;
        return RV_ERROR_UNKNOWN;
    }
    *lenDone += decLen;
    return RV_OK;
}


/************************************************************************
 * callToBuffer
 * purpose: Create a buffer of the call, the session, its property, and
 *          its channels.
 * input  : call      - Call element to buffer
 *          bufferLen - Size of the buffer
 * output : buffer    - The buffer to which the call will be added
 *          bufferLen - Size used or size needed if not enough allocated
 * return : negative on error
 ************************************************************************/
static RvH323HaResult callToBuffer(
    IN    callElem * call,
    OUT   RvUint8  * buffer,
    INOUT RvSize_t * bufferLen
    )
{
    RvSize_t length = 0;
    RvH323HaResult ret = RvH323HaOK;
    H245Channel* chanList, * curChan, * chanArray[20];
    H245Control* ctrl;
    cmElem *app = (cmElem *)emaGetInstance((EMAElement)call);
    HPVT hVal;
    int i = 0;

    hVal = app->hVal;

    /* copy the call to the buffer */
    if((length+sizeof(callElem)) <= *bufferLen)
    {
        memcpy((void*)(buffer+length), (void*)call, sizeof(callElem));
        if(call->timer)
            ret = RvH323HaTimerWarning;
    }
    else
        ret = RvH323HaNotEnoughBuffer;
    length += sizeof(callElem);

    /* copy the control element to the buffer */
    ctrl = (H245Control *)cmiGetControl((HCALL)call);
    if(((int)ret >=0) && ((length+sizeof(H245Control)) <= *bufferLen))
    {
        memcpy((void*)(buffer+length), (void*)ctrl, sizeof(H245Control));
        if( (ctrl->outCap.timer != NULL) ||
            (ctrl->msd.timer != NULL)    ||
            (ctrl->roundTrip.timer != NULL)    ||
            (ctrl->outRequestMode.timer != NULL) )
        {
            ret = RvH323HaTimerWarning;
        }
    }
    else
        ret = RvH323HaNotEnoughBuffer;
    length += sizeof(H245Control);
    if (ctrl->eState != ctrlNotInitialized)
    {
        /* encode the capabilities into the buffer, if they exists */
        encodeIntoBuffer(hVal, ctrl->outCap.termNodeId, buffer, *bufferLen, &length, &ret);
        encodeIntoBuffer(hVal, ctrl->inCap.termNodeId,  buffer, *bufferLen, &length, &ret);
    }

    /* copy the session to the buffer, if one exists */
    if(call->hsTransSession)
    {
        cmTransSession * session = (cmTransSession *) call->hsTransSession;
        if(((int)ret >= 0) && (length+sizeof(cmTransSession)) <= *bufferLen)
        {
            memcpy((void*)(buffer+length), (void*)session, sizeof(cmTransSession));
        }
        else
            ret = RvH323HaNotEnoughBuffer;
        length += sizeof(cmTransSession);
    }

    /* encode the property into the buffer, if it exists */
    encodeIntoBuffer(hVal, call->property,  buffer, *bufferLen, &length, &ret);
    encodeIntoBuffer(hVal, call->callSignalAddress,  buffer, *bufferLen, &length, &ret);
    encodeIntoBuffer(hVal, call->remoteCallSignalAddress,  buffer, *bufferLen, &length, &ret);
    encodeIntoBuffer(hVal, call->annexECallSignalAddress,  buffer, *bufferLen, &length, &ret);

    /*encodeIntoBuffer(hVal, call->destinationInfo,  buffer, *bufferLen, &length, &ret);*/
    if (app->callPropertyMode == pmodeDoNotUseProperty)
    {
        callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, (HCALL)call);
        encodeIntoBuffer(hVal, props->lastSentMessageNodeId,  buffer, *bufferLen, &length, &ret);
        for (i = (int)cmEnumSetupSendingComplete; (i<(int)cmEnumLast) && ((int)ret >= 0); i++)
            encodeIntoBuffer(hVal, props->paramNodeId[i],  buffer, *bufferLen, &length, &ret);
    }

    if (length+sizeof(int) <= *bufferLen)
    {
        int q931CallState = (int)((q931Elem*)cmiGetQ931((HCALL)call))->callState;
        memcpy((void*)(buffer+length), (void*)&q931CallState, sizeof(int));
    }
    length += sizeof(int);

    i=0;
    /* copy channels to buffer, but also an array of the channel pointer by the order they are linked */
    curChan = chanList = (H245Channel *) cmiGetChannelListForCtrl(cmiGetControl((HCALL)call));
    while(curChan)
    {
        chanArray[i] = curChan;
        curChan = curChan->pNext;
        i++;
    }

    if( ((int)ret >= 0) && (length+i*sizeof(H245Channel *)+sizeof(int) <= *bufferLen) )
    {
        /* copy the channel array first, it will help us later when we need to decipher which channel
        is which's partner */
        memcpy((void*)(buffer+length), (void*)&i, sizeof(int));
        memcpy((void*)(buffer+length+sizeof(int)), (void*)chanArray, (RvSize_t)(i*sizeof(H245Channel *)));
    }
    else
        ret = RvH323HaNotEnoughBuffer;
    length += i*sizeof(H245Channel *)+sizeof(int);

    curChan = chanList;
    while(curChan)
    {
        if( ((int)ret >= 0) && (length+sizeof(H245Channel) <= *bufferLen) )
        {
            /* copy the channel */
            memcpy((void*)(buffer+length), (void*)curChan, sizeof(H245Channel));
            if(curChan->pTimer || curChan->pRequestCloseTimer || curChan->pMediaLoopTimer)
                ret = RvH323HaTimerWarning;
            encodeIntoBuffer(hVal, curChan->data.h225Params.redEncID,        buffer, *bufferLen, &length, &ret);
            encodeIntoBuffer(hVal, curChan->data.h225Params.transCapID,      buffer, *bufferLen, &length, &ret);
            encodeIntoBuffer(hVal, curChan->requestCloseParamID,                            buffer, *bufferLen, &length, &ret);
            encodeIntoBuffer(hVal, curChan->dataTypeID,                                     buffer, *bufferLen, &length, &ret);
            encodeIntoBuffer(hVal, curChan->data.h225Params.separateStackID, buffer, *bufferLen, &length, &ret);
        }
        else
            ret = RvH323HaNotEnoughBuffer;
        length += sizeof(H245Channel);
        curChan = curChan->pNext;
    }

    *bufferLen = length;
    return ret;
}


RVAPI RvH323HaResult RVCALLCONV RvH323HaGetCallBuffer(
    IN    HCALL      hsCall,
    OUT   RvUint8  * buffer,
    INOUT RvSize_t * length
    )
{
    callElem * call = (callElem *) hsCall;
    H245Control* ctrl = (H245Control*)cmiGetControl(hsCall);
    cmTransSession * session;
    cmTransHost * Q931Connection;
    cmTransHost * annexEConnection;
    cmTransHost * H245Connection;
    RvH323HaResult ret = RvH323HaOK;

    /* Lock EVERYTHING */
    if((call == NULL) || !emaLock((EMAElement)call))
    {
        *length = 0;
        return RvH323HaError;
    }

    session = (cmTransSession *)call->hsTransSession;
    if(session && !emaLock((EMAElement)session))
    {
        *length = 0;
        emaUnlock((EMAElement)call);
        return RvH323HaError;
    }

    Q931Connection = session->Q931Connection;
    annexEConnection = session->annexEConnection;
    H245Connection = session->H245Connection;
    if(Q931Connection   && !emaLock((EMAElement)Q931Connection))
    {
        *length = 0;
        if(session) emaUnlock((EMAElement)session);
        emaUnlock((EMAElement)call);
        return RvH323HaError;
    }
    if(annexEConnection && !emaLock((EMAElement)annexEConnection))
    {
        *length = 0;
        if(Q931Connection) emaUnlock((EMAElement)Q931Connection);
        if(session) emaUnlock((EMAElement)session);
        emaUnlock((EMAElement)call);
        return RvH323HaError;
    }
    if(H245Connection   && !emaLock((EMAElement)H245Connection))
    {
        *length = 0;
        if(annexEConnection) emaUnlock((EMAElement)annexEConnection);
        if(Q931Connection) emaUnlock((EMAElement)Q931Connection);
        if(session) emaUnlock((EMAElement)session);
        emaUnlock((EMAElement)call);
        return RvH323HaError;
    }
    /* ok, all is locked */

    /* now, check if we are in a stable enough state to encode the call */
    /* if we are in the middle of a Fast Start prcedure, no go. */
    if ((ctrl->fastStartIndexes[0] != (RvUint8)-1) ||
        (ctrl->fastStartNodesAcptSend[0] != -1) || (ctrl->fastStartNodesAcptRcvd[0] != -1) ||
        (ctrl->fastStartNodesPrplSend[0] != -1) || (ctrl->fastStartNodesPrplRcvd[0] != -1))
    {
        ret = RvH323HaImpossible;
    }
    /* if there are both H245 listening socket and connecting socket, no way. */
    else if(H245Connection && H245Connection->h245Listen && H245Connection->hTpkt)
    {
        ret = RvH323HaImpossible;
    }
    /* if there are messages waiting on the hosts or sessions, close, but no cigar */
    else if( (session && session->firstMessage) )
    {
        ret = RvH323HaImpossible;
    }

    /* if we have a stable enough state, go on to create the buffer */
    if((int)ret >= 0)
    {
        ret = callToBuffer(call, buffer, length);
    }

    /* now, unlock all */
    if(H245Connection) emaUnlock((EMAElement)H245Connection);
    if(annexEConnection) emaUnlock((EMAElement)annexEConnection);
    if(Q931Connection) emaUnlock((EMAElement)Q931Connection);
    if(session) emaUnlock((EMAElement)session);
    emaUnlock((EMAElement)call);

    return ret;
}

RVAPI RvStatus RVCALLCONV RvH323HaAddCall(
    IN HAPP      hApp,
    IN HAPPCALL  haCall,
    IN RvUint8 * buffer,
    IN RvSize_t  length
    )
{
    cmElem * app = (cmElem *) hApp;
    cmTransGlobals * transGlobals = (cmTransGlobals *)app->hTransport;
    callElem call;
    callElem * newCall = NULL;
    H245Control control;
    H245Control * newControl;
    cmTransSession session;
    cmTransSession * newSession = NULL;
    H245Channel chanArray[20], * chanPtrArray[20];
    H245Channel * newChannelArray[20];
    RvSize_t chanArraySize = 0;
    int i, j;
    RvSize_t lenDone = 0;
    RvBool isNewCall = RV_TRUE;
    EMAElement * emaElem = NULL;
    RvStatus ret = RV_OK;

    if(buffer == NULL)
        return RV_ERROR_UNKNOWN;

    memset(&session, 0, sizeof(session)); /* Here to remove warning in release mode compilation */

    /* Get the call from the buffer */
    if((length-lenDone) >= sizeof(callElem))
        memcpy((void*)&call, (void*)(buffer+lenDone), sizeof(callElem));
    else
        return RV_ERROR_UNKNOWN;
    lenDone += sizeof(callElem);

    /* search for the same call in the hash */
    while((emaElem = emaGetNext(app->hCalls, emaElem)) != NULL)
    {
        if( (((callElem*)emaElem)->crv == call.crv) &&
            (!strncmp((char*)((callElem*)emaElem)->callId, (char*)call.callId, 16)) )
        {
            /* found call, this is an update */
            H245Channel * curChan, * nextChan;
            H245Control * ctrl;
            newCall = (callElem*)emaElem;

            if(!(m_callget(newCall, preservedCall)))
            {
                /* call was already activated. big error. */
                return RV_ERROR_UNKNOWN;
            }
            isNewCall = RV_FALSE;
            /* remove the previous control */
            ctrl = (H245Control *) cmiGetControl((HCALL)newCall);
            if (ctrl->eState != ctrlNotInitialized)
            {
                if(ctrl->outCap.termNodeId >= 0)
                {
                    pvtDelete(app->hVal, ctrl->outCap.termNodeId);
                }
                if(ctrl->inCap.termNodeId >= 0)
                {
                    pvtDelete(app->hVal, ctrl->inCap.termNodeId);
                }
            }
            /* remove the previous property */
            if(newCall->property >= 0)
            {
                pvtDelete(app->hVal, newCall->property);
            }
            /* remove the previous session */
            if(newCall->hsTransSession)
            {
                emaDelete((EMAElement)newCall->hsTransSession);
            }
            /* remove the previous channels */
            nextChan = curChan = (H245Channel *) cmiGetChannelListForCall((HCALL)newCall);
            while(nextChan != NULL)
            {
                nextChan = curChan->pNext;
                emaDelete(curChan);
            }
            break;
        }
    }
    if(newCall == NULL)
    {
        /* if no call yet, get a new call from the EMA */
        newCall = (callElem *)emaAdd(app->hCalls,(void*)haCall);
        if (!newCall)
        {
            /* no calls in EMA - exit */
            return RV_ERROR_UNKNOWN;
        }
        if (transParentNew(app->hTransport, (void*)newCall) != RV_OK)
        {
            emaDelete((EMAElement*)newCall);
            return RV_ERROR_UNKNOWN;
        }
    }
    /* copy the preserved call into the new Call */
    memcpy((void*)newCall,(void*)&call,sizeof(callElem));
    emaSetApplicationHandle((EMAElement) newCall, (void*) haCall);
    m_callset(newCall,preservedCall,RV_TRUE);

    /* now get the preserved control element */
    if((length-lenDone) >= sizeof(H245Control))
        memcpy((void*)&control, (void*)(buffer+lenDone), sizeof(H245Control));
    else
    {
        emaDelete((EMAElement)newCall);
        return RV_ERROR_UNKNOWN;
    }
    lenDone += sizeof(H245Control);
    /* rebuild the capabilities, if needed */
    if (control.eState != ctrlNotInitialized)
    {
        if(decodeFromBuffer(app->hVal, NULL, &control.outCap.termNodeId, buffer, length, &lenDone, &ret) < 0)
        {
            emaDelete((EMAElement)newCall);
            return RV_ERROR_UNKNOWN;
        }
        if(decodeFromBuffer(app->hVal, NULL, &control.inCap.termNodeId, buffer, length, &lenDone, &ret) < 0)
        {
            pvtDelete(app->hVal, control.outCap.termNodeId);
            emaDelete((EMAElement)newCall);
            return RV_ERROR_UNKNOWN;
        }
    }
    else
    {
        control.outCap.termNodeId = -1;
        control.inCap.termNodeId = -1;
    }
    /* set the timers to NULL */
    control.outCap.timer = NULL;
    control.msd.timer = NULL;
    control.roundTrip.timer = NULL;
    control.outRequestMode.timer = NULL;
    /* copy into the control element */
    newControl = (H245Control *) cmiGetControl((HCALL)newCall);
    memcpy((void*)newControl, (void*)&control, sizeof(H245Control));

    /* Get the session, if needed */
    if(call.hsTransSession)
    {
        if((length-lenDone) >= sizeof(cmTransSession))
            memcpy((void*)&session, (void*)(buffer+lenDone), sizeof(cmTransSession));
        else
        {
            pvtDelete(app->hVal, control.inCap.termNodeId);
            pvtDelete(app->hVal, control.outCap.termNodeId);
            emaDelete((EMAElement)newCall);
            return RV_ERROR_UNKNOWN;
        }
        lenDone += sizeof(cmTransSession);
    }

    /* decode the property, if needed*/
    decodeFromBuffer(app->hVal, app->hSyn, &newCall->property, buffer, length, &lenDone, &ret);
    decodeFromBuffer(app->hVal, app->hAddrSyn, &newCall->callSignalAddress, buffer, length, &lenDone, &ret);
    decodeFromBuffer(app->hVal, app->hAddrSyn, &newCall->remoteCallSignalAddress, buffer, length, &lenDone, &ret);
    decodeFromBuffer(app->hVal, app->hAddrSyn, &newCall->annexECallSignalAddress, buffer, length, &lenDone, &ret);
    /*decodeFromBuffer(app->hVal, app->hSyn, &newCall->destinationInfo, buffer, length, &lenDone, &ret);*/
    newCall->destinationInfo = RV_PVT_INVALID_NODEID;

    if (app->callPropertyMode == pmodeDoNotUseProperty)
    {
        callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, (HCALL)newCall);
        decodeFromBuffer(app->hVal, app->hSyn, &props->lastSentMessageNodeId, buffer, length, &lenDone, &ret);
        for (i=(int)cmEnumSetupSendingComplete; (i<(int)cmEnumLast) && (ret >= 0); i++)
            decodeFromBuffer(app->hVal, app->hSyn, &(props->paramNodeId[i]),  buffer, length, &lenDone, &ret);
    }

    if (ret != RV_OK)
    {
        pvtDelete(app->hVal, newCall->property);
        pvtDelete(app->hVal, newCall->callSignalAddress);
        pvtDelete(app->hVal, newCall->remoteCallSignalAddress);
        pvtDelete(app->hVal, newCall->annexECallSignalAddress);
        pvtDelete(app->hVal, newCall->destinationInfo);
        pvtDelete(app->hVal, control.inCap.termNodeId);
        pvtDelete(app->hVal, control.outCap.termNodeId);
        if (app->callPropertyMode == pmodeDoNotUseProperty)
        {
            callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, (HCALL)newCall);
            pvtDelete(app->hVal, props->lastSentMessageNodeId);
            for(i = (int)cmEnumSetupSendingComplete; i < (int)cmEnumLast; i++)
                pvtDelete(app->hVal, props->paramNodeId[i]);
        }

        emaDelete((EMAElement)newCall);
        return ret;
    }

    {
        RvInt32 t303=-1, t304=-1, t301=180, t310, t322=100;
        q931Elem* q931Call = (q931Elem*)cmiGetQ931((HCALL)newCall);
        int q931CallState;

        /* Get the timeout values for call setup related timeouts */
        pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(connectTimeOut), &t301, NULL);
        t301 *= 1000;
        if (isNewCall)
        {
            pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(responseTimeOut), &t304, NULL);
            t303 = t304 * 1000;
            pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(t304), &t304, NULL);
            t304 *= 1000;
            pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(t322), &t322, NULL);
            t322 *= 1000;
            t310 = t301;
            pvtGetChildByFieldId(app->hVal, app->q931Conf, __q931(t310), &t310, NULL);
            t310 *= 1000;

            /* Create the Q931 "object" of the call */
            q931CallCreate((HQ931)q931Call, t301, -1, t303, t304, t310, t322);
        }

        if((length-lenDone) >= sizeof(int))
        {
            memcpy((void*)&q931CallState, (void*)(buffer+lenDone), sizeof(int));
            q931Call->callState = (callStateE)q931CallState;
        }
        else
        {
            pvtDelete(app->hVal, newCall->property);
            pvtDelete(app->hVal, newCall->callSignalAddress);
            pvtDelete(app->hVal, newCall->remoteCallSignalAddress);
            pvtDelete(app->hVal, newCall->annexECallSignalAddress);
            pvtDelete(app->hVal, newCall->destinationInfo);
            if (app->callPropertyMode == pmodeDoNotUseProperty)
            {
                callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, (HCALL)newCall);
                pvtDelete(app->hVal, props->lastSentMessageNodeId);
                for(i = (int)cmEnumSetupSendingComplete; i < (int)cmEnumLast; i++)
                    pvtDelete(app->hVal, props->paramNodeId[i]);
            }
            pvtDelete(app->hVal, control.inCap.termNodeId);
            pvtDelete(app->hVal, control.outCap.termNodeId);
            emaDelete((EMAElement)newCall);
            return RV_ERROR_UNKNOWN;
        }
        lenDone += sizeof(int);

        /* See if the call has timers */
        if(newCall->timer)
        {
            /* check the call state to see if a timer should be set */
            if( (newCall->state == cmCallStateDialtone) ||
                (newCall->state == cmCallStateProceeding) ||
                (newCall->state == cmCallStateRingBack) ||
                (newCall->state == cmCallStateOffering) ||
                (newCall->state == cmCallStateOffering) ||
                (newCall->state == cmCallStateTransfering) ||
                (newCall->state == cmCallStateWaitAddressAck) )
            {
                /* set a connect timer for disconnection */
                t301 *= 1000;
                newCall->timer = RvH323TimerStart(app->hTimers, preservationTimeout, (void*)newCall, t301);
            }
        }
    }

    /* add the new session to the call */
    if(newCall->hsTransSession)
    {
        newSession = emaAdd(transGlobals->hEmaSessions, (void*)newCall);
        transParentShare(app->hTransport, (void*)newSession, (void*)newCall);

        if(!newSession)
        {
            pvtDelete(app->hVal, newCall->property);
            pvtDelete(app->hVal, newCall->callSignalAddress);
            pvtDelete(app->hVal, newCall->remoteCallSignalAddress);
            pvtDelete(app->hVal, newCall->annexECallSignalAddress);
            pvtDelete(app->hVal, newCall->destinationInfo);
            if (app->callPropertyMode == pmodeDoNotUseProperty)
            {
                callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, (HCALL)newCall);
                pvtDelete(app->hVal, props->lastSentMessageNodeId);
                for(i = (int)cmEnumSetupSendingComplete; i < (int)cmEnumLast; i++)
                    pvtDelete(app->hVal, props->paramNodeId[i]);
            }
            pvtDelete(app->hVal, control.inCap.termNodeId);
            pvtDelete(app->hVal, control.outCap.termNodeId);
            emaDelete((EMAElement)newCall);
            return RV_ERROR_UNKNOWN;
        }

        newCall->hsTransSession = (HSTRANSSESSION) newSession;
        memcpy((void*)newSession, (void*)&session, sizeof(cmTransSession));

        /* set a few session parameters */
        if(newSession->Q931Connection)   newSession->Q931Connection =   (cmTransHost*)-1;
        if(newSession->annexEConnection) newSession->annexEConnection = (cmTransHost*)-1;
        if(newSession->H245Connection)   newSession->H245Connection =   (cmTransHost*)-1;
        newSession->nextPending = NULL;
        newSession->prevPending = NULL;
        newSession->nextSession = NULL;
        newSession->prevSession = NULL;
    }

    /* now for the channels */
    memset (chanArray, 0, sizeof(chanArray));
    memset (newChannelArray, 0, sizeof(newChannelArray));
    memset (chanPtrArray, 0, sizeof(chanPtrArray));
    if(lenDone < length)
    {
        memcpy((void*)&chanArraySize, (void*)(buffer+lenDone), sizeof(int));
        lenDone += sizeof(int);
        /* see if this call has channels */
        if(chanArraySize > 0)
        {
            memcpy((void*)chanPtrArray, (void*)(buffer+lenDone+sizeof(int)), chanArraySize*sizeof(H245Channel *));
            lenDone += chanArraySize*sizeof(H245Channel *);
            memcpy((void*)chanArray, (void*)(buffer+lenDone), chanArraySize*sizeof(H245Channel));
            lenDone += chanArraySize*sizeof(H245Channel);
        }
    }

    /* create the channel reference array */
    for(i=0; i<(int)chanArraySize; i++)
    {
        newChannelArray[i] = (H245Channel *)emaAdd(((H245Object*)app->hh245)->hChannels, NULL);
        transParentShare(app->hTransport, (void*)newChannelArray[i], (void*)newCall);
        if (newChannelArray[i])
        {
            ret = RV_OK;

            memcpy((void*)newChannelArray[i], (void*)&chanArray[i], sizeof(H245Channel));
            decodeFromBuffer(app->hVal, app->h245RedEnc, &newChannelArray[i]->data.h225Params.redEncID, buffer, length, &lenDone, &ret);
            decodeFromBuffer(app->hVal, app->h245TransCap, &newChannelArray[i]->data.h225Params.transCapID, buffer, length, &lenDone, &ret);
            decodeFromBuffer(app->hVal, app->h245DataType, &newChannelArray[i]->requestCloseParamID, buffer, length, &lenDone, &ret);
            decodeFromBuffer(app->hVal, app->h245DataType, &newChannelArray[i]->dataTypeID, buffer, length, &lenDone, &ret);
            decodeFromBuffer(app->hVal, app->h245DataType, &newChannelArray[i]->data.h225Params.separateStackID, buffer, length, &lenDone, &ret);
            
            if (ret == RV_OK)
            {
                /* all ok, skip the deleting process */
                continue;
            }
            /* if we are here, we must delete this channel... */
            pvtDelete(app->hVal, newChannelArray[i]->data.h225Params.redEncID);
            pvtDelete(app->hVal, newChannelArray[i]->data.h225Params.transCapID);
            pvtDelete(app->hVal, newChannelArray[i]->requestCloseParamID);
            pvtDelete(app->hVal, newChannelArray[i]->dataTypeID);
            pvtDelete(app->hVal, newChannelArray[i]->data.h225Params.separateStackID);
            emaDelete((EMAElement)newChannelArray[i]);
        }
        /* ...and all those before it... */
        for(j=0; j < i; j++)
        {
            pvtDelete(app->hVal, newChannelArray[j]->data.h225Params.redEncID);
            pvtDelete(app->hVal, newChannelArray[j]->data.h225Params.transCapID);
            pvtDelete(app->hVal, newChannelArray[j]->requestCloseParamID);
            pvtDelete(app->hVal, newChannelArray[j]->dataTypeID);
            pvtDelete(app->hVal, newChannelArray[j]->data.h225Params.separateStackID);
            emaDelete((EMAElement)newChannelArray[j]);
        }
        /* ...and the call itself */
        emaDelete((EMAElement)newSession);
        pvtDelete(app->hVal, newCall->property);
        pvtDelete(app->hVal, newCall->callSignalAddress);
        pvtDelete(app->hVal, newCall->remoteCallSignalAddress);
        pvtDelete(app->hVal, newCall->annexECallSignalAddress);
        pvtDelete(app->hVal, newCall->destinationInfo);
        if (app->callPropertyMode == pmodeDoNotUseProperty)
        {
            callNoPropertiesElem *props = (callNoPropertiesElem *)cmiGetNoPropertiesParams((HAPP)app, (HCALL)newCall);
            pvtDelete(app->hVal, props->lastSentMessageNodeId);
            for(i = (int)cmEnumSetupSendingComplete; i < (int)cmEnumLast; i++)
                pvtDelete(app->hVal, props->paramNodeId[i]);
        }
        pvtDelete(app->hVal, control.inCap.termNodeId);
        pvtDelete(app->hVal, control.outCap.termNodeId);
        emaDelete((EMAElement)newCall);
        return RV_ERROR_UNKNOWN;
    }

    /* set the channel pointers to the correct chanels */
    for(i=0; i<(int)chanArraySize; i++)
    {
        if (newChannelArray[i]->data.h225Params.pPartner)
        {
            for (j=0; j<(int)chanArraySize; j++)
            {
                if(chanPtrArray[j] == newChannelArray[i]->data.h225Params.pPartner)
                {
                    newChannelArray[i]->data.h225Params.pPartner = newChannelArray[j];
                    break;
                }
            }
            if (j==(int)chanArraySize)
                newChannelArray[i]->data.h225Params.pPartner = NULL;
        }

        if(newChannelArray[i]->pBase)
        {
            for(j=0; j<(int)chanArraySize; j++)
            {
                if(chanPtrArray[j] == newChannelArray[i]->pBase)
                {
                    newChannelArray[i]->pBase = newChannelArray[j];
                    break;
                }
            }
            if(j==(int)chanArraySize)
                newChannelArray[i]->pBase = NULL;
        }

        if(newChannelArray[i]->data.h225Params.pAssociated)
        {
            for(j=0; j<(int)chanArraySize; j++)
            {
                if(chanPtrArray[j] == newChannelArray[i]->data.h225Params.pAssociated)
                {
                    newChannelArray[i]->data.h225Params.pAssociated = newChannelArray[j];
                    break;
                }
            }
            if(j==(int)chanArraySize)
                newChannelArray[i]->data.h225Params.pAssociated = NULL;
        }

        if(newChannelArray[i]->pReplacementCh)
        {
            for(j=0; j<(int)chanArraySize; j++)
            {
                if(chanPtrArray[j] == newChannelArray[i]->pReplacementCh)
                {
                    newChannelArray[i]->pReplacementCh = newChannelArray[j];
                    break;
                }
            }
            if(j==(int)chanArraySize)
                newChannelArray[i]->pReplacementCh = NULL;
        }

        if(i==0) newChannelArray[i]->pPrev = NULL;
        else newChannelArray[i]->pPrev = newChannelArray[i-1];
        if(i==(int)chanArraySize) newChannelArray[i]->pNext = NULL;
        else newChannelArray[i]->pNext = newChannelArray[i+1];

        if(newChannelArray[i]->pTimer)
        {
            int timeout;
            pvtGetChildByFieldId(app->hVal, app->h245Conf, __h245(channelsTimeout), &timeout, NULL);
            timeout *= 1000;
            newChannelArray[i]->pTimer = RvH323TimerStart(app->hTimers, channelTimeoutEventsHandler, (void*)&newChannelArray[i], timeout);
        }
        newChannelArray[i]->pRequestCloseTimer = NULL;
        newChannelArray[i]->pMediaLoopTimer = NULL;

        newChannelArray[i]->hCtrl = (HCONTROL)newControl;
    }

    if(chanArraySize > 0)
        cmiSetChannelListForCall((HCALL)newCall, (HCHAN)newChannelArray[0]);
    else
        cmiSetChannelListForCall((HCALL)newCall, (HCHAN)NULL);

    return RV_OK;
}


void delCall(cmElem * app, callElem * call)
{
    /* delete the call */
    H245Channel * curChan, * nextChan;

    /* remove the previous property */
    if(call->property >= 0)
        pvtDelete(app->hVal, call->property);

    /* remove the previous cat entry */
    if(call->hCatCall)
        catDelete(app->hCat, call->hCatCall);

    /* remove the previous session */
    if(call->hsTransSession)
    {
        cmTransSession * session = (cmTransSession *)call->hsTransSession;

        if(session->hashEntry)
        {
            cmTransGlobals * transGlobals = (cmTransGlobals *)app->hTransport;

            hashDelete(transGlobals->hHashSessions, session->hashEntry);

            if(session->H245Connection)
                closeHostInternal((HSTRANSHOST)session->H245Connection, RV_FALSE);
            if(session->Q931Connection)
                closeHostInternal((HSTRANSHOST)session->Q931Connection, RV_FALSE);
            if(session->annexEConnection)
                closeHostInternal((HSTRANSHOST)session->annexEConnection, RV_FALSE);

            emaDelete((EMAElement)call->hsTransSession);
        }
    }

    /* remove the previous channels */
    nextChan = curChan = (H245Channel *) cmiGetChannelListForCall((HCALL)call);
    while(nextChan != NULL)
    {
        nextChan = curChan->pNext;
        emaDelete((EMAElement)curChan);
    }

    emaDelete((EMAElement)call);
}


RVAPI RvStatus RVCALLCONV RvH323HaDelCall(
    IN HAPP      hApp,
    IN HAPPCALL  haCall
    )
{
    cmElem * app = (cmElem *) hApp;
    EMAElement emaElem = NULL;
    callElem * call;

    /* go over all the calls in the EMA */
    while((emaElem = emaGetNext(app->hCalls, emaElem)) != NULL)
    {
        /* see if this is the call we want */
        if(haCall == (HAPPCALL)emaGetApplicationHandle(emaElem))
        {
            call = (callElem *) emaElem;

            /* see is this call is preserved */
            if(m_callget(call, preservedCall))
            {
                delCall(app, call);
                return 0;
            }
            else
            {
                /* call was already activated. big error. */
                return RV_ERROR_UNKNOWN;
            }
        }
    }
    return 0;
}


RVAPI RvStatus RVCALLCONV RvH323HaActivateCalls(
    IN HAPP                       hApp,
    IN RvH323HaActivateCallCB     activateCallCB,
    IN RvH323HaActivateChannelCB  activateChanCB)
{
    cmElem * app = (cmElem *) hApp;
    EMAElement emaElem = NULL, nextElem = NULL;
    callElem * call;

    emaElem = emaGetNext(app->hCalls, emaElem);

    /* go over all the calls in the EMA */
    while(emaElem != NULL)
    {
        nextElem = emaGetNext(app->hCalls, emaElem);
        call = (callElem *) emaElem;

        /* see is this call is preserved */
        if(m_callget(call, preservedCall))
        {
            cmTransSession * session = (cmTransSession *)call->hsTransSession;
            RvH323HaHOST *q931=NULL, *h245=NULL, *annexE=NULL;
            catStruct callObj;
            RvBool okCall = RV_TRUE;
            RvBool lockedCall = RV_FALSE;
            HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle(emaElem);

            /* mark as no longer preserved */
            m_callset(call, preservedCall, RV_FALSE);

            /* get pointers to hosts, if needed */
            if(session)
            {
                if(session->annexEConnection) annexE = (RvH323HaHOST *)&session->annexEConnection;
                else annexE = NULL;
                if(session->Q931Connection)   q931 =   (RvH323HaHOST *)&session->Q931Connection;
                else q931 = NULL;
                if(session->H245Connection)   h245 =   (RvH323HaHOST *)&session->H245Connection;
                else h245 = NULL;
            }

            /* CAT changes */
            if((callObjectCreate(app, (HCALL)call, Incoming, &callObj) < 0) ||
                ((call->hCatCall = catAdd(app->hCat, &callObj, (HCALL)call))==NULL))
            {
                call->hCatCall = NULL;
                okCall = RV_FALSE;
            }

            if( (!okCall) ||
                (activateCallCB(hApp, (HCALL)call, haCall, call->state, call->stateMode, annexE, q931, h245) < 0)  ||
                ((lockedCall = emaLock(emaElem)) == RV_FALSE) )
            {
                okCall = RV_FALSE;
                session->annexEConnection = session->Q931Connection = session->H245Connection = NULL;
            }
            else
            {
                /* connect the session to the hosts */
                if(annexE)
                {
                    cmTransHost * host = (cmTransHost *) *annexE;

                    session->nextSession = host->firstSession;
                    if (session->nextSession)
                        session->nextSession->prevSession = session;
                    host->firstSession = session;
                }

                if(q931)
                {
                    cmTransHost * host = (cmTransHost *) *q931;

                    session->nextSession = host->firstSession;
                    if (session->nextSession)
                        session->nextSession->prevSession = session;
                    host->firstSession = session;
                }

                if(h245)
                {
                    cmTransHost * host = (cmTransHost *) *h245;

                    host->firstSession = session;
                }
            }

            if(session->hashEntry)
            {
                /* add the new session to the hash table */
                cmTransGlobals * transGlobals = (cmTransGlobals *)app->hTransport;
                void * loc;
                sessionKey key;

                key.CRV  = session->CRV;
                if( !okCall ||
                    ((loc = hashAdd(transGlobals->hHashSessions,(void *)&key,(void *)&session, RV_FALSE)) == NULL) )
                {
                    okCall = RV_FALSE;
                    session->hashEntry = NULL;
                }
                else
                {
                    session->hashEntry = loc;
                }
            }

            if(okCall)
            {
                H245Channel * curChan;
                HAPPCHAN      haChan;

                /* start activating the channels */
                curChan = (H245Channel *) cmiGetChannelListForCall((HCALL)call);
                while(curChan != NULL)
                {
                    activateChanCB(hApp, (HCALL)call, (HAPPCALL)emaGetApplicationHandle(emaElem),
                        (HCHAN) curChan, &haChan);
                    emaSetApplicationHandle((EMAElement)curChan, (void*)haChan);

                    curChan = curChan->pNext;
                }
            }

            if(lockedCall)
                emaUnlock(call);

            if(!okCall)
            {
                delCall(app, call);
            }
        }
        emaElem = nextElem;
    }
    return RV_OK;
}


RVAPI RvStatus RVCALLCONV RvH323HaGetAnnexEHost(
    IN  HAPP                hApp,
    IN  HCALL               hsCall,
    IN  cmTransportAddress  *remoteAddress,
    OUT RvH323HaHOST        *hHost)
{
    cmElem * app = (cmElem *) hApp;
    cmTransGlobals *transGlobals = (cmTransGlobals *)app->hTransport;
    callElem * call = (callElem *) hsCall;
    cmTransHost *host = NULL;

    if(hHost == NULL) return RV_ERROR_BADPARAM;

    /* Try first to look for an existing one in the hash */
    host = findHostInHash(transGlobals,  remoteAddress, NULL, RV_TRUE, RV_TRUE);

    /* No host, we must create a new one (should we???) */
    if (!host)
    {
        TRANSERR res = createHostByType(transGlobals, call->hsTransSession, (HSTRANSHOST *)&host,
            cmTransQ931Conn, NULL, cmTransUseAnnexE, 0);

        if (res != cmTransOK)
        {
            *hHost = NULL;
            return RV_ERROR_UNKNOWN;
        }
        else
        {
            /* Write the new host to the hash and get its local address */
            emaSetApplicationHandle(host, transGlobals);
            setRemoteAddress(host, remoteAddress);
            annexEGetLocalAddress(transGlobals->annexECntrl, 0, &host->localAddress);
        }
    }

    *hHost = (RvH323HaHOST) host;
    return RV_OK;
}

RVAPI int RVCALLCONV RvH323HaGetMuxHost(
    IN  HAPP                hApp,
    IN  HCALL               hsCall,
    IN  HCALL               hsSameConnectionCall,
    IN  cmTransportAddress  *remoteAddress,
    OUT RvH323HaHOST        *hHost)
{
    cmElem * app = (cmElem *) hApp;
    cmTransGlobals *transGlobals = (cmTransGlobals *)app->hTransport;
    callElem * call = (callElem *) hsCall;
    callElem * sameConnectionCall = (callElem *) hsSameConnectionCall;
    cmTransHost *host = NULL;

    if(hHost == NULL) return RV_ERROR_NULLPTR;

    /* see if a connection is specified */
    if(sameConnectionCall)
    {
        if (emaLock((EMAElement)hsCall))
        {
            if (emaLock((EMAElement)hsSameConnectionCall))
            {
                int value=RV_TRUE;
                cmTransSession * session = (cmTransSession *)sameConnectionCall->hsTransSession;

                host = session->Q931Connection;
                if ((host != (cmTransHost*)-1) && 
                    (host->isMultiplexed) && (host->remoteIsMultiplexed))
                {
                    ((cmTransSession *)call->hsTransSession)->Q931Connection = host;
                    cmTransSetSessionParam(call->hsTransSession,cmTransParam_isMultiplexed,&value);
                }
                else
                    host = NULL;

                emaUnlock((EMAElement)hsSameConnectionCall);
            }
            emaUnlock((EMAElement)hsCall);
        }
    }

    if(!host)
    {
        /* Try to look for an existing one in the hash */
        host = findHostInHash(transGlobals,  remoteAddress, NULL, RV_TRUE, RV_FALSE);
    }

    *hHost = (RvH323HaHOST) host;
    return RV_OK;
}

void RvH323HaAcceptHandler(HTPKT tpkt, RvSelectEvents event, int length, int error, void* context)
{
    cmTransHost    *host    = (cmTransHost *)context;
    cmTransSession *session;
    cmTransGlobals *transGlobals;

    RV_UNUSED_ARG(length);

    session = host->firstSession;

    /* retrieve the transport module global area */
    transGlobals = (cmTransGlobals *)emaGetUserData((EMAElement)session);

    if (error)
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "RvH323HaAcceptHandler got error on event %d", event));
    }
    else
    if ((session->H245Connection != host) && (session->Q931Connection != host))
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "RvH323HaAcceptHandler mismatched session and host %p %p",session->H245Connection, host));
    }
    else
    if ((event != RvSelectAccept) || (event != RvSelectConnect))
    {
        RvLogError(&transGlobals->hLog, (&transGlobals->hLog,
            "RvH323HaAcceptHandler got event %d (this cannot happen !!!)", event));
    }

    host->state = hostConnected;

    /* set the new local address of the host */
    tpktGetAddresses(tpkt, &host->localAddress, &host->remoteAddress);

    if(host->type == cmTransQ931Conn)
    {
        /* register a new callback */
        tpktRecvNotify(tpkt, transQ931Handler, host);
    }
    if(host->type == cmTransH245Conn)
    {
        /* register a new callback */
        tpktRecvNotify(tpkt, transH245Handler, host);
    }
}


RVAPI int RVCALLCONV RvH323HaGetTPKTHost(
    IN    HAPP                hApp,
    IN    RvBool              bQ931Host,
    IN    cmTransportAddress  *remoteAddress,
    INOUT cmTransportAddress  *localAddress,
    OUT   RvH323HaHOST        *hHost)
{
    cmElem* app = (cmElem*) hApp;
    cmTransGlobals * transGlobals = (cmTransGlobals *)app->hTransport;
    HTPKT hTpkt;
    TRANSERR res;
    cmTransHost* host;
    RvH323ConnectionType connType;

    if (hHost == NULL) return RV_ERROR_NULLPTR;
    if ((remoteAddress != NULL) && (localAddress == NULL)) return RV_ERROR_NULLPTR;

    if (bQ931Host)
    {
        connType = RvH323ConnectionQ931;
    }
    else
    {
        connType = RvH323ConnectionH245;
    }

    /* create host */
    res = cmTransCreateHost((HAPPTRANS)transGlobals, NULL, NULL, (HSTRANSHOST*) &host);
    if (res != cmTransOK)
    {
        return RV_ERROR_UNKNOWN;
    }

    /* initialize */
    memset(host, 0, sizeof(cmTransHost));
    host->annexEUsageMode = cmTransNoAnnexE;
    host->closeOnNoSessions = RV_TRUE;
    host->hostIsApproved = RV_TRUE;
    host->remoteCloseOnNoSessions = RV_TRUE;
    host->reported = RV_TRUE;
    host->type = cmTransQ931Conn;

    /* see if we got a remote address to connect to */
    if (remoteAddress != NULL)
    {
        /* connect to the remote address */
        host->state = hostConnecting;
        host->incoming = RV_FALSE;

        setRemoteAddress(host, remoteAddress);

        /* open a connection and stuff */
        hTpkt = tpktOpen(transGlobals->tpktCntrl, localAddress, connType,
            tpktClient, (EMAElement)host, RvH323HaAcceptHandler, (void *)host);
        if(hTpkt == NULL)
        {
            emaDelete(host);
            return RV_ERROR_UNKNOWN;
        }

        // TODO: Ran - this might return an error
        tpktConnect(hTpkt, remoteAddress);
    }
    else
    {
        /* just listen */
        host->state = hostListenning;
        host->incoming = RV_TRUE;

        /* open a connection and stuff */
        hTpkt = tpktOpen(transGlobals->tpktCntrl, localAddress, connType,
            tpktServer, (EMAElement)host, RvH323HaAcceptHandler, (void *)host);
        if(hTpkt == NULL)
        {
            emaDelete(host);
            return RV_ERROR_UNKNOWN;
        }
    }

    host->hTpkt = hTpkt;
    *hHost = (RvH323HaHOST) host;
    return RV_OK;
}


static RvBool preservationTimeout(void* context)
{
    callElem* call = (callElem*)context;

    if (emaLock((EMAElement)call))
    {
        cmElem *app = (cmElem *)cmGetAHandle((HPROTOCOL)call);
        RvH323TimerClear(app->hTimers, &call->timer);
        cmCallDrop((HCALL)call);

        emaUnlock((EMAElement)call);
    }
    return RV_FALSE;
}

#ifdef __cplusplus
}
#endif

