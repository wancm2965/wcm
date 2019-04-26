#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
 *                                channel.h
 *
 * This file provides functions for handling the channels of a call.
 *
 *
 *      Written by                        Version & Date                        Change
 *     ------------                       ---------------                      --------
 *      Oren Libis                          04-Mar-2000
 *
 ********************************************************************************************/

#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "TAP_init.h"
#include "TAP_general.h"
#include "TAP_defs.h"



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
    IN EpChannelObj         *chan);


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
    IN EpChannelObj         *chan);


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
    IN const RvChar *listboxWidget);




/********************************************************************************************
 *
 *  TCL procedures
 *
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
int Channel_OpenOutgoingWindow(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


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
int Channel_ConnectOutgoing(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/********************************************************************************************
 * Channel_DisplayChannelList
 * purpose : Display the list of channels inside the channels window by the selected call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_DisplayChannelList(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


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
int Channel_ResponseForOLC(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/********************************************************************************************
 * Channel_Answer
 * purpose : answers a logical channel
 * syntax  : Channel.Drop <channelHandle1> <channelHandle2>
 *           <channelHandle?>   - Channel information (call and channel id)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Channel_Answer(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

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
int Channel_Drop(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

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
int Channel_MediaLoop(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

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
int Channel_Rate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

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
int Channel_FastUpdate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

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
int Channel_ResponseForCLC(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);








/********************************************************************************************
 *
 *  Test Application related
 *
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
    IN const RvChar *widget);





#endif

#ifdef __cplusplus
}
#endif
