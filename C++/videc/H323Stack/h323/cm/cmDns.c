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

/***********************************************************************
cmDns.c

The DNS module of cm.
This module is responsible for dns resolutions. 
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/

#include "ra.h"
#include "cm.h"
#include "cmintr.h"
#include "cmutils.h"
#include "cmDns.h"
#include "rvselect.h"
#include "commonasn.h"
#include "rasdef.h"
#include "rvares.h"




#ifdef __cplusplus
extern "C" {
#endif



/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/
/******************************Local Functions*********************************/

/************************************************************************
 * readDnsParams
 * purpose: Reads all dns module configurable parameters from configuration tree.
 * input  : app         - Stack instance handle
 * output : maxDnsTx
 *          queryBuffSize
 *          dnsTimeOutInSec
 *          dnsNumTries
 *          dnsReadBuffSize
 *          dnsNunDomains
 *          dnsNumServers  - dns paramaters.
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
static void readDnsParams(IN  cmElem* app,
                          OUT RvUint32* maxDnsTx ,
                          OUT RvUint32* queryBuffSize,
                          OUT RvInt* dnsTimeOutInSec,
                          OUT RvInt* dnsNumTries,
                          OUT RvUint32* dnsReadBuffSize ,
                          OUT RvUint32* dnsNunDomains,
                          OUT RvUint32* dnsNumServers )
{
    ciGetValue(app->hCfg,"system.allocations.dnsMaxQueries" ,NULL,(int*)maxDnsTx);
    ciGetValue(app->hCfg,"system.allocations.dnsQueryBuffSize", NULL, (int*)queryBuffSize);
    ciGetValue(app->hCfg,"system.allocations.dnsMaxBuffSize" ,NULL,(int*)dnsReadBuffSize);
    ciGetValue(app->hCfg,"system.dnsMaxServers" ,NULL, (int*)dnsNumServers);
    ciGetValue(app->hCfg,"system.dnsMaxDomains", NULL, (int*)dnsNunDomains); 
    ciGetValue(app->hCfg, "system.tempSystemParams.dnsTimeOut", NULL, dnsTimeOutInSec);
    ciGetValue(app->hCfg, "system.tempSystemParams.dnsRetries", NULL, dnsNumTries);

}
/*-----------------------------------------------------------------------*/
static RvDnsQueryType cmDnsQueryType2RvDnsQueryType(cmDnsQueryType queryType)
{
    switch(queryType){
    case cmDnsQueryTypeIpv4:
        return RV_DNS_HOST_IPV4_TYPE;
    case cmDnsQueryTypeIpv6:
        return RV_DNS_HOST_IPV6_TYPE;
    case cmDnsQueryTypeSrvIpv4:
    case cmDnsQueryTypeSrvIpv6:
        return RV_DNS_SRV_TYPE;
    default:
        return (RvDnsQueryType)0;
    }
}
/*-----------------------------------------------------------------------*/
/******************************************************************************
 * callCBremoveTx
 * ----------------------------------------------------------------------------
 * General: calls the application call back, updates numChildTx of parent if exsist
 *          and removes the transaction from RA (and optionaly the parent).
 *          This function is not thread safe. The dns module should be locked
 *          prior to calling it.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  app              - Stack handle 
 *         dnsTx            - dns transaction.
 *         status           - with whitch the answer on the query was received. 
 * Output: None.
 *****************************************************************************/
