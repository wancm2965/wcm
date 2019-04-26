#include "../include/stl.h"
//#include "convert.h"

#ifdef _WIN32
#include <math.h>
#endif



static Word16 round_sat16(float in) 
{
   Word16 out;

    if (in >= 0.0f) 
    {
      in += 0.5f;
    }
    else 
    {
      in -= 0.5f;
    }

    if (in > 32767.0f) {
      out = 32767;
    }
    else 
    {
      if (in < -32768.0f) 
      {
	      out = -32768;
      }
      else {
	      out = (Word16) in;
      }
    }
    return out;
}

static Word32 round_sat32(float in) 
{
   Word32 out;

    if (in >= 0.0f) 
    {
      in += 0.5f;
    }
    else 
    {
      in -= 0.5f;
    }

    if (in > 2147483647.0) {
      out = 2147483647;
    }
    else 
    {
      if (in < -2147483648.0) 
      {
	      out = -2147483647 - 1; /* to avoid warning C4146*/
      }
      else {
	      out = (Word32) in;
      }
    }
    return out;
}


void convert_w16_to_float(Word16 *in, float *out, int length, int q)
{
   int i;
   float multiplier;
   /*
   if(q >= 0) multiplier = 1.0f / (float)(1 << q);
   else multiplier = (float)(1 << -q);
   */
   multiplier = 1.0f / powf(2.0f, q);

   for(i = 0; i < length; i++) 
   {
      out[i] = ((float)in[i]) * multiplier;
   }
}

void convert_float_to_w16(float *in, Word16 *out, int length, int q)
{
   int i;
   float multiplier;
   /*
   if(q >= 0) multiplier = (float)(1 << q);
   else multiplier = 1.0f / (float)(1 << -q);
   */
   multiplier = powf(2.0f, q);

   for(i = 0; i < length; i++) 
   {
      out[i] = round_sat16(in[i] * multiplier);
   }
}

void convert_w32_to_float(Word32 *in, float *out, int length, int q)
{
   int i;
   float multiplier;
   /*
   if(q >= 0) multiplier = 1.0f / (float)(1 << q);
   else multiplier = (float)(1 << -q);
   */
   multiplier = 1.0f / powf(2.0f, q);

   for(i = 0; i < length; i++) {
      out[i] = ((float)in[i]) * multiplier;
   }
}

void convert_float_to_w32(float *in, Word32 *out, int length, int q)
{
   int i;
   float multiplier;
   /*
   if(q >= 0) multiplier = (float)(1 << q);
   else multiplier = 1.0f / (float)(1 << -q);
   */
   multiplier = powf(2.0f, q);
   
   for(i = 0; i < length; i++) {
      out[i] = round_sat32(in[i] * multiplier);
   }
}

void convert_int_to_w16(int *in, Word16* out, int length)
{
   int i;

   for(i = 0; i < length; i++) {
      if(in[i] > 32767)       out[i] = (Word16)32767;
      else if(in[i] < -32768) out[i] = (Word16)-32768;
      else                    out[i] = (Word16)in[i];
   }
}

void convert_w16_to_int(Word16 *in, int* out, int length)
{
   int i;

   for(i = 0; i < length; i++) {
      out[i] = in[i];
   }
}
