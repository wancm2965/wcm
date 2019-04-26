/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - emms wrapper API header -
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
 * $Id: emms.h,v 1.1.1.1.2.1 2005/06/03 05:48:10 lixq Exp $
 *
 ****************************************************************************/
#ifndef _EMMS_H_
#define _EMMS_H_
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * emms API
 ****************************************************************************/

 /**
 * \defgroup x86_grp x86 specific cpu flags
 * @{
*/

#define XVID_CPU_MMX      0x00000001 /**< use/has MMX instruction set */
#define XVID_CPU_MMXEXT   0x00000002 /**< use/has MMX-ext (pentium3) instruction set */
#define XVID_CPU_SSE      0x00000004 /**< use/has SSE (pentium3) instruction set */
#define XVID_CPU_SSE2     0x00000008 /**< use/has SSE2 (pentium4) instruction set */
#define XVID_CPU_3DNOW    0x00000010 /**< use/has 3dNOW (k6-2) instruction set */
#define XVID_CPU_3DNOWEXT 0x00000020 /**< use/has 3dNOW-ext (athlon) instruction set */
#define XVID_CPU_TSC      0x00000040 /**< has TimeStampCounter instruction */

/** @} */

typedef void (emmsFunc) (void);

typedef emmsFunc *emmsFuncPtr;

/* Our global function pointer - defined in emms.c */
extern emmsFuncPtr emms;

/* Implemented functions */

emmsFunc emms_h261_c;
emmsFunc emms_h261_mmx;

/*****************************************************************************
 * Prototypes
 ****************************************************************************/
extern int sigill_check_h261(void (*func)());
extern int check_cpu_features_h261(void);
extern void sse_os_trigger_h261(void);
extern void sse2_os_trigger_h261(void);
#ifdef __cplusplus
}
#endif
#endif
