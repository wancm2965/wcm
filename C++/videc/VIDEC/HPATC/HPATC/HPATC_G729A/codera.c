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

/*
 File : CODERA.C
 Used for the floating point version of G.729A only
 (not for G.729 main body)
*/

#include "typedef.h"
#include "ld8a.h"


/*-------------------------------------------------*
 * Initialization of the coder.                    *
 *-------------------------------------------------*/
void* va_g729a_init_encoder()
{
	enc_inst*st=(enc_inst*)malloc(sizeof(enc_inst));

	st->past_qua_en[0]=(F)-14.0;
	st->past_qua_en[1]=(F)-14.0;
	st->past_qua_en[2]=(F)-14.0;
	st->past_qua_en[3]=(F)-14.0;

	init_pre_process(st);
	init_coder_ld8a(st);           /* Initialize the coder             */

	return st;
}

/*---------------------------------------------------------------------*
 * L_FRAME data are read. (L_FRAME = number of speech data per frame)  *  
 * output PRM_SIZE int encoded data                                    *
 *---------------------------------------------------------------------*/
void va_g729a_encoder(void*handle,short *speech, unsigned char *bitstream)
{
	enc_inst*st=(enc_inst*)handle;

	INT16  i;
    for (i = 0; i < L_FRAME; i++)  st->new_speech[i] = (FLOAT) speech[i];

    pre_process( st,st->new_speech, L_FRAME);

    coder_ld8a(st,st->prm);
    prm2bits_ld8k(st->prm, bitstream);
}

void va_g729a_free_encoder(void*handle)
{
	enc_inst*st=(enc_inst*)handle;
	free(st);
}