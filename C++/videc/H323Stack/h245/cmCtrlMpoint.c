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


#include "rvstdio.h"
#include "cmictrl.h"
#include "pvaltreeStackApi.h"
#include "cmConf.h"
#include "caputils.h"
#include "strutils.h"
#include "copybits.h"
#include "h245.h"
#include "cmChanGet.h"
#include "cmchan.h"
#include "cmCtrlMPoint.h"
#include "cmH245Object.h"
#include "cmH245.h"
#include "cmiParams.h"
#include "rvaddress.h"

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
#include "cm.h"
#include "rvinternal.h"

RvBool cmTaHasIp(IN cmTransportAddress* ta);
RvChar * getIP(IN cmTransportAddress* cmAddress, OUT RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE]);

#endif


#ifdef __cplusplus
extern "C" {
#endif

#if (RV_H245_LEAN_H223 == RV_NO)


#define markSID(sids,sid) setBitTrue((sids),(sid))
#define checkSID(sids,sid)  getBit((sids),(sid))
#define nprn(s) ((s)?(s):"(null)")



/* Multipoint __________________________________________________________________________________*/



int multipointConference(H245Control* ctrl,int lcn)
{
    RV_UNUSED_ARG(lcn);

    if (ctrl->eState == ctrlConnected)
    {
        ctrl->eState = ctrlConference;
        cmiReportControl((HCONTROL)ctrl, cmControlStateConference, (cmControlStateMode)0);
    }
    else
    {
        ctrl->bIsConference = RV_TRUE;
    }

    return RV_TRUE;
}

int cancelMultipointConference(H245Control* ctrl,int lcn)
{
    RV_UNUSED_ARG(lcn);

    ctrl->bIsConference = RV_FALSE;
    if (ctrl->eState == ctrlConference)
    {
        ctrl->eState=ctrlConnected;
        cmiReportControl((HCONTROL)ctrl, cmControlStateConnected, (cmControlStateMode)0);
    }

    return RV_TRUE;
}

RVAPI int RVCALLCONV
cmCallStartConference(
              /* enter call conference mode */
              IN      HCALL               hsCall
              )
{
    H245Object *app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    int nodeId, message, res = RV_ERROR_UNKNOWN;
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
    if (!hsCall || !app) return RV_ERROR_UNKNOWN;

    if (cmCallMasterSlaveStatus(hsCall) != (int)cmMSMaster) return RV_ERROR_UNKNOWN; /* master only operation */

    H245APIEnter((app->pAppObject, "cmCallStartConference: hsCall=%p", hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        HPVT hVal = app->hVal;

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(indication), __h245(miscellaneousIndication));
        pvtAddBranch2(hVal,nodeId, __h245(type), __h245(multipointConference));
        pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),1,NULL,NULL);

        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);

        if (res >= 0)
        {
            if (ctrl->eState == ctrlConnected)
            {
                ctrl->eState=ctrlConference;
                cmiReportControl((HCONTROL)ctrl, cmControlStateConference, (cmControlStateMode)0);
            }
            else
            {
                ctrl->bIsConference = RV_TRUE;
            }
        }
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallStartConference=%d", res));
    return res;
}




RVAPI int RVCALLCONV
cmCallCancelConference(
               /* cancel the call conference mode */
               IN      HCALL               hsCall
               )
{
    H245Object *app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
    int nodeId, message, res = RV_ERROR_UNKNOWN;

    if (!hsCall || !app) return RV_ERROR_UNKNOWN;

    if (cmCallMasterSlaveStatus(hsCall) != (int)cmMSMaster)
        return RV_ERROR_UNKNOWN; /* only master can cancel conference mode */

    H245APIEnter((app->pAppObject, "cmCallCancelConference: hsCall=%p", hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        HPVT hVal = app->hVal;

        if (ctrl->eState!=ctrlConference)
        {
            emaUnlock((EMAElement)hsCall);
            H245APIExit((app->pAppObject, "cmCallCancelConference: [-1]. (Wrong state)"));
            return RV_ERROR_UNKNOWN;
        }

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        nodeId=pvtAddBranch2(hVal,message, __h245(indication), __h245(miscellaneousIndication));
        pvtAddBranch2(hVal,nodeId, __h245(type), __h245(cancelMultipointConference));
        pvtAdd(hVal,nodeId,__h245(logicalChannelNumber),1,NULL,NULL);

        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);

        if (res >= 0)
        {
            ctrl->bIsConference = RV_FALSE;
            if (ctrl->eState == ctrlConference)
            {
                ctrl->eState=ctrlConnected;
                cmiReportControl((HCONTROL)ctrl, cmControlStateConnected, (cmControlStateMode)0);
            }
        }
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallCancelConference=%d", res));
    return res;
}




#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
/* When the conference becomes active, the master (MC) terminal shall call cmCallDeclareMC() to
   indicate the new conference status. */
RVAPI int RVCALLCONV
cmCallDeclareMC(
        /* declare this terminal to be the MC of the call */
        IN      HCALL               hsCall
        )
{
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    H245Object *app;
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
    int nodeId, message, res = RV_ERROR_UNKNOWN;
    cmTransportAddress ta;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    if (cmCallMasterSlaveStatus(hsCall) != (int)cmMSMaster)
        return RV_ERROR_UNKNOWN; /* only master can be active MC */
    app = (H245Object*)cmiGetH245Handle(hApp);

    H245APIEnter((app->pAppObject, "cmCallDeclareMC: hsCall=%p", hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        HPVT hVal = app->hVal;

        cmGetLocalCallSignalAddress(hApp,&ta);
        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        __pvtBuildByFieldIds(nodeId,hVal,message, {_h245(indication) _h245(mcLocationIndication)
                                               _h245(signalAddress) LAST_TOKEN}, 0, NULL);

        cmTAToVt_H245(hVal, nodeId, &ta);

        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);

        if (res >= 0)
        {
            if (ctrl->eState == ctrlConnected)
            {
                ctrl->eState=ctrlConference;
                cmiReportControl((HCONTROL)ctrl, cmControlStateConference, (cmControlStateMode)0);
            }
            else
            {
                ctrl->bIsConference = RV_TRUE;
            }
        }
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallDeclareMC=%d", res));
    return res;
}


int mcLocationIndication(H245Control* ctrl, int message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT hVal = ((H245Object *)hApp)->hVal;
    cmVtToTA_H245(hVal,pvtGetChild(hVal,message,__h245(signalAddress),NULL), &(ctrl->mcLocationTA));
    if (ctrl->eState == ctrlConnected)
    {
        ctrl->eState=ctrlConference;
        cmiReportControl((HCONTROL)ctrl, cmControlStateConference, (cmControlStateMode)0);
    }
    else
    {
        ctrl->bIsConference = RV_TRUE;
    }
    return RV_TRUE;
}


/* When the conference becomes active, this function provides the address of the Active MC. */
RVAPI
int RVCALLCONV cmCallGetMcTransportAddress(
                /* get active MC address */
                IN      HCALL               hsCall,
                OUT     cmTransportAddress* ta
                )
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Object *app;
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    RvChar buff[RV_TRANSPORT_ADDRESS_STRINGSIZE];
#endif

    if (!hsCall || !hApp) return RV_ERROR_BADPARAM;
    if ((int)ctrl->mcLocationTA.distribution < 0) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;

    H245APIEnter((app->pAppObject, "cmCallGetMcTransportAddress: hsCall=%p", hsCall));

    if (emaLock((EMAElement)hsCall))
    {
        memcpy(ta, &ctrl->mcLocationTA, sizeof(*ta));
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallGetMcTransportAddress: ip=%s, port=%u [%d].",
        getIP(ta, buff), ta->port, 0));
    return 0;
}

