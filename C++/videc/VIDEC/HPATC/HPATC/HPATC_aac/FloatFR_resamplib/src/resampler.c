/*
  IIR resampler tool box
*/

#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include "resampler.h"

#include "counters.h" /* the 3GPP instrumenting tool */

#define MAX_COEFF 32


struct IIR_PARAM{
  const float *coeffIIRa;
  const float *coeffIIRb;
  int noOffCoeffs;
  int transitionFactor;
  int delay;
};

static const float set1_a[] = {
  0.004959855f,   0.025814206f,   0.080964205f,   0.182462303f,
  0.322109621f,   0.462709927f,   0.552160404f,   0.552160404f,
  0.462709927f,   0.322109621f,   0.182462303f,   0.080964205f,
  0.025814206f,   0.004959855f
};

static const float set1_b[] = {
  0.0f,          -1.038537170f,   2.627279635f,  -1.609574122f,
  2.205922661f,  -0.751928739f,   0.787128253f,  -0.105573173f,
  0.131638380f,   0.003884641f,   0.010544805f,   0.001232040f,
  0.000320798f,   0.000023031f
};
  
  
static struct IIR_PARAM const set1 = {
  set1_a,
  set1_b,
  14,
  218,
  6
};

static const float set2_a[] = {
  6.1256250262e-03f, 5.0336404692e-02f, 2.3239261027e-01f, 7.6491041063e-01f, 
  1.9749759293e+00f, 4.2014377791e+00f, 7.5819469340e+00f, 1.1822456931e+01f, 
  1.6119323131e+01f, 1.9361556005e+01f, 2.0572170174e+01f, 1.9361556005e+01f, 
  1.6119323131e+01f, 1.1822456931e+01f, 7.5819469340e+00f, 4.2014377791e+00f, 
  1.9749759293e+00f, 7.6491041063e-01f, 2.3239261027e-01f, 5.0336404692e-02f, 
  6.1256250262e-03f
};
static const float set2_b[] = {
  0.0000000000e+00f, 1.9235108280e+00f, 5.7396121787e+00f, 8.8179912480e+00f, 
  1.4411789276e+01f, 1.7639247429e+01f, 2.0464585871e+01f, 2.0057402729e+01f, 
  1.7946428788e+01f, 1.4126496496e+01f, 1.0005961282e+01f, 6.2906792152e+00f, 
  3.5176830490e+00f, 1.7318682106e+00f, 7.4410297318e-01f, 2.7455714771e-01f, 
  8.5087571958e-02f, 2.1389243840e-02f, 4.1181257358e-03f, 5.4450709008e-04f, 
  3.7523290344e-05f
};

static struct IIR_PARAM const set2 = {
  set2_a,
  set2_b,
  21,
  82,
  8
};

static const float set3_a[] = {
  6.7256683321e-03f, 4.7346727920e-02f, 1.9033982920e-01f,
  5.4665226062e-01f, 1.2299552614e+00f, 2.2705899171e+00f, 3.5328440872e+00f,
  4.7078084067e+00f, 5.4230280330e+00f, 5.4230280330e+00f, 4.7078084067e+00f,   
  3.5328440872e+00f, 2.2705899171e+00f, 1.2299552614e+00f, 5.4665226062e-01f,
  1.9033982920e-01f, 4.7346727920e-02f, 6.7256683321e-03f
};

static const float set3_b[] = {
  0.0000000000e+00f, 8.9771105818e-01f, 3.5968768784e+00f,
  3.3337572583e+00f, 5.7999227302e+00f, 5.0357982121e+00f, 5.3922055377e+00f,
  4.0519786307e+00f, 3.0679045209e+00f, 1.8796348932e+00f, 1.0552130903e+00f,
  5.0000450703e-01f, 2.0536622945e-01f, 7.0057431346e-02f, 1.9404459650e-02f,
  4.1034042582e-03f, 5.9630785597e-04f, 4.5233505774e-05f
};

static struct IIR_PARAM const set3 = {
  set3_a,
  set3_b,
  18,
  106,
  7
};

static struct IIR_PARAM const *iir_paramSet[] ={
  &set2,&set3,&set1
};



/*
  Reset downsampler instance and clear delay lines     
  
  returns status
*/
int 
InitIIR21_Resampler(IIR21_RESAMPLER *ReSampler,
                    int transitionFactor,
                    int ratio)
     
