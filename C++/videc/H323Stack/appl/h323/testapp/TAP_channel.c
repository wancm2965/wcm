/***********************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
 *                                TAP_channel.c
 *
 * This file provides functions for handling the channels of a call.
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

#include "lepConversions.h"
#include "lepChannel.h"

#include "h245.h"
#include "TAP_channel.h"




/******************************************************************************
 * epChannelNew
 * ----------------------------------------------------------------------------
 * General: Indication of a new channel object that was created and should be
 *          displayed in the GUI.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  chan         - The channel object.
 * Output: None.
 *****************************************************************************/
void epChannelNew(
    IN EpChannelObj         *chan)
{
    if (chan->call->ep->cfg.bPopUp && !chan->isOutgoing && chan->call->wasConnectedControl)
    {
        /* Manually open the channel (ask user to accept it) */
        TclExecute("Window open .inchan");
        TclExecute(".inchan.callInfo configure -text {%p}", chan);
        TclExecute("Call:addWindowToList %p .inchan", chan->call);

        /* Display channel information inside the listbox */
        epDisplayChannelInfo(chan, ".inchan.chanInfo.txt");
    }
}


/******************************************************************************
 * epChannelStable
 * ----------------------------------------------------------------------------
 * General: Indication that a channel reached a stable state - either connected
 *          or idle.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  chan         - The channel object.
 * Output: None.
 *****************************************************************************/
void epChannelStable(
    IN EpChannelObj         *chan)
{
    /* terminate the inChan window */
    TclExecute("inchan:terminate {%p}", chan);
}


/********************************************************************************************
 * epDisplayChannelInfo
 * purpose : Display the information of a channel inside a listbox widget
 *           This function is called by Channel.DisplayChannelList or call related C functions
 * input   : chan           - Channel handle of the application
 *           listboxWidget  - Name of the listbox widget to fill
 * output  : none
 * return  : none
 ********************************************************************************************/
void epDisplayChannelInfo(
    IN EpChannelObj *chan,
    IN const RvChar *listboxWidget)
{
    EpCallObj *call;
    int isDuplex;
    int channelId;

    call = chan->call;

    /* Get the channel's parameters */
    isDuplex = cmChannelIsDuplex(chan->hsChan);
    channelId = cmChannelSessionId(chan->hsChan);

    TclExecute("%s delete 1.0 end\n"
               "%s insert end {"
               "Call: %d, %p\n"
               "Channel ID: %d\n"
               "Type: %s}",
               listboxWidget,
               listboxWidget,
               call->id, call,
               channelId,
               (isDuplex) ? "Bidirectional" : "Unidirectional");
}




/********************************************************************************************
 *                                                                                          *
 *                                  Private functions                                       *
 *                                                                                          *
 ********************************************************************************************/


/********************************************************************************************
 * DisplayChannelList
 * purpose : Display the list of channels inside the channels window by the selected call
 * input   : call   - Call information holding the channels
 *           widget - Listbox widget to fill with channel's information
 *                    If set to NULL, the default main channels list is used
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
void DisplayChannelList(
    IN EpCallObj    *call,
    IN const RvChar *widget)
{
    EpChannelObj *chan;
    const RvChar *chanWidget;
    RvBool mark = RV_FALSE;
    int incomingChannels = 0;
    int outgoingChannels = 0;

    if (widget == NULL)
    {
        const char * result;
        chanWidget = ".test.chan";

#ifdef USE_TCL
        /* Make sure the selected call is the one that we're updating */
        TclExecute(".test.calls.list selection includes [Call.FindCallIndex %d]", call->id);
        result = Tcl_GetStringResult(gInterp);
        if (result[0] == '0')
            return;
