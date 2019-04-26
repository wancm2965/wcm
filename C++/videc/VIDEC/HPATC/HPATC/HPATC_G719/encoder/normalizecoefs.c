/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  normalizecoefs                                                */
/*  ~~~~~~~~~~~~~~~~~~~~~~~~                                                */
/*                                                                          */
/*  Normalization of MLT coefficients with quantized norms                  */
/*--------------------------------------------------------------------------*/
/*  float     *coefs       (i)   MLT coefficients                           */
/*  short     *ynrm        (i)   quantization indices for norms             */
/*  short     N1           (i)   beginning sub-vector's number              */
/*  short     N2           (i)   ending sub-vector's number                 */
/*  short     L            (i)   number of coefficients per sub-vector      */
/*  float     *coefs_norm  (o)   normalized MLT coefficients                */
/*--------------------------------------------------------------------------*/
void normalizecoefs(
   float *coefs,
   short *ynrm,
   short N1,
   short N2,
   short L,
   float *coefs_norm
)
{
   short i, j;
   float normq;
   float *pcoefs, *pcoefs_norm;


   pcoefs = coefs;
   pcoefs_norm = coefs_norm;
   for (i=N1; i<N2; i++)
   {
      normq = dicn[ynrm[i]];
      for (j=0; j<L; j++)
      {
         *pcoefs_norm++ = (*pcoefs++) / normq;
      }
   }


   return;
}
