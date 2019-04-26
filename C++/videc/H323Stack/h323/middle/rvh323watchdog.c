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
#include "rvwatchdog.h"
#include "cm.h"
#include "cmintr.h"
#include "rvh323watchdog.h"

#ifdef __cplusplus
extern "C" {
#endif


/* The following functions are declared and implemented in cmCrossReference */
RVAPI RvWatchdog* RVCALLCONV cmiGetWatchdogHandle(IN HAPP hApp);
RvLogSource* cmiGetWatchdogLogSource(IN HAPP hApp);




/********************************************************************************
    RvH323WatchdogPrint
    this function goes over all recources from the table, get thier values and
    print it at the log file.
    INPUT -
        hApp  - the application handle
    OUTPUT - none
    RETURN - non negative value - if success.
             negative value     - if failed to initialize the ss instance.
*********************************************************************************/
RVAPI int RVCALLCONV RvH323WatchdogPrint(IN HAPP hApp)
{
    RvWatchdog* hWD;

    if (hApp == NULL)
        return RV_ERROR_UNKNOWN;

    hWD = cmiGetWatchdogHandle(hApp);

    return RvWatchdogPrint(hWD, cmiGetWatchdogLogSource(hApp));
}
#ifdef RV_DEBUG
/********************************************************************************
    RvH323WatchdogPrintResourceInfo
    This function prints out the information on allocated elements of a given
    resource type. It should be used for debugging purposes only.
    INPUT -
        hApp  - The application handle.
        name  - The name of resource to print out.
    OUTPUT - none
    RETURN - RV_OK for success, other on failure.
*********************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323WatchdogPrintResourceInfo(
    IN HAPP             hApp,
    IN const RvChar*    name)
{
    cmElem*     app = (cmElem *)hApp;
    RvWatchdog* hWD;
    RvChar      resourceName[20];
    void*       resource = NULL;

    hWD = cmiGetWatchdogHandle(hApp);

    while (RvWatchdogNextResource(hWD, resource, &resource, resourceName) >= 0)
    {
        if (strcmp(name, resourceName) == 0)
        {
            return RvWatchdogPrintResource(hWD, resource, &app->logWatchdog);
        }
    }

    return RV_ERROR_NOT_FOUND;
}
#endif


/********************************************************************************
    RvH323WatchdogNextResource
    this function gets the names of resources from the H.323 stack. It can be used
    to loop on all the resources of the stack with RvH323WatchdogGetResource().
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
                          Can be used for RvH323WatchdogGetResource()
    RETURN - non negative value - if success.
             negative value     - if failed or if there are no more resource.
*********************************************************************************/
RVAPI int RVCALLCONV RvH323WatchdogNextResource(
    IN  HAPP    hApp,
    IN  void*   prevResource,
    OUT void**  resourceHandle,
    OUT char*   resourceName)
{
    RvWatchdog* hWD;

    if (hApp == NULL)
        return RV_ERROR_UNKNOWN;

    hWD = cmiGetWatchdogHandle(hApp);

    return RvWatchdogNextResource(hWD, prevResource, resourceHandle, resourceName);
}


/********************************************************************************
RvH323WatchdogGetResource
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
RVAPI int RVCALLCONV RvH323WatchdogGetResource(
    IN  HAPP                        hApp,
    IN  const char*                 name,
    IN  RvH323WatchdogResourceType  type,
    OUT RvUint32*                   value,
    OUT char*                       resourceGroupName)
{
    RvWatchdog* hWD;

    if (hApp == NULL)
        return RV_ERROR_UNKNOWN;

    hWD = cmiGetWatchdogHandle(hApp);

    return RvWatchdogGetResource(hWD, name, (RvWatchdogResourceType)type, value, resourceGroupName);
}


#ifdef __cplusplus
}
#endif
