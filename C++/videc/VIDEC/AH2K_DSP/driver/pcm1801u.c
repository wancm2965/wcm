/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/* 
 *  ======== teb6416_edma_pcm3002.c ========
 * 
 *  Driver for the pcm3002 codec on the TMS320C6416 teb. Requires the
 *  generic TMS320C6x1x McBSP driver.
 */

#include <std.h>

#include <iom.h>

#include <csl.h>
#include <csl_edma.h>
#include <csl_mcbsp.h>

#include "pcm1801u.h"
#include "c6x1x_edma_mcbsp.h"

#define GENERICDEVPARAMS {                                                  \
    C6X1X_EDMA_MCBSP_VERSION_1,/* Version number of mcbsp driver  */        \
    FALSE, /* True if buffers are in cacheable memory (set in mdBindDev) */ \
    FALSE, /* Driver should not enable the McBSP sample rate generator */   \
    FALSE, /* Driver should not enable the McBSP frame sync generator */    \
    0,     /* IRQ id to use for the EDMA (set in mdBindDev) */              \
    NULL,  /* McBSP configuration for the codec (set in mdBindDev) */       \
    TEB6416_EDMA_PCM3002_INTR_MASK_DEFAULT /* Interrupt mask */             \
}

/* TEB6416 specific pcm3002 #defines */
#define CPLDBASEADDR   0x60000000
#define REGCODECMTRDY  0x80
#define REGACODECL     2                // Codec low byte 
#define REGACODECH     3                // Codec high byte
#define REGAMISC       5                // Misc

/* Structure containing port specific variables */
typedef struct {
        Ptr c6x1xPortObj;
        Int edmaPriority;
} PortObj, *PortHandle;

/* Declare the port structures */
/* Only Mcbsp 0 is used */
static PortObj port;

TEB6416_EDMA_PCM3002_DevParams TEB6416_EDMA_PCM3002_DEVPARAMS =
                                   TEB6416_EDMA_PCM3002_DEFAULT_DEVPARAMS;

#if 0
/* Local function prototypes */
static Int writeMode(Int modeReg, Int modeVal);
static Int writeCodecMode(Int modeVal);
static Int isCodecReady(Void);
#endif

/*
 * Forward declaration of the IOM interface functions. They are only
 * exposed via the IOM function table to avoid namespace pollution.
 */
static Int mdBindDev(Ptr *devp, Int devid, Ptr devParams);
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
                        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg);

/*
 * The codec driver function table. Notice how we copy most functions
 * from the generic c6x1x edma mcbsp driver.
 */
IOM_Fxns TEB6416_EDMA_PCM3002_FXNS = {
    &mdBindDev,
    NULL,
    NULL,
    &mdCreateChan,
    NULL,
    NULL
};


/*
 * ======== mdBindDev ========
 * This function allocates and configures the pcm3002 codec.
 */
