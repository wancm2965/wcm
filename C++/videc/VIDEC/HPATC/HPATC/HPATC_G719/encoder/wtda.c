/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/cnst.h"
#include "../include/rom.h"
 
/*--------------------------------------------------------------------------*/
/*  Function  wtda                                                          */
/*  ~~~~~~~~~~~~~~                                                          */
/*                                                                          */
/*  Windowing and Time Domain Aliasing                                      */
/*--------------------------------------------------------------------------*/
/*  float      new_audio[]   (i)    input audio                             */
/*  float      wtda_audio[]  (o)    windowed audio                          */
/*  float      old_wtda[]    (i/o)  windowed audio from previous frame      */
/*--------------------------------------------------------------------------*/
void wtda(float new_audio[], 
          float wtda_audio[],
          float old_wtda[])
{
   short i;   
   float *x1,*x2;
   float *y1,*y2;
   float *c ,*s;
   
   /* Second half */
   for(i = 0; i < FRAME_LENGTH / 2 ; i++) 
   {
		wtda_audio[FRAME_LENGTH/2 + i] =  old_wtda[i];                        
	}   

   /* First half */         
   y1 = wtda_audio;
   y2 = old_wtda + FRAME_LENGTH / 2 - 1;

   x1 = new_audio + FRAME_LENGTH/2;
   x2 = new_audio + FRAME_LENGTH / 2 - 1;

   c =  window_G719 + FRAME_LENGTH/2  - 1;
   s =  window_G719 + FRAME_LENGTH/2;

	for(i = 0; i < FRAME_LENGTH / 2 ; i++) 
   {
      *y2    =    *c * *x2 - *s  * *x1 ;
		*y1    =    -*s * *x2 - *c  * *x1 ;   
      
      y1++;
      y2--;
      x1++;
      x2--;
      c--;
      s++;
   }

}
