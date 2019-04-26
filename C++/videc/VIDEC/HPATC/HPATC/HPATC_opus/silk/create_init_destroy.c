/***********************************************************************
Copyright (c) 2006-2011, Skype Limited. All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, (subject to the limitations in the disclaimer below)
are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
- Neither the name of Skype Limited, nor the names of specific
contributors, may be used to endorse or promote products derived from
this software without specific prior written permission.
NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#if  1
#include "../win32/config.h"
#endif

#include "main.h"
#include "plc.h"

#define NB_ATT 2
static const opus_int16 HARM_ATT_Q15[NB_ATT]              = { 32440, 31130 }; /* 0.99, 0.95 */
static const opus_int16 PLC_RAND_ATTENUATE_V_Q15[NB_ATT]  = { 31130, 26214 }; /* 0.95, 0.8 */
static const opus_int16 PLC_RAND_ATTENUATE_UV_Q15[NB_ATT] = { 32440, 29491 }; /* 0.99, 0.9 */

void c_silk_PLC_Reset(
    silk_decoder_state      *psDec              /* I/O Decoder state        */
)
{
    psDec->sPLC.pitchL_Q8 = silk_RSHIFT( psDec->frame_length, 1 );
}

void silk_PLC(
    silk_decoder_state          *psDec,             /* I Decoder state          */
    silk_decoder_control        *psDecCtrl,         /* I Decoder control        */
    opus_int16                   frame[],            /* O Concealed signal       */
    opus_int                     length,             /* I length of residual     */
    opus_int                     lost                /* I Loss flag              */
)
{
    /* PLC control function */
    if( psDec->fs_kHz != psDec->sPLC.fs_kHz ) {
        c_silk_PLC_Reset( psDec );
        psDec->sPLC.fs_kHz = psDec->fs_kHz;
    }

    if( lost ) {
        /****************************/
        /* Generate Signal          */
        /****************************/
        silk_PLC_conceal( psDec, psDecCtrl, frame, length );

        psDec->lossCnt++;
    } else {
        /****************************/
        /* Update state             */
        /****************************/
        silk_PLC_update( psDec, psDecCtrl, frame, length );
    }
}

