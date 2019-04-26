/***********************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
 *                                init.c
 *
 * This file include functions that initiate the stack and the tcl.
 *
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

#include "stkutils.h"

#include "lep.h"
#include "lepRtp.h"
#include "lepH450.h"
#include "lepSecurity.h"
#include "lepLdap.h"
#include "lepFwNat.h"
#include "lepConversions.h"

#include "TAP_h450.h"
#include "TAP_security.h"
#include "TAP_wrapper.h"
#include "TAP_call.h"
#include "TAP_channel.h"
#include "TAP_log.h"
#include "TAP_snmp.h"
#include "TAP_ras.h"
#include "TAP_tools.h"
#include "TAP_init.h"


#define TCL_FILENAME    "TAP_testapp.tcl"
#define TCL_LIBPATH     "lib/tcl8.3"
#define TK_LIBPATH      "lib/tk8.3"

#ifdef WIN32
#define CONFIG_FILE     ".\\config.val"
#else
#define CONFIG_FILE     "./config.val"
#endif


/* Endpoint object used */
static EpObj ep;

Tcl_Interp * gInterp;

#ifndef USE_TCL
TclFuncRefLink * pFunctionList;
#endif

RvBool       LogWrappers;



/********************************************************************************************
 *                                                                                          *
 *                                  Private functions                                       *
 *                                                                                          *
 ********************************************************************************************/


/******************************************************************************
 * epAllocateResourceId
 * ----------------------------------------------------------------------------
 * General: Request user to allocate a resource identifier for an object.
 *
 * Return Value: Allocated resource on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         resourceType - Type of resource we need id for.
 * Output: None.
 *****************************************************************************/
static RvInt32 epAllocateResourceId(
    IN EpObj                *ep,
    IN EpResourceType       resourceType)
{
    static RvInt32 resCount[EpResourceLast] = {0};
    RvInt32 resValue = 0;

    RV_UNUSED_ARG(ep);

    switch (resourceType)
    {
    case EpResourceChannel:
        TclExecute("incr tmp(totalChannels)");
        resCount[(int)resourceType]++;
        resValue = resCount[(int)resourceType];
        break;

    case EpResourceCall:
        Call_UpdateCounters(1); /* Increase total */
        resCount[(int)resourceType]++;
        resValue = resCount[(int)resourceType];
        break;

    case EpResourceCompletionService:
        resCount[(int)resourceType]++;
        resValue = resCount[(int)resourceType];
        TclExecute("set tmp(record,cnt,compService) %u", resValue);
        break;

    case EpResourceInConnection:
        resCount[(int)resourceType]++;
        resValue = resCount[(int)resourceType];
        TclExecute("set tmp(record,cnt,conn,in) %u", resValue);
        break;

    case EpResourceOutConnection:
        resCount[(int)resourceType]++;
        resValue = resCount[(int)resourceType];
        TclExecute("set tmp(record,cnt,conn,out) %u", resValue);
        break;

    default:
        break;
    }

    return resValue;
}

#ifdef USE_TCL

/********************************************************************************************
 * tclGetFile
 * purpose : This function is automatically generated with the tcl scripts array in
 *           TAP_scripts.tcl.
 *           It returns the buffer holding the .tcl file information
 * input   : name   - Name of file to load
 * output  : none
 * return  : The file's buffer if found
 *           NULL if file wasn't found
 ********************************************************************************************/
char* tclGetFile(IN char* name);


/********************************************************************************************
 * test_Source
 * purpose : This function replaces the "source" command of the TCL
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int test_Source(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    FILE* exists;
    char* fileBuf;

    RV_UNUSED_ARG(clientData);

    if (argc != 2)
    {
        Tcl_SetResult(interp, (char *)"wrong # args: should be \"source <filename>\"", TCL_STATIC);
        return TCL_ERROR;
    }

    /* First see if we've got such a file on the disk */
    exists = fopen(argv[1], "r");
    if (exists == NULL)
    {
        /* File doesn't exist - get from compiled array */
        fileBuf = tclGetFile(argv[1]);
        if (fileBuf == NULL)
        {
            /* No such luck - we don't have a file to show */
            char error[300];
            sprintf(error, "file %s not found", argv[1]);
            Tcl_SetResult(interp, error, TCL_VOLATILE);
            return TCL_ERROR;
        }
        else
        {
            /* Found! */
            int retCode;
            retCode = Tcl_Eval(interp, fileBuf);
            if (retCode == TCL_ERROR)
            {
                char error[300];
                sprintf(error, "\n    (file \"%s\" line %d)", argv[1], interp->errorLine);
                Tcl_AddErrorInfo(interp, error);
            }
            return retCode;
        }
    }

    /* Close this file - we're going to read it */
    fclose(exists);

    /* File exists - evaluate from the file itself */
    return Tcl_EvalFile(interp, argv[1]);
}


/********************************************************************************************
 * test_File
 * purpose : This function replaces the "file" command of the TCL, to ensure that
 *           when checking if a file exists, we also look inside our buffers.
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int test_File(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int i, retCode;
    Tcl_DString str;

    RV_UNUSED_ARG(clientData);

    if ((argc == 3) && (strncmp(argv[1], "exis", 4)) == 0)
    {
        /* "file exist" command - overloaded... */
        if (tclGetFile(argv[2]) != NULL)
        {
            Tcl_SetResult(interp, (char *)"1", TCL_STATIC);
            return TCL_OK;
        }
    }

    /* Continue executing the real "file" command */
    Tcl_DStringInit(&str);
    Tcl_DStringAppendElement(&str, "fileOverloaded");
    for(i = 1; i < argc; i++)
        Tcl_DStringAppendElement(&str, argv[i]);
    retCode = Tcl_Eval(interp, Tcl_DStringValue(&str));
    Tcl_DStringFree(&str);
    return retCode;
}

#endif

/********************************************************************************************
 * test_Quit
 * purpose : This function is called when the test application is closed from the GUI.
 *           It is responsible for closing the application gracefully.
 * syntax  : test.Quit
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int test_Quit(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    TclExecute("test:Log {Shutting down the application};update");
    TclExecute("init:SaveOptions 0");

    /* We should kill the stack only after we save the application's options */
    EndStack();

    /* Finish with the interpreter */
#ifdef USE_TCL
    Tcl_DeleteInterp(interp);
#endif
    exit(0);
    return TCL_OK;
}



/********************************************************************************************
 * test_Rebind
 * purpose : This function binds all listening addresses used.
 * syntax  : test.Rebind
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int test_Rebind(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    epRebind(&ep);
    TclExecute("test:updateGui 0");

    return TCL_OK;
}


/********************************************************************************************
 * test_Reset
 * purpose : This function resets global information it the endpoint object.
 * syntax  : test.Reset
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int test_Reset(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    epReset(&ep);

    return TCL_OK;
}


/********************************************************************************************
 * test_Restart
 * purpose : This function restarts the stack while in the middle of execution.
 * syntax  : test.Restart
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int test_Restart(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    EndStack();

    TclExecute("after 5000 test.Init");

    return TCL_OK;
}


/********************************************************************************************
 * test_Init
 * purpose : This function initialized the stack as part of the restart operation.
 *           It should not be called from TCL scripts, but from test_Restart only
 * syntax  : test.Restart
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int test_Init(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    if (InitStack(NULL) < 0)
    {
        TclExecute("msgbox Error picExclamation {Unable to restart the stack. Stopping the application} { { Ok -1 <Key-Return> } }");
        TclExecute("test.Quit");
    }

    TclExecute("foreach tmpv $tmp(tvars) {set app($tmpv) $app($tmpv)};unset tmpv");

    TclExecute("test:updateGui 1");
    TclExecute("msgbox Restart picInformation {Stack restarted successfully} { { Ok -1 <Key-Return> } }");

    return TCL_OK;
}


#if 0
/********************************************************************************************
 * BooleanStr
 * purpose : Return a string representing a boolean value
 * input   : value  - The value to convert
 * output  : none
 * return  : String value of the boolean
 ********************************************************************************************/
