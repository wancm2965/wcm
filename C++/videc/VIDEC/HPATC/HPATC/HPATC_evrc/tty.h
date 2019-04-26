/**********************************************************************
Each of the companies; Lucent, Motorola, Nokia, and Qualcomm (hereinafter 
referred to individually as "Source" or collectively as "Sources") do 
hereby state:

To the extent to which the Source(s) may legally and freely do so, the 
Source(s), upon submission of a Contribution, grant(s) a free, 
irrevocable, non-exclusive, license to the Third Generation Partnership 
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA, 
and TTC, under the Source's copyright or copyright license rights in the 
Contribution, to, in whole or in part, copy, make derivative works, 
perform, display and distribute the Contribution and derivative works 
thereof consistent with 3GPP2's and each Organizational Partner's 
policies and procedures, with the right to (i) sublicense the foregoing 
rights consistent with 3GPP2's and each Organizational Partner's  policies 
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name 
or each Organizational Partner's name any 3GPP2 or transposed Publication 
even though this Publication may contain the Contribution or a derivative 
work thereof.  The Contribution shall disclose any known limitations on 
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the 
formulating groups of 3GPP2 or any of its Organizational Partners, it 
is proposed to the Committee as a basis for discussion and is not to 
be construed as a binding proposal on the Source(s).  The Source(s) 
specifically reserve(s) the right to amend or modify the material 
contained in the Contribution. Nothing contained in the Contribution 
shall, except as herein expressly provided, be construed as conferring 
by implication, estoppel or otherwise, any license or right under (i) 
any existing or later issuing patent, whether or not the use of 
information in the document necessarily employs an invention of any 
existing or later issued patent, (ii) any copyright, (iii) any 
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or 
all Normative portions of the Enhanced Variable Rate Codec (EVRC) as 
it exists on the date of submittal of this form, should the EVRC be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the EVRC or the field of use of practice of the 
EVRC Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
 
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Conexant Systems, Inc., Qualcomm Inc., Nokia Inc,. Nortel         */
/* Networks, Lucent Technologies, and Motorola Inc. grants a free,   */
/* irrevocable license to 3GPP2 and its Organizational Partners to   */
/* incorporate text or other copyrightable material contained in the */
/* contribution and any modifications thereof in the creation of     */
/* 3GPP2 publications; to copyright and sell in Organizational       */
/* Partner's name any Organizational Partner's standards publication */
/* even though it may include portions of the contribution; and at   */
/* the Organizational Partner's sole discretion to permit others to  */
/* reproduce in whole or in part such contributions or the resulting */
/* Organizational Partner's standards publication. The contributor   */
/* must also be willing to grant licenses under such contributor     */
/* copyrights to third parties on reasonable, non-discriminatory     */
/* terms and conditions, as appropriate.                             */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Copyright 2001 Conexant Systems, Inc.  This document is provided  */
/* as a discussion instrument only and is not binding on Conexant    */
/* Systems, Inc. The contributor reserves the right to add to, amend */
/* or otherwise modify the contents thereof.                         */
/*                                                                   */
/* Conexant Systems, Inc. grants a free, irrevocable license to 3GPP2*/
/* and its Organization Partners to incorporate, for any legitimate  */
/* 3GPP2 purpose, any copyrightable material contained in this       */
/* contribution and any revisions thereof, in publications of 3GPP2. */
/* The contributor may hold one or more patents or applications for  */
/* letters patent that cover the information contained in this       */
/* contribution.  Noting contained in this copyright statement shall */
/* be construed as conferring by implication or estoppel, or         */
/* otherwise a license or any other right under any existing  or     */
/* later issuing patent, whether or not the use of information       */
/* herein necessarily employs an invention of any such patent or     */
/* later issuing patent.                                             */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Copyright QUALCOMM Incorporated. All rights reserved.             */
/* The information contained in this contribution is provided for    */
/* the sole purpose of promoting discussion within the 3GPP2 and its */ 
/* Organization Partners and is not binding on the contributor.      */
/* The contributor reserves the right to add to, amend, or withdraw  */
/* the statements contained herein.                                  */
/* QUALCOMM Incorporated grants a free, irrevocable license to 3GPP2 */
/* and its Organization Partners to incorporate text or other        */
/* copyrightable material contained in the  contribution and any     */
/* modifications thereof in the creation of 3GPP2 publications;      */
/* to copyright and sell in Organizational Partner's name any        */
/* Organizational Partner's standards publication even though it may */
/* include portions of the contribution; and at the Organization     */
/* Partner's sole discretion to permit others to reproduce in whole  */
/* or in part such contributions or the resulting Organizational     */
/* Partner's standards publication.  The contributor may hold one or */
/* more patents or copyrights that cover information contained in    */
/* this contribution. A license will be made available to applicants */
/* under reasonable terms and conditions that are demonstrably free  */
/* of any unfair discrimination.                                     */
/* Nothing contained herein shall be construed as conferring by      */
/* implication, estoppel, or otherwise any license or right under    */
/* any patent, whether or not the use of information herein          */
/* necessarily employs an invention of any existing or later issued  */
/* patent, or copyright. The contributor reserves the right to use   */
/* all material submitted in this contribution for his own purposes, */
/* including republication and distribution to others.               */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* The proposals in this submission have been formulated by Nokia,   */
/* Inc. (Nokia), to assist the 3GPP2 Standards Committee. This       */
/* document is offered  to the committee as a basis for discussion   */
/* and is not binding on Nokia. The requirements are subject to      */
/* change in form and in numerical values after more study. Nokia    */
/* specifically reserves the right to add to, or amend, the          */
/* quantitative statements made herein. Nothing contained herein     */
/* shall be construed as conferring by implication, estoppel, or     */
/* otherwise any license or right under any patent, whether or not   */
/* the use of information herein necessarily employs an invention of */
/* any existing or later issued patent.                              */
/* Copyright 2001 Nokia, Inc. All Rights Reserved. Nokia hereby      */
/* gives permission for copying this submission for the legitimate   */
/* purposes of the 3GPP2 Standards Committee, provided Nokia, Inc.   */
/* is credited on all copies. Distribution or reproduction of this   */
/* document, by any means, electronic, mechanical, or otherwise, in  */
/* its entirety, or any portion thereof, for monetary gain or any    */
/*  non-3GPP2 purposes is expressly prohibited.                      */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Nortel Networks grants a free, irrevocable license to 3GPP2 and   */
/* its Organizational Partners to incorporate text or other          */
/* copyrightable material contained in the contribution and any      */
/* modifications thereof in the creation of 3GPP2 publications; to   */
/* copyright and sell in Organizational Partner's name any           */
/* Organizational Partner's standards publication even though it may */
/* include portions of the contribution; and at the Organizational   */
/* Partner's sole discretion to permit others to reproduce in whole  */
/* or in part such contributions or the resulting Organizational     */
/* Partner's standards publication. The contributor must also be     */
/* willing to grant licenses under such contributor copyrights to    */
/* third parties on reasonable, non-discriminatory terms and         */
/* conditions, as appropriate.                                       */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Notice:                                                           */
/* This contribution has been prepared by the contributor to assist  */
/* 3GPP2 Technical Specifications Group C. This document is offered  */
/* to 3GPP2 Technical Specifications Group C as a basis for          */
/* discussion and should not be construed as a binding proposal on   */
/* Lucent Technologies Inc. or any other company.  Specifically,     */
/* Lucent Technologies Inc. reserves the right to modify, amend, or  */
/* withdraw the statements contained herein.                         */
/*                                                                   */
/* Permission is granted to 3GPP2 Technical Specifications Group C   */
/* participants to copy any portion of this document for legitimate  */
/* purposes of 3GPP2 Technical Specifications Group C. Copying for   */
/* monetary gain or other non-3GPP2 Technical Specifications Group C */
/* purposes is prohibited.                                           */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Grant of license Motorola Inc. grants a free, irrevocable license */
/* to 3GPP2 and its organizational partners to incorporate Motorola- */
/* supplied text or other copyrightable material contained in the    */
/* contribution and any modifications thereof in the creation of     */ 
/* 3GPP2 publications, to copyright and sell in organizational       */
/* partners name any organizational partners standards publications  */
/* even though it may include portions of the contribution; and at   */
/* the organizational partners sole discretion to permit others      */
/* to reproduce in whole or in part such contributions or the        */
/* resulting organizational partners standards publication. Motorola */
/* is also willing to grant licenses under such Motorola copyrights  */
/* to third parties on reasonable, non-discriminatory terms and      */
/* conditions, as appropriate.                                       */
/*                                                                   */
/* Notice:                                                           */
/* This document has been prepared by Motorola Inc. to assist the    */
/* 3GPP2 standards committee. This document is offered to the        */
/* committee as a basis for discussion and should not be considered  */
/* as a binding proposal on Motorola Inc.  or any other company.     */
/* Specifically, Motorola Inc. reserves the right to modify, amend,  */
/* or withdraw the statement contained herein. Permission is granted */
/* to 3GPP2 and its organizational partners to copy any portion of   */
/* this document for the legitimate purposes of the 3GPP2.  Copying  */
/* this document for monetary gain or other non-3GPP2 purpose is     */
/* prohibited.  Motorola Inc. may hold one or more patents of        */
/* copyrights that cover information contained in this contribution, */
/* and agrees that a license under those rights will be made         */
/* available on reasonable and non-discriminatory terms and          */
/* conditions, subject to receiving a reciprocal license in return.  */
/* Nothing contained herein shall be construed as conferring by      */
/* implication, estoppel, or otherwise any license or right under    */
/* any patent, whether or not the use of information herein          */
/* necessarily employs an invention of any existing or later issued  */
/* patent, or copyright.                                             */
/*                                                                   */
/* Notice                                                            */
/* Permission is granted to 3GPP2 participants to copy any portion of*/
/* this contribution for the legitimate purpose of the 3GPP2.        */
/* Copying this contribution for monetary gain or other non-3GPP2    */
/* purpose is prohibited.                                            */
/*                                                                   */
/*-------------------------------------------------------------------*/


