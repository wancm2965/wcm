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

  ci.c  --  Configuration Interface

  Abstract:      Low level configuration interface for the H323 Stack.

                 Configuration information can reside in a number of
                 different sources.  CI converts the data from the
                 configuration source into an internal R-Tree, with the
                 aid of the helper functions found in cisupp.c.

***************************************************************************/

#include "rvmemory.h"
#include "rvstdio.h"
#include "rvcbase.h"
#include "rtree.h"
#include "rpool.h"

#include "h32xLog.h"

#include "ci.h"
#include "cisupp.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


#define TMP_LOG H32xLogGet()

/* local functions */
static int ciGetNodeID(IN HCFG hCfg, IN const char *path);


#define CONFIG_POOL_OVERHEAD  4


RVAPI
HCFG RVCALLCONV ciConstruct(IN const char *source)
{
    return ciConstructEx(source, 0, 0);
}

RVAPI
HCFG RVCALLCONV ciConstructEx(
        IN  const char * source,     /* configuration source identifier */
        IN  int          extraData,  /* maximum amount of data growth */
        IN  int          extraNodes  /* maximum number of nodes growth */
        )
{
    int nodes, data, size;
    cfgHandle *cfg = NULL;
    RvStatus res;

    /* Initialize the common core - this might be the first API function an application
       calls in H.323 */
    res = RvCBaseInit();
    if (res != RV_OK)
        return NULL;

    if(RvMemoryAlloc(NULL, sizeof(cfgHandle), TMP_LOG, (void**)&cfg) != RV_OK)
    {
        RvCBaseEnd();
        return NULL;
    }

    cfg->pool = 0;
    cfg->tree = 0;
    cfg->isMutexConstructed = RV_FALSE;
    cfg->logMgr = TMP_LOG;
    RvLogSourceConstruct(cfg->logMgr, &cfg->log, "CI", "Configuration Interface");

    if (RvMutexConstruct(cfg->logMgr, &cfg->mutex) != RV_OK)
    {
        ciDestruct((HCFG)cfg);
        return NULL;
    }
    cfg->isMutexConstructed = RV_TRUE;

    if (source != NULL)
    {
        ciEstimateCfgSize(cfg, source, &nodes, &data);
        if (nodes < 0  ||  data < 0)
        {
            ciDestruct((HCFG)cfg);
            return NULL;
        }
    }
    else
    {
        data = 0;
        nodes = 0;
    }

    data  += extraData;
    nodes += extraNodes;

    size = (nodes + 1) * 2 * CONFIG_POOL_OVERHEAD + data + 100;
    size = (size + CONFIG_RPOOL_BLOCK_SIZE) / CONFIG_RPOOL_BLOCK_SIZE;   /* set to chunks of 8 bytes */

    cfg->pool = rpoolConstruct(CONFIG_RPOOL_BLOCK_SIZE, size, RV_FALSE, "CONFIG", cfg->logMgr);
    if ((cfg->tree = ciBuildRTree(cfg, source, nodes, cfg->pool)) == NULL)
    {
        ciDestruct((HCFG)cfg);
        return NULL;
    }

    return (HCFG)cfg;
}

RVAPI
ci_errors RVCALLCONV ciDestruct(IN HCFG hCfg)
{
    cfgHandle* cfg = (cfgHandle *)hCfg;

    if (!hCfg)
        return ERR_CI_GENERALERROR;

    if (cfg->tree)  rtDestruct  (cfg->tree);
    if (cfg->pool)  rpoolDestruct(cfg->pool);

    if (cfg->isMutexConstructed)
        RvMutexDestruct(&cfg->mutex, cfg->logMgr);

    RvLogSourceDestruct(&cfg->log);
    RvMemoryFree((void *)hCfg, cfg->logMgr);

    RvCBaseEnd();

    return ERR_CI_NOERROR;

}

RVAPI
ci_errors RVCALLCONV ciSave(
    IN HCFG hCfg,
    IN char *target)
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    ci_errors result;

    if (hCfg == NULL)
        return ERR_CI_GENERALERROR;

    RvMutexLock(&cfg->mutex, cfg->logMgr);
    result = (ci_errors)ciOutputRTree(target, cfg->tree, cfg->pool);
    RvMutexUnlock(&cfg->mutex, cfg->logMgr);

    return result;
}

