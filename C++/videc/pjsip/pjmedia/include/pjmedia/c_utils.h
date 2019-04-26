#ifndef __UTILS_H__
#define __UTILS_H__

#include "c_types.h"

typedef struct _biquad {

  short negb12;
  short a12;
  short negb11;
  short a11;
  short a0;
  short dn_2;
  short dn_1;
  
} t_biquad;

#define abs16(data) ((data < 0) ? -data : data)
#define MIN_16 0x8000
#define MAX_16 0x7fff

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

extern short sature16(long data);
                         
extern void bqProcess (t_biquad *bq, short *datain, short *dataout, int n);

extern void bqInit (t_biquad *bq, short a0, short a1, short a2, short b1, short b2); 
                       
extern void calcPower ( int length, unsigned long *datain, float *logout); // formerly called log10_32()

#endif
