
#include <std.h>
#include <tsk.h>  
#include <sem.h>   
#include <gio.h>
#include <log.h>

#include <csl.h>
#include <csl_dat.h>
#include <csl_cache.h>
#include <csl_emifa.h>
#include <csl_i2c.h>

void image_copy(Uint32 addr_in,Uint32 addr_out, Uint16 width, Uint16 height)
{
	Uint8*dst=(Uint8*)addr_out;
	Uint8*src=(Uint8*)addr_in;
	Uint32 transformID;

	Uint16 index;
	for (index=0;index<height;index++)
	{
		transformID=DAT_copy(src,dst,width);
		dst+=width;
		src+=width;
	}
	DAT_wait(transformID);
}

void half_scale(Uint32 addr_in,Uint32 addr_out, Uint16 width, Uint16 height)
{
	Uint8*dst=(Uint8*)addr_out;
	Uint8*src=(Uint8*)addr_in;
	Uint32 transformID;
	Uint16 half_width=width>>1;

	Uint16 index;
	for (index=0;index<height;index++)
	{
		transformID=DAT_copy(src,dst,half_width);
		dst+=half_width;
		src+=width;
	}
	DAT_wait(transformID);
}

void image_copy_field(Uint32 addr_in,Uint32 addr_out, Uint16 width, Uint16 height)
{
	Uint8*dst=(Uint8*)addr_out;
	Uint8*src=(Uint8*)addr_in;
	Uint32 transformID;
	Uint32 nDoubleWidth=(width<<1);

	Uint16 index;
	for (index=0;index<height;index++)
	{
		transformID=DAT_copy(src,dst,width);
		dst+=width;
		src+=nDoubleWidth;
	}
	DAT_wait(transformID);
}

 
void half_scale_field(Uint32 addr_in,Uint32 addr_out, Uint16 width, Uint16 height)
{
	Uint8*dst=(Uint8*)addr_out;
	Uint8*src=(Uint8*)addr_in;
	Uint32 transformID;
	Uint16 half_width=width>>1;
	Uint32 nDoubleWidth=(width<<1);

	Uint16 index;
	for (index=0;index<height;index++)
	{
		transformID=DAT_copy(src,dst,half_width);
		dst+=half_width;
		src+=nDoubleWidth;
	}
	DAT_wait(transformID);
}

void interlace_to_double_field(Uint32 addr_src_y, Uint32 addr_src_u, Uint32 addr_src_v,
						 Uint32 addr_dst_y, Uint32 addr_dst_u, Uint32 addr_dst_v, 
			 			 Int32 width, Int32 height)
{
	Uint8*pDstEvenY=(Uint8*)addr_dst_y;
	Uint8*pDstOddY=pDstEvenY+(width*height>>1);
	Uint8*pSrcY=(Uint8*)addr_src_y;

	Uint32 transformID;
	Uint32 half_width=(width>>1);

	Uint8*pDstEvenU=(Uint8*)addr_dst_u;
	Uint8*pDstOddU=pDstEvenU+(half_width*height>>2);
	Uint8*pSrcU=(Uint8*)addr_src_u;

	Uint8*pDstEvenV=(Uint8*)addr_dst_v;
	Uint8*pDstOddV=pDstEvenV+(half_width*height>>2);
	Uint8*pSrcV=(Uint8*)addr_src_v;

	Uint16 index;
	height>>=2;
	for (index=0;index<height;index++)
	{
		transformID=DAT_copy(pSrcY,pDstEvenY,width);
		pDstEvenY+=width;
		pSrcY+=width;
		transformID=DAT_copy(pSrcY,pDstOddY,width);
		pDstOddY+=width;
		pSrcY+=width;
		transformID=DAT_copy(pSrcY,pDstEvenY,width);
		pDstEvenY+=width;
		pSrcY+=width;
		transformID=DAT_copy(pSrcY,pDstOddY,width);
		pDstOddY+=width;
		pSrcY+=width;

		transformID=DAT_copy(pSrcU,pDstEvenU,half_width);
		pDstEvenU+=half_width;
		pSrcU+=width;
		transformID=DAT_copy(pSrcU,pDstOddU,half_width);
		pDstOddU+=half_width;
		pSrcU+=width;

		transformID=DAT_copy(pSrcV,pDstEvenV,half_width);
		pDstEvenV+=half_width;
		pSrcV+=width;
		transformID=DAT_copy(pSrcV,pDstOddV,half_width);
		pDstOddV+=half_width;
		pSrcV+=width;
	}
	DAT_wait(transformID);

}

