/***********************************************************************
        Copyright (c) 2005 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#include "pvaltree.h"
#include "psyntreeStackApi.h"
#include "caputils.h"
#include "cmConf.h"
#include "h245.h"
#include "rvstdio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions used for sorting capability descriptors. */

/* The structure of a capability descriptor which the sorting array is built
   from. */
typedef struct
{
    void **ptr;
    int  descNum;
    int  index;
}Descriptor;

/* The comparison array of the capability descriptors sorting. */
static int compareDescriptors( const void *arg1, const void *arg2 )
{
    Descriptor *desc1 = (Descriptor*)arg1;
    Descriptor *desc2 = (Descriptor*)arg2;

    if (desc1->descNum == desc2->descNum)
    {
        return (desc1->index - desc2->index);
    }
    else
    {
        return (desc1->descNum - desc2->descNum);
    }
}

/*_________________________________________________________________________________*/
int
capStructBuild(
           IN  HPVT hVal,
           IN  int capEntryId, /* the entry Id of the requested capability  */
           cmCapStruct *capability
           )
{
    HPST synCap = pvtGetSynTree(hVal, capEntryId);
    int capEntryNumberId, capId, capItemId;
    RvPstFieldId fieldId;

    capEntryNumberId = pvtGetByIndex(hVal, capEntryId, 1, NULL); /* entry number of the requested CapabilityTableEntry */
    pvtGet(hVal, capEntryNumberId, NULL, NULL, &(capability->capabilityId), NULL); /* get requested CapabilityTableEntry from capabilityTable */
    capId = pvtGetByIndex(hVal, capEntryId, 2, NULL); /* get capability from CapabilityTableEntry*/

    capability->direction = cmCapReceiveAndTransmit; /* set default */

    if (capId<0)
    {
        /* no capability entry - return an empty struct */
        capability->name = (char*)"empty";
        capability->type = cmCapEmpty;
        capability->capabilityHandle = RV_ERROR_UNKNOWN;
        return RV_TRUE;
    }

    capability->name = (char*)"Error"; /* set default in case we didn't implement a case */
    capability->type = cmCapNonStandard; /* set default */

    capItemId = pvtChild(hVal, capId); /* get the capability CHOICE */
    capability->capabilityHandle = pvtChild(hVal, capItemId); /* get the capability itself */
    pvtGet(hVal, capItemId, &fieldId, NULL, NULL, NULL);

    switch (fieldId)
    {
    case __h245(nonStandard):
        pvtGet(hVal, capItemId, &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId); /* set name */
        capability->direction = cmCapReceiveAndTransmit; /* set direction */
        capability->type = cmCapNonStandard; /* set type */
        capability->capabilityHandle = capItemId; /* go one up - why? */
        break;

    case __h245(receiveVideoCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceive;
        capability->type = cmCapVideo;
        break;

    case __h245(transmitVideoCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapTransmit;
        capability->type = cmCapVideo;
        break;

    case __h245(receiveAndTransmitVideoCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceiveAndTransmit;
        capability->type = cmCapVideo;
        break;

    case __h245(receiveAudioCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceive;
        capability->type = cmCapAudio;
        break;

    case __h245(transmitAudioCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapTransmit;
        capability->type = cmCapAudio;
        break;

    case __h245(receiveAndTransmitAudioCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceiveAndTransmit;
        capability->type = cmCapAudio;
        break;

    case __h245(receiveDataApplicationCapability):
        pvtGet(hVal, pvtChild(hVal, pvtChild(hVal, capItemId)), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceive;
        capability->type = cmCapData;
        capability->capabilityHandle = pvtChild(hVal, capability->capabilityHandle); /* get the application CHOICE */
        break;

    case __h245(transmitDataApplicationCapability):
        pvtGet(hVal, pvtChild(hVal, pvtChild(hVal, capItemId)), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapTransmit;
        capability->type = cmCapData;
        capability->capabilityHandle = pvtChild(hVal, capability->capabilityHandle); /* get the application CHOICE */
        break;

    case __h245(receiveAndTransmitDataApplicationCapability):
        pvtGet(hVal, pvtChild(hVal, pvtChild(hVal, capItemId)), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceiveAndTransmit;
        capability->type = cmCapData;
        capability->capabilityHandle = pvtChild(hVal, capability->capabilityHandle); /* get the application CHOICE */
        break;

#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(h233EncryptionTransmitCapability):
        break;

    case __h245(h233EncryptionReceiveCapability):
        break;
#endif
        
    case __h245(conferenceCapability):
        capability->name = (char*)"Conference";
        capability->type = cmCapConference;
        capability->capabilityHandle=capId; /* output the whole capability - why? */
        break;

#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(h235SecurityCapability):
        capability->name = (char*)"H235";
        capability->type = cmCapH235;
        capability->capabilityHandle=capId; /* output the whole capability - why? */
        break;
#endif
    case __h245(maxPendingReplacementFor):
        capability->name = (char*)"MaxPendingReplacementFor";
        capability->type = cmCapMaxPendingReplacementFor;
        capability->capabilityHandle=capId; /* output the whole capability - why? */
        break;

    case __h245(receiveUserInputCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceive;
        capability->type = cmCapUserInput;
        break;

    case __h245(transmitUserInputCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapTransmit;
        capability->type = cmCapUserInput;
        break;

    case __h245(receiveAndTransmitUserInputCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceiveAndTransmit;
        capability->type = cmCapUserInput;
        break;

    case __h245(genericControlCapability):
        capability->name = (char*)"Generic";
        capability->direction = cmCapReceiveAndTransmit;
        capability->type = cmCapGeneric;
        break;

    case __h245(receiveMultiplexedStreamCapability):
        capability->name = (char*)"MultiplexedStream";
        capability->direction = cmCapReceive;
        capability->type = cmCapMultiplexedStream;
        break;

    case __h245(transmitMultiplexedStreamCapability):
        capability->name = (char*)"MultiplexedStream";
        capability->direction = cmCapTransmit;
        capability->type = cmCapMultiplexedStream;
        break;

    case __h245(receiveAndTransmitMultiplexedStreamCapability):
        capability->name = (char*)"MultiplexedStream";
        capability->direction = cmCapReceiveAndTransmit;
        capability->type = cmCapMultiplexedStream;
        break;

#if (RV_H245_LEAN_H223 == RV_NO)
    case __h245(receiveRTPAudioTelephonyEventCapability):
        capability->name = (char*)"AudioTelephonyEvent";
        capability->direction = cmCapReceive;
        capability->type = cmCapAudioTelephonyEvent;
        break;

    case __h245(receiveRTPAudioToneCapability):
        capability->name = (char*)"AudioTone";
        capability->direction = cmCapReceive;
        capability->type = cmCapAudioTone;
        break;

    case __h245(fecCapability):
        pvtGet(hVal, pvtChild(hVal, capItemId), &fieldId, NULL, NULL, NULL);
        capability->name = pstGetFieldNamePtr(synCap, fieldId);
        capability->direction = cmCapReceive;
        capability->type = cmCapFEC;
        break;

    case __h245(multiplePayloadStreamCapability):
        capability->name = (char*)"MultiplePayloadStream";
        capability->direction = cmCapReceiveAndTransmit;
        capability->type = cmCapMultiplePayloadStream;
        break;
#endif

    default:
        break;
    }

    return RV_TRUE;
}


int
capSetBuild(
        /* build array of capability set */
        IN  HPVT hVal,
        IN  int termCapSetId, /* terminalCapabilitySet node id */
        IN  int capabilitiesSize, /* number of elements in capabilities array */
        OUT cmCapStruct** capabilities /* cap names array */
        )
{
    RvPvtNodeId capTableId, capEntryId;
    int i;

    capTableId = pvtGetChild(hVal, termCapSetId, __h245(capabilityTable), NULL);

    if (!RV_PVT_NODEID_IS_VALID(capTableId))
    {
        capabilities[0] = NULL;
        return RV_ERROR_UNKNOWN;
    }

    /* loop all caps */
    capEntryId = pvtChild(hVal, capTableId);
    i = 0;
    while (RV_PVT_NODEID_IS_VALID(capEntryId) && (i < capabilitiesSize))
    {
        capStructBuild(hVal, capEntryId, capabilities[i]);

        capEntryId = pvtBrother(hVal, capEntryId);
        i++;
    }

    /* We only place a NULL at the end if we can */
    if (i < capabilitiesSize)
        capabilities[i] = NULL;

    return RV_TRUE;
}



static cmCapStruct *
capSetFind(
       IN  cmCapStruct **capabilities,
       IN  int capArraySize,
       IN  int capTableEntryNumber
       )
{
  int i;

  for (i=0; (i < capArraySize) && capabilities[i]; i++)
    if (capabilities[i]->capabilityId == capTableEntryNumber)
      return capabilities[i];
  return NULL;
}



/******************************************************************************
 * capDescBuild
 * ----------------------------------------------------------------------------
 * General: Build an array of capability descriptors from a TCS message sorted
 *          in ascending order of capabilityDescriptorNumbers.
 *          The capDesc array is built in 4 hierarchical levels:
 *          CapabilityDescriptor -> simultaneousCapabilities ->
 *          AlternativeCapabilitySet -> CapabilityTableEntry.
 *          Each one of the above 4 levels is built on capDesc according to hierarchy
 *          and separated by NULL pointers.
 *          Therefore, the caller to this function, should do a (cmCapStruct****)
 *          casting to capDesc to be able to read the content of the array.
 *          The figure below displays how it is done:
 *
 *          A: Alternative
 *          S: Simultaneous
 *          D: Descriptor
 *          -: null
 *
 *          (capDesc)
 *          \/
 *          ---------------------------------------------------------
 *          | D D ..D - S S S - S S - ==>  ...  <== A A - A A A A - |
 *          ---------------------------------------------------------
 *
 *
 * Return Value: If successful - Non negative.
 *               other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal              - The PVT handle.
 *         termCapSetId      - TerminalCapabilitySet node id.
 *         capabilities      - The actual capabilities from capabilityTable in TCS.
 *         capSetArraySize   - Maximum size of the capabilities array.
 *         capDescArraySize  - Number of elements in capDesc array.
 * Output: capDesc           - The descriptors array. This array should be allocated
 *                             before calling this function.
 *****************************************************************************/
int capDescBuild(
         IN  HPVT           hVal,
         IN  int            termCapSetId,
         IN  cmCapStruct    **capabilities,
         IN  int            capSetArraySize,
         IN  int            capDescArraySize,
         OUT void           **capDesc)
{
    RvPvtNodeId capDescId;
    RvPvtNodeId capDescEntryId;
    int         Spos;
    int         Apos = capDescArraySize, i = 0;
    Descriptor  descSort[RV_H245_CAP_DESC_SIZE];
    int         numOfDescs = 0;

    if (capDescArraySize > RV_H245_CAP_DESC_SIZE)
        capDescArraySize = RV_H245_CAP_DESC_SIZE;

    capDescId = pvtGetChild(hVal, termCapSetId, __h245(capabilityDescriptors), NULL);

    if (!RV_PVT_NODEID_IS_VALID(capDescId))
    {
        capDesc[0] = NULL;
        return RV_ERROR_NOT_FOUND;
    }
    Spos = pvtNumChilds(hVal, capDescId) + 1;
    if (Spos >= capDescArraySize)
        return RV_ERROR_UNKNOWN;

    /* Remembering the number of descriptors for the sorting. */
    numOfDescs = Spos - 1;
    capDesc[Spos-1] = NULL; /* D delimiter to separate the descriptors group
                               from the Simultaneous group. */

    capDescEntryId = pvtChild(hVal, capDescId);
    while (RV_PVT_NODEID_IS_VALID(capDescEntryId))
    {
        RvPvtNodeId capSimId;
        RvPvtNodeId capSimEntryId;
        RvInt32     descNum = -1;

        /* Building the Descriptors group. We also build another array (descSort)
           of descriptors and descriptors numbers to help us do the sorting. */
        capDesc[i] = &capDesc[Spos];
        descSort[i].ptr = &capDesc[Spos];
        descSort[i].index = i;
        pvtGet(
            hVal, pvtGetByIndex(hVal, capDescEntryId, 1, NULL),
            NULL, NULL, &descNum, NULL);
        descSort[i].descNum = descNum;

        /* Building the Simultaneous group. */
        capSimId = pvtGetByIndex(hVal, capDescEntryId, 2, NULL); /* simultaneous capability */
        capSimEntryId = pvtChild(hVal, capSimId);
        while (RV_PVT_NODEID_IS_VALID(capSimEntryId))
        {
            RvPvtNodeId capAltEntryId;
            int numEntries;
            int k;

            numEntries = pvtNumChilds(hVal, capSimEntryId);
            Apos = Apos - numEntries - 1;
            capDesc[Spos++] = &capDesc[Apos];
            if (Spos >= Apos)
                return RV_ERROR_UNKNOWN; /* overflow */

            /* Building the AlternativeCapSet group. */
            capAltEntryId = pvtChild(hVal, capSimEntryId);
            for (k = 0; k < numEntries; k++)
            {
                RvInt32 capTableEntryNumber;

                pvtGet(hVal, capAltEntryId, NULL, NULL, &capTableEntryNumber, NULL);
                capDesc[Apos+k] = (void*)capSetFind(capabilities, capSetArraySize, (int)capTableEntryNumber);

                capAltEntryId = pvtBrother(hVal, capAltEntryId);
            }

            capDesc[Apos+k] = NULL;

            capSimEntryId = pvtBrother(hVal, capSimEntryId);
        }

        capDesc[Spos++] = NULL;

        capDescEntryId = pvtBrother(hVal, capDescEntryId);
        i++;
    }

    /* Sorting the descriptors array according to the capabilityDescriptorNumber */
    if (numOfDescs > 1)
    {
        /* sort */
        RvQsort((void*)descSort, (size_t)numOfDescs, sizeof(Descriptor), compareDescriptors);
        for (i = 0; i < numOfDescs; i++)
        {
            capDesc[i] = descSort[i].ptr;
        }
    }

    return RV_TRUE;
}


/*_________________________________________________________________________________*/

int
capStructBuildFromStruct(
             /* build single capability entry */
             IN  HPVT hVal,
             IN  int confRootId, /* configuration root id */
             OUT int capId,
             IN  cmCapStruct *capability
             )
{
    HPST synConf = pvtGetSynTree(hVal, confRootId);
    int ret;
    int dataTypeId=-1, capDataId=-1, capsetDataId=-1;
    RvInt16 typeNameId=LAST_TOKEN;
    int iTypeNameId;

    if ((capability == NULL) || (capId < 0))
        return RV_ERROR_UNKNOWN;

    if (capability->name != NULL)
    { /* -- name */
        dataTypeId = pvtAddRootByPath(hVal, synConf, (char *)"channels.0.dataType", 0, NULL);
        if (confGetDataType(hVal, confRootId, capability->name, dataTypeId, NULL, RV_FALSE) < 0)
            return RV_ERROR_UNKNOWN;
        capDataId = pvtChild(hVal, dataTypeId);
    }
    else
    {
        /* No name - must have a handle instead */
        if (capability->capabilityHandle < 0)
            return RV_ERROR_UNKNOWN;
        capDataId = capability->capabilityHandle;
    }

    switch(capability->type)
    {
    case cmCapNonStandard:
        typeNameId = __h245(nonStandard);
        break;

    case cmCapVideo:
        switch (capability->direction)
        {
        case cmCapReceive: typeNameId = __h245(receiveVideoCapability); break;
        case cmCapTransmit: typeNameId = __h245(transmitVideoCapability); break;
        case cmCapReceiveAndTransmit: typeNameId = __h245(receiveAndTransmitVideoCapability); break;
        default: break;
        }
        break;

    case cmCapAudio:
        switch (capability->direction)
        {
        case cmCapReceive: typeNameId = __h245(receiveAudioCapability); break;
        case cmCapTransmit: typeNameId = __h245(transmitAudioCapability); break;
        case cmCapReceiveAndTransmit: typeNameId = __h245(receiveAndTransmitAudioCapability); break;
        default: break;
        }
        break;

    case cmCapData:
        switch (capability->direction)
        {
        case cmCapReceive: typeNameId = __h245(receiveDataApplicationCapability); break;
        case cmCapTransmit: typeNameId = __h245(transmitDataApplicationCapability); break;
        case cmCapReceiveAndTransmit: typeNameId = __h245(receiveAndTransmitDataApplicationCapability); break;
        default: break;
        }
        break;

    case cmCapUserInput:
        switch (capability->direction)
        {
        case cmCapReceive: typeNameId = __h245(receiveUserInputCapability); break;
        case cmCapTransmit: typeNameId = __h245(transmitUserInputCapability); break;
        case cmCapReceiveAndTransmit: typeNameId = __h245(receiveAndTransmitUserInputCapability); break;
        default: break;
        }
        break;

    case cmCapConference:
        typeNameId = __h245(conferenceCapability);
        break;

    case cmCapH235:
#if (RV_H245_LEAN_H223 == RV_NO)
        typeNameId = __h245(h235SecurityCapability);
#else
        return RV_FALSE;
#endif
        break;

    case cmCapMaxPendingReplacementFor:
        /* this is just an integer. the capability must be copied as a whole. */
        if ((ret=pvtAddChilds(hVal, capId, hVal, capDataId)) <0) return ret;
        pvtDelete(hVal, dataTypeId);
        return RV_TRUE;

    case cmCapGeneric:
        typeNameId = __h245(genericControlCapability);
        break;

    case cmCapMultiplexedStream:
        switch (capability->direction)
        {
        case cmCapReceive: typeNameId = __h245(receiveMultiplexedStreamCapability); break;
        case cmCapTransmit: typeNameId = __h245(transmitMultiplexedStreamCapability); break;
        case cmCapReceiveAndTransmit: typeNameId = __h245(receiveAndTransmitMultiplexedStreamCapability); break;
        }
        break;

#if (RV_H245_LEAN_H223 == RV_NO)
    case cmCapAudioTelephonyEvent:
        typeNameId = __h245(receiveRTPAudioTelephonyEventCapability);
        break;

    case cmCapAudioTone:
        typeNameId = __h245(receiveRTPAudioToneCapability);
        break;

    case cmCapFEC:
        typeNameId = __h245(fecCapability);
        break;

    case cmCapMultiplePayloadStream:
        typeNameId = __h245(multiplePayloadStreamCapability);
        break;
#endif

    default: break;
    }

    if (typeNameId == LAST_TOKEN)
    {
        pvtDelete(hVal, dataTypeId);
        return RV_ERROR_UNKNOWN;
    }

    iTypeNameId = typeNameId;
    capsetDataId = pvtAdd(hVal, capId, iTypeNameId, 0, NULL, NULL);

    if (capsetDataId < 0)
        return capsetDataId ;
    if ((ret=pvtAddChilds(hVal, capsetDataId, hVal, capDataId)) < 0)
        return ret;

    pvtDelete(hVal, dataTypeId);
    return RV_TRUE;
}




int
capSetBuildFromStruct(
              /* Build capability table from capability structure array.
             - The capabilityId field is updated here.
             - if name != 0 then the configuration channel data definition is used.
             - if name == 0 and capabilityHandle >=0 then the specified data tree is used.
             - type and direction values shall be set.
             */
              IN  HPVT hVal,
              IN  int confRootId, /* configuration root id */
              OUT int termCapSetId, /* terminalCapabilitySet node id */
              IN  cmCapStruct** capabilities /* cap names array */
              )
{
  int capTableId, capEntryId, capId;
  int i,ret;

  if (!capabilities || termCapSetId<0) return RV_ERROR_UNKNOWN;

  capTableId = pvtAdd(hVal, termCapSetId, __h245(capabilityTable), 0, NULL, NULL);
  if (capTableId <0) return capTableId;

  for (i=1; capabilities[i-1]; i++) { /* loop all caps */
    if ( (capEntryId = pvtAdd(hVal, capTableId, 0, -556, NULL, NULL)) <0) return capEntryId;
    if ( (ret=pvtAdd(hVal, capEntryId, __h245(capabilityTableEntryNumber), i, NULL, NULL)) <0) return ret;
    capabilities[i-1]->capabilityId = i;
    if ( (capId = pvtAdd(hVal, capEntryId, __h245(capability), i, NULL, NULL)) <0) return capId;

    capStructBuildFromStruct(hVal, confRootId, capId, capabilities[i-1]);
  }

  return RV_TRUE;
}


int
capDescBuildFromStruct(
               /* build capability combinations from nested array.
              - The capabilityId shall be set to correct value, meaning
              this is called after capStructBuildFromStruct().
              */
               IN  HPVT hVal,
               OUT int termCapSetId, /* terminalCapabilitySet node id */
               IN  cmCapStruct*** capabilities[] /* cap names array */
               )
{
    int i, j, k;
    RvPvtNodeId capDescId, capDescEntryId, capSimId, capSimEntryId, tmpNodeId;

    if ((capabilities == NULL) || (termCapSetId < 0))
        return RV_ERROR_UNKNOWN;

    capDescId = pvtAdd(hVal, termCapSetId, __h245(capabilityDescriptors), 0, NULL, NULL);
    if (!RV_PVT_NODEID_IS_VALID(capDescId))
        return capDescId;

    for (i = 0; capabilities[i]; i++)
    {
        capDescEntryId = pvtAdd(hVal, capDescId, 0, -556, NULL, NULL);
        if (!RV_PVT_NODEID_IS_VALID(capDescEntryId))
            return capDescEntryId;

        tmpNodeId = pvtAdd(hVal, capDescEntryId, __h245(capabilityDescriptorNumber), i, NULL, NULL);
        if (!RV_PVT_NODEID_IS_VALID(tmpNodeId))
            return tmpNodeId;

        capSimId = pvtAdd(hVal, capDescEntryId, __h245(simultaneousCapabilities), 0, NULL, NULL);
        if (!RV_PVT_NODEID_IS_VALID(capSimId))
            return capSimId;

        for (j = 0; capabilities[i][j]; j++)
        {
            capSimEntryId = pvtAdd(hVal, capSimId, 0, -556, NULL, NULL);
            if (!RV_PVT_NODEID_IS_VALID(capSimEntryId))
                return capSimEntryId;

            for (k = 0; capabilities[i][j][k]; k++)
            {
                tmpNodeId = pvtAdd(hVal, capSimEntryId, 0, capabilities[i][j][k]->capabilityId, NULL, NULL);
                if (!RV_PVT_NODEID_IS_VALID(tmpNodeId))
                    return tmpNodeId;
            }
        }
    }

    return RV_TRUE;
}


#ifdef __cplusplus
}
#endif



