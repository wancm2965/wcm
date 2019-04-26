/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/cnst.h"
#include "../include/rom.h"
#include <math.h>

/*--------------------------------------------------------------------------*/
/*  Function  find_last_band                                                */
/*  ~~~~~~~~~~~~~~~~~~~~~~~~                                                */
/*                                                                          */
/*  Find the last band which has bits allocated                             */
/*--------------------------------------------------------------------------*/
/*  short     *bitalloc  (i)   bit allocation                               */
/*--------------------------------------------------------------------------*/
/*  int       return     (o)   index of last band                           */
/*--------------------------------------------------------------------------*/
short find_last_band(short *bitalloc)
{
   short sfm, last_sfm;

   last_sfm = NB_SFM;
   
   for (sfm = NB_SFM-1; sfm >= 0; sfm--) 
   {
      if (bitalloc[sfm] != 0) 
      {
         last_sfm = sfm;
         break;
      }
   }

   return last_sfm;
}

/*--------------------------------------------------------------------------*/
/*  Function  noise_adjust                                                  */
/*  ~~~~~~~~~~~~~~~~~~~~~~                                                  */
/*                                                                          */
/*  Calculate attenuation                                                   */
/*--------------------------------------------------------------------------*/
/*  float     *coeffs_norm  (i)   normalized coefficients                   */
/*  short     *bitalloc     (i)   bit allocation                            */
/*--------------------------------------------------------------------------*/
/*  short     return        (o)   index of noise attenuation                */
/*--------------------------------------------------------------------------*/
short noise_adjust(float *coeffs_norm,                    
                   short *bitalloc)
{
   short nf_idx, sfm, bin, last_sfm, num_coeffs;
   int   E, diff, tmp;

   last_sfm = find_last_band(bitalloc);

   E = 0;
   num_coeffs = 0;

   for (sfm = 0; sfm < last_sfm; sfm++)
   {
      if (bitalloc[sfm] == 0)
      {
         for (bin = sfm_start[sfm]; bin < sfm_end[sfm]; bin++)
         {
            if (coeffs_norm[bin] == 0) 
            {
               E = E - 1;
            }
            else 
            {
               tmp = (int)(floor(log10(fabs(coeffs_norm[bin]*256.0f))/0.301030f)+1);
               if (tmp < 0)
                  tmp = 0;
               E = E + tmp;
            }
                        
            num_coeffs = num_coeffs + 1;
         }

      }
   }      

   if (num_coeffs != 0) 
   {      
      E = E/num_coeffs;
   }
   else
   {
      E = 0;      
   }

   diff = 7 - E;

   if (diff >= 0)
      nf_idx = diff;
   else
      nf_idx = 0;

   if (diff <= 3)
      nf_idx = diff;
   else
      nf_idx = 3;

   return nf_idx;
}



