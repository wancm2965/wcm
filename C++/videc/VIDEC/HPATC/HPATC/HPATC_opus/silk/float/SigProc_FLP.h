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

#ifndef _SILK_SIGPROC_FLP_H_
#define _SILK_SIGPROC_FLP_H_

#include "../SigProc_FIX.h"
#include <math.h>

#ifdef  __cplusplus
extern "C"
{
#endif

/********************************************************************/
/*                    SIGNAL PROCESSING FUNCTIONS                   */
/********************************************************************/

/* Chirp (bw expand) LP AR filter */
void silk_bwexpander_FLP(
    silk_float *ar,                     /* io   AR filter to be expanded (without leading 1)    */
    const opus_int d,                   /* i    length of ar                                       */
    const silk_float chirp              /* i    chirp factor (typically in range (0..1) )          */
);

/* compute inverse of LPC prediction gain, and                            */
/* test if LPC coefficients are stable (all poles within unit circle)    */
/* this code is based on silk_FLP_a2k()                                    */
opus_int silk_LPC_inverse_pred_gain_FLP( /* O:   returns 1 if unstable, otherwise 0    */
    silk_float            *invGain,      /* O:   inverse prediction gain, energy domain      */
    const silk_float      *A,            /* I:   prediction coefficients [order]           */
    opus_int32            order          /* I:   prediction order                          */
);

silk_float silk_schur_FLP(               /* O    returns residual energy                     */
    silk_float       refl_coef[],        /* O    reflection coefficients (length order)      */
    const silk_float auto_corr[],        /* I    autocorrelation sequence (length order+1)   */
    opus_int         order               /* I    order                                       */
);

void silk_k2a_FLP(
    silk_float           *A,             /* O:    prediction coefficients [order]           */
    const silk_float     *rc,            /* I:    reflection coefficients [order]           */
    opus_int32           order           /* I:    prediction order                          */
);

/* Solve the normal equations using the Levinson-Durbin recursion */
silk_float silk_levinsondurbin_FLP(        /* O    prediction error energy                        */
    silk_float        A[],                /* O    prediction coefficients    [order]                */
    const silk_float corr[],                /* I    input auto-correlations [order + 1]            */
    const opus_int    order                /* I    prediction order                             */
);

/* compute autocorrelation */
void silk_autocorrelation_FLP(
    silk_float *results,                 /* o    result (length correlationCount)            */
    const silk_float *inputData,         /* i    input data to correlate                     */
    opus_int inputDataSize,              /* i    length of input                             */
    opus_int correlationCount            /* i    number of correlation taps to compute       */
);

/* Pitch estimator */
#define SigProc_PE_MIN_COMPLEX        0
#define SigProc_PE_MID_COMPLEX        1
#define SigProc_PE_MAX_COMPLEX        2

opus_int silk_pitch_analysis_core_FLP(   /* O voicing estimate: 0 voiced, 1 unvoiced                         */
    const silk_float *signal,            /* I signal of length PE_FRAME_LENGTH_MS*Fs_kHz                     */
    opus_int         *pitch_out,         /* O 4 pitch lag values                                             */
    opus_int16       *lagIndex,          /* O lag Index                                                      */
    opus_int8        *contourIndex,      /* O pitch contour Index                                            */
    silk_float       *LTPCorr,           /* I/O normalized correlation; input: value from previous frame     */
    opus_int         prevLag,            /* I last lag of previous frame; set to zero is unvoiced            */
    const silk_float search_thres1,      /* I first stage threshold for lag candidates 0 - 1                 */
    const silk_float search_thres2,      /* I final threshold for lag candidates 0 - 1                       */
    const opus_int   Fs_kHz,             /* I sample frequency (kHz)                                         */
    const opus_int   complexity,         /* I Complexity setting, 0-2, where 2 is highest                    */
    const opus_int   nb_subfr            /* I    number of 5 ms subframes                                    */
);

#define PI               (3.1415926536f)

void silk_insertion_sort_decreasing_FLP(
    silk_float            *a,            /* I/O:  Unsorted / Sorted vector                */
    opus_int              *idx,          /* O:    Index vector for the sorted elements    */
    const opus_int        L,             /* I:    Vector length                           */
    const opus_int        K              /* I:    Number of correctly sorted positions    */
);

/* Compute reflection coefficients from input signal */
silk_float silk_burg_modified_FLP(           /* O    returns residual energy                                         */
    silk_float           A[],                /* O    prediction coefficients (length order)                          */
    const silk_float     x[],                /* I    input signal, length: nb_subfr*(D+L_sub)                        */
    const opus_int       subfr_length,       /* I    input signal subframe length (including D preceeding samples)   */
    const opus_int       nb_subfr,           /* I    number of subframes stacked in x                                */
    const silk_float     WhiteNoiseFrac,     /* I    fraction added to zero-lag autocorrelation                      */
    const opus_int       D                   /* I    order                                                           */
);

/* multiply a vector by a constant */
void silk_scale_vector_FLP(
    silk_float           *data1,
    silk_float           gain,
    opus_int             dataSize
);

/* copy and multiply a vector by a constant */
void silk_scale_copy_vector_FLP(
    silk_float           *data_out,
    const silk_float     *data_in,
    silk_float           gain,
    opus_int             dataSize
);

/* inner product of two silk_float arrays, with result as double */
double silk_inner_product_FLP(
    const silk_float     *data1,
    const silk_float     *data2,
    opus_int             dataSize
);

/* sum of squares of a silk_float array, with result as double */
double silk_energy_FLP(
    const silk_float     *data,
    opus_int             dataSize
);

/********************************************************************/
/*                                MACROS                                */
/********************************************************************/

#define silk_min_float(a, b)            (((a) < (b)) ? (a) :  (b))
#define silk_max_float(a, b)            (((a) > (b)) ? (a) :  (b))
#define silk_abs_float(a)            ((silk_float)fabs(a))

#define silk_LIMIT_float( a, limit1, limit2)    ((limit1) > (limit2) ? ((a) > (limit1) ? (limit1) : ((a) < (limit2) ? (limit2) : (a))) \
                                                                 : ((a) > (limit2) ? (limit2) : ((a) < (limit1) ? (limit1) : (a))))

