//////////////////////////////////////////////////////////////////////////////
//

#ifndef PJMEDIA_DTMF_DECODER_H
#define PJMEDIA_DTMF_DECODER_H

/**
 * Create DTMF decoder port.
 */
PJ_DECL(pj_status_t) pjmedia_dtmf_decoder_create(pj_pool_t *pool,
                                                const char *name,
                                                unsigned sampling_rate,
                                                unsigned samples_per_frame,
                                                pj_ssize_t buff_size,
                                                pjmedia_port **p_port);


/**
 *  Register the callback to be called when inband RTMF reached.
 */
PJ_DECL(pj_status_t) pjmedia_dtmf_decoder_set_cb(pjmedia_port *port,
                                                void *user_data,
                                                pj_status_t (*cb)(pjmedia_port *, void *usr_data, int));


#endif 
/* end of include guard: PJMEDIA_DTMF_DECODER_H */
