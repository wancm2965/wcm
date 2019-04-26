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

#ifndef _RV_CM_DNS_H_
#define _RV_CM_DNS_H_

/***********************************************************************
cmDns.h

The DNS module of cm.
This module is responsible for dns resolutions for the application. 
***********************************************************************/


/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/



#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/


#define MAX_DOMAIN_LEN      256



typedef struct dnsSrvData{
    RvUint16                          port;
    RvUint16                          priority;
    RvUint16                          weight;
}dnsSrvData;

typedef struct dnsRAElemContext{
    cmElem*                           app;  /*the handle to the stack*/
    int                               raElemLocation;
}dnsRAElemContext;

typedef struct cmDnsTxElem{
    dnsRAElemContext                  dnsContext;
    struct cmDnsTxElem*               parentDnsTx; /*parent transaction if exsist*/
    RvUint16                          numChildTx; /* number of children if exsist*/
    void*                             context; /*the context of the application*/
    cmEvDnsQueryResultT               cmEvDnsQueryResult; /*application callback*/
    RvBool                            txContinue;
    cmDnsQueryType                    queryType;
    cmTransportAddress*               addressBuffer;
    RvUint16                          addrBuffSize; /*in transport addresses*/
    RvChar*                           queryBuffer; 
    RvUint16                          numOfAddresses;/*num of retrieved addresses in the transaction*/
    dnsSrvData                        srvData;
    RvUint32                          queryId; /*received from ares*/
}cmDnsTxElem;


typedef struct dnsModule{
    RvLogMgr*      logMgr; /* Log manager to use */
    RvLogSource    log; /* Log instance used for logging messages */   
    HRA            dnsTxs; /*a RA holding dns transactions*/
    RvInt          queryBuffSize; /*1/2k by default*/
    HRA            dnsQueryBuffs; /*RA holding the query buffers to be given to dns resolver*/
    RvLock         lock; /* Mutex to use */
    RvDnsEngine    dnsEngine;    /*DNS engine of the module*/
    RvUint16       dnsNumServers;
    RvUint16       dnsNunDomains;
    RvUint16       dnsReadBuffSize;
    RvInt          dnsTimeOutInSec;
    RvInt          dnsNumTries;
}dnsModule;

/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/

/******************************************************************************
 * dnsConstruct
 * ----------------------------------------------------------------------------
 * General: create the DNS module.
 *
 * Return Value: Handle to dns module on success NULL on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input: hApp        - Stack's handle for the application.
 * Output: none
 *****************************************************************************/

RVHDNS dnsConstruct(
          IN HAPP        hApp);
/******************************************************************************
 * cmDnsStart
 * ----------------------------------------------------------------------------
 * General: Start the DNS module.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  app    -  Stack handle 
 * Output: none
 *****************************************************************************/
RvStatus cmDnsStart(IN cmElem *app , RvBool restart);

/******************************************************************************
 * cnDnsDestruct
 * ----------------------------------------------------------------------------
 * General: Destruct the DNS module.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hDns               - DNS handle.
 * Output: None.
 *****************************************************************************/
void cmDnsDestruct(IN HAPP hApp);

/******************************************************************************
 * cmDnsClear
 * ----------------------------------------------------------------------------
 * General: Stops all active transactions and clean the DNS DB.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:   app                 -   Stack handle 
 *          
 * Output: None.
 *****************************************************************************/
void cmDnsClear(IN cmElem *app);


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
                    IN HAPP        hApp);


#ifdef __cplusplus
}
#endif

#endif /* _RV_CM_DNS_H_ */
