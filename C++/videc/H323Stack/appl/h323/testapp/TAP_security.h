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


#ifndef _TAP_SECURITY_H
#define _TAP_SECURITY_H

#include "TAP_general.h"
#include "TAP_defs.h"
#include "TAP_call.h"


#ifdef __cplusplus
extern "C" {
#endif


int LDAP_init(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);
int LDAP_end(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[]);



#ifdef __cplusplus
}
#endif

#endif /*_TAP_SECURITY_H*/
