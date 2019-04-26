
#ifndef  _ENCODE_H_
#define  _ENCODE_H_

/*======================================================================*/
/*         ..Functions.                                                 */
/*----------------------------------------------------------------------*/
extern void   InitEncoder();
extern short  pre_encode(short*,long*);
extern void   encode(short,short*);
extern void   post_encode();

typedef struct
{
	short max_rate;
	short max_rate_default;
	short min_rate;
	short min_rate_default;
	short post_filter;
	short post_filter_default;
	short noise_suppression;
	short noise_suppression_default;
	short ibuf_len;
	short obuf_len;
}
EncodeArgs;

extern void * OpenEVRCEncoder( );
extern void CloseEVRCEncoder( void * handle );
extern void EncodeOneFrame( void * handle, short * pcmData, short * bitstream );//unsigned char * bitstream );//short * bitstream );

#endif