static const RvChar* BooleanStr(IN RvBool value)
{
    switch (value)
    {
        case RV_TRUE:  return "1";
        case RV_FALSE: return "0";
        default:    return "-? ERROR ?-";
    }
}
#endif

#ifdef USE_TCL

/********************************************************************************************
 * test_Support
 * purpose : This function check for support of specific stack functionality
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int test_Support(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(argc);

    if (strcmp(argv[0], "test.NetSupported") == 0)
    {
#if (RV_NET_TYPE == RV_NET_IPV4)
        Tcl_SetResult(interp, (char*)"IPv4 only", TCL_STATIC);
#elif (RV_NET_TYPE & RV_NET_IPV6)
        Tcl_SetResult(interp, (char*)"IPv4/IPv6", TCL_STATIC);
#else
        Tcl_SetResult(interp, (char*)"0", TCL_STATIC);
#endif
        return TCL_OK;
    }

#ifdef USE_RTP
    if (strcmp(argv[0], "test.RtpSupported") == 0)
    {
        Tcl_SetResult(interp, (char*)epRtpVersion(ep.rtp), TCL_VOLATILE);
        return TCL_OK;
    }
#endif

#ifdef USE_SECURITY
    if (strcmp(argv[0], "test.SecSupported") == 0)
    {
        Tcl_SetResult(interp, (char*)epSecurityVersion(&ep), TCL_VOLATILE);
        return TCL_OK;
    }
#endif

#ifdef USE_H450
    if (strcmp(argv[0], "test.H450Supported") == 0)
    {
        Tcl_SetResult(interp, (char*)epH450Version(&ep), TCL_VOLATILE);
        return TCL_OK;
    }
#endif

    if (strcmp(argv[0], "test.LdapSupported") == 0)
    {
        Tcl_SetResult(interp, (char*)epLdapVersion(&ep), TCL_VOLATILE);
        return TCL_OK;
    }

#if 0
    if (strcmp(argv[0], "test.SnmpSupported") == 0)
    {
        Tcl_SetResult(interp, (char*)BooleanStr(SNMP_IsInitialized()), TCL_VOLATILE);
        return TCL_OK;
    }
#endif


#ifdef USE_FWNAT
    if (strcmp(argv[0], "test.H460Supported") == 0)
    {
        Tcl_SetResult(interp, (char*)epFwNatVersion(&ep), TCL_VOLATILE);
        return TCL_OK;
    }
#endif
    /* Not compiled with this support at all... */
    Tcl_SetResult(interp, (char*)"0", TCL_STATIC);
    return TCL_OK;
}

#endif

/********************************************************************************************
 * setIcon
 * purpose : Set the icon for the Windows test application
 * input   : clientData - used for creating new command in tcl
 *           interp - interpreter for tcl commands
 *           argc - number of parameters entered to the new command
 *           argv - the parameters entered to the tcl command
 * output  : none
 * return  : TCL_OK
 ********************************************************************************************/
int setIcon(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
#if 0
    HWND        hWnd;
    HINSTANCE   hInstance;
    HICON       hIcon;

    if (sscanf(argv[1], "0x%p", &hWnd) == 1)
    {
        hInstance = GetModuleHandle(NULL);
        hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RVSN_LOGO));
        SetClassLong(hWnd, GCL_HICONSM, (LONG)hIcon);
        SetClassLong(hWnd, GCL_HICON, (LONG)hIcon);
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }

#endif  /* WIN32 */
    return TCL_OK;
}





#ifdef USE_TCL

#define WRAPPER_COMMAND(tclName, cName) \
{                                                                                                           \
    static TclWrapperData _data;                                                                            \
    _data.ep = &ep;                                                                                         \
    _data.proc = (Tcl_CmdProc*)cName;                                                                                     \
    Tcl_CreateCommand(gInterp, (char *)tclName, (Tcl_CmdProc*)WrapperFunc, (ClientData)&_data, (Tcl_CmdDeleteProc *)NULL); \
}

#define CREATE_COMMAND(tclName, cName) \
    Tcl_CreateCommand(gInterp, (char *)tclName, (Tcl_CmdProc*)cName, (ClientData)&ep, (Tcl_CmdDeleteProc *)NULL)

#else

#define WRAPPER_COMMAND(tclName, cName) \
{ \
    static TclWrapperData _data; \
    static TclFuncRefLink _link; \
    _data.ep = &ep; \
    _data.proc = cName; \
    _link.clientData = (ClientData)&_data; \
    _link.pCmd = WrapperFunc; \
    strncpy(_link.cmdName, tclName, sizeof(_link.cmdName)); \
    _link.next = pFunctionList; \
    pFunctionList = &_link; \
    Tcl_CreateCommand(gInterp, (char *)tclName, WrapperFunc, (ClientData)&_data, (Tcl_CmdDeleteProc *)NULL); \
}

#define CREATE_COMMAND(tclName, cName) \
{ \
    static TclFuncRefLink _link; \
    _link.clientData = (ClientData)&ep; \
    _link.pCmd = cName; \
    strncpy(_link.cmdName, tclName, sizeof(_link.cmdName)); \
    _link.next = pFunctionList; \
    pFunctionList = &_link; \
    Tcl_CreateCommand(gInterp, (char *)tclName, cName, (ClientData)&ep, (Tcl_CmdDeleteProc *)NULL); \
}

#endif

/********************************************************************************************
 * CreateTclCommands
 * purpose : Create all tcl commands that was written in c language.
 * input   : interp - interpreter for tcl commands
 * output  : none
 * return  : none
 ********************************************************************************************/
