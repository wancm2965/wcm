/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#include "rvinternal.h"
#include "ema.h"
#include "rpool.h"
#include "cmutils.h"
#include "cm.h"
#include "rvwatchdog.h"
#include "transportint.h"
#include "transparent.h"
#include "tpkt.h"

//{{gaoshizhong 20140305 Add send tcp keepalive
#if (RV_OS_TYPE == RV_OS_TYPE_WIN32)

#include <mstcpip.h>
#elif (RV_OS_TYPE == RV_OS_TYPE_LINUX)

#include  "/usr/include/linux/tcp.h"
#include "/usr/include/linux/socket.h"
#endif
//}}gaoshizhong 20140305 Add

#ifdef __cplusplus
extern "C" {
#endif

/* If the following is defined, then recv() calls that succeed without any bytes are
   counted until they cause the socket to close */
/* #define TPKT_CPU_COUNTER_CHECK */


#ifdef TPKT_CPU_COUNTER_CHECK
static RvInt cpuCounterMax = 1000; /* Maximum number of consecutive recv()=0 that are tolerated */

void tpktSetCpuCounterMax(IN RvInt counterMax)
{
    cpuCounterMax = counterMax;
}
#endif



typedef enum
{
    tpktIdle,       /* Idle state */
    tpktWaiting,    /* We are waiting for the first message to arrive */
    tpktConnected,  /* We are connected to the peer */
    tpktClosing,    /* We received an indication of a close event on the connection */
    tpktClosed,     /* We are closing/closed */

    tpktReportClose /* Socket was closed from remote side, but was not reported as such to the upper layers */
} tpktState;



/* TPKT instance */
typedef struct
{
    RvSelectEngine* pSelEngine; /* Select Engine, used for the file descriptors */
    RvPortRange*    pPortRange; /* Port range to use */
    HEMA            hEma; /* EMA used for TPKT elements */
    cmTransGlobals* transGlobals;
    RvLogMgr*       logMgr; /* Log manager to use */
    RvLogSource     log; /* Log source to use */
} tpktGlobals;

typedef struct
{
    RvUint8             header[TPKT_HEADER_SIZE];   /* TPKT header */
    HRPOOL              hRpool;                     /* the rpool where the message is */
    HRPOOLELEM          message;                    /* the message to be sent/read */
    int                 offset;                     /* the offset into the message.
                                                       For incoming messages only, the first 4 bytes indicated
                                                       by the offset are the TPKT header itself. */
    RvLock*             rpoolLock;                  /* the lock to protect the rpool */
} tpktTrans;

/* tpktInfo holds information about a single TPKT connection. This is held inside EMA */
typedef struct
{
    RvH323Connection    connection; /* The network connection */
    LPTPKTEVENTHANDLER  eventHandler;               /* Callback routine on transaction event */
    void*               context;                    /* additional data to be sent on the callback */
    tpktTrans           recv;    /* The receiving transaction data */
    tpktTrans           send;    /* The sending transaction data */
    tpktTypes           type;    /* The type of connection (MultiServer, Server or Client */
    RvSelectEvents      event;
    tpktState           state; /* Current state of TPKT connection */
    RvTimer             *timer;   /* shutdown timer */
#ifdef TPKT_CPU_COUNTER_CHECK
    RvInt               cpuCounter; /* consecutive recv()=0 calls counter */
#endif
} tpktInfo;

#define socketSendTcpIfConnected(globals, tpkt, finished) \
    (((tpkt)->state != tpktClosed)?(tpktSocketSendTcp(globals, tpkt, (finished))):RV_OK)

#define socketRecvTcpDirectIfConnected(globals, tpkt, buf, len, pLenRecv) \
    (((tpkt)->state != tpktClosed)?(RvSocketReceiveBuffer(RvFdGetSocket(&((tpkt)->connection.fd)), (buf), (RvSize_t)(len), globals->logMgr, (pLenRecv), NULL)):RV_OK)

#define socketRecvTcpIfConnected(globals, tpkt, finished) \
    (((tpkt)->state != tpktClosed)?(tpktSocketRecvTcp(globals, tpkt, (finished))):RV_OK)



/*******************************************************************************************
 * tpktShutdownTimeout
 *
 * Purpose: This timeout functions is called to close tpkt object (including a socket!!!)
 *          in case the socket is stack in FIN_WAIT_2 state, caused by shutdown command
 *
 * Input: the tpkt object to close
 *
 **************************************************************************************/
static RvBool tpktShutdownTimeout(void *context)
{
    tpktInfo *tpkt = (tpktInfo *)context;

    if (emaLock((EMAElement)tpkt))
    {
        tpktGlobals* globals = (tpktGlobals*)emaGetUserData((EMAElement)tpkt);

        /* Close the timer */
        RvH323TimerClear(globals->transGlobals->hTimers, &tpkt->timer);

        /* Close the connection */
        tpktClose((HTPKT)tpkt);

        emaUnlock((EMAElement)tpkt);
    }
    return RV_FALSE;
}


/*******************************************************************************************
 * tpktSocketSendTcp
 *
 * Purpose: internal routine that simulates RvSocketSendBuffer using rpool buffer rather than
 *          a contiguous one, handling the problems arising that.
 *
 * Input:  globals - Globals of TPKT.
 *         tpkt - The object of the send, having all the necessary data about rpool.
 *
 * Output: finished - whether the send was finished already or not.
 *
 * Return Value: number of bytes sent.
 *******************************************************************************************/
static int tpktSocketSendTcp(
    IN  tpktGlobals*    globals,
    IN  tpktInfo*       tpkt,
    OUT RvBool*         finished)
{
    int len, totalLength, totalSent = 0;
    RvSize_t sent = 0;
    RvUint8 *buffer;
    RvStatus ret;
    RvSocket *sock;

    /* lock around operation with the rpool */
    RvLockGet(tpkt->send.rpoolLock, globals->logMgr);

    /* get the total length of the message */
    totalLength = rpoolChunkSize(tpkt->send.hRpool, tpkt->send.message);

    sock = RvFdGetSocket(&tpkt->connection.fd);

    /* send contiguous segment at a time until all the message is sent or TCP is exhausted */
    do {
        /* get the current segment pointer and length */
        len = rpoolGetPtr(tpkt->send.hRpool,
                          tpkt->send.message,
                          tpkt->send.offset,
                          (void **)&buffer);

        /* do the actual send */
        ret = RvSocketSendBuffer(sock, buffer, (RvSize_t)len, NULL,
            globals->logMgr, &sent);

        /* update the pointer into the message and the current call counter */
        if (ret == RV_OK)
        {
            tpkt->send.offset += (int)sent;
            totalSent += (int)sent;
        }
        else
        {
            /* Error: we couldn't write even one segment */
            *finished = RV_FALSE;
            /* release rpool */
            RvLockRelease(tpkt->send.rpoolLock, globals->logMgr);
            /* report the error */
            return ret;
        }

        /* if TCP was exhausted before all we wanted was sent */
        if (sent < (RvSize_t)len)
        {
            /* we couldn't write even one segment, wait for event write */
            *finished = RV_FALSE;
            /* release rpool */
            RvLockRelease(tpkt->send.rpoolLock, globals->logMgr);
            /* report how many bytes were read in this call */
            return totalSent;
        }

    } while (tpkt->send.offset < totalLength);

    /* if we're here then all the message was sent */
    *finished = RV_TRUE;
    RvLockRelease(tpkt->send.rpoolLock, globals->logMgr);
    /* report that the whole message was sent */
    return totalSent;
}

/*******************************************************************************************
 * tpktSocketRecvTcp
 *
 * Purpose: internal routine that simulates RvSocketReceiveBuffer using rpool buffer rather than
 *          a contiguous one, handling the problems arising from that.
 *
 * Input:
 *        tpkt - The object of the receive, having all the necessary data about rpool
 * Output: finished - whether the receive was finished already or not
 *
 * Return Value: number of bytes received.
 *******************************************************************************************/
int tpktSocketRecvTcp(
    IN  tpktGlobals*    globals,
    IN  tpktInfo*       tpkt,
    OUT RvBool*         finished)
{
    int len, totalLength, totalReceived = 0;
    RvSize_t received = 0;
    RvUint8 *buffer;
    RvStatus status;
    RvSocket *sock;

    /* lock around operation with the rpool */
    RvLockGet(tpkt->recv.rpoolLock, globals->logMgr);

    /* get the total length of the currently allocated rpool message
       This should be exactly the size of the message as was received in the header */
    totalLength = rpoolChunkSize(tpkt->recv.hRpool, tpkt->recv.message) + TPKT_HEADER_SIZE;

    sock = RvFdGetSocket(&tpkt->connection.fd);

    /* read one segment after the other until the whole message is
       read or TCP is empty for a while */
    do {
        /* get a pointer and length for the current segment */
        len = rpoolGetPtr(tpkt->recv.hRpool,
                          tpkt->recv.message,
                          tpkt->recv.offset - TPKT_HEADER_SIZE,
                          (void **)&buffer);

        /* do the actual receive */
        status = RvSocketReceiveBuffer(sock, buffer, (RvSize_t)len,
            globals->logMgr, &received, NULL);

#ifdef TPKT_CPU_COUNTER_CHECK
        if ((status == RV_OK) && (received == 0))
        {
            tpkt->cpuCounter++;
            if (tpkt->cpuCounter > cpuCounterMax)
            {
                /* Out of bounds - treat as error */
                status = RV_ERROR_OUTOFRANGE;
            }
        }
        else
            tpkt->cpuCounter = 0;
#endif /* TPKT_CPU_COUNTER_CHECK */

        /* update the offset into the rpool message and the current call counter */
        tpkt->recv.offset += (int)received;
        totalReceived += (int)received;

        /* if TCP was empty before we read all we wanted wait for next event */
        if ((status != RV_OK) || (received < (RvSize_t)len))
        {
            /* we couldn't read even one segment, wait for event write */
            *finished = RV_FALSE;

            /* release rpool */
            RvLockRelease(tpkt->recv.rpoolLock, globals->logMgr);

            /* If we had an error receiving, then this connection is most likely dead */
            if (status != RV_OK)
                return status;

            /* report how many bytes were read in this call */
            return totalReceived;
        }
    } while (tpkt->recv.offset < totalLength);

    /* here it means that the whole message was read */
    *finished = RV_TRUE;
    RvLockRelease(tpkt->recv.rpoolLock, globals->logMgr);

    return totalLength;
}

/*******************************************************************************************
 * tpktCloseElement
 *
 * Purpose: internal routine that closes one tpkt element. Used at shutdown to be called
 *          on all the remaining elements.
 *
 * Input:   elem - The tpkt element
 *          param - Dummy parameter.
 *
 *******************************************************************************************/
static void* tpktCloseElement(IN EMAElement elem, IN void* param)
{
    tpktInfo* tpkt = (tpktInfo *)elem;
    tpktGlobals* globals = (tpktGlobals*)emaGetUserData(elem);

    RV_UNUSED_ARG(param);

    if ((tpkt != NULL) && (globals != NULL))
    {
        tpkt->state = tpktClosed;

        /* Close the connection */
        tpktClose((HTPKT)tpkt);
    }

    return NULL;
}

/*******************************************************************************************
 * tpktInit
 *
 * Purpose: Initialize the structures of the tpkt module
 *
 * Input:   hApp         - Stack's application handle
 *          selEngine    - Select Engine, used for the file descriptors
 *          sessions     - Maximal number of allowed connections
 *          portRange    - Range of ports to use
 *          transGlobals - transport handle
 *          logMgr       - Log manager to use
 * Return Value: Handle to the TPKT instance
 *******************************************************************************************/
HTPKTCTRL tpktInit(
    IN HAPP             hApp,
    IN RvSelectEngine*  selEngine,
    IN int              sessions,
    IN RvPortRange*     portRange,
    IN void*            voidTransGlobals,
    IN RvLogMgr*        logMgr)
{
    cmTransGlobals*  transGlobals = (cmTransGlobals *)voidTransGlobals;
    tpktGlobals*    globals;

    /* allocate global area for tpkt package */
    if (RvMemoryAlloc(NULL, sizeof(tpktGlobals), logMgr, (void**)&globals) != RV_OK)
        return NULL;

    /* Build the EMA pool of tpktInfo elements */
    globals->hEma = emaConstruct(
        sizeof(tpktInfo), sessions + 1,
        emaLockPointers, "TPKT elements", 0, globals, NULL, logMgr);
    if (globals->hEma == NULL)
    {
        RvMemoryFree(globals, logMgr);
        return NULL;
    }
    emaSetIdleEvent(globals->hEma, emaEvTpktElementIdle);

    emaAddWatchdogResource(globals->hEma, cmiGetWatchdogHandle(hApp), "TpktElements", "Transport", "TPKT Connection elements");

    globals->transGlobals = transGlobals;
    globals->logMgr = logMgr;
    globals->pSelEngine = selEngine;
    globals->pPortRange = portRange;

    RvLogSourceConstruct(logMgr, &globals->log, "TCP", "Lower TPKT layer");

    return (HTPKTCTRL)globals;
}


/*******************************************************************************************
 * tpktEnd
 *
 * Purpose: Destroy the tpktInfo elements and close all connections
 *
 * Input:   hCtrl - Handle to the EMA of tpktInfo elements
 *
 * Return Value: 0
 *******************************************************************************************/
int tpktEnd(HTPKTCTRL hCtrl)
{
    tpktGlobals *globals = (tpktGlobals *)hCtrl;

    /* Go over all tpktInfo elements in the EMA pool and close them */
    emaDoAll(globals->hEma, tpktCloseElement, NULL);

    /* destroy the EMA pool of tpktInfo elements */
    emaDestruct(globals->hEma);

    RvLogSourceDestruct(&globals->log);

    /* release the global area of tpkt */
    RvMemoryFree(globals, globals->logMgr);

    return 0;
}


/*******************************************************************************************
 * tpktEvent
 *
 * Purpose: Treats the events that may occur on a connection (Accept, Connect, Write, Read,
 *      close).
 *
 * Input:   socket  - The listening socket (in case of accept event)
 *          event   - The type of the event
 *          error   - indicates whether an error occured in the li level
 *          context - The tpkt object
 *
 * Output:  None.
 *******************************************************************************************/
void tpktEvent(
    IN RvSelectEngine*  selectEngine,
    IN RvSelectFd*      fd,
    IN RvSelectEvents   selectEvent,
    IN RvBool           error)
{
    RvH323Connection * connection = RV_GET_STRUCT(RvH323Connection, fd, fd);
    tpktInfo* tpkt = RV_GET_STRUCT(tpktInfo, connection, connection);
    tpktGlobals* globals = (tpktGlobals*) connection->context;
    RvStatus ret = RV_OK;

    if (fd == NULL) return;
    if (globals == NULL) return;

    if (!emaLock((EMAElement)tpkt))
        return;

    /* Treat the events on the given socket only if this socket was never deleted */
    if (!emaWasDeleted((EMAElement)tpkt))
    {
        if ((tpkt->state == tpktReportClose) && (selectEvent == RvSelectRead))
            selectEvent = RvSelectClose;
#ifdef TPKT_CPU_COUNTER_CHECK
        if ((tpkt->cpuCounter > cpuCounterMax) && (selectEvent == RvSelectRead))
        {
            /* Too many empty receive events - convert to close and drop this connection */
            selectEvent = RvSelectClose;
        }
#endif /* TPKT_CPU_COUNTER_CHECK */


        switch (selectEvent)
        {
            /* accept was done in order to open and incoming connection request */
            case RvSelectAccept:
            {
                RvSocket newSocket;
                RvAddress remoteAddress;

                RvLogDebug(&globals->log, (&globals->log,
                    "tpktEvent(connection=%p, RvSelectAccept, error=%d, context=%p)", connection, error, globals));

                ret = RvSocketAccept(RvFdGetSocket(&connection->fd), globals->logMgr, &newSocket, &remoteAddress);

                if (ret != RV_OK)
                {
                    /* call the callback with error indication */
                    tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_TRUE, tpkt->context);
                }
                else
                /* We want to leave the listening socket active */
                if (tpkt->type==tpktMultiServer)
                {
                    tpktInfo *newTpkt = (tpktInfo *)emaAdd(globals->hEma, NULL);

                    /* check that a new element was allocated */
                    if (newTpkt == NULL)
                    {
                        RvSocketDestruct(&newSocket, RV_TRUE, NULL, globals->logMgr);
                        tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_TRUE, tpkt->context);
                        break;
                    }

                    if (transParentNew((HAPPTRANS)globals->transGlobals, (void*)newTpkt) != RV_OK)
                    {
                        RvSocketDestruct(&newSocket, RV_TRUE, NULL, globals->logMgr);
                        emaDelete(newTpkt);
                        tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_TRUE, tpkt->context);
                        break;
                    }

                    /* initialize the new tpktInfo element */
                    emaLock((EMAElement)newTpkt);
                    memset(newTpkt, 0, sizeof(tpktInfo));
                    newTpkt->type = tpktAcceptedServer; /* The connection type is tpktServer for the individual connection */

                    /* We don't want to get a "close" event here - otherwise a new call
                       would be closed by event from previous call accepted on the same socket */
                    newTpkt->event = RvSelectRead; /* set the ability to receive read events on the connection */

                    newTpkt->connection.type     = tpkt->connection.type;
                    newTpkt->connection.context  = tpkt->connection.context;
                    RvFdConstruct(&newTpkt->connection.fd, &newSocket, globals->logMgr);

                    if (!error)
                        newTpkt->state = tpktWaiting;
                    else
                        newTpkt->state = tpktClosed;

                    /* register the event (close event) for the connection */
                    RvSelectAdd(selectEngine, &newTpkt->connection.fd, newTpkt->event, tpktEvent);
                    /* notify the application that the accept was done */
                    tpkt->eventHandler((HTPKT)newTpkt, selectEvent, 0, RV_FALSE, tpkt->context);
                    if (emaWasDeleted((EMAElement)newTpkt))
                    {
                        /* something bad happened in the event handler.
                           clean up the FD and the socket */
                        RvSelectRemove(globals->pSelEngine, &newTpkt->connection.fd);
                        RvSocketDestruct(&newSocket, RV_TRUE, NULL, globals->logMgr);
                        RvFdDestruct(&tpkt->connection.fd);
                    }
                    emaUnlock((EMAElement)newTpkt);
                }
                else
                /* we don't want the listening socket anymore */
                {
                    RvSelectRemove(selectEngine, &connection->fd);
                    RvSocketDestruct(RvFdGetSocket(&connection->fd), RV_FALSE, globals->pPortRange, globals->logMgr);
                    RvFdDestruct(&connection->fd);

                    if (!error)
                        tpkt->state = tpktWaiting;
                    else
                        tpkt->state = tpktClosed;

                    /* use the object created as the connection object */
                    tpkt->type = tpktAcceptedServer;

                    /* We don't want to get a "close" event here - otherwise a new call
                       would be closed by event from previous call accepted on the same socket */
                    tpkt->event = RvSelectRead; /* set the ability to receive read events on the connection */

                    RvFdConstruct(&tpkt->connection.fd, &newSocket, globals->logMgr);
                    RvSelectAdd(selectEngine, &tpkt->connection.fd, tpkt->event, tpktEvent);

                    /* notify that a connection was created */
                    tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_FALSE, tpkt->context);
                }
            }
            break;

            /* Treat a connect event */
            case RvSelectConnect:
            {
                RvLogDebug(&globals->log, (&globals->log,
                    "tpktEvent(connection=%p, RvSelectConnect, error=%d, context=%p)", connection, error, globals));

                /* remove the connect event from the allowed events bitmap and add a close event ability*/
                tpkt->event = (RvSelectEvents) (RvSelectRead | RvSelectClose);

                /* mark the connection as opened */
                if (!error)
                    tpkt->state = tpktConnected;

                /* register the events for the connection */
                RvSelectUpdate(selectEngine, fd, tpkt->event, tpktEvent);

                /* notify the application that the connect was done */
                tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, error, tpkt->context);
            }
            break;

            /* treat a write event */
            case RvSelectWrite:
            {
                RvBool finished = RV_FALSE;

                RvLogDebug(&globals->log, (&globals->log,
                    "tpktEvent(connection=%p, RvSelectWrite, error=%d, context=%p)", connection, error, globals));

                /* send the rest of the message */
                if (socketSendTcpIfConnected(globals, tpkt, &finished) < 0)
                {
                    /* remove the write event from the connection */
                    tpkt->event = (RvSelectEvents) (tpkt->event & ~RvSelectWrite);

                    /* notify that the message was sent but not completed */
                    RvSelectUpdate(selectEngine, fd, tpkt->event, tpktEvent);

                    tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_TRUE, tpkt->context);

                    break;
                }

                /* if ALL the message was sent already */
                if (finished)
                {
                    /* remove the write event from the connection */
                    tpkt->send.hRpool = NULL;
                    tpkt->event = (RvSelectEvents) (tpkt->event & ~RvSelectWrite);
                    RvSelectUpdate(selectEngine, fd, tpkt->event, tpktEvent);

                    /* notify that the message was completely sent and give the number of bytes sent */
                    tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_FALSE, tpkt->context);
                }
            }
            break;

            /* treat a read event */
            case RvSelectRead:
            {
                RvSize_t rcv = 0;
                RvBool finished = RV_FALSE;
                RvInt reallocRes = 0;

                RvLogDebug(&globals->log, (&globals->log,
                    "tpktEvent(connection=%p, RvSelectRead, error=%d, context=%p)", connection, error, globals));

                if (error)
                {
                    /* If we had an error on a read event, we assume there's nothing to do with the
                       connection and we close it */
                    tpkt->state = tpktClosed;
                }

                /* if the connection is closing, we better read it to make sure it won't get stuck on Windows */
                if (tpkt->state == tpktClosed)
                {
                    RvUint8 buffer[16];
                    RvSize_t received;
                    RvLogDebug(&globals->log, (&globals->log,
                        "Event read on closing connection (connection=%p, tpkt=%p)", connection, tpkt));
                    /* cancel the read event */
                    ret = RvSocketReceiveBuffer(RvFdGetSocket(&tpkt->connection.fd), buffer, sizeof(buffer),
                        globals->logMgr, &received, NULL);
                    if (ret != RV_OK)
                    {
                        tpktClose((HTPKT)tpkt);
                    }
                    break;
                }

                /* if there is no buffer */
                if (tpkt->recv.hRpool == NULL)
                {
                    /* notify that there was nothing read */
                    tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_FALSE, tpkt->context);
                    break;
                }

                /* if we are still reading the header */
                if (tpkt->recv.offset < TPKT_HEADER_SIZE)
                {
                    /* get the rest of the header */
                    ret = socketRecvTcpDirectIfConnected(globals, tpkt, (tpkt->recv.header+tpkt->recv.offset),
                        (TPKT_HEADER_SIZE-tpkt->recv.offset), &rcv);
#ifdef TPKT_CPU_COUNTER_CHECK
                    if ((ret == RV_OK) && (rcv == 0))
                    {
                        tpkt->cpuCounter++;
                        if (tpkt->cpuCounter > cpuCounterMax)
                        {
                            /* Out of bounds - treat as error */
                            ret = RV_ERROR_OUTOFRANGE;
                        }
                    }
                    else
                        tpkt->cpuCounter = 0;
#endif /* TPKT_CPU_COUNTER_CHECK */
                    if (ret != RV_OK)
                    {
                        /* On error, we're going to make sure this socket gets a close event */
                        tpkt->state = tpktReportClose;

                        tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_TRUE, tpkt->context);
                        break;
                    }

                    if (rcv > 0)
                    {
                        tpkt->recv.offset += rcv;

                        RvLogDebug(&globals->log, (&globals->log,
                            "Received header: %02X %02X %02X %02X",
                            tpkt->recv.header[0], tpkt->recv.header[1], tpkt->recv.header[2], tpkt->recv.header[3]));
                    }
                }

                /* if we have the header we can allocate the rpool message */
                if (tpkt->recv.offset == TPKT_HEADER_SIZE)
                {
                    int tpktLen = (((int)(tpkt->recv.header[2])<<8)+tpkt->recv.header[3])-TPKT_HEADER_SIZE;

                    if (tpkt->state == tpktWaiting)
                    {
                        /* First incoming header must be valid and non-empty */
                        if ((tpkt->recv.header[0] != 3) || (tpkt->recv.header[1] != 0) || (tpktLen <= 0))
                        {
                            RvLogWarning(&globals->log, (&globals->log, "Received a bad header..."));
                            tpktClose((HTPKT)tpkt);
                            break;
                        }
                    }
                    if (tpktLen > 0)
                    {
                        RvLockGet(tpkt->recv.rpoolLock, globals->logMgr);
                        reallocRes = rpoolRealloc(tpkt->recv.hRpool, tpkt->recv.message, tpktLen);
                        RvLockRelease(tpkt->recv.rpoolLock, globals->logMgr);
                    }
                }

                /* we are reading a new message body */
                if (tpkt->recv.offset >= TPKT_HEADER_SIZE)
                {
                    int tpktLen = (((int)(tpkt->recv.header[2])<<8)+tpkt->recv.header[3])-TPKT_HEADER_SIZE;

                    /* Do the actual reading */
                    if ((tpktLen > 0) && (reallocRes >= 0))
                    {
                        if (socketRecvTcpIfConnected(globals, tpkt, &finished) < 0)
                        {
                            /* On error, we're going to make sure this socket gets a close event */
                            tpkt->state = tpktReportClose;

                            tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_TRUE, tpkt->context);
                            break;
                        }
                    }
                    else
                    {
                        /* finish the reading, we got a corrupted header */
                        tpktLen  = 0;
                        finished = RV_TRUE;
                    }

                    /* if we read all that we asked - but we should make sure this is not
                       just an empty TPKT header.*/
                    if (finished && (tpktLen > 0))
                    {
                        /* If we are waiting for the first message to arrive, then it is here... */
                        if (tpkt->state == tpktWaiting)
                            tpkt->state = tpktConnected;

                        /* Close the read event, notify how many bytes were read */
                        tpkt->recv.hRpool = NULL;

                        tpkt->eventHandler((HTPKT)tpkt, selectEvent, tpktLen, RV_FALSE, tpkt->context);
                    }
                }

                if ((tpkt->state == tpktClosing) && (tpkt->context == NULL))
                {
                    /* Seems like it's time to remove the Read event from this socket... */
                    /* If we don't do this, we'll end up burning CPU time */
                    RvSelectUpdate(selectEngine, fd, RvSelectClose, tpktEvent);
                }
            }
            break;


            /* Treat the event of closing of a connection */
            case RvSelectClose:
            {
                RvLogDebug(&globals->log, (&globals->log,
                    "tpktEvent(connection=%p, RvSelectClose, error=%d, context=%p)", connection, error, globals));

                /* In case we are not in a closing process already */
                if (tpkt->state != tpktClosed)
                {
                    tpkt->state = tpktClosing;

                    /* notify that the close occured (as a recv event) */
                    if (tpkt->eventHandler != NULL)
                    {
                        tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_FALSE, tpkt->context);
                    }
                    else
                    /* if no recv callback use the send callback (?) */
                    if (tpkt->eventHandler != NULL)
                    {
                        tpkt->eventHandler((HTPKT)tpkt, selectEvent, 0, RV_FALSE, tpkt->context);
                    }

                    /* mark the connection as closing */
                    tpkt->state = tpktClosed;
                }
                /* if we already got a close event on this connection */
                else
                {
                    /* Close the tpkt object */
                    tpktClose((HTPKT)tpkt);
                }
            }
            break;

            default:
                break;
        }
    }
    emaUnlock((EMAElement)tpkt);
}

