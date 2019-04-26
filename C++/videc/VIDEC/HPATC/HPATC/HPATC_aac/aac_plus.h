#define AAC_SAMPLES_FER_FRAME	1024
#define AAC_SAMPLE_RATE			16000
#define AAC_BITRATE				32000


void* aac_plus_init_encoder();
void aac_plus_encoder(void*handle,short *speech, unsigned char *bitstream,int*numOutBytes);
void aac_plus_free_encoder(void*handle);

void* aac_plus_init_decoder();
void aac_plus_decoder(void*handle,unsigned char *bitstream,int numBytes,short *synth_short,int*numSamples);
void aac_plus_free_decoder(void*handle);
