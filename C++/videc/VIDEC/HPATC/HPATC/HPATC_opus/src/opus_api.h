///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AUDEC_opus API
// FILE : opus_api.h

#ifdef __cplusplus
extern "C" {
#endif

//Encoder
void *	OpenOpusEncoder(int nChannel, int nSamplerate, int nBitrate, int nFramesize);
void	CloseOpusEncoder(void * pHandle);
int		OpusEncoderFrame(void * pHandle, short * input, int nFrameSize, unsigned char * output );

//Decoder
void *	OpenOpusDecoder( int nChannel, int nSamplerate );
void	CloseOpusDecoder(void * pHandle);
int		OpusDecoderFrame(void * pHandle, unsigned char * input, int nSize, short * output );
int		OpusFillInFrame(void * pHandle, unsigned char * input, int nSize, short * output );


#ifdef __cplusplus
}
#endif