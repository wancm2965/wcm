/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/* 
 *  ======== c6x1x_edma_mcbsp.c ========
 * 
 *  Generic McBSP driver for the TMS320C6x1x series. Uses the EDMA.
 */


#include <std.h>

#include <atm.h>
#include <iom.h>
#include <que.h>
#include <hwi.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_edma.h>
#include <csl_cache.h>

#include <c6x1x_edma_mcbsp.h>

/* Maximum number of EDMA jobs linked at a time (Must be 2). */
#define MAXLINKCNT 2

/* Used as index since IOM mode is a bit mask and not an index */
#define INPUT 0
#define OUTPUT 1

/* States for chanCleanUp() */
#define SETFALSE 1
#define FREETCC 2
#define FREETABLE 3
#define FREETABLEEX 4
#define DELCHAN 5

/* Macro to increment and return the indice that ranges over MAXLINKCNT */
#define nextIndex(index) (index) ^ 1

/* Number of ports available */
#define NUMPORTS  _MCBSP_PORT_CNT 

/* Number of channels per port (one input and one output channel) */
#define NUMCHANS 2

/* Structure containing channel specific variables */
typedef struct ChanObj {
    Uns inUse;                /* True if the channel is in use */
    Int mode;                 /* Input or output channel */
    struct PortObj *port;     /* Pointer to the port which owns this chan */
    EDMA_Handle xferPram;     /* Handle to transfer PaRAM */
    EDMA_Handle pramTbl[MAXLINKCNT]; /* Handles to link PaRAMs */
    EDMA_Handle prevPramPtr;  /* Points to the PaRAM last used */
    EDMA_Handle loophEdma;    /* Handle to the Loop job PaRAM */
    IOM_Packet *flushPacket;  /* Holds the flushpacket (if any) */
    IOM_Packet *abortPacket;  /* Holds the abortpacket (if any) */
    IOM_Packet *packetList[MAXLINKCNT]; /* Holds linked  packets */
    QUE_Obj packetQueue;      /* Holds submitted but not linked packets */
    Int submitCount;          /* Number of submit calls pending */
    Int writeIndex;           /* Index of next PaRAM to write to */
    Int readIndex;            /* Index of next PaRAM to read from */
    Int tcc;                  /* Channel transfer complete code */
    IOM_TiomCallback cbFxn;   /* Called when I/O complete */
    Ptr cbArg;                /* Argument to callback function */
} ChanObj, *ChanHandle;

/* Structure containing port specific variables */
typedef struct PortObj {
    Uns inUse;                /* True if the port is in use */
    Int devid;                /* The device id passed to mdBindDev() */
    Bool cacheCalls;          /* Submitted buffers are in cacheable memory */
    Uint32 enableMask;        /* Holds enable Srgr and Fsgr if applicable */
    MCBSP_Handle hMcbsp;      /* CSL Device handle */
    ChanObj chans[NUMCHANS];  /* The channels associated with the port */
} PortObj, *PortHandle;

/* Declare the port and channel structures */
static PortObj ports[NUMPORTS];

/* Define EDMA Event Id's Array */
static Uns eventIds[NUMPORTS][2] = {
    { EDMA_CHA_REVT0, EDMA_CHA_XEVT0 },
    { EDMA_CHA_REVT1, EDMA_CHA_XEVT1 },
#if NUMPORTS == 3
    { EDMA_CHA_REVT2, EDMA_CHA_XEVT2 }
#endif
};

/*
 * Forward declaration of the IOM interface functions. They are only
 * exposed via the IOM function table to avoid namespace pollution.
 */
static Int mdBindDev(Ptr *devp, Int devid, Ptr devParams);
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
                        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg);
static Int mdDeleteChan(Ptr chanp);
static Int mdSubmitChan(Ptr chanp, IOM_Packet *packet);
static Int mdUnBindDev(Ptr devp);

/* Public IOM interface table */
IOM_Fxns C6X1X_EDMA_MCBSP_FXNS = {
    &mdBindDev,
    &mdUnBindDev,
    IOM_CONTROLCHANNOTIMPL,
    &mdCreateChan,
    &mdDeleteChan,
    &mdSubmitChan
};