/**************************************************/
/* Update state of PLC                            */
/**************************************************/
void silk_PLC_update(
    silk_decoder_state          *psDec,             /* (I/O) Decoder state          */
    silk_decoder_control        *psDecCtrl,         /* (I/O) Decoder control        */
    opus_int16                   frame[],
    opus_int                     length
)
{
    opus_int32 LTP_Gain_Q14, temp_LTP_Gain_Q14;
    opus_int   i, j;
    silk_PLC_struct *psPLC;

    psPLC = &psDec->sPLC;

    /* Update parameters used in case of packet loss */
    psDec->prevSignalType = psDec->indices.signalType;
    LTP_Gain_Q14 = 0;
    if( psDec->indices.signalType == TYPE_VOICED ) {
        /* Find the parameters for the last subframe which contains a pitch pulse */
        for( j = 0; j * psDec->subfr_length < psDecCtrl->pitchL[ psDec->nb_subfr - 1 ]; j++ ) {
            if( j == psDec->nb_subfr ){
                break;
            }
            temp_LTP_Gain_Q14 = 0;
            for( i = 0; i < LTP_ORDER; i++ ) {
                temp_LTP_Gain_Q14 += psDecCtrl->LTPCoef_Q14[ ( psDec->nb_subfr - 1 - j ) * LTP_ORDER  + i ];
            }
            if( temp_LTP_Gain_Q14 > LTP_Gain_Q14 ) {
                LTP_Gain_Q14 = temp_LTP_Gain_Q14;
                silk_memcpy( psPLC->LTPCoef_Q14,
                    &psDecCtrl->LTPCoef_Q14[ silk_SMULBB( psDec->nb_subfr - 1 - j, LTP_ORDER ) ],
                    LTP_ORDER * sizeof( opus_int16 ) );

                psPLC->pitchL_Q8 = silk_LSHIFT( psDecCtrl->pitchL[ psDec->nb_subfr - 1 - j ], 8 );
            }
        }

#if USE_SINGLE_TAP
        silk_memset( psPLC->LTPCoef_Q14, 0, LTP_ORDER * sizeof( opus_int16 ) );
        psPLC->LTPCoef_Q14[ LTP_ORDER / 2 ] = LTP_Gain_Q14;
#endif

        /* Limit LT coefs */
        if( LTP_Gain_Q14 < V_PITCH_GAIN_START_MIN_Q14 ) {
            opus_int   scale_Q10;
            opus_int32 tmp;

            tmp = silk_LSHIFT( V_PITCH_GAIN_START_MIN_Q14, 10 );
            scale_Q10 = silk_DIV32( tmp, silk_max( LTP_Gain_Q14, 1 ) );
            for( i = 0; i < LTP_ORDER; i++ ) {
                psPLC->LTPCoef_Q14[ i ] = silk_RSHIFT( silk_SMULBB( psPLC->LTPCoef_Q14[ i ], scale_Q10 ), 10 );
            }
        } else if( LTP_Gain_Q14 > V_PITCH_GAIN_START_MAX_Q14 ) {
            opus_int   scale_Q14;
            opus_int32 tmp;

            tmp = silk_LSHIFT( V_PITCH_GAIN_START_MAX_Q14, 14 );
            scale_Q14 = silk_DIV32( tmp, silk_max( LTP_Gain_Q14, 1 ) );
            for( i = 0; i < LTP_ORDER; i++ ) {
                psPLC->LTPCoef_Q14[ i ] = silk_RSHIFT( silk_SMULBB( psPLC->LTPCoef_Q14[ i ], scale_Q14 ), 14 );
            }
        }
    } else {
        psPLC->pitchL_Q8 = silk_LSHIFT( silk_SMULBB( psDec->fs_kHz, 18 ), 8 );
        silk_memset( psPLC->LTPCoef_Q14, 0, LTP_ORDER * sizeof( opus_int16 ));
    }

    /* Save LPC coeficients */
    silk_memcpy( psPLC->prevLPC_Q12, psDecCtrl->PredCoef_Q12[ 1 ], psDec->LPC_order * sizeof( opus_int16 ) );
    psPLC->prevLTP_scale_Q14 = psDecCtrl->LTP_scale_Q14;

    /* Save Gains */
    silk_memcpy( psPLC->prevGain_Q16, psDecCtrl->Gains_Q16, psDec->nb_subfr * sizeof( opus_int32 ) );
}

