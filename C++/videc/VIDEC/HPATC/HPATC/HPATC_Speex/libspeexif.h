//libspeexif.h
#ifndef __LIBSPEEXIF_H__
#define __LIBSPEEXIF_H__

/* ************************************************************************************************** 
  *  PCM data format for encoder & decoder
  * 	Sample Rate	: 	8000 samples per second
  *   Sample Width 	:	16bits per sample
  *	Sample Endia 	:	Little-Endian
  * 	Channels		:	1, mono
  * 	Time Interval	:	20ms
  *	Frame Size	:	160 samples per frame, 320bytes per frame
  * 
  * **************************************************************************************************
  */

/* 
  * Function: int speex_enc_init(int modeID)
  *   Input: 	int		--	modeID, set as 0
  *   Output:	null
  * 	Return: 	int		-- 	Handle of Encoder if successfully, 0 for failed
  */
void* libspeex_enc_init(int modeID);
/* 
  * Function: int speex_enc_encode(int hd, short * input, char *output)
  *   Input: 	int		--	Handle of Encoder
  			short *	--	input PCM buffer, type is : SamplesRate = 8000, SampleWidth=16bits, Channels=mono, Size is 160 Samples(320bytes)
  *   Output:	char * 	--	output encoded data buffer, size is > 20 bytes;
  * 	Return: 	int		-- 	the length of output data in buffer
  */
int libspeex_enc_encode(void*st, short * input, char *output);
/* 
  * Function: int speex_enc_close(int hd)
  *   Input: 	int		--	Handle of Decoder
  *   Output:	null
  * 	Return: 	int		-- 	0 for successfully, -1 for failed.
  */
int libspeex_enc_close(void*st);


/* 
  * Function: int speex_dec_init(int modeID)
  *   Input: 	int		--	modeID, set as 0
  *   Output:	null
  * 	Return: 	int		-- 	Handle of Decoder if successfully, 0 for failed
  */
void* libspeex_dec_init(int modeID);
/* 
  * Function: int speex_dec_decode(int hd, char * input, short * output, int in_len)
  *   Input: 	int		--	Handle of Decoder
  			char *	--	output encoded data buffer, size is > 20 bytes;
  			int		-- 	the length of input encoded data in buffer, 0 for packet lost
  *   Output:	short * 	--	output PCM buffer, type is : SamplesRate = 8000, SampleWidth=16bits, Channels=mono, Size is 160 Samples(320bytes)
  * 	Return: 	int		-- 	0 for successfully, -1 for failed.
  */
int libspeex_dec_decode(void*st, char * input, short * output, int in_len);
/* 
  * Function: int speex_dec_close(int hd)
  *   Input: 	int		--	Handle of Decoder
  *   Output:	null
  * 	Return: 	int		-- 	0 for successfully, -1 for failed.
  */
int libspeex_dec_close(void*st);


#endif