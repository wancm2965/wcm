/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/state.h"
#include "../include/cnst.h"
#include "../include/proto.h"

/*--------------------------------------------------------------------------*/
/*  Function  encode_frame                                                  */
/*  ~~~~~~~~~~~~~~~~~~~~~                                                   */
/*                                                                          */
/*  Encodes a single frame                                                  */
/*--------------------------------------------------------------------------*/
/*  float       *audio      (i)    input audio                              */
/*  int         num_bits    (i)    number of bits                           */
/*  short       *bitstream  (o)    encoded bitstream                        */
/*  CoderState  *c          (i/o)  coderstate                               */
/*--------------------------------------------------------------------------*/
void encode_frame(float      *audio, 
                  short      *bitstream, 
                  CoderState  *c)
{
   float wtda_audio[FRAME_LENGTH];
   float t_audio[FRAME_LENGTH];
   float t_audio_norm[FREQ_LENGTH];

   short is_transient;
   short bitalloc[NB_SFM];
   short i;

   short nf_idx;
     
   short bits;
   short **pbitstream;
   short *tbitstream;
   
   tbitstream = bitstream;

   is_transient = detect_transient(audio, c);

   if (is_transient) 
   {      
      *bitstream++ = G192_BIT1;
   } 
   else 
   {
      *bitstream++ = G192_BIT0;
   }  

   wtda(audio, wtda_audio, c->old_wtda);   

   direct_transform(wtda_audio, t_audio, is_transient);
  
   pbitstream = &bitstream;
   if (is_transient) 
   {
      flvqenc(pbitstream, t_audio, t_audio_norm, bitalloc,  
              c->num_bits_spectrum_transient, c->num_bits, is_transient);
   }        
   else 
   {  
      flvqenc(pbitstream, t_audio, t_audio_norm, bitalloc,  
              c->num_bits_spectrum_stationary, c->num_bits, is_transient);

      nf_idx = noise_adjust(t_audio_norm, bitalloc);
      idx2bitsn(nf_idx, 2, bitstream);
      bitstream += 2;
   }

   /* fill the bitstream buffer */
   bits = (short)(bitstream - tbitstream);
   bits = c->num_bits - bits;

   for (i = 0; i < bits; i++)
   {
      *bitstream++ = G192_BIT0;
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// AUDEC_g719 API
// add by liurui
#include <malloc.h>
#include <string.h>
typedef struct tagG719Encoder
{
	int				m_nFrameSize;
	int				m_nBitRate;
	int				m_nBitSum;
	int				m_nByteSum;
	CoderState		Encode;
	short *			g192Data;
	short *			tmpData;
	float *			m_fInput;

}G719EncoderWrapper;

static void enc_write_bitstream(short * outData, short bitstream[], short num_bits)
{   
	int i;
	unsigned short  G192_HEADER[2];

	outData[0] = G192_HEADER[0] = G192_SYNC_GOOD_FRAME;
	outData[1] = G192_HEADER[1] = (unsigned short) num_bits;

	for(i=0; i< num_bits; i++)
	{
		outData[2+i] = bitstream[i];
	}
}

int ConvertG192ToPacked(unsigned char in_data[], short *erasure, int number_of_bits, short * g192_data)
{
	int i;
	int j;
	int bit_pos;
	int nsamp;
	int limit;
	int rem;
	int len;
	int erased_frame;
	short packed_word;
	short bit;
	short in_array[2 + 1280];//in_array[2 + number_of_bits];

	*erasure = FALSE;
	in_array [0] = g192_data[0];
	in_array [1] = g192_data[1];
	nsamp = 2;

	if (in_array[0] < G192_FRAME_ERASURE  ||  in_array[0] > G192_FRAME_SYNC_15)
	{
		*erasure = TRUE;
		return 0;
	}
	erased_frame = (in_array[0] == G192_FRAME_ERASURE);
	len = in_array[1];
	if (len > number_of_bits)
	{
		*erasure = TRUE;
		return 0;
	}

	for(i=0; i<len; i++)
	{
		in_array[i] = g192_data[2 + i];
	}
	nsamp = len;

	limit = (nsamp + 7)/8;
	for (i = 0, j = 0;  i < limit;  i++)
	{
		packed_word = 0;
		rem = (i == (limit - 1))  ?  (limit*8 - nsamp)  :  0;
		for (bit_pos = 7;  bit_pos >= rem;  bit_pos--)
		{
			bit = in_array[j++];
			if (bit >= 0x0001  &&  bit <= G192_HARD_ZERO)
			{
				// Its a zero 
			}
			else if (bit >= G192_HARD_ONE  &&  bit <= 0x00FF)
			{
				// Its a one
				packed_word |= (1 << bit_pos);
			}
			else
			{
				// Bad bit 
				*erasure = 1;
			}
		}
		in_data[i] = (unsigned char)packed_word;
	}
	if (erased_frame)
		*erasure = TRUE;
	return nsamp;
}


void *	OpenG719Encoder(int nBitrate, int nFramesize)
{
	G719EncoderWrapper* pEnc = NULL;
	pEnc = (G719EncoderWrapper *)malloc(sizeof(G719EncoderWrapper));
	if(pEnc != NULL)
	{
		memset( pEnc, 0, sizeof(G719EncoderWrapper) );
		pEnc->m_nBitRate = nBitrate;
		pEnc->m_nFrameSize = nFramesize;
		
		pEnc->m_fInput = (float*)malloc(nFramesize*sizeof(float));
		memset( pEnc->m_fInput, 0, nFramesize*sizeof(float) );

		pEnc->m_nBitSum = (short)(nBitrate/50);
		pEnc->m_nByteSum  = (pEnc->m_nBitSum + 7)/8;

		pEnc->tmpData = (short*) malloc((pEnc->m_nBitSum+2)*sizeof(short));
		memset( pEnc->tmpData, 0, (pEnc->m_nBitSum+2)*sizeof(short) );
		pEnc->g192Data = (short*) malloc(MAX_BITS_PER_FRAME*sizeof(short));
		memset( pEnc->g192Data, 0, MAX_BITS_PER_FRAME*sizeof(short) );

		encoder_init(&pEnc->Encode, pEnc->m_nBitSum);
	}
	return pEnc;
}

void	CloseG719Encoder(void * pHandle)
{
	G719EncoderWrapper   *pEnc = (G719EncoderWrapper*) pHandle;
	if(pEnc != NULL)
	{
		if(pEnc->g192Data != NULL)
		{
			free(pEnc->g192Data);
			pEnc->g192Data = NULL;
		}

		if(pEnc->tmpData != NULL)
		{
			free(pEnc->tmpData);
			pEnc->tmpData = NULL;
		}

		if(pEnc->m_fInput != NULL)
		{
			free(pEnc->m_fInput);
			pEnc->m_fInput = NULL;
		}
		free(pEnc);
		pEnc = NULL;
	}
}

int		G719EncoderFrame(void * pHandle, short * input, unsigned char * output )
{
	int		i = 0;
	int		ret = 0;
	short	bfi = 0;
	G719EncoderWrapper   *pEnc = (G719EncoderWrapper*) pHandle;
	if(pEnc == NULL)
		return 0;
	if( pEnc->g192Data == NULL || pEnc->tmpData == NULL )
		return 0;

	for(i = 0; i< pEnc->m_nFrameSize; i++)
		pEnc->m_fInput[i] = (float)input[i];
	
	encode_frame(pEnc->m_fInput, pEnc->tmpData, &pEnc->Encode);
	enc_write_bitstream(pEnc->g192Data, pEnc->tmpData, pEnc->m_nBitSum);
	ret = ConvertG192ToPacked(output, &bfi, pEnc->m_nBitSum, pEnc->g192Data);
	if( ret <= 0 )
		return 0;

	ret = pEnc->m_nByteSum;
	return ret;
}