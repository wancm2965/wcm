
/* 
 * Copyright (C) 2009-2011 AVCON 
 *
 */

#include <pjmedia/echo.h>
#include <pjmedia/errno.h>
#include <pj/assert.h>
#include <pj/log.h>
#include <pj/pool.h>

#include "echo_internal.h"


// Create the Audec AEC. 
PJ_DEF(pj_status_t) avaec_create(pj_pool_t *pool,
				     unsigned clock_rate,
				     unsigned channel_count,
				     unsigned samples_per_frame,
				     unsigned tail_ms,
				     unsigned options,
				     void **p_echo )
{
	void * pAEC = NULL;
    *p_echo = pAEC;
    return PJ_SUCCESS;
}

// Destroy AEC
PJ_DEF(pj_status_t) avaec_destroy( void *state )
{
    return PJ_SUCCESS;
}

// Reset AEC , donothing
PJ_DEF(void) avaec_reset(void *state )
{
}

// Perform echo cancellation.
PJ_DEF(pj_status_t) avaec_cancel_echo( void *state,
					   pj_int16_t *rec_frm,
					   const pj_int16_t *play_frm,
					   unsigned options,
					   void *reserved )
{
	return 0;//hasEcho;
}

// Create the Audec AEC. 
PJ_DEF(pj_status_t) avaec_set_options(void *state,unsigned options)
{
	int nNS=((options>>8)&0xFF);
	int nAGC=((options>>16)&0xFF);
	int nAEC=((options>>24)&0xFF);
	int nVAD=(nAEC&0x7F);
	nAEC=(nAEC>>7);

    return PJ_SUCCESS;
}