#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY) || (defined RV_H323_COMPILE_WITH_DEAD_FUNCTIONS)

/* When the conference becomes active, this function provides the address of the Active MC. */
RVAPI
int RVCALLCONV cmCallGetMCAddress(
                /* get active MC address */
                IN      HCALL               hsCall,
                OUT     RvUint32*           ip,
                OUT     RvUint16*           port
                )
{
#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
    RV_UNUSED_ARG(hsCall);
    RV_UNUSED_ARG(ip);
    RV_UNUSED_ARG(port);
    return RV_ERROR_NOTSUPPORTED;
#else
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    H245Object *app;
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    if (((int)ctrl->mcLocationTA.distribution) < 0)
        return RV_ERROR_UNKNOWN;

    app = (H245Object*)hApp;

    H245APIEnter((app->pAppObject, "cmCallGetMCAddress: hsCall=%p, &ip=%p, &port=%p", hsCall, ip, port));

    if (emaLock((EMAElement)hsCall))
    {
        if (ip) *ip = ctrl->mcLocationTA.ip;
        if (port) *port = (RvUint16)ctrl->mcLocationTA.port;
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallGetMCAddress: ip=0x%x, port=%d [%d].", 0, (ip)?*ip:0, (port)?*port:-1));
    return 0;
#endif
}

#endif /* (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY) || (RV_H323_COMPILE_WITH_DEAD_FUNCTIONS) */

#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */



int conferenceIndication(H245Control* ctrl, int message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT hVal = ((H245Object *)hApp)->hVal;
    RvPvtNodeId childId;
    RvPstFieldId fieldId;

    childId = pvtChild(hVal, message);
    pvtGet(hVal, childId, &fieldId, NULL, NULL, NULL);

    switch (fieldId)
    {
    case __h245(sbeNumber)                              :conferenceMessage (ctrl,0,childId,h245ciSbeNumber);  break;

#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)
    case __h245(terminalNumberAssign)                   :
        {
            int         tmp;
			H245Object  *app = (H245Object*)hApp;

            /* register new terminal number */
            pvtGetChildByFieldId(hVal,childId,__h245(mcuNumber),&tmp,NULL);
            ctrl->myTerminalLabel.mcuNumber=(RvUint8)tmp;
            pvtGetChildByFieldId(hVal,childId,__h245(terminalNumber),&tmp,NULL);
            ctrl->myTerminalLabel.terminalNumber=(RvUint8)tmp;

            /* notify application that the terminal number has changed */
            if (app->cmMySessionEvent.cmEvCallTerminalNumberAssign != NULL)
            {
                HCALL hsCall = cmiGetByControl((HCONTROL)ctrl);
                HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);

                H245CBEnter((app->pAppObject, "cmEvCallTerminalNumberAssign(hsCall=%p)", hsCall));
                app->cmMySessionEvent.cmEvCallTerminalNumberAssign(
                    haCall, hsCall, ctrl->myTerminalLabel.mcuNumber, ctrl->myTerminalLabel.terminalNumber);
                H245APIExit((app->pAppObject, "cmEvCallTerminalNumberAssign=0"));
            }
        }
        break;
#endif  /* (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */

    case __h245(terminalJoinedConference)               :conferenceMessage (ctrl,0,childId,h245ciTerminalJoinedConference);  break;
    case __h245(terminalLeftConference)                 :conferenceMessage (ctrl,0,childId,h245ciTerminalLeftConference);  break;
    case __h245(seenByAtLeastOneOther)                  :conferenceMessage (ctrl,0,childId,h245ciSeenByAtLeastOneOther);  break;
    case __h245(cancelSeenByAtLeastOneOther)            :conferenceMessage (ctrl,0,childId,h245ciCancelSeenByAtLeastOneOther);  break;
    case __h245(seenByAll)                              :conferenceMessage (ctrl,0,childId,h245ciSeenByAll);  break;
    case __h245(cancelSeenByAll)                        :conferenceMessage (ctrl,0,childId,h245ciCancelSeenByAll);  break;
    case __h245(terminalYouAreSeeing)                   :conferenceMessage (ctrl,0,childId,h245ciTerminalYouAreSeeing);  break;
    case __h245(requestForFloor)                        :conferenceMessage (ctrl,0,childId,h245ciRequestForFloor);  break;
    case __h245(withdrawChairToken)                     :conferenceMessage (ctrl,0,childId,h245ciWithdrawChairToken);  break;
    case __h245(floorRequested)                         :conferenceMessage (ctrl,0,childId,h245ciFloorRequested);  break;
    case __h245(terminalYouAreSeeingInSubPictureNumber) :conferenceMessage (ctrl,0,childId,h245ciTerminalYouAreSeeingInSubPictureNumber);  break;
    case __h245(videoIndicateCompose)                   :conferenceMessage (ctrl,0,childId,h245ciVideoIndicateCompose);  break;
    default:
        break;
    }
    return RV_TRUE;
}


int conferenceCommand(H245Control* ctrl, int message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT hVal = ((H245Object *)hApp)->hVal;
    RvPvtNodeId childId;
    RvPstFieldId fieldId;

    childId = pvtChild(hVal, message);
    pvtGet(hVal, childId, &fieldId, NULL, NULL, NULL);

    switch (fieldId)
    {
    case __h245(broadcastMyLogicalChannel)      : conferenceMessage (ctrl,0,childId,h245ccBroadcastMyLogicalChannel);  break;
    case __h245(cancelBroadcastMyLogicalChannel): conferenceMessage (ctrl,0,childId,h245ccCancelBroadcastMyLogicalChannel);  break;
    case __h245(makeTerminalBroadcaster)        : conferenceMessage (ctrl,0,childId,h245ccMakeTerminalBroadcaster);  break;
    case __h245(cancelMakeTerminalBroadcaster)  : conferenceMessage (ctrl,0,childId,h245ccCancelMakeTerminalBroadcaster);  break;
    case __h245(sendThisSource)                 : conferenceMessage (ctrl,0,childId,h245ccSendThisSource);  break;
    case __h245(cancelSendThisSource)           : conferenceMessage (ctrl,0,childId,h245ccCancelSendThisSource);  break;
    case __h245(dropConference)                 : conferenceMessage (ctrl,0,childId,h245ccDropConference);  break;
    case __h245(substituteConferenceIDCommand)  : conferenceMessage (ctrl,0,childId,h245ccSubstituteConferenceIDCommand);  break;
    default:
        break;
    }
    return RV_TRUE;
}


int markFreeSID(RvUint8 sids[32],cmCapDataType type)
{
    int sid;
    if (type)
    {
        if (!checkSID(sids,type))
        {
            markSID(sids,type);
            return (int)type;
        }
    }
    sid = get1st0BitNumber(sids, 32, 255);
    if (sid <= 255)
        return sid;
    return -1;

}


#if (RV_H245_SUPPORT_H225_PARAMS == RV_YES)

