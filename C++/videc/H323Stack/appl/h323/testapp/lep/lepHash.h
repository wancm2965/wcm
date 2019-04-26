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

#ifndef _RV_EP_HASH_H_
#define _RV_EP_HASH_H_

/***********************************************************************
epHash.h

General hash table support.
This is used to hold the different objects of the test application.
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "lepDefs.h"



#ifdef __cplusplus
extern "C" {
#endif

    
/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/







/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * epHashInit
 * ----------------------------------------------------------------------------
 * General: Initialize a hash object.
 *
 * Return Value: Hash object handle on success, NULL on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ep       - Endpoint object to use.
 *         elemSize - Size of elements to store in hash.
 * Output: None.
 *****************************************************************************/
HashHANDLE epHashInit(
    IN EpObj    *ep,
    IN RvSize_t elemSize);


/******************************************************************************
 * epHashEnd
 * ----------------------------------------------------------------------------
 * General: Destruct a hash object.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 * Output: None.
 *****************************************************************************/
RvStatus epHashEnd(
    IN HashHANDLE       hashH);


/******************************************************************************
 * epHashAdd
 * ----------------------------------------------------------------------------
 * General: Add a new element to the hash.
 *          Only elements with new id's can be added.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 *         id       - Id to use for hash element.
 * Output: elem     - Element allocated.
 *****************************************************************************/
RvStatus epHashAdd(
    IN  HashHANDLE      hashH,
    IN  RvInt32         id,
    OUT void            **elem);


/******************************************************************************
 * epHashRemove
 * ----------------------------------------------------------------------------
 * General: Remove an element from the hash.
 *
 * Return Value: RV_OK on success, other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 *         id       - Id to use for hash element.
 * Output: None.
 *****************************************************************************/
RvStatus epHashRemove(
    IN  HashHANDLE      hashH,
    IN  RvInt32         id);


/******************************************************************************
 * epHashGet
 * ----------------------------------------------------------------------------
 * General: Get an element from the hash.
 *
 * Return Value: The element found. NULL if not found.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 *         id       - Id to use for hash element.
 * Output: None.
 *****************************************************************************/
void *epHashGet(
    IN  HashHANDLE      hashH,
    IN  RvInt32         id);


/******************************************************************************
 * epHashGetAny
 * ----------------------------------------------------------------------------
 * General: Get an element that was allocated in the hash.
 *          This function is SLOW and runs in o(n).
 *
 * Return Value: The element found. NULL if not found.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hashH    - Hash object handle to use.
 *         key      - Starting point of search. Pointer to NULL to begin from
 *                    the first element.
 * Output: key      - Starting point for the next search.
 *****************************************************************************/
void *epHashGetAny(
    IN      HashHANDLE  hashH,
    INOUT   void        **key);




#ifdef __cplusplus
}
#endif

#endif /* _RV_EP_HASH_H_ */
