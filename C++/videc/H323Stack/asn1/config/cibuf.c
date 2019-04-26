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

/***************************************************************************

  cibuf.c  --  Configuration load/save functions - buffer version

  Abstract:       CI routines for loading and saving the configuration to
                  and from buffers.

                  The module supplies routines for creating an managing
                  these buffers, as exported by cibuf.h.

  Platforms:      Supports all platforms.  Integer data is always output
                  in network order.

***************************************************************************/


#include "rvtypes.h"
#include "rvstdio.h"
#include "rvmemory.h"
#include "rvnet2host.h"
#include "rtree.h"
#include "rpool.h"

#include "ci.h"
#include "cisupp.h"
#include "cibuf.h"


#ifdef __cplusplus
extern "C" {
#endif




/* the buffer header - DONT USE A STRUCT because byte allignment
   could give wrong offset calculations:

     0               4               8
   +---+---+---+---+---+---+---+---+---+---+---+---+
   | ID STRING                     | SIZE OF DATA  |
   +---+---+---+---+---+---+---+---+---+---+---+---+

    12              16              20
   +---+---+---+---+---+---+---+---+---+---+---+---+ ...
   | # NODES       | STR DATA SIZE | DATA
   +---+---+---+---+---+---+---+---+---+---+---+---+ ...

   all int values in network order.
*/

/* CI_BUF_XXX_OFFSET macros define the offset of field XXX in the buffer */
#define CI_BUF_SIZE_OFFSET       8                             /* RvUint32 */
#define CI_BUF_NODES_OFFSET      (CI_BUF_SIZE_OFFSET    + 04)  /* RvUint32 */
#define CI_BUF_STRSIZE_OFFSET    (CI_BUF_NODES_OFFSET   + 04)  /* RvUint32 */
#define CI_BUF_DATA_OFFSET       (CI_BUF_STRSIZE_OFFSET + 04)  /* RvUint32 */

/* CI_BUF_XXX() macros etract the XXX field from a buffer */
#define CI_BUF_SIZE(buf)         (*(RvUint32 *)(buf + CI_BUF_SIZE_OFFSET))
#define CI_BUF_NODES(buf)        (*(RvUint32 *)(buf + CI_BUF_NODES_OFFSET))
#define CI_BUF_STRSIZE(buf)      (*(RvUint32 *)(buf + CI_BUF_STRSIZE_OFFSET))
#define CI_BUF_DATA(buffer)      (buffer + CI_BUF_DATA_OFFSET)



/* the data - formatted as below.  The structure is NEVER read or written
   as a whole in order to overcome compiler allignment calculations:
    RvInt32  level;
    RvBool   isString;
    RvInt32  value;
    RvUint32 nameLen;
    name data
    string data (optional)
*/
typedef struct
{
  RvInt32   level;
  RvInt8    isString;
  RvInt32   value;
  RvUint32  nameLen;
} bufInfoBlock;



static void buildFromBuffer(const char **pos, const char *end, HRPOOL pool,
                            HRTREE tree, int nodeID, int level);

static int outputToBuffer(char **pos, char *end, HRTREE tree, int nodeID,
                          int level, RvUint32 *nodes, RvUint32 *dataSize,
                          int estimateOnly, HRPOOL pool);



                  /* == Exported Functions (cibuf.h) == */

RVAPI
RvInt32 RVCALLCONV ciTargetBufferSize(IN HCFG hCfg)
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    int retVal;
    RvUint32 a=0, b=0;
    char *pos = 0;

    if (hCfg == NULL)
        return (RvInt32)ERR_CI_GENERALERROR;

    RvMutexLock(&cfg->mutex, cfg->logMgr);

    retVal = outputToBuffer(&pos, 0, cfg->tree, rtRoot(cfg->tree),
        0, &a, &b, 1, cfg->pool);

    RvMutexUnlock(&cfg->mutex, cfg->logMgr);

    if (retVal < 0)
        return (RvInt32)ERR_CI_GENERALERROR;

    return (RvInt32)(pos - (char *)0) + CI_BUF_DATA_OFFSET;
}

RVAPI
int RVCALLCONV ciPrepareTargetBuffer(
    IN OUT   void *  buffer,
    IN       RvInt32 bufferSize)
{
    if (bufferSize < (RvInt32)(CI_BUF_DATA_OFFSET + sizeof(bufInfoBlock)))
        return (int)ERR_CI_BUFFERTOOSMALL;

    strcpy((char*)buffer, CI_BUF_ID_STRING);
    CI_BUF_SIZE((char *)buffer) = RvConvertHostToNetwork32((RvUint32)bufferSize);

    return 0;
}

