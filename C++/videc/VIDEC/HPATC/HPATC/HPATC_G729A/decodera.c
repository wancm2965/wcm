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
 * Main program of the G.729a 8.0 kbit/s decoder.                  *
 *                                                                 *
 *    Usage : decoder  bitstream_file  synth_file                  *
 *                                                                 *
 *-----------------------------------------------------------------*/

#include "typedef.h"
#include "ld8a.h"


//int bad_lsf;        /* bad LSF indicator   */
/*
   This variable should be always set to zero unless transmission errors
   in LSP indices are detected.
   This variable is useful if the channel coding designer decides to
   perform error checking on these important parameters. If an error is
   detected on the  LSP indices, the corresponding flag is
   set to 1 signalling to the decoder to perform parameter substitution.
   (The flags should be set back to 0 for correct transmission).
*/

/*-----------------------------------------------------------------*
 *           Initialization of decoder                             *
 *-----------------------------------------------------------------*/

void * va_g729a_init_decoder()
{  
	int i;

	dec_inst*st = NULL;
	st = (dec_inst*)malloc(sizeof(dec_inst));

	for (i=0; i<M; i++) st->synth_buf[i] = (F)0.0;
	st->synth = st->synth_buf + M;

	st->bad_lsf = 0;          /* Initialize bad LSF indicator */
	st->mem_pre = (F)0.;

	st->past_gain = (F)1.0;
	st->past_qua_en[0]=(F)-14.0;
	st->past_qua_en[1]=(F)-14.0;
	st->past_qua_en[2]=(F)-14.0;
	st->past_qua_en[3]=(F)-14.0;

	init_decod_ld8a(st);
	init_post_filter(st);
	init_post_process(st);

	return st;
}

/*-----------------------------------------------------------------*
 *            Main decoder routine                                 *
 * parm buffer length 11                                           *
 * synth_short buffer space length (>=L_FRAME sizeof(short) bytes) *                             *
 * bad frame indicator (bfi)							           *
 *-----------------------------------------------------------------*/
void va_g729a_decoder(void*handle,unsigned char * bitstream, short *synth_short, int bfi)
{
    int  i; 
    FLOAT temp;

	dec_inst*st=(dec_inst*)handle;

    bits2prm_ld8k(bitstream, &st->parm[0]);	
	st->parm[3] = check_parity_pitch(st->parm[2], st->parm[3] ); /* get parity check result */

	decod_ld8a(st,st->parm, st->synth, st->Az_dec, st->T2, bfi);             /* decoder */

	post_filter(st,st->synth, st->Az_dec, st->T2);                  /* Post-filter */

	post_process(st,st->synth, L_FRAME);                    /* Highpass filter */

	/*---------------------------------------------------------------*
     * writes a FLOAT array as a Short to a output buf    *
     *---------------------------------------------------------------*/
	for(i=0; i < L_FRAME; i++)
    {
		/* round and convert to int  */
        temp = st->synth[i];
        if (temp >= (F)0.0)
			temp += (F)0.5;
        else  temp -= (F)0.5;
        if (temp >  (F)32767.0 ) temp =  (F)32767.0;
        if (temp < (F)-32768.0 ) temp = (F)-32768.0;
        synth_short[i] = (INT16) temp;
    }
}


void va_g729a_free_decoder(void*handle)
{
	dec_inst*st=(dec_inst*)handle;
	free(st);
}