void CreateTclCommands(Tcl_Interp* interp)
{
    /******************
     * CALLS: Creation
     ******************/
    CREATE_COMMAND("Call.Info", Call_Info);
    CREATE_COMMAND("Call.Dial", Call_Dial);
    CREATE_COMMAND("Call.Make", Call_Make);
    CREATE_COMMAND("Call.FastStartDial", Call_FastStartDial);
    CREATE_COMMAND("Call.FastStartMake", Call_FastStartMake);
    CREATE_COMMAND("Call.Drop", Call_Drop);
    CREATE_COMMAND("Call.DropAll", Call_DropAll);
    CREATE_COMMAND("Call.SendAlerting", Call_SendAlerting);
    CREATE_COMMAND("Call.SendCallProceeding", Call_SendCallProceeding);
    CREATE_COMMAND("Call.SendConnect", Call_SendConnect);

    /*******************
     * CALLS: Faststart
     *******************/
    CREATE_COMMAND("Call.OpenOutgoingFaststart", Call_OpenOutgoingFaststart);
    CREATE_COMMAND("Call.BuildOutgoingFaststart", Call_BuildOutgoingFaststart);
    CREATE_COMMAND("Call.ApproveFaststart", Call_ApproveFaststart);
    CREATE_COMMAND("Call.RefuseFaststart", Call_RefuseFaststart);
    CREATE_COMMAND("Call.DropAllFastStart", Call_DropAllFastStart);
    CREATE_COMMAND("Call.ConnectControl", Call_ConnectControl);

    /**************
     * CALLS: Misc
     **************/
    CREATE_COMMAND("Call.SetRate", Call_SetRate);
    CREATE_COMMAND("Call.SendStatusInquiry", Call_SendStatusInquiry);
    CREATE_COMMAND("Call.SendProgress", Call_SendProgress);
    CREATE_COMMAND("Call.SendNotify", Call_SendNotify);
    CREATE_COMMAND("Call.SendUserInformation", Call_SendUserInformation);
    CREATE_COMMAND("Call.SendFacility", Call_SendFacility);
    CREATE_COMMAND("Call.SeperateControl", Call_SeperateControl);
    CREATE_COMMAND("Call.CloseControl", Call_CloseControl);
    CREATE_COMMAND("Call.SendCaps", Call_SendCaps);
    CREATE_COMMAND("Call.SendEmptyCaps", Call_SendEmptyCaps);
    CREATE_COMMAND("Call.SendCapsAck", Call_SendCapsAck);
    CREATE_COMMAND("Call.SendMSD", Call_SendMSD);
    CREATE_COMMAND("Call.SendRoundTrip", Call_SendRoundTrip);
    CREATE_COMMAND("Call.CreateH245", Call_CreateH245);
    CREATE_COMMAND("Call.UII", Call_UII);
    CREATE_COMMAND("Call.LoopOff", Call_LoopOff);
    CREATE_COMMAND("Multiplex.Update", Multiplex_Update);
    CREATE_COMMAND("EFC.Set", EFC_Set);
    CREATE_COMMAND("RAS.SendBRQ", RAS_SendBRQ);
    CREATE_COMMAND("RAS.SendNSM", RAS_SendNSM);
    CREATE_COMMAND("RAS.SendRAI", RAS_SendRAI);

    /*************************
     * CALLS: Overlap sending
     *************************/
    CREATE_COMMAND("Call.IncompleteAddress", Call_IncompleteAddress);
    CREATE_COMMAND("Call.AddressComplete", Call_AddressComplete);
    CREATE_COMMAND("Call.SendAdditionalAddr", Call_SendAdditionalAddr);

    /***********
     * CHANNELS
     ***********/
    CREATE_COMMAND("Channel.ConnectOutgoing", Channel_ConnectOutgoing);
    CREATE_COMMAND("Channel.OpenOutgoingWindow", Channel_OpenOutgoingWindow);
    CREATE_COMMAND("Channel.DisplayChannelList", Channel_DisplayChannelList);
    CREATE_COMMAND("Channel.ResponseForOLC", Channel_ResponseForOLC);
    CREATE_COMMAND("Channel.ResponseForCLC", Channel_ResponseForCLC);
    CREATE_COMMAND("Channel.Drop", Channel_Drop);
    CREATE_COMMAND("Channel.Answer", Channel_Answer);
    CREATE_COMMAND("Channel.MediaLoop", Channel_MediaLoop);
    CREATE_COMMAND("Channel.Rate", Channel_Rate);
    CREATE_COMMAND("Channel.FastUpdate", Channel_FastUpdate);

    /********
     * TOOLS
     ********/
    CREATE_COMMAND("test.Quit", test_Quit);
    CREATE_COMMAND("test.Reset", test_Reset);
    CREATE_COMMAND("test.Rebind", test_Rebind);
    CREATE_COMMAND("test.Restart", test_Restart);
    CREATE_COMMAND("test.Init", test_Init);
#ifdef USE_TCL
    CREATE_COMMAND("test.NetSupported", test_Support);
    CREATE_COMMAND("test.RtpSupported", test_Support);
    CREATE_COMMAND("test.SnmpSupported", test_Support);
    CREATE_COMMAND("test.SecSupported", test_Support);
    CREATE_COMMAND("test.H450Supported", test_Support);
    CREATE_COMMAND("test.OspSupported", test_Support);
    CREATE_COMMAND("test.LdapSupported", test_Support);
    CREATE_COMMAND("test.H460Supported", test_Support);
#endif
    CREATE_COMMAND("Log.Update", Log_Update);
    CREATE_COMMAND("Log:Reset", Log_Reset);
    CREATE_COMMAND("Options.GetLocalIP", Options_GetLocalIP);
    CREATE_COMMAND("Options.Crash", Options_Crash);
    CREATE_COMMAND("Status.Display", Status_Display);
    CREATE_COMMAND("status:Check", Status_Check);
    CREATE_COMMAND("status:Pvt", Status_Pvt);
    CREATE_COMMAND("setIcon", setIcon);
    CREATE_COMMAND("LogFile:Reset", LogFile_Reset);

    /******
     * H450
     ******/
#ifdef USE_H450
    CREATE_COMMAND("H450.CallMake", H450_callMake);
    CREATE_COMMAND("H450.callTransfer", H450_callTransfer);
    CREATE_COMMAND("H450.TransferNonDefaultResponse", H450_TransferNonDefaultResponse);
    CREATE_COMMAND("H450.callReroute", H450_callReroute);
    CREATE_COMMAND("H450.forwardRequest", H450_forwardRequest);
    CREATE_COMMAND("H450.forwardInterrogate", H450_forwardInterrogate);
    CREATE_COMMAND("H450.callHold", H450_callHold);
    CREATE_COMMAND("H450.callHoldRtrv", H450_callHoldRtrv);
    CREATE_COMMAND("H450.HoldSendNonDefaultResponse", H450_HoldSendNonDefaultResponse);
    CREATE_COMMAND("H450.callPark", H450_callPark);
    CREATE_COMMAND("H450.callPick", H450_callPick);
    CREATE_COMMAND("H450.callWait", H450_callWait);
    CREATE_COMMAND("H450.MC.Activate", H450_MC_Activate);
    CREATE_COMMAND("H450.SU.Interogate", H450_SU_Interogate);
    CREATE_COMMAND("H450.MWISendNonDefaultResponse", H450_MWISendNonDefaultResponse);
    CREATE_COMMAND("H450.callCompletion", H450_callCompletion);
    CREATE_COMMAND("H450.callCompletionAction", H450_callCompletionAction);
    CREATE_COMMAND("H450.callOffer", H450_callOffer);
    CREATE_COMMAND("H450.remoteUserAlerting", H450_remoteUserAlerting);
    CREATE_COMMAND("H450.callIntrusion", H450_callIntrusion);
    CREATE_COMMAND("H450.commonInformation", H450_CMN_Activate);
#endif

    /*******
     * LDAP
     *******/
    CREATE_COMMAND("LDAP.init", LDAP_init);
    CREATE_COMMAND("LDAP.end", LDAP_end);

    /************************
     * WRAPPER API FUNCTIONS
     ************************/
    Tcl_CreateCommand(interp, (char *)"script:cb", (Tcl_CmdProc*)CallbackFunc, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    WRAPPER_COMMAND("api:cm:CallNew", api_cm_CallNew);
    WRAPPER_COMMAND("api:cm:CallDial", api_cm_CallDial);
    WRAPPER_COMMAND("api:cm:CallProceeding", api_cm_CallProceeding);
    WRAPPER_COMMAND("api:cm:CallAccept", api_cm_CallAccept);
    WRAPPER_COMMAND("api:cm:CallDrop", api_cm_CallDrop);
    WRAPPER_COMMAND("api:cm:CallAccept", api_cm_CallAccept);
    WRAPPER_COMMAND("api:cm:CallAnswer", api_cm_CallAnswer);
    WRAPPER_COMMAND("api:cm:CallClose", api_cm_CallClose);
    WRAPPER_COMMAND("api:cm:CallSetParam", api_cm_CallSetParam);
    WRAPPER_COMMAND("api:cm:CallGetParam", api_cm_CallGetParam);
    WRAPPER_COMMAND("api:cm:CallForward", api_cm_CallForward);
    WRAPPER_COMMAND("api:cm:CallStatusInquiry", api_cm_CallStatusInquiry);
    WRAPPER_COMMAND("api:cm:CallConnectControl", api_cm_CallConnectControl);
    WRAPPER_COMMAND("api:cm:FastStartBuild", api_cm_FastStartBuild);
    WRAPPER_COMMAND("api:cm:CallAddFastStartMessage", api_cm_CallAddFastStartMessage);
    WRAPPER_COMMAND("api:cm:FastStartChannelsAck", api_cm_FastStartChannelsAck);
    WRAPPER_COMMAND("api:cm:FastStartChannelsAckIndex", api_cm_FastStartChannelsAckIndex);
    WRAPPER_COMMAND("api:cm:FastStartChannelsReady", api_cm_FastStartChannelsReady);
    WRAPPER_COMMAND("api:cm:CallSendSupplementaryService", api_cm_CallSendSupplementaryService);
    WRAPPER_COMMAND("api:cm:CallSimulateMessage", api_cm_CallSimulateMessage);

    WRAPPER_COMMAND("api:cm:ChannelNew", api_cm_ChannelNew);
    WRAPPER_COMMAND("api:cm:ChannelOpen", api_cm_ChannelOpen);
    WRAPPER_COMMAND("api:cm:ChannelAnswer", api_cm_ChannelAnswer);
    WRAPPER_COMMAND("api:cm:ChannelDrop", api_cm_ChannelDrop);
    WRAPPER_COMMAND("api:cm:ChannelRequestCloseReject", api_cm_ChannelRequestCloseReject);
    WRAPPER_COMMAND("api:cm:ChannelClose", api_cm_ChannelClose);
    WRAPPER_COMMAND("api:cm:ChannelGetCallHandle", api_cm_ChannelGetCallHandle);
    WRAPPER_COMMAND("api:cm:ChannelGetDependency", api_cm_ChannelGetDependency);
    WRAPPER_COMMAND("api:cm:ChannelGetDuplexAddress", api_cm_ChannelGetDuplexAddress);
    WRAPPER_COMMAND("api:cm:ChannelGetNumber", api_cm_ChannelGetNumber);
    WRAPPER_COMMAND("api:cm:ChannelGetOrigin", api_cm_ChannelGetOrigin);
    WRAPPER_COMMAND("api:cm:ChannelGetSource", api_cm_ChannelGetSource);
    WRAPPER_COMMAND("api:cm:ChannelGetIsDuplex", api_cm_ChannelIsDuplex);
    WRAPPER_COMMAND("api:cm:H245DeleteCapabilityMessage", api_cm_H245DeleteCapabilityMessage);

    /* Automatic RAS */
    WRAPPER_COMMAND("api:cm:Register", api_cm_Register);
    WRAPPER_COMMAND("api:cm:Unregister", api_cm_Unregister);

    /* Manual RAS */
    WRAPPER_COMMAND("api:cmras:StartTransaction", api_cmras_StartTransaction);
    WRAPPER_COMMAND("api:cmras:SetParam", api_cmras_SetParam);
    WRAPPER_COMMAND("api:cmras:GetParam", api_cmras_GetParam);
    WRAPPER_COMMAND("api:cmras:GetNumOfParams", api_cmras_GetNumOfParams);
    WRAPPER_COMMAND("api:cmras:Request", api_cmras_Request);
    WRAPPER_COMMAND("api:cmras:DummyRequest", api_cmras_DummyRequest);
    WRAPPER_COMMAND("api:cmras:Confirm", api_cmras_Confirm);
    WRAPPER_COMMAND("api:cmras:Reject", api_cmras_Reject);
    WRAPPER_COMMAND("api:cmras:InProgress", api_cmras_InProgress);
    WRAPPER_COMMAND("api:cmras:Close", api_cmras_Close);
    WRAPPER_COMMAND("api:cmras:GetTransaction", api_cmras_GetTransaction);

    /* General functions */
    WRAPPER_COMMAND("api:cm:Start", api_cm_Start);
    WRAPPER_COMMAND("api:cm:Stop", api_cm_Stop);
    WRAPPER_COMMAND("api:cm:GetVersion", api_cm_GetVersion);
    WRAPPER_COMMAND("api:cm:GetInterfaces", api_cm_GetInterfaces);
    WRAPPER_COMMAND("api:cm:Vt2Alias", api_cm_Vt2Alias);
    WRAPPER_COMMAND("api:cm:Alias2Vt", api_cm_Alias2Vt);
    WRAPPER_COMMAND("api:cm:GetProperty", api_cm_GetProperty);
    WRAPPER_COMMAND("api:cm:GetTpktChanHandle", api_cm_GetTpktChanHandle);
    WRAPPER_COMMAND("api:cm:ProtocolSendMessage", api_cm_ProtocolSendMessage);

    WRAPPER_COMMAND("api:cm:GetValTree", api_cm_GetValTree);
    WRAPPER_COMMAND("api:cm:GetRASConfigurationHandle", api_cm_GetRASConfigurationHandle);
    WRAPPER_COMMAND("api:cm:GetLocalRASAddress", api_cm_GetLocalRASAddress);
    WRAPPER_COMMAND("api:cm:GetLocalCallSignalAddress", api_cm_GetLocalCallSignalAddress);
    WRAPPER_COMMAND("api:cm:GetLocalAnnexEAddress", api_cm_GetLocalAnnexEAddress);
    WRAPPER_COMMAND("api:cm:GetParam", api_cm_GetParam);
    WRAPPER_COMMAND("api:cm:LogMessage", api_cm_LogMessage);
    WRAPPER_COMMAND("api:cm:Encode", api_cm_Encode);
    WRAPPER_COMMAND("api:cm:Decode", api_cm_Decode);

    /* Call Preservation functions */
    //WRAPPER_COMMAND("api:cm:CallGetBuffer", api_cm_CallGetBuffer);
    //WRAPPER_COMMAND("api:cm:CallAddBuffer", api_cm_CallAddBuffer);
    //WRAPPER_COMMAND("api:cm:ActivateCalls", api_cm_ActivateCalls);

    /* PVT functions */
    WRAPPER_COMMAND("api:pvt:AddRoot", api_pvt_AddRoot);
    WRAPPER_COMMAND("api:pvt:GetByPath", api_pvt_GetByPath);
    WRAPPER_COMMAND("api:pvt:BuildByPath", api_pvt_BuildByPath);
    WRAPPER_COMMAND("api:pvt:SetByPath", api_pvt_SetByPath);
    WRAPPER_COMMAND("api:pvt:SetTree", api_pvt_SetTree);
    WRAPPER_COMMAND("api:pvt:Delete", api_pvt_Delete);
    WRAPPER_COMMAND("api:pvt:Get", api_pvt_Get);
    WRAPPER_COMMAND("api:pvt:GetString", api_pvt_GetString);
    WRAPPER_COMMAND("api:pvt:Print", api_pvt_Print);

    /* MS functions */
    WRAPPER_COMMAND("api:ms:GetLogFilename", api_ms_GetLogFilename);
    WRAPPER_COMMAND("api:app:GetResource", api_app_GetResource);

    /* Application functions */
    WRAPPER_COMMAND("api:app:SetCallMode", api_app_SetCallMode);
    WRAPPER_COMMAND("api:app:SetChannelMode", api_app_SetChannelMode);
    WRAPPER_COMMAND("api:app:GetDataTypes", api_app_GetDataTypes);
    WRAPPER_COMMAND("api:app:ChannelKill", api_app_ChannelKill);
    WRAPPER_COMMAND("api:app:GetChannelList", api_app_GetChannelList);
    WRAPPER_COMMAND("api:app:Vt2Address", api_app_Vt2Address);
    WRAPPER_COMMAND("api:app:Address2Vt", api_app_Address2Vt);
    WRAPPER_COMMAND("api:app:FSGetChanNum", api_app_FSGetChanNum);
    WRAPPER_COMMAND("api:app:FSGetAltChanNum", api_app_FSGetAltChanNum);
    WRAPPER_COMMAND("api:app:FSGetChanIndex", api_app_FSGetChanIndex);
    WRAPPER_COMMAND("api:app:FSGetChanName", api_app_FSGetChanName);
    WRAPPER_COMMAND("api:app:FSGetChanRTCP", api_app_FSGetChanRTCP);
    WRAPPER_COMMAND("api:app:FSGetChanRTP", api_app_FSGetChanRTP);

    /* Annexes */
    WRAPPER_COMMAND("api:app:CallCreateAnnexMMessage", api_cm_CallCreateAnnexMMessage);
    WRAPPER_COMMAND("api:app:CallSendAnnexMMessage", api_cm_CallSendAnnexMMessage);
    WRAPPER_COMMAND("api:app:CallCreateAnnexLMessage", api_cm_CallCreateAnnexLMessage);
    WRAPPER_COMMAND("api:app:CallSendAnnexLMessage", api_cm_CallSendAnnexLMessage);

#if ( defined(USE_RTP) && (RTP_TYPE != RTP_TYPE_NEW) )
    /* RTP */
    //CREATE_COMMAND("rtpInfo.Display", RTP_DisplayInfo);
/*    CREATE_COMMAND("RTP.AJB.AjbStartBuffering", RTP_AJB_AjbStartBuffering);
    CREATE_COMMAND("RTP.AJB.AjbStopBuffering", RTP_AJB_AjbStopBuffering);*/

#if 0
    /* DTMF */
    WRAPPER_COMMAND("api:dtmf:CallOpenLogicalChannel", api_dtmf_CallOpenLogicalChannel);
    WRAPPER_COMMAND("api:dtmf:Dial", api_dtmf_Dial);

    /* FECC */
    WRAPPER_COMMAND("api:fecc:CallOpenLogicalChannel", api_fecc_CallOpenLogicalChannel);
    WRAPPER_COMMAND("api:fecc:Start", api_fecc_Start);
    WRAPPER_COMMAND("api:fecc:Continue", api_fecc_Continue);
    WRAPPER_COMMAND("api:fecc:Stop", api_fecc_Stop);
    WRAPPER_COMMAND("api:fecc:RemoteSource", api_fecc_RemoteSource);
    WRAPPER_COMMAND("api:fecc:LocalSource", api_fecc_LocalSource);
    WRAPPER_COMMAND("api:fecc:StoreRemotePreset", api_fecc_StoreRemotePreset);
    WRAPPER_COMMAND("api:fecc:ActivateRemotePreset", api_fecc_ActivateRemotePreset);
#endif /* 0 */
#endif

#if defined(USE_H450)
    /* H.450 */
    CREATE_COMMAND("api:h450:SendNameId", api_h450_SendNameId);
#endif

    /* h245 - Enhanced Command Set */
    //WRAPPER_COMMAND("api:h245:Func", api_h245_Func);
}






/********************************************************************************************
 *                                                                                          *
 *                                  Public functions                                        *
 *                                                                                          *
 ********************************************************************************************/


/********************************************************************************************
 * PutError
 * purpose : Notify the user about errors that occured
 * input   : title  - Title of the error
 *           reason - Reason that caused the error
 * output  : none
 * return  : none
 ********************************************************************************************/
void PutError(const char* title, const char* reason)
{
    static RvBool TAP_inError = RV_FALSE;
    if ((reason == NULL) || (strlen(reason) == 0))
        reason = "Undefined error was encountered";

    fprintf(stderr, "%s: %s\n", title, reason);

    /* Make sure we don't get into an endless loop over this one */
    if (TAP_inError) return;
    TAP_inError = RV_TRUE;

#if defined (WIN32) && defined (_WINDOWS)
    if (gInterp == NULL)
        MessageBox(NULL, reason, title, MB_OK | MB_ICONERROR);
#endif  /* WIN32 */

    if (gInterp != NULL)
    {
        TclExecute("Log:Write {%s: %s}", title, reason);
        TclExecute("update;msgbox {%s} picExclamation {%s} {{Ok -1 <Key-Return>}}", title, reason);
    }

    TAP_inError = RV_FALSE;
}

#ifdef USE_TCL

/********************************************************************************************
 * InitTcl
 * purpose : Initialize the TCL part of the test application
 * input   : executable     - Program executable
 *           versionString  - Stack version string
 * output  : reason         - Reason of failure on failure
 * return  : Tcl_Interp interpreter for tcl commands
 *           NULL on failure
 ********************************************************************************************/
Tcl_Interp* InitTcl(const char* executable, char* versionString, char** reason)
{
    static char strBuf[1024];
    int retCode;

    /* Find TCL executable and create an interpreter */
    Tcl_FindExecutable(executable);
    gInterp = Tcl_CreateInterp();

    if (gInterp == NULL)
    {
        *reason = (char*)"Failed to create Tcl interpreter";
        return NULL;
    }

    /* Overload file and source commands */
    TclExecute("rename file fileOverloaded");
    CREATE_COMMAND("file", test_File);
    CREATE_COMMAND("source", test_Source);

    /* Reroute tcl libraries - we'll need this one later */
    /*TclSetVariable("tcl_library", TCL_LIBPATH);
    TclSetVariable("env(TCL_LIBRARY)", TCL_LIBPATH);
    TclSetVariable("tk_library", TK_LIBPATH);
    TclSetVariable("env(TK_LIBRARY)", TK_LIBPATH);*/

    /* Initialize TCL */
    retCode = Tcl_Init(gInterp);
    if (retCode != TCL_OK)
    {
        sprintf(strBuf, "Error in Tcl_Init: %s", Tcl_GetStringResult(gInterp));
        *reason = strBuf;
        Tcl_DeleteInterp(gInterp);
        return NULL;
    }

    /* Initialize TK */
    retCode = Tk_Init(gInterp);
    if (retCode != TCL_OK)
    {
        sprintf(strBuf, "Error in Tk_Init: %s", Tcl_GetStringResult(gInterp));
        *reason = strBuf;
        Tcl_DeleteInterp(gInterp);
        return NULL;
    }

    /* Set argc and argv parameters for the script.
       This allows us to work with C in the scripts. */
#ifdef RV_RELEASE
    retCode = TclExecute("set tmp(version) {%s Test Application (Release)}", versionString);
#elif RV_DEBUG
    retCode = TclExecute("set tmp(version) {%s Test Application (Debug)}", versionString);
#else
#error RV_RELEASE or RV_DEBUG must be defined!
#endif
    if (retCode != TCL_OK)
    {
        *reason = (char*)"Error setting stack's version for test application";
        return gInterp;
    }

    /* Create new commands that are used in the tcl script */
    CreateTclCommands(gInterp);

    Tcl_LinkVar(gInterp, (char *)"scriptLogs", (char *)&LogWrappers, TCL_LINK_BOOLEAN);

    /* Evaluate the Tcl script of the test application */
    retCode = Tcl_Eval(gInterp, (char*)"source " TCL_FILENAME);
    if (retCode != TCL_OK)
    {
        sprintf(strBuf, "Error reading testapp script (line %d): %s\n", gInterp->errorLine, Tcl_GetStringResult(gInterp));
        *reason = strBuf;
        return NULL;
    }

    /* Return the created interpreter */
    *reason = NULL;
    return gInterp;
}

#endif

/********************************************************************************************
 * InitStack
 * purpose : Initialize the H.323 stack
 * input   : configFile - Configuration file to use.
 * output  : none
 * return  : RV_Success on success
 *           other on failure
 ********************************************************************************************/
int InitStack(IN const RvChar *configFile)
{
    RvStatus            status;
    EpCallbacks         cb;

    status = cmStartUp();
    if (status != RV_OK)
        return status;

    LogInit();

    memset(&cb, 0, sizeof(cb));
    cb.epMalloc = epMalloc;
    cb.epFree = epFree;
    cb.epAllocateResourceId = epAllocateResourceId;
    cb.epRegistrationStatus = epRegistrationStatus;
    cb.epCallNew = epCallNew;
    cb.epCallClose = epCallClose;
    cb.epChannelNew = epChannelNew;
    cb.epChannelStable = epChannelStable;
    cb.epMessage = epMessage;
    cb.epEventIndication = epEventIndication;
    cb.epLog = epLog;

    if (configFile == NULL)
        configFile = CONFIG_FILE;

    /* Initialize the stack */
    status = epInit(&ep, &cb, configFile, 0,
#ifdef USE_TCL
        !atoi(TclGetVariable("app(disableSEC)")),
        !atoi(TclGetVariable("app(disableH450)")),
        !atoi(TclGetVariable("app(disableRTP)")),
        !atoi(TclGetVariable("app(disableH460)")));
#else
        RV_TRUE,
        RV_TRUE,
        RV_TRUE,
        RV_TRUE);
#endif
    if (status != RV_OK)
        return status;

#ifdef USE_TCL
    /* Initialize NAT address in case it's needed */
    cmString2TransportAddress(TclGetVariable("app(options,natAddress)"), &ep.cfg.natAddress);
#endif

    status = epStart(&ep);
    if (status != RV_OK)
    {
        epEnd(&ep);
        return status;
    }

#ifdef USE_TCL
    if (gInterp != NULL)
    {
        Tcl_CmdInfo info;
        RvChar fname[1024];
        RvChar *p;
        strncpy(fname, configFile, sizeof(fname));
        fname[sizeof(fname)-1] = '\0';
        p = fname;
        while (*p)
        {
            if (*p == '\\')
                *p = '/';
            p++;
        }
        TclExecute("set tmp(configFilename) %s", fname);

        Tcl_GetCommandInfo(gInterp, "status:Check", &info);
        info.clientData = (ClientData)&ep;
        Tcl_SetCommandInfo(gInterp, "status:Check", &info);
        Tcl_GetCommandInfo(gInterp, "status:Pvt", &info);
        info.clientData = (ClientData)&ep;
        Tcl_SetCommandInfo(gInterp, "status:Pvt", &info);

        TclExecute("test:SetStackStatus Running");
    }
#endif
    return 0;
}


