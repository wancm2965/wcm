/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// g719.c
#include <pjmedia-codec/g719.h>
#include <pjmedia/codec.h>
#include <pjmedia/errno.h>
#include <pjmedia/endpoint.h>
#include <pjmedia/port.h>
#include <pjmedia/silencedet.h>
#include <pj/assert.h>
#include <pj/log.h>
#include <pj/math.h>
#include <pj/pool.h>
#include <pj/string.h>
#include <pj/os.h>

#include <pjmedia/plc.h>
#define PLC_DISABLED	1


/*
* Only build this file if PJMEDIA_HAS_G719_CODEC != 0
*/
#if defined(PJMEDIA_HAS_G719_CODEC) && PJMEDIA_HAS_G719_CODEC!=0

#include "../../../../AUDEC_core/AUDEC_g719/include/g719api.h"

#define THIS_FILE	    "g719.c"

/* Codec tag, it is the SDP encoding name and also MIME subtype name */
#define CODEC_TAG	    "g719"

/* Sampling rates definition */
#define FWB_SAMPLE_RATE	    48000 

/* Maximum number of samples per frame. */
#define MAX_SAMPLES_PER_FRAME (FWB_SAMPLE_RATE * 20 / 1000)

/* Maximum number of codec params. */
#define MAX_CODEC_MODES	    8
#define START_RSV_MODES_IDX 6


/* Prototypes for g719 codec factory */
static pj_status_t test_alloc( pjmedia_codec_factory *factory, 
							  const pjmedia_codec_info *id );
static pj_status_t default_attr( pjmedia_codec_factory *factory, 
								const pjmedia_codec_info *id, 
								pjmedia_codec_param *attr );
static pj_status_t enum_codecs( pjmedia_codec_factory *factory, 
							   unsigned *count, 
							   pjmedia_codec_info codecs[]);
static pj_status_t alloc_codec( pjmedia_codec_factory *factory, 
							   const pjmedia_codec_info *id, 
							   pjmedia_codec **p_codec);
static pj_status_t dealloc_codec( pjmedia_codec_factory *factory, 
								 pjmedia_codec *codec );

/* Prototypes for g719 codec implementation. */
static pj_status_t codec_init( pjmedia_codec *codec, 
							  pj_pool_t *pool );
static pj_status_t codec_open( pjmedia_codec *codec, 
							  pjmedia_codec_param *attr );
static pj_status_t codec_close( pjmedia_codec *codec );
static pj_status_t codec_modify(pjmedia_codec *codec, 
								const pjmedia_codec_param *attr );
static pj_status_t codec_parse( pjmedia_codec *codec,
							   void *pkt,
							   pj_size_t pkt_size,
							   const pj_timestamp *ts,
							   unsigned *frame_cnt,
							   pjmedia_frame frames[]);
static pj_status_t codec_encode( pjmedia_codec *codec, 
								const struct pjmedia_frame *input,
								unsigned output_buf_len,
struct pjmedia_frame *output);
static pj_status_t codec_decode( pjmedia_codec *codec,
								const struct pjmedia_frame *input,
								unsigned output_buf_len, 
struct pjmedia_frame *output);


static pj_status_t codec_recover( pjmedia_codec *codec, 
								 unsigned output_buf_len, 
struct pjmedia_frame *output);


/* Definition for g719 codec operations. */
static pjmedia_codec_op codec_op = 
{
	&codec_init,
	&codec_open,
	&codec_close,
	&codec_modify,
	&codec_parse,
	&codec_encode,
	&codec_decode,
	&codec_recover

};

/* Definition for g719 codec factory operations. */
static pjmedia_codec_factory_op codec_factory_op =
{
	&test_alloc,
	&default_attr,
	&enum_codecs,
	&alloc_codec,
	&dealloc_codec,
	&pjmedia_codec_g719_deinit //liurui add for 2.0
};


/* Structure of g719 mode */
typedef struct codec_mode
{
	pj_bool_t	     enabled;		/* Is this mode enabled?	    */
	pj_uint8_t	     pt;		/* Payload type.		    */
	unsigned	     sample_rate;	/* Default sampling rate to be used.*/
	unsigned	     bitrate;		/* Bitrate.			    */
	char	     bitrate_str[8];	/* Bitrate in string.		    */
} codec_mode;


