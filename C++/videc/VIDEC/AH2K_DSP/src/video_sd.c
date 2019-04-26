/***********************************************************
*	
*	Copyright (C) 2004 Intelligent Motion Co. Ltd(IMDSP)
*                All Rights Reserved
* ========================================================
*	File Name: 		video_sd.c
*	Project Name:	IMDM642 Video Port Libary	
*	Description:	IMDM642 Video Port example code	
*					Fill the display buffer for dispaly bar or 
*					get the image data from vedio port, and loopback
*	Create Time:	2004.6.20
*	Last Modify:	2008.8.12
***********************************************************/
#include <std.h>
#include <stdio.h>

#include <tsk.h>
#include <csl.h>
#include <csl_dat.h>
#include <csl_cache.h>
#include <csl_i2c.h>
#include <csl_vp.h>
#include <assert.h>

#include "vport.h"
#include "vportcap.h"
#include "tvp5150.h" 
#include "vcapparams.h"
#include "vcapchan.h"
#include "i2c.h"
#include "hdedc.h"

GIO_Attrs sd_attrs = { 2, 400 /*SYS_FOREVER*/};

TSK_CAP *cap_open(Int id, VIDEOINFO *video, Ptr setting)
{
	I2C_INFO i2c;
	TSK_CAP *cap;
	char name[20];
	Int status;

	cap = (TSK_CAP *)MEM_alloc(EXTERNALHEAP, sizeof(TSK_CAP), 32);
	assert(cap != MEM_ILLEGAL);
	sprintf(name, "/VP%dCAPTURE/%c/%d", video->idPort, video->idChannel, id);
	cap->runStatus = CAPTURE_STOP;
	cap->handle  = FVID_create(name, IOM_INPUT, &status, 
							setting, &sd_attrs);

	i2c.mode = video->i2cType;
	i2c.addr = video->i2cAddr;
	if (i2c.mode == I2CMODE_SW)
	{
		i2c.pinSCL = video->i2cScl;
		i2c.pinSDA = video->i2cSda;
	}

	FVID_control(cap->handle, VPORT_CMD_EDC(INIT), (Ptr)&i2c);
	return cap;
}

void cap_stop(TSK_CAP *cap)
{
	FVID_free(cap->handle, &cap->frame);
	cap->frame = NULL;

	FVID_control(cap->handle, VPORT_CMD_EDC(RESET), INV);
	FVID_control(cap->handle, VPORT_CMD_STOP, INV); 
	cap->runStatus = CAPTURE_STOP;
}

/*
 * ======== tskVideo ========
 * video loopback function.
 */
