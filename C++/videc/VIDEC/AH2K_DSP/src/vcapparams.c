/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.21 06-26-03 (ddk-b10)" */
#include <vport.h>
#include <vportcap.h>
#include <csl_edma.h>

#include "ad9880.h"
#include "tvp5150.h"
#include <dev.h>

#define LINE_SZ  704
#define NUM_LINES 288

extern EDC_Fxns TVP5150_Fxns;

VPORTCAP_Params AH2K_vCapParamsChan = {
    //VPORT_MODE_BT656_8BIT, /* cmode:3  */
    VPORT_MODE_YCBCR_8BIT,
    VPORT_FLDOP_FRAME,     /* fldOp:3 i*/    
    //VPORT_FLDOP_FLD1,		//720p 20080516

    VPORT_SCALING_DISABLE, /* scale:1  */    
    VPORT_RESMPL_DISABLE,  /* resmpl:1 */
    //VPORT_RESMPL_ENABLE,  /* resmpl:1 */
    VPORTCAP_BPK_10BIT_ZERO_EXTENDED, /*bpk10Bit:2   */

    //VPORTCAP_HRST_EAV,     /*hCtRst:1  */
    //VPORTCAP_VRST_EAV_V0,  /*vCtRst:1  */
    VPORTCAP_HRST_SAV, 
    VPORTCAP_VRST_EAV_V1,
    //VPORTCAP_FLDD_DISABLE, /*fldDect:1 */
    VPORTCAP_FLDD_ENABLE,
    VPORTCAP_EXC_ENABLE,  /*extCtl:1  */   
    //VPORTCAP_FINV_ENABLE,  /* fldInv:1 */
    VPORTCAP_FINV_DISABLE,
    
    ////////////////////////////////////////
    //0,                     /*fldXStrt1 */
    264,
    //1,                     /*fldYStrt1 */  
    30,
    //0,                     /*fldXStrt2 */
    264,
    //1, //15,               /*fldYStrt2 */
    30,
    
    //LINE_SZ-1,             /*fldXStop1 */
    1287,
    //NUM_LINES, //+14,      /*fldYStop1 */
    797,
    
    //LINE_SZ-1,             /*fldXStop2 */
    1287,
    //NUM_LINES, //+14,      /*fldYStop2 */
    797,
    
    //(LINE_SZ>>3),          /*thrld     */
    1024 >> 3,//1280 >> 3,
    ///////////////////////////////////////
    
    3,                     /*numFrmBufs*/
    128,                     /*alignment */
    //VPORT_FLDS_MERGED,     /*mergeFlds */
    VPORT_FLDS_SEPARATED,     /*mergeFlds */
    NULL,                  /*segId     */            
    EDMA_OPT_PRI_URGENT, /*EDMA_OPT_PRI_HIGH,*/     /*edmaPri   */
    8,                          /* irqId    */
// add by chenzhenbao
	0                          /*  422 */
};

VPORT_PortParams vCapParamsPort_9880 = {
    FALSE,                      /*  enableDualChan;                */ 
    VPORT_POLARITY_ACTIVE_HIGH, /* vport control pin 1 polarity    */
    VPORT_POLARITY_ACTIVE_HIGH, /* vport control pin 2 polarity    */
    VPORT_POLARITY_ACTIVE_HIGH, /* vport control pin 3 polarity    */
    &AD9880_Fxns,
    INV,
};    

VPORTCAP_Params vCapParamsChan_5150 = {
    VPORT_MODE_BT656_8BIT, /* cmode:3  */
    VPORT_FLDOP_FRAME,//,     /* fldOp:3  */    

    VPORT_SCALING_DISABLE, /* scale:1  */    
    VPORT_RESMPL_DISABLE,  /* resmpl:1 */
    VPORTCAP_BPK_10BIT_ZERO_EXTENDED, /*bpk10Bit:2   */

    VPORTCAP_HRST_SAV,     /*hCtRst:1  */
    VPORTCAP_VRST_EAV_V1,  /*vCtRst:1  */
    VPORTCAP_FLDD_ENABLE, /*fldDect:1 */
    VPORTCAP_EXC_DISABLE,  /*extCtl:1  */   
    VPORTCAP_FINV_DISABLE,  /* fldInv:1 */
    
    8,                     /*fldXStrt1 */
    1,                     /*fldYStrt1 */  
    8,                     /*fldXStrt2 */  
    1,               	   /*fldYStrt2 */
    
    LINE_SZ + 7,             /*fldXStop1 */
    NUM_LINES,		       /*fldYStop1 */
    
    LINE_SZ + 7,             /*fldXStop2 */
    NUM_LINES,		       /*fldYStop2 */
    
    (LINE_SZ>>3),          /*thrld     */
    3,                     /*numFrmBufs*/
    128,                   /*alignment */
    VPORT_FLDS_MERGED,     /*mergeFlds */
    NULL,                  /*segId     */            
    EDMA_OPT_PRI_URGENT, /* EDMA_OPT_PRI_HIGH,*/     /*edmaPri   */
    8,                      /* irqId    */
// add by chenzhenbao
    0                       /* 422      */
};

VPORT_PortParams vCapParamsPort_5150 = {
    TRUE,                      		/*  enableDualChan;                */ 
    VPORT_POLARITY_ACTIVE_HIGH, 	/* vport control pin 1 polarity    */
    VPORT_POLARITY_ACTIVE_HIGH, 	/* vport control pin 2 polarity    */
    VPORT_POLARITY_ACTIVE_HIGH, 	/* vport control pin 3 polarity    */
    &TVP5150_Fxns,
    &TVP5150_Fxns, //INV,
};