/********************************************************************************************
 * EndStack
 * purpose : End the H.323 stack
 * input   : none
 * output  : none
 * return  : none
 ********************************************************************************************/
void EndStack(void)
{
#ifdef USE_TCL
    Tcl_CmdInfo info;

    TclExecute("Window close .status");

    Tcl_GetCommandInfo(gInterp, "status:Check", &info);
    info.clientData = NULL;
    Tcl_SetCommandInfo(gInterp, "status:Check", &info);
    Tcl_GetCommandInfo(gInterp, "status:Pvt", &info);
    info.clientData = NULL;
    Tcl_SetCommandInfo(gInterp, "status:Pvt", &info);
#endif
    epEnd(&ep);

    cmShutdown();

    TclExecute("test:SetStackStatus Finished");
}


/******************************************************************************
 * h323AppUpdateVariable
 * ----------------------------------------------------------------------------
 * General: Update a variable in the H.323 application object.
 *
 * Return Value: none.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  varName          - String name of the variable.
 *         varValue         - String value of the variable.
 * Output: None.
 *****************************************************************************/
void h323AppUpdateVariable(const char * varName, const char * varValue)
{
    if (strcmp(varName, "newcall,parallel") == 0)
    {
        ep.cfg.callConfig.parellel = atoi(varValue);
    }
    else if (strcmp(varName, "newcall,tunneling") == 0)
    {
        ep.cfg.callConfig.tunneling = atoi(varValue);
    }
    else if (strcmp(varName, "newcall,efc") == 0)
    {
        ep.cfg.callConfig.efc = atoi(varValue);
    }
    else if (strcmp(varName, "newcall,multiplexed") == 0)
    {
        ep.cfg.callConfig.multiplexing = atoi(varValue);
    }
    else if (strcmp(varName, "newcall,maintain") == 0)
    {
        ep.cfg.callConfig.maintainConnection = atoi(varValue);
    }
    else if (strcmp(varName, "newcall,canOvsp") == 0)
    {
        ep.cfg.callConfig.canOverlapSend = atoi(varValue);
    }
    else if (strcmp(varName, "newcall,autoAns") == 0)
    {
        ep.cfg.bAutoAnswer = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "newcall,fastStart") == 0)
    {
        ep.cfg.bSupportFaststart = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "newcall,manualRAS") == 0)
    {
        ep.cfg.callConfig.manualRasCalls = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "options,autoAccept") == 0)
    {
        ep.cfg.bAutoAcceptChannels = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "options,autoDrop") == 0)
    {
        ep.cfg.bAutoDropChannels = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "options,autoMimic") == 0)
    {
        ep.cfg.bAutoMimicChannels = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "options,autoAcceptFs") == 0)
    {
        ep.cfg.bAutomaticFaststart = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "options,replayMedia") == 0)
    {
        ep.cfg.mediaAction = (EpRtpAction)atoi(varValue);
    }
    else if (strcmp(varName, "options,tos") == 0)
    {
        ep.cfg.typeOfService = atoi(varValue);
    }
    else if (strcmp(varName, "options,hopCount") == 0)
    {
        ep.cfg.callConfig.hopCount = atoi(varValue);
    }
    else if (strcmp(varName, "options,ignoreConf") == 0)
    {
        ep.cfg.bIgnoreMasterSlaveConflicts = atoi(varValue);
    }

    else if (strcmp(varName, "h245,stage") == 0)
    {
        ep.cfg.callConfig.h245Stage = (RvInt32)String2CMH245Stage(varValue);
    }
    else if (strcmp(varName, "test,bw") == 0)
    {
        ep.cfg.callConfig.requestedBandwidth = atoi(varValue);
    }
    else if (strcmp(varName, "suppress,releaseComplete") == 0)
    {
        ep.cfg.bSuppressReleaseComplete = atoi(varValue);
    }
    else if (strcmp(varName, "suppress,endSessionCommand") == 0)
    {
        ep.cfg.bSuppressEndSessionCommand = atoi(varValue);
    }
    else if (strcmp(varName, "suppress,ras") == 0)
    {
        ep.cfg.bSuppressRAS = atoi(varValue);
    }
    else if (strcmp(varName, "suppress,q931") == 0)
    {
        ep.cfg.bSuppressQ931 = atoi(varValue);
    }
    else if (strcmp(varName, "suppress,h245") == 0)
    {
        ep.cfg.bSuppressH245 = atoi(varValue);
    }
    else if (strcmp(varName, "options,useNatAddress") == 0)
    {
        ep.cfg.bUseNatAddress = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "options,natAddress") == 0)
    {
        cmString2TransportAddress(varValue, &ep.cfg.natAddress );
    }
    else if (strcmp(varName, "options,dialFrom") == 0)
    {
        strncpy(ep.cfg.dialFromIp, varValue, sizeof(ep.cfg.dialFromIp));
    }
    else if (strcmp(varName, "ras,rereg") == 0)
    {
        ep.cfg.bPreventRereg = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "options,popUp") == 0)
    {
        ep.cfg.bPopUp = (atoi(varValue) != 0);
    }
    else if (strcmp(varName, "adHoc,use") == 0)
    {
        ep.cfg.bAdHocUse = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "adHoc,call") == 0)
    {
        strncpy(ep.cfg.strAdHocCall, varValue, sizeof(ep.cfg.strAdHocCall));
    }
    else if (strcmp(varName, "multiplex,use") == 0)
    {
        ep.cfg.bMultiplexUse = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "multiplex,call") == 0)
    {
        strncpy(ep.cfg.strMultiplexCall, varValue, sizeof(ep.cfg.strMultiplexCall));
    }
    else if (strcmp(varName, "options,userUser") == 0)
    {
        strncpy(ep.cfg.strUserUser, varValue, sizeof(ep.cfg.strUserUser));
    }
    else if (strcmp(varName, "options,display") == 0)
    {
        strncpy(ep.cfg.strDisplay, varValue, sizeof(ep.cfg.strDisplay));
    }
    else if (strcmp(varName, "options,nonStandardData") == 0)
    {
        ep.cfg.bUseNSD = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "nsd,str") == 0)
    {
        strncpy(ep.cfg.nsdString, varValue, sizeof(ep.cfg.nsdString));
    }

