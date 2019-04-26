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
#include "h32xLog.h"
#include "psyntreeStackApi.h"
#include "pvaltree.h"
#include "emanag.h"
#include "q931.h"


#ifdef __cplusplus
extern "C" {
#endif

#if (RV_ASN1_CODER_USE_Q931 == RV_YES)


#define ROOT_LEVEL      0
/*#define MSG_LEVEL       1*/
/*#define OTHER_IES_LEVEL 2*/
#define IE_LEVEL        3
/*#define OCTET_LEVEL     4*/
/*#define FIELD_LEVEL     5*/


static RvLogSource rvQ931LogSource;
static RvLogSource rvQ931ErrLogSource;


static int qILog2(RvInt32 N)
{
    int i=-1;
    while(N) {i++;N>>=1;}
    return i;
}


static int Q931EInter(
    IN emanagStruct *emanag,
    IN int          nodeId,
    IN RvInt32      fieldId,
    IN RvPvtNodeId  vNodeId,
    IN int          level,
    IN int          bit)
{
    int lengthBit=0;
    pstTagClass  tagClass;
    pstNodeType type;
    int numOfChildren;
    int tag;
    RvUint8 *buffer = emanag->buffer;
    HPST synH = emanag->hSyn;
    HPVT valH = emanag->hVal;

    RV_UNUSED_ARG(fieldId); /* Needed when compiling without logs */

    tag = pstGetTag(emanag->hSyn, nodeId, &tagClass);
    if (tagClass==pstTagPrivate && tag!=(int)emQ931)
    {
        int len, ret;
        RvLogDebug(&rvQ931LogSource, (&rvQ931LogSource, "Private tag %d", tag));
        emanag->buffer += (bit/8);
        emanag->length = (emanag->length - bit);
        ret = emEncode(emanag, vNodeId, &len);
        emanag->buffer -= (bit/8);
        emanag->length = (emanag->length + bit);
        if (ret < 0)
        {
            RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                "Q931EInter: Private tag encoding error (%d).", tag));
            return RV_ERROR_UNKNOWN;
        }
        return bit+len*8;
    }

    if (tag != RV_ERROR_UNKNOWN)
    {
        /* See if we have to place a tag in front of the encoded element */
        if ((tagClass == pstTagEmpty) || (tagClass == pstTagApplication))
        {
            if (emanag->length <= bit+8)
            {
                RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                    "Q931EInter: Buffer too small to encode this message."));
                return RV_ERROR_UNKNOWN;
            }

            buffer[bit/8] = (RvUint8)tag;
            bit += 8;
        }

        if (level == IE_LEVEL)
        {
            /* It's an information element - we have to add a length as well... */
            lengthBit = bit;
            if (tagClass == pstTagApplication)
                bit += 16;
            else
                bit += 8;
        }
    }

    level++;
    type = pstGetNodeType(synH,nodeId);

    RvLogDebug(&rvQ931LogSource, (&rvQ931LogSource,
        "Encoding %s: %s [%d].",
        (fieldId>=0)?pstGetFieldNamePtr(synH, fieldId):"(null)", pstGetTokenName(type), bit));
    numOfChildren=pstGetNumberOfChildren(synH,nodeId);
    switch(type)
    {
        case pstSet:
        {
            int i;
            pstChildExt child;
            RvPvtNodeId curChild;
            RvPstFieldId curFieldId = (RvPstFieldId)-1;
            RvUint32* iter;
            
            curChild = pvtChild(valH, vNodeId);
            pvtGet(valH, curChild, &curFieldId, NULL, NULL, NULL);
            iter = pstGetChildExt(synH, nodeId, 1, &child);
            for (i = 1; i <= numOfChildren; i++)
            {
                if (child.fieldId != curFieldId)
                {
                    if (child.isOptional || child.isDefault || emanag->isTolerant)
                    {
                        iter = pstGetBrotherExt(synH, i+1, iter, &child);
                        continue;
                    }
                    else
                    {
                          RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                              "Q931EInter: non optional Child not found [%d]%s.",
                              vNodeId,
                              pstGetFieldNamePtr(synH, child.fieldId)));
                          return RV_ERROR_UNKNOWN;
                    }
                }

                bit = Q931EInter(emanag, child.nodeId, child.fieldId, curChild, level, bit);
                if (bit < 0)
                    return bit;

                curChild = pvtBrother(valH, curChild);
                pvtGet(valH, curChild, &curFieldId, NULL, NULL, NULL);
                iter = pstGetBrotherExt(synH, i+1, iter, &child);
            }
        }
        break;

        case pstSequence:
        {
            int i;
            pstChildExt child;
            RvPvtNodeId curChild;
            RvPstFieldId curFieldId = (RvPstFieldId)-1;
            RvUint32* iter;
            
            curChild = pvtChild(valH, vNodeId);
            pvtGet(valH, curChild, &curFieldId, NULL, NULL, NULL);
            iter = pstGetChildExt(synH, nodeId, 1, &child);
            for (i = 1; ; i++)
            {
                if (pstGetIsExtended(synH,nodeId) && !(bit%8))
                {
                    if (emanag->length <= bit+8)
                    {
                        RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                            "Q931EInter: Buffer too small to encode this message."));
                        return RV_ERROR_UNKNOWN;
                    }

                    buffer[bit/8]=0x80;
                    bit++;
                }

                if (child.fieldId != curFieldId)
                {
                    if (child.isExtended)
                    {
                        if(bit%8==1)
                        {
                            bit--;
                            buffer[bit/8-1]|=0x80;
                        }
                        else
                        {
                            bit+=8-bit%8;
                            /* ?????????????? */
                        }
                        break;
                    }
                    else if (child.isOptional || child.isDefault || emanag->isTolerant)
                    {
                        /* Optional field - ignore and go on */
                        if (i == numOfChildren)
                            break;
                        iter = pstGetBrotherExt(synH, i+1, iter, &child);
                        continue;
                    }
                    else
                    {
                        RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                            "Q931EInter: non optional Child not found [%d]%s->%s.",
                            vNodeId, pstGetFieldNamePtr(synH,fieldId),
                            pstGetFieldNamePtr(synH, child.fieldId)));
                        return RV_ERROR_UNKNOWN;
                    }
                }

                bit = Q931EInter(emanag, child.nodeId, child.fieldId, curChild, level, bit);
                if (bit < 0)
                    return bit;
                if (i == numOfChildren)
                    break;
                if (pstGetIsExtended(synH, nodeId) && !(bit%8))
                    buffer[bit/8-1] &= 0x7f;

                curChild = pvtBrother(valH, curChild);
                pvtGet(valH, curChild, &curFieldId, NULL, NULL, NULL);
                iter = pstGetBrotherExt(synH, i+1, iter, &child);
            }
        }
        break;

        case pstSequenceOf:
        {
            /* SEQUENCE OF in Q931 is handled as if it's not there. We use it to allow several
               information elements with the same tag to be encoded/decoded together. */
            int childNodeId;
            int childSynNodeId;
            RvPstFieldId childFieldId;

            /* Loop through the array and encode each element as if it was a fresh one out
               of the box. */
            childNodeId = pvtChild(valH, vNodeId);
            pvtGet(valH, childNodeId, &childFieldId, &childSynNodeId, NULL, NULL);

            while (RV_PVT_NODEID_IS_VALID(childNodeId))
            {
                /* We go and encode our internal structures without increasing the level since we
                   don't want the SEQUENCE OF structure to show up in the encoding */
                bit = Q931EInter(emanag, childSynNodeId, childFieldId, childNodeId, level-1, bit);
                if (bit < 0)
                    return bit;

                /* Get the next one */
                childNodeId = pvtBrother(valH, childNodeId);
            }

            /* Make sure we don't think we're in the same length, since it causes a length
               value to be written on top of the protocol discriminator in Q931 */
            level++;
        }
        break;

        case pstChoice:
        {
            int childNodeId=pvtChild(valH,vNodeId);
            int childSynNodeId;
            RvPstFieldId childFieldId;

            if(childNodeId==RV_ERROR_UNKNOWN)
            {
                RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                    "perEncodeChoice: Value node does not exist. [%s]",
                    pstGetFieldNamePtr(synH, fieldId)));
                return RV_ERROR_UNKNOWN;
            }

            pvtGet(valH,childNodeId,&childFieldId,&childSynNodeId,NULL,NULL);

            bit = Q931EInter(emanag, childSynNodeId, childFieldId, childNodeId, level, bit);
            if(bit<0) return bit;
        }
        break;

        case pstInteger:
        {
            RvUint32 value;
            int to , from, bits;
            pstGetNodeRange( synH, nodeId, &from, &to);
            bits=qILog2(to-from)+1;
            pvtGet(valH,vNodeId,NULL,NULL,(RvInt32*)&value,NULL);

            if (emanag->length <= bit + bits)
            {
                RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                    "Q931EInter: Buffer too small to encode this message."));
                return RV_ERROR_UNKNOWN;
            }

            if (bits <= 8)
            {
                buffer[bit/8] &= ~(((1<<bits)-1)<<(8-(bit%8)-bits));
                buffer[bit/8] |= value<<(8-(bit%8)-bits);
            }
            else if (bits==16)
            {
                buffer[bit/8]=(RvUint8)(value>>8);
                buffer[bit/8+1]=(RvUint8)value;
            }
            bit += bits;
        }
        break;

        case pstIA5String:
        case pstOctetString:
        {
            RvInt32 strLength;
            if (bit % 8) {bit+=7; bit-=bit%8;}

            pvtGet(valH,vNodeId,NULL,NULL,&strLength,NULL);

            if (emanag->length <= bit+((int)strLength)*8)
            {
                RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                    "Q931EInter: Buffer too small to encode this message."));
                return RV_ERROR_UNKNOWN;
            }

            pvtGetString(valH,vNodeId,strLength,(char*)&(buffer[bit/8]));
            bit+=((int)strLength)*8;
        }
        break;

        case pstNull:
            if(bit-lengthBit==8) bit-=8;
            return bit;

        default:
            RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                "Q931EInter: Unknown type (node=%d) - don't know how to encode.",
                vNodeId));
            return -1;
    }

    if (level == IE_LEVEL+1)
    {
        /* We should place the length value for this field... */
        if (tagClass==pstTagApplication)
        {
            /* Application tags get a length of 2 bytes */
            buffer[lengthBit/8]=(unsigned char)(((bit-lengthBit)/8-2)>>8);
            buffer[lengthBit/8+1]=(unsigned char)((bit-lengthBit)/8-2);
        }
        else
        {
            /* Check if this field didn't overflow */
            RvInt fieldLength = (bit-lengthBit)/8-1;

            if (fieldLength > 0xff)
            {
                RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                    "Q931EInter: Field too long for tag %d.", tag));
                return -1;
            }

            /* Only one byte length for most fields... */
            buffer[lengthBit/8]=(unsigned char)fieldLength;
        }
    }

    return bit;
}