/* g719 codec factory */
static struct codec_factory {
	pjmedia_codec_factory    base;	    /**< Base class.		    */
	pjmedia_endpt	    *endpt;	    /**< PJMEDIA endpoint instance. */
	pj_pool_t		    *pool;	    /**< Codec factory pool.	    */
	pj_mutex_t		    *mutex;	    /**< Codec factory mutex.	    */

	unsigned		     mode_count;    /**< Number of g719 modes.    */
	codec_mode		     modes[MAX_CODEC_MODES]; /**< The g719 modes. */
	unsigned		     mode_rsv_start;/**< Start index of g719 non-
										standard modes, currently
										there can only be up to two 
										non-standard modes enabled
										at the same time.	    */
} codec_factory;

/* g719 codec private data. */
typedef struct codec_private {
	pj_pool_t		*pool;		    /**< Pool for each instance.    */
	pj_bool_t		 plc_enabled;	    /**< PLC enabled?		    */
	pj_bool_t		 vad_enabled;	    /**< VAD enabled?		    */
	pjmedia_silence_det	*vad;		    /**< PJMEDIA VAD instance.	    */
	pj_timestamp	 last_tx;	    /**< Timestamp of last transmit.*/

	/* ITU ref implementation seems to leave the codec engine states to be
	* managed by the application, so here we go.
	*/

	/* Common engine state */
	pj_uint16_t		 samples_per_frame; /**< Samples per frame.	    */
	pj_uint16_t		 bitrate;	    /**< Coded stream bitrate.	    */
	pj_uint16_t		 frame_size;	    /**< Coded frame size.	    */
	pj_uint16_t		 frame_size_bits;   /**< Coded frame size in bits.  */

	/* Encoder specific state */
	void*	enc_st;

	/* Decoder specific state */
	void*	dec_st;

#if !PLC_DISABLED
    pjmedia_plc		*plc;
#endif

} codec_private_t;

/* 
* Helper function for looking up mode based on payload type.
*/
static codec_mode* lookup_mode(unsigned pt)
{
	codec_mode* mode = NULL;
	unsigned i;

	for (i = 0; i < codec_factory.mode_count; ++i) {
		mode = &codec_factory.modes[i];
		if (mode->pt == pt)
			break;
	}

	return mode;
}

/* 
* Helper function to validate g719 mode. Valid modes are defined as:
* 1. sample rate must be 48kHz,
*/
static pj_bool_t validate_mode(unsigned sample_rate, unsigned bitrate)
{
	if (sample_rate == FWB_SAMPLE_RATE) 
	{
		if (bitrate < 24000 || bitrate > 96000)
		{
			return PJ_FALSE;
		}
	} 
	else 
	{
		return PJ_FALSE;
	}

	return PJ_TRUE;
}

#if defined(PJ_IS_LITTLE_ENDIAN) && PJ_IS_LITTLE_ENDIAN!=0
PJ_INLINE(void) swap_bytes(pj_uint16_t *buf, unsigned count)
{
	pj_uint16_t *end = buf + count;
	while (buf != end) {
		*buf = (pj_uint16_t)((*buf << 8) | (*buf >> 8));
		++buf;
	}
}
#else
#define swap_bytes(buf, count)
#endif

