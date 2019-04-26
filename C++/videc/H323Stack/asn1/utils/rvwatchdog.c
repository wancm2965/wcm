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

#include "rvstdio.h"
#include "rvassert.h"
#include "rvmemory.h"
#include "rvwatchdog.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif



/********************************************************************************
    RvWatchdogAllocate
    this function allocates the internal resource table of the watchdog. it is the
    first thing initialize in this module. cmInitialize calls it at the begining
    of its operation
    INPUT -
     watchdog       - Watchdog object to initialize
     numOfResources - the numbers of resources the watchdog support. it is used
                      to determine the size of the table.
     logMgr         - log manager to use
    OUTPUT - none
    RETURN -
        Non-negative value on success, other on failure
*********************************************************************************/
RVINTAPI int RVCALLCONV
RvWatchdogAllocate(
    IN RvWatchdog*  watchdog,
    IN int          numOfResources,
    IN RvLogMgr*    logMgr)
{
    RvSize_t tableSize;

    tableSize = numOfResources * sizeof(RvWatchdogTable_S);

    /*allocate place for the internal table*/
    if (RvMemoryAlloc(NULL, tableSize, logMgr, (void**)&watchdog->hTable) != RV_OK)
        return RV_ERROR_OUTOFRESOURCES;

    memset(watchdog->hTable, 0, tableSize);
    watchdog->maxSize = (RvUint32)numOfResources;
    watchdog->isInit = RV_FALSE;
    watchdog->logMgr = logMgr;

    return RV_OK;
}


/********************************************************************************
    RvWatchdogInit
    this it the main initialization of the watchdog.
    cmInitialize calls it after initializing all its modules.
    the function used the callback function already set in the internal table,
    and set initial and current values of each resource .
    INPUT -
        Hwd         - the watchdog handle
    OUTPUT - none
    RETURN -
            non negative value - if success.
            negative value     - if failed to initialize the ss instance.
*********************************************************************************/
RVINTAPI int RVCALLCONV
RvWatchdogInit(
    IN RvWatchdog*  Hwd)
{
    RvUint32 index;
    int result;
    RvWatchdogTable_S* entry;
    RvUint32         value;

    for (index = 0; index < Hwd->maxSize; index++)
    {
        entry = &(Hwd->hTable[index]);
        if (entry->initialized)
        {
            result = entry->cbFunc(entry->context, index, RvWatchdogCurrent, &value);
            if (result < 0)
                return result;
            entry->currentValue = entry->initialValue = value;
        }
    }
    Hwd->isInit = RV_TRUE;

    return 0;
}


/******************************************************************************
 * RvWatchdogAddResource
 * ----------------------------------------------------------------------------
 * General: Add a new resource element to work with the watchdog module.
 *
 * Return Value: The resource value allocated for the new resource.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hWD                      - Watchdog module handle to use.
 *         name                     - The name of the resource to use.
 *         group                    - The group this resource belongs to.
 *         description              - Short description of the resource.
 *         resourceCallback         - Callback function that is used to get the
 *                                    resource's current value.
 *         resourceDeleteCallback   - Callback function indicating this resource
 *                                    was deallocated from the watchdog.
 *         context                  - Context used for the callbacks.
 * Output: resourceEnumeration      - The enumeration of the resource that will
 *                                    be given in callback calls.
 *****************************************************************************/
RVINTAPI int RVCALLCONV
RvWatchdogAddResource(
    IN  RvWatchdog*                         hWD,
    IN  const RvChar*                       name,
    IN  const RvChar*                       group,
    IN  const RvChar*                       description,
    IN  RvWatchdogResourceCallback_T        resourceCallback,
    IN  RvWatchdogResourceDeleteCallback_T  resourceDeleteCallback,
    IN  RvWatchdogResourcePrintCallback_T   resourcePrintCallback,
    IN  void*                               context,
    OUT RvUint32*                           resourceEnumeration)
{
    RvWatchdogTable_S *entry;
    int result = 0;
    RvUint32 i;

#ifndef DEBUG
	RV_UNUSED_ARG(resourcePrintCallback);
#endif

    /* Find a place for this resource */
    for (i = 0; i < hWD->maxSize; i++)
    {
        if (!hWD->hTable[i].initialized)
            break;
    }
    if (i >= hWD->maxSize)
    {
        RvAssert(i < hWD->maxSize); /* We do this here so we know it during development phase... */
        return RV_ERROR_OUTOFRANGE; /* We're full */
    }

    entry = &(hWD->hTable[i]);
    memset(entry, 0, sizeof(RvWatchdogTable_S));
    entry->initialized  = RV_TRUE;
    entry->context      = context;
    entry->cbFunc       = resourceCallback;
    entry->delFunc      = resourceDeleteCallback;
#ifdef RV_DEBUG
    entry->printFunc    = resourcePrintCallback;
#endif

    strncpy(entry->name , name, sizeof(entry->name)-1);
    strncpy(entry->group, group, sizeof(entry->group)-1);
    entry->description = description;

    *resourceEnumeration = i;

    /* if table was already initialized, we should init this resource's data now*/
    if (hWD->isInit)
    {
        RvUint32 value;
        result = entry->cbFunc(entry->context, i, RvWatchdogCurrent, &value);
        if (result < 0)
            return result;
        entry->currentValue = entry->initialValue = value;
    }

    return result;
}



