

/***********************************************************************************************************************

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


#ifdef __cplusplus
extern "C" {
#endif

#include "lepUtils.h"

#include "TAP_init.h"
#include "TAP_tools.h"
#include "TAP_general.h"


/********************************************************************************************
 *                                                                                          *
 *                                  Public functions                                        *
 *                                                                                          *
 ********************************************************************************************/


/******************************************************************************************
 * TclExecute
 * purpose : execute a command in tcl
 * input   : cmd - the command that is going to be executed
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ******************************************************************************************/
int TclExecute(const char* cmd, ...)
{
    va_list v;
    char ptr[2048];

    va_start(v, cmd);
    vsprintf(ptr, cmd, v);
    va_end(v);

#ifdef USE_TCL
    if (Tcl_GlobalEval(gInterp, ptr) != TCL_OK)
    {
        if ((strlen(ptr) + strlen(Tcl_GetStringResult(gInterp))) < sizeof(ptr))
            sprintf(ptr + strlen(ptr), ": %s", Tcl_GetStringResult(gInterp));
        PutError("ERROR: GlobalEval", ptr);
    }
#endif

    return TCL_OK;
}


/******************************************************************************************
 * TclString
 * purpose : make a given string a printable TCL string
 * input   : line    - Line to convert
 * output  : none
 * return  : Converted string
 ******************************************************************************************/
char* TclString(const char* line)
{
    static char buf[2048];
    char* destPtr;
    char* srcPtr;
    srcPtr = (char *)line;
    destPtr = buf;

    while (*srcPtr)
    {
        switch (*srcPtr)
        {
            case '[':
            case ']':
            case '{':
            case '}':
            case '\\':
                *destPtr++ = '\\';
            default:
                break;
        }

        *destPtr = *srcPtr;
        destPtr++;
        srcPtr++;
    }

    *destPtr = '\0';
    return buf;
}

#ifdef USE_TCL

/******************************************************************************************
 * TclGetVariable
 * purpose : get variable from tcl
 * input   : varName - the name of the variable that is going to be imported
 * input   : none
 * output  : none
 * return  : The variable's string value
 ******************************************************************************************/
char* TclGetVariable(const char* varName)
{
    char arrayName[25];
    char indexName[128];
    char* token;
    const char * result;

    /* See if it's an array variable */
    token = (char *)strchr(varName, '(');

    if (token == NULL)
        result = Tcl_GetVar(gInterp, (char *)varName, TCL_GLOBAL_ONLY);
    else
    {
        /* Array - let's first split it into array name and index name */
        int arrayLen = token - varName;
        int indexLen = strlen(token+1)-1;

        if ((arrayLen >= (int)sizeof(arrayName)) || (indexLen >= (int)sizeof(indexName)) ||
            (arrayLen > (int)strlen(varName)))
        {
            PutError(varName, "Length of array name or index out of bounds in GetVar");
            return (char *)"-unknown-";
        }

        memcpy(arrayName, varName, arrayLen);
        arrayName[arrayLen] = '\0';
        memcpy(indexName, token+1, indexLen);
        indexName[indexLen] = '\0';
        result = Tcl_GetVar2(gInterp, arrayName, indexName, TCL_GLOBAL_ONLY);
    }

    if (result == NULL)
    {
        PutError(varName, Tcl_GetStringResult(gInterp));
        return (char *)"-unknown-";
    }

    return (char*)result;
}


/******************************************************************************************
 * TclSetVariable
 * purpose : set variable in tcl
 * input   : varName - the name of the variable that is going to be set
 * input   : value - the value that is entered to the tcl variable
 * output  : none
 * return  : TCL_OK - the command was invoked successfully.
 ******************************************************************************************/
int TclSetVariable(const char* varName, const char* value)
{
    char arrayName[25];
    char indexName[128];
    char* token;
    const char * result;

    /* See if it's an array variable */
    token = (char *)strchr(varName, '(');

    if (token == NULL)
        result = Tcl_SetVar(gInterp, (char *)varName, (char *)value, TCL_GLOBAL_ONLY);
    else
    {
        /* Array - let's first split it into array name and index name */
        int arrayLen = token - varName;
        int indexLen = strlen(token+1)-1;

        if ((arrayLen >= (int)sizeof(arrayName)) || (indexLen >= (int)sizeof(indexName)) ||
            (arrayLen > (int)strlen(varName)))
        {
            PutError(varName, "Length of array name or index out of bounds in SetVar");
            return TCL_OK;
        }

        memcpy(arrayName, varName, arrayLen);
        arrayName[arrayLen] = '\0';
        memcpy(indexName, token+1, indexLen);
        indexName[indexLen] = '\0';
        result = Tcl_SetVar2(gInterp, (char *)arrayName, (char *)indexName, (char *)value, TCL_GLOBAL_ONLY);
    }

    if (result == NULL)
        PutError(varName, Tcl_GetStringResult(gInterp));

    return TCL_OK;
}


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
    IN RvStatus     status)
{
    if (status < 0)
    {
        const RvChar *errStr = NULL;
        epUtilsGetError(ep, &errStr);
        if (errStr == NULL)
        {
            Tcl_SetResult(gInterp, "Unknown error...", TCL_VOLATILE);
        }
        else
        {
            Tcl_SetResult(gInterp, (RvChar *)errStr, TCL_VOLATILE);
        }
        return TCL_ERROR;
    }

    return TCL_OK;
}

#endif

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
    IN RvInt32              lineno)
{
    void* p;

    RV_UNUSED_ARG(filename);
    RV_UNUSED_ARG(lineno);

    p = malloc(size + sizeof(void*));

    if (p != NULL)
    {
        int* pBlock = (int *)p;
        *pBlock = size;

        ep->curAllocs++;
        ep->curMemory += (RvUint32)size;

        if (ep->curAllocs > ep->maxAllocs)
            ep->maxAllocs = ep->curAllocs;
        if (ep->curMemory > ep->maxMemory)
            ep->maxMemory = ep->curMemory;

        return ((void *)((char *)p + sizeof(void*)));
    }

    return NULL;
}


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
    IN void                 *ptr)
{
    int* myAlloc;
    int size;

    myAlloc = (int *)((char *)ptr - sizeof(void*));
    size = *myAlloc;

    free(myAlloc);

    ep->curAllocs--;
    ep->curMemory -= (RvUint32)size;

    return RV_OK;
}




#ifdef __cplusplus
}
#endif