static Int mdBindDev(Ptr *devp, Int devid, Ptr devParams)
{
    Int result;
    TEB6416_EDMA_PCM3002_DevParams *params =
        (TEB6416_EDMA_PCM3002_DevParams *) devParams;
    C6X1X_EDMA_MCBSP_DevParams genericParams = GENERICDEVPARAMS;

#if 0
    MCBSP_Config mcbspCfg = {
        MCBSP_FMKS(SPCR, FREE, YES)             |
        MCBSP_FMKS(SPCR, SOFT, NO)              |
        MCBSP_FMKS(SPCR, FRST, YES)             |
        MCBSP_FMKS(SPCR, GRST, YES)             |
        MCBSP_FMKS(SPCR, XINTM, XRDY)           |
        MCBSP_FMKS(SPCR, XSYNCERR, NO)          |
        MCBSP_FMKS(SPCR, XRST, YES)             |
        MCBSP_FMKS(SPCR, DLB, OFF)              |
        MCBSP_FMKS(SPCR, RJUST, RZF)            |
        MCBSP_FMKS(SPCR, CLKSTP, DISABLE)       |
        MCBSP_FMKS(SPCR, DXENA, OFF)            |
        MCBSP_FMKS(SPCR, RINTM, RRDY)           |
        MCBSP_FMKS(SPCR, RSYNCERR, NO)          |
        MCBSP_FMKS(SPCR, RRST, YES),

        MCBSP_FMKS(RCR, RPHASE, SINGLE)         |
        MCBSP_FMKS(RCR, RFRLEN2, OF(0))         |
        //MCBSP_FMKS(RCR, RWDLEN2, 8BIT)          |
		MCBSP_FMKS(RCR, RWDLEN2, 32BIT)          |
        MCBSP_FMKS(RCR, RCOMPAND, MSB)          |
        MCBSP_FMKS(RCR, RFIG, NO)               |
        MCBSP_FMKS(RCR, RDATDLY, 0BIT)          |
        MCBSP_FMKS(RCR, RFRLEN1, OF(3))         |
        //MCBSP_FMKS(RCR, RWDLEN1, 16BIT)         |
		MCBSP_FMKS(RCR, RWDLEN1, 32BIT)         |
        MCBSP_FMKS(RCR, RWDREVRS, DISABLE),

        MCBSP_FMKS(XCR, XPHASE, SINGLE)         |
        MCBSP_FMKS(XCR, XFRLEN2, OF(0))         |
        MCBSP_FMKS(XCR, XWDLEN2, 8BIT)          |
        MCBSP_FMKS(XCR, XCOMPAND, MSB)          |
        MCBSP_FMKS(XCR, XFIG, NO)               |
        MCBSP_FMKS(XCR, XDATDLY, 0BIT)          |
        MCBSP_FMKS(XCR, XFRLEN1, OF(3))         |
        MCBSP_FMKS(XCR, XWDLEN1, 16BIT)         |
        MCBSP_FMKS(XCR, XWDREVRS, DISABLE),

        MCBSP_SRGR_DEFAULT,

        MCBSP_FMKS(MCR, XMCME, DEFAULT)         |
        MCBSP_FMKS(MCR, XPBBLK, DEFAULT)        |
        MCBSP_FMKS(MCR, XPABLK, DEFAULT)        |
        MCBSP_FMKS(MCR, XCBLK, DEFAULT)         |
        MCBSP_FMKS(MCR, XMCM, DISXP)            |
        MCBSP_FMKS(MCR, RMCME, DEFAULT)         |
        MCBSP_FMKS(MCR, RPBBLK, DEFAULT)        |
        MCBSP_FMKS(MCR, RPABLK, DEFAULT)        |
        MCBSP_FMKS(MCR, RCBLK, DEFAULT)         |
        MCBSP_FMKS(MCR, RMCM, ELDISABLE),

        MCBSP_RCERE0_RCE_OF(0x5),
        MCBSP_RCERE1_DEFAULT,
        MCBSP_RCERE2_DEFAULT,
        MCBSP_RCERE3_DEFAULT,

        MCBSP_XCERE0_XCE_OF(0xa),
        MCBSP_XCERE1_DEFAULT,
        MCBSP_XCERE2_DEFAULT,
        MCBSP_XCERE3_DEFAULT,

        MCBSP_FMKS(PCR, XIOEN, DEFAULT)        |
        MCBSP_FMKS(PCR, RIOEN, DEFAULT)        |
        MCBSP_FMKS(PCR, FSXM, DEFAULT)         |
        MCBSP_FMKS(PCR, FSRM, DEFAULT)         |
        MCBSP_FMKS(PCR, CLKXM, DEFAULT)        |
        MCBSP_FMKS(PCR, CLKRM, DEFAULT)        |
        MCBSP_FMKS(PCR, CLKSSTAT, DEFAULT)     |
        MCBSP_FMKS(PCR, DXSTAT, DEFAULT)       |
        MCBSP_FMKS(PCR, FSXP, ACTIVELOW)       |
        MCBSP_FMKS(PCR, FSRP, ACTIVELOW)       |
        MCBSP_FMKS(PCR, CLKXP, FALLING)        |
        MCBSP_FMKS(PCR, CLKRP, RISING)
    };
#else
	MCBSP_Config mcbspCfg = {
					 0x02c00000,		/* spcr ..*/
					 0x00a500a0,		/* rcr ..*/		/* single channel */
					 //0x80a500a0,		/* rcr ..*/		/* double channel */
					 0x00000000,		/* xcr ..*/
					 0x103f1f03,		/* srgr ..*/
					 0x00000000,		/* mcr ..*/
					 0x00000000,		/* rcere0 */
					 0x00000000,		/* rcere1 */
					 0x00000000,		/* rcere2 */
					 0x00000000,		/* rcere3 */
					 0x00000000,		/* xcere0 */
					 0x00000000,		/* xcere1 */
					 0x00000000,		/* xcere2 */
					 0x00000000,		/* xcere3 */
				//	 0x00000f01 		/* pcr ..*/		/* right channel first */
					 0x00000f05 		/* pcr ..*/		/* left channel first */
	};
#endif

    /* If no device parameters are passed, use the defaults. */
    if (params == NULL) {
        params = &TEB6416_EDMA_PCM3002_DEVPARAMS;
    }

    /* Check if the version is supported by the driver */
    if (params->versionId != TEB6416_EDMA_PCM3002_VERSION_1) {
        /* Unsupported version */
        return(IOM_EBADIO);
    }

#if 0
    /* Set up the actual codec */

    result = writeMode(TEB6416_EDMA_PCM3002_MODE_REG0, params->reg[0]);

    if (result) {
        result = writeMode(TEB6416_EDMA_PCM3002_MODE_REG1, params->reg[1]);
    }

    if (result) {
        result = writeMode(TEB6416_EDMA_PCM3002_MODE_REG2, params->reg[2]);
    }

    if (result) {
        result = writeMode(TEB6416_EDMA_PCM3002_MODE_REG3, params->reg[3]);
    }

    if (!result) {
        return (IOM_EBADIO);
    }
#endif

    /* Set the IRQ id to use for EDMA (default is 8) */
    genericParams.irqId = params->irqId;
    
    /* Set the interrupt mask */
    genericParams.intrMask = params->intrMask;

    genericParams.cacheCalls = params->cacheCalls;
    genericParams.mcbspCfgPtr = &mcbspCfg;

    /* Call the generic mdBindDev to claim mcbsp port 0 */
    result = (C6X1X_EDMA_MCBSP_FXNS.mdBindDev)(&port.c6x1xPortObj, devid, 
                                               &genericParams);
    /* Store the edma priority in PortObj */
    port.edmaPriority = params->edmaPriority;
    *devp = &port;

    return (result);
}