RVAPI
int RVCALLCONV ciAllocateTargetBuffer(
    IN  HCFG    hCfg,
    OUT void ** buffer,
    OUT int *   bufferSize)
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    int size = ciTargetBufferSize(hCfg), err;

    *buffer = 0;

    if (bufferSize)
    {
        *bufferSize = 0;
    }

    if (size < 0)
        return size;

    if (RvMemoryAlloc(NULL, (RvSize_t)size, cfg->logMgr, (void**)buffer) != RV_OK)
        return (int)ERR_CI_ALLOCERROR;

    if ((err = ciPrepareTargetBuffer(*buffer, size)) < 0)
    {
        RvMemoryFree(*buffer, cfg->logMgr);
        *buffer = 0;
        return err;
    }

    *bufferSize = size;

    return 0;
}

RVAPI
int RVCALLCONV ciFreeTargetBuffer(IN  HCFG    hCfg,
                                IN  void *  buffer
                                )
{
    cfgHandle* cfg = (cfgHandle *)hCfg;

    RvMemoryFree(buffer, cfg->logMgr);

    return 0;
}

                      /* == CI Interface Functions == */

int ciIDBuffer(const char *source)
{
    return (!(memcmp(source, CI_BUF_ID_STRING, CI_BUF_ID_LEN)));
}


int ciEstimateCfgSizeBuffer(cfgHandle *cfg, const char *source, int *nodes, int *data)
{
    RV_UNUSED_ARG(cfg);

    *nodes = (int)(3 * RvConvertNetworkToHost32(CI_BUF_NODES(source)));
    *data  = (int)(3 * RvConvertNetworkToHost32(CI_BUF_STRSIZE(source)));

    return 0;
}

int ciBuildFromBuffer(cfgHandle *cfg, const char *source, HRTREE tree, HRPOOL pool)
{
    RvUint32 bufSize;
    const char *end;
    const char *pos;
    RvUint32 temp;

    RV_UNUSED_ARG(cfg);

    /* The next 2 statements together is supposed to be equivalent to the
       original statement bufSize  = rv_ntohl(CI_BUF_SIZE(source)), but
       without the integer alignment problem. In the original statement,
       if source is not aligned with an integer. then parameter
       CI_BUF_SIZE(source) may be an unexpected value to rv_ntohl.
    */
    memcpy(&temp, source + CI_BUF_SIZE_OFFSET, sizeof(RvUint32));
    bufSize = RvConvertNetworkToHost32(temp);

    end = source + bufSize;
    pos = CI_BUF_DATA(source);

    buildFromBuffer(&pos, end, pool, tree, rtRoot(tree), 0);

    return 0;
}


int ciOutputToBuffer(const char *target, HRTREE tree, HRPOOL pool)
{
    RvUint32 *bufSize = &CI_BUF_SIZE(target);
    char *end       = (char *)target + RvConvertNetworkToHost32(*bufSize);
    char *pos       = CI_BUF_DATA((char *)target);
    RvInt32 retVal;

    if ((end - pos) < CI_BUF_DATA_OFFSET)
        return (int)ERR_CI_BUFFERTOOSMALL;

    CI_BUF_NODES  (target) = 0;
    CI_BUF_STRSIZE(target) = 0;

    if ((retVal = outputToBuffer(&pos, end, tree, rtRoot(tree), 0,
        &CI_BUF_NODES(target), &CI_BUF_STRSIZE(target), 0, pool)) < 0)
        return retVal;

    CI_BUF_NODES(target) = RvConvertHostToNetwork32(CI_BUF_NODES(target));
    CI_BUF_STRSIZE(target) = RvConvertHostToNetwork32(CI_BUF_STRSIZE(target));

    *bufSize = (RvUint32)((char *)pos - (char *)target);
    *bufSize = RvConvertHostToNetwork32(*bufSize);

    return 0;
}

static void bufferReadRpool(const char **bufPtr, HRPOOL rpool, HRPOOLELEM dest, int size)
{
    rpoolCopyFromExternal(rpool, dest, (void *)*bufPtr, 0, size);
    *bufPtr += size;
}

static void bufferRead(const char **bufPtr, char *dest, int size)
{
    memcpy(dest, *bufPtr, (RvSize_t)size);
    *bufPtr += size;
}

static int bufferWrite(char **bufPtr, char *end, char *src, int size)
{
    if (*bufPtr + size - 1 > end)
        return (int)ERR_CI_BUFFERTOOSMALL;

    memcpy(*bufPtr, src, (RvSize_t)size);
    *bufPtr += size;

    return 0;
}