#ifndef _TTY_H_
#define _TTY_H_

#include <stdio.h>
#include "typedef_fx.h"
#include "tty_hdr.h"

#define TTY_VERSION     "SMV TTY 31 Aug 2001"


/* Thresholds for get_tty_bit() and get_tty_char() */

#define MIN_BIT_LEN         8   /* min # of dits, good and bad, to make a bit */
#define MAX_BIT_LEN         13  /* max # of dits, good and bad, to make a bit */

#define DITBUF_LEN          MAX_BIT_LEN+DITS_PER_FRAME+1

#define TTY_DISABLED        0
#define TTY_NO_GAIN         1
#define TTY_AUDIO_DET       2

#define TTY_BIT_HIST_LEN    9
#define MEM_BIT_IDX         0
#define START_BIT_IDX       1
#define STOP_BIT_IDX        7

#define START_BIT           0
#define STOP_BIT            1
#define BAUDOT_GAIN         8192
#define START_BIT_LEN       176
#define DATA_BIT_LEN        176
#define STOP_BIT_LEN        (2*DATA_BIT_LEN)
#define TTY_CHAR_LEN        (START_BIT_LEN+5*DATA_BIT_LEN+STOP_BIT_LEN)

#define NUM_TTY_RATES           2
#define TTY_45_BAUD             0
#define TTY_50_BAUD             1
#define DEFAULT_TTY_BAUD_RATE   TTY_45_BAUD

