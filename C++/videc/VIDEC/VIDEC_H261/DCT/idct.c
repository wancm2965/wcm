/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - inverse fast disrete cosine transformation - integer C version
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
 * $Id: idct.c,v 1.1.1.1.2.2 2005/06/03 05:48:10 lixq Exp $
 *
 *************************************************************************/

/**********************************************************/
/* inverse two dimensional DCT, Chen-Wang algorithm       */
/* (cf. IEEE ASSP-32, pp. 803-816, Aug. 1984)             */
/* 32-bit integer arithmetic (8 bit coefficients)         */
/* 11 mults, 29 adds per DCT                              */
/*                                      sE, 18.8.91       */
/**********************************************************/
/* coefficients extended to 12 bit for IEEE1180-1990      */
/* compliance                           sE,  2.1.94       */
/**********************************************************/

/* this code assumes >> to be a two's-complement arithmetic */
/* right shift: (-2)>>1 == -1 , (-3)>>1 == -2               */

#include "idct.h"

#define W1 2841					/* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676					/* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408					/* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609					/* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108					/* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565					/* 2048*sqrt(2)*cos(7*pi/16) */


/* global declarations */
/*void init_idct_int32 (void); */
/*void idct_int32 (short *block); */

/* private data */
static short iclip[1024];		/* clipping table */
static short *iclp;

/* private prototypes */
/*static void idctrow _ANSI_ARGS_((short *blk)); */
/*static void idctcol _ANSI_ARGS_((short *blk)); */

/* row (horizontal) IDCT
 *
 *           7                       pi         1
 * dst[k] = sum c[l] * src[l] * cos( -- * ( k + - ) * l )
 *          l=0                      8          2
 *
 * where: c[0]    = 128
 *        c[1..7] = 128*sqrt(2)
 */

#if 0
static void idctrow(blk)
short *blk;
{
  int X0, X1, X2, X3, X4, X5, X6, X7, X8;

  /* shortcut  */
  if (!((X1 = blk[4]<<11) | (X2 = blk[6]) | (X3 = blk[2]) |
        (X4 = blk[1]) | (X5 = blk[7]) | (X6 = blk[5]) | (X7 = blk[3])))
  {
    blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]<<3;
    return;
  }

  X0 = (blk[0]<<11) + 128; /* for proper rounding in the fourth stage  */

  /* first stage  */
  X8 = W7*(X4+X5);
  X4 = X8 + (W1-W7)*X4;
  X5 = X8 - (W1+W7)*X5;
  X8 = W3*(X6+X7);
  X6 = X8 - (W3-W5)*X6;
  X7 = X8 - (W3+W5)*X7;
  
  /* second stage  */
  X8 = X0 + X1;
  X0 -= X1;
  X1 = W6*(X3+X2);
  X2 = X1 - (W2+W6)*X2;
  X3 = X1 + (W2-W6)*X3;
  X1 = X4 + X6;
  X4 -= X6;
  X6 = X5 + X7;
  X5 -= X7;
  
  /* third stage  */
  X7 = X8 + X3;
  X8 -= X3;
  X3 = X0 + X2;
  X0 -= X2;
  X2 = (181*(X4+X5)+128)>>8;
  X4 = (181*(X4-X5)+128)>>8;
  
  /* fourth stage  */
  blk[0] = (X7+X1)>>8;
  blk[1] = (X3+X2)>>8;
  blk[2] = (X0+X4)>>8;
  blk[3] = (X8+X6)>>8;
  blk[4] = (X8-X6)>>8;
  blk[5] = (X0-X4)>>8;
  blk[6] = (X3-X2)>>8;
  blk[7] = (X7-X1)>>8;
}
#endif

/* column (vertical) IDCT
 *
 *             7                         pi         1
 * dst[8*k] = sum c[l] * src[8*l] * cos( -- * ( k + - ) * l )
 *            l=0                        8          2
 *
 * where: c[0]    = 1/1024
 *        c[1..7] = (1/1024)*sqrt(2)
 */
