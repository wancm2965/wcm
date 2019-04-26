/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.21 06-26-03 (ddk-b10)" */
#ifndef VCAPPARAMS_H
#define VCAPPARAMS_H
#include "ad9880.h"
#include <vport.h>
#include <vportcap.h>


//extern AD9880_ConfParams AH2K_vCapParamsAD9880;

extern VPORT_PortParams vCapParams_5150;
extern VPORTCAP_Params vCapParamsChan_5150;

extern VPORT_PortParams vCapParams_9880;
extern VPORTCAP_Params AH2K_vCapParamsChan;

#endif