RVAPI int RVCALLCONV
cmCallSetTerminalLabel(
               /* Set the remote endpoint terminal label. */
               IN      HCALL               hsCall,
               IN      cmTerminalLabel*    terminalLabel
               )
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    HPVT hVal = ((H245Object *)hApp)->hVal;
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
    int ret = RV_ERROR_UNKNOWN;
    int nodeId,message;
    int iMCU, iTerminal;
    H245Object *app;

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;

    if (!terminalLabel) return RV_ERROR_UNKNOWN;

    if (terminalLabel->mcuNumber>192) return RV_ERROR_UNKNOWN;
    if (terminalLabel->terminalNumber>192) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;

    H245APIEnter((app->pAppObject, "cmCallSetTerminalLabel: hsCall=%p label=(%d, %d)",
        hsCall, terminalLabel->mcuNumber, terminalLabel->terminalNumber));

    if (emaLock((EMAElement)hsCall))
    {
        if (cmCallMasterSlaveStatus(hsCall) == (int)cmMSMaster)
        {
            /* master only operation */
            message=pvtAddRoot(hVal,((H245Object*)hApp)->hSynProtH245,0,NULL);
            __pvtBuildByFieldIds(nodeId,hVal,message, {_h245(indication) _h245(conferenceIndication)
                                                   _h245(terminalNumberAssign) LAST_TOKEN}, 0, NULL);

            iMCU = terminalLabel->mcuNumber;
            iTerminal = terminalLabel->terminalNumber;
            pvtAdd(hVal,nodeId,__h245(mcuNumber),iMCU,NULL,NULL);
            pvtAdd(hVal,nodeId,__h245(terminalNumber),iTerminal,NULL,NULL);

            ret = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);
        }
        else
            ret = RV_ERROR_UNKNOWN;

        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallSetTerminalLabel=%d", ret));
    return ret;
}



RVAPI int RVCALLCONV /* RV_ERROR_UNKNOWN if terminal label is not defined for this terminal */
cmCallGetTerminalLabel(
               /* Get the local endpoint terminal label. */
               IN      HCALL               hsCall,
               OUT     cmTerminalLabel*    terminalLabel /* user allocated */
               )
{
    int ret=0;
    HAPP hApp=(HAPP)emaGetInstance((EMAElement)hsCall);
    H245Object *app;
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);

    if (!hsCall || !hApp) return RV_ERROR_UNKNOWN;
    if (!terminalLabel) return RV_ERROR_UNKNOWN;
    app = (H245Object*)hApp;

    H245APIEnter((app->pAppObject, "cmCallSetTerminalLabel: hsCall=%p &label=%p", hsCall, terminalLabel));

    if (emaLock((EMAElement)hsCall))
    {
        if (cmCallMasterSlaveStatus(hsCall) == (int)cmMSMaster)
        {
            terminalLabel->mcuNumber = 255;
            terminalLabel->terminalNumber = 255;
            ret = RV_ERROR_UNKNOWN;
        }
        else
        {
            *terminalLabel=ctrl->myTerminalLabel;
            if (terminalLabel->mcuNumber>192) ret=RV_ERROR_UNKNOWN;
        }
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallSetTerminalLabel: label=(%d, %d) [%d].",terminalLabel->mcuNumber, terminalLabel->terminalNumber, ret));
    return ret;
}


static
int cmCallSetChannelsInternal(
                 /* As an Active MC, set transfer channels for remote terminal */
        IN      HCALL               hsCall,
        IN      int                 channelSetSize, /* number of elements in channelSet */
        IN      cmChannelStruct     channelSet[],
        IN      RvBool              useNewFields
        );





/* The master (MC) terminal shall use cmCallSetChannels() to force the remote terminal to open
   the indicated channels for transmit. */
RVAPI
int RVCALLCONV cmCallSetChannels(
                   /* As an Active MC, set transfer channels for remote terminal */
                   IN      HCALL               hsCall,
                   IN      int                 channelSetSize, /* number of elements in channelSet */
                   IN      cmChannelStruct     channelSet[]
                   )
{
    int ret;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    H245Object *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
#endif

    H245APIEnter((app->pAppObject, "cmCallSetChannels: hsCall=%p channelSetSize=%d", hsCall, channelSetSize));
    ret = cmCallSetChannelsInternal(hsCall,channelSetSize,channelSet,RV_FALSE);
    H245APIExit((app->pAppObject, "cmCallSetChannels=%d", ret));
    return ret;
}

RVAPI
int RVCALLCONV cmCallSetChannelsExt(
                   /* As an Active MC, set transfer channels for remote terminal */
                   IN      HCALL               hsCall,
                   IN      int                 channelSetSize, /* number of elements in channelSet */
                   IN      cmChannelStruct     channelSet[]
                   )
{
    int ret;
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER | RV_LOGLEVEL_LEAVE))
    H245Object *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
#endif

    H245APIEnter((app->pAppObject, "cmCallSetChannelsExt: hsCall=%p channelSetSize=%d", hsCall, channelSetSize));
    ret = cmCallSetChannelsInternal(hsCall,channelSetSize,channelSet,RV_TRUE);
    H245APIExit((app->pAppObject, "cmCallSetChannelsExt=%d",ret));
    return ret;
}


