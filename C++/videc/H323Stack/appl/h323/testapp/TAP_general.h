/************************************************************************************************************************

Notice:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*************************************************************************************************************************/


/********************************************************************************************
 * General purpose functions
 *
 * This file provides general functions for the Test Application.
 * These functions are used for initiating the test application.
 * They contain no H.323 - specific include files for the header.
 *
 ********************************************************************************************/

#ifndef _TAP_GENERAL_H
#define _TAP_GENERAL_H

#include "TAP_defs.h"

#include "lepDefs.h"

#ifdef __cplusplus
extern "C" {
#endif


int Quit(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);



/******************************************************************************************
 * TclExecute
 * purpose : execute a command in tcl
 * input   : cmd - the command that is going to be executed
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ******************************************************************************************/
int TclExecute(const char* cmd, ...);


/******************************************************************************************
 * TclString
 * purpose : make a given string a printable TCL string
 * input   : line    - Line to convert
 * output  : none
 * return  : Converted string
 ******************************************************************************************/
char* TclString(const char* line);


/******************************************************************************************
 * TclGetVariable
 * purpose : get variable from tcl
 * input   : varName - the name of the variable that is going to be imported
 * input   : none
 * output  : none
 * return  : The variable's string value
 ******************************************************************************************/
char* TclGetVariable(const char* varName);


/******************************************************************************************
 * TclSetVariable
 * purpose : set variable in tcl
 * input   : varName - the name of the variable that is going to be set
 * input   : value - the value that is entered to the tcl variable
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ******************************************************************************************/
int TclSetVariable(const char* varName, const char* value);


/******************************************************************************
 * TclReturnValue
 * ----------------------------------------------------------------------------
 * General: Check the return value for a TCL wrapper function and set the
 *          error string accordingly.
 *
 * Return Value: Appropriate TCL return value.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         status       - Returned status.
 * Output: None.
 *****************************************************************************/
int TclReturnValue(
    IN EpObj        *ep,
    IN RvStatus     status);


/******************************************************************************
 * epMalloc
 * ----------------------------------------------------------------------------
 * General: Dynamic allocation function to call (the equivalent of malloc()).
 *
 * Return Value: Pointer to allocated memory on success, NULL on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         size         - Size of allocation requested.
 *         filename     - Name of the file doing the alloc.
 *         lineno       - Line number doing the alloc.
 * Output: None.
 *****************************************************************************/
void* epMalloc(
    IN EpObj                *ep,
    IN RvSize_t             size,
    IN const RvChar         *filename,
    IN RvInt32              lineno);


/******************************************************************************
 * epFree
 * ----------------------------------------------------------------------------
 * General: Request user to allocate a resource identifier for an object.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep           - Endpoint object to use.
 *         ptr          - Pointer of memory to free.
 * Output: None.
 *****************************************************************************/
RvStatus epFree(
    IN EpObj                *ep,
    IN void                 *ptr);




#ifdef __cplusplus
}
#endif

#endif  /* _TAP_GENERAL_H */
