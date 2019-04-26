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

  ciVirtFile.c  --  Configuration load/save functions - virtual file version

  Abstract:      CI routines for loading and saving the configuration to
                 and from virtual files.

***************************************************************************/

#include "rvstdio.h"
#include <stdlib.h>

#include "rvmemory.h"
#include "rvaddress.h"
#include "rvassert.h"
#include "rtree.h"
#include "rpool.h"
#include "oidutils.h"
#include "strutils.h"

#include "ci.h"
#include "cisupp.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Buffer size used to read lines from a configuration file */
#define FILE_BUF_SIZE (2200)

/* Maximum tree depth for a configuration file */
#define CI_MAX_DEPTH (50)


#define CI_VIRTFILE_ID_STRING   "**VF**"    /* with an appended NULL character */
#define CI_VIRTFILE_ID_LEN      7


/* Virtual file source struct. This one is given to ciCosntruct() as a string. */
typedef struct
{
    RvChar                  header[CI_VIRTFILE_ID_LEN]; /* Indication that this source is a virtual file */
    void*                   context; /* Context to use for the functions */
    ciVirtualFileOpenEv     openFunc; /* fopen() replacement */
    ciVirtualFileCloseEv    closeFunc; /* fclose() replacement */
    ciVirtualFileReadLineEv readLineEv; /* fgets() replacement */

} ciVirtFile;




static void buildFromVirtualFile(
    IN cfgHandle    *cfg,
    IN ciVirtFile  *inF,
    IN HRPOOL       pool,
    IN HRTREE       tree);

static int parseLine(IN int currLevel, IN char *line,
                     OUT char **name, OUT char **rawValue);


/******************************************************************************
 * ciCreateVirtualFileSource
 * ----------------------------------------------------------------------------
 * General: Create a configuration source for a virtual file.
 *          This kind of a configuration source can be used when you have a
 *          configuration file that is stored in something other than a regular
 *          file system. It actually allows reading these files from memory
 *          buffers, flash or over the network.
 *          The configuration source itself is only needed for the ciConstruct()
 *          function, and after calling ciConstruct(), or your Stack's
 *          initialization function, you can free any resources used by your
 *          virtual file.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  sourceResult - Source result string. This buffer is given by the
 *                        user and filled by this function with the relevant
 *                        information. The size of this string must be
 *                        at least CI_VIRTUAL_FILE_SOURCE_SIZE.
 *         context      - Context to use on calls to the virtual file's
 *                        functions.
 *         openFunc     - Open virtual file function. Called when CI wants
 *                        to open the virtual file. Might be called more than
 *                        once during ciConstruct(), but only on a "closed"
 *                        file.
 *         closeFunc    - Close virtual file function. Called when CI wants
 *                        to close the virtual file. Might be called more than
 *                        once during ciConstruct().
 *         readLineEv   - Request to read a line from the virtual file. Works
 *                        similar to the fgets() function.
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV ciCreateVirtualFileSource(
    IN RvChar*                  sourceResult,
    IN void*                    context,
    IN ciVirtualFileOpenEv      openFunc,
    IN ciVirtualFileCloseEv     closeFunc,
    IN ciVirtualFileReadLineEv  readLineEv)
{
    ciVirtFile *virtF = (ciVirtFile *)sourceResult;

    /* Sanity check, to make sure the code is fine */
    RvAssert(CI_VIRTUAL_FILE_SOURCE_SIZE >= sizeof(ciVirtFile));

    memcpy(virtF->header, CI_VIRTFILE_ID_STRING, CI_VIRTFILE_ID_LEN);
    virtF->context = context;
    virtF->openFunc = openFunc;
    virtF->closeFunc = closeFunc;
    virtF->readLineEv = readLineEv;

    return RV_OK;
}



int ciIDVirtualFile(const char *source)
{
    return (!(memcmp(source, CI_VIRTFILE_ID_STRING, CI_VIRTFILE_ID_LEN)));
}


