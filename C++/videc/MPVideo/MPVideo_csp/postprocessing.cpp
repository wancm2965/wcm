/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Postprocessing  functions -
 *
 *  Copyright(C) 2003-2004 Michael Militzer <isibaar@xvid.org>
 *                    2004 Marc Fauconneau
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: postprocessing.c,v 1.4 2004/04/18 07:55:11 syskin Exp $
 *
 ****************************************************************************/
 
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "postprocessing.h"


/* Some useful (and fast) macros
   Note that the MIN/MAX macros assume signed shift - if your compiler
   doesn't do signed shifts, use the default MIN/MAX macros from global.h */

#define FAST_MAX(x,y) ((x) - ((((x) - (y))>>(32 - 1)) & ((x) - (y))))
#define FAST_MIN(x,y) ((x) + ((((y) - (x))>>(32 - 1)) & ((y) - (x))))
#define FAST_ABS(x) ((((int)(x)) >> 31) ^ ((int)(x))) - (((int)(x)) >> 31)
#define ABS(X)    (((X)>0)?(X):-(X)) 

//const int h264_2_xvid_qp_tables[52]=
//{
//	0, 0, 0, 0, 0, 0, 0, 0,  /*  0-7 */		//qp=6 8M
//	0, 0, 0, 0,              /*  8-11 */
//	0, 0, 0, 0, 0, 0, 0, 0,  /* 12-19 */	//qp=15 2m
//	5, 5, 5, 5, 5, 5, 5, 5,  /* 20-27 */	//qp=21 768k
//	10, 10, 10, 10,15,15,15,15,  /* 28-35 */	//qp=26 384k
//	20,20,20,20,20,30,30,30,  /* 36-43 */
//	30,30,30,30,30,30,30,30   /* 44-51  */	//not used
//};
//
//void init_postproc(XVID_POSTPROC *tbls)
//{
//	init_deblock(tbls);
//}

void
image_postproc(XVID_POSTPROC *tbls, IMAGE * img, int edged_width,
				int qp, int mb_width, int mb_height, int mb_stride,
				int flags)
{
	const int edged_width2 = edged_width /2;
	int i,j;
	int quant;
	int dering = flags & XVID_DERINGY;

	qp=qp-20;//h264_2_xvid_qp_tables[qp];
	if (qp<0)
		qp=0;

	/* luma: j,i in block units */
	if ((flags & XVID_DEBLOCKY))
	{
		for (j = 1; j < mb_height*2; j++)		/* horizontal deblocking */
		for (i = 0; i < mb_width*2; i++)
		{
			quant = qp;
			deblock8x8_h(tbls, img->y + j*8*edged_width + i*8, edged_width, quant, dering);
		}

		for (j = 0; j < mb_height*2; j++)		/* vertical deblocking */
		for (i = 1; i < mb_width*2; i++)
		{
			quant = qp;
			deblock8x8_v(tbls, img->y + j*8*edged_width + i*8, edged_width, quant, dering);
		}
	}


	/* chroma */
	if ((flags & XVID_DEBLOCKUV))
	{
		dering = flags & XVID_DERINGUV;

		for (j = 1; j < mb_height; j++)		/* horizontal deblocking */
		for (i = 0; i < mb_width; i++)
		{
			quant = qp;
			deblock8x8_h(tbls, img->u + j*8*edged_width2 + i*8, edged_width2, quant, dering);
			deblock8x8_h(tbls, img->v + j*8*edged_width2 + i*8, edged_width2, quant, dering);
		}

		for (j = 0; j < mb_height; j++)		/* vertical deblocking */	
		for (i = 1; i < mb_width; i++)
		{
			quant = qp;
			deblock8x8_v(tbls, img->u + j*8*edged_width2 + i*8, edged_width2, quant, dering);
			deblock8x8_v(tbls, img->v + j*8*edged_width2 + i*8, edged_width2, quant, dering);
		}
	}
}

/******************************************************************************/

void init_deblock(XVID_POSTPROC *tbls)
{
	int i;

	for(i = -255; i < 256; i++) {
		tbls->xvid_thresh_tbl[i + 255] = 0;
		if(ABS(i) < THR1)
			tbls->xvid_thresh_tbl[i + 255] = 1;
		tbls->xvid_abs_tbl[i + 255] = ABS(i);
	}
}