/*
 * ======== mdCreateChan ========
 * This function creates and configures a device channel.
 */
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
                        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg)
{
    Int result;
    C6X1X_EDMA_MCBSP_ChanParams genericParams;
    PortHandle localPort = (PortHandle) devp;

    EDMA_Config edmaCfg = {
        EDMA_FMKS(OPT, ESIZE, 32BIT)          |
        EDMA_FMKS(OPT, 2DS, NO)               |
        EDMA_FMKS(OPT, SUM, NONE)             |
        EDMA_FMKS(OPT, 2DD, NO)               |
        EDMA_FMKS(OPT, DUM, NONE)             |
        EDMA_FMKS(OPT, TCINT, YES)            |
        EDMA_FMK (OPT, TCC, NULL)             |
        EDMA_FMKS(OPT, TCCM, DEFAULT)         |
        EDMA_FMKS(OPT, ATCINT, NO)            |
        EDMA_FMKS(OPT, ATCC, DEFAULT)         |
        EDMA_FMKS(OPT, PDTS, DEFAULT)         |
        EDMA_FMKS(OPT, PDTD, DEFAULT)         |
        EDMA_FMKS(OPT, LINK, YES)             |
        EDMA_FMKS(OPT, FS, NO),

        EDMA_FMK (SRC, SRC, NULL),

        EDMA_FMK (CNT, FRMCNT, NULL)          |
        EDMA_FMK (CNT, ELECNT, NULL),

        EDMA_FMK (DST, DST, NULL),

        EDMA_FMKS(IDX, FRMIDX, DEFAULT)       |
        EDMA_FMKS(IDX, ELEIDX, DEFAULT),

        EDMA_FMK (RLD, ELERLD, NULL)          |
        EDMA_FMK (RLD, LINK, NULL)
    };

    if (mode == IOM_INPUT) {
        edmaCfg.opt |= EDMA_FMK(OPT, DUM, EDMA_OPT_DUM_INC);
    }
    else {
        edmaCfg.opt |= EDMA_FMK(OPT, SUM, EDMA_OPT_SUM_INC);
    }

    /* set the edma transfer priority */
    edmaCfg.opt |= EDMA_FMK(OPT, PRI, localPort->edmaPriority);

    /*
     * This codec uses 4 McBSP TDM channels (although it's a stereo codec).
     * This value will set up the Loop job in the generic driver to keep
     * the external framesync.
     */
    genericParams.tdmChans = 4;
    genericParams.edmaCfgPtr = &edmaCfg;

    result = (C6X1X_EDMA_MCBSP_FXNS.mdCreateChan)(chanp, localPort->c6x1xPortObj, 
                                  name, mode, &genericParams, cbFxn, cbArg);
    return (result);
}

