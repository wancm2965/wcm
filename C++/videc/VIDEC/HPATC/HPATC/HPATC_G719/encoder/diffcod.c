/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  diffcod                                                       */
/*  ~~~~~~~~~~~~~~~~~                                                       */
/*                                                                          */
/*  Differential coding for indices of quantized norms                      */
/*--------------------------------------------------------------------------*/
/*  short    *normqlg2  (i)   quantized norms in log2                       */
/*  short    N          (i)   number of sub-vectors                         */
/*  short    *y         (i/o) indices of quantized norms                    */
/*  short    *difidx    (o)   differential code                             */
/*--------------------------------------------------------------------------*/
void diffcod(short *normqlg2, short N, short *y, short *difidx)
{
    short i, j, k, m, r;

    m = N - 1;
    for (i=m; i>0; i--)
    {
       r = i - 1;
       k = y[i] - y[r];
       if (k<(-15))
       {
         y[r] = y[i] + 15;
       }
    }
    for (i=1; i<N; i++)
    {
       r = i - 1;
       k = y[i] - y[r];
       if (k>16)
       {
         k = 16;
         y[i] = y[r] + 16;
       }
       j = y[i];
       normqlg2[i] = dicnlg2[j];
       difidx[r] = k + 15;
    }

    return;
}