#ifdef USE_SECURITY
    else if (strcmp(varName, "sec,AnxDsup") == 0)
    {
        if (atoi(varValue))
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) | (RvUint32)h235ModeProcedure1);
        else
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) & ~(RvUint32)h235ModeProcedure1);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,AnxDaut") == 0)
    {
        if (atoi(varValue))
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) | (RvUint32)h235ModeProcedure1a);
        else
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) & ~(RvUint32)h235ModeProcedure1a);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,AnxEsup") == 0)
    {
        if (atoi(varValue))
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) | (RvUint32)h235ModeProcedure2 | (RvUint32)h235ModeProcedure3);
        else
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) & (~(RvUint32)h235ModeProcedure2 & ~(RvUint32)h235ModeProcedure3));
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,AnxFsup") == 0)
    {
        if (atoi(varValue))
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) | (RvUint32)h235ModeProcedure4);
        else
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) & ~(RvUint32)h235ModeProcedure4);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,dh512sup") == 0)
    {
        if (atoi(varValue))
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) | (RvUint32)h235ModeDHgroup512);
        else
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) & ~(RvUint32)h235ModeDHgroup512);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,dh1024sup") == 0)
    {
        if (atoi(varValue))
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) | (RvUint32)h235ModeDHgroup1024);
        else
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) & ~(RvUint32)h235ModeDHgroup1024);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,dh1536sup") == 0)
    {
        if (atoi(varValue))
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) | (RvUint32)h235ModeDHgroup1536);
        else
            ep.cfg.securityMode = (h235Mode_t)(((RvUint32)ep.cfg.securityMode) & ~(RvUint32)h235ModeDHgroup1536);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,checkIncoming") == 0)
    {
        ep.cfg.bCheckIncoming = (RvBool)atoi(varValue);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,userPrime") == 0)
    {
        ep.cfg.bUserPrimes = (RvBool)atoi(varValue);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,password") == 0)
    {
        strncpy(ep.cfg.password, varValue, sizeof(ep.cfg.password));
        ep.cfg.password[sizeof(ep.cfg.password) - 1] = '\0';
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,generalID") == 0)
    {
        RvChar *tmpStr = (RvChar *)varValue;
        ep.cfg.generalId.length = utlChrn2Bmp(tmpStr, RvMin(strlen(tmpStr), 128), ep.cfg.generalId.data);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,senderID") == 0)
    {
        RvChar *tmpStr = (RvChar *)varValue;
        ep.cfg.senderId.length = utlChrn2Bmp(tmpStr, RvMin(strlen(tmpStr), 128), ep.cfg.senderId.data);
        epSecurityUpdateSettings(&ep);
    }
    else if (strcmp(varName, "sec,remoteID") == 0)
    {
        RvChar *tmpStr = (RvChar *)varValue;
        ep.cfg.remoteId.length = utlChrn2Bmp(tmpStr, RvMin(strlen(tmpStr), 128), ep.cfg.remoteId.data);
    }
#endif

#ifdef USE_H450
    else if (strcmp(varName, "h450,defAns") == 0)
    {
        ep.cfg.H450_responseType = String2ssAction(varValue);
    }
    else if (strcmp(varName, "h450,disableWait") == 0)
    {
        ep.cfg.bH4504_disableWait = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "h450,nameType") == 0)
    {
        ep.cfg.H4508_niNameChoice = String2H450niNameChoice(varValue);
    }
    else if (strcmp(varName, "h450,nameID") == 0)
    {
        strncpy(ep.cfg.H4508_name, varValue, sizeof(ep.cfg.H4508_name));
        ep.cfg.H4508_name[sizeof(ep.cfg.H4508_name)-1] = '\0';
    }
    else if (strcmp(varName, "h450,enablePrk") == 0)
    {
        ep.cfg.bH4505_enablePark = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "h450,alertingPrk") == 0)
    {
        ep.cfg.bH4505_alertingPrk = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "h450,groupList") == 0)
    {
        const RvChar *groupList = varValue;
        RvSize_t len;

        if (ep.cfg.H4505_parkGroup != NULL)
            epFree(&ep, ep.cfg.H4505_parkGroup);
        ep.cfg.H4505_parkGroup = NULL;

        len = strlen(groupList) + 1;
        if (len > 1)
        {
            ep.cfg.H4505_parkGroup = (RvChar *)epMalloc(&ep, len, __FILE__, __LINE__);
            strcpy(ep.cfg.H4505_parkGroup, groupList);
        }
    }
    else if (strcmp(varName, "h450,retainServ") == 0)
    {
        ep.cfg.bH4509_retainService = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "h450,retainConn") == 0)
    {
        ep.cfg.bH4509_retainConnection = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "h450,compBusy") == 0)
    {
        ep.cfg.bH4509_isBusy = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "h450,compFoundService") == 0)
    {
        ep.cfg.H4509_foundService = (RvInt32)atoi(varValue);
    }
    else if (strcmp(varName, "h450,intrPossible") == 0)
    {
        ep.cfg.bH45011_isPossible = (RvBool)atoi(varValue);
    }
    else if (strcmp(varName, "h450,intrIsIsolated") == 0)
    {
        ep.cfg.bH45011_isIsolated = (RvBool)atoi(varValue);
    }