/* Local function prototypes */
static Void isrInput(Int tcc);
static Void isrOutput(Int tcc);
static Void chanCleanUp(ChanHandle chan, Uns state);
static Void isrCommon(ChanHandle chan);
static Void linkPacket(ChanHandle chan, IOM_Packet *packet);

/* Local driver variables. */
static Uint32 loopDstBuf = 0;
static Uint32 loopSrcBuf = 0;


/*
 * ======== chanCleanUp ========
 * Cleans up the channel resources.
 */
static Void chanCleanUp(ChanHandle chan, Uns state)
{
    switch(state) {
    case DELCHAN:
        /* Close the EDMA channel */
        EDMA_close(chan->xferPram);

        /* Disable transfer interrupts from the EDMA */
        EDMA_intDisable(chan->tcc);

    case FREETABLEEX:
        /* Free the EDMA link PaRAM tables */
        EDMA_freeTableEx(MAXLINKCNT, chan->pramTbl);
    case FREETABLE:
        /* Free the loop EDMA PaRAM table */
        EDMA_freeTable(chan->loophEdma);
    case FREETCC:
        /* Free the transfer complete code */
        EDMA_intFree(chan->tcc);
    case SETFALSE:
        /* Mark the channel as closed */
        chan->inUse = FALSE;
        break;
    }
}
/*
 * ======== isrCommon ========
 * Shared ISR code between input and output. Processes a normal EDMA job.
 */
static Void isrCommon(ChanHandle chan)
{
    IOM_Packet *packet;
    Int cnt;

    /* Check to see if this is a completed async abort call */
    if (chan->abortPacket) {
        /* Discard all packets in transmission or queued up */
        if (chan->submitCount > MAXLINKCNT) {
            cnt = MAXLINKCNT;
        }
        else {
            cnt = chan->submitCount;
        }

        while (cnt > 0) {
            packet = chan->packetList[chan->readIndex];
            packet->status = IOM_ABORTED;
            (*chan->cbFxn)(chan->cbArg, packet);

            chan->readIndex = nextIndex(chan->readIndex);
            cnt--;
        }

        while (!QUE_empty(&chan->packetQueue)) {
            packet = QUE_dequeue(&chan->packetQueue);
            packet->status = IOM_ABORTED;
            (*chan->cbFxn)(chan->cbArg, packet);
        }

        /* Reset the driver channel state */
        chan->writeIndex = 0;
        chan->readIndex = 0;
        chan->submitCount = 0;

        chan->abortPacket->status = IOM_COMPLETED;
        (*chan->cbFxn)(chan->cbArg, chan->abortPacket);
        chan->abortPacket = NULL;
        return;
    }

    /* Fetch the completed packet */
    packet = chan->packetList[chan->readIndex];

    chan->readIndex = nextIndex(chan->readIndex);

    /* Mark the packet as completed */
    packet->status = IOM_COMPLETED;

    /* Call the callback function */
    (*chan->cbFxn)(chan->cbArg, packet);

    chan->submitCount--;

    /*
     * See if there are any unlinked packets in the packetQueue
     * and if so link them.
     */
    if (chan->submitCount >= MAXLINKCNT) {
        packet = QUE_dequeue(&chan->packetQueue);
        linkPacket(chan, packet);
    }
}

/*
 * ======== isrInput ========
 * The input isr called from the EDMA dispatcher every time an input
 * EDMA job completes.
 */