#endif
    }
    else
        chanWidget = widget;

    /* Delete channels list */
    TclExecute("%s.inList delete 0 end\n"
               "%s.outList delete 0 end\n",
               chanWidget, chanWidget);

    /* Rewrite the channels information inside the list */
    chan = call->firstChannel;
    if (chan != NULL)
        mark = chan->mark; /* coloring system. used for printing */
    while (chan != NULL)
    {
        int isDuplex;

        /* Get channel parameters */
        isDuplex = cmChannelIsDuplex(chan->hsChan);

        if (chan->isOutgoing)
            outgoingChannels++;
        else
            incomingChannels++;

        if (chan->mark == mark)
        {
            EpChannelObj *sameSessionChannel;
            int sessionId = cmChannelSessionId(chan->hsChan);

            /* Display channel's information */
            TclExecute("%s.%s insert end {%d %p %s %s %s}",
                chanWidget,
                (chan->isOutgoing ? "outList" : "inList"),
                sessionId,
                chan,
                chan->dataTypeName,
                (isDuplex ? "Duplex" : "Simplex"),
                CMChannelState2String(chan->state));

            sameSessionChannel = NULL;
            cmChannelSameSession(chan->hsChan, (HAPPCHAN *)&sameSessionChannel, NULL);
            if ((sameSessionChannel != NULL) && (sameSessionChannel->mark == mark))
            {
                TclExecute("%s.%s insert end {%d %p %s %s %s}",
                    chanWidget,
                    (sameSessionChannel->isOutgoing ? "outList" : "inList"),
                    sessionId,
                    sameSessionChannel,
                    sameSessionChannel->dataTypeName,
                    (isDuplex ? "Duplex" : "Simplex"),
                    CMChannelState2String(sameSessionChannel->state));
                sameSessionChannel->mark = !mark;
            }
            else
            {
                TclExecute("%s.%s insert end {}",
                chanWidget,
                (chan->isOutgoing ? "inList" : "outList"));
            }

            chan->mark = !mark;
        }
        chan = chan->nextChannel;
    }

    if (incomingChannels > 0)
        TclExecute("balloon:set %s.inList {Incoming channels (%d)}", chanWidget, incomingChannels);
    else
        TclExecute("balloon:set %s.inList {Incoming channels}", chanWidget);
    if (outgoingChannels > 0)
        TclExecute("balloon:set %s.outList {Outgoing channels (%d)}", chanWidget, outgoingChannels);
    else
        TclExecute("balloon:set %s.outList {Outgoing channels}", chanWidget);
}


/********************************************************************************************
 * AutoMimic
 * purpose : Automatically open a channel for an incoming channel
 * input   : InChan - The incoming channel to mimic
 * output  : none
 * return  : none
 ********************************************************************************************/
static void AutoMimic(
    IN EpChannelObj *inChan)
{
    /* Check if auto-mimic option is set or not */
    if (!inChan->call->ep->cfg.bAutoMimicChannels)
        return;

    epChannelMimic(inChan, 0);
}








/********************************************************************************************
 *                                                                                          *
 *                                  Public functions                                        *
 *                                                                                          *
 ********************************************************************************************/


/********************************************************************************************
 * Channel_OpenOutgoingWindow
 * purpose : Open the "New Channel" window
 *           This function is called from the TCL when the "New Channel" button is pressed in
 *           the main application window.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_OpenOutgoingWindow(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;

    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    TclExecute("Window open .newchan\n"
               ".newchan.callInfo configure -text {%d %p}",
               call->id, call);
    TclExecute("Call:addWindowToList %p .newchan", call);
    return TCL_OK;
}


/********************************************************************************************
 * Channel_ConnectOutgoing
 * purpose : Connect a new outgoing channel
 *           This function is called from the TCL when the "Ok" button is pressed in the
 *           "New Channel" window.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_ConnectOutgoing(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj           *ep = (EpObj *)clientData;
    char            *dataType;
    EpCallObj       *call;
    EpChannelObj    *newChan;
    EpChannelObj    *sessionChan = NULL;
    EpChannelObj    *replaceChan = NULL;
    RvStatus        status = RV_OK;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* set the dataType */
    dataType = argv[2];

    /* see if replace or mimic operation is called for */
    if(argv[3][0])
    {
        /* argv[3] is not "" */
        if(argv[3][0] == 'S') /* "SameAs" -> session chan */
            sscanf(argv[4], "%*d %p", &sessionChan);
        else if(argv[3][0] == 'R') /* "Replace" -> replace chan */
            sscanf(argv[4], "%*d %p", &replaceChan);
        else return TCL_ERROR; /* something not right with the command */
    }

    newChan = epChannelCreate(ep, call, 0);
    if (newChan != NULL)
        status = epChannelOpen(newChan, dataType, sessionChan, replaceChan, 0);
    if ((argv[3][0] == 'R') && (newChan != NULL) && (status == RV_OK))
    {
        /* The app is responsible for dropping the channel, so here it is: */
        TclExecute("after 3000 {Channel.Drop \"\" {%s} 3}", argv[4]);
    }

    return TCL_OK;
}


/********************************************************************************************
 * Channel_DisplayChannelList
 * purpose : Display the list of channels inside the channels window by the selected call
 * syntax  : Channel.DisplayChannelList <callInfo> <frameWidget>
 *           <callInfo>     - Call information (counter and handle)
 *           <frameWidget>  - Frame widget holding channel listboxes
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_DisplayChannelList(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    char* widget;
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);

    /* Make sure we have a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;

    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    /* Check if we have the second parameter (listbox widget to display in) */
    if ((argc < 2) || (argv[2] == NULL) || (strlen(argv[2]) == 0))
        widget = NULL;
    else
        widget = argv[2];

    /* Display the channels */
    DisplayChannelList(call, widget);

    return TCL_OK;
}


