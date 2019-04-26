
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include "mathadv.h"
#include "mathevrc.h"
#include "mathdp31.h"

#include  "decode.h"
#include  "d_globs.h"
#include  "fer.h"
#include  "globs.h"
#include  "macro.h"
#include  "proto.h"
#include  "rom.h"

#include "tty.h"


/*======================================================================*/
/*         ..Reset RCELP decode rate.                                   */
/*----------------------------------------------------------------------*/
void SetDecodeRate(short rate)
{

	/*....execute.... */
	bit_rate = rate;

	switch (rate)
	{
	case 1:
		FCBGainSize = 16;		/*...use half-rate... */
		gnvq = gnvq_4;
		break;
	case 3:
		nsub = nsub22;
		nsize = nsize22;
		lognsize = lognsize22;
		lsptab = lsptab22;
		knum = 3;
		FCBGainSize = 16;
		gnvq = gnvq_4;
		break;
	case 4:
		nsub = nsub28;
		nsize = nsize28;
		lognsize = lognsize28;
		lsptab = lsptab28;
		knum = 4;
		FCBGainSize = 32;
		gnvq = gnvq_8;
		break;
	}
}

/*======================================================================*/
/*         ..Reset RCELP decoder parameters.                            */
/*----------------------------------------------------------------------*/
void InitDecoder()
{
	/*....(local) variables.... */
	int j;

	/*....execute.... */
	SetDecodeRate(4);
	last_valid_rate = 1;		/* reset last_valid_rate */
	last_fer_flag = 0;			/* reset last_fer_flag */
	decode_fcnt = 0;

	for (j = 0; j < ORDER; j++)
		SynMemory[j] = 0;

	OldlspD[0] = 1573;
	OldlspD[1] = 3146;
	OldlspD[2] = 4719;
	OldlspD[3] = 6291;
	OldlspD[4] = 7864;
	OldlspD[5] = 9437;
	OldlspD[6] = 11010;
	OldlspD[7] = 12583;
	OldlspD[8] = 14156;
	OldlspD[9] = 15729;

	for (j = 0; j < ACBMemSize; j++)
		PitchMemoryD[j] = PitchMemoryD_back[j] = 0;
	pdelayD = 40;
	ave_acb_gain = ave_fcb_gain = 0;
	FadeScale = 32767;
	fer_counter = 0;

	erasureFlag = 0;
	errorFlag = 0;
}

