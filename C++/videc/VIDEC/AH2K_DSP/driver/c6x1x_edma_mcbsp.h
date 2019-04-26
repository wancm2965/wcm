/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/* 
 *  ======== c6x1x_edma_mcbsp.h ========
 */

#ifndef C6X1X_EDMA_MCBSP_
#define C6X1X_EDMA_MCBSP_

#include <iom.h>

#include <csl.h>
#include <csl_edma.h>
#include <csl_mcbsp.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Version number definition */
#define C6X1X_EDMA_MCBSP_VERSION_1 0xAB01

/* Driver function table to be used by applications. */
extern far IOM_Fxns C6X1X_EDMA_MCBSP_FXNS;

/* Device setup parameters */
typedef struct C6X1X_EDMA_MCBSP_DevParams {
    Int versionId;   /* Set to the version used by the application */
    Bool cacheCalls; /* Set to TRUE if buffers are in external memory */
    Bool enableSrgr; /* Set to TRUE if internal clock is to be generated */
    Bool enableFsg;  /* Set to TRUE if internal framesync to be generated */
    Int irqId;       /* IRQ number used for EDMA interrupt */
    MCBSP_Config *mcbspCfgPtr; /* Configuration to use for the McBSP */
    Uns intrMask;    /* Interrupt mask, set while executing EDMA int ISR */
} C6X1X_EDMA_MCBSP_DevParams;

/* Channel setup parameters */
typedef struct C6X1X_EDMA_MCBSP_ChanParams {
    Uns tdmChans;    /* Number of TDM channels used by the McBSP */
    EDMA_Config *edmaCfgPtr; /* Configuration to use for the EDMA channel */
} C6X1X_EDMA_MCBSP_ChanParams;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* C6X1X_EDMA_MCBSP_ */