static void callCBremoveTx(cmElem* app, cmDnsTxElem* dnsTx, RvStatus status)
{
    dnsModule* dns = (dnsModule*)app->dnsManager;
    RvBool isLastTx = RV_FALSE;
    RvBool* txContinue = &dnsTx->txContinue;

    if (dns == NULL) return;


    if (dnsTx->parentDnsTx != NULL )
        txContinue = &(dnsTx->parentDnsTx->txContinue);

    
    /*check if last transaction of a request*/
    if (dnsTx->parentDnsTx == NULL || dnsTx->parentDnsTx->numChildTx == 1)
        isLastTx = RV_TRUE;
    
    if (*txContinue == RV_TRUE){
        /*call application callback*/
        dnsTx->cmEvDnsQueryResult((HAPP)app,
                                  dnsTx->context,
                                  status,
                                  dnsTx->srvData.priority,
                                  dnsTx->srvData.weight,
                                  dnsTx->numOfAddresses,
                                  isLastTx,
                                  txContinue);
    }

    if (dnsTx->parentDnsTx != NULL){
        /*decrease num of children*/
        dnsTx->parentDnsTx->numChildTx--;
        if (dnsTx->parentDnsTx->numChildTx == 0)
        {
            /*remove parent*/
            raDelete(dns->dnsQueryBuffs, (RAElement)(dnsTx->parentDnsTx->queryBuffer));
            raDelete(dns->dnsTxs, (RAElement)(dnsTx->parentDnsTx));
        }
    }
    /*remove transaction*/
    raDelete(dns->dnsQueryBuffs, (RAElement)(dnsTx->queryBuffer));
    raDelete(dns->dnsTxs, (RAElement)dnsTx);
}
/*-----------------------------------------------------------------------*/
/******************************************************************************
 * cmDnsInitNewTx
 * ----------------------------------------------------------------------------
 * General: initialize a query received from application.
 *          This function is not thread safe. The dns module should be locked
 *          prior to calling it.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  app              - Stack handle  
 * Output: None.
 *****************************************************************************/
