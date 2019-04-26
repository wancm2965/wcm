/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/* 
 *  ======== evmdm642_edma_aic23.c ========
 * 
 *  Driver for the aic23 codec on the TMS320C642 EVM. 
 *  Requires the generic TMS320C6x1x McASP driver.
 */

#include <std.h>

#include <iom.h>

#include <csl.h>
#include <csl_mcasp.h>
#include <csl_edma.h>
#include <csl_mcasphal.h>


#include <c6x1x_edma_mcasp.h>
#include "pcm1801a.h"

#define MCASPPORT  0

#define  MCASP_AFSXCTL_XMOD_TDM2 0x00000002u
#define  MCASP_AFSRCTL_RMOD_TDM2 0x00000002u

#define GENERICDEVPARAMS {                                                 \
    C6X1X_EDMA_MCASP_VERSION_1, /* version id */                           \
    TRUE, /* True if buffers are in cacheable memory (set in mdBindDev) */ \
    /* Driver not enable the McASP high frequecy rate generator */         \
    C6X1X_EDMA_MCASP_RCV,/*C6X1X_EDMA_MCASP_XMT,  */                  \
    /* Driver not enable the McASP clk rate generator */                   \
    C6X1X_EDMA_MCASP_RCV,                                             \
    /* Driver not enable the McASP frame sync generator */                 \
    C6X1X_EDMA_MCASP_RCV,                                             \
    0,     /* IRQ id to use for the EDMA (set in mdBindDev) */             \
    NULL,  /* McASP configuration for the loopback (set in mdBindDev) */   \
    -1,    /* input event irq id */                                        \
    -1,    /* output event irq id */                                       \
    NULL,   /* event callback */                                           \
    TEB6416_EDMA_PCM1801_INTR_MASK_DEFAULT,                                 \
    TEB6416_EDMA_PCM1801_INTR_MASK_DEFAULT,                                 \
    TEB6416_EDMA_PCM1801_INTR_MASK_DEFAULT                                  \
}


TEB6416_EDMA_PCM1801_DevParams TEB6416_EDMA_PCM1801_DEVPARAMS =
                                   TEB6416_EDMA_PCM1801_DEFAULT_DEVPARAMS;

int audioInPin = 2;
int audioOutPin;
/*
 * Forward declaration of the IOM interface functions. They are only
 * exposed via the IOM function table to avoid namespace pollution.
 */
static Int mdBindDev(Ptr *devp, Int devid, Ptr devParams);
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
                        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg);

/*
 * The codec driver function table. Notice how we copy most functions
 * from the generic c6x1x edma mcasp driver.
 */

