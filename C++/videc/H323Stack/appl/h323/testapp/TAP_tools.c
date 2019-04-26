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


/********************************************************************************************
 *                                TAP_tools.c
 *
 * Application tools handling
 *
 * 1. Status
 *      Status information about the stack (All the resources information)
 * 2. Hooks
 *      Stack hooks that are catched by the test application
 *
 ********************************************************************************************/

#include <cm.h>
#include <msg.h>
#include "TAP_init.h"
#include "TAP_general.h"
#include "TAP_tools.h"

#ifdef __cplusplus
extern "C" {
#endif





/******************************************************************************
 * epMessage
 * ----------------------------------------------------------------------------
 * General: Indication of a message being sent or received by the endpoint.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         call     - Call this message belongs to (NULL if none).
 *         nodeId   - Message node id.
 *         name     - Message's name.
 *         isSend   - RV_TRUE for send, RV_FALSE for receive.
 * Output: None.
 *****************************************************************************/
void epMessage(
    IN EpObj                *ep,
    IN EpCallObj            *call,
    IN RvPvtNodeId          nodeId,
    IN const RvChar         *name,
    IN RvBool               isSend)
{
    RV_UNUSED_ARG(ep);
    RV_UNUSED_ARG(call);
    RV_UNUSED_ARG(nodeId);
    RV_UNUSED_ARG(name);
    RV_UNUSED_ARG(isSend);
#ifdef USE_TCL
    if (TclGetVariable("app(options,blinkMessage)")[0] == '1')
    {
        TclExecute("wm deiconify .test");
    }
#endif
}





/********************************************************************************************
 *                                                                                          *
 *                                  Public functions                                        *
 *                                                                                          *
 ********************************************************************************************/


/********************************************************************************************
 * Options_GetLocalIP
 * purpose : Get the Local IP address of the machine
 * syntax  : Options.GetLocalIP
 * input   : clientData - Not used
 *           interp     - Interpreter for tcl commands
 *           argc       - Number of arguments
 *           argv       - Arguments of the Tcl/Tk command
 * output  : Local IP string
 * return  : TCL_OK     - The command was invoked successfully.
 ********************************************************************************************/
int Options_GetLocalIP(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    char my_ip_addr[128];
    cmTransportAddress transportAddress;

    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    if ((cmGetLocalCallSignalAddress(ep->hApp, &transportAddress) >= 0) &&
        (cmTransportAddress2String(&transportAddress, sizeof(my_ip_addr), my_ip_addr) == RV_OK))
    {
        RvUint32 i;
        for (i=strlen(my_ip_addr); i>0; i--)
            if (my_ip_addr[i] == ':')
            {
                my_ip_addr[i] = '\0';
                break;
            }
        Tcl_SetResult(interp, my_ip_addr, TCL_VOLATILE);
        return TCL_OK;
    }

    return TCL_ERROR;
}


/********************************************************************************************
 * Options_Crash
 * purpose : Cause the test application to crash.
 * syntax  : Options.Crash
 * input   : clientData - Not used
 *           interp     - Interpreter for tcl commands
 *           argc       - Number of arguments
 *           argv       - Arguments of the Tcl/Tk command
 * output  : Local IP string
 * return  : TCL_OK     - The command was invoked successfully.
 ********************************************************************************************/
int Options_Crash(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    int crashMe;

    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);

    crashMe = 10 / (argc - 1);
    *argv[1] = (char)crashMe;

    return TCL_OK;
}


/********************************************************************************************
 * Status_Display
 * purpose : This function gets the resources of Timers, ASN, RAS etc and displays them
 *           inside the status window
 * syntax  : Status.Display
 * input   : clientData - Not used
 *           interp     - Interpreter for tcl commands
 *           argc       - Number of arguments
 *           argv       - Arguments of the Tcl/Tk command
 * output  : none
 * return  : TCL_OK     - The command was invoked successfully.
 ********************************************************************************************/
