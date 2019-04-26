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

#ifdef __cplusplus
extern "C" {
#endif

#include "h32xLog.h"
#include "rvstdio.h"
#include "msg.h"
#include "cmConfig.h"
#include <string.h>

/************************************************************************
 *
 *                          Public functions
 *
 ************************************************************************/


/************************************************************************
 * cmiCreateLog
 * purpose: Create a log manager for use by a stack instance
 *          This function reads the information from a configuration
 *          manager and initializes the log manager with it.
 * input  : app     - Stack's application handle
 * output : none
 * return : none
 ************************************************************************/
void cmiCreateLog(IN cmElem* app)
{
    app->logMgr = H32xLogGet();

    /* Create sources for the CM's log */
    RvLogSourceConstruct(app->logMgr, &app->log,       "CM",       "Conference Management");
    RvLogSourceConstruct(app->logMgr, &app->logAPI,    "CMAPI",    "Conference Management API");
    RvLogSourceConstruct(app->logMgr, &app->logCB,     "CMAPICB",  "Conference Management CallBack");
    RvLogSourceConstruct(app->logMgr, &app->logERR,    "CMERR",    "Conference Management Errors");
    RvLogSourceConstruct(app->logMgr, &app->logTPKT,   "TPKTCHAN", "TPKT Messages");
    RvLogSourceConstruct(app->logMgr, &app->logTPKTWIRE,   "TPKTWIRE", "TPKT Messages after Hook");
    RvLogSourceConstruct(app->logMgr, &app->logConfig, "CONFIG",   "Configuration settings");
    RvLogSourceConstruct(app->logMgr, &app->logAppl,   "APPL",     "User instigated messages");
    RvLogSourceConstruct(app->logMgr, &app->logWatchdog, "WATCHDOG", "Resource's data");

    msOpen();
}


/************************************************************************
 * cmiDestroyLog
 * purpose: Destroy a log manager and the CM's sources
 * input  : app     - Stack's application handle
 * output : none
 * return : none
 ************************************************************************/
void cmiDestroyLog(IN cmElem* app)
{
    if (app != NULL)
    {
        RvLogSourceDestruct(&app->log);
        RvLogSourceDestruct(&app->logAPI);
        RvLogSourceDestruct(&app->logCB);
        RvLogSourceDestruct(&app->logERR);
        RvLogSourceDestruct(&app->logTPKT);
        RvLogSourceDestruct(&app->logTPKTWIRE);
        RvLogSourceDestruct(&app->logConfig);
        RvLogSourceDestruct(&app->logAppl);
        RvLogSourceDestruct(&app->logWatchdog);
    }

    msClose();
}


/************************************************************************
 * pvtLoadFromConfig
 * purpose: Convert information from a configuration instance (HCFG) to
 *          a value tree of a given syntax.
 * input  : hCfg        - Configuration instance handle
 *          hVal        - Value tree handle
 *          name        - Name of root from configuration to convert
 *          rootNodeId  - Syntax of configuration
 *          logErrors   - RV_TRUE if we're interested in errors while
 *                        converting the configuration
 *          log         - Log handle to use for debug messages
 * output : none
 * return : Root ID of configuration created on success
 *          Negative value on failure
 ************************************************************************/
int pvtLoadFromConfig(
    IN HCFG             hCfg,
    IN HPVT             hVal,
    IN const RvChar     *name,
    IN RvPvtNodeId      rootNodeId,
    IN RvBool           logErrors,
    IN RvLogSource      *log)
{
    static char tempBuff[1024];
    int tempLen;

    RvSize_t length = strlen(name);
    char next[256], svalue[512], *str;
    RvInt32 value;
    ci_str_type strtype;

    RV_UNUSED_ARG(log);

    strncpy(next, name, sizeof(next));

    for (;;)
    {
        if (ciNext(hCfg, next, next, sizeof(next)) == ERR_CI_NOTFOUND)
            break;

        if (strncmp(name, next, length))
            break;

        ciGetValueExt(hCfg, next, &strtype, &value);

        tempLen=RvSprintf(tempBuff,"%s= (%d)", nprn(next), value);

        str = (char *)svalue;
        switch (strtype)
        {
            case ci_str_not:
            {
                str = NULL;
                break;
            }

            case ci_str_regular:
            {
                ciGetString(hCfg, next, svalue, sizeof(svalue));
                tempLen+=RvSprintf(tempBuff+tempLen, " '%*.*s'", value, value, nprn(svalue));
                break;
            }

            case ci_str_bit:
            {
                ciGetBitString(hCfg, next, svalue, sizeof(svalue),
                    (unsigned int *)&value);
                tempLen+=RvSprintf(tempBuff+tempLen, " bitstring, bits=%d", value);
                break;
            }
            default:
                break;
        }

        if (pvtBuildByPath(hVal, rootNodeId, (RvChar*) strchr(next, '.') , value, str) < 0)
        {
            if (logErrors)
            {
                RvLogError(log, (log, "Cannot create: %s", tempBuff));
            }
            else
            {
                RvLogInfo(log, (log, "Ignoring: %s", tempBuff));
            }
        }

        RvLogDebug(log, (log, "%s", tempBuff));
    }
    /*vtCheckTree(hVal, rootNodeId, -1);*/
    return 0;
}



#ifdef __cplusplus
}
#endif

