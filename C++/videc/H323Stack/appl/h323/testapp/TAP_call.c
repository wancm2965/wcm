/***********************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
 *                                TAP_call.c
 *
 * This file contains all the commands that are used for the
 * call\conference window of the test application.
 *
 *
 *
 *
 *      Written by                        Version & Date                        Change
 *     ------------                       ---------------                      --------
 *      Oren Libis                          04-Mar-2000
 *
 ********************************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <ctype.h>

#include "lepH450.h"
#include "lepUtils.h"
#include "lepConversions.h"
#include "lepControl.h"
#include "lepCall.h"

#include "TAP_call.h"
#include "TAP_channel.h"


#define CALL_ID(call) ((call != NULL) ? call->id : 0)


static void prepareIncomingCallWindow(
    IN EpCallObj    *call);
static void initFastStartPorts(
    IN EpCallObj    *call);



/******************************************************************************
 * epCallNew
 * ----------------------------------------------------------------------------
 * General: Indication of a new call object that was created as being used.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - The call object.
 *         isOutgoing   - Is this an incoming or an outgoing call?
 * Output: None.
 *****************************************************************************/
void epCallNew(
    IN EpCallObj            *call,
    IN RvBool               isOutgoing)
{
    Call_UpdateCounters(0); /* Increase current */

    if (call->isIndependent)
        return;

    if (!isOutgoing)
    {
        /* Open the incoming call window if we're not in auto-answer mode and pop-up are enabled */
        if ( !call->ep->cfg.bAutoAnswer &&
             call->ep->cfg.bPopUp )
        {
            TclExecute("Window open .incall\n"
                       ".incall.buttons.callInfo configure -text {%d %p}", call->id, call);
            TclExecute("Call:addWindowToList %p .incall", call);
            prepareIncomingCallWindow(call);
        }
    }
}


/******************************************************************************
 * epCallClose
 * ----------------------------------------------------------------------------
 * General: Indication of a call closure.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - The call object.
 * Output: None.
 *****************************************************************************/
void epCallClose(
    IN EpCallObj            *call)
{
    /* Make sure we clean the call from the main window */
    TclExecute("set index [Call.FindCallIndex %d]\n"
               ".test.calls.list delete $index\n"
               ".test.calls.conf delete $index\n"
               ".test.calls.conn delete $index\n",
               call->id);

    /* Close all windows that belong to that call */
    TclExecute("Call:cleanup %p", call);

    /* Delete the call's information window */
    TclExecute("Window delete .call%d", call->id);

    Call_UpdateCounters(-1); /* Decrease current */
}


/******************************************************************************
 * epEventIndication
 * ----------------------------------------------------------------------------
 * General: Indication of a message being sent or received by the endpoint.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep               - Endpoint object to use.
 *         eventType        - Type of the event indicated.
 *         call             - Call this event belongs to (NULL if none).
 *         eventStr         - Event string.
 * Output: None.
 *****************************************************************************/