/*******************************************************************************************
 * tpktOpen
 *
 * Purpose: Allocate a socket and start either a connect process in case of a client which
 *      supplied a full address, or a listening process in case of server.
 *      For a client that didn't supply an address yet, just create the socket.
 *
 * Input:   hTpktCtrl       - Handle to the tpkt instance
 *          localAddress    - Local address to use
 *          connectionType  - What type of connection we request (Tcp or TcpServer)
 *          standard        - Standard to use on connection (Q931 or H245)
 *          parentShare     - Element containing ParentLock to share
 *          eventHandler    - Callback to handle network events
 *          context         - Additional data to be supplied on the callback
 *
 * Return Value: A handle to the tpkt object created
 *******************************************************************************************/
HTPKT tpktOpen(
    IN HTPKTCTRL            hTpktCtrl,
    IN cmTransportAddress*  localAddress,
    IN RvH323ConnectionType connType,
    IN tpktTypes            tpktType,
    IN EMAElement           parentShare,
    IN LPTPKTEVENTHANDLER   eventHandler,
    IN void*                context)
{
    tpktGlobals* globals = (tpktGlobals *)hTpktCtrl;
    RvAddress coreAddress, *pCoreAddress;
    tpktInfo* tpktNew;
    RvSocket sock;
    RvStatus res;

    RvLogEnter(&globals->log, (&globals->log, "tpktOpen"));

    tpktNew = (tpktInfo *)emaAdd(globals->hEma, context);

    /* check that an element was created */
    if (tpktNew == NULL)
    {
        RvLogLeave(&globals->log, (&globals->log, "tpktOpen - Out of resoruces"));
        return NULL;
    }
    if (parentShare != NULL)
        transParentShare((HAPPTRANS)globals->transGlobals, (void*)tpktNew, parentShare);
    memset(tpktNew, 0, sizeof(tpktInfo));

    emaLock((EMAElement)tpktNew);

    if (localAddress == NULL)
        pCoreAddress = NULL;
    else
    {
        RvH323CmToCoreAddress(localAddress, &coreAddress);
        pCoreAddress = &coreAddress;
    }

    /* Create an IO connection to work with */
    tpktNew->connection.type = RvH323ConnectionNone;
    tpktNew->connection.context = globals;
    res = RvSocketConstruct((pCoreAddress!=NULL)?pCoreAddress->addrtype:RV_ADDRESS_TYPE_IPV4,
        RvSocketProtocolTcp, globals->logMgr, &sock);

    if (res != RV_OK)
    {
        RvLogLeave(&globals->log, (&globals->log, "tpktOpen - Error constructing a connection (%d)", res));
        emaDelete((EMAElement)tpktNew);
        emaUnlock((EMAElement)tpktNew);
        return NULL;
    }

    /* We're always working with non-blocking sockets */
    RvSocketSetBlocking(&sock, RV_FALSE, globals->logMgr);

    /* bind it to the listening address */
    if (RvSocketBind(&sock, pCoreAddress, globals->pPortRange, globals->logMgr) != RV_OK)
    {
       /* todo: h.e  (by the way we have this peace of code 3 times)
          (probably  "goto" is not so bad ) */
        RvSocketDestruct(&sock, RV_FALSE, NULL, globals->logMgr);
        RvLogLeave(&globals->log, (&globals->log, "tpktOpen - Error listening (%d)", res));
        emaDelete((EMAElement)tpktNew);
        emaUnlock((EMAElement)tpktNew);
        return NULL;
    }

    RvFdConstruct(&tpktNew->connection.fd, &sock, globals->logMgr);

    /* for server - start listening on the socket */
    switch (tpktType)
    {
    case tpktMultiServer:
        /*RvSocketReuseAddr(&sock, globals->logMgr);*/
        /* Continue on to tpktServer */

    case tpktServer:
        /* Listen to incoming connections */
        if (RvSocketListen(&sock, 128, globals->logMgr) != RV_OK)
        {
            RvSocketDestruct(&sock, RV_FALSE, globals->pPortRange, globals->logMgr);
            RvLogLeave(&globals->log, (&globals->log, "tpktOpen - Error listening (%d)", res));
            emaDelete((EMAElement)tpktNew);
            emaUnlock((EMAElement)tpktNew);
            return NULL;
        }
        else
        {
            tpktNew->event = RvSelectAccept;
            if (RvSelectAdd(globals->pSelEngine, &tpktNew->connection.fd, tpktNew->event, tpktEvent) != RV_OK)
            {
                RvFdDestruct(&tpktNew->connection.fd);
                RvSocketDestruct(&sock, RV_FALSE, globals->pPortRange, globals->logMgr);
                RvLogLeave(&globals->log, (&globals->log, "tpktOpen - Error adding to Select (%d)", res));
                emaDelete((EMAElement)tpktNew);
                emaUnlock((EMAElement)tpktNew);
                return NULL;
            }
        }
        break;

    default:
        break;
    }

    tpktNew->connection.type = connType;

    /* Initialize the TPKT object */
    tpktNew->type = tpktType;

    tpktNew->eventHandler = eventHandler;
    tpktNew->context = context;

    emaUnlock((EMAElement)tpktNew);
    RvLogLeave(&globals->log, (&globals->log, "tpktOpen(hTpkt=%p)=%d", tpktNew, res));
    return (HTPKT)tpktNew;
}


