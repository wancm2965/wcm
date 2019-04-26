/*
 * vad.cpp
 * 
 * This is a simple vad implementation.  It isn't tuned and testing
 * has been somewhat limited... but it seems to work ok.  All numbers
 * that are db are multiplied by 100 to keep them slightly more
 * accurate and easy to use. (so -7450 is -74.50db)  This code use
 * three functions not provided as part of this file : log10_32, bqInit
 * and bqProcess.  The log function calculates :
 *     100 * 10 * log(x) - 9333
 * which sould be an approximation of the signal energy x 100.
 * bqInit initializes a biquad data structure and bqProcess applies a
 * a biquad to a signal buffer.  A biquad is used by the vad to filter
 * out lower frequency background noise.
 * 
 * This code operates under the assumption that it will get frames with
 * lengths which are a multiple of 5ms.  This is relatively easy to 
 * change  since the only real dependency is the hangover count -- its in
 * frames now but could easily be changed to samples.
 * 
 */

#define VAD_CPP

#include "c_utils.h"
#include "c_vad.h"
 

#define DATA_FRAME_LENGTH    (5*8)

/* 
 * x = 1.5sec * 1000ms/sec * 8 samples/1 ms
 * samples => +1dB every x samples 
 */
#define VAD_NOISEFLOOR_CNT_INIT      (int)(8*1500)
#define VAD_SIGNALMAX_CNT_INIT       (int)(8*1500)

/* Power Thresholds */
#define VAD_NOISE_TH_BASE        (float) 10.00   /*  10.00 dB Noise Threshold */
#define VAD_NOISE_FLOOR_INIT     (float)-74.00   /* -74.00 dB Initial Noise Floor */
#define VAD_SIGNAL_MAX_INIT      (float)-80.00   /* -80.00 dB Initial Noise Max */
#define VAD_NOISE_TH_MIN         (float)  1.00   /*   1.00 dB Minimum Noise Threshold */

/* High Pass Filter for getting rid of background noise from
 * input signal before energy calculations */
/* Butter : */
#define vhpfB0   (short) 14339
#define vhpfB1   (short)-28678
#define vhpfB2   (short) 14339
#define vhpfA1   (short)-28422
#define vhpfA2   (short) 12550

/* Number of samples of silence before we declare silence period */
/* #samples = 8 samples/ms * 500ms */            
#define VAD_HANGOVER_CNT_INIT        (int)(8*500)


void vadInit(t_vad*pVAD)
{ 
  pVAD->enabled          = TRUE;
  pVAD->sta              = 10000;
  pVAD->noiseTH          = VAD_NOISE_TH_BASE;
  pVAD->state            = VadState_Unknown;
  pVAD->noiseFloorCnt    = VAD_NOISEFLOOR_CNT_INIT;      
  pVAD->noiseFloor       = VAD_NOISE_FLOOR_INIT;                         
  pVAD->hangoverCnt      = VAD_HANGOVER_CNT_INIT;
  pVAD->STARise          = 1;                                
  
  pVAD->stateTxCount     = 0;

  pVAD->signalMax        = VAD_SIGNAL_MAX_INIT;
  pVAD->signalMaxCnt     = VAD_SIGNALMAX_CNT_INIT;
    
  bqInit(&pVAD->bq, vhpfB0, vhpfB1, vhpfB2, vhpfA1, vhpfA2);
}
   
static unsigned long
computeSTA(t_vad*pVAD,short *pdata, int length, unsigned long *minSta)
{
  int  i;
  long  acc0,acc1;
  unsigned long  maxSta;
  
  *minSta = pVAD->sta;
  maxSta = pVAD->sta;
  
  for (i = 0; i < length; i++)
    {
      
      /* q.15 * q.15 = q.30 */       
      acc1  = pdata[i] * pdata[i];
      
      if ( pVAD->STARise )
        {
          acc0  = -1 * (long)(pVAD->sta >> 6);
          acc1  = acc1 >> 5;
        }
      else
        {
          acc0  = -1 * (long)(pVAD->sta >> 9);
          acc1  = acc1 >> 8;
        } /* if */
          
      acc0 += acc1;
      pVAD->STARise = ( 0 >= acc0 ) ? 0 : 1;
      pVAD->sta += acc0;      

      if ( pVAD->sta > maxSta )
        {
          maxSta = pVAD->sta;	// arijit - i added the cast
        }
      else if ( pVAD->sta < *minSta )
        {
          *minSta = pVAD->sta;
        }
                
    } /* for */
  
  return maxSta;
  
}

static void 
computeNFE(t_vad*pVAD,float minpower, float maxpower, int length)
{
    
  if ( minpower <= pVAD->noiseFloor ) 
    { 
      pVAD->noiseFloor = minpower;
      pVAD->noiseFloorCnt = VAD_NOISEFLOOR_CNT_INIT;
    }
  else
    { 
      if ( pVAD->noiseFloorCnt < length )
        {
          pVAD->noiseFloor += 1;
          pVAD->noiseFloorCnt = 
            (VAD_NOISEFLOOR_CNT_INIT + pVAD->noiseFloorCnt - length);
        }
      else
        {
          pVAD->noiseFloorCnt -= length;
        }
    }
 
}

unsigned long stopCount = 32000;

static unsigned char
vadSubProcess(t_vad*pVAD,short *data, int length)
{
  unsigned char  SpeechDetected;
  unsigned char  FrameSpeechFlag;
  short   tmpData[DATA_FRAME_LENGTH];
  unsigned long   sta[2];
  float    power[2];
  static unsigned long count = 0;
  
  SpeechDetected  = TRUE;
  FrameSpeechFlag = FALSE;
  
  bqProcess(&pVAD->bq, data, tmpData, length);
 
  sta[1] = computeSTA(pVAD,tmpData, length, &sta[0]);
  
  calcPower(2, sta, power);
   
  computeNFE(pVAD,power[0], power[1], length);

  count += length;
  if (count >= stopCount)
	  {
		  count = 0;
	  }

  if (power[1] > (pVAD->noiseFloor + pVAD->noiseTH)) 
    { 
      FrameSpeechFlag = TRUE;  
    } 
          
  if  ( FrameSpeechFlag == FALSE) 
    { 
      if ( pVAD->hangoverCnt < length ) 
        {
          SpeechDetected  = FALSE;
          pVAD->hangoverCnt = 0;
		  if ( pVAD->state != VadState_Silence )
		  {
			  pVAD->stateTxCount++;
		  }
          pVAD->state = VadState_Silence;
        }
      else
        { 
          pVAD->hangoverCnt -= length;
        }
    }
  else 
    {
      pVAD->hangoverCnt = VAD_HANGOVER_CNT_INIT;
	  if ( pVAD->state == VadState_Silence )
		  {
			  pVAD->stateTxCount++;
		  }
      pVAD->state = VadState_Speech;
    }
      
  return SpeechDetected;
  
}

/* 
 * Returns:  true for speech
 *          false for silence 
 */  
unsigned char
vadProcess(t_vad*pVAD,short *data, int length)
{ 

  /* vadProcess locals */
  int idx;
  int step;
  unsigned char ret;

  ret = FALSE;
    
  if ( pVAD->enabled == TRUE )
    {

	  /* Cut up the frame into 5ms chunks for processing purposes */
      for (idx = 0; length > 0; length -= step)
        {  
		  step = (length < DATA_FRAME_LENGTH) ? length : DATA_FRAME_LENGTH;
          ret |= vadSubProcess(pVAD,&data[idx], step);
          idx += step;
        }

  }
  else
    {
      ret = TRUE;
    }
    
  return ret;

}