static RvStatus cmDnsInitNewTx(IN cmElem*               app,
                               IN void*                 appContext,
                               IN cmDnsQueryType        queryType,
                               IN const RvChar*         dnsName,
                               IN RvUint16              dnsPort,
                               IN cmEvDnsQueryResultT   cmEvDnsQueryResult,
                               IN cmTransportAddress*   dnsResBuf,
                               IN RvUint16              numOfElems,
                               IN cmDnsTxElem*          ParentTx,
                               IN RvDnsData*            txDnsData,
                               OUT cmDnsTxElem**        dnsTx)
{
    dnsModule* dns = (dnsModule*)app->dnsManager;
    cmDnsTxElem* newElem;
    RvInt reqQueryBuffSize = dns->queryBuffSize;
    RvStatus status = RV_OK;
    int location;

    if (dns == NULL)
        return RV_ERROR_UNKNOWN;

    /*add transaction*/
    location = raAdd(dns->dnsTxs, (RAElement*)&newElem);

    /*set values*/
    newElem->dnsContext.app = app;
    newElem->dnsContext.raElemLocation = location;
    raAdd(dns->dnsQueryBuffs, (RAElement*)&newElem->queryBuffer);
    newElem->parentDnsTx = ParentTx; /*parent transaction if exsist*/
    newElem->numChildTx = 0; 
    newElem->context = appContext; /*the context of the application*/
    newElem->cmEvDnsQueryResult = cmEvDnsQueryResult; /*application callback*/
    newElem->txContinue = RV_TRUE;
    newElem->queryType = queryType;
    newElem->addressBuffer = dnsResBuf;
    newElem->addrBuffSize = numOfElems;
    newElem->numOfAddresses = 0;
    newElem->srvData.port = dnsPort;
    newElem->srvData.priority = 0;
    newElem->srvData.weight = 0;
    if (txDnsData != NULL)
    {
        newElem->srvData.port = txDnsData->data.dnsSrvData.port;
        newElem->srvData.priority = txDnsData->data.dnsSrvData.priority;
        newElem->srvData.weight = txDnsData->data.dnsSrvData.weight;
    }
    /*send query*/
    status = RvAresSendQuery(&dns->dnsEngine,
                             cmDnsQueryType2RvDnsQueryType(queryType),
                             dnsName,
                             RV_FALSE,
                             (void*)newElem->queryBuffer,
                             (RvInt*)(&reqQueryBuffSize),
                             &newElem->dnsContext,
                             &newElem->queryId);
    if ( reqQueryBuffSize > dns->queryBuffSize || status != RV_OK)
    {
        RvLogInfo(&dns->log, (&dns->log, "Unable to Send New Query (status=%d)", status));
        /*remove transaction*/
        if (newElem->parentDnsTx != NULL)
            newElem->parentDnsTx->numChildTx--;
        raDelete(dns->dnsQueryBuffs, (RAElement)newElem->queryBuffer);
        raDelete(dns->dnsTxs, (RAElement)newElem);
    }
    else
    { 
        if (newElem->parentDnsTx != NULL)
            newElem->parentDnsTx->numChildTx++;
        *dnsTx = newElem;
    }

    return status;
    
}
/*-----------------------------------------------------------------------*/
static void freeDomainList(IN RvChar** domainList,
                    IN RvUint16 numDomains,
                    IN RvLogMgr* logMgr)
{
    int index = 0;
    while(domainList[index] != NULL && index < numDomains){
        RvMemoryFree(domainList[index++], logMgr);
    }
    RvMemoryFree(domainList, logMgr);

}
/*-----------------------------------------------------------------------*/
/*create server list - updates the real number of configured servers*/
static RvAddress* dnsCreateServerList(IN cmElem* app,
                                      IN RvAddress** serverList, 
                                      IN RvUint16 numServers)
{
    RvUint16 index = 0;
    RvPvtNodeId nodeId;
    cmTransportAddress tmpAddr;
    RvSize_t size = sizeof(RvAddress) * numServers;
    dnsModule* dns = (dnsModule*)app->dnsManager;

    __pvtGetNodeIdByFieldIds(nodeId, app->hVal, app->sysConf, {_common(tempSystemParams) _common(dnsServers) LAST_TOKEN});
    if (RV_PVT_NODEID_IS_VALID(nodeId))
    {
        nodeId = pvtChild(app->hVal, nodeId);
        if (RV_PVT_NODEID_IS_VALID(nodeId))
        {
            if (RvMemoryAlloc(NULL, size, dns->logMgr, (void**)serverList) != RV_OK)
            return NULL;
            memset(*serverList, 0, size);
        
            while (RV_PVT_NODEID_IS_VALID(nodeId) && index < numServers)
            {
                cmVtToTA(app->hVal, nodeId, &tmpAddr);
                RvH323CmToCoreAddress(&tmpAddr, (*serverList + index++));
                nodeId = pvtBrother(app->hVal, nodeId);
            
            }
        }
        
    }
    dns->dnsNumServers = index;
    return *serverList;
}
/*-----------------------------------------------------------------------*/
/*create domain list - updates the real number of configured domains*/
static RvChar** dnsCreateDomaiList(IN cmElem* app,
                                   IN RvChar*** domainList, 
                                   IN RvUint16 numDomains)
{
    RvUint16 index = 0;
    RvChar currStr[MAX_DOMAIN_LEN];
    RvPvtNodeId nodeId;
    RvSize_t size = sizeof(RvChar*) * numDomains;
    dnsModule* dns = (dnsModule*)app->dnsManager;

    __pvtGetNodeIdByFieldIds(nodeId, app->hVal, app->sysConf, {_common(tempSystemParams) _common(dnsDomains) LAST_TOKEN});
    if (RV_PVT_NODEID_IS_VALID(nodeId))
    {
        nodeId = pvtChild(app->hVal, nodeId);
        if (RV_PVT_NODEID_IS_VALID(nodeId))
        {
            if (RvMemoryAlloc(NULL, size, dns->logMgr, (void**)domainList) != RV_OK)
                return NULL;
            memset(*domainList, 0, size);
            while (RV_PVT_NODEID_IS_VALID(nodeId) && index < numDomains)
            {
                memset(currStr, 0, sizeof(currStr));
                pvtGetString(app->hVal, nodeId, sizeof(currStr), currStr);
                RvMemoryAlloc(NULL, strlen(currStr) + 1, dns->logMgr, (void**)((*domainList)+index));
                memset((*domainList)[index], 0, strlen(currStr) + 1);
                memcpy((*domainList)[index++], currStr, strlen(currStr) );
                nodeId = pvtBrother(app->hVal, nodeId);
            }   
        }
        
    }
    dns->dnsNunDomains = index;
    return *domainList;
}
/*-----------------------------------------------------------------------*/
/*callback for ares*/
static RvStatus newRecordCB(
              IN  void*               context,
              IN  RvUint32            queryId,
	          IN  RvDnsData*          dnsData)