/********************************************************************************
    RvWatchdogDeleteResource
    this function is called by the various modules while to delete resources from
    the watchdog.
    INPUT -
        hApp                - the application handle - used to get the watchdog
                              handle
        resourceEnumeration - the enumeration of the resource to delete
    OUTPUT - none
    RETURN -
            non negative value - if success.
            negative value     - if failed to initialize the ss instance.
*********************************************************************************/
RVINTAPI int RVCALLCONV
RvWatchdogDeleteResource(
    IN  RvWatchdog* hWD,
    IN  RvUint32    resourceEnumeration)
{
    if (hWD->hTable[resourceEnumeration].initialized)
    {
        hWD->hTable[resourceEnumeration].initialized = RV_FALSE;
        return RV_OK;
    }

    return RV_ERROR_UNINITIALIZED;
}
#ifdef RV_DEBUG
/******************************************************************************
 * RvWatchdogPrintResource
 * ----------------------------------------------------------------------------
 * General: Print the items currently allocated from a given resource.
 *          This function can be used for debugging.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hWD                      - Watchdog module handle to use.
 *         resourceEnumeration      - The enumeration of the resource that will
 *                                    be printed.
 *         logSource                - Log to print the data out to.
 * Output: None
 *****************************************************************************/
RVINTAPI RvStatus RVCALLCONV
RvWatchdogPrintResource(
    IN RvWatchdog*                  hWD,
    IN void*                        resourceEnumeration,
    IN RvLogSource*                 logSource)
{
    RvWatchdogTable_S*  item;
    RvStatus            status = RV_ERROR_UNINITIALIZED;

    item = &hWD->hTable[(RvUint32)resourceEnumeration];
    if (item->initialized)
    {
        if (item->printFunc != NULL)
        {
            RvLogDebug(logSource, (logSource, "Allocated elements [%s]:", item->name));
            item->printFunc(item->context, logSource);
        }
        else
        {
            RvLogWarning(logSource, (logSource, "No print function for debugging '%s'", item->name));
            status = RV_ERROR_BADPARAM;
        }
    }

    return status;
}
#endif



/********************************************************************************
    RvWatchdogPrint
    this function goes over all resources from the table, get thier values and
    print it at the log file.
    INPUT -
        hWD - Watchdog handle
        logSource - Log source to print to
    OUTPUT - none
    RETURN - non negative value - if success.
             negative value     - if failed to initialize the ss instance.
*********************************************************************************/
RVINTAPI int RVCALLCONV
RvWatchdogPrint(IN RvWatchdog* hWD, IN RvLogSource* logSource)
{
    RvUint32 index;
    RvUint32 maxVal, maxUse, curVal, deltaVal;
    int useMax, useMaxUsage, useCur;
    RvChar buf[200];
    RvChar* bufPtr;

    RvWatchdogTable_S* entry;
    
    RV_UNUSED_ARG(logSource); /* Needed when compiling without logs */

    RvLogDebug(logSource, (logSource, "Resource name       |Initial data| Current   |Max usage |Max       |Delta     |"));

    for (index = 0; index < hWD->maxSize; index++)
    {
        entry = &(hWD->hTable[index]);

        if (entry->initialized)
        {
            useMax = entry->cbFunc(entry->context, index, RvWatchdogMaxVal, &maxVal);
            useMaxUsage = entry->cbFunc(entry->context, index, RvWatchdogMaxUsage, &maxUse);
            useCur = entry->cbFunc(entry->context, index, RvWatchdogCurrent, &curVal);
            deltaVal = curVal - entry->currentValue;

            bufPtr = buf;
            bufPtr += RvSprintf(bufPtr, "%19s | %11d| ", entry->name, entry->initialValue);
            if (useCur >= 0)
                bufPtr += RvSprintf(bufPtr, "%10d|", curVal);
            else
                bufPtr += RvSprintf(bufPtr, "        NA|");
            if (useMaxUsage >= 0)
                bufPtr += RvSprintf(bufPtr, "%10d|", maxUse);
            else
                bufPtr += RvSprintf(bufPtr, "        NA|");
            if (useMax >= 0)
                bufPtr += RvSprintf(bufPtr, "%10d|", maxVal);
            else
                bufPtr += RvSprintf(bufPtr, "        NA|");
            RvSprintf(bufPtr, "%10d|", deltaVal);

            RvLogDebug(logSource, (logSource, buf));
        }
    }

    return 0;
}




