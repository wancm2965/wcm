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

#include <stdio.h>
//#include <math.h>
#include "typedef.h"
#include "tty.h"
#include "basic_op.h"

#include "tty_dbg.h"

#define DEBUG_LEVEL     1
#define DEBUG(n,x)      if( (n <= DEBUG_LEVEL || DEBUG_LEVEL < 0) && tty_debug_print_flag) {x}
unsigned long	tty_dec_char_count = 0;		/* for debugging only */

#define STOP_BIT_MASK       0x00C0
#define DATA_BIT_MASK       0x001F

#define STOP_BIT_NUM        6
#define MARK_HOLD_BIT_NUM   7

Word16    bit_num;
Word16    bit_size;
Word16    current_counter;
Word16    current_char;
Word16    prev_bit;
Word16    tone_param[3];

Word16   stop_bit_len[NUM_TTY_RATES];    /* initialized in init_tty_dec() & init_tty_rxtx() */
Word16   data_bit_len[NUM_TTY_RATES];    /* initialized in init_tty_dec() & init_tty_rxtx() */

/*------------------------------------------------------------------
*                       init_tty_gen()
*-------------------------------------------------------------------*/
void init_tty_gen(Word16 counter, Word16 tty_char, Word16 tty_baud_rate)
{

    bit_size = data_bit_len[tty_baud_rate];                 
    bit_num = 0;                                            
    prev_bit = MARK_FREQ;                                   
    current_counter = counter;                              
    current_char = shl((tty_char & DATA_BIT_MASK),1) | STOP_BIT_MASK;

    DEBUG(1,
        if( (counter & COUNTER_BETWEEN_START_STOP) != 0 )
        {
            fprintf(stdout,"tty_dec(): %lu: Got new char = %2d  counter = %2d  rate = %d\n",
                ++tty_dec_char_count,
                tty_char,
                counter,
                tty_baud_rate);
        }
    )
}