{
    dnsRAElemContext tx = *((dnsRAElemContext*)context);
    cmElem* app = tx.app;
    dnsModule* dns = (dnsModule*)app->dnsManager;
    cmDnsTxElem* dnsTx, *newTx ;
    cmTransportAddress addr;
    cmDnsQueryType queryType = cmDnsQueryTypeIpv4; /*default*/
    RvStatus status = RV_OK;
    
    RV_UNUSED_ARG(queryId);

    /*lock the dns module*/
    RvLockGet(&dns->lock, dns->logMgr);

    /*get the right transaction*/
    dnsTx = (cmDnsTxElem*) raGet(dns->dnsTxs, tx.raElemLocation);

    if (dnsTx == NULL)
    {
        RvLockRelease(&dns->lock, dns->logMgr);
        return RV_ERROR_UNKNOWN;
    }
 
    switch(dnsData->dataType){
    case RV_DNS_STATUS_TYPE:
        RvLogDebug(&dns->log, (&dns->log, "newRecordCB called with RV_DNS_STATUS_TYPE"));
        /*call application callback and remove transaction*/
        callCBremoveTx(app, dnsTx, dnsData->data.status);
        break;   
    case RV_DNS_HOST_IPV4_TYPE:
	case RV_DNS_HOST_IPV6_TYPE:
        RvLogDebug(&dns->log, (&dns->log, "newRecordCB called with RV_DNS_HOST_IPV4/6_TYPE"));
        /*put address in buffer*/
        if (dnsTx->numOfAddresses < dnsTx->addrBuffSize )
        {
            RvH323CoreToCmAddress(&(dnsData->data.hostAddress), &addr);
            if(dnsTx->srvData.port)
                addr.port = dnsTx->srvData.port;
            else
                addr.port = 1719;
            dnsTx->addressBuffer[dnsTx->numOfAddresses++] = addr;
        }else{
            /*return error so the ares will not call the
            newItenCB for new records again (will call with endoflist) - 
            not calling application callback, will be called with endoflist*/
            RvLogInfo(&dns->log, (&dns->log, "DNS data: Application Address Buffer Full"));
            RvLockRelease(&dns->lock, dns->logMgr);
            return RV_ERROR_UNKNOWN;
        }
        break;
    case RV_DNS_SRV_TYPE:
        RvLogDebug(&dns->log, (&dns->log, "newRecordCB called with RV_DNS_SRV_TYPE"));
        /*decide what type the 'derived' transactions have*/
        switch(dnsTx->queryType){
            case cmDnsQueryTypeSrvIpv4:
                queryType = cmDnsQueryTypeIpv4;
                break;
            case cmDnsQueryTypeSrvIpv6:
                queryType = cmDnsQueryTypeIpv6;
                break;
            case cmDnsQueryTypeIpv4:
            case cmDnsQueryTypeIpv6:
            default:
                /*will never happen*/
                break;
        }

        /*create the derived transaction*/
        status = cmDnsInitNewTx(app,
                                dnsTx->context,
                                queryType,
                                dnsData->data.dnsSrvData.targetName,
                                dnsData->data.dnsSrvData.port,
                                dnsTx->cmEvDnsQueryResult,
                                dnsTx->addressBuffer,
                                dnsTx->addrBuffSize,
                                dnsTx,
                                dnsData,
                                &newTx);
        if (status != RV_OK)
        {
            RvLogInfo(&dns->log, (&dns->log, "cmDnsInitNewTx: queryType=%d, dns name= %s failed(status=%d",
            queryType,dnsData->data.dnsSrvData.targetName,status));
            /*callback must return RV_OK otherwise ares will stop send results of query*/
            status = RV_OK;
        }
        
        break;
    case RV_DNS_ENDOFLIST_TYPE:
        RvLogDebug(&dns->log, (&dns->log, "newRecordCB called with RV_DNS_ENDOFLIST_TYPE"));
        /*if not srv request call application callback and remove transaction*/
        if (dnsTx->queryType != cmDnsQueryTypeSrvIpv4 && dnsTx->queryType != cmDnsQueryTypeSrvIpv6)
            callCBremoveTx(app, dnsTx, RV_OK);
        /*if its a service and no children created - remove the transaction*/
        else if (dnsTx->numChildTx == 0)
        {
            raDelete(dns->dnsQueryBuffs, (RAElement)dnsTx->queryBuffer);
            raDelete(dns->dnsTxs, (RAElement)dnsTx);
        }
        break;
    default:
        break;
    }

    RvLockRelease(&dns->lock, dns->logMgr);
    return status;
}