/*******************************************************************************************
 * tpktConnect
 *
 * Purpose: This routine supplements the tpktOpen routine, for clients which didn't supply
 *          an address and now wish to do the actual connect operation on the already allocated
 *          socket.
 *
 * Input:   hTpkt           - Handle to the tpktInfo element
 *          remoteAddress   - Address to connect to
 *
 * Return Value: Non-negative value on success, other on failure
 *******************************************************************************************/
RvStatus tpktConnect(IN HTPKT hTpkt, IN cmTransportAddress* remoteAddress)
{
    tpktInfo* tpkt = (tpktInfo *)hTpkt;
    tpktGlobals* globals;
    RvAddress coreAddress;
    RvStatus status = RV_ERROR_UNKNOWN;

    if (emaMarkLocked((EMAElement)tpkt))
    {
        globals = (tpktGlobals *)emaGetUserData((EMAElement)tpkt);

        /* Let's try to connect it */
        RvH323CmToCoreAddress(remoteAddress, &coreAddress);
#if (RV_OS_TYPE != RV_OS_TYPE_WIN32) && (RV_OS_TYPE != RV_OS_TYPE_NUCLEUS)
        status = RvSocketConnect(RvFdGetSocket(&tpkt->connection.fd), &coreAddress, globals->logMgr);
        if (status == RV_OK)
#endif
        {
            /* wait for a connect event on this socket */
            tpkt->event = RvSelectConnect;
            RvSelectAdd(globals->pSelEngine, &tpkt->connection.fd, tpkt->event, tpktEvent);
        }
#if (RV_OS_TYPE == RV_OS_TYPE_WIN32) || (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS)
        status = RvSocketConnect(RvFdGetSocket(&tpkt->connection.fd), &coreAddress, globals->logMgr);
#endif
        RvAddressDestruct(&coreAddress);

        emaReleaseLocked((EMAElement)tpkt);
    }

    return status;
}