void epEventIndication(
    IN EpObj                *ep,
    IN const RvChar         *eventType,
    IN EpCallObj            *call,
    IN const RvChar         *eventStr)
{
    if (strcmp(eventType, "callStateDisplay") == 0)
    {
        /* Indication of a need to change the state displayed for a call in the GUI */
        /* ---------------------- */
        TclExecute("call:Log %d {State: %s}", call->id, eventStr);
        TclExecute("Call:ChangeState %d {%s}", call->id, CallInfoStr(call, eventStr));

        if (strcmp(eventStr, "ConnectedCallSetup") == 0)
        {
            RvInt32 efcState;
            cmCallGetParam(call->hsCall, cmParamExtendedFastConnectState, 0, &efcState, NULL);
            if ((cmExtendedFastConnectState_e)efcState != cmExtendedFastConnectApproved)
            {
                TclExecute("after idle {Window delete .redial%p;faststart:terminate {%d %p} %d}",
                    call->hsCall, call->id, call, !cmCallGetOrigin(call->hsCall, NULL));
            }
            TclExecute("after 100 {incall:terminate {%d %p}}", call->id, call);
        }
        else if ((strcmp(eventStr, "RingBack") == 0) || (strcmp(eventStr, "Proceeding") == 0))
        {
            TclExecute("Window delete .redial%p", call->hsCall);
        }
        else if (strcmp(eventStr, "WaitAddressAck") == 0)
        {
            if (!ep->cfg.bAutoAnswer)
                TclExecute(".incall.remote.canOvsp select");
        }
        else if (strncmp(eventStr, "Transfering", 11) == 0)
        {
            if (ep->cfg.bSupportFaststart)
            {
                if (ep->cfg.bAutomaticFaststart)
                {
                    epCallGenerateFaststartRequest(call, RV_TRUE);
                }
                else
                {
                    initFastStartPorts(call);
                    TclExecute("Call.OpenOutgoingFaststart {%d %p} {}", call->id, call);
                    TclExecute(".faststart0@%p.request invoke", call);
                }
            }
        }
    }
    else if (strcmp(eventType, "TCL") == 0)
    {
        TclExecute("%s", eventStr);
    }

    else if (strcmp(eventType, "callColor") == 0)
    {
        /* Update the coloring of calls. Calls are colored by their conference ID and connection handle */
        /* ---------------------- */
        EpConnectionInfo    *appCon;
        RvUint8             CID[16], xorRes;
        RvInt32             size = 16;
        cmGetTpktChanApplHandle(call->hCon, (HAPPCONN*)&appCon);
        cmCallGetParam(call->hsCall, cmParamCID, 0, &size, (char*)CID);
        xorRes = (RvUint8)(CID[9] & 0xf);
        xorRes = (RvUint8)(xorRes << 4);
        TclExecute("Call:AddColor %d #%02X%02X%02X #%02X%02X%02X",
            call->id, CID[2]^CID[1]^xorRes, CID[3]^CID[2]^xorRes, CID[4]^CID[3]^xorRes,
            (appCon->color>>16)&0xff, (appCon->color>>8)&0xff, appCon->color&0xff);
    }

    else if (strcmp(eventType, "chanStateDisplay") == 0)
    {
        TclExecute("Channel.DisplayChannelList {%d %p:}", call->id, call);
    }

    else if (strcmp(eventType, "chanDropRequest") == 0)
    {
        EpChannelObj * chan;

        /* Manual drop - we should interact with the user */
        TclExecute("Window open .inchan .dropchan {Request Channel Close} Channel.ResponseForCLC");
        TclExecute(".dropchan.callInfo configure -text {%s}", eventStr);
        TclExecute("Call:addWindowToList %p .dropchan", call);

        /* Display channel information inside the list-box */
        sscanf(eventStr, "%p", &chan);
        epDisplayChannelInfo(chan, (char *)".dropchan.chanInfo.txt");
    }

    else if (strcmp(eventType, "logRtp") == 0)
    {
        TclExecute("rtpInfo:InsertLine {%s}", eventStr);
    }

    else if (strcmp(eventType, "Tag") == 0)
    {
        TclExecute("catch {rmgr:setTag %s}", eventStr);
    }
    else if (strcmp(eventType, "Rec") == 0)
    {
        if (!ep->bIgnoreRec)
            TclExecute("catch {rmgr:send \"btn\" {%s}}", eventStr);
    }
    else if (strcmp(eventType, "Catch") == 0)
    {
        TclExecute("catch {%s}", eventStr);
    }
    else if (strcmp(eventType, "Script") == 0)
    {
        TclExecute("script:cb {%s}", eventStr);
    }
    else if (strcmp(eventType, "callState") == 0)
    {
        TclExecute("catch {record:event %s %d %s}", eventType, call->id, eventStr);
    }
    else if (strcmp(eventType, "chanState") == 0)
    {
        TclExecute("catch {record:event %s %d %s}", eventType, call->id, eventStr);
    }
    else if (strcmp(eventType, "chanReqClose") == 0)
    {
        TclExecute("catch {record:event %s %d %s}", eventType, call->id, eventStr);
    }
    else if ((strcmp(eventType, "Q931") == 0) ||
        (strcmp(eventType, "RAS") == 0) ||
        (strcmp(eventType, "H245") == 0) ||
        (strcmp(eventType, "H450") == 0) ||
        (strcmp(eventType, "H460") == 0) ||
        (strcmp(eventType, "SEC") == 0) ||
        (strcmp(eventType, "Hooks") == 0))
    {
        /* Some of the callbacks here cause GUI window updates */
        /* ---------------------- */
        TclExecute("catch {record:event %s %d %s}", eventType, CALL_ID(call), eventStr);

        if (strcmp(eventStr, "cmEvCallIncompleteAddress") == 0)
        {
            if (!call->isIndependent)
            {
                /* Open the Redial window */
                TclExecute("Window open .redial %p\n"
                    ".redial%p.callInfo configure -text {%d %p}",
                    call->hsCall, call->hsCall, call->id, call);
                TclExecute("Call:addWindowToList %p .redial%p", call, call->hsCall);
            }
        }
        else if (strncmp(eventStr, "cmEvCallAdditionalAddress.", 26) == 0)
        {
            if (!call->isIndependent && !call->sendComplete && ep->cfg.bPopUp)
            {
                TclExecute("Window open .incall\n"
                           ".incall.buttons.callInfo configure -text {%d %p}\n", call->id, call);
                prepareIncomingCallWindow(call);
                if (eventStr[strlen(eventStr)-1] == '1')
                {
                    TclExecute(".incall.remote.sendCmplt select");
                    TclExecute(".incall.buttons.ica configure -state disabled");
                }
                else
                    TclExecute(".incall.remote.sendCmplt deselect");
                TclExecute("Call:addWindowToList %p .incall", call);
            }
        }
        else if (strcmp(eventStr, "cmEvCallFastStart") == 0)
        {
            if (ep->cfg.bSupportFaststart && !ep->cfg.bAutomaticFaststart && !ep->cfg.bAutoAnswer)
            {
                cmTransportAddress          ownCSAddr;
                cmChannelDirection          chanDir;
                cmFastStartChannel          fsChan;
                RvChar                      rtpAddr[64], rtcpAddr[64];
                int                         i;
                int                         channelId = 0;
                int                         sessionId;
                const RvChar                *dir;

                /* user answers all channels */
                cmGetLocalCallSignalAddress(ep->hApp, &ownCSAddr);

                TclExecute("Window open .faststart {%d %p} 1 {} {}", call->id, call);
                TclExecute("Call:addWindowToList %p .faststart1@%p", call, call);

                for (i = 0; channelId >= 0; i++)
                {
                    channelId = cmFastStartGetByIndex(call->hsCall, i);
                    if (channelId < 0)
                        break;
                    cmFastStartGet(call->hsCall, channelId, &sessionId, &chanDir, &fsChan);

                    /* set colors of entries according to the fact that they are transmitted or received */
                    if (chanDir == dirReceive)
                    {
                        dir = "In";
                        memcpy(&fsChan.rtp, &ownCSAddr, sizeof(fsChan.rtp));
                    }
                    else if (chanDir == dirTransmit)
                    {
                        dir = "Out";
                        memset(&fsChan.rtp, 0, sizeof(fsChan.rtp));
                    }
                    else
                    {
                        dir = "Both";
                        memcpy(&fsChan.rtp, &ownCSAddr, sizeof(fsChan.rtp));
                    }

                    fsChan.rtp.port = 0;
                    fsChan.rtcp = ownCSAddr;
                    fsChan.rtcp.port = 0;
                    cmTransportAddress2String(&fsChan.rtcp, sizeof(rtcpAddr), rtcpAddr);
                    cmTransportAddress2String(&fsChan.rtp, sizeof(rtpAddr), rtpAddr);
                    TclExecute(".faststart1@%p.chan.lb insert end {%d. %s : %s , %d ; %s , %s}",
                        call, i, fsChan.channelName, dir, sessionId, rtcpAddr, rtpAddr);
                }
            }
        }
        else if (strcmp(eventStr, "rasEncapStatusEv.2") == 0)
        {
            TclExecute("set app(newcall,multiplexed) 1; set app(newcall,maintain) 1");
            TclExecute("$tmp(basicTab).multiplex.multiplexed configure -state disabled");
            TclExecute("$tmp(basicTab).multiplex.maintain configure -state disabled");
        }
        else if (strncmp(eventStr, "rasEncapStatusEv.", 17) == 0)
        {
            TclExecute("$tmp(basicTab).multiplex.multiplexed configure -state normal");
            TclExecute("$tmp(basicTab).multiplex.maintain configure -state normal");
        }
    }
    else if (strcmp(eventType, "ldapStateChanged") == 0)
    {
        TclExecute("$tmp(secTab).ids.ld.useLdap configure -state %s", eventStr);
    }
    else if (strcmp(eventType, "getPrivateCertificate") == 0)
    {
#ifdef USE_SECURITY
        FILE * crtFile;
        crtFile = fopen(eventStr, "rb");
        /* we have a file, so read from it */
        if (crtFile != NULL)
        {
            ep->cfg.privateCrt = epMalloc(ep, 1024, __FILE__, __LINE__);
            ep->cfg.privateCrtSz = fread(ep->cfg.privateCrt, sizeof(char), 1024, crtFile);
            fclose(crtFile);
        }
#endif /* USE_SECURITY */
    }
    else if (strcmp(eventType, "savePrivateCertificate") == 0)
    {
        TclExecute("catch {rmgr:sendDataFile %s 1 %s}", eventStr, eventStr);
    }
}


