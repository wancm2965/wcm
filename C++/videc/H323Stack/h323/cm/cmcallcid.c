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
#include "rvmemory.h"
#include "rvtimestamp.h"
#include "rvlock.h"
#include "cmcallcid.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(RV_DEBUG)
#define RV_CALLCID_DEBUG
#endif


#define RV_H323_UNIQUE_ID_CLOCKSEQ_SIZE RvUint16Const(16384) /* 16384 - '0100000000000000b' */

typedef struct
{
#ifdef RV_CALLCID_DEBUG
    RvUint8     lastCid[16]; /* Last CID value that was generated */
#endif
    RvUint16    clockSequence; /* Monotone clock sequence for the GUIDs */
    RvInt64     lastTimestamp; /* The last timestamp used */
    RvLock      lock; /* Lock protecting the clock sequence */
    RvUint8     randomNode[6]; /* Node field of a GUID (set to a random value) */
} RvH323UniqueIdGenerator;







/************************************************************************
 *
 *                          Public functions
 *
 ************************************************************************/

/************************************************************************
 * RvH323UniqueIdGeneratorConstruct
 * purpose: Create a globally unique identifier generator. This is needed
 *          to create call identifiers and conference identifiers in H.323
 *          calls.
 * input  : randomGenerator - Random numbers generator
 *          logMgr          - Log manager to use
 * output : none
 * return : pointer to GUID generator on success, NULL on failure
 ************************************************************************/
RvH323UniqueIdGeneratorHandle RvH323UniqueIdGeneratorConstruct(
    IN RvRandomGenerator    *randomGenerator,
    IN RvLogMgr             *logMgr)
{
    RvH323UniqueIdGenerator* guidGen;
    RvStatus res;
    RvRandom randomValue;
    int i;

    res = RvMemoryAlloc(NULL, sizeof(RvH323UniqueIdGenerator), logMgr, (void**)&guidGen);
    if (res != RV_OK)
        return NULL;

    memset(guidGen, 0, sizeof(RvH323UniqueIdGenerator));
    RvRandomGeneratorGetValue(randomGenerator, &randomValue);
    guidGen->clockSequence = (RvUint16)(randomValue % RV_H323_UNIQUE_ID_CLOCKSEQ_SIZE);
    guidGen->lastTimestamp = RvTimestampGet(logMgr);

    /* Generate a random value for the node field */
    for (i = 0; i < 6; i++)
    {
        RvRandomGeneratorGetValue(randomGenerator, &randomValue);
        guidGen->randomNode[i] = (RvUint8)randomValue;
    }
    guidGen->randomNode[5] |= 0x01; /* Indicate a random generated value and not a MAC address */

    res = RvLockConstruct(logMgr, &guidGen->lock);
    if (res != RV_OK)
    {
        RvMemoryFree(guidGen, logMgr);
        return NULL;
    }

    return (RvH323UniqueIdGeneratorHandle)guidGen;
}


/************************************************************************
 * RvH323UniqueIdGeneratorDestruct
 * purpose: Delete a globally unique identifier generator.
 * input  : uidGenerator    - GUID generator
 *          logMgr          - Log manager to use
 * output : none
 * return : RV_OK on success, other on failure
 ************************************************************************/
RvStatus RvH323UniqueIdGeneratorDestruct(
    IN RvH323UniqueIdGeneratorHandle uidGenerator,
    IN RvLogMgr                     *logMgr)
{
    RvH323UniqueIdGenerator* guidGen = (RvH323UniqueIdGenerator *)uidGenerator;

    RvLockDestruct(&guidGen->lock, logMgr);
    return RvMemoryFree(guidGen, logMgr);
}


/************************************************************************
 * RvH323UniqueIdGeneratorGet
 * purpose: Delete a globally unique identifier generator.
 * input  : uidGenerator    - GUID generator
 * output : guid            - The generated globally unique ID.
 *                            This string must be with at least 16 characters
 * return : RV_OK on success, other on failure
 ************************************************************************/
RvStatus RvH323UniqueIdGeneratorGet(
    IN  RvH323UniqueIdGeneratorHandle   uidGenerator,
    OUT RvUint8*                        guid)
{
    /* The GUID structure:
     * 0-3      low field of the timestamp
     * 4-5      middle field of the timestamp
     * 6-7      high field of the timestamp multiplexed with the version number
     * 8        high field of the clock sequence multiplexed with the variant
     * 9        low field of the clock sequence
     * 10-15    spatially unique node identifier (MAC)
     */
    RvH323UniqueIdGenerator* guidGen = (RvH323UniqueIdGenerator *)uidGenerator;
    RvInt64 timestamp, tmptime;
	RvInt i;

    /* Put the timestamp in */
    timestamp = RvTimestampGet(NULL);
	tmptime = timestamp;
	for(i = 0; i < 7; i++) {
		guid[i] = RvInt64ToRvUint8(tmptime);
		tmptime = RvInt64ShiftRight(tmptime, 8);
	}

    /* Put the 4 bit version number in the 8th byte */
    guid[7] = (RvUint8)((guid[7] & RvUint8Const(0x0f)) | 0x10); /* '0001' in binary for the msb of this byte */

    /* Let's see if we have to change the clock sequence */
    RvLockGet(&guidGen->lock, NULL);
    if (RvInt64IsEqual(timestamp, guidGen->lastTimestamp))
    {
        /* Change the clock sequence */
        guidGen->clockSequence++;
        guidGen->clockSequence %= RV_H323_UNIQUE_ID_CLOCKSEQ_SIZE;
    }
    guidGen->lastTimestamp = timestamp;
    guid[8] = (RvUint8)(guidGen->clockSequence >> 8); /* set clock_seq_hi */
    guid[9] = (RvUint8)(guidGen->clockSequence); /* set clock_seq_low to the 8 LSBs*/
    RvLockRelease(&guidGen->lock, NULL);

    /* Set the variant field to DCE */
    guid[8] &= 0xbf; /* Set 2 MSBs to '10b' */

    /* Set the MAC address to default value */
    memcpy(&guid[10], guidGen->randomNode, sizeof(guidGen->randomNode));

#ifdef RV_CALLCID_DEBUG
    /* Check to see if the last CID generated has the same value */
    RvLockGet(&guidGen->lock, NULL);
    if (memcmp(guidGen->lastCid, guid, sizeof(guidGen->lastCid)) == 0)
    {
        RvFprintf(stderr, "identical GUIDs were generated!!!\n");
        return RV_ERROR_UNKNOWN;
    }
    memcpy(guidGen->lastCid, guid, sizeof(guidGen->lastCid));
    RvLockRelease(&guidGen->lock, NULL);
#endif /* RV_CALLCID_DEBUG */

    return RV_OK;
}




#ifdef __cplusplus
}
#endif



