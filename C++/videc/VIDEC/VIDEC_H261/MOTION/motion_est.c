/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Motion Estimation module -
 *
 *  Copyright(C) 2002 Christoph Lampert <gruel@web.de>
 *               2002 Michael Militzer <michael@xvid.org>
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
 * $Id: motion_est.c,v 1.1.1.1.2.2 2005/06/03 05:48:10 lixq Exp $
 *
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "motion.h"



static int lambda_vec16[32] =	/* rounded values for lambda param for weight of motion bits as in modified H.26L */
{ 0, (int) (1.00235 + 0.5), (int) (1.15582 + 0.5), (int) (1.31976 + 0.5),
		(int) (1.49591 + 0.5), (int) (1.68601 + 0.5),
	(int) (1.89187 + 0.5), (int) (2.11542 + 0.5), (int) (2.35878 + 0.5),
		(int) (2.62429 + 0.5), (int) (2.91455 + 0.5),
	(int) (3.23253 + 0.5), (int) (3.58158 + 0.5), (int) (3.96555 + 0.5),
		(int) (4.38887 + 0.5), (int) (4.85673 + 0.5),
	(int) (5.37519 + 0.5), (int) (5.95144 + 0.5), (int) (6.59408 + 0.5),
		(int) (7.31349 + 0.5), (int) (8.12242 + 0.5),
	(int) (9.03669 + 0.5), (int) (10.0763 + 0.5), (int) (11.2669 + 0.5),
		(int) (12.6426 + 0.5), (int) (14.2493 + 0.5),
	(int) (16.1512 + 0.5), (int) (18.442 + 0.5), (int) (21.2656 + 0.5),
		(int) (24.8580 + 0.5), (int) (29.6436 + 0.5),
	(int) (36.4949 + 0.5)
};


/* mv.length table */
static const u_int mvtab[16] = {
	1, 2, 3, 4, 6, 7, 7, 7,
	9, 9, 9, 10, 10, 10, 10, 10
	
};


static __inline u_int
mv_bits(int component)
{
	if (component == 0)
		return 1;
	
	if (component < 0)
		component = -component;
	
	if (component > 16)
		component = 16;
	
	return mvtab[component] + 1;
}


static __inline u_int
calc_delta_16(const int dx,
			  const int dy,
			  const u_int iQuant)
{
	return NEIGH_TEND_16X16 * lambda_vec16[iQuant] * (mv_bits(dx) + mv_bits(dy));
}


/*
 * Calculate the min/max range (in halfpixels)
 * relative to the _MACROBLOCK_ position
 */

static void __inline
get_range(int * const min_dx,
		  int * const max_dx,
		  int * const min_dy,
		  int * const max_dy,
		  const u_int x,
		  const u_int y,
		  const u_int block_sz,	/* block dimension, 8 or 16 */
		  const u_int width,
		  const u_int height,
		  const u_int fcode)
{

	const int search_range = 14;//16<<fcode;
	const int high = search_range - 1;
	const int low = -search_range;

	/* convert full-pixel measurements to half pixel */
	const int hp_width =  width;
	const int hp_height =  height;
	const int hp_edge =  block_sz;

	/* we need _right end_ of block, not x-coordinate */
	const int hp_x = x * block_sz;

	/* same for _bottom end_ */
	const int hp_y = y * block_sz;
/*
	*max_dx = MIN(high, hp_width - hp_x);
	*max_dy = MIN(high, hp_height - hp_y);
	*min_dx = MAX(low, -(hp_edge + hp_x));
	*min_dy = MAX(low, -(hp_edge + hp_y));
*/
	*max_dx = MIN(high, hp_width-hp_edge- hp_x);
	*max_dy = MIN(high, hp_height-hp_edge- hp_y);
	*min_dx = MAX(low, -hp_x);
	*min_dy = MAX(low, -hp_y);
}


/*
 * getref: calculate reference image pointer 
 * the decision to use interpolation h/v/hv or the normal image is
 * based on dx & dy.
 */

static __inline const u_char *
get_ref(const u_char * const refn,
		const u_int x,
		const u_int y,
		const u_int block,	/* block dimension, 8 or 16 */
		const int dx,
		const int dy,
		const u_int stride)
{
	return refn + (int) ((x * block + dx ) + (y * block + dy) * stride);
}


/* This is somehow a copy of get_ref, but with MV instead of X,Y */

