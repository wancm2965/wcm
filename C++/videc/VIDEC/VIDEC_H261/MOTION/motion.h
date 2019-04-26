/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Motion Estimation header -
 *
 *  Copyright(C) 2002 Christoph Lampert <gruel@web.de>
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
 * $Id: motion.h,v 1.1.1.1 2004/10/29 10:03:57 lixq Exp $
 *
 ***************************************************************************/

#ifndef _MOTION_H_
#define _MOTION_H_

//#include "../portab.h"
#include "../h261/config.h"
#include "sad.h"
#ifdef __cplusplus
extern "C" {
#endif
	
#define MODE_INTER		0

#define	MODE_INTRA		3

/* hard coded motion search parameters for motion_est and smp_motion_est */

// very large value
#define MV_MAX_ERROR	(4096 * 256)


/* hard coded motion search parameters for motion_est */

/* very large value */
#define MV_MAX_ERROR	(4096 * 256)


/* how much a block's MV must differ from his neighbour  */
/* to be search for INTER4V. The more, the faster... */

/* sad16(0,0) bias; mpeg4 spec suggests nb/2+1 */
/* nb  = vop pixels * 2^(bpp-8) */
#define MV16_00_BIAS	(128+1)

/* INTER bias for INTER/INTRA decision; mpeg4 spec suggests 2*nb */
#define MV16_INTER_BIAS	512



/* vector map (vlc delta size) smoother parameters */
#define NEIGH_TEND_16X16	2

/* fast ((A)/2)*2 */
#define EVEN(A)		(((A)<0?(A)+1:(A)) & ~1)

#define MVzero(A) ( ((A).x)==(0) && ((A).y)==(0) )
#define MVequal(A,B) ( ((A).x)==((B).x) && ((A).y)==((B).y) )

#define MIN(X, Y) ((X)<(Y)?(X):(Y))
#define MAX(X, Y) ((X)>(Y)?(X):(Y))


/*****************************************************************************
 * Structures
 ****************************************************************************/

typedef struct
{
	u_int width;
	u_int height;
	u_int stride;
	u_int mb_width;
	u_int mb_height;

}
MBParam;

typedef struct
{
	u_char *y;
	u_char *u;
	u_char *v;
}
IMAGE;

typedef struct
{
	int x;
	int y;
}
VECTOR;

typedef struct
{
	/* decoder/encoder  */

	int mode;
//	int quant;					/* absolute quant */

	/* encoder specific */
	
	VECTOR mv16;

	int sad16;				/* SAD value for inter-VECTOR */

}
MACROBLOCK;


typedef struct
{
	u_int quant;
	IMAGE image;
	MACROBLOCK *mbs;

}
FRAMEINFO;




/* prototypes for MotionEstimation functions, i.e. PMVfast, EPZS or whatever */

typedef int(Search16Func) (const u_char * const pRef,
						   const IMAGE * const pCur,
						   const int x,
						   const int y,
						   const int start_x,
						   const int start_y,
						   const int center_x,
						   const int center_y,
						   const u_int MotionFlags,
						   const u_int iQuant,
						   const u_int iFcode,
						   const MBParam * const pParam,
						   const MACROBLOCK * const pMBs,
						   const MACROBLOCK * const prevMBs,
							VECTOR * const currMV);

typedef Search16Func *Search16FuncPtr;


Search16Func PMVfastSearch16;

/* default methods of Search, will be changed to function variable */

#ifndef SEARCH16
#define SEARCH16	PMVfastSearch16
#endif


/* prototypes for MainSearch functions, i.e. Diamondsearch, FullSearch or whatever */

typedef int(MainSearch16Func) (const u_char * const pRef,
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
								   int iFound);


typedef MainSearch16Func *MainSearch16FuncPtr;


#ifdef __cplusplus
}
#endif

#endif							/* _MOTION_H_ */