#if	0
static void idctcol(blk)
short *blk;
{
  int X0, X1, X2, X3, X4, X5, X6, X7, X8;

  /* shortcut  */
  if (!((X1 = (blk[8*4]<<8)) | (X2 = blk[8*6]) | (X3 = blk[8*2]) |
        (X4 = blk[8*1]) | (X5 = blk[8*7]) | (X6 = blk[8*5]) | (X7 = blk[8*3])))
  {
    blk[8*0]=blk[8*1]=blk[8*2]=blk[8*3]=blk[8*4]=blk[8*5]=blk[8*6]=blk[8*7]=
      iclp[(blk[8*0]+32)>>6];
    return;
  }

  X0 = (blk[8*0]<<8) + 8192;

  /* first stage  */
  X8 = W7*(X4+X5) + 4;
  X4 = (X8+(W1-W7)*X4)>>3;
  X5 = (X8-(W1+W7)*X5)>>3;
  X8 = W3*(X6+X7) + 4;
  X6 = (X8-(W3-W5)*X6)>>3;
  X7 = (X8-(W3+W5)*X7)>>3;
  
  /* second stage */
  X8 = X0 + X1;
  X0 -= X1;
  X1 = W6*(X3+X2) + 4;
  X2 = (X1-(W2+W6)*X2)>>3;
  X3 = (X1+(W2-W6)*X3)>>3;
  X1 = X4 + X6;
  X4 -= X6;
  X6 = X5 + X7;
  X5 -= X7;
  
  /* third stage  */
  X7 = X8 + X3;
  X8 -= X3;
  X3 = X0 + X2;
  X0 -= X2;
  X2 = (181*(X4+X5)+128)>>8;
  X4 = (181*(X4-X5)+128)>>8;
  
  /* fourth stage */
  blk[8*0] = iclp[(X7+X1)>>14];
  blk[8*1] = iclp[(X3+X2)>>14];
  blk[8*2] = iclp[(X0+X4)>>14];
  blk[8*3] = iclp[(X8+X6)>>14];
  blk[8*4] = iclp[(X8-X6)>>14];
  blk[8*5] = iclp[(X0-X4)>>14];
  blk[8*6] = iclp[(X3-X2)>>14];
  blk[8*7] = iclp[(X7-X1)>>14];
}
#endif

/* function pointer */
idctFuncPtr xvid_idct;

