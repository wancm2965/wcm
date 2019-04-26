/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - h263 quantization header -
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
 * $Id: quant_h263.h,v 1.1.1.1.2.1 2005/06/03 05:48:10 lixq Exp $
 *
 ****************************************************************************/

#ifndef _QUANT_H263_H_
#define _QUANT_H263_H_

#include "../h261/config.h"
#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 * Prototypes
 ****************************************************************************/
/*pre quant*/
typedef void (prequanth263_Func) (short coeff[6][64],
										u_int* quant);

typedef prequanth263_Func *prequanth263_FuncPtr;
extern prequanth263_FuncPtr prequant_intra;
extern prequanth263_FuncPtr prequant_inter;
prequanth263_Func prequant_intra_h261_c;
prequanth263_Func prequant_inter_h261_c;

/* intra */
typedef void (quanth263_intraFunc) (short * coeff,
									const short * data,
									const u_int quant,
									const u_int dcscalar);
typedef quanth263_intraFunc *quanth263_intraFuncPtr;

extern quanth263_intraFuncPtr quant_intra;
extern quanth263_intraFuncPtr dequant_intra;

quanth263_intraFunc quant_intra_h261_c;
quanth263_intraFunc quant_intra_h261_mmx;
quanth263_intraFunc quant_intra_h261_sse2;
quanth263_intraFunc quant_intra_h261_ia64;

quanth263_intraFunc dequant_intra_h261_c;
quanth263_intraFunc dequant_intra_h261_mmx;
quanth263_intraFunc dequant_intra_h261_xmm;
quanth263_intraFunc dequant_intra_h261_sse2;
quanth263_intraFunc dequant_intra_h261_ia64;





typedef u_int(quanth263_interFunc) (short * coeff,
									   const short * data,
									   const u_int quant);

typedef quanth263_interFunc *quanth263_interFuncPtr;

extern quanth263_interFuncPtr quant_inter;
quanth263_interFunc quant_inter_h261_c;
quanth263_interFunc quant_inter_h261_mmx;
quanth263_interFunc quant_inter_h261_sse2;
quanth263_interFunc quant_inter_h261_ia64;

/*inter_dequant */
typedef void (dequanth263_interFunc) (short * coeff,
									  const short * data,
									  const u_int quant);

typedef dequanth263_interFunc *dequanth263_interFuncPtr;

extern dequanth263_interFuncPtr dequant_inter;

dequanth263_interFunc dequant_inter_h261_c;
dequanth263_interFunc dequant_inter_h261_mmx;
dequanth263_interFunc dequant_inter_h261_xmm;
dequanth263_interFunc dequant_inter_h261_sse2;
dequanth263_interFunc dequant_inter_h261_ia64;
#ifdef __cplusplus
}
#endif
#endif /* _QUANT_H263_H_ */
