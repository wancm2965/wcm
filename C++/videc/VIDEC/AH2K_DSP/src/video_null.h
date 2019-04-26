/********************************************************************************/
/*	video_null.h file                                                           */
/********************************************************************************/
#ifndef _VIDEO_NULL_H
#define _VIDEO_NULL_H

#include "vcapchan.h"

#define SIG_TEMPLATE_WIDTH	176
#define SIG_TEMPLATE_HEIGHT	32
extern Uint8 number[10][32*16*2];
extern Uint8 sig_template[2][SIG_TEMPLATE_WIDTH * SIG_TEMPLATE_HEIGHT * 2];
void insert_null_image(PVID_CHAN_INFO pvi, Uint8 index,Int  lcf, Uint32 frame_rate, Uint16 nInputWidth, Uint16 nInputHeight);

#endif