static
int cmCallSetChannelsInternal(
                 /* As an Active MC, set transfer channels for remote terminal */
        IN      HCALL               hsCall,
        IN      int                 channelSetSize, /* number of elements in channelSet */
        IN      cmChannelStruct     channelSet[],
        IN      RvBool              useNewFields
        )
{
    H245Object *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
    int i, ret=RV_TRUE;
    int message, tableId,  elemId=-1;
    RvBool dependencySet=RV_TRUE;
    H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
    RvUint8 sids[32];

    if (!hsCall || !app || channelSetSize<=0) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmCallSetChannels: hsCall=%p, setSize=%d, &set=%p", hsCall, channelSetSize, channelSet));

    if (emaLock((EMAElement)hsCall))
    {
        HPVT hVal = app->hVal;

        if (ctrl->eState!=ctrlConference || !ctrl->bIsMaster)
        {
            emaUnlock((EMAElement)hsCall);
            H245APIExit((app->pAppObject, "cmCallSetChannelsExt: [-1]. (Wrong state)"));
            return RV_ERROR_UNKNOWN;
        }

        message=pvtAddRoot(hVal,app->hSynProtH245,0,NULL);
        __pvtBuildByFieldIds(tableId,hVal,message, {_h245(command) _h245(communicationModeCommand)
                                               _h245(communicationModeTable) LAST_TOKEN}, 0, NULL);

        memset(sids,0,sizeof(sids));
        for (i=0; i<channelSetSize; i++)
        {
            /*Go through all the incoming channels */
            /*Their SID must be preserved */
            H245Channel*channel=(H245Channel*)channelSet[i].hsChan;

            elemId=pvtAddBranch(hVal, tableId, -800);

            if (channel && !channel->bOrigin &&
                channel->data.h225Params.sessionId!=0)
            {
                pvtAdd(hVal, elemId, __h245(sessionID), channel->data.h225Params.sessionId, NULL, NULL);
                markSID(sids,channel->data.h225Params.sessionId);
            }
        }
        elemId = pvtChild(hVal,tableId);
        for (i=0; i<channelSetSize; i++)
        {
            /*Now go through all the outgoing channels */
            /*Try to preserve their SID*/
            H245Channel*channel=(H245Channel*)channelSet[i].hsChan;

            if (channel && channel->bOrigin &&
                channel->data.h225Params.sessionId!=0
                && !checkSID(sids,channel->data.h225Params.sessionId))
            {
                pvtAdd(hVal, elemId, __h245(sessionID), channel->data.h225Params.sessionId, NULL, NULL);
                markSID(sids,channel->data.h225Params.sessionId);
            }
            elemId = pvtBrother(hVal,elemId);
        }

        elemId = pvtChild(hVal,tableId);
        for (i=0; i<channelSetSize; i++)
        {
            H245Channel*channel=(H245Channel*)channelSet[i].hsChan;
            RvChar addr1str[55];
            RvChar addr2str[55];
            cmTransportAddress2String(&channelSet[i].rtpAddress, sizeof(addr1str), addr1str);
            cmTransportAddress2String(&channelSet[i].rtcpAddress, sizeof(addr1str), addr2str);
            RvLogDebug(&app->log,
                (&app->log, "cmCallSetChannels: [%d] hs=%p '%s'(%d) label: %d/%d desc:'%s' rtp=%s rtcp=%s.",
                 i, channelSet[i].hsChan,
                 nprn(channelSet[i].channelName), channelSet[i].channelDataTypeHandle,
                 channelSet[i]. terminalLabel.mcuNumber, channelSet[i].terminalLabel.terminalNumber,
                 nprn(channelSet[i].sessionDescription), addr1str, addr2str));

            /* ADD dataType field */
            {
                int dataId = pvtAddBranch(hVal, elemId, __h245(dataType));
                if (channelSet[i].channelName != NULL || channelSet[i].channelDataTypeHandle >=0)
                { /* new type */
                    if (channelSet[i].channelName)
                    {
                        if (confGetDataType(hVal, app->h245Conf, channelSet[i].channelName, dataId, NULL, RV_TRUE) <0)
                        {
                            RvLogError(&app->log,
                                (&app->log, "cmCallSetChannels: No configuration data type available for %s",nprn(channelSet[i].channelName)));
                            ret=-1;
                            break;
                        }
                    }
                    else
                    {
                        ret = pvtAddTree(hVal, dataId, hVal, (int)channelSet[i].channelDataTypeHandle);
                    }
                }
                else
                    if (channel && channel->dataTypeID>=0)
                    {
                        int nonH235DataTypeId=pvtGetChild2(hVal,channel->dataTypeID,__h245(h235Media) , __h245(mediaType));
                        pvtAddTree(hVal, dataId, hVal, pvtChild(hVal,(int)(nonH235DataTypeId>=0)?nonH235DataTypeId:channel->dataTypeID));
                    }
                    else
                    { /* dataType must be set in some way*/
                        RvLogError(&app->log, (&app->log,
                            "cmCallSetChannels: No data type available"));
                        ret=-1;
                        break;
                    }

                /* Add sessionID  field in the cases it was impossible to determine it before*/
                if (pvtGetChild(hVal,elemId,__h245(sessionID),NULL)<=0)
                {
                    RvPstFieldId fieldId;
                    cmCapDataType type=cmCapEmpty;
                    int sid;
                    pvtGet(hVal,pvtChild(hVal,dataId),&fieldId,NULL,NULL,NULL);
                    switch (fieldId)
                    {
                        case    __h245(audioData):  type=cmCapAudio;break;
                        case    __h245(videoData):  type=cmCapVideo;break;
                        case    __h245(data):       type=cmCapData; break;
                        default: break;
                    }
                    sid = markFreeSID(sids, type);
                    pvtAdd(hVal, elemId, __h245(sessionID), sid, NULL, NULL);
                }
            }

            /* ADD redundancyEncoding field */
            if (useNewFields)
            {
                int redEncId, redEncodingId=RV_ERROR_UNKNOWN;
                if (channelSet[i].redEncodingId>=0)
                    redEncodingId=channelSet[i].redEncodingId;
                else
                    if (channel && channel->data.h225Params.redEncID>=0)
                        redEncodingId=channel->data.h225Params.redEncID;
                if (redEncodingId>=0)
                {
                    redEncId=pvtAddBranch(hVal, elemId, __h245(redundancyEncoding));
                    pvtSetTree(hVal,redEncId,hVal,redEncodingId);
                }
            }

            /* ADD associatedSessionID field */
            if (channelSet[i].hsChanAssociate)
            {
                H245Channel* associatedChan = (H245Channel*)channelSet[i].hsChanAssociate;
                pvtAdd(hVal, elemId, __h245(associatedSessionID), associatedChan->data.h225Params.sessionId, NULL, NULL);
            }

            /* ADD terminalLabel field */
            if (channelSet[i].terminalLabel.mcuNumber <= 192 && channelSet[i].terminalLabel.terminalNumber <= 192)
            {
                int nid = pvtAdd(hVal, elemId, __h245(terminalLabel), 0, NULL, NULL);
                int iMCU = channelSet[i].terminalLabel.mcuNumber;
                int iTerminal = channelSet[i].terminalLabel.terminalNumber;
                pvtAdd(hVal, nid, __h245(mcuNumber), iMCU, NULL, NULL);
                ret=pvtAdd(hVal, nid, __h245(terminalNumber), iTerminal, NULL, NULL);
            }
            /* ADD sessionDescription field */
            {
                int l=0;
                char tmpBuf[258];
                tmpBuf[0] = tmpBuf[1] = 0;
                if (channelSet[i].sessionDescription != NULL && channelSet[i].sessionDescriptionLength>0)
                    l=chrn2bmp(channelSet[i].sessionDescription, (RvSize_t)RvMin(channelSet[i].sessionDescriptionLength, 128), (RvUint8*)tmpBuf);
                if (l<2) l=2;
                ret=pvtAdd(hVal, elemId, __h245(sessionDescription), l, tmpBuf, NULL);
            }

            /* ADD sessionDependency field, if possible */
            if (channelSet[i].dependency>=0) dependencySet=RV_TRUE;
            else if (channel && channel->pBase)
                ret=pvtAdd(hVal, elemId, __h245(sessionDependency), channel->pBase->data.h225Params.sessionId, NULL, NULL);

            /* ADD mediaControlChannel field */
            if (cmTaHasIp(&channelSet[i].rtcpAddress) || channelSet[i].rtcpAddress.port != 0)
            {
                int tmpId = pvtAdd(hVal, elemId, __h245(mediaControlChannel), 0, NULL, NULL);
                getGoodAddressForCtrl((HCONTROL)ctrl,&channelSet[i].rtcpAddress);
                cmTAToVt_H245(hVal,tmpId,&channelSet[i].rtcpAddress);
            }
            else if (channel)
            {
                if (channel->data.h225Params.pPartner && channel->data.h225Params.pPartner->data.h225Params.sendRtcpAddressID>=0)
                {
                    int tmpId = pvtAdd(hVal, elemId, __h245(mediaControlChannel), 0, NULL, NULL);
                    pvtSetTree(hVal,tmpId,hVal,channel->data.h225Params.pPartner->data.h225Params.sendRtcpAddressID);
                }
                else if (channel->data.h225Params.sendRtcpAddressID>=0)
                {
                    int tmpId = pvtAdd(hVal, elemId, __h245(mediaControlChannel), 0, NULL, NULL);
                    pvtSetTree(hVal,tmpId,hVal,channel->data.h225Params.sendRtcpAddressID);
                }
            }


            /* ADD mediaChannel field */
            if (cmTaHasIp(&channelSet[i].rtpAddress) || channelSet[i].rtpAddress.port != 0)
            {
                int tmpId = pvtAdd(hVal, elemId, __h245(mediaChannel), 0, NULL, NULL);
                getGoodAddressForCtrl((HCONTROL)ctrl,&channelSet[i].rtpAddress);
                cmTAToVt_H245(hVal,tmpId,&channelSet[i].rtpAddress);
            }
            else if (channel)
            {
                if (channel->data.h225Params.pPartner && channel->data.h225Params.pPartner->data.h225Params.sendRtpAddressID>=0)
                {
                    int tmpId = pvtAdd(hVal, elemId, __h245(mediaChannel), 0, NULL, NULL);
                    pvtSetTree(hVal,tmpId,hVal,channel->data.h225Params.pPartner->data.h225Params.sendRtpAddressID);
                }
                else if (channel->data.h225Params.sendRtpAddressID>=0)
                {
                    int tmpId = pvtAdd(hVal, elemId, __h245(mediaChannel), 0, NULL, NULL);
                    pvtSetTree(hVal,tmpId,hVal,channel->data.h225Params.sendRtpAddressID);
                }
            }
            elemId = pvtBrother(hVal,elemId);
        }
        if (ret>=0)
        {
            /* ADD sessionDependency field */
            if (useNewFields && dependencySet)
            {
                int j, sid, childNodeId_I,childNodeId_J;
                childNodeId_I=pvtChild(hVal,tableId);
                for (i=0; i<channelSetSize; i++)
                {
                    childNodeId_J=pvtChild(hVal,tableId);
                    for (j=0; j<channelSetSize; j++)
                    {
                        if (channelSet[i].dependency==channelSet[j].uid)
                        {
                            pvtGetChildByFieldId(hVal,childNodeId_J,__h245(sessionID),&sid,NULL);
                            ret=pvtAdd(hVal, childNodeId_J, __h245(sessionDependency), channelSet[i].dependency, NULL, NULL);
                        }
                        childNodeId_J=pvtBrother(hVal,childNodeId_J);
                    }
                    childNodeId_I=pvtBrother(hVal,childNodeId_I);
                }
            }

            ret = sendMessageH245((HCONTROL)ctrl, message, RV_FALSE, NULL);
        }
        pvtDelete(hVal,message);
        emaUnlock((EMAElement)hsCall);
    }

    H245APIExit((app->pAppObject, "cmCallSetChannelsExt=%d", ret));
    return ret;
}
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == RV_YES) */