static Void isrInput(Int tcc)
{
    ChanHandle chan;
    Int portNbr;

    /* Check which port was responsible for the interrupt */
    for (portNbr = 0; portNbr < NUMPORTS; portNbr++) {
        chan = &ports[portNbr].chans[INPUT];

        if (chan->tcc == tcc && chan->inUse) {

            if (EDMA_RGETH(chan->xferPram, DST) == (Uint32) &loopDstBuf &&
                chan->submitCount > 1 && !chan->abortPacket) {
                /*
                 * An emulation halt has occured with more than 1 job
                 * submitted. Link the currently executing job (the Loop job)
                 * to the first of the linked jobs which hadn't been called
                 * back. This way we still have the same number of submitted
                 * jobs after the execution continues as we had before the
                 * emulation halt (breakpoint) occured (this preserves double
                 * buffering if used).
                 */
                EDMA_disableChannel(chan->xferPram);
                EDMA_link(chan->xferPram, chan->pramTbl[chan->readIndex]);
                EDMA_enableChannel(chan->xferPram);
            }
            else {
                /* Call the common ISR code for a finished normal EDMA job */
                isrCommon(chan);
            }
        }
    }
}

/*
 * ======== isrOutput ========
 * The output isr called from the EDMA dispatcher every time an output
 * EDMA job completes.
 */
static Void isrOutput(Int tcc)
{
    ChanHandle chan;
    Int portNbr;

    /* Check which port was responsible for the interrupt */
    for (portNbr = 0; portNbr < NUMPORTS; portNbr++) {
        chan = &ports[portNbr].chans[OUTPUT];

        if (chan->tcc == tcc && chan->inUse) {

            if (EDMA_RGETH(chan->xferPram, SRC) == (Uint32) &loopSrcBuf &&
                chan->submitCount > 1 && !chan->abortPacket) {
                /*
                 * An emulation halt has occured with more than 1 job
                 * submitted. Link the currently executing job (the Loop job)
                 * to the first of the linked jobs which hadn't been called
                 * back. This way we still have the same number of submitted
                 * jobs after the execution continues as we had before the
                 * emulation halt (breakpoint) occured (this preserves double
                 * buffering if used).
                 */
                EDMA_link(chan->xferPram, chan->pramTbl[chan->readIndex]);
            }
            else {
                /* Call the common ISR code for a finished normal EDMA job */
                isrCommon(chan);

                /* Check to see if an async flush has completed */
                if (chan->submitCount == 0 && chan->flushPacket) {
                    chan->flushPacket->status = IOM_COMPLETED;
                    (*chan->cbFxn)(chan->cbArg,chan->flushPacket);
                    chan->flushPacket = NULL;
                }
            }
        }
    }
}

/*
 * ======== linkPacket ========
 * Links a packet with the EDMA. When called by mdSubmitChan() it is called
 * with all interrupts disabled, but when called by an ISR only the EDMA IRQ
 * is disabled.
 */
static Void linkPacket(ChanHandle chan, IOM_Packet *packet)
{
    EDMA_Handle pramPtr;

    /* Store the packet in the packetList */
    chan->packetList[chan->writeIndex] = packet;

    /* Set up pointer to link PaRAM to write submit job info to */
    pramPtr = chan->pramTbl[chan->writeIndex];
    chan->writeIndex = nextIndex(chan->writeIndex);

    /* Load the buffer pointer into the EDMA */
    if (chan->mode == INPUT) {
        EDMA_RSETH(pramPtr, DST, (Uint32) packet->addr);
    }
    else {
        EDMA_RSETH(pramPtr, SRC, (Uint32) packet->addr);
    }

    /*
     * Load the transfer count (in samples) into the EDMA. Use the ESIZE
     * field of the EDMA job to calculate number of samples.
     */
    EDMA_RSETH(pramPtr, CNT, (Uint32) packet->size >>
                                      (2 - EDMA_FGETH(pramPtr, OPT, ESIZE)));
	//EDMA_RSETH(pramPtr, CNT, (Uint32) packet->size >> 2);


    /*
     * Link to loop EDMA job upon termination. This way we won't
     * loose the frame sync if the channel is starved.
     */
    EDMA_link(pramPtr, chan->loophEdma);

    /* Disable the EDMA channel to make sure current job doesn't complete */
    EDMA_disableChannel(chan->xferPram);

    /*
     * Link the currently executing job to the new job. This can be
     * either the loop EDMA job or a real data EDMA job.
     */
    EDMA_link(chan->xferPram, pramPtr);

    if (chan->submitCount > 0) {
        /*
         * We need to link the parameter space corresponding to the running
         * job so that if a breakpoint occurs, we know how to recover.
         */
        EDMA_link(chan->prevPramPtr, pramPtr);
    }

    /* Reenable the EDMA channel */
    EDMA_enableChannel(chan->xferPram);

    /* Save the new job for the loop above for next time */
    chan->prevPramPtr = pramPtr;
}

