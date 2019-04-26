/*
   ITU-T G.729 Annex C - Reference C code for floating point
                         implementation of G.729
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

/*
 File : LSPDEC.C
 Used for the floating point version of both
 G.729 main body and G.729A
*/
#include <math.h>
#include "typedef.h"
#include "version.h"
#ifdef VER_G729A
 #include "ld8a.h"
 #include "tab_ld8a.h"
#else
 #include "ld8k.h"
 #include "tab_ld8k.h"
#endif

/* Prototype definitions of static functions */
static void lsp_iqua_cs( dec_inst*st,int prm[], FLOAT lsp[], int erase);



/*----------------------------------------------------------------------------
 * Lsp_decw_reset -   set the previous LSP vectors
 *----------------------------------------------------------------------------
 */
void lsp_decw_reset(dec_inst*st)
{
   int  i;

   st->freq_prev_reset[0] = (F)0.285599;
   st->freq_prev_reset[1] = (F)0.571199;
   st->freq_prev_reset[2] = (F)0.856798;
   st->freq_prev_reset[3] = (F)1.142397;
   st->freq_prev_reset[4] = (F)1.427997;
   st->freq_prev_reset[5] = (F)1.713596;
   st->freq_prev_reset[6] = (F)1.999195;
   st->freq_prev_reset[7] = (F)2.284795;
   st->freq_prev_reset[8] = (F)2.570394;
   st->freq_prev_reset[9] = (F)2.855993;

   for(i=0; i<MA_NP; i++)
     copy (st->freq_prev_reset, &st->freq_prev[i][0], M );

   st->prev_ma = 0;

   copy (st->freq_prev_reset, st->prev_lsp, M );

   return;
}


/*----------------------------------------------------------------------------
 * lsp_iqua_cs -  LSP main quantization routine
 *----------------------------------------------------------------------------
 */
static void lsp_iqua_cs(
 dec_inst*st,
 int    prm[],          /* input : codes of the selected LSP */
 FLOAT  lsp_q[],        /* output: Quantized LSP parameters  */
 int    erase           /* input : frame erase information   */
)
{
   int  mode_index;
   int  code0;
   int  code1;
   int  code2;
   FLOAT buf[M];


   if(erase==0)                 /* Not frame erasure */
     {
        mode_index = (prm[0] >> NC0_B) & 1;
        code0 = prm[0] & (INT16)(NC0 - 1);
        code1 = (prm[1] >> NC1_B) & (INT16)(NC1 - 1);
        code2 = prm[1] & (INT16)(NC1 - 1);

        lsp_get_quant(lspcb1, lspcb2, code0, code1, code2, fg[mode_index],
              st->freq_prev, lsp_q, fg_sum[mode_index]);

        copy(lsp_q, st->prev_lsp, M );
        st->prev_ma = mode_index;
     }
   else                         /* Frame erased */
     {
       copy(st->prev_lsp, lsp_q, M );

        /* update freq_prev */
       lsp_prev_extract(st->prev_lsp, buf,
          fg[st->prev_ma], st->freq_prev, fg_sum_inv[st->prev_ma]);
       lsp_prev_update(buf, st->freq_prev);
     }
     return;
}
/*----------------------------------------------------------------------------
 * d_lsp - decode lsp parameters
 *----------------------------------------------------------------------------
 */
void d_lsp(
    dec_inst*st,
	int     index[],    /* input : indexes                 */
    FLOAT   lsp_q[],    /* output: decoded lsp             */
    int     bfi         /* input : frame erase information */
)
{
   int i;

   lsp_iqua_cs(st,index, lsp_q,bfi); /* decode quantized information */

   /* Convert LSFs to LSPs */

   for (i=0; i<M; i++ )
     lsp_q[i] = (FLOAT)cos(lsp_q[i]);

   return;
}

