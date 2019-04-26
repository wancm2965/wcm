/************************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/

/********************************************************************************************
 *                                defs.h
 *
 * This file provides definitions for the entire test application.
 *
 *
 *      Written by                        Version & Date                        Change
 *     ------------                       ---------------                      --------
 *      Oren Libis                          04-Mar-2000
 *
 ********************************************************************************************/

#ifndef _DEFS_H
#define _DEFS_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lepDefs.h"


#define BUFFER_SIZE 1024
#include "cm.h"

#ifdef USE_H450
#include "sse.h"
#endif

#ifdef USE_SECURITY
#include "h235_api.h"
#endif


#if ((RV_OS_TYPE == RV_OS_TYPE_WIN32) || (RV_OS_TYPE == RV_OS_TYPE_SOLARIS) || \
 ((RV_OS_TYPE == RV_OS_TYPE_LINUX) && ((RV_OS_VERSION & RV_OS_LINUX_MVISTA) == 0)) || \
 (RV_OS_TYPE == RV_OS_TYPE_TRU64) || (RV_OS_TYPE == RV_OS_TYPE_HPUX)) && (!defined(APP_NO_TCL))

#define USE_TCL

#else

#define APP_SPLIT

#endif



#ifdef USE_TCL

#include <tcl.h>
#include <tk.h>

#if (TCL_MAJOR_VERSION < 8)
#error TCL version is too old
#endif

#else /* USE_TCL */

typedef void * ClientData;

typedef void * Tcl_Interp;

typedef int (Tcl_CmdProc) (ClientData clientData, Tcl_Interp *interp, int argc, const char *argv[]);

#define Tcl_CmdDeleteProc (void*)

#define Tcl_CreateCommand(a, b, c, d, e)

#define TCL_OK (0)
#define TCL_ERROR (-1)

typedef struct TclFuncRefLink_ {
    ClientData clientData;
    Tcl_CmdProc * pCmd;
    char cmdName[32];
    struct TclFuncRefLink_ * next;
} TclFuncRefLink;

#endif /* USE_TCL */


/* TCL resources */
extern Tcl_Interp*  gInterp;



/********************************************************************************************
 *                             struct ConnectionInfo
 *
 * This struct holds the application's information about a TCP connection.
 *
 ********************************************************************************************/
typedef struct 
{
    int     counter; /* Counter value of this connection */
    int     color; /* Color assigned for this connection host */
} ConnectionInfo;




#ifdef __cplusplus
}
#endif

#endif /* _DEFS_H */