//width half scale, get one pixel when passing one, @ * @ * @ * @ *
//1920X1080 -> 960X1080
void half_scale_width_interlace (Uint32 addr_src_y, Uint32 addr_src_u, Uint32 addr_src_v,
						 Uint32 addr_dst_y, Uint32 addr_dst_u, Uint32 addr_dst_v, 
			 			 Int32 width, Int32 height)
{

 	Int32 rowindex, j;
 	Int32 ch_width  = width >> 1;
 	Int32 ch_height = height >> 1;

	Uint8 *pyIn, *puIn, *pvIn; 
	Uint8 *pyOut, *puOut, *pvOut; 
	Uint8 *pyOut2, *puOut2, *pvOut2; 

	pyIn = (Uint8 *)addr_src_y;			
	pyOut = (Uint8 *)addr_dst_y;		
	
	puIn = (Uint8 *)addr_src_u;			
	pvIn = (Uint8 *)addr_src_v;

	puOut = (Uint8 *)addr_dst_u;			
	pvOut = (Uint8 *)addr_dst_v;

	pyOut2 = (Uint8 *)addr_dst_y + (width*height>> 2);		
	puOut2 = (Uint8 *)addr_dst_u + (ch_width*ch_height>> 2);			
	pvOut2 = (Uint8 *)addr_dst_v + (ch_width*ch_height>> 2);

	//Y only
	for( rowindex = 0; rowindex < height; rowindex ++)
	{
		for(j = 0; j < width; j +=16)
		{
			Int32 yb_0, yb_1, y0_0, y0_1;
			yb_0 = _lo(_amemd8_const(pyIn+j)); 			//-1 line
			yb_1 = _hi(_amemd8_const(pyIn+j));
			y0_0 = _lo(_amemd8_const(pyIn+j + 8)); 	//cur line
			y0_1 = _hi(_amemd8_const(pyIn+j + 8));

			yb_0 = _packl4(yb_1, yb_0);	// cur line				
			y0_0 = _packl4(y0_1, y0_0);	// cur line
			_amemd8(pyOut+(j>>1)) 	= _itod(y0_0, yb_0);	//write to -1 line				
		}

		pyIn  += width;
		for(j = 0; j < width; j +=16)
		{
			Int32 yb_0, yb_1, y0_0, y0_1;
			yb_0 = _lo(_amemd8_const(pyIn+j)); 			//-1 line
			yb_1 = _hi(_amemd8_const(pyIn+j));
			y0_0 = _lo(_amemd8_const(pyIn+j + 8)); 	//cur line
			y0_1 = _hi(_amemd8_const(pyIn+j + 8));

			yb_0 = _packl4(yb_1, yb_0);	// cur line				
			y0_0 = _packl4(y0_1, y0_0);	// cur line
			_amemd8(pyOut2+(j>>1)) 	= _itod(y0_0, yb_0);	//write to -1 line				
		}
		pyIn  += width;
		pyOut += width>>1;		
		pyOut2 += width>>1;	
		rowindex ++;
	}

	//U and V
	for( rowindex = 0; rowindex < ch_height; rowindex ++)
	{
		for(j = 0; j < ch_width; j +=8)
		{
			Int32 ub_0, u0_0;
			Int32 vb_0, v0_0;

			ub_0 = _amem4_const(puIn+j);
			u0_0 = _amem4_const(puIn+j+4);
			
			vb_0 = _amem4_const(pvIn+j);
			v0_0 = _amem4_const(pvIn+j+4);
			
			ub_0 = _packl4(u0_0, ub_0);				
			vb_0 = _packl4(v0_0, vb_0);

			_amem4(puOut+(j>>1) ) = ub_0; 
			_amem4(pvOut+(j>>1) ) = vb_0;
		}			
		
		puIn 	+= (ch_width<<1);
		pvIn 	+= (ch_width<<1);	
		for(j = 0; j < ch_width; j +=8)
		{
			Int32 ub_0, u0_0, u1_0;
			Int32 vb_0, v0_0;

			ub_0 = _amem4_const(puIn+j);
			u0_0 = _amem4_const(puIn+j+4);
			
			vb_0 = _amem4_const(pvIn+j);
			v0_0 = _amem4_const(pvIn+j+4);
			
			ub_0 = _packl4(u0_0, ub_0);				
			vb_0 = _packl4(v0_0, vb_0);

			_amem4(puOut2+(j>>1) ) = ub_0; 
			_amem4(pvOut2+(j>>1) ) = vb_0;
		}

		puIn 	+= (ch_width<<1);
		pvIn 	+= (ch_width<<1);
		puOut 	+= ch_width>>1;
		pvOut 	+= ch_width>>1;
		puOut2 	+= ch_width>>1;
		pvOut2 	+= ch_width>>1;
		rowindex ++;		
	}
}

