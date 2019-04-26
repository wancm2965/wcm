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
#include "cmH245GeneralDefs.h"
#include "cmControl.h"
#include "h245.h"
#include "cmictrl.h"
#include "cmH245Object.h"
#include "userinput.h"


#ifdef __cplusplus
extern "C" {
#endif

int userInput(H245Control* ctrl, int message)
{
    HCALL hsCall;
    H245Object *app;
    int nesting;

    hsCall = cmiGetByControl((HCONTROL)ctrl);
    app = (H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));

    if (app->cmMySessionEvent.cmEvCallUserInput != NULL)
    {
        HAPPCALL haCall = (HAPPCALL)emaGetApplicationHandle((EMAElement)hsCall);
        H245CBEnter((app->pAppObject, "cmEvCallUserInput(haCall=%p,hsCall=%p,id=%d)", haCall, hsCall, message));
        nesting=emaPrepareForCallback((EMAElement)hsCall);
        app->cmMySessionEvent.cmEvCallUserInput(haCall, hsCall, message);
        emaReturnFromCallback((EMAElement)hsCall, nesting);
        H245CBExit((app->pAppObject, "cmEvCallUserInput"));
    }

    return RV_TRUE;
}


RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputSupportIndicationBuild(
                 /* Build userUser message with alphanumeric data */
                 IN  HAPP hApp,
                 IN cmUserInputSupportIndication userInputSupport,
                 OUT int * nodeId  /* nodeId of nonstandard UserInputSupportIndication */
                 )
{
    static RvPstFieldId uisIds[] = {__h245(basicString), __h245(iA5String), __h245(generalString),
        __h245(hookflash), __h245(extendedAlphanumeric), __h245(encryptedBasicString),
        __h245(encryptedIA5String), __h245(encryptedGeneralString), __h245(secureDTMF)};

    HPVT       hVal;
    int        rootId,supId,ret=0;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (hApp == NULL)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputSupportIndicationBuild(hApp=%p,userInputSupport=%d)", hApp, userInputSupport));

    hVal = app->hVal;

    if (nodeId != NULL)
        *nodeId = RV_PVT_INVALID_NODEID;

    rootId = pvtAddRootByPath(hVal, app->hSynProtH245, "indication.userInput", 0, NULL);
    if (!RV_PVT_NODEID_IS_VALID(rootId))
    {
        H245APIExit((app->pAppObject, "cmUserInputSupportIndicationBuild=%d", rootId));
        return rootId;
    }

    supId = pvtAdd(hVal, rootId, __h245(userInputSupportIndication), 0, NULL, NULL);

    switch (userInputSupport)
    {
        case cmSupportNonStandard:
            ret = pvtAdd(hVal, supId, __h245(nonStandard), 0, NULL, NULL);
            if (nodeId != NULL)
                *nodeId = ret;
            break;

        case cmSupportBasicString:
        case cmSupportIA5String:
        case cmSupportGeneralString:
        case cmSupportDtmf:
        case cmSupportHookflash:
        case cmSupportExtendedAlphanumeric:
        case cmSupportEncryptedBasicString:
        case cmSupportEncryptedIA5String:
        case cmSupportEncryptedGeneralString:
        case cmSupportSecureDTMF:
            ret = pvtAdd(hVal, supId, uisIds[((int)userInputSupport) - 1], 0, NULL, NULL);
            break;
    }

    if (ret >= 0)
        ret = rootId;
    else
        pvtDelete(hVal, rootId);

    H245APIExit((app->pAppObject, "cmUserInputSupportIndicationBuild=%d", ret));
    return ret;
}


RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputSignalBuild(
                 /* Build userUser message with alphanumeric data */
                 IN  HAPP hApp,
                 cmUserInputSignalStruct *userInputSignalStruct
                 )
{
    HPVT       hVal;
    int        signalId,rtpId,rootId,ret=1;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputSignalBuild: hApp=%p", hApp));

    hVal = app->hVal;

    if ((rootId=pvtAddRootByPath(hVal, app->hSynProtH245, "indication.userInput", 0, NULL)) <0)
    {
        H245APIExit((app->pAppObject, "cmUserInputSignalBuild=%d", rootId));
        return rootId;
    }
    signalId = pvtAdd(hVal, rootId, __h245(signal), 0, NULL, NULL);
    ret=pvtAdd(hVal, signalId, __h245(signalType),1 ,(char *)&userInputSignalStruct->signalType, NULL);
    if (userInputSignalStruct->duration)
        ret=pvtAdd(hVal, signalId, __h245(duration),userInputSignalStruct->duration ,NULL, NULL);
    if (userInputSignalStruct->cmUserInputSignalRtp.logicalChannelNumber)
    {
        rtpId = pvtAdd(hVal, signalId, __h245(rtp),0 ,NULL, NULL);
        ret=pvtAdd(hVal, rtpId, __h245(logicalChannelNumber),userInputSignalStruct->cmUserInputSignalRtp.logicalChannelNumber ,NULL, NULL);
        if (userInputSignalStruct->cmUserInputSignalRtp.expirationTime)
            ret=pvtAdd(hVal, rtpId, __h245(expirationTime),userInputSignalStruct->cmUserInputSignalRtp.expirationTime ,NULL, NULL);
        if (userInputSignalStruct->cmUserInputSignalRtp.timestamp)
            ret=pvtAdd(hVal, rtpId, __h245(timestamp),userInputSignalStruct->cmUserInputSignalRtp.timestamp ,NULL, NULL);
    }

    if (ret >= 0)
        ret = rootId;

    H245APIExit((app->pAppObject, "cmUserInputSignalBuild=%d", ret));
    return ret;
}


RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputSignalUpdateBuild(
                 /* Build userUser message with alphanumeric data */
                 IN  HAPP hApp,
                 cmUserInputSignalStruct *userInputSignalStruct
                 )
{
    HPVT       hVal;
    int        rootId, ret = 0;
    int        signalId,rtpId;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputSignalUpdateBuild: hApp=%p", hApp));

    hVal = app->hVal;

    if ((rootId=pvtAddRootByPath(hVal, app->hSynProtH245, "indication.userInput", 0, NULL)) <0)
    {
        H245APIExit((app->pAppObject, "cmUserInputSignalUpdateBuild=%d", rootId));
        return rootId;
    }
    signalId = pvtAdd(hVal, rootId, __h245(signalUpdate), 0, NULL, NULL);
    if (userInputSignalStruct->duration > 0)
        ret = pvtAdd(hVal, signalId, __h245(duration), userInputSignalStruct->duration ,NULL, NULL);
    if (userInputSignalStruct->cmUserInputSignalRtp.logicalChannelNumber)
    {
        rtpId = pvtAdd(hVal, signalId, __h245(rtp),0 ,NULL, NULL);
        ret = pvtAdd(hVal, rtpId, __h245(logicalChannelNumber),userInputSignalStruct->cmUserInputSignalRtp.logicalChannelNumber ,NULL, NULL);
    }

    if (ret >= 0)
        ret = rootId;

    H245APIExit((app->pAppObject, "cmUserInputSignalUpdateBuild=%d", ret));
    return ret;
}




