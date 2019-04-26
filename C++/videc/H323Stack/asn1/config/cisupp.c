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

  cisupp.c  --  CI helper functions interface

  Abstract:      Builds the configuration tree from a configuration source.

  Platforms:     File and buffer interfaces are supported for all platforms.
                 Registry support is added for WIN32.
                 See indevidual interface source files for more detailed
                 platform notes.

***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#include "rvstdio.h"
#include "rtree.h"
#include "rpool.h"

#include "ci.h"
#include "cisupp.h"

typedef struct
{
  ciIDFunc       *idFunc;
  ciEstimateFunc *estimateFunc;
  ciBuildFunc    *buildFunc;
  ciOutputFunc   *outputFunc;
} interfaceFuncs;


                      /* == Interface Definitions == */

#undef RV_CI_DECLARE_INTERFACE

#define RV_CI_DECLARE_INTERFACE(n)                                                          \
    int ciID##n             (const char *source);                                           \
    int ciEstimateCfgSize##n(cfgHandle *cfg, const char *source, int *nodes, int *data);    \
    int ciBuildFrom##n      (cfgHandle *cfg, const char *source, HRTREE tree, HRPOOL pool); \
    int ciOutputTo##n       (const char *source, HRTREE tree, HRPOOL pool);

#define RV_CI_DEFINE_INTERFACE(name)                                       \
    { ciID##name, ciEstimateCfgSize##name,                                 \
      ciBuildFrom##name, ciOutputTo##name }                                \

/* declare external functions */
#if (RV_OS_TYPE == RV_OS_TYPE_WIN32)
RV_CI_DECLARE_INTERFACE(Registry)
#endif
RV_CI_DECLARE_INTERFACE(Buffer)
RV_CI_DECLARE_INTERFACE(VirtualFile)
RV_CI_DECLARE_INTERFACE(File)

/* cisupp will try to use the interfaces in the order they appear here: */
interfaceFuncs interfaces[] =
{
#if (RV_OS_TYPE == RV_OS_TYPE_WIN32)
    /* Registry interface */    RV_CI_DEFINE_INTERFACE(Registry),
#endif
  /* Buffer interface   */      RV_CI_DEFINE_INTERFACE(Buffer),
  /* Virtual File interface */  RV_CI_DEFINE_INTERFACE(VirtualFile),
#ifndef NOFILESYSTEM
  /* File interface     */      RV_CI_DEFINE_INTERFACE(File)
#endif
};


static int getInterface(const char *source)
{
    int i;

    for (i = 0; i < (int)(sizeof(interfaces) / sizeof(interfaceFuncs)); i++)
    {
        if (interfaces[i].idFunc(source))
            return i;
    }

    return (int)ERR_CI_INTERFACEUNKNOWN;
}


void
ciEstimateCfgSize(
                  cfgHandle *cfg,
                  const char *source,  /* configuration source identifier */
                  int *nodes,          /* number of nodes */
                  int *data            /* size of all data */
                  )
{
    int iface;

    *nodes = (int)ERR_CI_GENERALERROR;
    *data  = (int)ERR_CI_GENERALERROR;

    if ((iface = getInterface(source)) < 0) return;

    interfaces[iface].estimateFunc(cfg, source, nodes, data);
    return;
}


HRTREE ciBuildRTree(cfgHandle *cfg, const char *source, int nodes, HRPOOL pool)
{
    HRTREE tree;
    int iface;

    tree = rtConstruct(sizeof(cfgValue), nodes + 1, "CI tree", cfg->logMgr);
    rtAddRoot(tree, NULL);  /* a root for all configurations */

    if (source != NULL)
    {
        if ((iface = getInterface(source)) < 0)
            return NULL;

        if (interfaces[iface].buildFunc(cfg, source, tree, pool) <0)
            return NULL;
    }

    return tree;
}


int ciOutputRTree(const char *target, HRTREE tree, HRPOOL pool)
{
    int iface;

    if ((iface = getInterface(target)) < 0)
        return iface;

    return interfaces[iface].outputFunc(target, tree, pool);
}


/* == bit string support */

int
ciBuildBitString(const char *str, int bits, OUT char *bitstr)
{
    int len = (bits + 7) / 8;

    if (bitstr)
    {
        memcpy(bitstr, CI_BITSTR_ID, CI_BITSTR_ID_LEN);
        bitstr[CI_BITSTR_ID_LEN + 0] = (char)((bits & 0xFF00) >> 8);
        bitstr[CI_BITSTR_ID_LEN + 1] = (char)(bits & 0xFF);
        memcpy(bitstr + CI_BITSTR_ID_LEN + 2, str, (RvSize_t)len);
    }

    return len + CI_BITSTR_ID_LEN + 2;
}

int
ciIsBitString(const char *str, int length)
{
    if (length < (CI_BITSTR_ID_LEN + 2))
        return -1;

    if (memcmp(str, CI_BITSTR_ID, CI_BITSTR_ID_LEN) != 0)
        return -1;

    return (((int)str[CI_BITSTR_ID_LEN]) << 8) + str[CI_BITSTR_ID_LEN + 1];
}

/* returns the data section inside the bit string buffer */
const char *ciGetBitStringData(const char *str)
{
    return str + CI_BITSTR_ID_LEN + 2;
}



#ifdef __cplusplus
}
#endif