static __inline const u_char *
get_ref_mv(const u_char * const refn,
		   const u_int x,
		   const u_int y,
		   const u_int block,	/* block dimension, 8 or 16 */

		   const VECTOR * mv,	/* measured in half-pel! */

		   const u_int stride)
{

	return refn + (int) ((x * block + mv->x ) + (y * block + mv->y) * stride);

}

/*****************************************************************************
 * Inlined functions
 ****************************************************************************/

/*
 * MODE_INTER, vm18 page 48
 * MODE_INTER4V vm18 page 51
 *
 *   (x,y-1)      (x+1,y-1)
 *   [   |   ]    [   |   ]
 *   [ 2 | 3 ]    [ 2 |   ]
 *
 *   (x-1,y)       (x,y)        (x+1,y)
 *   [   | 1 ]    [ 0 | 1 ]    [ 0 |   ]
 *   [   | 3 ]    [ 2 | 3 ]    [   |   ]
 */

static __inline VECTOR
get_pmv2(const MACROBLOCK * const mbs,
         const int mb_width,
         const int bound,
         const int x,
         const int y,
         const int block)
{
	static const VECTOR zeroMV = { 0, 0 };
    
    int lx, ly;         /* left */
    int tx, ty;         /* top */
    int rx, ry;         /* top-right */
    int lpos, tpos, rpos;
    int num_cand, last_cand;

	VECTOR pmv[4];	/* left neighbour, top neighbour, top-right neighbour */

	switch (block) {
	case 0:
		lx = x - 1;	ly = y;		
		tx = x;		ty = y - 1;	
		rx = x + 1;	ry = y - 1;	
		break;
	case 1:
		lx = x;		ly = y;		
		tx = x;		ty = y - 1;	
		rx = x + 1;	ry = y - 1;	
		break;
	case 2:
		lx = x - 1;	ly = y;		
		tx = x;		ty = y;		
		rx = x;		ry = y;		
		break;
	default:
		lx = x;		ly = y;	
		tx = x;		ty = y;	
		rx = x;		ry = y;	
	}

    lpos = lx + ly * mb_width;
    rpos = rx + ry * mb_width;
    tpos = tx + ty * mb_width;
    last_cand = num_cand = 0;

    if (lpos >= bound && lx >= 0) {
        num_cand++;
        last_cand = 1;
        pmv[1] = mbs[lpos].mv16;
    } else {
        pmv[1] = zeroMV;
    }

    if (tpos >= bound) {
        num_cand++;
        last_cand = 2;
        pmv[2] = mbs[tpos].mv16;
    } else {
        pmv[2] = zeroMV;
    }
    
    if (rpos >= bound && rx < mb_width) {
        num_cand++;
        last_cand = 3;
        pmv[3] = mbs[rpos].mv16;
    } else {
        pmv[3] = zeroMV;
    }

    /*
	 * If there're more than one candidate, we return the median vector
	 * edgomez : the special case "no candidates" is handled the same way
	 *           because all vectors are set to zero. So the median vector
	 *           is {0,0}, and this is exactly the vector we must return
	 *           according to the mpeg4 specs.
	 */

	if (num_cand != 1) {
		/* set median */
		
   		pmv[0].x =
			MIN(MAX(pmv[1].x, pmv[2].x),
				MIN(MAX(pmv[2].x, pmv[3].x), MAX(pmv[1].x, pmv[3].x)));
		pmv[0].y =
			MIN(MAX(pmv[1].y, pmv[2].y),
				MIN(MAX(pmv[2].y, pmv[3].y), MAX(pmv[1].y, pmv[3].y)));
		return pmv[0];
	 }

	 return pmv[last_cand];  /* no point calculating median mv */
}



/*
 * pmv are filled with: 
 *  [0]: Median (or whatever is correct in a special case)
 *  [1]: left neighbour
 *  [2]: top neighbour
 *  [3]: topright neighbour
 * psad are filled with:
 *  [0]: minimum of [1] to [3]
 *  [1]: left neighbour's SAD (NB:[1] to [3] are actually not needed)  
 *  [2]: top neighbour's SAD
 *  [3]: topright neighbour's SAD
 */
	 