/*******************************************************************************************
 * CallInfoStr
 * purpose : Create a string representing call information to display on calls listbox in the
 *           main window
 * input   : CurrentCall    - Current call information to use
 *           StateStr       - State string to use
 * output  : none
 * return  : none
 *******************************************************************************************/
const RvChar *CallInfoStr(
    IN EpCallObj    *call,
    IN const RvChar *stateStr)
{
    static char data[100];

    sprintf(data, "%d %p: %s (%s)",
            call->id, call, stateStr, (call->isIncoming ? "in" : "out") );

    return data;
}


/*******************************************************************************************
 * Call_UpdateCounters
 * Description: Maintain the number of current and total calls.
 * Input : instruction  - -1 decrease current
 *                         0 increase current
 *                         1 increase total
 * Output: none
 * return: none
 ******************************************************************************************/
void Call_UpdateCounters(int instruction)
{
    static RvUint32 curCalls = 0;
    static RvUint32 totalCalls = 0;

    switch (instruction)
    {
    case -1:    curCalls--; break;
    case 0:     curCalls++; break;
    case 1:     totalCalls++; break;
    default: break;
    }

    TclExecute("test:SetCalls %d %d", curCalls, totalCalls);
}







/********************************************************************************************
 *                                                                                          *
 *                                  Private functions                                       *
 *                                                                                          *
 ********************************************************************************************/


/*******************************************************************************************
 * initFastStartPorts
 * purpose : Initiate FastStart RTP and RTCP ports
 * input   : call           - Current call information to use
 * output  : none
 * return  : none
 *******************************************************************************************/
static void initFastStartPorts(
    IN EpCallObj    *call)
{
    if (call->action == epRtpActionReplay)
    {
        int audioPort=0, videoPort=0, dataPort=0;
        /* init all the rtp sessions, we may need them later. */
        epCallInitRtpSessions(call, &audioPort, &videoPort, &dataPort);

        /* set audio, video and data ports */
        TclExecute("set tmp(faststart,audioPort) %d", audioPort);
        TclExecute("set tmp(faststart,videoPort) %d", videoPort);
        TclExecute("set tmp(faststart,dataPort) %d", dataPort);
    }
    else
    {
        /* use fictive RTP */
        TclExecute("set tmp(faststart,audioPort) 2536");
        TclExecute("set tmp(faststart,videoPort) 2536");
        TclExecute("set tmp(faststart,dataPort) 2536");
    }
}



/********************************************************************************************
 *                                                                                          *
 *                                  Public functions                                        *
 *                                                                                          *
 ********************************************************************************************/


/********************************************************************************************
 * Call_Info
 * purpose : Display a call's information.
 * syntax  : Call.Info <callInfo>
 *           <callInfo>   - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_Info(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj               *ep = (EpObj *)clientData;
    EpCallObj           *call;
    RvInt32             value;
    cmTransportAddress  addr1;
    cmTransportAddress  addr2;
    RvChar              tmpStr1[533];
    RvChar              tmpStr2[128];
    cmAlias             alias;
    RvChar              aliasBufStr[513];
    RvInt               i, maxParams;
    alias.string = aliasBufStr;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* Local and remote addresses */
    value = (RvInt32)sizeof(cmTransportAddress);
    strcpy(tmpStr1, "unknown");
    strcpy(tmpStr2, "unknown");
    if (cmCallGetParam(call->hsCall, cmParamSourceIpAddress, 0, &value, (char*)&addr1) >= 0)
    {
        cmTransportAddress2String(&addr1, sizeof(tmpStr1), tmpStr1);
    }
    value = (RvInt32)sizeof(cmTransportAddress);
    if (cmCallGetParam(call->hsCall, cmParamRemoteIpAddress, 0, &value, (char*)&addr1) >= 0)
    {
        cmTransportAddress2String(&addr1, sizeof(tmpStr2), tmpStr2);
    }
    else if (cmCallGetParam(call->hsCall, cmParamDestinationIpAddress, 0, &value, (char*)&addr1) >= 0)
    {
        cmTransportAddress2String(&addr1, sizeof(tmpStr2), tmpStr2);
    }
    TclExecute("call:Log %d {Q.931: %s -> %s}", call->id, tmpStr1, tmpStr2);

    /* H.245 address */
    memset(&addr1, 0, sizeof(addr1));
    memset(&addr2, 0, sizeof(addr2));
    if (cmCallHasControlSession(call->hsCall))
    {
        if ((cmCallGetParam(call->hsCall, cmParamH245Tunneling, 0, &value, NULL) >= 0) && (value == 0))
        {
            HPROTCONN hProt = cmGetTpktChanHandle(call->hsCall, cmH245TpktChannel);
            cmProtocolGetLocalAddress(ep->hApp, hProt, &addr1);
            cmProtocolGetRemoteAddress(ep->hApp, hProt, &addr2);
            strcpy(tmpStr1, "none");
            strcpy(tmpStr2, "none");
            cmTransportAddress2String(&addr1, sizeof(tmpStr1), tmpStr1);
            cmTransportAddress2String(&addr1, sizeof(tmpStr2), tmpStr2);
            TclExecute("call:Log %d {H.245: %s -> %s}", call->id, tmpStr1, tmpStr2);
        }
    }
    else
    {
        TclExecute("call:Log %d {H.245 Control is not connected}", call->id);
    }

    /* Type of call */
    if (cmCallGetParam(call->hsCall, cmParamIsMultiplexed, 0, &value, NULL) >= 0)
    {
        if (value != 0)
            TclExecute("call:Log %d {Q.931 Multiplexed call}", call->id);
    }
    if (cmCallGetParam(call->hsCall, cmParamH245Parallel, 0, &value, NULL) >= 0)
    {
        if (value != 0)
            TclExecute("call:Log %d {H.245 Parallel call}", call->id);
        else
        {
            if (cmCallGetParam(call->hsCall, cmParamH245Tunneling, 0, &value, NULL) >= 0)
            {
                if (value != 0)
                    TclExecute("call:Log %d {H.245 Tunnelled call}", call->id);
                else
                    TclExecute("call:Log %d {Normal or faststart call}", call->id);
            }
        }
    }

    /* Source aliases */
    maxParams = cmCallGetNumOfParams(call->hsCall, cmParamSourceAddress);
    if (maxParams > 0)
    {
        TclExecute("call:Log %d {Source aliases (%d):}", call->id, maxParams);

        for (i = 0; i < maxParams; i++)
        {
            RvInt32 aliasLen = (RvInt32)sizeof(alias);
            alias.length = (RvUint16)sizeof(aliasBufStr);
            if (cmCallGetParam(call->hsCall, cmParamSourceAddress, i, &aliasLen, (char *)&alias) >= 0)
                TclExecute("call:Log %d {%s}", call->id, Alias2String(&alias, NULL));
        }
    }

    /* Destination aliases */
    maxParams = cmCallGetNumOfParams(call->hsCall, cmParamDestinationAddress);
    if (maxParams > 0)
    {
        TclExecute("call:Log %d {Destination aliases (%d):}", call->id, maxParams);

        for (i = 0; i < maxParams; i++)
        {
            RvInt32 aliasLen = (RvInt32)sizeof(alias);
            alias.length = (RvUint16)sizeof(aliasBufStr);
            if (cmCallGetParam(call->hsCall, cmParamDestinationAddress, i, &aliasLen, (char *)&alias) >= 0)
            {
                TclExecute("call:Log %d {%s}", call->id, Alias2String(&alias, NULL));
            }
        }
    }

    return TCL_OK;
}


