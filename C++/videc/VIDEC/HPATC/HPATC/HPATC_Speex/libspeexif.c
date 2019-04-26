#include "libspeexif.h"
#include <speex/speex.h>
#include <stdlib.h>
#include <memory.h>


struct spx_encoder
{
    void		*enc;		    /**< Encoder state.		*/
    SpeexBits	enc_bits;	    /**< Encoder bits.		*/
};

struct spx_decoder
{
    void		*dec;		    /**< Decoder state.		*/
    SpeexBits	dec_bits;	    /**< Decoder bits.		*/
};


void* libspeex_enc_init(int modeID)
{
	struct spx_encoder*spx;
	int nTemp;
	const SpeexMode*pMode;

	if (modeID!=SPEEX_MODEID_NB)
		return NULL;

	pMode=speex_lib_get_mode (modeID);
	spx=(struct spx_encoder*)malloc(sizeof(struct spx_encoder));
	spx->enc = speex_encoder_init(pMode);
    speex_bits_init(&spx->enc_bits);

    nTemp = 8000;
    speex_encoder_ctl(spx->enc, SPEEX_SET_SAMPLING_RATE, &nTemp);

    nTemp = 8000;
    speex_encoder_ctl(spx->enc, SPEEX_SET_BITRATE, &nTemp);

    nTemp = 0;
    speex_encoder_ctl(spx->enc, SPEEX_SET_VAD, &nTemp);
    speex_encoder_ctl(spx->enc, SPEEX_SET_DTX, &nTemp);


	return spx;
}


int libspeex_enc_encode(void*st, short * input, char *output)
{
	int output_buf_len=160;
	int nTemp;
	struct spx_encoder *spx=(struct spx_encoder *)st;
    speex_bits_reset(&spx->enc_bits);
	speex_encode_int(spx->enc, input, &spx->enc_bits);
	nTemp=speex_bits_nbytes(&spx->enc_bits);
	if ( nTemp <= (int)output_buf_len)
	{
	}

	return speex_bits_write(&spx->enc_bits,output, output_buf_len);
}

int libspeex_enc_close(void*st)
{
	struct spx_encoder *spx=(struct spx_encoder *)st;
    if (spx->enc) 
	{
		speex_encoder_destroy( spx->enc );
		spx->enc = NULL;
		speex_bits_destroy( &spx->enc_bits );
    }

	free(spx);
	return 0;
}


void* libspeex_dec_init(int modeID)
{
	struct spx_decoder *spx;
	int enh_enabled=1;
	int bitrate=8000;
	const SpeexMode*pMode;

	if (modeID!=SPEEX_MODEID_NB)
		return NULL;

	pMode=speex_lib_get_mode (modeID);
	spx=(struct spx_decoder*)malloc(sizeof(struct spx_decoder));
	spx->dec = speex_decoder_init(pMode);
    speex_bits_init(&spx->dec_bits);
	speex_decoder_ctl(spx->dec, SPEEX_SET_ENH, &enh_enabled);
	speex_decoder_ctl(spx->dec, SPEEX_SET_SAMPLING_RATE, &bitrate);

	return spx;
}

int libspeex_dec_decode(void*st, char * input, short * output, int in_len)
{
	struct spx_decoder *spx=(struct spx_decoder *)st;
    speex_bits_read_from(&spx->dec_bits,input,in_len);
    speex_decode_int(spx->dec, &spx->dec_bits, (spx_int16_t*)output);
	return 0;
}


int libspeex_dec_close(void*st)
{
	struct spx_decoder *spx=(struct spx_decoder *)st;
    if (spx->dec) 
	{
		speex_decoder_destroy( spx->dec);
		spx->dec = NULL;
		speex_bits_destroy( &spx->dec_bits );
    }
	free(spx);
	return 0;
}