static __inline int
get_pmvdata2(const MACROBLOCK * const mbs,
         const int mb_width,
         const int bound,
         const int x,
         const int y,
         const int block,
		 VECTOR * const pmv,
		 int * const psad)
{
	static const VECTOR zeroMV = { 0, 0 };
    
    int lx, ly;         /* left */
    int tx, ty;         /* top */
    int rx, ry;         /* top-right */
    int lpos, tpos, rpos;
    int num_cand, last_cand;

	switch (block) {
	case 0:
		lx = x - 1;	ly = y;		
		tx = x;		ty = y - 1;	
		rx = x + 1;	ry = y - 1;	
		break;
	case 1:
		lx = x;		ly = y;		
		tx = x;		ty = y - 1;	
		rx = x + 1;	ry = y - 1;	
		break;
	case 2:
		lx = x - 1;	ly = y;	
		tx = x;		ty = y;	
		rx = x;		ry = y;	
		break;
	default:
		lx = x;		ly = y;	
		tx = x;		ty = y;	
		rx = x;		ry = y;	
	}

    lpos = lx + ly * mb_width;
    rpos = rx + ry * mb_width;
    tpos = tx + ty * mb_width;
    last_cand = num_cand = 0;

    if (lpos >= bound && lx >= 0) {
        num_cand++;
        last_cand = 1;
        pmv[1] = mbs[lpos].mv16;
		psad[1] = mbs[lpos].sad16;
    } else {
        pmv[1] = zeroMV;
		psad[1] = MV_MAX_ERROR;
    }

    if (tpos >= bound) {
        num_cand++;
        last_cand = 2;
        pmv[2]= mbs[tpos].mv16;
        psad[2] = mbs[tpos].sad16;
    } else {
        pmv[2] = zeroMV;
		psad[2] = MV_MAX_ERROR;
    }
    
    if (rpos >= bound && rx < mb_width) {
        num_cand++;
        last_cand = 3;
        pmv[3] = mbs[rpos].mv16;
        psad[3] = mbs[rpos].sad16;
    } else {
        pmv[3] = zeroMV;
		psad[3] = MV_MAX_ERROR;
    }

	/* original pmvdata() compatibility hack */
	if (x == 0 && y == 0 && block == 0)
	{
		pmv[0] = pmv[1] = pmv[2] = pmv[3] = zeroMV;
		psad[0] = 0;
		psad[1] = psad[2] = psad[3] = MV_MAX_ERROR;
		return 0;
	}

    /* if only one valid candidate preictor, the invalid candiates are set to the canidate */
	if (num_cand == 1) {
		pmv[0] = pmv[last_cand];
		psad[0] = psad[last_cand];
        /* return MVequal(pmv[0], zeroMV);  no point calculating median mv and minimum sad  */
		
		/* original pmvdata() compatibility hack */
		return y==0 && block <= 1 ? 0 : MVequal(pmv[0], zeroMV);
	}

	if ((MVequal(pmv[1], pmv[2])) && (MVequal(pmv[1], pmv[3]))) {
		pmv[0] = pmv[1];
		psad[0] = MIN(MIN(psad[1], psad[2]), psad[3]);
		return 1;
		/* compatibility patch */
		/*return y==0 && block <= 1 ? 0 : 1; */
	}

	/* set median, minimum */

	pmv[0].x =
		MIN(MAX(pmv[1].x, pmv[2].x),
			MIN(MAX(pmv[2].x, pmv[3].x), MAX(pmv[1].x, pmv[3].x)));
	pmv[0].y =
		MIN(MAX(pmv[1].y, pmv[2].y),
			MIN(MAX(pmv[2].y, pmv[3].y), MAX(pmv[1].y, pmv[3].y)));

	psad[0] = MIN(MIN(psad[1], psad[2]), psad[3]);

   	return 0;
}



#define CHECK_MV16_ZERO {\
  if ( (0 <= max_dx) && (0 >= min_dx) \
    && (0 <= max_dy) && (0 >= min_dy) ) \
  { \
    iSAD = sad16( cur, get_ref(pRef, x, y, 16, 0, 0 , iEdgedWidth), iEdgedWidth, MV_MAX_ERROR); \
    iSAD += calc_delta_16(-center_x, -center_y,iQuant);\
    if (iSAD < iMinSAD) \
    {  iMinSAD=iSAD; currMV->x=0; currMV->y=0; }  }	\
}

#define NOCHECK_MV16_CANDIDATE(X,Y) { \
    iSAD = sad16( cur, get_ref(pRef, x, y, 16, X, Y, iEdgedWidth),iEdgedWidth, iMinSAD); \
    iSAD += calc_delta_16((X) - center_x, (Y) - center_y,iQuant);\
    if (iSAD < iMinSAD) \
    {  iMinSAD=iSAD; currMV->x=(X); currMV->y=(Y); } \
}

