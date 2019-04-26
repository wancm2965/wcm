/***********************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
 *                                TAP_wrapper.c
 *
 * This file contains all the functions that are used for
 * wrapping the CM api functions so we can use them as tcl commands.
 * This is done for writing scripts in tcl that have specific scenarios.
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
#include "stkutils.h"
#include "msg.h"
#include "q931asn1.h"

#include "lep.h"
#include "lepH450.h"
#include "lepRas.h"
#include "lepCall.h"
#include "lepControl.h"
#include "lepChannel.h"
#include "lepConversions.h"

#include "TAP_general.h"
#include "TAP_call.h"
#include "TAP_channel.h"
#include "TAP_wrapper.h"


RvBool LogWrappers = RV_FALSE;


/********************************************************************************************
*
*  General Functions
*
*********************************************************************************************/


/********************************************************************************************
 * WrapperEnter
 * purpose : Log wrapper function on entry
 * input   : interp     - Interpreter to use
 *           ep         - Endpoint object used.
 *           argc       - Number of arguments
 *           argv       - Argument values
 * output  : none
 * return  : TCL_OK or TCL_ERROR
 ********************************************************************************************/
int WrapperEnter(Tcl_Interp* interp, EpObj *ep, int argc, const char *argv[])
{
    if (LogWrappers)
    {
        char buf[4096];
        char* ptr = buf+7;
        int i;
        strcpy(buf, "Enter: ");

        for (i = 0; i < argc; i++)
        {
            strcpy(ptr, argv[i]);
            ptr += strlen(ptr);
            *ptr = ' ';
            ptr++;
        }
        *ptr = '\0';

        cmLogMessage(ep->hApp, buf);
    }

    Tcl_SetResult(interp, NULL, TCL_STATIC);
    return TCL_OK;
}


/********************************************************************************************
 * WrapperExit
 * purpose : Log wrapper function on exit
 * input   : interp     - Interpreter to use
 *           ep         - Endpoint object used.
 *           argc       - Number of arguments
 *           argv       - Argument values
 * output  : none
 * return  : TCL_OK or TCL_ERROR
 ********************************************************************************************/
int WrapperExit(Tcl_Interp* interp, EpObj *ep, int argc, const char *argv[])
{
    if (LogWrappers)
    {
        char buf[4096];
        char* ptr = buf+6;
        int i;
        strcpy(buf, "Exit: ");

        for (i = 0; i < argc; i++)
        {
            strcpy(ptr, argv[i]);
            ptr += strlen(ptr);
            *ptr = ' ';
            ptr++;
        }
        *ptr = '\0';

        if (strlen(Tcl_GetStringResult(interp)) != 0)
            sprintf(ptr, " (%s)", Tcl_GetStringResult(interp));
        cmLogMessage(ep->hApp, buf);
    }

    return TCL_OK;
}


/********************************************************************************************
 * WrapperBadParams
 * purpose : Return an error result due to bad function parameters
 * input   : interp     - Interperter to use
 *           usage      - The right parameters
 * output  : none
 * return  : TCL_OK or TCL_ERROR
 ********************************************************************************************/
int WrapperBadParams(Tcl_Interp* interp, const char* usage)
{
    char buf[1024];
    sprintf(buf, "Bad parameters given. Usage: %s", usage);
    Tcl_SetResult(interp, buf, TCL_VOLATILE);
    return TCL_ERROR;
}


/********************************************************************************************
 * WrapperReply
 * purpose : Create and use the return result from the API and sent it back to TCL
 * input   : interp     - Interpreter to use
 *           status     - Return status got from API
 * output  : none
 * return  : TCL_OK or TCL_ERROR
 ********************************************************************************************/
int WrapperReply(Tcl_Interp* interp, int status, int argc, char *argv[])
{
    char buf[4096];
    char* ptr = buf;
    int i;
    Tcl_CmdInfo info;

    if (status >= 0)
        return TCL_OK;

    if (Tcl_GetCommandInfo(interp, (char *)"cb:app:Error", &info) == 0)
    {
        sprintf(buf, "Error executing %s - %s",
                argv[0], Status2String(status));
        /* Error command doesn't exist - we can throw an exception */
        Tcl_SetResult(interp, buf, TCL_VOLATILE);
        return TCL_ERROR;
    }

    /* We have to call the error function */
    for (i = 0; i < argc; i++)
    {
        strcpy(ptr, argv[i]);
        ptr += strlen(ptr);
        *ptr = ' ';
        ptr++;
    }
    *ptr = '\0';

    TclExecute("cb:app:Error {%s} {%s}",
               buf,
               Status2String(status));

    Tcl_SetResult(interp, (char *)"", TCL_STATIC);
    return TCL_OK;
}


/********************************************************************************************
 * WrapperFunc
 * purpose : wrapping stack functions - this function is called whenever we wrap a
 *           function for the scripts' use
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int WrapperFunc(ClientData clientData, Tcl_Interp *interp,int argc, const char *argv[])
{
    TclWrapperData  *wData = (TclWrapperData *)clientData;
    int             result = TCL_ERROR;

    WrapperEnter(interp, wData->ep, argc, argv);

    if (wData->proc != NULL)
        result = wData->proc((ClientData)wData->ep, interp, argc, argv);

    WrapperExit(interp, wData->ep, argc, argv);

    return result;
}


/********************************************************************************************
 * CallbackFunc
 * purpose : wrapping callback functions - this function is called whenever we wrap a
 *           callback function for the scripts' use
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int CallbackFunc(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    char* spacer;
    int result;
    Tcl_CmdInfo info;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
    {
        PutError(argv[0], "Callback wrapper called with too many arguments");
    }

    spacer = (char *)strchr(argv[1], ' ');

    if (spacer != NULL) *spacer = '\0';
    result = Tcl_GetCommandInfo(interp, argv[1], &info);
    *spacer = ' ';

    if (result == 0)
    {
        /* Callback command doesn't exist - skip it */
        return TCL_OK;
    }

    result = TclExecute(argv[1]);
    if (result == TCL_OK)
        Tcl_SetResult(interp, NULL, TCL_STATIC);

    return result;
}






/********************************************************************************************
*
* Generic api
*
*********************************************************************************************/


/********************************************************************************************
 * api_cm_GetVersion
 * purpose : wrapping the original CM api function - GetVersion
 * syntax  : api:cm:GetVersion
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_GetVersion(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    char* ver;

    RV_UNUSED_ARG(clientData);

    ver = cmGetVersion();
    if (ver != NULL)
        Tcl_SetResult(interp, ver, TCL_VOLATILE);

    return WrapperReply(interp, 0, argc, argv);
}

/********************************************************************************************
 * api_cm_GetInterfaces
 * purpose : wrapping the original CM api function - GetInterfaces (if there was one...)
 * syntax  : api:cm:GetInterfaces
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
RVCOREAPI const RvChar * RVCALLCONV RvCCoreInterfaces(void);
int api_cm_GetInterfaces(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    const RvChar    *interfaces;

    RV_UNUSED_ARG(clientData);

    interfaces = RvCCoreInterfaces();
    if (interfaces != NULL)
    {
        RvChar  *s = (RvChar *)interfaces;
        Tcl_Obj *lst = Tcl_NewListObj(0, NULL);

        while ((*s) != '\0')
        {
            Tcl_ListObjAppendElement(interp, lst, Tcl_NewStringObj(s+4, -1));
            s += strlen(s) + 1;
        }

        Tcl_SetObjResult(interp, lst);
    }

    return WrapperReply(interp, 0, argc, argv);
}

/********************************************************************************************
 * api_cm_Stop
 * purpose : wrapping the original CM api function - cmStop
 * syntax  : api:cm:Stop
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_Stop(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    int status;

    status = epStop(ep);

    if (status >= 0)
        TclExecute("test:SetStackStatus Stopped");

    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_Start
 * purpose : wrapping the original CM api function - cmStart
 * syntax  : api:cm:Start
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_Start(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    int status;

    status = epStart(ep);

    if (status >= 0)
        TclExecute("test:SetStackStatus Running;test:updateGui 1");

    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_LogMessage
 * purpose : wrapping the original CM api function - cmLogMessage
 * syntax  : api:cm:LogMessage <msg>
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_LogMessage(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:LogMessage <msg>");

    cmLogMessage(ep->hApp, argv[1]);

    return WrapperReply(interp, 0, argc, argv);
}

/********************************************************************************************
 * api_cm_Encode
 * purpose : wrapping the original CM api function - cmEmEncode
 * syntax  : api:cm:Encode <hVal> <nodeId>
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_Encode(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HPVT    hVal;
    RvInt32 nodeId;
    int     encBytes, status;
    RvUint8 buffer[2048], result[4096];

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:cm:Encode <valTree> <nodeId>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = atoi(argv[2]);

    if (nodeId >= 0)
    {
        status = cmEmEncode(hVal, nodeId, buffer, sizeof(buffer), &encBytes);
        if (status >= 0)
        {
            str2hex(buffer, (char*)result, encBytes);
            Tcl_SetResult(interp, (char*)result, TCL_VOLATILE);
        }
    }
    else
        status = nodeId;

    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_Decode
 * purpose : wrapping the original CM api function - cmEmDecode
 * syntax  : api:cm:Decode <hVal> <nodeId> <buffer>
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_Decode(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HPVT    hVal;
    RvInt32 nodeId;
    int     status, len;
    RvUint8 buffer[2048];

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:cm:Decode <valTree> <nodeId> <buffer>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = atoi(argv[2]);

    if (nodeId >= 0)
    {
        len = hex2str(argv[3], buffer, sizeof(buffer));
        status = cmEmDecode(hVal, nodeId, buffer, len, &len);
    }
    else
        status = nodeId;

    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
*
* Call api
*
*********************************************************************************************/



/********************************************************************************************
 * api_cm_CallNew
 * purpose : wrapping the original CM api function - CallNew
 * syntax  : api:cm:CallNew
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallNew(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    EpCallObj   *call;
    char        returnedHandle[20];

    if (argc != 1)
        return WrapperBadParams(interp, "api:cm:CallNew");

    /* Create a new call */
    call = epCallCreate(ep, NULL, 0);
    if (call == NULL)
        return WrapperReply(interp, RV_ERROR_UNKNOWN, argc, argv);

    call->isIndependent = RV_TRUE;
    epCallColorIt(call);

    sprintf(returnedHandle, "%p", call);

    if (LogWrappers)
    {
        char buf[100];
        sprintf(buf, "Call: app=%p ==> cm=%p", call, call->hsCall);
        cmLogMessage(ep->hApp, buf);
    }

    Tcl_SetResult(interp, returnedHandle, TCL_VOLATILE);
    return WrapperReply(interp, 0, argc, argv);
}