int ciEstimateCfgSizeVirtualFile(cfgHandle *cfg, const char *source, int *nodes, int *data)
{
    ciVirtFile *inF = (ciVirtFile *)source;
    RvChar fileBuffer[FILE_BUF_SIZE];
    char *name, *value;

    RV_UNUSED_ARG(cfg);

    /* Open the virtual file */
    if (inF->openFunc(inF->context) != RV_OK)
        return (int)ERR_CI_FILEOPEN;

    *nodes = 0;
    *data  = 0;

    while (inF->readLineEv(inF->context, fileBuffer, FILE_BUF_SIZE) == RV_OK)
    {
        parseLine(0, fileBuffer, &name, &value);
        if (name)
        {
            int len = strlen(name);
            (*nodes)++;
            (*data) += (len + CONFIG_RPOOL_BLOCK_SIZE - (len % CONFIG_RPOOL_BLOCK_SIZE));
        }
        if (value)
        {
            int len = strlen(value);
            (*data) += (len + CONFIG_RPOOL_BLOCK_SIZE - (len % CONFIG_RPOOL_BLOCK_SIZE));
        }
    }

    /* Close the file - we're done finding the size we need to allocate */
    inF->closeFunc(inF->context);

    return 0;
}

int ciBuildFromVirtualFile(cfgHandle *cfg, const char *source, HRTREE tree, HRPOOL pool)
{
    ciVirtFile *inF = (ciVirtFile *)source;

    RV_UNUSED_ARG(cfg);

    /* Open the virtual file */
    if (inF->openFunc(inF->context) != RV_OK)
        return (int)ERR_CI_FILEOPEN;

    buildFromVirtualFile(cfg, inF, pool, tree);

    /* Close the file - we're done with it */
    inF->closeFunc(inF->context);

    return 0;
}


int ciOutputToVirtualFile(const char *target, HRTREE tree, HRPOOL pool)
{
    RV_UNUSED_ARG(target);
    RV_UNUSED_ARG(tree);
    RV_UNUSED_ARG(pool);

    /* Not supported for virtual files... */
    return (int)ERR_CI_INTERFACEUNKNOWN;
}


static char *skipWhite(char *s)
{
  if (s)
    for (; *s  &&  isspace((int)*s); s++);

  return s;
}

static char *findMatchingQuote(char *s, char quote)
{
  /* stops only at '\n' */
  for (; *s != '\n'  &&  *s != quote; s++);

  return (*s == '\n') ? (char*)NULL : s;
}

/* parseLine() returns the new level, or -1 on parse error

   line format:
   [level] name [= value]
   lines beginning with # are a comment.

   level:  a number (the level number), '+' (advance to next level),
           +Number (advance Number levels), ditto for '-'.
           when no level is specified, the same level is intended
*/
static int parseLine(IN int currLevel, IN char *line,
                     OUT char **name, OUT char **rawValue)
{
    static char name_buf[256];
    static char val_buf[2200];

    int newLevel, i;
    char sign, *p;

    *name     = NULL;
    *rawValue = NULL;

    line = skipWhite(line);
    if (!line[0]  ||  line[0] == '#')  /* comment or end of line */
    {
        return -1;
    }

    /* first section (optional): the level */
    if (isdigit((int)line[0]))
    {
        newLevel = 0;
        while (line[0]  &&  isdigit((int)line[0]))
        {
            newLevel = (newLevel * 10) + (line[0] - '0');
            line++;
        }
    }
    else
    {
        /* '+' or '-' */
        newLevel = currLevel;
        sign = line[0];
        if (sign == '+'  ||  sign == '-')
        {
            line++;
            if (!line[0])
            {
                return -1;
            }
            if (isdigit((int)line[0]))
            {
                i = 0;
                while (line[0]  && isdigit((int)line[0]))
                {
                    i = (i * 10) + (line[0] - '0');
                    line++;
                }
            }
            else
            {
                i = 1;
            }

            newLevel = newLevel + ((sign == '+')? i : -i);
        }
    }

    line = skipWhite(line);
    if (!line[0]  ||  line[0] == '#')  /* comment or end of line */
    {
        return -1;
    }

    /* second section: the name */
    p = (char *)name_buf;
    while (line[0]  &&  !isspace((int)line[0])  &&  line[0] != '=')
    {
        *p = *line;
        line++;
        p++;
    }
    *p = '\0';
    *name = (char *)name_buf;
    /* simulate a value of zero in case there is no value */
    strcpy((char *)val_buf, "0");
    *rawValue = (char *)val_buf;

    line = skipWhite(line);
    if (!line[0]  ||  line[0] == '#'  ||  line[0] != '=')
    {
        return newLevel;
    }
    ++line;
    line = skipWhite(line);
    if (!line[0]  ||  line[0] == '#')
    {
        return newLevel;
    }

    /* third section: the value */
    p = (char *)val_buf;
    while (line[0] && (p < val_buf+sizeof(val_buf)-1))
    {
        *p = *line;
        line++;
        p++;
    }
    *p = '\0';

    return newLevel;
}




