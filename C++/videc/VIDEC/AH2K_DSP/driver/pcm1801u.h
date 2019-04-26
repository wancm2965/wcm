/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/* 
 *  ======== teb6416_edma_pcm3002.h ========
 */

#ifndef TEB6416_EDMA_PCM3002_
#define TEB6416_EDMA_PCM3002_

#include <iom.h>
#include <csl_edma.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version number definition */
#define TEB6416_EDMA_PCM3002_VERSION_1 0xAB01

#define TEB6416_EDMA_PCM3002_INTR_MASK_DEFAULT 1
/*
 * pcm3002 specific #defines
 */
#define TEB6416_EDMA_PCM3002_MODE_REG0        0
#define TEB6416_EDMA_PCM3002_MODE_REG1        1
#define TEB6416_EDMA_PCM3002_MODE_REG2        2
#define TEB6416_EDMA_PCM3002_MODE_REG3        3

#define TEB6416_EDMA_PCM3002_MR0_LDL          0x0100
#define TEB6416_EDMA_PCM3002_MR0_AL           0x00FF
#define TEB6416_EDMA_PCM3002_MR0_DEFAULT      0x00FF

#define TEB6416_EDMA_PCM3002_MR1_LDR          0x0100
#define TEB6416_EDMA_PCM3002_MR1_AR           0x00FF
#define TEB6416_EDMA_PCM3002_MR1_DEFAULT      0x00FF

#define TEB6416_EDMA_PCM3002_MR2_PDAD         0x0100
#define TEB6416_EDMA_PCM3002_MR2_BYPS         0x0080
#define TEB6416_EDMA_PCM3002_MR2_PDDA         0x0040
#define TEB6416_EDMA_PCM3002_MR2_ATC          0x0020
#define TEB6416_EDMA_PCM3002_MR2_IZD          0x0010
#define TEB6416_EDMA_PCM3002_MR2_OUT          0x0008
#define TEB6416_EDMA_PCM3002_MR2_DEM_44KH_ON  0x0000
#define TEB6416_EDMA_PCM3002_MR2_DEM_OFF      0x0002
#define TEB6416_EDMA_PCM3002_MR2_DEM_48KHZ_ON 0x0004
#define TEB6416_EDMA_PCM3002_MR2_DEM_32KHZ_ON 0x0006
#define TEB6416_EDMA_PCM3002_MR2_MUT          0x0001
#define TEB6416_EDMA_PCM3002_MR2_DEFAULT      0x0002

#define TEB6416_EDMA_PCM3002_MR3_LOP          0x0020
#define TEB6416_EDMA_PCM3002_MR3_FMT_16B_R    0x0000 
#define TEB6416_EDMA_PCM3002_MR3_FMT_20B_R    0x0004 
#define TEB6416_EDMA_PCM3002_MR3_FMT_20B_L    0x0008 
#define TEB6416_EDMA_PCM3002_MR3_FMT_20B_I2S  0x000C 
#define TEB6416_EDMA_PCM3002_MR3_LRP          0x0002
#define TEB6416_EDMA_PCM3002_MR3_DEFAULT      0x0000

#define TEB6416_EDMA_PCM3002_LEFT_CHANNEL     1      // Left channel only
#define TEB6416_EDMA_PCM3002_RIGHT_CHANNEL    2      // Right channel only
#define TEB6416_EDMA_PCM3002_BOTH_CHANNEL     3      // Both left and right channel

#define TEB6416_EDMA_PCM3002_NUMREGS          4

/* Default IRQ number for EDMA is 8 */          
#define TEB6416_EDMA_PCM3002_EDMA_ID_DEFAULT  8

#define TEB6416_PCM3002_DEFAULTPARAMS {                   \
        TEB6416_EDMA_PCM3002_MR0_DEFAULT,                 \
        TEB6416_EDMA_PCM3002_MR1_DEFAULT,                 \
        TEB6416_EDMA_PCM3002_MR2_DEFAULT,                 \
        TEB6416_EDMA_PCM3002_MR3_DEFAULT                  \
}                                                    
#define TEB6416_EDMA_PCM3002_DEFAULT_DEVPARAMS {          \
    TEB6416_EDMA_PCM3002_VERSION_1, /* Version number */  \
    TRUE, /* Buffers are in external memory by default */ \
    TEB6416_EDMA_PCM3002_EDMA_ID_DEFAULT,                 \
    TEB6416_PCM3002_DEFAULTPARAMS,                        \
    TEB6416_EDMA_PCM3002_INTR_MASK_DEFAULT,               \
    EDMA_OPT_PRI_HIGH                                     \
}

/* Driver function table to be used by applications */
extern far IOM_Fxns TEB6416_EDMA_PCM3002_FXNS;
extern Void TEB6416_EDMA_PCM3002_init();

/* Device setup parameters */
typedef struct TEB6416_EDMA_PCM3002_DevParams {
    Int versionId; /* Set to the version number used by the application */
    Bool cacheCalls; /* Set to TRUE if buffers are in external memory */
    Int irqId;       /* IRQ number to use for EDMA interrupt */
    Int reg[TEB6416_EDMA_PCM3002_NUMREGS]; /* PCM3002 registers */
    Uns intrMask; /* Interrupt mask, set while executing EDMA int ISR */
    Int edmaPriority;/* priority of EDMA transfers */
} TEB6416_EDMA_PCM3002_DevParams;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* TEB6416_EDMA_PCM3002_ */

