#include "bme_motion_est.h"
/*	  Maybe need this
  
	  int MeVAR[1024];
	  int MeVAROR[1024];
	  int MeMWOR[1024];
	  int MeX[1024];
	  int MeY[1024];
	  int MeVal[1024];
	  int MeOVal[1024];
*/
int FastBME(MBParam * const pParam,
							 FRAMEINFO * const current,
							 FRAMEINFO * const reference,
							 int rx,int ry,int cx,int cy,
							 int& VAR,int& VAROR,int& MWOR,
							 int& MX,int& MY,int& MV,int& OMV)
{
 
  	
  int px,py,dx,dy,incr,xdir,ydir;
  register int i,j,data,val;
  register unsigned char *bptr,*cptr;
  unsigned char *baseptr;

  MX=MY=MV=0;
  
  bptr=reference->image.y + rx + (ry * pParam->mb_width);
  baseptr=current->image.y + cx + (cy * pParam->mb_width);
  cptr=baseptr;
  for(i=0;i<16;i++)
    {
      for(j=0;j<16;j++)
	{
	  data=(*(bptr++)-*(cptr++));
	  if (data<0) {MV-=data;} else {MV+=data;}
	}
      bptr += (pParam->mb_width - 16);
      cptr += (pParam->mb_width - 16);
    }
  OMV=MV; //  printf("[00]MX %d MY %d MV %d\n",MX,MY,MV);
  px=rx;
  py=ry;
  xdir=1;
  ydir=1;
  int SearchLimit = 15; 
  for(incr=1;incr<SearchLimit;incr++)
    {
      for(dx=0;dx<incr;dx++)
	{
	  if (xdir) {px++;} else {px--;}
	  if (((px >= 0) && (px < (int)(pParam->mb_width-16))) &&
	      ((py >= 0) && (py < (int)(pParam->mb_height-16))))
	    {
	      val=0;
	      bptr = reference->image.y + px + (py * pParam->mb_width);
	      cptr = baseptr;
	      for(i=0;i<16;i++)
		{
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  if (val COMPARISON MV) break;
		  bptr += (pParam->mb_width - 16);
		  cptr += (pParam->mb_width - 16);
		}
	      if (val < MV)
		{
		  MV = val; 
		  MX = px - rx;
		  MY = py - ry;
		}
	    }
	}
      xdir = 1-xdir;
      for(dy=0;dy<incr;dy++)
	{
	  if (ydir) {py++;} else {py--;}
	  if (((px >= 0) && (px <= (int)(pParam->mb_width-16))) &&
	      ((py >= 0) && (py <= (int)(pParam->mb_height-16))))
	    {
	      bptr = reference->image.y + px + (py * pParam->mb_width);
	      cptr = baseptr;
	      for(val=0,i=0;i<16;i++)
		{
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  data=(*(bptr++)-*(cptr++));
		  if (data<0) {val-=data;} else	{val+=data;}
		  if (val COMPARISON MV) break;
		  bptr += (pParam->mb_width - 16);
		  cptr += (pParam->mb_width - 16);
		}
	      if (val < MV)
		{
		  MV = val; 
		  MX = px - rx;
		  MY = py - ry;
		}
	    }
	}
      ydir = 1-ydir;
    }
  bptr = reference->image.y + (MX+rx) + ((MY+ry) * pParam->mb_width);
  cptr = baseptr;
  for(VAR=0,VAROR=0,MWOR=0,i=0;i<16;i++)
    {
      for(j=0;j<16;j++)
	{
	  data = *(bptr) - *(cptr);
	  VAR += data*data;
	  VAROR += *(bptr)*(*(bptr));
	  MWOR += *(bptr);
	  bptr++;
	  cptr++;
	}
      bptr += (pParam->mb_width - 16);
      cptr += (pParam->mb_width - 16);
    }
  VAR = VAR/256;
  VAROR = (VAROR/256)-(MWOR/256)*(MWOR/256);
  
  return 0;
}


int BMEMotionEstimation(MBParam * const pParam,
										 FRAMEINFO * const current,
										 FRAMEINFO * const reference)
{
	
	int x,y;
	int MeN=0;
	int VAR=0;
	int VAROR=0;
	int MWOR=0;
	int MX=0;
	int MY=0;
	int MV=0;
	int OMV=0;
	for(MeN=0,y=0;y<height_;y+=16)
    {
		for(x=0;x<width_;x+=16)
		{
			FastBME(pParam,current,reference,x,y,x,y,VAR, VAROR, MWOR, MX, MY, MV, OMV);
			MeVAR[MeN] = VAR;
			MeVAROR[MeN] = VAROR;
			MeMWOR[MeN] = MWOR;
			MeX[MeN] = MX;
			MeY[MeN] = MY;
			MeVal[MeN] = MV;
			MeOVal[MeN] = OMV;
			MeN++;
		}
    }
	return 0;
}