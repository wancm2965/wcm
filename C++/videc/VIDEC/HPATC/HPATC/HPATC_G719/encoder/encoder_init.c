/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include "../include/state.h"
#include "../include/cnst.h"

/*--------------------------------------------------------------------------*/
/*  Function  encoder_reset_tables                                          */
/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                          */
/*                                                                          */
/*  Reset encoder tables                                                    */
/*--------------------------------------------------------------------------*/
/*  CoderState    *c          (i)   state of encoder                        */
/*  int           num_bits    (i)   number of bits                          */
/*--------------------------------------------------------------------------*/
void encoder_reset_tables(CoderState *c, short num_bits)
{
   c->num_bits_spectrum_stationary = num_bits - 3;
   c->num_bits_spectrum_transient  = num_bits - 1;
   c->num_bits                     = num_bits;
}

/*--------------------------------------------------------------------------*/
/*  Function  encoder_init                                                  */
/*  ~~~~~~~~~~~~~~~~~~~~~~                                                  */
/*                                                                          */
/*  Initialize the state of the encoder                                     */
/*--------------------------------------------------------------------------*/
/*  CoderState    *c          (i)   state of coder                          */
/*  int           num_bits    (i)   number of bits                          */
/*--------------------------------------------------------------------------*/
void encoder_init(CoderState *c, short num_bits)
{
   short i;

   for (i=0 ; i < FRAME_LENGTH/2 ; i++)
   {
      c->old_wtda[i] = 0.0f;
   }

   c->old_hpfilt_in = 0.0f;
   c->old_hpfilt_out = 0.0f;

   c->EnergyLT = 0.0f;
   
   c->TransientHangOver = FALSE;

   encoder_reset_tables(c, num_bits);
}
