/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#ifndef __H235BUFFNUM_H
#define __H235BUFFNUM_H


#include "cmintr.h"


#define MAX_BUFFNUM_SIZE 192 /* in two-byte units, twice that of actual possible prime (1536 bit) */

RvStatus rvh323BuffnumTest(HAPP hApp);


/********************************************************
 * randomize a buffer but have it between A and B (A>B) *
 ********************************************************/
void rvh323BuffnumRandomizeLimited(cmElem * app, RvUint16 * Res, RvUint16 * A, RvUint16 * B, int size);


/********************************
 * sets res to ((2^exp) % mod)  *
 ********************************/
RvStatus rvh323BuffnumTwoExpMod(RvUint16 * res, RvUint16 * exp, RvUint16 * mod);

/**********************************
 * sets res to ((base^num) % mod) *
 **********************************/
RvStatus rvh323BuffnumBaseExpMod(RvUint16 * res, RvUint16 * base, RvUint16 * exp, RvUint16 * mod);

/************************************************
 * generate Prime of length 'length' in bytes.  *
 * size must be at most MAX_BUFFNUM_SIZE.       *
 * length must be at most half of size.         *
 ************************************************/
RvStatus rvh323BuffnumGeneratePrime(cmElem * app, RvUint16 * prime, int length);

#endif

#ifdef __cplusplus
}
#endif
