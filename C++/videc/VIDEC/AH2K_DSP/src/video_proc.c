/*
video_proc.c

2008.8.12 create
2010.6.8 update
*/
#include <std.h>
#include <stdio.h>
#include <mem.h>
#include <csl.h>
#include <tsk.h>
#include <csl_cache.h>
#include <assert.h>
#include <async.h>
#include <csl_dat.h>
#include <assert.h>

#include "video_proc.h"
#include "pci_util.h"

Uint32	g_addr_tmp_y = 0;
Uint32	g_addr_tmp_u = 0;
Uint32	g_addr_tmp_v = 0;

#define BARS 8
void makecolorbar_yv12(Byte* pBuf,Uns x,Uns y)
{
	Byte* p;

	Byte* pY;
	Byte* pCr;
	Byte* pCb;

	/*
	Y00 Y01 Y02 Y03
	Y10 Y11 Y12 Y13
	Cb0     Cb1   
	Cr0     Cr1 
	4x2个像素,共10个字节
	使用空间 x*y+x*y/2 
	*/
	struct YCrCb
	{
		Byte y;
		Byte cb;
		Byte cr;
	}colorbar[BARS]=
	{
		{235,128,128}, //White
		{162,44,142},	//Yellow
		{131,156,44},	//Cyan
		{112,72,58},	//Green
		{84,184,198},	//Magenta
		{65,100,212},	//Read
		{35,212,114},	//Blue
		{16,128,128},	//Black	
	};


	//	ASSERT(0==x%BARS);
	//	ASSERT(0==y%2);

	p=pBuf;


	pY=p;
	pCb=pY+x*y;
	pCr=pCb+x*y/4;	

	if(p)
	{
		int bar=x/BARS;	//一个bar多少像素		
		int i;
		int j;
		//Y
		for(i=0;i<y;++i)
		{
			for(j=0;j<x;++j)
			{
				*p++=colorbar[j/bar].y;				
			}
		}

		//		ASSERT(pCb==p);
		for(i=0;i<y/2;++i)
		{
			for(j=0;j<x;++j)
			{
				if(j%2)
					*p++=colorbar[j/bar].cb;
			}
		}

		//		ASSERT(pCr==p);

		for(i=0;i<y/2;++i)
		{
			for(j=0;j<x;++j)
			{
				if(j%2)
					*p++=colorbar[j/bar].cr;
			}
		}			
	}

}


void makecolorbar_yv16(Byte* pBuf,Uns x,Uns y)
{
	Byte* p;

	Byte* pY;
	Byte* pCr;
	Byte* pCb;

	/*
	Y00 Y01 Y02 Y03
	Y10 Y11 Y12 Y13
	Cb0     Cb1   
	Cr0     Cr1 
	4x2个像素,共10个字节
	使用空间 x*y+x*y/2 
	*/
	struct YCrCb
	{
		Byte y;
		Byte cb;
		Byte cr;
	}colorbar[BARS]=
	{
		{235,128,128}, //White
		{162,44,142},	//Yellow
		{131,156,44},	//Cyan
		{112,72,58},	//Green
		{84,184,198},	//Magenta
		{65,100,212},	//Read
		{35,212,114},	//Blue
		{16,128,128},	//Black	
	};


	//	ASSERT(0==x%BARS);
	//	ASSERT(0==y%2);

	p=pBuf;


	pY=p;
	pCb=pY+x*y;
	pCr=pCb+x*y/2;	

	if(p)
	{
		int bar=x/BARS;	//一个bar多少像素		
		int i;
		int j;
		//Y
		for(i=0;i<y;++i)
		{
			for(j=0;j<x;++j)
			{
				*p++=colorbar[j/bar].y;				
			}
		}

		//	ASSERT(pCb==p);
		for(i=0;i<y;++i)
		{
			for(j=0;j<x;++j)
			{
				if(j%2)
					*p++=colorbar[j/bar].cb;
			}
		}

		//	ASSERT(pCr==p);

		for(i=0;i<y;++i)
		{
			for(j=0;j<x;++j)
			{
				if(j%2)
					*p++=colorbar[j/bar].cr;
			}
		}			
	}

}