#define CHECK_MV16_CANDIDATE(X,Y) { \
  if ( ((X) <= max_dx) && ((X) >= min_dx) \
    && ((Y) <= max_dy) && ((Y) >= min_dy) ) \
  { \
    iSAD = sad16( cur, get_ref(pRef, x, y, 16, X, Y, iEdgedWidth),iEdgedWidth, iMinSAD); \
    iSAD += calc_delta_16((X) - center_x, (Y) - center_y,iQuant);\
    if (iSAD < iMinSAD) \
    {  iMinSAD=iSAD; currMV->x=(X); currMV->y=(Y); } } \
}

#define CHECK_MV16_CANDIDATE_DIR(X,Y,D) { \
  if ( ((X) <= max_dx) && ((X) >= min_dx) \
    && ((Y) <= max_dy) && ((Y) >= min_dy) ) \
  { \
    iSAD = sad16( cur, get_ref(pRef, x, y, 16, X, Y, iEdgedWidth),iEdgedWidth, iMinSAD); \
    iSAD += calc_delta_16((X) - center_x, (Y) - center_y,iQuant);\
    if (iSAD < iMinSAD) \
    {  iMinSAD=iSAD; currMV->x=(X); currMV->y=(Y); iDirection=(D); } } \
}

#define CHECK_MV16_CANDIDATE_FOUND(X,Y,D) { \
  if ( ((X) <= max_dx) && ((X) >= min_dx) \
    && ((Y) <= max_dy) && ((Y) >= min_dy) ) \
  { \
    iSAD = sad16( cur, get_ref(pRef, x, y, 16, X, Y, iEdgedWidth),iEdgedWidth, iMinSAD); \
    iSAD += calc_delta_16((X) - center_x, (Y) - center_y,iQuant);\
    if (iSAD < iMinSAD) \
    {  iMinSAD=iSAD; currMV->x=(X); currMV->y=(Y); iDirection=(D); iFound=0; } } \
}




int
Diamond16_MainSearch(const u_char * const pRef,
					 const u_char * const cur,
					 const int x,
					 const int y,
					 const int start_x,
					 const int start_y,
					 int iMinSAD,
					 VECTOR * const currMV,
					 const int center_x,
					 const int center_y,
					 const int min_dx,
					 const int max_dx,
					 const int min_dy,
					 const int max_dy,
					 const int iEdgedWidth,
					 const int iDiamondSize,
					 const int iFcode,
					 const int iQuant,
					 int iFound)
{
/* Do a diamond search around given starting point, return SAD of best */

	int iDirection = 0;
	int iDirectionBackup;
	int iSAD;
	VECTOR backupMV;

	backupMV.x = start_x;
	backupMV.y = start_y;

/* It's one search with full Diamond pattern, and only 3 of 4 for all following diamonds */

	CHECK_MV16_CANDIDATE_DIR(backupMV.x - iDiamondSize, backupMV.y, 1);
	CHECK_MV16_CANDIDATE_DIR(backupMV.x + iDiamondSize, backupMV.y, 2);
	CHECK_MV16_CANDIDATE_DIR(backupMV.x, backupMV.y - iDiamondSize, 3);
	CHECK_MV16_CANDIDATE_DIR(backupMV.x, backupMV.y + iDiamondSize, 4);

	if (iDirection)	{	
		while (!iFound) {
			iFound = 1;
			backupMV = *currMV;
			iDirectionBackup = iDirection;

			if (iDirectionBackup != 2)
				CHECK_MV16_CANDIDATE_FOUND(backupMV.x - iDiamondSize,
										   backupMV.y, 1);
			if (iDirectionBackup != 1)
				CHECK_MV16_CANDIDATE_FOUND(backupMV.x + iDiamondSize,
										   backupMV.y, 2);
			if (iDirectionBackup != 4)
				CHECK_MV16_CANDIDATE_FOUND(backupMV.x,
										   backupMV.y - iDiamondSize, 3);
			if (iDirectionBackup != 3)
				CHECK_MV16_CANDIDATE_FOUND(backupMV.x,
										   backupMV.y + iDiamondSize, 4);
		}
	} else {
		currMV->x = start_x;
		currMV->y = start_y;
	}
	return iMinSAD;
}