static int Q931Encode(
    IN  emanagStruct    *emanag,
    IN  RvPvtNodeId     vNodeId,
    OUT int             *encoded)
{
    int nodeId;
    pvtGet(emanag->hVal, vNodeId, NULL, &nodeId, NULL, NULL);

    *encoded = Q931EInter(emanag, nodeId, -1, vNodeId, ROOT_LEVEL, 0);
    if (*encoded < 0)
        return (*encoded);
    else
    {
        *encoded /= 8;
        return 0;
    }
}


static int Q931DInter(
    IN emanagStruct *emanag,
    IN int          nodeId,
    IN int          fieldId,
    IN RvPvtNodeId  vNodeId,
    IN int          level,
    IN int          bit)
{
    int newBit, ret;
    int tag;
    pstTagClass  tagClass;
    pstNodeType type;
    int numOfChildren;
    int stag;
    RvUint8 *buffer = emanag->buffer;
    HPST synH = emanag->hSyn;
    HPVT valH = emanag->hVal;

    tag = buffer[bit/8];

    type = pstGetNodeType(synH, nodeId);
    if (type != pstSequenceOf)
    {
        /* "Regular node" - get the tag from it */
        stag = pstGetTag(synH, nodeId, &tagClass);
    }
    else
    {
        /* It's a SEQUENCE OF - the tag is one step down */
        stag = pstGetTag(synH, pstGetNodeOfId(synH, nodeId), &tagClass);
    }

    if (nodeId != RV_ERROR_UNKNOWN)
    {
        if ((tagClass == pstTagPrivate) && (stag != (int)emQ931))
        {
            int len;
            emanag->buffer += (bit/8);
            emanag->length = (emanag->length - bit);
            ret = emDecode(emanag, vNodeId, fieldId, &len);
            emanag->buffer -= (bit/8);
            emanag->length = (emanag->length + bit);
            if (ret >= 0)
                return bit + len*8;
            else
                return ret;
        }
        if ((tagClass == pstTagEmpty || tagClass == pstTagApplication) &&  stag != RV_ERROR_UNKNOWN)
        {
            if (tag == stag)
                bit += 8;
            else
                return bit;
        }
    }
    else
        bit += 8;

    if ((level == IE_LEVEL) && !(tag & 0x80))
    {
        int ieLen;
        int lenLen;
        if (nodeId != RV_ERROR_UNKNOWN && tagClass == pstTagApplication)
        {
            lenLen = 16;
            ieLen = (buffer[bit/8]*256 + buffer[bit/8 + 1])*8;
        }
        else
        {
            lenLen = 8;
            ieLen = buffer[bit/8]*8;
        }
        bit += lenLen;
        newBit = ieLen + bit;
    }
    else
    {
        newBit = bit;
    }

    level++;

    RvLogDebug(&rvQ931LogSource, (&rvQ931LogSource,
        "Decoding Tag:%d : Level %d [%d]", tag, level, bit));
    if (newBit > emanag->length)
    {
        RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
            "Q931DInter: Buffer size is too small - expected at least %d bits", newBit));
        return RV_ERROR_UNKNOWN;
    }

    if (nodeId != RV_ERROR_UNKNOWN)
    {
        int i;
        int oldBit;
        int vcNodeId=vNodeId;
        pstChildExt child;

        numOfChildren = pstGetNumberOfChildren(synH, nodeId);

        switch (type)
        {
            case pstSet:
            {
                int searchedTag = -1;
                int lastTag;
                RvUint32* iter;
                RvBool firstRun = RV_TRUE;
                int leftToProcess;

                if (fieldId!=RV_ERROR_UNKNOWN)
                if ((vcNodeId = pvtAdd(valH, vNodeId, fieldId, 0, NULL, NULL)) < 0)
                    return vcNodeId;
                iter = pstGetChildExt(synH, nodeId, 1, &child);
                i = 0;

                /* We're trying t count all of the fields we check so we don't have a decoding problem */
                leftToProcess = numOfChildren - pstGetNumberOfOptionalBeforeExtension(synH, nodeId);

                while (bit < emanag->length)
                {
                    lastTag = searchedTag;
                    searchedTag = buffer[bit/8];
                    if ((searchedTag < lastTag) || (searchedTag & 0x80))
                    {
                        /* Seems like this Q931 buffer has its tags unsorted (which is not standard) */

                        /* Make sure the iterator starts all over again - we must search it all */
                        firstRun = RV_TRUE;
                        iter = pstGetChildExt(synH, nodeId, 1, &child);
                        searchedTag = -1;
                        i = 0;
                    }
                    else if (searchedTag == lastTag)
                    {
                        if (pstGetNodeType(synH, child.nodeId) != pstSequenceOf)
                        {
                            /* Oh - same tag twice - let's see if our application is ready for this one */
                            /* If it's ready, then it will have a SEQUENCE OF field right after the current
                               one, which will have the same tag */
                            pstChildExt nextChild;
                            RvUint32* nextIter = pstGetBrotherExt(synH, 1, iter, &nextChild);
                            if ((nextIter != NULL) && (pstGetNodeType(synH, nextChild.nodeId) == pstSequenceOf))
                            {
                                if (pstGetTag(synH, pstGetNodeOfId(synH, nextChild.nodeId), NULL) == searchedTag)
                                {
                                    /* Definition has a SEQUENCE OF with the same tag value for the elements
                                       let's use them */
                                    iter = nextIter;
                                    child = nextChild;
                                    i++;
                                }
                            }
                        }

                        /* If we didn't use the SEQUENCE OF, then we'll just rewrite the current value,
                           as was the case in previous versions */
                    }
                    else
                    {
                        if (firstRun == RV_FALSE)
                        {
                            /* Skip the current field - we don't really have to check it */
                            iter = pstGetBrotherExt(synH, 1, iter, &child);
                            i++;
                        }
                        else
                            firstRun = RV_FALSE; /* We should make sure to check the first field */
                    }

                    /* Start searching for the tag and decode it while we're at it */
                    while (i < numOfChildren)
                    {
                        oldBit = bit;
                        bit = Q931DInter(emanag, child.nodeId, child.fieldId, vcNodeId, level, bit);
                        if (bit < 0)
                        {
                            /* Had an error */
                            return bit;
                        }
                        if (bit != oldBit)
                        {
                            /* We actually decoded it - we can get out of this while loop */
                            if (!child.isOptional)
                                leftToProcess--;
                            break;
                        }

                        /* Try the next one */
                        iter = pstGetBrotherExt(synH, 1, iter, &child);
                        i++;
                    }

                    if (i == numOfChildren)
                    {
                        /* Didn't find this tag - we should decode it and skip it */
                        bit = Q931DInter(emanag, RV_ERROR_UNKNOWN, RV_ERROR_UNKNOWN, RV_ERROR_UNKNOWN, level, bit);
                        if (bit < 0)
                            return  bit;

                        /* We want to start the search again so the tags we're going to see next
                           are going to be handled (iter==NULL) */
                        firstRun = RV_TRUE;
                        iter = pstGetChildExt(synH, nodeId, 1, &child);
                        searchedTag = -1;
                        i = 0;
                    }
                }

                if (leftToProcess > 0)
                {
                    RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                        "Q931DInter: Mandatory fields inside a Q931 message were missing"));
                    return RV_ERROR_UNKNOWN;
                }
            }
            break;

            case pstChoice:
            {
                RvUint32* iter;

                if (fieldId!=RV_ERROR_UNKNOWN)
                {
                    if ((vcNodeId = pvtAdd(valH, vNodeId, fieldId, 0, NULL, NULL)) < 0)
                        return vcNodeId;
                }

                iter = pstGetChildExt(synH, nodeId, 1, &child);
                for (i = 1; (i <= numOfChildren) && ((bit < newBit) || (level != IE_LEVEL + 1)); i++)
                {
                    oldBit = bit;
                    bit = Q931DInter(emanag, child.nodeId, child.fieldId, vcNodeId, level, bit);
                    if (bit < 0)
                        return bit;
                    if (bit != oldBit)
                    {
                        pvtSet(valH, vcNodeId, fieldId, child.fieldId, NULL);
                        break;
                    }
                    iter = pstGetBrotherExt(synH, i+1, iter, &child);
                }
                if (i > numOfChildren)
                {
                    RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                        "Q931DInter: Choice value out of range [%d].", nodeId));
                    return RV_ERROR_UNKNOWN;
                }
            }
            break;

            case pstSequence:
            {
                RvUint32* iter;
                RvInt32 storedLength;
                RvBool extended = RV_FALSE;
                RvBool extendedNode = (RvBool)pstGetIsExtended(synH, nodeId);
                if (fieldId != RV_ERROR_UNKNOWN)
                {
                    if ((vcNodeId = pvtAdd(valH, vNodeId, fieldId, 0, NULL, NULL)) < 0)
                        return vcNodeId;
                }

                storedLength = emanag->length;
                if (newBit != bit)
                    emanag->length = newBit;

                iter = pstGetChildExt(synH, nodeId, 1, &child);
                for (i = 1; (i <= numOfChildren) && ((bit < newBit) || (level != IE_LEVEL + 1)); i++)
                {
                    if (extendedNode && !(bit%8))
                    {
                        extended=!(buffer[bit/8]&0x80);
                        bit++;
                    }
                    bit = Q931DInter(emanag, child.nodeId, child.fieldId, vcNodeId, level, bit);
                    if (bit < 0)
                        return bit;
                    if (extendedNode && !(bit%8) && !extended)
                        break;
                    iter = pstGetBrotherExt(synH, i+1, iter, &child);
                }
                emanag->length = storedLength;
                pvtSet(valH, vcNodeId, fieldId, pvtNumChilds(valH, vcNodeId), NULL);
                if (extendedNode  && extended)
                    while (!(buffer[bit/8]&0x80)) bit += 8;
            }
            break;

            case pstSequenceOf:
            {
                /* Seems like we should simulate receiving several information elements */
                int vSeqOfNode;

                /* Let's make sure first that we have the SEQUENCE OF in the PVT */
                vSeqOfNode = pvtAdd(valH, vNodeId, fieldId, 0, NULL, NULL);
                if (vSeqOfNode < 0)
                    return vSeqOfNode;

                /* Now let's add a new last element to this array by decoding the field - make sure
                   we go back in the buffer and up in level to simulate proper type and position. */
                bit = Q931DInter(emanag, pstGetNodeOfId(synH, nodeId), -800, vSeqOfNode, level-1, bit-16);
            }
            break;

            case pstInteger:
            {
                RvUint32 value = 0;
                int to , from, bits;
                pstGetNodeRange( synH, nodeId, &from, &to);
                bits=qILog2(to-from)+1;

                if (bits <= 8)
                    value = (buffer[bit/8] >> ((8 - (bit%8)) - bits))&((1 << bits) - 1);
                else
                    if (bits == 16)
                        value = (buffer[bit/8] << 8) + buffer[bit/8 + 1];
                if ((ret = pvtAdd(valH, vNodeId, fieldId, (RvInt32)value, NULL, NULL)) < 0)
                    return ret;
                bit += bits;
            }
            break;

            case pstNull:
            {
                if ((ret = pvtAdd(valH, vNodeId, fieldId, 0, NULL, NULL)) < 0)
                    return ret;
            }
            break;

            case pstIA5String:
            case pstOctetString:
            {
                RvUint32 strLength;
                char buff[256];
                strLength = (RvUint32)(((level != IE_LEVEL + 1) ? emanag->length:newBit) - bit)/8;
                if (bit%8)
                {
                    bit += 7;
                    bit -= bit%8;
                }
                strLength = RvMin(strLength, 255);
                memcpy(buff, &(buffer[bit/8]), (RvSize_t)strLength);
                if ((ret = pvtAdd(valH, vNodeId, fieldId, (RvInt32)strLength, buff, NULL)) < 0)
                    return ret;
            }
            break;

        default:
            RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
                "Q931DInter: Unknown type (node=%d) - don't know how to decode.",
                vNodeId));
            return -1;
        }
    }
    if (level == IE_LEVEL + 1)
    {
        bit = newBit;
    }

    return bit;
}


