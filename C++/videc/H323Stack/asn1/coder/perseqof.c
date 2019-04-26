#ifdef __cplusplus
extern "C" {
#endif



/*
***********************************************************************************

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

***********************************************************************************
*/

/*
  perSequenceOf.c

  Ron S.
  16 May 1996


  ____________________________________________________________________________
  ___________________________SEQUENCE_OF______________________________________
  ____________________________________________________________________________


  format:

  +----------+--------+---------+       +---------+
  | ext. bit | length | field-1 |  ...  | field-n |
  +----------+--------+---------+       +---------+

  */


#include "rvstdio.h"
#include "perintr.h"
#include "perseqof.h"


/*
  Desc: Encode a sequence OF node.
  According to clause `9.
  Node value contains the number of components in the set.


*/
int
perEncodeSequeceOF(IN  HPER hPer,
           IN  int synParent,
           IN  int valParent, /* this is me */
           IN  RvInt32 fieldId)
{
    perStruct *per =(perStruct *)hPer;
    int vtPath=-1, numOfComponents=-1;
    RvBool IsOK = RV_TRUE, isExtended = RV_FALSE;
    int  to,from;
    RvBool toAbsent,fromAbsent;
    int ret = 0;

    /* -- get number of components */
    numOfComponents = pvtNumChilds(per->hVal, valParent);
    if (numOfComponents < 0)
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perEncodeSequeceOF: Value node not exist. [%s]",
            pstGetFieldNamePtr(per->hSyn, fieldId)));
        return RV_ERROR_UNKNOWN;
    }
    pstGetNodeRangeExt(per->hSyn,synParent,&from,&to,&fromAbsent,&toAbsent);

    if (pstGetIsExtended(per->hSyn,synParent) == RV_FALSE) /* -- validate numOfComponents */
    {
        if (!fromAbsent && !toAbsent &&
            from == to &&  numOfComponents != from) IsOK = RV_FALSE; /* no length */
        if ((!toAbsent && numOfComponents > to) || (!fromAbsent && numOfComponents <from))
            IsOK = RV_FALSE;

        if (!IsOK)
        {
            RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
                "perEncodeSequeceOF:%s: Illegal number of elements for set. [%d]",
                pstGetFieldNamePtr(per->hSyn, fieldId), numOfComponents));
            return RV_ERROR_UNKNOWN;
        }
    }
    else /* -- extension */
    {
        if ((!fromAbsent && numOfComponents < from) || (!toAbsent && to < numOfComponents))
        {
            isExtended = RV_TRUE;
            perEncodeBool(RV_TRUE, per->hBB); /* adding extension bit. */
        }
        else
            perEncodeBool(RV_FALSE, per->hBB);

            /*
            msaPrintFormat(ErrPER, "perEncodeSequeceOF: NO EXTENSION SUPPORT [%s].",
            pstGetFieldNamePtr(per->hSyn, fieldId));
            return RV_ERROR_UNKNOWN;
        */
    }

    /* -- length: 19.7 */
    if ((fromAbsent && toAbsent) || isExtended)
        /* encode length as unconstrained mode. */
        perEncodeLen(perLenTypeUNCONSTRAINED, (RvUint32)numOfComponents, 0, 0, per->hBB);
    else
        if (!fromAbsent && !toAbsent) /* encode length as constrained. */
            perEncodeLen(perLenTypeCONSTRAINED, (RvUint32)numOfComponents, (RvUint32)from, (RvUint32)to, per->hBB);
        else
            if (!fromAbsent && toAbsent)
                perEncodeLen(perLenTypeUNCONSTRAINED, (RvUint32)numOfComponents, (RvUint32)from, 0, per->hBB);

    /* -- components encoding */
    vtPath=pvtChild(per->hVal,valParent);
    while ((vtPath >= 0) && (ret >= 0))
    {
        ret = perEncNode(hPer, pstGetNodeOfId(per->hSyn, synParent), vtPath, fieldId, RV_FALSE);
        vtPath=pvtBrother(per->hVal,vtPath);
    }

    return ret;
}


/*
  Desc: Decode a sequence OF node.
  According to clause `9.
  Node value contains the number of components in the set.

*/
int
perDecodeSequeceOF(IN  HPER hPer,
           IN  int synParent, /* parent in syntax tree */
           IN  int valParent, /* field parent in value tree */
           IN  RvInt32 fieldId)   /* enum of current field */
{
  RvBool isExtended=RV_FALSE;
  perStruct *per = (perStruct *)hPer;

    RvUint32 dec = 0; /* decoded bits */
    int numOfComponent=0;
    int vtPath=-1,i;
    RvUint32 pos_from = per->decodingPosition; /* internal position */

    int  to,from;
    RvBool toAbsent,fromAbsent;
    RvBool extended = (RvBool)pstGetIsExtended(per->hSyn, synParent);

    /* -- extension */
    if (extended == RV_TRUE)
    {
        perDecodeBool( &isExtended, hPer, pos_from, &dec); /* decoding extension bit. Sergey M. */
        pos_from+=dec;
        /*
            msaPrintFormat(ErrPER, "perDecodeSequeceOF: NO EXTENSION SUPPORT [%s].",
            pstGetFieldNamePtr(per->hSyn, fieldId));
                return RV_ERROR_UNKNOWN;
        */
    }
    pstGetNodeRangeExt(per->hSyn,synParent,&from,&to,&fromAbsent,&toAbsent);

    /* -- number of components: 19.7 */
    /* -- length: 19.7 */
    if (toAbsent || isExtended)   /* encode length as unconstrained mode. */
    {
        if (perDecodeLen(perLenTypeUNCONSTRAINED, (RvUint32 *)&numOfComponent, 0, 0, hPer, pos_from, &dec) < 0)
            return RV_ERROR_UNKNOWN;
        else
            pos_from += dec;
    }
    else
    if (!fromAbsent) /* encode length as constrained. We're certain that toAbsent==0 here. */
    {
        if(from != to)
        {
            if (perDecodeLen(perLenTypeCONSTRAINED, (RvUint32 *)&numOfComponent,
                (RvUint32)from, (RvUint32)to, hPer, pos_from, &dec) < 0)
                return RV_ERROR_UNKNOWN;
            else
                pos_from+=dec;
        }
        else numOfComponent = from;
    }

    if (extended==RV_FALSE &&
        ((!toAbsent && (numOfComponent > to)) ||
         (!fromAbsent && (numOfComponent < from))))
    {
        RvLogError(&rvPerErrLogSource, (&rvPerErrLogSource,
            "perDecodeSequeceOF: %s: Illegal number of elements for set. [%d]",
            pstGetFieldNamePtr(per->hSyn, fieldId), numOfComponent));
       return RV_ERROR_UNKNOWN;
    }




    per->decodingPosition = pos_from;

    /* -- components decoding */
    vtPath=valParent;
    if (fieldId!=RV_ERROR_UNKNOWN)
    vtPath = pvtAdd(per->hVal, valParent, fieldId, numOfComponent, NULL, NULL);

    for (i=0; i<(int)numOfComponent; i++)
    {
        pstChildExt info;
        info.nodeId = pstGetNodeOfId(per->hSyn, synParent);
        info.fieldId = -33;
#if (RV_ASN1_CODER_USE_H450 == RV_YES)
        info.speciality = pstNotSpecial;
#endif
        if (perDecNode(hPer, &info, vtPath) < 0)
            return RV_ERROR_UNKNOWN;
    }

    return vtPath;
}

#ifdef __cplusplus
}
#endif