{
  unsigned int i;
  struct IIR_PARAM const *currentSet=NULL;

  COUNT_sub_start("InitDownsampler");

  PTR_INIT(1); /* counting previous operation */

  assert(ratio == 2);

  FUNC(2); LOOP(1); PTR_INIT(1); MOVE(1); STORE(sizeof(ReSampler->iirFilter.ring_buf_1));
  memset(ReSampler->iirFilter.ring_buf_1,0,sizeof(ReSampler->iirFilter.ring_buf_1));
 
  FUNC(2); LOOP(1); PTR_INIT(1); MOVE(1); STORE(sizeof(ReSampler->iirFilter.ring_buf_2));
  memset(ReSampler->iirFilter.ring_buf_2,0,sizeof(ReSampler->iirFilter.ring_buf_2));

  INDIRECT(1); MOVE(1);
  ReSampler->iirFilter.ptr   =   0;
 
  MOVE(1);
  currentSet =  iir_paramSet[0];

  PTR_INIT(1); /* iir_paramSet[] */
  LOOP(1);
  for(i=0;i<sizeof(iir_paramSet)/sizeof(struct IIR_PARAM *);i++){

    ADD(1); BRANCH(1);
    if(iir_paramSet[i]->transitionFactor <= transitionFactor){

      MOVE(1);
      currentSet = iir_paramSet[i];
    }
  }
    
  INDIRECT(10); MOVE(5);
  ReSampler->iirFilter.coeffIIRa = currentSet->coeffIIRa;
  ReSampler->iirFilter.coeffIIRb = currentSet->coeffIIRb;
  ReSampler->iirFilter.noOffCoeffs = currentSet->noOffCoeffs;
  ReSampler->delay=currentSet->delay;
  ReSampler->transitionFactor = currentSet->transitionFactor;


  assert(ReSampler->iirFilter.noOffCoeffs <= BUFFER_SIZE);

  INDIRECT(1); MOVE(1);
  ReSampler->ratio =   ratio;

  INDIRECT(1); ADD(1); STORE(1);
  ReSampler->pending = ratio-1;

  COUNT_sub_end();

  return 1;
}



/* 
   NOTE: enabling NEWIIR would save us some wMOPS, but result in 1 LSB diffs, it is worth a CR
*/

#ifdef NEWIIR
static float
AdvanceMAFilter( IIR_FILTER *iirFilter
                 )
{
  float y;
  int j;
  int ptr = iirFilter->ptr;
  int i = ptr + (BUFFER_SIZE-1);

  COUNT_sub_start("AdvanceMAFilter");

  INDIRECT(1); /* MOVE(1); --> ptr isn't needed */ ADD(1); /* counting previous operations */

  INDIRECT(2); MULT(1);
  y = (iirFilter->coeffIIRa[0] * iirFilter->ring_buf_2[i & (BUFFER_SIZE-1)]);

  PTR_INIT(3); /* iirFilter->noOffCoeffs
                  iirFilter->coeffIIRa[]
                  iirFilter->ring_buf_2[]
               */
  LOOP(1);
  for (j=1; j<iirFilter->noOffCoeffs; j++) {
    i--;

    MAC(1);
    y += (iirFilter->coeffIIRa[j] * iirFilter->ring_buf_2[i & (BUFFER_SIZE-1)]);
  }

  COUNT_sub_end();

  return y;
}


static void
AdvanceARFilter( IIR_FILTER *iirFilter,
                 float input
                 )

{
  int j;
  float y;
  int ptr = iirFilter->ptr;
  int i = ptr + (BUFFER_SIZE-1);

  COUNT_sub_start("AdvanceARFilter");

  INDIRECT(1); MOVE(1); ADD(1); /* counting previous operations */

  INDIRECT(2); MULT(1); ADD(1);
  y = input + (iirFilter->coeffIIRb[1] * (-iirFilter->ring_buf_2[i & (BUFFER_SIZE-1)]));

  PTR_INIT(4); /* iirFilter->noOffCoeffs
                  iirFilter->coeffIIRb[]
                  iirFilter->ring_buf_2[i]
                  iirFilter->ring_buf_2[ptr]
               */

  LOOP(1);
  for (j=2; j<iirFilter->noOffCoeffs; j++) {
    i--;

    MULT(1); MAC(1);
    y += (iirFilter->coeffIIRb[j] * (-iirFilter->ring_buf_2[i & (BUFFER_SIZE-1)]));
  }

  MOVE(1);
  iirFilter->ring_buf_2[ptr] = y;

  /* pointer update */
  iirFilter->ptr = (ptr+1) & (BUFFER_SIZE-1);

  COUNT_sub_end();

}
#else  //NEWIIR



