//	c_utils.cpp

//	utility functions for vad program

#include "c_utils.h"
#include <math.h>


short 
sature16(long data)
{
  if ( data > (short)0x7fff )
    {
    	return (short)MAX_16;
    }
  if ( data < (short)0xffff8000 )
    {
    	return (short)MIN_16;
    }
  return (short)data; 
}

void
bqInit (t_biquad *bq, short a0, short a1, short a2, short b1, short b2)
{
   bq->a0 = a0;
   bq->a11 = a1;
   bq->a12 = a2; 
   bq->negb11 = -b1;
   bq->negb12 = -b2;
   bq->dn_1 = 0;
   bq->dn_2 = 0;
}

void
bqProcess(t_biquad *bq, short *datain, short *dataout, int n) 
{
    int i;
    static short prev = 1700;

	for (i = 0; i < n; i++)
	{
		/* do biquad processing */
	    
		dataout[i] = datain[i] - prev;
		prev = datain[i];
	}
    
}


void 
calcPower ( int length, unsigned long *login, float *logout)	// formerly called log10_32()
{
    int i;

	for (i = 0; i < length; i++)
	{
		logout[i] = (float) (10 * log10((float)login[i]) - 10 * 9.332); 
	}
    
}