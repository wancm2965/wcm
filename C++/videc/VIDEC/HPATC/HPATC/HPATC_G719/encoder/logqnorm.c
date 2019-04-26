/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/cnst.h"
#include "../include/rom.h"
#include <math.h>

/*--------------------------------------------------------------------------*/
/*  Function  logqnorm                                                      */
/*  ~~~~~~~~~~~~~~~~~~~                                                     */
/*                                                                          */
/*  Log quantization for norms of sub-vectors                               */
/*--------------------------------------------------------------------------*/
/*  float     *x  (i)   coefficient vector                                  */
/*  short     *k  (o)   index                                               */
/*  short     L   (i)   codebook length                                     */
/*  short     N   (i)   sub-vector size                                     */
/*--------------------------------------------------------------------------*/
void logqnorm(float *x, short *k, short L, short N)
{
    short i, j, j1, j2;
    float temp, power;

    temp = 0.0;
    for (i=0; i<N; i++)
    {
       temp += (x[i] * x[i]);
    }
    power = (float)sqrt(temp / (float)N);
    if (power>=thren[0])
    {
      *k = 0;
    }
    else if (power<thren[L-2])
    {
      *k = L - 1;
    }
    else
    {
      j1 = 0;
      j2 = L - 1;
      while ((j2-j1)>1)
      {
        j = (j1 + j2) >> 1;
        if (power>=thren[j])
        {
          j2 = j;
        }
        else
        {
          j1 = j;
        }
      }
      *k = j2;
    }

    return;
}