static int ciGetNodeID(IN HCFG hCfg, IN const char *path)
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    int nodeID;
    int len;
    RvChar* dot;
    pcfgValue cfgVal;

    if (!path || !*path)
        return (int)ERR_CI_GENERALERROR;

    if ((nodeID = rtHead(cfg->tree, rtRoot(cfg->tree))) < 0)
        return (int)ERR_CI_GENERALERROR;

    while (*path)
    {
        dot = (RvChar*)strchr(path, '.');
        len = (dot != NULL) ? (int)(dot - path) : (int)strlen(path);
        cfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);

        while (rpoolChunkSize(cfg->pool, cfgVal->name) != len  ||
               rpoolCompareExternal(cfg->pool, cfgVal->name, (void*)path, len))
        {
            nodeID = rtBrother(cfg->tree, nodeID);
            if (nodeID < 0)
                return (int)ERR_CI_NOTFOUND;
            cfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);
        }

        if (!dot)
            break;

        nodeID = rtHead(cfg->tree, nodeID);
        if (nodeID < 0)
            return (int)ERR_CI_NOTFOUND;

        path = dot + 1;
    }

    return nodeID;
}


RVAPI
ci_errors RVCALLCONV ciGetValue(
        IN  HCFG         hCfg,
        IN  const char * path,      /* full path to nodeID, i.e. "a.b.c" */
        OUT RvBool*      isString,
        OUT RvInt32 *    value      /* data for ints, length for strings */
        )
{
    ci_str_type strtype = ci_str_not;
    ci_errors err;

    /* No need to lock this function... */
    err = ciGetValueExt(hCfg, path, &strtype, value);
    if (isString) *isString = (strtype != ci_str_not);
    return err;
}


RVAPI
ci_errors RVCALLCONV ciGetValueExt(
        IN  HCFG         hCfg,
        IN  const char  *path,      /* full path to nodeID, i.e. "a.b.c" */
        OUT ci_str_type *strType,
        OUT RvInt32 *    value      /* data for ints, length for strings */
        )
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    int bits, nodeID;
    pcfgValue cfgVal;
    ci_str_type dummy;

    if (hCfg == NULL)
        return ERR_CI_GENERALERROR;

    RvMutexLock(&cfg->mutex, cfg->logMgr);
    nodeID = ciGetNodeID(hCfg, (char *)path);

    if (nodeID < 0)
    {
        RvMutexUnlock(&cfg->mutex, cfg->logMgr);
        return (ci_errors)nodeID;
    }

    cfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);

    if (value) *value = cfgVal->value;

    if (!strType) strType = &dummy;

    if (cfgVal->isString)
    {
        char buff[MAX_CONFIG_TEMP_BUFFER_SIZE];
        rpoolCopyToExternal(cfg->pool, (void *)buff, cfgVal->str, 0, MAX_CONFIG_TEMP_BUFFER_SIZE);
        if ((bits = ciIsBitString(buff, cfgVal->value)) >= 0)
        {
            *strType = ci_str_bit;
            if (value) *value = (bits + 7) / 8;
        }
        else
        {
            *strType = ci_str_regular;
        }
    }
    else
        *strType = ci_str_not;

    RvMutexUnlock(&cfg->mutex, cfg->logMgr);
    return ERR_CI_NOERROR;
}


static
ci_errors RVCALLCONV ciGetStringInternal(
                                         IN   HCFG         hCfg,
                                         IN   const char * path,      /* full path to nodeID, i.e. "a.b.c" */
                                         OUT  char *       str,
                                         IN   RvUint32     maxStrLen, /* length of output string buffer */
                                         OUT  RvUint32 *   outbits
                                         )
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    int bits, nodeID = ciGetNodeID(hCfg, (char *)path);
    RvSize_t length;
    pcfgValue cfgVal;
    const char *srcstr;
    char buff[MAX_CONFIG_TEMP_BUFFER_SIZE];

    if (nodeID < 0)
        return (ci_errors)nodeID;

    cfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);

    if (!cfgVal->isString)
        return ERR_CI_GENERALERROR;

    rpoolCopyToExternal(cfg->pool, (void *)buff, cfgVal->str, 0, MAX_CONFIG_TEMP_BUFFER_SIZE);
    if ((bits = ciIsBitString(buff, cfgVal->value)) >= 0)
    {
        length = (RvSize_t)((bits + 7) / 8);
        srcstr = ciGetBitStringData(buff);
    }
    else
    {
        length = (RvSize_t)cfgVal->value;
        srcstr = buff;
    }

    if (maxStrLen < ((RvUint32)length))
        return ERR_CI_BUFFERTOOSMALL;

    memcpy(str, srcstr, length);
    if (maxStrLen > ((RvUint32)length))
        str[(int)length] = '\0';

    if (outbits)
        *outbits = (RvUint32)bits;

    return ERR_CI_NOERROR;
}

