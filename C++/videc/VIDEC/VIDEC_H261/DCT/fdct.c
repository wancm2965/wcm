/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - fast disrete cosine transformation - integer C version
 *
 *  These routines are from Independent JPEG Group's free JPEG software
 *  Copyright (C) 1991-1998, Thomas G. Lane (see the file README.IJG)
 *
 *  This file is part of XviD, a free MPEG-4 video encoder/decoder
 *
 *  XviD is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  Under section 8 of the GNU General Public License, the copyright
 *  holders of XVID explicitly forbid distribution in the following
 *  countries:
 *
 *    - Japan
 *    - United States of America
 *
 *  Linking XviD statically or dynamically with other modules is making a
 *  combined work based on XviD.  Thus, the terms and conditions of the
 *  GNU General Public License cover the whole combination.
 *
 *  As a special exception, the copyright holders of XviD give you
 *  permission to link XviD with independent modules that communicate with
 *  XviD solely through the VFW1.1 and DShow interfaces, regardless of the
 *  license terms of these independent modules, and to copy and distribute
 *  the resulting combined work under terms of your choice, provided that
 *  every copy of the combined work is accompanied by a complete copy of
 *  the source code of XviD (the version of XviD used to produce the
 *  combined work), being distributed under the terms of the GNU General
 *  Public License plus this exception.  An independent module is a module
 *  which is not derived from or based on XviD.
 *
 *  Note that people who make modified versions of XviD are not obligated
 *  to grant this special exception for their modified versions; it is
 *  their choice whether to do so.  The GNU General Public License gives
 *  permission to release a modified version without this exception; this
 *  exception also makes it possible to release a modified version which
 *  carries forward this exception.
 *
 * $Id: fdct.c,v 1.1.1.1.2.2 2005/06/03 05:48:10 lixq Exp $
 *
 *************************************************************************/

/* This routine is a slow-but-accurate integer implementation of the
 * forward DCT (Discrete Cosine Transform). Taken from the IJG software
 *
 * A 2-D DCT can be done by 1-D DCT on each row followed by 1-D DCT
 * on each column.  Direct algorithms are also available, but they are
 * much more complex and seem not to be any faster when reduced to code.
 *
 * This implementation is based on an algorithm described in
 *   C. Loeffler, A. Ligtenberg and G. Moschytz, "Practical Fast 1-D DCT
 *   Algorithms with 11 Multiplications", Proc. Int'l. Conf. on Acoustics,
 *   Speech, and Signal Processing 1989 (ICASSP '89), pp. 988-991.
 * The primary algorithm described there uses 11 multiplies and 29 adds.
 * We use their alternate method with 12 multiplies and 32 adds.
 * The advantage of this method is that no data path contains more than one
 * multiplication; this allows a very simple and accurate implementation in
 * scaled fixed-point arithmetic, with a minimal number of shifts.
 *
 * The poop on this scaling stuff is as follows:
 *
 * Each 1-D DCT step produces outputs which are a factor of sqrt(N)
 * larger than the true DCT outputs.  The final outputs are therefore
 * a factor of N larger than desired; since N=8 this can be cured by
 * a simple right shift at the end of the algorithm.  The advantage of
 * this arrangement is that we save two multiplications per 1-D DCT,
 * because the y0 and y4 outputs need not be divided by sqrt(N).
 * In the IJG code, this factor of 8 is removed by the quantization step
 * (in jcdctmgr.c), here it is removed.
 *
 * We have to do addition and subtraction of the integer inputs, which
 * is no problem, and multiplication by fractional constants, which is
 * a problem to do in integer arithmetic.  We multiply all the constants
 * by CONST_SCALE and convert them to integer constants (thus retaining
 * CONST_BITS bits of precision in the constants).  After doing a
 * multiplication we have to divide the product by CONST_SCALE, with proper
 * rounding, to produce the correct output.  This division can be done
 * cheaply as a right shift of CONST_BITS bits.  We postpone shifting
 * as long as possible so that partial sums can be added together with
 * full fractional precision.
 *
 * The outputs of the first pass are scaled up by PASS1_BITS bits so that
 * they are represented to better-than-integral precision.  These outputs
 * require 8 + PASS1_BITS + 3 bits; this fits in a 16-bit word
 * with the recommended scaling.  (For 12-bit sample data, the intermediate
 * array is INT32 anyway.)
 *
 * To avoid overflow of the 32-bit intermediate results in pass 2, we must
 * have 8 + CONST_BITS + PASS1_BITS <= 26.  Error analysis
 * shows that the values given below are the most effective.
 *
 * We can gain a little more speed, with a further compromise in accuracy,
 * by omitting the addition in a descaling shift.  This yields an incorrectly
 * rounded result half the time...
 */

