/*
	vidoe_null.c

	2010.6.9 create this file
*/
#include <std.h>
#include <stdio.h>
#include <assert.h>
#include <csl_cache.h>
 
#include "video_null.h" 
#include "vcapchan.h"
void DrawNumber(Uint16 width, Uint16 height,Int8 number_width, Int8 number_height, unsigned char* sz);

extern void makecolorbar_yv16(Byte* pBuf,Uns x,Uns y);
extern Uint32 g_addr_tmp_y;
extern Uint32 g_addr_tmp_u;
extern Uint32 g_addr_tmp_v;

/*  
   Parameters : CHANNAL_DESCRIPT * chan
   Return value : None
   Description : diapaly an image when no signal input
*/

static Uint8 g_nLastIndex=0xFF;
void insert_null_image(PVID_CHAN_INFO pvi, Uint8 index,Int  lcf, Uint32 frame_rate, Uint16 nInputWidth, Uint16 nInputHeight)
{
	Uint32 imageY,imageU,imageV;
	Uint8 *array;
	unsigned char *thousand,*hundred,*ten ,*unit, *frame_hundred, *frame_ten,*frame_unit;
	Uint16 h_start,v_start,h,v;
    Uint32 Width = nInputWidth;
	Uint32 Height = nInputHeight;
	Uint32 Image = Width*Height;
	Uint32 i = 0;
	Uint32 count;

	count = pvi->frame_count;
	h_start = (Height-32)/2;
	v_start = (Width-112)/2;
    
    imageY = (Uint32)g_addr_tmp_y;
	imageU = (Uint32)g_addr_tmp_u;
	imageV = (Uint32)g_addr_tmp_v; 

	if( lcf < 0 ||lcf >9999)
	{
		lcf = 9999;
	}
	if( frame_rate < 0 || frame_rate > 999)
	{
		frame_rate = 999;
	}

	thousand = number[lcf/1000];
	hundred = number[lcf/100%10];
	ten = number[lcf/10%10];
	unit = number[lcf%10];
	frame_hundred = number[frame_rate/100%10];
	frame_ten = number[frame_rate/10%10];
	frame_unit = number[frame_rate%10];

	if (g_nLastIndex!=index)
	{
		g_nLastIndex=index;
		switch (index)
		{
		case 1:
			makecolorbar_yv16((Byte*)imageY,nInputWidth,nInputHeight);
			DrawNumber(h_start, v_start, 16,32, thousand);
			DrawNumber(h_start, v_start+16, 16, 32, hundred);
			DrawNumber(h_start, v_start+32, 16, 32, ten);
			DrawNumber(h_start, v_start+48, 16, 32, unit);
			DrawNumber(h_start, v_start+64, 16, 32, frame_hundred);
			DrawNumber(h_start, v_start+80, 16, 32, frame_ten);
			DrawNumber(h_start, v_start+96, 16, 32, frame_unit);

			break;
		case 0:
			makecolorbar_yv16((Byte*)imageY,nInputWidth,nInputHeight);
			break;
		default:
			memset(imageY,0x10,Image);  //0x10
			memset(imageU,0x80,Image/2); //0x80
			memset(imageV,0x80,Image/2); //0x80
			break;
		}	
	}

   
	//select the state 
	assert(index < 2);
    array = sig_template[index];


	
    
	CACHE_wbAllL2(CACHE_WAIT);

	pvi->enc_width = nInputWidth;
	pvi->enc_height = nInputHeight;

	vid_chan_process_old(pvi,
					(Uint32)imageY,	
					(Uint32)imageU,
					(Uint32)imageV,
					2,//frame_rate,
					2, //nPortType,
					nInputWidth,
					nInputHeight);
}


static void DrawNumber(Uint16 width, Uint16 height,Int8 number_width, Int8 number_height, unsigned char* sz)
{
	Uint32 imageY,imageU,imageV;
	Uint16 j, k;
	Uint32 l = 0;
	imageY = (Uint32)g_addr_tmp_y;
	imageU = (Uint32)g_addr_tmp_u;
	imageV = (Uint32)g_addr_tmp_v;

		for(j = width; j < width+number_height; j++)
		{
			for(k = height; k <height+number_width;k++)
			{
				*(Uint8*)(imageY + k +j *320) = sz[l];
				if(l%2==0)
				{
						*(Uint8*)(imageU + ((k + j*320)>>1)) = sz[l / 2 + number_width*number_height];
						*(Uint8*)(imageU + ((k + j*320)>>1)) = sz[l / 2 + number_width*(number_height + number_height>>1)];
				}
				
				l++;
			}
		}	

}