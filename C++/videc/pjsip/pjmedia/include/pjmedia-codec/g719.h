/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// g719.h 

#ifndef __PJMEDIA_CODECS_G719_H__
#define __PJMEDIA_CODECS_G719_H__

/**
 * @file pjmedia-codec/g719.h
 * @brief G.719 codec.
 */

#include <pjmedia-codec/types.h>


PJ_BEGIN_DECL

/**
 * Initialize and register G.719 codec factory to pjmedia endpoint.
 *
 * @param endpt	    The pjmedia endpoint.
 *
 * @return	    PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_codec_g719_init( pjmedia_endpt *endpt );


/**
 * Enable and disable G.719 mode. By default, the standard modes are 
 * - sampling rate 48kHz, bitrate 24kbps, 32kbps, and 48kbps.
 * This function can also be used for enabling non-standard modes.
 * Note that currently only up to two non-standard modes can be enabled
 * at one time.
 *
 * @param sample_rate	PCM sampling rate, in Hz, valid values are only 
 *			48000.
 * @param bitrate	G.719 bitrate, in bps, the valid values are
 *			standard and non-standard bitrates as described 
 *			above.
 * @param enabled	PJ_TRUE for enabling specified mode.
 *
 * @return		PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_codec_g719_set_mode(unsigned sample_rate, unsigned bitrate, pj_bool_t enabled);

/**
 * Set the G.719 codec encoder and decoder level adjustment. 
 * If the value is non-zero, then PCM input samples to the encoder will 
 * be shifted right by this value, and similarly PCM output samples from
 * the decoder will be shifted left by this value.
 *
 * Default value is PJMEDIA_g719_DEFAULT_PCM_SHIFT.
 *
 * @param val		The value
 *
 * @return		PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_codec_g719_set_pcm_shift(int val);



/**
 * Unregister G.719 codecs factory from pjmedia endpoint.
 *
 * @return	    PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_codec_g719_deinit(void);


PJ_END_DECL


/**
 * @}
 */

#endif	/* __PJMEDIA_CODECS_G719_H__ */