static void buildFromBuffer(const char **pos, const char *end, HRPOOL pool,
                            HRTREE tree, int nodeID, int level)
{
    bufInfoBlock info;
    int newNodeID;
    cfgValue cfgVal;
    const char *lastPos;
    RvUint32 tmpValue;

    while (*pos < end)
    {
        lastPos = *pos;

        bufferRead(pos, (char *)&info.level, sizeof(info.level));
        tmpValue = (RvUint32)info.level;
        info.level = (RvInt32)(RvConvertNetworkToHost32(tmpValue));

        if (info.level < level)
        {
            /* a higher level, we must exit */
            *pos = lastPos;
            return;
        }

        bufferRead(pos, (char *)&info.isString, sizeof(info.isString));
        bufferRead(pos, (char *)&info.value,    sizeof(info.value));
        bufferRead(pos, (char *)&info.nameLen,  sizeof(info.nameLen));
        tmpValue = (RvUint32)info.value;
        info.value = (RvInt32)RvConvertNetworkToHost32(tmpValue);
        tmpValue = (RvUint32)info.nameLen;
        info.nameLen = RvConvertNetworkToHost32(tmpValue);

        cfgVal.isString = (RvBool)(info.isString != 0);
        cfgVal.value    = info.value;
        cfgVal.name     = rpoolAlloc(pool, (int)info.nameLen);
        bufferReadRpool(pos, pool, cfgVal.name, (int)(info.nameLen));
        if (cfgVal.isString)
        {
            cfgVal.str = rpoolAlloc(pool, cfgVal.value);
            bufferReadRpool(pos, pool, cfgVal.str, cfgVal.value);
        }
        else
            cfgVal.str = NULL;

        if (level == info.level)
        {
            newNodeID = rtAddBrother(tree, nodeID, &cfgVal);
            nodeID = newNodeID;
        }
        else
            newNodeID = rtAddTail(tree, nodeID, &cfgVal);

        if (nodeID != newNodeID)  /* nest to next level */
            buildFromBuffer(pos, end, pool, tree, newNodeID, level + 1);
    }
}

#define WRITE_AND_CHECK(dest, size) \
  { \
    if (estimateOnly) \
      (*pos) += size; \
    else \
      if ((retVal = bufferWrite(pos, end, dest, size)) < 0) \
        return retVal; \
  }

static int outputToBuffer(char **pos, char *end, HRTREE tree, int nodeID,
                          int level, RvUint32 *nodes, RvUint32 *dataSize,
                          int estimateOnly, HRPOOL pool)
{
    pcfgValue cfgVal;
    bufInfoBlock info;
    int child, retVal;
    char buff[MAX_CONFIG_TEMP_BUFFER_SIZE];
    RvUint32 nameLen;
    RvUint32 tmpValue;
    int chunkSize;

    for (;;)
    {
        if (level != 0)
        {
            cfgVal = (pcfgValue)rtGetByPath(tree, nodeID);

            tmpValue = (RvUint32)level;
            info.level    = (RvInt32)RvConvertHostToNetwork32(tmpValue);
            info.isString = (char)(cfgVal->isString != 0);
            tmpValue = (RvUint32)cfgVal->value;
            info.value    = (RvInt32)RvConvertHostToNetwork32(tmpValue);
            nameLen = (RvUint32)rpoolChunkSize(pool, cfgVal->name);
            info.nameLen  = RvConvertHostToNetwork32(nameLen);

            WRITE_AND_CHECK((char *)&info.level,    sizeof(info.level));
            WRITE_AND_CHECK((char *)&info.isString, sizeof(info.isString));
            WRITE_AND_CHECK((char *)&info.value,    sizeof(info.value));
            WRITE_AND_CHECK((char *)&info.nameLen,  sizeof(info.nameLen));
            rpoolCopyToExternal(pool, (void*)buff, cfgVal->name, 0, MAX_CONFIG_TEMP_BUFFER_SIZE);
            buff[rpoolChunkSize(pool, cfgVal->name)] = 0;
            tmpValue = RvConvertNetworkToHost32(info.nameLen);
            WRITE_AND_CHECK(buff, (int)tmpValue);

            if (info.isString)
            {
                rpoolCopyToExternal(pool, (void*)buff, cfgVal->str, 0, MAX_CONFIG_TEMP_BUFFER_SIZE);
                chunkSize = rpoolChunkSize(pool, cfgVal->str);
                if (chunkSize >= 0)
                    buff[chunkSize] = 0;
                WRITE_AND_CHECK(buff, cfgVal->value);
                *dataSize += cfgVal->value;
            }

            (*nodes)++;
            *dataSize += RvConvertNetworkToHost32(info.nameLen) + 1;
        }

        /* process children */
        child = rtHead(tree, nodeID);
        if (child >= 0)
        {
            retVal = outputToBuffer(pos, end, tree, child, level + 1, nodes,
                dataSize, estimateOnly, pool);
            if (retVal < 0)
                return retVal;
        }

        /* move to brother */
        nodeID = rtBrother(tree, nodeID);
        if (nodeID < 0)
            return 0;
    }
}


#ifdef __cplusplus
}
#endif



