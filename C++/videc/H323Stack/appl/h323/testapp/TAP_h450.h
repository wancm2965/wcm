/***********************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
*                                TAP_h450.h
*
* This file contains all the functions which enable the use of h450 functions. The functions
* are activated either in the H450 tab in the TestApp, of in the incoming call window (only
* some may be thus activated).
*
*
*      Written by                        Version & Date                        Change
*     ------------                       ---------------                      --------
*       Ran Arad                          18-Dec-2000
*
********************************************************************************************/


#ifndef _TAP_H450_H
#define _TAP_H450_H

#include "TAP_general.h"
#include "TAP_defs.h"
#include "TAP_call.h"

#ifdef USE_H450

#include "sse.h"
#include "h450.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* General
 ********************************/
int H450_callMake(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Call Transfer - H450.2
 ********************************/

/* Initiates Call Transfer service, called by the TCL */
int H450_callTransfer(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Send an asynchronous response on call transfer service, called by the TCL */
int H450_TransferNonDefaultResponse(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Call Reroute - H450.3
 ********************************/

/* Initiates Call Reroute (deflect) service, called by the TCL */
int H450_callReroute(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Initiates/deactivates Forwarding service, called by the TCL */
int H450_forwardRequest(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Interrogates server as to Forwarding service, called by the TCL */
int H450_forwardInterrogate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Call Hold - H450.4
 ********************************/

/* Initiates Call Hold service, called by the TCL */
int H450_callHold(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Retrieves a call from Hold, called by the TCL */
int H450_callHoldRtrv(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Answers a Remote Hold request, called by the TCL */
int H450_HoldSendNonDefaultResponse(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Park Pickup - H450.5
 ********************************/

/* Initiates Parking service, called by the TCL */
int H450_callPark(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Initiates Pickup service, called by the TCL - does not work yet */
int H450_callPick(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Call Waiting - H450.6
 ********************************/

/* Initiates Call Waiting service, called by the TCL */
int H450_callWait(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Message Waiting Indication - H450.7
 *******************************************/

/* Sends Message Indication to the Served User */
int H450_MC_Activate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Sends Interogation Message to the Message Center */
int H450_SU_Interogate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* General function for sending non deafult response to MWI messages */
int H450_MWISendNonDefaultResponse(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Call Completion - H450.9
 ********************************/

/* Sends a CallCompletion Setup message */
int H450_callCompletion(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/* Sends a CallCompletion message other than Setup */
int H450_callCompletionAction(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Call Offer - H450.10
 ********************************/

/* Sends a CallOffer Setup message */
int H450_callOffer(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

/*  */
int H450_remoteUserAlerting(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Call Intrusion - H450.11
 ********************************/

int H450_getCallID(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);

int H450_callIntrusion(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);


/* Common Information - H450.12
 *******************************************/

/* Sends a Common Information message on the selected call */
int H450_CMN_Activate(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);





#ifdef __cplusplus
}
#endif

#endif /*_TAP_H450_H*/