/*******************************************************************************************
 * tpktClose
 *
 * Purpose: Starts the closing procedure of a tpkt object.
 *          Frees all its resources if an Idle event won't arrive from the IO layer for the
 *          TPKT object.
 *
 * Input:   hTpkt - Handle to the tpktInfo element
 *
 * Return Value: Non-negative value on success, other on failure
 *******************************************************************************************/
int tpktClose(IN HTPKT hTpkt)
{
    tpktInfo* tpkt = (tpktInfo *)hTpkt;
    tpktGlobals* globals;
    RvSocket *sock;
    RvStatus res = RV_ERROR_UNKNOWN;
    RvStatus updatedRes;

    if (!emaLock((EMAElement)tpkt)) return RV_ERROR_DESTRUCTED;
    globals = (tpktGlobals *)emaGetUserData((EMAElement)tpkt);
    if (globals == NULL)
    {
        emaUnlock((EMAElement)tpkt);
        return RV_ERROR_NULLPTR;
    }

    RvLogInfo(&globals->log, (&globals->log, "tpktClose(HTPKT %p)", hTpkt));

    tpkt->context = NULL;
    tpkt->send.hRpool = NULL;
    tpkt->recv.hRpool = NULL;

    sock = RvFdGetSocket(&tpkt->connection.fd);

    /* if indeed the connection is connected and not yet in a closing state */
    if ((tpkt->state == tpktConnected) || (tpkt->state == tpktWaiting))
    {
        /* Mark the connection as closing */
        tpkt->state = tpktClosed;

        /* make sure we're called for the grand closing */
        tpkt->event = (RvSelectEvents) (RvSelectRead | RvSelectClose);

        updatedRes = RvSelectUpdate(globals->pSelEngine, &tpkt->connection.fd, tpkt->event, tpktEvent);

        /* now close */
        res = RvSocketShutdown(sock, RV_TRUE, globals->logMgr);
        RvH323TimerCancel(globals->transGlobals->hTimers, &tpkt->timer);
        if ((res == RV_OK) && (updatedRes == RV_OK))
        {
            /* Set a shutdown timer. If we won't get a network close event, we are going
               to force this socket to a closure */
            tpkt->timer = RvH323TimerStart(globals->transGlobals->hTimers, tpktShutdownTimeout, tpkt, 5000);
        }

        if (tpkt->timer == NULL)
        {
            /* In case of an error trying to close the TPKT gracefully, we just brutally
            kill it and get it all over with */
            RvSelectRemove(globals->pSelEngine, &tpkt->connection.fd);
            res = RvSocketDestruct(sock, (tpkt->type != tpktMultiServer) && (tpkt->type != tpktServer),
                ((tpkt->type==tpktAcceptedServer) ? NULL : globals->pPortRange), globals->logMgr);
            RvFdDestruct(&tpkt->connection.fd);
            emaDelete((EMAElement)tpkt);
        }
    }
    else
    {
        /* The connection is already closing -
           Vehemently close the socket and release the tpkt object */
        RvH323TimerCancel(globals->transGlobals->hTimers, &tpkt->timer);
        if (tpkt->event != 0)
        {
            /* We have an event to remove */
            RvSelectRemove(globals->pSelEngine, &tpkt->connection.fd);
        }
        res = RvSocketDestruct(sock, (tpkt->type != tpktMultiServer) && (tpkt->type != tpktServer),
            ((tpkt->type==tpktAcceptedServer) ? NULL : globals->pPortRange), globals->logMgr);
        RvFdDestruct(&tpkt->connection.fd);
        emaDelete((EMAElement)tpkt);
    }
    emaUnlock((EMAElement)tpkt);
    return res;
}


