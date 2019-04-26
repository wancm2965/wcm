/*
	last modify : 2008.7.18
*/
#include <std.h>
#include <stdio.h>
#include <fvid.h>
#include <tsk.h>
#include <csl.h>
#include <csl_dat.h>
#include <csl_cache.h>
#include <csl_i2c.h>
#include <csl_vp.h>
#include "vport.h"
#include "vportcap.h"
#include "vcapparams.h"

#include "vcapchan.h"
#include "pci_util.h"
#include "hdedc.h"
#include "i2c.h"

#define MAX_WIDTH	1920
#define MAX_HEIGHT	1080

#define DARK_VALUE   0x16

extern Uint16 idPci;
extern int g_ImgFmt;

void tskHVideo(CHANNAL_DESCRIPT *chan)
{
	TSK_CAP *cap;
	Int status = 0;
	Int input, bChange;

	PVID_CHAN_INFO ptr_vid_chan;
	Uint32 frame_rate = 30;
    Uint32 act_frame_rate = 30;
    AD9880_ConfParams timing;

    Int16 OldWidth = 0;
	Int16 OldHeight = 0;
    Int16 OldWidth720P = 0;
	Int16 OldHeight720P = 0;

// add by chenzhenbao
	Int tmpwidth;

	chan->vi = vid_chan_init(chan->index, MAX_WIDTH, MAX_HEIGHT, chan->video->numPCIControl);
	ptr_vid_chan = chan->vi;

	AH2K_vCapParamsChan.segId = EXTERNALHEAP;
	//modified by chenzhenbao
	//AH2K_vCapParamsChan.bufSize = MAX_WIDTH * (MAX_HEIGHT + 20) * 2;
    AH2K_vCapParamsChan.bufSize = MAX_WIDTH * (MAX_HEIGHT +20) * 5/2+128;
    AH2K_vCapParamsChan.m_420   = 0;
 //   AH2K_vCapParamsChan.frmDrop = (chan->video->limitFrame!=0)? 1:0;
 // add by chenzhenbao
       AH2K_vCapParamsChan.frmDrop = 0;
	cap = cap_open(chan->index, chan->video, (Ptr)&AH2K_vCapParamsChan);

    while (1)
    {
		switch (cap->runStatus)
		{ 
		case CAPTURE_STOP:
			if (!ptr_vid_chan->flag_enable_cap)
			{
				TSK_sleep(1000);
				break;
			}

			// 输入尚不稳定
			TSK_sleep(500);

			FVID_control(cap->handle, VPORT_CMD_EDC(GETINPUT), (Ptr)&input);

			if (input == 0)
			{
				insert_null_image(ptr_vid_chan, 0, 0, 0, 320, 240);
				break;
			}

			FVID_control(cap->handle, VPORT_CMD_EDC(CONFIG), NULL);
			FVID_control(cap->handle, VPORT_CMD_GET_RATE, &frame_rate);
			if (frame_rate != 0)
			{
				frame_rate = (CPU_CLK + (frame_rate * 8 / 2)) / (frame_rate * 8);
				frame_rate = (frame_rate > 100 ) ? 25 : frame_rate;
			}
			
			// 输入尚不稳定,
			TSK_sleep(200);
#ifdef _DEBUG
			printf("rate is %d\n", frame_rate);
#endif
			
			timing.rate = frame_rate;
			FVID_control(cap->handle, VPORT_CMD_EDC(GETRES), (Ptr)&timing);

			// have signal but no match
			if (timing.hSize == 0)
			{
				insert_null_image(ptr_vid_chan, 1, timing.lcf, timing.rate, 320, 240);
				break;
			}
			
			//  是否限制分辨率
			if ((chan->video->limitRes != 0) && (chan->video->limitRes < timing.hSize))
			{
				insert_null_image(ptr_vid_chan, 1, timing.lcf, timing.rate, 320, 240);
				break;
			}

			// 是否限制最大帧率
			//if (chan->video->limitFrame != 0 && ptr_vid_chan->framerate>chan->video->limitFrame)
			//{
			//	ptr_vid_chan->framerate = chan->video->limitFrame;
			//}

			//cap->runStatus = CAPTURE_TEST; 
			cap->runStatus = CAPTURE_START;
			break;

		case CAPTURE_START:
		{
			Int	XStart,	YStart, XOffset;
			Int	XStop, YStop, YOffset;
				
			XStart = timing.hProch;
			XStop  = timing.hProch + timing.hSize - 1;
			YStart = timing.vProch;
			chan->vi->vp_chan_interlaced =timing.interlaced;

			if (timing.interlaced == 1)
			{
				YStop = timing.vProch + timing.vSize / 2 - 1;
				AH2K_vCapParamsChan.fldOp = VPORT_FLDOP_FRAME;//720p 20080516
			}
			else
			{
				YStop=timing.vProch + timing.vSize - 1;
				AH2K_vCapParamsChan.fldOp = VPORT_FLDOP_FLD1;//720p 20080516
			}

			if ((OldWidth > timing.hSize || OldHeight>timing.vSize) ||
				(OldWidth == 0 || OldHeight == 0) || 
				(OldWidth == timing.hSize && OldHeight == timing.vSize))
			{

				//设置的区域比原始的大或一样大或没有设置区域
				if ( timing.hSize == 1280 && timing.vSize == 720 &&
					 OldWidth > 0 && OldHeight > 0 && 
					 OldWidth != timing.hSize && OldHeight != timing.vSize)
				{
					OldWidth720P = OldWidth;
					OldHeight720P = OldHeight;
					OldWidth720P = 720 * OldWidth720P / OldHeight720P;
					if (OldWidth720P % 16)
					{
						OldWidth720P += (16 - (OldWidth720P % 16));
					}
					ptr_vid_chan->enc_width = OldWidth720P;
					ptr_vid_chan->enc_height = OldHeight720P = 720;

					XOffset = ((timing.hSize - OldWidth720P) >> 1) + ((timing.hSize - OldWidth720P) % 2);
					XStart += XOffset;
					XStop  -= XOffset;

					YOffset = ((timing.vSize - OldHeight720P) >> 1) + ((timing.vSize - OldHeight720P) % 2);
					YStart += YOffset;
					YStop  -= YOffset;
				}
				else

				{
					ptr_vid_chan->enc_width = timing.hSize;
					ptr_vid_chan->enc_height = timing.vSize;
				}
			}
			else //if (OldWidth<timing.hSize || OldHeight<timing.vSize)
			{
				//设置的区域比原始的小
				ptr_vid_chan->enc_width = OldWidth;
				ptr_vid_chan->enc_height = OldHeight;

				XOffset = ((timing.hSize - OldWidth) >> 1) + ((timing.hSize - OldWidth) % 2);
				XStart += XOffset;
				XStop  -= XOffset;

				YOffset = ((timing.vSize - OldHeight) >> 1) + ((timing.vSize - OldHeight) % 2);
				YStart += YOffset;
				YStop  -= YOffset;
			}

			//计算出新的位置
			AH2K_vCapParamsChan.fldXStrt2 = AH2K_vCapParamsChan.fldXStrt1 = XStart;
			AH2K_vCapParamsChan.fldXStop2 = AH2K_vCapParamsChan.fldXStop1 = XStop;
			AH2K_vCapParamsChan.fldYStrt2 = AH2K_vCapParamsChan.fldYStrt1 = YStart;
			AH2K_vCapParamsChan.fldYStop2 = AH2K_vCapParamsChan.fldYStop1 = YStop;

// add by chenzhenbao
            tmpwidth = timing.hSize;
//add by hedaihua
			//标记连接VGA的VP口
			if (idPci==AH3110L && chan->video->idPort==VPORT2||
				idPci==AH3111  && chan->video->idPort==VPORT2 ||
				idPci==AH3111A && chan->video->idPort==VPORT2 ||
				idPci==AH3111E  && chan->video->idPort==VPORT0||
				idPci==AH3111EA && chan->video->idPort==VPORT0||
				idPci==AH3210  && chan->video->idPort==VPORT2 ||
				idPci==AH3210A && chan->video->idPort==VPORT2 ||
				idPci==QAVP3210 && chan->video->idPort==VPORT2||
				idPci==AH2205L && chan->video->idPort==VPORT2)
			{
				ptr_vid_chan->g_VpIsVga=0;
			}
			else
			{
				ptr_vid_chan->g_VpIsVga=1;
			}	
			

//modified by chenzhenbao

			//AH2K_vCapParamsChan.thrld=(ptr_vid_chan->enc_width>>3);
			if (timing.interlaced && ptr_vid_chan->g_VpIsVga)
			{
				AH2K_vCapParamsChan.mergeFlds=VPORT_FLDS_MERGED;
				AH2K_vCapParamsChan.fldDect=VPORTCAP_FLDD_DISABLE;
// add by chenzhenbao
                AH2K_vCapParamsChan.m_420    =1;
				chan->vi->m_420               =1;
                AH2K_vCapParamsChan.thrld=(ptr_vid_chan->enc_width>>3)<<1; 
			}
			else
			{
				AH2K_vCapParamsChan.mergeFlds=VPORT_FLDS_SEPARATED;
				AH2K_vCapParamsChan.fldDect=VPORTCAP_FLDD_ENABLE;
// add by chenzhenbao
                if(tmpwidth<=1280 && ptr_vid_chan->g_VpIsVga)
				{
				    AH2K_vCapParamsChan.m_420     =1;
					chan->vi->m_420               =1;
					AH2K_vCapParamsChan.thrld=(ptr_vid_chan->enc_width>>3)<<1;
				}
				else
				{
                    AH2K_vCapParamsChan.m_420    =0;
					chan->vi->m_420               =0;
					AH2K_vCapParamsChan.thrld=(ptr_vid_chan->enc_width>>3);

				}
			}
				    
                
			


            
            AH2K_vCapParamsChan.frmDrop = (chan->video->limitFrame!=0)? 1:0;
// add by chenzhenbao

           if((AH2K_vCapParamsChan.frmDrop==1)&&(chan->vi->vp_chan_interlaced ==1))
		   {
		        AH2K_vCapParamsChan.frmDrop=0;
		   }
		      
   
           if(AH2K_vCapParamsChan.frmDrop==1)
		   {
		        act_frame_rate= frame_rate/6 ;    
		   }
		   else
		   {
		        act_frame_rate=  frame_rate;    
		   }

			FVID_control(cap->handle, VPORT_CMD_CONFIG_CHAN, (Ptr)&AH2K_vCapParamsChan);
			FVID_control(cap->handle, VPORT_CMD_EDC(CONFIG), NULL);
			TSK_sleep(200);
			FVID_control(cap->handle, VPORT_CMD_START, 0);
			TSK_sleep(200);
			cap->runStatus = CAPTURE_RUN;
			cap->frame = NULL;
			status = FVID_alloc(cap->handle, &cap->frame);
			if (status != 0)
			{
				cap_stop(cap);	
			}
			break;
		}
		case CAPTURE_RUN:
		{
			static Uint8 Pre_channel=0;
			static Uint8 Pre_flag_enable_cap=0;

			if (!ptr_vid_chan->flag_enable_cap)
			{
				cap_stop(cap);
				break;
			}

			FVID_control(cap->handle, VPORT_CMD_EDC(GETSTATUS), (Ptr)&bChange);
			if (bChange)
			{
		
				cap_stop(cap);
				break;
			}

			if (OldWidth != ptr_vid_chan->width ||
				OldHeight != ptr_vid_chan->height)
			{
				OldWidth = ptr_vid_chan->width;
				OldHeight = ptr_vid_chan->height;

				cap_stop(cap);
				break;
			}

			status = FVID_exchange(cap->handle, &cap->frame);
			if (status != 0)
			{
				FVID_control(cap->handle, VPORT_CMD_EDC(RESET), NULL);
				cap_stop(cap);
				break;
			}

			//if opened the SDI and then limit the HDMI
			if( ptr_vid_chan->channel == 1 && 
				ptr_vid_chan->flag_enable_cap == 1 &&
			    chan->video->adcType==ADC2971)
			{
				Pre_channel = ptr_vid_chan->channel;
				Pre_flag_enable_cap = ptr_vid_chan->flag_enable_cap;
			}
			else if( ptr_vid_chan->channel == 1 &&
					 ptr_vid_chan->flag_enable_cap == 0 &&
					 chan->video->adcType == ADC2971)
			{
				Pre_flag_enable_cap = ptr_vid_chan->flag_enable_cap;
			}
	
			if( ptr_vid_chan->channel== 0 && 
				ptr_vid_chan->flag_enable_cap == 1 &&
				Pre_channel == 1 &&
				Pre_flag_enable_cap == 1)
			{
				ptr_vid_chan->framerate = 5;
			}
			else if(ptr_vid_chan->channel ==  0 && 
					ptr_vid_chan->flag_enable_cap == 1 &&
			 		Pre_channel == 1 &&
			  		Pre_flag_enable_cap == 0)
			{
				ptr_vid_chan->framerate = frame_rate;
				Pre_channel = 0 ;
			}


            chan->vi->m_yIn = cap->frame->frame.iFrm.y1;
			chan->vi->m_uIn = cap->frame->frame.iFrm.cb1;
			chan->vi->m_vIn = cap->frame->frame.iFrm.cr1;

			vid_chan_process(ptr_vid_chan,
							(Uint32)cap->frame->frame.iFrm.y1,	
							(Uint32)cap->frame->frame.iFrm.cb1,
							(Uint32)cap->frame->frame.iFrm.cr1,
							act_frame_rate,
							(Uint8)PORT_TYPE_HDMI,
							(Uint16)timing.hSize,
							(Uint16)timing.vSize,0);

			cap->frame->frame.iFrm.y1  = chan->vi->m_yIn;
            cap->frame->frame.iFrm.cb1 = chan->vi->m_uIn;
			cap->frame->frame.iFrm.cr1 = chan->vi->m_vIn;

			cap->frame->frame.iFrm.y2  = (char*)chan->vi->m_yIn+ptr_vid_chan->enc_width;
            cap->frame->frame.iFrm.cb2 = chan->vi->m_uIn;
			cap->frame->frame.iFrm.cr2 = chan->vi->m_vIn;

			break;
		}
		}

		TSK_sleep(1);
	}
}