#endif

#ifdef USE_LDAP
    else if (strcmp(varName, "sec,useLdap") == 0)
    {
        ep.cfg.bUseLdap = (RvBool)atoi(varValue);
    }
#endif

#ifdef USE_FWNAT
    else if (strcmp(varName, "h460,cfg17") == 0)
    {
        ep.cfg.fwNatConfig.e17Config = (RvH323FwNatH46017Config)atoi(varValue);
        epFwNatConfigure(&ep);
    }
    else if (strcmp(varName, "h460,cfg18") == 0)
    {
        ep.cfg.fwNatConfig.e18Config = (RvH323FwNatH46018Config)atoi(varValue);
        epFwNatConfigure(&ep);
    }
    else if (strcmp(varName, "h460,cfg19") == 0)
    {
        ep.cfg.fwNatConfig.e19Config = (RvH323FwNatH46019Config)atoi(varValue);
        epFwNatConfigure(&ep);
    }
    else if (strcmp(varName, "h460,muxMode") == 0)
    {
        ep.cfg.fwNatConfig.e19MultiplexMediaConfig = atoi(varValue);
        epFwNatConfigure(&ep);
    }
    else if (strcmp(varName, "options,scriptMode") == 0)
    {
        ep.cfg.bIndependent = (atoi(varValue) != 0);
    }
