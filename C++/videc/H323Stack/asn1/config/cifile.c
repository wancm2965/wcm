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

  ciFile.c  --  Configuration load/save functions - file version

  Abstract:      CI routines for loading and saving the configuration to
                 and from files.

***************************************************************************/

#ifndef NOFILESYSTEM

#include "rvstdio.h"
#include <stdlib.h>
#include <string.h>
#include "rvmemory.h"
#include "rvaddress.h"
#include "rtree.h"
#include "rpool.h"
#include "strutils.h"

#include "ci.h"
#include "cisupp.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    const RvChar*   filename; /* Filename given as source */
    RvFILE*         f; /* OS File pointer */
} ciFileData;


/* Declared here, although working */
int ciEstimateCfgSizeVirtualFile(cfgHandle *cfg, const char *source, int *nodes, int *data);
int ciBuildFromVirtualFile(cfgHandle *cfg, const char *source, HRTREE tree, HRPOOL pool);




static void outputToFile(RvFILE *outf, HRTREE tree, int nodeID, int level, HRPOOL pool);





int ciIDFile(const char *source)
{
  return (source  &&  source[0]);
}


static void ciGetModuleDirectoryFileName(const char* name, char* szPath, int szPathSize)
{
    strncpy(szPath, name, (RvSize_t)szPathSize);
#if (RV_OS_TYPE == RV_OS_TYPE_WIN32)
    {
        /* If no path is given with the filename, we try to get the filename by the
           exact path of the binary file we're executing now */
        RvChar* p;
        p = (RvChar*) strchr(name, '\\');
        if (p == NULL)
        {
            /* Make sure we check both slash and backslash */
            p = (RvChar*) strchr(name, '/');
        }

        if (!p)
        {
            int len;

            len = (int)(GetModuleFileName(GetModuleHandle(NULL), szPath, (DWORD)szPathSize));
            p = strrchr(szPath, '\\');
            if (p)
            {
                p++;
                strncpy(p, name, (RvSize_t)(szPathSize - len));
            }
        }
    }
#endif
}


/******************************************************************************
 * ciFileOpenEv
 * ----------------------------------------------------------------------------
 * General: Function for opening a file through the virtual file interface.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  context      - ciFileData struct.
 * Output: None.
 *****************************************************************************/
static RvStatus RVCALLCONV ciFileOpenEv(
    IN void* context)
{
    ciFileData* fileData = (ciFileData *)context;
    char sourceWithPath[300];

    ciGetModuleDirectoryFileName(fileData->filename, sourceWithPath, sizeof(sourceWithPath));
    
    fileData->f = RvFopen(sourceWithPath, "rb");
    if (fileData->f == NULL)
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}


/******************************************************************************
 * ciFileCloseEv
 * ----------------------------------------------------------------------------
 * General: Function for closing a file through the virtual file interface.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  context      - ciFileData struct.
 * Output: None.
 *****************************************************************************/
static RvStatus RVCALLCONV ciFileCloseEv(
    IN void* context)
{
#if (RV_OS_TYPE != RV_OS_TYPE_NUCLEUS)
    ciFileData* fileData = (ciFileData *)context;
#else
	RV_UNUSED_ARG(context);
#endif
    (void)RvFclose(fileData->f);
    return RV_OK;
}


/******************************************************************************
 * ciFileReadLineEv
 * ----------------------------------------------------------------------------
 * General: Function for reading a line from a file through the virtual file
 *          interface.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure or if we reached the end of the virtual file.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  context      - ciFileData struct.
 *         buffer       - Buffer to fill in with the read line. This line must
 *                        be filled with a NULL terminated string.
 *         bufferSize   - Size of the buffer to fill.
 * Output: None.
 *****************************************************************************/
static RvStatus RVCALLCONV ciFileReadLineEv(
    IN void*    context,
    IN RvChar*  buffer,
    IN RvSize_t bufferSize)
{
    ciFileData* fileData = (ciFileData *)context;

    if (RvFgets(buffer, (RvInt)bufferSize, fileData->f) == NULL)
        return RV_ERROR_UNKNOWN;

    return RV_OK;
}



int ciEstimateCfgSizeFile(cfgHandle *cfg, const char *source, int *nodes, int *data)
{
    ciFileData fileData;
    RvChar fileSource[CI_VIRTUAL_FILE_SOURCE_SIZE];

    fileData.filename = source;
    fileData.f = NULL;
    ciCreateVirtualFileSource(fileSource, &fileData, ciFileOpenEv, ciFileCloseEv, ciFileReadLineEv);

    return ciEstimateCfgSizeVirtualFile(cfg, fileSource, nodes, data);
}


int ciBuildFromFile(cfgHandle *cfg, const char *source, HRTREE tree, HRPOOL pool)
{
    ciFileData fileData;
    RvChar fileSource[CI_VIRTUAL_FILE_SOURCE_SIZE];

    fileData.filename = source;
    fileData.f = NULL;
    ciCreateVirtualFileSource(fileSource, &fileData, ciFileOpenEv, ciFileCloseEv, ciFileReadLineEv);

    return ciBuildFromVirtualFile(cfg, fileSource, tree, pool);
}