#if 0
/*
 * ======== writeMode() ========
 * This function writes the value modeVal to the codec mode register.
 */
static Int writeMode(int modeReg, Int modeVal)
{
    Int modeMask;
    volatile Int timeout;

    switch(modeReg) {
    case 0:
    case 1:
    case 2:
        modeMask = 0x01ff;
        break;
    case 3:
        modeMask = 0x002e;
        break;
    default:
        return (0);
    }

    modeVal &= modeMask;
    modeVal |= (modeReg << 9);

    for (timeout = 1000; timeout > 0; timeout--) {
        if (isCodecReady()) {
            writeCodecMode(modeVal);
            return(modeVal);
        }
    }

    return (0);
}

/*
 * ======== WriteCodecMode() ========
 * This function writes the codec mode register.
 */
static Int writeCodecMode(Int modeVal)
{
    volatile Char *ptr = (volatile Char *) CPLDBASEADDR;

    ptr[REGACODECL] = modeVal;
    ptr[REGACODECH] = modeVal >> 8;

    return (modeVal & 0x0ffff);
}

/*
 * ======== isCodecReady() ========
 * This function determines whether the codec is ready or not.
 */
static Int isCodecReady(Void)
{
    volatile Char *ptr = (volatile Char *) CPLDBASEADDR;

    return ((ptr[REGAMISC] & REGCODECMTRDY) == 0);
}
#endif

/*
 * ======== TEB6416_EDMA_PCM3002_init ========
 * This function initializes the driver's structures.
 */
Void TEB6416_EDMA_PCM3002_init()
{
    /* Pass some IOM fxns straight to the generic driver */
    TEB6416_EDMA_PCM3002_FXNS.mdControlChan =
        C6X1X_EDMA_MCBSP_FXNS.mdControlChan;
    TEB6416_EDMA_PCM3002_FXNS.mdDeleteChan =
        C6X1X_EDMA_MCBSP_FXNS.mdDeleteChan;
    TEB6416_EDMA_PCM3002_FXNS.mdSubmitChan =
        C6X1X_EDMA_MCBSP_FXNS.mdSubmitChan;
    TEB6416_EDMA_PCM3002_FXNS.mdUnBindDev =
        C6X1X_EDMA_MCBSP_FXNS.mdUnBindDev;

    C6X1X_EDMA_MCBSP_init();
}