void silk_PLC_conceal(
    silk_decoder_state          *psDec,             /* I/O Decoder state */
    silk_decoder_control        *psDecCtrl,         /* I/O Decoder control */
    opus_int16                   frame[],            /* O concealed signal */
    opus_int                     length              /* I length of residual */
)
{
    opus_int   i, j, k;
    opus_int16 *B_Q14, exc_buf[ MAX_FRAME_LENGTH ], *exc_buf_ptr;
    opus_int16 rand_scale_Q14, A_Q12_tmp[ MAX_LPC_ORDER ];
    opus_int32 rand_seed, harm_Gain_Q15, rand_Gain_Q15;
    opus_int   lag, idx, sLTP_buf_idx, shift1, shift2;
    opus_int32 energy1, energy2, *rand_ptr, *pred_lag_ptr;
    opus_int32 sig_Q10[ MAX_FRAME_LENGTH ], *sig_Q10_ptr, LPC_exc_Q10, LPC_pred_Q10,  LTP_pred_Q14;
    silk_PLC_struct *psPLC;
    psPLC = &psDec->sPLC;

    /* Update LTP buffer */
    silk_memmove( psDec->sLTP_Q16, &psDec->sLTP_Q16[ psDec->frame_length ], psDec->ltp_mem_length * sizeof( opus_int32 ) );

    /* LPC concealment. Apply BWE to previous LPC */
    silk_bwexpander( psPLC->prevLPC_Q12, psDec->LPC_order, SILK_FIX_CONST( BWE_COEF, 16 ) );

    /* Find random noise component */
    /* Scale previous excitation signal */
    exc_buf_ptr = exc_buf;
    /* FIXME: JMV: Is this the right fix? */
    for (i=0;i<MAX_FRAME_LENGTH;i++)
        exc_buf[i] = 0;
    for( k = ( psDec->nb_subfr >> 1 ); k < psDec->nb_subfr; k++ ) {
        for( i = 0; i < psDec->subfr_length; i++ ) {
            exc_buf_ptr[ i ] = ( opus_int16 )silk_RSHIFT(
                silk_SMULWW( psDec->exc_Q10[ i + k * psDec->subfr_length ], psPLC->prevGain_Q16[ k ] ), 10 );
        }
        exc_buf_ptr += psDec->subfr_length;
    }
    /* Find the subframe with lowest energy of the last two and use that as random noise generator */
    silk_sum_sqr_shift( &energy1, &shift1, exc_buf,                         psDec->subfr_length );
    silk_sum_sqr_shift( &energy2, &shift2, &exc_buf[ psDec->subfr_length ], psDec->subfr_length );

    if( silk_RSHIFT( energy1, shift2 ) < silk_RSHIFT( energy2, shift1 ) ) {
        /* First sub-frame has lowest energy */
        rand_ptr = &psDec->exc_Q10[ silk_max_int( 0, 3 * psDec->subfr_length - RAND_BUF_SIZE ) ];
    } else {
        /* Second sub-frame has lowest energy */
        rand_ptr = &psDec->exc_Q10[ silk_max_int( 0, psDec->frame_length - RAND_BUF_SIZE ) ];
    }

    /* Setup Gain to random noise component */
    B_Q14          = psPLC->LTPCoef_Q14;
    rand_scale_Q14 = psPLC->randScale_Q14;

    /* Setup attenuation gains */
    harm_Gain_Q15 = HARM_ATT_Q15[ silk_min_int( NB_ATT - 1, psDec->lossCnt ) ];
    if( psDec->prevSignalType == TYPE_VOICED ) {
        rand_Gain_Q15 = PLC_RAND_ATTENUATE_V_Q15[  silk_min_int( NB_ATT - 1, psDec->lossCnt ) ];
    } else {
        rand_Gain_Q15 = PLC_RAND_ATTENUATE_UV_Q15[ silk_min_int( NB_ATT - 1, psDec->lossCnt ) ];
    }

    /* First Lost frame */
    if( psDec->lossCnt == 0 ) {
        rand_scale_Q14 = 1 << 14;

        /* Reduce random noise Gain for voiced frames */
        if( psDec->prevSignalType == TYPE_VOICED ) {
            for( i = 0; i < LTP_ORDER; i++ ) {
                rand_scale_Q14 -= B_Q14[ i ];
            }
            rand_scale_Q14 = silk_max_16( 3277, rand_scale_Q14 ); /* 0.2 */
            rand_scale_Q14 = ( opus_int16 )silk_RSHIFT( silk_SMULBB( rand_scale_Q14, psPLC->prevLTP_scale_Q14 ), 14 );
        } else {
            /* Reduce random noise for unvoiced frames with high LPC gain */
            opus_int32 invGain_Q30, down_scale_Q30;

            silk_LPC_inverse_pred_gain( &invGain_Q30, psPLC->prevLPC_Q12, psDec->LPC_order );

            down_scale_Q30 = silk_min_32( silk_RSHIFT( 1 << 30, LOG2_INV_LPC_GAIN_HIGH_THRES ), invGain_Q30 );
            down_scale_Q30 = silk_max_32( silk_RSHIFT( 1 << 30, LOG2_INV_LPC_GAIN_LOW_THRES ), down_scale_Q30 );
            down_scale_Q30 = silk_LSHIFT( down_scale_Q30, LOG2_INV_LPC_GAIN_HIGH_THRES );

            rand_Gain_Q15 = silk_RSHIFT( silk_SMULWB( down_scale_Q30, rand_Gain_Q15 ), 14 );
        }
    }

    rand_seed    = psPLC->rand_seed;
    lag          = silk_RSHIFT_ROUND( psPLC->pitchL_Q8, 8 );
    sLTP_buf_idx = psDec->ltp_mem_length;

    /***************************/
    /* LTP synthesis filtering */
    /***************************/
    sig_Q10_ptr = sig_Q10;
    for( k = 0; k < psDec->nb_subfr; k++ ) {
        /* Setup pointer */
        pred_lag_ptr = &psDec->sLTP_Q16[ sLTP_buf_idx - lag + LTP_ORDER / 2 ];
        for( i = 0; i < psDec->subfr_length; i++ ) {
            rand_seed = silk_RAND( rand_seed );
            idx = silk_RSHIFT( rand_seed, 25 ) & RAND_BUF_MASK;

            /* Unrolled loop */
            LTP_pred_Q14 = silk_SMULWB(               pred_lag_ptr[  0 ], B_Q14[ 0 ] );
            LTP_pred_Q14 = silk_SMLAWB( LTP_pred_Q14, pred_lag_ptr[ -1 ], B_Q14[ 1 ] );
            LTP_pred_Q14 = silk_SMLAWB( LTP_pred_Q14, pred_lag_ptr[ -2 ], B_Q14[ 2 ] );
            LTP_pred_Q14 = silk_SMLAWB( LTP_pred_Q14, pred_lag_ptr[ -3 ], B_Q14[ 3 ] );
            LTP_pred_Q14 = silk_SMLAWB( LTP_pred_Q14, pred_lag_ptr[ -4 ], B_Q14[ 4 ] );
            pred_lag_ptr++;

            /* Generate LPC residual */
            LPC_exc_Q10 = silk_LSHIFT( silk_SMULWB( rand_ptr[ idx ], rand_scale_Q14 ), 2 ); /* Random noise part */
            LPC_exc_Q10 = silk_ADD32( LPC_exc_Q10, silk_RSHIFT_ROUND( LTP_pred_Q14, 4 ) );  /* Harmonic part */

            /* Update states */
            psDec->sLTP_Q16[ sLTP_buf_idx ] = silk_LSHIFT( LPC_exc_Q10, 6 );
            sLTP_buf_idx++;

            /* Save LPC residual */
            sig_Q10_ptr[ i ] = LPC_exc_Q10;
        }
        sig_Q10_ptr += psDec->subfr_length;
        /* Gradually reduce LTP gain */
        for( j = 0; j < LTP_ORDER; j++ ) {
            B_Q14[ j ] = silk_RSHIFT( silk_SMULBB( harm_Gain_Q15, B_Q14[ j ] ), 15 );
        }
        /* Gradually reduce excitation gain */
        rand_scale_Q14 = silk_RSHIFT( silk_SMULBB( rand_scale_Q14, rand_Gain_Q15 ), 15 );

        /* Slowly increase pitch lag */
        psPLC->pitchL_Q8 += silk_SMULWB( psPLC->pitchL_Q8, PITCH_DRIFT_FAC_Q16 );
        psPLC->pitchL_Q8 = silk_min_32( psPLC->pitchL_Q8, silk_LSHIFT( silk_SMULBB( MAX_PITCH_LAG_MS, psDec->fs_kHz ), 8 ) );
        lag = silk_RSHIFT_ROUND( psPLC->pitchL_Q8, 8 );
    }

    /***************************/
    /* LPC synthesis filtering */
    /***************************/
    sig_Q10_ptr = sig_Q10;
    /* Preload LPC coeficients to array on stack. Gives small performance gain */
    silk_memcpy( A_Q12_tmp, psPLC->prevLPC_Q12, psDec->LPC_order * sizeof( opus_int16 ) );
    silk_assert( psDec->LPC_order >= 10 ); /* check that unrolling works */
    for( k = 0; k < psDec->nb_subfr; k++ ) {
        for( i = 0; i < psDec->subfr_length; i++ ){
            /* partly unrolled */
            LPC_pred_Q10 = silk_SMULWB(               psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  1 ], A_Q12_tmp[ 0 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  2 ], A_Q12_tmp[ 1 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  3 ], A_Q12_tmp[ 2 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  4 ], A_Q12_tmp[ 3 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  5 ], A_Q12_tmp[ 4 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  6 ], A_Q12_tmp[ 5 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  7 ], A_Q12_tmp[ 6 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  8 ], A_Q12_tmp[ 7 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i -  9 ], A_Q12_tmp[ 8 ] );
            LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i - 10 ], A_Q12_tmp[ 9 ] );

            for( j = 10; j < psDec->LPC_order; j++ ) {
                LPC_pred_Q10 = silk_SMLAWB( LPC_pred_Q10, psDec->sLPC_Q14[ MAX_LPC_ORDER + i - j - 1 ], A_Q12_tmp[ j ] );
            }

            /* Add prediction to LPC residual */
            sig_Q10_ptr[ i ] = silk_ADD32( sig_Q10_ptr[ i ], LPC_pred_Q10 );

            /* Update states */
            psDec->sLPC_Q14[ MAX_LPC_ORDER + i ] = silk_LSHIFT( sig_Q10_ptr[ i ], 4 );
        }
        sig_Q10_ptr += psDec->subfr_length;
        /* Update LPC filter state */
        silk_memcpy( psDec->sLPC_Q14, &psDec->sLPC_Q14[ psDec->subfr_length ], MAX_LPC_ORDER * sizeof( opus_int32 ) );
    }

    /* Scale with Gain */
    for( i = 0; i < psDec->frame_length; i++ ) {
        frame[ i ] = ( opus_int16 )silk_SAT16( silk_RSHIFT_ROUND( silk_SMULWW( sig_Q10[ i ], psPLC->prevGain_Q16[ psDec->nb_subfr - 1 ] ), 10 ) );
    }

    /**************************************/
    /* Update states                      */
    /**************************************/
    psPLC->rand_seed     = rand_seed;
    psPLC->randScale_Q14 = rand_scale_Q14;
    for( i = 0; i < MAX_NB_SUBFR; i++ ) {
        psDecCtrl->pitchL[ i ] = lag;
    }
}