/******************************************************************************
 * dnsConstruct
 * ----------------------------------------------------------------------------
 * General: create the DNS module.
 *
 * Return Value: Handle to dns module on success NULL on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input: hApp     -   Stack's handle for the application.
 * Output: none
 *****************************************************************************/

RVHDNS dnsConstruct(IN HAPP        hApp )
{
    dnsModule* dns;
    cmElem* app = (cmElem*)hApp;
    RvLogMgr* logMgr = app->logMgr;
    /*dns default parameters*/
    RvUint32 maxDnsTx           = 20;
    RvUint32 queryBuffSize      = 512;
    RvInt dnsTimeOutInSec = -1;
    RvInt dnsNumTries = -1;
    RvUint32 dnsReadBuffSize = 2*1024;
    RvUint32 dnsNunDomains = 3;
    RvUint32 dnsNumServers = 3;


    if (app == NULL )
        return NULL;

    /*if dns supported*/
    if (ciGetValue(app->hCfg,"system.dnsSupported",NULL,NULL) == ERR_CI_NOERROR)
    {
        /* Allocate the DNS module */
        if (RvMemoryAlloc(NULL, sizeof(dnsModule), logMgr, (void**)&dns) != RV_OK)
            return NULL;

        memset(dns, 0, sizeof(dnsModule));
        /*read all dns parameters*/
        readDnsParams(app,
                      &maxDnsTx,
                      &queryBuffSize,
                      &dnsTimeOutInSec,
                      &dnsNumTries,
                      &dnsReadBuffSize,
                      &dnsNunDomains,
                      &dnsNumServers);

   
        /*set dns module parameters*/
        dns->dnsTimeOutInSec = dnsTimeOutInSec;
        dns->dnsNumTries = dnsNumTries;
        dns->dnsReadBuffSize = (RvInt16)dnsReadBuffSize;
        dns->dnsNunDomains = (RvInt16)dnsNunDomains;
        dns->dnsNumServers = (RvInt16)dnsNumServers,
        dns->queryBuffSize = (RvInt16)queryBuffSize;

        dns->logMgr = logMgr;

        RvLogSourceConstruct(logMgr, &dns->log, "CMDNS", "CM DNS Module");

        /* Allocate the transaction DB */
        dns->dnsTxs =
            raConstruct(sizeof(cmDnsTxElem), (int)maxDnsTx, RV_TRUE, "DNS Transactions", logMgr);
        if (dns->dnsTxs == NULL)
            return NULL;

        /*Allocate query buffers*/
        dns->dnsQueryBuffs = 
            raConstruct(queryBuffSize, (int)maxDnsTx, RV_TRUE, "DNS Query Buffers", logMgr);
        if (dns->dnsQueryBuffs == NULL)
            return NULL;

        /* Allocate a mutex */
        RvLockConstruct(logMgr, &dns->lock);
        RvLogInfo(&dns->log,
            (&dns->log, "CM DNS Module Constructed"));

        return (RVHDNS)dns;
    }
    return NULL;
}