IOM_Fxns TEB6416_EDMA_PCM1801_FXNS = {
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
  
   
   TEB6416_EDMA_PCM1801_DevParams *params =
        (TEB6416_EDMA_PCM1801_DevParams *) devParams;
    C6X1X_EDMA_MCASP_DevParams genericParams = GENERICDEVPARAMS;
	int retVal;
	

    MCASP_ConfigGbl configGbl = {
        MCASP_FMKS(PFUNC, AFSR, MCASP)          |
        MCASP_FMKS(PFUNC, AHCLKR, MCASP)        |
        MCASP_FMKS(PFUNC, ACLKR, MCASP)         |
        MCASP_FMKS(PFUNC, AFSX, MCASP)          |
        MCASP_FMKS(PFUNC, AHCLKX, MCASP)        |
        MCASP_FMKS(PFUNC, ACLKX, MCASP)         |
        MCASP_FMKS(PFUNC, AMUTE, MCASP)         |
        MCASP_FMKS(PFUNC, AXR7, MCASP)          |
        MCASP_FMKS(PFUNC, AXR6, MCASP)          |
        MCASP_FMKS(PFUNC, AXR5, MCASP)          |
        MCASP_FMKS(PFUNC, AXR4, MCASP)          |
        MCASP_FMKS(PFUNC, AXR3, MCASP)          |
        MCASP_FMKS(PFUNC, AXR2, MCASP)          |
        MCASP_FMKS(PFUNC, AXR1, MCASP)          |
        MCASP_FMKS(PFUNC, AXR0, MCASP),

        MCASP_FMKS(PDIR, AFSR, OUT)              |
        MCASP_FMKS(PDIR, AHCLKR, IN)            |
        MCASP_FMKS(PDIR, ACLKR, OUT)             |
        MCASP_FMKS(PDIR, AFSX, IN)              |
        MCASP_FMKS(PDIR, AHCLKX, IN)            |
        MCASP_FMKS(PDIR, ACLKX, IN)             |
        MCASP_FMKS(PDIR, AMUTE, IN)       		 |
        MCASP_FMKS(PDIR, AXR7, IN)              |
        MCASP_FMKS(PDIR, AXR6, IN)              |
        MCASP_FMKS(PDIR, AXR5, IN)              |
        MCASP_FMKS(PDIR, AXR4, IN)              |
        MCASP_FMKS(PDIR, AXR3, IN)              |
        MCASP_FMKS(PDIR, AXR2, IN)              |
        MCASP_FMKS(PDIR, AXR1, IN)              |
        MCASP_FMKS(PDIR, AXR0, OUT),

        MCASP_DITCTL_DEFAULT,   

        MCASP_FMKS(DLBCTL, MODE, DEFAULT)       |
        MCASP_FMKS(DLBCTL, ORD, DEFAULT)        |
        MCASP_FMKS(DLBCTL, DLBEN, DISABLE),

        MCASP_AMUTE_DEFAULT
    };
    
    MCASP_ConfigRcv configRcv = {
        MCASP_FMKS(RMASK, RMASK31, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK30, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK29, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK28, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK27, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK26, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK25, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK24, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK23, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK22, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK21, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK20, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK19, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK18, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK17, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK16, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK15, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK14, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK13, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK12, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK11, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK10, NOMASK)      |
        MCASP_FMKS(RMASK, RMASK9, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK8, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK7, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK6, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK5, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK4, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK3, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK2, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK1, NOMASK)       |
        MCASP_FMKS(RMASK, RMASK0, NOMASK),
      
        MCASP_FMKS(RFMT, RDATDLY, 1BIT)         |
        MCASP_FMKS(RFMT, RRVRS, MSBFIRST)       |
        MCASP_FMKS(RFMT, RPAD, ZERO)            |
        MCASP_FMKS(RFMT, RPBIT, DEFAULT)        |
        MCASP_FMKS(RFMT, RSSZ, 16BITS)          |
        MCASP_FMKS(RFMT, RBUSEL, DAT)           |
        MCASP_FMKS(RFMT, RROT, NONE),

        MCASP_FMKS(AFSRCTL, RMOD, TDM2)        |
        MCASP_FMKS(AFSRCTL, FRWID, WORD) |
        MCASP_FMKS(AFSRCTL, FSRM, INTERNAL)     |
        MCASP_FMKS(AFSRCTL, FSRP, ACTIVEHIGH),
		//MCASP_FMKS(AFSRCTL, FSRP, ACTIVEHIGH),FALLING RISING
		//MCASP_FMKS(AFSRCTL, FSRP, ACTIVELOW),
        MCASP_FMKS(ACLKRCTL, CLKRP, RISING)     |
        MCASP_FMKS(ACLKRCTL, CLKRM, INTERNAL)   |
        MCASP_FMKS(ACLKRCTL, CLKRDIV, OF(15)),

        MCASP_FMKS(AHCLKRCTL, HCLKRM, EXTERNAL) |
        MCASP_FMKS(AHCLKRCTL, HCLKRP, FALLING )   |     
        MCASP_FMKS(AHCLKRCTL, HCLKRDIV, OF(1)),

        MCASP_FMKS(RTDM, RTDMS31, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS30, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS29, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS28, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS27, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS26, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS25, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS24, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS23, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS22, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS21, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS20, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS19, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS18, INACTIVE)     |      
        MCASP_FMKS(RTDM, RTDMS17, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS16, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS15, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS14, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS13, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS12, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS11, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS10, INACTIVE)     |
        MCASP_FMKS(RTDM, RTDMS9, INACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS8, INACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS7, INACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS6, INACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS5, INACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS4, INACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS3, INACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS2, INACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS1, ACTIVE)      |
        MCASP_FMKS(RTDM, RTDMS0, ACTIVE),

        MCASP_FMKS(RINTCTL, RSTAFRM, DISABLE)   |
        MCASP_FMKS(RINTCTL, RDATA, DISABLE)     |
        MCASP_FMKS(RINTCTL, RLAST, DISABLE)     |
        MCASP_FMKS(RINTCTL, RDMAERR, DISABLE)   |
        MCASP_FMKS(RINTCTL, RCKFAIL, DISABLE)   |
        MCASP_FMKS(RINTCTL, RSYNCERR, DISABLE)  |
        MCASP_FMKS(RINTCTL, ROVRN, DISABLE),

        MCASP_FMKS(RCLKCHK, RMAX, DEFAULT)      |
        MCASP_FMKS(RCLKCHK, RMIN, DEFAULT)      |
        MCASP_FMKS(RCLKCHK, RPS, DEFAULT)       
    }; 

    MCASP_ConfigXmt configXmt = {
        MCASP_FMKS(XMASK, XMASK31, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK30, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK29, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK28, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK27, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK26, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK25, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK24, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK23, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK22, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK21, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK20, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK19, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK18, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK17, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK16, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK15, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK14, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK13, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK12, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK11, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK10, NOMASK)      |
        MCASP_FMKS(XMASK, XMASK9, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK8, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK7, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK6, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK5, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK4, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK3, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK2, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK1, NOMASK)       |
        MCASP_FMKS(XMASK, XMASK0, NOMASK),
        
        MCASP_FMKS(XFMT, XDATDLY, 1BIT)         |
        MCASP_FMKS(XFMT, XRVRS, MSBFIRST)       |
        MCASP_FMKS(XFMT, XPAD, ZERO)            |
        MCASP_FMKS(XFMT, XPBIT, DEFAULT)        |
        MCASP_FMKS(XFMT, XSSZ, 16BITS)          |
        MCASP_FMKS(XFMT, XBUSEL, DAT)           |
        MCASP_FMKS(XFMT, XROT, NONE),

        MCASP_FMKS(AFSXCTL, XMOD, TDM2)        |
        MCASP_FMKS(AFSXCTL, FXWID, WORD) |
        MCASP_FMKS(AFSXCTL, FSXM, INTERNAL)     |
        MCASP_FMKS(AFSXCTL, FSXP, ACTIVELOW),

        MCASP_FMKS(ACLKXCTL, CLKXP, FALLING)    |
        MCASP_FMKS(ACLKXCTL, ASYNC, ASYNC)      |
        MCASP_FMKS(ACLKXCTL, CLKXM, INTERNAL)   |
        MCASP_FMKS(ACLKXCTL, CLKXDIV, OF(7)),
  
        MCASP_FMKS(AHCLKXCTL, HCLKXM, EXTERNAL) |
        MCASP_FMKS(AHCLKXCTL, HCLKXP, FALLING)   |     
        MCASP_FMKS(AHCLKXCTL, HCLKXDIV, OF(8)),


        MCASP_FMKS(XTDM, XTDMS31, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS30, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS29, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS28, INACTIVE)     |       
        MCASP_FMKS(XTDM, XTDMS27, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS26, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS25, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS24, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS23, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS22, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS21, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS20, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS19, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS18, INACTIVE)     |      
        MCASP_FMKS(XTDM, XTDMS17, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS16, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS15, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS14, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS13, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS12, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS11, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS10, INACTIVE)     |
        MCASP_FMKS(XTDM, XTDMS9, INACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS8, INACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS7, INACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS6, INACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS5, INACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS4, INACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS3, INACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS2, INACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS1, ACTIVE)      |
        MCASP_FMKS(XTDM, XTDMS0, ACTIVE),

        MCASP_FMKS(XINTCTL, XSTAFRM, DISABLE)   |
        MCASP_FMKS(XINTCTL, XDATA, DISABLE)     |
        MCASP_FMKS(XINTCTL, XLAST, DISABLE)     |
        MCASP_FMKS(XINTCTL, XDMAERR, DISABLE)   |
        MCASP_FMKS(XINTCTL, XCKFAIL, DISABLE)   |
        MCASP_FMKS(XINTCTL, XSYNCERR, DISABLE)  |
        MCASP_FMKS(XINTCTL, XUNDRN, DISABLE),

        MCASP_FMKS(XCLKCHK, XMAX, DEFAULT)      |
        MCASP_FMKS(XCLKCHK, XMIN, DEFAULT)      |
        MCASP_FMKS(XCLKCHK, XPS, DEFAULT)       
    }; 

    MCASP_ConfigSrctl configSrctl = {
        MCASP_FMKS(SRCTL, DISMOD, HIGH)         |
        MCASP_FMKS(SRCTL, SRMOD, RCV),

        MCASP_FMKS(SRCTL, DISMOD, HIGH)         |
        MCASP_FMKS(SRCTL, SRMOD, RCV),

        MCASP_FMKS(SRCTL, DISMOD, 3STATE)       |
        MCASP_FMKS(SRCTL, SRMOD, INACTIVE),

        MCASP_FMKS(SRCTL, DISMOD, 3STATE)       |
        MCASP_FMKS(SRCTL, SRMOD, INACTIVE),

        MCASP_FMKS(SRCTL, DISMOD, 3STATE)       |
        MCASP_FMKS(SRCTL, SRMOD, INACTIVE),

        MCASP_FMKS(SRCTL, DISMOD, 3STATE)       |
        MCASP_FMKS(SRCTL, SRMOD, INACTIVE),

        MCASP_FMKS(SRCTL, DISMOD, 3STATE)       |
        MCASP_FMKS(SRCTL, SRMOD, INACTIVE),

        MCASP_FMKS(SRCTL, DISMOD, HIGH)       |
        MCASP_FMKS(SRCTL, SRMOD, INACTIVE)
};

    MCASP_Config mcaspCfg;

	int i;
		 
   if(devid == 0xffffffff)
   		return 0;
   		
	
    mcaspCfg.global = &configGbl;
    mcaspCfg.receive = &configRcv;
    mcaspCfg.transmit = &configXmt;
    mcaspCfg.srctl = &configSrctl;

	/* If no device parameters are passed, use the defaults. */
    if (params == NULL) 
    {
        params = &TEB6416_EDMA_PCM1801_DEVPARAMS;
    }
    /* Check if the version is supported by the driver. */
    if(params->versionId != TEB6416_EDMA_PCM1801_VERSION_1){
        /* Unsupported version */
        return(IOM_EBADARGS);
    }

    /* Set the McASP */ 
    genericParams.mcaspCfgPtr = &mcaspCfg;

    genericParams.irqId = params->irqId;
    genericParams.enableClkg = params->enableClkg;
    genericParams.enableHclkg = params->enableHclkg;
    genericParams.enableFsyncg = params->enableFsyncg;
    genericParams.cacheCalls = params->cacheCalls;
    genericParams.inEvtIrqId = params->inEvtIrqId;
    genericParams.outEvtIrqId = params->outEvtIrqId;
    genericParams.evtCallback = params->evtCallback;
    genericParams.inEvtIntrMask  = params->inEvtIntrMask;
    genericParams.outEvtIntrMask  = params->outEvtIntrMask;
    /* Set the interrupt mask */
    genericParams.edmaIntrMask = params->edmaIntrMask;
	
     /* Call the generic mdBindDev to claim mcasp port MCASPPORT */
    retVal = (C6X1X_EDMA_MCASP_FXNS.mdBindDev)(devp, MCASPPORT, &genericParams);
    
    return  retVal;
}