/*------------------------------------------------------------------
*                       tty_gen()
*-------------------------------------------------------------------*/
Word16 tty_gen(
    Word16   outbuf[],           /* (o): output buffer                   */
    Word16   length,             /* (i): length of buffer                */
    Word16   subframe,           /* (i): subframe counter                */
    Word16   num_subfr,          /* (i): number of subframes/frame       */
    Word16   counter_hist[],     /* (i/o): tty counter history           */
    Word16   char_hist[],        /* (i/o): tty character history         */
    Word16   tty_rate_hist[]     /* (i/o): tty baud rate history         */
)
{
    Word16   i;
    Word16   num;
    Word16   bit;
    Word16   *p_buf;
    Word16   write_flag;



    bit = 0;    /* eliminates compiler warning */

                                                            
    /* Change onset to silence */
    if( sub(counter_hist[CURRENT_FRAME],TTY_ONSET) == 0 )
    {
        counter_hist[CURRENT_FRAME] = TTY_SILENCE;          
        char_hist[CURRENT_FRAME] = TTY_SILENCE_CHAR;        
    }

                                                            
    /* If transition from silence to anything else */
    if( (current_counter & (TTY_SILENCE|NON_TTY)) != 0
        && sub(counter_hist[CURRENT_FRAME],TTY_SILENCE) != 0
      )
    {
        init_tty_gen( counter_hist[CURRENT_FRAME],
                      char_hist[CURRENT_FRAME],
                      tty_rate_hist[CURRENT_FRAME]);
    }

    /* else if transition from NON_TTY to TTY_SILENCE */

    else if( sub(current_counter,NON_TTY) == 0
             && sub(counter_hist[CURRENT_FRAME],TTY_SILENCE) == 0
           )
    {
                                                            
        init_tty_gen( TTY_SILENCE, TTY_SILENCE_CHAR, 0);
    }

    /* else if a new character is coming before the old one is finished */
    else if( subframe == 0
             && sub(current_counter,counter_hist[CURRENT_FRAME]) != 0
             && (counter_hist[CURRENT_FRAME] & COUNTER_BETWEEN_START_STOP) != 0
           )
    {

        if( sub(bit_num,MARK_HOLD_BIT_NUM) == 0 )
        {
            /* Cut short the mark hold tone to start new character */
            init_tty_gen( counter_hist[CURRENT_FRAME],
                          char_hist[CURRENT_FRAME],
                          tty_rate_hist[CURRENT_FRAME]);
        }
        else if( sub(bit_num,STOP_BIT_NUM) == 0
                 && sub(bit_size,FRAMESIZE) >= 0
               )
        {
            /* Shorten the stop bit to 1 bit length if it will be done this frame */

            data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD-4; 
            data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD-4; 
            bit_size = add(bit_size,data_bit_len[tty_rate_hist[CURRENT_FRAME]]);
            bit_size = sub(bit_size,stop_bit_len[tty_rate_hist[CURRENT_FRAME]]);
        }
        else if( sub(bit_num,STOP_BIT_NUM) < 0 )
        {
            /*
            *  Shorten to 1 stop bit if next character comes before
            *  generating stop bit of current character.
            */

            data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD-4; 
            data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD-4; 
            stop_bit_len[tty_rate_hist[CURRENT_FRAME]] = data_bit_len[tty_rate_hist[CURRENT_FRAME]];
        }

    } /* end if( falling behind) */

    p_buf = outbuf;                                         
    num = length;                                           
                                                            
    if( sub(bit_size,length) < 0 )
    {
        num = bit_size;                                     
    }

    write_flag = 0;                                         

    while( num > 0 )
    {

        /*-----------------------------------------------------------------------*/
        /*-----------------------------------------------------------------------*/
                                                            
        /* If current character non_tty, get out of loop */
        if( sub(current_counter,NON_TTY) == 0 )
        {
              init_tty_gen(NON_TTY,0,0);
              break;
        }

        /* else if silence, generate silence */
        else if( sub(current_counter,TTY_SILENCE) == 0 )
        {
                                                            
            for( i=0 ; i < num ; i++ )
            {
                p_buf[i] = 0;                               
            }
            init_tty_gen(TTY_SILENCE,TTY_SILENCE_CHAR,0);         /* init in preparation for next char */
            write_flag = 1;                                 
        }
        else
        {
                                                            
            bit = current_char & shl(1,bit_num);            
                                                            
            if( bit == 0 )
            {
                bit = SPACE_FREQ;                           
            }
            else
            {
                bit = MARK_FREQ;                            
            }
                                                            
            if( sub(bit,prev_bit) != 0 )
            {
                init_tone_gen(tone_param,bit);
            }

            tone_gen(p_buf,bit,BAUDOT_GAIN,num,tone_param);
            write_flag = 1;                                 

            /* Update for next iteration */

            p_buf += num; 
            bit_size = sub(bit_size,num);                                
                                                            
            if( sub(bit_size,1) < 0 )                 /* if end of bit */
            {
                bit_num = add(bit_num,1);                                  

                /*
                *  If a new character immediately follows the
                *  current character, eliminate the mark hold tone.
                */
                                                            
                if( sub(bit_num,MARK_HOLD_BIT_NUM) == 0 )
                {
                    /* Reset stop bit to 1.5 bits */

                    stop_bit_len[TTY_45_BAUD] = STOP_BIT_LEN_45_BAUD;
                    stop_bit_len[TTY_50_BAUD] = STOP_BIT_LEN_50_BAUD;

                    if( sub(current_counter,counter_hist[CURRENT_FRAME]) != 0
                        && (counter_hist[CURRENT_FRAME] & COUNTER_BETWEEN_START_STOP) != 0
                    )
                    {
                        bit_num = add(bit_num,1);                              
                    }
                }

                                                            
                if( sub(bit_num,MARK_HOLD_BIT_NUM) > 0 )               /* if end of character */
                {
                                                            
                    /* if start of silence */
                    if( sub(current_counter,counter_hist[CURRENT_FRAME]) == 0
                        || sub(counter_hist[CURRENT_FRAME],NON_TTY) == 0 )
                    {
                        counter_hist[CURRENT_FRAME] = TTY_SILENCE;      
                        char_hist[CURRENT_FRAME] = TTY_SILENCE_CHAR;    

                        init_tty_gen(TTY_SILENCE,TTY_SILENCE_CHAR,0);
                    }

                    /* else... next character begins right away. */
                    else                        /* else start next character */
                    {
                        init_tty_gen( counter_hist[CURRENT_FRAME],
                                      char_hist[CURRENT_FRAME],
                                      tty_rate_hist[CURRENT_FRAME]);
                    }
                }
                else                            /* start of next bit */
                {
                                                            
                    if( sub(bit_num,STOP_BIT_NUM) == 0 )
                    {
                        bit_size = stop_bit_len[tty_rate_hist[CURRENT_FRAME+1]];            
                    }
                    else if( sub(bit_num,MARK_HOLD_BIT_NUM) == 0 )
                    {
                                                            
                        bit_size = MARK_HOLD_LEN;           
                    }
                    else
                    {
                                                            
                        bit_size = data_bit_len[tty_rate_hist[CURRENT_FRAME+1]]; 
                    }
                    DEBUG(2,printf("bit_num = %d  bit_size = %d\n",bit_num,bit_size);)
                }
            }
        } /* end if */


        /* Update for next iteration */
        prev_bit = bit;                                     
        length = sub(length,num); 
        num = length;                                       
                                                            
        if( sub(bit_size,length) < 0 )
        {
            num = bit_size;                                 
        }

    } /* end while */


    /* Update history buffers for next iteration at the end of the frame */
                                                            
    if( sub(subframe,sub(num_subfr,1)) == 0 )
    {

        /* Put last generated character in history buffers */
                                                            
        if( sub(counter_hist[CURRENT_FRAME],current_counter) != 0 )
        {
            tty_rate_hist[CURRENT_FRAME] = tty_rate_hist[CURRENT_FRAME+1];  
        }
        counter_hist[CURRENT_FRAME] = current_counter;      
        char_hist[CURRENT_FRAME] = shr(current_char,1) & DATA_BIT_MASK;     

        for( i=TTY_BUF_SIZE-1 ; i > 0 ; i-- )
        {
            counter_hist[i] = counter_hist[i-1];            
            char_hist[i] = char_hist[i-1];                  
            tty_rate_hist[i] = tty_rate_hist[i-1];          
        }

        /* During the mark hold, allow the next character to be generated */
        if( sub(bit_num,MARK_HOLD_BIT_NUM) == 0 )
        {
            stop_bit_len[TTY_45_BAUD] = STOP_BIT_LEN_45_BAUD;
            data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD;
            stop_bit_len[TTY_50_BAUD] = STOP_BIT_LEN_50_BAUD;
            data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD;

            for( i=CURRENT_FRAME ; i >= 0 ; i-- )
            {
                                                            
                /* Remove current_counter from history */
                if( current_counter == counter_hist[i] )
                {                                                            
                    counter_hist[i] = TTY_FER;      
                    char_hist[i] = 0;    
                }
                else
                {
                    break;
                }                                
            }
        }

    }



    return(write_flag);
        
} /* end tty_gen() */