RVAPI
ci_errors RVCALLCONV ciGetString(
        IN   HCFG         hCfg,
        IN   const char * path,      /* full path to nodeID, i.e. "a.b.c" */
        OUT  char *       str,
        IN   RvUint32     maxStrLen  /* length of output string buffer */
        )
{
    ci_errors result;
    cfgHandle* cfg = (cfgHandle *)hCfg;

    if (hCfg == NULL)
        return ERR_CI_GENERALERROR;

    RvMutexLock(&cfg->mutex, cfg->logMgr);
    result = ciGetStringInternal(hCfg, path, str, maxStrLen, NULL);
    RvMutexUnlock(&cfg->mutex, cfg->logMgr);
    return result;
}

RVAPI
ci_errors RVCALLCONV ciGetBitString(
        IN  HCFG         hCfg,
        IN  const char * path,       /* full path to node, i.e. "a.b.c" */
        OUT char *       str,
        IN  RvUint32     maxStrLen,  /* length of output string buffer */
        OUT RvUint32 *   bits
        )
{
    ci_errors result;
    cfgHandle* cfg = (cfgHandle *)hCfg;

    if (hCfg == NULL)
        return ERR_CI_GENERALERROR;

    RvMutexLock(&cfg->mutex, cfg->logMgr);
    result = ciGetStringInternal(hCfg, path, str, maxStrLen, bits);
    RvMutexUnlock(&cfg->mutex, cfg->logMgr);
    return result;
}


RVAPI
ci_errors RVCALLCONV ciNext(
        IN   HCFG         hCfg,
        IN   const char * path,       /* full path to nodeID */
        OUT  char       * nextPath,   /* buffer for next path in cfg */
        IN   RvUint32     maxPathLen  /* length of output buffer */
        )
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    int len, firstTime = 1;
    int root;
    int nodeID;
    pcfgValue cfgVal;

    if (hCfg == NULL)
        return ERR_CI_GENERALERROR;

    RvMutexLock(&cfg->mutex, cfg->logMgr);

    root = rtRoot(cfg->tree);
    nodeID = path ? ciGetNodeID(hCfg, path) : root;

    if (root < 0)
    {
        RvMutexUnlock(&cfg->mutex, cfg->logMgr);
        return ERR_CI_GENERALERROR;
    }
    if (nodeID < 0)
    {
        RvMutexUnlock(&cfg->mutex, cfg->logMgr);
        return ERR_CI_NOTFOUND;
    }

    do
    {
        nodeID = rtNext(cfg->tree, root, nodeID);
        if (nodeID < 0)
        {
            RvMutexUnlock(&cfg->mutex, cfg->logMgr);
            return ERR_CI_NOTFOUND;
        }
    } while (rtParent(cfg->tree, nodeID) == root); /* skip first level */

    /* build full path */

    maxPathLen--;

    do
    {
        cfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);

        len = rpoolChunkSize(cfg->pool, cfgVal->name) + 1;
        if ((RvUint32)len > maxPathLen)
        {
            RvMutexUnlock(&cfg->mutex, cfg->logMgr);
            return ERR_CI_BUFFERTOOSMALL;
        }

        if (firstTime)
        {
            rpoolCopyToExternal(cfg->pool, nextPath, cfgVal->name, 0, len);
            nextPath[len - 1] = '\0';
            firstTime = 0;
        }
        else
        {
            memmove(nextPath + len, nextPath, strlen(nextPath) + 1);
            rpoolCopyToExternal(cfg->pool, nextPath, cfgVal->name, 0, len);
            nextPath[len - 1] = '.';
        }

        maxPathLen -= len;
        nodeID = rtParent(cfg->tree, nodeID);
    } while (nodeID != root);

    RvMutexUnlock(&cfg->mutex, cfg->logMgr);
    return ERR_CI_NOERROR;
}