/*
 * ======== mdBindDev ========
 * This function allocates and configures the McBSP port specified by devid.
 */
static Int mdBindDev(Ptr *devp, Int devid, Ptr devParams)
{
    Uns old;
    PortHandle port;
    HWI_Attrs hwiAttrs;
    C6X1X_EDMA_MCBSP_DevParams *params =
        (C6X1X_EDMA_MCBSP_DevParams *) devParams;

    /* This driver must receive a valid devparams */
    if (params == NULL) {
        return (IOM_EBADARGS);
    }
    
    /* Check if the driver is supporting the version used by
     * the application */
    if(params->versionId != C6X1X_EDMA_MCBSP_VERSION_1){ 
        /* Unsupported version */
        return(IOM_EBADARGS);
    }

    /* Get the device parameters of the specified port */
    port = &ports[devid];

    /* Mark the port as in use */
    old = ATM_setu(&(port->inUse), TRUE);

    /* Check if the port was already bound */
    if (old) {
        return (IOM_EALLOC);
    }

    /* Map the supplied IRQ to the EDMA event */
    IRQ_map(IRQ_EVT_EDMAINT, params->irqId);

    /* set the interrupt mask */
    hwiAttrs.intrMask = params->intrMask;
    hwiAttrs.ccMask = IRQ_CCMASK_NONE; /* the default value */
    hwiAttrs.arg = NULL;
    /* Plug the EDMA dispatcher into the HWI dispatcher */
    HWI_dispatchPlug(params->irqId, (Fxn)EDMA_intDispatcher, -1, &hwiAttrs);

    /* Initialise the enableMask */
    port->enableMask = 0;

    /* Set the McBSP sample rate generator to be enabled */
    if (params->enableSrgr) {
        port->enableMask |= MCBSP_SRGR_START;
    }

    /* Set the McBSP frame sync generator to be enabled */
    if (params->enableFsg) {
        port->enableMask |= MCBSP_SRGR_FRAMESYNC;
    }

    /* True if buffers are in external memory */
    port->cacheCalls = params->cacheCalls;

    /* Store the devid */
    port->devid = devid;

    /* Open and reset the McBSP */
    port->hMcbsp = MCBSP_open(devid, MCBSP_OPEN_RESET);

    if (port->hMcbsp == INV) {
        return (IOM_EALLOC);
    }

    /* Configure the McBSP with the supplied configuration */
    MCBSP_config(port->hMcbsp, params->mcbspCfgPtr);

    /* Return the device handle and a status code for success */
    *devp = port;
    return (IOM_COMPLETED);
}