#endif /* (RV_H245_LEAN_H223 == RV_NO) */



/************************************************************************
 * conferenceMessage
 * purpose: Deal with various incoming H245 conference messages
 *          This function uses the callback to the application to handle
 *          the message
 * input  : ctrl        - H245 control object of the call
 *          lcn         - Logical channel of this message.
 *                        0 if no channel is related to this call
 *          message     - root node ID of the message to handle
 *          messaheType - Type of message
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int conferenceMessage(
    IN H245Control*         ctrl,
    IN int                  lcn,
    IN int                  message,
    IN cmH245MessageType    messageType)
{
    HCALL        call=(HCALL)cmiGetByControl((HCONTROL)ctrl);
    H245Object*  app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)call));
    H245Channel* channel=NULL;

    if(lcn>0)
        channel=getOutChanByLCN((HCONTROL)ctrl,lcn);

	//{{gaoshizhong 20131129 Add
	if(messageType == h245mcEncryptionUpdateCommand || messageType == h245mcEncryptionUpdateRequest || messageType == h245mcEncryptionUpdateAck)
	{
		HPVT hVal = ((H245Object *)app)->hVal;
		RvPvtNodeId child = pvtParent(hVal, message);
		RvPvtNodeId grandchild = pvtParent(hVal, child);
		int msdStatus = cmCallMasterSlaveStatus(call);
		if (msdStatus == 2)
		{
			char slaveMasterPath[128]={0};
			int nodeGet=pvtGetNodeIdByPath(hVal,grandchild,"direction.slaveToMaster");
			if(nodeGet >=0 ) channel=getInChanByLCN((HCONTROL)ctrl,lcn);
		}
		if (msdStatus != 2)
		{
			char slaveMasterPath[128]={0};
			int nodeGet=pvtGetNodeIdByPath(hVal,grandchild,"direction.masterToSlave");
			if(nodeGet >=0 ) channel=getInChanByLCN((HCONTROL)ctrl,lcn);
		}
	}
	//}}gaoshizhong 20131129 Add

    if(app->cmMySessionEvent.cmEvCallControlMessage)
    {
        H245CBEnter((app->pAppObject, "cmEvCallControlMessage(hsCall=%p, hsChan=%p, message=%d, messageType=%d)", call, channel, message, messageType));
        app->cmMySessionEvent.cmEvCallControlMessage(
            (HAPPCALL)emaGetApplicationHandle((EMAElement)call),    (HCALL)call,
            (HAPPCHAN)emaGetApplicationHandle((EMAElement)channel), (HCHAN)channel,
            message, messageType);
        H245CBExit((app->pAppObject, "cmEvCallControlMessage"));
    }

    return RV_TRUE;
}



int miscellaneousCommand(H245Control* ctrl, int lcn, int message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT hVal = ((H245Object *)hApp)->hVal;
    RvPvtNodeId childId;
    RvPstFieldId fieldId;

    childId = pvtChild(hVal, message);
    pvtGet(hVal, childId, &fieldId, NULL, NULL, NULL);

    switch (fieldId)
    {
    case __h245(equaliseDelay)                        : conferenceMessage (ctrl,lcn,childId,h245mcEqualiseDelay);  break;
    case __h245(zeroDelay)                            : conferenceMessage (ctrl,lcn,childId,h245mcZeroDelay);  break;
    case __h245(maxH223MUXPDUsize)                    : conferenceMessage (ctrl,lcn,childId,h245mcMaxH223MUXPDUsize);  break;
#if (RV_H245_LEAN_H223 == RV_NO)
//{{gaoshizhong 20131122 Add
	case __h245(encryptionUpdateCommand)              : conferenceMessage (ctrl,lcn,childId,h245mcEncryptionUpdateCommand);  break;
	case __h245(encryptionUpdateAck)				  : conferenceMessage (ctrl,lcn,childId,h245mcEncryptionUpdateAck);  break;
//}}gaoshizhong 20131122 Add
    case __h245(encryptionUpdate)                     : conferenceMessage (ctrl,lcn,childId,h245mcEncryptionUpdate);  break;
    case __h245(encryptionUpdateRequest)              : conferenceMessage (ctrl,lcn,childId,h245mcEncryptionUpdateRequest);  break;
#endif
    case __h245(progressiveRefinementStart)           : conferenceMessage (ctrl,lcn,childId,h245mcProgressiveRefinementStart);  break;
    case __h245(progressiveRefinementAbortOne)        : conferenceMessage (ctrl,lcn,childId,h245mcProgressiveRefinementAbortOne);  break;
    case __h245(progressiveRefinementAbortContinuous) : conferenceMessage (ctrl,lcn,childId,h245mcProgressiveRefinementAbortContinuous);  break;
    case __h245(videoBadMBs)                          : conferenceMessage (ctrl,lcn,childId,h245mcVideoBadMBs);  break;
    case __h245(lostPicture)                          : conferenceMessage (ctrl,lcn,childId,h245mcLostPicture);  break;
    case __h245(lostPartialPicture)                   : conferenceMessage (ctrl,lcn,childId,h245mcLostPartialPicture);  break;
    case __h245(recoveryReferencePicture)             : conferenceMessage (ctrl,lcn,childId,h245mcRecoveryReferencePicture);  break;
    default:
        break;
    }
    return RV_TRUE;
}



int indication(H245Control* ctrl, int lcn, int message)
{
    HH245 hApp=cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)cmiGetByControl((HCONTROL)ctrl)));
    HPVT hVal = ((H245Object *)hApp)->hVal;
    RvPvtNodeId childId;
    RvPstFieldId fieldId;

    childId = pvtChild(hVal, message);
    pvtGet(hVal, childId, &fieldId, NULL, NULL, NULL);

    switch (fieldId)
    {
#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(jitterIndication)      : conferenceMessage (ctrl,lcn,childId,h245iJitterIndication);  break;
#endif
    case __h245(h223SkewIndication)    : conferenceMessage (ctrl,lcn,childId,h245iH223SkewIndication);  break;
    case __h245(functionNotSupported)  : conferenceMessage (ctrl,lcn,childId,h245iFunctionNotSupported);  break;
    case __h245(flowControlIndication) : conferenceMessage (ctrl,lcn,childId,h245iFlowControlIndication);  break;
    default: break;
    }
    return RV_TRUE;
}

/******************************************************************************
 * vendorIdentification
 * ----------------------------------------------------------------------------
 * General: Indication to the application on an incoming VendorIdentification
 *          message.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Input:  ctrl              - The control object.
 *         message           - PVT node id of the vendorIdentification message.
 * Output: none.
 *****************************************************************************/
