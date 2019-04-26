/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/state.h"
#include "../include/cnst.h"
#include "../include/proto.h"

/*--------------------------------------------------------------------------*/
/*  Function  decode_frame                                                  */
/*  ~~~~~~~~~~~~~~~~~~~~~~                                                  */
/*                                                                          */
/*  Decodes a single frame                                                  */
/*--------------------------------------------------------------------------*/
/*  short         bitstream[]  (i)    bitstream to decode                   */
/*  short         bfi          (i)    bad frame indicator                   */
/*  short         out16[]      (o)    decoded audio                         */
/*  DecoderState  *d           (i/o)  state of decoder                      */
/*--------------------------------------------------------------------------*/
void decode_frame(short bitstream[], 
                  short bfi, 
                  short out16[],
                  DecoderState *d)
{
   short is_transient;
   float t_audio_q[FRAME_LENGTH];
   float wtda_audio[2*FRAME_LENGTH];
   short bitalloc[NB_SFM];   
   short ynrm[NB_SFM];
   short i;
   float audio_q_norm[FREQ_LENGTH];
   short nf_idx;
   short **pbitstream;
   short *tbitstream;

   if (bfi) 
   {      
      for (i=0; i < FRAME_LENGTH; i++) 
      {
         t_audio_q[i] = d->old_coeffs[i];
         d->old_coeffs[i] = d->old_coeffs[i]/2;
      }
      is_transient = d->old_is_transient;
   }
   else
   {
      if (*bitstream == G192_BIT1) 
      {
         is_transient = 1;
      } 
      else 
      {
         is_transient = 0;
      }   

      bitstream++;

      tbitstream = bitstream;
      pbitstream = &bitstream;
      if (is_transient) 
      {
         flvqdec(pbitstream, t_audio_q, audio_q_norm, bitalloc, (short)d->num_bits_spectrum_transient, ynrm, is_transient); 
         nf_idx = 0;
      }
      else
      {
         flvqdec(pbitstream, t_audio_q, audio_q_norm, bitalloc, (short)d->num_bits_spectrum_stationary, ynrm, is_transient); 
         bits2idxn(bitstream, 2, &nf_idx);
         bitstream += 2;
      }
      
      for (i = FREQ_LENGTH; i < FRAME_LENGTH; i++) 
      {
         t_audio_q[i] = 0.0f;
      }

      fill_spectrum(audio_q_norm, t_audio_q,  bitalloc, is_transient, ynrm, nf_idx);

      if (is_transient) 
      {
         de_interleave_spectrum(t_audio_q);
      }

      for (i=0; i < FRAME_LENGTH; i++) 
      {
         d->old_coeffs[i] = t_audio_q[i];

      }
      d->old_is_transient = is_transient;
   }
   
   inverse_transform(t_audio_q, wtda_audio, is_transient);
   window_ola(wtda_audio, out16, d->old_out);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// AUDEC_g719 API
// add by liurui
#include <malloc.h>
#include <string.h>

static int ConvertPackedToG192(const unsigned char out_data[],
						int number_of_bits,
						short * g192_data)
{
	int i;
	int j;
	int bit_count;
	int number_of_bytes;
	unsigned char packed_word;
	short out_array[2 + 1280 + 7];//out_array[2 + number_of_bits + 7];

	number_of_bytes = (number_of_bits + 7)/8;
	j = 0;
	out_array[j++] = G192_FRAME_SYNC_1;
	out_array[j++] = number_of_bits;
	for (i = 0;  i < number_of_bytes;  i++)
	{
		packed_word = out_data[i];
		for (bit_count = 7;  bit_count >= 0;  bit_count--)
			out_array[j++] = ((packed_word >> bit_count) & 1)  ?  G192_HARD_ONE  :  G192_HARD_ZERO;
	}   

	for(i = 0; i < number_of_bits + 2; i ++)
	{
		g192_data[i] = out_array[i];
	}
	return (number_of_bits + 2);
}

static short dec_read_bitstream(short * in_data, short bitstream[], short *num_bits) // short *bfi)
{   
	int i;
	unsigned short   G192_SYNC_WORD;   
	short    result;
	G192_SYNC_WORD = in_data[0];
	{
		if((G192_SYNC_WORD != G192_SYNC_GOOD_FRAME) && (G192_SYNC_WORD != G192_SYNC_BAD_FRAME)) 
		{
			fprintf(stderr, "\n Invalid bitstream. Wrong G192_SYNC_WORD ");
			return -1;
		} 
		else 
		{
			//if(G192_SYNC_WORD == G192_SYNC_BAD_FRAME) 
			//{
			//	*bfi = 1;
			//} 
			//else 
			//{
			//	*bfi = 0;         
			//}

			num_bits[0] = in_data[1];
			if(*num_bits > MAX_BITS_PER_FRAME) 
			{
				fprintf(stderr, "\n Frame is too large  ");
				return -1;
			}
			for(i=0; i< *num_bits; i++)
			{
				bitstream[i] = in_data[2+i];
			}
		}
		result = 1;
	}
	return(result);
}

typedef struct tagG719Decoder
{
	int				m_nFrameSize;
	int				m_nBitRate;
	short			m_nBitSum;
	DecoderState	Decode;
	short *			g192Data;
	short *			tmpData;

}G719DecoderWrapper;

void * OpenG719Decoder( int nBitrate, int nFramesize )
{
	G719DecoderWrapper* pDec = NULL;
	pDec = (G719DecoderWrapper *)malloc(sizeof(G719DecoderWrapper));
	if(pDec != NULL)
	{
		memset( pDec, 0, sizeof(G719DecoderWrapper) );
		pDec->m_nBitRate = nBitrate;
		pDec->m_nFrameSize = nFramesize;

		pDec->m_nBitSum = (short)(nBitrate/50);

		pDec->tmpData = (short*) malloc((pDec->m_nBitSum+2)*sizeof(short));
		memset( pDec->tmpData, 0, (pDec->m_nBitSum+2)*sizeof(short) );
		pDec->g192Data = (short*) malloc(MAX_BITS_PER_FRAME*sizeof(short));
		memset( pDec->g192Data, 0, MAX_BITS_PER_FRAME*sizeof(short) );

		decoder_init(&pDec->Decode, pDec->m_nBitSum);	
	}
	return pDec;
}
void CloseG719Decoder(void * pHandle)
{
	G719DecoderWrapper   *pDec = (G719DecoderWrapper*) pHandle;
	if(pDec != NULL)
	{
		if(pDec->g192Data != NULL)
		{
			free(pDec->g192Data);
			pDec->g192Data = NULL;
		}

		if(pDec->tmpData != NULL)
		{
			free(pDec->tmpData);
			pDec->tmpData = NULL;
		}
		free(pDec);
		pDec = NULL;
	}
}
int G719DecoderFrame(void * pHandle, unsigned char * input, short * output, int bfi )
{
	int ret = 0;
	G719DecoderWrapper   *pDec = (G719DecoderWrapper*) pHandle;
	if(pDec == NULL)
		return 0;
	if( pDec->g192Data == NULL || pDec->tmpData == NULL )
		return 0;

	ConvertPackedToG192(input, pDec->m_nBitSum, pDec->g192Data);
	dec_read_bitstream(pDec->g192Data, pDec->tmpData, &pDec->m_nBitSum);
	if( !bfi) 
	{
		decoder_reset_tables(&pDec->Decode, pDec->m_nBitSum);
	} 
	decode_frame(pDec->tmpData, bfi, output, &pDec->Decode);
	return ret;
}