/******************************************************************************
 * cmDnsStart
 * ----------------------------------------------------------------------------
 * General: Start the DNS module.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  app              - Stack handle        
 * Output: none
 *****************************************************************************/
RvStatus cmDnsStart(IN cmElem *app, RvBool restart)
{

    dnsModule* dns = (dnsModule*)app->dnsManager;
    RvAddress* serverList = NULL;
    RvChar** domainList = NULL;
    RvSelectEngine* selectEngine;
    RvStatus status = RV_OK;
    RvInt32 dnsNunDomains;
    RvInt32 dnsNumServers;

    if (app == NULL || dns == NULL)
        return RV_ERROR_UNKNOWN;

    if (!restart)
    {
         dnsNumServers = dns->dnsNumServers;
         dnsNunDomains = dns->dnsNunDomains;
        /*get the select engine*/
        status = RvSelectGetThreadEngine(dns->logMgr, &selectEngine);

        if (status != RV_OK)
            return RV_ERROR_UNKNOWN;
        /*construct dns resolver engine*/
        status = RvAresConstruct(selectEngine, 
                                 newRecordCB,
                                 dnsNumServers,/*watchout - inout*/ 
                                 dnsNunDomains ,/*watchout - inout*/  
                                 dns->dnsReadBuffSize,
                                 dns->logMgr, 
                                 &dns->dnsEngine);
    }
    else
    {
        /*if restart - read the max number of servers and domains again*/
        ciGetValue(app->hCfg,"system.dnsMaxServers" ,NULL, &dnsNumServers );
        ciGetValue(app->hCfg,"system.dnsMaxDomains", NULL, &dnsNunDomains);
        dns->dnsNumServers = (RvUint16)dnsNumServers;
        dns->dnsNunDomains = (RvUint16)dnsNunDomains;
    }

    /*create the list of servers & get actual number of servers*/
    dnsCreateServerList(app, &serverList, dns->dnsNumServers);
    /*create the list of domains & get actual number of domains*/
    dnsCreateDomaiList(app, &domainList, dns->dnsNunDomains);

    if (status == RV_OK)
    {
        status = RvAresSetParams(&dns->dnsEngine, 
                                 dns->dnsTimeOutInSec, 
                                 dns->dnsNumTries,
                                 serverList,
                                 dns->dnsNumServers,
                                 domainList,
                                 dns->dnsNunDomains);
        if (serverList != NULL)
            RvMemoryFree(serverList, dns->logMgr);
        if (domainList != NULL)
            freeDomainList(domainList, dns->dnsNunDomains, dns->logMgr);
        RvLogInfo(&dns->log, (&dns->log, "cmDns Module Started( status = %d)", status));
    }
    else
    {
        cmDnsDestruct((HAPP)app);
        RvLogInfo(&dns->log, (&dns->log, "cmDns Module Not Started( status = %d)", status));
    }
    
    return status;
}


/******************************************************************************
 * cmDnsDestruct
 * ----------------------------------------------------------------------------
 * General: Destruct the DNS module.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hDns               - DNS handle.
 * Output: None.
 *****************************************************************************/
void cmDnsDestruct(IN HAPP  hApp)
{
    cmElem* app = (cmElem*)hApp;
    dnsModule* dns = (dnsModule*)app->dnsManager;

    if (dns != NULL){
        /*stop all active Txs and clean dns module DB*/
        cmDnsClear(app);

        /*free resources*/
        if (dns->dnsTxs != NULL)
            raDestruct(dns->dnsTxs);

        if (dns->dnsQueryBuffs != NULL)
            raDestruct(dns->dnsQueryBuffs);
        
        RvAresDestruct(&dns->dnsEngine);
        RvLockDestruct(&dns->lock, dns->logMgr);
        RvLogSourceDestruct(&dns->log);
        RvMemoryFree(dns, dns->logMgr);

        dns = NULL;
    }
}

