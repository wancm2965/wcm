/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#ifndef _STATE_H
#define _STATE_H

#include <stdio.h>
#include "cnst.h"

typedef struct 
{
   float                old_wtda[FRAME_LENGTH/2];
   float				   	old_hpfilt_in;
   float				   	old_hpfilt_out;
   float			      	EnergyLT;

   short                TransientHangOver;

   short                num_bits;
   short                num_bits_spectrum_stationary ;
   short                num_bits_spectrum_transient  ;
} CoderState;

typedef struct {
   float             old_out[FRAME_LENGTH];
   float             old_coeffs[FRAME_LENGTH];

   short             num_bits;
   short             num_bits_spectrum_stationary;
   short             num_bits_spectrum_transient;

   short             old_is_transient;
} DecoderState;

// {{ liurui add
enum
{
	G192_FRAME_ERASURE = 0x6B20,
	G192_FRAME_SYNC_1 = 0x6B21,
	G192_FRAME_SYNC_2 = 0x6B22,
	G192_FRAME_SYNC_3 = 0x6B23,
	G192_FRAME_SYNC_4 = 0x6B24,
	G192_FRAME_SYNC_5 = 0x6B25,
	G192_FRAME_SYNC_6 = 0x6B26,
	G192_FRAME_SYNC_7 = 0x6B27,
	G192_FRAME_SYNC_8 = 0x6B28,
	G192_FRAME_SYNC_9 = 0x6B29,
	G192_FRAME_SYNC_10 = 0x6B2A,
	G192_FRAME_SYNC_11 = 0x6B2B,
	G192_FRAME_SYNC_12 = 0x6B2C,
	G192_FRAME_SYNC_13 = 0x6B2D,
	G192_FRAME_SYNC_14 = 0x6B2E,
	G192_FRAME_SYNC_15 = 0x6B2F,
	G192_HARD_ZERO = 0x7F,
	G192_INDETERMINATE = 0x00,
	G192_HARD_ONE = 0x81
};
// }} liurui add

#endif