int
AdvDiamond16_MainSearch(const u_char * const pRef,
						const u_char * const cur,
						const int x,
						const int y,
						const int start_xi,
						const int start_yi,
						int iMinSAD,
						VECTOR * const currMV,
						const int center_x,
						const int center_y,
						const int min_dx,
						const int max_dx,
						const int min_dy,
						const int max_dy,
						const int iEdgedWidth,
						const int iDiamondSize,
						const int iFcode,
						const int iQuant,
						int iDirection)
{

	int iSAD;
	int start_x = start_xi, start_y = start_yi;

/* directions: 1 - left (x-1); 2 - right (x+1), 4 - up (y-1); 8 - down (y+1) */

	if (iDirection) {
		CHECK_MV16_CANDIDATE(start_x - iDiamondSize, start_y);
		CHECK_MV16_CANDIDATE(start_x + iDiamondSize, start_y);
		CHECK_MV16_CANDIDATE(start_x, start_y - iDiamondSize);
		CHECK_MV16_CANDIDATE(start_x, start_y + iDiamondSize);
	} else {
		int bDirection = 1 + 2 + 4 + 8;

		do {
			iDirection = 0;
			if (bDirection & 1)	/*we only want to check left if we came from the right (our last motion was to the left, up-left or down-left) */
				CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize, start_y, 1);

			if (bDirection & 2)
				CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize, start_y, 2);

			if (bDirection & 4)
				CHECK_MV16_CANDIDATE_DIR(start_x, start_y - iDiamondSize, 4);

			if (bDirection & 8)
				CHECK_MV16_CANDIDATE_DIR(start_x, start_y + iDiamondSize, 8);

			/* now we're doing diagonal checks near our candidate */

			if (iDirection)		/*checking if anything found */
			{
				bDirection = iDirection;
				iDirection = 0;
				start_x = currMV->x;
				start_y = currMV->y;
				if (bDirection & 3)	/*our candidate is left or right */
				{
					CHECK_MV16_CANDIDATE_DIR(start_x, start_y + iDiamondSize, 8);
					CHECK_MV16_CANDIDATE_DIR(start_x, start_y - iDiamondSize, 4);
				} else			/* what remains here is up or down */
				{
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize, start_y, 2);
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize, start_y, 1);
				}

				if (iDirection) {
					bDirection += iDirection;
					start_x = currMV->x;
					start_y = currMV->y;
				}
			} else				/*about to quit, eh? not so fast.... */
			{
				switch (bDirection) {
				case 2:
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y - iDiamondSize, 2 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y + iDiamondSize, 2 + 8);
					break;
				case 1:
	
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y - iDiamondSize, 1 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y + iDiamondSize, 1 + 8);
					break;
				case 2 + 4:
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y - iDiamondSize, 1 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y - iDiamondSize, 2 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y + iDiamondSize, 2 + 8);
					break;
				case 4:
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y - iDiamondSize, 2 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y - iDiamondSize, 1 + 4);
					break;
				case 8:
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y + iDiamondSize, 2 + 8);
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y + iDiamondSize, 1 + 8);
					break;
				case 1 + 4:
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y + iDiamondSize, 1 + 8);
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y - iDiamondSize, 1 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y - iDiamondSize, 2 + 4);
					break;
				case 2 + 8:
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y - iDiamondSize, 1 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y + iDiamondSize, 1 + 8);
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y + iDiamondSize, 2 + 8);
					break;
				case 1 + 8:
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y - iDiamondSize, 2 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y + iDiamondSize, 2 + 8);
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y + iDiamondSize, 1 + 8);
					break;
				default:		/*1+2+4+8 == we didn't find anything at all */
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y - iDiamondSize, 1 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x - iDiamondSize,
											 start_y + iDiamondSize, 1 + 8);
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y - iDiamondSize, 2 + 4);
					CHECK_MV16_CANDIDATE_DIR(start_x + iDiamondSize,
											 start_y + iDiamondSize, 2 + 8);
					break;
				}
				if (!iDirection)
					break;		/*ok, the end. really */
				else {
					bDirection = iDirection;
					start_x = currMV->x;
					start_y = currMV->y;
				}
			}
		}
		while (1);				/*forever */
	}

	return iMinSAD;
}




#define PMV_HALFPEL16 (PMV_HALFPELDIAMOND16|PMV_HALFPELREFINE16)

#define  PMV_FULLPEL16   (PMV_EARLYSTOP16 | PMV_ADVANCEDDIAMOND16)

