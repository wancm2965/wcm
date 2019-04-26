/***********************************************************************************************
  Notice:
  This document contains information that is proprietary to RADVISION LTD..
  No part of this publication may be reproduced in any form whatsoever without
  written prior approval by RADVISION LTD..

    RADVISION LTD. reserves the right to revise this publication and make changes
    without obligation to notify any person of such revisions or changes.

*************************************************************************************************/

/********************************************************************************************
*                                TAP_security.c
*
* This file contains all the functions which enable the use of security modes
*
*
*      Written by                        Version & Date                        Change
*     ------------                       ---------------                      --------
*                                         10-Jan-2001
*
********************************************************************************************/
/*****************************************************************************************/

/*****************************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "TAP_security.h"
#include "lepLdap.h"


/* LDAP.init <serverName> <userDN> <password> <group> */
int LDAP_init(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;
    RvStatus st;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
    RV_UNUSED_ARG(ep);

    st = epLdapInit(ep, argv[1], argv[2], argv[3], argv[4]);
    if (st == RV_ERROR_NOTSUPPORTED)
    {
        TclExecute("test:Log {LDAP is not supported}");
        return RV_OK;
    }
    return st;
}


int LDAP_end(ClientData clientData, Tcl_Interp *interp,int argc, char *argv[])
{
    EpObj *ep = (EpObj *)clientData;

    RV_UNUSED_ARG(interp);
    RV_UNUSED_ARG(argc);
    RV_UNUSED_ARG(argv);
    RV_UNUSED_ARG(ep);

    return epLdapEnd(ep);
}


#ifdef __cplusplus
}
#endif