/* two dimensional inverse discrete cosine transform */
/*void j_rev_dct(block) */
/*short *block; */
void
idct_h261_int32(short *const block)
{

	/* idct_int32_init() must be called before the first call to this function! */


	/*int i;
	   long i;

	   for (i=0; i<8; i++)
	   idctrow(block+8*i);

	   for (i=0; i<8; i++)
	   idctcol(block+i); */
	static short *blk;
	static long i;
	static long X0, X1, X2, X3, X4, X5, X6, X7, X8;


	for (i = 0; i < 8; i++)		/* idct rows */
	{
		blk = block + (i << 3);
		if (!
			((X1 = blk[4] << 11) | (X2 = blk[6]) | (X3 = blk[2]) | (X4 =
																	blk[1]) |
			 (X5 = blk[7]) | (X6 = blk[5]) | (X7 = blk[3]))) {
			blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] =
				blk[7] = blk[0] << 3;
			continue;
		}

		X0 = (blk[0] << 11) + 128;	/* for proper rounding in the fourth stage  */

		/* first stage  */
		X8 = W7 * (X4 + X5);
		X4 = X8 + (W1 - W7) * X4;
		X5 = X8 - (W1 + W7) * X5;
		X8 = W3 * (X6 + X7);
		X6 = X8 - (W3 - W5) * X6;
		X7 = X8 - (W3 + W5) * X7;

		/* second stage  */
		X8 = X0 + X1;
		X0 -= X1;
		X1 = W6 * (X3 + X2);
		X2 = X1 - (W2 + W6) * X2;
		X3 = X1 + (W2 - W6) * X3;
		X1 = X4 + X6;
		X4 -= X6;
		X6 = X5 + X7;
		X5 -= X7;

		/* third stage  */
		X7 = X8 + X3;
		X8 -= X3;
		X3 = X0 + X2;
		X0 -= X2;
		X2 = (181 * (X4 + X5) + 128) >> 8;
		X4 = (181 * (X4 - X5) + 128) >> 8;

		/* fourth stage  */

		blk[0] = (short) ((X7 + X1) >> 8);
		blk[1] = (short) ((X3 + X2) >> 8);
		blk[2] = (short) ((X0 + X4) >> 8);
		blk[3] = (short) ((X8 + X6) >> 8);
		blk[4] = (short) ((X8 - X6) >> 8);
		blk[5] = (short) ((X0 - X4) >> 8);
		blk[6] = (short) ((X3 - X2) >> 8);
		blk[7] = (short) ((X7 - X1) >> 8);

	}							/* end for ( i = 0; i < 8; ++i ) IDCT-rows */



	for (i = 0; i < 8; i++)		/* idct columns */
	{
		blk = block + i;
		/* shortcut  */
		if (!
			((X1 = (blk[8 * 4] << 8)) | (X2 = blk[8 * 6]) | (X3 =
															 blk[8 *
																 2]) | (X4 =
																		blk[8 *
																			1])
			 | (X5 = blk[8 * 7]) | (X6 = blk[8 * 5]) | (X7 = blk[8 * 3]))) {
			blk[8 * 0] = blk[8 * 1] = blk[8 * 2] = blk[8 * 3] = blk[8 * 4] =
				blk[8 * 5] = blk[8 * 6] = blk[8 * 7] =
				iclp[(blk[8 * 0] + 32) >> 6];
			continue;
		}

		X0 = (blk[8 * 0] << 8) + 8192;

		/* first stage  */
		X8 = W7 * (X4 + X5) + 4;
		X4 = (X8 + (W1 - W7) * X4) >> 3;
		X5 = (X8 - (W1 + W7) * X5) >> 3;
		X8 = W3 * (X6 + X7) + 4;
		X6 = (X8 - (W3 - W5) * X6) >> 3;
		X7 = (X8 - (W3 + W5) * X7) >> 3;

		/* second stage  */
		X8 = X0 + X1;
		X0 -= X1;
		X1 = W6 * (X3 + X2) + 4;
		X2 = (X1 - (W2 + W6) * X2) >> 3;
		X3 = (X1 + (W2 - W6) * X3) >> 3;
		X1 = X4 + X6;
		X4 -= X6;
		X6 = X5 + X7;
		X5 -= X7;

		/* third stage  */
		X7 = X8 + X3;
		X8 -= X3;
		X3 = X0 + X2;
		X0 -= X2;
		X2 = (181 * (X4 + X5) + 128) >> 8;
		X4 = (181 * (X4 - X5) + 128) >> 8;

		/* fourth stage  */
		blk[8 * 0] = iclp[(X7 + X1) >> 14];
		blk[8 * 1] = iclp[(X3 + X2) >> 14];
		blk[8 * 2] = iclp[(X0 + X4) >> 14];
		blk[8 * 3] = iclp[(X8 + X6) >> 14];
		blk[8 * 4] = iclp[(X8 - X6) >> 14];
		blk[8 * 5] = iclp[(X0 - X4) >> 14];
		blk[8 * 6] = iclp[(X3 - X2) >> 14];
		blk[8 * 7] = iclp[(X7 - X1) >> 14];
	}

}								/* end function idct_int32(block) */