/*
* Initialize and register g719 codec factory to pjmedia endpoint.
*/
PJ_DEF(pj_status_t) pjmedia_codec_g719_init( pjmedia_endpt *endpt )
{
	pjmedia_codec_mgr *codec_mgr;
	codec_mode *mode;
	pj_status_t status;

	if (codec_factory.pool != NULL) {
		/* Already initialized. */
		return PJ_SUCCESS;
	}

	/* Initialize codec modes, by default all standard bitrates are enabled */
	codec_factory.mode_count = 0;

	mode = &codec_factory.modes[codec_factory.mode_count++];
	mode->enabled = PJ_TRUE;
	mode->pt = PJMEDIA_RTP_PT_G719_32;
	mode->sample_rate = FWB_SAMPLE_RATE;
	mode->bitrate = 32000;
	pj_utoa(mode->bitrate, mode->bitrate_str);

	mode = &codec_factory.modes[codec_factory.mode_count++];
	mode->enabled = PJ_TRUE;
	mode->pt = PJMEDIA_RTP_PT_G719_48;
	mode->sample_rate = FWB_SAMPLE_RATE;
	mode->bitrate = 48000;
	pj_utoa(mode->bitrate, mode->bitrate_str);

	mode = &codec_factory.modes[codec_factory.mode_count++];
	mode->enabled = PJ_TRUE;
	mode->pt = PJMEDIA_RTP_PT_G719_64;
	mode->sample_rate = FWB_SAMPLE_RATE;
	mode->bitrate = 64000;
	pj_utoa(mode->bitrate, mode->bitrate_str);

	pj_assert(codec_factory.mode_count <= MAX_CODEC_MODES);

	/* Create g719 codec factory. */
	codec_factory.base.op = &codec_factory_op;
	codec_factory.base.factory_data = NULL;
	codec_factory.endpt = endpt;

	codec_factory.pool = pjmedia_endpt_create_pool(endpt, "g719 codec",
		4000, 4000);
	if (!codec_factory.pool)
		return PJ_ENOMEM;

	/* Create mutex. */
	status = pj_mutex_create_simple(codec_factory.pool, "g719 codec",
		&codec_factory.mutex);
	if (status != PJ_SUCCESS)
		goto on_error;

	/* Get the codec manager. */
	codec_mgr = pjmedia_endpt_get_codec_mgr(endpt);
	if (!codec_mgr) {
		status = PJ_EINVALIDOP;
		goto on_error;
	}

	/* Register codec factory to endpoint. */
	status = pjmedia_codec_mgr_register_factory(codec_mgr, 
		&codec_factory.base);
	if (status != PJ_SUCCESS)
		goto on_error;

	/* Done. */
	return PJ_SUCCESS;

on_error:
	if (codec_factory.mutex) {
		pj_mutex_destroy(codec_factory.mutex);
		codec_factory.mutex = NULL;
	}

	pj_pool_release(codec_factory.pool);
	codec_factory.pool = NULL;
	return status;
}


/**
* Enable and disable g719 modes, including non-standard modes.
*/
PJ_DEF(pj_status_t) pjmedia_codec_g719_set_mode(unsigned sample_rate, 
												unsigned bitrate, 
												pj_bool_t enabled)
{
	unsigned i;

	/* Validate mode */
	if (!validate_mode(sample_rate, bitrate))
		return PJMEDIA_CODEC_EINMODE;

	/* Look up in factory modes table */
	for (i = 0; i < codec_factory.mode_count; ++i) {
		if (codec_factory.modes[i].sample_rate == sample_rate && codec_factory.modes[i].bitrate == bitrate)
		{
			codec_factory.modes[i].enabled = enabled;
			return PJ_SUCCESS;
		}
	}

	/* Mode not found in modes table, this may be a request to enable
	* a non-standard g719 mode.
	*/

	/* Non-standard mode need to be initialized first before user 
	* can disable it.
	*/
	if (!enabled)
		return PJ_ENOTFOUND;

	/* Initialize a non-standard mode, look for available space. */
	for (i = codec_factory.mode_rsv_start; i < codec_factory.mode_count; ++i) 
	{
		if (!codec_factory.modes[i].enabled)
		{
			codec_mode *mode = &codec_factory.modes[i];
			mode->enabled = PJ_TRUE;
			mode->sample_rate = sample_rate;
			mode->bitrate = bitrate;
			pj_utoa(mode->bitrate, mode->bitrate_str);

			return PJ_SUCCESS;
		}
	}

	/* No space for non-standard mode. */
	return PJ_ETOOMANY;
}