/*
 * ======== mdCreateChan ========
 * This function creates and configures a device channel.
 */
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
                        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg)
{
    PortHandle port = (PortHandle) devp;
    C6X1X_EDMA_MCBSP_ChanParams *params =
        (C6X1X_EDMA_MCBSP_ChanParams *) chanParams;
    ChanHandle chan;
    Uns mcbspAddr;
    Uns old;
    Uns esize;
    Int i;

    /*
     * Configuration structure for the loop EDMA job. If the McBSP
     * is left to free running, this loop job is running when there is
     * no data to transfer. This is useful if the McBSP is externally
     * clocked.  If the driver is starved or if an emulation halt
     * (breakpoint) occurs, the frame sync will still be correct when
     * the driver continues transmitting data.
     */
    EDMA_Config loopEdmaCfg = {
        EDMA_FMKS(OPT, PRI, HIGH)           |
        //EDMA_FMKS(OPT, ESIZE, DEFAULT)      |
		EDMA_FMKS(OPT, ESIZE, 32BIT)      |
        EDMA_FMKS(OPT, 2DS, NO)             |
        EDMA_FMKS(OPT, SUM, NONE)           |
        EDMA_FMKS(OPT, 2DD, NO)             |
        EDMA_FMKS(OPT, DUM, NONE)           |
        EDMA_FMKS(OPT, TCINT, NO)           |
        EDMA_FMKS(OPT, TCC, DEFAULT)        |
        EDMA_FMKS(OPT, LINK, YES)           |
        EDMA_FMKS(OPT, FS, NO),

        EDMA_FMK (SRC, SRC, NULL),

        EDMA_FMK (CNT, FRMCNT, NULL)        |
        EDMA_FMK (CNT, ELECNT, NULL),

        EDMA_FMKS(IDX, FRMIDX, DEFAULT)     |
        EDMA_FMKS(IDX, ELEIDX, DEFAULT),

        EDMA_FMK (DST, DST, NULL),

        EDMA_FMK (RLD, ELERLD, NULL)        |
        EDMA_FMK (RLD, LINK, NULL)
    };

    /* This driver needs a valid channel parameter structure passed */
    if (params == NULL) {
        return (IOM_EBADARGS);
    }

    /* Use own indexes since IOM mode is a bit mask and not an index */
    if (mode == IOM_INPUT) {
        chan = &port->chans[INPUT];
        chan->mode = INPUT;
    }
    else {
        chan = &port->chans[OUTPUT];
        chan->mode = OUTPUT;
    }

    /* Mark the channel as used */
    old = ATM_setu(&(chan->inUse), TRUE);

    /* Make sure the channel wasn't already created */
    if (old) {
        return (IOM_EALLOC);
    }

    /* Initialise the channel structure */
    chan->cbFxn = cbFxn;
    chan->cbArg = cbArg;
    chan->port = port;
    chan->writeIndex = 0;
    chan->readIndex = 0;
    chan->submitCount = 0;
    chan->flushPacket = NULL;
    chan->abortPacket = NULL;

    /* Initialize the packet queue */
    QUE_new(&chan->packetQueue);

#if 1
    /*
     * Set the number of elements (corresponding to number of McBSP TDM
     * channels) used in the Loop job to preserve the frame sync.
     */
    loopEdmaCfg.cnt = EDMA_FMK(CNT, ELECNT, params->tdmChans);

    /* Use the same sample size in the Loop job as in normal jobs */
    esize = EDMA_FGETA(params->edmaCfgPtr, OPT, ESIZE);
    EDMA_FSETA(&loopEdmaCfg, OPT, ESIZE, esize);
#else
    loopEdmaCfg.cnt = EDMA_FMK(CNT, ELECNT, 4);

    /* Use the same sample size in the Loop job as in normal jobs */
   
    EDMA_FSETA(&loopEdmaCfg, OPT, ESIZE, 0);
#endif
    /* Allocate a TCC for the EDMA */
    chan->tcc = EDMA_intAlloc(-1);

    /* If tcc > 15 (possible on c64x) we abort. */
    if (chan->tcc == -1 || chan->tcc > 15) {
        chanCleanUp(chan, SETFALSE);
        return (IOM_EALLOC);
    }

    /* Allocate an EDMA PaRAM for the Loop EDMA job. */
    chan->loophEdma = EDMA_allocTable(-1);

    if (chan->loophEdma == EDMA_HINV) {
        chanCleanUp(chan, FREETCC);
        return (IOM_EALLOC);
    }

    /*
     * Allocate EDMA PaRAM link area based on max number of
     * submits possible.
     */
    if (EDMA_allocTableEx(MAXLINKCNT, chan->pramTbl) != MAXLINKCNT) {
        chanCleanUp(chan, FREETABLE);
        return (IOM_EALLOC);
    }

    /* 
     * Process the big endian problem for McBSP 
     * See EDMA App Note (spra636) for details setting in big endian
     */
    if (chan->mode == INPUT) {
        mcbspAddr = MCBSP_getRcvAddr(port->hMcbsp);
    }
    else {
        mcbspAddr = MCBSP_getXmtAddr(port->hMcbsp);
    }
#ifdef _BIG_ENDIAN
    if (esize == EDMA_OPT_ESIZE_8BIT) {
        mcbspAddr += 3;
    }
    else if (esize == EDMA_OPT_ESIZE_16BIT) {
        mcbspAddr += 2;
    }
#endif

    if (chan->mode == INPUT) {
        /* Put input specific parameters in the Loop EDMA config */
        loopEdmaCfg.src = mcbspAddr;
        loopEdmaCfg.dst = (Uint32) &loopDstBuf;

        params->edmaCfgPtr->src = mcbspAddr;

        /* Register our isrInput with the EDMA dispatcher */
        EDMA_intHook(chan->tcc, &isrInput);
    }
    else {
        /* Put output specific parameters in the Loop EDMA config */
        loopEdmaCfg.src = (Uint32) &loopSrcBuf;
        loopEdmaCfg.dst = mcbspAddr;

        params->edmaCfgPtr->dst = mcbspAddr;

        /* Register our isrOutput with the EDMA dispatcher */
        EDMA_intHook(chan->tcc, &isrOutput);
    }

    /* Open the EDMA channel */
    chan->xferPram = 
            EDMA_open(eventIds[port->devid][chan->mode], EDMA_OPEN_RESET);

    if (chan->xferPram == EDMA_HINV) {
        chanCleanUp(chan, FREETABLEEX);
        return (IOM_EALLOC);
    }

    /* Program the data EDMA job with the TCC */
    EDMA_FSETA(params->edmaCfgPtr, OPT, TCC, chan->tcc);

    /* Program the link PaRAMs with the config struct */
    for (i=0; i < MAXLINKCNT; i++) {
        EDMA_config(chan->pramTbl[i], params->edmaCfgPtr);
    }

    /* Program the Loop EDMA job with its config struct */
    EDMA_config(chan->loophEdma, &loopEdmaCfg);

    /*
     * Link the Loop EDMA job to itself to make it run
     * continuously when there is no data to transmit.
     */
    EDMA_link(chan->loophEdma, chan->loophEdma);

    /* Configure the EDMA channel to start with the Loop EDMA job */
    EDMA_config(chan->xferPram, &loopEdmaCfg);

    /* Transfer complete is edge triggered, so clear before enabling */
    EDMA_intClear(chan->tcc);
    EDMA_intEnable(chan->tcc);

    /* Enable the EDMA interrupt */
    IRQ_enable(IRQ_EVT_EDMAINT);

    /* Start the McBSP */
    if (chan->mode == INPUT) {
        MCBSP_start(port->hMcbsp, port->enableMask | MCBSP_RCV_START,
                    MCBSP_SRGR_DEFAULT_DELAY);
    }
    else {
        MCBSP_start(port->hMcbsp, port->enableMask | MCBSP_XMIT_START,
                    MCBSP_SRGR_DEFAULT_DELAY);
    }

    *chanp = (Ptr) chan;
    return (IOM_COMPLETED);
}