/*
 * ======== mdCreateChan ========
 * This function creates and configures a device channel.
 */
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
                        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg)
{
    Int result;
    C6X1X_EDMA_MCASP_ChanParams genericParams;

    EDMA_Config edmaCfg = {
        EDMA_FMKS(OPT, PRI, HIGH)             |
        EDMA_FMKS(OPT, ESIZE, 32BIT)          |
        EDMA_FMKS(OPT, 2DS, NO)               |
        EDMA_FMKS(OPT, SUM, NONE)             |
        EDMA_FMKS(OPT, 2DD, NO)               |
        EDMA_FMKS(OPT, DUM, NONE)             |
        EDMA_FMKS(OPT, TCINT, YES)            |
        EDMA_FMK (OPT, TCC, NULL)             |
        EDMA_FMKS(OPT, LINK, YES)             |
        EDMA_FMKS(OPT, FS, YES),

        EDMA_FMK(SRC, SRC, NULL),

        EDMA_FMKS(CNT, FRMCNT, DEFAULT)       |
        EDMA_FMKS(CNT, ELECNT, DEFAULT),

        EDMA_FMK(DST, DST, NULL),

        EDMA_FMKS(IDX, FRMIDX, DEFAULT)       |
        EDMA_FMKS(IDX, ELEIDX, DEFAULT),

        EDMA_FMKS(RLD, ELERLD, DEFAULT)       |
        EDMA_FMK(RLD, LINK, NULL)
    };      

    /* Set ELECNT, ELERLD to the active serializers */
    /* FRMCNT will be 1 pair of SRCTL and set in C6x1x_edma_mcasp driver */
    EDMA_FSETA(&edmaCfg.cnt, CNT, ELECNT, 1);   
    EDMA_FSETA(&edmaCfg.rld, RLD, ELERLD, 1); 

    if (mode == IOM_INPUT) {
        EDMA_FSETSA(&edmaCfg.opt, OPT, DUM, INC);
        EDMA_FSETSA(&edmaCfg.opt, OPT, ESIZE,32BIT);
        EDMA_FSETA(&edmaCfg.cnt, CNT, ELECNT, audioInPin);   
	    EDMA_FSETA(&edmaCfg.rld, RLD, ELERLD, audioInPin); 
        /* Select EDMA channel 13 for rcv here */
        genericParams.edmaChan = 13;
    } else {

        EDMA_FSETSA(&edmaCfg.opt, OPT, SUM, INC);
        EDMA_FSETSA(&edmaCfg.opt, OPT, ESIZE,16BIT);

        /* Select EDMA channel 12 for xmt here */
        genericParams.edmaChan = 12;
    }

    genericParams.edmaCfgPtr = &edmaCfg;
    /* 
     * for loopback, no need to sync to external frame sync, 
     * set to 5 to run loop edma job long enough for sync real xmt & rcv job
     */  
    genericParams.tdmChans = 1;
    result = (C6X1X_EDMA_MCASP_FXNS.mdCreateChan)(chanp, devp, name, mode,
                                                &genericParams, cbFxn, cbArg);
    return (result);
}

/*
 * ======== EVMDM642_EDMA_AIC23_init ========
 * This function initializes the driver's structures.
 */
Void TEB6416_EDMA_PCM1801_init()
{	 
    /* Pass some IOM fxns straight to the generic driver */
    TEB6416_EDMA_PCM1801_FXNS.mdControlChan =
        C6X1X_EDMA_MCASP_FXNS.mdControlChan;
    TEB6416_EDMA_PCM1801_FXNS.mdDeleteChan =
        C6X1X_EDMA_MCASP_FXNS.mdDeleteChan;
    TEB6416_EDMA_PCM1801_FXNS.mdSubmitChan =
        C6X1X_EDMA_MCASP_FXNS.mdSubmitChan;
    TEB6416_EDMA_PCM1801_FXNS.mdUnBindDev =
        C6X1X_EDMA_MCASP_FXNS.mdUnBindDev;

    C6X1X_EDMA_MCASP_init();

}