void video_proc_init(void)
{
	Byte* g_buff=NULL;
	/*	g_addr_tmp_y = (Uint32)MEM_alloc(EXTERNALHEAP, 320 * 240, 128);
	assert(g_addr_tmp_y != (Uint32)MEM_ILLEGAL);

	g_addr_tmp_u = (Uint32)MEM_alloc(EXTERNALHEAP, 320 * 120, 128);
	assert(g_addr_tmp_u != (Uint32)MEM_ILLEGAL);

	g_addr_tmp_v = (Uint32)MEM_alloc(EXTERNALHEAP, 320 * 120, 128);
	assert(g_addr_tmp_v != (Uint32)MEM_ILLEGAL);*/

	g_buff=MEM_alloc(EXTERNALHEAP, 320 * 240*2, 128);

	makecolorbar_yv16(g_buff,320,240);
	assert(g_buff != MEM_ILLEGAL);

	g_addr_tmp_y=(Uint32)g_buff;
	g_addr_tmp_u=(Uint32)g_buff+320*240;
	g_addr_tmp_v=(Uint32)g_buff+320*240+320*240/2;


}

PVID_CHAN_INFO vid_chan_init(Uint32 vid, Int maxWidth, Int maxHeight, Int numPCIControl)
{
	PVID_CHAN_INFO pvi;
	int i;

	pvi = (PVID_CHAN_INFO)MEM_alloc(EXTERNALHEAP, sizeof(VID_CHAN_INFO), 32);
	memset(pvi, 0, sizeof(VID_CHAN_INFO));

	pvi->channel		= vid;
	pvi->flag_enable_cap= 0;
	pvi->scale_type		= 0;
	pvi->framerate		= 30;
	pvi->enc_width 		= maxWidth;
	pvi->enc_height		= maxHeight;
	pvi->width			= maxWidth;
	pvi->height			= maxHeight;
	pvi->frame_count	= 0;
	pvi->numPCIControl  = numPCIControl;
	pvi->m_420          = 0;

	for (i = 0; i < pvi->numPCIControl; i++)
	{
		Uint8 *p;
		PPCI_CONTROL pControl;

		p = (Uint8 *)MEM_alloc(EXTERNALHEAP, maxWidth * (maxHeight+20) * 5/2 + 128, 128);
		assert(p != MEM_ILLEGAL);
		pvi->pPCIControl[i] = (Ptr)p;

		pControl = (PPCI_CONTROL)p;
		// 不建议使用data域，add by chenzhenbao
		pControl->data = p + 128;
		pControl->inUse = 0;
	}

	return pvi;
}

static Void _callback(Ptr arg, Int status, Ptr reqp, Uns size)
{
	//add
	//*(Int*)arg = 0;
	*(Uint32*)arg = 0;
}

static PPCI_CONTROL FindFreeControl(PVID_CHAN_INFO pvi)
{
	int i;
	PPCI_CONTROL pControl;

	for (i = 0; i < pvi->numPCIControl; i++)	
	{
		pControl = (PPCI_CONTROL)pvi->pPCIControl[i];
		if (!pControl->inUse)
		{
			// add by chenzhenbao
			pvi->curPCIControl = i;
			return pControl;
		}
	}

	return NULL;
}


void image_half_scale(Uint32 srcY,Uint32 srcU,Uint32 srcV,Uint32 dstY,Uint32 dstU,Uint32 dstV,Uint16 width,Uint16 height)
{
	CACHE_clean(CACHE_L2ALL, 0, 0);
	x_image_scale_2t0(srcY,dstY,
		srcU,dstU,
		srcV,dstV,
		width,height,0);
	CACHE_clean(CACHE_L2ALL, 0, 0);
}

void image_half_scale_420(Uint32 srcY,Uint32 srcU,Uint32 srcV,Uint32 dstY,Uint32 dstU,Uint32 dstV,Uint16 width,Uint16 height)
{
	CACHE_clean(CACHE_L2ALL, 0, 0);
	x_image_scale_w2h0(srcY,dstY,
		srcU,dstU,
		srcV,dstV,
		width,height,0);




	CACHE_clean(CACHE_L2ALL, 0, 0);
}

