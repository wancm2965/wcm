/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/* 
 *  ======== teb6416_edma_PCM1801.h ========
 */

#ifndef TEB6416_EDMA_PCM1801_
#define TEB6416_EDMA_PCM1801_

#include <iom.h>
#include <csl_edma.h>
#include <c6x1x_edma_mcasp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version number definition */
#define TEB6416_EDMA_PCM1801_VERSION_1 0xAB01

#define TEB6416_EDMA_PCM1801_INTR_MASK_DEFAULT 1
/*
 * PCM1801 specific #defines
 */
#define TEB6416_EDMA_PCM1801_MODE_REG0        0
#define TEB6416_EDMA_PCM1801_MODE_REG1        1
#define TEB6416_EDMA_PCM1801_MODE_REG2        2
#define TEB6416_EDMA_PCM1801_MODE_REG3        3

#define TEB6416_EDMA_PCM1801_MR0_LDL          0x0100
#define TEB6416_EDMA_PCM1801_MR0_AL           0x00FF
#define TEB6416_EDMA_PCM1801_MR0_DEFAULT      0x00FF

#define TEB6416_EDMA_PCM1801_MR1_LDR          0x0100
#define TEB6416_EDMA_PCM1801_MR1_AR           0x00FF
#define TEB6416_EDMA_PCM1801_MR1_DEFAULT      0x00FF

#define TEB6416_EDMA_PCM1801_MR2_PDAD         0x0100
#define TEB6416_EDMA_PCM1801_MR2_BYPS         0x0080
#define TEB6416_EDMA_PCM1801_MR2_PDDA         0x0040
#define TEB6416_EDMA_PCM1801_MR2_ATC          0x0020
#define TEB6416_EDMA_PCM1801_MR2_IZD          0x0010
#define TEB6416_EDMA_PCM1801_MR2_OUT          0x0008
#define TEB6416_EDMA_PCM1801_MR2_DEM_44KH_ON  0x0000
#define TEB6416_EDMA_PCM1801_MR2_DEM_OFF      0x0002
#define TEB6416_EDMA_PCM1801_MR2_DEM_48KHZ_ON 0x0004
#define TEB6416_EDMA_PCM1801_MR2_DEM_32KHZ_ON 0x0006
#define TEB6416_EDMA_PCM1801_MR2_MUT          0x0001
#define TEB6416_EDMA_PCM1801_MR2_DEFAULT      0x0002

#define TEB6416_EDMA_PCM1801_MR3_LOP          0x0020
#define TEB6416_EDMA_PCM1801_MR3_FMT_16B_R    0x0000 
#define TEB6416_EDMA_PCM1801_MR3_FMT_20B_R    0x0004 
#define TEB6416_EDMA_PCM1801_MR3_FMT_20B_L    0x0008 
#define TEB6416_EDMA_PCM1801_MR3_FMT_20B_I2S  0x000C 
#define TEB6416_EDMA_PCM1801_MR3_LRP          0x0002
#define TEB6416_EDMA_PCM1801_MR3_DEFAULT      0x0000

#define TEB6416_EDMA_PCM1801_LEFT_CHANNEL     1      // Left channel only
#define TEB6416_EDMA_PCM1801_RIGHT_CHANNEL    2      // Right channel only
#define TEB6416_EDMA_PCM1801_BOTH_CHANNEL     3      // Both left and right channel

#define TEB6416_EDMA_PCM1801_NUMREGS          4

/* Default IRQ number for EDMA is 8 */          
#define TEB6416_EDMA_PCM1801_EDMA_ID_DEFAULT  8

#define TEB6416_PCM1801_DEFAULTPARAMS {                   \
        TEB6416_EDMA_PCM1801_MR0_DEFAULT,                 \
        TEB6416_EDMA_PCM1801_MR1_DEFAULT,                 \
        TEB6416_EDMA_PCM1801_MR2_DEFAULT,                 \
        TEB6416_EDMA_PCM1801_MR3_DEFAULT                  \
}                                                    
#define TEB6416_EDMA_PCM1801_DEFAULT_DEVPARAMS {          \
    TEB6416_EDMA_PCM1801_VERSION_1, /* Version number */  \
    TRUE, /* Buffers are in external memory by default */               \
    /* Driver disable the McASP high frequecy rate generator */         \
    C6X1X_EDMA_MCASP_RCV/*,C6X1X_EDMA_MCASP_XMT	*/,                                          \
    /* Driver disable the McASP clk rate generator */                   \
    C6X1X_EDMA_MCASP_RCV/*,C6X1X_EDMA_MCASP_EXTERNAL	*/,                                          \
    /* Driver disable the McASP frame sync generator */                 \
    C6X1X_EDMA_MCASP_RCV/*,C6X1X_EDMA_MCASP_EXTERNAL	*/,                                          \
    TEB6416_EDMA_PCM1801_EDMA_ID_DEFAULT, /* Default IRQ number is 8 */  \
    -1, /* use default */                                               \
    -1, /* use default */                                               \
    NULL,  /* do not register mcasp event */                            \
    TEB6416_EDMA_PCM1801_INTR_MASK_DEFAULT,                              \
    TEB6416_EDMA_PCM1801_INTR_MASK_DEFAULT,                              \
    TEB6416_EDMA_PCM1801_INTR_MASK_DEFAULT,                                    \
}

/* Driver function table to be used by applications */
extern far IOM_Fxns TEB6416_EDMA_PCM1801_FXNS;
extern Void TEB6416_EDMA_PCM1801_init();

/* Device setup parameters */
typedef struct TEB6416_EDMA_PCM1801_DevParams {
    Int versionId;   /* Set to the version number used by the application */
    Bool cacheCalls; /* Set to TRUE if buffers are in external memory */
    Uns enableHclkg; /* Set VALUE for internal high frequency clock generator */
    Uns enableClkg;  /* Set VALUE for internal clock generator */
    Uns enableFsyncg;     /* Set VALUE for internal framesync generator */
    Int irqId;       /* IRQ number to use for EDMA interrupt */
    Int inEvtIrqId;  /* IRQ number used for McASP Event interrupt */
    Int outEvtIrqId; /* IRQ number used for McASP Event interrupt */
    C6X1X_EDMA_MCASP_EvtCallback *evtCallback; /* Register events callback */ 
    Uns inEvtIntrMask;    /* Interrupt mask, set while executing input ISR */
    Uns outEvtIntrMask;   /* Interrupt mask, set while executing output ISR */
    Uns edmaIntrMask;
} TEB6416_EDMA_PCM1801_DevParams;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* TEB6416_EDMA_PCM1801_ */