#define LOAD_DATA_HOR(x) \
		/* Load pixel addresses and data for filtering */ \
	    s[0] = *(v[0] = img - 5*stride + x); \
		s[1] = *(v[1] = img - 4*stride + x); \
		s[2] = *(v[2] = img - 3*stride + x); \
		s[3] = *(v[3] = img - 2*stride + x); \
		s[4] = *(v[4] = img - 1*stride + x); \
		s[5] = *(v[5] = img + 0*stride + x); \
		s[6] = *(v[6] = img + 1*stride + x); \
		s[7] = *(v[7] = img + 2*stride + x); \
		s[8] = *(v[8] = img + 3*stride + x); \
		s[9] = *(v[9] = img + 4*stride + x);

#define LOAD_DATA_VER(x) \
		/* Load pixel addresses and data for filtering */ \
		s[0] = *(v[0] = img + x*stride - 5); \
		s[1] = *(v[1] = img + x*stride - 4); \
		s[2] = *(v[2] = img + x*stride - 3); \
		s[3] = *(v[3] = img + x*stride - 2); \
		s[4] = *(v[4] = img + x*stride - 1); \
		s[5] = *(v[5] = img + x*stride + 0); \
		s[6] = *(v[6] = img + x*stride + 1); \
		s[7] = *(v[7] = img + x*stride + 2); \
		s[8] = *(v[8] = img + x*stride + 3); \
		s[9] = *(v[9] = img + x*stride + 4);

#define APPLY_DERING(x) \
		*v[x] = (e[x] == 0) ? (			\
			(e[x-1] == 0) ? (			\
			(e[x+1] == 0) ? 			\
			((s[x-1]+s[x]*2+s[x+1])>>2)	\
			: ((s[x-1]+s[x])>>1) )		\
			: ((s[x]+s[x+1])>>1) )		\
			: s[x];	