void image_half_height_422to420(Uint32 srcY,Uint32 srcU,Uint32 srcV,Uint32 dstY,Uint32 dstU,Uint32 dstV,Uint16 width,Uint16 height)
{
	CACHE_clean(CACHE_L2ALL, 0, 0);

	x_image_half_height_422to420(srcY,dstY,
		srcU,dstU,
		srcV,dstV,
		width,height,0);



	CACHE_clean(CACHE_L2ALL, 0, 0);
}
void image_half_height_420to420(Uint32 srcY,Uint32 srcU,Uint32 srcV,Uint32 dstY,Uint32 dstU,Uint32 dstV,Uint16 width,Uint16 height)
{
	CACHE_clean(CACHE_L2ALL, 0, 0);

	x_image_half_height_420to420(srcY,dstY,
		srcU,dstU,
		srcV,dstV,
		width,height,0);



	CACHE_clean(CACHE_L2ALL, 0, 0);
}



void vid_chan_process(PVID_CHAN_INFO pvi, Uint32 yIn, Uint32 uIn, Uint32 vIn,
					  Uint8 actual_framerate, Uint8 nPortType, Uint16 nInputWidth, Uint16 nInputHeight,Bool nosignel)
{
	PVIDEO_FRAME pFrame;
	Uint32 nWidth  = pvi->enc_width;
	Uint32 nHeight = pvi->enc_height;

	Uint32 nHeaderLen = sizeof(VIDEO_FRAME);
	Uint32 nImageSize = nWidth * nHeight;
	Uint8 nFlag422=0;

	Uint8 *yOut;
	Uint8 *uOut;
	Uint8 *vOut;

	Uint8 merge = 0;
	Uint8 nInputFrameType = 0;
	Uint32 mode = 0;
	Uint8 scale_type = pvi->scale_type;
	Uint16 nFrameRate=pvi->framerate;
	Uint8 nInputFrameRate;
	PPCI_CONTROL pControl;

	if((nImageSize)> (1920*1080) )
	{
		//#ifdef _DEBUG
		printf("buffer overflow \n");
		//#endif		
		return ;
	}


	// 空闲的PCI控制块
	while ((pControl = FindFreeControl(pvi)) == NULL)
	{
#ifdef _DEBUG
		//	printf("Video #%d frame #%d is discarded!\n", pvi->channel, pvi->frame_count);
#endif
		//TSK_sleep(1);
		return;
	}

	pvi->frame_count++;

	yOut = (Uint8*)pControl+128;

	if(pvi->m_420== 0)
	{  	
		uOut =yOut+nImageSize;
		vOut =uOut+(nImageSize >> 1);
	}
	else
	{
		uOut = yOut + nImageSize;
		vOut = uOut + (nImageSize >> 2);   
	}

	if (actual_framerate >= 40)
	{
		if (nHeight==1080)
		{
			actual_framerate=(actual_framerate>>1);
		}
	}



	nInputFrameRate = actual_framerate;

	//if actual fps < need,we do nothing.
	if (actual_framerate<15)
	{
		if (nFrameRate==1)
		{
			mode = pvi->frame_count % 10;
			if (mode != 1)
			{	
				return;
			}
			actual_framerate = actual_framerate / 10;
		}
		else if (nFrameRate==5)
		{
			mode = pvi->frame_count % 2;
			if (mode != 1)
			{	
				return;
			}
			actual_framerate = actual_framerate / 2;
		}
	}
	else if(actual_framerate > nFrameRate ) 
	{	
		if (actual_framerate >= 40)
		{
			switch (nFrameRate)
			{
			case 1:
				mode = pvi->frame_count % actual_framerate;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = 1;
				break;
			case 5:
				mode = pvi->frame_count % 10;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = actual_framerate / 10;
				break;
			case 10:
				mode = pvi->frame_count % 5;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = actual_framerate / 5;
				break;
			case 15:
				mode = pvi->frame_count % 4;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = actual_framerate / 4;
				break;
			case 30:
				mode = pvi->frame_count % 2;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = actual_framerate / 2;
				break;
			}
		}
		else
		{
			switch (nFrameRate)
			{
			case 1:
				mode = pvi->frame_count % actual_framerate;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = 1;
				break;
			case 5:
				mode = pvi->frame_count % 5;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = actual_framerate / 5;
				break;
			case 10:
				mode = pvi->frame_count % 3;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = actual_framerate / 3;
				break;
			case 15:
				mode = pvi->frame_count % 2;
				if (mode != 1)
				{	
					return;
				}
				actual_framerate = actual_framerate / 2;
				break;
			}
		}	
	}



	// 添加缩放部分，不进行内存交换，
	if ((nHeight==1080 && scale_type==2) || (nHeight!=1080 && pvi->width != 0 && pvi->width <= 352)) //1/4 缩放
	{
		if(pvi->m_420==0)//for YUV422
		{
			// 重新计算目的地址偏移
			uOut = yOut + (nImageSize>>2);
			vOut = uOut + (nImageSize>>4);
			nWidth=pvi->enc_width>>1;
			nHeight=pvi->enc_height>>1;
			image_half_scale((Uint32)yIn,
				(Uint32)uIn,
				(Uint32)vIn,
				(Uint32)yOut,
				(Uint32)uOut,
				(Uint32)vOut,
				pvi->enc_width,
				pvi->enc_height);
		}		
		else
		{
			// 重新计算目的地址偏移
			uOut = yOut + (nImageSize>>2);
			vOut = uOut + (nImageSize>>4);

			nWidth=pvi->enc_width>>1;
			nHeight=pvi->enc_height>>1;

			image_half_scale_420((Uint32)yIn,
				(Uint32)uIn,
				(Uint32)vIn,
				(Uint32)yOut,
				(Uint32)uOut,
				(Uint32)vOut,
				pvi->enc_width,
				pvi->enc_height);

		}			
	}
	else if (nHeight==1080 && scale_type==1) //高度减半
	{
		if(pvi->m_420==0)//for YUV422
		{
			// 重新计算目的地址偏移
			uOut = yOut + (nImageSize>>1);
			vOut = uOut + (nImageSize>>3);

			nWidth=pvi->enc_width;
			nHeight=pvi->enc_height>>1;

			image_half_height_422to420((Uint32)yIn,
				(Uint32)uIn,
				(Uint32)vIn,
				(Uint32)yOut,
				(Uint32)uOut,
				(Uint32)vOut,
				pvi->enc_width,
				pvi->enc_height); 
		}
		else
		{
			// 重新计算目的地址偏移
			uOut = yOut + (nImageSize>>1);
			vOut = uOut + (nImageSize>>3);
		

			nWidth=pvi->enc_width;
			nHeight=pvi->enc_height>>1;

			image_half_height_420to420((Uint32)yIn,
				(Uint32)uIn,
				(Uint32)vIn,
				(Uint32)yOut,
				(Uint32)uOut,
				(Uint32)vOut,
				pvi->enc_width,
				pvi->enc_height); 
		}							
	}
	else//不缩放，
	{
		if(pvi->m_420 == 0)
		{
			if (pvi->g_VpIsVga)
			{
				Uint32 nDstY,nDstU,nDstV;
				Uint32 id;
				nDstY=yIn;
				nDstU=nDstY+nImageSize;
				nDstV=nDstU+nImageSize/4;
				id=DAT_copy2d(DAT_2D1D,	
					(void *)uIn,
					(void *)nDstU,
					pvi->enc_width/2,
					pvi->enc_height/2,
					pvi->enc_width);
				DAT_wait(id);

				id=DAT_copy2d(DAT_2D1D,	
					(void *)vIn,
					(void *)nDstV,
					pvi->enc_width/2,
					pvi->enc_height/2,
					pvi->enc_width);
				DAT_wait(id);
			}
			else
			{
				nFlag422=1;
			}
		} 

		if(pvi->vp_chan_interlaced==1)
		{
			merge=1;
		}
		// 内存还给VP口，add by chenzhenbao
		pvi->m_yIn   = yOut;
		pvi->m_uIn   = uOut;
		pvi->m_vIn   = vOut;

		pvi->pPCIControl[pvi->curPCIControl] = (Ptr)(yIn-128);
		pControl    = pvi->pPCIControl[pvi->curPCIControl];
	}

	nInputFrameType = merge;
	nInputFrameType = nInputFrameType | (nFlag422?0:1)<<1;

	pFrame = (PVIDEO_FRAME)((Uint8*)pControl+128 - nHeaderLen);
	pFrame->base_frame.version		= FRAME_VERSION;
	pFrame->base_frame.frame_type	= FRAME_VIDEO;
	pFrame->base_frame.framerate	= actual_framerate;
	pFrame->base_frame.channel		= pvi->channel;

	pFrame->nOutputWidth	= nWidth;
	pFrame->nOutputHeight	= nHeight;
	pFrame->nInputFrameRate	= nInputFrameRate;
	pFrame->nPortType		= nPortType;
	pFrame->nInputWidth		= nInputWidth;
	pFrame->nInputHeight	= nInputHeight;

	pFrame->nInputFrameType = nInputFrameType;

	pControl->inUse				= 1;
	pControl->request.srcAddr	= (char *)pFrame;
	pControl->request.byteCnt	= nHeaderLen + (nFlag422?(nImageSize*2):(nImageSize*3/2)); 
	pControl->callback.fxn		= (GIO_TappCallback)_callback;
	pControl->callback.arg		= (Ptr)&(pControl->inUse);

	pci_write_ex(pControl);
}

