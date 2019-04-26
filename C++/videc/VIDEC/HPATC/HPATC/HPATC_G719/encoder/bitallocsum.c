/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/cnst.h"

/*--------------------------------------------------------------------------*/
/*  Function  bitallocsum                                                   */
/*  ~~~~~~~~~~~~~~~~~~~~~                                                   */
/*                                                                          */
/*  Calculate the total number of bits allocated over frame                 */
/*--------------------------------------------------------------------------*/
/*  short       *R      (i)    bit-allocation vector                        */
/*  short       nb_sfm  (i)    number of sub-vectors                        */
/*  short       *sum    (o)    total number of bits allocated               */
/*--------------------------------------------------------------------------*/
void bitallocsum(short *R, short nb_sfm, short *sum)
{
   short i, temp;
   short sumg1, sumg2, sumg3, sumgx;


   temp = 0;
   for (i=0; i<SFM_G1; i++)
   {
      temp += R[i];
   }
   sumg1 = temp * WID_G1;
   temp = 0;
   for (i=SFM_G1; i<SFM_G1G2; i++)
   {
      temp += R[i];
   }
   sumg2 = temp * WID_G2;
   temp = 0;
   for (i=SFM_G1G2; i<SFM_N; i++)
   {
      temp += R[i];
   }
   sumg3 = temp * WID_G3;
   temp = 0;
   for (i=SFM_N; i<nb_sfm; i++)
   {
      temp += R[i];
   }
   sumgx = temp * WID_GX;
   *sum = sumg1 + sumg2 + sumg3 + sumgx;

   return;
}
