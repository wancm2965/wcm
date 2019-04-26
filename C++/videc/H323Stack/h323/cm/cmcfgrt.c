/*

NOTICE:
This document contains information that is proprietary to RADVISION LTD.
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD.

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*/


/****************************************************************************

  cmcfgrt.c  --  Configuration Roots

  This Comment:   25-May-1997
  Module Author:  Oz Solomonovich

  Abstract:       Interface to the configuration root names.

  Platforms:      All.

  Known Bugs:     None.

****************************************************************************/


#include "rvinternal.h"
#include "rvstdio.h"
#include "cm.h"
#include "cmcfgrt.h"
#include "cmdebprn.h"

#ifdef __cplusplus
extern "C" {
#endif


static
const RvChar *asnList[] =
{
    "SystemConfiguration",
    "RASConfiguration",
    "Q931Configuration",
    "H245Configuration",

    "H450SystemConfiguration",
    "CTConfig",
    "CDConfig",
    "CHConfig",
    "CPConfig",
    "CWConfig",
    "MWIConfig",
    "CCConfig",
    "COConfig",
    "CIConfig",
    "CMNConfig",
};

static
RvUint8*
    (RVCALLCONV*syntaxList[])(void) =
{
    cmEmGetCommonSyntax,
    cmEmGetQ931Syntax,
    cmEmGetQ931Syntax,
    cmEmGetH245Syntax,

    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax,
    cmEmGetCommonSyntax
};

static
const RvChar *cfgList[] =
{
    "system",
    "RAS",
    "Q931",
    "h245",
    "h450system",
    "h4502",
    "h4503",
    "h4504",
    "h4505",
    "h4506",
    "h4507",
    "h4509",
    "h45010",
    "h45011",
    "h45012"
};


RVAPI void RVCALLCONV
cmGetCfgRootNameList(
                    IN   HAPP               hApp,
                    OUT  int *              count,
                    OUT  const char ***     list)
{
    RV_UNUSED_ARG(hApp);
    /*if (!hApp) return;*/
    /*cmiAPIEnter((hApp, "cmGetCfgRootNameList: hApp=%p",hApp));*/

    if (count)
        *count = sizeof(cfgList)/sizeof(char *);

    if (list)
        *list = cfgList;
    /*cmiAPIExit((hApp, "cmGetCfgRootNameList: hApp=%p count=%d",hApp,*count));*/
}

RVAPI const unsigned char * RVCALLCONV
cmGetSyntaxFromCfgName(
                    IN   HAPP               hApp,
                    IN   const char *       rootName)
{
    int i;

    RV_UNUSED_ARG(hApp);
    /*if (!hApp) return NULL;*/
    /*cmiAPIEnter((hApp, "cmGetSyntaxFromCfgName: hApp=%p,rootName %s",hApp,nprn(rootName)));*/

    for (i = 0; i < (int)(sizeof(asnList)/sizeof(char *)); i++)
    {
        if (!strcmp(cfgList[i], rootName))
        {
            /*cmiAPIExit((hApp, "cmGetSyntaxFromCfgName: hApp=%p",hApp));*/
            return syntaxList[i]();
        }
    }
    /*cmiAPIExit((hApp, "cmGetSyntaxFromCfgName: hApp=%p rootName null",hApp));*/
    return NULL;
}

RVAPI const char * RVCALLCONV
cmGetAsnNameFromCfgName(
                    IN   HAPP               hApp,
                    IN   const char *       rootName)
{
    int i;

    RV_UNUSED_ARG(hApp);
    /*if (!hApp) return NULL;*/
    /*cmiAPIEnter((hApp, "cmGetAsnNameFromCfgName: hApp=%p,rootName %s",hApp,nprn(rootName)));*/

    for (i = 0; i < (int)(sizeof(asnList)/sizeof(char *)); i++)
    {
        if (!strcmp(cfgList[i], rootName))
        {
            /*cmiAPIExit((hApp, "cmGetAsnNameFromCfgName: hApp=%p asnName %s",hApp,nprn(asnList[i])));*/
            return asnList[i];
        }
    }
    /*cmiAPIExit((hApp, "cmGetAsnNameFromCfgName: hApp=%p rootName null",hApp));*/
    return NULL;
}


RVAPI const char * RVCALLCONV
cmGetCfgNameFromAsnName(
                    IN   HAPP               hApp,
                    IN   const char *       asnName)
{
    int i;

    RV_UNUSED_ARG(hApp);
    /*if (!hApp) return NULL;*/
    /*cmiAPIEnter((hApp, "cmGetCfgNameFromAsnName: hApp=%p asnName %s",hApp,nprn(asnName)));*/

    for (i = 0; i < (int)(sizeof(asnList)/sizeof(char *)); i++)
    {
        if (!strcmp(asnList[i], asnName))
        {
            /*cmiAPIExit((hApp, "cmGetCfgNameFromAsnName: hApp=%p cfgName %s",hApp,nprn(cfgList[i])));*/
            return cfgList[i];
        }
    }
    /*cmiAPIExit((hApp, "cmGetCfgNameFromAsnName: hApp=%p cfgName null",hApp));*/
    return NULL;
}



#ifdef __cplusplus
}
#endif



