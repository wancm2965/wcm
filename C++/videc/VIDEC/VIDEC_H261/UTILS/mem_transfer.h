/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - 8<->16 bit buffer transfer header -
 *
 *  Copyright(C) 2002 Michael Militzer <isibaar@xvid.org>
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
 * $Id: mem_transfer.h,v 1.1.1.1.2.2 2005/06/06 09:17:12 lixq Exp $
 *
 ****************************************************************************/

#ifndef _MEM_TRANSFER_H
#define _MEM_TRANSFER_H
#include "../h261/config.h"
#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 * transfer8to16 API
 ****************************************************************************/

typedef void (TRANSFER_8TO16COPY) (short * const dst,
								   const u_char * const src,
								   u_int stride);

typedef TRANSFER_8TO16COPY *TRANSFER_8TO16COPY_PTR;

/* Our global function pointer - Initialized in xvid.c */
extern TRANSFER_8TO16COPY_PTR transfer_8to16copy;

/* Implemented functions */
TRANSFER_8TO16COPY transfer_8to16copy_h261_c;
TRANSFER_8TO16COPY transfer_8to16copy_h261_mmx;
TRANSFER_8TO16COPY transfer_8to16copy_h261_ia64;

/*****************************************************************************
 * transfer16to8 API
 ****************************************************************************/

typedef void (TRANSFER_16TO8COPY) (u_char * const dst,
								   const short * const src,
								   u_int stride);
typedef TRANSFER_16TO8COPY *TRANSFER_16TO8COPY_PTR;

/* Our global function pointer - Initialized in xvid.c */
extern TRANSFER_16TO8COPY_PTR transfer_16to8copy;

/* Implemented functions */
TRANSFER_16TO8COPY transfer_16to8copy_h261_c;
TRANSFER_16TO8COPY transfer_16to8copy_h261_mmx;
TRANSFER_16TO8COPY transfer_16to8copy_h261_ia64;

/*****************************************************************************
 * transfer8to16 + substraction op API
 ****************************************************************************/

typedef void (TRANSFER_8TO16SUB_EXT) (short * const dct,
								  u_char * const cur,
								  const u_int curStride,
								  const u_char * ref,
								  const u_int refStride);
typedef TRANSFER_8TO16SUB_EXT *TRANSFER_8TO16SUB_EXT_PTR;
extern TRANSFER_8TO16SUB_EXT_PTR transfer_8to16sub_ext;

TRANSFER_8TO16SUB_EXT transfer_8to16sub_ext_h261_c;
TRANSFER_8TO16SUB_EXT transfer_8to16sub_ext_h261_mmx;

typedef void (TRANSFER_8TO16SUB) (short * const dct,
								  u_char * const cur,
								  const u_char * ref,
								  const u_int stride);
typedef TRANSFER_8TO16SUB *TRANSFER_8TO16SUB_PTR;

/* Our global function pointer - Initialized in xvid.c */
extern TRANSFER_8TO16SUB_PTR transfer_8to16sub;

/* Implemented functions */
TRANSFER_8TO16SUB transfer_8to16sub_h261_c;
TRANSFER_8TO16SUB transfer_8to16sub_h261_mmx;
TRANSFER_8TO16SUB transfer_8to16sub_h261_ia64;

/*****************************************************************************
 * transfer8to16 + substraction op API - Bidirectionnal Version
 ****************************************************************************/

typedef void (TRANSFER_8TO16SUB2) (short * const dct,
								   u_char * const cur,
								   const u_char * ref1,
								   const u_char * ref2,
								   const u_int stride);

typedef TRANSFER_8TO16SUB2 *TRANSFER_8TO16SUB2_PTR;

/* Our global function pointer - Initialized in xvid.c */
extern TRANSFER_8TO16SUB2_PTR transfer_8to16sub2;

/* Implemented functions */
TRANSFER_8TO16SUB2 transfer_8to16sub2_h261_c;
TRANSFER_8TO16SUB2 transfer_8to16sub2_h261_mmx;
TRANSFER_8TO16SUB2 transfer_8to16sub2_xmm;
TRANSFER_8TO16SUB2 transfer_8to16sub2_h261_ia64;


/*****************************************************************************
 * transfer16to8 + addition op API
 ****************************************************************************/

typedef void (TRANSFER_16TO8ADD) (u_char * const dst,
								  const short * const src,
								  u_int stride);

typedef TRANSFER_16TO8ADD *TRANSFER_16TO8ADD_PTR;

/* Our global function pointer - Initialized in xvid.c */
extern TRANSFER_16TO8ADD_PTR transfer_16to8add;

/* Implemented functions */
TRANSFER_16TO8ADD transfer_16to8add_h261_c;
TRANSFER_16TO8ADD transfer_16to8add_h261_mmx;
TRANSFER_16TO8ADD transfer_16to8add_h261_ia64;

/*****************************************************************************
 * transfer8to8 + no op
 ****************************************************************************/

typedef void (TRANSFER8X8_COPY) (u_char * const dst,
								 const u_char * const src,
								 const u_int stride);

typedef TRANSFER8X8_COPY *TRANSFER8X8_COPY_PTR;

/* Our global function pointer - Initialized in xvid.c */
extern TRANSFER8X8_COPY_PTR transfer8x8_copy;

/* Implemented functions */
TRANSFER8X8_COPY transfer8x8_copy_h261_c;
TRANSFER8X8_COPY transfer8x8_copy_h261_mmx;
TRANSFER8X8_COPY transfer8x8_copy_h261_ia64;

#ifdef __cplusplus
}
#endif
#endif