#define BIT_LEN_45_BAUD         11      /* # dits in a 50 Baud TTY bit */
#define DATA_BIT_LEN_45_BAUD    176
#define STOP_BIT_LEN_45_BAUD    ((3*DATA_BIT_LEN_45_BAUD) >> 1 )

#define BIT_LEN_50_BAUD         10      /* # dits in a 50 Baud TTY bit */
#define DATA_BIT_LEN_50_BAUD    160
#define STOP_BIT_LEN_50_BAUD    280 /* 1.75 bits for stop bit */

#define MARK_HOLD_BIT       1
#define MARK_HOLD_LEN       2400    /* 300 ms. */

#define DEFAULT_DATA_BIT_LEN    DATA_BIT_LEN_45_BAUD
#define DEFAULT_STOP_BIT_LEN    STOP_BIT_LEN_45_BAUD



#define MARK_FREQ               0x3A1C  /* Q14 2*cos(w) 1400 Hz */
#define SPACE_FREQ              0x1406  /* Q14 2*cos(w) 1800 Hz */

/* Define dit values */
#define LOGIC_0         0
#define LOGIC_1         1
#define SILENCE         2
#define ERASE           3
#define UNKNOWN         4 /*NON_TTY*/
#define START_BIT_VAL   LOGIC_0
#define STOP_BIT_VAL    LOGIC_1