/*
* Unregister g719 codec factory from pjmedia endpoint.
*/
PJ_DEF(pj_status_t) pjmedia_codec_g719_deinit(void)
{
	pjmedia_codec_mgr *codec_mgr;
	pj_status_t status;

	if (codec_factory.pool == NULL) {
		/* Already deinitialized */
		return PJ_SUCCESS;
	}

	pj_mutex_lock(codec_factory.mutex);

	/* Get the codec manager. */
	codec_mgr = pjmedia_endpt_get_codec_mgr(codec_factory.endpt);
	if (!codec_mgr) {
		pj_pool_release(codec_factory.pool);
		codec_factory.pool = NULL;
		return PJ_EINVALIDOP;
	}

	/* Unregister g719 codec factory. */
	status = pjmedia_codec_mgr_unregister_factory(codec_mgr,
		&codec_factory.base);

	/* Destroy mutex. */
	pj_mutex_destroy(codec_factory.mutex);

	/* Destroy pool. */
	pj_pool_release(codec_factory.pool);
	codec_factory.pool = NULL;

	return status;
}

/* 
* Check if factory can allocate the specified codec. 
*/
static pj_status_t test_alloc( pjmedia_codec_factory *factory, 
							  const pjmedia_codec_info *info )
{
	PJ_UNUSED_ARG(factory);

	/* Type MUST be audio. */
	if (info->type != PJMEDIA_TYPE_AUDIO)
		return PJMEDIA_CODEC_EUNSUP;

	/* Check encoding name. */
	if (pj_stricmp2(&info->encoding_name, CODEC_TAG) != 0)
		return PJMEDIA_CODEC_EUNSUP;

	/* Check clock-rate */
	if (info->clock_rate != FWB_SAMPLE_RATE)
	{
		return PJMEDIA_CODEC_EUNSUP;
	}

	return PJ_SUCCESS;
}

/*
* Generate default attribute.
*/
static pj_status_t default_attr ( pjmedia_codec_factory *factory, 
								 const pjmedia_codec_info *id, 
								 pjmedia_codec_param *attr )
{
	codec_mode *mode;

	PJ_ASSERT_RETURN(factory==&codec_factory.base, PJ_EINVAL);

	pj_bzero(attr, sizeof(pjmedia_codec_param));

	mode = lookup_mode(id->pt);
	if (mode == NULL || !mode->enabled)
		return PJMEDIA_CODEC_EUNSUP;

	attr->info.pt = (pj_uint8_t)id->pt;
	attr->info.channel_cnt = 1;
	attr->info.clock_rate = mode->sample_rate;
	attr->info.max_bps = mode->bitrate;
	attr->info.avg_bps = mode->bitrate;
	attr->info.pcm_bits_per_sample = 16;
	attr->info.frm_ptime = 20;

	/* Default flags. */
	attr->setting.plc = 1;
	attr->setting.vad = 0;
	attr->setting.frm_per_pkt = 1;

	/* Default FMTP setting */
	attr->setting.dec_fmtp.cnt = 1;
	attr->setting.dec_fmtp.param[0].name = pj_str("bitrate");
	attr->setting.dec_fmtp.param[0].val = pj_str(mode->bitrate_str);

	return PJ_SUCCESS;
}

/*
* Enum codecs supported by this factory.
*/
static pj_status_t enum_codecs( pjmedia_codec_factory *factory, 
							   unsigned *count, 
							   pjmedia_codec_info codecs[])
{
	unsigned i, max_cnt;

	PJ_ASSERT_RETURN(factory==&codec_factory.base, PJ_EINVAL);
	PJ_ASSERT_RETURN(codecs && *count > 0, PJ_EINVAL);

	max_cnt = *count;
	*count = 0;

	for (i=0; (i < codec_factory.mode_count) && (*count < max_cnt); ++i)
	{
		if (!codec_factory.modes[i].enabled)
			continue;

		pj_bzero(&codecs[*count], sizeof(pjmedia_codec_info));
		codecs[*count].encoding_name = pj_str((char*)CODEC_TAG);
		codecs[*count].pt = codec_factory.modes[i].pt;
		codecs[*count].type = PJMEDIA_TYPE_AUDIO;
		codecs[*count].clock_rate = codec_factory.modes[i].sample_rate;
		codecs[*count].channel_cnt = 1;

		++ *count;
	}

	return PJ_SUCCESS;
}

