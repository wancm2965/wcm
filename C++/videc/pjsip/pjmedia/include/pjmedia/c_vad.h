#ifndef __C_VAD_H__
#define __C_VAD_H__

#include <stdio.h>
#include <stdlib.h>

#include "c_types.h"
#include "c_utils.h"

typedef enum {
  VadState_Silence = 0,
  VadState_Speech,
  VadState_Unknown
} t_VadState;

struct _vad {
  
  unsigned char       enabled;
  
  /* Saved STA between input frames */
  unsigned long     sta;

  /* state == 1 if VOICE
   * state == 0 if SILENCE */
  t_VadState state;
  
  /* Countdown of consecutive frames before we declare silence */
  int     hangoverCnt;

  /* Threshold above which a signal is considered to be speech */
  float      noiseTH;

  /* Countdown after which the noise floor is 
   * incremented by 1dB */
  int     noiseFloorCnt;
  
  /* Noise floor in dB */
  float      noiseFloor;
  
  /* Countdown after which the signal max is
   * decremented by 1dB */
  int     signalMaxCnt;

  /* Signal max in dB */
  float      signalMax;  
    
  /* STARise == 1 if sta is rising
   * STARise == 0 if sta is falling */   
  int     STARise;

  int     stateTxCount;
  
  /* High Pass Filter for input signal */
  t_biquad  bq;
    
};

typedef struct _vad t_vad;

#ifdef __cplusplus
extern "C" {
#endif

void vadInit(t_vad*pVAD);
unsigned char vadProcess(t_vad*pVAD,short *data, int length);

#ifdef __cplusplus
}
#endif

#endif