int
PMVfastSearch16(const u_char * const pRef,
				const IMAGE * const pCur,
				const int x,
				const int y,
				const int start_x,	/* start is searched first, so it should contain the most */
				const int start_y,  /* likely motion vector for this block */
				const int center_x,	/* center is from where length of MVs is measured */
				const int center_y,
				const u_int MotionFlags,
				const u_int iQuant,
				const u_int iFcode,
				const MBParam * const pParam,
				const MACROBLOCK * const pMBs,
				const MACROBLOCK * const prevMBs,
				VECTOR * const currMV)
{
	const u_int iWcount = pParam->mb_width;
	const int iWidth = pParam->width;
	const int iHeight = pParam->height;
	const int iEdgedWidth = pParam->stride;

	const u_char *cur = pCur->y + x * 16 + y * 16 * iEdgedWidth;

	int iDiamondSize;

	int min_dx;
	int max_dx;
	int min_dy;
	int max_dy;

	int iFound;

	VECTOR newMV;
	VECTOR backupMV;			/* just for PMVFAST */

	VECTOR pmv[4];
	int psad[4];

	MainSearch16FuncPtr MainSearchPtr;

	const MACROBLOCK *const prevMB = prevMBs + x + y * iWcount;

	int threshA, threshB;
	int bPredEq;
	int iMinSAD, iSAD;

/* Get maximum range */
	get_range(&min_dx, &max_dx, &min_dy, &max_dy, x, y, 16, iWidth, iHeight,
			  iFcode);

/* we work with abs. MVs, not relative to prediction, so get_range is called relative to 0,0 */

//	if (!(MotionFlags & PMV_HALFPEL16)) 
	{
		min_dx = EVEN(min_dx);
		max_dx = EVEN(max_dx);
		min_dy = EVEN(min_dy);
		max_dy = EVEN(max_dy);
	}

	/* because we might use something like IF (dx>max_dx) THEN dx=max_dx; */
	bPredEq = get_pmvdata2(pMBs, iWcount, 0, x, y, 0, pmv, psad);

	if ((x == 0) && (y == 0)) {
		threshA = 512;
		threshB = 1024;
	} else {
		threshA = psad[0];
		threshB = threshA + 256;
		if (threshA < 512)
			threshA = 512;
		if (threshA > 1024)
			threshA = 1024;
		if (threshB > 1792)
			threshB = 1792;
	}

	iFound = 0;

/* Step 4: Calculate SAD around the Median prediction. 
   MinSAD=SAD 
   If Motion Vector equal to Previous frame motion vector 
   and MinSAD<PrevFrmSAD goto Step 10. 
   If SAD<=256 goto Step 10. 
*/

	currMV->x = start_x;		
	currMV->y = start_y;		

//	if (!(MotionFlags & PMV_HALFPEL16)) 
	{	/* This should NOT be necessary! */
		currMV->x = EVEN(currMV->x);
		currMV->y = EVEN(currMV->y);
	}

	if (currMV->x > max_dx) {
		currMV->x = max_dx;
	}
	if (currMV->x < min_dx) {
		currMV->x = min_dx;
	}
	if (currMV->y > max_dy) {
		currMV->y = max_dy;
	}
	if (currMV->y < min_dy) {
		currMV->y = min_dy;
	}

	iMinSAD =
		sad16(cur,
			  get_ref_mv(pRef, x, y, 16, currMV,
						 iEdgedWidth), iEdgedWidth, MV_MAX_ERROR);
	iMinSAD +=
		calc_delta_16(currMV->x - center_x, currMV->y - center_y,iQuant);

	if ((iMinSAD < 256) ||
		((MVequal(*currMV, prevMB->mv16)) &&
		 ((int) iMinSAD < prevMB->sad16))) {
		if (iMinSAD < (int)(2 * iQuant))	/* high chances for SKIP-mode */
		{
			if (!MVzero(*currMV)) {
				iMinSAD += MV16_00_BIAS;
				CHECK_MV16_ZERO;	/* (0,0) saves space for letterboxed pictures */
				iMinSAD -= MV16_00_BIAS;
			}
		}

//		if (MotionFlags & PMV_QUICKSTOP16)
			goto PMVfast16_Terminate_without_Refine;
//		if (MotionFlags & PMV_EARLYSTOP16)
//			goto PMVfast16_Terminate_with_Refine;
	}


/* Step 2 (lazy eval): Calculate Distance= |MedianMVX| + |MedianMVY| where MedianMV is the motion 
   vector of the median. 
   If PredEq=1 and MVpredicted = Previous Frame MV, set Found=2  
*/

	if ((bPredEq) && (MVequal(pmv[0], prevMB->mv16)))
		iFound = 2;

/* Step 3 (lazy eval): If Distance>0 or thresb<1536 or PredEq=1 Select small Diamond Search. 
   Otherwise select large Diamond Search. 
*/

	if ((!MVzero(pmv[0])) || (threshB < 1536) || (bPredEq))
		iDiamondSize = 1;		/* halfpel! */
	else
		iDiamondSize = 2;		/* halfpel! */

//	if (!(MotionFlags & PMV_HALFPELDIAMOND16))
//		iDiamondSize *= 2;

/* 
   Step 5: Calculate SAD for motion vectors taken from left block, top, top-right, and Previous frame block. 
   Also calculate (0,0) but do not subtract offset. 
   Let MinSAD be the smallest SAD up to this point. 
   If MV is (0,0) subtract offset. 
*/

/* (0,0) is always possible */

	if (!MVzero(pmv[0]))
		CHECK_MV16_ZERO;

/* previous frame MV is always possible */

	if (!MVzero(prevMB->mv16))
		if (!MVequal(prevMB->mv16, pmv[0]))
			CHECK_MV16_CANDIDATE(prevMB->mv16.x, prevMB->mv16.y);

/* left neighbour, if allowed */

	if (!MVzero(pmv[1]))
		if (!MVequal(pmv[1], prevMB->mv16))
			if (!MVequal(pmv[1], pmv[0])) {
			//	if (!(MotionFlags & PMV_HALFPEL16)) 
				{
					pmv[1].x = EVEN(pmv[1].x);
					pmv[1].y = EVEN(pmv[1].y);
				}

				CHECK_MV16_CANDIDATE(pmv[1].x, pmv[1].y);
			}
/* top neighbour, if allowed */
	if (!MVzero(pmv[2]))
		if (!MVequal(pmv[2], prevMB->mv16))
			if (!MVequal(pmv[2], pmv[0]))
				if (!MVequal(pmv[2], pmv[1])) {
				//	if (!(MotionFlags & PMV_HALFPEL16))
					{
						pmv[2].x = EVEN(pmv[2].x);
						pmv[2].y = EVEN(pmv[2].y);
					}
					CHECK_MV16_CANDIDATE(pmv[2].x, pmv[2].y);

/* top right neighbour, if allowed */
					if (!MVzero(pmv[3]))
						if (!MVequal(pmv[3], prevMB->mv16))
							if (!MVequal(pmv[3], pmv[0]))
								if (!MVequal(pmv[3], pmv[1]))
									if (!MVequal(pmv[3], pmv[2])) {
										//if (!(MotionFlags & PMV_HALFPEL16)) 
										{
											pmv[3].x = EVEN(pmv[3].x);
											pmv[3].y = EVEN(pmv[3].y);
										}
										CHECK_MV16_CANDIDATE(pmv[3].x,
															 pmv[3].y);
									}
				}

	if ((MVzero(*currMV)) &&
		(!MVzero(pmv[0])) /* && (iMinSAD <= iQuant * 96) */ )
		iMinSAD -= MV16_00_BIAS;


/* Step 6: If MinSAD <= thresa goto Step 10. 
   If Motion Vector equal to Previous frame motion vector and MinSAD<PrevFrmSAD goto Step 10. 
*/

	if ((iMinSAD <= threshA) ||
		(MVequal(*currMV, prevMB->mv16) &&
		 ((int) iMinSAD < prevMB->sad16))) {
	//	if (MotionFlags & PMV_QUICKSTOP16)
			goto PMVfast16_Terminate_without_Refine;
	//	if (MotionFlags & PMV_EARLYSTOP16)
	//		goto PMVfast16_Terminate_with_Refine;
	}


/************ (Diamond Search)  **************/
/* 
   Step 7: Perform Diamond search, with either the small or large diamond. 
   If Found=2 only examine one Diamond pattern, and afterwards goto step 10 
   Step 8: If small diamond, iterate small diamond search pattern until motion vector lies in the center of the diamond. 
   If center then goto step 10. 
   Step 9: If large diamond, iterate large diamond search pattern until motion vector lies in the center. 
   Refine by using small diamond and goto step 10. 
*/

//	if (MotionFlags & PMV_ADVANCEDDIAMOND16)
//		MainSearchPtr = AdvDiamond16_MainSearch;
//	else
		MainSearchPtr = Diamond16_MainSearch;

	backupMV = *currMV;			/* save best prediction, actually only for EXTSEARCH */


/* default: use best prediction as starting point for one call of PMVfast_MainSearch */
	iSAD =
		(*MainSearchPtr) (pRef, cur, x, y, 
						  currMV->x, currMV->y, iMinSAD, &newMV, center_x, center_y, 
						  min_dx, max_dx,
						  min_dy, max_dy, iEdgedWidth, iDiamondSize, iFcode,
						  iQuant, iFound);

	if (iSAD < iMinSAD) {
		*currMV = newMV;
		iMinSAD = iSAD;
	}

//	if (MotionFlags & PMV_EXTSEARCH16)
	{
/* extended: search (up to) two more times: orignal prediction and (0,0) */

		if (!(MVequal(pmv[0], backupMV))) {
			iSAD =
				(*MainSearchPtr) (pRef,cur, x, y,
								  center_x, center_y, iMinSAD, &newMV, center_x, center_y,
								  min_dx, max_dx, min_dy, max_dy, iEdgedWidth,
								  iDiamondSize, iFcode, iQuant, iFound);

			if (iSAD < iMinSAD) {
				*currMV = newMV;
				iMinSAD = iSAD;
			}
		}

		if ((!(MVzero(pmv[0]))) && (!(MVzero(backupMV)))) {
			iSAD =
				(*MainSearchPtr) (pRef,cur, x, y, 0, 0,
								  iMinSAD, &newMV, center_x, center_y, 
								  min_dx, max_dx, min_dy, max_dy, 
								  iEdgedWidth, iDiamondSize, iFcode,
								  iQuant, iFound);

			if (iSAD < iMinSAD) {
				*currMV = newMV;
				iMinSAD = iSAD;
			}
		}
	}

/* 
   Step 10:  The motion vector is chosen according to the block corresponding to MinSAD.
*/


  PMVfast16_Terminate_without_Refine:
   return iMinSAD;
}