/********************************************************************************************
 * Call_Dial
 * purpose : Dial a new call to a destination.
 * syntax  : Call.Dial <remoteAddress> <aliases>
 *           <aliases box>  - location of the aliases
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_Dial(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj               *ep = (EpObj *)clientData;
    EpCallObj           *call;
    int                 index;
    RvStatus            status;

    RV_UNUSED_ARG(interp);

    /* Create a new call handle */
    call = epCallCreate(ep, NULL, 0);
    if (call == NULL)
        return TCL_OK;

    /* Set destination information */
    for (index = 2; index < argc; index++)
    {
        epCallSetDestAddress(call, argv[index], index);
    }

    /* Initiate FastStart procedures if needs be */
    if (ep->cfg.bSupportFaststart)
    {
        if (ep->cfg.bAutomaticFaststart)
            epCallGenerateFaststartRequest(call, RV_TRUE);
        else
        {
            initFastStartPorts(call);
            return TclExecute("Call.OpenOutgoingFaststart {%d %p} {Call.FastStartDial {%d %p} {%s}}",
                call->id, call, call->id, call, argv[2]);
        }
    }

    /* Create the call */
    status = epCallDial(call, argv[1]);
    if (status != RV_OK)
    {
        /* Make sure we close this call if we didn't succeed */
        TclExecute("test:Log {Call Dial failed, status=%d}", status);
        epCallDrop(call, (cmReasonType)-1, NULL);
    }

    return TCL_OK;
}


/********************************************************************************************
 * Call_Make
 * purpose : Make a quick new call to a destination.
 * syntax  : Call.Make <callInfo> <aliases>
 *           <alias> - whom to call
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_Make(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj               *ep = (EpObj *)clientData;
    EpCallObj           *call;
    RvChar              destAddr[256];
    cmNonStandardParam  param, *p_param = NULL;
    RvStatus            status;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if (argc == 2)
    {
        /* Create a new call handle */
        call = epCallCreate(ep, NULL, 0);
        if (call == NULL)
            return TCL_OK;
    }
    else
    {
        /* Got the call from the caller of this function */
        if (sscanf(argv[2], "%p:", &call) != 1)
            return TCL_OK;
    }

    if (isdigit((int)argv[1][0]))
    {
        int a, b, c, d, e;
        /* check if it's an IP v4 address */
        if (sscanf(argv[1], "%d.%d.%d.%d:%d", &a, &b, &c, &d, &e) == 5)
            sprintf(destAddr, "TA:%s", argv[1]);
        else /* so it's a tel-number */
            sprintf(destAddr, "TEL:%s", argv[1]);
    }
    else if (argv[1][0] == '[') /* maybe it's an IP v6 address */
        sprintf(destAddr, "TA:%s", argv[1]);
    else /* so it's just some other alias */
        strcpy(destAddr, argv[1]);

    /* see if Ad-Hoc conferencing is indicated */
    if (ep->cfg.bAdHocUse)
    {
        EpCallObj *sameConferenceCall;

        if (sscanf(ep->cfg.strAdHocCall, "%*d %p:", &sameConferenceCall) == 1)
            cmCallInvite(call->hsCall, sameConferenceCall->hsCall);
        TclSetVariable("tmp(adHoc,use)", "0");
    }

    /* see if Connection Re-use is indicated */
    if (ep->cfg.bMultiplexUse)
    {
        EpCallObj *sameConnectionCall;

        if (sscanf(ep->cfg.strMultiplexCall, "%*d %p:", &sameConnectionCall) == 1)
            cmCallMultiplex(call->hsCall, sameConnectionCall->hsCall);
        TclExecute("$tmp(basicTab).cnc.useMultiplex invoke");
    }

    /* Initiate FastStart Procedures if needs be */
    if (ep->cfg.bSupportFaststart)
    {
        if (ep->cfg.bAutomaticFaststart)
            epCallGenerateFaststartRequest(call, RV_TRUE);
        else
        {
            initFastStartPorts(call);
            return TclExecute("Call.OpenOutgoingFaststart {%d %p} {Call.FastStartMake {%d %p} {%s}}",
                call->id, call,
                call->id, call, destAddr);
        }
    }

    if (ep->cfg.bUseNSD)
    {
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;
    }

    /* Create the call */
    status = epCallMake(call, destAddr,
        ep->cfg.callConfig.requestedBandwidth,
        ep->cfg.strDisplay, ep->cfg.strUserUser, p_param);
    if (status != RV_OK)
    {
        TclExecute("test:Log {Call Make failed, status=%d}", status);
        epCallDrop(call, (cmReasonType)-1, NULL);
    }

    return TCL_OK;
}