#include "fdct.h"

#define USE_ACCURATE_ROUNDING

#define RIGHT_SHIFT(x, shft)  ((x) >> (shft))

#ifdef USE_ACCURATE_ROUNDING
#define ONE ((int) 1)
#define DESCALE(x, n)  RIGHT_SHIFT((x) + (ONE << ((n) - 1)), n)
#else
#define DESCALE(x, n)  RIGHT_SHIFT(x, n)
#endif

#define CONST_BITS  13
#define PASS1_BITS  2

#define FIX_0_298631336  ((int)  2446)	/* FIX(0.298631336) */
#define FIX_0_390180644  ((int)  3196)	/* FIX(0.390180644) */
#define FIX_0_541196100  ((int)  4433)	/* FIX(0.541196100) */
#define FIX_0_765366865  ((int)  6270)	/* FIX(0.765366865) */
#define FIX_0_899976223  ((int)  7373)	/* FIX(0.899976223) */
#define FIX_1_175875602  ((int)  9633)	/* FIX(1.175875602) */
#define FIX_1_501321110  ((int) 12299)	/* FIX(1.501321110) */
#define FIX_1_847759065  ((int) 15137)	/* FIX(1.847759065) */
#define FIX_1_961570560  ((int) 16069)	/* FIX(1.961570560) */
#define FIX_2_053119869  ((int) 16819)	/* FIX(2.053119869) */
#define FIX_2_562915447  ((int) 20995)	/* FIX(2.562915447) */
#define FIX_3_072711026  ((int) 25172)	/* FIX(3.072711026) */

/* function pointer */
fdctFuncPtr xvid_fdct;

/*
 * Perform an integer forward DCT on one block of samples.
 */

