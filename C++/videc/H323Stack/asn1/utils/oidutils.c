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

#include "rvstdio.h"
#include "rvtypes.h"
#include "rverror.h"
#include "oidutils.h"
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif


static const RvChar* node[] = {
    "itu-t", "iso", "joint-iso-itu-t", "ccitt", "joint-iso-ccitt", NULL};
static const RvChar* node0[] = {
    "recommendation", "question", "administration",
    "network-operator", "identified-organization", NULL};
static const RvChar* node1[] = {
    "standard", "1", "member-body", "identified-organization", NULL};
static const RvChar* series[] = {
    "",  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
    "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w",
    "x", "y", "z", NULL};

static const RvChar** oidNodes[] = {node0, node1, NULL};
static const int sizeNodes[] = {(int)(sizeof(node0) / sizeof(char*)), (int)(sizeof(node1) / sizeof(char*)), -1};



static int findName(IN const RvChar** names, IN const RvChar* name, IN RvSize_t nameSize)
{
    RvSize_t curNameLen;
    int i=0;
    while(names[i])
    {
        curNameLen = strlen(names[i]);
        if ((nameSize == curNameLen) && !strncmp(names[i], name, nameSize))
            return i;
    i++;
    }
    return RV_ERROR_UNKNOWN;
}


static void eatWhiteSpaces(IN const RvChar** startPtr)
{
    while(isspace((int)**startPtr))
        (*startPtr)++;
}


static RvBool isNonFirstNameChar(IN const RvChar c)
{
    return (isalnum((int)c) || (c == '-') || (c == '/') || (c == '\\'));
}


static void eatName(IN const RvChar** startPtr)
{
    while (isNonFirstNameChar(**startPtr))
        (*startPtr)++;
}


static int eatNumber(IN const RvChar** startPtr)
{
    int number = 0;
    while (isdigit((int)**startPtr))
        number = (number * 10) + (*((*startPtr)++)-'0');
    return number;
}


static int addComp(IN RvInt* buffSize, IN RvUint8** buff, IN unsigned number)
{
    int bytes=0;
    int i;
    char c;

    for (i=0;i<5;i++)
    {
        c = (RvUint8)(number>>(i?25:28));
        if (*buff)
            **buff = (RvUint8)c;
        number<<=(i?7:4);
        if (c||bytes||i==4)
        {
            if (*buff)
            {
                if (*buffSize <= 1)
                    return -1;
                if (i!=4)
                    **buff += (RvUint8)0x80;
                (*buff)++;
            }
            (*buffSize)--;
            bytes++;
        }
    }
    return bytes;
}


/* == oidEncodeOID ==

   oidSize  size of target oid buffer
   oid      target oid buffer, or null
   buff     buffer of string data to encode

   Returns the size of the output string or -1 if there was not enough space.
   If (oid==NULL), the function doesn't write it's output to the oid buffer,
   but the size of the output string is still returned for valid inputs.
   oidSize is ignored in this case.
*/
RVINTAPI int RVCALLCONV
oidEncodeOID(
    IN  int         oidSize,
    OUT char        *oid,
    IN  const char  *buff)
{
    const RvChar *startPtr=(RvChar*)buff;
    int size =oidSize;
    int number;
    int compNum=1;
    int comps[2] = {0, 0};

    if (oid == NULL)
        oidSize = 0;

    eatWhiteSpaces(&startPtr);
    while (*startPtr && *startPtr!='}')
    {
        const RvChar *nexttoken;
        if (isalpha((int)*startPtr))
        {
            const RvChar*stopPtr;
            nexttoken=startPtr+1;
            eatName(&nexttoken);
            stopPtr=nexttoken;
            eatWhiteSpaces(&nexttoken);
            if (*nexttoken=='(') /*NameAndNumberForm*/
            {
                nexttoken++;
                eatWhiteSpaces(&nexttoken);
                number=eatNumber(&nexttoken);
                eatWhiteSpaces(&nexttoken);
                if (*nexttoken==')')  nexttoken++;
            }
            else /*NameForm*/
            {
                if (compNum==3 && !comps[0] && !comps[1])
                    number=findName(series,startPtr, (RvSize_t)(stopPtr-startPtr));
                else
                    number=findName((compNum>1)?oidNodes[comps[0]]:node, startPtr, (RvSize_t)(stopPtr-startPtr));
                if (compNum==1) number%=3;
            }
        }
        else if (isdigit((int)*startPtr)) /*NumberForm*/
        {
            nexttoken=startPtr;
            eatWhiteSpaces(&nexttoken);
            number=eatNumber(&nexttoken);
            if (*nexttoken=='(') /*NameAndNumberForm*/
            {
                nexttoken++;
                eatWhiteSpaces(&nexttoken);
                if (eatNumber(&nexttoken) != number)
                    return -1;
                eatWhiteSpaces(&nexttoken);
                if (*nexttoken != ')')
                    return -1;
                nexttoken++;
            }
        }
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
        else if (*startPtr == '(')
        {
            nexttoken=++startPtr;
            eatWhiteSpaces(&nexttoken);
            number=eatNumber(&nexttoken);
            eatWhiteSpaces(&nexttoken);
            if (*nexttoken != ')')
                return -1;
            nexttoken++;
        }
        else
            return -1;

        if (compNum<3) comps[compNum-1]=number;
        if (compNum==2)
        {
            number=comps[0]*40+comps[1];
        }

        if (compNum>=2)
            if (addComp(&oidSize, (RvUint8**)&oid, (unsigned) number) < 0)
                return -1;
        compNum++;
        startPtr=nexttoken;
        eatWhiteSpaces(&startPtr);
    }
    return size-oidSize;
}


