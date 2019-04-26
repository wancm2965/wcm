/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/


#include "../include/cnst.h"

/*--------------------------------------------------------------------------*/
/*  Function  reordernorm                                                   */
/*  ~~~~~~~~~~~~~~~~~~~~~                                                   */
/*                                                                          */
/*  Reorder quantization indices and norms                                  */
/*--------------------------------------------------------------------------*/
/*  short     *ynrm      (i)   quantization indices for norms               */
/*  short     *normqlg2  (i)   quantized norms                              */
/*  short     *idxbuf    (o)   reordered quantization indices               */
/*  short     *normbuf   (o)   reordered quantized norms                    */
/*--------------------------------------------------------------------------*/
void reordernorm(short *ynrm, short *normqlg2, short *idxbuf, short *normbuf)
{
    short i;
    short *pidx, *pnormq;

    for (i=0; i<2; i++)
    {
       pidx = ynrm + i;
       pnormq = normqlg2 + i;
       idxbuf[i]     = *pidx;
       normbuf[i]    = *pnormq;
       pidx += 2;
       pnormq += 2;
       idxbuf[21-i]  = *pidx;
       normbuf[21-i] = *pnormq;
       pidx += 2;
       pnormq += 2;
       idxbuf[22+i]  = *pidx;
       normbuf[22+i] = *pnormq;
       pidx += 2;
       pnormq += 2;
       idxbuf[43-i]  = *pidx;
       normbuf[43-i] = *pnormq;
       pidx += 2;
       pnormq += 2;
       idxbuf[i+2]   = *pidx;
       normbuf[i+2]  = *pnormq;
       pidx += 2;
       pnormq += 2;
       idxbuf[19-i]  = *pidx;
       normbuf[19-i] = *pnormq;
       pidx += 2;
       pnormq += 2;
       idxbuf[24+i]  = *pidx;
       normbuf[24+i] = *pnormq;
       pidx += 2;
       pnormq += 2;
       idxbuf[41-i]  = *pidx;
       normbuf[41-i] = *pnormq;
    }

    pidx = ynrm + 16;
    pnormq = normqlg2 + 16;
    for (i=4; i<(NB_SFM/4); i++)
    {
       idxbuf[i]    = *pidx++;
       idxbuf[21-i] = *pidx++;
       idxbuf[22+i] = *pidx++;
       idxbuf[43-i] = *pidx++;
       normbuf[i]    = *pnormq++;
       normbuf[21-i] = *pnormq++;
       normbuf[22+i] = *pnormq++;
       normbuf[43-i] = *pnormq++;
    }
   
    return;
}