void
fdct_h261_int32(short *const block)
{
	int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	int tmp10, tmp11, tmp12, tmp13;
	int z1, z2, z3, z4, z5;
	short *blkptr;
	int *dataptr;
	int data[64];
	int i;

	/* Pass 1: process rows. */
	/* Note results are scaled up by sqrt(8) compared to a true DCT; */
	/* furthermore, we scale the results by 2**PASS1_BITS. */

	dataptr = data;
	blkptr = block;
	for (i = 0; i < 8; i++) {
		tmp0 = blkptr[0] + blkptr[7];
		tmp7 = blkptr[0] - blkptr[7];
		tmp1 = blkptr[1] + blkptr[6];
		tmp6 = blkptr[1] - blkptr[6];
		tmp2 = blkptr[2] + blkptr[5];
		tmp5 = blkptr[2] - blkptr[5];
		tmp3 = blkptr[3] + blkptr[4];
		tmp4 = blkptr[3] - blkptr[4];

		/* Even part per LL&M figure 1 --- note that published figure is faulty;
		 * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
		 */

		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		dataptr[0] = (tmp10 + tmp11) << PASS1_BITS;
		dataptr[4] = (tmp10 - tmp11) << PASS1_BITS;

		z1 = (tmp12 + tmp13) * FIX_0_541196100;
		dataptr[2] =
			DESCALE(z1 + tmp13 * FIX_0_765366865, CONST_BITS - PASS1_BITS);
		dataptr[6] =
			DESCALE(z1 + tmp12 * (-FIX_1_847759065), CONST_BITS - PASS1_BITS);

		/* Odd part per figure 8 --- note paper omits factor of sqrt(2).
		 * cK represents cos(K*pi/16).
		 * i0..i3 in the paper are tmp4..tmp7 here.
		 */

		z1 = tmp4 + tmp7;
		z2 = tmp5 + tmp6;
		z3 = tmp4 + tmp6;
		z4 = tmp5 + tmp7;
		z5 = (z3 + z4) * FIX_1_175875602;	/* sqrt(2) * c3 */

		tmp4 *= FIX_0_298631336;	/* sqrt(2) * (-c1+c3+c5-c7) */
		tmp5 *= FIX_2_053119869;	/* sqrt(2) * ( c1+c3-c5+c7) */
		tmp6 *= FIX_3_072711026;	/* sqrt(2) * ( c1+c3+c5-c7) */
		tmp7 *= FIX_1_501321110;	/* sqrt(2) * ( c1+c3-c5-c7) */
		z1 *= -FIX_0_899976223;	/* sqrt(2) * (c7-c3) */
		z2 *= -FIX_2_562915447;	/* sqrt(2) * (-c1-c3) */
		z3 *= -FIX_1_961570560;	/* sqrt(2) * (-c3-c5) */
		z4 *= -FIX_0_390180644;	/* sqrt(2) * (c5-c3) */

		z3 += z5;
		z4 += z5;

		dataptr[7] = DESCALE(tmp4 + z1 + z3, CONST_BITS - PASS1_BITS);
		dataptr[5] = DESCALE(tmp5 + z2 + z4, CONST_BITS - PASS1_BITS);
		dataptr[3] = DESCALE(tmp6 + z2 + z3, CONST_BITS - PASS1_BITS);
		dataptr[1] = DESCALE(tmp7 + z1 + z4, CONST_BITS - PASS1_BITS);

		dataptr += 8;			/* advance pointer to next row */
		blkptr += 8;
	}

	/* Pass 2: process columns.
	 * We remove the PASS1_BITS scaling, but leave the results scaled up
	 * by an overall factor of 8.
	 */

	dataptr = data;
	for (i = 0; i < 8; i++) {
		tmp0 = dataptr[0] + dataptr[56];
		tmp7 = dataptr[0] - dataptr[56];
		tmp1 = dataptr[8] + dataptr[48];
		tmp6 = dataptr[8] - dataptr[48];
		tmp2 = dataptr[16] + dataptr[40];
		tmp5 = dataptr[16] - dataptr[40];
		tmp3 = dataptr[24] + dataptr[32];
		tmp4 = dataptr[24] - dataptr[32];

		/* Even part per LL&M figure 1 --- note that published figure is faulty;
		 * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
		 */

		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		dataptr[0] = DESCALE(tmp10 + tmp11, PASS1_BITS);
		dataptr[32] = DESCALE(tmp10 - tmp11, PASS1_BITS);

		z1 = (tmp12 + tmp13) * FIX_0_541196100;
		dataptr[16] =
			DESCALE(z1 + tmp13 * FIX_0_765366865, CONST_BITS + PASS1_BITS);
		dataptr[48] =
			DESCALE(z1 + tmp12 * (-FIX_1_847759065), CONST_BITS + PASS1_BITS);

		/* Odd part per figure 8 --- note paper omits factor of sqrt(2).
		 * cK represents cos(K*pi/16).
		 * i0..i3 in the paper are tmp4..tmp7 here.
		 */

		z1 = tmp4 + tmp7;
		z2 = tmp5 + tmp6;
		z3 = tmp4 + tmp6;
		z4 = tmp5 + tmp7;
		z5 = (z3 + z4) * FIX_1_175875602;	/* sqrt(2) * c3 */

		tmp4 *= FIX_0_298631336;	/* sqrt(2) * (-c1+c3+c5-c7) */
		tmp5 *= FIX_2_053119869;	/* sqrt(2) * ( c1+c3-c5+c7) */
		tmp6 *= FIX_3_072711026;	/* sqrt(2) * ( c1+c3+c5-c7) */
		tmp7 *= FIX_1_501321110;	/* sqrt(2) * ( c1+c3-c5-c7) */
		z1 *= -FIX_0_899976223;	/* sqrt(2) * (c7-c3) */
		z2 *= -FIX_2_562915447;	/* sqrt(2) * (-c1-c3) */
		z3 *= -FIX_1_961570560;	/* sqrt(2) * (-c3-c5) */
		z4 *= -FIX_0_390180644;	/* sqrt(2) * (c5-c3) */

		z3 += z5;
		z4 += z5;

		dataptr[56] = DESCALE(tmp4 + z1 + z3, CONST_BITS + PASS1_BITS);
		dataptr[40] = DESCALE(tmp5 + z2 + z4, CONST_BITS + PASS1_BITS);
		dataptr[24] = DESCALE(tmp6 + z2 + z3, CONST_BITS + PASS1_BITS);
		dataptr[8] = DESCALE(tmp7 + z1 + z4, CONST_BITS + PASS1_BITS);

		dataptr++;				/* advance pointer to next column */
	}
	/* descale */
	for (i = 0; i < 64; i++)
		block[i] = (short int) DESCALE(data[i], 3);
}


#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

#define f0 11585	// 11585.237503 = (2|14) 16384*cos(pi/4) 
#define f1  8035	// 8034.593017  = (2|14) 16384*cos(1*pi/16)/2 
#define f2  7568	// 7568.421130  = (2|14) 16384*cos(2*pi/16)/2 
#define f3  6811	// 6811.399064  = (2|14) 16384*cos(3*pi/16)/2 
#define f4  5793	// 5792.618751  = (2|14) 16384*cos(4*pi/16)/2 
#define f5  4551	// 4551.231349  = (2|14) 16384*cos(5*pi/16)/2 
#define f6  3135	// 3134.942678  = (2|14) 16384*cos(6*pi/16)/2 
#define f7  1598	// 1598.179918  = (2|14) 16384*cos(7*pi/16)/2 