/********************************************************************************************
 * api_cm_CallDial
 * purpose : wrapping the original CM api function - CallDial
 * syntax  : api:cm:CallDial <handle>
 *           <handle>         - handle of the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallDial(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    int       retVal;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:CallDial <callHandle>");

    sscanf(argv[1], "%p", &call);

    retVal = cmCallDial(call->hsCall);

    return WrapperReply(interp, retVal, argc, argv);
}


/********************************************************************************************
 * api_cm_CallProceeding
 * purpose : wrapping the original CM api function - CallProceeding
 * syntax  : api:cm:CallProceeding <handle>
 *           <handle>         - handle of the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallProceeding(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    int       retVal;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:CallProceeding <callHandle>");

    sscanf(argv[1], "%p", &call);

    retVal = epCallProceeding(call, NULL);

    return WrapperReply(interp, retVal, argc, argv);
}

/********************************************************************************************
 * api_cm_CallAccept
 * purpose : wrapping the original CM api function - CallAccept
 * syntax  : api:cm:CallAccept <handle>
 *           <handle>         - handle of the call

 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallAccept(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    int retVal;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:CallAccept <callHandle> ");

    sscanf(argv[1], "%p", &call);

    retVal = epCallAlerting(call, NULL);

    return WrapperReply(interp, retVal, argc, argv);
}


/********************************************************************************************
 * api_cm_CallAnswer
 * purpose : wrapping the original CM api function - CallAnswer
 * syntax  : api:cm:CallAnswer <handle>
 *           <handle>         - handle of the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallAnswer(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    int       retVal;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:CallAnswer <callHandle>");

    sscanf(argv[1], "%p", &call);

    retVal = epCallConnect(call, NULL, NULL, NULL);

    return WrapperReply(interp, retVal, argc, argv);
}


/********************************************************************************************
 * api_cm_CallClose
 * purpose : wrapping the original CM api function - CallClose
 * syntax  : api:cm:CallClose <handle>
 *           <handle>         - handle of the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallClose(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int status = RV_OK;
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:CallClose <callHandle>");

    sscanf(argv[1], "%p", &call);

    epCallDestruct(call);

    return WrapperReply(interp, status, argc, argv);
}




/********************************************************************************************
 * api_cm_CallDrop
 * purpose : wrapping the original CM api function - CallDrop
 * syntax  : api:cm:CallDrop <handle>
 *           <handle>         - handle of the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallDrop(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    int         retVal;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:CallDrop <callHandle> ");

    if(sscanf(argv[1], "%p", &call) != 1) return TCL_OK;

    retVal = epCallDrop(call, (cmReasonType)-1, NULL);

    return WrapperReply(interp, retVal, argc, argv);
}


/********************************************************************************************
 * api_cm_CallSetParam
 * purpose : wrapping the original CM api function - CallSetParam
 * syntax  : api:cm:CallSetParam <handle> <RV323_CM_CallParam> <str>
 *         : <handle>         - handle of the call
 *         : <RV323_CM_CallParam>    - determine which call param
 *         : <RV_BOOL>            - for boolean parameters
 *         : <str>                - display or UserUser or TransportAddress or
 *                                      RV323_CM_CallParamCallState
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallSetParam(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj               *ep = (EpObj *)clientData;
    cmCallParam         callParam;
    int                 intValue;
    EpCallObj           *call;
    int                 status = 0;
    cmTransportAddress  addr;
    cmAlias             alias;

    if (argc < 4)
        return WrapperBadParams(interp, "api:cm:CallSetParam <callHandle> <callParam> <value> [<index> <isNewAlias>]");

    sscanf(argv[1], "%p", &call);
    callParam = String2CMCallParam(argv[2]);

    switch (callParam)
    {
        case cmParamEarlyH245:
        case cmParamH245Tunneling:
        case cmParamH245Parallel:
        case cmParamCanOverlapSending:
        case cmParamSetupCanOverlapSending:
        case cmParamEstablishH245:
        case cmParamRate:
        case cmParamIsMultiplexed:
        case cmParamSetupSendingComplete:
        case cmParamAutomaticRAS:
            {
                intValue = atoi(argv[3]);
                status = cmCallSetParam(call->hsCall, callParam, 0, intValue, NULL);
                break;
            }
        case cmParamDisplay:
        case cmParamUserUser:
            {
                status = cmCallSetParam(call->hsCall, callParam, 0, strlen(argv[3]), argv[3]);
                break;
            }
        case cmParamDestCallSignalAddress:
        case cmParamDestinationIpAddress:
        case cmParamDestinationAnnexEAddress:
            {
                /* building Address */
                cmString2TransportAddress(argv[3], &addr);
                status = cmCallSetParam(call->hsCall, callParam, 0, sizeof(addr), (char *)&addr);
                break;
            }
        case cmParamDestinationAddress:
        case cmParamSourceAddress:
        case cmParamConnectedAddress:
        case cmParamCallingPartyNumber:
        case cmParamCalledPartyNumber:
            {
                int index;
                if (argc != 5)
                    return WrapperBadParams(interp, "api:cm:CallSetParam <callHandle> <callParam> <value> <index>");

                alias.string = NULL;
                String2Alias(ep, argv[3], &alias);
                index = atoi(argv[4]);
                status = cmCallSetParam(call->hsCall, callParam, index, sizeof(alias), (char *)&alias);
                FreeAlias(ep, &alias);
                break;
            }
        case cmParamH245Stage:
            {
                status = cmCallSetParam(call->hsCall, callParam, 0, String2CMH245Stage(argv[3]), NULL);
                break;
            }
        case cmParamAnnexE:
            {
                status = cmCallSetParam(call->hsCall, callParam, 0, String2CMAnnexEUsageMode(argv[3]), NULL);
                break;
            }
        case cmParamExtendedFastConnectState:
            {
                status = cmCallSetParam(call->hsCall, callParam, 0, String2CMExtendedFastConnectState(argv[3]), NULL);
                break;
            }
        default: break;
    }

    /* TODO : add more cases to the switch */
    return WrapperReply(interp, status, argc, argv);
}



/********************************************************************************************
 * api_cm_CallGetParam
 * purpose : wrapping the original CM api function - cmCallGetParam
 * syntax  : api:cm:CallGetParam <handle> <callParam> <index>
 *         : <handle>       - handle of the call
 *         : <callParam>    - determine which call param
 *         : <index>        - index of the parameter
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallGetParam(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int                 status = RV_ERROR_UNKNOWN;
    int                 intValue;
    EpCallObj           *call;
    cmCallParam         callParam;
    char                *returnedValue;
    char                data[1024];

    RV_UNUSED_ARG(clientData);

    if ((argc < 3) || (argc > 4))
       return WrapperBadParams(interp, "api:cm:CallGetParam <callHandle> <callParam> [<index>]");

    if (sscanf(argv[1],"%p",&call) != 1) return TCL_ERROR;

    callParam = String2CMCallParam(argv[2]);
    returnedValue = data;

    switch (callParam)
    {
    case cmParamEarlyH245:
    case cmParamH245Tunneling:
    case cmParamH245Parallel:
    case cmParamCanOverlapSending:
    case cmParamSetupCanOverlapSending:
    case cmParamEstablishH245:
    case cmParamRate:
    case cmParamIsMultiplexed:
    case cmParamSetupSendingComplete:
    case cmParamAutomaticRAS:
        {
            status = cmCallGetParam(call->hsCall, callParam, 0, &intValue, (char *) NULL);
            sprintf(returnedValue, "%d", (int)intValue);
            break;
        }
    case cmParamDisplay:
    case cmParamUserUser:
        {
            intValue = sizeof(data);
            status = cmCallGetParam(call->hsCall, callParam, 0, &intValue, data);
            break;
        }
    case cmParamDestCallSignalAddress:
    case cmParamDestinationIpAddress:
    case cmParamDestinationAnnexEAddress:
        {
            cmTransportAddress addr;
            intValue = sizeof(addr);
            status = cmCallGetParam(call->hsCall, callParam, 0, &intValue, (char *) &addr);
            /* building Address */
            cmTransportAddress2String(&addr, sizeof(data), data);
            break;
        }
    case cmParamDestinationAddress:
    case cmParamSourceAddress:
    case cmParamConnectedAddress:
        {
            int index;
            cmAlias alias;
            char string[256];

            if (argc != 4)
                return WrapperBadParams(interp, "api:cm:CallGetParam <callHandle> <callParam> <index>");

            index = atoi(argv[3]);
            alias.string = string;
            alias.length = sizeof(string);
            intValue = sizeof(alias);
            status = cmCallGetParam(call->hsCall, callParam, index, &intValue, (char *) &alias);
            Alias2String(&alias, data);
            break;
        }
    case cmParamH245Stage:
        {
            status = cmCallGetParam(call->hsCall, callParam, 0, &intValue, (char *) NULL);
            returnedValue = (char *)CMH245Stage2String((cmH245Stage)intValue);
            break;
        }
    case cmParamAnnexE:
        {
            status = cmCallGetParam(call->hsCall, callParam, 0, &intValue, (char *) NULL);
            returnedValue = (char *)CMAnnexEUsageMode2String((cmAnnexEUsageMode)intValue);
            break;
        }
    case cmParamExtendedFastConnectState:
        {
            status = cmCallGetParam(call->hsCall, callParam, 0, &intValue, (char *) NULL);
            returnedValue = (char *)CMExtendedFastConnectState2String((cmExtendedFastConnectState_e) intValue);
            break;
        }
    case cmParamCallID:
    case cmParamCID:
    case cmParamFacilityCID:
        {
            RvInt i;
            RvChar callId[16];
            intValue = (int)sizeof(callId);
            status = cmCallGetParam(call->hsCall, callParam, 0, &intValue, (RvChar *)callId);
            if (status == RV_OK)
            {
                for (i = 0; i < 16; i++)
                    sprintf(returnedValue+2*i, "%02X", (unsigned char)callId[i]);
                returnedValue[32] = 0;
            }
            break;
        }
    default: break;
    }

    /* todo: add some cases */
    if (status >= 0)
        Tcl_SetResult(interp, returnedValue, TCL_VOLATILE);
    return WrapperReply(interp, status, argc, argv);
}



/***********************************************************************************
 * api_cm_CallForward
 * purpose: wrapping the original CM api function - cmCallForward
 * syntax : api:cm:CallForward <handle> <destAddress>
 *        : <handle>      - handle of the call
 *        : <destAddress> - string of the destination adrress
 * input  : clientData - used for creating new command in tcl
 *          interp - interpreter for tcl commands
 *          argc - number of parameters entered to the new command
 *          argv - the parameters entered to the tcl command
 * output : none
 * return : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ***********************************************************************************/
int api_cm_CallForward(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int status;
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &call);

    status = epCallFacility(call, cmReasonTypeUndefinedReason, argv[2], NULL, EpCallFacilityForward);

    return WrapperReply(interp, status, argc, argv);
}



/***********************************************************************************
 * api_cm_CallStatusInquiry
 * purpose: wrapping the original CM api function - CallStatusInquiry
 * syntax : api:cm:CallStatusInquiry <handle> <DisplayInfo>
 *        : <handle>         - handle of the call
 *        : <DisplayInfo>    - Display Info string
 * input  : clientData - used for creating new command in tcl
 *          interp      - interpreter for tcl commands
 *          argc - number of parameters entered to the new command
 *          argv - the parameters entered to the tcl command
 * output : none
 * return : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ***********************************************************************************/
int api_cm_CallStatusInquiry(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj   *call;
    int         status;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &call);

    status = epCallStatusEnquiry(call, argv[2]);

    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
 * api_cm_CallConnectControl
 * purpose : wrapping the original CM api function - CallConnectControl
 * syntax  : api:cm:CallConnectControl <handle>
 *           <handle>                  - handle of the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallConnectControl(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int       status;
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &call);

    status = epCallConnectControl(call);

    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
*
* H245 api
*
*********************************************************************************************/

/********************************************************************************************
 * api_cm_H245DeleteCapabilityMessage
 * purpose : wrapping the original CM api function - cmFreeCapability
 * syntax  : api:cm:H245DeleteCapabilityMessage <callHandle>
 *           <callHandle>   - app handle of the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_H245DeleteCapabilityMessage(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int       status;
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &call);

    status = cmFreeCapability(call->hsCall);
    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
*
* Channel api
*
*********************************************************************************************/




/********************************************************************************************
 * api_cm_ChannelNew
 * purpose : wrapping the original CM api function - ChannelCreate
 * syntax  : api:cm:ChannelNew <CallHandle>
 *           <CallHandle>         - handle of the call
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelNew(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status = RV_ERROR_UNKNOWN;
    EpChannelObj    *channel;
    EpCallObj       *call;
    char            returnedHandle[20];

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:ChannelNew <callHandle> ");

    sscanf(argv[1], "%p", &call);

    channel = epChannelCreate(call->ep, call, 0);

    if (channel != NULL)
    {
#if 0
        LOG_Print(wrapperLog, RV_INFO,
                  (wrapperLog, RV_INFO,
                  "Channel: app=%p ==> cm=%p",
                  CurrentChannel,
                  CurrentChannel->handle));
#endif
        sprintf(returnedHandle, "%p", channel);
        Tcl_SetResult(interp, returnedHandle, TCL_VOLATILE);
        status = RV_OK;
    }
    return WrapperReply(interp, status, argc, argv);
}



/********************************************************************************************
 * api_cm_ChannelOpen
 * purpose : wrapping the original CM api function - ChannelOpen
 * syntax  : api:cm:ChannelOpen <CallHandle> <ChannelHandle>
 *                <MimmicedCahnnel> <dataType>
 *           <CallHandle>     - application handle of the Call
 *           <ChannelHandle>  - app handle of the channel
 *           <MimmicedCahnnel> - app handle of the channel mimiced
 *           <MediaType> - Media type of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelOpen(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status = 0;
    EpCallObj       *call = NULL;
    EpChannelObj    *curChan = NULL;
    EpChannelObj    *mimicChan = NULL;
    char            *dataType;

    RV_UNUSED_ARG(clientData);

    if (argc < 5)
        return WrapperBadParams(interp, "api:cm:ChannelOpen <callHandle> <ChannelHandle> <MimmicedChannel> <dataType>");

    sscanf(argv[1], "%p", &call);
    sscanf(argv[2], "%p", &curChan);
    sscanf(argv[3], "%p", &mimicChan);

    dataType = argv[4];

    status = epChannelOpen(curChan, dataType, mimicChan, NULL, 0);

    /* We do nothing if cmChannelOpen() fails since such a failure also changes the state of
       the channel to disconnected and idle internally. */

    return WrapperReply(interp, status, argc, argv);
}