/*******************************************************************************************
 * tpktSendFromRpool
 *
 * Purpose: This routine sends a given rpool message over a connection, assuming that the message
 *      already contains a tpkt header.
 *
 * Input:   hTpkt           - Handle to the tpktInfo element
 *          hRpool          - handle to the rpool.
 *          message         - points to the message to be sent.
 *          offset          - where in the message to start the sending.
 *          rpoolLock       - The lock of the rpool
 *
 * Return Value: 0 or RV_ERROR_UNKNOWN
 *******************************************************************************************/
int tpktSendFromRpool(HTPKT hTpkt,
                      HRPOOL hRpool,
                      HRPOOLELEM message,
                      int offset,
                      RvLock* rpoolLock)
{
    tpktInfo    *tpkt=(tpktInfo*)hTpkt;
    tpktGlobals *globals;

    int         length;
    RvUint8     *buffer;
    RvBool      finished = RV_FALSE;

    if (!emaMarkLocked((EMAElement)tpkt)) return RV_ERROR_UNKNOWN;
    globals = (tpktGlobals *)emaGetUserData((EMAElement)tpkt);
    if ((globals == NULL) || (tpkt->send.hRpool != NULL) || (tpkt->state == tpktIdle) || (tpkt->state == tpktClosed))
    {
        emaReleaseLocked((EMAElement)tpkt);
        return RV_ERROR_UNKNOWN;
    }

    /* update the tpkt object with the rpool data */
    tpkt->send.hRpool    = hRpool;
    tpkt->send.message   = message;
    tpkt->send.offset    = offset;
    tpkt->send.rpoolLock = rpoolLock;

    RvLockGet(rpoolLock, globals->logMgr);

    /* get the total message size */
    length = rpoolChunkSize(hRpool, message) - MSG_HEADER_SIZE; /* minus the pointer to the next message */

    /* Mark the tpkt header as a new message (tpkt ver) and set into it the length
       of the buffer + header */
    rpoolGetPtr(tpkt->send.hRpool, tpkt->send.message, tpkt->send.offset, (void **)&buffer);

    buffer[0]=tpkt->send.header[0]=3;
    buffer[1]=tpkt->send.header[1]=0;
    buffer[2]=tpkt->send.header[2]=(unsigned char)((length)>>8);
    buffer[3]=tpkt->send.header[3]=(unsigned char)(length);

    RvLockRelease(rpoolLock, globals->logMgr);

    /* Do the actual send of header + buffer */
    if (socketSendTcpIfConnected(globals, tpkt, &finished) < 0)
    {
        tpkt->send.hRpool = NULL;
        emaReleaseLocked((EMAElement)tpkt);
        return RV_ERROR_UNKNOWN;
    }

    /* if all was sent, return the num of bytes sent */
    if (finished)
    {
        tpkt->send.hRpool = NULL;
        emaReleaseLocked((EMAElement)tpkt);
        return length;
    }

    /* trigger the write event for the next chunk of data */
    tpkt->event=(RvSelectEvents) (tpkt->event | RvSelectWrite);
    RvSelectUpdate(globals->pSelEngine, &tpkt->connection.fd, tpkt->event, tpktEvent);

    emaReleaseLocked((EMAElement)tpkt);
    return 0;
}


