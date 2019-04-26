/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/proto.h"
#include "../include/cnst.h"
#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  qcoefs                                                        */
/*  ~~~~~~~~~~~~~~~~                                                        */
/*                                                                          */
/*  Vector quantization for normalized MLT coefficients                     */
/*--------------------------------------------------------------------------*/
/*  float     *coefs  (i)   normalized MLT coefficients                     */
/*  short     *R      (i)   number of bits per coefficient                  */
/*  short     N1      (i)   beginning sub-vector's number in the groups     */
/*  short     N2      (i)   ending sub-vector's number in the group         */
/*  short     L       (i)   number of coefficients in each sub-vector       */
/*  short     *y      (o)   indices of the selected codevectors             */
/*--------------------------------------------------------------------------*/
void qcoefs(float *coefs, short *R, short N1, short N2, short L, short *y)
{
    short i, j, n, v, rv;
    short nb_vecs;
    float temp;
    float x[8];
    short xx[8];
    short *pidx;
    float *pcoefs;


    pidx = y;
    pcoefs = coefs;
    nb_vecs = L >> 3;
    for (n=N1; n<N2; n++)
    {
       v = R[n];
       if (v>1)
       {
         rv = RV[v];
         for (i=0; i<nb_vecs; i++)
         {
            for (j=0; j<8; j++)
            {
               temp = ((*pcoefs++) - OFFSETf) * FCT_LVQ2fQ13;
               if (temp > 32767.0f)
               {
                 temp = 32767.0f;
               }
               else if (temp < -32768.0f)
               {
                 temp = -32768.0f;
               }
               xx[j] = (short)temp;
            }
            lvq2(xx, pidx, rv, v);
            pidx += 8;
         }
       }
       else if (v==1)
       {
         for (i=0; i<nb_vecs; i++)
         {
            for (j=0; j<8; j++)
            {
               x[j] = ((*pcoefs++) - OFFSETf) * FCT_LVQ1f;
            }
            lvq1(x, pidx);
            pidx += 8;
         }
       }
       else
       {
         pidx += L;
         pcoefs += L;
       }
    }

    return;
}
