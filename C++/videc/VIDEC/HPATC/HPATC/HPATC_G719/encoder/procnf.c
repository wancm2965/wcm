/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/proto.h"
#include "../include/cnst.h"
#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  procnf                                                        */
/*  ~~~~~~~~~~~~~~~~                                                        */
/*                                                                          */
/*  Quantization for sub-vectors originally allocated with 0 bits           */
/*--------------------------------------------------------------------------*/
/*  short     *coefs  (i)   normalized MLT coefficients                     */
/*  short     *y      (i)   indices of the selected codevectors             */
/*  short     *pbits  (i)   pointer to bitstream                            */
/*  short     nb_vecs (i)   number of 8-D vectors in current sub-vector     */
/*--------------------------------------------------------------------------*/
void procnf(float *coefs, short *y, short *pbits, short nb_vecs)
{
    short i, j;
    float x[8];

    for (i=0; i<nb_vecs; i++)
    {
       for (j=0; j<8; j++)
       {
          x[j] = ((*coefs++) - OFFSETf) * FCT_LVQ1f;
       }
       lvq1(x, y);
       idx2bitsc(y, 8, 1, pbits);
       pbits += 8;
       y += 8;
    }

    return;
}
