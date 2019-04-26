/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/cnst.h"
#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  lvq1                                                          */
/*  ~~~~~~~~~~~~~~                                                          */
/*                                                                          */
/*  1-bit lattice vector quantization                                       */
/*--------------------------------------------------------------------------*/
/*  short     *x  (i)   arbitrary vector                                    */
/*  short     *k  (o)   index of the codevector                             */
/*--------------------------------------------------------------------------*/
void lvq1(float *x, short *k)
{
    short i, j, m, n, im, offset;
    short idx[8];
    float dist, temp;


    /* Searching */
    for (i=0; i<8; i++)
    {
       idx[i] = i;
    }
    for (i=0; i<7; i++)
    {
       im = i;
       n = i + 1;
       for (j=n; j<8; j++)
       {
          if (x[im]<x[j])
          {
            im = j;
          }
       }
       temp = x[i];
       x[i] = x[im];
       x[im] = temp;
       j = idx[i];
       idx[i] = idx[im];
       idx[im] = j;
    }
    m = 0;
    dist = MAX_DIST;
    for (i=0; i<N_LEADER1; i++)
    {
       temp = 0.0;
       for (j=0; j<8; j++)
       {
          temp += ((x[j] - (float)dic1[i][j]) * (x[j] - (float)dic1[i][j]));
       }
       if (temp<dist)
       {
         dist = temp;
         m = i;
       }
    }

    /* Indexing */
    offset = dic2[m];
    if (m==3)
    {
      j = 0;
      for (i=0; i<8; i++)
      {
         j += (idx[i] * dic1[m][i]);
      }
      if (j<0)
      {
        offset += 28;
      }
    }
    j = 0;
    for (i=0; i<8; i++)
    {
       j += (dic0[m][i] << idx[i]);
    }
    *k = offset + dic3[j];


    return;
}