/********************************************************************************************
 * Call_IncompleteAddress
 * purpose : This procedure is called when the "Incomplete Address" button is pressed on
 *           the incoming call window. It sends incomplete address message to the originator
 *           of the call
 * syntax  : Call.IncompleteAddress <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_IncompleteAddress(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    int         status;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    TclExecute("Window close .incall");

    status = epCallOverlapSending(call, NULL, RV_FALSE);

    return TCL_OK;
}


/********************************************************************************************
 * Call_AddressComplete
 * purpose : This procedure is called when the "Address Complete" button is pressed on
 *           the incoming call window. It sends Address Complete message to the originator
 *           of the call
 * syntax  : Call.AddressComplete <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_AddressComplete(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    RvStatus    status;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    TclExecute("Window close .incall");

    status = epCallOverlapSending(call, NULL, RV_TRUE);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SendAdditionalAddr
 * purpose : This procedure is called when the "Send" button is pressed on the redial
 *           window. It sends additional address information to the destination of the call.
 * syntax  : Call.SendAdditionalAddr <callInfo> <isARJ> <digits> <sendComplete> <aliases>
 *           <callInfo>   - Call information (counter and handle)
 *           <isARJ>      - RV_TRUE if message sent was ARJ
 *                          RV_FALSE if message sent was SETUP ACK
 *           <digits>     - In case we're dealing with SETUP ACK, we also get the digits
 *           <sendComplete> - sending is complete
 *           <aliases>    - List of the aliases for the ARJ case
 *
 * note    : if the case is not ARJ, the last two parameters can be omited
 *
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendAdditionalAddr(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    int         status;
    RvBool      isARJ;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if ((argv[3] == NULL) || (strlen(argv[3]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    isARJ = (atoi(argv[2]) != 0);

    call->sendComplete = (atoi(argv[4]) != 0);

    status = epCallOverlapSending(call, argv[3], call->sendComplete);
    TclExecute("call:Log %d {INFORMATION sent (status=%s, digits=%s)}",
        call->id, Status2String(status), argv[3]);

    if (call->sendComplete)
        TclExecute("Window delete .redial%p", call->hsCall);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SetRate
 * purpose : Set the rate of the call
 * syntax  : Call.SetRate <callInfo> <desiredBandwidth>
 *           <callInfo>         - Call information (counter and handle)
 *           <desiredBandwidth> - Amount of bandwidth to request
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SetRate(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj *call;
    RvUint32 bw;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    bw = atoi(argv[2]);
    cmCallSetRate(call->hsCall, bw);

    return TCL_OK;
}





/********************************************************************************************
 * Call_Drop
 * purpose : Drop the current selected call
 * syntax  : Call.drop <callInfo> <reason>
 *           <callInfo> - Call information (counter and handle)
 *           <reason>   - Reason string for dropping the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_Drop(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmNonStandardParam param, *p_param = NULL;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if (call->ep->cfg.bUseNSD)
    {
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;
    }

    epCallDrop(call, String2CMReasonType(argv[2]), p_param);

    return TCL_OK;
}


/********************************************************************************************
 * Call_DropAll
 * purpose : Drop all calls
 * syntax  : Call.DropAll <reason>
 *           <reason>   - Reason string for dropping the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_DropAll(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    epCallDropAll(ep, String2CMReasonType(argv[1]), RV_FALSE);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SendCallProceeding
 * purpose : send call proceeding message
 * syntax  : Call.SendCallProceeding <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendCallProceeding(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmNonStandardParam param, *p_param = NULL;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if (call->ep->cfg.bUseNSD)
    {
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;
    }

    epCallProceeding(call, p_param);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SendAlerting
 * purpose : send alert message
 * syntax  : Call.SendAlerting <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendAlerting(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmNonStandardParam param, *p_param = NULL;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if (call->ep->cfg.bUseNSD)
    {
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;
    }

    /* Send alerting message */
    epCallAlerting(call, p_param);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SendConnect
 * purpose : send connect message
 * syntax  : Call.SendConnect <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendConnect(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmNonStandardParam param, *p_param = NULL;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* see if Ad-Hoc conferencing is indicated */
    if (call->ep->cfg.bAdHocUse)
    {
        EpCallObj   *adHocConfCall;
        TclSetVariable("tmp(adHoc,use)", "0");

        if (sscanf(call->ep->cfg.strAdHocCall, "%*d %p:", &adHocConfCall) == 1)
            if (cmCallJoin(call->hsCall, adHocConfCall->hsCall) >= 0)
                return TCL_OK;
    }

    if (call->ep->cfg.bUseNSD)
    {
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;
    }

    epCallConnect(call,
        call->ep->cfg.strDisplay,
        call->ep->cfg.strUserUser,
        p_param);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SendStatusInquiry
 * purpose : Send Status Inquiry message
 * syntax  : Call.SendStatusInquiry <callInfo>
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendStatusInquiry(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallStatusEnquiry(call, call->ep->cfg.strDisplay);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SendProgress
 * purpose : Send Progress message
 * syntax  : Call.SendProgress <callInfo>
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendProgress(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmNonStandardParam param, *p_param = NULL;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(interp);

    /* Get CallInfo and display */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if (call->ep->cfg.bUseNSD)
    {
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;
    }

    epCallProgress(call, call->ep->cfg.strDisplay, p_param);

    return TCL_OK;
}




/********************************************************************************************
 * Call_SendNotify
 * purpose : Send Notify message
 * syntax  : Call.SendNotify <callInfo>
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendNotify(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmNonStandardParam param, *p_param = NULL;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(interp);

    /* Get CallInfo and display */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if (call->ep->cfg.bUseNSD)
    {
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;
    }

    epCallNotify(call, 7, call->ep->cfg.strDisplay, p_param);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SendUserInformation
 * purpose : Send user info message
 * syntax  : Call.SendNotify <callInfo> <display>
 *           <callInfo> - Call information (counter and handle)
 *           <display> - Display string
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendUserInformation(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmNonStandardParam param;
    cmNonStandardParam *p_param = NULL;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(interp);

    /* Get CallInfo and display */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if (call->ep->cfg.bUseNSD)
    {
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;
    }

    epCallUserInformation(call, argv[2], p_param);

    return TCL_OK;
}


/********************************************************************************************
 * Call_CreateH245
 * purpose : Create an H245 session is got ip and port, listen for an H245 connection if not.
 * syntax  : Call.CreateH245 <callInfo> <ipAddress>
 *           <callInfo>  - Call information (counter and handle)
 *           <ipAddress> - Address to connect to. if not present, will listen.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_CreateH245(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmTransportAddress trans, *pTrans = NULL;
    char addrStr[64];

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    memset(&trans, 0, sizeof(trans));

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if ( argv[2][0] && (cmString2TransportAddress(argv[2], &trans) >= 0) )
        pTrans = &trans;

    epCallCreateH245(call, pTrans);

    memset(addrStr, 0, sizeof(addrStr));
    cmTransportAddress2String(pTrans, sizeof(addrStr), addrStr);
    TclExecute("set tmp(h245,address) {%s}", addrStr);

    return TCL_OK;
}


/********************************************************************************************
 * Call_LoopOff
 * purpose : Releases all media loops in this call. Sends a maintenanceLoopOffCommand message
 *           to the remote terminal.
 * syntax  : Call.LoopOff <callInfo>
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_LoopOff(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallMediaLoopOff(call);

    return TCL_OK;
}


/********************************************************************************************
 * Call_ConnectControl
 * purpose : Request to establish H245 control channel.
 *           This function may be called after call was established with fast start,
 *           and h245 connection wasn't opened yet
 * syntax  : Call.ConnectControl <callInfo>
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_ConnectControl(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallConnectControl(call);

    return TCL_OK;
}

/********************************************************************************************
 * Call_SeperateControl
 * purpose : Request to establish H245 control channel.
 *           This function may be called after call was established with tunneling,
 *           and h245 connection wasn't opened yet
 * syntax  : Call.SeperateControl <callInfo>
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SeperateControl(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallSeparateControl(call);

    return TCL_OK;
}

/********************************************************************************************
 * Call_CloseControl
 * purpose : Request to close H245 control channel.
 * syntax  : Call.CloseControl <callInfo>
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_CloseControl(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallCloseControl(call);

    return TCL_OK;
}

/********************************************************************************************
 * Call_SendCaps
 * purpose : Request to send a new capabilities message.
 * syntax  : Call.SendCaps <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendCaps(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallSendCapabilities(call, RV_PVT_INVALID_NODEID, RV_FALSE);

    return TCL_OK;
}

/********************************************************************************************
 * Call_SendEmptyCaps
 * purpose : Request to send an empty capabilities message.
 * syntax  : Call.SendEmptyCaps <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendEmptyCaps(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallSendCapabilities(call, RV_PVT_INVALID_NODEID, RV_TRUE);

    return TCL_OK;
}

/********************************************************************************************
 * Call_SendCapsAck
 * purpose : Request to send a new capabilities ack message.
 * syntax  : Call.SendCapsAck <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendCapsAck(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallSendCapabilitiesAck(call);

    return TCL_OK;
}

/********************************************************************************************
 * Call_SendMSD
 * purpose : Request to send a new master slave determination message.
 * syntax  : Call.SendMSD <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendMSD(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallSendMasterSlave(call, -1);

    return TCL_OK;
}

/********************************************************************************************
 * Call_SendRoundTrip
 * purpose : Request to send a round trip delay message.
 * syntax  : Call.SendRoundTrip <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendRoundTrip(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    epCallSendRoundTripDelay(call, 10);

    return TCL_OK;
}

/********************************************************************************************
 * Call_UII
 * purpose : Request to send a user input indication.
 * syntax  : Call.UII <callInfo> <user input>
 *           <callInfo> - Call information (counter and handle)
 *           <user input> - Input from the user.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_UII(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;
    cmNonStandardParam param;
    cmNonStandardParam *p_param = NULL;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(interp);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if (call->ep->cfg.bUseNSD)
    {
        /* create the non-standard-data UII node ID */
        char * nsdStr = call->ep->cfg.nsdString;
        if (String2CMNonStandardParam(nsdStr, &param) >= 0)
            p_param = &param;

        epUtilsEvent(call->ep, "Rec", NULL, "sendUserInputIndication entry= %d value= \"%s\" nsd= \"%s\"",
            call->id, argv[2], nsdStr);
    }
    else
    {
        epUtilsEvent(call->ep, "Rec", NULL, "sendUserInputIndication entry= %d value= \"%s\"",
            call->id, argv[2]);
    }

    epCallSendUserInputIndication(call, argv[2], p_param);

    return TCL_OK;
}