void tskVideo(CHANNAL_DESCRIPT * chan)
{
	Int status;
	Uint8 frame_rate = 25;
	Int video_mode, current_video_mode;
	TSK_CAP *cap;

	chan->vi = vid_chan_init(chan->index, 720, 600, chan->video->numPCIControl);

	vCapParamsChan_5150.segId = EXTERNALHEAP;
	// modified by chenzhenbao
	//vCapParamsChan_5150.bufSize = 720 * 600 * 2;
	vCapParamsChan_5150.bufSize = 720 * 600 * 5/2+128;
    vCapParamsChan_5150.m_420   = 0   ;
	//vCapParamsChan_5150.frmDrop = (chan->video->limitFrame!=0)? 1:0;
// add by chenzhenbao
	  vCapParamsChan_5150.frmDrop = 0;
	cap = cap_open(chan->index, chan->video, (Ptr)&vCapParamsChan_5150);

	while (1)
	{
		switch (cap->runStatus)
		{
		case CAPTURE_STOP:
			if (!chan->vi->flag_enable_cap)
			{
				TSK_sleep(500);
				break;
			}

			FVID_control(cap->handle, VPORT_CMD_EDC(GETINPUT), (Ptr)(&video_mode));
			if (video_mode == MODE_NONE)
			{
				TSK_sleep(1000);
				break;
			}

			cap->runStatus = CAPTURE_START;
			break;
		case CAPTURE_START:
			FVID_control(cap->handle, VPORT_CMD_EDC(RESET), NULL);

			TSK_sleep(500);

			FVID_control(cap->handle, VPORT_CMD_EDC(GETINPUT), (Ptr)(&video_mode));
		
			if (video_mode == MODE_PAL || video_mode == MODE_SECAM)
			{
				vCapParamsChan_5150.fldXStrt1 =
				vCapParamsChan_5150.fldXStrt2 = 8;

				vCapParamsChan_5150.fldXStop1 = 
				vCapParamsChan_5150.fldXStop2 = 704 + vCapParamsChan_5150.fldXStrt1 - 1;

				vCapParamsChan_5150.fldYStrt1 = 
				vCapParamsChan_5150.fldYStrt2 = 1;

				vCapParamsChan_5150.fldYStop1 = 
				vCapParamsChan_5150.fldYStop2 = 288 + vCapParamsChan_5150.fldYStrt1 - 1;

				chan->vi->enc_width = 704;
				chan->vi->enc_height = 576;
				vCapParamsChan_5150.thrld = chan->vi->enc_height >> 3;
				frame_rate = 25;
			}
			else if(video_mode==MODE_NTSC)
			{
				vCapParamsChan_5150.fldYStop1 = 240 + 1;
				vCapParamsChan_5150.fldYStop2 = 240 + 1;

				chan->vi->enc_width = 704;
				chan->vi->enc_height = 480;
				frame_rate = 30;
			}
			else
			{
			    
				FVID_control(cap->handle, VPORT_CMD_EDC(RESET), NULL);

			}

// add by chenzhenbao
            vCapParamsChan_5150.m_420 = 1;
            chan->vi->m_420            = 1; 
			vCapParamsChan_5150.frmDrop =0;
			

			chan->vi->vp_chan_interlaced = 0;
			FVID_control(cap->handle, VPORT_CMD_CONFIG_CHAN, (Ptr)&vCapParamsChan_5150);
			TSK_sleep(200);
			FVID_control(cap->handle, VPORT_CMD_START, NULL);
			TSK_sleep(200);
			status = FVID_alloc(cap->handle, &cap->frame);
			if ( status != 0)
			{
				//FVID_control(cap->handle, VPORT_CMD_EDC(RESET), NULL);
				cap_stop(cap);

				insert_null_image(chan->vi,0,0,0,320,240);

				//TSK_yield();			
				break;
			}

			chan->vi->video_mode = video_mode;
			cap->runStatus = CAPTURE_RUN;
			break;

		case CAPTURE_RUN:
			if (!chan->vi->flag_enable_cap)
			{
				cap_stop(cap);			
				break;
			}
			
			//NTSC and PAL exchange
			FVID_control(cap->handle, VPORT_CMD_EDC(GETINPUT), (Ptr)(&current_video_mode));
			if(current_video_mode!=video_mode)
			{
				cap_stop(cap);
				
				chan->vi->video_mode = video_mode=current_video_mode;
				
				break;
			}

			status = FVID_exchange(cap->handle, &cap->frame);
			if ( status != 0)
			{
				//FVID_control(cap->handle, VPORT_CMD_EDC(RESET), NULL);
				cap_stop(cap);				
				break;
			}
//add by chenzhenbao
if(vCapParamsChan_5150.m_420==0)
{
            printf("the control error,please debug!\n");
			return ;
}
else
{
            chan->vi->m_yIn = cap->frame->frame.iFrm.y1;
			chan->vi->m_uIn = cap->frame->frame.iFrm.cb1;
			chan->vi->m_vIn = cap->frame->frame.iFrm.cr1;

            vid_chan_process(chan->vi,
							(Uint32)cap->frame->frame.iFrm.y1,	
							(Uint32)cap->frame->frame.iFrm.cb1,
							(Uint32)cap->frame->frame.iFrm.cr1,
							frame_rate,
								PORT_TYPE_CVBS,chan->vi->enc_width,chan->vi->enc_height,0);              


			
			cap->frame->frame.iFrm.y1  = chan->vi->m_yIn;
			cap->frame->frame.iFrm.cb1 = chan->vi->m_uIn;
			cap->frame->frame.iFrm.cr1 = chan->vi->m_vIn;

            cap->frame->frame.iFrm.y2  =(char *) chan->vi->m_yIn+chan->vi->enc_width;
			cap->frame->frame.iFrm.cb2 = chan->vi->m_uIn;
			cap->frame->frame.iFrm.cr2 = chan->vi->m_vIn;
}


			break;
		}

		TSK_sleep(1);

	}
}