RVAPI int RVCALLCONV  /* RV_TRUE or negative value on failure */
cmUserInputGetDetail(
    IN  HAPP                    hApp,
    IN  RvInt32                 userInputId,
    OUT cmUserInputIndication*  userInputIndication)
{
    HPVT            hVal;
    RvPvtNodeId     nodeId;
    RvPstFieldId    fieldId;
    H245Object      *app = (H245Object*)cmiGetH245Handle(hApp);

    if (hApp == NULL)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputGetDetail(hApp=%p,UI id=%d)", hApp, userInputId));

    hVal = app->hVal;

    nodeId = pvtChild(hVal, userInputId);
    pvtGet(hVal, nodeId, &fieldId, NULL, NULL, NULL);

    switch (fieldId)
    {
        case __h245(nonStandard):
            *userInputIndication = cmUserInputNonStandard;
            break;

        case __h245(alphanumeric):
            *userInputIndication = cmUserInputAlphanumeric;
            break;

        case __h245(userInputSupportIndication):
            *userInputIndication = cmUserInputSupport;
            break;

        case __h245(signal):
            *userInputIndication = cmUserInputSignal;
            break;

        case __h245(signalUpdate):
            *userInputIndication = cmUserInputSignalUpdate;
            break;

        case __h245(extendedAlphanumeric):
            *userInputIndication = cmUserInputExtendedAlphanumeric;
            break;

#if (RV_H245_LEAN_H223 == RV_NO)
        case __h245(encryptedAlphanumeric):
            *userInputIndication = cmUserInputEncryptedAlphanumeric;
            break;
#endif

        case __h245(genericInformation):
            *userInputIndication = cmUserInputGenericInformation;
            break;

        default:
            break;
    }

#if (RV_LOGMASK & RV_LOGLEVEL_LEAVE)
    {
        char buff[30];
        pvtGet(hVal, nodeId, &fieldId, NULL, NULL, NULL);
        buff[0] = '\0';
        pstGetFieldName(pvtGetSynTree(hVal, userInputId), fieldId, sizeof(buff), buff);
        H245APIExit((app->pAppObject, "cmUserInputGetDetail: (%s,%d) = %d", buff, fieldId, nodeId));
    }
#endif

    return nodeId;
}

RVAPI int RVCALLCONV
cmUserInputGetSignal(
           IN  HAPP    hApp,
           IN  RvInt32 signalUserInputId,
           OUT cmUserInputSignalStruct * userInputSignalStruct
           )
{
    HPVT       hVal;
    int        nodeId,optId;
    RvInt32    length;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp ) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputGetSignal: hApp=%p, UI id=%d", hApp, signalUserInputId));
    hVal = app->hVal;

    nodeId=pvtGetChildByFieldId(hVal, signalUserInputId, __h245(signalType), &length, NULL);
    pvtGetString(hVal, nodeId, 1, (char *)&userInputSignalStruct->signalType);

    nodeId=pvtGetChild(hVal,signalUserInputId,__h245(duration), NULL);
    if(nodeId>=0)
    pvtGet(hVal,nodeId,NULL,NULL,&userInputSignalStruct->duration,NULL);
    else
    userInputSignalStruct->duration=0;
    nodeId = pvtGetChild(hVal,signalUserInputId,__h245(rtp),NULL);
    if (nodeId<0)
    userInputSignalStruct->cmUserInputSignalRtp.logicalChannelNumber=0;
    else
    {
        pvtGetChildByFieldId(hVal, nodeId, __h245(logicalChannelNumber),
                    &userInputSignalStruct->cmUserInputSignalRtp.logicalChannelNumber, NULL);
        optId = pvtGetChildByFieldId(hVal, nodeId, __h245(timestamp),
                    &userInputSignalStruct->cmUserInputSignalRtp.timestamp, NULL);
        if (optId<0)
          userInputSignalStruct->cmUserInputSignalRtp.timestamp =0;

        optId = pvtGetChildByFieldId(hVal, nodeId, __h245(expirationTime),
                    &userInputSignalStruct->cmUserInputSignalRtp.expirationTime, NULL);
        if (optId<0)
          userInputSignalStruct->cmUserInputSignalRtp.expirationTime =0;

    }
    H245APIExit((app->pAppObject, "cmUserInputGetSignal: hApp=%p", hApp));
    return RV_TRUE;
}

RVAPI int RVCALLCONV
cmUserInputGetSignalUpdate(
           IN  HAPP    hApp,
           IN  RvInt32 signalUserInputId,
         OUT cmUserInputSignalStruct * userInputSignalStruct
           )
{
    int        nodeId;
    HPVT       hVal;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp ) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputGetSignalUpdate: hApp=%p, UI id=%d", hApp, signalUserInputId));

    hVal = app->hVal;

    nodeId=pvtGetChild(hVal,signalUserInputId,__h245(duration), NULL);
    pvtGet(hVal,nodeId,NULL,NULL,&userInputSignalStruct->duration,NULL);
    nodeId = pvtGetChild(hVal,signalUserInputId,__h245(rtp), NULL);
    if (nodeId<0)
        userInputSignalStruct->cmUserInputSignalRtp.logicalChannelNumber=0;
    else
    pvtGetChildByFieldId(hVal, nodeId, __h245(logicalChannelNumber),
                &userInputSignalStruct->cmUserInputSignalRtp.logicalChannelNumber, NULL);

    H245APIExit((app->pAppObject, "cmUserInputGetSignalUpdate: hApp=%p", hApp));
    return RV_TRUE;
}