/*
* Allocate a new codec instance.
*/
static pj_status_t alloc_codec( pjmedia_codec_factory *factory, 
							   const pjmedia_codec_info *id,
							   pjmedia_codec **p_codec)
{
	codec_private_t *codec_data;
	pjmedia_codec *codec;
	pj_pool_t *pool;
	pj_status_t status;

	PJ_ASSERT_RETURN(factory && id && p_codec, PJ_EINVAL);
	PJ_ASSERT_RETURN(factory == &codec_factory.base, PJ_EINVAL);

	pj_mutex_lock(codec_factory.mutex);

	/* Create pool for codec instance */
	pool = pjmedia_endpt_create_pool(codec_factory.endpt, "g719", 512, 512);
	codec = PJ_POOL_ZALLOC_T(pool, pjmedia_codec);
	codec->op = &codec_op;
	codec->factory = factory;
	codec->codec_data = PJ_POOL_ZALLOC_T(pool, codec_private_t);
	codec_data = (codec_private_t*) codec->codec_data;
	codec_data->pool = pool;

	/* Create silence detector */
	status = pjmedia_silence_det_create(pool, id->clock_rate, 
		id->clock_rate * 20 / 1000,
		&codec_data->vad);
	if (status != PJ_SUCCESS) {
		pj_mutex_unlock(codec_factory.mutex);
		return status;
	}

#if !PLC_DISABLED
	/* Create PLC, always with 10ms ptime */
	status = pjmedia_plc_create(pool, 48000, 960, 0, &codec_data->plc);
	if (status != PJ_SUCCESS) {
	    pj_mutex_unlock(codec_factory.mutex);
	    return status;
	}
#endif

	pj_mutex_unlock(codec_factory.mutex);

	*p_codec = codec;
	return PJ_SUCCESS;
}

/*
* Free codec.
*/
static pj_status_t dealloc_codec( pjmedia_codec_factory *factory, 
								 pjmedia_codec *codec )
{
	codec_private_t *codec_data;
	pj_pool_t *pool;

	PJ_ASSERT_RETURN(factory && codec, PJ_EINVAL);
	PJ_ASSERT_RETURN(factory == &codec_factory.base, PJ_EINVAL);

	/* Close codec, if it's not closed. */
	codec_data = (codec_private_t*) codec->codec_data;

	pjmedia_silence_det_destroy(codec_data->vad);

#if !PLC_DISABLED
    /* Clear left samples in the PLC, since codec+plc will be reused
     * next time.
     */
	for (i=0; i<2; ++i) 
	{
		pj_int16_t frame[960];
		pjmedia_zero_samples(frame, PJ_ARRAY_SIZE(frame));
		pjmedia_plc_save(codec_data->plc, frame);
	}
#endif

	pool = codec_data->pool;
	codec_close(codec);

	/* Release codec pool */
	pj_pool_release(pool);

	return PJ_SUCCESS;
}

/*
* Init codec.
*/
static pj_status_t codec_init( pjmedia_codec *codec, 
							  pj_pool_t *pool )
{
	PJ_UNUSED_ARG(codec);
	PJ_UNUSED_ARG(pool);
	return PJ_SUCCESS;
}

/*
* Open codec.
*/
static pj_status_t codec_open( pjmedia_codec *codec, 
							  pjmedia_codec_param *attr )
{
	codec_private_t *codec_data = (codec_private_t*) codec->codec_data;
	pj_pool_t *pool;

	/* Validation mode first! */
	if (!validate_mode(attr->info.clock_rate, attr->info.avg_bps))
		return PJMEDIA_CODEC_EINMODE;

	pool = codec_data->pool;

	/* Initialize common state */
	codec_data->vad_enabled = (attr->setting.vad != 0);
	codec_data->plc_enabled = (attr->setting.plc != 0);

	codec_data->bitrate = (pj_uint16_t)attr->info.avg_bps;
	codec_data->frame_size_bits = (pj_uint16_t)(attr->info.avg_bps*20/1000);
	codec_data->frame_size = (pj_uint16_t)(codec_data->frame_size_bits>>3);
	codec_data->samples_per_frame = (pj_uint16_t)(attr->info.clock_rate*20/1000);

	/* Initialize encoder state */
	codec_data->enc_st= OpenG719Encoder(codec_data->bitrate, codec_data->samples_per_frame);

	/* Initialize decoder state */
	codec_data->dec_st= OpenG719Decoder(codec_data->bitrate, codec_data->samples_per_frame);

	return PJ_SUCCESS;
}