/********************************************************************************************
 * Call_FastStart_Dial
 * purpose : Finishes the Call_Dial process after the user has selected channels.
 * syntax  : Call.FastStartDial <callInfo> <address>
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_FastStartDial(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj   *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get the call handle */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* Create the call */
    if (epCallDial(call, argv[2]) != RV_OK)
    {
        /* Make sure we close this call if we didn't succeed */
        epCallDrop(call, (cmReasonType)-1, NULL);
    }
    return TCL_OK;
}


/********************************************************************************************
 * Call_FastStartMake
 * purpose : Finishes the Call_Make process after the user has selected channels.
 * syntax  : Call.FastStartMake <callInfo> <destAddress> (formatted)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_FastStartMake(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    EpCallObj   *call;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get the call handle */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* Create the call */
    if (epCallMake(call, argv[2],
        ep->cfg.callConfig.requestedBandwidth,
        ep->cfg.strDisplay, ep->cfg.strUserUser, NULL) != RV_OK)
    {
        /* Make sure we close this call if we didn't succeed */
        epCallDrop(call, (cmReasonType)-1, NULL);
    }
    return TCL_OK;
}


/********************************************************************************************
 * Call_OpenOutgoingFaststart
 * purpose : Open faststart options for an outgoing call
 * syntax  : Call.OpenOutgoingFaststart <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_OpenOutgoingFaststart(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    int retVal;
    EpObj *ep = (EpObj *)clientData;
    EpCallObj *call;
    char *DataTypeNameList[20], DataTypeNames[20][20];
    char TclDataType[20*20], *temp;
    int i;
    RvInt32 efcState = (RvInt32)cmExtendedFastConnectNo;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* preparing DataTypeNameList */
    for (i=0;i<20;i++)
        DataTypeNameList[i] = DataTypeNames[i];

    /* get DataType name list from configuration file */
    retVal = cmGetConfigChannels(ep->hApp, 20, 20, DataTypeNameList);

    if (retVal < 0)
        return TCL_OK;

    /*preparing string of DataTypes for the combo box in the fast start window*/
    strcpy(TclDataType,DataTypeNameList[0]);
    temp = TclDataType;
    if (retVal > 20) retVal = 20;
    for(i=1; i < retVal; i++)
    {
        temp = temp + strlen(DataTypeNameList[i-1])-1 ;
        *(++temp) = ' ';
        ++temp;
        strcpy(temp,DataTypeNameList[i]);
    }

    cmCallGetParam(call->hsCall, cmParamExtendedFastConnectState, 0, &efcState, NULL);
    if ((cmExtendedFastConnectState_e)efcState == cmExtendedFastConnectApproved)
    {
        TclExecute("Window open .faststart {%d %p} 0 {%s nullData} {%s}",
            call->id, call, TclDataType, argv[2]);
    }
    else
    {
        TclExecute("Window open .faststart {%d %p} 0 {%s} {%s}",
            call->id, call, TclDataType, argv[2]);
    }
    TclExecute("Call:addWindowToList %p .faststart0@%p", call, call);

    return TCL_OK;
}