#define pf0  23170	// 23170.475006  = (2|15) 32768*cos(pi/4) 
#define pf1  16069	// 16069.186034  = (2|15) 32768*cos(1*pi/16)/2 
#define pf2  15137	// 15136.842261  = (2|15) 32768*cos(2*pi/16)/2 
#define pf3  13623	// 13622.798128  = (2|15) 32768*cos(3*pi/16)/2 
#define pf4  11585	// 11585.237503  = (2|15) 32768*cos(4*pi/16)/2 
#define pf5  9102	// 9102.462698   = (2|15) 32768*cos(5*pi/16)/2 
#define pf6  6270	// 6269.885356   = (2|15) 32768*cos(6*pi/16)/2 
#define pf7  3196	// 3196.359836   = (2|15) 32768*cos(7*pi/16)/2 



void DCT_Block(short *block)
{
	int    i,j,j1;
	short    b[8],b1[8];
	short    *bp;
	
	// DCT 
	
	// Horizontal transform 
	
	bp = block;
	for(i = 0; i < 8; i++)
	{
		for (j = 0 ; j < 4 ; j++)
		{
			j1 = 7 - j;
			b1[j]  = bp[j] + bp[j1];
			b1[j1] = bp[j] - bp[j1];
		}
		b[0] = b1[0] + b1[3];
		b[1] = b1[1] + b1[2];
		b[2] = b1[1] - b1[2];
		b[3] = b1[0] - b1[3];
		b[4] = b1[4];
		b[5] = (11585 * (b1[6] - b1[5])) >> 14; // + 8192) >> 14; 
		b[6] = (11585 * (b1[6] + b1[5])) >> 14; // + 8192) >> 14; 
		b[7] = b1[7];
		
		bp[0] = ((b[0] + b[1]) * f4) >> 11; //9; // + 256) >> 9; 
		bp[4] = ((b[0] - b[1]) * f4) >> 11; //9; // + 256) >> 9; 
		
		bp[2] = (b[2]*f6 + b[3]*f2) >> 11; //9; // + 256) >> 9;
		bp[6] = (b[3]*f6 - b[2]*f2) >> 11; //9; // + 256) >> 9;
		
		b1[4] = b[4] + b[5];
		b1[7] = b[7] + b[6];
		b1[5] = b[4] - b[5];
		b1[6] = b[7] - b[6];
		
		bp[1] = (b1[4]*f7 + b1[7]*f1) >> 11; //9; // + 256) >> 9;
		bp[7] = (b1[7]*f7 - b1[4]*f1) >> 11; //9; // + 256) >> 9;
		
		bp[5] = (b1[5]*f3 + b1[6]*f5) >> 11; //9; // + 256) >> 9;
		bp[3] = (b1[6]*f3 - b1[5]*f5) >> 11; //9; // + 256) >> 9;
		
		bp += 8;
		
	}
	
	// Vertical transform
	
	bp = block;
	for (i = 0 ; i < 8 ; i++)
	{
		for (j = 0 ; j < 4 ; j++)
		{
			j1 = 7 - j;
			b1[j]  = bp[8*j] + bp[8*j1];
			b1[j1] = bp[8*j] - bp[8*j1];
		}
		
		b[0] = b1[0] + b1[3];
		b[1] = b1[1] + b1[2];
		b[2] = b1[1] - b1[2];
		b[3] = b1[0] - b1[3];
		b[4] = b1[4];
		b[5] = (23170 * (b1[6] - b1[5])) >> 15; // + 16384) >> 15; 
		b[6] = (23170 * (b1[6] + b1[5])) >> 15; // + 16384) >> 15; 
		b[7] = b1[7];
		
		bp[8*0] = ((b[0] + b[1]) * pf4) >> 18; //20; // + 524288) >> 20; // 14
		bp[8*4] = ((b[0] - b[1]) * pf4) >> 18; //20; // + 524288) >> 20; // 14
		bp[8*2] = (b[2]*pf6 + b[3]*pf2) >> 18; //20; // + 524288) >> 20;
		bp[8*6] = (b[3]*pf6 - b[2]*pf2) >> 18; //20; // + 524288) >> 20;
		
		b1[4] = b[4] + b[5];
		b1[7] = b[7] + b[6];
		b1[5] = b[4] - b[5];
		b1[6] = b[7] - b[6];
		
		bp[8*1] = (b1[4]*pf7 + b1[7]*pf1) >> 18; //20; // + 524288) >> 20;
		bp[8*7] = (b1[7]*pf7 - b1[4]*pf1) >> 18; //20; // + 524288) >> 20;
		
		bp[8*5] = (b1[5]*pf3 + b1[6]*pf5) >> 18; //20; // + 524288) >> 20;
		bp[8*3] = (b1[6]*pf3 - b1[5]*pf5) >> 18; //20; // + 524288) >> 20;
		
		bp++;
	}
	
}	// End of function DCT_Block()