/******************************************************************************
 * cmDnsClear
 * ----------------------------------------------------------------------------
 * General: Stops all active transactions and cleans the DNS DB..
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  app              - Stack handle  
 * Output: None.
 *****************************************************************************/
void cmDnsClear(IN cmElem* app)
{
    dnsModule* dns = (dnsModule*)app->dnsManager;
    cmDnsTxElem* dnsTx;
    int currLocation = -1;
    

    if (dns != NULL){

        RvLockGet(&dns->lock, dns->logMgr);

        currLocation = raGetNext(dns->dnsTxs, currLocation);
        if(currLocation >= 0)
        {
            dnsTx = (cmDnsTxElem*)raGet(dns->dnsTxs, currLocation);

            /*cancel all transactions*/
            while(dnsTx != NULL)
            { 
                RvAresCancelQuery(&dns->dnsEngine, dnsTx->queryId);
                currLocation = raGetNext(dns->dnsTxs, currLocation);
                dnsTx = (cmDnsTxElem*)raGet(dns->dnsTxs, currLocation);
            } 
        }

        /*remove all transaction*/
        if (dns->dnsTxs != NULL)
            raClear(dns->dnsTxs);
        if (dns->dnsQueryBuffs != NULL)
            raClear(dns->dnsQueryBuffs);
        RvLockRelease(&dns->lock, dns->logMgr);

        RvLogInfo(&dns->log, (&dns->log, "cmDns Module Cleared."));
    }
}
/******************************************************************************
 * cmDnsAddressRequest
 * ----------------------------------------------------------------------------
 * General: Request a DNS address resolution of a name or a service.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp               - The stack's application handle.
 *         context            - The application context.
 *         queryType          - ipv4/ipv6/srv.
 *         dnsName            - The dns name to be resolved.
 *         dnsPort            - The gk port number (none for default)
 *         cmEvDnsQueryResult - The handle of the application callback.
 *         dnsResBuf          - The address buffer of the application.
 *         numOfElems         - The number of cmTransportAddress elements that can be 
 *                              put in dnsResBuf.
 * Output: none                             
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmDnsAddressRequest(
       IN      HAPP                hApp,
       IN      void*               context,
       IN      cmDnsQueryType      queryType,
       IN      const RvChar*       dnsName,
       IN      RvUint16            dnsPort,
       IN      cmEvDnsQueryResultT cmEvDnsQueryResult,
       INOUT   cmTransportAddress* dnsResBuf,
       IN      RvUint16            numOfElems)
{
    cmElem* app = (cmElem*)hApp;
    dnsModule* dns = (dnsModule*)app->dnsManager;
    cmDnsTxElem* dnsTx;
    int status;

    RvLockGet(&dns->lock, dns->logMgr);

    status = (int)(cmDnsInitNewTx(app,
                                  context,
                                  queryType,
                                  dnsName,
                                  dnsPort,
                                  cmEvDnsQueryResult,
                                  dnsResBuf,
                                  numOfElems,
                                  NULL,
                                  NULL,
                                  &dnsTx));
    RvLockRelease(&dns->lock, dns->logMgr);
    return status;

}


/******************************************************************************
 * cmRestartDnsModule
 * ----------------------------------------------------------------------------
 * General: restart of the DNS module.
 *          Will cause all active transactions to be canceled.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  app               - The stack's element.
 * Output: none                             
 *****************************************************************************/
RvStatus cmRestartDnsModule(
                    IN HAPP        hApp)
{
    cmElem* app = (cmElem*)hApp;

    if (app == NULL || app->dnsManager == NULL)
        return RV_ERROR_UNKNOWN;

    /*clear the dns module*/
    cmDnsClear(app);

    /*restart*/
    return cmDnsStart(app, RV_TRUE);
}


#ifdef __cplusplus
}
#endif