/*******************************************************************************************
 * tpktGetAddresses
 *
 * Purpose: return the addresses associated with the given tpkt object.
 *
 * Input:   hTpkt - A handle to the TPKT module
 * Output:  localAddress    - Local address of the connection. If NULL, not updated
 *          remoteAddress   - Remote address of the connection. If NULL, not updated
 *
 * Return Value: Non-negative value on success, other on failure
 *******************************************************************************************/
RvStatus tpktGetAddresses(
    IN  HTPKT               hTpkt,
    OUT cmTransportAddress* localAddress,
    OUT cmTransportAddress* remoteAddress)
{
    tpktInfo* tpkt = (tpktInfo *)hTpkt;
    RvStatus res = RV_ERROR_UNKNOWN;

    if (emaMarkLocked((EMAElement)tpkt))
    {
        RvSocket *sock;
        RvAddress address;
        res = RV_OK;

        sock = RvFdGetSocket(&tpkt->connection.fd);

        if (localAddress)
        {
            res = RvSocketGetLocalAddress(sock, NULL, &address);
            if (res == RV_OK)
            {
                res = RvH323CoreToCmAddress(&address, localAddress);
                RvAddressDestruct(&address);
            }
        }

        if (remoteAddress && (res == RV_OK))
        {
            res = RvSocketGetRemoteAddress(sock, NULL, &address);
            if (res == RV_OK)
            {
                res = RvH323CoreToCmAddress(&address, remoteAddress);
                RvAddressDestruct(&address);
            }
        }

        emaReleaseLocked((EMAElement)tpkt);
    }

    return res;
}