int vendorIdentification(H245Control* ctrl,RvPvtNodeId message)
{
    HPVT                    hVal;
    EMAElement              callE;
    H245Object              *app;
    int                     nesting;
    cmNonStandardIdentifier nonStandardId;
    RvUint8                 productNumber[260];
    RvUint8                 versionNumber[260];
    RvInt32                 productNumberSize = 0;
    RvInt32                 versionNumberSize = 0;
    RvPvtNodeId             nodeId = RV_PVT_INVALID_NODEID;

    callE = (EMAElement)cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance(callE));

    if (!app) return RV_ERROR_UNKNOWN;

    if (app->cmMySessionEvent.cmEvCallVendorIdentification != NULL)
    {
        hVal = app->hVal;

        if (emaLock(callE))
        {
            /* get non standard identifier */
            __pvtGetByFieldIds(nodeId, hVal, message,{ _h245(vendor) _h245(h221NonStandard) _h245(t35CountryCode) LAST_TOKEN}, NULL, (RvInt32*)&nonStandardId.t35CountryCode, NULL);
            __pvtGetByFieldIds(nodeId, hVal, message,{ _h245(vendor) _h245(h221NonStandard) _h245(t35Extension) LAST_TOKEN}, NULL, (RvInt32*)&nonStandardId.t35Extension, NULL);
            __pvtGetByFieldIds(nodeId, hVal, message,{ _h245(vendor) _h245(h221NonStandard) _h245(manufacturerCode) LAST_TOKEN}, NULL, (RvInt32*)&nonStandardId.manufacturerCode, NULL);
            __pvtGetByFieldIds(nodeId, hVal, message,{ _h245(vendor) _h245(object) LAST_TOKEN}, NULL, NULL, NULL);
            nonStandardId.objectLength = pvtGetString(hVal, nodeId, sizeof(nonStandardId.object), nonStandardId.object);

            /* get product number */
            nodeId = pvtGetChildByFieldId(hVal, message, __h245(productNumber), &productNumberSize, NULL);
            if (RV_PVT_NODEID_IS_VALID(nodeId))
            {
                productNumberSize = pvtGetString(hVal, nodeId, sizeof(productNumber), (RvChar*)productNumber);
                if (productNumberSize < 0)
                {
                    productNumberSize = 0;
                }
            }

            /* get version number */
            nodeId = pvtGetChildByFieldId(hVal, message, __h245(versionNumber), &versionNumberSize, NULL);
            if (RV_PVT_NODEID_IS_VALID(nodeId))
            {
                versionNumberSize = pvtGetString(hVal, nodeId, sizeof(versionNumber), (RvChar*)versionNumber);
                if (versionNumberSize < 0)
                {
                    versionNumberSize = 0;
                }
            }

            /* notify the application about the VendorIdentification message */
            if (app->cmMySessionEvent.cmEvCallVendorIdentification != NULL)
            {
                HCALL    hsCall = cmiGetByControl((HCONTROL)ctrl);
                HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);

                H245CBEnter((app->pAppObject, "cmEvCallVendorIdentification: haCall=%p, hsCall=%p", haCall, hsCall));
                nesting = emaPrepareForCallback(callE);
                app->cmMySessionEvent.cmEvCallVendorIdentification(haCall, hsCall,
                    &nonStandardId, (RvUint8*)productNumber, (RvUint16)productNumberSize,
                    (RvUint8*)versionNumber, (RvUint16)versionNumberSize);
                emaReturnFromCallback(callE, nesting);
                H245CBExit((app->pAppObject, "cmEvCallVendorIdentification."));
            }
            emaUnlock(callE);
        }
    }
    return RV_OK;
}



RVAPI int RVCALLCONV
cmBuildMultimediaSystemControlMessage(IN  HAPP              hApp,
                                      IN  cmH245MessageType messageType,
                                      IN  int               lcn,
                                      OUT RvPvtNodeId *     messageNode,
                                      OUT RvPvtNodeId *     insertionNode)
