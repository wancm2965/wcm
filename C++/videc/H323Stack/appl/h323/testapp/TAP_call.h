#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
 *                                call.h
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


#ifndef _CALL_H
#define _CALL_H

#include "TAP_defs.h"


/******************************************************************************
 * epCallNew
 * ----------------------------------------------------------------------------
 * General: Indication of a new call object that was created as is being used.
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
    IN RvBool               isOutgoing);


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
    IN EpCallObj            *call);


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
    IN const RvChar         *eventStr);





/********************************************************************************************
 *
 *  TCL procedures
 *
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
int Call_Info(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


/********************************************************************************************
 * Call_Dial
 * purpose : Dial a new call to a destination.
 * syntax  : Call.Dial <aliases> <remoteAddress>
 *           <callInfo> - Call information (counter and handle)
 *           <aliases>  - Destination aliases for the call
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_Dial(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_Make(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


/********************************************************************************************
 * Call_Drop
 * purpose : Drop the current selected call
 * syntax  : Call.Drop <callInfo> <reason>
 *           <callInfo> - Call information (counter and handle)
 *           <reason>   - Reason string for dropping the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_Drop(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_DropAll(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_SendCallProceeding(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_SendAlerting(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_SendConnect(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_SendStatusInquiry(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_SendProgress(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

/********************************************************************************************
 * Call_SendUserInformation
 * purpose : Send Notify message
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
int Call_SendUserInformation(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_SendNotify(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

/********************************************************************************************
 * Call_CreateH245
 * purpose : Create an H245 session is got ip and port, listen for an H245 connection if not.
 * syntax  : Call.CreateH245 <callInfo> ?<ipAddress>?
 *           <callInfo>  - Call information (counter and handle)
 *           <ipAddress> - Address to connect to. if not present, will listen.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_CreateH245(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_LoopOff(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_ConnectControl(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_SeperateControl(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_CloseControl(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_SendCaps(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_SendEmptyCaps(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_SendCapsAck(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_SendMSD(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_SendRoundTrip(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

/********************************************************************************************
 * Call_UII
 * purpose : Request to send a user input indication.
 * syntax  : Call.SendNotify <callInfo> <non-standard data> <user input>
 *           <callInfo> - Call information (counter and handle)
 *           <non-standard data> - string containing the non-standard data params
 *           <user input> - Input from the user.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_UII(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


/********************************************************************************************
 * Call_FastStartDial
 * purpose : Finishes the Call_Dial process after the user has selected channels.
 * syntax  : Call.FastStart.Dial <callInfo>
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_FastStartDial(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

/********************************************************************************************
 * Call_FastStartMake
 * purpose : Finishes the Call_Make process after the user has selected channels.
 * syntax  : Call.FastStart.Dial <callInfo> <destAddress> (formatted)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_FastStartMake(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_IncompleteAddress(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

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
int Call_AddressComplete(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


/********************************************************************************************
 * Call_SendAdditionalAddr
 * purpose : This procedure is called when the "Send" button is pressed on the redial
 *           window. It sends additional address information to the destination of the call.
 * syntax  : Call.SendAdditionalAddr <callInfo>
 *           <callInfo>   - Call information (counter and handle)
 *           <isARJ>      - RV_TRUE if message sent was ARJ
 *                          RV_FALSE if message sent was SETUP ACK
 *           <digits>     - In case we're dealing with SETUP ACK, we also get the digits
 *           <aliases>    - List of the aliases for the ARJ case
 *           <resaendARQ> - resend ARQ or just send additional address
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
int Call_SendAdditionalAddr(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_OpenOutgoingFaststart(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

/********************************************************************************************
 * Call_BuildOutgoingFaststart
 * purpose : build the faststart message with the new API.
 * syntax  : Call.BuildOutgoingFaststart <callInfo> <followUp>
 *           <callInfo> - Call information (counter and handle)
 *           <followUp> - the command that will be executed after done
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_BuildOutgoingFaststart(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

/********************************************************************************************
 * Call_ApproveFaststart
 * purpose : approve the channels that are going to be opened in faststart
 * syntax  : Call.ApproveFaststart <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_ApproveFaststart(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_RefuseFaststart(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_DropAllFastStart(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/********************************************************************************************
 * Call_SendFacility
 * purpose : Send Facility message
 * syntax  : Call.SendFacility <callInfo>
 *           <callInfo>         - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Call_SendFacility(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int Call_SetRate(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);



/********************************************************************************************
 * Multiplex_Update
 * purpose : This procedure is called when the Update button is pressed on the multiplex frame.
 *           It sets the multiplex and maintain parameters of a call.
 * syntax  : Call.Accept <callInfo>
 *           <callInfo> - Call information (counter and handle)
 * input   : clientData - used for creating new command in tcl
 *         : interp - interpreter for tcl commands
 *         : argc - number of parameters entered to the new command
 *         : argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int Multiplex_Update(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
int EFC_Set(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);


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
    IN const RvChar *stateStr);


/*******************************************************************************************
 * Call_UpdateCounters
 * Description: Maintain the number of current and total calls.
 * Input : instruction  - -1 decrease current
 *                         0 increase current
 *                         1 increase total
 * Output: none
 * return: none
 ******************************************************************************************/
void Call_UpdateCounters(int instruction);



#endif

#ifdef __cplusplus
}
#endif