/* Define values that can be used in char_counter and tty_char */
#define TTY_CHAR_MIN            0
#define TTY_CHAR_MAX            31

/* TTY states */
#define NON_TTY_MODE            1
#define TTY_MODE_NOT_FRAMED     2
#define TTY_MODE_FRAMED         4

#ifndef PI
#define PI              3.14159265358979323846
#endif

#ifndef MAX
#define MAX(x,y)    (((x)>(y))?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y)    (((x)<(y))?(x):(y))
#endif

/*** TTY Globals ***/
extern Word16   tty_option;
extern Word16   tty_enc_flag;
extern Word16   tty_enc_header;
extern Word16   tty_enc_char;
extern Word16   tty_enc_baud_rate;
extern Word16   tty_dec_flag;
extern Word16   tty_dec_header;
extern Word16   tty_dec_char;
extern Word16   tty_dec_baud_rate;
extern Word16   data_flag;
extern Word16   speech_data_flag;

extern void init_tty_enc(
    Word16   *tty_char,
    Word16   *counter,
    Word16   *tty_baud_rate
);

extern Word16 tty_enc(
    Word16   *tty_char,      /* (o): tty character           */
    Word16   *counter,       /* (o): tty character counter   */
    Word16   *tty_baud_rate, /* (i/o): prev/new character counter    */
    Word16   pcm_buf[],      /* (i): input pcm               */
    Word16   len             /* (i): length of pcm buffer (FRAMESIZE)    */
);

extern void init_dit_to_ascii();

extern void dit_to_ascii(
    Word16   *tty_char,      /* (i/o): prev/new character decision   */
    Word16   *char_counter,  /* (i/o): prev/new character counter    */
    Word16   *tty_baud_rate, /* (i/o): prev/new character counter    */
    Word16   ditInbuf[]      /* (i): dits for current frame          */
);

extern void get_tty_state(
    Word16   *ttyState,              /* (i/o): TTY State                 */
    Word16   first_tty_char_flag,    /* (i): first char detected flag    */
    Word16   tty_bit_hist[],         /* (i): TTY bit history buffer        */
    Word16   tty_bit_len_hist[]      /* (i): TTY bit length history buffer */
);

extern void init_get_tty_bit();

extern Word16 get_tty_bit(
    Word16   tty_bit_hist[],         /* (i/o): bit decision          */
    Word16   tty_bit_len_hist[],     /* (i/o): bit length            */
    Word16   *bit_index,             /* (o): bit index in ditbuf     */
    Word16   *inbuf,                 /* (i/o): dit buffer            */
    Word16   baud_rate               /* (i): baud_rate               */
);

extern Word16 get_tty_char(
    Word16   *tty_char,              /* (o): TTY character               */
    Word16   tty_bit_hist[],         /* (i): TTY bit history buffer      */
    Word16   tty_bit_len_hist[]      /* (i): TTY bit length history buffer */
);

extern void init_baudot_to_dit();

extern void baudot_to_dit(
    Word16    ditbuf[],              /* (o): Dit decisions           */
    Word16    inbuf[]                /* (i): Input PCM               */
);

extern void init_tty_dec();

extern Word16 tty_dec(
    Word16   buf[],
    Word16   acb_gain,
    Word16   counter,
    Word16   tty_char,
    Word16   tty_baud_rate,
    Word16   fer_flag,
    Word16   subframe,
    Word16   num_subfr,
    Word16   length
);

extern void init_tty_gen(
    Word16 counter,
    Word16 tty_char,
    Word16 tty_baud_rate
);

extern Word16 tty_gen(
    Word16   outbuf[],
    Word16   length,
    Word16   subframe,
    Word16   num_subfr,
    Word16   counter_hist[],
    Word16   char_hist[],
    Word16   tty_rate_hist[]
);

extern void tone_gen(
    Word16       outbuf[],
    Word16       freq,
    Word16       volume,
    Word16       len,           
    Word16       param[]);

extern void init_tone_gen(
    Word16       param[],        /* (o) tone generation parameters       */
    Word16       freq );         /* (i) Desired freq in Hz               */

extern void tty_rate(
    Word16   *tty_baud_rate,     /* (i/o): prev/new tty baud rate    */
    Word16   tty_bit_len_hist[]  /* (i): bit len buffer              */
);

extern void tty_header_in(Word16 *counter);
extern void tty_header_out(Word16 *counter);

#endif /* _TTY_H_ */