/* This function will build a Multimedia System Control Message, and will return the node ID of the message, and
   the node ID to which the message itself (according to the messageType enumerations) should be inserted (if at
   all). the function will return 0 on success and -1 on failure. */
{
    HPVT hVal;
    HH245 hH245;
#if (RV_CHECK_MASK & RV_CHECK_NULL)
    if(!hApp || !messageNode || !insertionNode) return RV_ERROR_NULLPTR;
#endif

    hH245 = cmiGetH245Handle(hApp);
    hVal = ((H245Object *)hH245)->hVal;
    *messageNode = pvtAddRoot(hVal,((H245Object*)hH245)->hSynProtH245,0,NULL);
    *insertionNode = -1;

    switch(messageType)
    {
        /*  ConferenceRequest */
    case h245crqTerminalListRequest:
    case h245crqMakeMeChair:
    case h245crqCancelMakeMeChair:
    case h245crqDropTerminal:
    case h245crqRequestTerminalID:
    case h245crqEnterH243Password:
    case h245crqEnterH243TerminalID:
    case h245crqEnterH243ConferenceID:
    case h245crqRequestChairTokenOwner:
    case h245crqRequestTerminalCertificate:
    case h245crqBroadcastMyLogicalChannel:
    case h245crqMakeTerminalBroadcaster:
    case h245crqSendThisSource:
    case h245crqRequestAllTerminalIDs:
    case h245crqRemoteMCRequest:
#if (RV_H245_LEAN_H223 == RV_NO)
        *insertionNode = pvtAddBranch(hVal, *messageNode, __h245(request));
        *insertionNode = pvtAddBranch(hVal, *insertionNode,  __h245(conferenceRequest));
        switch(messageType)
        {
        case h245crqTerminalListRequest:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalListRequest));  break;
        case h245crqMakeMeChair:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(makeMeChair));          break;
        case h245crqCancelMakeMeChair:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(cancelMakeMeChair));    break;
        case h245crqDropTerminal:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(dropTerminal));         break;
        case h245crqRequestTerminalID:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(requestTerminalID));    break;
        case h245crqEnterH243Password:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(enterH243Password));    break;
        case h245crqEnterH243TerminalID:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(enterH243TerminalID));  break;
        case h245crqEnterH243ConferenceID:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(enterH243ConferenceID)); break;
        case h245crqRequestChairTokenOwner:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(requestChairTokenOwner)); break;
        case h245crqRequestTerminalCertificate:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(requestTerminalCertificate));break;
        case h245crqBroadcastMyLogicalChannel:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(broadcastMyLogicalChannel)); break;
        case h245crqMakeTerminalBroadcaster:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(makeTerminalBroadcaster)); break;
        case h245crqSendThisSource:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(sendThisSource));          break;
        case h245crqRequestAllTerminalIDs:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(requestAllTerminalIDs));   break;
        case h245crqRemoteMCRequest:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(remoteMCRequest));         break;
        default:
            break;
        }
#else
        *messageNode = -1;
        return RV_ERROR_UNKNOWN;
#endif
        break;

        /* Conference Response */
    case h245crsMCTerminalIDResponse:
    case h245crsTerminalIDResponse:
    case h245crsConferenceIDResponse:
    case h245crsPasswordResponse:
    case h245crsTerminalListResponse:
    case h245crsVideoCommandReject:
    case h245crsTerminalDropReject:
    case h245crsMakeMeChairResponse:
    case h245crsChairTokenOwnerResponse:
    case h245crsTerminalCertificateResponse:
    case h245crsBroadcastMyLogicalChannelResponse:
    case h245crsMakeTerminalBroadcasterResponse:
    case h245crsSendThisSourceResponse:
    case h245crsRequestAllTerminalIDsResponse:
    case h245crsRemoteMCResponse:
#if (RV_H245_LEAN_H223 == RV_NO)
        *insertionNode = pvtAddBranch(hVal,*messageNode,  __h245(response));
        *insertionNode = pvtAddBranch(hVal,*insertionNode,  __h245(conferenceResponse));
        switch(messageType)
        {
        case h245crsMCTerminalIDResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(mCTerminalIDResponse)); break;
        case h245crsTerminalIDResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalIDResponse)); break;
        case h245crsConferenceIDResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(conferenceIDResponse)); break;
        case h245crsPasswordResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(passwordResponse)); break;
        case h245crsTerminalListResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalListResponse)); break;
        case h245crsVideoCommandReject:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(videoCommandReject)); break;
        case h245crsTerminalDropReject:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalDropReject)); break;
        case h245crsMakeMeChairResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(makeMeChairResponse)); break;
        case h245crsChairTokenOwnerResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(chairTokenOwnerResponse)); break;
        case h245crsTerminalCertificateResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalCertificateResponse)); break;
        case h245crsBroadcastMyLogicalChannelResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(broadcastMyLogicalChannelResponse)); break;
        case h245crsMakeTerminalBroadcasterResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(makeTerminalBroadcasterResponse)); break;
        case h245crsSendThisSourceResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(sendThisSourceResponse)); break;
        case h245crsRequestAllTerminalIDsResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(requestAllTerminalIDsResponse)); break;
        case h245crsRemoteMCResponse:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(remoteMCResponse)); break;
        default:
            break;
        }
#endif
        break;
        /* For all Indications */
    case h245iJitterIndication:
    case h245iH223SkewIndication:
    case h245iFunctionNotSupported:
    case h245iFlowControlIndication:
    case h245iVendorIdentification:
        /* Including Conference Indications */
    case h245ciSbeNumber:
    case h245ciTerminalJoinedConference:
    case h245ciTerminalLeftConference:
    case h245ciSeenByAtLeastOneOther:
    case h245ciCancelSeenByAtLeastOneOther:
    case h245ciSeenByAll:
    case h245ciCancelSeenByAll:
    case h245ciTerminalYouAreSeeing:
    case h245ciRequestForFloor:
    case h245ciWithdrawChairToken:
    case h245ciFloorRequested:
    case h245ciTerminalYouAreSeeingInSubPictureNumber:
    case h245ciVideoIndicateCompose:
        /* And Including Miscellaneous Indications */
    case h245miMultipointZeroComm:
    case h245miCancelMultipointZeroComm:
    case h245miMultipointSecondaryStatus:
    case h245miCancelMultipointSecondaryStatus:
    case h245miVideoIndicateReadyToActivate:
    case h245miVideoNotDecodedMBs:
            /* we do this: */
        *insertionNode = pvtAddBranch(hVal,*messageNode, __h245(indication));
        switch(messageType)
        {
            /* For each Indication: */
#if (RV_H245_LEAN_H223 == RV_NO)
        case h245iJitterIndication:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(jitterIndication));
            break;
#endif
        case h245iH223SkewIndication:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(h223SkewIndication));
            break;
        case h245iFunctionNotSupported:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(functionNotSupported));
            break;
        case h245iFlowControlIndication:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(flowControlIndication));
            break;
        case h245iVendorIdentification:
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(vendorIdentification));
            break;
            /* But for Conference Indications... */
        case h245ciSbeNumber:
        case h245ciTerminalJoinedConference:
        case h245ciTerminalLeftConference:
        case h245ciSeenByAtLeastOneOther:
        case h245ciCancelSeenByAtLeastOneOther:
        case h245ciSeenByAll:
        case h245ciCancelSeenByAll:
        case h245ciTerminalYouAreSeeing:
        case h245ciRequestForFloor:
        case h245ciWithdrawChairToken:
        case h245ciFloorRequested:
        case h245ciTerminalYouAreSeeingInSubPictureNumber:
#if (RV_H245_LEAN_H223 == RV_NO)
        case h245ciVideoIndicateCompose:
            /* We do this: */
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(conferenceIndication));
            switch(messageType)
            {
                /* For each Conference Indication: */
            case h245ciSbeNumber:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(sbeNumber));
                break;
            case h245ciTerminalJoinedConference:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalJoinedConference));
                break;
            case h245ciTerminalLeftConference:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalLeftConference));
                break;
            case h245ciSeenByAtLeastOneOther:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(seenByAtLeastOneOther));
                break;
            case h245ciCancelSeenByAtLeastOneOther:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(cancelSeenByAtLeastOneOther));
                break;
            case h245ciSeenByAll:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(seenByAll));
                break;
            case h245ciCancelSeenByAll:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(cancelSeenByAll));
                break;
            case h245ciTerminalYouAreSeeing:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalYouAreSeeing));
                break;
            case h245ciRequestForFloor:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(requestForFloor));
                break;
            case h245ciWithdrawChairToken:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(withdrawChairToken));
                break;
            case h245ciFloorRequested:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(floorRequested));
                break;
            case h245ciTerminalYouAreSeeingInSubPictureNumber:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(terminalYouAreSeeingInSubPictureNumber));
                break;
            case h245ciVideoIndicateCompose:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(videoIndicateCompose));
                break;
            default: /* We won't get here */ break;
            }
            break;