/********************************************************************************************
 * Call_BuildOutgoingFaststart
 * purpose : build the faststart message with the new API.
 * syntax  : Call.BuildOutgoingFaststart <callInfo> <followUp> <accept> [channel]
 *           <callInfo> - Call information (counter and handle)
 *           <followUp> - the command that will be executed after done
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_BuildOutgoingFaststart(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj *call;
    int                 status;
    int                 sessionId;
    cmChannelDirection  direction = (cmChannelDirection)-1;
    cmTransportAddress  rtp, rtcp;
    char                name[32], dir[8], rtpS[64], rtcpS[64];
    RvBool              bAccept = RV_FALSE;
    RvInt32             efcState = (RvInt32)cmExtendedFastConnectNo;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(interp);

    /* Make sure we have a call */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p", &call) != 1) return TCL_OK;

    cmCallGetParam(call->hsCall, cmParamExtendedFastConnectState, 0, &efcState, NULL);
    if ((cmExtendedFastConnectState_e)efcState == cmExtendedFastConnectApproved)
    {
        bAccept = argv[3][0] == '1';
    }

    if (argc == 5)
    {
        if (sscanf(argv[4], "%s : %s , %d ; %s , %s", name, dir, &sessionId, rtcpS, rtpS) != 5)
            return RV_OK;

        if (dir[0] == 'I') direction = dirReceive;
        else if (dir[0] == 'O') direction = dirTransmit;
        else if (dir[0] == 'B') direction = dirBoth;

        cmString2TransportAddress(rtpS,  &rtp);
        cmString2TransportAddress(rtcpS, &rtcp);

        status = epCallFaststartAdd(call, sessionId, direction, bAccept, name, RV_PVT_INVALID_NODEID, &rtp, &rtcp);
        if (status != RV_OK)
        {
            TclExecute("msgbox Error picExclamation {Unable to set faststart proposals} { { Ok -1 <Key-Return> } }");
        }
        return TCL_OK;
    }

    epCallFaststartReady(call, bAccept);

    /* Close this window when we're done... */
    TclExecute("after idle {Window delete .faststart0@%p}", call);
    return TclExecute(argv[2]);
}


/********************************************************************************************
 * Call_ApproveFaststart
 * purpose : approve the channels that are going to be opened in faststart
 * syntax  : Call.ApproveFaststart <callInfo> [channel]
 *           <callInfo> - Call information (counter and handle)
 *           <msgHandle> - pointer to the fs message.
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_ApproveFaststart(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{

    int                         index;
    EpCallObj                   *call;
    cmTransportAddress          rtp, rtcp;
    int                         sessionId;
    char                        name[32], dir[8], rtpS[32], rtcpS[32];

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(interp);

    /* Make sure we have a call */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p", &call) != 1) return TCL_OK;

    if (argc == 3)
    {
        if (sscanf(argv[2], "%d. %s : %s , %d ; %s , %s", &index, name, dir, &sessionId, rtcpS, rtpS) != 6)
            return TCL_OK;

        cmString2TransportAddress(rtcpS, &rtcp);
        cmString2TransportAddress(rtpS,  &rtp);
        epCallFaststartAckIndex(call, index, &rtp, &rtcp);
        return TCL_OK;
    }

    /* Respond to the request */
    epCallFaststartReady(call, RV_TRUE);

    TclExecute("Window delete .faststart1@%p", call);
    return TCL_OK;
}

/********************************************************************************************
 * Call_RefuseFaststart
 * purpose : Refuse the channels that are going to be opened in faststart
 * syntax  : Call.RefuseFaststart <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_RefuseFaststart(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p", &call) != 1) return TCL_OK;

     /* reject the FS */
    epCallFaststartRefuse(call);

    TclExecute("Window delete .faststart1@%p", call);

    return TCL_OK;
}


/********************************************************************************************
 * Call_DropAllFastStart
 * purpose : Drop all fast connect channels in the call
 * syntax  : Call.DropAllFastStart <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_DropAllFastStart(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p", &call) != 1) return TCL_OK;

     /* close all EFC channels */
    epCallExtFastConnectCloseAllChannels(call);

    TclExecute("Window delete .faststart0@%p", call);

    return TCL_OK;
}


/********************************************************************************************
 * Call_SendFacility
 * purpose : Send Facility message
 * syntax  : Call.SendFacility <callInfo> [type [userInput nsdChoice [nsdMan nsdCoun nsdExten]/[nsdObj]]/[transAddress aliases]]
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendFacility(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    if (argc == 2)
    {
        epCallFacility(call, cmReasonTypeUndefinedReason, NULL, NULL, EpCallFacilityEmpty);
    }
    else if (argv[2][0] == 'u') /* type == undefined */
    {
        cmNonStandardParam  param;
        cmNonStandardParam  *p_param = NULL;

        if (call->ep->cfg.bUseNSD)
        {
            /* Get the User Input string from the facility screen */
            param.data = argv[3];
            param.length = (int)strlen(param.data);
            /* get the NSD param */
            if (argv[4][0] == 'H') /* H.221 */
            {
                /* get the H221 vars from the MiscTab */
                param.info.objectLength = 0;
                param.info.manufacturerCode = (RvUint16)atoi(argv[5]);
                param.info.t35CountryCode = (RvUint8)atoi(argv[6]);
                param.info.t35Extension = (RvUint8)atoi(argv[7]);
            }
            else /* Object */
            { /* get the Object vars from the MiscTab */
                strcpy(param.info.object, argv[5]);
                param.info.objectLength = strlen(param.info.object);
            }

            p_param = &param;
        }

        /* send an undefined facility message */
        epCallFacility(call, cmReasonTypeUndefinedReason, NULL, p_param, EpCallFacilityReason);
    }
    else /* type == forward */
    {
        /* send a forwarding message */
        char fwdAddr[256];
        char * ipAddr = argv[3];
        char * aliases = argv[4];

        fwdAddr[0] = '\0';
        if (ipAddr[0]) /* if the transportAddress field has been filled */
            sprintf(fwdAddr, "TA:%s", ipAddr);
        if (aliases[0] != '\0')
        {/* if there are any aliases */
            int i,j;
            /* add after the ip, if present */
            j = strlen(fwdAddr);
            if (j)
            {
                fwdAddr[j] = ',';
                j++;
            }
            /* transfer the aliases while formatting */
            for(i=0; i <= (int) strlen(aliases); i++)
            {
                if (aliases[i] == ' ') fwdAddr[j] = ',';
                else if (aliases[i] == '{') j--;
                else if (aliases[i] == '}') j--;
                else
                {
                    fwdAddr[j] = aliases[i];
                    if (fwdAddr[j] == '\0')
                        break;
                }
                j++;
            }
        }

        /* sent call forward */
        epCallFacility(call, cmReasonTypeUndefinedReason, fwdAddr, NULL, EpCallFacilityForward);
    }

    TclExecute("Window close .facility");

    return TCL_OK;
}