/*
* Close codec.
*/
static pj_status_t codec_close( pjmedia_codec *codec )
{
	codec_private_t *codec_data = (codec_private_t*) codec->codec_data;

	if (codec_data->enc_st!=NULL)
	{
		CloseG719Encoder(codec_data->enc_st);
		codec_data->enc_st=NULL;
	}
	if (codec_data->dec_st!=NULL)
	{
		CloseG719Decoder(codec_data->dec_st);
		codec_data->dec_st=NULL;
	}

	return PJ_SUCCESS;
}


/*
* Modify codec settings.
*/
static pj_status_t codec_modify( pjmedia_codec *codec, 
								const pjmedia_codec_param *attr )
{
	codec_private_t *codec_data = (codec_private_t*) codec->codec_data;

	codec_data->vad_enabled = (attr->setting.vad != 0);
    codec_data->plc_enabled = (attr->setting.plc != 0);

	return PJ_SUCCESS;
}

/*
* Get frames in the packet.
*/
static pj_status_t codec_parse( pjmedia_codec *codec,
							   void *pkt,
							   pj_size_t pkt_size,
							   const pj_timestamp *ts,
							   unsigned *frame_cnt,
							   pjmedia_frame frames[])
{
	codec_private_t *codec_data = (codec_private_t*) codec->codec_data;
	unsigned count = 0;

	PJ_ASSERT_RETURN(frame_cnt, PJ_EINVAL);

	//	frames[count].type = PJMEDIA_FRAME_TYPE_AUDIO;
	//	frames[count].buf = pkt;
	//	frames[count].size = pkt_size;//codec_data->frame_size;
	//	frames[count].timestamp.u64 = ts->u64 + count * codec_data->samples_per_frame;
	//	pkt = (pj_uint8_t*)pkt + pkt_size;
	//	++count;
	//*frame_cnt = count;

	/* Parse based on fixed frame size. */
	while (pkt_size >= codec_data->frame_size && count < *frame_cnt) {
		frames[count].type = PJMEDIA_FRAME_TYPE_AUDIO;
		frames[count].buf = pkt;
		frames[count].size = codec_data->frame_size;
		frames[count].timestamp.u64 = ts->u64 + 
			count * codec_data->samples_per_frame;

		pkt = (pj_uint8_t*)pkt + codec_data->frame_size;
		pkt_size -= codec_data->frame_size;

		++count;
	}

	pj_assert(pkt_size == 0);
	*frame_cnt = count;

	return PJ_SUCCESS;
}

/*
* Encode frames.
*/
static pj_status_t codec_encode( pjmedia_codec *codec, 
								const struct pjmedia_frame *input,
								unsigned output_buf_len, 
struct pjmedia_frame *output)
{
	codec_private_t *codec_data = (codec_private_t*) codec->codec_data;

	/* Check frame in & out size */
	PJ_ASSERT_RETURN((pj_uint16_t)input->size == 
		(codec_data->samples_per_frame<<1),
		PJMEDIA_CODEC_EPCMTOOSHORT);
	PJ_ASSERT_RETURN(output_buf_len >= codec_data->frame_size,
		PJMEDIA_CODEC_EFRMTOOSHORT);

