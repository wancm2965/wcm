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

/***********************************************************************
h32xLog.h

Global log used and needed in H.323, since the Common Core uses a
log manager per instance.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "h32xLog.h"


#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                           MODULE VARIABLES                            */
/*-----------------------------------------------------------------------*/

/* The general log manager we're going to use */
static RvLogMgr rvH32xLogManager;

/* todo: remove !!! */
static RvBool initialized = RV_FALSE;

/* todo: remove !!! */
static RvBool logInitCnt = 0;



/*-----------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS PROTOTYPES                    */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/*                           MODULE FUNCTIONS                            */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * H32xLogInit
 * ----------------------------------------------------------------------------
 * General: Initialize the global log manager.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  None.
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV H32xLogInit(void)
{
    RvStatus status = RV_OK;

#if (RV_LOGMASK != RV_LOGLEVEL_NONE)
	if (logInitCnt == 0)
        status = RvLogConstruct(&rvH32xLogManager);
#endif
	logInitCnt++;
    initialized = RV_TRUE;
    
    return status;
}


/******************************************************************************
 * H32xLogEnd
 * ----------------------------------------------------------------------------
 * General: Deinitialize the global log manager.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  None.
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV H32xLogEnd(void)
{
    RvStatus status = RV_OK;

	logInitCnt--;
    if (logInitCnt == 0)
	{ 	
#if (RV_LOGMASK != RV_LOGLEVEL_NONE)
        status = RvLogDestruct(&rvH32xLogManager);
#endif
        initialized = RV_FALSE;
    } 

    return status;
}


/******************************************************************************
 * H32xLogGet
 * ----------------------------------------------------------------------------
 * General: Get the global log manager for the use of H.323.
 *
 * Return Value: Global log manager used.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  None.
 * Output: None.
 *****************************************************************************/
RVAPI RvLogMgr* RVCALLCONV H32xLogGet(void)
{
    if (!initialized)
        H32xLogInit();

    return &rvH32xLogManager;
}






/*-----------------------------------------------------------------------*/
/*                           STATIC FUNCTIONS                            */
/*-----------------------------------------------------------------------*/






#ifdef __cplusplus
}
#endif