RVAPI int RVCALLCONV  /* return nodeId to make possible getting nonStandrd */
cmUserInputSupportGet(
    IN  HAPP                            hApp,
    IN  RvInt32                         supportUserInputId,
    OUT cmUserInputSupportIndication*   userInputSupportIndication)
{
    HPVT        hVal;
    RvPvtNodeId nodeId;
    H245Object  *app = (H245Object*)cmiGetH245Handle(hApp);

    if (hApp == NULL)
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputSupportGet(hApp=%p,UI id=%d)", hApp, supportUserInputId));

    hVal = app->hVal;

    nodeId = pvtChild(hVal, supportUserInputId);
    *userInputSupportIndication = (cmUserInputSupportIndication)(pvtGetSyntaxIndex(hVal, nodeId)-1);

#if (RV_LOGMASK & RV_LOGLEVEL_LEAVE)
    {
        RvPstFieldId fieldId;
        char buff[30];
        pvtGet(hVal, nodeId, &fieldId, NULL, NULL, NULL);
        buff[0] = '\0';
        pstGetFieldName(pvtGetSynTree(hVal, supportUserInputId), fieldId, sizeof(buff), buff);
        H245APIExit((app->pAppObject, "cmUserInputSupportGet: (%s,%d) hApp=%p", buff, fieldId));
    }
#endif

    if (*userInputSupportIndication == cmSupportNonStandard)
        return nodeId;
    else
        return (int)*userInputSupportIndication;
}


/* user input________________________________________________________________________________ */


RVAPI int RVCALLCONV
cmCallSendUserInput(
            /* send user input msg: userInputId tree is deleted */
            /* Note: Select one of nonStandard or userData options */
            IN  HCALL hsCall,
            IN  RvPvtNodeId userInputId /* indication.userInput tree node id */
            )
{
    RvPvtNodeId message;
    int res = RV_ERROR_DESTRUCTED;

    if (hsCall == NULL) return RV_ERROR_NULLPTR;

    if (emaLock((EMAElement)hsCall))
    {
        H245Object *app=(H245Object*)cmiGetH245Handle((HAPP)emaGetInstance((EMAElement)hsCall));
        H245Control* ctrl=(H245Control*)cmiGetControl(hsCall);
        HPVT        hVal;

        if (app == NULL) return RV_ERROR_NULLPTR;
        if (!RV_PVT_NODEID_IS_VALID(userInputId)) return RV_ERROR_UNKNOWN;

        H245APIEnter((app->pAppObject, "cmCallSendUserInput: hsCall=%p, id=%d", hsCall, userInputId));

        hVal = app->hVal;

        message = pvtAddRoot(hVal, app->hSynProtH245, 0, NULL);
        pvtMoveTree(hVal,pvtAddBranch2(hVal,message, __h245(indication), __h245(userInput)), userInputId);
        res = sendMessageH245((HCONTROL)ctrl, message, RV_TRUE, NULL);
        H245APIExit((app->pAppObject, "cmCallSendUserInput=%d", res));
        emaUnlock((EMAElement)hsCall);
    }

    return res;
}




RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputBuildNonStandard(
                /* Build userUser message with non-standard data */
                IN  HAPP hApp,
                IN  cmNonStandardIdentifier *identifier,
                IN  const RvChar *data,
                IN  int dataLength /* in bytes */
                )
{
    HPVT       hVal;
    int        rootId, nsId,ret;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;
    if (!identifier) return RV_ERROR_UNKNOWN;
    if (!data || dataLength<1) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputBuildNonStandard: hApp=%p, id=%p, data=%p (%d)",
        hApp, identifier, data, dataLength));

    hVal = app->hVal;

    if ((rootId=pvtAddRootByPath(hVal, app->hSynProtH245, (char*)"indication.userInput", 0, NULL)) <0)
    {
        H245APIExit((app->pAppObject, "cmUserInputBuildNonStandard=-1"));
        return rootId;
    }
    nsId = pvtBuildByPath(hVal, rootId, "nonStandard", 0, NULL);

    if ((ret=cmNonStandardParameterCreate(hVal, nsId, identifier, data, dataLength)) <0)
    {
        pvtDelete(hVal, rootId);
        H245APIExit((app->pAppObject, "cmUserInputBuildNonStandard=%d", ret));
        return ret;
    }

    H245APIExit((app->pAppObject, "cmUserInputBuildNonStandard=%d", rootId));
    return rootId;
}




RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputBuildAlphanumeric(
                 /* Build userUser message with alphanumeric data */
                 IN  HAPP            hApp,
                 IN  cmUserInputData *userData
                 )
{
    HPVT       hVal;
    int        rootId,ret;
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;
    if (!userData) return RV_ERROR_UNKNOWN;
    if (!userData->data || userData->length<1) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputBuildAlphanumeric: hApp=%p, userData=%p", hApp, userData));

    hVal = app->hVal;

    if ((rootId=pvtAddRootByPath(hVal, app->hSynProtH245, "indication.userInput", 0, NULL)) <0)
    {
        H245APIExit((app->pAppObject, "cmUserInputBuildAlphanumeric=%d", rootId));
        return rootId;
    }
    ret = pvtAdd(hVal, rootId, __h245(alphanumeric), userData->length, userData->data, NULL);

    if (ret >= 0)
        ret = rootId;
    else
        pvtDelete(hVal, rootId);

    H245APIExit((app->pAppObject, "cmUserInputBuildAlphanumeric=%d", ret));
    return ret;
}



/******************************************************************************
 * cmUserInputGet
 * ----------------------------------------------------------------------------
 * General: Creates a string of user data from data stored in a sub-tree.
 *          The data can be standard or Non standard.
 *
 * Return Value: If an error occurs, the function returns a negative value. 
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - The Stack handle for the application.
 *         userInputId  - The PVT node ID of the userInputIndicator sub-tree
 *                        that contains the user data.
 *         identifier   - The vendor and type of the non-standard information.
 *         dataLength   - The length of the data buffer in bytes.
 * Output: data         - Pointer to the data buffer for non-standard data.
 *         dataLength   - 0 for standard data, or the length of buffer for
 *                        non-standard data.
 *         userData     - Pointer to cmUserInputData.
 *****************************************************************************/  
RVAPI int RVCALLCONV
cmUserInputGet(
    IN    HAPP                      hApp,
    IN    RvPvtNodeId               userInputId,
    IN    cmNonStandardIdentifier   *identifier,
    OUT   RvChar                    *data,
    INOUT RvInt32                   *dataLength,
    OUT   cmUserInputData           *userData)
{
    HPVT        hVal;
    RvPvtNodeId nsId;
    int         userDataLen;
    H245Object  *app = (H245Object*)cmiGetH245Handle(hApp);

    if ((hApp == NULL) || (dataLength == NULL))
        return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputGet: hApp=%p, UI id=%d", hApp, userInputId));

    hVal = app->hVal;

    if ((identifier != NULL) && (data != NULL))
    {
        nsId = pvtGetChild(hVal, userInputId, __h245(nonStandard), NULL);
        if (RV_PVT_NODEID_IS_VALID(nsId))
        {
            cmNonStandardParameterGet(hVal, nsId, identifier, data, dataLength);
            userData->length = 0;
            H245APIExit((app->pAppObject, "cmUserInputGet: [1] (nonStandard)"));
            return RV_TRUE;
        }
    }

    if (userData != NULL)
    {
        userDataLen = userData->length;
        nsId = pvtGetChildByFieldId(hVal, userInputId, __h245(alphanumeric), &(userData->length), NULL);
        if (RV_PVT_NODEID_IS_VALID(nsId))
        {
            pvtGetString(hVal, nsId, userDataLen - 1, userData->data);
            *dataLength = 0; /* Indicate a standard user input */
            H245APIExit((app->pAppObject, "cmUserInputGet: [1] (alphanumeric)"));
            return RV_TRUE;
        }
    }

    H245APIExit((app->pAppObject, "cmUserInputGet: [-1] (unknown/unsupported)"));
    return RV_ERROR_UNKNOWN;
}

