
/*======================================================================*/
/*  Module:     decode.h                                                */
/*----------------------------------------------------------------------*/
#ifndef  _DECODE_H_
#define  _DECODE_H_

/*======================================================================*/
/*         ..Functions.                                                 */
/*----------------------------------------------------------------------*/

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
}DecodeArgs;

extern void * OpenEVRCDecoder( );
extern void CloseEVRCDecoder( void * handle );
extern void DecodeOneFrame( void * handle, short * bitstream, short * pcmData );


extern void InitDecoder();
extern void decode(short*,short,short,short*);
extern void decode_no_fer(short*,short,short*);
extern void decode_fer(short,short*);
extern void decode_rate_1(short*,short,short*);

#endif