/********************************************************************************
    RvWatchdogNextResource
    this function gets the names of resources from the stack. It can be used
    to loop on all the resources of the stack with RvWatchdogGetResource().
    To stop looping, make sure the return value is non-negative.
    INPUT -
        hApp            - the application handle
        prevResource    - Handle to previous resource.
                          First time should be called with NULL
    OUTPUT -
        resourceHandle  - Handle to the current resource
                          Should be used as prevResourcefor the next call to this
                          function.
        resourceName    - Name of the resource (maximum of 40 bytes)
                          Can be used for RvWatchdogGetResource()
    RETURN - non negative value - if success.
             negative value     - if failed or if there are no more resource.
*********************************************************************************/
RVINTAPI int RVCALLCONV
RvWatchdogNextResource(
    IN  RvWatchdog* hWD,
    IN  void*       prevResource,
    OUT void**      resourceHandle,
    OUT RvChar*     resourceName)
{
    RvUint32 index = (RvUint32)prevResource;

    *resourceHandle = NULL;
    resourceName[0] = '\0';

    /* Find the next unused entry */
    while ((index < hWD->maxSize) && (!hWD->hTable[index].initialized))
        index++;

    if (index >= hWD->maxSize)
    {
        /* We're done... */
        return RV_ERROR_UNKNOWN;
    }

    strncpy(resourceName, hWD->hTable[index].name, 40);
    index++;
    *resourceHandle = (void*)index;

    return 0;
}


/********************************************************************************
RvWatchdogGetResource
this function get the value of a specific resource and return it
    INPUT -
        hApp  - the application handle
        name  - the resource name, used to locate resource in the table
        type  - the type of data needed (current/max/....)
    OUTPUT -
        value - the value of the resource requested
        resourceGroupName - name of group this resource belongs to. Can be passed
                            as NULL. Up to 40 characters
    RETURN - non negative value - if success.
             negative value     - if failed to initialize the ss instance.
*********************************************************************************/
RVINTAPI int RVCALLCONV
RvWatchdogGetResource(
    IN  RvWatchdog*                 hWD,
    IN  const RvChar*               name,
    IN  RvWatchdogResourceType      type,
    OUT RvUint32*                   value,
    OUT RvChar*                     resourceGroupName)
{
    RvUint32 index;
    int result = RV_ERROR_UNKNOWN;
    RvWatchdogTable_S* entry;

    for (index = 0; index < hWD->maxSize; index++)
    {
        entry = &(hWD->hTable[index]);
        if ((entry->initialized) && (!strcmp(name, entry->name)))
        {
            /* Found! */
            if (resourceGroupName != NULL)
                strncpy(resourceGroupName, entry->group, 40);

            switch (type)
            {
            case RvWatchdogInitialVal:
                {
                    *value = entry->initialValue;
                    result = 0;
                }
                break;
            case RvWatchdogMaxVal:
            case RvWatchdogMaxUsage:
            case RvWatchdogCurrent:
                {
                    result = entry->cbFunc(entry->context, index, type, value);
                }
                break;
            case RvWatchdogDeltaVal:
                {
                    result = entry->cbFunc(entry->context, index, type, value);
                    if (result >= 0)
                        *value -= entry->currentValue;
                }
                break;
            }

            return result;
        }
    }

    return result;
}



/********************************************************************************
    RvWatchdogEnd
        this function  free allocations made in the module.
    INPUT -
     watchdog       - Watchdog object to initialize
    OUTPUT - none
    RETURN -
        Non-negative value
*********************************************************************************/
RVINTAPI int RVCALLCONV
RvWatchdogEnd(IN RvWatchdog* watchdog)
{
    RvUint32 i;
    RvWatchdogTable_S* entry;

    for (i = 0; i < watchdog->maxSize; i++)
    {
        entry = &(watchdog->hTable[i]);
        if (entry->initialized && (entry->delFunc != NULL))
            entry->delFunc(entry->context, i);
    }

    RvMemoryFree(watchdog->hTable, watchdog->logMgr);
    return 0;
}



#ifdef __cplusplus
}
#endif