#endif
            /* And for Miscellaneous Indications... */
        case h245miMultipointZeroComm:
        case h245miCancelMultipointZeroComm:
        case h245miMultipointSecondaryStatus:
        case h245miCancelMultipointSecondaryStatus:
        case h245miVideoIndicateReadyToActivate:
        case h245miVideoNotDecodedMBs:
            /* We do this: */
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(miscellaneousIndication));
            pvtAdd(hVal, *insertionNode, __h245(logicalChannelNumber), lcn, NULL, NULL);
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(type));
            switch(messageType)
            {
                /* For each Miscellaneous Indication: */
            case h245miMultipointZeroComm:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(multipointZeroComm));
                break;
            case h245miCancelMultipointZeroComm:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(cancelMultipointZeroComm));
                break;
            case h245miMultipointSecondaryStatus:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(multipointSecondaryStatus));
                break;
            case h245miCancelMultipointSecondaryStatus:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(cancelMultipointSecondaryStatus));
                break;
            case h245miVideoIndicateReadyToActivate:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(videoIndicateReadyToActivate));
                break;
            case h245miVideoNotDecodedMBs:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(videoNotDecodedMBs));
                break;
            default: /* We won't get here */ break;
            }
            break;
        default: /* We won't get here */ break;
        }
        /* We're done with indications (thank god in heaven) */
        break;

        /* For all commands, which are:  Conference Commands */
    case h245ccBroadcastMyLogicalChannel:
    case h245ccCancelBroadcastMyLogicalChannel:
    case h245ccMakeTerminalBroadcaster:
    case h245ccCancelMakeTerminalBroadcaster:
    case h245ccSendThisSource:
    case h245ccCancelSendThisSource:
    case h245ccDropConference:
    case h245ccSubstituteConferenceIDCommand:
        /* And Miscellaneous Commands */
    case h245mcEqualiseDelay:
    case h245mcZeroDelay:
    case h245mcMaxH223MUXPDUsize:
    case h245mcProgressiveRefinementStart:
    case h245mcProgressiveRefinementAbortOne:
    case h245mcProgressiveRefinementAbortContinuous:
    case h245mcVideoBadMBs:
    case h245mcLostPicture:
    case h245mcLostPartialPicture:
    case h245mcRecoveryReferencePicture:
            /* We do this: */
        *insertionNode = pvtAddBranch(hVal,*messageNode, __h245(command));
        switch(messageType)
        {
            /* For Conference Commands only... */
        case h245ccBroadcastMyLogicalChannel:
        case h245ccCancelBroadcastMyLogicalChannel:
        case h245ccMakeTerminalBroadcaster:
        case h245ccCancelMakeTerminalBroadcaster:
        case h245ccSendThisSource:
        case h245ccCancelSendThisSource:
        case h245ccDropConference:
        case h245ccSubstituteConferenceIDCommand:
#if (RV_H245_LEAN_H223 == RV_NO)
                /*We do this: */
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(conferenceCommand));
            switch(messageType)
            {
                /* For each Conference Command: */
            case h245ccBroadcastMyLogicalChannel:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(broadcastMyLogicalChannel));
                break;
            case h245ccCancelBroadcastMyLogicalChannel:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(cancelBroadcastMyLogicalChannel));
                break;
            case h245ccMakeTerminalBroadcaster:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(makeTerminalBroadcaster));
                break;
            case h245ccCancelMakeTerminalBroadcaster:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(cancelMakeTerminalBroadcaster));
                break;
            case h245ccSendThisSource:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(sendThisSource));
                break;
            case h245ccCancelSendThisSource:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(cancelSendThisSource));
                break;
            case h245ccDropConference:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(dropConference));
                break;
            case h245ccSubstituteConferenceIDCommand:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(substituteConferenceIDCommand));
                break;
            default: /* We won't get here */ break;
            }
#endif
            break;
            /* But for Miscellaneous Commands only... */
        case h245mcEqualiseDelay:
        case h245mcZeroDelay:
        case h245mcMaxH223MUXPDUsize:
        case h245mcProgressiveRefinementStart:
        case h245mcProgressiveRefinementAbortOne:
        case h245mcProgressiveRefinementAbortContinuous:
        case h245mcVideoBadMBs:
        case h245mcLostPicture:
        case h245mcLostPartialPicture:
        case h245mcRecoveryReferencePicture:
                /* We do this: */
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(miscellaneousCommand));
            pvtAdd(hVal, *insertionNode, __h245(logicalChannelNumber), lcn, NULL, NULL);
            *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(type));
            switch(messageType)
            {
                /* For each Miscellaneous Command: */
            case h245mcEqualiseDelay:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(equaliseDelay));
                break;
            case h245mcZeroDelay:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(zeroDelay));
                break;
            case h245mcMaxH223MUXPDUsize:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(maxH223MUXPDUsize));
                break;
            case h245mcProgressiveRefinementStart:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(progressiveRefinementStart));
                break;
            case h245mcProgressiveRefinementAbortOne:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(progressiveRefinementAbortOne));
                break;
            case h245mcProgressiveRefinementAbortContinuous:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(progressiveRefinementAbortContinuous));
                break;
            case h245mcVideoBadMBs:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(videoBadMBs));
                break;
            case h245mcLostPicture:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(lostPicture));
                break;
            case h245mcLostPartialPicture:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(lostPartialPicture));
                break;
            case h245mcRecoveryReferencePicture:
                *insertionNode = pvtAddBranch(hVal,*insertionNode, __h245(recoveryReferencePicture));
                break;
            default: /* We won't get here */ break;
            }
            break;
        default: /* We won't get here */ break;
        }
        /* We're done with commands */
        break;

    /* Generic Messages */
    case h245grqGenericRequest:
        *insertionNode = pvtAddBranch(hVal, *messageNode, __h245(request));
        *insertionNode = pvtAddBranch(hVal, *insertionNode,  __h245(genericRequest));
        break;
    case h245grsGenericResponse:
        *insertionNode = pvtAddBranch(hVal, *messageNode, __h245(response));
        *insertionNode = pvtAddBranch(hVal, *insertionNode,  __h245(genericResponse));
        break;
    case h245gcGenericCommand:
        *insertionNode = pvtAddBranch(hVal, *messageNode, __h245(command));
        *insertionNode = pvtAddBranch(hVal, *insertionNode,  __h245(genericCommand));
        break;
    case h245giGenericIndication:
        *insertionNode = pvtAddBranch(hVal, *messageNode, __h245(indication));
        *insertionNode = pvtAddBranch(hVal, *insertionNode,  __h245(genericIndication));
        break;

    default:
        pvtDelete(hVal, *messageNode);
        *messageNode = -1;
        return RV_ERROR_UNKNOWN;
    }
    return 0;
}



#ifdef __cplusplus
}
#endif