#endif

    return;
}


static char *tclVarUpdate(
    IN ClientData   clientData,
    IN Tcl_Interp   *interp,
    IN char         *name1,
    IN char         *name2,
    IN int          flags)
{
    char varName[64];
    char * val;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(name1);
    RV_UNUSED_ARG(flags);

    sprintf(varName, "%s(%s)", name1, name2);
    val = TclGetVariable(varName);
    h323AppUpdateVariable(name2, val);
    return NULL;
}


static void TraceTclVariable(IN const RvChar    *varName)
{
    Tcl_TraceVar2(gInterp, (char*)"app", (char*)varName, TCL_TRACE_WRITES, (Tcl_VarTraceProc *)tclVarUpdate, NULL);

    TclExecute("if {[lsearch $tmp(tvars) %s] < 0} {lappend tmp(tvars) %s}", varName, varName);

    /* Make sure we get the initial value here */
    tclVarUpdate(NULL, gInterp, (char*)"app", (char*)varName, TCL_TRACE_WRITES);
}


static void TraceTclVariableEx(
    IN const RvChar    *arrName,
    IN const RvChar    *varName)
{
    Tcl_TraceVar2(gInterp, (char*)arrName, (char*)varName, TCL_TRACE_WRITES, (Tcl_VarTraceProc *)tclVarUpdate, NULL);

    /*TclExecute("if {[lsearch $tmp(tvars) %s] < 0} {lappend tmp(tvars) %s}", varName, varName);*/

    /* Make sure we get the initial value here */
    tclVarUpdate(NULL, gInterp, (char*)arrName, (char*)varName, TCL_TRACE_WRITES);
}