/*******************************************************************************************
 * tpktRecvNotify
 *
 * Purpose: to set a CallBack routine to be called when data arrives.
 *      This routine is called on EvAccept since it's fast.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *      eventHandler - The callback to be read on receiving data
 *      context - To be returned with the callback (may be appl handle etc.)
 * Return Value: -1 : on Error;
 *        0 : OK
 *******************************************************************************************/
int tpktRecvNotify(HTPKT hTpkt,LPTPKTEVENTHANDLER eventHandler,void*context)
{
    tpktInfo *tpkt=(tpktInfo*)hTpkt;
    tpktGlobals *globals;

    if (!emaMarkLocked((EMAElement)tpkt))
        return RV_ERROR_UNKNOWN;

    globals = (tpktGlobals *)emaGetUserData((EMAElement)tpkt);
    if ((globals == NULL) || (tpkt->state == tpktClosed))
    {
        emaReleaseLocked((EMAElement)tpkt);
        return RV_ERROR_UNKNOWN;
    }

    if (tpkt->recv.hRpool == NULL)
    {
        tpkt->context = context;
        tpkt->eventHandler = eventHandler;
    }

    /* Trigger the evRead event mechanism */
    if (tpkt->state != tpktWaiting)
        tpkt->event = (RvSelectEvents) (tpkt->event | RvSelectRead | RvSelectClose);
    else
    {
        /* If we got no message yet, we don't wait for a close event */
        tpkt->event = (RvSelectEvents) (tpkt->event | RvSelectRead);
    }
    RvSelectUpdate(globals->pSelEngine, &tpkt->connection.fd, tpkt->event, tpktEvent);

    emaReleaseLocked((EMAElement)tpkt);
    return 0;
}

/*******************************************************************************************
 * tpktRecvIntoRpool
 *
 * Purpose: to initiate the reading of a new message while seting a new CallBack routine
 *          to be called when data arrives.
 *          The routine handles any problems that arise from the fact that an rpool message
 *          may be not contiguous.
 *          This routine is called on each EvRead event, but NOT on EvAccept since it's
 *          too slow.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *          buffer - The buffer into which to read the received data
 *          length - Maximal size of the buffer
 *
 * Return Value: -1 : on Error;
 *        0 : on not receiving a full buffer or a full message;
 *       >0 : Number of bytes read (may be either the length of the buffer
 *            in case of an incomplete message or the message length).
 *******************************************************************************************/
