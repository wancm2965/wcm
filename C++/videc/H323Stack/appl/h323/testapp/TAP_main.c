/***********************************************************************
Copyright (c) 2003 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..
RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

/***********************************************************************
TAP_main.c

This file is the main file of the Test Application.
This file generates the GUI that was written in TCL/TK.
It also creates new TCL commands.
***********************************************************************/


/*****************************************************************
 * DEFINITIONS for ADDONS
 * ----------------------
 * USE_H450     - H.450 Supplementary Services
 * USE_SECURITY - H.235 Security
 * USE_RTP      - RTP (loopback of channels)
 * USE_SNMP     - H.341 MIB
 *****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "cm.h"

#ifdef WIN32
#pragma warning (push,3)
#include <windows.h>
#include <shellapi.h>
#include <locale.h>
#pragma warning (pop)
#else
#include "seli.h"
#include "mti.h"
#if (RV_OS_TYPE == RV_OS_TYPE_SOLARIS) || (RV_OS_TYPE == RV_OS_TYPE_LINUX) || \
    (RV_OS_TYPE == RV_OS_TYPE_TRU64) || (RV_OS_TYPE == RV_OS_TYPE_HPUX)
#include <sys/resource.h>
#endif


#endif  /* WIN32 */


#include <stdlib.h>
#include "TAP_general.h"
#include "TAP_defs.h"
#include "TAP_init.h"
#include "TAP_tools.h"

/* Global variables declarations */
Tcl_Interp* interp;


#ifndef WIN32

static HSTIMER unixGuiTimers;
static HTI unixGuiTimer;


/********************************************************************************************
 * handleGuiEvent
 * purpose : Handle an X GUI event on Unix machines
 * input   : fdHandle   - File descriptor handle used for GUI events
 *           event      - Event that occured
 *           userData   - User specific data (unused here)
 *           error      - Error indication on event
 * output  : none
 ********************************************************************************************/
void handleGuiEvent(IN int          fdHandle,
                    IN seliEvents   event,
                    IN RvBool       error)
{
    RvBool notFinished = RV_TRUE;
    RV_UNUSED_ARG(fdHandle);
    RV_UNUSED_ARG(event);
    RV_UNUSED_ARG(error);

    /* Handle GUI events until we don't have any */
    while (notFinished)
    {
        if (!Tk_DoOneEvent(TK_DONT_WAIT))
            notFinished = RV_FALSE; /* no more events */
    }
}

/********************************************************************************************
 * handleTimedGuiEvents
 * purpose : Handle an X GUI event on Unix machines in specific intervals of a timer
 * input   : context    - Unused
 * output  : none
 ********************************************************************************************/
void RVCALLCONV handleTimedGuiEvents(IN void* context)
{
    RvBool notFinished = RV_TRUE;
    RV_UNUSED_ARG(context);

    /* Handle GUI events until we don't have any */
    while (notFinished)
    {
        if (!Tk_DoOneEvent(TK_DONT_WAIT))
            notFinished = RV_FALSE; /* no more events */
    }
}

#endif  /* WIN32 */




/********************************************************************************************
 * main
 * purpose : the main function of the test application.
 * input   : argc - number of parameters entered to main
 * input   : argv - parameters entered to test application
 * output  : none
 ********************************************************************************************/
int main(int argc, char *argv[])
{
    char verStr[60];
    char* reason = NULL;
    int status;
    char *configFile = NULL;

    RV_UNUSED_ARG(argc);

    sprintf(verStr, "RADVISION H.323 Protocol Toolkit v%s", cmGetVersion());


    /* Initialize the TCL part of the test application */
    interp = InitTcl(argv[0], verStr, &reason);
    if (reason != NULL)
    {
        PutError("Error initializing Tcl/Tk", reason);
        if (interp)
            Tcl_DeleteInterp(interp);
        exit(-1);
    }

    if (argc > 1)
        configFile = argv[1];

#if (RV_SELECT_TYPE == RV_SELECT_POLL) || (RV_SELECT_TYPE == RV_SELECT_DEVPOLL)
#ifdef RLIMIT_NOFILE
    {
        struct rlimit r;

        if (getrlimit(RLIMIT_NOFILE, &r) < 0)
            perror("getrlimit");

        /* set to hard limit if necessary */
        if (r.rlim_cur != r.rlim_max)
        {
            r.rlim_cur = r.rlim_max;
            if (setrlimit(RLIMIT_NOFILE, &r) < 0)
            {
                perror("setrlimit");
            }
            /*** We do not have to set max Descriptors.
            else
            {
                seliSetMaxDescs(r.rlim_cur);
            }
            ***/
        }
    }
#endif
#endif
    if ((status = InitStack(configFile)) < 0)
    {
        switch (status)
        {
            case -2: reason = (char*)"Resource problem"; break;
            case -10: reason = (char*)"Memory problem"; break;
            case -11: reason = (char*)"Configuration problem"; break;
            case -13: reason = (char*)"Network problem"; break;
        }
        PutError("Error initializing H.323 Stack", reason);
        exit(-1);
    }

    /* Initialize other application packages */
    if (InitApp() < 0)
    {
        PutError("InitApp", "Error initializing test application");
        exit(-1);
    }

    TclExecute("update;wm withdraw .;wm geometry .test $app(test,size);wm deiconify .test;update;notebook:refresh test;after 100 {wm deiconify .test}");

    /* Run the test application */
#ifdef WIN32
    Tk_MainLoop();

#else  /* WIN32 */

    /* We should most probably initialize seli... */
    seliInit();

    /* Get a nice timer to work for us. Every 50ms should be enough */
    unixGuiTimers = mtimerInit(1, (HAPPTIMER)&unixGuiTimer);

    {
        int appFd = XConnectionNumber(Tk_Display(Tk_MainWindow(interp)));
        printf("Application is using fd=%d for X Windows events\n", appFd);

        /* Unix - use seliSelect() loop */
        seliCallOn(appFd, seliEvRead, handleGuiEvent);

        /* Also use a timer, otherwise we take all the CPU of the poor Unix machine. */
        /* There are probably other good ways of doing it through the X Windows, but */
        /* it's not really part of our job to do it good enough (it's only the TCL */
        /* part of the test application after all. */
        unixGuiTimer = mtimerSet(unixGuiTimers, handleTimedGuiEvents, NULL, 50);
    }

    while (RV_TRUE)
    {
        /* Use seliSelect() to handle this while loop */
        seliSelect();
    }

    /* Remove our GUI timer */
    mtimerReset(unixGuiTimers, unixGuiTimer);
    mtimerEnd(unixGuiTimers);

    seliEnd();

#endif  /* WIN32 */

    return 0;
}




#if defined (WIN32) && defined (_WINDOWS)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int nullLoc;
    int argc;
    char *cmdLine;
    char *argv[2];

    RV_UNUSED_ARG(nCmdShow);
    RV_UNUSED_ARG(hPrevInstance);
    RV_UNUSED_ARG(hInstance);

    argc = 1;
    if ((lpCmdLine != NULL) && *lpCmdLine)
        argc = 2;

    /* Find out the executable's name */
    setlocale(LC_ALL, "C");
    cmdLine = GetCommandLine();
    if (strlen(lpCmdLine) > 0)
    {
        nullLoc = strlen(cmdLine) - strlen(lpCmdLine) - 1;
        cmdLine[nullLoc] = '\0';
    }

    argv[0] = cmdLine;
    argv[1] = lpCmdLine;

    /* Deal with it as if we're running a console or Unix application */
    return main(argc, argv);
}



#endif  /* Win32 App */




#ifdef __cplusplus
}
#endif