/*
  faster simple folding operation    

  returns filtered value
*/


static float
AdvanceIIRFilter(IIR_FILTER *iirFilter,
                 float input
                 )

{
  float y = 0.0f;
  int j = 0;
  int i;

  COUNT_sub_start("AdvanceIIRFilter");

  MOVE(2); /* counting previous operations */

  INDIRECT(1); MOVE(1);
  iirFilter->ring_buf_1[iirFilter->ptr] = input;

  PTR_INIT(4); /* pointer for iirFilter->ring_buf_1,
                              iirFilter->ring_buf_2,
                              iirFilter->coeffIIRa,
                              iirFilter->coeffIIRb
               */
  ADD(1); LOOP(1);
  for (i = iirFilter->ptr; i > iirFilter->ptr - iirFilter->noOffCoeffs; i--, j++) {
    MULT(2); ADD(1);
    y += iirFilter->coeffIIRa[j] * iirFilter->ring_buf_1[i & (BUFFER_SIZE - 1)] - iirFilter->coeffIIRb[j] * iirFilter->ring_buf_2[i & (BUFFER_SIZE - 1)];
  }

  MOVE(1);
  iirFilter->ring_buf_2[(iirFilter->ptr) & (BUFFER_SIZE - 1)] = y;

  iirFilter->ptr = ++iirFilter->ptr & (BUFFER_SIZE - 1);

  COUNT_sub_end();

  return y;
}
#endif  //NEWIIR






/*
  Downsample numInSamples of type short

  returns  success of operation
*/

int 
IIR21_Downsample(IIR21_RESAMPLER *DownSampler,
                 float *inSamples,
                 int numInSamples,
                 int inStride,
                 float *outSamples,
                 int *numOutSamples,
                 int outStride
               )
{
  int i;
  *numOutSamples=0;

  COUNT_sub_start("Downsample");

  MOVE(1); /* counting previous operations */

  PTR_INIT(2); /* pointer for inSamples[],
                              outSamples[]
               */
  LOOP(1); 
  for(i=0;i<numInSamples;i++){
    float iirOut;

#ifdef NEWIIR
    FUNC(2);
    AdvanceARFilter(&(DownSampler->iirFilter), inSamples[i*inStride]);
#else
    FUNC(2);
    iirOut = AdvanceIIRFilter(&(DownSampler->iirFilter), inSamples[i*inStride]);
#endif

    ADD(1);
    DownSampler->pending++;

    ADD(1); BRANCH(1);
    if(DownSampler->pending == DownSampler->ratio){

#ifdef NEWIIR
      FUNC(1);
      outSamples[(*numOutSamples)*outStride] = AdvanceMAFilter(&(DownSampler->iirFilter));;
#else
      MOVE(1);
      outSamples[(*numOutSamples)*outStride] = iirOut;
#endif

      (*numOutSamples)++;

      MOVE(1);
      DownSampler->pending=0;
    }
  }

  COUNT_sub_end();

  return 1;
}


int 
IIR21_Upsample(IIR21_RESAMPLER *UpSampler,
               float *inSamples,
               int numInSamples,
               int inStride,
               float *outSamples,
               int *numOutSamples,
               int outStride
               )
{
  int i,k;
  int idxOut=0;

  COUNT_sub_start("Upsample");

  MOVE(1); /* counting previous operations */

  PTR_INIT(2); /* pointer for inSamples[],
                              outSamples[]
               */
  LOOP(1); 
  for(i=0;i<numInSamples;i++){

    MULT(1); FUNC(2); STORE(1);
    outSamples[idxOut] = AdvanceIIRFilter(&(UpSampler->iirFilter), inSamples[i*inStride] * UpSampler->ratio);

    idxOut += outStride;

    LOOP(1);
    for (k=0; k<UpSampler->ratio-1; k++) {

      FUNC(2); STORE(1);
      outSamples[idxOut] = AdvanceIIRFilter(&(UpSampler->iirFilter), 0.0f);

      idxOut += outStride;
    }      
  }

  MULT(1); STORE(1);
  *numOutSamples=numInSamples*UpSampler->ratio;

  COUNT_sub_end();

  return 1;
}