#define APPLY_FILTER_CORE \
		/* First, decide whether to use default or DC-offset mode */ \
		\
		eq_cnt = 0; \
		\
		eq_cnt += tbls->xvid_thresh_tbl[s[0] - s[1] + 255]; \
		eq_cnt += tbls->xvid_thresh_tbl[s[1] - s[2] + 255]; \
		eq_cnt += tbls->xvid_thresh_tbl[s[2] - s[3] + 255]; \
		eq_cnt += tbls->xvid_thresh_tbl[s[3] - s[4] + 255]; \
		eq_cnt += tbls->xvid_thresh_tbl[s[4] - s[5] + 255]; \
		eq_cnt += tbls->xvid_thresh_tbl[s[5] - s[6] + 255]; \
		eq_cnt += tbls->xvid_thresh_tbl[s[6] - s[7] + 255]; \
		eq_cnt += tbls->xvid_thresh_tbl[s[7] - s[8] + 255]; \
		\
		if(eq_cnt < THR2) { /* Default mode */  \
			int a30, a31, a32;					\
			int diff, limit;					\
												\
			if(tbls->xvid_abs_tbl[(s[4] - s[5]) + 255] < quant) {			\
				a30 = ((s[3]<<1) - s[4] * 5 + s[5] * 5 - (s[6]<<1));	\
				a31 = ((s[1]<<1) - s[2] * 5 + s[3] * 5 - (s[4]<<1));	\
				a32 = ((s[5]<<1) - s[6] * 5 + s[7] * 5 - (s[8]<<1));	\
																		\
				diff = (5 * ((SIGN(a30) * MIN(FAST_ABS(a30), MIN(FAST_ABS(a31), FAST_ABS(a32)))) - a30) + 32) >> 6;	\
				limit = (s[4] - s[5]) / 2;	\
				\
				if (limit > 0)				\
					diff = (diff < 0) ? 0 : ((diff > limit) ? limit : diff);	\
				else	\
					diff = (diff > 0) ? 0 : ((diff < limit) ? limit : diff);	\
																				\
				*v[4] -= diff;	\
				*v[5] += diff;	\
			}	\
			if (dering) {	\
				e[0] = (tbls->xvid_abs_tbl[(s[0] - s[1]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[1] = (tbls->xvid_abs_tbl[(s[1] - s[2]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[2] = (tbls->xvid_abs_tbl[(s[2] - s[3]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[3] = (tbls->xvid_abs_tbl[(s[3] - s[4]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[4] = (tbls->xvid_abs_tbl[(s[4] - s[5]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[5] = (tbls->xvid_abs_tbl[(s[5] - s[6]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[6] = (tbls->xvid_abs_tbl[(s[6] - s[7]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[7] = (tbls->xvid_abs_tbl[(s[7] - s[8]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[8] = (tbls->xvid_abs_tbl[(s[8] - s[9]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				\
				e[1] |= e[0];	\
				e[2] |= e[1];	\
				e[3] |= e[2];	\
				e[4] |= e[3];	\
				e[5] |= e[4];	\
				e[6] |= e[5];	\
				e[7] |= e[6];	\
				e[8] |= e[7];	\
				e[9]  = e[8];	\
				\
				APPLY_DERING(1)	\
				APPLY_DERING(2)	\
				APPLY_DERING(3)	\
				APPLY_DERING(4)	\
				APPLY_DERING(5)	\
				APPLY_DERING(6)	\
				APPLY_DERING(7)	\
				APPLY_DERING(8) \
			}	\
		}	\
		else {	/* DC-offset mode */	\
			uint8_t p0, p9;	\
			int min, max;	\
							\
			/* Now decide whether to apply smoothing filter or not */	\
			max = FAST_MAX(s[1], FAST_MAX(s[2], FAST_MAX(s[3], FAST_MAX(s[4], FAST_MAX(s[5], FAST_MAX(s[6], FAST_MAX(s[7], s[8])))))));	\
			min = FAST_MIN(s[1], FAST_MIN(s[2], FAST_MIN(s[3], FAST_MIN(s[4], FAST_MIN(s[5], FAST_MIN(s[6], FAST_MIN(s[7], s[8])))))));	\
			\
			if(((max-min)) < 2*quant) {	\
										\
				/* Choose edge pixels */	\
				p0 = (tbls->xvid_abs_tbl[(s[1] - s[0]) + 255] < quant) ? s[0] : s[1];	\
				p9 = (tbls->xvid_abs_tbl[(s[8] - s[9]) + 255] < quant) ? s[9] : s[8];	\
																\
				*v[1] = (uint8_t) ((6*p0 + (s[1]<<2) + (s[2]<<1) + (s[3]<<1) + s[4] + s[5] + 8) >> 4);	\
				*v[2] = (uint8_t) (((p0<<2) + (s[1]<<1) + (s[2]<<2) + (s[3]<<1) + (s[4]<<1) + s[5] + s[6] + 8) >> 4);	\
				*v[3] = (uint8_t) (((p0<<1) + (s[1]<<1) + (s[2]<<1) + (s[3]<<2) + (s[4]<<1) + (s[5]<<1) + s[6] + s[7] + 8) >> 4);	\
				*v[4] = (uint8_t) ((p0 + s[1] + (s[2]<<1) + (s[3]<<1) + (s[4]<<2) + (s[5]<<1) + (s[6]<<1) + s[7] + s[8] + 8) >> 4);	\
				*v[5] = (uint8_t) ((s[1] + s[2] + (s[3]<<1) + (s[4]<<1) + (s[5]<<2) + (s[6]<<1) + (s[7]<<1) + s[8] + p9 + 8) >> 4);	\
				*v[6] = (uint8_t) ((s[2] + s[3] + (s[4]<<1) + (s[5]<<1) + (s[6]<<2) + (s[7]<<1) + (s[8]<<1) + (p9<<1) + 8) >> 4);	\
				*v[7] = (uint8_t) ((s[3] + s[4] + (s[5]<<1) + (s[6]<<1) + (s[7]<<2) + (s[8]<<1) + (p9<<2) + 8) >> 4);	\
				*v[8] = (uint8_t) ((s[4] + s[5] + (s[6]<<1) + (s[7]<<1) + (s[8]<<2) + 6*p9 + 8) >> 4);	\
			}	\
		}	

void deblock8x8_h(XVID_POSTPROC *tbls, uint8_t *img, int stride, int quant, int dering)
{
	int eq_cnt;
	uint8_t *v[10];
	int s[10];
	int e[10];

	LOAD_DATA_HOR(0)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(1)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(2)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(3)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(4)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(5)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(6)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(7)
	APPLY_FILTER_CORE
}


void deblock8x8_v(XVID_POSTPROC *tbls, uint8_t *img, int stride, int quant, int dering)
{
	int eq_cnt;
	uint8_t *v[10];
	int s[10];
	int e[10];

	LOAD_DATA_VER(0)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(1)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(2)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(3)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(4)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(5)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(6)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(7)
	APPLY_FILTER_CORE
}

