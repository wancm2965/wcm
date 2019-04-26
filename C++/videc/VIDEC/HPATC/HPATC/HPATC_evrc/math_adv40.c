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
 
/*============================================================================*/

/*===================================================================*/
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
/*===================================================================*/
/* LIBRARY: math_adv40.c                                             */
/*-------------------------------------------------------------------*/
/* PURPOSE: Advanced mathematical functions 40bits.                  */
/*===================================================================*/

#include "typedef_fx.h"
#include "basic_op.h"
#include "basic_op40.h"
#include "math_ext32.h"
#include "math_adv.h"
#include "math_adv40.h"

#ifdef WMOPS_FX
#include "typedef_fx.h"
#include "main_fx.h"
#include "const_fx.h"
#include "lib_wmp_fx.h"

#endif

/*----------------------------------------------------------------------------*/
/*-------------------------------- FUNCTIONS ---------------------------------*/
/*----------------------------------------------------------------------------*/


/***************************************************************************
 *
 *   FUNCTION NAME: fn1_sqroot
 *
 *   PURPOSE:
 *     The purpose of this function is to implement an approximation
 *     of 1/sqroot(x) function
 *
 *
 *   INPUTS:
 *
 *     Input
 *                     normalized input (input range constrained to be < 1.0)
 *                     (Q15)
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     Output
 *                     = 1/sqroot( L_Input) (Q14)
 *
 *   DESCRIPTION:
 *
 *     The following recursive algorithm is used for the approximation
 *    
 *     x = 1.0;
 *     for (i = 0; i < M; i++)
 *	  x = x*(1.5 - v*x*x/2);
 *
 *     where v i the normalized input value
 *
 *************************************************************************/

Word16  fn1_sqroot (Word16 Input)
{

/*_________________________________________________________________________
 |                                                                         |
 |                           Local Static Variables                        |
 |_________________________________________________________________________|
*/

/*_________________________________________________________________________
 |                                                                         |
 |                            Automatic Variables                          |
 |_________________________________________________________________________|
*/
        Word16 i;
	Word16 Output;
	Word16 reg16_1, reg16_2;
	Word32 reg32;
	Word40 acc40;
/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	reg16_1    = 0x4000;                     //(1.0 in Q14)
	for (i = 0; i < 5; i++)
	  {
#ifdef WMOPS_FX
	    move16();
#endif
	    reg16_2 = reg16_1;
	    reg32   = L_mult(reg16_1, reg16_1);  // x^2 Q29 32bits
	    reg32   = L_shl (reg32, 1);          // x^2 Q30 32bits
	    reg16_1 = round32(reg32);             // x^2 Q14 16bits
	      
	    acc40   = L_mac40(0, reg16_1, Input); // v/2*x^2 Q31 40 bits 
	    acc40   = L_shr40(acc40, 1);          // v/2*x^2 Q30 40 bits 
	    reg32   = (Word32)acc40;              // no need saturation
	    reg16_1 = round32(reg32);              // y = v/2*x^2 Q14 16bits

	    reg16_1 = sub(0x6000, reg16_1);       // z = 1.5 -y Q14 16 bits

	    reg32   = L_mult(reg16_2, reg16_1);   // x*z Q29 32bits
	    reg32   = L_shl (reg32, 1);           // x*z Q30 32bits
	    reg16_1 = round32(reg32);              // x*z Q14 16bits
	  }
	Output = reg16_1;

	/* return result */
	/* ------------- */

	return (Output);
}


/***************************************************************************
 *
 *   FUNCTION NAME: sqrt_ratio40
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform a single precision square
 *     root function on a ratio of Word40
 *
 *   INPUTS:
 *
 *     L_num
 *                     input numerator 
 *     L_den
 *                     input denominator
 *     shift_result   
 *                     input additional shift for the result
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     reg16_2
 *                     output to square root function 16 bits
 *
 *   DESCRIPTION:
 *
 *      Input assumed to be NOT normalized
 *
 *
 *************************************************************************/

Word16 sqrt_ratio40 (Word40 L_num40, Word40 L_den40, Word16 shift)

{
  /*-------------------------------------------------------------------------*/

  Word16 exp_num, exp_den, exp_ratio;
  Word32 num, den;

  Word16 reg16, reg16_2;
  Word32 reg32;

  /*-------------------------------------------------------------------------*/
  /*                              Normalize the Energies                     */
  /*-------------------------------------------------------------------------*/
  
  exp_num = norm32_l40(L_num40);
  num = (Word32) L_shl40(L_num40, exp_num);
  
  exp_den = norm32_l40(L_den40);
  den = (Word32) L_shl40(L_den40, exp_den);
  
  /*-------------------------------------------------------------------------*/
  /*                          Calculate the ratio num / den                  */
  /*-------------------------------------------------------------------------*/
  
  if (L_sub(num, den) > 0L)
    {
      num = L_shr(num, 1);
      exp_num = sub(exp_num, 1);
    }
  
  reg32 = L_divide(num, den);     //Q31
  
  /*-------------------------------------------------------------------------*/
  /*              Normalize num / den to calculate sqroot(num / den)         */
  /*-------------------------------------------------------------------------*/
  
  exp_ratio = norm_l(reg32);
  reg32 = L_shl (reg32, exp_ratio);
  
#ifdef WMOPS_FX
  test();
#endif
  
  if (reg32 == 0)
    reg16_2 = 0;
  else
    reg16_2 = sqroot (reg32);
  
  /*-------------------------------------------------------------------------*/
  /*                    Calculate the sqroot for the exponent                */
  /*-------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------*/
  /*                     Test if the exponent is even or odd                 */
  /*-------------------------------------------------------------------------*/
  
#ifdef WMOPS_FX
  test();
  logic16();
#endif
  reg16 = add(exp_ratio, exp_num);
  reg16 = sub(reg16, exp_den);
  
  if ((reg16 & 0x0001) != 0)
    reg16_2 = mult_r(reg16_2, 0x5A82);     // 1/sqrt(2) in Q15
  
  /*-------------------------------------------------------------------------*/
  /*                      Calculate the sqroot for the exponent              */
  /*-------------------------------------------------------------------------*/
	  
  reg16 = shr(reg16, 1);
  
  /*-------------------------------------------------------------------------*/
  /*                          reg16 = sqroot( L_num/L_den)                   */
  /*-------------------------------------------------------------------------*/

  reg16   = add(reg16, shift);
  reg16_2 = shr(reg16_2, reg16);    // Q(15 - shift)

  /*-------------------------------------------------------------------------*/

  return (reg16_2);

  /*-------------------------------------------------------------------------*/
}

/*---------------------------------------------------------------------------*/

/*============================================================================*/
/*----------------------------------- END ------------------------------------*/
/*============================================================================*/
