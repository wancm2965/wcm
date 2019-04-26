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

#include "rvinternal.h"
#include "rvstdio.h"
#include "cmintr.h"
#include "cmdebprn.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Make sure that both ENTER and LEAVE are declared */
#if (RV_LOGMASK & (RV_LOGLEVEL_ENTER || RV_LOGLEVEL_LEAVE))
#if (!(RV_LOGMASK & RV_LOGLEVEL_ENTER))
#error RV_LOGMASK must have both ENTER and LEAVE. It cannot have only LEAVE!
#endif
#if (!(RV_LOGMASK & RV_LOGLEVEL_LEAVE))
#error RV_LOGMASK must have both ENTER and LEAVE. It cannot have only ENTER!
#endif


void _cmiAPIEnter(HAPP hApp,const char*line,...)
{
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
    cmElem* app = (cmElem *)hApp;

    if (RvLogIsSelected(&app->logAPI, RV_LOGLEVEL_ENTER))
    {
        RvChar buf[1024];
        va_list v;
        va_start(v, line);
        RvVsnprintf(buf, sizeof(buf), line, v);
        buf[sizeof(buf)-1] = '\0';

        cmLock(hApp);
        app->level++;
        RvLogTextEnter(&app->logAPI, "%d >>>%*c%s", app->level, app->level * 3, ' ', buf);
        cmUnlock(hApp);
    }
#else
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(line);
#endif
}

void _cmiAPIExit(HAPP hApp,const char*line,...)
{
#if (RV_LOGMASK & RV_LOGLEVEL_LEAVE)
    cmElem* app = (cmElem *)hApp;

    if (RvLogIsSelected(&app->logAPI, RV_LOGLEVEL_LEAVE))
    {
        RvChar buf[1024];
        va_list v;
        va_start(v, line);
        RvVsnprintf(buf, sizeof(buf), line, v);
        buf[sizeof(buf)-1] = '\0';

        cmLock(hApp);
        RvLogTextLeave(&app->logAPI, "%d <<<%*c%s", app->level, app->level * 3, ' ', buf);
        app->level--;
        cmUnlock(hApp);
    }
#else
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(line);
#endif
}

void _cmiCBEnter(HAPP hApp,const char*line,...)
{
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
    cmElem* app = (cmElem *)hApp;

    if (RvLogIsSelected(&app->logCB, RV_LOGLEVEL_ENTER))
    {
        RvChar buf[1024];
        va_list v;
        va_start(v, line);
        RvVsnprintf(buf, sizeof(buf), line, v);
        buf[sizeof(buf)-1] = '\0';

        cmLock(hApp);
        app->level++;
        RvLogTextEnter(&app->logCB, "%d >>>%*c%s", app->level, app->level * 3, ' ', buf);
        cmUnlock(hApp);
    }
#else
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(line);
#endif
}

void _cmiCBExit(HAPP hApp,const char*line,...)
{
#if (RV_LOGMASK & RV_LOGLEVEL_LEAVE)
    cmElem* app = (cmElem *)hApp;

    if (RvLogIsSelected(&app->logCB, RV_LOGLEVEL_LEAVE))
    {
        RvChar buf[1024];
        va_list v;
        va_start(v, line);
        RvVsnprintf(buf, sizeof(buf), line, v);
        buf[sizeof(buf)-1] = '\0';

        cmLock(hApp);
        RvLogTextLeave(&app->logCB, "%d <<<%*c%s", app->level, app->level * 3, ' ', buf);
        app->level--;
        cmUnlock(hApp);
    }
#else
    RV_UNUSED_ARG(hApp);
    RV_UNUSED_ARG(line);
#endif
}


#endif


#ifdef __cplusplus
}
#endif