	/* Apply silence detection if VAD is enabled */
	if (codec_data->vad_enabled) 
	{
		pj_bool_t is_silence;
		pj_int32_t silence_duration;

		pj_assert(codec_data->vad);

		silence_duration = pj_timestamp_diff32(&codec_data->last_tx, &input->timestamp);

		is_silence = pjmedia_silence_det_detect(codec_data->vad, 
			(pj_int16_t*) input->buf,
			(input->size >> 1),
			NULL);
		if (is_silence) //&& (PJMEDIA_CODEC_MAX_SILENCE_PERIOD == -1 || 
			//silence_duration < (PJMEDIA_CODEC_MAX_SILENCE_PERIOD * (int)320 / 20)))
			//silence_duration < (PJMEDIA_CODEC_MAX_SILENCE_PERIOD * (int)codec_data->samples_per_frame / 20)))
		{
			output->type = PJMEDIA_FRAME_TYPE_NONE;
			output->buf = NULL;
			output->size = 0;
			output->timestamp = input->timestamp;
			return PJ_SUCCESS;
		} 
		else 
		{
			codec_data->last_tx = input->timestamp;
		}
	}

	codec_data->frame_size = G719EncoderFrame(codec_data->enc_st, input->buf, output->buf);

	output->type = PJMEDIA_FRAME_TYPE_AUDIO;
	output->size = codec_data->frame_size;
	output->timestamp = input->timestamp;

	return PJ_SUCCESS;
}

/*
* Decode frame.
*/
static pj_status_t codec_decode( pjmedia_codec *codec, 
								const struct pjmedia_frame *input,
								unsigned output_buf_len, 
struct pjmedia_frame *output)
{
	codec_private_t *codec_data = (codec_private_t*) codec->codec_data;

	/* Check frame out length size */
	PJ_ASSERT_RETURN(output_buf_len >= 
		(unsigned)(codec_data->samples_per_frame<<1),
		PJMEDIA_CODEC_EPCMTOOSHORT);

	/* If input is NULL, perform PLC by settting frame_error_flag to 1 */
	if (input) 
	{
		/* Check frame in length size */
		//PJ_ASSERT_RETURN((pj_uint16_t)input->size == codec_data->frame_size,
		//	PJMEDIA_CODEC_EFRMINLEN);
		G719DecoderFrame(codec_data->dec_st, input->buf, output->buf, 0);
		output->timestamp = input->timestamp;
	} 
	else 
	{
		unsigned char tmpBuf[240] ={0};
		G719DecoderFrame(codec_data->dec_st, tmpBuf, output->buf, 1);
	}

	output->type = PJMEDIA_FRAME_TYPE_AUDIO;
	output->size = codec_data->samples_per_frame << 1;

#if !PLC_DISABLED
    if (codec_data->plc_enabled)
		pjmedia_plc_save( codec_data->plc, (pj_int16_t*)output->buf);
#endif

	return PJ_SUCCESS;
}

/* 
* Recover lost frame.
*/
#if !PLC_DISABLED
static pj_status_t codec_recover( pjmedia_codec *codec, unsigned output_buf_len, struct pjmedia_frame *output)
{
	codec_private_t *codec_data = (codec_private_t*) codec->codec_data;

    if (!codec_data->plc_enabled)
		return PJ_EINVALIDOP;

    PJ_ASSERT_RETURN(output_buf_len >= codec_data->samples_per_frame * 2, 
		     PJMEDIA_CODEC_EPCMTOOSHORT);

    pjmedia_plc_generate(codec_data->plc, (pj_int16_t*)output->buf);
	/* Otherwise just return zero-fill frame. */
	output->type = PJMEDIA_FRAME_TYPE_AUDIO;
	output->size = codec_data->samples_per_frame << 1;

	return PJ_SUCCESS;
}
#else


static pj_status_t codec_recover( pjmedia_codec *codec, 
								 unsigned output_buf_len, 
struct pjmedia_frame *output)
{
	codec_private_t *codec_data = (codec_private_t*) codec->codec_data;

	/* Use native PLC when PLC is enabled. */
	//if (codec_data->plc_enabled)
	return codec_decode(codec, NULL, output_buf_len, output);

	/* Otherwise just return zero-fill frame. */
	output->type = PJMEDIA_FRAME_TYPE_AUDIO;
	output->size = codec_data->samples_per_frame << 1;

	pjmedia_zero_samples((pj_int16_t*)output->buf, 
		codec_data->samples_per_frame);

	return PJ_SUCCESS;
}


#endif

#endif	/* PJMEDIA_HAS_G719_CODEC */