int Status_Display(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvUint32 initVal, curVal, maxUsageVal, maxVal;
    int anyInit, anyCur, anyMaxUsage, anyMax;
    void* prevResource = NULL;
    void* curResource;
    char curResourceName[40];
    char lastGroup[40];
    char curResourceGroup[40];
    char* printGroup;
    int res = 0;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    /* Delete listbox contents */
    TclExecute("status:Clean");

    memset(lastGroup, 0, sizeof(lastGroup));

    while (res >= 0)
    {
        /* Get the next resource */
        res = RvH323WatchdogNextResource(ep->hApp, prevResource, &curResource, curResourceName);
        prevResource = curResource;

        if (res >= 0)
        {
            /* Get this resource information */
            anyInit = RvH323WatchdogGetResource(ep->hApp, curResourceName,
                RvH323WatchdogInitialVal, &initVal, curResourceGroup);
            anyCur = RvH323WatchdogGetResource(ep->hApp, curResourceName,
                RvH323WatchdogCurrent, &curVal, curResourceGroup);
            anyMaxUsage = RvH323WatchdogGetResource(ep->hApp, curResourceName,
                RvH323WatchdogMaxUsage, &maxUsageVal, NULL);
            anyMax = RvH323WatchdogGetResource(ep->hApp, curResourceName,
                RvH323WatchdogMaxVal, &maxVal, NULL);

            /* Make sure we have something to print */
            if (anyCur >= 0)
            {
                /* Match group with previous one */
                if (strcmp(curResourceGroup, lastGroup) == 0)
                    printGroup = (char*)""; /* Don't print group's name */
                else
                {
                    /* New group */
                    strcpy(lastGroup, curResourceGroup);
                    printGroup = curResourceGroup;
                }

                /* See if we have a max value to print */
                if (anyMax >= 0)
                {
                    TclExecute("status:InsertLine {%s} {%s} %d %d %d %d\n",
                        printGroup, curResourceName, initVal, curVal, maxUsageVal, maxVal);
                }
                else
                {
                    TclExecute("status:InsertLine {%s} {%s} %d %d %d {}\n",
                        printGroup, curResourceName, initVal, curVal, maxUsageVal);
                }
            }
        }
    }

    /* Handle the application's resources - always last... */
    TclExecute("status:InsertLine {APP} {Allocations} {} %d %d {}\n"
        "status:InsertLine {} {Memory} {} %d %d {}",
        ep->curAllocs, ep->maxAllocs,
        ep->curMemory, ep->maxMemory);

    TclExecute("status:DrawGraphs");
    return TCL_OK;
}


/********************************************************************************************
 * Status_Check
 * purpose : This function gets the resources of Timers, ASN, RAS etc and compares them to
 *           the stored application variables.
 * syntax  : status:Check <tpktSize> <messagesSize> <pvtSize> <timersSize>
 * input   : clientData - Not used
 *           interp     - Interpreter for tcl commands
 *           argc       - Number of arguments
 *           argv       - Arguments of the Tcl/Tk command
 * output  : none
 * return  : TCL_OK     - The command was invoked successfully.
 ********************************************************************************************/
int Status_Check(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    int res;
    RvUint32 i;
    RvUint32 initial, current = 0;

    static const char* resourceNames[] = {
        "CmCalls", "CmChannels", "TransHosts", "TransSessions", "CmValTree", "CmTimers",
#ifdef USE_H450
#endif
        NULL
    };
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    if (clientData != NULL)
    {
        i = 0;
        while (resourceNames[i] != NULL)
        {
            res = RvH323WatchdogGetResource(ep->hApp, resourceNames[i],
                    RvH323WatchdogInitialVal, &initial, NULL);
            if (res >= 0)
                res = RvH323WatchdogGetResource(ep->hApp, resourceNames[i],
                    RvH323WatchdogCurrent, &current, NULL);

            /* Only check if previous calls succeeded */
            if ((res >= 0) && (initial != current))
            {
                TclExecute("test:Log {WARNING: memory leak in %s: was %d; now %d}",
                    resourceNames[i], initial, current);
            }

            i++;
        }
    }

    return TCL_OK;
}


/********************************************************************************************
 * Status_Pvt
 * purpose : This function sets the resources of the PVT on the test application window
 *           to the proper current value.
 * syntax  : status:Pvt
 * input   : clientData - Not used
 *           interp     - Interpreter for tcl commands
 *           argc       - Number of arguments
 *           argv       - Arguments of the Tcl/Tk command
 * output  : none
 * return  : TCL_OK     - The command was invoked successfully.
 ********************************************************************************************/
int Status_Pvt(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    static RvBool firstPvtCheck = RV_TRUE;
    static RvInt cmPvtNodesStart = 0;
    static RvInt cmPvtNodes = 1;
    RvInt curValue;
    
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    if (clientData != NULL)
    {
        if (firstPvtCheck == RV_TRUE)
        {
            /* First time visitor - get the initial status since we're going to use it for
               future display */
            cmPvtNodesStart = pvtCurSize(ep->hVal);

            firstPvtCheck = RV_FALSE;
        }

        curValue = pvtCurSize(ep->hVal) - cmPvtNodesStart;

        if (curValue != cmPvtNodes)
        {
            const RvChar *fgColor;
#if 0
            if ((curValue < 0) && bIsManualRAS)
                fgColor = "red";
            else
#endif
                fgColor = "black";
            TclExecute(".test.status.pvt configure -text %d -foreground %s", curValue, fgColor);
        }
    }

    /* Make sure we're going to recheck this value some time from now */
    TclExecute("after 1000 status:Pvt");

    return TCL_OK;
}


/********************************************************************************************
 * LogFile_Reset
 * purpose : This function calls ms Close and msOpen to restart the log with the current 
 *           rvtele.ini.
 * syntax  : LogFile:Reset
 * input   : clientData - Not used
 *           interp     - Interpreter for tcl commands
 *           argc       - Number of arguments
 *           argv       - Arguments of the Tcl/Tk command
 * output  : none
 * return  : TCL_OK     - The command was invoked successfully.
 ********************************************************************************************/
int LogFile_Reset(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    RV_UNUSED_ARG(clientData);
    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);

    msClose();
    msOpen();
    return TCL_OK;
}


#ifdef __cplusplus
}
#endif


