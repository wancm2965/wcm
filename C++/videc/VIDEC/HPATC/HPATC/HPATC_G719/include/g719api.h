///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AUDEC_g719 API
// FILE : g719api.h

#ifdef __cplusplus
extern "C" {
#endif

//Encoder
void *	OpenG719Encoder(int nBitrate, int nFramesize);
void	CloseG719Encoder(void * pHandle);
int		G719EncoderFrame(void * pHandle, short * input, unsigned char * output );

//Decoder
void *	OpenG719Decoder( int nBitrate, int nFramesize );
void	CloseG719Decoder(void * pHandle);
int		G719DecoderFrame(void * pHandle, unsigned char * input, short * output, int bfi );


#ifdef __cplusplus
}
#endif