/* this macro scans the buffer for the matching quote character, then
   updates the following variables:
     p              - beginning of string between quotes
     p2             - end of string (points to the asciiz)
     length         - length of string
     cfgVal->string - allocated variable string
   the macro will break from the loop if the string is null
*/
#define MATCH_QUOTE(quote_char)                                             \
  p2 = findMatchingQuote(p + 1, quote_char);                                \
  if (!p2  ||  ((p2 - p) < 1))                                              \
    break;                                                                  \
  p++;                                                                      \
  *p2 = '\0';                                                               \
  length = (unsigned int)(p2 - p);                                          \
  assistLen = length;                                                       \
  assistBuff[length] = '\0';


/* Check that a parent doesn't have a node with a given name.
   If it does - return that node, so we can use it instead of adding a new one
   with the same name */
static int getSameNode(IN HRTREE tree, IN int parent, IN HRPOOL pool, IN const void* nodeName)
{
    int         nodeId;
    int         len;
    pcfgValue   cfgVal;

    nodeId = rtHead(tree, parent);
    if (nodeId < 0)
        return nodeId;

    len = rpoolChunkSize(pool, (void *)nodeName);
    cfgVal = (pcfgValue)rtGetByPath(tree, nodeId);

    while (rpoolChunkSize(pool, cfgVal->name) != len  ||
           rpoolCompareInternal(pool, cfgVal->name, (void *)nodeName, len))
    {
        nodeId = rtBrother(tree, nodeId);
        if (nodeId < 0)
            return RV_ERROR_UNKNOWN;
        cfgVal = (pcfgValue)rtGetByPath(tree, nodeId);
    }

    return nodeId;
}


/******************************************************************************
 * buildFromVirtualFile
 * ----------------------------------------------------------------------------
 * General: Build the configuration tree from a virtual file.
 *          This function does the actual work.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  cfg      - Configuration handle used (needed for printing errors).
 *         inF      - Input virtual file we're reading.
 *         pool     - Strings RPOOL to use for names and values.
 *         tree     - RTREE to use for the configuration nodes.
 * Output: address  - Resulting transport address struct.
 *****************************************************************************/