RVAPI
ci_errors RVCALLCONV ciSetValue(
        IN  HCFG         hCfg,
        IN  const char * path,      /* full path to nodeID */
        IN  RvBool       isString,
        IN  RvInt32      value,     /* data for ints, length for strings */
        IN  const char * str        /* null for ints, data for strings */
        )
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    int nodeID, newNodeID;
    int len;
    RvChar* dot;
    cfgValue cfgVal;
    pcfgValue pCfgVal;

    if (hCfg == NULL)
        return ERR_CI_GENERALERROR;

    RvMutexLock(&cfg->mutex, cfg->logMgr);

    nodeID = ciGetNodeID(hCfg, path);

    if (!path  ||  !path[0])
    {
        RvMutexUnlock(&cfg->mutex, cfg->logMgr);
        return ERR_CI_GENERALERROR;
    }

    if (nodeID < 0)  /* no previous value, make a new node */
    {
        /* values for dummy nodes */
        cfgVal.isString  = RV_FALSE;
        cfgVal.value     = 0;
        cfgVal.str       = NULL;

        if ((nodeID = rtHead(cfg->tree, rtRoot(cfg->tree))) < 0)
        {
            /* a special case - an empty configuration */
            dot = (RvChar*)strchr(path, '.');
            len = (dot != NULL) ? (int)(dot - path) : (int)strlen(path);

            cfgVal.name = rpoolAllocCopyExternal(cfg->pool, 0, path, len);

            newNodeID = rtAddTail(cfg->tree, rtRoot(cfg->tree), &cfgVal);
            nodeID = newNodeID;
        }

        while (*path)
        {
            dot = (RvChar*)strchr(path, '.');
            len = (dot != NULL) ? (int)(dot - path) : (int)strlen(path);
            pCfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);
            if (pCfgVal == NULL) break;

            while (rpoolChunkSize(cfg->pool, pCfgVal->name) != len  ||
                   rpoolCompareExternal(cfg->pool, pCfgVal->name, (void*)path, len))
            {
                newNodeID = rtBrother(cfg->tree, nodeID);
                if (newNodeID < 0)
                {
                    cfgVal.name = rpoolAllocCopyExternal(cfg->pool, 0, path, len);

                    newNodeID = rtAddBrother(cfg->tree, nodeID, &cfgVal);
                    nodeID = newNodeID;
                    break;
                }
                nodeID = newNodeID;
                pCfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);
            }

            newNodeID = rtHead(cfg->tree, nodeID);
            path = dot + 1;

            /* if not found, insert a dummy value */
            if (newNodeID < 0)
            {
                if (!dot)
                    break;

                dot = (RvChar*)strchr(path, '.');
                len = (dot != NULL) ? (int)(dot - path) : (int)strlen(path);

                cfgVal.name = rpoolAllocCopyExternal(cfg->pool, 0, path, len);

                newNodeID = rtAddTail(cfg->tree, nodeID, &cfgVal);
            }

            nodeID = newNodeID;

            if (!dot)
                break;
        }
    }

    pCfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);
    if (pCfgVal == NULL)
    {
        RvMutexUnlock(&cfg->mutex, cfg->logMgr);
        return ERR_CI_GENERALERROR;
    }

    /* free, resize, or leave alone the old string buffer */
    if (pCfgVal->isString)
    {
        if (!isString)
        {
            rpoolFree(cfg->pool, pCfgVal->str);
            pCfgVal->str = NULL;
        }
        else
        {
            if (pCfgVal->value != value)
            {
                rpoolFree(cfg->pool, pCfgVal->str);
                pCfgVal->str = rpoolAlloc(cfg->pool, value);
            }
        }
    }
    else
    {
        if (isString)
        {
            pCfgVal->str = rpoolAlloc(cfg->pool, value);
        }
    }

    pCfgVal->isString  = isString;
    pCfgVal->value     = value;

    if (isString)
        rpoolCopyFromExternal(cfg->pool, pCfgVal->str, (void *)str, 0, value);

    RvMutexUnlock(&cfg->mutex, cfg->logMgr);
    return ERR_CI_NOERROR;
}


RVAPI
ci_errors RVCALLCONV ciSetBitString(
        IN  HCFG         hCfg,
        IN  const char * path,       /* full path to node */
        IN  RvInt32      bits,       /* number of bits in the string */
        IN  const char * str         /* null for ints, data for strings */
        )
{
    char buf[1024];
    return
        ciSetValue(hCfg, path, RV_TRUE, ciBuildBitString(str, bits, buf), buf);
}


RVAPI
ci_errors RVCALLCONV ciDeleteValue(
        IN  HCFG         hCfg,
        IN  const char * path
        )
{
    cfgHandle* cfg = (cfgHandle *)hCfg;
    ci_errors result = ERR_CI_NOERROR;
    int nodeID;
    pcfgValue pCfgVal;

    if (hCfg == NULL)
        return ERR_CI_GENERALERROR;

    RvMutexLock(&cfg->mutex, cfg->logMgr);

    nodeID = ciGetNodeID(hCfg, path);

    if (nodeID >= 0)
    {
        pCfgVal = (pcfgValue)rtGetByPath(cfg->tree, nodeID);
        if (pCfgVal && pCfgVal->str)
            rpoolFree(cfg->pool, pCfgVal->str);
        if (pCfgVal&& pCfgVal->name)
            rpoolFree(cfg->pool, pCfgVal->name);

        if (rtDelete(cfg->tree, nodeID, NULL, 0) < 0)
            result = ERR_CI_GENERALERROR;
    }
    else
        result = ERR_CI_NOTFOUND;

    RvMutexUnlock(&cfg->mutex, cfg->logMgr);
    return result;
}


#ifdef __cplusplus
}
#endif