/********************************************************************************************
 * api_cm_ChannelAnswer
 * purpose : wrapping the original CM api function - ChannelAnswer
 * syntax  : api:cm:ChannelAnswer <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelAnswer(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    status = cmChannelAnswer(chan->hsChan);

    return WrapperReply(interp, status, argc, argv);
}



/********************************************************************************************
 * api_cm_ChannelDrop
 * purpose : wrapping the original CM api function - ChannelDrop
 * syntax  : api:cm:ChannelDrop <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelDrop(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    status = cmChannelDrop(chan->hsChan);

    return WrapperReply(interp, status, argc, argv);
}



/********************************************************************************************
 * api_cm_ChannelRequestCloseReject
 * purpose : wrapping the original CM api function - cmChannelRequestCloseReject
 * syntax  : api:cm:ChannelRequestCloseReject <ChannelHandle>
 *           <ChannelHandle> - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelRequestCloseReject(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    status = cmChannelRequestCloseReject(chan->hsChan);

    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
 * api_cm_ChannelClose
 * purpose : wrapping the original CM api function - ChannelClose
 * syntax  : api:cm:ChannelClose <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelClose(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* don't forget that the drop reason is hard coded */
    if (chan->keepAliveTimer != HTI_NULL_TIMER)
        status = mtimerReset(chan->call->ep->hTimers, chan->keepAliveTimer);
    status = cmChannelClose(chan->hsChan);

    chan->hsChan = NULL;

    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
 * api_cm_ChannelGetCallHandle
 * purpose : wrapping the original CM api function - cmChannelGetCallHandles
 * syntax  : api:cm:ChannelGetCallHandle <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelGetCallHandle(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;
    HCALL           hsCall;
    HAPPCALL        appCall;
    char            handle[32];

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* get the call handles */
    status = cmChannelGetCallHandles(chan->hsChan, &hsCall, &appCall);

    if (status >= 0)
    {
        sprintf(handle, "%p", appCall);
        Tcl_SetResult(interp, handle, TCL_VOLATILE);
    }
    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_ChannelGetDependency
 * purpose : wrapping the original CM api function - cmChannelGetDependency
 * syntax  : api:cm:ChannelGetDependency <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelGetDependency(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;
    HCHAN           hschan;
    HAPPCHAN        appchan;
    char            handle[32];

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* get the channel handles */
    status = cmChannelGetDependency(chan->hsChan, &appchan, &hschan);

    if (status >= 0)
    {
        sprintf(handle, "%p", appchan);
        Tcl_SetResult(interp, handle, TCL_VOLATILE);
    }
    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_ChannelGetDuplexAddress
 * purpose : wrapping the original CM api function - cmChannelGetDuplexAddress
 * syntax  : api:cm:ChannelGetDuplexAddress <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelGetDuplexAddress(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int                 status;
    EpChannelObj        *chan;
    cmTransportAddress  address;
    char                addrStr[32];
    char                extRef[64];
    RvBool              isAssociated;
    char                result[128];

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* get the address of a bi - directional channel */
    status = cmChannelGetDuplexAddress(chan->hsChan, &address, 64, extRef, &isAssociated);
    cmTransportAddress2String(&address, sizeof(addrStr), addrStr);

    if (status >= 0)
    {
        sprintf(result, "{ {%s} {%s} {%d} }", addrStr, extRef, isAssociated);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }
    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_ChannelGetNumber
 * purpose : wrapping the original CM api function - cmChannelGetNumber
 * syntax  : api:cm:ChannelGetNumber <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelGetNumber(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;
    char            result[32];

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* get the channel number and convert it to string */
    status = cmChannelGetNumber(chan->hsChan);

    if (status >= 0)
    {
        sprintf(result, "%d", status);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }
    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_ChannelGetOrigin
 * purpose : wrapping the original CM api function - cmChannelGetOrigin
 * syntax  : api:cm:ChannelGetOrigin <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelGetOrigin(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;
    RvBool          origin;
    char            result[32];

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* get the origin of the channel */
    status = cmChannelGetOrigin(chan->hsChan, &origin);

    if (status >= 0)
    {
        sprintf(result, "%d", origin);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }
    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_ChannelGetSource
 * purpose : wrapping the original CM api function - cmChannelGetSource
 * syntax  : api:cm:ChannelGetSource <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelGetSource(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;
    cmTerminalLabel label;
    char            result[32];

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* get the origin of the channel */
    status = cmChannelGetSource(chan->hsChan, &label);

    if (status >= 0)
    {
        sprintf(result, "{ {%d} {%d} }", label.mcuNumber, label.terminalNumber);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }
    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_ChannelIsDuplex
 * purpose : wrapping the original CM api function - cmChannelIsDuplex
 * syntax  : api:cm:ChannelIsDuplex <ChannelHandle>
 *           <ChannelHandle>     - cm handle of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_ChannelIsDuplex(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;
    char            result[4];

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* find out if the channel is bi - directional */
    status = cmChannelIsDuplex(chan->hsChan);

    if (status >= 0)
    {
        sprintf(result, "%d", status);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}







/********************************************************************************************
*
* Application api
*
*********************************************************************************************/

/********************************************************************************************
 * api_app_SetCallMode
 * purpose : wrapping an application script function - SetCallMode
 * syntax  : api:app:SetCallMode <callhandle> <mode>
 *           <callhandle>   - handle of the call.
 *           <mode>         - Script or Normal
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_SetCallMode(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj       *call;
    EpChannelObj    *chan;
    RvBool          isScript;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    sscanf(argv[1], "%p", &call);

    if (tolower(argv[2][0]) == 's')
        isScript = RV_TRUE;
    else
        isScript = RV_FALSE;

    call->isIndependent = isScript;
    chan = call->firstChannel;
    while (chan != NULL)
    {
        chan->isIndependent = isScript;
        chan = chan->nextChannel;
    }

    return TCL_OK;
}




/********************************************************************************************
 * api_app_SetChannelMode
 * purpose : wrapping an application script function - SetChannelMode
 * syntax  : api:app:SetChannelMode <channelhandle> <mode>
 *           <channelhandle>    - handle of the channel.
 *           <mode>             - Script or Normal
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_SetChannelMode(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpChannelObj    *chan;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);

    sscanf(argv[1], "%p", &chan);

    if (tolower(argv[2][0]) == 's')
        chan->isIndependent = RV_TRUE;
    else
        chan->isIndependent = RV_FALSE;

    return TCL_OK;
}


/********************************************************************************************
 * api_app_GetDataTypes
 * purpose : wrapping an application script function - GetDataTypes
 * syntax  : api:app:GetDataTypes
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_GetDataTypes(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    int numChannels;
    char *DataTypeNameList[20], DataTypeNames[20][20];
    char TclDataType[20 * 21], *temp;
    int i;

    /* preparing DataTypeNameList */
    for (i = 0; i < 20; i++)
        DataTypeNameList[i] = DataTypeNames[i];

    /* get DataType name list from configuration file */
    numChannels = cmGetConfigChannels(ep->hApp, 20, 20, DataTypeNameList);
    if (numChannels >= 0)
    {
        /*preparing string of DataTypes for the combo box in the fast start window*/
        strcpy(TclDataType, DataTypeNameList[0]);
        temp = TclDataType;

        if (numChannels > 20) numChannels = 20;
        for(i = 0; i < numChannels; i++)
            temp += sprintf(temp, "%s ", DataTypeNameList[i]);

        Tcl_SetResult(interp, TclDataType, TCL_VOLATILE);
    }
    else
    {
        Tcl_SetResult(interp, (char *)"", TCL_STATIC);
        numChannels = 0;
    }

    return WrapperReply(interp, numChannels, argc, argv);
}


/********************************************************************************************
 * api_app_ChannelKill
 * purpose : wrapping an application script function - ChannelKill
 * syntax  : api:app:ChannelKill <channelHandle>
 *           <channelHandle>    - Channel handle to kill
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_ChannelKill(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int             status;
    EpChannelObj    *chan;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &chan);

    /* Drop this as if it was Idle... */
    cmEvChannelStateChanged((HAPPCHAN)chan, chan->hsChan,
        cmChannelStateIdle,
        (cmChannelStateMode_e)0);
    status = 0;
    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
 * api_app_GetChannelList
 * purpose : Get the list of channels of certain call
 * syntax  : api:app:GetChannelList <Call>
 *           <Call> - the struct that holds the call parameters
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int api_app_GetChannelList(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpChannelObj    *chan;
    EpCallObj       *call;
    char            *temp, chanList[2048];
    int             args;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);

    args = sscanf(argv[1], "%p", &call);
    if (args < 0)
        return TCL_OK;

    /* Rewrite the channels information inside the list */
    temp = chanList;
    chan = call->firstChannel;
    /* if the list is empty */
    if (chan == NULL)
    {
        Tcl_SetResult(interp, (char *)"", TCL_VOLATILE);
        return TCL_OK;
    }

    while (chan != NULL)
    {
        if (chan->isOutgoing)
        {
            sprintf(temp, "%p ", chan->hsChan);
            temp = temp + strlen(temp);
        }
        chan = chan->nextChannel;

    }

     Tcl_SetResult(interp, chanList, TCL_VOLATILE);

    return TCL_OK;
}


/************************************************************************
 * api_app_Vt2Address
 * purpose : Convert a node ID of a TransportAddress to a string
 * syntax  : api:app:Vt2Address <valTree> <nodeId>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - TransportAddress type node id
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Address string
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_app_Vt2Address(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    cmTransportAddress  addr;
    HPVT                hVal;
    RvPvtNodeId         nodeId;
    int                 status = 0;
    char                result[256];

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:app:Vt2Address <valTree> <nodeId>");

    memset(&addr, 0, sizeof(addr));

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = atoi(argv[2]);

    if (nodeId < 0)
        status = RV_ERROR_UNKNOWN;
    else
    {
        status = cmVt2TransportAddress(hVal, nodeId, &addr);
    }

    if (status >= 0)
    {
        cmTransportAddress2String(&addr, sizeof(result), result);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_app_Address2Vt
 * purpose : Convert a TransportAddress to a node id
 * syntax  : api:app:Address2Vt <valTree> <address> <nodeId>
 *           <valTree>  - Value tree to use
 *           <address>  - Address string to convert
 *           <nodeId>   - TransportAddress type node id
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Address string
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_app_Address2Vt(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    cmTransportAddress  addr;
    HPVT                hVal;
    RvInt32             nodeId;
    int                 status;

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:app:Address2Vt <valTree> <address> <nodeId>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = atoi(argv[3]);

    status = cmString2TransportAddress(argv[2], &addr);

    if ((status < 0) || (nodeId < 0) ||
        ((addr.type != cmTransportTypeIP) && (addr.type != cmTransportTypeIPv6)))
        status = RV_ERROR_UNKNOWN;
    else
    {
        status = cmTransportAddress2Vt(hVal, &addr, nodeId);
    }

    return WrapperReply(interp, status, argc, argv);
}










/************************************************************************
 * api_cm_GetLocalRASAddress
 * purpose : wrapping the original ASN api function - cmGetLocalRASAddress
 * syntax  : api:cm:GetLocalRASAddress
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : The ras transaction handle
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_GetLocalRASAddress(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    cmTransportAddress  addr;
    char                result[128];
    int                 status;

    status = cmGetLocalRASAddress(ep->hApp, &addr);
    if (status >= 0)
    {
        cmTransportAddress2String(&addr, sizeof(result), result);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cm_GetLocalCallSignalAddress
 * purpose : wrapping the original ASN api function - cmGetLocalCallSignalAddress
 * syntax  : api:cm:GetLocalCallSignalAddress
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : The ras transaction handle
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_GetLocalCallSignalAddress(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    cmTransportAddress  addr;
    char                result[128];
    int                 status;

    RV_UNUSED_ARG(clientData);

    status = cmGetLocalCallSignalAddress(ep->hApp, &addr);
    if (status >= 0)
    {
        cmTransportAddress2String(&addr, sizeof(result), result);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cm_GetLocalAnnexEAddress
 * purpose : wrapping the original ASN api function - cmGetLocalAnnexEAddress
 * syntax  : api:cm:GetLocalAnnexEAddress
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : The ras transaction handle
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_GetLocalAnnexEAddress(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    cmTransportAddress  addr;
    char                result[128];
    int                 status;

    RV_UNUSED_ARG(clientData);

    status = cmGetLocalAnnexEAddress(ep->hApp, &addr);
    if (status >= 0)
    {
        cmTransportAddress2String(&addr, sizeof(result), result);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
 * api_cm_GetParam
 * purpose : wrapping the original CM api function - cmGetParam
 * syntax  : api:cm:GetParam <param> <index>
 *         : <param>        - determine which param
 *         : <index>        - index of the parameter
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_GetParam(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    int                 status = RV_ERROR_UNKNOWN;
    int                 intValue;
    cmParam             param;
    char                *returnedValue;
    char                data[1024];

    if (argc != 3)
       return WrapperBadParams(interp, "api:cm:GetParam <param> <index>");

    param = String2CMParam(argv[1]);
    returnedValue = data;

    switch (param)
    {
    case cmParamLocalCallSignalAddress:
    case cmParamLocalAnnexEAddress:
    case cmParamLocalRASAddress:
        {
            int index;
            cmTransportAddress addr;
            intValue = sizeof(addr);

            index = atoi(argv[2]);
            status = cmGetParam(ep->hApp, param, index, &intValue, (char *)&addr);
            if (status >= 0)
            {
                /* building Address */
                cmTransportAddress2String(&addr, sizeof(data), data);
            }
            break;
        }
    default: break;
    }

    if (status >= 0)
        Tcl_SetResult(interp, returnedValue, TCL_VOLATILE);
    return WrapperReply(interp, status, argc, argv);
}







/********************************************************************************************
*
* Automatic RAS api
*
*********************************************************************************************/


/************************************************************************
 * api_cm_Register
 * purpose : wrapping the original ASN api function - cmRegister
 * syntax  : api:cm:Register <hasAliases> <aliases> <address> <gkId>
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : The ras transaction handle
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_Register(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvChar *aliases = NULL;
    int status;

    if (argc == 1)
    {
        status = epRasRegister(ep, NULL, RV_FALSE, NULL, RV_FALSE, NULL);
    }
    else if (argc == 5)
    {
        if (atoi(argv[1]))
            aliases = argv[2];
        else
            aliases = "";

        status = epRasRegister(ep, aliases, (strlen(argv[3]) != 0), argv[3], (strlen(argv[4]) != 0), argv[4]);
    }
    else
    {
        return WrapperBadParams(interp, "api:cm:Register <hasAliases> <aliases> <address> <gkId>");
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cm_Unregister
 * purpose : wrapping the original ASN api function - cmUnregister
 * syntax  : api:cm:Unregister (optional) <hasAliases> <aliases>
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : The ras transaction handle
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_Unregister(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    int status;

    if (argc == 1)
    {
        status = epRasUnregister(ep, RV_FALSE, NULL);
    }
    else if (argc == 3)
    {
        status = epRasUnregister(ep, atoi(argv[1]), argv[2]);
    }
    else
    {
        return WrapperBadParams(interp, "api:cm:Unregister");
    }

    return WrapperReply(interp, status, argc, argv);
}







/********************************************************************************************
*
* CMRAS api
*
*********************************************************************************************/


/************************************************************************
 * api_cmras_StartTransaction
 * purpose : wrapping the original ASN api function - cmRASStartTransaction
 * syntax  : api:cmras:StartTransaction <trasnaction> <destAddr> [<hsCall>]
 *           <transaction>  - Transaction type
 *           <destAddr>     - Destination address
 *           <hsCall>       - Call handle of transaction
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : The ras transaction handle
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_StartTransaction(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    HRAS                hRas = NULL;
    cmRASTransaction    transaction;
    cmTransportAddress  destAddr;
    EpCallObj           *call;
    HCALL               hsCall;
    int                 status;
    char                result[20];

    if ((argc < 3) || (argc > 4))
        return WrapperBadParams(interp, "api:cmras:StartTransaction <transaction> <destAddr> [<hsCall>]");

    transaction = String2RASTransaction(argv[1]);
    status = cmString2TransportAddress(argv[2], &destAddr);
    if ((argc == 4) && (sscanf(argv[3], "%p", &call) == 1) && (call != NULL))
        hsCall = call->hsCall;
    else
        hsCall = NULL;

    if (status >= 0)
        status = cmRASStartTransaction(ep->hApp, NULL, &hRas, transaction, &destAddr, hsCall);

    if (status >= 0)
    {
        sprintf(result, "%p", hRas);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cmras_SetParam
 * purpose : wrapping the original ASN api function - cmRASSetParam
 * syntax  : api:cmras:SetParam <hsRas> <trStage> <paramType> <index> <value>
 *           <hsRas>    - Stack's transaction handle
 *           <trStage>  - Stage of transaction to set
 *           <paramType>- Parameter type to set
 *           <index>    - Index to set
 *           <value>    - Value to set
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_SetParam(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    HRAS            hRas;
    int             status = 0, freeAlias = 0;
    cmRASTrStage    stage;
    cmRASParam      param;
    int             index;
    RvInt32         value = 0;
    char*           svalue;
    char            buffer[1024];
    svalue = buffer;

    if (argc != 6)
        return WrapperBadParams(interp, "api:cmras:SetParam <hRas> <trStage> <paramType> <index> <value>");

    sscanf(argv[1], "%p", (void**)&hRas);
    stage = String2RASTrStage(argv[2]);
    param = String2RASParam(argv[3]);
    index = atoi(argv[4]);

    switch (param)
    {
        case cmRASParamDiscoveryComplete:
        case cmRASParamActiveMC:
        case cmRASParamAnswerCall:
        case cmRASParamMulticastTransaction:
        case cmRASParamAnsweredCall:
        case cmRASParamAlmostOutOfResources:
        case cmRASParamIrrFrequency:
        case cmRASParamBandwidth:
        case cmRASParamCRV:
        case cmRASParamKeepAlive:
        case cmRASParamTimeToLive:
        case cmRASParamDelay:
        case cmRASParamAltGKisPermanent:
        case cmRASParamNonStandardData:
        case cmRASParamNeedResponse:
        case cmRASParamMaintainConnection:
        case cmRASParamMultipleCalls:
        case cmRASParamWillRespondToIRR:
        case cmRASParamSupportsAltGk:
        case cmRASParamAdditiveRegistration:
        case cmRASParamSupportsAdditiveRegistration:
        case cmRASParamSegmentedResponseSupported:
        case cmRASParamNextSegmentRequested:
        case cmRASParamCapacityInfoRequested:
        case cmRASParamHopCount:
        case cmRASParamRetries:
        case cmRASParamTimeout:
        case cmRASParamLocalAddressIndex:
        case cmRASParamCanMapSrcAlias:
            /* RvBool/int */
            svalue = NULL;
            value = atoi(argv[5]);
            break;

        case cmRASParamRASAddress:
        case cmRASParamCallSignalAddress:
        case cmRASParamDestCallSignalAddress:
        case cmRASParamSrcCallSignalAddress:
        case cmRASParamReplyAddress:
        case cmRASParamDestinationIpAddress:
            /* cmTransportAddress */
            value = sizeof(cmRASTransport);
            status = cmString2TransportAddress(argv[5], (cmRASTransport*)svalue);
            break;

        case cmRASParamGatekeeperID:
        case cmRASParamEndpointID:
        {
            cmRASAlias* alias;
            char * aliasString = (buffer + sizeof(cmAlias));
            alias = (cmRASAlias *)svalue;

            value = sizeof(cmRASAlias);
            alias->length = (RvUint16)(2*strlen(argv[5]));
            if (param == cmRASParamEndpointID)
                alias->type = cmAliasTypeEndpointID;
            else
                alias->type = cmAliasTypeGatekeeperID;
            utlChr2Bmp(argv[5], (RvUint8*)aliasString);
            alias->string = aliasString;
            break;
        }

        case cmRASParamEndpointAlias:
        case cmRASParamTerminalAlias:
        case cmRASParamDestInfo:
        case cmRASParamSrcInfo:
        case cmRASParamDestExtraCallInfo:
        case cmRASParamRejectedAlias:
        case cmRASParamSourceInfo:
        case cmRASParamInvalidTerminalAlias:
        case cmRASParamExtension:
        case cmRASParamSourceEndpointInfo:
        case cmRASParamModifiedSrcInfo:
            /* cmRASAlias */
            value = sizeof(cmRASAlias);
            status = String2Alias(ep, argv[5], (cmAlias*)svalue);
            freeAlias = (status >= 0);
            break;

        case cmRASParamNonStandard:
        {
            cmNonStandardParam* nsdParam;
            nsdParam = (cmNonStandardParam *)buffer;
            nsdParam->data = buffer + sizeof(cmNonStandardParam);
            value = sizeof(*nsdParam);

            status = String2CMNonStandardParam(argv[5], nsdParam);
            break;
        }

        case cmRASParamAlternateGatekeeper:
        case cmRASParamAltGKInfo:
        {
            cmAlternateGatekeeper* altGk;
            altGk = (cmAlternateGatekeeper *)buffer;
            value = sizeof(*altGk);

            status = String2CMAlternateGatekeeper(argv[5], altGk);
            break;
        }

        case cmRASParamEndpointType:
        case cmRASParamTerminalType:
            {
                value = (RvInt32)String2EndpointType(argv[5]);
                if(value == -1)
                {
                    int i = 0;

                    sprintf(buffer, "Enum value incorrect. Possible values: ");
                    while (strcmp(EndpointType2String((cmEndpointType) i), "-unknown-"))
                    {
                        sprintf(buffer, "%s %s,", buffer, EndpointType2String((cmEndpointType) i));
                        i++;
                    }
                    buffer[strlen(buffer)-1] = '.';

                    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
                    return TCL_ERROR;
                }
                break;
            }

        case cmRASParamCallType:
            {
                value = (RvInt32)String2CallType(argv[5]);
                if(value == -1)
                {
                    int i = 0;

                    sprintf(buffer, "Enum value incorrect. Possible values: ");
                    while(strcmp(CallType2String((cmCallType) i), "-unknown-"))
                    {
                        sprintf(buffer, "%s %s,", buffer, CallType2String((cmCallType) i));
                        i++;
                    }
                    buffer[strlen(buffer)-1] = '.';

                    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
                    return TCL_ERROR;
                }
                break;
            }
        case cmRASParamCallModel:
            {
                value = (RvInt32)String2CallModelType(argv[5]);
                if(value == -1)
                {
                    int i = 0;

                    sprintf(buffer, "Enum value incorrect. Possible values: ");
                    while (strcmp(CallModelType2String((cmCallModelType) i), "-unknown-"))
                    {
                        sprintf(buffer, "%s %s,", buffer, CallModelType2String((cmCallModelType) i));
                        i++;
                    }
                    buffer[strlen(buffer)-1] = '.';

                    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
                    return TCL_ERROR;
                }
                break;
            }
        case cmRASParamDisengageReason:
            {
                value = (RvInt32)String2DisengageReason(argv[5]);
                if(value == -1)
                {
                    int i = 0;

                    sprintf(buffer, "Enum value incorrect. Possible values: ");
                    while (strcmp(DisengageReason2String((cmRASDisengageReason) i), "-unknown-"))
                    {
                        sprintf(buffer, "%s %s,", buffer, DisengageReason2String((cmRASDisengageReason) i));
                        i++;
                    }
                    buffer[strlen(buffer)-1] = '.';

                    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
                    return TCL_ERROR;
                }
                break;
            }
        case cmRASParamRejectReason:
            {
                value = (RvInt32)String2RASReason(argv[5]);
                if(value == -1)
                {
                    int i = 0;

                    sprintf(buffer, "Enum value incorrect. Possible values: ");
                    while (strcmp(RASReason2String((cmRASReason) i), "-unknown-"))
                    {
                        sprintf(buffer, "%s %s,", buffer, RASReason2String((cmRASReason) i));
                        i++;
                    }
                    buffer[strlen(buffer)-1] = '.';

                    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
                    return TCL_ERROR;
                }
                break;
            }
        case cmRASParamUnregReason:
            {
                value = (RvInt32)String2UnregReason(argv[5]);
                if(value == -1)
                {
                    int i = 0;

                    sprintf(buffer, "Enum value incorrect. Possible values: ");
                    while (strcmp(UnregReason2String((cmRASUnregReason) i), "-unknown-"))
                    {
                        sprintf(buffer, "%s %s,", buffer, UnregReason2String((cmRASUnregReason) i));
                        i++;
                    }
                    buffer[strlen(buffer)-1] = '.';

                    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
                    return TCL_ERROR;
                }
                break;
            }
        case cmRASParamTransportQOS:
            {
                value = (RvInt32)String2TransportQOS(argv[5]);
                if(value == -1)
                {
                    int i = 0;

                    sprintf(buffer, "Enum value incorrect. Possible values: ");
                    while (strcmp(TransportQOS2String((cmTransportQOS) i), "-unknown-"))
                    {
                        sprintf(buffer, "%s %s,", buffer, TransportQOS2String((cmTransportQOS) i));
                        i++;
                    }
                    buffer[strlen(buffer)-1] = '.';

                    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
                    return TCL_ERROR;
                }
                break;
            }
            /* enums */
        case cmRASParamIrrStatus:
            {
                value = (RvInt32)String2IrrStatus(argv[5]);
                if (value == -1)
                {
                    Tcl_SetResult(interp, (char *)"Enum value incorrect. Possible values: "
                        "Complete, Incomplete, InvalidCall.", TCL_VOLATILE);
                    return TCL_ERROR;
                }
                break;
            }

        case cmRASParamCID:
        case cmRASParamCallID:
            {
                int i;
                for(i=0; i<16; i++)
                {
                    svalue[i] = argv[5][i];
                }
                value = 16;
                break;
            }
            /* misc/check */

            /* HCALL */
        case cmRASParamCallHandle:
        case cmRASParamApplicationHandle:
            /* This parameter is READ ONLY */
            status = RV_ERROR_UNKNOWN;
            break;

        case cmRASParamEndpointVendor:
        case cmRASParamEmpty:
        default:
            break;
    }

    if (status >= 0)
        status = cmRASSetParam(hRas, stage, param, index, value, svalue);

    if (freeAlias)
        FreeAlias(ep, (cmAlias *)svalue);

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cmras_GetParam
 * purpose : wrapping the original ASN api function - cmRASGetParam
 * syntax  : api:cmras:GetParam <hsRas> <trStage> <paramType> <index>
 *           <hsRas>    - Stack's transaction handle
 *           <trStage>  - Stage of transaction to set
 *           <paramType>- Parameter type to set
 *           <index>    - Index to set
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : value
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_GetParam(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS            hRas;
    int             status;
    cmRASTrStage    stage;
    cmRASParam      param;
    int             index;
    RvInt32         value;
    char*           svalue;
    char            buffer[1024];
    char*           result;
    char            resultBuf[1024];
    svalue = buffer;
    result = resultBuf;

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:cmras:GetParam <hRas> <trStage> <paramType> <index>");

    sscanf(argv[1], "%p", (void**)&hRas);
    stage = String2RASTrStage(argv[2]);
    param = String2RASParam(argv[3]);
    index = atoi(argv[4]);

    /* Make sure the svalue parameter is initialized properly */
    memset(buffer, 0, sizeof(buffer));
    switch (param)
    {
        case cmRASParamAlternateGatekeeper:
        case cmRASParamAltGKInfo:
        {
            cmAlternateGatekeeper* altGk = (cmAlternateGatekeeper *)svalue;
            altGk->gatekeeperIdentifier.string = svalue + sizeof(cmAlternateGatekeeper);
            break;
        }
        default:
            /* do nothing */
            break;
    }

    switch(param)
    {
        case (cmRASParamNonStandard):
            ((cmNonStandardParam *) svalue)->data = (svalue + sizeof(cmNonStandardParam));
            ((cmNonStandardParam *) svalue)->length = (sizeof(buffer) - sizeof(cmNonStandardParam));
            break;
        case cmRASParamGatekeeperID:
        case cmRASParamEndpointID:
        case cmRASParamEndpointAlias:
        case cmRASParamTerminalAlias:
        case cmRASParamDestInfo:
        case cmRASParamSrcInfo:
        case cmRASParamDestExtraCallInfo:
        case cmRASParamRejectedAlias:
        case cmRASParamSourceInfo:
        case cmRASParamInvalidTerminalAlias:
            ((cmAlias *) svalue)->string = (svalue + sizeof(cmAlias));
            break;
        default:
            break;
    }

    status = cmRASGetParam(hRas, stage, param, index, &value, svalue);

    if (status >= 0)
    {
        switch (param)
        {
            case cmRASParamDiscoveryComplete:
            case cmRASParamActiveMC:
            case cmRASParamAnswerCall:
            case cmRASParamMulticastTransaction:
            case cmRASParamAnsweredCall:
            case cmRASParamAlmostOutOfResources:
            case cmRASParamIrrFrequency:
            case cmRASParamBandwidth:
            case cmRASParamCRV:
            case cmRASParamKeepAlive:
            case cmRASParamTimeToLive:
            case cmRASParamDelay:
            case cmRASParamAltGKisPermanent:
            case cmRASParamNonStandardData:
            case cmRASParamNeedResponse:
            case cmRASParamMaintainConnection:
            case cmRASParamMultipleCalls:
            case cmRASParamWillRespondToIRR:
            case cmRASParamSupportsAltGk:
            case cmRASParamAdditiveRegistration:
            case cmRASParamSupportsAdditiveRegistration:
            case cmRASParamSegmentedResponseSupported:
            case cmRASParamNextSegmentRequested:
            case cmRASParamCapacityInfoRequested:
            case cmRASParamHopCount:
            case cmRASParamRetries:
            case cmRASParamTimeout:
            case cmRASParamLocalAddressIndex:
                /* RvBool/int */
                sprintf(resultBuf, "%d", value);
                break;

            case cmRASParamRASAddress:
            case cmRASParamDestCallSignalAddress:
            case cmRASParamSrcCallSignalAddress:
            case cmRASParamReplyAddress:
            case cmRASParamDestinationIpAddress:
            {   /* cmTransportAddress */
                cmTransportAddress * address = (cmTransportAddress *) svalue;
                cmTransportAddress2String(address, sizeof(result), result);
                break;
            }

            case cmRASParamEndpointAlias:
            case cmRASParamTerminalAlias:
            case cmRASParamDestInfo:
            case cmRASParamSrcInfo:
            case cmRASParamDestExtraCallInfo:
            case cmRASParamRejectedAlias:
            case cmRASParamSourceInfo:
            case cmRASParamInvalidTerminalAlias:
            {   /*cmAlias */
                cmAlias * alias = (cmAlias *) svalue;
                Alias2String(alias, result);
                break;
            }
            case cmRASParamGatekeeperID:
            case cmRASParamEndpointID:
            {   /*cmAlias BMP */
                cmAlias * alias = (cmAlias *) svalue;
                utlBmp2Chr(result, (RvUint8 *)alias->string, alias->length);
                break;
            }
            case cmRASParamNonStandard:
            {
                cmNonStandardParam* nsdParam = (cmNonStandardParam *)svalue;
                result = (char *)CMNonStandardParam2String(nsdParam);
                break;
            }

            case cmRASParamAlternateGatekeeper:
            case cmRASParamAltGKInfo:
            {
                cmAlternateGatekeeper* altGk = (cmAlternateGatekeeper *)svalue;
                result = (char *)CMAlternateGatekeeper2String(altGk);
                break;
            }

            case cmRASParamEndpointType:
            case cmRASParamTerminalType:
                {
                    result = (char *)EndpointType2String((cmEndpointType)value);
                    break;
                }
            case cmRASParamCallType:
                {
                    result = (char *)CallType2String((cmCallType)value);
                    break;
                }
            case cmRASParamCallModel:
                {
                    result = (char *)CallModelType2String((cmCallModelType)value);
                    break;
                }
            case cmRASParamDisengageReason:
                {
                    result = (char *)DisengageReason2String((cmRASDisengageReason)value);
                    break;
                }
            case cmRASParamRejectReason:
                {
                    result = (char *)RASReason2String((cmRASReason)value);
                    break;
                }
            case cmRASParamUnregReason:
                {
                    result = (char *)UnregReason2String((cmRASUnregReason)value);
                    break;
                }
            case cmRASParamTransportQOS:
                {
                    result = (char *)TransportQOS2String((cmTransportQOS)value);
                    break;
                }
                /* enums */
            case cmRASParamIrrStatus:
                {
                    result = (char *)IrrStatus2String(value);
                    break;
                }

            case cmRASParamCID:
            case cmRASParamCallID:
                {
                    result = svalue;
                    result[16] = '\0';
                    break;
                }

                /* misc/check */

                /* HCALL */
            case cmRASParamCallHandle:
                {
                    HCALL       hsCall    = *(HCALL*)svalue;
                    HAPPCALL    haCall;
                    if (hsCall != NULL)
                        cmCallGetHandle(hsCall, &haCall);
                    else
                        haCall = NULL;
                    sprintf(result, "%p", haCall);
                    break;
                }

            case cmRASParamApplicationHandle:
            case cmRASParamEndpointVendor:
            case cmRASParamEmpty:

            default:
                break;
        }
    }

    if (status >= 0)
        Tcl_SetResult(interp, result, TCL_VOLATILE);

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cmras_GetNumOfParams
 * purpose : wrapping the original ASN api function - cmRASGetParam
 * syntax  : api:cmras:GetNumOfParams <hsRas> <trStage> <param>
 *           <hsRas>    - Stack's transaction handle
 *           <trStage>  - Stage to get from
 *           <param>    - Parameter to get
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : The number of parameters
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_GetNumOfParams(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS            hRas;
    int             status;
    cmRASTrStage    stage;
    cmRASParam      param;
    char            result[20];

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:cmras:GetNumOfParams <hRas> <trStage> <paramType>");

    sscanf(argv[1], "%p", (void**)&hRas);
    stage = String2RASTrStage(argv[2]);
    param = String2RASParam(argv[3]);

    status = cmRASGetNumOfParams(hRas, stage, param);

    if (status >= 0)
    {
        sprintf(result, "%d", status);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cmras_Request
 * purpose : wrapping the original ASN api function - cmRASRequest
 * syntax  : api:cmras:Request <hsRas>
 *           <hsRas>    - Stack's transaction handle
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_Request(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS    hRas;
    int     status;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cmras:Request <hRas>");

    sscanf(argv[1], "%p", (void**)&hRas);
    status = cmRASRequest(hRas);

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cmras_DummyRequest
 * purpose : wrapping the original ASN api function - cmRASDummyRequest
 * syntax  : api:cmras:DummyRequest <hsRas>
 *           <hsRas>    - Stack's transaction handle
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_DummyRequest(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS    hRas;
    int     status;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cmras:DummyRequest <hRas>");

    sscanf(argv[1], "%p", (void**)&hRas);
    status = cmRASDummyRequest(hRas);

    return WrapperReply(interp, status, argc, argv);
}



/************************************************************************
 * api_cmras_Confirm
 * purpose : wrapping the original ASN api function - cmRASConfirm
 * syntax  : api:cmras:Confirm <hsRas>
 *           <hsRas>    - Stack's transaction handle
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_Confirm(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS    hRas;
    int     status;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cmras:Confirm <hRas>");

    sscanf(argv[1], "%p", (void**)&hRas);
    status = cmRASConfirm(hRas);

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cmras_Reject
 * purpose : wrapping the original ASN api function - cmRASReject
 * syntax  : api:cmras:Reject <hsRas> <reason>
 *           <hsRas>    - Stack's transaction handle
 *           <reason>   - Reject reason
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_Reject(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS    hRas;
    int     status;
    cmRASReason reason;
    EpObj *ep = (EpObj *)clientData;

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:cmras:Reject <hRas> <reason>");

    sscanf(argv[1], "%p", (void**)&hRas);
    reason = String2RASReason(argv[2]);
    if (reason == cmRASReasonUnknownMessageResponse)
    {
        RvPvtNodeId prop = cmGetProperty((HPROTOCOL)hRas), nodeId;
        __pvtBuildByFieldIds(nodeId, ep->hVal, prop,
            {_q931(response) _q931(unknownMessageResponse) _q931(messageNotUnderstood) LAST_TOKEN},
            0, "");
    }
    status = cmRASReject(hRas, reason);

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cmras_InProgress
 * purpose : wrapping the original ASN api function - cmRASInProgress
 * syntax  : api:cmras:InProgress <hsRas> <delay>
 *           <hsRas>    - Stack's transaction handle
 *           <delay>    - Delay in milliseconds
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_InProgress(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS    hRas;
    int     status;
    int     delay;

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:cmras:InProgress <hRas> <delay>");

    sscanf(argv[1], "%p", (void**)&hRas);
    sscanf(argv[2], "%d", &delay);

    status = cmRASInProgress(hRas, delay);

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cmras_Close
 * purpose : wrapping the original ASN api function - cmRASClose
 * syntax  : api:cmras:Close <hsRas>
 *           <hsRas>    - Stack's transaction handle
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_Close(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS    hRas;
    int     status;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cmras:Close <hRas>");

    sscanf(argv[1], "%p", (void**)&hRas);
    status = cmRASClose(hRas);

    return WrapperReply(interp, status, argc, argv);
}



/************************************************************************
 * api_cmras_GetTransaction
 * purpose : wrapping the original ASN api function - cmRASGetTransaction
 * syntax  : api:cmras:GetTransaction <hsRas>
 *           <hsRas>    - Stack's transaction handle
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Transaction's type
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cmras_GetTransaction(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    HRAS                hRas;
    cmRASTransaction    transaction;
    int                 status;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cmras:GetTransaction <hRas>");

    sscanf(argv[1], "%p", (void**)&hRas);
    status = cmRASGetTransaction(hRas, &transaction);
    if (status >= 0)
        Tcl_SetResult(interp, (char *)RASTransaction2String(transaction), TCL_STATIC);

    return WrapperReply(interp, status, argc, argv);
}







/********************************************************************************************
*
* General CM api
*
*********************************************************************************************/


/************************************************************************
 * api_cm_Vt2Alias
 * purpose : wrapping the original ASN api function - cmVt2Alias
 * syntax  : api:cm:Vt2Alias <valTree> <nodeId>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - AliasAddress type node id
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Alias string
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_Vt2Alias(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    char    aliasStr[256];
    cmAlias alias;
    RvInt32 nodeId;
    int     status;
    char    result[256];
    HPVT    hVal;

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:cm:Vt2Alias <valTree> <nodeId>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    alias.length = sizeof(aliasStr);
    alias.string = aliasStr;

    nodeId = atoi(argv[2]);
    status = cmVt2Alias(hVal, &alias, nodeId);
    if (status >= 0)
    {
        Alias2String(&alias, result);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cm_Alias2Vt
 * purpose : wrapping the original ASN api function - cmAlias2Vt
 * syntax  : api:cm:Alias2Vt <valTree> <alias> <nodeId>
 *           <valTree>  - Value tree to use
 *           <alias>    - Alias string to convert
 *           <nodeId>   - AliasAddress node Id under which the alias is built
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Alias node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_Alias2Vt(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    cmAlias alias;
    RvInt32 nodeId;
    int     status;
    char    result[20];
    HPVT    hVal;

    if (argc != 4)
        return WrapperBadParams(interp, "api:cm:Alias2Vt <valTree> <alias> <nodeId>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    status = String2Alias(ep, argv[2], &alias);
    nodeId = atoi(argv[3]);

    if (status >= 0)
    {
        status = cmAlias2Vt(hVal, &alias, nodeId);
        FreeAlias(ep, &alias);
    }

    if (status >= 0)
    {
        sprintf(result, "%d", status);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cm_GetProperty
 * purpose : wrapping the original ASN api function - cmGetProperty
 * syntax  : api:cm:GetProperty <handle> [CALL]
 *           <handle>   - Ras or Call handle whose property we want
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Alias node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_GetProperty(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPROTOCOL   handle;
    int         status;
    char        result[20];

    RV_UNUSED_ARG(clientData);

    if ((argc < 2) || (argc > 3))
        return WrapperBadParams(interp, "api:cm:GetProperty <handle> [CALL]");

    sscanf(argv[1], "%p", (void**)&handle);
    if (handle == NULL)
        return WrapperReply(interp, RV_ERROR_NULLPTR, argc, argv);

    if ((argc == 3) && (strcmp("CALL", argv[2]) == 0))
    {
        /* Call handle - from CallInfo */
        EpCallObj *call = (EpCallObj *)handle;
        status = cmGetProperty((HPROTOCOL)call->hsCall);
    }
    else
    {
        /* RAS handle - take directly */
        status = cmGetProperty(handle);
    }

    if (status >= 0)
    {
        sprintf(result, "%d", status);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cm_GetTpktChanHandle
 * purpose : wrapping the original ASN api function - cmGetProperty
 * syntax  : api:cm:GetTpktChanHandle <callHandle> <protocolType>
 *           <callHandle>   - Call handle whose protocol connection we want
 *           <protocolType> - Q931 or H245
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Alias node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_GetTpktChanHandle(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPROTCONN   handle = NULL;
    EpCallObj   *call;
    int         status;
    char        result[20];

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:cm:GetTpktChanHandle <callHandle> <protocolType>");

    if (sscanf(argv[1], "%p", (void**)&call) != 1)
        return TCL_ERROR;

    if (strcmp("H245", argv[2]) == 0)
    {
        handle = cmGetTpktChanHandle(call->hsCall, cmH245TpktChannel);
    }
    else if (strcmp("Q931", argv[2]) == 0)
    {
        handle = cmGetTpktChanHandle(call->hsCall, cmQ931TpktChannel);
    }
    else
        handle = NULL;

    if (handle != NULL)
    {
        sprintf(result, "%p", handle);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
        status = RV_OK;
    }
    else
        status = -1;

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cm_ProtocolSendMessage
 * purpose : wrapping the original ASN api function - cmGetProperty
 * syntax  : api:cm:ProtocolSendMessage <protocol> <nodeId>
 *           <protocol> - Protocol handle to send on
 *           <nodeId>   - Node Id of the message to send
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Alias node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_ProtocolSendMessage(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    HPROTCONN   handle;
    int         status;

    if (argc != 3)
        return WrapperBadParams(interp, "api:cm:ProtocolSendMessage <protocol> <nodeId>");

    if (sscanf(argv[1], "%p", (void**)&handle) != 1)
        return TCL_ERROR;

    status = cmProtocolSendMessage(ep->hApp, handle, atoi(argv[2]));

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_cm_GetValTree
 * purpose : wrapping the original ASN api function - cmGetValTree
 * syntax  : api:cm:GetValTree
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Value tree
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_GetValTree(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    char    result[20];

    if (argc != 1)
        return WrapperBadParams(interp, "api:cm:GetValTree");

    sprintf(result, "%p", cmGetValTree(ep->hApp));
    Tcl_SetResult(interp, result, TCL_VOLATILE);

    return WrapperReply(interp, 0, argc, argv);
}


/************************************************************************
 * api_cm_GetRASConfigurationHandle
 * purpose : wrapping the original ASN api function - cmGetRASConfigurationHandle
 * syntax  : api:cm:GetRASConfigurationHandle
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : RAS configuration node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_cm_GetRASConfigurationHandle(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    int     nodeId;
    char    result[20];

    if (argc != 1)
        return WrapperBadParams(interp, "api:cm:GetRASConfigurationHandle");

    nodeId = cmGetRASConfigurationHandle(ep->hApp);

    sprintf(result, "%d", nodeId);
    Tcl_SetResult(interp, result, TCL_VOLATILE);

    return WrapperReply(interp, nodeId, argc, argv);
}






/********************************************************************************************
*
* General CM api
*
*********************************************************************************************/


/************************************************************************
 * api_pvt_AddRoot
 * purpose : wrapping the original ASN api function - pvtAddRoot
 * syntax  : api:pvt:AddRoot <valTree> <rootName> <value>
 *           <valTree>  - Value tree to use
 *           <rootName> - Name of root
 *           <value>    - Value of root node
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Root node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_AddRoot(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    HPVT    hVal;
    RvInt32 nodeId;
    char    result[20];

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:pvt:AddRoot <valTree> <rootName> <value>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(ep->hApp, argv[2]), atoi(argv[3]), NULL);

    if (nodeId >= 0)
    {
        sprintf(result, "%d", nodeId);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, nodeId, argc, argv);
}


/************************************************************************
 * api_pvt_GetByPath
 * purpose : wrapping the original ASN api function - pvtGetByPath
 * syntax  : api:pvt:GetByPath <valTree> <nodeId> <path>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - Node id to start from
 *           <path>     - Path to traverse
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_GetByPath(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPVT    hVal;
    RvInt32 nodeId;
    char    result[20];

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:pvt:GetByPath <valTree> <nodeId> <path>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = pvtGetByPath(hVal, atoi(argv[2]), argv[3], NULL, NULL, NULL);

    if (nodeId >= 0)
    {
        sprintf(result, "%d", nodeId);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, nodeId, argc, argv);
}


/************************************************************************
 * api_pvt_BuildByPath
 * purpose : wrapping the original ASN api function - pvtBuildByPath
 * syntax  : api:pvt:BuildByPath <valTree> <nodeId> <path> <value>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - Node id to start from
 *           <path>     - Path to traverse
 *           <value>    - Value of node
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_BuildByPath(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPVT    hVal;
    RvInt32 nodeId;
    char    result[20];

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:pvt:BuildByPath <valTree> <nodeId> <path> <value>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = pvtBuildByPath(hVal, atoi(argv[2]), argv[3], 0, NULL);

    if (nodeId >= 0)
    {
        int synNodeId;

        pvtGet(hVal, nodeId, NULL, &synNodeId, NULL, NULL);
        switch (pstGetNodeType(pvtGetSynTree(hVal, nodeId), synNodeId))
        {
            case pstInteger:
            case pstNull:
            case pstBoolean:
            case pstEnumeration:
                pvtSet(hVal, nodeId, -1, atoi(argv[4]), NULL);
                break;

            case pstObjectIdentifier:
            case pstOctetString:
            case pstBitString:
            case pstGeneralString:
            case pstUniversalString:
            case pstIA5String:
            case pstVisibleString:
            case pstNumericString:
            case pstPrintableString:
                pvtSet(hVal, nodeId, -1, strlen(argv[4]), argv[4]);
                break;

            case pstBMPString:
            {
                char buffer[1024];
                utlChr2Bmp(argv[4], (RvUint8*)buffer);
                pvtSet(hVal, nodeId, -1, strlen(argv[4])*2, buffer);
                break;
            }

            case pstChoice:
            case pstSequence:
            case pstSet:
            case pstOf:
            case pstSequenceOf:
            case pstSetOf:
            case pstNotSupported:
                break;
        }

        sprintf(result, "%d", nodeId);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, nodeId, argc, argv);
}


/************************************************************************
 * api_pvt_SetByPath
 * purpose : wrapping the original ASN api function - pvtSetByPath
 * syntax  : api:pvt:SetByPath <valTree> <nodeId> <path> <value>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - Node id to start from
 *           <path>     - Path to traverse
 *           <value>    - Value of node
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_SetByPath(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPVT    hVal;
    RvInt32 nodeId;
    char    result[20];

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:pvt:SetByPath <valTree> <nodeId> <path> <value>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = pvtSetByPath(hVal, atoi(argv[2]), argv[3], 0, NULL);

    if (nodeId >= 0)
    {
        int synNodeId;

        pvtGet(hVal, nodeId, NULL, &synNodeId, NULL, NULL);
        switch (pstGetNodeType(pvtGetSynTree(hVal, nodeId), synNodeId))
        {
            case pstInteger:
            case pstNull:
            case pstBoolean:
            case pstEnumeration:
                nodeId = pvtSetByPath(hVal, atoi(argv[2]), argv[3], atoi(argv[4]), NULL);
                break;

            case pstObjectIdentifier:
            case pstOctetString:
            case pstBitString:
            case pstGeneralString:
            case pstUniversalString:
            case pstIA5String:
            case pstVisibleString:
            case pstNumericString:
            case pstPrintableString:
                nodeId = pvtSetByPath(hVal, atoi(argv[2]), argv[3], strlen(argv[4]), argv[4]);
                break;

            case pstBMPString:
            {
                char buffer[1024];
                utlChr2Bmp(argv[4], (RvUint8*)buffer);
                nodeId = pvtSetByPath(hVal, atoi(argv[2]), argv[3], strlen(argv[4])*2, buffer);
                break;
            }

            case pstChoice:
            case pstSequence:
            case pstSet:
            case pstOf:
            case pstSequenceOf:
            case pstSetOf:
            case pstNotSupported:
                break;
        }

        sprintf(result, "%d", nodeId);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, nodeId, argc, argv);
}


/************************************************************************
 * api_pvt_SetTree
 * purpose : wrapping the original ASN api function - pvtSetTree
 * syntax  : api:pvt:SetTree <destVal> <destNodeId> <srcVal> <srcNodeId>
 *           <destVal>      - Value tree to use for the destination
 *           <destNodeId>   - Node id to copy to
 *           <srcVal>       - Value tree to use for the source
 *           <srcNodeId>    - Node id to copy from
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : Node ID
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_SetTree(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPVT    hValSrc, hValDest;
    RvInt32 status;

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:pvt:SetTree <destVal> <destNodeId> <srcVal> <srcNodeId>");

    if (sscanf(argv[1], "%p", (void**)&hValDest) != 1)
        return TCL_ERROR;

    if (sscanf(argv[3], "%p", (void**)&hValSrc) != 1)
        return TCL_ERROR;

    status = pvtSetTree(hValDest, atoi(argv[2]), hValSrc, atoi(argv[4]));

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_pvt_Delete
 * purpose : wrapping the original ASN api function - pvtDelete
 * syntax  : api:pvt:Delete <valTree> <nodeId>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - Node id to start from
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_Delete(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPVT    hVal;
    int     status;

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:pvt:Delete <valTree> <nodeId>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    status = pvtDelete(hVal, atoi(argv[2]));

    return WrapperReply(interp, status, argc, argv);
}

/************************************************************************
 * api_pvt_Get
 * purpose : wrapping the original ASN api function - pvtGet
 * syntax  : api:pvt:Get <valTree> <nodeId>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - Node id to start from
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : String
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_Get(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPVT    hVal;
    RvInt32 nodeId;
    RvInt32 value = -1;
    int     status;
    RvChar  result[10];

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:pvt:Get <valTree> <nodeId>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = atoi(argv[2]);
    status = pvtGet(hVal, nodeId, NULL, NULL, &value, NULL);
    sprintf(result, "%d", value);

    Tcl_SetResult(interp, result, TCL_VOLATILE);

    return WrapperReply(interp, status, argc, argv);
}

/************************************************************************
 * api_pvt_GetString
 * purpose : wrapping the original ASN api function - pvtGetString
 * syntax  : api:pvt:GetString <valTree> <nodeId>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - Node id to start from
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : String
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_GetString(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPVT    hVal;
    RvInt32 nodeId;
    char    result[256];
    RvInt32 length = 256;

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:pvt:GetString <valTree> <nodeId>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    nodeId = atoi(argv[2]);
    length = pvtGetString(hVal, nodeId, length, (char *) result);

    if (length > 0)
    {
        int synNodeId;

        pvtGet(hVal, nodeId, NULL, &synNodeId, NULL, NULL);
        if (pstGetNodeType(pvtGetSynTree(hVal, nodeId), synNodeId) == pstBMPString)
        {
            int i;
            for(i=0; i<=length; i++)
                result[i] = result[i*2+1];
            result[length/2] = '\0';
        }
        Tcl_SetResult(interp, result, TCL_VOLATILE);
    }

    return WrapperReply(interp, nodeId, argc, argv);
}

/************************************************************************
 * myPrintFunc
 * purpose : Printing function used by pvtPrint()
 * input   : type   - Parameter used (here it's the pointer to the
 *                    callback's function name
 *           line   - The line to print
 * output  : none
 * return  : none
 ************************************************************************/
static void myPrintFunc(IN void* type, IN const char*line, ...)
{
    TclExecute("%s {%s}", (char *)type, TclString(line));
}


/************************************************************************
 * api_pvt_Print
 * purpose : wrapping the original ASN api function - pvtPrint
 * syntax  : api:pvt:Print <valTree> <nodeId> <callback>
 *           <valTree>  - Value tree to use
 *           <nodeId>   - Node id to start from
 *           <callback> - Callback function in TCL to use
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_pvt_Print(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    HPVT    hVal;
    int     status;

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:pvt:Print <valTree> <nodeId> <callback>");

    if (sscanf(argv[1], "%p", (void**)&hVal) != 1)
        return TCL_ERROR;

    status = pvtPrint(hVal, atoi(argv[2]), myPrintFunc, argv[3]);

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_ms_GetLogFilename
 * purpose : wrapping the original API function - msGetLogFilename
 * syntax  : api:ms:GetLogFilename
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_ms_GetLogFilename(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    char    filename[300];
    int     status;

    RV_UNUSED_ARG(clientData);

    if (argc != 1)
        return WrapperBadParams(interp, "api:ms:GetLogFilename");

    status = msGetLogFilename(sizeof(filename), filename);

    if (status >= 0)
        Tcl_SetResult(interp, filename, TCL_VOLATILE);

    return WrapperReply(interp, status, argc, argv);
}


/************************************************************************
 * api_app_GetResource
 * purpose : Get resource information
 * syntax  : api:app:GetResource <type> <name>
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ************************************************************************/
int api_app_GetResource(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvH323WatchdogResourceType resourceType;
    RvUint32 res;
    RvChar resStr[10];
    RvStatus status;

    if (argc != 3)
        return WrapperBadParams(interp, "api:appetResource <type> <name>");

    if (strcmp(argv[1], "init") == 0)
        resourceType = RvH323WatchdogInitialVal;
    else if (strcmp(argv[1], "max") == 0)
        resourceType = RvH323WatchdogMaxVal;
    else if (strcmp(argv[1], "usage") == 0)
        resourceType = RvH323WatchdogMaxUsage;
    else if (strcmp(argv[1], "cur") == 0)
        resourceType = RvH323WatchdogCurrent;
    else if (strcmp(argv[1], "delta") == 0)
        resourceType = RvH323WatchdogDeltaVal;
    else
        return WrapperBadParams(interp, "<type> not in init|max|usage|cur|delta");

    status = RvH323WatchdogGetResource(ep->hApp, argv[2], resourceType, &res, NULL);
    if (status >= 0)
    {
        sprintf(resStr, "%d", res);
        Tcl_SetResult(interp, resStr, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}













/********************************************************************************************
*
* Faststart API
*
*********************************************************************************************/


/********************************************************************************************
 * api_cm_FastStartBuild
 * purpose : building a single FS channel.
 * syntax  : api:cm:FastStartBuild <handle> <chanNum> <chanDir> <chanName> <chanType>
 *           <handle>   - handle of the call
 *           <chanNum>  - number of the channel
 *           <chanDir>  - direction of the channel (transmit or recieve)
 *           <chanName> - name of the channel
 *           <chanType> - type of the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : node ID of the channle if ok, negative o.w.
 ********************************************************************************************/
int api_cm_FastStartBuild(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    int status;
    char * chanDir, * chanType, nodeID[16];
    cmCapDataType DataType = cmCapEmpty;
    cmChannelDirection direction = dirReceive;
    cmFastStartChannel fsChannel;
    memset(&fsChannel, 0, sizeof(fsChannel));

    RV_UNUSED_ARG(clientData);

    if (argc != 6)
        return WrapperBadParams(interp, "api:app:FastStartBuild <handle> <chanNum> <chanDir> <chanName> <chanType>");

    sscanf(argv[1], "%p", &call);
    fsChannel.index = atoi(argv[2]);
    chanDir = argv[3];
    fsChannel.channelName = argv[4];
    chanType = argv[5];

    if (chanDir[0] == 't')
    {
        /* Add this channel to the alternatives of this partnerChannel */
        direction = dirTransmit;
        fsChannel.rtp.port = 0;
        fsChannel.rtp.type = cmTransportTypeIP;
        fsChannel.rtcp.port = 2327;
        fsChannel.rtcp.type = cmTransportTypeIP;
        fsChannel.dataTypeHandle = -1;
    }
    if (chanDir[0] == 'r')
    {
        /* Add this channel to the alternatives of this partnerChannel */
        direction = dirReceive;
        fsChannel.rtp.port = 2326;
        fsChannel.rtp.type = cmTransportTypeIP;
        fsChannel.rtcp.port = 2327;
        fsChannel.rtcp.type = cmTransportTypeIP;
        fsChannel.dataTypeHandle = -1;
    }

    switch (chanType[0])
    {
    case('e'):
        {
            DataType = cmCapEmpty;
            break;
        }
    case('a'):
        {
            DataType = cmCapAudio;
            break;
        }
    case('v'):
        {
            DataType = cmCapVideo;
            break;
        }
    case('d'):
        {
            DataType = cmCapData;
            break;
        }
    case('n'):
        {
            DataType = cmCapNonStandard;
            break;
        }
    case('u'):
        {
            DataType = cmCapUserInput;
            break;
        }
    case('c'):
        {
            DataType = cmCapConference;
            break;
        }
    case('h'):
        {
            DataType = cmCapH235;
            break;
        }
    case('m'):
        {
            DataType = cmCapMaxPendingReplacementFor;
            break;
        }
    case('g'):
        {
            DataType = cmCapGeneric;
            break;
        }
    }
    status = cmFastStartBuild(call->hsCall, DataType, direction, &fsChannel);

    if (status >= 0)
    {
        sprintf(nodeID, "%d", status);
        Tcl_SetResult(interp, nodeID, TCL_VOLATILE);
    }

    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_CallAddFastStartMessage
 * purpose : adding an FS channel to the setup message.
 * syntax  : api:cm:FastStartBuild <handle> <nodeID>
 *           <handle>   - handle of the call
 *           <nodeID>   - node ID of the built channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK or TCL_ERROR
 ********************************************************************************************/
int api_cm_CallAddFastStartMessage(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    int nodeID, status = RV_ERROR_UNKNOWN;

    RV_UNUSED_ARG(clientData);

    sscanf(argv[1], "%p", &call);
    nodeID = atoi(argv[2]);

    if(nodeID >= 0)
        status = cmCallAddFastStartMessage(call->hsCall, nodeID);

    return WrapperReply(interp, status, argc, argv);
}



/********************************************************************************************
 * api_app_FSGetChanNum
 * purpose :
 * syntax  : api:app:FSGetChanNum <fsMessage>
 *           <fsMessage> - pointer to the FS message
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_FSGetChanNum(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    cmFastStartMessage *fsMessage;
    char numChan[16];

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:app:FSGetChanNum <fsMessage>");

    sscanf(argv[1], "%p", &fsMessage);
    sprintf(numChan, "%d", fsMessage->partnerChannelsNum);

    Tcl_SetResult(interp, numChan, TCL_VOLATILE);
    return TCL_OK;
}

/********************************************************************************************
 * api_app_FSGetAltChanNum
 * purpose :
 * syntax  : api:app:FSGetAltChanNum <fsMessage> <chanNum> <chanDir>
 *           <fsMessage> - pointer to the FS message
 *           <chanNum> - partnerChannel number
 *           <chanDir> - partnerChannel direction
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_FSGetAltChanNum(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    cmFastStartMessage *fsMessage;
    int chanNum;
    char altChanNum[16];

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:app:FSGetAltChanNum <fsMessage> <chanNum><chanDir>");

    sscanf(argv[1], "%p", &fsMessage);
    chanNum = atoi(argv[2]);
    if(argv[3][0] == 't')
        sprintf(altChanNum, "%d", fsMessage->partnerChannels[chanNum].transmit.altChannelNumber);
    if(argv[3][0] == 'r')
        sprintf(altChanNum, "%d", fsMessage->partnerChannels[chanNum].receive.altChannelNumber);
    Tcl_SetResult(interp, altChanNum, TCL_VOLATILE);
    return TCL_OK;
}

/********************************************************************************************
 * api_app_FSGetChanIndex
 * purpose :
 * syntax  : api:app:FSGetChanIndex <fsMessage> <chanNum> <altChan> <chanDir>
 *           <fsMessage> - pointer to the FS message
 *           <chanNum> - partnerChannel number
 *           <chanDir> - partnerChannel direction
 *           <altChan> - alternateChannel number
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_FSGetChanIndex(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    cmFastStartMessage *fsMessage;
    int chanNum, altChanNum;
    char chanIndex[16];

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:app:FSGetChanIndex <fsMessage> <chanNum> <chanDir> <altChan>");

    sscanf(argv[1], "%p", &fsMessage);
    chanNum = atoi(argv[2]);
    altChanNum = atoi(argv[4]);
    if(argv[3][0] == 'r')
        sprintf(chanIndex, "%d", fsMessage->partnerChannels[chanNum].receive.channels[altChanNum].index);
    if(argv[3][0] == 't')
        sprintf(chanIndex, "%d", fsMessage->partnerChannels[chanNum].transmit.channels[altChanNum].index);
    Tcl_SetResult(interp, chanIndex, TCL_VOLATILE);
    return TCL_OK;
}

/********************************************************************************************
 * api_app_FSGetChanName
 * purpose :
 * syntax  : api:app:FSGetChanName <fsMessage> <chanNum> <chanDir> <altChan>
 *           <fsMessage> - pointer to the FS message
 *           <chanNum> - partnerChannel number
 *           <chanDir> - partnerChannel direction
 *           <altChan> - alternateChannel number
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_FSGetChanName(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    cmFastStartMessage *fsMessage;
    int chanNum, altChanNum;

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:app:FSGetChanName <fsMessage> <chanNum> <chanDir> <altChan>");

    sscanf(argv[1], "%p", &fsMessage);
    chanNum = atoi(argv[2]);
    altChanNum = atoi(argv[4]);
    if(argv[3][0] == 'r') Tcl_SetResult(interp,
        fsMessage->partnerChannels[chanNum].receive.channels[altChanNum].channelName,
        TCL_VOLATILE);
    if(argv[3][0] == 't') Tcl_SetResult(interp,
        fsMessage->partnerChannels[chanNum].transmit.channels[altChanNum].channelName,
        TCL_VOLATILE);
    return TCL_OK;
}

/********************************************************************************************
 * api_app_FSGetChanRTCP
 * purpose :
 * syntax  : api:app:FSGetChanRTCP <fsMessage> <chanNum> <chanDir> <altChan>
 *           <fsMessage> - pointer to the FS message
 *           <chanNum> - partnerChannel number
 *           <chanDir> - partnerChannel direction
 *           <altChan> - alternateChannel number
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_FSGetChanRTCP(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    cmFastStartMessage *fsMessage;
    int chanNum, altChanNum;
    char rtcpString[32];

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:app:FSGetChanRTCP <fsMessage> <chanNum> <chanDir> <altChan>");

    sscanf(argv[1], "%p", &fsMessage);
    chanNum = atoi(argv[2]);
    altChanNum = atoi(argv[4]);
    if(argv[3][0] == 'r')
        cmTransportAddress2String(&fsMessage->partnerChannels[chanNum].receive.channels[altChanNum].rtcp,
            sizeof(rtcpString), rtcpString);
    if(argv[3][0] == 't')
        cmTransportAddress2String(&fsMessage->partnerChannels[chanNum].transmit.channels[altChanNum].rtcp,
            sizeof(rtcpString), rtcpString);
    Tcl_SetResult(interp, rtcpString, TCL_VOLATILE);
    return TCL_OK;
}

/********************************************************************************************
 * api_app_FSGetChanRTP
 * purpose :
 * syntax  : api:app:FSGetChanRTP <fsMessage> <chanNum> <chanDir> <altChan>
 *           <fsMessage> - pointer to the FS message
 *           <chanNum> - partnerChannel number
 *           <chanDir> - partnerChannel direction
 *           <altChan> - alternateChannel number
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_app_FSGetChanRTP(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    cmFastStartMessage * fsMessage;
    int chanNum, altChanNum;
    char rtpString[32];

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:app:FSGetChanRTP <fsMessage> <chanNum> <chanDir> <altChan>");

    sscanf(argv[1], "%p", &fsMessage);
    chanNum = atoi(argv[2]);
    altChanNum = atoi(argv[4]);
    if(argv[3][0] == 'r')
        cmTransportAddress2String(&fsMessage->partnerChannels[chanNum].receive.channels[altChanNum].rtp,
            sizeof(rtpString), rtpString);
    if(argv[3][0] == 't')
        cmTransportAddress2String(&fsMessage->partnerChannels[chanNum].transmit.channels[altChanNum].rtp,
            sizeof(rtpString), rtpString);
    Tcl_SetResult(interp, rtpString, TCL_VOLATILE);
    return TCL_OK;
}


/********************************************************************************************
 * api_cm_FastStartChannelsAck
 * purpose : wrapping cmFastStartChannelsAck.
 *           acknowledging a faststart channel from the fsMessage struct.
 * syntax  : api:cm:FastStartChannelsAck <handle> <fsMessage> <chanNum> <chanDir> <altChan>
 *           <handle> - handle of the call
 *           <fsMessage> - pointer to the FS message
 *           <chanNum> - partnerChannel number
 *           <chanDir> - partnerChannel direction
 *           <altChan> - alternateChannel number
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_FastStartChannelsAck(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    cmFastStartMessage *fsMessage;
    int chanNum, altChanNum;
    int status = RV_ERROR_UNKNOWN;
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);

    if (argc != 6)
        return WrapperBadParams(interp, "api:cm:FastStartChannelsAck <handle> <fsMessage> <chanNum> <chanDir> <altChan>");

    sscanf(argv[1], "%p", &call);
    sscanf(argv[2], "%p", &fsMessage);
    chanNum = atoi(argv[3]);
    altChanNum = atoi(argv[5]);
    if(argv[4][0] == 'r')
        status = cmFastStartChannelsAck(call->hsCall, &fsMessage->partnerChannels[chanNum].receive.channels[altChanNum]);
    if(argv[4][0] == 't')
        status = cmFastStartChannelsAck(call->hsCall, &fsMessage->partnerChannels[chanNum].transmit.channels[altChanNum]);

    return TCL_OK;
}

/********************************************************************************************
 * api_cm_FastStartChannelsAckIndex
 * purpose : wrapping cmFastStartChannelsAckIndex.
 *           acknowledging a faststart channel by index.
 * syntax  : api:cm:FastStartChannelsAckIndex <handle> <index> <rtcp> <rtp>
 *           <handle> - handle of the call
 *           <index> - index of the channel to be acknowledged
 *           <rtcp> - RTCP address for the channel
 *           <rtp> - RTP address for the channel
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_FastStartChannelsAckIndex(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int index, status;
    EpCallObj *call;
    cmTransportAddress rtp, rtcp;

    RV_UNUSED_ARG(clientData);

    if (argc != 5)
        return WrapperBadParams(interp, "api:cm:FastStartChannelsAckIndex <handle> <index> <rtcp> <rtp>");

    sscanf(argv[1], "%p", &call);
    index = atoi(argv[2]);
    cmString2TransportAddress(argv[3], &rtcp);
    cmString2TransportAddress(argv[4], &rtp);
    status = epCallFaststartAckIndex(call, index, &rtp, &rtcp);

    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
* api_cm_FastStartChannelsReady
* purpose : wrapping cmFastStartChannelsReady.
*           notifys the cm that no more FS channels will be acked.
* syntax  : api:cm:FastStartChannelsReady <handle>
*           <handle> - handle of the call
* input   : clientData - used for creating new command in tcl
*           interp - interpreter for tcl commands
*           argc - number of parameters entered to the new command
*           argv - the parameters entered to the tcl command
* output  : none
* return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
********************************************************************************************/
int api_cm_FastStartChannelsReady(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int status;
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
        return WrapperBadParams(interp, "api:cm:FastStartChannelsAckIndex <handle>");

    sscanf(argv[1], "%p", &call);
    status = epCallFaststartReady(call, RV_TRUE);

    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
* api_cm_CallSendSupplementaryService
* purpose : wrapping cmCallSendSupplementaryService.
*           Sends H450 message on a call.
* syntax  : api:cm:CallSendSupplementaryService <handle> <buffer> <force>
*           <handle> - handle of the call
*           <buffer> - buffer to send
*           <force>  - force message sending or not
* input   : clientData - used for creating new command in tcl
*           interp - interpreter for tcl commands
*           argc - number of parameters entered to the new command
*           argv - the parameters entered to the tcl command
* output  : none
* return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
********************************************************************************************/
int api_cm_CallSendSupplementaryService(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int status;
    char buf[1024];
    EpCallObj *call;

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:cm:CallSendSupplementaryService <handle> <buffer> <force>");

    sscanf(argv[1], "%p", &call);
    hex2str(argv[2], buf, sizeof(buf));
    status = cmCallSendSupplementaryService(call->hsCall, buf, strlen(argv[2])/2, (RvBool)atoi(argv[3]));

    return WrapperReply(interp, status, argc, argv);
}

/********************************************************************************************
 * api_cm_CallSimulateMessage
 * purpose : wrapping cmCallSimulateMessage.
 *           Simulates a received message on a call.
 * syntax  : api:cm:CallSimulateMessage <handle> <nodeId>
 *           <handle> - handle of the call
 *           <nodeId> - Node ID to "received"
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully or TCL_ERROR if not.
 ********************************************************************************************/
int api_cm_CallSimulateMessage(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int status;
    EpCallObj *call;
    int nodeId;

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:cm:CallSimulateMessage <handle> <nodeId>");

    sscanf(argv[1], "%p", &call);
    nodeId = atoi(argv[2]);
    status = cmCallSimulateMessage(call->hsCall, nodeId);

    return WrapperReply(interp, status, argc, argv);
}


/********************************************************************************************
 * api_cm_CallCreateAnnexMMessage
 * purpose : Create an annex M message
 * syntax  : Call.SendAnnexMMessage <requiered> <objectID> <altIDType> <altIDVariant> <subID> <nsdString> <messages>...
 *           <callInfo> - Call information (counter and handle)
 *           <requiered> - message needs to be understood by host
 *           <force> - send it now or wait for message to be sent
 *           <objectID> <altIDType> <altIDVariant> <subID> - components of the cmTunnelledProtocolID struct
 *           <nsdString> - formatted string containing the non standard data
 *           <messages> - the messages to send, each message in an arg.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int api_cm_CallCreateAnnexMMessage(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvBool requiered;
    cmTunnelledProtocolID tunnelledProtocolID;
    cmNonStandardParam  nsParam;
    cmOctetString message[10];
    int i, retVal;
    char retStr[32];

    if (argc != 17)
        return WrapperBadParams(interp, "api:cm:api_cm_SendAnnexMMessage <requiered> <objectID> <altIDType> <altIDVariant> <subID> <nsdString> <messages>...");

    requiered = (argv[1][0] == '1');

    strcpy(tunnelledProtocolID.tunnelledProtocolObjectID, argv[2]);
    strcpy(tunnelledProtocolID.protocolType, argv[3]);
    tunnelledProtocolID.protocolTypeLength = strlen(argv[3]);
    strcpy(tunnelledProtocolID.protocolVariant, argv[4]);
    tunnelledProtocolID.protocolVariantLength = strlen(argv[4]);
    strcpy(tunnelledProtocolID.subIdentifier, argv[5]);
    tunnelledProtocolID.subIdentifierLength = strlen(argv[5]);

    String2CMNonStandardParam(argv[6], &nsParam);

    for(i=0; i<10; i++)
    {
        if(argv[i+7][0])
        {
            message[i].message = argv[i+7];
            message[i].size = strlen(argv[i+7]);
        }
        else
        {
            message[i].message = NULL;
            message[i].size = 0;
            break;
        }
    }

    retVal = cmCallCreateAnnexMMessage(ep->hApp,requiered,&tunnelledProtocolID,message,&nsParam);
    sprintf(retStr, "%d", retVal);
    Tcl_SetResult(interp, retStr, TCL_VOLATILE);

    return TCL_OK;
}

/********************************************************************************************
 * api_cm_CallSendAnnexMMessage
 * purpose : Send an annex M message
 * syntax  : Call.SendAnnexMMessage <callInfo> <force> <AnnexMmessage>
 *           <callInfo> - Call information (counter and handle)
 *           <force> - send it now or wait for message to be sent
 *           <AnnexMmessage> - value returned by CreateAnnexMMessage
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int api_cm_CallSendAnnexMMessage(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    RvBool force;
    int AnnexMmessage;
    RvStatus status;

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:cm:api_cm_SendAnnexMMessage <callInfo> <force> <AnnexMmessage>");

    if (sscanf(argv[1], "%p", &call) != 1)
        return TCL_ERROR;

    AnnexMmessage = atoi(argv[3]);

    force = (argv[2][0] == '1');

    status = cmCallSendAnnexMMessage(call->hsCall, AnnexMmessage ,force);
    if (status != RV_OK)
        pvtDelete(call->ep->hVal, AnnexMmessage);

    return TCL_OK;
}

/********************************************************************************************
 * api_cm_CallCreateAnnexLMessage
 * purpose : Create an annex L message
 * syntax  : Call.SendAnnexMMessage <isText> <nsdString> <message>
 *           <isText> - message is text
 *           <nsdString> - formatted string containing the non standard data
 *           <message> - the message text to send.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int api_cm_CallCreateAnnexLMessage(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvBool isText;
    cmNonStandardParam  nsParam;
    int retVal;
    char retStr[32];

    if (argc != 4)
        return WrapperBadParams(interp, "api:cm:api_cm_SendAnnexLMessage <callInfo> <isText> <force> <nsdString> <messages>");

    isText = (argv[1][0] == '1');

    String2CMNonStandardParam(argv[2], &nsParam);

    retVal = cmCallCreateAnnexLMessage(ep->hApp, isText, argv[3], strlen(argv[3]), &nsParam);
    sprintf(retStr, "%d", retVal);
    Tcl_SetResult(interp, retStr, TCL_VOLATILE);

    return TCL_OK;
}

/********************************************************************************************
 * api_cm_CallSendAnnexLMessage
 * purpose : Send an annex L message
 * syntax  : Call.SendAnnexLMessage <callInfo> <force> <AnnexLmessage>
 *           <callInfo> - Call information (counter and handle)
 *           <force> - send it now or wait for message to be sent
 *           <AnnexLmessage> - the message created by CreateAnnexLMessage.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ********************************************************************************************/
int api_cm_CallSendAnnexLMessage(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    RvBool force;
    int AnnexLMessage;
    RvStatus status;

    RV_UNUSED_ARG(clientData);

    if (argc != 4)
        return WrapperBadParams(interp, "api:cm:api_cm_SendAnnexLMessage <callInfo> <force> <AnnexLmessage>");

    if (sscanf(argv[1], "%p", &call) != 1)
        return TCL_ERROR;
    force = (argv[2][0] == '1');
    AnnexLMessage = atoi(argv[3]);

    status = cmCallSendAnnexLMessage(call->hsCall, AnnexLMessage, force);
    if (status != RV_OK)
        pvtDelete(call->ep->hVal, AnnexLMessage);

    return TCL_OK;
}


#if defined(USE_H450)
/********************************************************************************************
 * api_h450_SendNameId
 * purpose : Send an name ID message tunneled in the next Q.931 message
 * syntax  : api:h450:SendNameId <callInfo> <operation>
 *           <callInfo> - Call information (counter and handle)
 *           <operation> - type of name ID (connecting, alerting, etc.)
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK or TCL_ERROR
 ********************************************************************************************/
int api_h450_SendNameId(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpCallObj *call;
    niOperation op = calling;

    RV_UNUSED_ARG(clientData);

    if (argc != 3)
        return WrapperBadParams(interp, "api:h450:SendNameId <callInfo> <operation>");

    sscanf(argv[1], "%p", &call);

    switch (argv[2][0])
    {
    case ('c'):
        if (argv[2][1] == 'a')
            op = calling;
        if (argv[2][1] == 'o')
            op = connected;
        break;
    case ('a'):
        op = alerting;
        break;
    case ('b'):
        op = busy;
        break;
    default:
        return TCL_ERROR;
    }

    epH450CallSendNameID(call, op);
    return TCL_OK;
}
#endif



#ifdef __cplusplus
}
#endif