/*void */
/*idct_int32_init() */
void
idct_h261_int32_init(void)
{
	int i;

	iclp = iclip + 512;
	for (i = -512; i < 512; i++)
		iclp[i] = (i < -256) ? -256 : ((i > 255) ? 255 : i);
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




// Process: blk_in -> d2 -> blk_out, d2 is global data
void IDCT_Block(short *blk_in)
{	
	int    i,j,j1;
	int    e,f,g,h;
	int    b[8],b1[8];
	short    *bp,*dp;	// d2[64]
	short d2[64];
	// Horizontal Transform: blk_in -> d2
	dp = d2;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
			//b[j] = *(block + zig_zag_scan[8*i+j]); 		
			b[j] = blk_in[8*i+j]; 
									
		// b[0] -- F0 
		if(b[1]==0 && b[2]==0 && b[3]==0 && b[4]==0 &&
			b[5]==0 && b[6]==0 && b[7]==0)
		{
			dp[0] = dp[1] = dp[2] =
			dp[3] = dp[4] = dp[5] =
			dp[6] = dp[7] = ((b[0]==0) ? 0 : (b[0]*f4+8192)>>14);
			dp += 8;
			continue;
		}

		e = b[1] * f7 - b[7] * f1 + 8192;
		h = b[7] * f7 + b[1] * f1 + 8192;
		f = b[5] * f3 - b[3] * f5;
		g = b[3] * f3 + b[5] * f5;

		b1[0] = (b[0] + b[4]) * f4 + 8192;
		b1[1] = (b[0] - b[4]) * f4 + 8192;
		b1[2] = b[2] * f6 - b[6] * f2;
		b1[3] = b[6] * f6 + b[2] * f2;

		b[4]  = (e + f) >> 14; // + 8192;
		b1[5] = (e - f) >> 14; // + 8192;
		b1[6] = (h - g) >> 14; // + 8192;
		b[7]  = (h + g) >> 14; // + 8192;

		b[5] = ((b1[6] - b1[5]) * f0 + 8192) >> 14; // + 8192;
		b[6] = ((b1[6] + b1[5]) * f0 + 8192) >> 14; // + 8192;
		b[0] = (b1[0] + b1[3]) >> 14; // + 8192;
		b[1] = (b1[1] + b1[2]) >> 14; // + 8192;
		b[2] = (b1[1] - b1[2]) >> 14; // + 8192;
		b[3] = (b1[0] - b1[3]) >> 14; // + 8192;

		for (j = 0; j < 4; j++)
		{
			j1 = 7 - j;
			dp[j]  = ((short)b[j] + (short)b[j1]); // >> 14; 
			dp[j1] = ((short)b[j] - (short)b[j1]); // >> 14;
		}

		dp += 8;
	}

	// Vertical Transform: d2 -> blk_out

	dp = d2; bp = blk_in;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
			b[j] = dp[8*j];

		if(b[1]==0 && b[2]==0 && b[3]==0 && b[4]==0 &&
			b[5]==0 && b[6]==0 && b[7]==0)
		{
			bp[8*0] = bp[8*1] = bp[8*2] =
			bp[8*3] = bp[8*4] = bp[8*5] =
			bp[8*6] = bp[8*7] = ((b[0]==0) ? 0 : (b[0]*pf4+16384)>>15);
			dp++ ; bp++;
			continue;
		}

		e = b[1] * pf7 - b[7] * pf1 + 16384;
		h = b[7] * pf7 + b[1] * pf1 + 16384;
		f = b[5] * pf3 - b[3] * pf5;
		g = b[3] * pf3 + b[5] * pf5;

		b1[0] = (b[0] + b[4]) * pf4 + 16384;
		b1[1] = (b[0] - b[4]) * pf4 + 16384;
		b1[2] = b[2] * pf6 - b[6] * pf2;
		b1[3] = b[6] * pf6 + b[2] * pf2;

		b[4]  = (e + f) >> 15; // + 16384;
		b1[5] = (e - f) >> 15; // + 16384;
		b1[6] = (h - g) >> 15; // + 16384;
		b[7]  = (h + g) >> 15; // + 16384;

		b[5]  = ((b1[6] - b1[5]) * pf0 + 16384) >> 15; // + 16384;
		b[6]  = ((b1[6] + b1[5]) * pf0 + 16384) >> 15; // + 16384;
		b[0]  = (b1[0] + b1[3]) >> 15; // + 16384;
		b[1]  = (b1[1] + b1[2]) >> 15; // + 16384;
		b[2]  = (b1[1] - b1[2]) >> 15; // + 16384;
		b[3]  = (b1[0] - b1[3]) >> 15; // + 16384;

		for (j = 0; j < 4; j++)
		{
			j1 = 7 - j;
			bp[8*j]  = ((short)b[j] + (short)b[j1]); // >> 15; 
			bp[8*j1] = ((short)b[j] - (short)b[j1]); // >> 15; 
		}
		dp++; bp++;
	}

}	// End of function IDCT_Block()
