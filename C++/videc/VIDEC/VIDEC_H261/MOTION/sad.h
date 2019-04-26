/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - SAD Routines header -
 *
 *  Copyright(C) 2002 Michael Militzer <isibaar@xvid.org>
 *               2002 Peter Ross <pross@xvid.org>
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
 * $Id: sad.h,v 1.1.1.1.2.1 2005/06/03 05:48:10 lixq Exp $
 *
 ****************************************************************************/

#ifndef _ENCODER_SAD_H_
#define _ENCODER_SAD_H_

#include "../h261/config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (sadInitFunc) (void);
typedef sadInitFunc *sadInitFuncPtr;

extern sadInitFuncPtr sadInit;
sadInitFunc sadInit_altivec;


typedef u_int(sad16Func) (const u_char * const cur,
							 const u_char * const ref,
							 const u_int stride,
							 const u_int best_sad);
typedef sad16Func *sad16FuncPtr;
extern sad16FuncPtr sad16;
sad16Func sad16_h261_c;
sad16Func sad16_h261_mmx;
sad16Func sad16_h261_xmm;
sad16Func sad16_h261_sse2;
sad16Func mrsad16_h261_c;


typedef u_int(dev16Func) (const u_char * const cur,
							 const u_int stride);
typedef dev16Func *dev16FuncPtr;
extern dev16FuncPtr dev16;
dev16Func dev16_h261_c;
dev16Func dev16_h261_mmx;
dev16Func dev16_h261_xmm;
dev16Func dev16_h261_sse2;

#ifdef __cplusplus
}
#endif
#endif							/* _ENCODER_SAD_H_ */