int ciOutputToFile(const char *target, HRTREE tree, HRPOOL pool)
{
  RvFILE *outf;
  char targetWithPath[300];
  ciGetModuleDirectoryFileName(target, targetWithPath,
                               sizeof(targetWithPath));

  if ( (outf = RvFopen(targetWithPath, "wb")) == NULL)
    return (int)ERR_CI_FILEOPEN;

  /* output standard header */
  RvFprintf(outf,
    "# RADVISION H.323/3G-324M Stack Configuration File\n"
    "#\n"
    "# Value encodings:\n"
    "#    ''   -  String (and asciiz is not appended)\n"
    "#    \"\"   -  BMP string of ASCII characters\n"
    "#    []   -  Hex octet string\n"
    "#    <>   -  IP\n"
    "#    {}   -  Object ID\n"
    "#    %%   -  Bit string\n"
    "#   Other -  Integer\n"
    "\n");

  outputToFile(outf, tree, rtRoot(tree), 0, pool);
  (void)RvFclose(outf);

  return 0;
}






#define IS_PRINTABLE(c) ((c) >= ' '  &&  (c) < 127)

static void outputToFile(RvFILE *outf, HRTREE tree, int nodeID, int level, HRPOOL pool)
{
    pcfgValue cfgVal;
    int child, i, printable, isBMP;
    static int lastOutputLevel;
    char buff[MAX_CONFIG_TEMP_BUFFER_SIZE];

    if (level == 0)
        lastOutputLevel = 0;

    for (;;)
    {
        if (level != 0)
        {
            cfgVal = (pcfgValue)rtGetByPath(tree, nodeID);

            /* make the file legal by adding some extra blank lines */
            if (level == 1)
                RvFprintf(outf, "\n\n");
            else
                if (lastOutputLevel > level + 2)
                    RvPutc('\n', outf);


            /* output the level */
            if (level == 1)  /* always print number on first level */
                RvPutc('1', outf);
            else
                if (level == lastOutputLevel)
                    RvPutc(' ', outf);
                else
                    if (level == lastOutputLevel + 1)
                        RvPutc('+', outf);
                    else
                        if (level == lastOutputLevel - 1)
                            RvPutc('-', outf);
                        else
                            RvFprintf(outf, "%d", level);

            /* output the name, padded with spaces on the left */
            rpoolCopyToExternal(pool, (void*)buff, cfgVal->name, 0, MAX_CONFIG_TEMP_BUFFER_SIZE);
            buff[rpoolChunkSize(pool, cfgVal->name)] = 0;
            RvFprintf(outf, "%*c%s = ", level, ' ',
                ((level > 1) && atoi(buff)) ? "*" : buff);

            if (cfgVal->isString)
            {
                rpoolCopyToExternal(pool, (void*)buff, cfgVal->str, 0, MAX_CONFIG_TEMP_BUFFER_SIZE);

                /* see if we can print it as a BMP string */

                isBMP = 1; /* assume so */

                if (cfgVal->value % 2 == 0) /* even length */
                {
                    for (i = 0; i < cfgVal->value; i += 2)
                    {
                        if (buff[i] != '\0' ||
                            !IS_PRINTABLE(buff[i + 1]))
                        {
                            isBMP = 0;
                            break;
                        }
                    }
                }
                else
                    isBMP = 0;

                if (isBMP)
                {
                    RvFputc('"', outf);
                    for (i = 0; i < cfgVal->value; i += 2)
                    {
                        int iChar = buff[i + 1];
                        RvFputc(iChar, outf);
                    }
                    RvFprintf(outf, "\"\n");
                }
                else  /* non BMP strings */
                {
                    /* see if string is printable */
                    printable = 1;

                    for (i = 0; i < cfgVal->value; i++)
                    {
                        if (!IS_PRINTABLE(buff[i]))
                        {
                            printable = 0;
                        }
                    }

                    if (printable)
                    {
                        RvFputc('\'', outf);
                        RvFwrite(buff, (RvSize_t)cfgVal->value, 1, outf);
                        RvFputc('\'', outf);
                        RvFputc('\n', outf);
                    }
                    else
                    {
                        /* output octet string */
                        RvFputc('[', outf);
                        for (i = 0; i < cfgVal->value; i++)
                            RvFprintf(outf, "%02x",
                                    (unsigned char)buff[i]);
                        RvFprintf(outf, "]\n");
                    }
                }
            }
            else  /* an int */
                RvFprintf(outf, "%d\n", cfgVal->value);
        }

        lastOutputLevel = level;

        /* process children */
        child = rtHead(tree, nodeID);
        if (child >= 0)
            outputToFile(outf, tree, child, level + 1, pool);

        /* move to brother */
        nodeID = rtBrother(tree, nodeID);
        if (nodeID < 0)
            return;
    }
}


#ifdef __cplusplus
}
#endif

#endif /* NOFILESYSTEM */