int tpktRecvIntoRpool(
    HTPKT       hTpkt,
    HRPOOL      hRpool,
    HRPOOLELEM  message,
    RvLock      *rpoolLock)
{
    tpktInfo* tpkt=(tpktInfo*)hTpkt;
    tpktGlobals* globals;
    RvStatus status;

    RvSize_t rcv = 0;
    RvBool finished = RV_FALSE;
    int retVal = 0;

    if (!emaMarkLocked((EMAElement)tpkt)) return RV_ERROR_UNKNOWN;
    globals = (tpktGlobals *)emaGetUserData((EMAElement)tpkt);
    if ((globals == NULL) || (tpkt->recv.hRpool != NULL) || (tpkt->state == tpktIdle) || (tpkt->state == tpktClosed))
    {
        emaReleaseLocked((EMAElement)tpkt);
        return RV_ERROR_UNKNOWN;
    }

    /* update the tpkt object with the rpool data */
    tpkt->recv.hRpool    = hRpool;
    tpkt->recv.message   = message;
    tpkt->recv.offset    = 0;
    tpkt->recv.rpoolLock = rpoolLock;

    /* We can start getting the new message header */
    status = socketRecvTcpDirectIfConnected(globals, tpkt, tpkt->recv.header, TPKT_HEADER_SIZE, &rcv);
#ifdef TPKT_CPU_COUNTER_CHECK
    if ((status == RV_OK) && (rcv == 0))
    {
        tpkt->cpuCounter++;
        if (tpkt->cpuCounter > cpuCounterMax)
        {
            /* Out of bounds - treat as error */
            status = RV_ERROR_OUTOFRANGE;
        }
    }
    else
        tpkt->cpuCounter = 0;
#endif /* TPKT_CPU_COUNTER_CHECK */

    if (status != RV_OK)
    {
        /* We had an error, and the transnet is going to free the rpool buffer -
           we should do the same before we let it know of the error */
        tpkt->recv.hRpool = NULL;
        tpkt->state = tpktReportClose; /* Indicate for next round to close the connection on a read event */
        emaReleaseLocked((EMAElement)tpkt);
        return RV_ERROR_UNKNOWN;
    }

    tpkt->recv.offset = rcv;

    /* we have a valid header */
    if (tpkt->recv.offset == TPKT_HEADER_SIZE)
    {
        /* Calculate the message length */
        int tpktLen = (((int)(tpkt->recv.header[2])<<8)+tpkt->recv.header[3])-TPKT_HEADER_SIZE;
        int recvInt;

        RvLogDebug(&globals->log, (&globals->log,
            "Received header: %02X %02X %02X %02X",
            tpkt->recv.header[0], tpkt->recv.header[1], tpkt->recv.header[2], tpkt->recv.header[3]));

 		//gaoshizhong add 针对收到TPKT的打洞包（空包）进行处理
 		//if (0 == tpktLen)
 		//{
 		//	tpkt->recv.header[0]=3;
 		//	tpkt->recv.hRpool = NULL;
 
 		//	if (tpkt->state == tpktWaiting)
 		//		tpkt->state = tpktConnected;
 
   //          emaReleaseLocked((EMAElement)tpkt);
   //          return tpktLen;
 		//} //end not use temp

        if (tpktLen > 0)
        {
            /* allocate the rest of the rpool message by the size in the header */
            RvLockGet(tpkt->recv.rpoolLock, globals->logMgr);
            recvInt = rpoolRealloc(tpkt->recv.hRpool, tpkt->recv.message, tpktLen);
            RvLockRelease(tpkt->recv.rpoolLock, globals->logMgr);

            if (recvInt >= 0)
            {
                /* mark as new message and calculate how many bytes we may attempt to read */
                tpkt->recv.header[0]=3;

                /* Do the actual reading */
                recvInt = socketRecvTcpIfConnected(globals, tpkt, &finished);
                if (recvInt >= 0)
                {
                    /* we read all we requested */
                    if (finished)
                    {
                        /* If we are waiting for the first message to arrive, then it is here... */
                        if (tpkt->state == tpktWaiting)
                            tpkt->state = tpktConnected;

                        tpkt->recv.hRpool = NULL;
                        retVal = tpktLen;
                    }
                }
                else
                {
                    tpkt->recv.hRpool = NULL;
                    tpkt->state = tpktReportClose; /* Indicate for next round to close the connection on a read event */
                    retVal = RV_ERROR_UNKNOWN;
                }
            }
            else
            {
                tpkt->recv.hRpool = NULL;
                retVal = RV_ERROR_UNKNOWN;
            }
        }
        else
        {
            tpkt->recv.hRpool = NULL;
            retVal = RV_ERROR_UNKNOWN;
        }
    }

    emaReleaseLocked((EMAElement)tpkt);
    return retVal;
}


/*******************************************************************************************
 * tpktGetContext
 *
 * Purpose: return the context of the given tpkt object. The context is the host of this
 *          connection.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: Context on success, NULL on failure
 *******************************************************************************************/
void* tpktGetContext(
    IN  HTPKT   hTpkt)
{
    return emaGetApplicationHandle((EMAElement)hTpkt);
}


/*******************************************************************************************
 * tpktGetConnection
 *
 * Purpose: return the connection of the given tpkt object.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: IO connection on success, NULL on failure
 *******************************************************************************************/
RvH323Connection* tpktGetConnection(
    IN  HTPKT   hTpkt)
{
    tpktInfo* tpkt = (tpktInfo *)hTpkt;

    return &tpkt->connection;
}


/*******************************************************************************************
 * tpktGetFromConnection
 *
 * Purpose: return the TPKT object from a connection.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: TPK handle on success, NULL on failure
 *******************************************************************************************/
HTPKT tpktGetFromConnection(
    IN  RvH323Connection*   connection)
{
    tpktInfo*   tpkt;
    tpkt = RV_GET_STRUCT(tpktInfo, connection, connection);
    return (HTPKT)tpkt;
}


/*******************************************************************************************
 * tpktIsConnected
 *
 * Purpose: return isConnected member of the TPKT object.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: isConnected
 *******************************************************************************************/
RvBool tpktIsConnected(
    IN  HTPKT   hTpkt)
{
    tpktInfo* tpkt = (tpktInfo *)hTpkt;

    return ((tpkt != NULL) && ((tpkt->state == tpktConnected) || (tpkt->state == tpktWaiting)));
}


/*******************************************************************************************
 * tpktGetTransGlobals
 *
 * Purpose: get the transport globals from a TPKT element.
 *
 * Input:   hTpkt - A handle to the TPKT element
 *
 * Return Value: transport globals
 *******************************************************************************************/
void* tpktGetTransGlobals(
    IN  HTPKT   hTpkt)
{
    tpktGlobals * globals = (tpktGlobals *)emaGetUserData((EMAElement)hTpkt);
    return (void*)globals->transGlobals;
}


/*******************************************************************************************
 * tpktSetClosed
 *
 * Purpose: set tpkt state to closed
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: RV_TRUE if set, otherwise - RV_FALSE
 *******************************************************************************************/
RvBool tpktSetClosed(IN  HTPKT   hTpkt)
{
    tpktInfo* tpkt = (tpktInfo *)hTpkt;

    if (tpkt != NULL)
    {
        tpkt->state = tpktClosed;
        return RV_TRUE;
    }
    else
        return RV_FALSE;
}



#ifdef __cplusplus
}
#endif



