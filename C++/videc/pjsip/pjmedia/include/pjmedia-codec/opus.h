/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// opus.h 

#ifndef __PJMEDIA_CODECS_OPUS_H__
#define __PJMEDIA_CODECS_OPUS_H__

/**
 * @file pjmedia-codec/opus.h
 * @brief OPUS codec.
 */

#include <pjmedia-codec/types.h>


PJ_BEGIN_DECL

/**
 * Initialize and register OPUS codec factory to pjmedia endpoint.
 *
 * @param endpt	    The pjmedia endpoint.
 *
 * @return	    PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_codec_opus_init( pjmedia_endpt *endpt );


/**
 * Enable and disable OPUS mode. By default, the standard modes are 
 * enabled upon initialization, i.e.:
 * - sampling rate 16kHz, bitrate 24kbps and 32kbps.
 * - sampling rate 32kHz, bitrate 24kbps, 32kbps, and 48kbps.
 * This function can also be used for enabling non-standard modes.
 * Note that currently only up to two non-standard modes can be enabled
 * at one time.
 *
 * @param sample_rate	PCM sampling rate, in Hz, valid values are only 
 *			16000 and 32000.
 * @param bitrate	G722.1 bitrate, in bps, the valid values are
 *			standard and non-standard bitrates as described 
 *			above.
 * @param enabled	PJ_TRUE for enabling specified mode.
 *
 * @return		PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_codec_opus_set_mode(unsigned sample_rate, unsigned bitrate, pj_bool_t enabled);

/**
 * Set the OPUS codec encoder and decoder level adjustment. 
 * If the value is non-zero, then PCM input samples to the encoder will 
 * be shifted right by this value, and similarly PCM output samples from
 * the decoder will be shifted left by this value.
 *
 * Default value is PJMEDIA_opus_DEFAULT_PCM_SHIFT.
 *
 * @param val		The value
 *
 * @return		PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_codec_opus_set_pcm_shift(int val);



/**
 * Unregister OPUS codecs factory from pjmedia endpoint.
 *
 * @return	    PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_codec_opus_deinit(void);


PJ_END_DECL


/**
 * @}
 */

#endif	/* __PJMEDIA_CODECS_OPUS_H__ */


