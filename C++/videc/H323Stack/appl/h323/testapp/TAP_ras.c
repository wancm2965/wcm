/***********************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
 *                                TAP_ras.c
 *
 * This file contains all the commands that are used for the
 * GRQ/RRQ window of the test application.
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

#include "lepConversions.h"
#include "lepRas.h"

#include "TAP_ras.h"
#include "TAP_general.h"



/******************************************************************************
 * epRegistrationStatus
 * ----------------------------------------------------------------------------
 * General: Indication of a change in the registration status.
 *          This indication is received before the actual event from the Stack
 *          is handled by cmEvRegEvent().
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         newState     - The new registration state.
 * Output: None.
 *****************************************************************************/
void epRegistrationStatus(
    IN EpObj                *ep,
    IN cmRegState           newState)
{
    RV_UNUSED_ARG(ep);

    switch (newState)
    {
        case cmIdle:
            TclExecute("set tmp(status,gk) {Not Registered}");
            break;

        case cmRegistered:
        {
            TclExecute("set tmp(status,gk) Registered");
            break;
        }

        case cmDiscovered:
            TclExecute("set tmp(status,gk) Discovered");
            break;
    }
}





/********************************************************************************************
 * RAS_SendBRQ
 * purpose : Send a BRQ request for more bandwidth
 * syntax  : RAS.SendBRQ <callInfo> <desiredBandwidth>
 *           <callInfo>         - Call information (counter and handle)
 *           <desiredBandwidth> - Amount of bandwidth to request
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int RAS_SendBRQ(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    EpCallObj *call;
    RvUint32 bw;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    /* Make sure we got a call related to the channel */
    if ((argv[1] == NULL) || (strlen(argv[1]) == 0)) return TCL_OK;
    if (sscanf(argv[1], "%*d %p:", &call) != 1) return TCL_OK;

    bw = (RvUint32)atoi(argv[2]);
    epRasSendBRQ(ep, call, bw);

    return TCL_OK;
}


/********************************************************************************************
 * RAS_SendNSM
 * purpose : Send a Non Standard Message
 * syntax  : RAS.SendNSM <nsParam>
 *           <nsParam> - non standard param
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int RAS_SendNSM(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    cmNonStandardParam nsParam;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    String2CMNonStandardParam(argv[1], &nsParam);
    epRasSendNSM(ep, &nsParam);

    return TCL_OK;
}

/********************************************************************************************
 * RAS_SendRAI
 * purpose : Send a Non Standard Message
 * syntax  : RAS.SendRAI <aoor>
 *           <aoor> - boolean, is almost out of resources
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int RAS_SendRAI(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvBool aoor;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    aoor = atoi(argv[1]);
    epRasSendRAI(ep, aoor);

    return TCL_OK;
}


#ifdef __cplusplus
}
#endif