int
MotionEstimation_h261(MBParam * const pParam,
				 FRAMEINFO * const current,
				 FRAMEINFO * const reference)
{
	
	const u_int iWcount = pParam->mb_width;
	const u_int iHcount = pParam->mb_height;
	MACROBLOCK *const pMBs = current->mbs;
	MACROBLOCK *const prevMBs = reference->mbs;
	const IMAGE *const pCurrent = &current->image;
	const IMAGE *const pRef = &reference->image;
	
	static const VECTOR zeroMV = { 0, 0 };
	VECTOR predMV;
	const u_int fcode=0;
	const u_int motion_flags=0;
	u_int x, y;
	u_int iIntra = 0;
	u_int osad16=0;
	int VAR=0;
	int VAROR=0;

	if (sadInit)
		(*sadInit) ();
	
	for (y = 0; y < iHcount; y++)	{
		for (x = 0; x < iWcount; x ++)	{
			
			MACROBLOCK *const pMB = &pMBs[x + y * iWcount];
			
			predMV = get_pmv2(pMBs, pParam->mb_width, 0, x, y, 0);    
			
			pMB->sad16 =
				SEARCH16(pRef->y,pCurrent, 
				x, y, predMV.x, predMV.y, predMV.x, predMV.y, 
				motion_flags, current->quant,
				fcode, pParam, pMBs, prevMBs, &pMB->mv16);
			
			if (0 < (pMB->sad16 - MV16_INTER_BIAS)) 
			{
				int deviation;
				
				deviation =
					dev16(pCurrent->y + x * 16 + y * 16 * pParam->stride,
					pParam->stride);
				
				if (deviation < (pMB->sad16 - MV16_INTER_BIAS)) 
				{
					pMB->mode = 0;
					pMB->mv16 = zeroMV;
					pMB->sad16 =  deviation;
					
					iIntra++;
					continue;
				}
			}
			if (0==pMB->mv16.x&&0==pMB->mv16.y)
				pMB->mode = 2;                    // Inter mode 
			else 
		//		pMB->mv16 = zeroMV;
				pMB->mode = 5;                    // No Filter MC 
			
	/*::todo MType decision <reference code>
			if ((VAR < 64) || (VAROR > VAR))
			{                               // (MC+Inter)mode 
				if ((xValue < 1.0) ||
					((xValue < 3.0) && (yValue > (xValue*0.5))) ||
					((yValue > (xValue/1.1))))
					MType = 2;                    // Inter mode 
				else if (VAR < (double) D_FILTERTHRESHOLD) // MC mode 
					MType = 5;                    // No Filter MC 
				else
					MType = 8;                    // Filter MC 
			}
			else 
				MType = 0; //Intramod
	*/
		}
	}
	return 0;
}