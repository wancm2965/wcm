
#ifndef wb_vad_h
#define wb_vad_h

/******************************************************************************
*                         INCLUDE FILES
******************************************************************************/
//#include "typedef.h"


#include "wb_vad_c.h"

/******************************************************************************
*                         DEFINITION OF DATA TYPES
******************************************************************************/



typedef struct
{
	float bckr_est[COMPLEN];    /* background noise estimate */
	float ave_level[COMPLEN];   /* averaged input components for stationary */
	/* estimation */
	float old_level[COMPLEN];   /* input levels of the previous frame */
	float sub_level[COMPLEN];   /* input levels calculated at the end of a frame (lookahead) */
	float a_data5[F_5TH_CNT][2];        /* memory for the filter bank */
	float a_data3[F_3TH_CNT];   /* memory for the filter bank */

	short burst_count;         /* counts length of a speech burst */
	short hang_count;          /* hangover counter */
	short stat_count;          /* stationary counter */

	/* Note that each of the following two variables holds 15 flags. Each flag reserves 1 bit of the variable. The newest flag is
	* in the bit 15 (assuming that LSB is bit 1 and MSB is bit 16). */
	short vadreg;              /* flags for intermediate VAD decisions */
	short pitch_tone;          /* flags for pitch and tone detection */

	short sp_est_cnt;          /* counter for speech level estimation */
	float sp_max;               /* maximum level */
	short sp_max_cnt;          /* counts frames that contains speech */
	float speech_level;         /* estimated speech level */
	double prev_pow_sum;        /* power of previous frame */

	float level[COMPLEN];
	float prevLevel[COMPLEN];

	int	  frame_size;	// add by ryan liu
	float * tmp_buf;

	//xmc add
	float*in_buf;
	unsigned long	m_nDelayCount;
} VadVars;

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
#ifdef __cplusplus
extern "C" {
#endif

int wb_vad_init(VadVars ** st , int frameSize);
int wb_vad_reset(VadVars * st);
void wb_vad_exit(VadVars ** st);
void wb_vad_pitch_tone_detection(VadVars * st, float p_gain);
short wb_vad(VadVars * st, float in_buf[]);

//xmc add
short wb_vad_ext(VadVars * st, short in_buf[]);

#ifdef __cplusplus
}
#endif


#endif