/*
 * ======== mdDeleteChan ========
 * This function frees a channel and all it's associated resources.
 */
static Int mdDeleteChan(Ptr chanp)
{
    ChanHandle chan = (ChanHandle) chanp;
    PortHandle port = chan->port;

    /* Clean up the channel resources */
    chanCleanUp(chan, DELCHAN);

    /*
     * Reset the McBSP transmitter or receiver. If the channel is
     * recreated, mdCreateChan() will reenable the transmitter/receiver
     * and by pulling it out of reset it will also restart the EDMA channel.
     */
    if (chan->mode == INPUT) {
        MCBSP_FSETSH(port->hMcbsp, SPCR, RRST, YES);
    }
    else {
        MCBSP_FSETSH(port->hMcbsp, SPCR, XRST, YES);
    }

    return (IOM_COMPLETED);
}

/*
 * ======== mdSubmitChan ========
 * This function transmits a buffer to or from the McASP using the EDMA.
 */
static Int mdSubmitChan(Ptr chanp, IOM_Packet *packet)
{
    ChanHandle chan = (ChanHandle) chanp;
    PortHandle port = chan->port;
    Uns imask;

    /* No packets can be submitted while abort or flush is active */
    if (chan->flushPacket || chan->abortPacket) {
        return (IOM_EBADIO);
    }

    /*
     * Check to see if an abort command has been issued. Note that
     * flushing the input channel is handled the same as abort.
     */
    if ((packet->cmd == IOM_FLUSH && chan->mode == INPUT) ||
        packet->cmd == IOM_ABORT) {

        /* Disable interrupts to protect submitCount */
        imask = HWI_disable();

        /* Store the abort packet for the ISR to check */
        if (chan->submitCount > 0) {
            chan->abortPacket = packet;

            /*
             * Disable the EDMA channel while linking the currently
             * executing job while linking it with the Loop job to make
             * sure the currently executing job doesn't complete before
             * the link is complete.
             */
            EDMA_disableChannel(chan->xferPram);
            EDMA_link(chan->xferPram, chan->loophEdma);
            EDMA_enableChannel(chan->xferPram);
        }

        /* Reenable interrupts */
        HWI_restore(imask);

        if (chan->abortPacket) {
            return(IOM_PENDING);
        }

        /* If there were no buffers in the channel, return synchronously */
        packet->status = IOM_COMPLETED;

        return (IOM_COMPLETED);
    }

    /* Check to see if the submitted packet is an output flush packet */
    if (packet->cmd == IOM_FLUSH && chan->mode == OUTPUT) {
        /* Disable interrupts to protect submitCount */
        imask = HWI_disable();

        /* Store the flush packet for the ISR to check */
        if (chan->submitCount > 0) {
            chan->flushPacket = packet;
        }

        /* Reenable interrupts */
        HWI_restore(imask);

        if (chan->flushPacket) {
            return(IOM_PENDING);
        }

        /* If there were no buffers in the channel, return synchronously */
        packet->status = IOM_COMPLETED;

        return (IOM_COMPLETED);
    }

    if (packet->cmd != IOM_READ && packet->cmd != IOM_WRITE) {
        /* Unsupported command passed */
        return (IOM_ENOTIMPL);
    }

    /* maintain cache coherency */
    if (chan->mode == INPUT) {
        /* CACHE uses words, and packet->size is in nmaus (bytes on c6x) */
        if (port->cacheCalls) {
            CACHE_clean(CACHE_L2, packet->addr, packet->size >> 2);
        }
    }
    else {
        /* CACHE uses words, and packet->size is in nmaus (bytes on c6x) */
        if (port->cacheCalls) {
            CACHE_flush(CACHE_L2, packet->addr, packet->size >> 2);
        }
    }

    /* Disable interrupts to protect submitCount */
    imask = HWI_disable();

    /*
     * If there is no space available for the new packet, put it on a
     * queue to be linked in when space is available. Otherwise link it in.
     */
    if (chan->submitCount >= MAXLINKCNT) {
        QUE_enqueue(&chan->packetQueue, packet);
    }
    else {
        linkPacket(chan, packet);
    }

    chan->submitCount++;

    HWI_restore(imask);

    return (IOM_PENDING);
}

/*
 * ======== mdUnBindDev ========
 * This function frees a port and all it's associated resources.
 */
static Int mdUnBindDev(Ptr devp)
{
    PortHandle port = (PortHandle) devp;

    port->inUse = FALSE;

    /* Close the McBSP */
    MCBSP_close(port->hMcbsp);

    return (IOM_COMPLETED);
}

/*
 * ======== C6X1X_EDMA_MCBSP_init ========
 * This function initializes the driver's data structures.
 */
#pragma CODE_SECTION(C6X1X_EDMA_MCBSP_init, ".text:init");
Void C6X1X_EDMA_MCBSP_init()
{
    PortHandle port;
    ChanHandle chan;
    Int i, j;
    /* Make sure initialization only happens once for this driver */
    static Bool curInit = FALSE;
    if (curInit) {
        return;
    }
    curInit = TRUE;

    for (i=0; i<NUMPORTS; i++) {
        port = &ports[i];

        port->inUse = FALSE;

        for (j=0; j<NUMCHANS; j++) {
            chan = &port->chans[j];

            chan->inUse = FALSE;
        }
    }
}

