/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/cnst.h"
#include "../include/proto.h"
#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  direct_transform                                              */
/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~                                              */
/*                                                                          */
/*  Transformation of the signal to DCT domain                              */
/*--------------------------------------------------------------------------*/
/*  float       *in32         (i)  input signal                             */
/*  float       *out32        (o)  output transformation                    */
/*  short       is_transient  (i)  is transient                             */
/*--------------------------------------------------------------------------*/
void direct_transform(float *in32,
                      float *out32,
                      short is_transient)
{     
   short i;
   short seg;

   float *wh;
   float *wl;
   float *sh;
   float *sl;
   float *iseg;
   float *oseg;
   float dctin32[FRAME_LENGTH];   
   float in32_r16[FRAME_LENGTH];

   if (is_transient) 
   {    
      for (i = 0; i < FRAME_LENGTH/2; i++) 
      {                 
         in32_r16[i]                = in32[FRAME_LENGTH-1-i];          
         in32_r16[FRAME_LENGTH-1-i] = in32[i];
      }
      iseg = in32_r16 - MAX_SEGMENT_LENGTH/4; 
      oseg = out32;

      wh = (float *)wscw16q15 + MAX_SEGMENT_LENGTH/4;
      wl = (float *)wscw16q15 + MAX_SEGMENT_LENGTH/4 - 1;
      sh = iseg + 3*MAX_SEGMENT_LENGTH/4;
      sl = iseg + 3*MAX_SEGMENT_LENGTH/4 - 1;
      
      for (i = 0; i < MAX_SEGMENT_LENGTH/4; i++) 
      {
         dctin32[i] = ((*wl-- *  *sl--) - (*wh++ *  *sh++));
      }

      sl = iseg + MAX_SEGMENT_LENGTH/2 - 1;

      for (i = 0; i < MAX_SEGMENT_LENGTH/4; i++) 
      {
         dctin32[MAX_SEGMENT_LENGTH/4 + i] = -(*sl--);                       
      }

      dct4_240(dctin32, oseg);


      iseg = iseg + MAX_SEGMENT_LENGTH/2;                 
      oseg = oseg + MAX_SEGMENT_LENGTH/2;
                                                                       
      for (seg = 1 ; seg <  NUM_TIME_SWITCHING_BLOCKS-1; seg++) 
      {      
         wh = (float *)wscw16q15 + MAX_SEGMENT_LENGTH/4;
         wl = (float *)wscw16q15 + MAX_SEGMENT_LENGTH/4 - 1;
         sh = iseg + 3*MAX_SEGMENT_LENGTH/4;
         sl = iseg + 3*MAX_SEGMENT_LENGTH/4 - 1;

         for (i = 0; i < MAX_SEGMENT_LENGTH/4; i++) 
         {
            dctin32[i] = ((*wl-- *  *sl--) - (*wh++ *  *sh++));               
         }

         sh = iseg;
         sl = iseg + MAX_SEGMENT_LENGTH/2 - 1;
         wh = (float *)wscw16q15 + MAX_SEGMENT_LENGTH/2 - 1;
         wl = (float *)wscw16q15 + 0;

         for (i = 0; i < MAX_SEGMENT_LENGTH/4; i++) 
         {
            dctin32[MAX_SEGMENT_LENGTH/4 + i] = ((*wl++ * *sl--) + (*wh-- * *sh++));             
         }

         dct4_240(dctin32, oseg);

         iseg = iseg + MAX_SEGMENT_LENGTH/2;              
         oseg = oseg + MAX_SEGMENT_LENGTH/2; 
      } 

      sh = iseg + 3*MAX_SEGMENT_LENGTH/4 - 1;

      for (i = 0; i < MAX_SEGMENT_LENGTH /4; i++) 
      {
         dctin32[i] = -(*sh--);
      }

      sh = iseg;
      sl = iseg + MAX_SEGMENT_LENGTH/2 - 1;
      wh = (float *)wscw16q15 + MAX_SEGMENT_LENGTH/2 - 1;
      wl = (float *)wscw16q15 + 0;

      for (i = 0; i < MAX_SEGMENT_LENGTH/4; i++) 
      {   
         dctin32[MAX_SEGMENT_LENGTH/4 + i] = ((*wh-- * *sh++) + (*wl++ * *sl--));      
      }
  
      dct4_240(dctin32, oseg);
   }
   else
   {
      dct4_960(in32, out32);
   }
}
