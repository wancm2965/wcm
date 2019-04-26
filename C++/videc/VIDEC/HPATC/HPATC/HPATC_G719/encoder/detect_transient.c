/*-----------------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                             */
/* ?2008 Ericsson AB. and Polycom Inc.                                              */
/* All rights reserved.                                                              */
/*-----------------------------------------------------------------------------------*/

#include "../include/state.h"
#include "../include/cnst.h"

/*--------------------------------------------------------------------------*/
/*  Function  hp_filter                                                     */
/*  ~~~~~~~~~~~~~~~~~~~~                                                    */
/*                                                                          */
/*  High pass filter                                                        */
/*--------------------------------------------------------------------------*/
/*  float      x      (i)    input to filter                                */
/*  float      y      (o)    output of filter                               */
/*  float      *oldy  (i/o)  previous output of filter                      */
/*  float      *oldx  (i/o)  previous input of filter                       */
/*--------------------------------------------------------------------------*/
void hp_filter(float x[], float y[], float *oldy, float *oldx)
{
	short i;

   y[0] = 0.4931f * *oldy + 0.7466f*(x[0] - *oldx);

   for(i = 1; i < FRAME_LENGTH; i++) 
   {
      y[i] = 0.4931f*y[i-1] + 0.7466f*(x[i] - x[i-1]);
   }

	*oldx = x[FRAME_LENGTH-1];
	*oldy = y[FRAME_LENGTH-1];
}

/*--------------------------------------------------------------------------*/
/*  Function  detect_transient                                              */
/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~                                              */
/*                                                                          */
/*  Detect if the signal is a transient                                     */
/*--------------------------------------------------------------------------*/
/*  float       in[]    (i)      input signal                               */
/*  CoderState  *c      (i/o)    state of coder                             */
/*--------------------------------------------------------------------------*/
/*  short       return  (o)      result of transient check                  */
/*--------------------------------------------------------------------------*/
short detect_transient(float in[],
					        CoderState *c)
{
   float Energy;
   float EnergyLT;
   short i, blk;
   short IsTransient;
   float out_filt[FRAME_LENGTH];

   IsTransient = FALSE;

   /* High pass filter */
   hp_filter(in, out_filt, &(c->old_hpfilt_in), &(c->old_hpfilt_out));

   /* Long term energy */
   EnergyLT = c->EnergyLT;

   /* Compute block Energy */
   for(blk = 0; blk < 4; blk++) 
   {
      Energy = EPSILON;

      for(i = 0; i < FRAME_LENGTH / 4; i++) 
      {
			Energy += out_filt[i + blk*(FRAME_LENGTH / 4)] * out_filt[i + blk*(FRAME_LENGTH / 4)];                    
      }

      if(Energy > 6.0f * EnergyLT) 
      {
         IsTransient = TRUE;   
      }
 
      EnergyLT = 0.75f*EnergyLT + 0.25f*Energy;            
   }

   c->EnergyLT = EnergyLT;
   
   if(IsTransient) 
   {
      /* Set Hangover */
      c->TransientHangOver = TRUE;   
   }
   else 
   {
      if( c->TransientHangOver ) 
      {
          c->TransientHangOver = FALSE;
          IsTransient          = TRUE;      
      }   
   }

   return IsTransient;
}
