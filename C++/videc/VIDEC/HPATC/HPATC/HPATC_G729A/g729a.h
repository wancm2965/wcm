#ifndef __G729A_H__
#define __G729A_H__

#define  L_FRAME_COMPRESSED 10
#define  L_FRAME            80
#define  L_FRAME_BYTES		160

void* va_g729a_init_encoder();
void va_g729a_encoder(void*handle,short *speech, unsigned char *bitstream);
void va_g729a_free_encoder(void*handle);

void* va_g729a_init_decoder();
void va_g729a_decoder(void*handle,unsigned char *bitstream, short *synth_short, int bfi);
void va_g729a_free_decoder(void*handle);


#endif