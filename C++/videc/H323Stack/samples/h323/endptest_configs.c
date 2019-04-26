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


/********************************************************************************
 * endptest_configs.c
 * This file is generated automatically.
 * It contains all of the config.val files necessary for the sample application
 * in the same directory on an operating system without a file system.
 ********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#include "ci.h"
#include <string.h>

extern const char* myValFile0[];

static int myValIndexed = 0;
static char** myValIndex[1];

static void valIndexFiles(void)
{
    myValIndexed = 1;
    myValIndex[0] = (char**)myValFile0;
}


typedef struct
{
    char **buf;
    int curLine;
} viFilePos;

static RvStatus RVCALLCONV virtualFileOpenEv(IN void* context)
{
    viFilePos *p = (viFilePos *)context;
    p->curLine = 1;
    return RV_OK;
}


static RvStatus RVCALLCONV virtualFileCloseEv(IN void* context)
{
    RV_UNUSED_ARG(context);
    return RV_OK;
}


static RvStatus RVCALLCONV virtualFileReadLineEv(IN void* context, IN RvChar* buffer, IN RvSize_t bufferSize)
{
    viFilePos *p = (viFilePos *)context;
    if (p->buf[p->curLine] == NULL)
        return RV_ERROR_UNKNOWN; /* eof */
    strncpy(buffer, p->buf[p->curLine], bufferSize);
    p->curLine++;
    return RV_OK;
}


char* valGetFile(const char* name)
{
    int i;

    if (myValIndexed == 0) valIndexFiles();

    for (i = 0; i < 1; i++)
    {
        if (strcmp(name, myValIndex[i][0]) == 0)
        {
            /* Got a match */
            static RvChar source[CI_VIRTUAL_FILE_SOURCE_SIZE+sizeof(viFilePos)];
            viFilePos *p = (viFilePos *)(source+CI_VIRTUAL_FILE_SOURCE_SIZE);
            p->buf = myValIndex[i];
            ciCreateVirtualFileSource(source, source+CI_VIRTUAL_FILE_SOURCE_SIZE,
                virtualFileOpenEv, virtualFileCloseEv, virtualFileReadLineEv);
            return source;
        }
    }

    return NULL;
}


#ifdef __cplusplus
}
#endif