/******************************************************************************
 * cmUserInputGetGenericInformation
 * ----------------------------------------------------------------------------
 * General: Get the genericInformation node of the userInputIndication message..
 *
 * Return Value: RV_OK on success 
 *				 Negative value on error.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp                 - The Stack handle for the application. 
 *		   userInputId			- The userInput message PVT node.
 * Output: genericInformation   - The PVT node ID of the genericInformation sub-tree
 *                                to be added to the message.
 *****************************************************************************/ 
RVAPI RvStatus RVCALLCONV cmUserInputGetGenericInformation(
	   IN  HAPP            hApp,
       IN  RvPvtNodeId     userInputId,
	   OUT RvPvtNodeId	   *genericInformationNodeId)
{
    HPVT        hVal;
    RvPvtNodeId genInfoId = RV_PVT_INVALID_NODEID;
    
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_ERROR_UNKNOWN;

    H245APIEnter((app->pAppObject, "cmUserInputGetGenericInformation: hApp=%p", hApp));

    hVal = app->hVal;

    __pvtGetByFieldIds(genInfoId, hVal, userInputId, { _h245(genericInformation) LAST_TOKEN}, NULL, NULL, NULL);

    
    *genericInformationNodeId = genInfoId;


    H245APIExit((app->pAppObject, "cmUserInputGetGenericInformation=%d", RV_OK));
    return RV_OK;
}

/******************************************************************************
 * cmUserInputBuildWithGenericInformation
 * ----------------------------------------------------------------------------
 * General: Create a userInputIndication message tree and add the 
 *          genericInformation node supplied by the user.
 *
 * Return Value: userInputIndication message PVT node ID on success.
 *               RV_PVT_INVALID_NODE_ID on failure. 
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp                 - The Stack handle for the application.
 *         genericInformation   - The PVT node ID of the genericInformation sub-tree
 *                                to be added to the message (The user's node is deleted).
 * Output: None
 *****************************************************************************/ 
RVAPI RvPvtNodeId RVCALLCONV cmUserInputBuildWithGenericInformation(
    IN  HAPP            hApp,
    IN  RvPvtNodeId     genericInformation)
{
    HPVT        hVal;
    RvPvtNodeId rootId = RV_PVT_INVALID_NODEID, tmpNodeId;
    
    H245Object *app = (H245Object*)cmiGetH245Handle(hApp);

    if (!hApp) return RV_PVT_INVALID_NODEID;

    H245APIEnter((app->pAppObject, "cmUserInputBuildWithGenericInformation: hApp=%p", hApp));

    hVal = app->hVal;

    if ((rootId = pvtAddRootByPath(hVal, app->hSynProtH245, "indication.userInput", 0, NULL)) <0)
    {
        H245APIExit((app->pAppObject, "cmUserInputBuildWithGenericInformation=%d", rootId));
        return rootId;
    }

    tmpNodeId = pvtAdd(hVal, rootId, __h245(genericInformation), 0, NULL, NULL);

    if (tmpNodeId < 0)
    {
        pvtDelete(hVal, rootId);
    }

    /* Add entry */
    tmpNodeId = pvtAdd(hVal, tmpNodeId, 0, 0, NULL, NULL);

    if (tmpNodeId < 0)
    {
        pvtDelete(hVal, rootId);
    }


    pvtMoveTree(hVal, tmpNodeId, genericInformation);

    H245APIExit((app->pAppObject, "cmUserInputBuildWithGenericInformation=%d", rootId));
    return rootId;
}


#ifdef __cplusplus
}
#endif