/********************************************************************************************
 * InitApp
 * purpose : Initialize the test application
 *           This includes parts as RTP/RTCP support, etc.
 * input   : none
 * output  : none
 * return  : Non-negative value on success
 *           Negative value on failure
 ********************************************************************************************/
int InitApp(void)
{
    /* Set tracing on specific variables */
    TraceTclVariable("newcall,parallel");
    TraceTclVariable("newcall,tunneling");
    TraceTclVariable("newcall,efc");
    TraceTclVariable("newcall,multiplexed");
    TraceTclVariable("newcall,maintain");
    TraceTclVariable("newcall,canOvsp");
    TraceTclVariable("newcall,autoAns");
    TraceTclVariable("newcall,fastStart");
    TraceTclVariable("newcall,manualRAS");
    TraceTclVariable("options,autoAccept");
    TraceTclVariable("options,autoDrop");
    TraceTclVariable("options,autoMimic");
    TraceTclVariable("options,autoAcceptFs");
    TraceTclVariable("options,replayMedia");
    TraceTclVariable("options,ignoreConf");
    TraceTclVariable("options,tos");
    TraceTclVariable("options,hopCount");
    TraceTclVariable("h245,stage");
    TraceTclVariable("test,bw");
    TraceTclVariable("suppress,releaseComplete");
    TraceTclVariable("suppress,endSessionCommand");
    TraceTclVariable("suppress,ras");
    TraceTclVariable("suppress,q931");
    TraceTclVariable("suppress,h245");
    TraceTclVariable("options,useNatAddress");
    TraceTclVariable("options,natAddress");
    TraceTclVariable("options,dialFrom");
    TraceTclVariable("ras,rereg");
    TraceTclVariable("options,popUp");
    TraceTclVariable("options,userUser");
    TraceTclVariable("options,display");
    TraceTclVariable("options,nonStandardData");

    TraceTclVariableEx("tmp", "adHoc,use");
    TraceTclVariableEx("tmp", "adHoc,call");
    TraceTclVariableEx("tmp", "multiplex,use");
    TraceTclVariableEx("tmp", "multiplex,call");

    /* Security */
    TraceTclVariable("sec,AnxDsup");
    TraceTclVariable("sec,AnxDaut");
    TraceTclVariable("sec,AnxEsup");
    TraceTclVariable("sec,AnxFsup");
    TraceTclVariable("sec,dh512sup");
    TraceTclVariable("sec,dh1024sup");
    TraceTclVariable("sec,dh1536sup");
    TraceTclVariable("sec,checkIncoming");
    TraceTclVariable("sec,userPrime");
    TraceTclVariable("sec,password");
    TraceTclVariable("sec,generalID");
    TraceTclVariable("sec,senderID");
    TraceTclVariable("sec,remoteID");

    /* H.450 */
    TraceTclVariable("h450,defAns");
    TraceTclVariable("h450,disableWait");
    TraceTclVariable("h450,nameType");
    TraceTclVariable("h450,nameID");
    TraceTclVariable("h450,enablePrk");
    TraceTclVariable("h450,alertingPrk");
    TraceTclVariable("h450,groupList");
    TraceTclVariable("h450,retainServ");
    TraceTclVariable("h450,retainConn");
    TraceTclVariable("h450,compBusy");
    TraceTclVariable("h450,compFoundService");
    TraceTclVariable("h450,intrPossible");
    TraceTclVariable("h450,intrIsIsolated");

    TraceTclVariable("sec,useLdap");

    TraceTclVariable("h460,cfg17");
    TraceTclVariable("h460,cfg18");
    TraceTclVariable("h460,cfg19");
    TraceTclVariable("h460,muxMode");

    TraceTclVariableEx("tmp", "options,scriptMode");
    TraceTclVariableEx("tmp", "nsd,str");

    TclExecute("test:updateGui 1");

    return 0;
}



#ifdef __cplusplus
}
#endif