/********************************************************************************************
 * Channel_ResponseForOlC
 * purpose : send ACK or REJECT after receiving OLC
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_ResponseForOLC(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    EpChannelObj *chan;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get channel handle */
    if (sscanf(argv[1], "%p", &chan) != 1)
        return TCL_OK;

    if (chan == NULL)
        return TCL_OK;
    call = chan->call;

    /* Check response value */
    if (strcmp(argv[2], "Accept") == 0)
    {
        epChannelRespond(chan, RV_TRUE, (cmRejectLcnReason)0);

        /* Mimic channel opening if we have to */
        AutoMimic(chan);
    }
    else
    {
        RvChar *reasonStr = argv[3];
        cmRejectLcnReason reason = (cmRejectLcnReason)atoi(reasonStr);

        epChannelRespond(chan, RV_FALSE, reason);
    }

    /* Close incoming channel window */
    TclExecute("Window close .inchan");

    /* Refresh list of channels */
    DisplayChannelList(call, NULL);

    return TCL_OK;
}


/********************************************************************************************
 * Channel_ResponseForClC
 * purpose : send ACK or REJECT after receiving CLC
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_ResponseForCLC(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    EpChannelObj *chan;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get channel handle */
    if (sscanf(argv[1], "%p", &chan) != 1)
        return TCL_OK;

    if (chan == NULL)
        return TCL_OK;
    call = chan->call;

    /* Check response value */
    /* Check response value */
    if (strcmp(argv[2], "Accept") == 0)
    {
        epChannelRespond(chan, RV_TRUE, (cmRejectLcnReason)0);
        epChannelDrop(chan, cmCloseReasonUnknown);
    }
    else
    {
        epChannelRespond(chan, RV_FALSE, (cmRejectLcnReason)0);
    }

    /* Close incoming channel window */
    TclExecute("Window close .dropchan");

    /* Refresh list of channels */
    DisplayChannelList(call, NULL);

    return TCL_OK;
}


/********************************************************************************************
 * Channel_Drop
 * purpose : drops a logical channel
 * syntax  : Channel.Drop <channelHandle1> <channelHandle2> <dropReason>
 *           <channelHandle?>   - Channel information (call and channel id)
 *           <dropReason>       - Drop reason for the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_Drop(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    cmCloseLcnReason dropReason;
    EpChannelObj *chan;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Get drop reason */
    dropReason = (cmCloseLcnReason)atoi(argv[3]);

    if (sscanf(argv[1], "%*d %p", &chan) == 1)
        epChannelDrop(chan, dropReason);

    if (sscanf(argv[2], "%*d %p", &chan) == 1)
        epChannelDrop(chan, dropReason);

    return TCL_OK;
}


/********************************************************************************************
 * Channel_Answer
 * purpose : answers a logical channel
 * syntax  : Channel.Answer <channelHandle1> <channelHandle2>
 *           <channelHandle?>   - Channel information (call and channel id)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_Answer(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpChannelObj *chan;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if (sscanf(argv[1], "%*d %p", &chan) == 1)
    {
        epChannelRespond(chan, RV_TRUE, (cmRejectLcnReason)0);
    }

    return TCL_OK;
}


/********************************************************************************************
 * Channel_MediaLoop
 * purpose : Requests media loop on the specified channel. Sends a maintenanceLoopRequest
 *           message to the remote terminal.
 * syntax  : Channel.MediaLoop <channelHandle1> <channelHandle2>
 *           <channelHandle?>   - Channel information (call and channel id)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_MediaLoop(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpChannelObj *chan;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if (sscanf(argv[1], "%*d %p", &chan) == 1)
        epChannelMediaLoopRequest(chan);

    return TCL_OK;
}


/********************************************************************************************
 * Channel_Rate
 * purpose : Changes the channel rate
 * syntax  : Channel.Rate <rate> <channelHandle1> <channelHandle2>
 *           <rate>           - rate of the channel
 *           <channelHandle?> - Channel information (call and channel id)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_Rate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpChannelObj *chan;
    RvUint32 rate = (RvUint32)atoi(argv[1]);

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if (sscanf(argv[2], "%*d %p", &chan) == 1)
        epChannelSetRate(chan, rate);
    if (sscanf(argv[3], "%*d %p", &chan) == 1)
        epChannelSetRate(chan, rate);

    return TCL_OK;
}


/********************************************************************************************
 * Channel_FastUpdate
 * purpose : Send a videoFastUpdate request on an incoming channel.
 * syntax  : Channel.FastUpdate <channelHandle>
 *           <channelHandle> - Channel information (call and channel id)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_FastUpdate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpChannelObj *chan;
    RvStatus status = RV_OK;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    if (sscanf(argv[1], "%*d %p", &chan) == 1)
        status = epChannelVideoFastUpdate(chan);

    if (status != RV_OK)
        return TCL_ERROR;

    return TCL_OK;
}



#ifdef __cplusplus
}
#endif
