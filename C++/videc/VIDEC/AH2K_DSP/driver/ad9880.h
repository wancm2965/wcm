/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/********************************************************************************/
/* saa7115.h file */
/********************************************************************************/
#ifndef _AD9880_H
#define _AD9880_H

#include <edc.h>

#define	PLL_MSB(pll) (((pll) >> 4) & 0xFF)
#define PLL_LSB(pll) (((pll) << 4) & 0xF0)

typedef unsigned int AD9880_Cmd;

typedef enum AD9880_AnalogFormat {
	AD9880_AFMT_NONE,
    AD9880_AFMT_VGA,
    AD9880_AFMT_DVI,
    AD9880_AFMT_COMPONENT
} AD9880_AnalogFormat;

typedef struct {
	Int	hSize;
	Int vSize;
	Int hProch;
	Int vProch;
	
    Bool interlaced; 
	Int rate;
	Int lcf;

} AD9880_ConfParams;

typedef struct {
	Int	clock;
	Int camp;
	Int total_size;
	Bool interlaced;
} AD9880_ConfParams1;

typedef struct {
	Int min_lines;
	Int max_lines;
	Int frame_rate;
	AD9880_ConfParams params;
	AD9880_ConfParams1 param1;
} AD9880_tblMode;

extern EDC_Fxns AD9880_Fxns;

#endif