/*======================================================================*/
/*         ..Decode bitstream data.                                     */
/*----------------------------------------------------------------------*/
void decodebitstream(
			   //short *codeBuf,
			   unsigned char * codeBuf,
			   short rate,
			   short post_filter,
			   short *outFbuf
)
{
	/*....(local) variables.... */
	short local_rate;
    short acbgain_flag;
    short *oP;
    short subframesize;
    short j;


	/*....execute.... */
    acbgain_flag = 1; /* initialize to get rid of compiler warning */
	local_rate = rate;
	FrameErrorHandler(&local_rate, codeBuf);  /* check frame for some errors */
	SetDecodeRate(local_rate);

	fer_flag = erasureFlag | errorFlag;

	if( tty_option == TTY_NO_GAIN )
    {
        if( local_rate == 4 )
        {
            /* Get pitch delay */
			/* pitch_index = (((codeBuf[1] & 0x7) << 4) | ((codeBuf[2] >> 12) & 0xf)); */
            tty_dec_header = shr((codeBuf[1] & 0x6), 1);
            tty_dec_char = shl((codeBuf[1] & 0x01), 4);
            tty_dec_char = add(tty_dec_char,(shr(codeBuf[2],12) & 0xF));
            tty_dec_baud_rate = (codeBuf[10] & 0x0020) >> 5; /* Get baud rate from bit 171 */

			/* Check for non-zero ACB gain in all three subframes */
            acbgain_flag = 0;
			acbgain_flag |= (codeBuf[2] & 0x0070);
			acbgain_flag |= (codeBuf[5] & 0x0e00);
			acbgain_flag |= (codeBuf[7] & 0x0001);
			acbgain_flag |= (codeBuf[8] & 0xc000);
		}
        else if( local_rate == 3 )
        {
            /* Get pitch delay */
			/* pitch_index = ((codeBuf[1] & 0x3f8) >> 3); */
            tty_dec_header = shr((codeBuf[1] & 0x300), 8);
            tty_dec_char = shr((codeBuf[1] & 0xF8), 3);

			/* Check for non-zero ACB gain in all three subframes */
            acbgain_flag = 0;
			acbgain_flag |= (codeBuf[1] & 0x0007);
			acbgain_flag |= (codeBuf[2] & 0x0003);
			acbgain_flag |= (codeBuf[3] & 0x8001);
			acbgain_flag |= (codeBuf[4] & 0xc000);
		}
        else
        {
            tty_dec_header = 0;
            tty_dec_char = 0;
            acbgain_flag = 1;
        }
		acbgain_flag = abs(acbgain_flag);

		oP = outFbuf;

		/* TTY subframe loop */
        for( j=0 ; j < NoOfSubFrames ; j++)  /* loop over all subframes */
        {
			if (j < 2)
            {
				subframesize = SubFrameSize - 1;
			}
            else
            {
                subframesize = SubFrameSize;
            }

            tty_dec_flag = tty_dec( oP,
				        acbgain_flag,
				        tty_dec_header,
				        tty_dec_char,
				        tty_dec_baud_rate,
                        fer_flag,
				        j,
				        NoOfSubFrames,
				        subframesize);
             
			oP += subframesize;
        } /* end for(j) */

        if( tty_dec_flag != 0 )
        {
            InitDecoder();
            return;
        }

	} /* end if( tty_option == TTY_NO_GAIN ) */


	if (!fer_flag){
		decode_no_fer(codeBuf, post_filter, outFbuf); /* no FER */
		fer_flag = erasureFlag | errorFlag;
		}

	if (fer_flag)
		decode_fer(post_filter, outFbuf); /* FER has occured */
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapped functions
void * OpenEVRCDecoder( )
{
	DecodeArgs * handle = malloc(sizeof(DecodeArgs));
	
	handle->max_rate = 4;
	handle->max_rate_default = 4;
	handle->min_rate = 1;
	handle->min_rate_default = 1;
	handle->post_filter = 1;
	handle->post_filter_default = 1;
	handle->noise_suppression = 1;
	handle->noise_suppression_default = 1;
	
//	handle->ibuf_len = BITSTREAM_BUFFER_LEN - 1;
//	handle->obuf_len = SPEECH_BUFFER_LEN;
	
	// Gloab var
	tty_option = TTY_NO_GAIN;
	
	// Open
	InitDecoder();
    if( tty_option == TTY_NO_GAIN )
    {
        init_tty_enc( &tty_enc_char, &tty_enc_header, &tty_enc_baud_rate);
        init_tty_dec();
        tty_debug();
    }
	
	return handle;
}

void CloseEVRCDecoder( void * handle )
{
	DecodeArgs * pHandle = handle;
	if( pHandle != NULL )
	{
		free( pHandle );
		pHandle = NULL;
	}
}

void DecodeOneFrame( void * handle, Shortword * bitstream, Shortword * pcmData )
					//unsigned char * bitstream, short * pcmData )//Shortword * bitstream, Shortword * pcmData )
{
	DecodeArgs * pHandle = handle;
	Shortword rate = 1;
	if( bitstream == NULL || pcmData == NULL || handle == NULL)
		return;


	rate = bitstream[0];
	decodebitstream(&bitstream[1], rate, pHandle->post_filter, pcmData);

	//memcpy( &rate, bitstream, sizeof(short) );
	//decodebitstream(bitstream+sizeof(short), rate, pHandle->post_filter, pcmData);
}