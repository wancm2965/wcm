///////////////////////////////////////////////////////////////////////////////////////////////////
//

#include <pjmedia.h>
#include <pjlib.h>

#include "pjmedia_dtmf_decoder.h"
#include "dsp.h"


#define THIS_FILE   __FILE__
#define SIGNATURE   PJMEDIA_SIG_CLASS_PORT_AUD('D','D')

//FILE * outTestFile = NULL;


struct dtmf_decoder_port 
{
    pjmedia_port         base;
    unsigned             samples_per_frame;

    pj_size_t            bufsize;
    char                 *buf;

    digit_detect_state_t digit_detect_state;
    int                  last_number;

	void	    *user_data; //liurui

    pj_status_t          (*cb)(pjmedia_port*, void *user_data, int);
};


/*
 * Put a frame into the buffer.
 */
static pj_status_t dtmf_put_frame(pjmedia_port *this_port, pjmedia_frame *frame)
{
	pj_status_t status = PJ_SUCCESS;
    struct dtmf_decoder_port *dport = (struct dtmf_decoder_port *)this_port;

    if (PJMEDIA_FRAME_TYPE_AUDIO == frame->type) 
	{
        dtmf_detect(&dport->digit_detect_state, (short *)frame->buf, dport->samples_per_frame, DSP_DIGITMODE_NOQUELCH); 
		//if( outTestFile != NULL )
		//	fwrite(frame->buf, sizeof(short), dport->samples_per_frame, outTestFile);
        if(NULL != dport->cb) 
		{
			int nIsVaildDigit = 0;	
            //if (dport->last_number < dport->digit_detect_state.detected_digits) 
			if( dport->digit_detect_state.detected_digits == 1 )
			{
				int digit;
                //dport->last_number = dport->digit_detect_state.detected_digits;
				//digit = (int)dport->digit_detect_state.digits[dport->last_number-1];

                digit = (int)dport->digit_detect_state.digits[0];	
				dport->digit_detect_state.detected_digits = 0;

				if( digit > 47 && digit < 58 )
					nIsVaildDigit = 1;
				else if( digit > 64 && digit < 69 )
					nIsVaildDigit = 1;
				else if(digit == 35 || digit == 42)
					nIsVaildDigit = 1;
				if( nIsVaildDigit == 0 )
					return PJ_SUCCESS;

                status = (dport->cb) (this_port, dport->user_data, digit);
                return status;
            }
        }
    }
    return PJ_SUCCESS;
}


/*
 * Get frame, basicy is a no-op operation.
 */
static pj_status_t dtmf_get_frame(pjmedia_port *this_port, pjmedia_frame *frame)
{
    PJ_UNUSED_ARG(this_port);
    PJ_UNUSED_ARG(frame);

    return PJ_EINVALIDOP;
}


/*
 * Close the port.
 */
static pj_status_t dtmf_on_destroy(pjmedia_port *this_port)
{
    struct dtmf_decoder_port *dport = (struct dtmf_decoder_port *)this_port;
    
    PJ_LOG(4,(THIS_FILE, "Inband DTMF detector '%.*s' destroy", 
             (int)dport->base.info.name.slen, dport->base.info.name.ptr));

	//fclose(outTestFile);

    return PJ_SUCCESS;
}


/*
 * Create dtmf decoder port.
 */
PJ_DEF(pj_status_t) pjmedia_dtmf_decoder_create(pj_pool_t *pool,
                                                const char *name,
                                                unsigned sampling_rate,
                                                unsigned samples_per_frame,
                                                pj_ssize_t buff_size,
                                                pjmedia_port **p_port)
{
    struct dtmf_decoder_port *dport;
    pj_str_t pjname;
    PJ_ASSERT_RETURN(pool && name && p_port, PJ_EINVAL);

    dport = PJ_POOL_ZALLOC_T(pool, struct dtmf_decoder_port);
    PJ_ASSERT_RETURN(dport != NULL, PJ_ENOMEM);

    pj_strdup2(pool, &pjname, name);
    pjmedia_port_info_init(&dport->base.info, &pjname, SIGNATURE,
                           sampling_rate, 1, 16, samples_per_frame);
    ast_digit_detect_init(&dport->digit_detect_state, 1, sampling_rate);
    dport->base.get_frame = &dtmf_get_frame;
    dport->base.put_frame = &dtmf_put_frame;
    dport->base.on_destroy = &dtmf_on_destroy;
    dport->samples_per_frame = samples_per_frame;

    if (1 > buff_size) 
        buff_size = PJMEDIA_FILE_PORT_BUFSIZE;
    dport->bufsize = buff_size;

    pj_assert(dport->bufsize >= PJMEDIA_PIA_AVG_FSZ(&dport->base.info));

    dport->buf = (char*) pj_pool_alloc(pool, dport->bufsize);
    if (NULL == dport->buf)
        return PJ_ENOMEM;

    *p_port = &dport->base;

    PJ_LOG(4,(THIS_FILE, 
             "Inband DTMF decoder '%.*s' created: samp.rate=%d, bufsize=%uKB",
             (int)dport->base.info.name.slen, dport->base.info.name.ptr,
             PJMEDIA_PIA_SRATE(&dport->base.info), dport->bufsize / 1000));

	//outTestFile = fopen("dtmf-play.pcm", "wb");

	

    return PJ_SUCCESS;
}


/*
 * Register callback.
 */
PJ_DEF(pj_status_t) pjmedia_dtmf_decoder_set_cb(pjmedia_port *port, void *user_data, pj_status_t (*cb) (pjmedia_port *, void *user_data, int)) 
{
    struct dtmf_decoder_port *dport;

    /* Sanity check */
    PJ_ASSERT_RETURN(port && cb, PJ_EINVAL);

    /* Check that this is really a decoder port */
    PJ_ASSERT_RETURN(port->info.signature == SIGNATURE, PJ_EINVALIDOP);

    dport = (struct dtmf_decoder_port *) port;
    dport->base.port_data.pdata = user_data;

	dport->user_data = user_data;
    dport->cb = cb;

    return PJ_SUCCESS;
}
