/*
   ITU-T G.729 Annex C - Reference C code for floating point
                         implementation of G.729 Annex A
                         Version 1.01 of 15.September.98
*/

/*
----------------------------------------------------------------------
                    COPYRIGHT NOTICE
----------------------------------------------------------------------
   ITU-T G.729 Annex C ANSI C source code
   Copyright (C) 1998, AT&T, France Telecom, NTT, University of
   Sherbrooke.  All rights reserved.

----------------------------------------------------------------------
*/

/*-----------------------------------------------------------------*
 *   Functions init_decod_ld8a  and decod_ld8a                     *
 *-----------------------------------------------------------------*/

#include "typedef.h"
#include "ld8a.h"

/*---------------------------------------------------------------*
 *   Decoder constant parameters (defined in "ld8a.h")           *
 *---------------------------------------------------------------*
 *   L_FRAME     : Frame size.                                   *
 *   L_SUBFR     : Sub-frame size.                               *
 *   M           : LPC order.                                    *
 *   MP1         : LPC order+1                                   *
 *   PIT_MIN     : Minimum pitch lag.                            *
 *   PIT_MAX     : Maximum pitch lag.                            *
 *   L_INTERPOL  : Length of filter for interpolation            *
 *   PRM_SIZE    : Size of vector containing analysis parameters *
 *---------------------------------------------------------------*/

/*--------------------------------------------------------*
 *         Static memory allocation.                      *
 *--------------------------------------------------------*/



/*-----------------------------------------------------------------*
 *   Function init_decod_ld8a                                      *
 *            ~~~~~~~~~~~~~~~                                      *
 *                                                                 *
 *   ->Initialization of variables for the decoder section.        *
 *                                                                 *
 *-----------------------------------------------------------------*/

void init_decod_ld8a(dec_inst*st)
{
	st->lsp_old[0]=(F)0.9595;
	st->lsp_old[1]=(F)0.8413;
	st->lsp_old[2]=(F)0.6549;
	st->lsp_old[3]=(F)0.4154;
	st->lsp_old[4]=(F)0.1423;
	st->lsp_old[5]=(F)-0.1423;
	st->lsp_old[6]=(F)-0.4154;
	st->lsp_old[7]=(F)-0.6549;
	st->lsp_old[8]=(F)-0.8413;
	st->lsp_old[9]= (F)-0.9595;

  /* Initialize static pointer */

  st->exc = st->old_exc + PIT_MAX + L_INTERPOL;

  /* Static vectors to zero */

  set_zero(st->old_exc, PIT_MAX+L_INTERPOL);
  set_zero(st->mem_syn, M);

  st->sharp  = SHARPMIN;
  st->old_T0 = 60;
  st->gain_code = (F)0.0;
  st->gain_pitch = (F)0.0;

  lsp_decw_reset(st) ;

  return;
}

/*-----------------------------------------------------------------*
 *   Function decod_ld8a                                           *
 *           ~~~~~~~~~~                                            *
 *   ->Main decoder routine.                                       *
 *                                                                 *
 *-----------------------------------------------------------------*/

