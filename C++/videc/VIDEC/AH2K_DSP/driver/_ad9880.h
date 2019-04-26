/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/********************************************************************************/
/* _ad9880.h file */
/********************************************************************************/
#ifndef __AD9880_H
#define __AD9880_H

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>

typedef struct _AD9880_Generics
{
	Uint32	subAddress;
	Uint32	value;   
}_AD9880_Generics;

extern _AD9880_Generics _AD9880_settings[];
extern _AD9880_Generics _AD9880_settingsCSC[];
extern Uint8 _AD9880_Configure[];
extern Uint8 _AD9880_CSC[][24];
extern AD9880_tblMode *tblMode[];

#ifdef __cplusplus
}
#endif


#endif /*#ifndef __AD9880_H*/

/********************************************************************************/
/* end of file */
/********************************************************************************/