static void buildFromVirtualFile(
    IN cfgHandle    *cfg,
    IN ciVirtFile   *inF,
    IN HRPOOL       pool,
    IN HRTREE       tree)
{
    /* Node list that allows this function to work without being recursive */
    int nodeList[CI_MAX_DEPTH];
    int nameCounter[CI_MAX_DEPTH];
    int currentLevel; /* Current level we're dealing with */

    int readLevel, newNodeID;
    RvUint32 length;
    cfgValue cfgVal;
    RvChar *p, *p2, *value=NULL, *name=NULL;
    RvChar assistBuff[2*FILE_BUF_SIZE];
    RvChar fileBuffer[FILE_BUF_SIZE];
    int assistLen;

    RV_UNUSED_ARG(cfg);

    memset(nameCounter, 0, sizeof(nameCounter));
    nodeList[0] = rtRoot(tree);
    currentLevel = 0;

    /* Run this loop until we're done or until we get an error */
    while (inF->readLineEv(inF->context, fileBuffer, sizeof(fileBuffer)) == RV_OK)
    {
        /* Parse the line we just read */
        readLevel = parseLine(currentLevel, fileBuffer, &name, &value);

        if (readLevel < 0)
            continue; /* an error or a comment - ignore this line */

        if (readLevel >= CI_MAX_DEPTH)
        {
            /* We're out of bounds - reached a depth that is too low in the configuration tree */
            RvLogWarning(&cfg->log, (&cfg->log, "Tree too deep to work with"));
            currentLevel = readLevel;
            continue; /* ignore and continue reading the configuration file */
        }
        else if (readLevel < currentLevel)
        {
            /* A higher level than we have now - wrap up to that level */
            currentLevel = readLevel;
        }
        else if (readLevel> currentLevel + 1)
        {
            /* This level is way too down the tree - we can't resolve it properly */
            RvLogWarning(&cfg->log, (&cfg->log,
                "%s=%s is too low in tree", name, value));
            continue;
        }

        /* Resolve '*' names if necessary */
        if (strcmp(name, "*") == 0)
        {
            /* We're dealing with an "array" */
            if (readLevel != currentLevel)
            {
                /* First time here - make sure we actually treat it as a first time */
                RvSprintf(name, "%d", 1);
                nameCounter[readLevel] = 1;
            }
            else
            {
                /* Been here before - let's calculate the actual index number */
                nameCounter[readLevel]++;
                RvSprintf(name, "%d", nameCounter[readLevel]);
            }
        }
        else if (atoi(name) != 0)
        {
            /* Got a number as a name - make sure we modify the counter accordingly */
            nameCounter[readLevel] = atoi(name);
        }

        /* Make sure we allocate a value for this new kid */
        cfgVal.name = rpoolAllocCopyExternal(pool, 0, name, (int)strlen(name));
        cfgVal.isString = RV_TRUE; /* Assume it's a string value */
        cfgVal.value = 0;
        cfgVal.str = NULL;

        /* Parse the value to the configuration value we just generated */
        p = value;

        switch (*p)
        {
            case '\'':   /* string */
                MATCH_QUOTE('\'');
                cfgVal.str = rpoolAllocCopyExternal(pool, 0, (void*)p, assistLen);
                cfgVal.value = assistLen;
                break;

            case '[':    /* hex string */
            {
                RvBool bIsHighByte;
                char c;
                RvUint8 i;

                MATCH_QUOTE(']');
                p2 = assistBuff;
                cfgVal.value = (assistLen + 1) >> 1;
                memset(assistBuff, 0, (RvSize_t)cfgVal.value);
                bIsHighByte = !(length & 1);
                for (; length; length--, p++)
                {
                    c = (char)toupper((int)*p);
                    i = (RvUint8)((c > '9')? (c - 'A' + (char)10) : (c - '0'));
                    if (bIsHighByte)
                    {
                        *p2 = (char)(i << (char)4);
                    }
                    else
                    {
                        *p2 = (char)(*p2 + i);
                        p2++;
                    }
                    bIsHighByte = !bIsHighByte;
                }
                cfgVal.str = rpoolAllocCopyExternal(pool, 0, (void*)assistBuff, cfgVal.value);
                break;
            }

            case '"':   /* BMP string */
                p2 = findMatchingQuote(p + 1, '"');
                if (!p2  ||  ((p2 - p) < 1))
                    break;
                p++;
                *p2 = '\0';
                length = (RvUint32)(p2 - p);
                cfgVal.value = length * 2;
                {
                    chrn2bmp(p, (RvSize_t)length, (RvUint8*)assistBuff);
                    cfgVal.str = rpoolAllocCopyExternal(pool, 0, (void *)assistBuff, cfgVal.value);
                }
                break;

            case '%':    /* bit string */
            {
                char  buf[1024];
                char  bitBuf[1152];
                int   i, val;

                p2 = findMatchingQuote(p + 1, '%');
                if (!p2  ||  ((p2 - p) < 1))
                    break;
                p++;
                *p2 = '\0';

                /* str -> bits */
                p2 = buf;
                val = 0;
                i = length = 0;
                while (*p)
                {
                    if (*p != '1'  &&  *p != '0')
                        goto bitstr_out;
                    val += (int)(((RvUint)(RvInt)(*p - '0')) << (7 - i));
                    if (++i > 7)
                    {
                        *p2 = (char)val;
                        i = val = 0;
                        p2++;
                    }
                    p++;
                    length++;  /* count bits */
                }

                if (i)
                    *p2 = (char)val;

                cfgVal.value = ciBuildBitString(buf, (int)length, bitBuf);
                cfgVal.str   = rpoolAllocCopyExternal(pool, 0, (void *)bitBuf, cfgVal.value);
bitstr_out:
                break;
            }

            case '{':    /* object ID */
                MATCH_QUOTE('}');
                /* the target string should be big enough */
                cfgVal.value = oidEncodeOID((int)length, assistBuff, p);
                cfgVal.str   = rpoolAllocCopyExternal(pool, 0, (void *)assistBuff, cfgVal.value);
                break;
#if (RV_NET_TYPE != RV_NET_NONE)
            case '<':    /* IP */
            {
                RvAddress addr;
                const RvAddressIpv4* ipv4Addr;
                RvUint32 ipV4 = RV_UINT32_MAX;

                MATCH_QUOTE('>');

                RvAddressConstruct(RV_ADDRESS_TYPE_IPV4, &addr);
                if (RvAddressSetString((RvChar *)p, &addr) != NULL)
                {
                    ipv4Addr = RvAddressGetIpv4(&addr);
                    if (ipv4Addr != NULL)
                        ipV4 = RvAddressIpv4GetIp(ipv4Addr);
                    RvAddressDestruct(&addr);
                }

                cfgVal.value = 4;
                cfgVal.str = rpoolAllocCopyExternal(pool, 0, (void*)&ipV4, cfgVal.value);
                break;
            }
#endif /* (RV_NET_TYPE != RV_NET_NONE) */

            default:
                cfgVal.isString = RV_FALSE;
                cfgVal.value    = atoi(p);
        }

        /* Now that we're done parsing, let's see where in the tree are we going to shove this
           node */
        if (readLevel == currentLevel)
        {
            /* We're handling another field in the same level as the previous one */

            /* See if it's a new name or a name replacing an older value */
            newNodeID = getSameNode(tree, nodeList[currentLevel-1], pool, cfgVal.name);
            if (newNodeID <= 0)
            {
                /* Such a node doesn't exist - add it in */
                newNodeID = rtAddBrother(tree, nodeList[currentLevel], &cfgVal);
                nodeList[currentLevel] = newNodeID;
            }
            else
            {
                /* Already have such a node - ignore this new one */
                if (cfgVal.name != NULL)
                    rpoolFree(pool, cfgVal.name);
                if (cfgVal.str != NULL)
                    rpoolFree(pool, cfgVal.str);
            }
        }
        else
        {
            /* Node in a new level from it's parent - add it in */
            newNodeID = rtAddTail(tree, nodeList[currentLevel], &cfgVal);

            /* Get on to the next level */
            nodeList[readLevel] = newNodeID;
            currentLevel = readLevel;
        }
    }
}


#ifdef __cplusplus
}
#endif