/* Glues concealed frames with new good recieved frames             */
void silk_PLC_glue_frames(
    silk_decoder_state          *psDec,             /* I/O decoder state    */
    silk_decoder_control        *psDecCtrl,         /* I/O Decoder control  */
    opus_int16                   frame[],            /* I/O signal           */
    opus_int                     length              /* I length of residual */
)
{
    opus_int   i, energy_shift;
    opus_int32 energy;
    silk_PLC_struct *psPLC;
    psPLC = &psDec->sPLC;

    if( psDec->lossCnt ) {
        /* Calculate energy in concealed residual */
        silk_sum_sqr_shift( &psPLC->conc_energy, &psPLC->conc_energy_shift, frame, length );

        psPLC->last_frame_lost = 1;
    } else {
        if( psDec->sPLC.last_frame_lost ) {
            /* Calculate residual in decoded signal if last frame was lost */
            silk_sum_sqr_shift( &energy, &energy_shift, frame, length );

            /* Normalize energies */
            if( energy_shift > psPLC->conc_energy_shift ) {
                psPLC->conc_energy = silk_RSHIFT( psPLC->conc_energy, energy_shift - psPLC->conc_energy_shift );
            } else if( energy_shift < psPLC->conc_energy_shift ) {
                energy = silk_RSHIFT( energy, psPLC->conc_energy_shift - energy_shift );
            }

            /* Fade in the energy difference */
            if( energy > psPLC->conc_energy ) {
                opus_int32 frac_Q24, LZ;
                opus_int32 gain_Q16, slope_Q16;

                LZ = silk_CLZ32( psPLC->conc_energy );
                LZ = LZ - 1;
                psPLC->conc_energy = silk_LSHIFT( psPLC->conc_energy, LZ );
                energy = silk_RSHIFT( energy, silk_max_32( 24 - LZ, 0 ) );

                frac_Q24 = silk_DIV32( psPLC->conc_energy, silk_max( energy, 1 ) );

                gain_Q16 = silk_LSHIFT( silk_SQRT_APPROX( frac_Q24 ), 4 );
                slope_Q16 = silk_DIV32_16( ( 1 << 16 ) - gain_Q16, length );
                /* Make slope 4x steeper to avoid missing onsets after DTX */
                slope_Q16 = silk_LSHIFT( slope_Q16, 2 );

                for( i = 0; i < length; i++ ) {
                    frame[ i ] = silk_SMULWB( gain_Q16, frame[ i ] );
                    gain_Q16 += slope_Q16;
                    if( gain_Q16 > 1 << 16 ) {
                        break;
                    }
                }
            }
        }
        psPLC->last_frame_lost = 0;
    }
}


/************************/
/* Init Decoder State   */
/************************/
opus_int silk_init_decoder(
    silk_decoder_state      *psDec              /* I/O  Decoder state pointer                       */
)
{
    /* Clear the entire encoder state, except anything copied */
    silk_memset( psDec, 0, sizeof( silk_decoder_state ) );

    /* Used to deactivate e.g. LSF interpolation and fluctuation reduction */
    psDec->first_frame_after_reset = 1;
    psDec->prev_inv_gain_Q16 = 65536;

    /* Reset CNG state */
    silk_CNG_Reset( psDec );

    /* Reset PLC state */
    c_silk_PLC_Reset( psDec );

    return(0);
}

