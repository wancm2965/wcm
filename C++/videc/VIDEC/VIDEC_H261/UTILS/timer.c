/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Some timing functions to profile the library -
 *
 *  NB : not thread safe and only for debug purposes.
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
 * $Id: timer.c,v 1.1.1.1.2.1 2005/06/03 00:57:46 lixq Exp $
 *
 ****************************************************************************/

#include <stdio.h>
#include <time.h>
#include "timer.h"

#ifdef _PROFILING_
typedef unsigned __int64 uint64_t;
struct ts
{
	__int64 current;
	__int64 global;
	__int64 overall;
	__int64 dct;
	__int64 idct;
	__int64 quant;
	__int64 iquant;
	__int64 requant;
	__int64 motion;
	__int64 comp;
	__int64 prediction;
	__int64 coding;

};

struct ts tim;
uint64_t count_frames=0;
double frequency = 0.0;
FILE *fpLog=NULL;
/* 
    determine cpu frequency
	not very precise but sufficient
*/
static __inline __int64
read_counter()
{
	__int64 ts;
	unsigned int ts1, ts2;
	
	__asm {
		rdtsc
			mov ts1, eax
			mov ts2, edx
	}
	
	ts = ((unsigned __int64) ts2 << 32) | ((unsigned __int64) ts1);
	
	return ts;
}

double
get_freq(void)
{
/*	
    __int64 x, y;
	int i;

	i = time(NULL);

	while (i == time(NULL));

	x = read_counter();
	i++;

	while (i == time(NULL));

	y = read_counter();

	return (double) (y - x) / 1000.;
*/
	return 2400;
}

/* set everything to zero */
void
init_timer(void)
{
	frequency = get_freq();

	tim.dct = tim.quant = tim.idct = tim.iquant = tim.motion = tim.requant =
		tim.prediction=tim.coding =tim.comp= tim.global = tim.overall = 0;
}

void
start_timer(void)
{
	tim.current = read_counter();
}

void
start_global_timer(void)
{
	tim.global = read_counter();
}

void
stop_dct_timer(void)
{
	tim.dct += (read_counter() - tim.current);
}

void
stop_idct_timer(void)
{
	tim.idct += (read_counter() - tim.current);
}

void
stop_quant_timer(void)
{
	tim.quant += (read_counter() - tim.current);
}

void
stop_iquant_timer(void)
{
	tim.iquant += (read_counter() - tim.current);
}

void
stop_requant_timer(void)
{
	tim.requant += (read_counter() - tim.current);
}


void
stop_motion_timer(void)
{
	tim.motion += (read_counter() - tim.current);
}

void
stop_comp_timer(void)
{
	tim.comp += (read_counter() - tim.current);
}

void
stop_prediction_timer(void)
{
	tim.prediction += (read_counter() - tim.current);
}

void
stop_coding_timer(void)
{
	tim.coding += (read_counter() - tim.current);
}



void
stop_global_timer(void)
{
	tim.overall += (read_counter() - tim.global);
}
void end_timer(void)
{
	if(fpLog)
	{
		fclose(fpLog);
		fpLog=NULL;
	}

}
/*
    write log file with some timer information
*/
void
write_timer(void)
{
	float dct_per, quant_per, idct_per, iquant_per, requant_per,mot_per, comp_per;
	float  measured,pred_per, cod_per;
	__int64 sum_ticks = 0;

	count_frames++;
	if(NULL==fpLog)
		fpLog = fopen("encoder.log", "w+a");
	/* only write log file every 50 processed frames */
	if (fpLog ) 
	{
		
		
		dct_per =
			(float) (((float) ((float) tim.dct / (float) tim.overall)) *
					 100.0);
		quant_per =
			(float) (((float) ((float) tim.quant / (float) tim.overall)) *
					 100.0);
		idct_per =
			(float) (((float) ((float) tim.idct / (float) tim.overall)) *
					 100.0);
		iquant_per =
			(float) (((float) ((float) tim.iquant / (float) tim.overall)) *
					 100.0);
		requant_per =
			(float) (((float) ((float) tim.requant / (float) tim.overall)) *
			100.0);
		mot_per =
			(float) (((float) ((float) tim.motion / (float) tim.overall)) *
					 100.0);
		comp_per =
			(float) (((float) ((float) tim.comp / (float) tim.overall)) *
					 100.0);
	
		pred_per =
			(float) (((float) ((float) tim.prediction / (float) tim.overall)) *
					 100.0);
		cod_per =
			(float) (((float) ((float) tim.coding / (float) tim.overall)) *
					 100.0);
	
		sum_ticks =
			tim.coding +  tim.dct + tim.idct + tim.iquant + tim.motion + tim.quant + tim.prediction
				+tim.comp+tim.requant;

		measured =
			(float) (((float) ((float) sum_ticks / (float) tim.overall)) *
					 100.0);

		fprintf(fpLog,
				"\nnum:%3.0f\n"
				"Mot estimation:\nTotal time: %f ms (%3f percent of total encoding time)\n\n"
				"Prediction:\nTotal time: %f ms (%3f percent of total encoding time)\n\n"	
				"DCT:\nTotal time: %f ms (%3f percent of total encoding time)\n\n"
				"Quant:\nTotal time: %f ms (%3f percent of total encoding time)\n\n"
				"IDCT:\nTotal time: %f ms (%3f percent of total encoding time)\n\n"
				"IQuant:\nTotal time: %f ms (%3f percent of total encoding time)\n\n"
				"ReQuant:\nTotal time: %f ms (%3f percent of total encoding time)\n\n"
				"Mot compensation:\nTotal time: %f ms (%3f percent of total encoding time)\n\n"
				"Coding:\nTotal time: %f ms (%3f percent of total encoding time)\n\n\n"
				"Overall encoding time: %f ms, we measured %f ms (%3f percent)\n",
				(float) count_frames,
				(float) (tim.motion / frequency), mot_per,
				(float) (tim.prediction / frequency), pred_per,
				(float) (tim.dct / frequency), dct_per,
				(float) (tim.quant / frequency), quant_per,
				(float) (tim.idct / frequency), idct_per,
				(float) (tim.iquant / frequency), iquant_per,
				(float) (tim.requant / frequency), requant_per,
				(float) (tim.comp / frequency), comp_per,
				(float) (tim.coding / frequency), cod_per,
				(float) (tim.overall / frequency),
				(float) (sum_ticks / frequency), measured);

	
	}
}

#endif