/* sigmoid function */
static inline silk_float silk_sigmoid(silk_float x)
{
    return (silk_float)(1.0 / (1.0 + exp(-x)));
}

/* floating-point to integer conversion (rounding) */
static inline opus_int32 silk_float2int(double x)
{
#ifdef _WIN32
    double t = x + 6755399441055744.0;
    return *((opus_int32 *)( &t ));
#else
    return (opus_int32)( ( x > 0 ) ? x + 0.5 : x - 0.5 );
#endif
}

/* floating-point to integer conversion (rounding) */
static inline void silk_float2short_array(
    opus_int16       *out,
    const silk_float *in,
    opus_int32       length
)
{
    opus_int32 k;
    for (k = length-1; k >= 0; k--) {
#ifdef _WIN32
        double t = in[k] + 6755399441055744.0;
        out[k] = (opus_int16)silk_SAT16(*(( opus_int32 * )( &t )));
#else
        double x = in[k];
        out[k] = (opus_int16)silk_SAT16( ( x > 0 ) ? x + 0.5 : x - 0.5 );
#endif
    }
}

/* integer to floating-point conversion */
static inline void silk_short2float_array(
    silk_float       *out,
    const opus_int16 *in,
    opus_int32       length
)
{
    opus_int32 k;
    for (k = length-1; k >= 0; k--) {
        out[k] = (silk_float)in[k];
    }
}

/* using log2() helps the fixed-point conversion */
static inline silk_float silk_log2( double x ) { return ( silk_float )( 3.32192809488736 * log10( x ) ); }

#ifdef  __cplusplus
}
#endif

#endif