void vid_chan_process_old(PVID_CHAN_INFO pvi, Uint32 yIn, Uint32 uIn, Uint32 vIn, Uint8 actual_framerate, Uint8 nPortType, Uint16 nInputWidth, Uint16 nInputHeight)
{
	PVIDEO_FRAME pFrame;
	Uint32 nWidth  = pvi->enc_width;
	Uint32 nHeight = pvi->enc_height;

	Uint32 nHeaderLen = sizeof(VIDEO_FRAME);
	Uint32 nImageSize = nWidth * nHeight;

	Uint8 *yOut;
	Uint8 *uOut;
	Uint8 *vOut;
	
	Uint8 nFlag420=1;
	Uint8 nInputFrameType = 0;
	Uint8 merge =0;
	Uint32 mode = 0;
	Uint8 scale_type = pvi->scale_type;
	Uint8 nInputFrameRate;
	PPCI_CONTROL pControl;

	// 空闲的PCI控制块
	while ((pControl = FindFreeControl(pvi)) == NULL)
	{
#ifdef _DEBUG
		//printf("Video #%d frame #%d is discarded!\n", pvi->channel, pvi->frame_count);
#endif
		//TSK_sleep(1);
		return;
	}

	pvi->frame_count++;

	yOut = (Uint8*)pControl+128;
	uOut = yOut + nImageSize;
	vOut = uOut + (nImageSize >> 2);

	nInputFrameRate = actual_framerate;
	image_copy(yIn, (Uint32)yOut, nWidth, nHeight);
	half_scale(uIn, (Uint32)uOut, nWidth, (nHeight >>1 ));
	half_scale(vIn, (Uint32)vOut, nWidth, (nHeight >>1 ));
	
	nInputFrameType = merge;
	nInputFrameType |= nFlag420<<1 ;

	pFrame = (PVIDEO_FRAME)((Uint8*)pControl+128 - nHeaderLen);
	pFrame->base_frame.version		= FRAME_VERSION;
	pFrame->base_frame.frame_type	= FRAME_VIDEO_NO_SIGNAL;
	pFrame->base_frame.framerate	= actual_framerate;
	pFrame->base_frame.channel		= pvi->channel;

	pFrame->nOutputWidth	= nWidth;
	pFrame->nOutputHeight	= nHeight;
	pFrame->nInputFrameRate	= nInputFrameRate;
	pFrame->nPortType		= nPortType;
	pFrame->nInputWidth		= nInputWidth;
	pFrame->nInputHeight	= nInputHeight;
	
	pFrame->nInputFrameType = nInputFrameType;

	pControl->inUse				= 1;
	pControl->request.srcAddr	= (char *)pFrame;
	pControl->request.byteCnt	= nHeaderLen + nImageSize * 3 / 2; 

	pControl->callback.fxn		= (GIO_TappCallback)_callback;
	pControl->callback.arg		= (Ptr)&(pControl->inUse);

	pci_write_ex(pControl);
}