void decod_ld8a(
  dec_inst*st,
  int      parm[],      /* (i)   : vector of synthesis parameters      */
  FLOAT   synth[],     /* (o)   : synthesis speech                     */
  FLOAT   A_t[],       /* (o)   : decoded LP filter in 2 subframes     */
  int     *T2,          /* (o)   : decoded pitch lag in 2 subframes     */
  int     bfi           /* (i)   :bad frame indicator (bfi)      */
)
{
  FLOAT   *Az;                  /* Pointer on A_t   */
  FLOAT   lsp_new[M];           /* Decoded LSP's    */
  FLOAT   code[L_SUBFR];        /* ACELP codevector */

   /* Scalars */

   int i, i_subfr;
   int T0, T0_frac, index;
   int  bad_pitch; //bfi, 
   //extern int bad_lsf;        /* bad LSF indicator   */

   /* Decode the LSPs */

   d_lsp(st,parm, lsp_new, bfi+st->bad_lsf );
   parm += 2;                        /* Advance synthesis parameters pointer */

  /*
  Note: "bad_lsf" is introduce in case the standard is used with
         channel protection.
  */

   /* Interpolation of LPC for the 2 subframes */

   int_qlpc(st->lsp_old, lsp_new, A_t);

   /* update the LSFs for the next frame */

   copy(lsp_new, st->lsp_old, M);

   /*------------------------------------------------------------------------*
    *          Loop for every subframe in the analysis frame                 *
    *------------------------------------------------------------------------*
    * The subframe size is L_SUBFR and the loop is repeated L_FRAME/L_SUBFR  *
    *  times                                                                 *
    *     - decode the pitch delay                                           *
    *     - decode algebraic code                                            *
    *     - decode pitch and codebook gains                                  *
    *     - find the excitation and compute synthesis speech                 *
    *------------------------------------------------------------------------*/

   Az = A_t;                     /* pointer to interpolated LPC parameters */

   for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR) {

      /*-------------------------------------------------*
       *  - Find the adaptive codebook vector.            *
       *--------------------------------------------------*/

      index = *parm++;            /* pitch index */

      if(i_subfr == 0)
      {

        i = *parm++; /* get parity check result */
        bad_pitch = bfi + i;
        if( bad_pitch == 0)
        {
            dec_lag3(index, PIT_MIN, PIT_MAX, i_subfr, &T0, &T0_frac);
            st->old_T0 = T0;
        }
        else        /* Bad frame, or parity error */
        {
          T0  =  st->old_T0;
          T0_frac = 0;
          st->old_T0++;
          if( (st->old_T0 - PIT_MAX) > 0)
            st->old_T0 = PIT_MAX;
        }

      }
      else                  /* second subframe */
      {
        if( bfi == 0)
        {
          dec_lag3(index, PIT_MIN, PIT_MAX, i_subfr, &T0, &T0_frac);
          st->old_T0 = T0;
        }
        else
        {
          T0  =  st->old_T0;
          T0_frac = 0;
          st->old_T0++;
          if( (st->old_T0 - PIT_MAX) > 0)
            st->old_T0 = PIT_MAX;
        }
      }
      *T2++ = T0;

     /*-------------------------------------------------*
      * - Find the adaptive codebook vector.            *
      *-------------------------------------------------*/

      pred_lt_3(&st->exc[i_subfr], T0, T0_frac, L_SUBFR);

      /*-------------------------------------------------------*
       * - Decode innovative codebook.                         *
       * - Add the fixed-gain pitch contribution to code[].    *
       *-------------------------------------------------------*/

      if(bfi != 0)        /* Bad frame */
      {
        parm[0] = random_g729() & (INT16)0x1fff;     /* 13 bits random */
        parm[1] = random_g729() & (INT16)0x000f;     /*  4 bits random */
      }

      decod_ACELP(parm[1], parm[0], code);
      parm +=2;

      for (i = T0; i < L_SUBFR; i++)   code[i] += st->sharp * code[i-T0];

      /*-------------------------------------------------*
       * - Decode pitch and codebook gains.              *
       *-------------------------------------------------*/

      index = *parm++;          /* index of energy VQ */

      dec_gain(st,index, code, L_SUBFR, bfi, &st->gain_pitch, &st->gain_code);

      /*-------------------------------------------------------------*
       * - Update pitch sharpening "sharp" with quantized gain_pitch *
       *-------------------------------------------------------------*/

      st->sharp = st->gain_pitch;
      if (st->sharp > SHARPMAX) st->sharp = SHARPMAX;
      if (st->sharp < SHARPMIN) st->sharp = SHARPMIN;

      /*-------------------------------------------------------*
       * - Find the total excitation.                          *
       * - Find synthesis speech corresponding to exc[].       *
       *-------------------------------------------------------*/

      for (i = 0; i < L_SUBFR;  i++)
         st->exc[i+i_subfr] = st->gain_pitch*st->exc[i+i_subfr] + st->gain_code*code[i];

      syn_filt(Az, &st->exc[i_subfr], &synth[i_subfr], L_SUBFR, st->mem_syn, 1);

      Az  += MP1;        /* interpolated LPC parameters for next subframe */
   }

  /*--------------------------------------------------*
   * Update signal for next frame.                    *
   * -> shift to the left by L_FRAME  exc[]           *
   *--------------------------------------------------*/

   copy(&st->old_exc[L_FRAME], &st->old_exc[0], PIT_MAX+L_INTERPOL);

   return;
}