/********************************************************************************************
 * Multiplex_Update
 * purpose : This procedure is called when the Update button is pressed on the multiplex frame.
 *           It sets the multiplex and maintain parameters of a call.
 * syntax  : Multiplex.Update <callInfo> <multiplex> <maintain>
 *           <callInfo> - Call information (counter and handle)
 *           <multiplex> - value of the multiplex check box.
 *           <maintain> - value of the maintain check box.
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Multiplex_Update(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* set Multiplex and Maintain */
    cmCallSetParam(call->hsCall, cmParamIsMultiplexed, 0, atoi(argv[2]), NULL);
    cmCallSetParam(call->hsCall, cmParamShutdownEmptyConnection, 0, !atoi(argv[3]), NULL);

    /* We're making sure the other side knows about this change by sending a FACILITY message */
    epCallFacility(call, cmReasonTypeUndefinedReason, NULL, NULL, EpCallFacilityDefault);

    /* Give out indication */
    TclExecute("call:Log %d {Updated: Multiplex = %s Maintain = %s}", call->id, argv[2], argv[3]);

    return TCL_OK;
}



/********************************************************************************************
 * EFC_Set
 * purpose : This procedure is called when the Set button next to EFC is pressed.
 *           It sets the EFC state on the call.
 * syntax  : EFC.Set <callInfo> <efc-state>
 *           <callInfo> - Call information (counter and handle)
 *           <multiplex> - value of the multiplex check box.
 *           <maintain> - value of the maintain check box.
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int EFC_Set(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* set EFC */
	if ((!call->isIncoming && (call->state == cmCallStateInit)) ||
        (call->isIncoming  && (call->state != cmCallStateConnected))) 
	{
		cmCallSetParam(call->hsCall, cmParamExtendedFastConnectState, 0, atoi(argv[2]), NULL);
	}

    return TCL_OK;
}





/******************************************************************************
 * prepareIncomingCallWindow
 * ----------------------------------------------------------------------------
 * General: Fill in the incoming call window with tall the relevant call
 *          fields.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  call         - The call object.
 * Output: None.
 *****************************************************************************/
static void prepareIncomingCallWindow(
    IN EpCallObj    *call)
{
    static const RvChar* incallInfo = ".incall.info.sourDest.lis";
    static const RvChar* checkboxState[] = {"deselect", "select"};

    RvInt32 isEarly245 = RV_FALSE;
    RvInt32 isFastConnect = RV_FALSE;
    RvInt32 isTunneling = RV_FALSE;
    RvInt32 isComplete = RV_FALSE;
    cmAlias alias;
    RvChar str[513];
    int i, aliasStatus;
    RvInt32 aliasLen;
    cmTransportAddress earlyH245Addr;

    /* Get some boolean parameters about this call */
    isEarly245 = (cmCallGetParam(call->hsCall, cmParamSetupH245Address, 0, NULL, (char *)&earlyH245Addr) >= 0);
    if (cmCallGetParam(call->hsCall, cmParamSetupSendingComplete, 0, &isComplete, NULL) < 0)
        isComplete = RV_FALSE;
    isTunneling = (cmCallHasControlSession(call->hsCall) > 0);
    isFastConnect = (cmCallGetParam(call->hsCall, cmParamSetupFastStart, 0, &isFastConnect, NULL)>=0);

    /* Clear source-dest information window */
    TclExecute("%s delete 0 end", incallInfo);

    /* Get Source information */
    TclExecute("%s insert end {Source:}\n"
               "%s itemconfigure end -foreground red",
               incallInfo, incallInfo);
    aliasStatus = 0;
    i = 0;
    alias.string = str;
    while (aliasStatus >= 0)
    {
        /* Get an alias from the CM */
        aliasLen = (RvInt32)sizeof(alias);
        alias.length = (RvUint16)sizeof(str);
        aliasStatus = cmCallGetParam(call->hsCall, cmParamSourceAddress, i, &aliasLen, (char *)&alias);
        if (aliasStatus >= 0)
        {
            /* Convert the alias into a string */

            /* Add the string to the source-dest information */
            TclExecute("%s insert end {%s}", incallInfo, Alias2String(&alias, NULL));
            i++;
        }
    }

    /* Get Destination information */
    TclExecute("%s insert end {} {Destination:}\n"
               "%s itemconfigure end -foreground red",
               incallInfo, incallInfo);
    aliasStatus = 0;
    i = 0;
    while (aliasStatus >= 0)
    {
        /* Get an alias from the CM */
        aliasLen = (RvInt32)sizeof(alias);
        alias.length = (RvUint16)sizeof(str);
        aliasStatus =  cmCallGetParam(call->hsCall, cmParamDestinationAddress, i, &aliasLen, (char *)&alias);
        if (aliasStatus >= 0)
        {
            /* Add the string to the source-dest information */
            TclExecute("%s insert end {%s}", incallInfo, Alias2String(&alias, NULL));
            i++;
        }
    }

    /* Set the remote parameters to the incoming call window */
    TclExecute(".incall.remote.fs %s\n"
               ".incall.remote.e245 %s\n"
               ".incall.remote.sendCmplt %s\n"
               ".incall.remote.tun %s\n",
               checkboxState[isFastConnect],
               checkboxState[isEarly245],
               checkboxState[isComplete],
               checkboxState[isTunneling]);

    aliasLen = (RvInt32)sizeof(str);
    if (cmCallGetParam(call->hsCall, cmParamDisplay, 0, &aliasLen, str) >= 0)
        TclExecute(".incall.info.disp configure -text {%s}", str);
    aliasLen = (RvInt32)sizeof(str);
    if (cmCallGetParam(call->hsCall, cmParamUserUser, 0, &aliasLen, str) >= 0)
    {
        str[aliasLen] = '\0'; /* Make sure userUser gets a NULL termination */
        TclExecute(".incall.info.uui configure -text {%s}", str);
    }
}




#ifdef __cplusplus
}
#endif