static int Q931Decode(
        IN  emanagStruct    *emanag,
        IN  RvPvtNodeId     vNodeId,
        IN  RvInt32         fieldId,
        OUT int             *decoded)
{
    int nodeId, ret;

    pvtGet(emanag->hVal, vNodeId, NULL, &nodeId, NULL, NULL);

    ret = Q931DInter(emanag, nodeId, fieldId, vNodeId, ROOT_LEVEL, 0);
    if (ret < 0)
    {
        RvLogError(&rvQ931ErrLogSource, (&rvQ931ErrLogSource,
            "Q931Decode: Error decoding Q931 message (%d).", ret));
        return ret;
    }

    *decoded = ret/8;

    return 0;
}



void q931coderInit(void)
{
    emTypeOfEncoding eSys;

    eSys.Encode = Q931Encode;
    eSys.Decode = Q931Decode;

    RvLogSourceConstruct(H32xLogGet(), &rvQ931LogSource, "Q931", "PER Encoder/Decoder");
    RvLogSourceConstruct(H32xLogGet(), &rvQ931ErrLogSource, "Q931ERR", "Q931 Errors");

    emSetEncoding((int)emQ931, &eSys);
}

void q931coderEnd(void)
{
    RvLogSourceDestruct(&rvQ931LogSource);
    RvLogSourceDestruct(&rvQ931ErrLogSource);
}


#endif /* (RV_ASN1_CODER_USE_Q931 == RV_YES) */


#ifdef __cplusplus
}
#endif