static int getComp(IN int *oidSize, IN unsigned char** oid)
{
    int number=0;
    while (*oidSize)
    {
        number <<= 7;
        number += (**oid) & 0x7f;
        (*oidSize)--;
        if (*((*oid)++) < 0x80)
            break;
    }
    return number;
}

/* The macro below should NEVER be used with a semicolon in the end of it! */
#define IF_SPACE_OK(checklen)                   \
{                                               \
    addlen = (checklen);                        \
    if (*buff)                                  \
    {                                           \
        (*buffSize) -= addlen;                  \
        if (*buffSize > 0)

/* The macro below should NEVER be used with a semicolon in the end of it! */
#define END_IF_SPACE                            \
    }                                           \
    len += addlen;                              \
}


static int putComp(OUT int* buffSize, OUT RvChar** buff, IN int num, IN const RvChar** nodes, IN int numNodes, IN form f)
{
    char nBuf[10];
    int i=10, len = 0, addlen;
    int saveNum = num;
    nBuf[9] = 0;

    /* Make sure the user's number here is within boundaries of the nodes array, otherwise,
       we won't try to write down the node's name since we don't know it. Please note that
       if numNodes < 0, we treat it as if the value is always within the range. */
    if ((numNodes > 0) && (saveNum >= numNodes))
        nodes = NULL;

    if ((nodes != NULL) && nodes[saveNum] && f!=numberForm)
    {
        IF_SPACE_OK(strlen(nodes[saveNum]) + (f!=nameForm))
        {
            strcpy(*buff,nodes[saveNum]);
            *buff+=strlen(nodes[saveNum]);
            if (f!=nameForm)
                *((*buff)++)='(';
        }
        END_IF_SPACE;
    }

    do
    {
        nBuf[--i] = (RvChar)((num%10)+'0');
        num /= 10;
    }
    while(num);

    if ((f != nameForm) || !(nodes && nodes[saveNum]))
    {
        IF_SPACE_OK(10 - i)
        {
            memcpy(*buff, nBuf+i, (RvSize_t)(10-i));
            *buff+=(10-i);
        }
        END_IF_SPACE;
    }

    if (nodes && nodes[saveNum] && (f == nameAndNumberForm))
    {
        IF_SPACE_OK(1)
        {
            *((*buff)++)=')';
        }
        END_IF_SPACE;
    }

    IF_SPACE_OK(1)
    {
        *((*buff)++)=' ';
    }
    END_IF_SPACE;

    return len;
}


/* == oidEncodeOID ==

   oidSize  size of source oid buffer
   oid      source oid buffer
   buffSize size of target buffer
   buff     target buffer for decoded oid
   form     the format for the decoded oid

   Returns the size of the output string or -1 if there was not enough space.
   If (buff==NULL), the function doesn't write it's output to the buffer,
   but the size of the output string is still returned for valid inputs.
   buffSize is ignored in this case.
*/
RVINTAPI int RVCALLCONV
oidDecodeOID(int oidSize, char* oid, int buffSize, char *buff,form f)
{
    char *origBuff = buff;
    int compNum=2, num, root=0, len = 0;

    if (oidSize < 0)
    {
        if (buff != NULL)
            *buff = 0;
        return RV_ERROR_UNKNOWN;
    }

    while (oidSize)
    {
        num = getComp(&oidSize,(unsigned char **)&oid);
        if (num < 0)
        {
            /* Bad OID - number is out of range */
            if ((buff != NULL) && (buffSize > 0))
                *buff = 0;
            return RV_ERROR_OUTOFRANGE;
        }

        if (compNum>2)
        {
            const RvChar** curNodes = NULL;
            int numNodes = 0;

            if (!root && (compNum == 3))
            {
                curNodes = series;
                numNodes = (int)(sizeof(series) / sizeof(char*));
            }
            len += putComp(&buffSize, &buff, num, curNodes, numNodes, f);
        }
        else
        {
            root = num / 40;
            if (root > 2)
            {
                /* joint-iso-itu-t may have more than 40 sub-arcs */
                root = 2;
                num -= 80;
            }
            else
            {
                num %= 40; /* the second level has up to 40 arcs */
            }

            len += putComp(&buffSize, &buff, root, node, (int)(sizeof(node) / sizeof(char*)), f);
            len += putComp(&buffSize, &buff, num, oidNodes[root], sizeNodes[root], f);
        }
        compNum++;
    }

    if ((buff != NULL) && (buffSize > 0))
    {
        if ((buff > origBuff) && (*(buff-1) == ' '))
            buff--; /* Remove the trailing space-bar character */
        *buff = 0;
    }

    return (buffSize >= 0)? len : buffSize;
}


RVINTAPI int RVCALLCONV
oidDecodeOIDInt(
        IN  int             oidSize,
        IN  char*           oid,
        IN  int             buffSize,
        OUT int *           buff)
{
    int compNum=2, num, root=0, len = 0;

    if (oidSize<0)
        return RV_ERROR_UNKNOWN;

    while (oidSize || buffSize>len)
    {
        num = getComp(&oidSize,(unsigned char **)&oid);
        if (num < 0)
        {
            /* Bad OID - number is out of range */
            return RV_ERROR_OUTOFRANGE;
        }

        if (compNum>2)
        {
            if (buffSize!=len) buff[len++]=num;
        }
        else
        {
            root = num / 40;
            if (root > 2)
            {
                /* joint-iso-itu-t may have more than 40 sub-arcs */
                root = 2;
                num -= 80;
            }
            else
            {
                num %= 40; /* the second level has up to 40 arcs */
            }

            if (buffSize!=len) buff[len++]=root;
            if (buffSize!=len) buff[len++]=num;
        }
        compNum++;
    }
    return len;
}

#ifdef __cplusplus
}
#endif



